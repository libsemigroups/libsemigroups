#!/bin/bash
set -e

ci/travis-setup.sh
ci/travis-setup-hpcombi.sh

# Check standard tests with HPCombi enabled
./configure --enable-hpcombi
make test_all -j4 && ./test_all "[quick],[standard]"

# Check standard tests with HPCombi disabled
make clean
./configure --disable-hpcombi
make test_all -j4 && ./test_all "[quick],[standard]"
