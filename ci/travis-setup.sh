#!/bin/bash
set -e

mkdir -p m4
./autogen.sh
./configure
