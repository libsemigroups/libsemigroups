#!/bin/bash
set -e

# Install cpplint
sudo pip install cpplint

# Setup
ci/travis-setup.sh

# Stop cpplint complaining about the header guards
cd ..
mkdir -p gap/pkg/semigroups/src
cd gap
git init
cd ..
mv libsemigroups gap/pkg/semigroups/src
cd gap/pkg/semigroups/src/libsemigroups

# Lint the file
make lint
