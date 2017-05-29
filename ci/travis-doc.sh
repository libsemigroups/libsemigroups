#!/bin/bash
set -e

# Install doxygen
wget -q ftp://ftp.stack.nl/pub/users/dimitri/doxygen-1.8.13.linux.bin.tar.gz
tar -xf doxygen-1.8.13.linux.bin.tar.gz
cd doxygen-1.8.13/
./configure
head -n 15 Makefile > temp.txt
mv temp.txt Makefile
sudo make install
cd ..

# Setup
ci/travis-setup.sh

# Print version
echo "Doxygen version:"
doxygen --version

# make doc and look for warnings
if [ ! -e log ]
then
    mkdir log
fi
touch log/make-doc-log.txt
make doc 2>&1 | tee log/make-doc-log.txt

echo
( ! grep -i "warning" log/make-doc-log.txt )
