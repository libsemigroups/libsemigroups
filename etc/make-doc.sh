#!/bin/bash
set -e

mkdir -p docs/build
cd docs/
echo "doxygen --version"
doxygen --version
doxygen
echo "Deploying doc fixups . . ."
./fixup.py
echo "See: docs/html/index.html"
