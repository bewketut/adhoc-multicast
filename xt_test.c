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
/*
static inline struct igmpv3_report *
			igmpv3_report_hdr(const struct sk_buff *skb)
{
	return (struct igmpv3_report *)skb_transport_header(skb);
}

*/


static unsigned int mcast_tg4(const struct sk_buff **pskb,
			      const struct xt_target_param *par)
{
    const struct sk_buff *skb = *pskb;
    const struct iphdr *iph = ip_hdr(skb);
    const struct igmphdr *igm = igmp_hdr(skb);
    const struct igmpv3_report *igm3 = igmpv3_report_hdr(skb);
    if (iph == NULL)
	return NF_ACCEPT;
    if (ipv4_is_multicast(iph->daddr))	//broadcast also mcast so: skb->pkt_type==PACKET_MULTICAST check?? 
    {

	if (ip_hdr(skb)->protocol == IPPROTO_UDP)
                 printk("UDPm: "NIPQUAD_FMT,NIPQUAD(iph->daddr)); 
	if (ip_hdr(skb)->protocol == IPPROTO_IGMP){
          printk("type %u ngrec %u\n", igm3->type,htons(igm3->ngrec));
          printk("Nsrcs: %d, Address: "NIPQUAD_FMT "\n", htons(igm3->grec->grec_nsrcs),NIPQUAD(igm3->grec->grec_mca));
    printk("IGMP sent to: "NIPQUAD_FMT " for group: " NIPQUAD_FMT "\n",NIPQUAD(iph->daddr),NIPQUAD(igm->group)); 
         }
    }
    else
      if (ip_hdr(skb)->protocol == IPPROTO_UDP);
	//the heartbeat protocol can be ICMP, or any other so long as we 
	// get its ip header to extract msrc.
//	return unicast_handler(iph);	//this needs to make fast construction of <s,c> pair
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


    return xt_register_target(&mcast_tg_reg);
}
static void __exit mcast_tg_exit(void)
{
    xt_unregister_target(&mcast_tg_reg);
}

module_init(mcast_tg_init);
module_exit(mcast_tg_exit);
MODULE_LICENSE("GPL");
MODULE_ALIAS("ipt_MCAST");
MODULE_DESCRIPTION("Xtables: Mcast packet filter");
MODULE_AUTHOR("Bewketu Tadilo<xxxx@xxxx.edu.au>");
