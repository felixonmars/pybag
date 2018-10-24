#include <memory>
#include <utility>

#include <cbag/util/interval.h>

#include <pybag/base/util/interval.h>

namespace cu = cbag::util;

namespace pybag {
namespace util {

using py_interval_base = cu::interval;

class py_interval : public py_interval_base {
  public:
    py::object value;

    using py_interval_base::py_interval_base;

    py_interval(coord_type first, coord_type second, const py::object &obj)
        : py_interval_base(first, second), value(obj) {}
};

using dis_intvs_val_base = cu::disjoint_intvs<py_interval>;

class dis_intvs_val : public dis_intvs_val_base {
  public:
    using dis_intvs_val_base::dis_intvs_val_base;

    py::iterator py_intv_iterator() const { return py::make_iterator(intv_begin(), intv_end()); }
    bool add(py::tuple intv, py::object val = py::none(), bool merge = false, bool abut = false) {
        return emplace(merge, abut, intv[0].cast<py::int_>(), intv[1].cast<py::int_>(), val);
    }
};

} // namespace util
} // namespace pybag

namespace pu = pybag::util;

using c_dis_intvs = pu::dis_intvs_val;
using c_intv_type = pu::py_interval::intv_type;

void bind_util_interval(py::module &m_util) {
    py::module m = m_util.def_submodule("interval");
    m.doc() = "This module contains utility classes for manipulating intervals.";

    // add interval class
    auto py_dis_intvs = py::class_<c_dis_intvs>(m, "PyDisjointIntervals");
    py_dis_intvs.doc() = R"mydelim(A class that keeps track of disjoint intervals.

Parameters
----------
intv_list : Optional[List[Tuple[int, int]]]
    list of intervals.
val_list : Optional[List[Any]]
    list of values.
)mydelim";

    py_dis_intvs.def(py::init<>());
    py_dis_intvs.def("__contains__", &c_dis_intvs::contains<c_intv_type>,
                     "Returns True if given interval is in this object.", py::arg("key)"));
    py_dis_intvs.def("__iter__", &c_dis_intvs::py_intv_iterator, py::keep_alive<0, 1>(),
                     "Iterates through all intervals.");
    py_dis_intvs.def("__len__", &c_dis_intvs::size, "Returns number of intervals.");
    py_dis_intvs.def("get_start", &c_dis_intvs::start,
                     "int: the start coordinate of first interval.");
    py_dis_intvs.def("get_end", &c_dis_intvs::stop, "int: the stop coordinate of last interval.");
    py_dis_intvs.def("get_copy", &c_dis_intvs::get_copy, "Returns a copy of this object.");
    py_dis_intvs.def("has_overlap", &c_dis_intvs::overlaps<c_intv_type>,
                     "Returns True if given interval overlaps this object.", py::arg("key"));
    py_dis_intvs.def("has_single_cover", &c_dis_intvs::covers<c_intv_type>,
                     "Returns True if given interval is cover by a signle interval in this object.",
                     py::arg("key"));
    py_dis_intvs.def("remove", &c_dis_intvs::remove<c_intv_type>, "Removes the given interval.",
                     py::arg("key"));
    py_dis_intvs.def("get_intersection", &c_dis_intvs::get_intersect, "Returns the intersection.",
                     py::arg("other"));
    py_dis_intvs.def("get_complement", &c_dis_intvs::get_complement<c_intv_type>,
                     "Returns the complement.", py::arg("total_intv"));
    py_dis_intvs.def("remove_all_overlaps", &c_dis_intvs::remove_overlaps<c_intv_type>,
                     "Remove all overlapping intervals.", py::arg("key"));
    py_dis_intvs.def("add", &c_dis_intvs::add, "add the given interval.", py::arg("intv"),
                     py::arg("val") = py::none(), py::arg("merge") = false,
                     py::arg("abut") = false);
    py_dis_intvs.def("subtract", &c_dis_intvs::subtract<c_intv_type>,
                     "Subtracts the given interval.", py::arg("key"));
}
