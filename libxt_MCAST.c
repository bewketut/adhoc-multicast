#include <getopt.h>
#include <stdio.h>
#include <string.h>

#include <xtables.h>
#include <linux/netfilter/x_tables.h>
#include "xt_MCAST.h"

static void mcast_tg_help(void)
{
	printf(
		"MCAST target options:\n"
		"  --multicast-source-size value    expected number of multicast sources\n"
		"  --multicast-group-size value    expected number of multicast groups \n");

}

enum {
      MCAST_OPT_MSRC_SIZE,
      MCAST_OPT_MGRP_SIZE,
};
static const struct option mcast_opts[]= {
            {"multicast-source-size", 1, NULL, MCAST_OPT_MSRC_SIZE},
            {"multicast-group-size", 1, NULL, MCAST_OPT_MGRP_SIZE},
            {.name= NULL},
};
static int mcast_tg_parse(int c, char **argv, int invert, unsigned int *flags,
    const void *entry, struct xt_entry_target **target)
{
   struct xt_mcast_target_info *info= (void *)(*target)->data;
   unsigned int num;
    switch(c){
      case MCAST_OPT_MSRC_SIZE: 
        if(*flags & (1<<c)){
			xtables_error(PARAMETER_PROBLEM,
				   "MCAST: can't specify `--multicast-source-size' twice");
        }
        if (!xtables_strtoui(optarg, NULL, &num, 1,
				     UINT32_MAX)) {
			xtables_error(PARAMETER_PROBLEM,
				      "Unable to parse `%s' in "
				      "`--mulitcast-souce-size'", optarg);
		}
        info->msrc_size= num;
       *flags|=1<<c; 
         break;
      case MCAST_OPT_MGRP_SIZE:
        if(*flags & (1<<c))
			xtables_error(PARAMETER_PROBLEM,
				   "MCAST: can only specify `--multicast-group-size' once");
 if (!xtables_strtoui(optarg, NULL, &num, 1,
				     UINT32_MAX)) {
			xtables_error(PARAMETER_PROBLEM,
				      "Unable to parse `%s' in "
				      "`--mulitcast-souce-size'", optarg);
		}
         info->mgrp_size= num;
       *flags|=1<<c; 
            break;
      default:
        return 0;
    }
	return 1;
}

static void mcast_tg_check(unsigned int flags)
{
}

static struct xtables_target mcast_tg_reg = {
	.version       = XTABLES_VERSION,
	.name          = "MCAST",
	.revision      = 0,
	.family        = PF_INET,
	.size          = XT_ALIGN(sizeof(struct xt_mcast_target_info)),
	.userspacesize = XT_ALIGN(sizeof(struct xt_mcast_target_info)),
	.help          = mcast_tg_help,
	.parse         = mcast_tg_parse,
	.final_check   = mcast_tg_check,
};

static __attribute__((constructor)) void mcast_tg_ldr(void)
{
	xtables_register_target(&mcast_tg_reg);
}
