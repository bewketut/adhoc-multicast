#include <getopt.h>
#include <stdio.h>
#include <string.h>

#include <xtables.h>
#include <linux/netfilter/x_tables.h>

static void mcast_tg_help(void)
{
	printf("MCAST takes no options\n\
            You NEED to know what it does\n");
}

static int mcast_tg_parse(int c, char **argv, int invert, unsigned int *flags,
    const void *entry, struct xt_entry_target **target)
{
	return 0;
}

static void mcast_tg_check(unsigned int flags)
{
}

static struct xtables_target mcast_tg_reg = {
	.version       = XTABLES_VERSION,
	.name          = "MCAST",
	.revision      = 0,
	.family        = PF_INET,
	.size          = XT_ALIGN(0),
	.userspacesize = XT_ALIGN(0),
	.help          = mcast_tg_help,
	.parse         = mcast_tg_parse,
	.final_check   = mcast_tg_check,
};

static __attribute__((constructor)) void mcast_tg_ldr(void)
{
	xtables_register_target(&mcast_tg_reg);
}
