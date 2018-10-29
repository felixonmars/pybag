
#include <map>
#include <string>
#include <variant>

#include <pybind11/pybind11.h>

#include <cbag/schematic/instance.h>

#include <pybag/base/schematic.h>

namespace pybag {
namespace schematic {

using inst_iter_t = std::map<std::string, cbag::sch::instance>::iterator;

class sch_inst_ref {
  private:
    inst_iter_t iter_;

  public:
    sch_inst_ref(inst_iter_t iter) : iter_(std::move(iter)) {}

    std::string lib_name() const { return iter_->second.lib_name; }
    std::string cell_name() const { return iter_->second.cell_name; }
    std::string name() const { return iter_->first; }
    uint32_t width() const { return iter_->second.width(); }
    uint32_t height() const { return iter_->second.height(); }
    bool is_primitive() const { return iter_->second.is_primitive; }

    void set_lib_name(std::string val) { iter_->second.lib_name = std::move(val); }
    void set_cell_name(std::string val) { iter_->second.cell_name = std::move(val); }
    void set_is_primitive(bool val) { iter_->second.is_primitive = val; }
    void set_param(std::string key, const std::variant<int32_t, double, bool, std::string> &val) {
        iter_->second.set_param(std::move(key), val);
    }
    void reset() {
        iter_->second.clear_params();
        iter_->second.connections.clear();
    }
    void update_connection(std::string term_name, std::string net_name) {
        iter_->second.update_connection(name(), std::move(term_name), std::move(net_name));
    }
};

} // namespace schematic
} // namespace pybag

void bind_schematic(py::module &m_top) {
    py::module m = m_top.def_submodule("schematic");

    m.doc() = "This module contains various classes for schematic manipulation.";
}
