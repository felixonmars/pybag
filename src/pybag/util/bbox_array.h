#ifndef PYBAG_BBOX_ARRAY_H
#define PYBAG_BBOX_ARRAY_H

#include <pybind11/pybind11.h>

#include <pybind11_generics/tuple.h>

#include <cbag/common/box_t.h>

namespace py = pybind11;
namespace pyg = pybind11_generics;

using c_box = cbag::box_t;
using coord_t = cbag::coord_t;
using offset_t = cbag::offset_t;

namespace pybag {
namespace util {

class box_arr_iter;

class box_collection;

class box_arr {
  public:
    c_box base;
    uint32_t nx = 1;
    uint32_t ny = 1;
    offset_t spx = 0;
    offset_t spy = 0;

    box_arr();
    box_arr(c_box base, uint32_t nx, uint32_t ny, offset_t spx, offset_t spy);
    box_arr(c_box base, uint32_t nx, uint32_t ny, offset_t spx, offset_t spy, py::kwargs kwargs);

    coord_t xl() const;
    coord_t xh() const;
    coord_t yl() const;
    coord_t yh() const;
    coord_t xm() const;
    coord_t ym() const;

    std::string to_string() const;

    bool operator==(const box_arr &other) const;

    c_box get_bbox(uint32_t idx) const;

    c_box get_overall_bbox() const;

    c_box as_bbox() const;

    box_collection as_bbox_collection() const;

    box_arr get_move_by(offset_t dx, offset_t dy, bool unit_mode) const;

    box_arr get_transform(offset_t dx, offset_t dy, uint32_t ocode) const;

    box_arr get_transform_compat(pyg::Tuple<offset_t, offset_t> loc, py::str orient,
                                 bool unit_mode) const;

    box_arr_iter begin() const;
    box_arr_iter end() const;

    box_arr &transform(offset_t dx, offset_t dy, uint32_t ocode);
};

} // namespace util
} // namespace pybag

using c_box_arr = pybag::util::box_arr;

void bind_bbox_array(py::module &);

#endif
