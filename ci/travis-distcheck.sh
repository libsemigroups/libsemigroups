#!/bin/bash
set -e

ci/travis-setup.sh
ci/travis-setup-hpcombi.sh
./configure
make distcheck
