#!/bin/bash
set -e

# Setup
ci/travis-setup.sh

# Check standard tests standard build
make check-standard -j

# Check that different calls to make work
make distcheck

# Check standard tests with assertions on
etc/debug.sh
make check-standard -j

# Check standard tests with assertions off
etc/ndebug.sh
make check-standard -j
