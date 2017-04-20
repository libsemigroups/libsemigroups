# Python bindings for libsemigroups

## Basic instructions

Install the libsemigroups C++ library, e.g. from sources:

    git clone https://github.com/james-d-mitchell/libsemigroups/ python-bindings

    cd libsemigroups
    make
    sudo make install
    sudo ldconfig

Install the python bindings::

    cd python-bindings
    pip3 install --user . --upgrade

Try it out:

    python3

    >>> from semigroups import Semigroup, Transformation
    >>> S = Semigroup([Transformation([1,1,4,5,4,5]),Transformation([2,3,2,3,5,5])])
    >>> S.size()
    5

## Uploading the package to pipy

Build the binary wheel:

    rm -rf dist
    python setup.py sdist

The first time:

    twine register dist/*.tar.gz

For latter releases:

    twine upload -s dist/*.tar.gz

## Trick to debug segmentation faults

Install Sage

Install gdb in Sage:

    sage -i gdb

Run sage as:

    sage -gdb

and type the commands that trigger the segfault. Then gdb will be
fired automatically, allowing for analysing the stack trace.

## [TODO list](TODO)
