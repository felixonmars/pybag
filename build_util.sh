#!/usr/bin/env bash
# this script builds the C++ extension

source .bashrc

if [ -z ${OA_LINK_DIR+x} ]
then
    echo "OA_LINK_DIR is unset"
    exit 1
fi

export BUILD_TYPE=${1:-Debug}
echo "CMAKE_BULD_TYPE: ${BUILD_TYPE}"
cmake -H. -B_build/temp -DCMAKE_LIBRARY_OUTPUT_DIRECTORY='_build/lib/pybag' -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DCMAKE_CXX_COMPILER_LAUNCHER=ccache || exit 1
cmake --build _build/temp --target write_cv_yaml -- -j 8 || exit 1
cmake --build _build/temp --target write_tech_info -- -j 8 || exit 1
cmake --build _build/temp --target print_gds -- -j 8 || exit 1
