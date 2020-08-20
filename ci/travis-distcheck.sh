#!/bin/bash
set -e

ci/travis-setup.sh

./configure
make distcheck
