#!/bin/bash
set -e

if [ -f .debug_build ] || [ -f .coverage_build ]; then
  make clean
  ./configure
fi
