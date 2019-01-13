# -*- coding: utf-8 -*-

"""This package contains various enums used by the C++ extension."""

from __future__ import annotations

from enum import IntEnum


class DesignOutput(IntEnum):
    LAYOUT = 0
    GDS = 1
    SCHEMATIC = 2
    YAML = 3
    CDL = 4
    VERILOG = 5
    SYSVERILOG = 6


class TermType(IntEnum):
    input = 0
    output = 1
    inout = 2


class RoundMode(IntEnum):
    LESS = -2
    LESS_EQ = -1
    NEAREST = 0
    GREATER_EQ = 1
    GREATER = 2
    NONE = 3


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

    def perpendicular(self) -> Orient2D:
        return Orient2D(1 - self.value)


class Direction(IntEnum):
    LOWER = 0
    UPPER = 1

    def flip(self) -> Direction:
        return Direction(1 - self.value)


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
    DIFF_COLOR = 0
    SAME_COLOR = 1
    LINE_END = 2


def get_extension(output_type: DesignOutput) -> str:
    """Get file extension of the given output type."""
    if output_type is DesignOutput.GDS:
        return 'gds'
    elif output_type is DesignOutput.YAML:
        return 'yaml'
    elif output_type is DesignOutput.CDL:
        return 'cdl'
    elif output_type is DesignOutput.VERILOG:
        return 'v'
    elif output_type is DesignOutput.SYSVERILOG:
        return 'sv'
    else:
        raise ValueError('Unsupported output type: {}'.format(output_type.name))


def is_model_type(output_type: DesignOutput) -> bool:
    """Returns true if the given output type represents a behavioral model."""
    return output_type is DesignOutput.SYSVERILOG


def is_netlist_type(output_type: DesignOutput) -> bool:
    """Returns true if the given output type represents a netlist."""
    return output_type is DesignOutput.CDL or output_type is DesignOutput.VERILOG
