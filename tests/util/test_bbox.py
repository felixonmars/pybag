# -*- coding: utf-8 -*-

import pytest

from pybag.util.geometry import BBox
from pybag.enum import Orientation

# test data for PyDisjointIntervals
bbox_data = [
    (0, 0, 0, 0, False, True),
    (0, 0, -1, -1, False, False),
    (2, 4, 2, 7, False, True),
    (2, 4, 2, -1, False, False),
    (1, 4, 4, 4, False, True),
    (1, 4, 4, 3, False, False),
    (-2, -3, 6, 12, True, True),
    (0, 0, 3, 5, True, True),
]

transform_data = [
    ((1, 1, 3, 6), 2, -4, "R0", (3, -3, 5, 2)),
    ((0, 0, 3, 6), 0, 0, "R90", (-6, 0, 0, 3)),
    ((0, 0, 3, 6), 1, 1, "R90", (-5, 1, 1, 4)),
]


def test_invalid_bbox():
    ans = BBox.get_invalid_bbox()
    assert ans.is_valid() == False


@pytest.mark.parametrize("xl, yl, xh, yh, physical, valid", bbox_data)
def test_properties(xl, yl, xh, yh, physical, valid):
    ans = BBox(xl, yl, xh, yh)
    assert ans.xl == xl
    assert ans.left_unit == xl
    assert ans.yl == yl
    assert ans.bottom_unit == yl
    assert ans.xh == xh
    assert ans.right_unit == xh
    assert ans.yh == yh
    assert ans.top_unit == yh
    assert ans.xm == (xl + xh) // 2
    assert ans.xc_unit == (xl + xh) // 2
    assert ans.ym == (yl + yh) // 2
    assert ans.yc_unit == (yl + yh) // 2
    assert ans.w == xh - xl
    assert ans.width_unit == xh - xl
    assert ans.h == yh - yl
    assert ans.height_unit == yh - yl
    assert ans.get_immutable_key() == (xl, yl, xh, yh)


@pytest.mark.parametrize("xl, yl, xh, yh, physical, valid", bbox_data)
def test_physical_valid(xl, yl, xh, yh, physical, valid):
    ans = BBox(xl, yl, xh, yh)
    assert ans.is_physical() == physical
    assert ans.is_valid() == valid


@pytest.mark.parametrize("box0, dx, dy, orient, box1", transform_data)
def test_transform(box0, dx, dy, orient, box1):
    a = BBox(box0[0], box0[1], box0[2], box0[3])
    b = a.transform(dx, dy, Orientation[orient])
    c = a.transform((dx, dy), orient)
    ans = BBox(box1[0], box1[1], box1[2], box1[3])
    assert b == ans
    assert c == ans
    if orient == 'R0':
        assert a.transform((dx, dy)) == ans
    if dx == 0 and dy == 0:
        assert a.transform(orient=orient) == ans


def test_merge_invalid():
    a = BBox(0, 0, 2, 3)
    b = BBox(100, 103, 200, 102)

    assert a.merge(b) == a
    assert b.merge(a) == a
