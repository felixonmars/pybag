#!/usr/bin/env bash
# this script runs the python unit tests.
# you must first run the setup script.

./run_test_cbag.sh || exit 1
./build.sh || exit 1

source .bashrc

# smart prepend to PYTHONPATH even if variable is unset
export PYTHONPATH="${PYBAG_BUILD_LIB}:${PYTHONPATH:+:$PYTHONPATH}"

${PYBAG_PYTEST} tests -n 4 $@ || exit 1
./gen_stubs.sh || exit 1
./build.sh || exit 1
