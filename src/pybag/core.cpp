#include <pybag/geometry.h>
#include <pybag/interval.h>
#include <pybag/lay_objects.h>
#include <pybag/oa.h>
#include <pybag/tech.h>

#include <pybind11/pybind11.h>

namespace py = pybind11;

PYBIND11_MODULE(core, m) {
    m.doc() = "Python wrappers for cbag library.";

    bind_interval(m);
    bind_geometry(m);
    bind_tech(m);
    bind_oa(m);
    bind_lay_objects(m);
}
