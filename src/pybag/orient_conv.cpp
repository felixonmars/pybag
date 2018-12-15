#include <pybag/orient_conv.h>

namespace pybag {
namespace util {

uint32_t get_orient_code(const py::str &orient) {
    // use Python enum class to convert string to int
    py::object orient_cls = py::module::import("pybag.enum").attr("Orientation");
    return orient_cls.attr("__getitem__")(orient).template cast<uint32_t>();
}

py::object code_to_orient(uint32_t code) {
    py::object orient_cls = py::module::import("pybag.enum").attr("Orientation");
    return orient_cls(code);
}

} // namespace util
} // namespace pybag
