#include <pybind11/pybind11.h>

    #include "pydflow.h"



namespace py = pybind11;

PYBIND11_MODULE(pydflow, m) {
    m.doc() = "Main PyDFlow bindings";

    py::class_<PyDFlowWrapper>(m, "PyDFlow")
        .def(py::init<std::string>())
        .def("add_numbers", &add_numbers)
        .def("create_testing_pipe", &create_testing_pipe);
}