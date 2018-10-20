#!/usr/bin/env bash

source .bashrc

export BUILD_TYPE=${1:-Debug}
echo "CMAKE_BULD_TYPE: ${BUILD_TYPE}"
cmake -H. -B_build -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DPYTHON_EXECUTABLE=${CBAG_PYTHON}
cmake --build _build -- -j 8
