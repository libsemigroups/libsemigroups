#!/bin/bash
set -e

if [[ $# -ne 0 ]]; then
  bold "error expected 0 arguments, got $#!"
  exit 1
fi

find include/libsemigroups/*.*pp -type f -exec clang-format -i --verbose {} \;
find include/libsemigroups/detail/*.*pp -type f -exec clang-format -i --verbose {} \;
find src/*.*pp -type f -exec clang-format -i --verbose {} \;
find src/detail/*.*pp -type f -exec clang-format -i --verbose {} \;
find tests/*.*pp -type f ! -iname "catch.hpp" -exec clang-format -i --verbose {} \;
find benchmarks/*.*pp -type f ! -iname "catch.hpp" -exec clang-format -i --verbose {} \;
find benchmarks/examples/*.*pp -type f -exec clang-format -i --verbose {} \;

exit 0
