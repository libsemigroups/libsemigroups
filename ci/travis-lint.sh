#!/bin/bash
set -e
set -o pipefail

# Install cpplint
sudo pip install cpplint

# Lint all files...
etc/make-lint.sh
