
#include <pybind11/pybind11.h>

#include <cbagoa/cbagoa.h>

namespace pybagoa {} // namespace pybagoa

namespace py = pybind11;

using c_db = cbagoa::oa_database;

PYBIND11_MODULE(oa, m) {
    m.doc() = "This module contains various classes for reading/writing OA database.";

    auto py_cv = py::class_<c_db>(m, "PyOADatabase");
    py_cv.doc() = "A class that reads/writes OA Database.";
    py_cv.def(py::init<std::string>(), "Opens the given library file for read/write.",
              py::arg("lib_def_fname"));
}
