#!/bin/bash
set -e

# Install valgrind
sudo apt-get install -y valgrind

# Setup
ci/travis-setup.sh

# Print version
echo "Valgrind version:"
valgrind --version

./configure --enable-debug
make test_all -j2

if [ ! -e log ]
then
    mkdir log
fi
touch log/valgrind-log.txt

libtool --mode=execute valgrind --leak-check=full ./test_all -d yes [quick] 2>&1 | tee log/valgrind-log.txt

echo
( ! grep -i "Invalid" log/valgrind-log.txt )
( ! grep -E "lost: [^0]" log/valgrind-log.txt )
