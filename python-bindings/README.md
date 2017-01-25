# Python bindings for libsemigroups

## Basic instructions

    git clone ...

    cd libsemigroups
    make
    sudo make install

    cd python-bindings
    pip3 install --user .

    python3

    >>> from semigroups import Semigroup, Transformation
    >>> S = Semigroup([Transformation([1,1,4,5,4,5]),Transformation([2,3,2,3,5,5])])
    >>> S.size()
    5

## Debugging instructions

Install Sage

Install gdb in Sage:

    sage -i gdb

    sage -gdb

    ... type commands as usual ...
