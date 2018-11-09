# -*- coding: utf-8 -*-
# Stubs for pybag.schematic (Python 3.7)
#
# NOTE: This dynamically typed stub was automatically generated by stubgen.

from typing import Any, Union, Tuple, Optional, Iterable, Iterator

class PySchCellView:
    def __init__(self, *args, **kwargs) -> Any: ...
    def add_pin(self, new_name: str, term_type: int) -> None: ...
    def array_instance(self, old_name: str, dx: int, dy: int, name_conn_range: Iterable[Tuple[str, Iterable[Tuple[str, str]]]]) -> None: ...
    def clear_params(self) -> None: ...
    def get_inst_ref(self, name: str) -> Optional[PySchInstRef]: ...
    def get_terminal_type(self, term: str) -> int: ...
    def has_terminal(self, term: str) -> bool: ...
    def inst_refs(self) -> Iterator[Tuple[str, PySchInstRef]]: ...
    def remove_instance(self, name: str) -> bool: ...
    def remove_pin(self, name: str) -> bool: ...
    def rename_instance(self, old_name: str, new_name: str) -> None: ...
    def rename_pin(self, old_name: str, new_name: str) -> None: ...
    def set_param(self, name: str, val: Union[int, float, bool, str]) -> None: ...
    def terminals(self) -> Iterator[Tuple[str, int]]: ...
    @property
    def cell_name(self) -> str: ...
    @cell_name.setter
    def cell_name(self, val: str) -> None: ...
    @property
    def lib_name(self) -> str: ...
    @property
    def view_name(self) -> str: ...

class PySchInstRef:
    def __init__(self, *args, **kwargs) -> Any: ...
    def set_param(self, name: str, val: Union[int, float, bool, str]) -> None: ...
    def update_connection(self, inst_name: str, term: str, net: str) -> None: ...
    def update_master(self, lib: str, cell: str, prim: bool = False) -> None: ...
    @property
    def cell_name(self) -> str: ...
    @cell_name.setter
    def cell_name(self, val: str) -> None: ...
    @property
    def height(self) -> int: ...
    @property
    def is_primitive(self) -> bool: ...
    @is_primitive.setter
    def is_primitive(self, val: bool) -> None: ...
    @property
    def lib_name(self) -> str: ...
    @lib_name.setter
    def lib_name(self, val: str) -> None: ...
    @property
    def width(self) -> int: ...