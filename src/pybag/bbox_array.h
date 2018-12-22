#ifndef PYBAG_BBOX_ARRAY_H
#define PYBAG_BBOX_ARRAY_H

#include <pybind11/pybind11.h>

#include <pybind11_generics/tuple.h>

#include <cbag/common/box_t.h>

#include <pybag/typedefs.h>

namespace py = pybind11;
namespace pyg = pybind11_generics;

using c_box = cbag::box_t;

namespace pybag {
namespace util {

class box_arr_iter;

class box_collection;

class box_arr {
  private:
    std::array<uint32_t, 2> num;
    std::array<offset_t, 2> sp;

  public:
    c_box base;

    box_arr();
    box_arr(c_box base, int32_t nx = 1, int32_t ny = 1, offset_t spx = 0, offset_t spy = 0);
    box_arr(c_box base, uint8_t orient_code, int32_t nt = 1, offset_t spt = 0, int32_t np = 1,
            offset_t spp = 0);

    uint32_t nx() const;
    uint32_t ny() const;
    offset_t spx() const;
    offset_t spy() const;

    coord_t get_coord(uint8_t orient_code, uint8_t bnd_code) const;

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

    box_arr get_move_by(offset_t dx = 0, offset_t dy = 0) const;

    box_arr get_transform(const cbag::transformation &xform) const;

    box_arr_iter begin() const;
    box_arr_iter end() const;

    box_arr &move_by(offset_t dx = 0, offset_t dy = 0);
    box_arr &transform(const cbag::transformation &xform);
};

} // namespace util
} // namespace pybag

using c_box_arr = pybag::util::box_arr;

void bind_bbox_array(py::class_<c_box_arr> &);

#endif
