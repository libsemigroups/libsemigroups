#!/bin/bash
set -e

ci/travis-setup.sh
ci/travis-setup-hpcombi.sh

./configure --enable-compile-warnings --enable-hpcombi
make test_all -j4 
# Don't run tests just checking what happens when we compile with the warnings
# enabled.

./configure --enable-compile-warnings --disable-hpcombi
make test_all -j4 
# Don't run tests just checking what happens when we compile with the warnings
# enabled.
