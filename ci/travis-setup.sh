#!/bin/bash
set -e

if ! [[ -z "$CXX" ]]; then
  echo "g++ version:"
  $CXX --version
fi

if ! [[ -z "$CC" ]]; then
  echo "gcc version:"
  $CC --version
fi

mkdir -p m4

./autogen.sh
