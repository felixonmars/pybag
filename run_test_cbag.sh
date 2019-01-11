#!/usr/bin/env bash

if [ -z ${OA_LINK_DIR+x} ]
then
    echo "OA_LINK_DIR is unset"
    exit 1
fi

export BUILD_TYPE=${1:-Debug}
echo "CMAKE_BULD_TYPE: ${BUILD_TYPE}"
cmake -H. -B_build/temp -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DCMAKE_CXX_COMPILER_LAUNCHER=ccache || exit 1
cmake --build _build/temp --target test_main -- -j 8 || exit 1
cd cbag
../_build/temp/cbag/tests/test_main
cd ..
