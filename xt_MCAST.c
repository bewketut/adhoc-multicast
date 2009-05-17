#define MSRC_SIZE    20		//src->cli_head_nodes
#define MGRP_SIZE    20		//grp->cli_head_nodes
#define GRP_EXPIRE   120	//seconds
#define SRC_EXPIRE   300	//seconds (5-mins)
#include "compat_xtables.h"
#include <linux/skbuff.h>
#include <linux/module.h>
#include <linux/jiffies.h>

#include <net/ipv6.h>
#include <net/ip.h>
#include <linux/ip.h>
#include <linux/igmp.h>

#include <linux/list.h>
#include <linux/jhash.h>

#include <linux/netfilter.h>
#include <linux/netfilter/x_tables.h>
//you can compare addresses directly without htonl
	 //if (ipv6_addr_cmp(&iph->saddr, &new_addr) == 0)

static u_int32_t jhash_rnd;
struct xt_mcast {
    struct hlist_node node;
    __be32 ip;
    unsigned long timeout;
    //unsigned long  expire; //this is in jiffies (allowed per group)
};
struct mtable {
    struct hlist_head members[0];
};
static struct mtable *msrc_hash;	//client list indexed by hash(src)->cli_ip
static struct mtable *mgrp_hash;	//client list indexed by hash(gid)->cli_ip

static DEFINE_MUTEX(mcast_mutex);
static DEFINE_SPINLOCK(mcast_lock);

static unsigned int
init_new_entry(struct xt_mcast *entry, __be32 ip, uint32_t h,
	       struct mtable *t, uint16_t expire)
{
    //we are not going to simply add it.
    //go to the list if there is someone who has been expired start from the head downwards
    //if the head is expired simple update the head with this
//we want to avoid as many memory alloc calls as possible 
    struct hlist_node *pos, *temp;
    struct xt_mcast *temp_entry;
    spin_lock_bh(&mcast_lock);
    hlist_for_each(pos, &t->members[h]) {
	entry = hlist_entry(pos, struct xt_mcast, node);
	if (entry != NULL && time_before(jiffies, entry->timeout)) {	//timeout
	    //the first time this we want to delete all but the first expired
	    //go until you find the first timeout and delete downwards.
	    pos = pos->next;	//don't delete pos- just advance it
	    while (pos != NULL) {
		temp_entry = hlist_entry(pos, struct xt_mcast, node);
		temp = pos;
		pos = NULL;	//No pter reshuffle here
		pos = temp->next;
		kfree(temp_entry);
	    }
	    break;
	}
    }
    if (entry == NULL) {
	spin_unlock_bh(&mcast_lock);
	entry = kmalloc(sizeof(struct xt_mcast), GFP_KERNEL);
	printk("Allocating for NEW entry\n");
	spin_lock_bh(&mcast_lock);
    }
    else printk("Updating entry\n");
    entry->ip = ip;
    entry->timeout = jiffies + expire * HZ;
    printk("new entry=" NIPQUAD_FMT " and expires %us\n",
	   NIPQUAD(entry->ip), jiffies_to_msecs(entry->timeout - jiffies));
    hlist_add_head(&entry->node, &t->members[h]);
    spin_unlock_bh(&mcast_lock);
    return NF_ACCEPT;
}



static inline uint32_t mcast_hash(u32 name)
{
    return jhash_1word(name, jhash_rnd) & (MSRC_SIZE - 1);
}
static struct hlist_node *lookup(__be32 hkey, __be32 ip, struct mtable *t)
{
    struct xt_mcast *entry;
    struct hlist_node *n = NULL;
    if (t == NULL) {
	printk("Table is NULL\n");
	return NULL;
    }
    mutex_lock(&mcast_mutex);
    hlist_for_each_entry(entry, n, &t->members[hkey], node) {
	if (entry != NULL && entry->ip == ip) {
	    mutex_unlock(&mcast_mutex);
	    return n;
	}
    }
    mutex_unlock(&mcast_mutex);
    return NULL;
}

static struct hlist_node *get_headnode(struct mtable *t, uint32_t h)
{
    struct hlist_node *n = t->members[h].first;
    return n;
}

static unsigned int igmp_report(const struct iphdr *iph,
				struct igmphdr *igmph)
{

    __be32 cli = iph->saddr, grp = igmph->group;
    struct xt_mcast *entry = NULL, *head_entry;	//aka head
    uint32_t h = mcast_hash(grp);
    struct hlist_node *head_node = get_headnode(mgrp_hash, h);
    struct hlist_node *entry_node;
    printk("IGMP REPORT\n");
    if (head_node == NULL)
	return init_new_entry(entry, cli, h, mgrp_hash, GRP_EXPIRE);
    //go fish 
    entry_node = lookup(h, cli, mgrp_hash);	//here there is grp alrdy but is the client in it.
    if (entry_node == NULL)
	return init_new_entry(entry, cli, h, mgrp_hash, GRP_EXPIRE);
    //good the client is in the grp. 
    spin_lock_bh(&mcast_lock);
    head_entry = hlist_entry(head_node, struct xt_mcast, node);
    entry = hlist_entry(entry_node, struct xt_mcast, node);
    entry->timeout = jiffies + GRP_EXPIRE * HZ;
    if (entry->ip == head_entry->ip)
       goto out;
       //it the head. don't do anything 
    hlist_del(entry_node);	//shuffle and fix
    hlist_add_head(&entry->node, &mgrp_hash->members[h]);
    //we want this node to be head so we need to del:
    //NOTE: when deleting a grp if it the head pointer  
 out: spin_unlock_bh(&mcast_lock);
	return NF_ACCEPT;	
}

/* this probably will increase complexity
 * we probably just need to ignore this and use
 * our time based cleaning
 */
static unsigned igmp_leave(const struct iphdr *iph, struct igmphdr *igmph)
{
    __be32 cli = iph->saddr;
    uint32_t h = mcast_hash(igmph->group);
    struct hlist_node *entry_node;
    struct xt_mcast *entry, *head_entry;	//aka head
    struct hlist_node *head_node = get_headnode(mgrp_hash, h);
    printk("IGMP leave called");
/* entry is NULL => "I am leaving this grp"-yet there is no such grp or doesn't have me
 * shouldn't be possible */
    if (head_node == NULL)
	return NF_ACCEPT;
    entry_node = lookup(h, cli, mgrp_hash);
    if (entry_node == NULL)
	return NF_ACCEPT;
    //some clients may just join and leave straight away after becoming last_seen
    //Note: Also IGMPv1 don't issue leave
    //The simple solution here is to make the head of the group last_seen.
    //which often is true
    spin_lock_bh(&mcast_lock);
    head_entry = hlist_entry(head_node, struct xt_mcast, node);
    entry = hlist_entry(entry_node, struct xt_mcast, node);
    if (head_entry->ip == entry->ip)	//promote the successor to be head
	hlist_add_head(entry_node->next, &mgrp_hash->members[h]);
    //else it is the last entry
    //we also want to delete the msrc_hash  entries we built
    hlist_del(entry_node);
    printk("Deleted entry:" NIPQUAD_FMT "\n",NIPQUAD(entry->ip) );
    kfree(entry);
    spin_unlock_bh(&mcast_lock);
    return NF_ACCEPT;
}

//Incoming traffic MCAST traffic
static unsigned int mcast_handler4(const struct iphdr *iph)
{
    __be32 msrc_ip = iph->saddr;
    __be32 mgrp_ip = iph->daddr;
    uint32_t hs = mcast_hash(msrc_ip);	//h(msrc_ip)->cli_ip       we do cartesian product
    uint32_t hg = mcast_hash(mgrp_ip);	//h(mgrp_ip)->cli_ip
    struct xt_mcast *msrc2cli, *mgrp2cli;
    struct hlist_node *n, *n2;
    //check if there exists any client(s) that are mapped to msrc_ip
    //while looking hash them and check if they have 
    mutex_lock(&mcast_mutex);
    hlist_for_each_entry(msrc2cli, n, &msrc_hash->members[hs], node) {
	hlist_for_each_entry(mgrp2cli, n2, &mgrp_hash->members[hg], node) {
	    if (msrc2cli->ip == mgrp2cli->ip) {	//success
		mutex_unlock(&mcast_mutex);
		return NF_ACCEPT;	//remember we won't update anything here (
	    }
	}

    }
    mutex_unlock(&mcast_mutex);
    return NF_DROP;
}

static unsigned
int igmp_handler(const struct sk_buff *skb, const struct iphdr *iph)
{
    struct igmphdr *igmph;
    igmph = igmp_hdr(skb);
    printk("IGMP called\n");
    if (igmph == NULL)
	return NF_ACCEPT;	//we can't properly strip the header (i.e already problematic)
    switch (igmph->type) {
    case IGMP_HOST_MEMBERSHIP_REPORT:
	//fallthru
    case IGMPV2_HOST_MEMBERSHIP_REPORT:
    case IGMPV3_HOST_MEMBERSHIP_REPORT:
	return igmp_report(iph, igmph);
    case IGMP_HOST_LEAVE_MESSAGE:
         {
           printk("LEAVE CALLED\n");
	return igmp_leave(iph, igmph);	//from IGMP_V2 to delete a group (ehnancement on Time interval timer) 
         }
	/**case IGMPV3_HOST_MEMBERSHIP_REPORT  
     *Note: The routers and hosts are too modern: don't think so.Likely there is one INCAPABLE
     *IGMPV3 is SSM and has already mechanism put in place for filtering 
     *But most NICs construct the IGMPv1 & v2 reports types and it is often the case IGMPv3 capable 
     *router falls back to using those types.k
      *case IGMP_HOST_MEMBERSHIP_QUERY: return NF_ACCEPT; No information to snoop from this.
        **/
    default:
	return NF_ACCEPT;
    }
    return NF_ACCEPT;
}

static inline struct mtable *init_table(uint32_t nmembers)
{
    uint32_t i;
    struct mtable *t;
    t = kzalloc(sizeof(*t) + nmembers * sizeof(t->members[0]), GFP_KERNEL);
    for (i = 0; i < nmembers; i++)
	INIT_HLIST_HEAD(&t->members[i]);
    return t;
}
static void delete_list(struct hlist_head *head)
{
    struct xt_mcast *entry;
    struct hlist_node *pos, *temp;
    pos = head->first;
    while (pos != NULL) {
	entry = hlist_entry(pos, struct xt_mcast, node);
	temp = pos;
	pos = NULL;		//No pter reshuffle here
	pos = temp->next;
	kfree(entry);
    }

}


static inline void delete_table(struct mtable *t, uint32_t nsize)
{
    uint32_t i;
    if (t == NULL)
	return;
    for (i = 0; i < nsize; i++)
	delete_list(&t->members[i]);
    kfree(t);
}

/**OUTGOING traffic only thus client = ip_hrd(skb)->saddr;
 * mcast_src_addr= ip_hdr(skb)->daddr;
 */
static unsigned int unicast_handler(const struct iphdr *iph)
{
    __be32 mcli_ip = iph->saddr;
    uint32_t hs = mcast_hash(iph->daddr);
    struct hlist_node *mcli_node;
    struct xt_mcast *mcli = NULL;
    printk("protocol: %d\n", iph->protocol);
    printk("source: " NIPQUAD_FMT " dest: " NIPQUAD_FMT "\n",
	   NIPQUAD(iph->saddr), NIPQUAD(iph->daddr));
    mcli_node = lookup(hs, mcli_ip, msrc_hash);
    if (mcli_node == NULL)
	return init_new_entry(mcli, mcli_ip, hs, msrc_hash, SRC_EXPIRE);
    //the possible source of inefficiencies are a client  joining a lot of groups
    //and msrc multicasting  to *too many* clients (ref:linked list search complexity)
    // but traditional multicast is supposes to be pretty symmetrical
    //first check if this relation already exists.
//here it is *may* be possible a cli is in msrc_hash and msrc is not msrc_hash: think about it!!
//we cannot  use different a perfect hashing function for the msrc as it is pretty random
    //we want timer to be updated
    spin_lock_bh(&mcast_lock);
    mcli = hlist_entry(mcli_node, struct xt_mcast, node);
    mcli->timeout = jiffies + SRC_EXPIRE * HZ;
    spin_unlock_bh(&mcast_lock);

    return NF_ACCEPT;
}


static unsigned int mcast_tg4(const struct sk_buff **pskb,
			      const struct xt_target_param *par)
{
    const struct sk_buff *skb = *pskb;
    const struct iphdr *iph = ip_hdr(skb);

    if (ipv4_is_multicast(iph->daddr))	//broadcast also mcast so: skb->pkt_type==PACKET_MULTICAST check?? 
    {

	if (ip_hdr(skb)->protocol == IPPROTO_UDP)
	    return mcast_handler4(iph);	//there maybe NF_DROPs
	if (ip_hdr(skb)->protocol == IPPROTO_IGMP)

	    return igmp_handler(skb, iph);	//always NF_ACCEPT just for snooping
	return NF_ACCEPT;
    } else if (ip_hdr(skb)->protocol == IPPROTO_UDP) {
	//we are not sure the heartbeat should *only*  be udp
//	printk("It is unicast UDP\n");

	return unicast_handler(iph);	//this needs to make fast construction of <s,c> pair
    }
    return NF_ACCEPT;
}


static struct xt_target mcast_tg_reg __read_mostly = {
    .name = "MCAST",
    .revision = 0,
    .family = NFPROTO_IPV4,
    .target = mcast_tg4,
    .targetsize = XT_ALIGN(0),
    .me = THIS_MODULE,
};

static int __init mcast_tg_init(void)
{

    mgrp_hash = init_table(MGRP_SIZE);
    msrc_hash = init_table(MSRC_SIZE);

    return xt_register_target(&mcast_tg_reg);
}
static void __exit mcast_tg_exit(void)
{
    delete_table(mgrp_hash, MGRP_SIZE);
    delete_table(msrc_hash, MSRC_SIZE);
    xt_unregister_target(&mcast_tg_reg);
}

module_init(mcast_tg_init);
module_exit(mcast_tg_exit);
MODULE_LICENSE("GPL");
MODULE_ALIAS("ipt_MCAST");
MODULE_DESCRIPTION("Xtables: Mcast packet filter");
MODULE_AUTHOR("Bewketu Tadilo<xxxx@xxxx.edu.au>");
