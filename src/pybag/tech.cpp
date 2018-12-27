#include <pybind11/pybind11.h>

#include <cbag/layout/tech_util.h>

#include <pybag/tech.h>

namespace py = pybind11;

using c_tech = cbag::layout::tech;

void bind_tech(py::module &m) {
    auto py_cls = py::class_<c_tech>(m, "PyTech");
    py_cls.doc() = "A class that handles technology-specific information.";
    py_cls.def(py::init<std::string>(), "Create a new PyTech class from file.",
               py::arg("tech_fname"));

    py_cls.def_property_readonly("pin_purpose", &cbag::layout::get_pin_purpose_name,
                                 "The pin purpose name.");
    py_cls.def_property_readonly("default_purpose", &cbag::layout::get_default_purpose_name,
                                 "The default purpose name.");
    py_cls.def("get_min_space", cbag::layout::get_min_space,
               "Returns the minimum required spacing.", py::arg("layer_type"), py::arg("width"),
               py::arg("sp_type"));
}
