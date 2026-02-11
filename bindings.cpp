#include <pybind11/pybind11.h>
#include <string>
#include "pydflow.h"
#include "pipe.h"

namespace py = pybind11;

PYBIND11_MODULE(pydflow, m) {
    m.doc() = "Main PyDFlow bindings";

    py::class_<PyDFlow>(m, "PyDFlow")
        .def(py::init<std::string, std::string, std::string>())
        .def("dump_pipe_information_for_port", &PyDFlow::dumpPipeInformationForPort)
        .def("create_pipe", &PyDFlow::create_pipe);

    py::class_<Pipe>(m, "Pipe")
        .def("create_entry", &Pipe::create_entry);
        // Konstruktor ist nicht öffentlich zugänglich - Pipes werden über PyDFlow.create_pipe() erstellt
}