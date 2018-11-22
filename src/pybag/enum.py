# -*- coding: utf-8 -*-

"""This package contains various enums used by the C++ extension."""

from enum import IntEnum


class DesignOutput(IntEnum):
    LAYOUT = 0
    GDS = 1
    SCHEMATIC = 2
    YAML = 3
    CDL = 4
    VERILOG = 5


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


class Orient2D(IntEnum):
    x = 0
    y = 1


class PathStyle(IntEnum):
    truncate = 0
    extend = 1
    round = 2
    triangle = 3


class BlockageType(IntEnum):
    routing = 0
    via = 1
    placement = 2
    wiring = 3
    fill = 4
    slot = 5
    pin = 6
    feed_thru = 7
    screen = 8


class BoundaryType(IntEnum):
    PR = 0
    snap = 1


class GeometryMode(IntEnum):
    POLY_90 = 0
    POLY_45 = 1
    POLY = 2


class SpaceQueryMode(IntEnum):
    LINE_END = 0
    DIFF_COLOR = 1
    SAME_COLOR = 2
