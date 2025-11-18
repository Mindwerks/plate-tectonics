#include "platecapi.hpp"
#ifdef __MINGW32__ // this is to avoid a problem with the hypot function which is messed up by Python...
#undef __STRICT_ANSI__
#endif
#include <cmath>
#include <Python.h>

#define __STDC_CONSTANT_MACROS
#if _WIN32 || _WIN64
#include <Windows.h>
typedef UINT32 uint32_t;
typedef INT32 int32_t;
#else
#include <stdint.h>
#endif

static PyObject * platec_create(PyObject *self, PyObject *args, PyObject *kwargs)
{
    unsigned int seed;
    unsigned int width;
    unsigned int height;
    float sea_level;
    unsigned int erosion_period;
    float folding_ratio;
    unsigned int aggr_overlap_abs;
    float aggr_overlap_rel;
    unsigned int cycle_count;
    unsigned int num_plates;
    
    static char *kwlist[] = {
        (char*)"seed",
        (char*)"width",
        (char*)"height",
        (char*)"sea_level",
        (char*)"erosion_period",
        (char*)"folding_ratio",
        (char*)"aggr_overlap_abs",
        (char*)"aggr_overlap_rel",
        (char*)"cycle_count",
        (char*)"num_plates",
        nullptr
    };
    
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "IIIfIfIfII", kwlist,
                                      &seed, &width, &height, &sea_level, &erosion_period,
                                      &folding_ratio, &aggr_overlap_abs, &aggr_overlap_rel,
                                      &cycle_count, &num_plates))
        return nullptr;
    srand(seed);

    void *litho = platec_api_create(seed, width, height, sea_level, erosion_period,
                                    folding_ratio, aggr_overlap_abs, aggr_overlap_rel,
                                    cycle_count, num_plates);

    Py_ssize_t pointer = (Py_ssize_t)litho;
    return Py_BuildValue("n", pointer);
}

static PyObject * platec_step(PyObject *self, PyObject *args)
{
    void *litho;
    if (!PyArg_ParseTuple(args, "n", &litho))
        return nullptr;
    platec_api_step(litho);
    return Py_BuildValue("i", 0);
}

static PyObject * platec_destroy(PyObject *self, PyObject *args)
{
    void *litho;
    if (!PyArg_ParseTuple(args, "n", &litho))
        return nullptr;
    platec_api_destroy(litho);
    return Py_BuildValue("i", 0);
}

PyObject *makelist(float array[], size_t size) {
    PyObject *l = PyList_New(size);
    for (size_t i = 0; i != size; ++i) {
        PyList_SET_ITEM(l, i, Py_BuildValue("f",array[i]));
    }
    return l;
}

PyObject *makelist_int(uint32_t array[], uint32_t size) {
    PyObject *l = PyList_New(size);
    for (uint32_t i = 0; i != size; ++i) {
        PyList_SET_ITEM(l, i, Py_BuildValue("i",array[i]));
    }
    return l;
}

static PyObject * platec_get_heightmap(PyObject *self, PyObject *args)
{
    void *litho;
    if (!PyArg_ParseTuple(args, "n", &litho))
        return nullptr;
    float *hm = platec_api_get_heightmap(litho);

    size_t width = lithosphere_getMapWidth(litho);
    size_t height = lithosphere_getMapHeight(litho);

    PyObject* res =  makelist(hm,width*height);
    Py_INCREF(res);
    return res;
}

static PyObject * platec_get_platesmap(PyObject *self, PyObject *args)
{
    void *litho;
    if (!PyArg_ParseTuple(args, "n", &litho))
        return nullptr;
    uint32_t *hm = platec_api_get_platesmap(litho);

    size_t width = lithosphere_getMapWidth(litho);
    size_t height = lithosphere_getMapHeight(litho);

    PyObject* res =  makelist_int(hm,width*height);
    Py_INCREF(res);
    return res;
}

static PyObject * platec_is_finished(PyObject *self, PyObject *args)
{
    void *litho;
    if (!PyArg_ParseTuple(args, "n", &litho))
        return nullptr;
    PyObject* res = Py_BuildValue("b",platec_api_is_finished(litho));
    return res;
}

static PyMethodDef PlatecMethods[] = {
    {   "create",  (PyCFunction)platec_create, METH_VARARGS | METH_KEYWORDS,
        "Create initial plates configuration."
    },
    {   "destroy",  platec_destroy, METH_VARARGS,
        "Release the data for the simulation."
    },
    {   "get_heightmap",  platec_get_heightmap, METH_VARARGS,
        "Get current heightmap."
    },
    {   "get_platesmap",  platec_get_platesmap, METH_VARARGS,
        "Get current plates map."
    },
    {   "step", platec_step, METH_VARARGS,
        "Perform next step of the simulation."
    },
    {   "is_finished",  platec_is_finished, METH_VARARGS,
        "Is the simulation finished?"
    },
    {nullptr, nullptr, 0, nullptr} /* Sentinel */
};

static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "platec",     /* m_name */
    "Plate tectonics simulation",  /* m_doc */
    -1,                  /* m_size */
    PlatecMethods,       /* m_methods */
    nullptr,                /* m_reload */
    nullptr,                /* m_traverse */
    nullptr,                /* m_clear */
    nullptr,                /* m_free */
};

PyMODINIT_FUNC PyInit_platec(void)
{
    return PyModule_Create(&moduledef);
}
