#!/bin/bash
set -e

mkdir -p m4
./autogen.sh

# Check standard tests standard build
./configure
make check-standard -j

# Check make 
make distcheck

# Check standard tests with assertions on
etc/debug.sh
make check-standard -j

# Check standard tests with assertions on
etc/ndebug.sh
make check-standard -j

echo -e "\nRunning cpplint . . ."
make lint

# The following checks code coverage but for some reason on travis runs out of
# memory at present
#etc/coverage.sh
#for d in log; do
#  etc/lcov-summary.py $d | tee log/coverage-log.txt
#  break
#done;
#( ! grep -q -E "Warning!" log/coverage-log.txt )
