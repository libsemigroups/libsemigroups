#!/bin/bash
set -e

# Setup
ci/travis-setup.sh

./configure --with-external-fmt
make test_all -j4 && ./test_all "[quick],[standard]"

./configure --disable-popcnt --disable-clzll 
make test_bitset -j4 && ./test_bitset "[quick]"
