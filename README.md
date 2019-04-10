# python-libnetfilter-conntrack
Python wrapper for libnetfilter_conntrack.

Notice: Not yet tested.

Usage:

```python
import libnetfilterconntrack
import socket
import struct

def callback_ct(nfmsgtype, conntrack):
    (orig_l3proto, orig_l4proto,
     repl_l3proto, repl_l4proto) = \
        struct.unpack('!BBBB', conntrack.orig_l3proto +
                               conntrack.orig_l4proto +
                               conntrack.repl_l3proto +
                               conntrack.repl_l4proto)

    if orig_l4proto in (socket.IPPROTO_TCP, socket.IPPROTO_UDP):

        orig_ipv4_src = socket.inet_ntoa(conntrack.orig_ipv4_src)
        orig_ipv4_dst = socket.inet_ntoa(conntrack.orig_ipv4_dst)
        repl_ipv4_src = socket.inet_ntoa(conntrack.repl_ipv4_src)
        repl_ipv4_dst = socket.inet_ntoa(conntrack.repl_ipv4_dst)

        (orig_port_src, orig_port_dst,
         repl_port_src, repl_port_dst) = \
            struct.unpack('!HHHH', conntrack.orig_port_src +
                                   conntrack.orig_port_dst +
                                   conntrack.repl_port_src +
                                   conntrack.repl_port_dst)

        print 'type: %s | orig (3: %s, 4: %s): %s:%s => %s:%s | repl (3: %s, 4: %s): %s:%s => %s:%s' % \
            (nfmsgtype,
             orig_l3proto, orig_l4proto,
                orig_ipv4_src, orig_port_src,
                orig_ipv4_dst, orig_port_dst,
             repl_l3proto, repl_l4proto,
                repl_ipv4_src, repl_port_src,
                repl_ipv4_dst, repl_port_dst)

handle = libnetfilterconntrack.open(libnetfilterconntrack.NFNL_SUBSYS_CTNETLINK_CT,
                                    libnetfilterconntrack.NF_NETLINK_CONNTRACK_CT_ALL)
handle.ct_callback_set(libnetfilterconntrack.NFCT_T_ALL, callback_ct)
handle.ct_catch()
handle.ct_callback_clear()
handle.close()

```
