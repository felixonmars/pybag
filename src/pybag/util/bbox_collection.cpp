
#include <pybag/util/bbox_collection.h>

namespace pybag {
namespace util {

box_collection::box_collection() = default;

} // namespace util
} // namespace pybag

namespace pu = pybag::util;

void bind_bbox_collection(py::module &m) {

    auto py_cls = py::class_<c_box_col>(m, "BBoxCollection");
    py_cls.doc() = "A collection of BBoxArrays.";
}
