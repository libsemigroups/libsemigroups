#!/bin/bash
set -e

# Install lcov and gcov
sudo apt-get install -y lcov ggcov
sudo ln -sf /usr/bin/gcov-5 /usr/bin/gcov

# Setup
ci/travis-setup.sh

# Versions
echo "gcc-5 version:"
gcc-5 --version
echo "gcov version:"
gcov --version

# Check code coverage
make clean
./configure CXXFLAGS='-O0 -g --coverage' LDFLAGS='-O0 -g --coverage' --enable-code-coverage
make check-code-coverage -j2
DIR=$(find . -maxdepth 1 -type d | grep "libsemigroups-.*-coverage")
if [ -z DIR ] ; then 
  echo "Cannot find the libsemigroups-x.y.z-coverage directory"
  exit 1
fi
mkdir -p log
ci/lcov-summary.py $DIR | tee log/coverage-log.txt
echo

# Exclude report.h from coverage checks
( ! grep '31m\|33m' log/coverage-log.txt | sed 's/^.*\([0-9 ][0-9]\.[0-9]\%\).*src/LOW COVERAGE: src/' | grep --invert-match -E "report" )
