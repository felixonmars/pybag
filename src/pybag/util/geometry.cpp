#include <memory>
#include <utility>

#include <pybind11/pybind11.h>

#include <cbag/common/box_t.h>

#include <pybind11_generics/iterator.h>
#include <pybind11_generics/optional.h>
#include <pybind11_generics/tuple.h>

namespace py = pybind11;
namespace pyg = pybind11_generics;

namespace cbag {

class box_array_t {};

} // namespace cbag

using coord_t = cbag::coord_t;
using c_box = cbag::box_t;

namespace pybag {
namespace util {

std::unique_ptr<c_box> bbox_init(coord_t xl, coord_t yl, coord_t xh, coord_t yh, py::args args,
                                 py::kwargs kwargs) {
    return std::make_unique<c_box>(xl, yl, xh, yh);
}

std::unique_ptr<c_box> invalid_bbox() { return std::make_unique<c_box>(0, 0, -1, -1); }

} // namespace util
} // namespace pybag

namespace pu = pybag::util;

PYBIND11_MODULE(geometry, m) {
    m.doc() = "This module contains various geometry utility classes.";

    auto py_box = py::class_<c_box>(m, "BBox");
    py_box.doc() = "The bounding box class.";
    py_box.def(py::init(&pu::bbox_init), "Construct a new BBox.", py::arg("xl"), py::arg("yl"),
               py::arg("xh"), py::arg("yh"));
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
}
