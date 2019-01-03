#include <memory>
#include <utility>

#include <pybind11/stl.h>

#include <cbag/common/box_t_util.h>
#include <cbag/common/transformation.h>
#include <cbag/enum/orientation.h>

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
    return pyg::Tuple<coord_t, coord_t, coord_t, coord_t>::make_tuple(
        cbag::xl(self), cbag::yl(self), cbag::xh(self), cbag::yh(self));
}

c_box_arr as_bbox_array(const c_box &self) { return {self, 1, 1, 0, 0}; }

c_box_col as_bbox_collection(const c_box &self) {
    return {std::vector<c_box_arr>{as_bbox_array(self)}};
}

std::pair<coord_t, coord_t> get_interval(const c_box &self, cbag::orient_2d_t orient_code) {
    const auto &intv = self.intvs[orient_code];
    return {intv[0], intv[1]};
}

} // namespace util
} // namespace pybag

namespace pu = pybag::util;

void bind_bbox(py::class_<c_box> &py_cls) {
    py_cls.doc() = "The bounding box class.";
    py_cls.def(py::init<coord_t, coord_t, coord_t, coord_t>(), "Construct a new BBox.",
               py::arg("xl"), py::arg("yl"), py::arg("xh"), py::arg("yh"));
    py_cls.def(
        py::init([](cbag::orient_2d_t orient_code, coord_t tl, coord_t th, coord_t pl, coord_t ph) {
            return c_box{static_cast<cbag::orient_2d>(orient_code), tl, th, pl, ph};
        }),
        "Construct a new BBox from orientation.", py::arg("orient_code"), py::arg("tl"),
        py::arg("th"), py::arg("pl"), py::arg("ph"));

    py_cls.def("__repr__", &cbag::to_string, "Returns a string representation of BBox.");
    py_cls.def("__eq__", &c_box::operator==, "Returns True if the two BBox are equal.",
               py::arg("other"));

    py_cls.def_static("get_invalid_bbox", &c_box::get_invalid_box, "Create an invalid BBox.");
    py_cls.def_property_readonly("xl", &cbag::xl, "Left coordinate.");

    py_cls.def_property_readonly("yl", &cbag::yl, "Bottom coordinate.");
    py_cls.def_property_readonly("xh", &cbag::xh, "Right coordinate.");
    py_cls.def_property_readonly("yh", &cbag::yh, "Top coordinate.");
    py_cls.def_property_readonly("xm", &cbag::xm, "Center X coordinate.");
    py_cls.def_property_readonly("ym", &cbag::ym, "Center Y coordinate.");
    py_cls.def_property_readonly("w", &cbag::width, "Width.");
    py_cls.def_property_readonly("h", &cbag::height, "Height.");

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

    py_cls.def("get_dim",
               [](const c_box &self, cbag::orient_2d_t orient_code) {
                   return cbag::get_dim(self, static_cast<cbag::orient_2d>(orient_code));
               },
               "Returns the dimension along the given orientation.", py::arg("orient_code"));
    py_cls.def("get_coord",
               [](const c_box &self, cbag::orient_2d_t orient_code, bool bnd_code) {
                   return self.intvs[orient_code][bnd_code];
               },
               "Returns coordinate given orient/bound code.", py::arg("orient_code"),
               py::arg("bnd_code"));
    py_cls.def("get_center",
               [](const c_box &self, cbag::orient_2d_t orient_code) {
                   return cbag::get_center(self, static_cast<cbag::orient_2d>(orient_code));
               },
               "Returns center coordinate given orient code.", py::arg("orient_code"));
    py_cls.def("get_interval", &pu::get_interval, "Returns interval given orient code.",
               py::arg("orient_code"));

    py_cls.def("is_physical", &cbag::is_physical, "True if this BBox has positive area.");
    py_cls.def("is_valid", &cbag::is_valid, "True if this BBox has nonnegative area.");
    py_cls.def("overlaps", &cbag::overlaps, "True if the two BBox overlaps.", py::arg("bbox"));

    py_cls.def("merge", &cbag::merge, "Set to union of BBox", py::arg("bbox"));
    py_cls.def("get_merge", &cbag::get_merge, "Get union of BBox", py::arg("bbox"));
    py_cls.def("intersect", &cbag::intersect, "Set to intersection of BBox", py::arg("bbox"));
    py_cls.def("get_intersect", &cbag::get_intersect, "Get intersection of BBox", py::arg("bbox"));
    py_cls.def("extend", &cbag::extend, "Extend BBox to the given coordinates.",
               py::arg("x") = py::none(), py::arg("y") = py::none());
    py_cls.def("get_extend", &cbag::get_extend,
               "Returns an extended BBox to the given coordinates.", py::arg("x") = py::none(),
               py::arg("y") = py::none());
    py_cls.def("extend_orient",
               [](c_box &self, cbag::orient_2d_t orient_code, const std::optional<coord_t> &ct,
                  const std::optional<coord_t> &cp) {
                   return cbag::extend_orient(self, static_cast<cbag::orient_2d>(orient_code), ct,
                                              cp);
               },
               "Extends this BBox.", py::arg("orient_code"), py::arg("ct") = py::none(),
               py::arg("cp") = py::none());
    py_cls.def("get_extend_orient",
               [](const c_box &self, cbag::orient_2d_t orient_code,
                  const std::optional<coord_t> &ct, const std::optional<coord_t> &cp) {
                   return cbag::get_extend_orient(self, static_cast<cbag::orient_2d>(orient_code),
                                                  ct, cp);
               },
               "Returns an extended BBox.", py::arg("orient_code"), py::arg("ct") = py::none(),
               py::arg("cp") = py::none());
    py_cls.def("expand", &cbag::expand, "Expand BBox (on all sides).", py::arg("dx") = 0,
               py::arg("dy") = 0);
    py_cls.def("get_expand", &cbag::get_expand, "Returns an expanded BBox (on all sides).",
               py::arg("dx") = 0, py::arg("dy") = 0);
    py_cls.def("transform", &cbag::transform, "Transforms the BBox.", py::arg("xform"));
    py_cls.def("get_transform", &cbag::get_transform, "Returns a transformed BBox.",
               py::arg("xform"));
    py_cls.def("move_by", &cbag::move_by, "Moves the BBox.", py::arg("dx") = 0, py::arg("dy") = 0);
    py_cls.def("get_move_by", &cbag::get_move_by, "Returns a moved BBox.", py::arg("dx") = 0,
               py::arg("dy") = 0);
    py_cls.def("move_by_orient",
               [](c_box &self, cbag::orient_2d_t orient_code, offset_t dt, offset_t dp) {
                   return cbag::move_by_orient(self, static_cast<cbag::orient_2d>(orient_code), dt,
                                               dp);
               },
               "Moves the BBox.", py::arg("orient_code"), py::arg("dt") = 0, py::arg("dp") = 0);
    py_cls.def("get_move_by_orient",
               [](const c_box &self, cbag::orient_2d_t orient_code, offset_t dt, offset_t dp) {
                   return cbag::get_move_by_orient(self, static_cast<cbag::orient_2d>(orient_code),
                                                   dt, dp);
               },
               "Returns a moved BBox.", py::arg("orient_code"), py::arg("dt") = 0,
               py::arg("dp") = 0);
    py_cls.def("flip_xy", &cbag::flip_xy, "Flips the BBox X and Y coordinates.");
    py_cls.def("get_flip_xy", &cbag::get_flip_xy,
               "Returns a flipped the BBox X and Y coordinates.");

    py_cls.def("get_immutable_key", &pu::get_immutable_key,
               "Returns a tuple representing this box.");

    py_cls.def("as_bbox_array", &pu::as_bbox_array,
               "Returns a BBoxArray representation of this BBox.");
    py_cls.def("as_bbox_collection", &pu::as_bbox_collection,
               "Returns a BBoxCollection representation of this BBox.");
}
