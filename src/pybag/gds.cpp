#include <pybind11/pybind11.h>

#include <pybind11_generics/iterable.h>

#include <cbag/gdsii/write.h>
#include <cbag/layout/cellview.h>

#include <pybag/gds.h>

namespace py = pybind11;
namespace pyg = pybind11_generics;

using c_lay_cv_info = std::pair<std::string, cbag::layout::cellview *>;

void bind_gds(py::module &m) {
    m.def("implement_gds", &cbag::gdsii::implement_gds<pyg::Iterable<c_lay_cv_info>>,
          "Write the given layouts to GDS.", py::arg("fname"), py::arg("lib_name"),
          py::arg("layer_map"), py::arg("obj_map"), py::arg("resolution"), py::arg("user_unit"),
          py::arg("cv_list"));
}
