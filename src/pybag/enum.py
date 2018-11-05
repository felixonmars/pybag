# -*- coding: utf-8 -*-

"""This package contains various enums used by the C++ extension."""

from enum import IntEnum

class TermType(IntEnum):
    input = 0
    output = 1
    inout = 2
