#include <pybind11/pybind11.h>

#include <cbag/common/transformation.h>

#include <pybag/transform.h>

namespace py = pybind11;

using c_transform = cbag::transformation;

void bind_transform(py::module &m) {
    auto py_cls = py::class_<c_transform>(m, "Transform");
    py_cls.doc() = "A class that represents instance transformation.";
    py_cls.def(py::init<std::string>(), "Create a new PyTech class from file.",
               py::arg("tech_fname"));
}
