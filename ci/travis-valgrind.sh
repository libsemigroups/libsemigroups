#!/bin/bash
set -e

# Print version
echo "Valgrind version:"
valgrind --version

ci/travis-setup.sh
./configure --enable-debug --disable-hpcombi
make test_all -j4

unbuffer libtool --mode=execute valgrind --leak-check=full ./test_all "[quick][exclude:no-valgrind]" 2>&1 | tee valgrind.log

echo
( ! grep -i "Invalid" valgrind.log )
( ! grep -E "lost: [^0]" valgrind.log )
