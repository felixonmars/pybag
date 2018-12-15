#include <pybind11/pybind11.h>

#include <cbag/common/transformation.h>

#include <pybag/orient_conv.h>
#include <pybag/transform.h>
#include <pybag/typedefs.h>

namespace py = pybind11;

using c_transform = cbag::transformation;

py::object get_xform_orient(const c_transform &xform) {
    return pybag::util::code_to_orient(xform.orient_code());
}

void bind_transform(py::module &m) {
    auto py_cls = py::class_<c_transform>(m, "Transform");
    py_cls.doc() = "A class that represents instance transformation.";
    py_cls.def(py::init<coord_t, coord_t, uint32_t>(), "Create a new transformation object.",
               py::arg("dx") = 0, py::arg("dy") = 0, py::arg("mode") = code_R0);
    py_cls.def_property_readonly("x", &c_transform::x, "X shift.");
    py_cls.def_property_readonly("y", &c_transform::y, "Y shift.");
    py_cls.def_property_readonly("orient", &get_xform_orient, "Orientation.");
}
