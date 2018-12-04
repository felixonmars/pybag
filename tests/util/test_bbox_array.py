# -*- coding: utf-8 -*-

import pytest

from pybag.core import BBox, BBoxArray
from pybag.enum import Orientation

transform_data = [
    (((0, 0, 3, 6), 2, 3, 7, 12), 2, 4, "R0", ((2, 4, 5, 10), 2, 3, 7, 12)),
    (((0, 0, 3, 6), 2, 3, 7, 12), 0, 0, "R90", ((-6, 0, 0, 3), 3, 2, -12, 7)),
    (((0, 0, 3, 6), 2, 3, 7, 12), 2, 4, "R90", ((-4, 4, 2, 7), 3, 2, -12, 7)),
]

def make_bbox_array(barr_info):
    base = BBox(barr_info[0][0], barr_info[0][1], barr_info[0][2], barr_info[0][3])
    return BBoxArray(base, barr_info[1], barr_info[2], barr_info[3], barr_info[4])


@pytest.mark.parametrize("barr1_info, dx, dy, orient, barr2_info", transform_data)
def test_transform(barr1_info, dx, dy, orient, barr2_info):
    barr1 = make_bbox_array(barr1_info)
    barr2 = make_bbox_array(barr2_info)
    a = barr1.transform(dx, dy, Orientation[orient])
    b = barr1.transform((dx, dy), orient)
    assert a == barr2
    assert b == barr2
    if orient == 'R0':
        assert barr1.transform((dx, dy)) == barr2
    if dx == 0 and dy == 0:
        assert barr1.transform(orient=orient) == barr2
