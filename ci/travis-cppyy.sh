#!/bin/bash
set -e

# Setup
ci/travis-setup.sh

./configure
make -j2 && sudo make install 

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
export CFLAGS="$CFLAGS -I$/usr/local/include -O2 -g"
export LDFLAGS="$LDFLAGS -L/usr/local/lib"
export STDCXX=11

sudo -H python3 -m pip install --upgrade pip
sudo -H python3 -m pip install setuptools
sudo -H python3 -m pip install cppyy
sudo -H python3 -m pip install tox

git clone -b master --depth=1 https://github.com/libsemigroups/libsemigroups_cppyy
cd libsemigroups_cppyy 
make check 
