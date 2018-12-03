#ifndef PYBAG_ORIENT_CONV_H
#define PYBAG_ORIENT_CONV_H

#include <cstdint>

#include <pybind11/pybind11.h>

#include <cbag/common/orientation.h>

namespace py = pybind11;

constexpr auto code_R0 = static_cast<uint32_t>(cbag::oR0);

namespace pybag {
namespace util {

// inline to get rid of multiple defintion warning
uint32_t inline get_orient_code(const py::str &orient) {
    // use Python enum class to convert string to int
    py::module enum_module = py::module::import("pybag.enum");
    py::object orient_enum = enum_module.attr("Orientation");
    return orient_enum.attr("__getitem__")(orient).template cast<uint32_t>();
}

} // namespace util
} // namespace pybag

#endif
