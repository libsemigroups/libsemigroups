//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 James D. Mitchell
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

#include "cong-common-class.hpp"

#include <string>

#include "libsemigroups/exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/runner.hpp"     // for Runner

namespace libsemigroups {

  CongruenceCommon::~CongruenceCommon() = default;

  CongruenceCommon::CongruenceCommon(CongruenceCommon const&) = default;
  CongruenceCommon::CongruenceCommon(CongruenceCommon&&)      = default;

  CongruenceCommon& CongruenceCommon::operator=(CongruenceCommon const&)
      = default;
  CongruenceCommon& CongruenceCommon::operator=(CongruenceCommon&&) = default;

  /////////////////////////////////////////////////////////////////////////
  // CongruenceCommon - non-pure virtual methods - private
  /////////////////////////////////////////////////////////////////////////

  void CongruenceCommon::throw_if_started() const {
    if (started()) {
      LIBSEMIGROUPS_EXCEPTION(
          "cannot add further generating pairs at this stage");
    }
  }

}  // namespace libsemigroups
