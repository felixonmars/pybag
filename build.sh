#!/usr/bin/env bash
# this script builds the C++ extension

source .bashrc

if [ -z ${OA_LINK_DIR+x} ]
then
    echo "OA_LINK_DIR is unset"
    exit 1
fi

if [ -z ${PYBAG_PYTHON+x} ]
then
    echo "PYBAG_PYTHON is unset"
    exit 1
fi

${PYBAG_PYTHON} setup.py build \
    --build-temp ${PYBAG_BUILD_TEMP} \
    --build-lib ${PYBAG_BUILD_LIB} \
