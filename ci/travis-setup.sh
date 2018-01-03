#!/bin/bash
set -e

echo "g++ version:"
$CXX --version
echo "gcc version:"
$CC --version
mkdir -p m4
./autogen.sh
./configure
