#include <cbag/util/interval.h>

#include <pybag/base/util/interval.h>

namespace cu = cbag::util;

namespace pybag {
namespace util {

using py_interval_base = cu::interval;

class py_interval : py_interval_base {
  public:
    py::object value;

    using py_interval_base::py_interval_base;

    py_interval(coord_type first, coord_type second, const py::object &obj)
        : py_interval_base(first, second), value(obj) {}
};

} // namespace util
} // namespace pybag

namespace pu = pybag::util;

void bind_util_interval(py::module &m_util) {
    py::module m = m_util.def_submodule("interval");
    m.doc() = "This module contains utility classes for manipulating intervals.";

    auto py_dis_intvs = py::class_<cu::disjoint_intvs<pu::py_interval>>(m, "PyDisjointIntervals");
    py_dis_intvs.doc() = "A class that keeps track of disjoint intervals.";

    py_dis_intvs.def(py::init<>());
}
