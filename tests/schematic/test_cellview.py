# -*- coding: utf-8 -*-

import os

import pkg_resources

import pytest

from pybag.schematic import PySchCellView

cv_files = [
    'sense_amp_strongarm.yaml',
]


def build_cv(fname):
    yaml_file = pkg_resources.resource_filename(__name__, os.path.join('data', fname))

    return PySchCellView(yaml_file)


@pytest.mark.parametrize("fname", cv_files)
def test_constructor(fname):
    """Check PySchCellView is constructed properly."""

    # TODO: update yaml file
    """
    cv = build_cv(fname)

    assert list(cv.in_terms()) == ['clk', 'inn', 'inp']
    """

@pytest.mark.parametrize("fname", cv_files)
def test_array_instance(fname):
    """Check array_instance works."""

    # TODO: update yaml file
    """
    cv = build_cv(fname)

    conn1 = [('B', 'foo'), ('D', 'bar')]
    conn2 = [('G', 'baz'), ('S', 'ok')]

    name_conn_range = [('XFOO1', conn1), ('XFOO2', iter(conn2))]

    cv.array_instance('XRMR', 0, 0, name_conn_range)

    # make sure old instance is deleted
    assert cv.get_inst_ref('XRMR') is None
    inst1 = cv.get_inst_ref('XFOO1')
    inst2 = cv.get_inst_ref('XFOO2')
    # make sure new instance exists
    assert inst1 is not None
    assert inst2 is not None

    # make sure new instance properties are correct
    assert inst1.name == 'XFOO1'
    assert inst2.name == 'XFOO2'
    for i in (inst1, inst2):
        assert i.lib_name == 'BAG_prim'
        assert i.cell_name == 'pmos4_standard'
        assert not i.is_primitive
    """
