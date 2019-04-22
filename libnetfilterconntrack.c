#include <Python.h>
#include <structmember.h>

#include <string.h>

#include <libnetfilter_conntrack/libnetfilter_conntrack.h>

typedef __int128 int128_t;
typedef unsigned __int128 uint128_t;

#define NF_NETLINK_CONNTRACK_ALL (NF_NETLINK_CONNTRACK_NEW|NF_NETLINK_CONNTRACK_UPDATE|NF_NETLINK_CONNTRACK_DESTROY)
#define NF_NETLINK_CONNTRACK_EXP_ALL (NF_NETLINK_CONNTRACK_EXP_UPDATE|NF_NETLINK_CONNTRACK_EXP_UPDATE|NF_NETLINK_CONNTRACK_EXP_DESTROY)

// BEGIN: _nf_conntrack_attr_spec

typedef struct {
    char* attr_name;
    uint32_t attr_type;
    uint8_t attr_size;
} _nf_conntrack_attr_spec;

static PyObject* _nf_conntrack_attr_spec_dict_new (_nf_conntrack_attr_spec attrs[]) {
    PyObject* attr_key;
    PyObject* attr_value;
    PyObject* attr_value_type;
    PyObject* attr_value_size;
    PyObject* attr_dict;
    int i;

    attr_dict = PyDict_New();
    if (attr_dict != NULL) {
        for (i = 0; attrs[i].attr_name != NULL; i++) {
            attr_key = PyString_FromString(attrs[i].attr_name);

            attr_value_type = PyInt_FromLong((long) attrs[i].attr_type);
            attr_value_size = PyInt_FromLong((long) attrs[i].attr_size);
            attr_value = PyTuple_Pack(2, attr_value_type, attr_value_size);
            Py_DECREF(attr_value_size);
            Py_DECREF(attr_value_type);

            PyDict_SetItem(attr_dict, attr_key, attr_value);

            Py_DECREF(attr_value);
            Py_DECREF(attr_key);
        }
    }

    return attr_dict;
}

static uint8_t _nf_conntrack_attr_spec_dict_get (PyObject* attr_dict, PyObject* attr_name, uint32_t* attr_type, uint8_t* attr_size) {
    PyObject* attr_value = NULL;
    if (attr_dict && PyDict_Contains(attr_dict, attr_name)) {
        attr_value = PyDict_GetItem(attr_dict, attr_name);
        if (attr_value) {
            *attr_type = (uint32_t) PyInt_AsLong(PyTuple_GetItem(attr_value, 0));
            *attr_size = (uint8_t) PyInt_AsLong(PyTuple_GetItem(attr_value, 1));
            return 1;
        }
    }
    return 0;
}

// END: _nf_conntrack_attr_spec

// BEGIN: NetfilterConntrackTuple

typedef struct {
    PyObject_HEAD
    struct nf_conntrack* tuple;
} NetfilterConntrackTuple;

static PyObject* NetfilterConntrackTuple_new (PyTypeObject* type, PyTupleObject* args) {
    NetfilterConntrackTuple* self;
    self = (NetfilterConntrackTuple*) type->tp_alloc(type, 0);
    self->tuple = NULL;
    return (PyObject*) self;
}

static int NetfilterConntrackTuple_init (NetfilterConntrackTuple* self, PyTupleObject* args) {
    return 0;
}

static void NetfilterConntrackTuple_dealloc (NetfilterConntrackTuple* self) {
    Py_TYPE(self)->tp_free((PyObject*) self);
}

static PyObject* NetfilterConntrackTupleAttributesDict = NULL;

static PyObject* NetfilterConntrackTuple_GetAttr (NetfilterConntrackTuple* self, PyObject* name) {
    PyObject* dict = NetfilterConntrackTupleAttributesDict;
    const char* attr_value;
    uint32_t attr_type;
    uint8_t attr_size;

    if (_nf_conntrack_attr_spec_dict_get(dict, name, &attr_type, &attr_size)) {
        attr_value = nfct_get_attr(self->tuple, (enum nf_conntrack_attr) attr_type);
        return PyString_FromStringAndSize(attr_value, attr_size);
    }

    return PyObject_GenericGetAttr(self, name);
}

static int NetfilterConntrackTuple_SetAttr (NetfilterConntrackTuple* self, PyObject* name, PyObject* value) {
    PyObject* dict = NetfilterConntrackTupleAttributesDict;
    const char* attr_value;
    uint32_t attr_type;
    uint8_t attr_size;

    if (_nf_conntrack_attr_spec_dict_get(dict, name, &attr_type, &attr_size)) {
        if (!PyString_Check(value)) {
            PyErr_SetString(PyExc_OSError, "Attribute must be a string");
            return -1;
        }

        if (PyString_Size(value) != attr_size) {
            PyErr_SetString(PyExc_OSError, "Invalid attribute size");
            return -1;
        }

        attr_value = PyString_AsString(value);
        nfct_set_attr(self->tuple, (enum nf_conntrack_attr) attr_type, attr_value);

        return 0;
    }

    return PyObject_GenericSetAttr(self, name, value);
}

static _nf_conntrack_attr_spec NetfilterConntrackTupleAttributes [] = {
    {"ipv4_src", ATTR_IPV4_SRC, sizeof(uint32_t)},
    {"ipv4_dst", ATTR_IPV4_DST, sizeof(uint32_t)},
    {"ipv6_src", ATTR_IPV6_SRC, sizeof(uint128_t)},
    {"ipv6_dst", ATTR_IPV6_DST, sizeof(uint128_t)},
    {"port_src", ATTR_PORT_SRC, sizeof(uint16_t)},
    {"port_dst", ATTR_PORT_DST, sizeof(uint16_t)},
    {"l3proto", ATTR_L3PROTO, sizeof(uint8_t)},
    {"l4proto", ATTR_L4PROTO, sizeof(uint8_t)},
    {"icmp_type", ATTR_ICMP_TYPE, sizeof(uint8_t)},
    {"icmp_code", ATTR_ICMP_CODE, sizeof(uint8_t)},
    {"icmp_id", ATTR_ICMP_ID, sizeof(uint16_t)},
    {NULL}
};

static PyMemberDef NetfilterConntrackTuple_members[] = {
    {NULL}
};

static PyMethodDef NetfilterConntrackTuple_methods[] = {
    {NULL}
};

static PyTypeObject NetfilterConntrackTupleType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "libnetfilterconntrack.NetfilterConntrackTuple", /* tp_name */
    sizeof(NetfilterConntrackTuple),                 /* tp_basicsize */
    0,                                               /* tp_itemsize */
    (destructor) NetfilterConntrackTuple_dealloc,    /* tp_dealloc */
    0,                                               /* tp_print */
    0,                                               /* tp_getattr */
    0,                                               /* tp_setattr */
    0,                                               /* tp_compare */
    0,                                               /* tp_repr */
    0,                                               /* tp_as_number */
    0,                                               /* tp_as_sequence */
    0,                                               /* tp_as_mapping */
    0,                                               /* tp_hash */
    0,                                               /* tp_call */
    0,                                               /* tp_str */
    (getattrofunc) NetfilterConntrackTuple_GetAttr,  /* tp_getattro */
    (setattrofunc) NetfilterConntrackTuple_SetAttr,  /* tp_setattro */
    0,                                               /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /* tp_flags */
    "Wrapper for (struct nfct_tuple_head *)",        /* tp_doc */
    0,                                               /* tp_traverse */
    0,                                               /* tp_clear */
    0,                                               /* tp_richcompare */
    0,                                               /* tp_weaklistoffset */
    0,                                               /* tp_iter */
    0,                                               /* tp_iternext */
    NetfilterConntrackTuple_methods,                 /* tp_methods */
    NetfilterConntrackTuple_members,                 /* tp_members */
    0,                                               /* tp_getset */
    0,                                               /* tp_base */
    0,                                               /* tp_dict */
    0,                                               /* tp_descr_get */
    0,                                               /* tp_descr_set */
    0,                                               /* tp_dictoffset */
    (initproc) NetfilterConntrackTuple_init,         /* tp_init */
    0,                                               /* tp_alloc */
    (newfunc) NetfilterConntrackTuple_new,           /* tp_new */
};

// END: NetfilterConntrackTuple

// BEGIN: NetfilterConntrackConntrack

typedef struct {
    PyObject_HEAD
    struct nf_conntrack* conntrack;
} NetfilterConntrackConntrack;

static PyObject* NetfilterConntrackConntrack_new (PyTypeObject* type, PyTupleObject* args) {
    NetfilterConntrackConntrack* self;
    self = (NetfilterConntrackConntrack*) type->tp_alloc(type, 0);
    self->conntrack = NULL;
    return (PyObject*) self;
}

static int NetfilterConntrackConntrack_init (NetfilterConntrackConntrack* self, PyTupleObject* args) {
    return 0;
}

static void NetfilterConntrackConntrack_dealloc (NetfilterConntrackConntrack* self) {
    Py_TYPE(self)->tp_free((PyObject*) self);
}

static PyObject* NetfilterConntrackConntrackAttributesDict = NULL;

static PyObject* NetfilterConntrackConntrack_GetAttr (NetfilterConntrackConntrack* self, PyObject* name) {
    PyObject* dict = NetfilterConntrackConntrackAttributesDict;
    const char* attr_value;
    uint32_t attr_type;
    uint8_t attr_size;

    if (_nf_conntrack_attr_spec_dict_get(dict, name, &attr_type, &attr_size)) {
        attr_value = nfct_get_attr(self->conntrack, (enum nf_conntrack_attr) attr_type);
        return PyString_FromStringAndSize(attr_value, attr_size);
    }

    return PyObject_GenericGetAttr(self, name);
}

static int NetfilterConntrackConntrack_SetAttr (NetfilterConntrackConntrack* self, PyObject* name, PyObject* value) {
    PyObject* dict = NetfilterConntrackConntrackAttributesDict;
    const char* attr_value;
    uint32_t attr_type;
    uint8_t attr_size;

    if (_nf_conntrack_attr_spec_dict_get(dict, name, &attr_type, &attr_size)) {
        if (!PyString_Check(value)) {
            PyErr_SetString(PyExc_OSError, "Attribute must be a string");
            return -1;
        }

        if (PyString_Size(value) != attr_size) {
            PyErr_SetString(PyExc_OSError, "Invalid attribute size");
            return -1;
        }

        attr_value = PyString_AsString(value);
        nfct_set_attr(self->conntrack, (enum nf_conntrack_attr) attr_type, attr_value);

        return 0;
    }

    return PyObject_GenericSetAttr(self, name, value);
}

static _nf_conntrack_attr_spec NetfilterConntrackConntrackAttributes [] = {
    {"orig_ipv4_src", ATTR_ORIG_IPV4_SRC, sizeof(uint32_t)},
    {"ipv4_src", ATTR_IPV4_SRC, sizeof(uint32_t)},
    {"orig_ipv4_dst", ATTR_ORIG_IPV4_DST, sizeof(uint32_t)},
    {"ipv4_dst", ATTR_IPV4_DST, sizeof(uint32_t)},
    {"repl_ipv4_src", ATTR_REPL_IPV4_SRC, sizeof(uint32_t)},
    {"repl_ipv4_dst", ATTR_REPL_IPV4_DST, sizeof(uint32_t)},
    {"orig_ipv6_src", ATTR_ORIG_IPV6_SRC, sizeof(uint128_t)},
    {"ipv6_src", ATTR_IPV6_SRC, sizeof(uint128_t)},
    {"orig_ipv6_dst", ATTR_ORIG_IPV6_DST, sizeof(uint128_t)},
    {"ipv6_dst", ATTR_IPV6_DST, sizeof(uint128_t)},
    {"repl_ipv6_src", ATTR_REPL_IPV6_SRC, sizeof(uint128_t)},
    {"repl_ipv6_dst", ATTR_REPL_IPV6_DST, sizeof(uint128_t)},
    {"orig_port_src", ATTR_ORIG_PORT_SRC, sizeof(uint16_t)},
    {"port_src", ATTR_PORT_SRC, sizeof(uint16_t)},
    {"orig_port_dst", ATTR_ORIG_PORT_DST, sizeof(uint16_t)},
    {"port_dst", ATTR_PORT_DST, sizeof(uint16_t)},
    {"repl_port_src", ATTR_REPL_PORT_SRC, sizeof(uint16_t)},
    {"repl_port_dst", ATTR_REPL_PORT_DST, sizeof(uint16_t)},
    {"icmp_type", ATTR_ICMP_TYPE, sizeof(uint8_t)},
    {"icmp_code", ATTR_ICMP_CODE, sizeof(uint8_t)},
    {"icmp_id", ATTR_ICMP_ID, sizeof(uint16_t)},
    {"orig_l3proto", ATTR_ORIG_L3PROTO, sizeof(uint8_t)},
    {"l3proto", ATTR_L3PROTO, sizeof(uint8_t)},
    {"repl_l3proto", ATTR_REPL_L3PROTO, sizeof(uint8_t)},
    {"orig_l4proto", ATTR_ORIG_L4PROTO, sizeof(uint8_t)},
    {"l4proto", ATTR_L4PROTO, sizeof(uint8_t)},
    {"repl_l4proto", ATTR_REPL_L4PROTO, sizeof(uint8_t)},
    {"tcp_state", ATTR_TCP_STATE, sizeof(uint8_t)},
    {"snat_ipv4", ATTR_SNAT_IPV4, sizeof(uint32_t)},
    {"dnat_ipv4", ATTR_DNAT_IPV4, sizeof(uint32_t)},
    {"snat_port", ATTR_SNAT_PORT, sizeof(uint16_t)},
    {"dnat_port", ATTR_DNAT_PORT, sizeof(uint16_t)},
    {"timeout", ATTR_TIMEOUT, sizeof(uint32_t)},
    {"mark", ATTR_MARK, sizeof(uint32_t)},
    {"orig_counter_packets", ATTR_ORIG_COUNTER_PACKETS, sizeof(uint64_t)},
    {"repl_counter_packets", ATTR_REPL_COUNTER_PACKETS, sizeof(uint64_t)},
    {"orig_counter_bytes", ATTR_ORIG_COUNTER_BYTES, sizeof(uint64_t)},
    {"repl_counter_bytes", ATTR_REPL_COUNTER_BYTES, sizeof(uint64_t)},
    {"use", ATTR_USE, sizeof(uint32_t)},
    {"id", ATTR_ID, sizeof(uint32_t)},
    {"status", ATTR_STATUS, sizeof(uint32_t)},
    {"tcp_flags_orig", ATTR_TCP_FLAGS_ORIG, sizeof(uint8_t)},
    {"tcp_flags_repl", ATTR_TCP_FLAGS_REPL, sizeof(uint8_t)},
    {"tcp_mask_orig", ATTR_TCP_MASK_ORIG, sizeof(uint8_t)},
    {"tcp_mask_repl", ATTR_TCP_MASK_REPL, sizeof(uint8_t)},
    {"master_ipv4_src", ATTR_MASTER_IPV4_SRC, sizeof(uint32_t)},
    {"master_ipv4_dst", ATTR_MASTER_IPV4_DST, sizeof(uint32_t)},
    {"master_ipv6_src", ATTR_MASTER_IPV6_SRC, sizeof(uint128_t)},
    {"master_ipv6_dst", ATTR_MASTER_IPV6_DST, sizeof(uint128_t)},
    {"master_port_src", ATTR_MASTER_PORT_SRC, sizeof(uint16_t)},
    {"master_port_dst", ATTR_MASTER_PORT_DST, sizeof(uint16_t)},
    {"master_l3proto", ATTR_MASTER_L3PROTO, sizeof(uint8_t)},
    {"master_l4proto", ATTR_MASTER_L4PROTO, sizeof(uint8_t)},
    {"secmark", ATTR_SECMARK, sizeof(uint32_t)},
    {"orig_nat_seq_correction_pos", ATTR_ORIG_NAT_SEQ_CORRECTION_POS, sizeof(uint32_t)},
    {"orig_nat_seq_offset_before", ATTR_ORIG_NAT_SEQ_OFFSET_BEFORE, sizeof(uint32_t)},
    {"orig_nat_seq_offset_after", ATTR_ORIG_NAT_SEQ_OFFSET_AFTER, sizeof(uint32_t)},
    {"repl_nat_seq_correction_pos", ATTR_REPL_NAT_SEQ_CORRECTION_POS, sizeof(uint32_t)},
    {"repl_nat_seq_offset_before", ATTR_REPL_NAT_SEQ_OFFSET_BEFORE, sizeof(uint32_t)},
    {"repl_nat_seq_offset_after", ATTR_REPL_NAT_SEQ_OFFSET_AFTER, sizeof(uint32_t)},
    {"sctp_state", ATTR_SCTP_STATE, sizeof(uint8_t)},
    {"sctp_vtag_orig", ATTR_SCTP_VTAG_ORIG, sizeof(uint32_t)},
    {"sctp_vtag_repl", ATTR_SCTP_VTAG_REPL, sizeof(uint32_t)},
    {"helper_name", ATTR_HELPER_NAME, NFCT_HELPER_NAME_MAX},
    {"dccp_state", ATTR_DCCP_STATE, sizeof(uint8_t)},
    {"dccp_role", ATTR_DCCP_ROLE, sizeof(uint8_t)},
    {"dccp_handshake_seq", ATTR_DCCP_HANDSHAKE_SEQ, sizeof(uint64_t)},
    {"tcp_wscale_orig", ATTR_TCP_WSCALE_ORIG, sizeof(uint8_t)},
    {"tcp_wscale_repl", ATTR_TCP_WSCALE_REPL, sizeof(uint8_t)},
    {"zone", ATTR_ZONE, sizeof(uint16_t)},
    {"timestamp_start", ATTR_TIMESTAMP_START, sizeof(uint64_t)},
    {"timestamp_stop", ATTR_TIMESTAMP_STOP, sizeof(uint64_t)},
    {"orig_zone", ATTR_ORIG_ZONE, sizeof(uint16_t)},
    {"repl_zone", ATTR_REPL_ZONE, sizeof(uint16_t)},
    {"snat_ipv6", ATTR_SNAT_IPV6, sizeof(uint128_t)},
    {"dnat_ipv6", ATTR_DNAT_IPV6, sizeof(uint128_t)},
    {NULL}
};

static PyObject* NetfilterConntrackConntrack_destroy (NetfilterConntrackConntrack* self) {
    nfct_destroy(self->conntrack);
    self->conntrack = NULL;
    Py_RETURN_NONE;
}

static PyMemberDef NetfilterConntrackConntrack_members[] = {
    {NULL}
};

static PyMethodDef NetfilterConntrackConntrack_methods[] = {
    {"destroy", (PyCFunction) NetfilterConntrackConntrack_destroy, METH_NOARGS, NULL},
    {NULL}
};

static PyTypeObject NetfilterConntrackConntrackType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "libnetfilterconntrack.NetfilterConntrackConntrack", /* tp_name */
    sizeof(NetfilterConntrackConntrack),                 /* tp_basicsize */
    0,                                                   /* tp_itemsize */
    (destructor) NetfilterConntrackConntrack_dealloc,    /* tp_dealloc */
    0,                                                   /* tp_print */
    0,                                                   /* tp_getattr */
    0,                                                   /* tp_setattr */
    0,                                                   /* tp_compare */
    0,                                                   /* tp_repr */
    0,                                                   /* tp_as_number */
    0,                                                   /* tp_as_sequence */
    0,                                                   /* tp_as_mapping */
    0,                                                   /* tp_hash */
    0,                                                   /* tp_call */
    0,                                                   /* tp_str */
    (getattrofunc) NetfilterConntrackConntrack_GetAttr,  /* tp_getattro */
    (setattrofunc) NetfilterConntrackConntrack_SetAttr,  /* tp_setattro */
    0,                                                   /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,            /* tp_flags */
    "Wrapper for (struct nfct_conntrack *)",             /* tp_doc */
    0,                                                   /* tp_traverse */
    0,                                                   /* tp_clear */
    0,                                                   /* tp_richcompare */
    0,                                                   /* tp_weaklistoffset */
    0,                                                   /* tp_iter */
    0,                                                   /* tp_iternext */
    NetfilterConntrackConntrack_methods,                 /* tp_methods */
    NetfilterConntrackConntrack_members,                 /* tp_members */
    0,                                                   /* tp_getset */
    0,                                                   /* tp_base */
    0,                                                   /* tp_dict */
    0,                                                   /* tp_descr_get */
    0,                                                   /* tp_descr_set */
    0,                                                   /* tp_dictoffset */
    (initproc) NetfilterConntrackConntrack_init,         /* tp_init */
    0,                                                   /* tp_alloc */
    (newfunc) NetfilterConntrackConntrack_new,           /* tp_new */
};

// END: NetfilterConntrackConntrack

// BEGIN: NetfilterConntrackExpect

typedef struct {
    PyObject_HEAD
    struct nf_expect* expect;
} NetfilterConntrackExpect;

static PyObject* NetfilterConntrackExpect_new (PyTypeObject* type, PyTupleObject* args) {
    NetfilterConntrackExpect* self;
    self = (NetfilterConntrackExpect*) type->tp_alloc(type, 0);
    self->expect = NULL;
    return (PyObject*) self;
}

static int NetfilterConntrackExpect_init (NetfilterConntrackExpect* self, PyTupleObject* args) {
    return 0;
}

static void NetfilterConntrackExpect_dealloc (NetfilterConntrackExpect* self) {
    Py_TYPE(self)->tp_free((PyObject*) self);
}

static PyObject* NetfilterConntrackExpectAttributesDict = NULL;

static PyObject* NetfilterConntrackExpect_GetAttr (NetfilterConntrackExpect* self, PyObject* name) {
    PyObject* dict = NetfilterConntrackExpectAttributesDict;
    const char* attr_value;
    uint32_t attr_type;
    uint8_t attr_size;

    if (_nf_conntrack_attr_spec_dict_get(dict, name, &attr_type, &attr_size)) {
        attr_value = nfexp_get_attr(self->expect, (enum nf_expect_attr) attr_type);
        return PyString_FromStringAndSize(attr_value, attr_size);
    }

    return PyObject_GenericGetAttr(self, name);
}

static int NetfilterConntrackExpect_SetAttr (NetfilterConntrackExpect* self, PyObject* name, PyObject* value) {
    PyObject* dict = NetfilterConntrackExpectAttributesDict;
    const char* attr_value;
    uint32_t attr_type;
    uint8_t attr_size;

    if (_nf_conntrack_attr_spec_dict_get(dict, name, &attr_type, &attr_size)) {
        if (!PyString_Check(value)) {
            PyErr_SetString(PyExc_OSError, "Attribute must be a string");
            return -1;
        }

        if (PyString_Size(value) != attr_size) {
            PyErr_SetString(PyExc_OSError, "Invalid attribute size");
            return -1;
        }

        attr_value = PyString_AsString(value);
        nfexp_set_attr(self->expect, (enum nf_expect_attr) attr_type, attr_value);

        return 0;
    }

    return PyObject_GenericSetAttr(self, name, value);
}

static _nf_conntrack_attr_spec NetfilterConntrackExpectAttributes [] = {
    {"timeout", ATTR_EXP_TIMEOUT, sizeof(uint32_t)},
    {"zone", ATTR_EXP_ZONE, sizeof(uint16_t)},
    {"flags", ATTR_EXP_FLAGS, sizeof(uint32_t)},
    {"helper_name", ATTR_EXP_HELPER_NAME, NFCT_HELPER_NAME_MAX},
    {"exp_class", ATTR_EXP_CLASS, sizeof(uint32_t)},
    {"nat_dir", ATTR_EXP_NAT_DIR, sizeof(uint8_t)},
    {NULL}
};

static PyObject* NetfilterConntrackExpect_master (NetfilterConntrackExpect* self) {
    PyObject* empty;
    NetfilterConntrackTuple* tuple;

    empty = PyTuple_New(0);
    tuple = (NetfilterConntrackTuple*) PyObject_CallObject((PyObject*) &NetfilterConntrackTupleType, empty);
    Py_DECREF(empty);

    tuple->tuple = (struct nf_conntrack*) nfexp_get_attr(self->expect, (enum nf_expect_attr) ATTR_EXP_MASTER);
    if (!tuple->tuple) {
        Py_DECREF(tuple);
        PyErr_SetString(PyExc_OSError, "Unable to retrieve nfct tuple");
        return NULL;
    }

    return tuple;
}

static PyObject* NetfilterConntrackExpect_expected (NetfilterConntrackExpect* self) {
    PyObject* empty;
    NetfilterConntrackTuple* tuple;

    empty = PyTuple_New(0);
    tuple = (NetfilterConntrackTuple*) PyObject_CallObject((PyObject*) &NetfilterConntrackTupleType, empty);
    Py_DECREF(empty);

    tuple->tuple = (struct nf_conntrack*) nfexp_get_attr(self->expect, (enum nf_expect_attr) ATTR_EXP_EXPECTED);
    if (!tuple->tuple) {
        Py_DECREF(tuple);
        PyErr_SetString(PyExc_OSError, "Unable to retrieve nfct tuple");
        return NULL;
    }

    return tuple;
}

static PyObject* NetfilterConntrackExpect_mask (NetfilterConntrackExpect* self) {
    PyObject* empty;
    NetfilterConntrackTuple* tuple;

    empty = PyTuple_New(0);
    tuple = (NetfilterConntrackTuple*) PyObject_CallObject((PyObject*) &NetfilterConntrackTupleType, empty);
    Py_DECREF(empty);

    tuple->tuple = (struct nf_conntrack*) nfexp_get_attr(self->expect, (enum nf_expect_attr) ATTR_EXP_MASK);
    if (!tuple->tuple) {
        Py_DECREF(tuple);
        PyErr_SetString(PyExc_OSError, "Unable to retrieve nfct tuple");
        return NULL;
    }

    return tuple;
}

static PyObject* NetfilterConntrackExpect_nat_tuple (NetfilterConntrackExpect* self) {
    PyObject* empty;
    NetfilterConntrackTuple* tuple;

    empty = PyTuple_New(0);
    tuple = (NetfilterConntrackTuple*) PyObject_CallObject((PyObject*) &NetfilterConntrackTupleType, empty);
    Py_DECREF(empty);

    tuple->tuple = (struct nf_conntrack*) nfexp_get_attr(self->expect, (enum nf_expect_attr) ATTR_EXP_NAT_TUPLE);
    if (!tuple->tuple) {
        Py_DECREF(tuple);
        PyErr_SetString(PyExc_OSError, "Unable to retrieve nfct tuple");
        return NULL;
    }

    return tuple;
}

static PyObject* NetfilterConntrackExpect_destroy (NetfilterConntrackExpect* self) {
    nfexp_destroy(self->expect);
    self->expect = NULL;
    Py_RETURN_NONE;
}

static PyMemberDef NetfilterConntrackExpect_members[] = {
    {NULL}
};

static PyMethodDef NetfilterConntrackExpect_methods[] = {
    {"master", (PyCFunction) NetfilterConntrackExpect_master, METH_NOARGS, NULL},
    {"expected", (PyCFunction) NetfilterConntrackExpect_expected, METH_NOARGS, NULL},
    {"mask", (PyCFunction) NetfilterConntrackExpect_mask, METH_NOARGS, NULL},
    {"nat_tuple", (PyCFunction) NetfilterConntrackExpect_nat_tuple, METH_NOARGS, NULL},
    {"destroy", (PyCFunction) NetfilterConntrackExpect_destroy, METH_NOARGS, NULL},
    {NULL}
};

static PyTypeObject NetfilterConntrackExpectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "libnetfilterconntrack.NetfilterConntrackExpect", /* tp_name */
    sizeof(NetfilterConntrackExpect),                 /* tp_basicsize */
    0,                                                /* tp_itemsize */
    (destructor) NetfilterConntrackExpect_dealloc,    /* tp_dealloc */
    0,                                                /* tp_print */
    0,                                                /* tp_getattr */
    0,                                                /* tp_setattr */
    0,                                                /* tp_compare */
    0,                                                /* tp_repr */
    0,                                                /* tp_as_number */
    0,                                                /* tp_as_sequence */
    0,                                                /* tp_as_mapping */
    0,                                                /* tp_hash */
    0,                                                /* tp_call */
    0,                                                /* tp_str */
    (getattrofunc) NetfilterConntrackExpect_GetAttr,  /* tp_getattro */
    (setattrofunc) NetfilterConntrackExpect_SetAttr,  /* tp_setattro */
    0,                                                /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,         /* tp_flags */
    "Wrapper for (struct nfct_expect *)",             /* tp_doc */
    0,                                                /* tp_traverse */
    0,                                                /* tp_clear */
    0,                                                /* tp_richcompare */
    0,                                                /* tp_weaklistoffset */
    0,                                                /* tp_iter */
    0,                                                /* tp_iternext */
    NetfilterConntrackExpect_methods,                 /* tp_methods */
    NetfilterConntrackExpect_members,                 /* tp_members */
    0,                                                /* tp_getset */
    0,                                                /* tp_base */
    0,                                                /* tp_dict */
    0,                                                /* tp_descr_get */
    0,                                                /* tp_descr_set */
    0,                                                /* tp_dictoffset */
    (initproc) NetfilterConntrackExpect_init,         /* tp_init */
    0,                                                /* tp_alloc */
    (newfunc) NetfilterConntrackExpect_new,           /* tp_new */
};

// END: NetfilterConntrackExpect

// BEGIN: NetfilterConntrackHandle

typedef struct {
    PyObject_HEAD
    struct nfct_handle* handle;
    PyObject* callback_ct;
    PyObject* callback_exp;
} NetfilterConntrackHandle;

static PyObject* NetfilterConntrackHandle_new (PyTypeObject* type, PyTupleObject* args) {
    NetfilterConntrackHandle* self;
    self = (NetfilterConntrackHandle*) type->tp_alloc(type, 0);
    self->handle = NULL;
    self->callback_ct = NULL;
    self->callback_exp = NULL;
    return (PyObject*) self;
}

static int NetfilterConntrackHandle_init (NetfilterConntrackHandle* self, PyTupleObject* args) {
    return 0;
}

static void NetfilterConntrackHandle_dealloc (NetfilterConntrackHandle* self) {
    Py_TYPE(self)->tp_free((PyObject*) self);
}

static int NetfilterConntrackHandle_ct_callback (enum nf_conntrack_msg_type type_ct, struct nf_conntrack* ct, void* data) {
    PyObject* args;
    NetfilterConntrackHandle* self;

    PyObject* type;
    NetfilterConntrackConntrack* conntrack;

    PyObject* result_object;
    int result_value;

    self = (NetfilterConntrackHandle*) data;

    if (self->callback_ct) {
        type = PyInt_FromLong((long) type_ct);

        args = PyTuple_New(0);
        conntrack = (NetfilterConntrackConntrack*) PyObject_CallObject((PyObject*) &NetfilterConntrackConntrackType, args);
        Py_DECREF(args);

        conntrack->conntrack = ct;

        args = PyTuple_Pack(2, type, conntrack);
        result_object = PyObject_CallObject(self->callback_ct, args);
        Py_DECREF(args);

        Py_DECREF(conntrack);

        Py_DECREF(type);

        if (PyErr_Occurred()) {
            PyErr_PrintEx(1);
            return NFCT_CB_FAILURE;
        }

        if (!PyInt_Check(result_object)) {
            Py_DECREF(result_object);
            return NFCT_CB_STOP;
        }

        result_value = (int) PyInt_AsLong(result_object);
        Py_DECREF(result_object);
    }

    return result_value;
}

static PyObject* NetfilterConntrackHandle_ct_callback_set (NetfilterConntrackHandle* self, PyTupleObject* args) {
    unsigned int type_ct;
    PyObject* callback_ct;

    if (!PyArg_ParseTuple((PyObject*) args, "IO", &type_ct, &callback_ct)) {
        PyErr_SetString(PyExc_ValueError, "Parameters must be (enum nf_conntrack_msg_type type_ct, function callback_ct)");
        return NULL;
    }
    if (!PyCallable_Check(callback_ct)) {
        PyErr_SetString(PyExc_ValueError, "Parameters must be (enum nf_conntrack_msg_type type_ct, function callback_ct)");
        return NULL;
    }
    if (self->callback_ct) {
        PyErr_SetString(PyExc_ValueError, "Handle callback_ct already set");
        return NULL;
    }

    self->callback_ct = callback_ct;
    Py_INCREF(callback_ct);

    if (nfct_callback_register(self->handle, (enum nf_conntrack_msg_type) type_ct,
                               &NetfilterConntrackHandle_ct_callback, self)) {
        Py_DECREF(callback_ct);
        self->callback_ct = NULL;
        PyErr_SetString(PyExc_OSError, "Call to nfct_callback_register failed");
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject* NetfilterConntrackHandle_ct_callback_clear (NetfilterConntrackHandle* self) {
    if (self->callback_ct == NULL) {
        PyErr_SetString(PyExc_ValueError, "Handle callback_ct not set");
        return NULL;
    }

    nfct_callback_unregister(self->handle);
    Py_DECREF(self->callback_ct);
    self->callback_ct = NULL;

    Py_RETURN_NONE;
}

static PyObject* _NetfilterConntrackHandle_ct_send_conntrack (NetfilterConntrackHandle* self, enum nf_conntrack_query query, PyObject* data) {
    NetfilterConntrackConntrack* conntrack;
    if (Py_TYPE(data) != &NetfilterConntrackConntrackType) {
        PyErr_SetString(PyExc_ValueError, "Parameters must be (enum nf_conntrack_query query, NetfilterConntrackConntrack data)");
        return NULL;
    }
    conntrack = (NetfilterConntrackConntrack*) data;
    if (!nfct_send(self->handle, query, conntrack->conntrack))
        Py_RETURN_TRUE;
    Py_RETURN_FALSE;
}

static PyObject* _NetfilterConntrackHandle_ct_send_family (NetfilterConntrackHandle* self, enum nf_conntrack_query query, PyObject* data) {
    uint32_t family;
    if (!PyNumber_Check(data)) {
        PyErr_SetString(PyExc_ValueError, "Parameters must be (enum nf_conntrack_query query, uint32_t data)");
        return NULL;
    }
    family = (uint32_t) PyInt_AsLong(data);
    if (!nfct_send(self->handle, query, &family))
        Py_RETURN_TRUE;
    Py_RETURN_FALSE;
}

static PyObject* NetfilterConntrackHandle_ct_send (NetfilterConntrackHandle* self, PyTupleObject* args) {
    uint32_t query;
    PyObject* data;

    if (!PyArg_ParseTuple((PyObject*) args, "IO", &query, &data)) {
        PyErr_SetString(PyExc_ValueError, "Parameters must be (enum nf_conntrack_query query, object data)");
        return NULL;
    }

    switch(query) {
        case NFCT_Q_CREATE:
        case NFCT_Q_UPDATE:
        case NFCT_Q_DESTROY:
        case NFCT_Q_GET:
        case NFCT_Q_CREATE_UPDATE:
            return _NetfilterConntrackHandle_ct_send_conntrack(self, (enum nf_conntrack_query) query, data);
        case NFCT_Q_FLUSH:
        case NFCT_Q_DUMP:
        case NFCT_Q_DUMP_RESET:
            return _NetfilterConntrackHandle_ct_send_family(self, (enum nf_conntrack_query) query, data);
    }

    PyErr_SetString(PyExc_ValueError, "Unsupported query operation specified");
    return NULL;
}

static int NetfilterConntrackHandle_exp_callback (enum nf_conntrack_msg_type type_exp, struct nf_expect* exp, void* data) {
    PyObject* args;
    NetfilterConntrackHandle* self;

    PyObject* type;
    NetfilterConntrackExpect* expect;

    PyObject* result_object;
    int result_value;

    self = (NetfilterConntrackHandle*) data;

    if (self->callback_exp) {
        type = PyInt_FromLong((long) type_exp);

        args = PyTuple_New(0);
        expect = (NetfilterConntrackExpect*) PyObject_CallObject((PyObject*) &NetfilterConntrackExpectType, args);
        Py_DECREF(args);

        expect->expect = exp;

        args = PyTuple_Pack(2, type, expect);
        result_object = PyObject_CallObject(self->callback_exp, args);
        Py_DECREF(args);

        Py_DECREF(expect);

        Py_DECREF(type);

        if (PyErr_Occurred()) {
            PyErr_PrintEx(1);
            return NFCT_CB_FAILURE;
        }

        if (!PyInt_Check(result_object)) {
            Py_DECREF(result_object);
            return NFCT_CB_STOP;
        }

        result_value = (int) PyInt_AsLong(result_object);
        Py_DECREF(result_object);
    }

    return result_value;
}

static PyObject* NetfilterConntrackHandle_exp_callback_set (NetfilterConntrackHandle* self, PyTupleObject* args) {
    unsigned int type_exp;
    PyObject* callback_exp;

    if (!PyArg_ParseTuple((PyObject*) args, "IO", &type_exp, &callback_exp)) {
        PyErr_SetString(PyExc_ValueError, "Parameters must be (enum nf_conntrack_msg_type type_exp, function callback_exp)");
        return NULL;
    }
    if (!PyCallable_Check(callback_exp)) {
        PyErr_SetString(PyExc_ValueError, "Parameters must be (enum nf_conntrack_msg_type type_exp, function callback_exp)");
        return NULL;
    }
    if (self->callback_exp) {
        PyErr_SetString(PyExc_ValueError, "Handle callback_exp already set");
        return NULL;
    }

    self->callback_exp = callback_exp;
    Py_INCREF(callback_exp);

    if (nfexp_callback_register(self->handle, (enum nf_conntrack_msg_type) type_exp,
                                &NetfilterConntrackHandle_exp_callback, self)) {
        Py_DECREF(callback_exp);
        self->callback_exp = NULL;
        PyErr_SetString(PyExc_OSError, "Call to nfexp_callback_register failed");
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject* NetfilterConntrackHandle_exp_callback_clear (NetfilterConntrackHandle* self) {
    if (self->callback_exp == NULL) {
        PyErr_SetString(PyExc_ValueError, "Handle callback_exp not set");
        return NULL;
    }

    nfexp_callback_unregister(self->handle);
    Py_DECREF(self->callback_exp);
    self->callback_exp = NULL;

    Py_RETURN_NONE;
}

static PyObject* _NetfilterConntrackHandle_exp_send_conntrack (NetfilterConntrackHandle* self, enum nf_conntrack_query query, PyObject* data) {
    NetfilterConntrackExpect* expect;
    if (Py_TYPE(data) != &NetfilterConntrackExpectType) {
        PyErr_SetString(PyExc_ValueError, "Parameters must be (enum nf_conntrack_query query, NetfilterConntrackExpect data)");
        return NULL;
    }
    expect = (NetfilterConntrackExpect*) data;
    if (!nfexp_send(self->handle, query, expect->expect))
        Py_RETURN_TRUE;
    Py_RETURN_FALSE;
}

static PyObject* _NetfilterConntrackHandle_exp_send_family (NetfilterConntrackHandle* self, enum nf_conntrack_query query, PyObject* data) {
    uint32_t family;
    if (!PyNumber_Check(data)) {
        PyErr_SetString(PyExc_ValueError, "Parameters must be (enum nf_conntrack_query query, uint32_t data)");
        return NULL;
    }
    family = (uint32_t) PyInt_AsLong(data);
    if (!nfexp_send(self->handle, query, &family))
        Py_RETURN_TRUE;
    Py_RETURN_FALSE;
}

static PyObject* NetfilterConntrackHandle_exp_send (NetfilterConntrackHandle* self, PyTupleObject* args) {
    uint32_t query;
    PyObject* data;

    if (!PyArg_ParseTuple((PyObject*) args, "IO", &query, &data)) {
        PyErr_SetString(PyExc_ValueError, "Parameters must be (enum nf_conntrack_query query, object data)");
        return NULL;
    }

    switch(query) {
        case NFCT_Q_CREATE:
        case NFCT_Q_CREATE_UPDATE:
        case NFCT_Q_GET:
        case NFCT_Q_DESTROY:
            return _NetfilterConntrackHandle_exp_send_conntrack(self, (enum nf_conntrack_query) query, data);
        case NFCT_Q_FLUSH:
        case NFCT_Q_DUMP:
            return _NetfilterConntrackHandle_exp_send_family(self, (enum nf_conntrack_query) query, data);
    }

    PyErr_SetString(PyExc_ValueError, "Unsupported query operation specified");
    return NULL;
}

static PyObject* NetfilterConntrackHandle_handle (NetfilterConntrackHandle* self, PyTupleObject* args) {
    PyStringObject* data;
    unsigned char* data_str;
    unsigned int data_len;
    PyObject* address;
    uint32_t address_pid;
    uint32_t address_type;

    if (!PyArg_ParseTuple((PyObject*) args, "SO", &data, &address)) {
        PyErr_SetString(PyExc_ValueError, "Parameters must be (string data, tuple address (uint32_t pid, uint32_t type))");
        return NULL;
    }

    if (!PyTuple_Check(address)) {
        PyErr_SetString(PyExc_ValueError, "Parameters must be (string data, tuple address (uint32_t pid, uint32_t type))");
        return NULL;
    }

    if (!PyArg_ParseTuple(address, "II", &address_pid, &address_type)) {
        PyErr_SetString(PyExc_ValueError, "Parameters must be (string data, tuple address (uint32_t pid, uint32_t type))");
        return NULL;
    }

    data_str = PyString_AsString(data);
    data_len = PyString_Size(data);

    if (data_len <= 0) {
        PyErr_SetString(PyExc_ValueError, "Message has invalid length (message is empty)");
        return NULL;
    }

    if (address_pid != 0) {
        PyErr_SetString(PyExc_ValueError, "Invalid message source (message did not come from kernel)");
        return NULL;
    }

    return PyInt_FromLong((long) nfnl_process(nfct_nfnlh(self->handle), data_str, data_len));
}

static PyObject* NetfilterConntrackHandle_fd (NetfilterConntrackHandle* self) {
    return PyInt_FromLong(nfct_fd(self->handle));
}

static PyObject* NetfilterConntrackHandle_close (NetfilterConntrackHandle* self) {
    if (self->handle == NULL) {
        PyErr_SetString(PyExc_ValueError, "Handle pointer not initialized");
        return NULL;
    }

    if (nfct_close(self->handle)) {
        PyErr_SetString(PyExc_OSError, "Call to nfct_close failed");
        return NULL;
    }
    self->handle = NULL;

    if (self->callback_ct)
        NetfilterConntrackHandle_ct_callback_clear(self);
    if (self->callback_exp)
        NetfilterConntrackHandle_exp_callback_clear(self);

    Py_RETURN_NONE;
}

static PyMemberDef NetfilterConntrackHandle_members[] = {
    {NULL}
};

static PyMethodDef NetfilterConntrackHandle_methods[] = {
    {"ct_callback_set", (PyCFunction) NetfilterConntrackHandle_ct_callback_set, METH_VARARGS, NULL},
    {"ct_callback_clear", (PyCFunction) NetfilterConntrackHandle_ct_callback_clear, METH_NOARGS, NULL},
    {"ct_send", (PyCFunction) NetfilterConntrackHandle_ct_send, METH_VARARGS, NULL},
    {"exp_callback_set", (PyCFunction) NetfilterConntrackHandle_exp_callback_set, METH_VARARGS, NULL},
    {"exp_callback_clear", (PyCFunction) NetfilterConntrackHandle_exp_callback_clear, METH_NOARGS, NULL},
    {"exp_send", (PyCFunction) NetfilterConntrackHandle_exp_send, METH_VARARGS, NULL},
    {"handle", (PyCFunction) NetfilterConntrackHandle_handle, METH_VARARGS, NULL},
    {"fd", (PyCFunction) NetfilterConntrackHandle_fd, METH_NOARGS, NULL},
    {"close", (PyCFunction) NetfilterConntrackHandle_close, METH_NOARGS, NULL},
    {NULL}
};

static PyTypeObject NetfilterConntrackHandleType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "libnetfilterconntrack.NetfilterConntrackHandle", /* tp_name */
    sizeof(NetfilterConntrackHandle),                 /* tp_basicsize */
    0,                                                /* tp_itemsize */
    (destructor) NetfilterConntrackHandle_dealloc,    /* tp_dealloc */
    0,                                                /* tp_print */
    0,                                                /* tp_getattr */
    0,                                                /* tp_setattr */
    0,                                                /* tp_compare */
    0,                                                /* tp_repr */
    0,                                                /* tp_as_number */
    0,                                                /* tp_as_sequence */
    0,                                                /* tp_as_mapping */
    0,                                                /* tp_hash */
    0,                                                /* tp_call */
    0,                                                /* tp_str */
    0,                                                /* tp_getattro */
    0,                                                /* tp_setattro */
    0,                                                /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,         /* tp_flags */
    "Wrapper for (struct nfct_handle *)",             /* tp_doc */
    0,                                                /* tp_traverse */
    0,                                                /* tp_clear */
    0,                                                /* tp_richcompare */
    0,                                                /* tp_weaklistoffset */
    0,                                                /* tp_iter */
    0,                                                /* tp_iternext */
    NetfilterConntrackHandle_methods,                 /* tp_methods */
    NetfilterConntrackHandle_members,                 /* tp_members */
    0,                                                /* tp_getset */
    0,                                                /* tp_base */
    0,                                                /* tp_dict */
    0,                                                /* tp_descr_get */
    0,                                                /* tp_descr_set */
    0,                                                /* tp_dictoffset */
    (initproc) NetfilterConntrackHandle_init,         /* tp_init */
    0,                                                /* tp_alloc */
    (newfunc) NetfilterConntrackHandle_new,           /* tp_new */
};

// END: NetfilterConntrackHandle

static PyObject* libnetfilterconntrack_ct_new (PyObject* self) {
    PyObject* empty;
    NetfilterConntrackConntrack* conntrack;

    empty = PyTuple_New(0);
    conntrack = (NetfilterConntrackConntrack*) PyObject_CallObject((PyObject*) &NetfilterConntrackConntrackType, empty);
    Py_DECREF(empty);

    conntrack->conntrack = nfct_new();
    if (!conntrack->conntrack) {
        Py_DECREF(conntrack);
        PyErr_SetString(PyExc_OSError, "Call to nfct_new failed");
        return NULL;
    }

    return conntrack;
}

static PyObject* libnetfilterconntrack_ct_copy (PyObject* self, PyTupleObject* args) {
    PyObject* destination_object;
    PyObject* source_object;
    unsigned int flags;

    NetfilterConntrackConntrack* destination;
    NetfilterConntrackConntrack* source;

    if (!PyArg_ParseTuple((PyObject*) args, "OOI", &destination_object, &source_object, &flags)) {
        PyErr_SetString(PyExc_ValueError, "Parameters must be (NetfilterConntrackConntrack destination, NetfilterConntrackConntrack source, unsigned int flags)");
        return NULL;
    }

    if (Py_TYPE(destination_object) != &NetfilterConntrackConntrackType ||
             Py_TYPE(source_object) != &NetfilterConntrackConntrackType) {
        PyErr_SetString(PyExc_ValueError, "Parameters must be (NetfilterConntrackConntrack destination, NetfilterConntrackConntrack source, unsigned int flags)");
        return NULL;
    }

    destination = (NetfilterConntrackConntrack*) destination_object;
    source = (NetfilterConntrackConntrack*) source_object;
    nfct_copy(destination->conntrack, source->conntrack, flags);

    Py_RETURN_NONE;
}

static PyObject* libnetfilterconntrack_exp_new (PyObject* self) {
    PyObject* empty;
    NetfilterConntrackExpect* expect;

    empty = PyTuple_New(0);
    expect = (NetfilterConntrackExpect*) PyObject_CallObject((PyObject*) &NetfilterConntrackExpectType, empty);
    Py_DECREF(empty);

    expect->expect = nfexp_new();
    if (!expect->expect) {
        Py_DECREF(expect);
        PyErr_SetString(PyExc_OSError, "Call to nfexp_new failed");
        return NULL;
    }

    return expect;
}

static PyObject* libnetfilterconntrack_exp_copy (PyObject* self, PyTupleObject* args) {
    PyObject* destination_object;
    PyObject* source_object;

    NetfilterConntrackExpect* destination;
    NetfilterConntrackExpect* source;

    if (!PyArg_ParseTuple((PyObject*) args, "OO", &destination_object, &source_object)) {
        PyErr_SetString(PyExc_ValueError, "Parameters must be (NetfilterConntrackExpect destination, NetfilterConntrackExpect source)");
        return NULL;
    }

    if (Py_TYPE(destination_object) != &NetfilterConntrackExpectType ||
             Py_TYPE(source_object) != &NetfilterConntrackExpectType) {
        PyErr_SetString(PyExc_ValueError, "Parameters must be (NetfilterConntrackExpect destination, NetfilterConntrackExpect source)");
        return NULL;
    }

    destination = (NetfilterConntrackExpect*) destination_object;
    source = (NetfilterConntrackExpect*) source_object;
    memcpy(destination->expect, source->expect, nfexp_maxsize());

    Py_RETURN_NONE;
}

static PyObject* libnetfilterconntrack_open (PyObject* self, PyTupleObject* args) {
    PyObject* empty;
    NetfilterConntrackHandle* handle_object;
    struct nfct_handle* handle_struct;

    uint8_t systems;
    unsigned groups;

    if (!PyArg_ParseTuple((PyObject*) args, "bI", &systems, &groups)) {
        PyErr_SetString(PyExc_ValueError, "Parameters must be (uint8_t systems, unsigned groups)");
        return NULL;
    }

    handle_struct = nfct_open(systems, groups);
    if (!handle_struct) {
        PyErr_SetString(PyExc_OSError, "Call to nfct_open failed");
        return NULL;
    }

    empty = PyTuple_New(0);
    handle_object = (NetfilterConntrackHandle*) PyObject_CallObject((PyObject*) &NetfilterConntrackHandleType, empty);
    Py_DECREF(empty);

    handle_object->handle = handle_struct;
    return (PyObject*) handle_object;
}

static PyMethodDef libnetfilterconntrack_methods[] = {
    {"ct_new", (PyCFunction) libnetfilterconntrack_ct_new, METH_NOARGS, NULL},
    {"ct_copy", (PyCFunction) libnetfilterconntrack_ct_copy, METH_VARARGS, NULL},
    {"exp_new", (PyCFunction) libnetfilterconntrack_exp_new, METH_NOARGS, NULL},
    {"exp_copy", (PyCFunction) libnetfilterconntrack_exp_copy, METH_VARARGS, NULL},
    {"open", (PyCFunction) libnetfilterconntrack_open, METH_VARARGS, NULL},
    {NULL}
};

PyMODINIT_FUNC initlibnetfilterconntrack (void) {
    PyObject* module;
    PyObject* attrs;

    if (PyType_Ready(&NetfilterConntrackTupleType) < 0)
        return;
    if (PyType_Ready(&NetfilterConntrackConntrackType) < 0)
        return;
    if (PyType_Ready(&NetfilterConntrackExpectType) < 0)
        return;
    if (PyType_Ready(&NetfilterConntrackHandleType) < 0)
        return;

    module = Py_InitModule("libnetfilterconntrack", libnetfilterconntrack_methods);
    if (module == NULL)
        return;

    Py_INCREF((PyObject*) &NetfilterConntrackTupleType);
    PyModule_AddObject(module, "NetfilterConntrackTuple", (PyObject*) &NetfilterConntrackTupleType);

    Py_INCREF((PyObject*) &NetfilterConntrackConntrackType);
    PyModule_AddObject(module, "NetfilterConntrackConntrack", (PyObject*) &NetfilterConntrackConntrackType);

    Py_INCREF((PyObject*) &NetfilterConntrackExpectType);
    PyModule_AddObject(module, "NetfilterConntrackExpect", (PyObject*) &NetfilterConntrackExpectType);

    Py_INCREF((PyObject*) &NetfilterConntrackHandleType);
    PyModule_AddObject(module, "NetfilterConntrackHandle", (PyObject*) &NetfilterConntrackHandleType);

    /* Subsystems */

    PyModule_AddIntConstant(module, "NFNL_SUBSYS_NONE", NFNL_SUBSYS_NONE);
    PyModule_AddIntConstant(module, "NFNL_SUBSYS_CTNETLINK_CT", NFNL_SUBSYS_CTNETLINK);
    PyModule_AddIntConstant(module, "NFNL_SUBSYS_CTNETLINK_EXP", NFNL_SUBSYS_CTNETLINK_EXP);

    /* Groups */

    PyModule_AddIntConstant(module, "NF_NETLINK_CONNTRACK_CT_NEW", NF_NETLINK_CONNTRACK_NEW);
    PyModule_AddIntConstant(module, "NF_NETLINK_CONNTRACK_CT_UPDATE", NF_NETLINK_CONNTRACK_UPDATE);
    PyModule_AddIntConstant(module, "NF_NETLINK_CONNTRACK_CT_DESTROY", NF_NETLINK_CONNTRACK_DESTROY);
    PyModule_AddIntConstant(module, "NF_NETLINK_CONNTRACK_CT_ALL", NF_NETLINK_CONNTRACK_ALL);

    PyModule_AddIntConstant(module, "NF_NETLINK_CONNTRACK_EXP_NEW", NF_NETLINK_CONNTRACK_EXP_NEW);
    PyModule_AddIntConstant(module, "NF_NETLINK_CONNTRACK_EXP_UPDATE", NF_NETLINK_CONNTRACK_EXP_UPDATE);
    PyModule_AddIntConstant(module, "NF_NETLINK_CONNTRACK_EXP_DESTROY", NF_NETLINK_CONNTRACK_EXP_DESTROY);
    PyModule_AddIntConstant(module, "NF_NETLINK_CONNTRACK_EXP_ALL", NF_NETLINK_CONNTRACK_EXP_ALL);

    /* Types */

    PyModule_AddIntConstant(module, "NFCT_T_UNKNOWN", NFCT_T_UNKNOWN);
    PyModule_AddIntConstant(module, "NFCT_T_NEW_BIT", NFCT_T_NEW_BIT);
    PyModule_AddIntConstant(module, "NFCT_T_NEW", NFCT_T_NEW);
    PyModule_AddIntConstant(module, "NFCT_T_UPDATE_BIT", NFCT_T_UPDATE_BIT);
    PyModule_AddIntConstant(module, "NFCT_T_UPDATE", NFCT_T_UPDATE);
    PyModule_AddIntConstant(module, "NFCT_T_DESTROY_BIT", NFCT_T_DESTROY_BIT);
    PyModule_AddIntConstant(module, "NFCT_T_DESTROY", NFCT_T_DESTROY);
    PyModule_AddIntConstant(module, "NFCT_T_ALL", NFCT_T_ALL);
    PyModule_AddIntConstant(module, "NFCT_T_ERROR_BIT", NFCT_T_ERROR_BIT);
    PyModule_AddIntConstant(module, "NFCT_T_ERROR", NFCT_T_ERROR);

    /* Queries */

    PyModule_AddIntConstant(module, "NFCT_Q_CREATE", NFCT_Q_CREATE);
    PyModule_AddIntConstant(module, "NFCT_Q_UPDATE", NFCT_Q_UPDATE);
    PyModule_AddIntConstant(module, "NFCT_Q_DESTROY", NFCT_Q_DESTROY);
    PyModule_AddIntConstant(module, "NFCT_Q_GET", NFCT_Q_GET);
    PyModule_AddIntConstant(module, "NFCT_Q_FLUSH", NFCT_Q_FLUSH);
    PyModule_AddIntConstant(module, "NFCT_Q_DUMP", NFCT_Q_DUMP);
    PyModule_AddIntConstant(module, "NFCT_Q_DUMP_RESET", NFCT_Q_DUMP_RESET);
    PyModule_AddIntConstant(module, "NFCT_Q_CREATE_UPDATE", NFCT_Q_CREATE_UPDATE);
    PyModule_AddIntConstant(module, "NFCT_Q_DUMP_FILTER", NFCT_Q_DUMP_FILTER);
    PyModule_AddIntConstant(module, "NFCT_Q_DUMP_FILTER_RESET", NFCT_Q_DUMP_FILTER_RESET);

    /* Verdicts */

    PyModule_AddIntConstant(module, "NFCT_CB_FAILURE", NFCT_CB_FAILURE);
    PyModule_AddIntConstant(module, "NFCT_CB_STOP", NFCT_CB_STOP);
    PyModule_AddIntConstant(module, "NFCT_CB_CONTINUE", NFCT_CB_CONTINUE);
    PyModule_AddIntConstant(module, "NFCT_CB_STOLEN", NFCT_CB_STOLEN);

    /* Copy Flags */

    PyModule_AddIntConstant(module, "NFCT_CP_ALL", NFCT_CP_ALL);
    PyModule_AddIntConstant(module, "NFCT_CP_ORIG", NFCT_CP_ORIG);
    PyModule_AddIntConstant(module, "NFCT_CP_REPL", NFCT_CP_REPL);
    PyModule_AddIntConstant(module, "NFCT_CP_META", NFCT_CP_META);
    PyModule_AddIntConstant(module, "NFCT_CP_OVERRIDE", NFCT_CP_OVERRIDE);


    /* Attributes */

    attrs = _nf_conntrack_attr_spec_dict_new(NetfilterConntrackTupleAttributes);
    PyModule_AddObject(module, "NF_CONNTRACK_ATTR_SPECS_TUPLE", attrs);
    NetfilterConntrackTupleAttributesDict = attrs;

    attrs = _nf_conntrack_attr_spec_dict_new(NetfilterConntrackConntrackAttributes);
    PyModule_AddObject(module, "NF_CONNTRACK_ATTR_SPECS_CT", attrs);
    NetfilterConntrackConntrackAttributesDict = attrs;

    attrs = _nf_conntrack_attr_spec_dict_new(NetfilterConntrackExpectAttributes);
    PyModule_AddObject(module, "NF_CONNTRACK_ATTR_SPECS_EXP", attrs);
    NetfilterConntrackExpectAttributesDict = attrs;
}
