#include <memory>

#include <cbag/util/interval.h>

#include <pybag/base/util/interval.h>

namespace cu = cbag::util;

namespace pybag {
namespace util {

using py_interval_base = cu::interval;

class py_interval : py_interval_base {
  public:
    py::object value;

    using py_interval_base::py_interval_base;

    py_interval(coord_type first, coord_type second, const py::object &obj)
        : py_interval_base(first, second), value(obj) {}
};

} // namespace util
} // namespace pybag

namespace pu = pybag::util;

using c_dis_intvs = cu::disjoint_intvs<pu::py_interval>;

void bind_util_interval(py::module &m_util) {
    py::module m = m_util.def_submodule("interval");
    m.doc() = "This module contains utility classes for manipulating intervals.";

    auto py_dis_intvs = py::class_<c_dis_intvs>(m, "PyDisjointIntervals");
    py_dis_intvs.doc() = R"mydelim(A class that keeps track of disjoint intervals.

Parameters
----------
intv_list : Optional[List[Tuple[int, int]]]
    list of intervals.
val_list : Optional[List[Any]]
    list of values.
)mydelim";

    py::options options;
    options.disable_function_signatures();

    const char *init_doc = "__init__(self, "
                           "intv_list: Optional[List[Tuple[int, int]]]=None, "
                           "val_list: Optional[List[Any]]=None) -> None";
    py_dis_intvs.def(py::init([](py::list intv_list, py::list val_list) {
                         auto ans = std::make_unique<c_dis_intvs>();
                         return ans;
                     }),
                     init_doc, py::arg_v("intv_list", nullptr).none(true),
                     py::arg_v("val_list", nullptr).none(true));

    options.enable_function_signatures();
}
