#include <pybag/orient_conv.h>

namespace pybag {
namespace util {

cbag::orient_t get_orient_code(const py::str &orient) {
    // use Python enum class to convert string to int
    py::object orient_cls = py::module::import("pybag.enum").attr("Orientation");
    return orient_cls.attr("__getitem__")(orient).template cast<cbag::orient_t>();
}

py::object code_to_orient(cbag::orient_t code) {
    py::object orient_cls = py::module::import("pybag.enum").attr("Orientation");
    return orient_cls(code);
}

py::object code_to_orient_2d(cbag::orient_2d_t code) {
    py::object orient_cls = py::module::import("pybag.enum").attr("Orient2D");
    return orient_cls(code);
}

} // namespace util
} // namespace pybag
