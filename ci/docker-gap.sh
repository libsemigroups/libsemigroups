#!/bin/bash
set -e

sudo apt-get --yes update
sudo apt-get --yes upgrade

sudo apt-get install git --yes
sudo apt-get install libtool-bin --yes 

# Next commands executed in the container...
GAP_SEMIGROUPS_BRANCH=konieczny
GAP_SEMIGROUPS_REPO=james-d-mitchell
GAP_VERSION=$(ls inst)
export GAPROOT=$HOME/inst/$GAP_VERSION
GAP_SH="$GAPROOT/bin/gap.sh"

sudo chown 1000:1000 -R libsemigroups/
if [ -x libsemigroups/etc/version-number.sh ] && [ ! -f libsemigroups/.VERSION ]; then
  cd libsemigroups
  etc/version-number.sh > .TMP_VERSION
  mv .TMP_VERSION .VERSION
  cd ..
fi

# Remove packaged GAP version of Semigroups package
rm -rf $HOME/inst/$GAP_VERSION/pkg/semigroups-*.*.*

# Clone Semigroups
cd $HOME/inst/$GAP_VERSION/pkg/

git clone -b $GAP_SEMIGROUPS_BRANCH --depth=1 https://github.com/$GAP_SEMIGROUPS_REPO/Semigroups.git semigroups
cd semigroups
# Move the libsemigroups to the correct location
mv $HOME/libsemigroups . 
./autogen.sh
./configure
make -j4

cd ..
git clone -b master --depth=1 https://github.com/gap-packages/PackageManager.git 

INSTALL_PKGS="InstallPackage(\"digraphs\", false); "
INSTALL_PKGS+="InstallPackage(\"io\", false); "
INSTALL_PKGS+="InstallPackage(\"orb\", false); "
INSTALL_PKGS+="InstallPackage(\"genss\", false); "
INSTALL_PKGS+="InstallPackage(\"images\", false);"

echo "LoadPackage(\"PackageManager\"); InstallPackage(\"PackageManager\", false); $INSTALL_PKGS QUIT;" | $GAP_SH -A -r -T
# The next line is a hack until the package manager compiles Orb
cd $HOME/.gap/pkg/orb-* && ./configure $GAPROOT && make
cd $HOME/inst/$GAP_VERSION/pkg/semigroups

# The next lines are required by semigroups/scripts/travis-test.sh
export SUITE="test"

# Run the tests defined in Semigroups
scripts/travis-test.sh
