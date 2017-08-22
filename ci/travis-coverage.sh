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
./configure --enable-coverage ; make coverage
for d in log; do
  if [ -d $d ]; then
    ci/lcov-summary.py $d | tee log/coverage-log.txt
    break
  fi;
done;

echo

# Exclude timer.h and report.h from coverage checks
( ! grep 31m log/coverage-log.txt | sed 's/^.*\([0-9 ][0-9]\.[0-9]\%\).*src/LOW COVERAGE: src/' | grep --invert-match -E "timer|report" )
