
#include <pybag/util/module.h>

void bind_util(py::module &m_top) {
    py::module m = m_top.def_submodule("util");

    m.doc() = "This package contains various utility classes.";

    bind_util_interval(m);
}
