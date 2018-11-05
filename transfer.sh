#!/usr/bin/env bash

export ROOT='erichang@bwrcrdsl-6.eecs.berkeley.edu:~/projects/bag_gen/central/BAG_framework/pybag'

rsync -zv CMakeLists.txt ${ROOT}/CMakeLists.txt
rsync -zv build.sh ${ROOT}/build.sh
rsync -zv run_test.sh ${ROOT}/run_test.sh
rsync -zv setup.py ${ROOT}/setup.py

rsync -zarv --delete src/ ${ROOT}/src
rsync -zarv --delete tests/ ${ROOT}/tests
rsync -zarv --delete --exclude '_build' --exclude '.git' cbag/ ${ROOT}/cbag
rsync -zarv --delete --exclude '_build' --exclude '.git' pybind11_generics/ ${ROOT}/pybind11_generics
