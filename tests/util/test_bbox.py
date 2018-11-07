# -*- coding: utf-8 -*-

import pytest

from pybag.util.geometry import BBox

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

@pytest.mark.parametrize("xl, yl, xh, yh, physical, valid", bbox_data)
def test_physical_valid(xl, yl, xh, yh, physical, valid):
    ans = BBox(xl, yl, xh, yh)
    assert ans.is_physical() == physical
    assert ans.is_valid() == valid
