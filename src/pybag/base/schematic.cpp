
#include <map>
#include <string>
#include <variant>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <cbag/schematic/instance.h>

#include <pybag/base/schematic.h>

namespace pybag {
namespace schematic {

using inst_iter_t = std::map<std::string, cbag::sch::instance>::iterator;

class sch_inst_ref {
  private:
    inst_iter_t iter_;

  public:
    sch_inst_ref(inst_iter_t iter) : iter_(std::move(iter)) {}

    std::string lib_name() const { return iter_->second.lib_name; }
    std::string cell_name() const { return iter_->second.cell_name; }
    std::string name() const { return iter_->first; }
    uint32_t width() const { return iter_->second.width(); }
    uint32_t height() const { return iter_->second.height(); }
    bool is_primitive() const { return iter_->second.is_primitive; }

    void set_lib_name(std::string val) { iter_->second.lib_name = std::move(val); }
    void set_cell_name(std::string val) { iter_->second.cell_name = std::move(val); }
    void set_is_primitive(bool val) { iter_->second.is_primitive = val; }
    void set_param(std::string key, const std::variant<int32_t, double, bool, std::string> &val) {
        iter_->second.set_param(std::move(key), val);
    }
    void update_master(std::string lib, std::string cell, bool prim = false) {
        set_lib_name(std::move(lib));
        set_cell_name(std::move(cell));
        set_is_primitive(prim);

        iter_->second.clear_params();
        iter_->second.connections.clear();
    }

    void update_connection(std::string term_name, std::string net_name) {
        iter_->second.update_connection(name(), std::move(term_name), std::move(net_name));
    }
};

} // namespace schematic
} // namespace pybag

namespace pysch = pybag::schematic;

void bind_schematic(py::module &m_top) {
    py::module m = m_top.def_submodule("schematic");

    m.doc() = "This module contains various classes for schematic manipulation.";
    auto py_inst = py::class_<pysch::sch_inst_ref>(m, "PySchInstRef");
    py_inst.doc() = "A reference to a schematic instance inside a cellview.";
    py_inst.def_property_readonly("name", &pysch::sch_inst_ref::name, "Instance name.");
    py_inst.def_property_readonly("width", &pysch::sch_inst_ref::width, "Instance symbol width.");
    py_inst.def_property_readonly("height", &pysch::sch_inst_ref::width, "Instance symbol height.");
    py_inst.def_property("lib_name", &pysch::sch_inst_ref::lib_name,
                         &pysch::sch_inst_ref::set_lib_name, "Instance master library name.");
    py_inst.def_property("cell_name", &pysch::sch_inst_ref::cell_name,
                         &pysch::sch_inst_ref::set_cell_name, "Instance master cell name.");
    py_inst.def_property("is_primitive", &pysch::sch_inst_ref::is_primitive,
                         &pysch::sch_inst_ref::set_is_primitive,
                         "True if the instance master is not a generator.");
    py_inst.def("set_param", &pysch::sch_inst_ref::set_param, "Set instance parameter value.",
                py::arg("key"), py::arg("val"));
    py_inst.def("update_master", &pysch::sch_inst_ref::update_master, "Update the instance master.",
                py::arg("lib"), py::arg("cell"), py::arg("prim") = false);
    py_inst.def("update_connection", &pysch::sch_inst_ref::update_connection,
                "Update instance pin connection.", py::arg("term_name"), py::arg("net_name"));
}
