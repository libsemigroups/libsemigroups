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

#include "libsemigroups/cong-intf-class.hpp"

#include <string>

#include "libsemigroups/exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION

namespace libsemigroups {

  CongruenceInterface::~CongruenceInterface() = default;

  CongruenceInterface::CongruenceInterface(CongruenceInterface const&)
      = default;
  CongruenceInterface::CongruenceInterface(CongruenceInterface&&) = default;

  CongruenceInterface&
  CongruenceInterface::operator=(CongruenceInterface const&)
      = default;
  CongruenceInterface& CongruenceInterface::operator=(CongruenceInterface&&)
      = default;

  /////////////////////////////////////////////////////////////////////////
  // CongruenceInterface - non-pure virtual methods - private
  /////////////////////////////////////////////////////////////////////////

  void CongruenceInterface::throw_if_started() const {
    if (started()) {
      LIBSEMIGROUPS_EXCEPTION(
          "cannot add further generating pairs at this stage");
    }
  }

}  // namespace libsemigroups
