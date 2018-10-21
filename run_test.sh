#!/usr/bin/env bash
# this script runs the python unit tests.
# you must first run the setup script.

source .bashrc

# smart prepend to PYTHONPATH even if variable is unset
export PYTHONPATH="${PYBAG_BUILD_LIB}:${PYTHONPATH:+:$PYTHONPATH}"

${PYBAG_PYTEST} tests
