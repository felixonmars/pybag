#!/usr/bin/env bash
# this script builds the documentation using sphinx

source .bashrc

cd docs
# remove auto-generated files to force regeneration
rm -rf build/doctrees/api
make html
