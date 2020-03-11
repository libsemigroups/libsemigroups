#!/bin/bash
set -e

# Setup
ci/travis-setup.sh

./configure
make bench_all -j4 && ./bench_all "[quick]"
