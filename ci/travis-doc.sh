#!/bin/bash
set -e

# Install doxygen
echo "deb http://us.archive.ubuntu.com/ubuntu/ yakkety main" | sudo tee -a /etc/apt/sources.list
sudo apt-get update -qq
sudo apt-get install -y doxygen

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
