#include <cstdlib>
#include <iterator>

#include <fmt/core.h>

#include <pybind11/stl.h>

#include <cbag/common/box_t_util.h>
#include <cbag/common/transformation_util.h>
#include <cbag/util/math.h>

#include <pybind11_generics/iterator.h>

#include <pybag/bbox_array.h>
#include <pybag/bbox_collection.h>
#include <pybag/orient_conv.h>

namespace pyg = pybind11_generics;

namespace pybag {
namespace util {

box_arr::box_arr() : num{1, 1}, sp{0, 0} {}

box_arr::box_arr(c_box base, cbag::scnt_t nx, cbag::scnt_t ny, offset_t spx, offset_t spy)
    : num{static_cast<cbag::cnt_t>(nx), static_cast<cbag::cnt_t>(ny)}, sp{spx, spy},
      base(std::move(base)) {
    if (nx <= 0 || ny <= 0)
        throw std::invalid_argument(
            fmt::format("nx = {} and ny = {} cannot be non-positive.", nx, ny));
}

box_arr::box_arr(c_box base, cbag::orient_2d_t orient_code, cbag::scnt_t nt, offset_t spt,
                 cbag::scnt_t np, offset_t spp)
    : base(std::move(base)) {
    if (nt <= 0 || np <= 0)
        throw std::invalid_argument(
            fmt::format("nt = {} and np = {} cannot be non-positive.", nt, np));
    num[orient_code] = static_cast<cbag::cnt_t>(nt);
    sp[orient_code] = spt;
    num[1 - orient_code] = static_cast<cbag::cnt_t>(np);
    sp[1 - orient_code] = spp;
}

cbag::cnt_t box_arr::nx() const noexcept { return num[0]; }
cbag::cnt_t box_arr::ny() const noexcept { return num[1]; }
offset_t box_arr::spx() const noexcept { return sp[0]; }
offset_t box_arr::spy() const noexcept { return sp[1]; }

const std::array<cbag::cnt_t, 2> &box_arr::get_num() const noexcept { return num; }

const std::array<cbag::offset_t, 2> &box_arr::get_sp() const noexcept { return sp; }

coord_t box_arr::get_coord(cbag::orient_2d_t orient_code, bool bnd_code) const {
    auto cur_coord = base.intvs[orient_code][bnd_code];
    offset_t delta = (num[orient_code] - 1) * sp[orient_code];
    if (delta < 0)
        bnd_code = 1 - bnd_code;
    return cur_coord + bnd_code * delta;
}

std::pair<cbag::cnt_t, offset_t> box_arr::get_array_info(cbag::orient_2d_t orient_code) const {
    return {num[orient_code], sp[orient_code]};
}

coord_t box_arr::xl() const noexcept { return get_coord(0, 0); }
coord_t box_arr::xh() const noexcept { return get_coord(0, 1); }
coord_t box_arr::yl() const noexcept { return get_coord(1, 0); }
coord_t box_arr::yh() const noexcept { return get_coord(1, 1); }
coord_t box_arr::xm() const noexcept { return cbag::util::floor2(xl() + xh()); }
coord_t box_arr::ym() const noexcept { return cbag::util::floor2(yl() + yh()); }

std::string box_arr::to_string() const {
    return fmt::format("BBoxArray({}, {}, {}, {}, {})", cbag::to_string(base), num[0], num[1],
                       sp[0], sp[1]);
}

bool box_arr::operator==(const box_arr &other) const {
    return base == other.base && num == other.num && sp == other.sp;
}

c_box box_arr::get_bbox(cbag::cnt_t idx) const {
    auto result = std::div(static_cast<long>(idx), static_cast<long>(num[0]));
    return cbag::get_move_by(base, result.rem * sp[0], result.quot * sp[1]);
}

c_box box_arr::get_overall_bbox() const { return {xl(), yl(), xh(), yh()}; }

c_box box_arr::as_bbox() const {
    if (num[0] != 1 || num[1] != 1)
        throw std::invalid_argument(
            fmt::format("Cannot cast this BBoxArray to BBox (nx = {}, ny = {})", num[0], num[1]));
    return base;
}

box_collection box_arr::as_bbox_collection() const { return {std::vector<box_arr>{*this}}; }

box_arr &box_arr::move_by(offset_t dx, offset_t dy) {
    cbag::move_by(base, dx, dy);
    return *this;
}

box_arr box_arr::get_move_by(offset_t dx, offset_t dy) const {
    return box_arr(*this).move_by(dx, dy);
}

box_arr &box_arr::transform(const cbag::transformation &xform) {
    xform.get_axis_transformation().transform(sp[0], sp[1]);
    if (cbag::flips_xy(xform)) {
        auto tmp = num[0];
        num[0] = num[1];
        num[1] = tmp;
    }
    cbag::transform(base, xform);
    return *this;
}

box_arr box_arr::get_transform(const cbag::transformation &xform) const {
    return box_arr(*this).transform(xform);
}

box_arr &box_arr::extend_orient(cbag::orient_2d_t orient_code, const std::optional<coord_t> &ct,
                                const std::optional<coord_t> &cp) {
    cbag::extend_orient(base, static_cast<cbag::orient_2d>(orient_code), ct, cp);
    return *this;
}
box_arr box_arr::get_extend_orient(cbag::orient_2d_t orient_code, const std::optional<coord_t> &ct,
                                   const std::optional<coord_t> &cp) const {
    return box_arr(*this).extend_orient(orient_code, ct, cp);
}

box_arr box_arr::get_copy() const { return box_arr(*this); }

class box_arr_iter {
  public:
    using iterator_category = std::input_iterator_tag;
    using difference_type = Py_ssize_t;
    using value_type = c_box;
    using reference = const value_type &;
    using pointer = const value_type *;

  private:
    const box_arr *parent_ = nullptr;
    cbag::cnt_t idx_ = 0;

  public:
    box_arr_iter() = default;
    box_arr_iter(const box_arr *parent, cbag::cnt_t idx) : parent_(parent), idx_(idx) {}

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

box_arr_iter box_arr::end() const { return {this, num[0] * num[1]}; }

pyg::Iterator<c_box> get_box_iter(const box_arr &barr) {
    return pyg::make_iterator(barr.begin(), barr.end());
}

} // namespace util
} // namespace pybag

namespace pu = pybag::util;

void bind_bbox_array(py::class_<c_box_arr> &py_cls) {

    pyg::declare_iterator<pu::box_arr_iter>();

    py_cls.doc() = "The bounding box array class.";
    py_cls.def(py::init<c_box, cbag::scnt_t, cbag::scnt_t, offset_t, offset_t>(),
               "Construct a new BBoxArray.", py::arg("base"), py::arg("nx") = 1, py::arg("ny") = 1,
               py::arg("spx") = 0, py::arg("spy") = 0);
    py_cls.def(py::init<c_box, cbag::orient_2d_t, cbag::scnt_t, offset_t, cbag::scnt_t, offset_t>(),
               "Construct a new BBoxArray from orientation.", py::arg("base"),
               py::arg("orient_code"), py::arg("nt") = 1, py::arg("spt") = 0, py::arg("np") = 1,
               py::arg("spp") = 0);

    py_cls.def("__repr__", &c_box_arr::to_string, "Returns a string representation of BBoxArray.");
    py_cls.def("__eq__", &c_box_arr::operator==, "Returns True if the two BBoxArray are equal.",
               py::arg("other"));
    py_cls.def("__iter__", &pu::get_box_iter, "Returns an iterator over BBox in this BBoxArray.");

    py_cls.def_readonly("base", &c_box_arr::base, "The base bounding box.");
    py_cls.def_property_readonly("nx", &c_box_arr::nx, "Number of columns.");
    py_cls.def_property_readonly("ny", &c_box_arr::ny, "Number of rows.");
    py_cls.def_property_readonly("spx", &c_box_arr::spx, "Column pitch");
    py_cls.def_property_readonly("spy", &c_box_arr::spy, "Row pitch.");
    py_cls.def_property_readonly("xl", &c_box_arr::xl, "Left-most edge");
    py_cls.def_property_readonly("xh", &c_box_arr::xh, "Right-most edge");
    py_cls.def_property_readonly("yl", &c_box_arr::yl, "Bottom-most edge");
    py_cls.def_property_readonly("yh", &c_box_arr::yh, "Top-most edge");
    py_cls.def_property_readonly("xm", &c_box_arr::xm, "Cetner X coordinate");
    py_cls.def_property_readonly("ym", &c_box_arr::ym, "Cetner X coordinate");
    py_cls.def(
        "get_num",
        [](c_box_arr &self, cbag::orient_2d_t orient_code) { return self.get_num()[orient_code]; },
        "Number of BBox in the given direction.", py::arg("orient_code"));
    py_cls.def(
        "get_sp",
        [](c_box_arr &self, cbag::orient_2d_t orient_code) { return self.get_sp()[orient_code]; },
        "Pitch of BBox in the given direction.", py::arg("orient_code"));

    /*
    py_cls.def_property_readonly("left_unit", &c_box_arr::xl, "Left-most edge");
    py_cls.def_property_readonly("right_unit", &c_box_arr::xh, "Right-most edge");
    py_cls.def_property_readonly("bottom_unit", &c_box_arr::yl, "Bottom-most edge");
    py_cls.def_property_readonly("top_unit", &c_box_arr::yh, "Top-most edge");
    py_cls.def_property_readonly("xc_unit", &c_box_arr::xm, "Center X coordinate");
    py_cls.def_property_readonly("yc_unit", &c_box_arr::ym, "Center X coordinate");
    py_cls.def_readonly("spx_unit", &c_box_arr::spx, "Column pitch");
    py_cls.def_readonly("spy_unit", &c_box_arr::spy, "Row pitch.");
    */

    py_cls.def("get_coord", &c_box_arr::get_coord, "Returns coordinate given orient/bound code.",
               py::arg("orient_code"), py::arg("bnd_code"));
    py_cls.def("get_array_info", &c_box_arr::get_array_info, "Returns num/pitch given orient code.",
               py::arg("orient_code"));

    py_cls.def("get_bbox", &c_box_arr::get_bbox, "Returns the BBox with the given index.",
               py::arg("idx"));
    py_cls.def("get_overall_bbox", &c_box_arr::get_overall_bbox, "Returns the overall BBox.");

    py_cls.def("move_by", &c_box_arr::move_by, "Moves this BBoxArray.", py::arg("dx") = 0,
               py::arg("dy") = 0);
    py_cls.def("get_move_by", &c_box_arr::get_move_by, "Returns a moved BBoxArray.",
               py::arg("dx") = 0, py::arg("dy") = 0);
    py_cls.def("transform", &c_box_arr::transform, "Transforms this BBoxArray.", py::arg("xform"));
    py_cls.def("get_transform", &c_box_arr::get_transform, "Returns a transformed BBoxArray.",
               py::arg("xform"));
    py_cls.def("set_interval",
               [](c_box_arr &self, cbag::orient_2d_t orient_code, coord_t lo, coord_t hi) {
                   cbag::set_interval(self.base, static_cast<cbag::orient_2d>(orient_code), lo, hi);
                   return self;
               },
               "Set the interval of the basee BBox of this BBoxArray.", py::arg("orient_code"),
               py::arg("lo"), py::arg("hi"));
    py_cls.def("extend_orient", &c_box_arr::extend_orient, "Extends this BBoxArray.",
               py::arg("orient_code"), py::arg("ct") = py::none(), py::arg("cp") = py::none());
    py_cls.def("get_extend_orient", &c_box_arr::get_extend_orient, "Returns an extended BBoxArray.",
               py::arg("orient_code"), py::arg("ct") = py::none(), py::arg("cp") = py::none());
    py_cls.def("get_copy", &c_box_arr::get_copy, "Returns a copy of this BBoxArray.");

    py_cls.def("as_bbox", &c_box_arr::as_bbox,
               "Returns a BBox representation of this BBoxArray if able.");
    py_cls.def("as_bbox_collection", &c_box_arr::as_bbox_collection,
               "Returns a BBoxCollection representation of this BBoxArray.");
}
