# -*- coding: utf-8 -*-
# Stubs for pybag.layout.tech (Python 3.7)
#
# NOTE: This dynamically typed stub was automatically generated by stubgen.


class PyTech:
    @property
    def default_purpose(self) -> str: ...
    @property
    def pin_purpose(self) -> str: ...
    def __init__(self, tech_fname: str) -> None: ...
    def get_min_space(self, layer_type: str, width: int, sp_type: int) -> int: ...


