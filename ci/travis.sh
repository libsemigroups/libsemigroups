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
