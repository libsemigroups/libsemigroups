#!/bin/bash
set -e

# This is required to generate the config/config.h file which is included in
# libsemigroups-debug.h
./autogen.sh ; ./configure

# Install libtool and GMP
echo "deb http://us.archive.ubuntu.com/ubuntu/ xenial main" | sudo tee -a /etc/apt/sources.list
sudo apt-get update -qq
sudo apt-get install libtool libgmp-dev

INITIALDIR=`pwd`

# Download and compile GAP
cd ..
git clone -b $GAP_BRANCH --depth=1 https://github.com/$GAP_FORK/gap.git gap
cd gap
./autogen.sh
./configure --with-gmp=system $GAP_FLAGS
make
mkdir pkg
cd pkg

# Download and install packages

# Semigroups
git clone -b $SEMIGROUPS_BR --depth=1 https://github.com/gap-packages/Semigroups.git semigroups
cd semigroups
# Move the libsemigroups to the correct location
mv $INITIALDIR src/libsemigroups
./autogen.sh
./configure $PKG_FLAGS
make
cd ..

# Digraphs
git clone -b $DIGRAPHS_BR --depth=1 https://github.com/gap-packages/Digraphs.git digraphs
cd digraphs
./autogen.sh
./configure $PKG_FLAGS
make
cd ..

# GenSS
GENSS=genss-$GENSSVERS
echo "Downloading $GENSS..."
curl -L -O https://www.gap-system.org/pub/gap/gap4/tar.gz/packages/$GENSS.tar.gz
tar xzf $GENSS.tar.gz
rm $GENSS.tar.gz

# IO
IO=io-$IOVERS
echo "Downloading $IO..."
curl -L -O https://github.com/gap-packages/io/releases/download/v$IOVERS/$IO.tar.gz
tar xzf $IO.tar.gz
rm $IO.tar.gz
cd $IO
./configure $PKG_FLAGS
make
cd ..

# Orb
ORB=orb-$ORBVERS
echo "Downloading $ORB..."
curl -L -O https://github.com/gap-packages/orb/releases/download/v$ORBVERS/$ORB.tar.gz
tar xzf $ORB.tar.gz
rm $ORB.tar.gz
cd $ORB
./configure $PKG_FLAGS
make
cd ..

################################################################################
# Install required GAP packages

curl -L -O http://www.gap-system.org/pub/gap/gap4pkgs/packages-required-master.tar.gz
tar xf packages-required-master.tar.gz
rm packages-required-master.tar.gz

# Run the tests defined in Semigroups
cd semigroups
scripts/travis-test.sh
