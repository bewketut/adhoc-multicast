#!/bin/sh
IPT=/usr/sbin/iptables
modprobe  compat_xtables

$IPT -F
$IPT -t nat -A POSTROUTING --out-interface eth0 -j MASQUERADE
rmmod  xt_MCAST 
insmod xt_MCAST.ko
$IPT -I OUTPUT -p udp --dport 4444 -j MCAST 
$IPT -I OUTPUT -p igmp -j MCAST

