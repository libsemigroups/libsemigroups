#!/bin/bash
set -e

echo "Checking doc order . . ."
./etc/check_doc_order.py
mkdir -p docs/build
cd docs/
echo "doxygen --version"
doxygen --version
doxygen
echo "Deploying doc fixups . . ."
./fixup.py
echo "See: docs/html/index.html"
