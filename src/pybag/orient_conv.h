#ifndef PYBAG_ORIENT_CONV_H
#define PYBAG_ORIENT_CONV_H

#include <cstdint>

#include <pybind11/pybind11.h>

#include <cbag/enum/orientation.h>

namespace py = pybind11;

constexpr auto code_R0 = static_cast<cbag::orient_t>(cbag::oR0);

namespace pybag {
namespace util {

cbag::orient_t get_orient_code(const py::str &orient);

py::object code_to_orient(cbag::orient_t code);

} // namespace util
} // namespace pybag

#endif
