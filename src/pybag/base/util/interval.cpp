#include <memory>
#include <utility>

#include <cbag/util/interval.h>

#include <pybind11_generics/iterator.h>

#include <pybag/base/util/interval.h>

namespace pyg = pybind11_generics;

using py_interval = std::pair<std::pair<cbag::offset_t, cbag::offset_t>, py::object>;

namespace cbag {
namespace util {
namespace traits {

template <> struct coordinate_type<py_interval> { using type = cbag::offset_t; };
template <> struct interval<py_interval> {
    using coordinate_type = coordinate_type<py_interval>::type;

    static std::pair<coordinate_type, coordinate_type> &intv(py_interval &i) { return i.first; }
    static const std::pair<coordinate_type, coordinate_type> &intv(const py_interval &i) {
        return i.first;
    }
    static coordinate_type start(const py_interval &i) { return i.first.first; }
    static coordinate_type stop(const py_interval &i) { return i.first.second; }
    static void set_start(py_interval &i, coordinate_type val) { i.first.first = val; }
    static void set_stop(py_interval &i, coordinate_type val) { i.first.second = val; }
    static py_interval construct(coordinate_type start, coordinate_type stop) {
        return std::make_pair(std::make_pair(start, stop), py::none());
    }
};

} // namespace traits
} // namespace util
} // namespace cbag

using c_intv_type = std::pair<cbag::offset_t, cbag::offset_t>;

namespace pybag {
namespace util {

using dis_intvs_val_base = cbag::util::disjoint_intvs<py_interval>;

class dis_intvs_val : public dis_intvs_val_base {
  public:
    class const_val_iterator {
      public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = const py::object;
        using difference_type = const_intv_iterator::difference_type;
        using pointer = value_type *;
        using reference = value_type &;

      private:
        const_iterator iter_;

      public:
        const_val_iterator() = default;
        const_val_iterator(const_iterator val) : iter_(std::move(val)) {}

        bool operator==(const const_val_iterator &other) const { return iter_ == other.iter_; }
        bool operator!=(const const_val_iterator &other) const { return iter_ != other.iter_; }

        reference operator*() const { return iter_->second; }
        pointer operator->() const { return &operator*(); }

        const_val_iterator &operator++() {
            ++iter_;
            return *this;
        }
        const_val_iterator operator++(int) {
            const_val_iterator ans(iter_);
            operator++();
            return ans;
        }
    };

    using dis_intvs_val_base::dis_intvs_val_base;

    pyg::Iterator<c_intv_type> py_intv_iterator() const {
        return pyg::make_iterator(intv_begin(), intv_end());
    }
    py::iterator py_item_iterator() const { return py::make_iterator(begin(), end()); }
    py::iterator py_val_iterator() const {
        return py::make_iterator(const_val_iterator(begin()), const_val_iterator(end()));
    }
    py::iterator py_ovl_intv_iterator(const c_intv_type &key) const {
        auto iter_pair = overlap_range(key);
        return py::make_iterator(const_intv_iterator(iter_pair.first),
                                 const_intv_iterator(iter_pair.second));
    }
    py::iterator py_ovl_item_iterator(const c_intv_type &key) const {
        auto iter_pair = overlap_range(key);
        return py::make_iterator(iter_pair.first, iter_pair.second);
    }
    py::iterator py_ovl_val_iterator(const c_intv_type &key) const {
        auto iter_pair = overlap_range(key);
        return py::make_iterator(const_val_iterator(iter_pair.first),
                                 const_val_iterator(iter_pair.second));
    }

    py::tuple get_first_overlap_item(const c_intv_type &key) const {
        auto iter_pair = overlap_range(key);
        if (iter_pair.first == iter_pair.second)
            return py::none();
        return py::cast(iter_pair.first);
    }

    bool add(py::tuple intv, py::object val = py::none(), bool merge = false, bool abut = false) {
        return emplace(merge, abut,
                       std::make_pair(intv[0].cast<py::int_>(), intv[1].cast<py::int_>()), val);
    }
};

} // namespace util
} // namespace pybag

namespace pu = pybag::util;

using c_dis_intvs = pu::dis_intvs_val;

void bind_util_interval(py::module &m_util) {
    pyg::declare_iterator<c_dis_intvs::const_intv_iterator>();

    py::module m = m_util.def_submodule("interval");
    m.doc() = "This module contains utility classes for manipulating intervals.";

    // add interval class
    auto py_dis_intvs = py::class_<c_dis_intvs>(m, "PyDisjointIntervals");
    py_dis_intvs.doc() = "A class that keeps track of disjoint intervals.";

    py_dis_intvs.def(py::init<>());
    py_dis_intvs.def_property_readonly("start", &c_dis_intvs::start,
                                       "the start coordinate of first interval.");
    py_dis_intvs.def_property_readonly("stop", &c_dis_intvs::stop,
                                       "the stop coordinate of last interval.");
    py_dis_intvs.def("__contains__", &c_dis_intvs::contains<c_intv_type>,
                     "Returns True if given interval is in this object.", py::arg("key"));
    py_dis_intvs.def("__iter__", &c_dis_intvs::py_intv_iterator, py::keep_alive<0, 1>(),
                     "Iterates through intervals.");
    py_dis_intvs.def("__len__", &c_dis_intvs::size, "Returns number of intervals.");
    py_dis_intvs.def("overlaps", &c_dis_intvs::overlaps<c_intv_type>,
                     "Returns True if given interval overlaps this object.", py::arg("key"));
    py_dis_intvs.def("covers", &c_dis_intvs::covers<c_intv_type>,
                     "Returns True if given interval is cover by a signle interval in this object.",
                     py::arg("key"));
    py_dis_intvs.def("items", &c_dis_intvs::py_item_iterator, py::keep_alive<0, 1>(),
                     "Iterates through intervals and values.");
    py_dis_intvs.def("intervals", &c_dis_intvs::py_intv_iterator, py::keep_alive<0, 1>(),
                     "Iterates through intervals.");
    py_dis_intvs.def("values", &c_dis_intvs::py_val_iterator, py::keep_alive<0, 1>(),
                     "Iterates through values.");
    py_dis_intvs.def("overlap_items", &c_dis_intvs::py_ovl_item_iterator, py::keep_alive<0, 1>(),
                     "Iterates through overlapping intervals and values.", py::arg("key"));
    py_dis_intvs.def("overlap_intervals", &c_dis_intvs::py_ovl_intv_iterator,
                     py::keep_alive<0, 1>(), "Iterates through overlapping intervals.",
                     py::arg("key"));
    py_dis_intvs.def("overlap_values", &c_dis_intvs::py_ovl_val_iterator, py::keep_alive<0, 1>(),
                     "Iterates through overlapping values.", py::arg("key"));
    py_dis_intvs.def("get_first_overlap_item", &c_dis_intvs::get_first_overlap_item,
                     "Returns the first overlap interval and value.", py::arg("key"));
    py_dis_intvs.def("get_copy", &c_dis_intvs::get_copy, "Returns a copy of this object.");
    py_dis_intvs.def("get_intersection", &c_dis_intvs::get_intersection,
                     "Returns the intersection.", py::arg("other"));
    py_dis_intvs.def("get_complement", &c_dis_intvs::get_complement<c_intv_type>,
                     "Returns the complement.", py::arg("total_intv"));
    py_dis_intvs.def("get_transform", &c_dis_intvs::get_transform,
                     "Returns the transformed intervals.", py::arg("scale") = 1,
                     py::arg("shift") = 0);
    py_dis_intvs.def("remove", &c_dis_intvs::remove<c_intv_type>, "Removes the given interval.",
                     py::arg("key"));
    py_dis_intvs.def("remove_overlaps", &c_dis_intvs::remove_overlaps<c_intv_type>,
                     "Removes overlapping intervals.", py::arg("key"));
    py_dis_intvs.def("add", &c_dis_intvs::add, "add the given interval.", py::arg("intv"),
                     py::arg("val") = py::none(), py::arg("merge") = false,
                     py::arg("abut") = false);
    py_dis_intvs.def("subtract", &c_dis_intvs::subtract<c_intv_type>,
                     "Subtracts the given interval.", py::arg("key"));
}
