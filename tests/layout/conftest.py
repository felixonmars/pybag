# -*- coding: utf-8 -*-

import os
import pkg_resources

import pytest

from pybag.core import PyRoutingGrid, PyTech

tech_fname = pkg_resources.resource_filename(__name__, os.path.join('data', 'tech_params.yaml'))
grid_fname = pkg_resources.resource_filename(__name__, os.path.join('data', 'grid.yaml'))


@pytest.fixture(scope='session')
def tech():
    return PyTech(tech_fname)


@pytest.fixture(scope='session')
def routing_grid(tech):
    return PyRoutingGrid(tech, grid_fname, None)
