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

#include "libsemigroups/cong-intf.hpp"

#include <algorithm>  // for remove_if

#include "libsemigroups/constants.hpp"          // for UNDEFINED
#include "libsemigroups/debug.hpp"              // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/exception.hpp"          // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/froidure-pin-base.hpp"  // for FroidurePinBase
#include "libsemigroups/report.hpp"             // for REPORT_VERBOSE_DEFAULT
#include "libsemigroups/string.hpp"             // for detail::to_string

namespace libsemigroups {

  ////////////////////////////////////////////////////////////////////////////
  // CongruenceInterface - constructors + destructor - public
  ////////////////////////////////////////////////////////////////////////////

  CongruenceInterface::CongruenceInterface(congruence_kind type)
      : Runner(),
        // Non-mutable
        _type(type) {
    // Mutable
    // TODO reset();
  }

  void CongruenceInterface::init() {
    Runner::init();
  }

  void CongruenceInterface::init(congruence_kind type) {
    Runner::init();
    _type = type;
  }

  CongruenceInterface::~CongruenceInterface() = default;

  /////////////////////////////////////////////////////////////////////////
  // CongruenceInterface - non-pure virtual methods - private
  /////////////////////////////////////////////////////////////////////////

  void CongruenceInterface::add_pair(word_type const& u,
                                         word_type const& v) {
    if (started()) {
      LIBSEMIGROUPS_EXCEPTION(
          "cannot add further generating pairs at this stage");
    }
    for (auto const& w : {u, v}) {
      validate_word(w);
      _generating_pairs.push_back(w);
      if (kind() == congruence_kind::left) {
        std::reverse(_generating_pairs.back().begin(),
                     _generating_pairs.back().end());
      }
    }
  }

  void
  CongruenceInterface::add_pair_no_checks_no_reverse(word_type const& u,
                                                         word_type const& v) {
    _generating_pairs.push_back(u);
    _generating_pairs.push_back(v);
  }

  void CongruenceInterface::add_pair(word_type&& u, word_type&& v) {
    if (started()) {
      LIBSEMIGROUPS_EXCEPTION(
          "cannot add further generating pairs at this stage");
    }
    validate_word(u);
    validate_word(v);
    // Note that _gen_pairs might contain pairs of distinct words that
    // represent the same element of the parent semigroup (if any).
    if (kind() == congruence_kind::left) {
      std::reverse(u.begin(), u.end());
      std::reverse(v.begin(), v.end());
    }

    _generating_pairs.push_back(std::move(u));
    _generating_pairs.push_back(std::move(v));
  }
}  // namespace libsemigroups
