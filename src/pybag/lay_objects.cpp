#include <boost/polygon/polygon.hpp>

#include <pybind11/pybind11.h>

#include <cbag/common/box_t.h>
#include <cbag/layout/blockage.h>
#include <cbag/layout/boundary.h>
#include <cbag/layout/cv_obj_ref.h>
#include <cbag/layout/polygon45_fwd.h>
#include <cbag/layout/polygon45_set_fwd.h>
#include <cbag/layout/polygon90_fwd.h>
#include <cbag/layout/polygon_fwd.h>
#include <cbag/layout/via.h>

#include <pybag/lay_objects.h>

template <class Shape> decltype(auto) bind_shape_ref(py::module &m, const char *name) {
    auto py_cls = py::class_<cbag::layout::shape_ref<Shape>>(m, name);
    py_cls.def(py::init<>(), "Create an empty layout object.");
    py_cls.def("commint", &cbag::layout::shape_ref<Shape>::commit,
               "Commit this object to cellview.");
    return py_cls;
}

template <class Object> decltype(auto) bind_cv_obj_ref(py::module &m, const char *name) {
    auto py_cls = py::class_<cbag::layout::cv_obj_ref<Object>>(m, name);
    py_cls.def(py::init<>(), "Create an empty layout object.");
    py_cls.def("commint", &cbag::layout::cv_obj_ref<Object>::commit,
               "Commit this object to cellview.");
    return py_cls;
}

cbag::box_t via_bot_box(const cbag::layout::cv_obj_ref<cbag::layout::via> &ref) {
    return ref.obj.bot_box();
}

cbag::box_t via_top_box(const cbag::layout::cv_obj_ref<cbag::layout::via> &ref) {
    return ref.obj.top_box();
}

void bind_lay_objects(py::module &m) {
    bind_shape_ref<cbag::box_t>(m, "PyRect");
    bind_shape_ref<cbag::layout::polygon90>(m, "PyPolygon90");
    bind_shape_ref<cbag::layout::polygon45>(m, "PyPolygon45");
    bind_shape_ref<cbag::layout::polygon>(m, "PyPolygon");
    bind_shape_ref<cbag::layout::polygon45_set>(m, "PyPath");

    bind_cv_obj_ref<cbag::layout::blockage>(m, "PyBlockage");
    bind_cv_obj_ref<cbag::layout::boundary>(m, "PyBoundary");
    auto py_cls = bind_cv_obj_ref<cbag::layout::via>(m, "PyVia");
    py_cls.def_property_readonly("bottom_box", &via_bot_box,
                                 "Returns the via bottom bounding box.");
    py_cls.def_property_readonly("top_box", &via_top_box, "Returns the via top bounding box.");
}