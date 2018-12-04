# -*- coding: utf-8 -*-

import sys

import pytest

from pybag.core import PyDisjointIntervals

# test data for PyDisjointIntervals
intv_vals_data = [
    [],
    [((1, 2), 'A'), ((3, 5), 'B')],
    [((1, 2), 1), ((4, 5), 2), ((6, 8), 3)],
]

ovl_data = [
    ([], (1, 2), None),
    ([((1, 2), 'A'), ((3, 5), 'B')], (3, 5), ((3, 5), 'B')),
    ([((1, 2), 1), ((4, 7), 2), ((10, 12), 3)], (6, 8), ((4, 7), 2)),
    ([((1, 2), 1), ((4, 7), 2), ((10, 12), 3)], (6, 11), ((4, 7), 2)),
]

def make_dis_intvs(intv_vals):
    ans = PyDisjointIntervals()
    for intv, val in intv_vals:
        ans.add(intv, val=val)
    return ans


@pytest.mark.parametrize("intv_vals", intv_vals_data)
def test_constructor_refcount(intv_vals):
    """Check class increment reference count of value objects."""
    bc_list = [sys.getrefcount(v) for _, v in intv_vals]
    # keep reference to PyDisjointInterval to avoid garbage collection
    # noinspection PyUnusedLocal
    dis_intvs = make_dis_intvs(intv_vals)
    ac_list = [sys.getrefcount(v) for _, v in intv_vals]
    for bc, ac in zip(bc_list, ac_list):
        assert ac == bc + 1


@pytest.mark.parametrize("intv_vals", intv_vals_data)
def test_destructor_refcount(intv_vals):
    """Check class decrement reference count of value objects."""
    dis_intvs = make_dis_intvs(intv_vals)
    bc_list = [sys.getrefcount(v) for _, v in intv_vals]
    del dis_intvs
    ac_list = [sys.getrefcount(v) for _, v in intv_vals]
    for bc, ac in zip(bc_list, ac_list):
        assert ac == bc - 1


@pytest.mark.parametrize("intv_vals", intv_vals_data)
def test_contains(intv_vals):
    dis_intvs = make_dis_intvs(intv_vals)
    for intv, _ in intv_vals:
        assert intv in dis_intvs
        assert (intv[0], intv[1] + 1) not in dis_intvs


@pytest.mark.parametrize("intv_vals", intv_vals_data)
def test_iter(intv_vals):
    dis_intvs = make_dis_intvs(intv_vals)
    for (a, _), b in zip(intv_vals, dis_intvs):
        assert a == b


@pytest.mark.parametrize("intv_vals", intv_vals_data)
def test_len(intv_vals):
    dis_intvs = make_dis_intvs(intv_vals)
    assert len(intv_vals) == len(dis_intvs)


@pytest.mark.parametrize("intv_vals", intv_vals_data)
def test_get_start(intv_vals):
    dis_intvs = make_dis_intvs(intv_vals)
    if intv_vals:
        assert intv_vals[0][0][0] == dis_intvs.start
    else:
        with pytest.raises(IndexError):
            dis_intvs.start


@pytest.mark.parametrize("intv_vals", intv_vals_data)
def test_get_end(intv_vals):
    dis_intvs = make_dis_intvs(intv_vals)
    if intv_vals:
        assert intv_vals[-1][0][1] == dis_intvs.stop
    else:
        with pytest.raises(IndexError):
            dis_intvs.stop


@pytest.mark.parametrize("intv_vals,key,item", ovl_data)
def test_get_first_overlap_item(intv_vals, key, item):
    """Check getting the first overlap item"""
    dis_intvs = make_dis_intvs(intv_vals)
    assert dis_intvs.get_first_overlap_item(key) == item
