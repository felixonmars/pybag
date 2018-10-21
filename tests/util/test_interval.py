# -*- coding: utf-8 -*-

import sys

import pytest

from pybag.base.util.interval import PyDisjointIntervals

# test data for PyDisjointIntervals
intvs_vals_data = [
    ([], []),
    ([(1, 2), (3, 5)], ['A', 'B']),
    ([(1, 2), (4, 5), (6, 8)], [1, 2, 3]),
]


@pytest.mark.parametrize("intvs,vals", intvs_vals_data)
def test_constructor_refcount(intvs, vals):
    """Check Cython increment reference count of value objects."""
    bc_list = [sys.getrefcount(v) for v in vals]
    # keep reference to PyDisjointInterval to avoid garbage collection
    # noinspection PyUnusedLocal
    dis_intvs = PyDisjointIntervals(intv_list=intvs, val_list=vals)
    ac_list = [sys.getrefcount(v) for v in vals]
    for bc, ac in zip(bc_list, ac_list):
        assert ac == bc + 1


@pytest.mark.parametrize("intvs,vals", intvs_vals_data)
def test_destructor_refcount(intvs, vals):
    """Check Cython increment reference count of value objects."""
    dis_intvs = PyDisjointIntervals(intv_list=intvs, val_list=vals)
    bc_list = [sys.getrefcount(v) for v in vals]
    del dis_intvs
    ac_list = [sys.getrefcount(v) for v in vals]
    for bc, ac in zip(bc_list, ac_list):
        assert ac == bc - 1


@pytest.mark.parametrize("intvs,vals", intvs_vals_data)
def test_contains(intvs, vals):
    dis_intvs = PyDisjointIntervals(intv_list=intvs, val_list=vals)
    for intv in intvs:
        assert intv in dis_intvs
        assert (intv[0], intv[1] + 1) not in dis_intvs


@pytest.mark.parametrize("intvs,vals", intvs_vals_data)
def test_iter(intvs, vals):
    dis_intvs = PyDisjointIntervals(intv_list=intvs, val_list=vals)
    for a, b in zip(intvs, dis_intvs):
        assert a == b


@pytest.mark.parametrize("intvs,vals", intvs_vals_data)
def test_len(intvs, vals):
    dis_intvs = PyDisjointIntervals(intv_list=intvs, val_list=vals)
    assert len(intvs) == len(dis_intvs)


@pytest.mark.parametrize("intvs,vals", intvs_vals_data)
def test_get_start(intvs, vals):
    dis_intvs = PyDisjointIntervals(intv_list=intvs, val_list=vals)
    if intvs:
        assert intvs[0][0] == dis_intvs.get_start()
    else:
        with pytest.raises(IndexError):
            dis_intvs.get_start()


@pytest.mark.parametrize("intvs,vals", intvs_vals_data)
def test_get_end(intvs, vals):
    dis_intvs = PyDisjointIntervals(intv_list=intvs, val_list=vals)
    if intvs:
        assert intvs[-1][-1] == dis_intvs.get_end()
    else:
        with pytest.raises(IndexError):
            dis_intvs.get_end()
