#include <pybind11/pybind11.h>

#include <cbag/layout/cv_obj_ref.h>
#include <cbag/layout/instance.h>

#include <pybag/layout.h>

using c_instance = cbag::layout::instance;
using c_inst_ref = cbag::layout::cv_obj_ref<c_instance>;

namespace pybag {
namespace lay {

void check_ref(const c_inst_ref &ref) {
    if (ref.parent == nullptr) {
        throw std::runtime_error("Cannot modify committed instance");
    }
}

uint32_t get_nx(const c_inst_ref &ref) { return ref.obj.nx; }
uint32_t get_ny(const c_inst_ref &ref) { return ref.obj.ny; }
cbag::offset_t get_spx(const c_inst_ref &ref) { return ref.obj.spx; }
cbag::offset_t get_spy(const c_inst_ref &ref) { return ref.obj.spy; }
const cbag::transformation &get_xform(const c_inst_ref &ref) { return ref.obj.xform; }

void set_nx(c_inst_ref &ref, uint32_t val) {
    check_ref(ref);
    ref.obj.nx = val;
}
void set_ny(c_inst_ref &ref, uint32_t val) {
    check_ref(ref);
    ref.obj.ny = val;
}
void set_spx(c_inst_ref &ref, cbag::offset_t val) {
    check_ref(ref);
    ref.obj.spx = val;
}
void set_spy(c_inst_ref &ref, cbag::offset_t val) {
    check_ref(ref);
    ref.obj.spy = val;
}

void move_by(c_inst_ref &ref, cbag::offset_t dx, cbag::offset_t dy) {
    check_ref(ref);
    ref.obj.xform.move_by(dx, dy);
}

void transform(c_inst_ref &ref, const cbag::transformation &xform) {
    check_ref(ref);
    ref.obj.xform.transform_by(xform);
}

void set_master(c_inst_ref &ref, const cbag::layout::cellview *new_master) {
    check_ref(ref);
    ref.obj.set_master(new_master);
}

} // namespace lay
} // namespace pybag

namespace pl = pybag::lay;

void bind_inst_ref(py::module &m) {

    auto py_cls = py::class_<c_inst_ref>(m, "PyLayInstRef");
    py_cls.doc() = "A reference to a layout instance inside a cellview.";
    py_cls.def_property("nx", &pl::get_nx, &pl::set_nx, "Number of columns.");
    py_cls.def_property("ny", &pl::get_ny, &pl::set_ny, "Number of rows.");
    py_cls.def_property("spx", &pl::get_spx, &pl::set_spx, "Column pitch.");
    py_cls.def_property("spy", &pl::get_spy, &pl::set_spy, "Row pitch.");
    py_cls.def_property_readonly("xform", &pl::get_xform, "The Transform object.");
    py_cls.def("move_by", &pl::move_by, "Moves the instance.", py::arg("dx"), py::arg("dy"));
    py_cls.def("transform", &pl::transform, "Transforms the instance.", py::arg("xform"));
    py_cls.def("set_master", &pl::set_master, "Sets the instance master.", py::arg("new_master"));
    py_cls.def("commit", &c_inst_ref::commit, "Commits the instance object.");
}

void bind_layout(py::module &m) { bind_inst_ref(m); }
