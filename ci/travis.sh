#!/bin/bash
set -e

mkdir -p m4
./autogen.sh
./configure

# These lines stop cpplint complaining about the header guards
cd ..
mkdir -p gap/pkg/semigroups/src
cd gap
git init
cd ..
mv libsemigroups gap/pkg/semigroups/src
cd gap/pkg/semigroups/src/libsemigroups
make lint

# Check standard tests standard build
make check-standard -j

# Check make 
make distcheck

# Check standard tests with assertions on
etc/debug.sh
make check-standard -j

# Check standard tests with assertions on
etc/ndebug.sh
make check-standard -j

# Check the documentation (the version of doxygen installed on travis is too
# old), and this is currently too much work to install

# DOXYVERS = 1.8.14
# gunzip doxygen-$DOXYVERS.src.tar.gz    # uncompress the archive
# tar xf doxygen-$DOXYVERS.src.tar       # unpack it
# cd doxygen-$DOXYVERS
# mkdir build
# cd build
# cmake -G "Unix Makefiles" ..
# make
# ./configure
# make install

#touch log/make-doc-log.txt
#make doc 2>&1 | tee log/make-doc-log.txt
#( ! grep -E "warning:" log/make-doc-log.txt )

# The following checks code coverage but for some reason on travis runs out of
# memory at present
#etc/coverage.sh
#for d in log; do
#  if [ -d $d ]; then 
#    etc/lcov-summary.py $d | tee log/coverage-log.txt
#    break
#  fi;
#done;
#( ! grep -q -E "Warning!" log/coverage-log.txt )
