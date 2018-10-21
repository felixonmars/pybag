#!/usr/bin/env bash

source .bashrc

${CBAG_PYTHON} setup.py build_ext --parallel 8
