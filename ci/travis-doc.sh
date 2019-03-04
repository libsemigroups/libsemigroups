#!/bin/bash
set -e

# Install doxygen
curl -L -O http://doxygen.nl/files/doxygen-1.8.15.src.tar.gz
tar -xf doxygen-1.8.15.src.tar.gz

# From http://www.linuxfromscratch.org/blfs/view/8.3-systemd/general/doxygen.html
cd doxygen-1.8.15/
mkdir -v build &&
cd       build &&

cmake -G "Unix Makefiles"         \
      -DCMAKE_BUILD_TYPE=Release  \
      -DCMAKE_INSTALL_PREFIX=/usr \
      -Wno-dev .. &&
make
sudo make install
cd ../..

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
