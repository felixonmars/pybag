#include <fmt/core.h>

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
    c_box ans = c_box::invalid_box();
    for (const auto &barr : data_) {
        ans.merge(barr.get_overall_bbox());
    }
    return ans;
}

box_collection box_collection::get_transform(offset_t dx, offset_t dy, uint32_t ocode) const {
    std::vector<c_box_arr> temp(data_);
    for (auto &barr : temp) {
        barr.transform(dx, dy, ocode);
    }

    return {std::move(temp)};
}

box_collection box_collection::get_transform_compat(pyg::Tuple<offset_t, offset_t> loc,
                                                    py::str orient, bool unit_mode) const {
    if (!unit_mode)
        throw std::invalid_argument("unit_mode = False is not supported.");
    return get_transform(loc.get<0>(), loc.get<1>(), get_orient_code(orient));
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
    py_cls.def("transform", &c_box_col::get_transform, "Returns a transformed BBoxCollection.",
               py::arg("dx") = 0, py::arg("dy") = 0, py::arg("ocode") = code_R0);
    py_cls.def("transform", &c_box_col::get_transform_compat,
               "Returns a transformed BBoxCollection.",
               py::arg("loc") = std::pair<offset_t, offset_t>(0, 0), py::arg("orient") = "R0",
               py::arg("unit_mode") = true);

    py_cls.def("as_bbox_array", &c_box_col::as_bbox_array,
               "Cast this BBoxCollection to BBoxArray if able.");
    py_cls.def("as_bbox", &c_box_col::as_bbox, "Cast this BBoxCollection to BBox if able.");
}
