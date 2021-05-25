#!/bin/sh -ex
#
# libsemigroups - C++ library for semigroups and monoids
# Copyright (C) 2019 James D. Mitchell
#
# This file is part of the build system of libsemigroups.
# Requires GNU autoconf, GNU automake and GNU libtool.
#
autoreconf -if `dirname "$0"`
