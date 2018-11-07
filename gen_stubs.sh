#!/usr/bin/env bash
# this script generates .pyi interface files for Pycharm using mypy

export OUTPUT_ROOT="src"
export PYTHONPATH="_build/lib:./pybind11_generics"

# python -m stubgen -h
python -m stubgen --recursive -o ${OUTPUT_ROOT} pybag.oa
python -m stubgen --recursive -o ${OUTPUT_ROOT} pybag.schematic
python -m stubgen --recursive -o ${OUTPUT_ROOT} pybag.util.geometry
python -m stubgen --recursive -o ${OUTPUT_ROOT} pybag.util.interval
