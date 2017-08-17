#!/bin/bash
set -e

# Setup
ci/travis-setup.sh

# Check standard tests standard build with rpmalloc
./configure
make check-standard -j

# Check standard tests standard build with system malloc
./configure --enable-system-malloc
make check-standard -j

# Check that different calls to make work
make distcheck

# Check standard tests with assertions on
etc/debug.sh
make check-standard -j

# Check standard tests with assertions off
etc/ndebug.sh
make check-standard -j
