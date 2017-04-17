#!/bin/bash
# This script only exists to enable the appveyor tests to run. For whatever
# reason on appveyor (in cygwin) the check program lstest is compiled into an
# executable named lstest.exe, and so this 
set -e

if [ -f lstest ]; then
    echo "what the fuck"
    lstest "$@"
  elif [ -f lstest.exe ]; then
    lstest.exe "$@"
fi
