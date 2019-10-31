#!/bin/bash
set -e

# JDM: the following lines are commented out because I couldn't make it work,
# on travis I get: 
# 
#   UnsatisfiableError: The following specifications were found to be
#   incompatible with each other:
# 
# from conda. 
# 
# if [[ "$TRAVIS_OS_NAME" == "osx" && "$TRAVIS_COMPILER" == "gcc" ]]; then 
#       wget https://repo.anaconda.com/miniconda/Miniconda3-latest-MacOSX-x86_64.sh -O miniconda.sh;
#       chmod +x miniconda.sh
#       ./miniconda.sh -b
#       export PATH=$HOME/miniconda3/bin:$PATH
#       conda update --yes conda
#       conda install -c psi4 gcc-5
# fi

if ! [[ -z "$CXX" ]]; then
  echo "g++ version:"
  $CXX --version
fi

if ! [[ -z "$CC" ]]; then
  echo "gcc version:"
  $CC --version
fi

mkdir -p m4

cd extern
curl -L -O https://github.com/fmtlib/fmt/archive/5.3.0.tar.gz
tar -xzf 5.3.0.tar.gz && rm -f 5.3.0.tar.gz && cd ..

./autogen.sh
