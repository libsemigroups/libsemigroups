#!/bin/bash
set -e

if [[ $# -ne 0 ]]; then
  bold "error expected 0 arguments, got $#!"
  exit 1
fi

find include/libsemigroups/*.hpp -type f -exec cpplint --repository='include' {} \; | tee lint.log
find src/*.hpp -type f -exec cpplint --repository='..' {} \; | tee -a lint.log
find src/*.cpp -type f -exec cpplint --repository='include' {} \; | tee -a lint.log 
find tests/*.hpp -type f ! -iname "catch.hpp" -exec cpplint --repository='..' {} \; | tee -a lint.log
find tests/*.cpp -type f -exec cpplint --repository='include' {} \; | tee -a lint.log
find benchmarks/*.hpp -type f ! -iname "catch.hpp" -exec cpplint --repository='..' {} \; | tee -a lint.log
find benchmarks/*.cpp -type f -exec cpplint --repository='include' {} \; | tee -a lint.log
find benchmarks/examples/*.hpp -type f -exec cpplint --repository='..' {} \; | tee -a lint.log
find benchmarks/examples/*.cpp -type f -exec cpplint {} \; | tee -a lint.log

! grep -E "Total errors found:" lint.log
exit_code=$?
rm -f lint.log
exit $exit_code
