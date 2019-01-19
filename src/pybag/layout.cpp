#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <cbag/common/transformation_util.h>
#include <cbag/layout/cellview_poly.h>
#include <cbag/layout/cellview_util.h>
#include <cbag/layout/grid_object.h>
#include <cbag/layout/instance.h>
#include <cbag/layout/path_util.h>
#include <cbag/layout/via_wrapper.h>

#include <pybag/bbox_array.h>
#include <pybag/layout.h>
#include <pybag/py_pt_vector.h>

using c_instance = cbag::layout::instance;
using c_inst_ref = cbag::layout::cv_obj_ref<c_instance>;
using c_cellview = cbag::layout::cellview;

namespace pybag {
namespace lay {

void check_ref(const c_inst_ref &ref) {
    if (!ref.editable()) {
        throw std::runtime_error("Cannot modify committed instance");
    }
}

cbag::cnt_t get_nx(const c_inst_ref &ref) { return ref->nx; }
cbag::cnt_t get_ny(const c_inst_ref &ref) { return ref->ny; }
cbag::offset_t get_spx(const c_inst_ref &ref) { return ref->spx; }
cbag::offset_t get_spy(const c_inst_ref &ref) { return ref->spy; }
const cbag::transformation &get_xform(const c_inst_ref &ref) { return ref->xform; }

void set_nx(c_inst_ref &ref, cbag::scnt_t val) {
    check_ref(ref);
    if (val < 0)
        throw std::runtime_error("Cannot set nx to be negative.");
    ref->nx = val;
}
void set_ny(c_inst_ref &ref, cbag::scnt_t val) {
    check_ref(ref);
    if (val < 0)
        throw std::runtime_error("Cannot set nx to be negative.");
    ref->ny = val;
}
void set_spx(c_inst_ref &ref, cbag::offset_t val) {
    check_ref(ref);
    ref->spx = val;
}
void set_spy(c_inst_ref &ref, cbag::offset_t val) {
    check_ref(ref);
    ref->spy = val;
}

void move_by(c_inst_ref &ref, cbag::offset_t dx, cbag::offset_t dy) {
    check_ref(ref);
    cbag::move_by(ref->xform, dx, dy);
}

void transform(c_inst_ref &ref, const cbag::transformation &xform) {
    check_ref(ref);
    cbag::transform_by(ref->xform, xform);
}

void set_master(c_inst_ref &ref, const cbag::layout::cellview *new_master) {
    check_ref(ref);
    ref->set_master(new_master);
}

} // namespace lay
} // namespace pybag

namespace pl = pybag::lay;

void bind_inst_ref(py::module &m) {

    auto py_cls = py::class_<c_inst_ref>(m, "PyLayInstRef");
    py_cls.doc() = "A reference to a layout instance inside a cellview.";
    py_cls.def_property("nx", &pl::get_nx, &pl::set_nx, "Number of columns.");
    py_cls.def_property("ny", &pl::get_ny, &pl::set_ny, "Number of rows.");
    py_cls.def_property("spx", &pl::get_spx, &pl::set_spx, "Column pitch.");
    py_cls.def_property("spy", &pl::get_spy, &pl::set_spy, "Row pitch.");
    py_cls.def_property_readonly("xform", &pl::get_xform, "The Transform object.");
    py_cls.def("move_by", &pl::move_by, "Moves the instance.", py::arg("dx"), py::arg("dy"));
    py_cls.def("transform", &pl::transform, "Transforms the instance.", py::arg("xform"));
    py_cls.def("set_master", &pl::set_master, "Sets the instance master.", py::arg("new_master"));
    py_cls.def("commit", &c_inst_ref::commit, "Commits the instance object.");
}

void bind_cellview(py::module &m) {
    using tup_int = pyg::Tuple<py::int_, py::int_>;

    auto py_cls = py::class_<c_cellview>(m, "PyLayCellView");
    py_cls.doc() = "A layout cellview.";

    py_cls.def(py::init([](cbag::layout::routing_grid *grid_ptr, std::string cell_name,
                           cbag::enum_t geo_mode) {
                   return c_cellview{grid_ptr, std::move(cell_name),
                                     static_cast<cbag::geometry_mode>(geo_mode)};
               }),
               "Construct a new cellview.", py::arg("grid"), py::arg("cell_name"),
               py::arg("geo_mode") = 0);
    py_cls.def_property_readonly("is_empty", &c_cellview::empty, "True if this cellview is empty.");
    py_cls.def_property_readonly("cell_name", &c_cellview::get_name, "The cell name.");

    py_cls.def("__eq__", &c_cellview::operator==,
               "Returns True if the two layout cellviews are equal.", py::arg("other"));
    py_cls.def("set_geometry_mode",
               [](c_cellview &self, cbag::enum_t new_mode) {
                   self.set_geometry_mode(static_cast<cbag::geometry_mode>(new_mode));
               },
               "Set the geometry mode.", py::arg("new_mode"));
    py_cls.def("get_rect_bbox", &cbag::layout::get_bbox,
               "Get the overall bounding box on the given layer.", py::arg("layer"),
               py::arg("purpose"));
    py_cls.def("add_prim_instance", &cbag::layout::add_prim_instance, "Adds a primitive instance.",
               py::arg("lib"), py::arg("cell"), py::arg("view"), py::arg("name"), py::arg("xform"),
               py::arg("nx"), py::arg("ny"), py::arg("spx"), py::arg("spy"), py::arg("commit"));
    py_cls.def("add_instance", &cbag::layout::add_instance, "Adds an instance", py::arg("cv"),
               py::arg("name"), py::arg("xform"), py::arg("nx"), py::arg("ny"), py::arg("spx"),
               py::arg("spy"), py::arg("commit"));
    py_cls.def("add_rect", &cbag::layout::add_rect, "Adds a rectangle.", py::arg("layer"),
               py::arg("purpose"), py::arg("bbox"), py::arg("commit"));
    py_cls.def("add_rect_arr",
               [](c_cellview &cv, const std::string &layer, const std::string &purpose,
                  const cbag::box_t &box, cbag::cnt_t nx, cbag::cnt_t ny, cbag::offset_t spx,
                  cbag::offset_t spy) {
                   auto key = cbag::layout::layer_t_at(*(cv.get_tech()), layer, purpose);
                   cbag::layout::add_rect_arr(cv, key, box, {nx, ny}, {spx, spy});
               },
               "Adds an array of rectangles.", py::arg("layer"), py::arg("purpose"), py::arg("box"),
               py::arg("nx"), py::arg("ny"), py::arg("spx"), py::arg("spy"));
    py_cls.def("add_rect_arr",
               [](c_cellview &cv, const std::string &layer, const std::string &purpose,
                  const pybag::util::box_arr &barr) {
                   auto key = cbag::layout::layer_t_at(*(cv.get_tech()), layer, purpose);
                   cbag::layout::add_rect_arr(cv, key, barr.base, barr.get_num(), barr.get_sp());
               },
               "Adds an array of rectangles.", py::arg("layer"), py::arg("purpose"),
               py::arg("barr"));
    py_cls.def(
        "add_warr",
        py::overload_cast<c_cellview &, const cbag::layout::wire_array &>(&cbag::layout::add_warr),
        "Adds a WireArray.", py::arg("warr"));
    py_cls.def("add_poly", &cbag::layout::add_poly<py_pt_vector>, "Adds a new polygon.",
               py::arg("layer"), py::arg("purpose"), py::arg("points"), py::arg("commit"));
    py_cls.def("add_blockage", &cbag::layout::add_blockage<py_pt_vector>, "Adds a blockage object.",
               py::arg("layer"), py::arg("blk_code"), py::arg("points"), py::arg("commit"));
    py_cls.def("add_boundary", &cbag::layout::add_boundary<py_pt_vector>, "Adds a boundary object.",
               py::arg("bnd_code"), py::arg("points"), py::arg("commit"));
    py_cls.def("add_pin", &cbag::layout::add_pin, "Adds a pin object.", py::arg("layer"),
               py::arg("net"), py::arg("label"), py::arg("bbox"));
    py_cls.def("add_pin_arr", &cbag::layout::add_pin_arr, "Adds an arry of pins.", py::arg("net"),
               py::arg("label"), py::arg("warr"));
    py_cls.def("add_label", &cbag::layout::add_label, "Adds a label object.", py::arg("layer"),
               py::arg("purpose"), py::arg("xform"), py::arg("label"));
    py_cls.def("add_path", &cbag::layout::add_path<py_pt_vector>, "Adds a new path.",
               py::arg("layer"), py::arg("purpose"), py::arg("points"), py::arg("half_width"),
               py::arg("style0"), py::arg("style1"), py::arg("stylem"), py::arg("commit"));
    py_cls.def("add_path45_bus",
               &cbag::layout::add_path45_bus<py_pt_vector, pyg::List<cbag::offset_t>>,
               "Adds a new 45 degree path bus.", py::arg("layer"), py::arg("purpose"),
               py::arg("points"), py::arg("widths"), py::arg("spaces"), py::arg("style0"),
               py::arg("style1"), py::arg("stylem"), py::arg("commit"));
    py_cls.def("add_via", &cbag::layout::add_via, "Add a via.", py::arg("xform"), py::arg("via_id"),
               py::arg("params"), py::arg("add_layers"), py::arg("commit"));
    py_cls.def(
        "add_via_arr",
        [](c_cellview &cv, const cbag::transformation &xform, const std::string &via_id,
           const cbag::layout::via_param &params, bool add_layers, cbag::cnt_t nx, cbag::cnt_t ny,
           cbag::offset_t spx, cbag::offset_t spy) {
            cbag::layout::add_via_arr(cv, xform, via_id, params, add_layers, {nx, ny}, {spx, spy});
        },
        "Add an array of vias.", py::arg("xform"), py::arg("via_id"), py::arg("params"),
        py::arg("add_layers"), py::arg("nx"), py::arg("ny"), py::arg("spx"), py::arg("spy"));
    py_cls.def("add_via_on_intersections",
               [](c_cellview &cv, const cbag::layout::wire_array &warr1,
                  const cbag::layout::wire_array &warr2, bool extend, bool contain) {
                   auto tmp =
                       cbag::layout::add_via_on_intersections(cv, warr1, warr2, extend, contain);
                   return pyg::Tuple<tup_int, tup_int>::make_tuple(
                       tup_int::make_tuple(tmp[0][0], tmp[0][1]),
                       tup_int::make_tuple(tmp[1][0], tmp[1][1]));
               },
               "Add vias on the wire intersections.", py::arg("warr1"), py::arg("warr2"),
               py::arg("extend"), py::arg("contain"));
    py_cls.def("connect_barr_to_tracks",
               [](c_cellview &cv, cbag::enum_t lev_code, const std::string &layer,
                  const std::string &purpose, const pybag::util::box_arr &barr,
                  const cbag::layout::track_id &tid, std::optional<cbag::coord_t> tr_lower,
                  std::optional<cbag::coord_t> tr_upper, int min_len_code,
                  std::optional<cbag::coord_t> w_lower, std::optional<cbag::coord_t> w_upper) {
                   auto vdir = static_cast<cbag::direction>(lev_code);
                   auto mode = static_cast<cbag::min_len_mode>(min_len_code);
                   auto key = cbag::layout::layer_t_at(*(cv.get_tech()), layer, purpose);
                   auto tmp = cbag::layout::connect_box_track(
                       cv, vdir, key, barr.base, barr.get_num(), barr.get_sp(), tid,
                       {w_lower, w_upper}, {tr_lower, tr_upper}, mode);
                   return pyg::Tuple<tup_int, tup_int>::make_tuple(
                       tup_int::make_tuple(tmp[0][0], tmp[0][1]),
                       tup_int::make_tuple(tmp[1][0], tmp[1][1]));
               },
               "Connect the given BBoxArray to tracks.", py::arg("lev_code"), py::arg("layer"),
               py::arg("purpose"), py::arg("barr"), py::arg("tid"), py::arg("tr_lower"),
               py::arg("tr_upper"), py::arg("min_len_code"), py::arg("w_lower"),
               py::arg("w_upper"));
    py_cls.def("connect_warr_to_tracks",
               [](c_cellview &cv, const cbag::layout::wire_array &warr,
                  const cbag::layout::track_id &tid, std::optional<cbag::coord_t> w_lower,
                  std::optional<cbag::coord_t> w_upper, std::optional<cbag::coord_t> t_lower,
                  std::optional<cbag::coord_t> t_upper) {
                   auto tmp = cbag::layout::connect_warr_track(cv, warr, tid, {w_lower, w_upper},
                                                               {t_lower, t_upper});
                   return pyg::Tuple<tup_int, tup_int>::make_tuple(
                       tup_int::make_tuple(tmp[0][0], tmp[0][1]),
                       tup_int::make_tuple(tmp[1][0], tmp[1][1]));
               },
               "Connect the given WireArray to tracks.", py::arg("warr"), py::arg("tid"),
               py::arg("w_lower"), py::arg("w_upper"), py::arg("t_lower"), py::arg("t_upper"));
}

void bind_layout(py::module &m) {
    bind_inst_ref(m);
    bind_cellview(m);

    m.attr("COORD_MIN") = std::numeric_limits<cbag::coord_t>::min();
    m.attr("COORD_MAX") = std::numeric_limits<cbag::coord_t>::max();
}
