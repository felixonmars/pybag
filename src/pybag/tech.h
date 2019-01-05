#ifndef PYBAG_TECH_H
#define PYBAG_TECH_H

#include <pybind11/pybind11.h>

namespace py = pybind11;

void bind_tech(py::module &);

void bind_routing_grid(py::module &);

#endif
