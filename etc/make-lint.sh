#!/bin/bash
set -e

if [[ $# -ne 0 ]]; then
  bold "error expected 0 arguments, got $#!"
  exit 1
fi

find include/*.hpp -type f -exec cpplint --repository='..' {} \; | tee lint.log
find src/*.hpp -type f -exec cpplint --repository='..' {} \; | tee --append lint.log
find src/*.cpp -type f -exec cpplint --repository='..' {} \; | tee --append lint.log 
find tests/*.hpp -type f ! -iname "catch.hpp" -exec cpplint --repository='..' {} \; | tee --append lint.log
find tests/*.cpp -type f -exec cpplint --repository='..' {} \; | tee --append lint.log

! grep -E "Total errors found:" lint.log
exit_code=$?
rm -f lint.log
exit $exit_code
