#!/bin/bash
set -e

# Install clang-format and override old versions
sudo apt-get install clang-format-4.0
sed -i 's/clang-format /clang-format-4.0 /g' Makefile.am
git add Makefile.am

# Setup
ci/travis-setup.sh

# clang-format the file
make format

# did anything change?
git diff
if [[ $(git diff) ]]; then
    exit 1
else
    exit 0
fi
