#include <pybind11/pybind11.h>

#include <cbag/layout/tech.h>

namespace py = pybind11;

using c_tech = cbag::layout::tech;

PYBIND11_MODULE(tech, m) {
    m.doc() = "This module defines the layout technology information class.";

    // add interval class
    auto py_cls = py::class_<c_tech>(m, "PyTech");
    py_cls.doc() = "A class that handles technology-specific information.";
    py_cls.def(py::init<std::string>(), "Create a new PyTech class from file.",
               py::arg("tech_fname"));

    py_cls.def_property_readonly("pin_purpose", &c_tech::get_pin_purpose_name,
                                 "The pin purpose name.");
    py_cls.def_property_readonly("default_purpose", &c_tech::get_default_purpose_name,
                                 "The default purpose name.");
    py_cls.def("get_min_space",
               py::overload_cast<const std::string &, cbag::offset_t, uint32_t>(
                   &c_tech::get_min_space, py::const_),
               "Returns the minimum required spacing.", py::arg("layer_type"), py::arg("width"),
               py::arg("sp_type"));
}
