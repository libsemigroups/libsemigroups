#!/bin/bash

if [[ $# -ne 0 ]]; then
  bold "error expected 0 arguments, got $#!"
  exit 1
fi

exit_code=0

cpplint --repository='include' include/libsemigroups/*.hpp || ((exit_code = 1))
cpplint --repository='include' --extensions='tpp' include/libsemigroups/*.tpp || ((exit_code = 1))
cpplint --repository='include' include/libsemigroups/detail/*.hpp || ((exit_code = 1))
cpplint --repository='include' --extensions='tpp' include/libsemigroups/detail/*.tpp || ((exit_code = 1))
cpplint --repository='include' src/*.cpp || ((exit_code = 1))
cpplint --repository='include' src/detail/*.cpp || ((exit_code = 1))
cpplint --repository='..' tests/*.hpp || ((exit_code = 1))
cpplint --repository='include' tests/*.cpp || ((exit_code = 1))
cpplint --repository='..' benchmarks/*.hpp || ((exit_code = 1))
cpplint --repository='include' benchmarks/*.cpp || ((exit_code = 1))

exit $exit_code
