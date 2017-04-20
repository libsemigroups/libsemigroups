#!/bin/bash
set -e

if [ ! -f .debug_build ]; then
  if [ -f Makefile ]; then 
    make distclean
  else 
    ./autogen.sh
  fi
  ./configure CPPFLAGS='-UNDEBUG -DDEBUG' CXXFLAGS='-g3 -O0 -std=c++11 -Wall -Wextra -Wno-unused-parameter -Wtype-limits -Wformat-security -Wpointer-arith -Wno-div-by-zero -Wreturn-type -Wswitch -Wsizeof-array-argument -Wunused-label -fno-omit-frame-pointer -fno-omit-frame-pointer -Wno-c++11-extensions' CFLAGS='-g3 -O0 -Wall -Wextra -Wno-unused-parameter -Wtype-limits -Wformat-security -Wpointer-arith -Wno-div-by-zero -Wreturn-type -Wswitch -Wsizeof-array-argument -Wunused-label -fno-omit-frame-pointer -fno-omit-frame-pointer'
  touch .debug_build
fi
