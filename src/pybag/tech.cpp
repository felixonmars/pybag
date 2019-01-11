#include <pybind11/pybind11.h>

#include <pybind11/stl.h>

#include <pybind11_generics/list.h>
#include <pybind11_generics/tuple.h>

#include <cbag/common/transformation.h>
#include <cbag/enum/space_type.h>
#include <cbag/layout/flip_parity.h>
#include <cbag/layout/routing_grid.h>
#include <cbag/layout/routing_grid_util.h>
#include <cbag/layout/tech_util.h>
#include <cbag/layout/track_info_util.h>
#include <cbag/layout/via_param_util.h>
#include <cbag/layout/wire_info.h>

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

std::string get_via_id(const c_tech &tech, int lev_code, const std::string &lay,
                       const std::string &purp, const std::string &adj_lay,
                       const std::string &adj_purp) {
    return tech.get_via_id(static_cast<cbag::direction>(lev_code),
                           cbag::layout::layer_t_at(tech, lay, purp),
                           cbag::layout::layer_t_at(tech, adj_lay, adj_purp));
}

c_via_param get_via_param(const c_tech &tech, int w, int h, const std::string &via_id, int lev_code,
                          cbag::orient_2d_t ex_dir, cbag::orient_2d_t adj_ex_dir, bool extend) {

    return tech.get_via_param(cbag::vector{w, h}, via_id, static_cast<cbag::direction>(lev_code),
                              static_cast<cbag::orient_2d>(ex_dir),
                              static_cast<cbag::orient_2d>(adj_ex_dir), extend);
}

cbag::offset_t get_min_space(const c_tech &tech, const std::string &layer, cbag::offset_t width,
                             const std::string &purpose, bool same_color, bool even) {
    auto sp_type = cbag::get_space_type(same_color);
    return tech.get_min_space(cbag::layout::layer_t_at(tech, layer, purpose), width, sp_type, even);
}

cbag::offset_t get_min_le_space(const c_tech &tech, const std::string &layer, cbag::offset_t width,
                                const std::string &purpose, bool even) {
    return tech.get_min_space(cbag::layout::layer_t_at(tech, layer, purpose), width,
                              cbag::space_type::LINE_END, even);
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
    py_cls.def("get_box",
               [](const c_via_param &p, const cbag::transformation &xform, int level) {
                   return cbag::layout::get_box(p, xform, static_cast<cbag::direction>(level));
               },
               "Computes the via metal BBox.", py::arg("xform"), py::arg("level"));
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
               py::arg("purpose") = "", py::arg("same_color") = false, py::arg("even") = false);
    py_cls.def("get_min_line_end_space", &pybag::tech::get_min_le_space,
               "Returns the minimum required spacing.", py::arg("layer"), py::arg("width"),
               py::arg("purpose") = "", py::arg("even") = false);
    py_cls.def("get_min_length", &cbag::layout::get_min_length,
               "Returns the minimum required length.", py::arg("layer"), py::arg("purpose"),
               py::arg("width"), py::arg("even") = false);
    py_cls.def("get_via_id", &pybag::tech::get_via_id, "Returns the via ID name.",
               py::arg("lev_code"), py::arg("lay"), py::arg("purp"), py::arg("adj_lay"),
               py::arg("adj_purp"));
    py_cls.def("get_via_param", &pybag::tech::get_via_param,
               "Calculates the via parameters from the given specs.", py::arg("w"), py::arg("h"),
               py::arg("via_id"), py::arg("lev_code"), py::arg("ex_dir"), py::arg("adj_ex_dir"),
               py::arg("extend"));
}

void bind_track_info(py::module &m) {
    using c_tr_info = cbag::layout::track_info;

    auto py_cls = py::class_<c_tr_info>(m, "TrackInfo");
    py_cls.doc() = "A class containing routing grid information on a specific layer.";
    py_cls.def_property_readonly("pitch", &c_tr_info::get_pitch, "Track pitch.");
    py_cls.def("compatible", &c_tr_info::compatible,
               "True if the two TrackInfo objects are compatible.", py::arg("other"));
}

void bind_flip_parity(py::module &m) {
    using c_fp = cbag::layout::flip_parity;

    auto py_cls = py::class_<c_fp>(m, "FlipParity");
    py_cls.doc() = "A class containing routing grid flip parity information.";
    py_cls.def("__eq__", &c_fp::operator==, py::arg("other"));
    py_cls.def("__hash__", &c_fp::get_hash);
    py_cls.def("__repr__", &c_fp::to_string);
}

void bind_routing_grid(py::module &m) {
    bind_track_info(m);
    bind_flip_parity(m);

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
    py_cls.def("get_track_info", &c_grid::track_info_at,
               "Returns the TrackInfo object on the given layer.", py::arg("lay_id"));
    py_cls.def("get_direction",
               [](const c_grid &g, int lay_id) -> pyg::PyOrient2D {
                   return pybag::util::code_to_orient_2d(
                       static_cast<cbag::orient_2d_t>(g.track_info_at(lay_id).get_direction()));
               },
               "Returns the track direction for the given layer.", py::arg("lay_id"));
    py_cls.def("get_track_offset",
               [](const c_grid &g, int lay_id) { return g.track_info_at(lay_id).get_offset(); },
               "Returns the track offset for the given layer.", py::arg("lay_id"));
    py_cls.def("get_track_pitch",
               [](const c_grid &g, int lay_id) { return g.track_info_at(lay_id).get_pitch(); },
               "Returns the track pitch for the given layer.", py::arg("lay_id"));
    py_cls.def("get_min_length",
               [](const c_grid &g, int lay_id, int num_tr, bool even) {
                   return g.track_info_at(lay_id).get_wire_info(num_tr).get_min_length(
                       *g.get_tech(), lay_id, even);
               },
               "Returns the track pitch for the given layer.", py::arg("lay_id"), py::arg("num_tr"),
               py::arg("even") = false);
    py_cls.def("get_space",
               [](const c_grid &g, int lay_id, int num_tr, bool same_color, bool even) {
                   auto sp_type = cbag::get_space_type(same_color);
                   return g.track_info_at(lay_id).get_wire_info(num_tr).get_min_space(
                       *g.get_tech(), lay_id, sp_type, even);
               },
               "Returns the track pitch for the given layer.", py::arg("lay_id"), py::arg("num_tr"),
               py::arg("same_color") = false, py::arg("even") = false);
    py_cls.def("get_line_end_space",
               [](const c_grid &g, int lay_id, int num_tr, bool even) {
                   return g.track_info_at(lay_id).get_wire_info(num_tr).get_min_space(
                       *g.get_tech(), lay_id, cbag::space_type::LINE_END, even);
               },
               "Returns the track pitch for the given layer.", py::arg("lay_id"), py::arg("num_tr"),
               py::arg("even") = false);
    py_cls.def(
        "get_min_space_htr",
        [](const c_grid &g, int lay_id, int num_tr, bool same_color, bool even) {
            return cbag::layout::get_min_space_htr(g.track_info_at(lay_id), *g.get_tech(), lay_id,
                                                   num_tr, same_color, even);
        },
        "Returns the minimum space required around the given wire in number of half-pitches.",
        py::arg("lay_id"), py::arg("num_tr"), py::arg("same_color") = false,
        py::arg("even") = false);
    py_cls.def("get_via_extensions",
               [](const c_grid &g, int lev_code, int lay_id, int bot_ntr, int top_ntr) {
                   auto tmp = cbag::layout::get_via_extensions(
                       g, static_cast<cbag::direction>(lev_code), lay_id, bot_ntr, top_ntr);
                   return pyg::Tuple<py::int_, py::int_>::make_tuple(tmp[0], tmp[1]);
               },
               "Returns the via extensions .", py::arg("lev_code"), py::arg("lay_id"),
               py::arg("num_tr"), py::arg("adj_num_tr"));
    py_cls.def("get_line_end_space_htr",
               [](const c_grid &g, int lev_code, int lay_id, int ntr) {
                   return cbag::layout::get_line_end_space_htr(
                       g, static_cast<cbag::direction>(lev_code), lay_id, ntr);
               },
               "Returns the line-end space measured in half-tracks.", py::arg("lev_code"),
               py::arg("lay_id"), py::arg("ntr"));
    py_cls.def(
        "get_block_size",
        [](const c_grid &g, int level, bool include_private, bool half_blk_x, bool half_blk_y) {
            std::array<bool, 2> half_blk = {half_blk_x, half_blk_y};
            auto ans = cbag::layout::get_blk_size(g, level, include_private, half_blk);
            return pyg::Tuple<py::int_, py::int_>::make_tuple(ans[0], ans[1]);
        },
        "Returns the unit block size given top routing level.", py::arg("layer_id"),
        py::arg("include_private") = false, py::arg("half_blk_x") = false,
        py::arg("half_blk_y") = false);
    py_cls.def("size_defined", &cbag::layout::block_defined_at,
               "Returns True if both width and height are quantized at the given layer.",
               py::arg("layer_id"));
    py_cls.def("get_size_pitch",
               [](const c_grid &g, int lay_id) {
                   auto ans = cbag::layout::get_top_track_pitches(g, lay_id);
                   return pyg::Tuple<py::int_, py::int_>::make_tuple(ans[0], ans[1]);
               },
               "Returns the pitches that defines template size.", py::arg("layer_id"));
    py_cls.def("get_flip_parity_at", &c_grid::get_flip_parity_at,
               "Gets the flip_parity information at the given location.", py::arg("bot_layer"),
               py::arg("top_layer"), py::arg("xform"));
    py_cls.def("get_htr_parity", &c_grid::get_htr_parity,
               "Gets parity of the given half-track index.", py::arg("layer_id"), py::arg("htr"));
    py_cls.def("get_htr_layer_purpose",
               [](const c_grid &g, int lay_id, int htr) {
                   auto key = cbag::layout::get_layer_t(g, lay_id, htr);
                   auto &tech = *g.get_tech();
                   return pyg::Tuple<py::str, py::str>::make_tuple(
                       tech.get_layer_name(key.first), tech.get_purpose_name(key.second));
               },
               "Returns the layer/purpose pair of the given half-track index.", py::arg("layer_id"),
               py::arg("htr"));
    py_cls.def("get_wire_bounds_htr",
               [](const c_grid &g, int lay_id, int htr, int ntr) {
                   auto ans = cbag::layout::get_wire_bounds(g, lay_id, htr, ntr);
                   return pyg::Tuple<py::int_, py::int_>::make_tuple(ans[0], ans[1]);
               },
               "Returns the wire boundary coordinates.", py::arg("layer_id"), py::arg("htr"),
               py::arg("ntr"));
    py_cls.def("set_flip_parity", &c_grid::set_flip_parity, "Sets the flip_parity information.",
               py::arg("fp_data"));
}
