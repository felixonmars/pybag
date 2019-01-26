# -*- coding: utf-8 -*-

import pytest

from pybag.core import PyWireArray, PyTrackID

class TrackID(PyTrackID):
    def __init__(self, layer_id: int, track_idx: float, width: int = 1, num: int = 1,
                 pitch: float = 0) -> None:
        if num < 1:
            raise ValueError('TrackID must have 1 or more tracks.')

        PyTrackID.__init__(self, layer_id, int(round(2 * track_idx)), width, num,
                           int(round(2 * pitch)))

    def is_tid(self):
        return True



class WireArray(PyWireArray):
    def __init__(self, track_id: TrackID, lower: int, upper: int) -> None:
        PyWireArray.__init__(self, track_id, lower, upper)

    @PyWireArray.track_id.getter
    def track_id(self) -> TrackID:
        """TrackID: The TrackID of this WireArray."""
        tid = super(WireArray, self).track_id  # type: TrackID
        return tid

    @classmethod
    def wire_grp_iter(cls, warr):
        """Iterate through WireArrays in the given WireArray or WireArray list."""
        if isinstance(warr, WireArray):
            yield warr
        else:
            yield from warr

    def is_warr(self):
        return True


def add_wires_emulator():
    lower = 10
    upper = 200
    tid = TrackID(3, 5, 1, 2, 4)
    warr = WireArray(tid, lower, upper)
    return warr


def test_get_track_id():
    """Test to check that WireArray keeps its TrackID alive in Python land."""
    warr = add_wires_emulator()
    assert warr.is_warr()

    for new_w in WireArray.wire_grp_iter(warr):
        tid = new_w.track_id

        assert new_w.is_warr()
        assert isinstance(tid, TrackID)
        assert type(tid) is TrackID
        assert tid.is_tid()
