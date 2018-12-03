#include <memory>
#include <utility>

#include <pybind11/stl.h>

#include <cbag/common/orientation.h>
#include <cbag/common/transformation.h>

#include <pybind11_generics/tuple.h>

#include <pybag/util/bbox.h>
#include <pybag/util/bbox_array.h>
#include <pybag/util/bbox_collection.h>
#include <pybag/util/orient_conv.h>

namespace pyg = pybind11_generics;

namespace pybag {
namespace util {

std::unique_ptr<c_box> bbox_init(coord_t xl, coord_t yl, coord_t xh, coord_t yh, py::args args,
                                 py::kwargs kwargs) {
    return std::make_unique<c_box>(xl, yl, xh, yh);
}

c_box extend(const c_box &self, const std::optional<coord_t> &x, const std::optional<coord_t> &y,
             bool unit_mode) {
    if (!unit_mode)
        throw std::invalid_argument("unit_mode = False is not supported.");
    return self.get_extend(x, y);
}

c_box expand(const c_box &self, offset_t dx, offset_t dy, bool unit_mode) {
    if (!unit_mode)
        throw std::invalid_argument("unit_mode = False is not supported.");
    return self.get_expand(dx, dy);
}

c_box transform(const c_box &self, offset_t dx, offset_t dy, uint32_t ocode) {
    return self.get_transform(cbag::transformation(dx, dy, ocode));
}

c_box transform_compat(const c_box &self, pyg::Tuple<offset_t, offset_t> loc, py::str orient,
                       bool unit_mode) {
    if (!unit_mode)
        throw std::invalid_argument("unit_mode = False is not supported.");

    return transform(self, loc.get<0>(), loc.get<1>(), get_orient_code(orient));
}

c_box move_by(const c_box &self, offset_t dx, offset_t dy, bool unit_mode) {
    if (!unit_mode)
        throw std::invalid_argument("unit_mode = False is not supported.");
    return self.get_move_by(dx, dy);
}

pyg::Tuple<coord_t, coord_t, coord_t, coord_t> immutable_key(const c_box &self) {
    return pyg::Tuple<coord_t, coord_t, coord_t, coord_t>::make_tuple(self.xl(), self.yl(),
                                                                      self.xh(), self.yh());
}

c_box_arr as_bbox_array(const c_box &self) { return {self, 1, 1, 0, 0}; }

c_box_col as_bbox_collection(const c_box &self) {
    return {std::vector<c_box_arr>{as_bbox_array(self)}};
}

} // namespace util
} // namespace pybag

namespace pu = pybag::util;

void bind_bbox(py::class_<c_box> &py_cls) {
    py_cls.doc() = "The bounding box class.";
    py_cls.def(py::init(&pu::bbox_init), "Construct a new BBox.", py::arg("xl"), py::arg("yl"),
               py::arg("xh"), py::arg("yh"));

    py_cls.def("__repr__", &c_box::to_string, "Returns a string representation of BBox.");
    py_cls.def("__eq__", &c_box::operator==, "Returns True if the two BBox are equal.",
               py::arg("other"));

    py_cls.def_static("get_invalid_bbox", &c_box::invalid_box, "Create an invalid BBox.");
    py_cls.def_property_readonly("xl", &c_box::xl, "Left coordinate.");
    py_cls.def_property_readonly("left_unit", &c_box::xl, "Left coordinate.");
    py_cls.def_property_readonly("yl", &c_box::yl, "Bottom coordinate.");
    py_cls.def_property_readonly("bottom_unit", &c_box::yl, "Bottom coordinate.");
    py_cls.def_property_readonly("xh", &c_box::xh, "Right coordinate.");
    py_cls.def_property_readonly("right_unit", &c_box::xh, "Right coordinate.");
    py_cls.def_property_readonly("yh", &c_box::yh, "Top coordinate.");
    py_cls.def_property_readonly("top_unit", &c_box::yh, "Top coordinate.");
    py_cls.def_property_readonly("xm", &c_box::xm, "Center X coordinate.");
    py_cls.def_property_readonly("xc_unit", &c_box::xm, "Center X coordinate.");
    py_cls.def_property_readonly("ym", &c_box::ym, "Center Y coordinate.");
    py_cls.def_property_readonly("yc_unit", &c_box::ym, "Center Y coordinate.");
    py_cls.def_property_readonly("w", &c_box::w, "Width.");
    py_cls.def_property_readonly("width_unit", &c_box::w, "Width.");
    py_cls.def_property_readonly("h", &c_box::h, "Height.");
    py_cls.def_property_readonly("height_unit", &c_box::h, "Height.");

    py_cls.def("is_physical", &c_box::is_physical, "True if this BBox has positive area.");
    py_cls.def("is_valid", &c_box::is_valid, "True if this BBox has nonnegative area.");
    py_cls.def("overlaps", &c_box::overlaps, "True if the two BBox overlaps.", py::arg("bbox"));

    py_cls.def("merge", &c_box::get_merge, "Returns a new merged BBox.", py::arg("bbox"));
    py_cls.def("intersect", &c_box::get_intersect, "Returns a new intersection BBox.",
               py::arg("bbox"));
    py_cls.def("extend", &pu::extend, "Returns an extended BBox to the given coordinates.",
               py::arg("x") = py::none(), py::arg("y") = py::none(), py::arg("unit_mode") = true);
    py_cls.def("expand", &pu::expand, "Returns an expanded BBox (on all sides).", py::arg("dx") = 0,
               py::arg("dy") = 0, py::arg("unit_mode") = true);
    py_cls.def("transform", &pu::transform, "Returns a transformed BBox.", py::arg("dx") = 0,
               py::arg("dy") = 0, py::arg("ocode") = code_R0);
    py_cls.def("transform", &pu::transform_compat, "Returns a transformed BBox.",
               py::arg("loc") = std::pair<offset_t, offset_t>(0, 0), py::arg("orient") = "R0",
               py::arg("unit_mode") = true);
    py_cls.def("move_by", &pu::move_by, "Returns a new moved BBox.", py::arg("dx") = 0,
               py::arg("dy") = 0, py::arg("unit_mode") = true);
    py_cls.def("flip_xy", &c_box::get_flip_xy,
               "Returns a new BBox with flipped X and Y coordinates.");
    py_cls.def("get_immutable_key", &pu::immutable_key, "Returns a tuple representing this box.");

    py_cls.def("as_bbox_array", &pu::as_bbox_array,
               "Returns a BBoxArray representation of this BBox.");
    py_cls.def("as_bbox_collection", &pu::as_bbox_collection,
               "Returns a BBoxCollection representation of this BBox.");
}
