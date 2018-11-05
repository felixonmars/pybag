
#include <memory>
#include <string>
#include <variant>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <pybind11_generics/iterable.h>
#include <pybind11_generics/iterator.h>
#include <pybind11_generics/optional.h>
#include <pybind11_generics/tuple.h>

#include <cbag/schematic/cellview.h>
#include <cbag/schematic/cellview_inst_mod.h>
#include <cbag/schematic/instance.h>
#include <cbag/schematic/pin_figure.h>
#include <cbag/schematic/shape_t_def.h>

#include <cbagyaml/cbagyaml.h>

namespace pyg = pybind11_generics;

using c_instance = cbag::sch::instance;
using c_cellview = cbag::sch::cellview;

namespace pybag {
namespace schematic {

class const_inst_iterator {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = std::pair<std::string, c_instance *>;
    using difference_type = cbag::sch::inst_map_t::const_iterator::difference_type;
    using pointer = const value_type *;
    using reference = const value_type &;

  private:
    cbag::sch::inst_map_t::const_iterator iter_;

  public:
    const_inst_iterator() = default;
    const_inst_iterator(cbag::sch::inst_map_t::const_iterator val) : iter_(std::move(val)) {}

    bool operator==(const const_inst_iterator &other) const { return iter_ == other.iter_; }
    bool operator!=(const const_inst_iterator &other) const { return iter_ != other.iter_; }

    value_type operator*() const { return {iter_->first, iter_->second.get()}; }

    const_inst_iterator &operator++() {
        ++iter_;
        return *this;
    }
    const_inst_iterator operator++(int) {
        const_inst_iterator ans(iter_);
        operator++();
        return ans;
    }
};

class const_term_iterator {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = std::pair<std::string, int>;
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

    value_type operator*() const { return {iter_->first, static_cast<int>(iter_->second.ttype)}; }

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

// python/C++ interface functions for cellview
pyg::Iterator<std::pair<std::string, c_instance>> inst_ref_iter(const c_cellview &cv) {
    return pyg::make_iterator(const_inst_iterator(cv.instances.begin()),
                              const_inst_iterator(cv.instances.end()));
}

pyg::Iterator<std::pair<std::string, int>> terminals_iter(const c_cellview &cv) {
    return pyg::make_iterator(const_term_iterator(cv.terminals.begin()),
                              const_term_iterator(cv.terminals.end()));
}

bool has_terminal(const c_cellview &cv, const std::string &term) {
    return cv.terminals.find(term) != cv.terminals.end();
}

int get_terminal_type(const c_cellview &cv, const std::string &term) {
    auto iter = cv.terminals.find(term);
    if (iter == cv.terminals.end())
        throw py::key_error("cellview has no terminal named: " + term);
    return static_cast<int>(iter->second.ttype);
}

pyg::Optional<c_instance *> get_inst_ref(c_cellview &cv, const std::string &name) {
    auto iter = cv.instances.find(name);
    if (iter == cv.instances.end())
        return py::none();
    return py::cast(iter->second.get());
}

void array_instance(
    c_cellview &cv, const std::string &old_name, cbag::coord_t dx, cbag::coord_t dy,
    const pyg::Iterable<std::pair<std::string, pyg::Iterable<std::pair<std::string, std::string>>>>
        &name_conn_range) {
    cbag::sch::array_instance(cv.instances, old_name, dx, dy, name_conn_range);
}

} // namespace schematic
} // namespace pybag

namespace pysch = pybag::schematic;

PYBIND11_MODULE(schematic, m) {
    m.doc() = "This module contains various classes for schematic manipulation.";

    auto py_inst = py::class_<c_instance>(m, "PySchInstRef");
    py_inst.doc() = "A reference to a schematic instance inside a cellview.";
    py_inst.def_property_readonly("width", &c_instance::width, "Instance symbol width.");
    py_inst.def_property_readonly("height", &c_instance::width, "Instance symbol height.");
    py_inst.def_readwrite("lib_name", &c_instance::lib_name, "Instance master library name.");
    py_inst.def_readwrite("cell_name", &c_instance::cell_name, "Instance master cell name.");
    py_inst.def_readwrite("is_primitive", &c_instance::is_primitive,
                          "True if the instance master is not a generator.");
    py_inst.def("set_param", &c_instance::set_param, "Set instance parameter value.",
                py::arg("name"), py::arg("val"));
    py_inst.def("update_master", &c_instance::update_master, "Update the instance master.",
                py::arg("lib"), py::arg("cell"), py::arg("prim") = false);
    py_inst.def("update_connection",
                py::overload_cast<const std::string &, std::string, std::string>(
                    &c_instance::update_connection),
                "Update instance pin connection.", py::arg("inst_name"), py::arg("term"),
                py::arg("net"));

    pyg::declare_iterator<pysch::const_inst_iterator>();
    pyg::declare_iterator<pysch::const_term_iterator>();

    auto py_cv = py::class_<c_cellview>(m, "PySchCellView");
    py_cv.doc() = "A schematic master cellview.";
    py_cv.def(py::init(&cbag::cv_from_file), "Load cellview from yaml file.", py::arg("yaml_fname"),
              py::arg("sym_view") = "");
    py_cv.def_readonly("lib_name", &c_cellview::lib_name, "Master library name.");
    py_cv.def_readonly("view_name", &c_cellview::view_name, "Master view name.");
    py_cv.def_readwrite("cell_name", &c_cellview::cell_name, "Master cell name.");
    py_cv.def("clear_params", &c_cellview::clear_params, "Clear all schematic parameters.");
    py_cv.def("set_param", &c_cellview::set_param, "Set schematic parameter value.",
              py::arg("name"), py::arg("val"));
    py_cv.def("inst_refs", &pysch::inst_ref_iter,
              "Returns an iterator over all instance references.");
    py_cv.def("terminals", &pysch::terminals_iter,
              "Returns an iterator over all (terminal, terminal_type) tuples.");
    py_cv.def("has_terminal", &pysch::has_terminal,
              "Returns true if cellview contains the given terminal.", py::arg("term"));
    py_cv.def("get_terminal_type", &pysch::get_terminal_type, "Get the type of the given terminal.",
              py::arg("term"));
    py_cv.def("rename_pin", &c_cellview::rename_pin, "Rename the given pin.", py::arg("old_name"),
              py::arg("new_name"));
    py_cv.def("add_pin", &c_cellview::add_pin, "Add the given pin.", py::arg("new_name"),
              py::arg("term_type"));
    py_cv.def("remove_pin", &c_cellview::remove_pin, "Removes the given pin.", py::arg("name"));
    py_cv.def("rename_instance", &c_cellview::rename_instance, "Renames the given instance.",
              py::arg("old_name"), py::arg("new_name"));
    py_cv.def("remove_instance", &c_cellview::remove_instance, "Removes the given instance.",
              py::arg("name"));
    py_cv.def("get_inst_ref", &pysch::get_inst_ref, "Returns the given instance reference.",
              py::arg("name"));
    py_cv.def("array_instance", &pysch::array_instance, "Arrays the given instance.",
              py::arg("old_name"), py::arg("dx"), py::arg("dy"), py::arg("name_conn_range"));
}
