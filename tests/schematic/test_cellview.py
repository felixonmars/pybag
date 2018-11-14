# -*- coding: utf-8 -*-

import os

import pkg_resources

from pybag.schematic import PySchCellView


def build_cv(fname):
    yaml_file = pkg_resources.resource_filename(__name__, os.path.join('data', fname))

    return PySchCellView(yaml_file)


def test_constructor():
    """Check PySchCellView is constructed properly."""
    fname = 'inv.yaml'

    cv = build_cv(fname)
    assert list(cv.terminals()) == [('VDD', 0), ('VSS', 0), ('in', 0), ('out', 1)]


def test_array_instance():
    """Check array_instance works."""
    fname = 'inv.yaml'

    cv = build_cv(fname)

    conn1 = [('B', 'foo'), ('D', 'bar')]
    conn2 = [('G', 'baz'), ('S', 'ok')]

    name_conn_range = [('XFOO1', conn1), ('XFOO2', iter(conn2))]

    cv.array_instance('XP', 0, 0, name_conn_range)

    # make sure old instance is deleted
    assert cv.get_inst_ref('XP') is None
    inst1 = cv.get_inst_ref('XFOO1')
    inst2 = cv.get_inst_ref('XFOO2')
    # make sure new instance exists
    assert inst1 is not None
    assert inst2 is not None

    # make sure new instance properties are correct
    for inst, conn in ((inst1, conn1), (inst2, conn2)):
        assert inst.lib_name == 'BAG_prim'
        assert inst.cell_name == 'pmos4_standard'
        assert inst.is_primitive
        for term, net in conn:
            assert inst.get_connection(term) == net


def test_to_yaml():
    """Check yaml conversion method works."""
    fname = 'inv.yaml'

    cv = build_cv(fname)

    content = cv.to_yaml()
    yaml_file = pkg_resources.resource_filename(__name__, os.path.join('data', fname))
    with open(yaml_file, 'r') as f:
        file_content = f.read()

    assert content == file_content
