#include <pybag/util/bbox.h>
#include <pybag/util/bbox_array.h>
#include <pybag/util/bbox_collection.h>

PYBIND11_MODULE(geometry, m) {
    m.doc() = "This module contains various geometry utility classes.";

    auto py_box = py::class_<c_box>(m, "BBox");
    auto py_barr = py::class_<c_box_arr>(m, "BBoxArray");
    auto py_bcol = py::class_<c_box_col>(m, "BBoxCollection");

    bind_bbox(py_box);
    bind_bbox_array(py_barr);
    bind_bbox_collection(py_bcol);
}
