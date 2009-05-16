#!/bin/sh
IPT=/usr/sbin/iptables
modprobe  compat_xtables

$IPT -F
rmmod  xt_MCAST 
insmod xt_MCAST.ko
$IPT -I OUTPUT -p udp --dport 4444 -j MCAST 

