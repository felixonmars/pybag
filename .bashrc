#!/usr/bin/env bash
# set up environment variables needed to build library

# Use gcc 7; latest gcc 8 has internal compiler error
export CC=/usr/bin/gcc-7
export CXX=/usr/bin/g++-7

# OA library/include directories
export OA_LINK_DIR=/home/erichang/programs/oa_dist/lib/linux_rhel50_gcc44x_64/opt
export OA_INCLUDE_DIR=/home/erichang/programs/oa_dist/include

# python/pytest executables
export PYBAG_PYTHON=$(which python)
export PYBAG_PYTEST=$(which pytest)

# build temporary/output directories
# we override these settings because we need the build files to be at a known
# location so that we can add the right directory to PYTHONPATH for
# documentation generating or unit testing
export PYBAG_BUILD_ROOT="$(pwd)/_build"
export PYBAG_BUILD_TEMP="${PYBAG_BUILD_ROOT}/temp"
export PYBAG_BUILD_LIB="${PYBAG_BUILD_ROOT}/lib"
