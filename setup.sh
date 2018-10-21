#!/usr/bin/env bash
# this script builds the C++ extension

source .bashrc

${CBAG_PYTHON} setup.py build_ext --parallel 8
