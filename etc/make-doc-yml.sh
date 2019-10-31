#!/bin/bash
set -e

echo "Generating sphinx doc from docs/yml . . ." 
mkdir -p docs/build
cd docs/ 
./generate_from_yml.py 
