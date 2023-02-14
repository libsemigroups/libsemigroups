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

#include "libsemigroups/cong-intf-new.hpp"

#include <algorithm>  // for remove_if

#include "libsemigroups/constants.hpp"          // for UNDEFINED
#include "libsemigroups/debug.hpp"              // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/exception.hpp"          // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/fpsemi-intf.hpp"        // for FpSemigroupInterface
#include "libsemigroups/froidure-pin-base.hpp"  // for FroidurePinBase
#include "libsemigroups/report.hpp"             // for REPORT_VERBOSE_DEFAULT
#include "libsemigroups/string.hpp"             // for detail::to_string

namespace libsemigroups {

  ////////////////////////////////////////////////////////////////////////////
  // v3::CongruenceInterface - constructors + destructor - public
  ////////////////////////////////////////////////////////////////////////////

  v3::CongruenceInterface::CongruenceInterface(congruence_kind type)
      : Runner(),
        // Non-mutable
        _type(type) {
    // Mutable
    // TODO reset();
  }

  void v3::CongruenceInterface::init() {
    Runner::init();
  }

  void v3::CongruenceInterface::init(congruence_kind type) {
    Runner::init();
    _type = type;
  }

  v3::CongruenceInterface::~CongruenceInterface() = default;

  ////////////////////////////////////////////////////////////////////////////
  // v3::CongruenceInterface - non-pure virtual methods - public
  ////////////////////////////////////////////////////////////////////////////

  tril v3::CongruenceInterface::const_contains(word_type const& u,
                                               word_type const& v) const {
    if (u == v) {
      return tril::TRUE;
    }
    class_index_type uu, vv;
    try {
      uu = const_word_to_class_index(u);
      vv = const_word_to_class_index(v);
    } catch (LibsemigroupsException const& e) {
      REPORT_VERBOSE_DEFAULT("ignoring exception:\n%s", e.what());
      return tril::unknown;
    }
    if (uu == UNDEFINED || vv == UNDEFINED) {
      return tril::unknown;
    } else if (uu == vv) {
      return tril::TRUE;
    } else if (finished()) {
      return tril::FALSE;
    } else {
      return tril::unknown;
    }
  }

  word_type v3::CongruenceInterface::class_index_to_word(class_index_type i) {
    if (i >= number_of_classes()) {
      LIBSEMIGROUPS_EXCEPTION("invalid class index, expected a value in the "
                              "range [0, %d), found %d",
                              number_of_classes(),
                              i);
    }
    return class_index_to_word_impl(i);
  }

  // Basic exception guarantee (since is_quotient_obviously_infinite() may
  // change the object).
  // TODO
  // std::shared_ptr<FroidurePinBase>
  // v3::CongruenceInterface::quotient_froidure_pin() {
  //   if (_quotient != nullptr) {
  //     LIBSEMIGROUPS_ASSERT(kind() == congruence_kind::twosided);
  //     return _quotient;
  //   } else if (kind() != congruence_kind::twosided) {
  //     LIBSEMIGROUPS_EXCEPTION("the congruence must be two-sided");
  //   }
  //   _quotient = quotient_impl();
  //   _quotient->immutable(true);
  //   return _quotient;
  // }

  size_t v3::CongruenceInterface::number_of_classes() {
    // FIXME since this passes right through it shouldn't exist at all
    return number_of_classes_impl();
  }

  v3::CongruenceInterface::class_index_type
  v3::CongruenceInterface::word_to_class_index(word_type const& word) {
    return word_to_class_index_impl(word);
  }

  /////////////////////////////////////////////////////////////////////////
  // v3::CongruenceInterface - non-pure virtual methods - private
  /////////////////////////////////////////////////////////////////////////

  v3::CongruenceInterface::class_index_type
  v3::CongruenceInterface::const_word_to_class_index(word_type const&) const {
    return UNDEFINED;
  }

  // TODO not in the right place in this file
  void v3::CongruenceInterface::add_pair(word_type const& u,
                                         word_type const& v) {
    if (started()) {
      LIBSEMIGROUPS_EXCEPTION(
          "cannot add further generating pairs at this stage");
    }
    validate_word(u);
    validate_word(v);
    // Note that _gen_pairs might contain pairs of distinct words that
    // represent the same element of the parent semigroup (if any).
    _generating_pairs.push_back(u);
    if (kind() == congruence_kind::left) {
      std::reverse(_generating_pairs.back().begin(),
                   _generating_pairs.back().end());
    }
    _generating_pairs.push_back(v);
    if (kind() == congruence_kind::left) {
      std::reverse(_generating_pairs.back().begin(),
                   _generating_pairs.back().end());
    }
  }

  void v3::CongruenceInterface::add_pair(word_type&& u, word_type&& v) {
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

  /////////////////////////////////////////////////////////////////////////
  // v3::CongruenceInterface - non-virtual methods - private
  /////////////////////////////////////////////////////////////////////////

  // TODO
  // void v3::CongruenceInterface::init_non_trivial_classes() {
  //   if (!_init_ntc_done) {
  //     _non_trivial_classes = non_trivial_classes_impl();
  //     _init_ntc_done       = true;
  //   }
  // }

  // TODO
  // void v3::CongruenceInterface::reset() noexcept {
  //   // set_finished(false);
  //   _non_trivial_classes.reset();
  //   _init_ntc_done = false;
  //   _quotient.reset();
  //   _is_obviously_finite   = false;
  //   _is_obviously_infinite = false;
  // }

  /////////////////////////////////////////////////////////////////////////
  // v3::CongruenceInterface - non-virtual static methods - protected
  /////////////////////////////////////////////////////////////////////////

  // TODO use magic_enum or at the very least move this into the helper
  // namespace
  // std::string const&
  // v3::CongruenceInterface::congruence_kind_to_string(congruence_kind typ) {
  //   switch (typ) {
  //     case congruence_kind::twosided:
  //       return STRING_TWOSIDED;
  //     case congruence_kind::left:
  //       return STRING_LEFT;
  //     case congruence_kind::right:
  //       return STRING_RIGHT;
  //     default:
  //       LIBSEMIGROUPS_EXCEPTION("incorrect type");
  //   }
  // }

  /////////////////////////////////////////////////////////////////////////
  // v3::CongruenceInterface - static data members - private
  /////////////////////////////////////////////////////////////////////////

  // const std::string v3::CongruenceInterface::STRING_TWOSIDED = "two-sided";
  // const std::string v3::CongruenceInterface::STRING_LEFT     = "left";
  // const std::string v3::CongruenceInterface::STRING_RIGHT    = "right";
}  // namespace libsemigroups
