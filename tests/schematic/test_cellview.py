# -*- coding: utf-8 -*-

import os

import pkg_resources

import pytest

from pybag.base.schematic import PySchCellView

cv_files = [
    'sense_amp_strongarm.yaml',
]


@pytest.mark.parametrize("fname", cv_files)
def test_constructor(fname):
    """Check class increment reference count of value objects."""
    yaml_file = pkg_resources.resource_filename(__name__, os.path.join('data', fname))

    cv = PySchCellView(yaml_file)

    assert list(cv.in_terms()) == ['clk', 'inn', 'inp']
