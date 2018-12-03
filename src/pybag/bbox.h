#ifndef PYBAG_BBOX_H
#define PYBAG_BBOX_H

#include <pybind11/pybind11.h>

#include <cbag/common/box_t.h>

namespace py = pybind11;

using c_box = cbag::box_t;

void bind_bbox(py::class_<c_box> &);

#endif
