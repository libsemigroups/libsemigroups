#!/bin/bash
set -e

exit 0

sudo apt-get --yes update

sudo apt-get install git --yes
sudo apt-get install libtool-bin --yes

# Next commands executed in the container...
GAP_SEMIGROUPS_BRANCH="stable-5.2"
GAP_SEMIGROUPS_REPO="semigroups"
GAP_VERSION=$(ls inst)
GAP_SH="$HOME/inst/$GAP_VERSION/bin/gap.sh"

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
./configure --disable-hpcombi
make -j4

cd ..
git clone -b master --depth=1 https://github.com/gap-packages/PackageManager.git

INSTALL_PKGS="if not InstallPackage(\"https://digraphs.github.io/Digraphs/PackageInfo.g\", false) then QuitGap(1); fi;"
INSTALL_PKGS+="if not InstallPackage(\"io\", false) then QuitGap(1); fi;"
INSTALL_PKGS+="if not (InstallPackage(\"orb\", false) and CompilePackage(\"orb\")) then QuitGap(1); fi;"
INSTALL_PKGS+="if not InstallPackage(\"genss\", false) then QuitGap(1); fi;"
INSTALL_PKGS+="if not InstallPackage(\"images\", false) then QuitGap(1); fi;"
INSTALL_PKGS+="if not InstallPackage(\"autodoc\", false) then QuitGap(1); fi;"

echo "LoadPackage(\"PackageManager\"); InstallPackage(\"PackageManager\", false); $INSTALL_PKGS QUIT;" | $GAP_SH -A -T || exit 1
cd $HOME/inst/$GAP_VERSION/pkg/semigroups

export ABI="64"
# Run the tests defined in Semigroups
ci/run-tests.sh
