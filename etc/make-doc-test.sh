#!/bin/bash
set -e

# This file exists to allow us to regenerate the doc-tests without
# doing so via make

echo "Running code example extraction script . . ."
python3 etc/extract-doc-examples.py ./include/libsemigroups/ -r
echo "Formatting tests/test-docs-code-examples.cpp . . ."
clang-format-15 -i tests/test-docs-code-examples.cpp
