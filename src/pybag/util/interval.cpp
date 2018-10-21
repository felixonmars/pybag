
#include <pybind11/pybind11.h>

#include <cbag/util/interval.h>

namespace py = pybind11;
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

PYBIND11_MODULE(interval, m) {
    m.doc() = "This module contains classes for manipulating intervals.";

    py::class_<cu::disjoint_intvs<pu::py_interval>>(m, "PyDisjointIntervals").def(py::init<>());
}
