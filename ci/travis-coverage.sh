#!/bin/bash
set -e

ci/travis-setup.sh

sudo ln -sf /usr/bin/gcov-5 /usr/bin/gcov

etc/test-code-coverage.sh test_all "[quick]"
bash <(curl -s https://codecov.io/bash)
rm -f coverage.info
