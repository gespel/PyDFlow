#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>
#include <string>
#include "pydflow.h"
#include "pipe.h"

namespace py = pybind11;

PYBIND11_MAKE_OPAQUE(std::vector<std::string>)

PYBIND11_MODULE(pydflow, m) {
    m.doc() = "Main PyDFlow bindings";

    py::class_<PyDFlow>(m, "PyDFlow")
        .def(py::init<std::string, std::string, std::string>())
        .def("dump_pipe_information_for_port", &PyDFlow::dumpPipeInformationForPort)
        .def("create_pipe", &PyDFlow::create_pipe);

    py::class_<Pipe>(m, "Pipe")
        .def("create_entry", &Pipe::create_entry)
        .def("get_entries", &Pipe::getEntries);

    py::bind_vector<std::vector<std::string>>(m, "get_entries");

    py::class_<Entry>(m, "Entry");
}