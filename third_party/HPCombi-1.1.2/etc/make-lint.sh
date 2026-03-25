#!/bin/bash

if [[ $# -ne 0 ]]; then
  bold "error expected 0 arguments, got $#!"
  exit 1
fi

exit_code=0

cpplint --repository='include' include/hpcombi/*.hpp || ((exit_code = 1))

exit $exit_code
