#!/bin/bash
set -e

# Setup
ci/travis-setup.sh

# Check standard tests standard build
make check -j2
