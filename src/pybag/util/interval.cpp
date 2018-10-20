
#include <pybag/util/interval.h>
#include <cbag/util/interval.h>

namespace cu = cbag::util;

void bind_util_interval(py::module &m_util) {
    py::module m = m_util.def_submodule("interval");

    m.doc() = "This module contains utility classes for manipulating intervals.";

    py::class_<cu::disjoint_intvs<>>(m, "PyDisjointIntervals")
            .def(py::init<>());
}
