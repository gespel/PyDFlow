#include <pybind11/pybind11.h>
#include "pydflow.h"

namespace py = pybind11;

PYBIND11_MODULE(pydflow, m) {
    m.doc() = "Main PyDFlow bindings";

    py::class_<PyDFlowPipe>(m, "PyDFlow")
        .def(py::init<std::string>())
        .def("add_numbers", &PyDFlowPipe::add_numbers)
        //.def("get_port", &PyDFlow::getPort)
        .def("create_testing_pipe", &PyDFlowPipe::create_testing_pipe)
        .def("create_entry", &PyDFlowPipe::create_entry)
        .def("dump_pipe_information_for_port", &PyDFlowPipe::dumpPipeInformationForPort);
}