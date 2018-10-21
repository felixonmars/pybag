#!/usr/bin/env bash
# this script builds the C++ extension

source .bashrc

${PYBAG_PYTHON} setup.py build --parallel 8 --build-temp ${PYBAG_BUILD_TEMP} \
                --build-lib ${PYBAG_BUILD_LIB}
