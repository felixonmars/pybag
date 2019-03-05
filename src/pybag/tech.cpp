#include <pybind11/pybind11.h>

#include <pybind11/stl.h>

#include <pybind11_generics/list.h>
#include <pybind11_generics/tuple.h>

#include <cbag/common/transformation.h>
#include <cbag/enum/space_type.h>
#include <cbag/layout/tech_util.h>
#include <cbag/layout/via_param_util.h>

#include <pybag/tech.h>

namespace py = pybind11;
namespace pyg = pybind11_generics;

using c_via_param = cbag::layout::via_param;
using c_tech = cbag::layout::tech;

namespace pybag {
namespace tech {

class PyTech : public c_tech {
  public:
    using c_tech::c_tech;

    cbag::em_specs_t get_metal_em_specs(const std::string &layer, const std::string &purpose,
                                        cbag::offset_t width, cbag::offset_t length, bool vertical,
                                        cbag::temp_t dc_temp, cbag::temp_t rms_dt) const override {
        PYBIND11_OVERLOAD_PURE(cbag::em_specs_t, cbag::layout::tech, get_metal_em_specs, layer,
                               purpose, width, length, vertical, dc_temp, rms_dt);
    }

    cbag::em_specs_t get_via_em_specs(cbag::enum_t layer_dir, const std::string &layer,
                                      const std::string &purpose, const std::string &adj_layer,
                                      const std::string &adj_purpose, cbag::offset_t cut_w,
                                      cbag::offset_t cut_h, cbag::offset_t m_w, cbag::offset_t m_l,
                                      cbag::offset_t adj_m_w, cbag::offset_t adj_m_l, bool array,
                                      cbag::temp_t dc_temp, cbag::temp_t rms_dt) const override {
        PYBIND11_OVERLOAD_PURE(cbag::em_specs_t, cbag::layout::tech, get_via_em_specs, layer_dir,
                               layer, purpose, adj_layer, adj_purpose, cut_w, cut_h, m_w, m_l,
                               adj_m_w, adj_m_l, array, dc_temp, rms_dt);
    }
};

using py_lp = pyg::Tuple<py::str, py::str>;

std::optional<cbag::level_t> get_level(const c_tech &tech, const std::string &layer,
                                       const std::string &purpose) {
    return tech.get_level(cbag::layout::layer_t_at(tech, layer, purpose));
}

pyg::List<py_lp> get_lay_purp_list(const c_tech &tech, cbag::level_t level) {
    pyg::List<py_lp> ans;
    const auto &lp_list = tech.get_lay_purp_list(level);
    for (const auto & [ lay_id, purp_id ] : lp_list) {
        ans.push_back(
            py_lp::make_tuple(tech.get_layer_name(lay_id), tech.get_purpose_name(purp_id)));
    }
    return ans;
}

std::string get_via_id(const c_tech &tech, cbag::enum_t lev_code, const std::string &lay,
                       const std::string &purp, const std::string &adj_lay,
                       const std::string &adj_purp) {
    return tech.get_via_id(static_cast<cbag::direction>(lev_code),
                           cbag::layout::layer_t_at(tech, lay, purp),
                           cbag::layout::layer_t_at(tech, adj_lay, adj_purp));
}

c_via_param get_via_param(const c_tech &tech, cbag::offset_t w, cbag::offset_t h,
                          const std::string &via_id, cbag::enum_t lev_code,
                          cbag::orient_2d_t ex_dir, cbag::orient_2d_t adj_ex_dir, bool extend) {

    return tech.get_via_param(cbag::vector{w, h}, via_id, static_cast<cbag::direction>(lev_code),
                              static_cast<cbag::orient_2d>(ex_dir),
                              static_cast<cbag::orient_2d>(adj_ex_dir), extend);
}

cbag::offset_t get_min_space(const c_tech &tech, const std::string &layer, cbag::offset_t width,
                             const std::string &purpose, bool same_color, bool even) {
    auto sp_type = cbag::get_space_type(same_color);
    return tech.get_min_space(cbag::layout::layer_t_at(tech, layer, purpose), width, sp_type, even);
}

cbag::offset_t get_min_le_space(const c_tech &tech, const std::string &layer, cbag::offset_t width,
                                const std::string &purpose, bool even) {
    return tech.get_min_space(cbag::layout::layer_t_at(tech, layer, purpose), width,
                              cbag::space_type::LINE_END, even);
}

} // namespace tech
} // namespace pybag

void bind_via_param(py::module &m) {
    auto py_cls = py::class_<c_via_param>(m, "ViaParam");
    py_cls.doc() = "The via parameter class.";
    py_cls.def(
        py::init<cbag::cnt_t, cbag::cnt_t, cbag::offset_t, cbag::offset_t, cbag::offset_t,
                 cbag::offset_t, cbag::offset_t, cbag::offset_t, cbag::offset_t, cbag::offset_t,
                 cbag::offset_t, cbag::offset_t, cbag::offset_t, cbag::offset_t>(),
        "Create a new ViaParam object.", py::arg("vnx"), py::arg("vny"), py::arg("w"), py::arg("h"),
        py::arg("vspx"), py::arg("vspy"), py::arg("enc1l"), py::arg("enc1r"), py::arg("enc1t"),
        py::arg("enc1b"), py::arg("enc2l"), py::arg("enc2r"), py::arg("enc2t"), py::arg("enc2b"));
    py_cls.def_property_readonly(
        "empty", [](const c_via_param &p) { return p.num[0] == 0 || p.num[1] == 0; },
        "True if this ViaParam represents an empty via.");
    py_cls.def_property_readonly("nx", [](const c_via_param &p) { return p.num[0]; },
                                 "Number of via columns.");
    py_cls.def_property_readonly("ny", [](const c_via_param &p) { return p.num[1]; },
                                 "Number of via rows.");
    py_cls.def_property_readonly(
        "cut_dim", [](const c_via_param &p) { return std::make_pair(p.cut_dim[0], p.cut_dim[1]); },
        "Via cut dimension.");
    py_cls.def("get_box",
               [](const c_via_param &p, const cbag::transformation &xform, cbag::level_t level) {
                   return cbag::layout::get_box(p, xform, static_cast<cbag::direction>(level));
               },
               "Computes the via metal BBox.", py::arg("xform"), py::arg("level"));
}

void bind_tech(py::module &m) {
    auto py_cls = py::class_<c_tech, pybag::tech::PyTech, std::shared_ptr<c_tech>>(m, "PyTech");
    py_cls.doc() = "A class that handles technology-specific information.";
    py_cls.def(py::init<std::string>(), "Create a new PyTech class from file.",
               py::arg("tech_fname"));

    py_cls.def_property_readonly("tech_lib", &c_tech::get_tech_lib, "The technology library name.");
    py_cls.def_property_readonly("layout_unit", &c_tech::get_layout_unit, "The layout unit.");
    py_cls.def_property_readonly("resolution", &c_tech::get_resolution, "The layout resolution.");
    py_cls.def_property_readonly("use_flip_parity", &c_tech::get_use_flip_parity,
                                 "True if flip_parity dictionary is used.");
    py_cls.def_property_readonly("default_purpose", &cbag::layout::get_default_purpose_name,
                                 "The default purpose name.");
    py_cls.def_property_readonly("pin_purpose", &cbag::layout::get_pin_purpose_name,
                                 "The pin purpose name.");
    py_cls.def_property_readonly("make_pin", &c_tech::get_make_pin, "True to make pin objects.");

    py_cls.def("get_layer_id", &pybag::tech::get_level, "Returns the layer level ID.",
               py::arg("layer"), py::arg("purpose") = "");
    py_cls.def("get_lay_purp_list", &pybag::tech::get_lay_purp_list,
               "Returns the layer/purpose pairs on the given layer level.", py::arg("layer_id"));
    py_cls.def("get_min_space", &pybag::tech::get_min_space,
               "Returns the minimum required spacing.", py::arg("layer"), py::arg("width"),
               py::arg("purpose") = "", py::arg("same_color") = false, py::arg("even") = false);
    py_cls.def("get_min_line_end_space", &pybag::tech::get_min_le_space,
               "Returns the minimum required spacing.", py::arg("layer"), py::arg("width"),
               py::arg("purpose") = "", py::arg("even") = false);
    py_cls.def("get_min_length",
               py::overload_cast<const c_tech &, const std::string &, const std::string &,
                                 cbag::offset_t, bool>(&cbag::layout::get_min_length),
               "Returns the minimum required length.", py::arg("layer"), py::arg("purpose"),
               py::arg("width"), py::arg("even") = false);
    py_cls.def("get_via_id", &pybag::tech::get_via_id, "Returns the via ID name.",
               py::arg("lev_code"), py::arg("lay"), py::arg("purp"), py::arg("adj_lay"),
               py::arg("adj_purp"));
    py_cls.def("get_via_param", &pybag::tech::get_via_param,
               "Calculates the via parameters from the given specs.", py::arg("w"), py::arg("h"),
               py::arg("via_id"), py::arg("lev_code"), py::arg("ex_dir"), py::arg("adj_ex_dir"),
               py::arg("extend"));
    py_cls.def("get_metal_em_specs", &c_tech::get_metal_em_specs,
               "Returns the EM specs for the given metal wire.", py::arg("layer"),
               py::arg("purpose"), py::arg("width"), py::arg("length") = -1,
               py::arg("vertical") = false, py::arg("dc_temp") = -1000, py::arg("rms_dt") = -1000);
    py_cls.def("get_via_em_specs", &c_tech::get_via_em_specs,
               "Returns the EM specs for the given via.", py::arg("layer_dir"), py::arg("layer"),
               py::arg("purpose"), py::arg("adj_layer"), py::arg("adj_purpose"), py::arg("cut_w"),
               py::arg("cut_h"), py::arg("m_w") = -1, py::arg("m_l") = -1, py::arg("adj_m_w") = -1,
               py::arg("adj_m_l") = -1, py::arg("array") = false, py::arg("dc_temp") = -1000,
               py::arg("rms_dt") = -1000);
}
