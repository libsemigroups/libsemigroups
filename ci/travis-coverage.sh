#!/bin/bash
set -e

sudo ln -sf /usr/bin/gcov-7 /usr/bin/gcov
export GCOV=/usr/bin/gcov-7

ci/travis-setup.sh

etc/test-code-coverage.sh test_all "[quick]"
bash <(curl -s https://codecov.io/bash)
rm -f coverage.info
