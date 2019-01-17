#include <pybind11/pybind11.h>

#include <pybind11_generics/iterator.h>
#include <pybind11_generics/optional.h>
#include <pybind11_generics/tuple.h>

#include <cbag/common/transformation.h>
#include <cbag/layout/flip_parity.h>
#include <cbag/layout/grid_object.h>
#include <cbag/layout/routing_grid.h>
#include <cbag/layout/routing_grid_util.h>
#include <cbag/layout/tech_util.h>
#include <cbag/layout/track_info_util.h>
#include <cbag/layout/wire_width.h>

#include <pybag/grid.h>
#include <pybag/orient_conv.h>

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

using c_tech = cbag::layout::tech;
using c_grid = cbag::layout::routing_grid;

namespace pybag {
namespace tech {

class py_warr_rect_iterator {
  private:
    cbag::layout::warr_rect_iterator iter;

  public:
    py_warr_rect_iterator() = default;

    py_warr_rect_iterator(cbag::layout::warr_rect_iterator &&iter) : iter(std::move(iter)) {}

    bool operator==(const py_warr_rect_iterator &rhs) const { return iter == rhs.iter; }
    bool operator!=(const py_warr_rect_iterator &rhs) const { return iter != rhs.iter; }

    pyg::Tuple<py::str, py::str, cbag::box_t> operator*() const {
        auto [lay_t, box] = *iter;
        auto &tech = *(iter.get_grid().get_tech());
        return pyg::Tuple<py::str, py::str, cbag::box_t>::make_tuple(
            tech.get_layer_name(lay_t.first), tech.get_purpose_name(lay_t.second), box);
    }
    py_warr_rect_iterator &operator++() {
        ++iter;
        return *this;
    }
};

std::unique_ptr<c_grid> make_grid(const c_tech *tech_ptr, const std::string &fname,
                                  pyg::Optional<const c_grid> grid) {
    std::unique_ptr<c_grid> ans;
    if (grid) {
        ans = std::make_unique<c_grid>(*grid);
    } else {
        ans = std::make_unique<c_grid>(tech_ptr, fname);
    }
    return ans;
}

} // namespace tech
} // namespace pybag

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

void bind_track_id(py::module &m) {
    using c_tid = cbag::layout::track_id;

    auto py_cls = py::class_<c_tid, std::shared_ptr<c_tid>>(m, "PyTrackID");
    py_cls.doc() = "A class containing track index information.";
    py_cls.def(py::init<cbag::level_t, cbag::htr_t, cbag::cnt_t, cbag::cnt_t, cbag::htr_t>(),
               "Create a new PyTrackID object.", py::arg("layer_id"), py::arg("htr"),
               py::arg("ntr"), py::arg("num"), py::arg("htr_pitch"));
    py_cls.def("__eq__", &c_tid::operator==, py::arg("other"));
    py_cls.def("__hash__", &c_tid::get_hash);
    py_cls.def("__repr__", &c_tid::to_string);

    py_cls.def_property_readonly("layer_id", &c_tid::get_level, "The layer ID.");
    py_cls.def_property_readonly("width", &c_tid::get_ntr, "The wire width in number of tracks.");
    py_cls.def_property_readonly("num", &c_tid::get_num, "Number of wires in this PyTrackID.");
    py_cls.def_property("base_htr", &c_tid::get_htr, &c_tid::set_htr, "The half-track index.");
    py_cls.def_property("htr_pitch", &c_tid::get_pitch, &c_tid::set_pitch,
                        "Pitch betweek wires in half-tracks.");
    py_cls.def("get_bounds",
               [](const c_tid &self, const c_grid &grid) {
                   auto ans = self.get_bounds(grid);
                   return pyg::Tuple<py::int_, py::int_>::make_tuple(ans[0], ans[1]);
               },
               "Returns the bounds of this track ID.", py::arg("grid"));
}

void bind_wire_array(py::module &m) {
    using c_warr = cbag::layout::wire_array;

    pyg::declare_iterator<pybag::tech::py_warr_rect_iterator>();

    auto py_cls = py::class_<c_warr>(m, "PyWireArray");
    py_cls.doc() = "A class containing track index information.";
    py_cls.def(py::init<std::shared_ptr<cbag::layout::track_id>, cbag::offset_t, cbag::offset_t>(),
               "Create a new PyWireArray object.", py::arg("track_id"), py::arg("lower"),
               py::arg("upper"));
    py_cls.def("__eq__", &c_warr::operator==, py::arg("other"));
    py_cls.def("__hash__", &c_warr::get_hash);
    py_cls.def("__repr__", &c_warr::to_string);

    py_cls.def_property_readonly(
        "lower", [](const c_warr &self) { return self.get_coord(cbag::direction::LOWER); },
        "The lower line-end coordinate.");
    py_cls.def_property_readonly(
        "upper", [](const c_warr &self) { return self.get_coord(cbag::direction::UPPER); },
        "The upper line-end coordinate.");
    py_cls.def_property_readonly("middle", &c_warr::get_middle, "The midpoint coordinate.");
    py_cls.def_property_readonly(
        "layer_id", [](const c_warr &self) { return self.get_track_id_ref().get_level(); },
        "The layer ID.");
    py_cls.def_property_readonly(
        "width", [](const c_warr &self) { return self.get_track_id_ref().get_ntr(); },
        "The wire width in number of tracks.");
    py_cls.def_property_readonly(
        "base_htr", [](const c_warr &self) { return self.get_track_id_ref().get_htr(); },
        "The base half-track index.");

    py_cls.def_property("track_id", &c_warr::get_track_id, &c_warr::set_track_id,
                        "The PyTrackID object.");
    py_cls.def("set_coord", &c_warr::set_coord, "Sets the line-end coordinates.", py::arg("lower"),
               py::arg("upper"));

    py_cls.def("wire_iter",
               [](const c_warr &self,
                  const c_grid &grid) -> pyg::Iterator<pyg::Tuple<py::str, py::str, cbag::box_t>> {
                   return pyg::make_iterator(
                       pybag::tech::py_warr_rect_iterator(self.begin_rect(grid)),
                       pybag::tech::py_warr_rect_iterator(self.end_rect(grid)));
               },
               "Returns an iterator over laypur/purpose/BBox.  The WireArray object must not be "
               "temporary.",
               py::arg("grid"));
}

void bind_routing_grid(py::module &m) {
    bind_track_info(m);
    bind_flip_parity(m);

    auto py_cls = py::class_<c_grid>(m, "PyRoutingGrid");

    bind_track_id(m);
    bind_wire_array(m);

    py_cls.doc() = "The routing grid class.";
    py_cls.def(py::init(&pybag::tech::make_grid),
               "Create a new PyRoutingGrid class from file or another RoutingGrid.",
               py::arg("tech"), py::arg("fname"), py::arg("grid"));
    py_cls.def("__eq__", &c_grid::operator==, py::arg("other"));
    py_cls.def_property("top_ignore_layer", &c_grid::get_top_ignore_level,
                        &c_grid::set_top_ignore_level, "The top ignore layer ID.");
    py_cls.def_property_readonly("bot_layer", &c_grid::get_bot_level, "The bottom layer ID.");
    py_cls.def_property_readonly("top_layer", &c_grid::get_top_level, "The top layer ID.");
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
               [](const c_grid &g, cbag::level_t lay_id) -> pyg::PyOrient2D {
                   return pybag::util::code_to_orient_2d(
                       static_cast<cbag::orient_2d_t>(g.track_info_at(lay_id).get_direction()));
               },
               "Returns the track direction for the given layer.", py::arg("lay_id"));
    py_cls.def(
        "get_track_offset",
        [](const c_grid &g, cbag::level_t lay_id) { return g.track_info_at(lay_id).get_offset(); },
        "Returns the track offset for the given layer.", py::arg("lay_id"));
    py_cls.def(
        "get_track_pitch",
        [](const c_grid &g, cbag::level_t lay_id) { return g.track_info_at(lay_id).get_pitch(); },
        "Returns the track pitch for the given layer.", py::arg("lay_id"));
    py_cls.def("get_min_length",
               [](const c_grid &g, cbag::level_t lay_id, cbag::cnt_t num_tr, bool even) {
                   auto wire_w = g.track_info_at(lay_id).get_wire_width(num_tr);
                   return cbag::layout::get_min_length(*g.get_tech(), lay_id, wire_w, even);
               },
               "Returns the track pitch for the given layer.", py::arg("lay_id"), py::arg("num_tr"),
               py::arg("even") = false);
    py_cls.def(
        "get_space",
        [](const c_grid &g, cbag::level_t lay_id, cbag::cnt_t num_tr, bool same_color, bool even) {
            auto sp_type = cbag::get_space_type(same_color);
            auto wire_w = g.track_info_at(lay_id).get_wire_width(num_tr);
            return cbag::layout::get_min_space(*g.get_tech(), lay_id, wire_w, sp_type, even);
        },
        "Returns the track pitch for the given layer.", py::arg("lay_id"), py::arg("num_tr"),
        py::arg("same_color") = false, py::arg("even") = false);
    py_cls.def("get_line_end_space",
               [](const c_grid &g, cbag::level_t lay_id, cbag::cnt_t num_tr, bool even) {
                   auto wire_w = g.track_info_at(lay_id).get_wire_width(num_tr);
                   return cbag::layout::get_min_space(*g.get_tech(), lay_id, wire_w,
                                                      cbag::space_type::LINE_END, even);
               },
               "Returns the track pitch for the given layer.", py::arg("lay_id"), py::arg("num_tr"),
               py::arg("even") = false);
    py_cls.def(
        "get_min_space_htr",
        [](const c_grid &g, cbag::level_t lay_id, cbag::cnt_t num_tr, bool same_color, bool even) {
            return cbag::layout::get_min_space_htr(g.track_info_at(lay_id), *g.get_tech(), lay_id,
                                                   num_tr, same_color, even);
        },
        "Returns the minimum space required around the given wire in number of half-pitches.",
        py::arg("lay_id"), py::arg("num_tr"), py::arg("same_color") = false,
        py::arg("even") = false);
    py_cls.def("get_via_extensions",
               [](const c_grid &g, cbag::enum_t lev_code, cbag::level_t lay_id, cbag::cnt_t bot_ntr,
                  cbag::cnt_t top_ntr) {
                   auto tmp = cbag::layout::get_via_extensions(
                       g, static_cast<cbag::direction>(lev_code), lay_id, bot_ntr, top_ntr);
                   return pyg::Tuple<py::int_, py::int_>::make_tuple(tmp[0], tmp[1]);
               },
               "Returns the via extensions .", py::arg("lev_code"), py::arg("lay_id"),
               py::arg("num_tr"), py::arg("adj_num_tr"));
    py_cls.def("get_line_end_space_htr",
               [](const c_grid &g, cbag::enum_t lev_code, cbag::level_t lay_id, cbag::cnt_t ntr) {
                   return cbag::layout::get_line_end_space_htr(
                       g, static_cast<cbag::direction>(lev_code), lay_id, ntr);
               },
               "Returns the line-end space measured in half-tracks.", py::arg("lev_code"),
               py::arg("lay_id"), py::arg("ntr"));
    py_cls.def("get_block_size",
               [](const c_grid &g, cbag::level_t level, bool include_private, bool half_blk_x,
                  bool half_blk_y) {
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
               [](const c_grid &g, cbag::level_t lay_id) {
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
               [](const c_grid &g, cbag::level_t lay_id, cbag::htr_t htr) {
                   auto key = cbag::layout::get_layer_t(g, lay_id, htr);
                   auto &tech = *g.get_tech();
                   return pyg::Tuple<py::str, py::str>::make_tuple(
                       tech.get_layer_name(key.first), tech.get_purpose_name(key.second));
               },
               "Returns the layer/purpose pair of the given half-track index.", py::arg("layer_id"),
               py::arg("htr"));
    py_cls.def("get_wire_total_width",
               [](const c_grid &g, cbag::level_t lay_id, cbag::cnt_t ntr) {
                   auto &tinfo = g.track_info_at(lay_id);
                   return tinfo.get_wire_width(ntr).get_total_width(tinfo.get_pitch() / 2);
               },
               "Returns the total width of the given wire bundle.", py::arg("layer_id"),
               py::arg("ntr"));
    py_cls.def("get_wire_bounds_htr",
               [](const c_grid &g, cbag::level_t lay_id, cbag::htr_t htr, cbag::cnt_t ntr) {
                   auto ans = cbag::layout::get_wire_bounds(g, lay_id, htr, ntr);
                   return pyg::Tuple<py::int_, py::int_>::make_tuple(ans[0], ans[1]);
               },
               "Returns the wire boundary coordinates.", py::arg("layer_id"), py::arg("htr"),
               py::arg("ntr"));
    py_cls.def("get_wire_em_specs", &cbag::layout::get_wire_em_specs, "Returns the wire EM specs.",
               py::arg("layer_id"), py::arg("num_tr"), py::arg("length") = -1,
               py::arg("vertical") = false, py::arg("dc_temp") = -1000, py::arg("rms_dt") = -1000);
    py_cls.def("get_min_track_width", &cbag::layout::get_min_num_tr,
               "Returns the minimum number of tracks required for the given EM specs or connection "
               "to lower/upper layers.",
               py::arg("layer_id"), py::arg("idc") = 0, py::arg("iac_rms") = 0,
               py::arg("iac_peak") = 0, py::arg("length") = -1, py::arg("bot_ntr") = 0,
               py::arg("top_ntr") = 0, py::arg("dc_temp") = -1000, py::arg("rms_dt") = -1000);
    py_cls.def("coord_to_htr",
               [](const c_grid &g, cbag::level_t lay_id, cbag::offset_t coord,
                  cbag::enum_t round_mode, bool even) {
                   return cbag::layout::coord_to_htr(g.track_info_at(lay_id), coord,
                                                     static_cast<cbag::round_mode>(round_mode),
                                                     even);
               },
               "Convert coordinate to half-track index.", py::arg("layer_id"), py::arg("coord"),
               py::arg("round_mode"), py::arg("even"));
    py_cls.def(
        "find_next_htr",
        [](const c_grid &g, cbag::level_t lay_id, cbag::offset_t coord, cbag::cnt_t ntr,
           cbag::enum_t round_mode, bool even) {
            return cbag::layout::find_next_htr(g.track_info_at(lay_id), coord, ntr,
                                               static_cast<cbag::round_mode>(round_mode), even);
        },
        "Find the wire htr index that have both edges greater/less than the given coordinate.",
        py::arg("layer_id"), py::arg("coord"), py::arg("ntr"), py::arg("round_mode"),
        py::arg("even"));
    py_cls.def("htr_to_coord",
               [](const c_grid &g, cbag::level_t lay_id, cbag::htr_t htr) {
                   return cbag::layout::htr_to_coord(g.track_info_at(lay_id), htr);
               },
               "Convert half-track index to coordinate.", py::arg("layer_id"), py::arg("htr"));
    py_cls.def("transform_htr",
               [](const c_grid &g, cbag::level_t lay_id, cbag::htr_t htr,
                  const cbag::transformation &xform) {
                   return cbag::layout::transform_htr(g.track_info_at(lay_id), htr, xform);
               },
               "Transforms the given half-track index.", py::arg("layer_id"), py::arg("htr"),
               py::arg("xform"));
    py_cls.def("set_flip_parity", &c_grid::set_flip_parity, "Sets the flip_parity information.",
               py::arg("fp_data"));
    py_cls.def("set_track_offset", &c_grid::set_track_offset,
               "Sets the track offset on the given layer.", py::arg("layer_id"), py::arg("offset"));
    py_cls.def("add_new_layer",
               [](c_grid &g, cbag::level_t lay_id, cbag::orient_2d_t dir, cbag::offset_t w,
                  cbag::offset_t sp, bool is_private) {
                   g.add_new_level(lay_id, is_private, static_cast<cbag::orient_2d>(dir), w, sp);
               },
               "Adds/modifies a routing grid layer.", py::arg("layer_id"), py::arg("direction"),
               py::arg("w"), py::arg("sp"), py::arg("is_private") = true);

    m.def("coord_to_custom_htr",
          [](cbag::offset_t coord, cbag::offset_t pitch, cbag::offset_t off,
             cbag::enum_t round_mode, bool even) {
              return cbag::layout::coord_to_htr(coord, pitch, off,
                                                static_cast<cbag::round_mode>(round_mode), even);
          },
          "Convert coordinate to half-track index given pitch/offset.", py::arg("coord"),
          py::arg("pitch"), py::arg("off"), py::arg("round_mode"), py::arg("even"));
}
