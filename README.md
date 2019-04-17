# python-libnetfilter-conntrack
Python wrapper for libnetfilter_conntrack.

Notice: Not yet tested.

Usage:

```python
import libnetfilterconntrack
import socket
import struct

def callback_ct(nfmsgtype, conntrack):
    ct_id, = struct.unpack('!I', conntrack.id)

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

        print ' | '.join(('conntrack', 'id: %s', 'type: %s',
                          'orig (3: %s, 4: %s): %s:%s => %s:%s',
                          'repl (3: %s, 4: %s): %s:%s => %s:%s')) % \
            (ct_id, nfmsgtype,
             orig_l3proto, orig_l4proto,
                orig_ipv4_src, orig_port_src,
                orig_ipv4_dst, orig_port_dst,
             repl_l3proto, repl_l4proto,
                repl_ipv4_src, repl_port_src,
                repl_ipv4_dst, repl_port_dst)


def callback_exp(nfmsgtype, expect):
    master = expect.master()
    (master_l3proto,
     master_l4proto) = struct.unpack('!BB', master.l3proto +
                                            master.l4proto)

    if master_l4proto in (socket.IPPROTO_TCP, socket.IPPROTO_UDP):

        master_ipv4_src = socket.inet_ntoa(master.ipv4_src)
        master_ipv4_dst = socket.inet_ntoa(master.ipv4_dst)

        (master_port_src,
         master_port_dst) = struct.unpack('!HH', master.port_src +
                                                 master.port_dst)

        print ' | '.join(('expect', 'master',
                          '3: %s, 4: %s',
                          '%s:%s => %s:%s')) % \
            (master_l3proto, master_l4proto,
                master_ipv4_src, master_port_src,
                master_ipv4_dst, master_port_dst)


subsystem = libnetfilterconntrack.NFNL_SUBSYS_NONE
groups = (libnetfilterconntrack.NF_NETLINK_CONNTRACK_CT_ALL |
          libnetfilterconntrack.NF_NETLINK_CONNTRACK_EXP_ALL)

handle = libnetfilterconntrack.open(subsystem, groups)

handle.ct_callback_set(libnetfilterconntrack.NFCT_T_ALL, callback_ct)
handle.exp_callback_set(libnetfilterconntrack.NFCT_T_ALL, callback_exp)

try:
    sock = socket.fromfd(handle.fd(),
                         socket.AF_NETLINK,
                         socket.SOCK_RAW)
    while True:
        try:
            data, address = sock.recvfrom(8192)
            handle.handle(data, address)
        except socket.error as e:
            if e.errno is socket.errno.ENOBUFS:
                print 'Unable to hold processed packets'
                continue
            raise
finally:
    sock.close()

handle.ct_callback_clear()
handle.exp_callback_clear()

handle.close()

```
