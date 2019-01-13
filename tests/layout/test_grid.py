# -*- coding: utf-8 -*-

import pytest

from pybag.core import PyRoutingGrid, PyTech
from pybag.enum import RoundMode

coord_htr_data = [
    (1, 60, RoundMode.NONE, False, 0),
    (1, 120, RoundMode.NONE, False, 1),
    (1, 180, RoundMode.NONE, False, 2),
    (1, 0, RoundMode.NONE, False, -1),
    (1, -60, RoundMode.NONE, False, -2),
    (5, 90, RoundMode.NONE, False, 0),
    (5, 180, RoundMode.NONE, False, 1),
    (5, 270, RoundMode.NONE, False, 2),
    (5, 0, RoundMode.NONE, False, -1),
    (5, -90, RoundMode.NONE, False, -2),
]


pitch_data = [
    (1, 120),
    (2, 120),
    (3, 120),
    (4, 120),
    (5, 180),
    (6, 180),
    (7, 180),
    (8, 360),
]


@pytest.mark.parametrize("layer_id, coord, mode, even, htr", coord_htr_data)
def test_coord_htr(layer_id: int, coord: int, mode: RoundMode, even: bool,
                   htr: int, routing_grid: PyRoutingGrid):
    assert routing_grid.coord_to_htr(layer_id, coord, mode, even) == htr


@pytest.mark.parametrize("layer_id, pitch", pitch_data)
def test_coord_htr(layer_id: int, pitch: int, routing_grid: PyRoutingGrid):
    assert routing_grid.get_track_pitch(layer_id) == pitch
    assert routing_grid.get_track_offset(layer_id) == pitch // 2


def test_copy_construct(tech: PyTech, routing_grid: PyRoutingGrid):
    copy_grid = PyRoutingGrid(tech, '', routing_grid)
    assert copy_grid == routing_grid
    assert copy_grid is not routing_grid
