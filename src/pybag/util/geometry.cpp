#include <pybag/util/bbox.h>
#include <pybag/util/bbox_array.h>
#include <pybag/util/bbox_collection.h>

PYBIND11_MODULE(geometry, m) {
    m.doc() = "This module contains various geometry utility classes.";

    bind_bbox(m);
    bind_bbox_array(m);
    bind_bbox_collection(m);
}
