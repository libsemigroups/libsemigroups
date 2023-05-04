#!/usr/bin/env python3
"""
A simple script to generate the header file libsemigroups.hpp.
"""
# pylint: disable=invalid-name

import datetime
import os
import sys

year = datetime.date.today().year

head = f"""//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-{year} James D. Mitchell
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

// This file includes all the headers in libsemigroups, for convenience.

#ifndef LIBSEMIGROUPS_LIBSEMIGROUPS_HPP_
#define LIBSEMIGROUPS_LIBSEMIGROUPS_HPP_

"""

foot = """
# endif  // LIBSEMIGROUPS_LIBSEMIGROUPS_HPP_
"""

include_dir = "include/libsemigroups"

output = head
files = [file for file in os.listdir(include_dir) if file.endswith(".hpp")]
files.sort()

for file in files:
    output += f'#include "{file}"\n'

output += foot

with open(
    os.path.join(include_dir, "libsemigroups.hpp"), "w", encoding="utf-8"
) as f:
    print(f"Writing {include_dir}/libsemigroups.hpp . . .")
    f.write(output)

sys.exit(0)
