#include <cstdlib>
#include <iterator>

#include <fmt/format.h>

#include <cbag/common/box_t.h>
#include <cbag/common/transformation.h>
#include <cbag/util/floor_half.h>

#include <pybind11_generics/iterator.h>
#include <pybind11_generics/tuple.h>

#include <pybag/util/bbox_array.h>
#include <pybag/util/orient_conv.h>

namespace pyg = pybind11_generics;

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
    box_arr(c_box base, uint32_t nx, uint32_t ny, offset_t spx, offset_t spy)
        : base(std::move(base)), nx(nx), ny(ny), spx(spx), spy(spy) {}
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

    bool operator==(const box_arr &other) const {
        return base == other.base && nx == other.nx && ny == other.ny && spx == other.spx &&
               spy == other.spy;
    }

    c_box get_bbox(uint32_t idx) const {
        auto result = std::div(static_cast<long>(idx), static_cast<long>(nx));
        return base.get_move_by(result.rem * spx, result.quot * spy);
    }

    c_box get_overall_bbox() const { return {xl(), yl(), xh(), yh()}; }

    box_arr get_move_by(offset_t dx, offset_t dy, bool unit_mode) const {
        if (!unit_mode)
            throw std::invalid_argument("unit_mode = False is not supported.");
        return {base.get_move_by(dx, dy), nx, ny, spx, spy};
    }

    box_arr get_transform(offset_t dx, offset_t dy, uint32_t ocode) const {
        coord_t spx_new = spx;
        coord_t spy_new = spy;
        cbag::transformation xform(dx, dy, ocode);
        xform.get_axis_transformation().transform(spx_new, spy_new);
        return {base.get_transform(xform), nx, ny, spx_new, spy_new};
    }

    box_arr get_transform_compat(pyg::Tuple<offset_t, offset_t> loc, py::str orient,
                                 bool unit_mode) {
        if (!unit_mode)
            throw std::invalid_argument("unit_mode = False is not supported.");
        return get_transform(loc.get<0>(), loc.get<1>(), get_orient_code(orient));
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

pyg::Iterator<c_box> get_box_iter(box_arr *barr) {
    box_iter start(barr, 0);
    box_iter end(barr, barr->nx * barr->ny);
    return pyg::make_iterator(start, end);
}

} // namespace util
} // namespace pybag

namespace pu = pybag::util;

using c_box_arr = pu::box_arr;

void bind_bbox_array(py::module &m) {

    pyg::declare_iterator<pu::box_iter>();

    auto py_cls = py::class_<c_box_arr>(m, "BBoxArray");
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
               py::arg("dx") = 0, py::arg("dy") = 0, py::arg("ocode") = code_R0);
    py_cls.def("transform", &c_box_arr::get_transform_compat, "Returns a transformed BBoxArray.",
               py::arg("loc") = std::pair<offset_t, offset_t>(0, 0), py::arg("orient") = "R0",
               py::arg("unit_mode") = true);
}