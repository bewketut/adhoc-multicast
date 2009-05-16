#!/bin/sh

IPT=/usr/sbin/iptables

$IPT -F

#default policies
$IPT -P OUTPUT ACCEPT #outgoing connections
$IPT -P INPUT  DROP   #incoming connections 
$IPT -P FORWARD DROP 
$IPT -t nat -P OUTPUT ACCEPT
$IPT -N SERVICES


#allow loopback
$IPT -A INPUT --in-interface lo -j ACCEPT
$IPT -A INPUT -j SERVICES
#allow responses 
$IPT -A INPUT -m state --state ESTABLISHED,RELATED -j ACCEPT

#allow services
$IPT -A SERVICES -p tcp --dport 8008 -j ACCEPT
$IPT -A SERVICES -m iprange --src-range  192.168.1.1-192.168.254 -p udp --dport 60 -j ACCEPT

