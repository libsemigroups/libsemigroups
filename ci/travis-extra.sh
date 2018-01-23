#!/bin/bash
set -e

# Check configure with --enable-densehashmap and densehashmap not available
echo -e "\nConfiguring with --enable-densehashmap with sparsehash not installed . . ."
./autogen.sh && ./configure --enable-densehashmap && make check-quick -j2

# Check configure with --enable-googlehashmap and googlehashmap available
echo -e "\nDownloading sparsehash . . ."
git clone -b master --depth=1 https://github.com/sparsehash/sparsehash-c11.git extern/sparsehash
cd extern/sparsehash
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Release  ..
sudo make install
sudo mv /usr/local/include/google/sparsehash/ /usr/local/include/
cd ../../..

echo -e "\nConfiguring with --enable-densehashmap with sparsehash installed . . ."
make clean && ./configure --enable-densehashmap && make check-quick -j2

# Check configure with --enable-hpcombi and hpcombi not available
echo -e "\nConfiguring with --enable-hpcombi with hpcombi not installed . . ."
make clean && ./configure --enable-hpcombi && make check-quick -j2

# Get hpcombi version from file
if [ -f extern/.HPCombi_VERSION ]; then
  HPCOMBI=`tr -d '\n' < extern/.HPCombi_VERSION`
else
  echo -e "\nError, cannot find extern/.HPCombi_VERSION"
  exit 1
fi

echo -e "\nDownloading HPCombi $HPCombi. . ."
curl -L -O https://github.com/hivert/HPCombi/releases/download/v$HPCOMBI/HPCombi-$HPCOMBI.tar.gz
tar xzf HPCombi-$HPCOMBI.tar.gz
rm HPCombi-$HPCOMBI.tar.gz
mv HPCombi-$HPCOMBI extern/HPCombi

echo -e "\nConfiguring with --enable-hpcombi with hpcombi installed . . ."
make clean && ./configure --enable-hpcombi && make check-quick -j2
