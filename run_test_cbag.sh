#!/usr/bin/env bash

if [ -z ${OA_LINK_DIR+x} ]
then
    echo "OA_LINK_DIR is unset"
    exit 1
fi

export NUM_CORES=`nproc --all`

export BUILD_TYPE=${1:-Debug}
echo "CMAKE_BULD_TYPE: ${BUILD_TYPE}"
cmake -H. -B_build/temp -DCMAKE_LIBRARY_OUTPUT_DIRECTORY='_build/lib/pybag' -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DCMAKE_CXX_COMPILER_LAUNCHER=ccache || exit 1
cmake --build _build/temp --target test_main -- -j ${NUM_CORES} || exit 1
cd cbag
../_build/temp/cbag/tests/test_main
cd ..
