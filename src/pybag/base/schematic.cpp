
#include <map>
#include <string>
#include <variant>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <pybind11_generics/iterator.h>

#include <cbag/schematic/arc.h>
#include <cbag/schematic/cellview.h>
#include <cbag/schematic/donut.h>
#include <cbag/schematic/ellipse.h>
#include <cbag/schematic/eval_text.h>
#include <cbag/schematic/instance.h>
#include <cbag/schematic/line.h>
#include <cbag/schematic/path.h>
#include <cbag/schematic/pin_figure.h>
#include <cbag/schematic/polygon.h>
#include <cbag/schematic/term_t.h>

#include <pybag/base/schematic.h>

namespace pyg = pybind11_generics;

using inst_iter_t = std::map<std::string, cbag::sch::instance>::iterator;

using c_cellview = cbag::sch::cellview;

namespace pybag {
namespace schematic {

class const_term_iterator {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = std::string;
    using difference_type = cbag::sch::term_t::const_iterator::difference_type;
    using pointer = const value_type *;
    using reference = const value_type &;

  private:
    cbag::sch::term_t::const_iterator iter_;

  public:
    const_term_iterator() = default;
    const_term_iterator(cbag::sch::term_t::const_iterator val) : iter_(std::move(val)) {}

    bool operator==(const const_term_iterator &other) const { return iter_ == other.iter_; }
    bool operator!=(const const_term_iterator &other) const { return iter_ != other.iter_; }

    reference operator*() const { return iter_->first; }
    pointer operator->() const { return &operator*(); }

    const_term_iterator &operator++() {
        ++iter_;
        return *this;
    }
    const_term_iterator operator++(int) {
        const_term_iterator ans(iter_);
        operator++();
        return ans;
    }
};

class c_inst_ref {
  private:
    inst_iter_t iter_;

  public:
    c_inst_ref(inst_iter_t iter) : iter_(std::move(iter)) {}

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

// python/C++ interface functions for cellview
pyg::Iterator<c_inst_ref> inst_ref_iter(const c_cellview &cv) {
    return pyg::make_iterator(cv.instances.begin(), cv.instances.end());
}

pyg::Iterator<std::string> get_term_iter(const cbag::sch::term_t &term_map) {
    return pyg::make_iterator(const_term_iterator(term_map.begin()),
                              const_term_iterator(term_map.end()));
}

} // namespace schematic
} // namespace pybag

namespace pysch = pybag::schematic;

void bind_schematic(py::module &m_top) {
    py::module m = m_top.def_submodule("schematic");

    m.doc() = "This module contains various classes for schematic manipulation.";
    auto py_inst = py::class_<pysch::c_inst_ref>(m, "PySchInstRef");
    py_inst.doc() = "A reference to a schematic instance inside a cellview.";
    py_inst.def_property_readonly("name", &pysch::c_inst_ref::name, "Instance name.");
    py_inst.def_property_readonly("width", &pysch::c_inst_ref::width, "Instance symbol width.");
    py_inst.def_property_readonly("height", &pysch::c_inst_ref::width, "Instance symbol height.");
    py_inst.def_property("lib_name", &pysch::c_inst_ref::lib_name, &pysch::c_inst_ref::set_lib_name,
                         "Instance master library name.");
    py_inst.def_property("cell_name", &pysch::c_inst_ref::cell_name,
                         &pysch::c_inst_ref::set_cell_name, "Instance master cell name.");
    py_inst.def_property("is_primitive", &pysch::c_inst_ref::is_primitive,
                         &pysch::c_inst_ref::set_is_primitive,
                         "True if the instance master is not a generator.");
    py_inst.def("set_param", &pysch::c_inst_ref::set_param, "Set instance parameter value.",
                py::arg("key"), py::arg("val"));
    py_inst.def("update_master", &pysch::c_inst_ref::update_master, "Update the instance master.",
                py::arg("lib"), py::arg("cell"), py::arg("prim") = false);
    py_inst.def("update_connection", &pysch::c_inst_ref::update_connection,
                "Update instance pin connection.", py::arg("term_name"), py::arg("net_name"));

    pyg::declare_iterator<inst_iter_t>();
    pyg::declare_iterator<pysch::const_term_iterator>();

    auto py_cv = py::class_<c_cellview>(m, "PySchCellView");
    py_cv.doc() = "A schematic master cellview.";
    py_cv.def_readonly("lib_name", &c_cellview::lib_name, "Master library name.");
    py_cv.def_readonly("view_name", &c_cellview::view_name, "Master view name.");
    py_cv.def_readwrite("cell_name", &c_cellview::cell_name, "Master cell name.");
    py_cv.def("clear_params", &c_cellview::clear_params, "Clear all schematic parameters.");
    py_cv.def("set_param", &c_cellview::set_param, "Set schematic parameter value.",
              py::arg("name"), py::arg("val"));
    py_cv.def("inst_refs", &pysch::inst_ref_iter,
              "Returns an iterator over all instance references.");
    py_cv.def("in_terms", [](const c_cellview &cv) { return pysch::get_term_iter(cv.in_terms); },
              "Returns an iterator over all input terminals.");
    py_cv.def("out_terms", [](const c_cellview &cv) { return pysch::get_term_iter(cv.out_terms); },
              "Returns an iterator over all output terminals.");
    py_cv.def("io_terms", [](const c_cellview &cv) { return pysch::get_term_iter(cv.io_terms); },
              "Returns an iterator over all inout terminals.");
    py_cv.def("rename_pin", &c_cellview::rename_pin, "Rename the given pin.", py::arg("old_name"),
              py::arg("new_name"));
    py_cv.def("add_pin", &c_cellview::add_pin, "Add the given pin.", py::arg("new_name"),
              py::arg("term_type"));
    py_cv.def("remove_pin", &c_cellview::remove_pin, "Removes the given pin.", py::arg("name"));
    // TODO: complete the rest of the wrapping
}
