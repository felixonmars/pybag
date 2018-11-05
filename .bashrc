#!/usr/bin/env bash
# set up environment variables needed to build library

# build temporary/output directories
# we override these settings because we need the build files to be at a known
# location so that we can add the right directory to PYTHONPATH for
# documentation generating or unit testing
export PYBAG_BUILD_ROOT="$(pwd)/_build"
export PYBAG_BUILD_TEMP="${PYBAG_BUILD_ROOT}/temp"
export PYBAG_BUILD_LIB="${PYBAG_BUILD_ROOT}/lib"
