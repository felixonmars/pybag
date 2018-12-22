# -*- coding: utf-8 -*-

import pytest

from pybag.core import BBox, BBoxArray, Transform
from pybag.enum import Orientation, Orient2D

arr_data = [
    ((2, 4, 5, 10), 2, 3, 7, 12),
    ((-6, 0, 0, 3), 3, 2, -12, 7),
    ((-4, 4, 2, 7), 3, 2, -12, 7),
]


transform_data = [
    (((0, 0, 3, 6), 2, 3, 7, 12), 2, 4, "R0", ((2, 4, 5, 10), 2, 3, 7, 12)),
    (((0, 0, 3, 6), 2, 3, 7, 12), 0, 0, "R90", ((-6, 0, 0, 3), 3, 2, -12, 7)),
    (((0, 0, 3, 6), 2, 3, 7, 12), 2, 4, "R90", ((-4, 4, 2, 7), 3, 2, -12, 7)),
]

def make_bbox_array(barr_info):
    base = BBox(barr_info[0][0], barr_info[0][1], barr_info[0][2], barr_info[0][3])
    return BBoxArray(base, barr_info[1], barr_info[2], barr_info[3], barr_info[4])


@pytest.mark.parametrize("box_data, nx, ny, spx, spy", arr_data)
def test_constructor(box_data, nx, ny, spx, spy):
    base = BBox(*box_data)
    ref = BBoxArray(base, nx, ny, spx, spy)
    assert ref.base == base
    assert ref.nx == nx
    assert ref.ny == ny
    assert ref.spx == spx
    assert ref.spy == spy

    a1 = BBoxArray(base, Orient2D.x, nx, spx, ny, spy)
    a2 = BBoxArray(base, Orient2D.y, ny, spy, nx, spx)
    assert a1 == ref
    assert a2 == ref


@pytest.mark.parametrize("barr1_info, dx, dy, orient, barr2_info", transform_data)
def test_transform(barr1_info, dx, dy, orient, barr2_info):
    barr1 = make_bbox_array(barr1_info)
    barr2 = make_bbox_array(barr2_info)
    xform = Transform(dx, dy, Orientation[orient])
    a = barr1.get_transform(xform)
    assert a == barr2
    assert a is not barr1

    b = barr1.transform(xform)
    assert barr1 == barr2
    assert b is barr1
