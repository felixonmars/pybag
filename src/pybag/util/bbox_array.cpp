#include <cstdlib>
#include <iterator>

#include <fmt/format.h>

#include <cbag/common/box_t.h>
#include <cbag/util/floor_half.h>

#include <pybind11_generics/iterator.h>

#include <pybag/util/bbox_array.h>

using c_box = cbag::box_t;
using coord_t = cbag::coord_t;
using offset_t = cbag::offset_t;

namespace pybag {
namespace util {

class box_arr {
  public:
    c_box base;
    uint32_t nx = 1;
    uint32_t ny = 1;
    offset_t spx = 0;
    offset_t spy = 0;

    box_arr() = default;
    box_arr(c_box base, uint32_t nx, uint32_t ny, offset_t spx, offset_t spy, py::kwargs kwargs)
        : base(std::move(base)), nx(nx), ny(ny), spx(spx), spy(spy) {}

    coord_t xl() const { return (spx >= 0) ? base.xl() : (nx - 1) * spx + base.xl(); }
    coord_t xh() const { return (spx >= 0) ? (nx - 1) * spx + base.xh() : base.xh(); }
    coord_t yl() const { return (spx >= 0) ? base.yl() : (ny - 1) * spy + base.yl(); }
    coord_t yh() const { return (spx >= 0) ? (ny - 1) * spy + base.yh() : base.yh(); }
    coord_t xm() const { return cbag::util::floor_half(xl() + xh()); }
    coord_t ym() const { return cbag::util::floor_half(yl() + yh()); }

    std::string to_string() const {
        return fmt::format("BBoxArray({}, {}, {}, {}, {})", base.to_string(), nx, ny, spx, spy);
    }

    bool operator==(const box_arr &other) {
        return base == other.base && nx == other.nx && ny == other.ny && spx == other.spx &&
               spy == other.spy;
    }

    c_box get_bbox(uint32_t idx) {
        auto result = std::div(static_cast<long>(idx), static_cast<long>(nx));
        return base.get_move_by(result.rem * spx, result.quot * spy);
    }
};

class box_iter {
  public:
    using iterator_category = std::input_iterator_tag;
    using difference_type = Py_ssize_t;
    using value_type = c_box;
    using reference = const value_type &;
    using pointer = const value_type *;

  private:
    box_arr *parent_ = nullptr;
    uint32_t idx_ = 0;

  public:
    box_iter() = default;
    box_iter(box_arr *parent, uint32_t idx) : parent_(parent), idx_(idx) {}

    box_iter &operator++() {
        ++idx_;
        return *this;
    }

    value_type operator*() const { return parent_->get_bbox(idx_); }

    bool operator==(const box_iter &other) {
        return parent_ == other.parent_ && idx_ == other.idx_;
    }
    bool operator!=(const box_iter &other) { return !(*this == other); }
};

} // namespace util
} // namespace pybag

namespace pu = pybag::util;

using c_box_arr = pu::box_arr;

void bind_bbox_array(py::module &m) {
    auto py_cls = py::class_<c_box_arr>(m, "BBoxArray");
    py_cls.doc() = "The bounding box array class.";
    py_cls.def(py::init<c_box, uint32_t, uint32_t, offset_t, offset_t, py::kwargs>(),
               "Construct a new BBoxArray.", py::arg("base"), py::arg("nx") = 1, py::arg("ny") = 1,
               py::arg("spx") = 0, py::arg("spy") = 0);

    py_cls.def("__repr__", &c_box_arr::to_string, "Returns a string representation of BBoxArray.");
    py_cls.def("__eq__", &c_box_arr::operator==, "Returns True if the two BBoxArray are equal.",
               py::arg("other"));
}
