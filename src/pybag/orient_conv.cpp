#include <pybag/orient_conv.h>

namespace pybag {
namespace util {

uint32_t get_orient_code(const py::str &orient) {
    // use Python enum class to convert string to int
    py::module enum_module = py::module::import("pybag.enum");
    py::object orient_enum = enum_module.attr("Orientation");
    return orient_enum.attr("__getitem__")(orient).template cast<uint32_t>();
}

} // namespace util
} // namespace pybag
