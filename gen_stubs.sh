#!/usr/bin/env bash
# this script generates .pyi interface files for Pycharm using mypy

export PYTHONPATH="build/lib.linux-x86_64-3.7"

stubgen -o src pybag.base
stubgen -o src pybag.base.util
stubgen -o src pybag.base.util.interval
