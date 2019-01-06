#include <pybind11/pybind11.h>

#include <pybind11/stl.h>

#include <pybind11_generics/list.h>
#include <pybind11_generics/tuple.h>

#include <cbag/common/transformation.h>
#include <cbag/enum/space_type.h>
#include <cbag/layout/routing_grid.h>
#include <cbag/layout/tech_util.h>
#include <cbag/layout/via_param_util.h>

#include <pybag/orient_conv.h>
#include <pybag/tech.h>

namespace py = pybind11;
namespace pyg = pybind11_generics;

// custom Orient2D typing definition

namespace pybind11_generics {

using obj_base = py::object;

class PyOrient2D : public obj_base {
  public:
    static bool true_check(PyObject *ptr) { return true; }

    PYBIND11_OBJECT_DEFAULT(PyOrient2D, obj_base, true_check);
};

} // namespace pybind11_generics

namespace pybind11 {
namespace detail {

template <> struct handle_type_name<pybind11_generics::PyOrient2D> {
    static constexpr auto name = _("pybag.enum.Orient2D");
};

} // namespace detail
} // namespace pybind11

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

cbag::offset_t get_min_space(const c_tech &tech, const std::string &layer, cbag::offset_t width,
                             const std::string &purpose, bool same_color) {
    auto sp_type = (same_color) ? cbag::space_type::SAME_COLOR : cbag::space_type::DIFF_COLOR;
    return tech.get_min_space(cbag::layout::layer_t_at(tech, layer, purpose), width, sp_type);
}

cbag::offset_t get_min_le_space(const c_tech &tech, const std::string &layer, cbag::offset_t width,
                                const std::string &purpose) {
    return tech.get_min_space(cbag::layout::layer_t_at(tech, layer, purpose), width,
                              cbag::space_type::LINE_END);
}

pyg::PyOrient2D get_direction(const c_grid &grid, int level) {
    return pybag::util::code_to_orient_2d(
        static_cast<cbag::orient_2d_t>(grid.get_track_info(level).get_direction()));
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
    py_cls.def_property_readonly("nx", [](const c_via_param &p) { return p.num[0]; },
                                 "Number of via columns.");
    py_cls.def_property_readonly("ny", [](const c_via_param &p) { return p.num[1]; },
                                 "Number of via rows.");
    py_cls.def_property_readonly(
        "cut_dim", [](const c_via_param &p) { return std::make_pair(p.cut_dim[0], p.cut_dim[1]); },
        "Via cut dimension.");
    py_cls.def("get_box", &cbag::layout::get_box, "Computes the via metal BBox.", py::arg("xform"),
               py::arg("level"));
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

    py_cls.def("get_layer_id", &pybag::tech::get_level, "Returns the layer level ID.",
               py::arg("layer"), py::arg("purpose") = "");
    py_cls.def("get_lay_purp_list", &pybag::tech::get_lay_purp_list,
               "Returns the layer/purpose pairs on the given layer level.", py::arg("layer_id"));
    py_cls.def("get_min_space", &pybag::tech::get_min_space,
               "Returns the minimum required spacing.", py::arg("layer"), py::arg("width"),
               py::arg("purpose") = "", py::arg("same_color") = false);
    py_cls.def("get_min_line_end_space", &pybag::tech::get_min_le_space,
               "Returns the minimum required spacing.", py::arg("layer"), py::arg("width"),
               py::arg("purpose") = "");
    py_cls.def("get_min_length", &cbag::layout::get_min_length,
               "Returns the minimum required length.", py::arg("layer"), py::arg("purpose"),
               py::arg("width"), py::arg("even") = false);
    py_cls.def("get_via_id", &pybag::tech::get_via_id, "Returns the via ID name.",
               py::arg("bot_lay"), py::arg("bot_purp"), py::arg("top_lay"), py::arg("top_purp"));
    py_cls.def("get_via_param", &pybag::tech::get_via_param,
               "Calculates the via parameters from the given specs.", py::arg("w"), py::arg("h"),
               py::arg("via_id"), py::arg("bot_dir"), py::arg("top_dir"), py::arg("extend"));
}

void bind_track_info(py::module &m) {
    using c_tr_info = cbag::layout::track_info;

    auto py_cls = py::class_<c_tr_info>(m, "TrackInfo");
    py_cls.doc() = "A class containing routing grid information on a specific layer.";
    py_cls.def_property_readonly("pitch", &c_tr_info::get_pitch, "Track pitch.");
    py_cls.def("compatible", &c_tr_info::compatible,
               "True if the two TrackInfo objects are compatible.", py::arg("other"));
}

void bind_routing_grid(py::module &m) {
    bind_track_info(m);

    auto py_cls = py::class_<c_grid>(m, "PyRoutingGrid");
    py_cls.doc() = "The routing grid class.";
    py_cls.def(py::init<const c_tech *, std::string>(),
               "Create a new PyRoutingGrid class from file.", py::arg("tech"), py::arg("fname"));
    py_cls.def_property_readonly("bot_layer", &c_grid::get_bot_level, "The bottom layer ID.");
    py_cls.def_property_readonly("top_private_layer", &c_grid::get_top_private_level,
                                 "The top private layer ID.");
    py_cls.def_property_readonly("resolution",
                                 [](const c_grid &g) { return g.get_tech()->get_resolution(); },
                                 "The layout resolution.");
    py_cls.def_property_readonly("layout_unit",
                                 [](const c_grid &g) { return g.get_tech()->get_layout_unit(); },
                                 "The layout resolution.");
    py_cls.def("get_track_info", &c_grid::get_track_info,
               "Returns the TrackInfo object on the given layer.", py::arg("lay_id"));
    py_cls.def("get_direction", &pybag::tech::get_direction,
               "Returns the track direction for the given layer.", py::arg("lay_id"));
    py_cls.def("get_track_offset",
               [](const c_grid &g, int lay_id) { return g.get_track_info(lay_id).get_offset(); },
               "Returns the track offset for the given layer.", py::arg("lay_id"));
    py_cls.def("set_flip_parity", &c_grid::set_flip_parity,
               "Sets the flip_parity parameters for the given layer.", py::arg("lay_id"),
               py::arg("scale"), py::arg("offset"));
}
