#!/bin/bash
set -e

# Setup
ci/travis-setup.sh

# Install libtool and GMP
echo "deb http://us.archive.ubuntu.com/ubuntu/ xenial main" | sudo tee -a /etc/apt/sources.list
sudo apt-get update -qq
sudo apt-get install libtool libgmp-dev

INITIALDIR=`pwd`

# Download and compile GAP
cd ..
# The next line is required by semigroups/scripts/travis-test.sh
export GAPROOT=`pwd`/gap
git clone -b $GAP_BRANCH --depth=1 https://github.com/$GAP_FORK/gap.git gap
cd gap
./autogen.sh
./configure --with-gmp=system $GAP_FLAGS
make -j2
mkdir pkg
cd pkg

# Download and install packages

# Semigroups
git clone -b $SEMIGROUPS_BR --depth=1 https://github.com/gap-packages/Semigroups.git semigroups
cd semigroups
# Move the libsemigroups to the correct location
mv $INITIALDIR . 
./autogen.sh
./configure $PKG_FLAGS
make -j2

DIGRAPHS_VERS=$(perl -lne '/"digraphs",\s*">=(\d+.\d+.\d+)/ && print $1' PackageInfo.g)
GENSS_VERS=$(perl -lne '/"genss",\s*">=(\d+.\d+.\d+)/ && print $1' PackageInfo.g)
ORB_VERS=$(perl -lne '/"orb",\s*">=(\d+.\d+.\d+)/ && print $1' PackageInfo.g)
IO_VERS=$(perl -lne '/"io",\s*">=(\d+.\d+.\d+)/ && print $1' PackageInfo.g)
cd ..

# Digraphs
DIGRAPHS=digraphs-$DIGRAPHS_VERS
echo "Downloading $DIGRAPHS . . ."
curl -L -O https://github.com/gap-packages/digraphs/releases/download/v$DIGRAPHS_VERS/$DIGRAPHS.tar.gz
tar xzf $DIGRAPHS.tar.gz
rm $DIGRAPHS.tar.gz
cd $DIGRAPHS
./configure $PKG_FLAGS
make -j2
cd .. # pkg

IO=io-$IO_VERS
echo "Downloading $IO . . ."
curl -L -O https://github.com/gap-packages/io/releases/download/v$IO_VERS/$IO.tar.gz
tar xzf $IO.tar.gz
rm $IO.tar.gz
cd $IO
./configure $PKG_FLAGS
make -j2
cd .. # pkg

# Orb
ORB=orb-$ORB_VERS
echo "Downloading $ORB..."
curl -L -O https://github.com/gap-packages/orb/releases/download/v$ORB_VERS/$ORB.tar.gz
tar xzf $ORB.tar.gz
rm $ORB.tar.gz
cd $ORB
./configure $PKG_FLAGS
make -j2
cd ..

# GenSS
GENSS=genss-$GENSS_VERS
echo "Downloading $GENSS..."
curl -L -O https://www.gap-system.org/pub/gap/gap4/tar.gz/packages/$GENSS.tar.gz
tar xzf $GENSS.tar.gz
rm $GENSS.tar.gz

################################################################################
# Install required GAP packages

curl -L -O http://www.gap-system.org/pub/gap/gap4pkgs/packages-required-master.tar.gz
tar xf packages-required-master.tar.gz
rm packages-required-master.tar.gz

# Run the tests defined in Semigroups
cd semigroups
# The next line is needed by scripts/travis-test.sh
export SUITE="test"
scripts/travis-test.sh
