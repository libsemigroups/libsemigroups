# Python bindings for libsemigroups

## Basic instructions

    git clone ...

    git checkout autotools
    ./autogen.sh
    ./configure

    cd libsemigroups
    make
    sudo make install
    sudo ldconfig

    cd python-bindings
    pip3 install --user . --upgrade

    python3

    >>> from semigroups import Semigroup, Transformation
    >>> S = Semigroup([Transformation([1,1,4,5,4,5]),Transformation([2,3,2,3,5,5])])
    >>> S.size()
    5

## Trick to debug segmentation faults

Install Sage

Install gdb in Sage:

    sage -i gdb

Run sage as:

    sage -gdb

and type the commands that trigger the segfault. Then gdb will be
fired automatically, allowing for analysing the stack trace.

## [TODO list](TODO)
