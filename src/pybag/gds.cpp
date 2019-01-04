#include <memory>

#include <pybind11/pybind11.h>

#include <pybind11_generics/iterable.h>
#include <pybind11_generics/list.h>

#include <cbag/gdsii/read.h>
#include <cbag/gdsii/write.h>
#include <cbag/layout/cellview.h>

#include <pybag/gds.h>

namespace py = pybind11;
namespace pyg = pybind11_generics;

using c_lay_cv = cbag::layout::cellview;
using c_lay_cv_info = std::pair<std::string, c_lay_cv *>;
using py_cv_list = pyg::List<std::unique_ptr<c_lay_cv>>;

namespace pybag {
namespace util {

py_cv_list read_gds(const std::string &fname, const std::string &layer_map,
                    const std::string &obj_map, const cbag::layout::tech *tech_ptr) {
    py_cv_list ans;

    cbag::gdsii::read_gds(fname, layer_map, obj_map, *tech_ptr, std::back_inserter(ans));

    return ans;
}

} // namespace util
} // namespace pybag

void bind_gds(py::module &m) {
    m.def("implement_gds", &cbag::gdsii::implement_gds<pyg::Iterable<c_lay_cv_info>>,
          "Write the given layouts to GDS.", py::arg("fname"), py::arg("lib_name"),
          py::arg("layer_map"), py::arg("obj_map"), py::arg("resolution"), py::arg("user_unit"),
          py::arg("cv_list"));

    m.def("read_gds", &pybag::util::read_gds, "Reads layout cellviews from the given GDS file.",
          py::arg("fname"), py::arg("layer_map"), py::arg("obj_map"), py::arg("tech_ptr"));
}
