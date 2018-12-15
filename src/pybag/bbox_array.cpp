#include <cstdlib>
#include <iterator>

#include <fmt/core.h>

#include <cbag/common/box_t.h>
#include <cbag/common/transformation.h>
#include <cbag/util/floor_half.h>

#include <pybind11_generics/iterator.h>

#include <pybag/bbox_array.h>
#include <pybag/bbox_collection.h>
#include <pybag/orient_conv.h>

namespace pyg = pybind11_generics;

namespace pybag {
namespace util {

box_arr::box_arr() = default;
box_arr::box_arr(c_box base, uint32_t nx, uint32_t ny, offset_t spx, offset_t spy)
    : base(std::move(base)), nx(nx), ny(ny), spx(spx), spy(spy) {
    if (nx <= 0 || ny <= 0)
        throw std::invalid_argument(
            fmt::format("nx = {} and ny = {} cannot be non-positive.", nx, ny));
}
box_arr::box_arr(c_box base, uint32_t nx, uint32_t ny, offset_t spx, offset_t spy,
                 py::kwargs kwargs)
    : box_arr(std::move(base), nx, ny, spx, spy) {}

coord_t box_arr::xl() const { return (spx >= 0) ? base.xl() : (nx - 1) * spx + base.xl(); }
coord_t box_arr::xh() const { return (spx >= 0) ? (nx - 1) * spx + base.xh() : base.xh(); }
coord_t box_arr::yl() const { return (spx >= 0) ? base.yl() : (ny - 1) * spy + base.yl(); }
coord_t box_arr::yh() const { return (spx >= 0) ? (ny - 1) * spy + base.yh() : base.yh(); }
coord_t box_arr::xm() const { return cbag::util::floor_half(xl() + xh()); }
coord_t box_arr::ym() const { return cbag::util::floor_half(yl() + yh()); }

std::string box_arr::to_string() const {
    return fmt::format("BBoxArray({}, {}, {}, {}, {})", base.to_string(), nx, ny, spx, spy);
}

bool box_arr::operator==(const box_arr &other) const {
    return base == other.base && nx == other.nx && ny == other.ny && spx == other.spx &&
           spy == other.spy;
}

c_box box_arr::get_bbox(uint32_t idx) const {
    auto result = std::div(static_cast<long>(idx), static_cast<long>(nx));
    return base.get_move_by(result.rem * spx, result.quot * spy);
}

c_box box_arr::get_overall_bbox() const { return {xl(), yl(), xh(), yh()}; }

c_box box_arr::as_bbox() const {
    if (nx != 1 || ny != 1)
        throw std::invalid_argument(
            fmt::format("Cannot cast this BBoxArray to BBox (nx = {}, ny = {})", nx, ny));
    return base;
}

box_collection box_arr::as_bbox_collection() const { return {std::vector<box_arr>{*this}}; }

box_arr box_arr::get_move_by(offset_t dx, offset_t dy, bool unit_mode) const {
    if (!unit_mode)
        throw std::invalid_argument("unit_mode = False is not supported.");
    return {base.get_move_by(dx, dy), nx, ny, spx, spy};
}

box_arr box_arr::get_transform(const cbag::transformation &xform) const {
    return box_arr(*this).transform(xform);
}

box_arr box_arr::get_transform_compat(pyg::Tuple<offset_t, offset_t> loc, py::str orient,
                                      bool unit_mode) const {
    if (!unit_mode)
        throw std::invalid_argument("unit_mode = False is not supported.");
    return get_transform(cbag::transformation(loc.get<0>(), loc.get<1>(), get_orient_code(orient)));
}

box_arr &box_arr::transform(const cbag::transformation &xform) {
    xform.get_axis_transformation().transform(spx, spy);
    if (xform.flips_xy()) {
        auto tmp = nx;
        nx = ny;
        ny = tmp;
    }
    base.transform(xform);
    return *this;
}

class box_arr_iter {
  public:
    using iterator_category = std::input_iterator_tag;
    using difference_type = Py_ssize_t;
    using value_type = c_box;
    using reference = const value_type &;
    using pointer = const value_type *;

  private:
    const box_arr *parent_ = nullptr;
    uint32_t idx_ = 0;

  public:
    box_arr_iter() = default;
    box_arr_iter(const box_arr *parent, uint32_t idx) : parent_(parent), idx_(idx) {}

    box_arr_iter &operator++() {
        ++idx_;
        return *this;
    }

    value_type operator*() const { return parent_->get_bbox(idx_); }

    bool operator==(const box_arr_iter &other) {
        return parent_ == other.parent_ && idx_ == other.idx_;
    }
    bool operator!=(const box_arr_iter &other) { return !(*this == other); }
};

box_arr_iter box_arr::begin() const { return {this, 0}; }

box_arr_iter box_arr::end() const { return {this, nx * ny}; }

pyg::Iterator<c_box> get_box_iter(const box_arr &barr) {
    return pyg::make_iterator(barr.begin(), barr.end());
}

} // namespace util
} // namespace pybag

namespace pu = pybag::util;

void bind_bbox_array(py::class_<c_box_arr> &py_cls) {

    pyg::declare_iterator<pu::box_arr_iter>();

    py_cls.doc() = "The bounding box array class.";
    py_cls.def(py::init<c_box, uint32_t, uint32_t, offset_t, offset_t, py::kwargs>(),
               "Construct a new BBoxArray.", py::arg("base"), py::arg("nx") = 1, py::arg("ny") = 1,
               py::arg("spx") = 0, py::arg("spy") = 0);

    py_cls.def("__repr__", &c_box_arr::to_string, "Returns a string representation of BBoxArray.");
    py_cls.def("__eq__", &c_box_arr::operator==, "Returns True if the two BBoxArray are equal.",
               py::arg("other"));
    py_cls.def("__iter__", &pu::get_box_iter, "Returns an iterator over BBox in this BBoxArray.");

    py_cls.def_readonly("base", &c_box_arr::base, "The base BBox object.");
    py_cls.def_readonly("nx", &c_box_arr::nx, "Number of columns.");
    py_cls.def_readonly("ny", &c_box_arr::ny, "Number of rows.");
    py_cls.def_readonly("spx_unit", &c_box_arr::spx, "Column pitch");
    py_cls.def_readonly("spy_unit", &c_box_arr::spy, "Row pitch.");
    py_cls.def_property_readonly("xl", &c_box_arr::xl, "Left-most edge");
    py_cls.def_property_readonly("left_unit", &c_box_arr::xl, "Left-most edge");
    py_cls.def_property_readonly("xh", &c_box_arr::xh, "Right-most edge");
    py_cls.def_property_readonly("right_unit", &c_box_arr::xh, "Right-most edge");
    py_cls.def_property_readonly("yl", &c_box_arr::yl, "Bottom-most edge");
    py_cls.def_property_readonly("bottom_unit", &c_box_arr::yl, "Bottom-most edge");
    py_cls.def_property_readonly("yh", &c_box_arr::yh, "Top-most edge");
    py_cls.def_property_readonly("top_unit", &c_box_arr::yh, "Top-most edge");
    py_cls.def_property_readonly("xm", &c_box_arr::xm, "Cetner X coordinate");
    py_cls.def_property_readonly("xc_unit", &c_box_arr::xm, "Center X coordinate");
    py_cls.def_property_readonly("ym", &c_box_arr::ym, "Cetner X coordinate");
    py_cls.def_property_readonly("yc_unit", &c_box_arr::ym, "Center X coordinate");

    py_cls.def("get_bbox", &c_box_arr::get_bbox, "Returns the BBox with the given index.",
               py::arg("idx"));
    py_cls.def("get_overall_bbox", &c_box_arr::get_overall_bbox, "Returns the overall BBox.");
    py_cls.def("move_by", &c_box_arr::get_move_by, "Returns a new translated BBoxArray.",
               py::arg("dx") = 0, py::arg("dy") = 0, py::arg("unit_mode") = true);
    py_cls.def("transform", &c_box_arr::get_transform, "Returns a transformed BBoxArray.",
               py::arg("xform"));
    py_cls.def("transform", &c_box_arr::get_transform_compat, "Returns a transformed BBoxArray.",
               py::arg("loc") = std::pair<offset_t, offset_t>(0, 0), py::arg("orient") = "R0",
               py::arg("unit_mode") = true);

    py_cls.def("as_bbox", &c_box_arr::as_bbox,
               "Returns a BBox representation of this BBoxArray if able.");
    py_cls.def("as_bbox_collection", &c_box_arr::as_bbox_collection,
               "Returns a BBoxCollection representation of this BBoxArray.");
}
