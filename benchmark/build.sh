#!/bin/bash

# This should be run from the libsemigroups/ directory, to build the benchmark
# executable.
set -e

# Build and install libsemigroups, required since the benchmark executables
# link against the libsemigroups library.
./configure ; make ; make install

# Build the benchmark executables
if [ -d benchmark/src/build ]; then
  cd benchmark/src/build
else
  mkdir -p benchmark/src/build
  cd benchmark/src/build
  cmake -DCMAKE_BUILD_TYPE=Release  ..
fi
make -j3
