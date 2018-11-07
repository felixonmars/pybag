#include <fmt/core.h>

#include <pybag/util/bbox_collection.h>
#include <pybag/util/orient_conv.h>

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

} // namespace util
} // namespace pybag

namespace pu = pybag::util;

void bind_bbox_collection(py::module &m) {

    auto py_cls = py::class_<c_box_col>(m, "BBoxCollection");
    py_cls.doc() = "A collection of BBoxArrays.";
}
