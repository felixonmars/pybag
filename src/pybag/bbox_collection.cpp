#include <fmt/core.h>

#include <cbag/common/box_t_util.h>
#include <cbag/common/transformation.h>

#include <pybag/bbox_collection.h>
#include <pybag/orient_conv.h>

namespace pybag {
namespace util {

box_collection::box_collection() = default;

box_collection::box_collection(std::vector<c_box_arr> &&data) : data_(std::move(data)) {}

box_collection::box_collection(pyg::Iterable<c_box_arr> barr_iter) {
    // move the temporary C++ objects in the iterator to vector for efficiency.
    for (auto &barr : barr_iter) {
        data_.emplace_back(std::move(barr));
    }
}

std::size_t box_collection::size() const { return data_.size(); }

std::string box_collection::to_string() const {
    switch (data_.size()) {
    case 0:
        return "BBoxCollection()";
    case 1:
        return fmt::format("BBoxCollection({})", data_.front().to_string());
    default:
        auto iter = data_.begin();
        std::string ans = "BBoxCollection(" + iter->to_string();
        ++iter;
        for (; iter != data_.end(); ++iter) {
            ans += ", ";
            ans += iter->to_string();
        }
        ans += ")";
        return ans;
    }
}

bool box_collection::operator==(const box_collection &other) const { return data_ == other.data_; }

box_collection::const_iterator box_collection::begin() const { return data_.begin(); }

box_collection::const_iterator box_collection::end() const { return data_.end(); }

c_box_arr box_collection::as_bbox_array() const {
    if (data_.size() != 1)
        throw std::invalid_argument(fmt::format(
            "Cannot convert BBoxCollection to BBoxArray (size = {} != 1)", data_.size()));
    return data_.front();
}

c_box box_collection::as_bbox() const {
    if (data_.size() != 1)
        throw std::invalid_argument(fmt::format(
            "Cannot convert BBoxCollection to BBoxArray (size = {} != 1)", data_.size()));
    return data_.front().as_bbox();
}

c_box box_collection::get_bounding_box() const {
    auto ans = c_box::get_invalid_box();
    for (const auto &barr : data_) {
        merge(ans, barr.get_overall_bbox());
    }
    return ans;
}

box_collection &box_collection::move_by(offset_t dx, offset_t dy) {
    for (auto &barr : data_) {
        barr.move_by(dx, dy);
    }
    return *this;
}

box_collection box_collection::get_move_by(offset_t dx, offset_t dy) const {
    return box_collection(*this).move_by(dx, dy);
}

box_collection &box_collection::transform(const cbag::transformation &xform) {
    for (auto &barr : data_) {
        barr.transform(xform);
    }
    return *this;
}

box_collection box_collection::get_transform(const cbag::transformation &xform) const {
    return box_collection(*this).transform(xform);
}

pyg::Iterator<c_box_arr> get_box_arr_iter(const box_collection &bcol) {
    return pyg::make_iterator(bcol.begin(), bcol.end());
}

} // namespace util
} // namespace pybag

namespace pu = pybag::util;

void bind_bbox_collection(py::class_<c_box_col> &py_cls) {

    pyg::declare_iterator<c_box_col::const_iterator>();

    py_cls.doc() = "A collection of BBoxArrays.";
    py_cls.def(py::init<pyg::Iterable<c_box_arr>>(), "Create a new BBoxCollection.",
               py::arg("data"));

    py_cls.def("__repr__", &c_box_col::to_string,
               "Returns a string representation of BBoxCollection.");
    py_cls.def("__eq__", &c_box_col::operator==, "Returns True if the two BBoxCollection are equal",
               py::arg("other"));
    py_cls.def("__iter__", &pu::get_box_arr_iter,
               "Returns an iterator over BBoxArray in this collection.");
    py_cls.def("__len__", &c_box_col::size, "Returns the number of BBoxArrays in this collection.");

    py_cls.def("get_bounding_box", &c_box_col::get_bounding_box,
               "Returns the bounding box of this collection.");

    py_cls.def("move_by", &c_box_col::move_by, "Moves this BBoxCollection.", py::arg("dx") = 0,
               py::arg("dy") = 0);
    py_cls.def("get_move_by", &c_box_col::get_move_by, "Returns a moved BBoxCollection.",
               py::arg("dx") = 0, py::arg("dy") = 0);
    py_cls.def("transform", &c_box_col::transform, "Transforms BBoxCollection.", py::arg("xform"));
    py_cls.def("get_transform", &c_box_col::get_transform, "Returns a transformed BBoxCollection.",
               py::arg("xform"));

    py_cls.def("as_bbox_array", &c_box_col::as_bbox_array,
               "Cast this BBoxCollection to BBoxArray if able.");
    py_cls.def("as_bbox", &c_box_col::as_bbox, "Cast this BBoxCollection to BBox if able.");
}
