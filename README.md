# python-libnetfilter-conntrack

Python wrapper for `libnetfilter_conntrack`.

## Usage

To listen to `conntrack` events, refer to the following snippet:

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

    return libnetfilterconntrack.NFCT_CB_CONTINUE

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

    return libnetfilterconntrack.NFCT_CB_CONTINUE

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
            if handle.handle(data, address):
                continue
            break
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

To query `conntrack` objects, refer to the following snippet:

```python
import libnetfilterconntrack
import socket
import struct

def query_ct(protocols, source, destination):
    l3proto, l4proto = protocols
    src_addr, src_port = source
    dst_addr, dst_port = destination

    l3proto, l4proto = struct.pack('!BB', l3proto, l4proto)
    src_addr, src_port = socket.inet_aton(src_addr), struct.pack('!H', src_port)
    dst_addr, dst_port = socket.inet_aton(dst_addr), struct.pack('!H', dst_port)

    ct_req = libnetfilterconntrack.ct_new()

    ct_req.l3proto = l3proto
    ct_req.ipv4_src = src_addr
    ct_req.ipv4_dst = dst_addr

    ct_req.l4proto = l4proto
    ct_req.port_src = src_port
    ct_req.port_dst = dst_port

    subsystem = libnetfilterconntrack.NFNL_SUBSYS_CTNETLINK_CT

    def catch_ct(nfmsgtype, ct_res):
        libnetfilterconntrack.ct_copy(ct_req, ct_res, libnetfilterconntrack.NFCT_CP_ALL)
        return libnetfilterconntrack.NFCT_CB_STOP

    handle = libnetfilterconntrack.open(subsystem, 0)
    handle.ct_callback_set(libnetfilterconntrack.NFCT_T_ALL, catch_ct)

    handle.ct_send(libnetfilterconntrack.NFCT_Q_GET, ct_req)

    try:
        sock = socket.fromfd(handle.fd(),
                             socket.AF_NETLINK,
                             socket.SOCK_RAW)
        sock.settimeout(0)
        while True:
            try:
                data, address = sock.recvfrom(8192)
                if handle.handle(data, address):
                    continue
                break
            except socket.error as e:
                if e.errno is socket.errno.EWOULDBLOCK:
                    ct_req.destroy()
                    return None
                if e.errno is socket.errno.ENOBUFS:
                    print 'Unable to hold processed packets'
                    continue
                raise
    finally:
        sock.close()

    handle.ct_callback_clear()
    handle.close()

    return ct_req

conntrack = query_ct((socket.AF_INET, socket.IPPROTO_TCP),
                     ('192.168.50.40', 58198),
                     ('192.168.50.50', 21))

if conntrack:
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

        print ' | '.join(('response', 'id: %s',
                          'orig (3: %s, 4: %s): %s:%s => %s:%s',
                          'repl (3: %s, 4: %s): %s:%s => %s:%s')) % \
            (ct_id,
             orig_l3proto, orig_l4proto,
                orig_ipv4_src, orig_port_src,
                orig_ipv4_dst, orig_port_dst,
             repl_l3proto, repl_l4proto,
                repl_ipv4_src, repl_port_src,
                repl_ipv4_dst, repl_port_dst)

    conntrack.destroy()

```
