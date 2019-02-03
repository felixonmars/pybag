#!/usr/bin/env bash

if [ -z ${OA_LINK_DIR+x} ]
then
    echo "OA_LINK_DIR is unset"
    exit 1
fi

export NUM_CORES=`nproc --all`

export WORK_DIR=$(pwd)
export BUILD_TYPE=${1:-Debug}
echo "CMAKE_BULD_TYPE: ${BUILD_TYPE}"
cmake -H. -B${WORK_DIR}/_build/temp \
      -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=${WORK_DIR}/_build/lib/pybag \
      -DPYTHON_EXECUTABLE=${PYBAG_PYTHON} \
      -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
      -DCMAKE_CXX_COMPILER_LAUNCHER=ccache || exit 1
cd _build/temp
cmake --build . --target test_main -- -j${NUM_CORES} || exit 1
cd ../../cbag
../_build/temp/cbag/tests/test_main
cd ..
