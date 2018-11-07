#ifndef PYBAG_BBOX_COLLECTION_H
#define PYBAG_BBOX_COLLECTION_H

#include <pybind11/pybind11.h>

#include <pybag/util/bbox_array.h>

namespace pybag {
namespace util {

class box_collection {
  public:
    std::vector<c_box_arr> barr_list;

    box_collection();
};

} // namespace util
} // namespace pybag

using c_box_col = pybag::util::box_collection;

void bind_bbox_collection(py::module &);

#endif
