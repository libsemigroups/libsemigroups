#!/bin/bash
set -e

echo "Checking whether to build the to-table . . ."
if command -v pdflatex && command -v ghostscript 2>&1 >/dev/null; then
    cd docs/pictures
    echo "Building to-table . . ."
    pdflatex -shell-escape to-table.tex
    cd ../..
else
    echo "Not building to-table"
fi
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
