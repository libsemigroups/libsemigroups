#!/bin/bash
set -e

if [[ $# -ne 0 ]]; then
  bold "error expected 0 arguments, got $#!"
  exit 1
fi

find include/libsemigroups/*.hpp -type f -exec clang-format -i --verbose {} \;
find src/*.hpp -type f -exec clang-format -i --verbose {} \;
find src/*.cpp -type f -exec clang-format -i --verbose {} \;
find tests/*.hpp -type f ! -iname "catch.hpp" -exec clang-format -i --verbose {} \;
find tests/*.cpp -type f -exec clang-format -i --verbose {} \;
find benchmarks/*.hpp -type f ! -iname "catch.hpp" -exec clang-format -i --verbose {} \;
find benchmarks/*.cpp -type f -exec clang-format -i --verbose {} \;
find benchmarks/examples/*.hpp -type f -exec clang-format -i --verbose {} \;
find benchmarks/examples/*.cpp -type f -exec clang-format -i --verbose {} \;

exit 0
