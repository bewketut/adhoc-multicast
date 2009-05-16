static inline bool ipv6_is_multicast(const struct in6_addr *addr)
{
    __be32 st = addr->s6_addr32[0];
    return ((st & htonl(0xFF000000)) == htonl(0xFF000000));
}

//you maynot even need the full support of this function
static inline bool is_multicast(const struct sk_buff *skb, u_int8_t family)
{
    bool is_multicast = false;
    switch (family) {
    case NFPROTO_IPV4:
	is_multicast = ipv4_is_multicast(ip_hdr(skb)->daddr);
	break;
    case NFPROTO_IPV6:
	is_multicast = ipv6_is_multicast(&ipv6_hdr(skb)->daddr);
	break;
    }
    return is_multicast;
}
