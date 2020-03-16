#!/bin/bash
set -e

ci/travis-setup.sh
ci/travis-setup-hpcombi.sh

./configure --enable-hpcombi --enable-verbose
make test_all -j4 && ./test_all "[quick]"

make clean
./configure --disable-hpcombi --enable-verbose
make test_all -j4 && ./test_all "[quick]"
