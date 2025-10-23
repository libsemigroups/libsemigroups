//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025 James D. Mitchell
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

// This file contains the implementations of some things from word-graph.hpp.

#include "libsemigroups/word-graph.hpp"

namespace libsemigroups {

  // TODO(v4) delete this file
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
  Meeter::Meeter()                         = default;
  Meeter::Meeter(Meeter const&)            = default;
  Meeter::Meeter(Meeter&&)                 = default;
  Meeter& Meeter::operator=(Meeter const&) = default;
  Meeter& Meeter::operator=(Meeter&&)      = default;

  Meeter::~Meeter() = default;

  Joiner::Joiner()                         = default;
  Joiner::Joiner(Joiner const&)            = default;
  Joiner::Joiner(Joiner&&)                 = default;
  Joiner& Joiner::operator=(Joiner const&) = default;
  Joiner& Joiner::operator=(Joiner&&)      = default;

  Joiner::~Joiner() = default;
#pragma GCC diagnostic pop
}  // namespace libsemigroups
