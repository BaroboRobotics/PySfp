#include <Python.h>

#include "sfpmodule.h"
#include "libsfp/include/sfp/serial_framing_protocol.h"

extern "C" {
static PyObject* new_context(PyObject *self, PyObject *args);
static void del_context(PyObject *obj);
static PyObject* write_packet(PyObject *self, PyObject *args);
static PyObject* connect(PyObject *self, PyObject *args);
static PyObject* set_deliver_callback(PyObject *self, PyObject *args);
static PyObject* set_write_callback(PyObject *self, PyObject *args);
}

static PyMethodDef SfpMethods[] = {
    {"new_context", new_context, METH_VARARGS, "Get a new SFP context"},
    {"write_packet", write_packet, METH_VARARGS, "Write a packet"},
    {"set_deliver_callback", set_deliver_callback, METH_VARARGS,
        "Set the deliver callback"},
    {"set_write_callback", set_write_callback, METH_VARARGS,
        "Set the write callback"},
    {"connect", connect, METH_VARARGS, "Connect the context"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef sfpmodule = {
    PyModuleDef_HEAD_INIT,
    "sfp",
    NULL,
    -1,
    SfpMethods
};

static PyObject *SfpError;

void _deliver_fun (uint8_t* buf, size_t len, void *userdata)
{
    PyObject *cb = (PyObject*)userdata;
    PyObject *arglist;
    arglist = Py_BuildValue("(y#,n)", buf, len, len);
    PyObject_CallObject(cb, arglist);
    Py_DECREF(arglist);
}

void _write_fun(uint8_t* octets, size_t len, size_t* outlen, void *userdata)
{
    PyObject *cb = (PyObject*)userdata;
    PyObject *arglist;
    arglist = Py_BuildValue("(y#,n)", octets, len, len);
    PyObject *result;
    /* Python CB should return a size_t-like value */
    result = PyObject_CallObject(cb, arglist);
    *outlen = PyLong_AsSize_t(result);
    Py_DECREF(arglist);
}

PyMODINIT_FUNC
PyInit_sfp(void)
{
    PyObject *m;

    m = PyModule_Create(&sfpmodule);
    if (m == NULL)
        return NULL;

    SfpError = PyErr_NewException("sfp.error", NULL, NULL);
    Py_INCREF(SfpError);
    PyModule_AddObject(m, "error", SfpError);
    return m;
}

static PyObject* new_context(PyObject *self, PyObject *args)
{
    pysfp_context_t *ctx;
    ctx = (pysfp_context_t*)malloc(sizeof(pysfp_context_t));
    memset(ctx, 0, sizeof(pysfp_context_t));
    sfpInit(&ctx->ctx);
    return PyCapsule_New(ctx, "SFP Context", del_context);
}

static void del_context(PyObject *obj)
{
    free(PyCapsule_GetPointer(obj, "SFP Context"));
}

/* Arguments should be: (context, buf, len)
 * Returns: len written */
static PyObject*
write_packet(PyObject *self, PyObject *args)
{
    PyObject *ctx;
    const char* buf;
    size_t len;
    if(!PyArg_ParseTuple(args, "Osn", &ctx, &buf, &len)) {
        return NULL;
    }
    size_t outlen;
    pysfp_context_t *_ctx = (pysfp_context_t*)PyCapsule_GetPointer(ctx, "SFP Context");
    int rc = sfpWritePacket(&_ctx->ctx, (const uint8_t*)buf, len, &outlen);
    if(rc < 0) {
        PyErr_SetString(SfpError, "Write packet failed.");
        return NULL;
    }
    return PyLong_FromLong(rc);
}

static PyObject*
connect(PyObject *self, PyObject *args)
{
    PyObject *ctx;
    if(!PyArg_ParseTuple(args, "O", &ctx)) {
        return NULL;
    }
    pysfp_context_t* _ctx = (pysfp_context_t*)PyCapsule_GetPointer(ctx, "SFP Context");
    sfpConnect(&_ctx->ctx);
    Py_INCREF(Py_None);
    return Py_None;
}

/* Arguments: ctx, callback */
static PyObject*
set_deliver_callback(PyObject *self, PyObject *args)
{
    PyObject *ctx;
    PyObject *cb;
    if(!PyArg_ParseTuple(args, "OO", &ctx, &cb)) {
        return NULL;
    }
    if(!PyCallable_Check(cb)) {
        PyErr_SetString(PyExc_TypeError, "parameter must be callable");
        return NULL;
    }
    pysfp_context_t* _ctx = (pysfp_context_t*)PyCapsule_GetPointer(ctx, "SFP Context");
    Py_XINCREF(cb);
    Py_XDECREF(_ctx->deliver_cb);
    _ctx->deliver_cb = cb;

    sfpSetDeliverCallback(&_ctx->ctx, _deliver_fun, _ctx->deliver_cb);

    Py_INCREF(Py_None);
    return Py_None;
}

/* Arguments: ctx, callback */
static PyObject*
set_write_callback(PyObject *self, PyObject *args)
{
    PyObject *ctx;
    PyObject *cb;
    if(!PyArg_ParseTuple(args, "OO", &ctx, &cb)) {
        return NULL;
    }
    if(!PyCallable_Check(cb)) {
        PyErr_SetString(PyExc_TypeError, "parameter must be callable");
        return NULL;
    }
    pysfp_context_t* _ctx = (pysfp_context_t*)PyCapsule_GetPointer(ctx, "SFP Context");
    Py_XINCREF(cb);
    Py_XDECREF(_ctx->deliver_cb);
    _ctx->deliver_cb = cb;
    Py_INCREF(Py_None);
    return Py_None;
}
