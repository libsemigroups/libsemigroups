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
  echo "Usage: $0 <filename>"
  exit 1
fi

if [ ! -d "json" ]; then
  mkdir json
fi

FILENAME=json/$(date "+%Y-%m-%dT%H-%M-%S")-$(uname -m)-$(uname -s | tr '[:upper:]' '[:lower:]')-$(uname -r)-$1.3-manifolds.json
printf "\033[7mWriting to $FILENAME . . .\033[0m\n"
pytest -v --benchmark-json="$FILENAME" "$1"
