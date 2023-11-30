#!/bin/bash
set -e

for file in tests/test-*.cpp; do
    executable="${file%.*}" # remove file extension
    executable=${executable##*/} # remove path
    # remove suffix from knuth-bendix-3.cpp
    executable=$(echo $executable | gsed -r 's/-[0-9]//g') 
    executable=$(echo $executable | gsed -r 's/test-konieczny-.*/test-konieczny/g') 
    # change test-action -> test_action
    executable=$(echo $executable | gsed -r 's/-/_/g') 
    if [ "$executable" != "test_fpsemi_intf" ] && [ "$executable" != "test_fpsemi" ] && [ "$executable" != "test_main" ]; then
      echo "Building $executable..."
      make "$executable"
      "./$executable" "[quick]"
    fi
done
