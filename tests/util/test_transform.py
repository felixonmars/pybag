# -*- coding: utf-8 -*-

import pytest

from pybag.core import Transform
from pybag.enum import Orientation

# test data
data = [
    (0, 0, Orientation.R0),
    (1, -2, Orientation.MX),
    (-3, 10, Orientation.MY),
    (-17, -21, Orientation.R180),
    (0, 3, Orientation.MXR90),
]

def test_default1():
    xform = Transform()
    assert xform.x == 0
    assert xform.y == 0
    assert xform.orient is Orientation.R0


def test_default2():
    xform = Transform(3)
    assert xform.x == 3
    assert xform.y == 0
    assert xform.orient is Orientation.R0


def test_default3():
    xform = Transform(3, -4)
    assert xform.x == 3
    assert xform.y == -4
    assert xform.orient is Orientation.R0


@pytest.mark.parametrize("dx, dy, mode", data)
def test_constructor(dx, dy, mode):
    xform = Transform(dx, dy, mode)
    assert xform.x == dx
    assert xform.y == dy
    assert xform.orient is mode
