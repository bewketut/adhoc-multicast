#ifndef _XT_MCAST_TARGET_H
#define _XT_MCAST_TARGET_H

#include <linux/types.h>

struct xt_mcast_target_info {
 uint32_t           msrc_size;
 uint32_t           mgrp_size;
};

#endif /* XT_MCAST_TARGET_H */
