
#include <pybind11/pybind11.h>

#include <pybind11_generics/list.h>

#include <cbagoa/cbagoa.h>
#include <cbagoa/oa_read_lib.h>

using c_db = cbagoa::oa_database;

namespace pyg = pybind11_generics;

namespace pybag {
namespace oa {

pyg::List<std::string> get_cells_in_library(const c_db &db, const std::string &lib_name) {
    pyg::List<std::string> ans(0);
    cbagoa::get_cells(db.ns_native, *db.logger, lib_name, std::back_inserter(ans));
    return ans;
}

} // namespace oa
} // namespace pybag

namespace py = pybind11;

PYBIND11_MODULE(oa, m) {
    m.doc() = "This module contains various classes for reading/writing OA database.";

    auto py_cv = py::class_<c_db>(m, "PyOADatabase");
    py_cv.doc() = "A class that reads/writes OA Database.";

    py_cv.def(py::init<std::string>(), "Opens the given library file for read/write.",
              py::arg("lib_def_fname"));
    py_cv.def("get_cells_in_library", &pybag::oa::get_cells_in_library,
              "Get all cells in given library.", py::arg("lib_name"));
}
