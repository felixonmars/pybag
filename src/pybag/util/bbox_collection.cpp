
#include <pybag/util/bbox_collection.h>

namespace pybag {
namespace util {

box_collection::box_collection() = default;

box_collection::box_collection(pyg::Iterable<c_box_arr> barr_iter) {
    // move the temporary C++ objects in the iterator to vector for efficiency.
    for (auto &barr : barr_iter) {
        data_.emplace_back(std::move(barr));
    }
}

std::size_t box_collection::size() const { return data_.size(); }

} // namespace util
} // namespace pybag

namespace pu = pybag::util;

void bind_bbox_collection(py::module &m) {

    auto py_cls = py::class_<c_box_col>(m, "BBoxCollection");
    py_cls.doc() = "A collection of BBoxArrays.";
}
