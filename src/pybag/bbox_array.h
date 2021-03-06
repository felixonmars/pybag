#ifndef PYBAG_BBOX_ARRAY_H
#define PYBAG_BBOX_ARRAY_H

#include <optional>
#include <utility>

#include <pybind11/pybind11.h>

#include <pybind11_generics/tuple.h>

#include <cbag/common/box_t.h>
#include <cbag/common/transformation_fwd.h>

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
    std::array<cbag::cnt_t, 2> num;
    std::array<offset_t, 2> sp;

  public:
    c_box base;

    box_arr();
    box_arr(c_box base, cbag::scnt_t nx = 1, cbag::scnt_t ny = 1, offset_t spx = 0,
            offset_t spy = 0);
    box_arr(c_box base, cbag::orient_2d_t orient_code, cbag::scnt_t nt = 1, offset_t spt = 0,
            cbag::scnt_t np = 1, offset_t spp = 0);

    cbag::cnt_t nx() const noexcept;
    cbag::cnt_t ny() const noexcept;
    offset_t spx() const noexcept;
    offset_t spy() const noexcept;

    const std::array<cbag::cnt_t, 2> &get_num() const noexcept;
    const std::array<cbag::offset_t, 2> &get_sp() const noexcept;

    coord_t get_coord(cbag::orient_2d_t orient_code, bool bnd_code) const;
    std::pair<cbag::cnt_t, offset_t> get_array_info(cbag::orient_2d_t orient_code) const;

    coord_t xl() const noexcept;
    coord_t xh() const noexcept;
    coord_t yl() const noexcept;
    coord_t yh() const noexcept;
    coord_t xm() const noexcept;
    coord_t ym() const noexcept;

    std::string to_string() const;

    bool operator==(const box_arr &other) const;

    c_box get_bbox(cbag::cnt_t idx) const;

    c_box get_overall_bbox() const;

    c_box as_bbox() const;

    box_collection as_bbox_collection() const;

    box_arr get_copy() const;
    box_arr get_move_by(offset_t dx = 0, offset_t dy = 0) const;
    box_arr get_transform(const cbag::transformation &xform) const;
    box_arr get_extend_orient(cbag::orient_2d_t orient_code, const std::optional<coord_t> &ct,
                              const std::optional<coord_t> &cp) const;

    box_arr_iter begin() const;
    box_arr_iter end() const;

    box_arr &move_by(offset_t dx = 0, offset_t dy = 0);
    box_arr &transform(const cbag::transformation &xform);
    box_arr &extend_orient(cbag::orient_2d_t orient_code, const std::optional<coord_t> &ct,
                           const std::optional<coord_t> &cp);
};

} // namespace util
} // namespace pybag

using c_box_arr = pybag::util::box_arr;

void bind_bbox_array(py::class_<c_box_arr> &);

#endif
