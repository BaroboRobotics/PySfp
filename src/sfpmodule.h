#ifndef SFPMODULE_H_
#define SFPMODULE_H_

#include <Python.h>

#include "sfp/serial_framing_protocol.h"

typedef struct pysfp_context {
    SFPcontext ctx;
    PyObject* deliver_cb;
    PyObject* write_cb;
    PyObject* lock_cb;
    PyObject* unlock_cb;
    uint8_t buf[512];
} pysfp_context_t;

#endif
