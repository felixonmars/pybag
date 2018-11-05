
#include <pybind11/pybind11.h>

#include <pybind11_generics/iterable.h>
#include <pybind11_generics/list.h>

#include <cbag/layout/cellview.h>
#include <cbag/schematic/pin_figure.h>
#include <cbag/schematic/shape_t_def.h>

#include <cbagoa/cbagoa.h>
#include <cbagoa/oa_read_lib.h>
#include <cbagoa/oa_write_lib.h>

using c_db = cbagoa::oa_database;

namespace py = pybind11;
namespace pyg = pybind11_generics;

namespace pybag {
namespace oa {

using cell_key_t = cbagoa::cell_key_t;

pyg::List<std::string> get_cells_in_lib(const c_db &db, const std::string &lib_name) {
    pyg::List<std::string> ans(0);
    cbagoa::get_cells(db.ns_native, *db.logger, lib_name, std::back_inserter(ans));
    return ans;
}

pyg::List<cell_key_t> read_sch_recursive(const c_db &db, const std::string &lib_name,
                                         const std::string &cell_name,
                                         const std::string &view_name) {
    pyg::List<cell_key_t> ans(0);
    cbagoa::read_sch_recursive(db.ns_native, db.ns, *db.logger, lib_name, cell_name, view_name,
                               db.yaml_path_map, db.primitive_libs, std::back_inserter(ans));
    return ans;
}

pyg::List<cell_key_t> read_library(const c_db &db, const std::string &lib_name,
                                   const std::string &view_name) {
    pyg::List<cell_key_t> ans(0);
    cbagoa::read_library(db.ns_native, db.ns, *db.logger, lib_name, view_name, db.yaml_path_map,
                         db.primitive_libs, std::back_inserter(ans));
    return ans;
}

void implement_sch_list(const c_db &db, const std::string &lib_name, const std::string &sch_view,
                        const std::string &sym_view,
                        const pyg::Iterable<cbagoa::sch_cv_info> &cv_list) {
    cbagoa::implement_sch_list<pyg::Iterable<cbagoa::sch_cv_info>>(
        db.ns_native, db.ns, *db.logger, lib_name, sch_view, sym_view, cv_list);
}

void implement_lay_list(const c_db &db, const std::string &lib_name, const std::string &view,
                        const pyg::Iterable<cbagoa::lay_cv_info> &cv_list) {
    cbagoa::implement_lay_list<pyg::Iterable<cbagoa::lay_cv_info>>(db.ns_native, db.ns, *db.logger,
                                                                   lib_name, view, cv_list);
}

} // namespace oa
} // namespace pybag

namespace pyoa = pybag::oa;

PYBIND11_MODULE(oa, m) {
    m.doc() = "This module contains various classes for reading/writing OA database.";

    auto py_cls = py::class_<c_db>(m, "PyOADatabase");
    py_cls.doc() = "A class that reads/writes OA Database.";

    py_cls.def(py::init<std::string>(), "Opens the given library file for read/write.",
               py::arg("lib_def_fname"));
    py_cls.def("add_yaml_path", &c_db::add_yaml_path, "Add library yaml file path.",
               py::arg("lib_name"), py::arg("yaml_path"));
    py_cls.def("add_primitive_lib", &c_db::add_primitive_lib, "Register primitive library.",
               py::arg("lib_name"));
    py_cls.def("is_primitive_lib", &c_db::add_primitive_lib,
               "Returns true if the given library is primitive.", py::arg("lib_name"));
    py_cls.def("get_cells_in_lib", &pyoa::get_cells_in_lib, "Get all cells in given library.",
               py::arg("lib_name"));
    py_cls.def("get_lib_path", &c_db::get_lib_path, "Get path to the given library.",
               py::arg("lib_name"));
    py_cls.def("create_lib", &c_db::create_lib, "Create a new library if it does not exist.",
               py::arg("lib_name"), py::arg("lib_path"), py::arg("tech_lib"));
    py_cls.def("read_sch_recursive", &pyoa::read_sch_recursive,
               "Read the given schematic (recursively).", py::arg("lib_name"), py::arg("cell_name"),
               py::arg("view_name"));
    py_cls.def("read_library", &pyoa::read_library,
               "Read all schematics in the given library (recursively).", py::arg("lib_name"),
               py::arg("view_name"));
    py_cls.def("implement_sch_list", &pyoa::implement_sch_list, "Write all given schematics.",
               py::arg("lib_name"), py::arg("sch_view"), py::arg("sym_view"), py::arg("cv_list"));
    py_cls.def("implement_lay_list", &pyoa::implement_lay_list, "Write all given layouts.",
               py::arg("lib_name"), py::arg("view"), py::arg("cv_list"));
}
