# -*- coding: utf-8 -*-

import os
import re
import sys
import platform
import subprocess

from distutils.version import LooseVersion
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext

pkg_name = 'pybag'


class CMakePyBind11Extension(Extension):
    def __init__(self, name, sourcedir=''):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)


class CMakePyBind11Build(build_ext):
    user_options = build_ext.user_options
    user_options.append(('compiler-launcher', None, "specify compiler launcher program"))
    user_options.append(('build-type', None, "CMake build type"))

    def initialize_options(self):
        build_ext.initialize_options(self)
        # noinspection PyAttributeOutsideInit
        self.compiler_launcher = ''
        self.build_type = 'Debug'

    def run(self):
        print('comp_launcher =', self.compiler_launcher)
        try:
            out = subprocess.check_output(['cmake', '--version'])
        except OSError:
            raise RuntimeError(
                "CMake must be installed to build the following extensions: " +
                ", ".join(e.name for e in self.extensions))

        if platform.system() == "Windows":
            cmake_version = LooseVersion(re.search(r'version\s*([\d.]+)',
                                                   out.decode()).group(1))
            if cmake_version < '3.1.0':
                raise RuntimeError("CMake >= 3.1.0 is required on Windows")

        for ext in self.extensions:
            self.build_extension(ext)

    def _get_num_workers(self):
        workers = self.parallel
        if self.parallel == 0:
            workers = os.cpu_count()  # may return None

        return 1 if workers is None else workers

    def build_extension(self, ext):
        extdir = os.path.abspath(os.path.join(self.build_lib, pkg_name))
        cmake_args = [
            'cmake',
            '-H.',
            '-B' + self.build_temp,
            '-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=' + extdir,
            '-DPYTHON_EXECUTABLE=' + sys.executable,
            '-DCMAKE_BUILD_TYPE=' + self.build_type,
        ]
        build_args = [
            'cmake',
            '--build',
            '.',
            '--',
        ]

        if self.compiler_launcher:
            cmake_args.append('-DCMAKE_CXX_COMPILER_LAUNCHER=' + self.compiler_launcher)

        if platform.system() == "Windows":
            if sys.maxsize > 2 ** 32:
                cmake_args.append('-A')
                cmake_args.append('x64')
            build_args.append('/m')

        num_workers = self._get_num_workers()
        print('parallel =', num_workers)
        build_args.append('-j' + str(num_workers))
        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)
        print(' '.join(cmake_args))
        subprocess.check_call(cmake_args, cwd=ext.sourcedir)
        print(' '.join(build_args))
        subprocess.check_call(build_args, cwd=self.build_temp)
        # Add an empty line for cleaner output
        print()


setup(
    name=pkg_name,
    version='0.2',
    author='Eric Chang',
    author_email='pkerichang@berkeley.edu',
    description='Python wrappers of cbag library using pybind11',
    long_description='',
    install_requires=[],
    setup_requires=[],
    tests_require=[
        'pytest',
        'pytest-xdist',
    ],
    packages=[
        pkg_name,
    ],
    package_dir={'': 'src'},
    package_data={
        # install stub files
        pkg_name: [
            '*.pyi',
        ],
    },
    ext_modules=[
        CMakePyBind11Extension('all'),
    ],
    cmdclass={
        'build_ext': CMakePyBind11Build,
    },
    zip_safe=False,
)
