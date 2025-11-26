#include <pybind11/pybind11.h>

extern "C" {
    #include "pydflow.h"
}


namespace py = pybind11;

PYBIND11_MODULE(pydflow, m) {
    m.doc() = "Main PyDFlow bindings";

    py::class_<PyDFLowWrapper>(m, "PyDFlow")
        .def(py::init<>())
        .def("add_numbers", &add_numbers);
}