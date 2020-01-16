#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <linux/types.h>
#include <stdlib.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/ip6.h>
#include <getopt.h>
#include "list.h"
#include "mtest.h"
#include <linux/netfilter.h>
#include "jhash.h"
#include <sys/time.h>
#define MSRC_SIZE    20		//src->cli_head_nodes
#define MGRP_SIZE    20		//grp->cli_head_nodes
#define MGRP_EXPIRE   120	//2-minutes about the same timetime router will issue Group Query.
#define MCAST_GRP_ID   10
#define ARRAR_SIZE(array) (sizeof(array)/sizeof(array[0]))
static void print_hash_list(struct hlist_head head);


void print_usage(FILE * stream, char **argv);
static inline void simulate(int, char **);
static struct mtable *msrc_hash;	//client list indexed by hash(src)->cli_ip
static struct mtable *mgrp_hash;	//client list indexed by hash(gid)->cli_ip
static unsigned int source_size = MSRC_SIZE;
static unsigned int group_size = MGRP_SIZE;
static unsigned int source_expire = 2 * 60 * 60;	//2-hours
static unsigned int *rand_src, *rand_grp;
static unsigned int *rand_cli;

static struct timeval now;
static u_int32_t jhash_rnd;
// all insert functions are f(keyip,cli)  format
int main(int argc, char **argv)
{
    int i;
    if (argc < 2)
	print_usage(stderr, argv);
    jhash_rnd = time(NULL);
    mgrp_hash = init_table(group_size);
    msrc_hash = init_table(source_size);
    simulate(argc, argv);
    // for(i = 0; i < 10; i++)
    //    igmp_report(i,MCAST_GRP_ID);


    //print_hash_list(mgrp_hash->members[mcast_hash(MCAST_GRP_ID,group_size)]);
    //unicast_handler(10,12);
    //mcast_search(MCAST_GRP_ID,9);
    delete_table(mgrp_hash, group_size);
    delete_table(msrc_hash, source_size);
    return 0;
}
static struct option long_opt[] = {
    {"ngrp", 1, 0, 'g'},
    {"ncli", 1, 0, 'c'},
    {"nsrc", 1, 0, 's'},
    {"percentag", 1, 0, 'r'},
    {NULL, 0, NULL, 0}
};
static char *short_opt = "g:c:s:r:";

static inline void simulate(int argc, char **argv)
{
//15-groups 
//15-clients
    int i;
    srand(time(NULL));
    int c, sizec, sizes, sizeg, per;
    long mtime, seconds, useconds;
    struct timeval start;
    struct timeval end;
    long average = 0;
    while (1) {
	c = getopt_long(argc, argv, short_opt, long_opt, NULL);
	if (c == -1)
	    break;
	switch (c) {
	case 'c':{
		sizec = atoi(optarg);
		rand_cli = malloc(sizec * sizeof(int *));
		for (i = 0; i < sizec; i++)
		    rand_cli[i] = rand();
	    }
	    break;

	case 's':{
		sizes = atoi(optarg);
		rand_src = malloc(sizes * sizeof(int *));
		for (i = 0; i < sizes; i++)
		    rand_src[i] = rand();
	    }
	    break;
	case 'g':{
		sizeg = atoi(optarg);
		rand_grp = malloc(sizes * sizeof(int *));
		for (i = 0; i < sizes; i++)
		    rand_grp[i] = rand();
	    }
	    break;


	case 'r':
	    per = atoi(optarg);
	    break;

	default:
	    print_usage(stderr, argv);
	}
    }
    for (i = 0; i < sizec; i++) {
	igmp_report(rand_cli[i], rand_grp[rand() % sizeg]);
	unicast_handler(rand_cli[i], rand_src[rand() % sizes]);
    }


    //again here now we simulate a random source contacts our network
    //here we measure how long does it take to block it to let it go with a relation to the our memory
    //do 10 random searches printing the raw time it took.

    for (i = 0; i < 300; i++) {
	gettimeofday(&start, NULL);
	mcast_search(rand_src[rand() % sizes], rand_grp[rand() % sizeg]);
	gettimeofday(&end, NULL);
	//seconds = end.tv_sec - start.tv_sec;
	useconds = end.tv_usec - start.tv_usec;
	average += useconds;
	//  mtime=  (seconds *1000 + useconds/1000.0)+ 0.5;
	//printf("Elapsed: %ld us\n",useconds);
    }
    printf("Averag elapsed %ld useconds\n", average / 300);

}
static void print_hash_list(struct hlist_head head)	// this is array of hashtables
{
    struct xt_mcast *entry;
    struct hlist_node *pos;
    hlist_for_each_entry(entry, pos, &head, node)
	printf("ip: %u\n", entry->ip);
}
static unsigned int
init_new_entry(struct xt_mcast *entry, __be32 ip, uint32_t h,
	       struct mtable *t, uint16_t expire)
{
    struct hlist_node *pos, *temp;
    struct xt_mcast *temp_entry;
    struct xt_mcast *place;
    struct hlist_node *ref = get_headnode(t, h);
    hlist_for_each_entry_safe(place, pos, temp, &t->members[h], node) {
	gettimeofday(&now, NULL);
	if (place != NULL && time_after(now.tv_sec, place->timeout)) {
	    entry = place;
	    pos = pos->next;	//don't delete pos- just advance it
	    while (pos != NULL && pos != ref) {
		temp_entry = hlist_entry(pos, struct xt_mcast, node);
		free(temp_entry);
		temp = pos;
		pos = NULL;	//No pter reshuffle here
		pos = temp->next;
	    }
	    break;
	}
    }
    if (entry == NULL)
	entry = (struct xt_mcast *) malloc(sizeof(struct xt_mcast));
    entry->ip = ip;
    gettimeofday(&now, NULL);
    entry->timeout = now.tv_sec + expire;
    hlist_add_head(&entry->node, &t->members[h]);
    return NF_ACCEPT;
}



static inline uint32_t mcast_hash(__be32 name, unsigned int size)
{
    return jhash_1word(name, jhash_rnd) & (size - 1);
}
static struct hlist_node *lookup(__be32 hkey, __be32 ip, struct mtable *t)
{
    struct xt_mcast *entry;
    struct hlist_node *n;
    struct hlist_node *temp = n;
    //use of the mutex should be appropriate here-Noisy Kernel unless new!!
    hlist_for_each_entry_safe(entry, n, temp, &t->members[hkey], node)
	if (ip == entry->ip)
	return n;
    return NULL;
}

static struct hlist_node *get_headnode(struct mtable *t, uint32_t h)
{
    return t->members[h].first;
}

static unsigned int igmp_report(__be32 cli, __be32 grp)
{

    struct xt_mcast *entry = NULL, *head_entry;	//aka head
    uint32_t h = mcast_hash(grp, group_size);

    struct hlist_node *head_node = get_headnode(mgrp_hash, h);
    struct hlist_node *entry_node;
    if (head_node == NULL)
	return init_new_entry(entry, cli, h, mgrp_hash, MGRP_EXPIRE);
    //go fish 

    entry_node = lookup(h, cli, mgrp_hash);	//here there is grp alrdy but is the client in it.
    if (entry_node == NULL) {
	return init_new_entry(entry, cli, h, mgrp_hash, MGRP_EXPIRE);
    }
    //good the client is in the grp. 
    head_entry = hlist_entry(head_node, struct xt_mcast, node);
    entry = hlist_entry(entry_node, struct xt_mcast, node);
    gettimeofday(&now, NULL);
    entry->timeout = now.tv_sec + MGRP_EXPIRE;
    //we need to also update the same entry in the source list

    if (entry->ip == head_entry->ip)
	return NF_ACCEPT;
    //it the head. don't do anything 
    hlist_del(entry_node);	//shuffle and fix
    hlist_add_head(&entry->node, &mgrp_hash->members[h]);
    //we want this node to be head so we need to del:
    //NOTE: when deleting a grp if it the head pointer  
    printf("IGMP REPORT END\n");
    return NF_ACCEPT;
}

static unsigned int mcast_search(__be32 msrc_ip, __be32 mgrp_ip)
{
    uint32_t hs = mcast_hash(msrc_ip, source_size);	//h(msrc_ip)->cli_ip       we do cartesian product
    uint32_t hg = mcast_hash(mgrp_ip, group_size);	//h(mgrp_ip)->cli_ip
    struct xt_mcast *msrc2cli, *mgrp2cli;
    struct hlist_node *n, *n2;
    //check if there exists any client(s) that are mapped to msrc_ip
    //while looking hash them and check if they have 
    hlist_for_each_entry(msrc2cli, n, &msrc_hash->members[hs], node) {
	hlist_for_each_entry(mgrp2cli, n2, &mgrp_hash->members[hg], node) {
	    if (msrc2cli->ip == mgrp2cli->ip) {	//success
		return NF_ACCEPT;	//remember we won't update anything here (
	    }
	}
    }
    return NF_DROP;
}

static unsigned igmp_leave(__be32 cli, __be32 grp)
{
    uint32_t h = mcast_hash(grp, group_size);
    struct hlist_node *entry_node;
    struct xt_mcast *entry, *head_entry;	//aka head
    struct hlist_node *head_node = get_headnode(mgrp_hash, h);
/* entry is NULL => "I am leaving this grp"-yet there is no such grp or doesn't have me
 * shouldn't be possible */
    if (head_node == NULL)
	return NF_ACCEPT;
    entry_node = lookup(h, cli, mgrp_hash);
    if (entry_node == NULL)	//weird
	return NF_ACCEPT;
    entry = hlist_entry(entry_node, struct xt_mcast, node);
    //some clients may just join and leave straight away after becoming last_seen
    //Note: Also IGMPv1 don't issue leave
    //The simple solution here is to make the head of the group last_seen.
    //which often is true

    head_entry = hlist_entry(head_node, struct xt_mcast, node);
    entry = hlist_entry(entry_node, struct xt_mcast, node);

    if (head_entry->ip == entry->ip && entry_node->next != NULL)	//promote the successor to be head
	hlist_add_head(entry_node->next, &mgrp_hash->members[h]);
    //else it is the last entry
    hlist_del(entry_node);
    free(entry);

    return NF_ACCEPT;
}


static inline struct mtable *init_table(uint32_t nmembers)
{
    uint32_t i;
    struct mtable *t;
    t = malloc(sizeof(*t) + nmembers * sizeof(t->members[0]));
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
	free(entry);
    }

}


static inline void delete_table(struct mtable *t, uint32_t nsize)
{
    uint32_t i;
    if (t == NULL)
	return;
    for (i = 0; i < nsize; i++)
	delete_list(&t->members[i]);
    free(t);
}

static unsigned int unicast_handler(__be32 mcli_ip, __be32 msrc_ip)
{
    uint32_t hs = mcast_hash(msrc_ip, source_size);
    struct hlist_node *mcli_node;
    struct xt_mcast *mcli = NULL;

    mcli_node = lookup(hs, mcli_ip, msrc_hash);
    if (mcli_node == NULL)
	return init_new_entry(mcli, mcli_ip, hs, msrc_hash, source_expire);

    mcli = hlist_entry(mcli_node, struct xt_mcast, node);
    gettimeofday(&now, NULL);
    mcli->timeout = now.tv_sec + source_expire;

    return NF_ACCEPT;
}

void print_usage(FILE * stream, char **argv)
{
    fprintf(stream, "\n" "Usage: %s -c <ncli> " "-s <nsrc> \n", argv[0]);
    fprintf(stream,
	    "\n"
	    "Opitions:\n"
	    "       -c, --ncli     number of clients\n"
	    "       -s, --nsrc       expected number of sources\n"
	    "       -r, --percentage    multicast connection percentage (from the above)\n"
	    "\n");
    exit(0);
}
