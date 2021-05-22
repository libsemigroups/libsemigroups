#!/bin/bash
set -e

echo "Generating sphinx doc from docs/yml . . ." 
mkdir -p docs/build
cd docs/
echo "doxygen --version"
doxygen --version
./generate_from_yml.py 
