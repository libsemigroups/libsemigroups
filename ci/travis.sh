#!/bin/bash
set -e
mkdir -p m4
./autogen.sh

# Check standard tests standard build
./configure
make check-standard -j

#etc/ndebug.sh
#make check-standard -j
#etc/debug.sh
##make check-standard -j
