#!/bin/bash
set -e

# Setup
ci/travis-setup.sh

./configure
make test_all -j2 && ./test_all "[quick],[standard]"
