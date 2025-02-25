#!/bin/bash
set -e

if [[ $# -ne 0 ]]; then
  bold "error expected 0 arguments, got $#!"
  exit 1
fi

find include/libsemigroups/*.hpp -type f -exec cpplint --repository='include' {} \; | tee lint.log
find include/libsemigroups/*.tpp -type f -exec cpplint --repository='include' --extensions='tpp' {} \; | tee -a lint.log
find include/libsemigroups/detail/*.hpp -type f -exec cpplint --repository='include' {} \; | tee lint.log
find include/libsemigroups/detail/*.tpp -type f -exec cpplint --repository='include' --extensions='tpp' {} \; | tee -a lint.log
find src/*.cpp -type f -exec cpplint --repository='include' {} \; | tee -a lint.log
find src/detail/*.cpp -type f -exec cpplint --repository='include' {} \; | tee -a lint.log
find tests/*.hpp -type f ! -iname "catch_*" -exec cpplint --repository='..' {} \; | tee -a lint.log
find tests/*.cpp -type f ! -iname "catch_*" -exec cpplint --repository='include' {} \; | tee -a lint.log
find benchmarks/*.hpp -type f ! -iname "catch_*" -exec cpplint --repository='..' {} \; | tee -a lint.log
find benchmarks/*.cpp -type f ! -iname "catch_*" -exec cpplint --repository='include' {} \; | tee -a lint.log

! grep -E "Total errors found:" lint.log
exit_code=$?
rm -f lint.log
exit $exit_code
