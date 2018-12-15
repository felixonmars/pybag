#ifndef PYBAG_ORIENT_CONV_H
#define PYBAG_ORIENT_CONV_H

#include <cstdint>

#include <pybind11/pybind11.h>

#include <cbag/common/orientation.h>

namespace py = pybind11;

constexpr auto code_R0 = static_cast<uint32_t>(cbag::oR0);

namespace pybag {
namespace util {

uint32_t get_orient_code(const py::str &orient);

py::object code_to_orient(uint32_t code);

} // namespace util
} // namespace pybag

#endif
