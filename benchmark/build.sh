#!/bin/bash

# This should be run from the libsemigroups/ directory, to build the benchmark
# executable.
set -e

# Build the benchmark executables
if [ -d benchmark/src/build ]; then
  make -j4 ; make install
  cd benchmark/src/build
else
  # Build and install libsemigroups, required since the benchmark executables
  # link against the libsemigroups library.
  if [ ! -f configure ]; then 
    ./autogen.sh
  elif [ -f Makefile ]; then
    make clean
  fi
  ./configure ; make -j4 ; make install
  mkdir -p benchmark/src/build
  cd benchmark/src/build
  cmake -DCMAKE_BUILD_TYPE=Release  ..
fi
make -j3
