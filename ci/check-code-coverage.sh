#!/bin/bash
set -e

if [[ $# -ne 1 ]]; then
  echo "Error: expected 1 argument, got $#"; 
  exit 1
elif [[ -f $1 && ! -x $1 ]]; then 
  echo "Error: expected an executable, $1 is not executable"; 
  exit 1
fi

if ! [[ -x configure ]]; then 
  ./autogen.sh;
fi
if ! [[ -f config.log ]]; then
  ./configure --enable-code-coverage;
elif grep -q \"--enable-code-coverage\" config.log; then 
  ./configure --enable-code-coverage;
fi

make $1 -j4
if [[ ! -f $1 || ! -x $1 ]]; then 
  echo "Error: can't find the executable $1"; 
  exit 1
fi
find . -name '*.gcda' -delete

./$1 "[quick]"
lcov  --directory . --capture --output-file "coverage.info.tmp" --test-name "libsemigroups_1_0_0" --no-checksum --no-external --compat-libtool --gcov-tool "gcov" | grep -v "ignoring data for external file"
lcov  --directory . --remove "coverage.info.tmp" "/tmp/*" "/Applications/*" --output-file "coverage.info"
LANG=C genhtml  --prefix . --output-directory "coverage" --title "libsemigroups Code Coverage" --legend --show-details "coverage.info"

fnam=$1
fnam=${fnam#*_}
fnam=${fnam%.*}
dir=$(pwd)
hpp=coverage${dir}/include/${fnam}.hpp.gcov.html
echo "See: $hpp"
cpp=coverage${dir}/src/${fnam}.cpp.gcov.html
echo "Or:  $cpp"

exit 0
