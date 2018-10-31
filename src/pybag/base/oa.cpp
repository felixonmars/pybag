
#include <cbagoa/cbagoa.h>

#include <pybag/base/oa.h>

namespace pybagoa {} // namespace pybagoa

using c_db = cbagoa::oa_database;

void bind_oa(py::module &m_top) {
    py::module m = m_top.def_submodule("oa");
    m.doc() = "This module contains various classes for reading/writing OA database.";

    auto py_cv = py::class_<c_db>(m, "PyOADatabase");
    py_cv.doc() = "A class that reads/writes OA Database.";
    py_cv.def(py::init<std::string>(), "Opens the given library file for read/write.",
              py::arg("lib_def_fname"));
}
