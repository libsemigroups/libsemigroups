#!/bin/bash
set -e

echo "Checking whether to build the to-table . . ."
if command -v pdflatex && command -v inkscape 2>&1 >/dev/null; then
    cd docs/pictures
    echo "Building to-table . . ."
    pdflatex to-table.tex
    inkscape --pdf-poppler --export-type="svg" -o to-table.svg to-table.pdf
    cd ../..
else
    echo "Not building to-table"
fi
echo "Checking doc order . . ."
./etc/check_doc_order.py
echo "Checking doxygen linebreaks . . ."
./etc/check_doxygen_line_breaks.py
mkdir -p docs/build
cd docs/
echo "doxygen --version"
doxygen --version
doxygen
echo "Deploying doc fixups . . ."
./fixup.py
echo "See: docs/html/index.html"
