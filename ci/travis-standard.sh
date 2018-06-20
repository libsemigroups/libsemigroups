#!/bin/bash
set -e

echo -e "\nCloning HPCombi master branch . . ."
git clone -b master --depth=1 https://github.com/hivert/HPCombi.git extern/HPCombi
echo "0.0.1" >> extern/HPCombi/VERSION

# JDM: not completely sure why the next line is necessary
CXXFLAGS="${CXXFLAGS} -flax-vector-conversions"

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
