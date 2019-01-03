#include <pybind11/pybind11.h>

#include <cbag/common/transformation_util.h>
#include <cbag/layout/cellview_poly.h>
#include <cbag/layout/cellview_util.h>
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

cbag::cnt_t get_nx(const c_inst_ref &ref) { return ref.obj.nx; }
cbag::cnt_t get_ny(const c_inst_ref &ref) { return ref.obj.ny; }
cbag::offset_t get_spx(const c_inst_ref &ref) { return ref.obj.spx; }
cbag::offset_t get_spy(const c_inst_ref &ref) { return ref.obj.spy; }
const cbag::transformation &get_xform(const c_inst_ref &ref) { return ref.obj.xform; }

void set_nx(c_inst_ref &ref, cbag::int_t val) {
    check_ref(ref);
    if (val < 0)
        throw std::runtime_error("Cannot set nx to be negative.");
    ref.obj.nx = val;
}
void set_ny(c_inst_ref &ref, cbag::int_t val) {
    check_ref(ref);
    if (val < 0)
        throw std::runtime_error("Cannot set nx to be negative.");
    ref.obj.ny = val;
}
void set_spx(c_inst_ref &ref, cbag::offset_t val) {
    check_ref(ref);
    ref.obj.spx = val;
}
void set_spy(c_inst_ref &ref, cbag::offset_t val) {
    check_ref(ref);
    ref.obj.spy = val;
}

void move_by(c_inst_ref &ref, cbag::offset_t dx, cbag::offset_t dy) {
    check_ref(ref);
    cbag::move_by(ref.obj.xform, dx, dy);
}

void transform(c_inst_ref &ref, const cbag::transformation &xform) {
    check_ref(ref);
    cbag::transform_by(ref.obj.xform, xform);
}

void set_master(c_inst_ref &ref, const cbag::layout::cellview *new_master) {
    check_ref(ref);
    ref.obj.set_master(new_master);
}

void add_rect_arr(c_cellview &self, const std::string &layer, const std::string &purpose,
                  bool is_horiz, const util::box_arr &barr) {
    cbag::layout::add_rect_arr(self, layer, purpose, barr.base, is_horiz, barr.nx(), barr.ny(),
                               barr.spx(), barr.spy());
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

    auto py_cls = py::class_<c_cellview>(m, "PyLayCellView");
    py_cls.doc() = "A layout cellview.";

    py_cls.def(
        py::init([](cbag::layout::tech *tech_ptr, std::string cell_name, cbag::enum_t geo_mode) {
            return c_cellview{tech_ptr, std::move(cell_name),
                              static_cast<cbag::geometry_mode>(geo_mode)};
        }),
        "Construct a new cellview.", py::arg("tech"), py::arg("cell_name"),
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
               py::arg("purpose"), py::arg("is_horiz"), py::arg("bbox"), py::arg("commit"));
    py_cls.def("add_rect_arr", &cbag::layout::add_rect_arr, "Adds an array of rectangles.",
               py::arg("layer"), py::arg("purpose"), py::arg("box"), py::arg("is_horiz"),
               py::arg("nx"), py::arg("ny"), py::arg("spx"), py::arg("spy"));
    py_cls.def("add_rect_arr", &pl::add_rect_arr, "Adds an array of rectangles.", py::arg("layer"),
               py::arg("purpose"), py::arg("is_horiz"), py::arg("barr"));
    py_cls.def("add_poly", &cbag::layout::add_poly<py_pt_vector>, "Adds a new polygon.",
               py::arg("layer"), py::arg("purpose"), py::arg("is_horiz"), py::arg("points"),
               py::arg("commit"));
    py_cls.def("add_blockage", &cbag::layout::add_blockage<py_pt_vector>, "Adds a blockage object.",
               py::arg("layer"), py::arg("blk_code"), py::arg("points"), py::arg("commit"));
    py_cls.def("add_boundary", &cbag::layout::add_boundary<py_pt_vector>, "Adds a boundary object.",
               py::arg("bnd_code"), py::arg("points"), py::arg("commit"));
    py_cls.def("add_pin", &c_cellview::add_pin, "Adds a pin object.", py::arg("layer"),
               py::arg("net"), py::arg("label"), py::arg("bbox"));
    py_cls.def("add_label", &c_cellview::add_label, "Adds a label object.", py::arg("layer"),
               py::arg("purpose"), py::arg("xform"), py::arg("label"));
    py_cls.def("add_path", &cbag::layout::add_path<py_pt_vector>, "Adds a new path.",
               py::arg("layer"), py::arg("purpose"), py::arg("is_horiz"), py::arg("points"),
               py::arg("half_width"), py::arg("style0"), py::arg("style1"), py::arg("stylem"),
               py::arg("commit"));
    py_cls.def("add_path45_bus",
               &cbag::layout::add_path45_bus<py_pt_vector, pyg::List<cbag::int_t>>,
               "Adds a new 45 degree path bus.", py::arg("layer"), py::arg("purpose"),
               py::arg("is_horiz"), py::arg("points"), py::arg("widths"), py::arg("spaces"),
               py::arg("style0"), py::arg("style1"), py::arg("stylem"), py::arg("commit"));
    py_cls.def("add_via", &cbag::layout::add_via, "Add a via.", py::arg("xform"), py::arg("via_id"),
               py::arg("add_layers"), py::arg("bot_horiz"), py::arg("top_horiz"), py::arg("vnx"),
               py::arg("vny"), py::arg("w"), py::arg("h"), py::arg("vspx"), py::arg("vspy"),
               py::arg("enc1l"), py::arg("enc1r"), py::arg("enc1t"), py::arg("enc1b"),
               py::arg("enc2l"), py::arg("enc2r"), py::arg("enc2t"), py::arg("enc2b"),
               py::arg("commit"));
    py_cls.def("add_via_arr", &cbag::layout::add_via_arr, "Add an array of vias.", py::arg("xform"),
               py::arg("via_id"), py::arg("add_layers"), py::arg("bot_horiz"), py::arg("top_horiz"),
               py::arg("vnx"), py::arg("vny"), py::arg("w"), py::arg("h"), py::arg("vspx"),
               py::arg("vspy"), py::arg("enc1l"), py::arg("enc1r"), py::arg("enc1t"),
               py::arg("enc1b"), py::arg("enc2l"), py::arg("enc2r"), py::arg("enc2t"),
               py::arg("enc2b"), py::arg("nx"), py::arg("ny"), py::arg("spx"), py::arg("spy"));
}

void bind_layout(py::module &m) {
    bind_inst_ref(m);
    bind_cellview(m);
}
