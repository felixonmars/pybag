#include <pybind11/pybind11.h>

#include <cbag/layout/routing_grid.h>
#include <cbag/layout/tech_util.h>

#include <pybag/tech.h>

namespace py = pybind11;

using c_tech = cbag::layout::tech;
using c_grid = cbag::layout::routing_grid;

void bind_tech(py::module &m) {
    auto py_cls = py::class_<c_tech>(m, "PyTech");
    py_cls.doc() = "A class that handles technology-specific information.";
    py_cls.def(py::init<std::string>(), "Create a new PyTech class from file.",
               py::arg("tech_fname"));

    py_cls.def_property_readonly("tech_lib", &c_tech::get_tech_lib, "The technology library name.");
    py_cls.def_property_readonly("layout_unit", &c_tech::get_layout_unit, "The layout unit.");
    py_cls.def_property_readonly("resolution", &c_tech::get_resolution, "The layout resolution.");
    py_cls.def_property_readonly("use_flip_parity", &c_tech::get_use_flip_parity,
                                 "True if flip_parity dictionary is used.");
    py_cls.def_property_readonly("default_purpose", &cbag::layout::get_default_purpose_name,
                                 "The default purpose name.");
    py_cls.def_property_readonly("pin_purpose", &cbag::layout::get_pin_purpose_name,
                                 "The pin purpose name.");
    py_cls.def_property_readonly("make_pin", &c_tech::get_make_pin, "True to make pin objects.");

    py_cls.def("get_min_space", cbag::layout::get_min_space,
               "Returns the minimum required spacing.", py::arg("layer"), py::arg("purpose"),
               py::arg("width"), py::arg("sp_type"));
    py_cls.def("get_min_length", cbag::layout::get_min_length,
               "Returns the minimum required length.", py::arg("layer"), py::arg("purpose"),
               py::arg("width"), py::arg("even"));
}

void bind_routing_grid(py::module &m) {
    auto py_cls = py::class_<c_grid>(m, "PyRoutingGrid");
    py_cls.doc() = "The routing grid class.";
    py_cls.def(py::init<const c_tech *, std::string>(),
               "Create a new PyRoutingGrid class from file.", py::arg("tech"), py::arg("fname"));
}
