from typing import Callable, Dict, Optional, Tuple, Union

NFNL_SUBSYS_NONE: int
NFNL_SUBSYS_CTNETLINK_CT: int
NFNL_SUBSYS_CTNETLINK_EXP: int

NF_NETLINK_CONNTRACK_CT_NEW: int
NF_NETLINK_CONNTRACK_CT_UPDATE: int
NF_NETLINK_CONNTRACK_CT_DESTROY: int
NF_NETLINK_CONNTRACK_CT_ALL: int

NF_NETLINK_CONNTRACK_EXP_NEW: int
NF_NETLINK_CONNTRACK_EXP_UPDATE: int
NF_NETLINK_CONNTRACK_EXP_DESTROY: int
NF_NETLINK_CONNTRACK_EXP_ALL: int

NFCT_T_UNKNOWN: int
NFCT_T_NEW_BIT: int
NFCT_T_NEW: int
NFCT_T_UPDATE_BIT: int
NFCT_T_UPDATE: int
NFCT_T_DESTROY_BIT: int
NFCT_T_DESTROY: int
NFCT_T_ALL: int
NFCT_T_ERROR_BIT: int
NFCT_T_ERROR: int

NFCT_Q_CREATE: int
NFCT_Q_UPDATE: int
NFCT_Q_DESTROY: int
NFCT_Q_GET: int
NFCT_Q_FLUSH: int
NFCT_Q_DUMP: int
NFCT_Q_DUMP_RESET: int
NFCT_Q_CREATE_UPDATE: int
NFCT_Q_DUMP_FILTER: int
NFCT_Q_DUMP_FILTER_RESET: int

NFCT_CB_FAILURE: int
NFCT_CB_STOP: int
NFCT_CB_CONTINUE: int
NFCT_CB_STOLEN: int

NFCT_CP_ALL: int
NFCT_CP_ORIG: int
NFCT_CP_REPL: int
NFCT_CP_META: int
NFCT_CP_OVERRIDE: int

NF_CONNTRACK_ATTR_SPECS_TUPLE: Dict[str, Tuple[int, int]]
NF_CONNTRACK_ATTR_SPECS_CT: Dict[str, Tuple[int, int]]
NF_CONNTRACK_ATTR_SPECS_EXP: Dict[str, Tuple[int, int]]

class NetfilterConntrackTuple:
    """Wrapper for (struct nfct_tuple_head *)."""

    ipv4_src: Optional[str]
    ipv4_dst: Optional[str]
    ipv6_src: Optional[str]
    ipv6_dst: Optional[str]
    port_src: Optional[str]
    port_dst: Optional[str]
    l3proto: Optional[str]
    l4proto: Optional[str]
    icmp_type: Optional[str]
    icmp_code: Optional[str]
    icmp_id: Optional[str]

class NetfilterConntrackConntrack:
    """Wrapper for (struct nfct_conntrack *)."""

    orig_ipv4_src: Optional[str]
    ipv4_src: Optional[str]
    orig_ipv4_dst: Optional[str]
    ipv4_dst: Optional[str]
    repl_ipv4_src: Optional[str]
    repl_ipv4_dst: Optional[str]
    orig_ipv6_src: Optional[str]
    ipv6_src: Optional[str]
    orig_ipv6_dst: Optional[str]
    ipv6_dst: Optional[str]
    repl_ipv6_src: Optional[str]
    repl_ipv6_dst: Optional[str]
    orig_port_src: Optional[str]
    port_src: Optional[str]
    orig_port_dst: Optional[str]
    port_dst: Optional[str]
    repl_port_src: Optional[str]
    repl_port_dst: Optional[str]
    icmp_type: Optional[str]
    icmp_code: Optional[str]
    icmp_id: Optional[str]
    orig_l3proto: Optional[str]
    l3proto: Optional[str]
    repl_l3proto: Optional[str]
    orig_l4proto: Optional[str]
    l4proto: Optional[str]
    repl_l4proto: Optional[str]
    tcp_state: Optional[str]
    snat_ipv4: Optional[str]
    dnat_ipv4: Optional[str]
    snat_port: Optional[str]
    dnat_port: Optional[str]
    timeout: Optional[str]
    mark: Optional[str]
    orig_counter_packets: Optional[str]
    repl_counter_packets: Optional[str]
    orig_counter_bytes: Optional[str]
    repl_counter_bytes: Optional[str]
    use: Optional[str]
    id: Optional[str]
    status: Optional[str]
    tcp_flags_orig: Optional[str]
    tcp_flags_repl: Optional[str]
    tcp_mask_orig: Optional[str]
    tcp_mask_repl: Optional[str]
    master_ipv4_src: Optional[str]
    master_ipv4_dst: Optional[str]
    master_ipv6_src: Optional[str]
    master_ipv6_dst: Optional[str]
    master_port_src: Optional[str]
    master_port_dst: Optional[str]
    master_l3proto: Optional[str]
    master_l4proto: Optional[str]
    secmark: Optional[str]
    orig_nat_seq_correction_pos: Optional[str]
    orig_nat_seq_offset_before: Optional[str]
    orig_nat_seq_offset_after: Optional[str]
    repl_nat_seq_correction_pos: Optional[str]
    repl_nat_seq_offset_before: Optional[str]
    repl_nat_seq_offset_after: Optional[str]
    sctp_state: Optional[str]
    sctp_vtag_orig: Optional[str]
    sctp_vtag_repl: Optional[str]
    helper_name: Optional[str]
    dccp_state: Optional[str]
    dccp_role: Optional[str]
    dccp_handshake_seq: Optional[str]
    tcp_wscale_orig: Optional[str]
    tcp_wscale_repl: Optional[str]
    zone: Optional[str]
    timestamp_start: Optional[str]
    timestamp_stop: Optional[str]
    orig_zone: Optional[str]
    repl_zone: Optional[str]
    snat_ipv6: Optional[str]
    dnat_ipv6: Optional[str]

    def destroy(self) -> None: ...

class NetfilterConntrackExpect:
    """Wrapper for (struct nfct_expect *)."""

    timeout: Optional[str]
    zone: Optional[str]
    flags: Optional[str]
    helper_name: Optional[str]
    exp_class: Optional[str]
    nat_dir: Optional[str]

    def master(self) -> NetfilterConntrackTuple: ...
    def expected(self) -> NetfilterConntrackTuple: ...
    def mask(self) -> NetfilterConntrackTuple: ...
    def nat_tuple(self) -> NetfilterConntrackTuple: ...
    def destroy(self) -> None: ...

class NetfilterConntrackHandle:
    """Wrapper for (struct nfct_handle *)."""

    def ct_callback_set(self, type_ct: int, callback_ct: Callable[[int, NetfilterConntrackConntrack], object]) -> None: ...
    def ct_callback_clear(self) -> None: ...
    def ct_send(self, query: int, data: Union[NetfilterConntrackConntrack, int]) -> bool: ...
    def exp_callback_set(self, type_exp: int, callback_exp: Callable[[int, NetfilterConntrackExpect], object]) -> None: ...
    def exp_callback_clear(self) -> None: ...
    def exp_send(self, query: int, data: Union[NetfilterConntrackExpect, int]) -> bool: ...
    def handle(self, data: str, address: Tuple[int, int]) -> int: ...
    def fd(self) -> int: ...
    def close(self) -> None: ...

def ct_new() -> NetfilterConntrackConntrack: ...
def ct_copy(destination: NetfilterConntrackConntrack, source: NetfilterConntrackConntrack, flags: int) -> None: ...
def exp_new() -> NetfilterConntrackExpect: ...
def exp_copy(destination: NetfilterConntrackExpect, source: NetfilterConntrackExpect) -> None: ...
def open(systems: int, groups: int) -> NetfilterConntrackHandle: ...
