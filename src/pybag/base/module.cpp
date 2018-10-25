#include <pybag/base/module.h>

PYBIND11_MODULE(base, m) {
    m.doc() = "This package contains various python wrappers of the cbag library.";

    bind_util(m);

    bind_schematic(m);
}
