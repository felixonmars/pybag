
#include <pybag/module.h>

PYBIND11_MODULE(pybag, m) {
    m.doc() = "This package contains various python wrappers of the cbag library.";

    bind_util(m);
}
