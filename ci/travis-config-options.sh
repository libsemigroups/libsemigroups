#!/bin/bash
set -e

# Setup
ci/travis-setup.sh

./configure --with-external-fmt
make test_all -j4 && ./test_all "[quick],[standard]"

make clean
./configure --disable-popcnt --disable-clzll 
make test_bitset -j4 && ./test_bitset "[quick]"

./configure --enable-verbose
make test_all -j4 && ./test_all "[quick]"

make clean
./configure --with-external-eigen
make test_obvinf -j4 && ./test_obvinf "[quick]"
