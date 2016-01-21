#include <Python.h>

#include "libsfp/include/sfp/serial_framing_protocol.h"

extern "C" {
static PyObject* new_context(PyObject *self, PyObject *args);
static void del_context(PyObject *obj);
static PyObject* write_packet(PyObject *self, PyObject *args);
static PyObject* connect(PyObject *self, PyObject *args);
}

static PyMethodDef SfpMethods[] = {
    {"new_context", new_context, METH_VARARGS, "Get a new SFP context"},
    {"write_packet", write_packet, METH_VARARGS, "Write a packet"},
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

PyObject *deliverCallback = NULL;

static PyObject* new_context(PyObject *self, PyObject *args)
{
    SFPcontext *ctx;
    ctx = (SFPcontext*)malloc(sizeof(SFPcontext));
    sfpInit(ctx);
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
    SFPcontext *ctx;
    const char* buf;
    size_t len;
    if(!PyArg_ParseTuple(args, "Osn", &ctx, &buf, &len)) {
        return NULL;
    }
    size_t outlen;
    int rc = sfpWritePacket(ctx, (const uint8_t*)buf, len, &outlen);
    if(rc < 0) {
        PyErr_SetString(SfpError, "Write packet failed.");
        return NULL;
    }
    return PyLong_FromLong(rc);
}

static PyObject*
connect(PyObject *self, PyObject *args)
{
    SFPcontext *ctx;
    if(!PyArg_ParseTuple(args, "O", &ctx)) {
        return NULL;
    }
    sfpConnect(ctx);
    Py_INCREF(Py_None);
    return Py_None;
}

