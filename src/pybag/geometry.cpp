#include <pybag/bbox.h>
#include <pybag/bbox_array.h>
#include <pybag/bbox_collection.h>
#include <pybag/geometry.h>

void bind_geometry(py::module &m) {
    auto py_box = py::class_<c_box>(m, "BBox");
    auto py_barr = py::class_<c_box_arr>(m, "BBoxArray");
    auto py_bcol = py::class_<c_box_col>(m, "BBoxCollection");

    bind_bbox(py_box);
    bind_bbox_array(py_barr);
    bind_bbox_collection(py_bcol);
}
