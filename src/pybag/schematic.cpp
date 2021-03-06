
#include <fstream>
#include <memory>
#include <string>
#include <variant>

#include <yaml-cpp/yaml.h>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <pybind11_generics/iterable.h>
#include <pybind11_generics/iterator.h>
#include <pybind11_generics/list.h>
#include <pybind11_generics/optional.h>
#include <pybind11_generics/tuple.h>

#include <cbag/netlist/netlist.h>
#include <cbag/schematic/cellview.h>
#include <cbag/schematic/cellview_inst_mod.h>
#include <cbag/schematic/instance.h>
#include <cbag/util/io.h>
#include <cbag/yaml/cellviews.h>

#include <pybag/schematic.h>

namespace pyg = pybind11_generics;

using c_instance = cbag::sch::instance;
using c_cellview = cbag::sch::cellview;

namespace pybag {
namespace schematic {

std::string get_connection(const c_instance &inst, const std::string &term_name) {
    auto iter = inst.connections.find(term_name);
    if (iter == inst.connections.end())
        return "";
    return iter->second;
}

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

std::string cv_to_yaml(const c_cellview &cv) {
    YAML::Node node(cv);
    YAML::Emitter emitter;
    emitter << node;
    std::string str = emitter.c_str();
    str += "\n";
    return str;
}

void implement_yaml(
    const std::string &fname,
    pyg::Iterable<std::pair<std::string, std::pair<c_cellview *, std::string>>> content_list) {

    YAML::Emitter emitter;
    emitter << YAML::BeginMap;
    for (const auto &p : content_list) {
        c_cellview *ptr = p.second.first;
        if (ptr) {
            emitter << YAML::Key << p.first;
            emitter << YAML::Value << YAML::Node(*ptr);
        }
    }
    emitter << YAML::EndMap;

    cbag::util::make_parent_dirs(fname);
    std::ofstream outfile(fname, std::ios_base::out);
    outfile << emitter.c_str() << std::endl;
    outfile.close();
}

void implement_netlist(
    const std::string &fname,
    pyg::List<std::pair<std::string, std::pair<const c_cellview *, std::string>>> content_list,
    cbag::enum_t fmt_code, bool flat, bool shell, bool top_subckt, cbag::cnt_t rmin,
    const std::string &prim_fname, pyg::List<const cbag::sch::cellview_info *> cv_info_list,
    const std::string &cv_netlist,
    pyg::Optional<pyg::List<std::unique_ptr<cbag::sch::cellview_info>>> cv_info_out) {

    auto format = static_cast<cbag::design_output>(fmt_code);

    // read primitives information from file
    std::vector<std::string> inc_list;
    std::string append_file;
    cbag::sch::netlist_map_t netlist_map;
    cbag::netlist::read_prim_info(prim_fname, inc_list, netlist_map, append_file, format);

    // append cv_info_list to netlist_map
    for (const auto &cv_info_ptr : cv_info_list) {
        cbag::sch::record_cv_info(netlist_map, cbag::sch::cellview_info(*cv_info_ptr));
    }

    if (!cv_netlist.empty()) {
        inc_list.clear();
        append_file.clear();
        inc_list.emplace_back(cbag::util::get_canonical_path(cv_netlist).c_str());
    }

    cbag::netlist::write_netlist(content_list, fname, format, netlist_map, append_file, inc_list,
                                 flat, shell, top_subckt, rmin);

    if (cv_info_out.has_value()) {
        auto cv_out_list = *cv_info_out;
        for (const auto & [ cell_name, cv_netlist_pair ] : content_list) {
            auto & [ cv_ptr, netlist ] = cv_netlist_pair;
            auto cv_info_mod =
                cbag::sch::get_cv_info(netlist_map, cv_ptr->lib_name, cv_ptr->cell_name);
            cv_info_mod.cell_name = cell_name;
            cv_out_list.emplace_back(
                std::make_unique<cbag::sch::cellview_info>(std::move(cv_info_mod)));
        }
    }
}

} // namespace schematic
} // namespace pybag

namespace pysch = pybag::schematic;

void bind_schematic(py::module &m) {

    auto py_inst = py::class_<c_instance>(m, "PySchInstRef");
    py_inst.doc() = "A reference to a schematic instance inside a cellview.";
    py_inst.def_property_readonly("width", &c_instance::width, "Instance symbol width.");
    py_inst.def_property_readonly("height", &c_instance::height, "Instance symbol height.");
    py_inst.def_readwrite("lib_name", &c_instance::lib_name, "Instance master library name.");
    py_inst.def_readwrite("cell_name", &c_instance::cell_name, "Instance master cell name.");
    py_inst.def_readwrite("is_primitive", &c_instance::is_primitive,
                          "True if the instance master is not a generator.");
    py_inst.def("set_param", &c_instance::set_param, "Set instance parameter value.",
                py::arg("name"), py::arg("val"));
    py_inst.def("update_master", &c_instance::update_master, "Update the instance master.",
                py::arg("lib"), py::arg("cell"), py::arg("prim"), py::arg("keep_connections"));
    py_inst.def("update_connection",
                py::overload_cast<const std::string &, std::string, std::string>(
                    &c_instance::update_connection),
                "Update instance pin connection.", py::arg("inst_name"), py::arg("term"),
                py::arg("net"));
    py_inst.def("get_connection", pysch::get_connection, "Get net connected to the given terminal.",
                py::arg("term_name"));

    auto py_info = py::class_<cbag::sch::cellview_info>(m, "PySchCellViewInfo");
    py_info.doc() = "An information object describing a schematic master instance.";

    pyg::declare_iterator<pysch::const_inst_iterator>();
    pyg::declare_iterator<pysch::const_term_iterator>();

    auto py_cv = py::class_<c_cellview>(m, "PySchCellView");
    py_cv.doc() = "A schematic master cellview.";
    py_cv.def(py::init<std::string, std::string>(), "Load cellview from yaml file.",
              py::arg("yaml_fname"), py::arg("sym_view") = "");
    py_cv.def_readonly("view_name", &c_cellview::view_name, "Master view name.");
    py_cv.def_readwrite("lib_name", &c_cellview::lib_name, "Master library name.");
    py_cv.def_readwrite("cell_name", &c_cellview::cell_name, "Master cell name.");
    py_cv.def("get_copy", &c_cellview::get_copy, "Returns a copy of this cellview.");
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
    py_cv.def("set_pin_attribute", &c_cellview::set_pin_attribute,
              "Sets the attribute of the given pin.", py::arg("pin_name"), py::arg("key"),
              py::arg("val"));
    py_cv.def("rename_instance", &c_cellview::rename_instance, "Renames the given instance.",
              py::arg("old_name"), py::arg("new_name"));
    py_cv.def("remove_instance", &c_cellview::remove_instance, "Removes the given instance.",
              py::arg("name"));
    py_cv.def("get_inst_ref", &pysch::get_inst_ref, "Returns the given instance reference.",
              py::arg("name"));
    py_cv.def("array_instance", &pysch::array_instance, "Arrays the given instance.",
              py::arg("old_name"), py::arg("dx"), py::arg("dy"), py::arg("name_conn_range"));
    py_cv.def("to_yaml", &pysch::cv_to_yaml,
              "Returns a YAML format string representing this cellview.");

    m.def("implement_yaml", &pysch::implement_yaml, "Write the given schematics to YAML file.",
          py::arg("fname"), py::arg("content_list"));
    m.def("implement_netlist", &pysch::implement_netlist,
          "Write the given schematics to a netlist file.", py::arg("fname"),
          py::arg("content_list"), py::arg("fmt_code"), py::arg("flat"), py::arg("shell"),
          py::arg("top_subckt"), py::arg("rmin"), py::arg("prim_fname"), py::arg("cv_info_list"),
          py::arg("cv_netlist"), py::arg("cv_info_out"));
}
