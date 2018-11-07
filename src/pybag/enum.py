# -*- coding: utf-8 -*-

"""This package contains various enums used by the C++ extension."""

from enum import IntEnum


class TermType(IntEnum):
    input = 0
    output = 1
    inout = 2


class Orientation(IntEnum):
    R0 = 0
    MY = 1
    MX = 2
    R180 = 3
    MXR90 = 4
    R270 = 5
    R90 = 6
    MYR90 = 7
