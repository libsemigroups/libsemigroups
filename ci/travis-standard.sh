#!/bin/bash
set -e

ci/travis-setup.sh
ci/travis-setup-hpcombi.sh

# Check standard tests with HPCombi enabled
make clean
./configure --enable-hpcombi
make test_all -j2 && ./test_all "[quick],[standard]"

# Check standard tests with HPCombi disabled
make clean
./configure --disable-hpcombi
make test_all -j2 && ./test_all "[quick],[standard]"

# Check standard tests with assertions on
make clean
./configure --enable-debug
make test_all -j2 && ./test_all "[quick],[standard]"

# Check quick tests with verbose mode
make clean
./configure --enable-verbose
make test_all -j2 && ./test_all "[quick]"

# Check compiler warnings
make clean
./configure --enable-compile-warnings
make test_all -j2 
# Don't run tests just checking what happens when we compile with the warnings
# enabled.
