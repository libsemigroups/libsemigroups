#!/bin/bash
set -e

# This script doesn't really need to exist it's here to allow for uniform
# filenames in the output from benchmarks.
#
# Use it like:
#
#   ./bench-sims-to-xml.sh "braid-group-5"
#
# i.e. without the square brackets around the tag

if [ "$#" -ne 1 ]; then
  echo "Usage: $0 <tag with no square brackets>"
  exit 1
fi

# Check if bench_sims is available
if [ ! -f "../../bench_sims" ]; then
  echo "The benchmark executable \"bench_sims\" cannot be found"
  exit 2
fi

# check for unbuffer, this is optional, it permits more incremental reporting
# of the result. I.e. it is more like watching the actual xml be output, when
# piping it into tee it is buffered and so looks like it's doing nothing.
if [ -x "$(command -v unbuffer 2>&1 >/dev/null)" ]; then
  UNBUFFER="unbuffer -p"
else
  UNBUFFER=
fi

if [ ! -d "xml" ]; then
  mkdir xml
fi

FILENAME=xml/$(date "+%Y-%m-%dT%H-%M-%S")-$(uname -m)-$(uname -s | tr '[:upper:]' '[:lower:]')-$(uname -r)-$1.xml
printf "\033[7mWriting to $FILENAME . . .\033[0m\n"
$UNBUFFER ../../bench_sims "[$1]" --reporter=xml | tee "$FILENAME"
