#include <memory>
#include <utility>

#include <pybind11/stl.h>

#include <cbag/common/orientation.h>
#include <cbag/common/transformation.h>

#include <pybind11_generics/custom.h>
#include <pybind11_generics/tuple.h>

#include <pybag/bbox.h>
#include <pybag/bbox_array.h>
#include <pybag/bbox_collection.h>
#include <pybag/orient_conv.h>

namespace pyg = pybind11_generics;

namespace pybag {
namespace util {

pyg::Tuple<coord_t, coord_t, coord_t, coord_t> get_immutable_key(const c_box &self) {
    return pyg::Tuple<coord_t, coord_t, coord_t, coord_t>::make_tuple(self.xl(), self.yl(),
                                                                      self.xh(), self.yh());
}

c_box_arr as_bbox_array(const c_box &self) { return {self, 1, 1, 0, 0}; }

c_box_col as_bbox_collection(const c_box &self) {
    return {std::vector<c_box_arr>{as_bbox_array(self)}};
}

std::pair<coord_t, coord_t> get_interval(const c_box &self, uint8_t orient_code) {
    auto &intv = self.get_interval(orient_code);
    return {intv[0], intv[1]};
}

} // namespace util
} // namespace pybag

namespace pu = pybag::util;

void bind_bbox(py::class_<c_box> &py_cls) {
    py_cls.doc() = "The bounding box class.";
    py_cls.def(py::init<coord_t, coord_t, coord_t, coord_t>(), "Construct a new BBox.",
               py::arg("xl"), py::arg("yl"), py::arg("xh"), py::arg("yh"));
    py_cls.def(py::init<uint8_t, coord_t, coord_t, coord_t, coord_t>(),
               "Construct a new BBox from orientation.", py::arg("orient_code"), py::arg("tl"),
               py::arg("th"), py::arg("pl"), py::arg("ph"));

    py_cls.def("__repr__", &c_box::to_string, "Returns a string representation of BBox.");
    py_cls.def("__eq__", &c_box::operator==, "Returns True if the two BBox are equal.",
               py::arg("other"));

    py_cls.def_static("get_invalid_bbox", &c_box::invalid_box, "Create an invalid BBox.");
    py_cls.def_property_readonly("xl", &c_box::xl, "Left coordinate.");

    py_cls.def_property_readonly("yl", &c_box::yl, "Bottom coordinate.");
    py_cls.def_property_readonly("xh", &c_box::xh, "Right coordinate.");
    py_cls.def_property_readonly("yh", &c_box::yh, "Top coordinate.");
    py_cls.def_property_readonly("xm", &c_box::xm, "Center X coordinate.");
    py_cls.def_property_readonly("ym", &c_box::ym, "Center Y coordinate.");
    py_cls.def_property_readonly("w", &c_box::w, "Width.");
    py_cls.def_property_readonly("h", &c_box::h, "Height.");

    /*
    py_cls.def_property_readonly("left_unit", &c_box::xl, "Left coordinate.");
    py_cls.def_property_readonly("bottom_unit", &c_box::yl, "Bottom coordinate.");
    py_cls.def_property_readonly("right_unit", &c_box::xh, "Right coordinate.");
    py_cls.def_property_readonly("top_unit", &c_box::yh, "Top coordinate.");
    py_cls.def_property_readonly("xc_unit", &c_box::xm, "Center X coordinate.");
    py_cls.def_property_readonly("yc_unit", &c_box::ym, "Center Y coordinate.");
    py_cls.def_property_readonly("width_unit", &c_box::w, "Width.");
    py_cls.def_property_readonly("height_unit", &c_box::h, "Height.");
    */

    py_cls.def("get_dim", &c_box::get_dim, "Returns the dimension along the given orientation.",
               py::arg("orient_code"));
    py_cls.def("get_coord", &c_box::get_coord, "Returns coordinate given orient/bound code.",
               py::arg("orient_code"), py::arg("bnd_code"));
    py_cls.def("get_interval", &pu::get_interval, "Returns interval given orient code.",
               py::arg("orient_code"));

    py_cls.def("is_physical", &c_box::is_physical, "True if this BBox has positive area.");
    py_cls.def("is_valid", &c_box::is_valid, "True if this BBox has nonnegative area.");
    py_cls.def("overlaps", &c_box::overlaps, "True if the two BBox overlaps.", py::arg("bbox"));

    py_cls.def("merge", &c_box::merge, "Set to union of BBox", py::arg("bbox"));
    py_cls.def("get_merge", &c_box::get_merge, "Get union of BBox", py::arg("bbox"));
    py_cls.def("intersect", &c_box::intersect, "Set to intersection of BBox", py::arg("bbox"));
    py_cls.def("get_intersect", &c_box::get_intersect, "Get intersection of BBox", py::arg("bbox"));
    py_cls.def("extend", &c_box::extend, "Extend BBox to the given coordinates.",
               py::arg("x") = py::none(), py::arg("y") = py::none());
    py_cls.def("get_extend", &c_box::get_extend,
               "Returns an extended BBox to the given coordinates.", py::arg("x") = py::none(),
               py::arg("y") = py::none());
    py_cls.def("expand", &c_box::expand, "Expand BBox (on all sides).", py::arg("dx") = 0,
               py::arg("dy") = 0);
    py_cls.def("get_expand", &c_box::get_expand, "Returns an expanded BBox (on all sides).",
               py::arg("dx") = 0, py::arg("dy") = 0);
    py_cls.def("transform", &c_box::transform, "Transforms the BBox.", py::arg("xform"));
    py_cls.def("get_transform", &c_box::get_transform, "Returns a transformed BBox.",
               py::arg("xform"));
    py_cls.def("move_by", &c_box::move_by, "Moves the BBox.", py::arg("dx") = 0, py::arg("dy") = 0);
    py_cls.def("get_move_by", &c_box::get_move_by, "Returns a moved BBox.", py::arg("dx") = 0,
               py::arg("dy") = 0);
    py_cls.def("move_by_orient", &c_box::move_by_orient, "Moves the BBox.", py::arg("orient_code"),
               py::arg("dt") = 0, py::arg("dp") = 0);
    py_cls.def("get_move_by_orient", &c_box::get_move_by_orient, "Returns a moved BBox.",
               py::arg("orient_code"), py::arg("dt") = 0, py::arg("dp") = 0);
    py_cls.def("flip_xy", &c_box::flip_xy, "Flips the BBox X and Y coordinates.");
    py_cls.def("get_flip_xy", &c_box::get_flip_xy,
               "Returns a flipped the BBox X and Y coordinates.");

    py_cls.def("get_immutable_key", &pu::get_immutable_key,
               "Returns a tuple representing this box.");

    py_cls.def("as_bbox_array", &pu::as_bbox_array,
               "Returns a BBoxArray representation of this BBox.");
    py_cls.def("as_bbox_collection", &pu::as_bbox_collection,
               "Returns a BBoxCollection representation of this BBox.");
}
