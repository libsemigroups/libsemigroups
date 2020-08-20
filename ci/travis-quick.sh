#!/bin/bash
set -e

# Setup
ci/travis-setup.sh

# $CONFIG_FLAGS contains --disable-hpcombi when compiling with clang-5.0
# because clang-5.0 is so old that it can't compile HPCombi
./configure "$CONFIG_FLAGS" 
make test_all -j4 && ./test_all "[quick],[standard]"
