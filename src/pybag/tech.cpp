#include <pybind11/pybind11.h>

#include <pybind11/stl.h>

#include <pybind11_generics/list.h>
#include <pybind11_generics/tuple.h>

#include <cbag/layout/routing_grid.h>
#include <cbag/layout/tech_util.h>
#include <cbag/layout/via_param.h>

#include <pybag/tech.h>

namespace py = pybind11;
namespace pyg = pybind11_generics;

using c_via_param = cbag::layout::via_param;
using c_tech = cbag::layout::tech;
using c_grid = cbag::layout::routing_grid;

namespace pybag {
namespace tech {

using py_lp = pyg::Tuple<py::str, py::str>;

std::optional<int> get_level(const c_tech &tech, const std::string &layer,
                             const std::string &purpose) {
    return tech.get_level(cbag::layout::layer_t_at(tech, layer, purpose));
}

pyg::List<py_lp> get_lay_purp_list(const c_tech &tech, int level) {
    pyg::List<py_lp> ans;
    const auto &lp_list = tech.get_lay_purp_list(level);
    for (const auto &[lay_id, purp_id] : lp_list) {
        ans.push_back(
            py_lp::make_tuple(tech.get_layer_name(lay_id), tech.get_purpose_name(purp_id)));
    }
    return ans;
}

std::string get_via_id(const c_tech &tech, const std::string &bot_lay, const std::string &bot_purp,
                       const std::string &top_lay, const std::string &top_purp) {
    return tech.get_via_id(cbag::layout::layer_t_at(tech, bot_lay, bot_purp),
                           cbag::layout::layer_t_at(tech, top_lay, top_purp));
}

c_via_param get_via_param(const c_tech &tech, int w, int h, const std::string &via_id,
                          cbag::orient_2d_t bot_dir, cbag::orient_2d_t top_dir, bool extend) {

    return tech.get_via_param(cbag::vector{w, h}, via_id, static_cast<cbag::orient_2d>(bot_dir),
                              static_cast<cbag::orient_2d>(top_dir), extend);
}

} // namespace tech
} // namespace pybag

void bind_via_param(py::module &m) {
    auto py_cls = py::class_<c_via_param>(m, "ViaParam");
    py_cls.doc() = "The via parameter class.";
    py_cls.def(py::init<>(), "Create a new ViaParam object.");
    py_cls.def_property_readonly(
        "empty", [](const c_via_param &p) { return p.num[0] == 0 || p.num[1] == 0; },
        "True if this ViaParam represents an empty via.");
}

void bind_tech(py::module &m) {
    auto py_cls = py::class_<c_tech>(m, "PyTech");
    py_cls.doc() = "A class that handles technology-specific information.";
    py_cls.def(py::init<std::string>(), "Create a new PyTech class from file.",
               py::arg("tech_fname"));

    py_cls.def_property_readonly("tech_lib", &c_tech::get_tech_lib, "The technology library name.");
    py_cls.def_property_readonly("layout_unit", &c_tech::get_layout_unit, "The layout unit.");
    py_cls.def_property_readonly("resolution", &c_tech::get_resolution, "The layout resolution.");
    py_cls.def_property_readonly("use_flip_parity", &c_tech::get_use_flip_parity,
                                 "True if flip_parity dictionary is used.");
    py_cls.def_property_readonly("default_purpose", &cbag::layout::get_default_purpose_name,
                                 "The default purpose name.");
    py_cls.def_property_readonly("pin_purpose", &cbag::layout::get_pin_purpose_name,
                                 "The pin purpose name.");
    py_cls.def_property_readonly("make_pin", &c_tech::get_make_pin, "True to make pin objects.");

    py_cls.def("get_level", &pybag::tech::get_level, "Returns the level ID of the given layer.",
               py::arg("layer"), py::arg("purpose"));
    py_cls.def("get_lay_purp_list", &pybag::tech::get_lay_purp_list,
               "Returns the layer/purpose pairs on the given level.", py::arg("level"));
    py_cls.def("get_min_space", &cbag::layout::get_min_space,
               "Returns the minimum required spacing.", py::arg("layer"), py::arg("purpose"),
               py::arg("width"), py::arg("sp_type"));
    py_cls.def("get_min_length", &cbag::layout::get_min_length,
               "Returns the minimum required length.", py::arg("layer"), py::arg("purpose"),
               py::arg("width"), py::arg("even"));
    py_cls.def("get_via_id", &pybag::tech::get_via_id, "Returns the via ID name.",
               py::arg("bot_lay"), py::arg("bot_purp"), py::arg("top_lay"), py::arg("top_purp"));
    py_cls.def("get_via_param", &pybag::tech::get_via_param,
               "Calculates the via parameters from the given specs.", py::arg("w"), py::arg("h"),
               py::arg("via_id"), py::arg("bot_dir"), py::arg("top_dir"), py::arg("extend"));
}

void bind_routing_grid(py::module &m) {
    auto py_cls = py::class_<c_grid>(m, "PyRoutingGrid");
    py_cls.doc() = "The routing grid class.";
    py_cls.def(py::init<const c_tech *, std::string>(),
               "Create a new PyRoutingGrid class from file.", py::arg("tech"), py::arg("fname"));
}
