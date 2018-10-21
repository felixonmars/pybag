#!/usr/bin/env bash
# this script generates .pyi interface files for Pycharm using mypy

export PYTHONPATH="_build/lib:/home/erichang/projects/mypy"
python -m mypy.stubgen --recursive -o src pybag.base
