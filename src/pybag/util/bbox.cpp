#include <memory>
#include <utility>

#include <pybind11/stl.h>

#include <cbag/common/box_t.h>
#include <cbag/common/orientation.h>
#include <cbag/common/transformation.h>

#include <pybind11_generics/tuple.h>

#include <pybag/util/bbox.h>

namespace pyg = pybind11_generics;

using offset_t = cbag::offset_t;
using coord_t = cbag::coord_t;
using c_box = cbag::box_t;

constexpr auto code_R0 = static_cast<uint32_t>(cbag::oR0);

namespace pybag {
namespace util {

std::unique_ptr<c_box> bbox_init(coord_t xl, coord_t yl, coord_t xh, coord_t yh, py::args args,
                                 py::kwargs kwargs) {
    return std::make_unique<c_box>(xl, yl, xh, yh);
}

std::unique_ptr<c_box> invalid_bbox() { return std::make_unique<c_box>(0, 0, -1, -1); }

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

    // use the Python enum class to convert string to int
    py::module enum_module = py::module::import("pybag.enum");
    py::object orient_enum = enum_module.attr("Orientation");
    uint32_t code = orient_enum.attr("__getitem__")(orient).template cast<int>();
    return transform(self, loc.get<0>(), loc.get<1>(), code);
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

} // namespace util
} // namespace pybag

namespace pu = pybag::util;

void bind_bbox(py::module &m) {
    auto py_box = py::class_<c_box>(m, "BBox");
    py_box.doc() = "The bounding box class.";
    py_box.def(py::init(&pu::bbox_init), "Construct a new BBox.", py::arg("xl"), py::arg("yl"),
               py::arg("xh"), py::arg("yh"));

    py_box.def("__repr__", &c_box::to_string, "Returns a string representation of BBox.");
    py_box.def("__eq__", &c_box::operator==, "Returns True if the two BBox are equal.",
               py::arg("other"));

    py_box.def_static("get_invalid_bbox", &pu::invalid_bbox, "Create an invalid BBox.");
    py_box.def_property_readonly("xl", &c_box::xl, "Left coordinate.");
    py_box.def_property_readonly("left_unit", &c_box::xl, "Left coordinate.");
    py_box.def_property_readonly("yl", &c_box::yl, "Bottom coordinate.");
    py_box.def_property_readonly("bottom_unit", &c_box::yl, "Bottom coordinate.");
    py_box.def_property_readonly("xh", &c_box::xh, "Right coordinate.");
    py_box.def_property_readonly("right_unit", &c_box::xh, "Right coordinate.");
    py_box.def_property_readonly("yh", &c_box::yh, "Top coordinate.");
    py_box.def_property_readonly("top_unit", &c_box::yh, "Top coordinate.");
    py_box.def_property_readonly("xm", &c_box::xm, "Center X coordinate.");
    py_box.def_property_readonly("xc_unit", &c_box::xm, "Center X coordinate.");
    py_box.def_property_readonly("ym", &c_box::ym, "Center Y coordinate.");
    py_box.def_property_readonly("yc_unit", &c_box::ym, "Center Y coordinate.");
    py_box.def_property_readonly("w", &c_box::w, "Width.");
    py_box.def_property_readonly("width_unit", &c_box::w, "Width.");
    py_box.def_property_readonly("h", &c_box::h, "Height.");
    py_box.def_property_readonly("height_unit", &c_box::h, "Height.");

    py_box.def("is_physical", &c_box::is_physical, "True if this BBox has positive area.");
    py_box.def("is_valid", &c_box::is_valid, "True if this BBox has nonnegative area.");
    py_box.def("overlaps", &c_box::overlaps, "True if the two BBox overlaps.", py::arg("bbox"));

    py_box.def("merge", &c_box::get_merge, "Returns a new merged BBox.", py::arg("bbox"));
    py_box.def("intersect", &c_box::get_intersect, "Returns a new intersection BBox.",
               py::arg("bbox"));
    py_box.def("extend", &pu::extend, "Returns an extended BBox to the given coordinates.",
               py::arg("x") = py::none(), py::arg("y") = py::none(), py::arg("unit_mode") = true);
    py_box.def("expand", &pu::expand, "Returns an expanded BBox (on all sides).", py::arg("dx") = 0,
               py::arg("dy") = 0, py::arg("unit_mode") = true);
    py_box.def("transform", &pu::transform, "Returns a transformed BBox.", py::arg("dx") = 0,
               py::arg("dy") = 0, py::arg("ocode") = code_R0);
    py_box.def("transform", &pu::transform_compat, "Returns a transformed BBox.",
               py::arg("loc") = std::pair<offset_t, offset_t>(0, 0), py::arg("orient") = "R0",
               py::arg("unit_mode") = true);
    py_box.def("move_by", &pu::move_by, "Returns a new moved BBox.", py::arg("dx") = 0,
               py::arg("dy") = 0, py::arg("unit_mode") = true);
    py_box.def("flip_xy", &c_box::get_flip_xy,
               "Returns a new BBox with flipped X and Y coordinates.");
    py_box.def("get_immutable_key", &pu::immutable_key, "Returns a tuple representing this box.");
}
