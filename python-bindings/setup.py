# -*- coding: utf-8 -*-

from setuptools import setup, Extension
from Cython.Build import cythonize
from codecs import open
from os import path

#from setuptools.dist import Distribution
#from Cython.Distutils import build_ext
#Distribution(dict(setup_requires='Cython'))

here = path.abspath(path.dirname(__file__))

# Get the long description from the README file
with open(path.join(here, 'README.rst'), encoding='utf-8') as f:
    long_description = f.read()

setup(
    version = "0.2.2",
    name = "libsemigroups-python-bindings",
    description='Python bindings for the libsemigroups mathematics library',
    long_description=long_description,
    url='https://github.com/james-d-mitchell/libsemigroups/tree/master/python-bindings',
    author='James Mitchell and Nicolas M. Thiéry',
    author_email='TODO',
    license='GPLv3',
    classifiers=[
        'Development Status :: 3 - Alpha',
        'Intended Audience :: Developers',
        'Intended Audience :: Science/Research',
        'Topic :: Scientific/Engineering :: Mathematics',
        'License :: OSI Approved :: GNU General Public License v3 (GPLv3)',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 3',
    ],
    keywords='Mathematics, semigroup theory',

    ext_modules = cythonize([
        Extension("semigroups",
                      sources=["semigroups.pyx", "semigroups_cpp.cpp"],
                      depends=["semigroups_cpp.h", "semigroups_cpp.pxd"],
                      libraries=["semigroups"],
                      language="c++",             # generate C++ code
                      extra_compile_args=["-std=c++11"],
                      )]),

    setup_requires=['pytest-runner', 'pytest-cython'],
    tests_require=['pytest'],
)

# Note: getting the headers included in the source distribution seems tricky.
# For now, we use the MANIFEST.in, as recommended on
# https://stackoverflow.com/questions/43163315/how-to-include-header-file-in-cython-correctly-setup-py
