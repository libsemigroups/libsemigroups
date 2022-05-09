#!/bin/bash
set -e

if [[ -x "$(command -v unbuffer)" ]]; then
  UNBUF="unbuffer"
  UNBUF_P="unbuffer -p"
else
  UNBUF=
  UNBUF_P=
fi

cd docs/
mkdir -p source/_static
$UNBUF make html-no-doxy 2>&1 | $UNBUF_P grep -v --color=always "WARNING: Duplicate declaration\|WARNING: Inline emphasis start-string without end-string.\|WARNING: Too many template argument lists compared to parameter lists\|^\s*libsemigroups\|Citation .* is not referenced\|WARNING: cpp:member targets a"

echo "Post-processing generated html files to correct links . . ."

cd .. && etc/post-proc-doc.sh

echo "See: docs/build/html/index.html"
