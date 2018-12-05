#include <pybind11/pybind11.h>

#include <cbag/layout/instance.h>

#include <pybag/layout.h>

using c_instance = cbag::layout::instance;

void bind_layout(py::module &m) {

    auto py_inst = py::class_<c_instance>(m, "PyLayInstRef");
    py_inst.doc() = "A reference to a layout instance inside a cellview.";
}
