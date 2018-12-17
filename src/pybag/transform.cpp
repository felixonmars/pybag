#include <pybind11/pybind11.h>

#include <pybind11_generics/type_name.h>

#include <cbag/common/transformation.h>

#include <pybag/orient_conv.h>
#include <pybag/transform.h>
#include <pybag/typedefs.h>

namespace py = pybind11;

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
    static constexpr auto name = _("Orientation");
};

} // namespace detail
} // namespace pybind11

using c_transform = cbag::transformation;

pybind11_generics::PyOrient get_xform_orient(const c_transform &xform) {
    return pybag::util::code_to_orient(xform.orient_code());
}

std::pair<coord_t, coord_t> get_xform_location(const c_transform &xform) {
    coord_t x = 0, y = 0;
    xform.get_location(x, y);
    return std::make_pair(x, y);
}

void bind_transform(py::module &m) {
    auto py_cls = py::class_<c_transform>(m, "Transform");
    py_cls.doc() = "A class that represents instance transformation.";
    py_cls.def(py::init<coord_t, coord_t, uint32_t>(), "Create a new transformation object.",
               py::arg("dx") = 0, py::arg("dy") = 0, py::arg("mode") = code_R0);
    py_cls.def_property_readonly("x", &c_transform::x, "X shift.");
    py_cls.def_property_readonly("y", &c_transform::y, "Y shift.");
    py_cls.def_property_readonly("orient", &get_xform_orient, "Orientation.");
    py_cls.def_property_readonly("location", &get_xform_location, "Location.");
    py_cls.def("move_by", &c_transform::move_by, "Moves this transform object", py::arg("dx") = 0,
               py::arg("dy") = 0);
    py_cls.def("get_move_by", &c_transform::get_move_by, "Get a shifted transform object",
               py::arg("dx") = 0, py::arg("dy") = 0);
    py_cls.def("transform_by", &c_transform::transform_by, "Transforms this transform object",
               py::arg("xform"));
    py_cls.def("get_transform_by", &c_transform::get_transform_by,
               "Returns a transformed transform object", py::arg("xform"));
}
