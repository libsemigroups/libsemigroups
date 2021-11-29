#!/bin/bash
# This entire script should be executed in the libsemigroups container
set -e

sudo chown -R libsemigroups: /home/libsemigroups
cd /home/libsemigroups
mkdir -p m4
./autogen.sh
./configure
make "$GH_ACTIONS_TEST_PROG" -j4
./"$GH_ACTIONS_TEST_PROG" "$GH_ACTIONS_TEST_TAGS" --abort
