#ifndef PYBAG_PY_PT_VECTOR_H
#define PYBAG_PY_PT_VECTOR_H

#include <pybind11_generics/list.h>
#include <pybind11_generics/tuple.h>

#include <boost/polygon/polygon.hpp>

#include <cbag/common/typedefs.h>
#include <cbag/layout/pt_list.h>

using py_point = pyg::Tuple<cbag::coord_t, cbag::coord_t>;
using py_pt_vector = pyg::List<py_point>;

namespace boost {
namespace polygon {
template <> struct geometry_concept<py_point> { using type = point_concept; };

template <> struct point_traits<py_point> {
    using point_type = py_point;
    using coordinate_type = cbag::coord_t;

    static coordinate_type get(const point_type &point, orientation_2d orient) {
        return point[orient.to_int()].cast<coordinate_type>();
    }
};
} // namespace polygon
} // namespace boost

namespace cbag {
namespace layout {
namespace traits {

template <> struct pt_list<py_pt_vector> {
    using coordinate_type = coord_t;

    static std::size_t size(const py_pt_vector &vec) { return vec.size(); }
    static coordinate_type x(const py_pt_vector &vec, std::size_t idx) { return vec[idx].get<0>(); }
    static coordinate_type y(const py_pt_vector &vec, std::size_t idx) { return vec[idx].get<1>(); }
    static auto begin(const py_pt_vector &vec) -> decltype(vec.begin()) { return vec.begin(); }
    static auto end(const py_pt_vector &vec) -> decltype(vec.end()) { return vec.end(); }
};

} // namespace traits
} // namespace layout
} // namespace cbag

#endif
