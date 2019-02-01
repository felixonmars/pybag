#!/usr/bin/env bash
# this script generates .pyi interface files for Pycharm using mypy

export OUTPUT_ROOT="src"
export PYTHONPATH="_build/lib:./pybind11_generics"

# python -m stubgen -h
${PYBAG_PYTHON} -m stubgen --recursive -o ${OUTPUT_ROOT} pybag.core
