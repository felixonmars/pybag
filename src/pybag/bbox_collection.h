#ifndef PYBAG_BBOX_COLLECTION_H
#define PYBAG_BBOX_COLLECTION_H

#include <pybind11/pybind11.h>

#include <pybind11_generics/iterable.h>

#include <pybag/util/bbox_array.h>

namespace pybag {
namespace util {

class box_col_iter;

class box_collection {
  public:
    using const_iterator = std::vector<c_box_arr>::const_iterator;

  private:
    std::vector<c_box_arr> data_;

  public:
    box_collection();
    box_collection(std::vector<c_box_arr> &&data);
    box_collection(pyg::Iterable<c_box_arr> barr_iter);

    std::size_t size() const;

    std::string to_string() const;

    bool operator==(const box_collection &other) const;

    c_box_arr as_bbox_array() const;
    c_box as_bbox() const;

    const_iterator begin() const;
    const_iterator end() const;

    c_box get_bounding_box() const;
    box_collection get_transform(offset_t dx, offset_t dy, uint32_t ocode) const;
    box_collection get_transform_compat(pyg::Tuple<offset_t, offset_t> loc, py::str orient,
                                        bool unit_mode) const;
};

} // namespace util
} // namespace pybag

using c_box_col = pybag::util::box_collection;

void bind_bbox_collection(py::class_<c_box_col> &);

#endif
