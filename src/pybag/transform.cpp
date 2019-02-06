#include <pybind11/pybind11.h>

#include <pybind11_generics/tuple.h>
#include <pybind11_generics/type_name.h>

#include <cbag/common/transformation_util.h>

#include <pybag/orient_conv.h>
#include <pybag/transform.h>
#include <pybag/typedefs.h>

namespace py = pybind11;
namespace pyg = pybind11_generics;

// custom orientation typing definition

namespace pybind11_generics {

using obj_base = py::object;

class PyOrient : public obj_base {
  public:
    static bool true_check(PyObject *ptr) { return true; }

    PYBIND11_OBJECT_DEFAULT(PyOrient, obj_base, true_check);
};

} // namespace pybind11_generics

namespace pybind11 {
namespace detail {

template <> struct handle_type_name<pybind11_generics::PyOrient> {
    static constexpr auto name = _("pybag.enum.Orientation");
};

} // namespace detail
} // namespace pybind11

using c_transform = cbag::transformation;

namespace pybag {
namespace xform {

pybind11_generics::PyOrient get_xform_orient(const c_transform &xform) {
    return pybag::util::code_to_orient(cbag::orient_code(xform));
}

c_transform make_xform(coord_t dx, coord_t dy, cbag::orient_t mode) {
    return cbag::make_xform(dx, dy, static_cast<cbag::orientation>(mode));
}

pyg::Tuple<int, int> axis_scale(const c_transform &xform) {
    auto tmp = cbag::axis_scale(xform);
    return pyg::Tuple<int, int>::make_tuple(tmp[0], tmp[1]);
}

pyg::Tuple<int, int> location(const c_transform &xform) {
    auto tmp = cbag::location(xform);
    return pyg::Tuple<int, int>::make_tuple(tmp[0], tmp[1]);
}

} // namespace xform
} // namespace pybag

void bind_transform(py::module &m) {
    auto py_cls = py::class_<c_transform>(m, "Transform");
    py_cls.doc() = "A class that represents instance transformation.";
    py_cls.def(py::init(&pybag::xform::make_xform), "Create a new transformation object.",
               py::arg("dx") = 0, py::arg("dy") = 0, py::arg("mode") = code_R0);
    py_cls.def("__repr__", py::overload_cast<const c_transform &>(&cbag::to_string),
               "Returns a string representation of BBox.");
    py_cls.def_property_readonly("x", &cbag::x, "X shift.");
    py_cls.def_property_readonly("y", &cbag::y, "Y shift.");
    py_cls.def_property_readonly("orient", &pybag::xform::get_xform_orient, "Orientation.");
    py_cls.def_property_readonly("location", &pybag::xform::location, "Location.");
    py_cls.def_property_readonly("flips_xy", &cbag::flips_xy,
                                 "True if this transformation flips the axis");
    py_cls.def_property_readonly("axis_scale", &pybag::xform::axis_scale,
                                 "The transformation scale factor of each axis.");
    py_cls.def("move_by", &cbag::move_by, "Moves this transform object", py::arg("dx") = 0,
               py::arg("dy") = 0);
    py_cls.def("get_move_by", &cbag::get_move_by, "Get a shifted transform object",
               py::arg("dx") = 0, py::arg("dy") = 0);
    py_cls.def("transform_by", &cbag::transform_by, "Transforms this transform object",
               py::arg("xform"));
    py_cls.def("get_transform_by", &cbag::get_transform_by,
               "Returns a transformed transform object", py::arg("xform"));
    py_cls.def("get_transform", &cbag::get_transform_by, "Returns a transformed transform object",
               py::arg("xform"));
}
