
#include <pybag/base/schematic.h>

namespace pybag {
namespace schematic {} // namespace schematic
} // namespace pybag

void bind_schematic(py::module &m_top) {
    py::module m = m_top.def_submodule("schematic");

    m.doc() = "This module contains various classes for schematic manipulation.";
}
