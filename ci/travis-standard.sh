#!/bin/bash
set -e

# Setup
ci/travis-quick.sh

# Check that different calls to make work
make distcheck

# Check standard tests with assertions on
make clean
./configure --enable-debug
make check -j2

# Check standard tests with extra code for statistics 
make clean
./configure --enable-stats
make check -j2
