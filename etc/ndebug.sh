#!/bin/bash
set -e

if [ -f .debug_build ] || [ -f .coverage_build ]; then
  if [ -f Makefile ]; then 
    make clean
  fi
  ./configure
fi
