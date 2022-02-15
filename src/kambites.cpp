//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2021 James D. Mitchell + Maria Tsalakou
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

// This file contains the implementation of the KambitesImpl class.

#include "libsemigroups/kambites.hpp"

#include <algorithm>  // for max, find_if, equal, copy, min
#include <string>     // for string
#include <tuple>      // for tie, tuple

#include <iostream>

#include "libsemigroups/cong-intf.hpp"  // for CongruenceInterface
#include "libsemigroups/constants.hpp"  // for UNDEFINED, POSITIVE_INFINITY
#include "libsemigroups/debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/froidure-pin-base.hpp"  // for FroidurePinBase
#include "libsemigroups/froidure-pin.hpp"       // for FroidurePin
#include "libsemigroups/int-range.hpp"          // for IntegralRange
#include "libsemigroups/order.hpp"              // for lexicographical_compare
#include "libsemigroups/string.hpp"  // for is_prefix, maximum_common_suffix
#include "libsemigroups/types.hpp"   // for word_type, tril
#include "libsemigroups/uf.hpp"      // for Duf<>
#include "libsemigroups/word.hpp"    // for word_to_string

namespace libsemigroups {
  using MultiStringView = std::string;  // detail::MultiStringView;

  using froidure_pin_type = FroidurePin<
      detail::KE,
      FroidurePinTraits<detail::KE, fpsemigroup::Kambites<std::string>>>;

  namespace detail {

    KE::KE(KE::string_type const& w) : _string(w) {}
    KE::KE(KE::string_type&& w) : _string(std::move(w)) {}

    bool KE::operator==(KE const& that) const {
      return that._string == this->_string;
    }

    bool KE::operator<(KE const& that) const {
      return shortlex_compare(_string, that._string);
    }

    void KE::swap(KE& x) {
      std::swap(x._string, _string);
    }

    typename KE::string_type const& KE::string() const noexcept {
      return _string;
    }

  }  // namespace detail

  namespace congruence {
    using class_index_type = CongruenceInterface::class_index_type;

    ////////////////////////////////////////////////////////////////////////////
    // Kambites - constructors - public
    ////////////////////////////////////////////////////////////////////////////

    Kambites::Kambites()
        : CongruenceInterface(congruence_kind::twosided),
          _k(std::make_unique<fpsemigroup::Kambites<std::string>>()) {}

    Kambites::Kambites(fpsemigroup::Kambites<std::string> const& k)
        : Kambites() {
      if (!k.alphabet().empty()) {
        _k->set_alphabet(k.alphabet());
        set_number_of_generators(_k->alphabet().size());
      }
      for (auto it = k.cbegin_rules(); it < k.cend_rules(); ++it) {
        add_pair(k.string_to_word(it->first), k.string_to_word(it->second));
      }
    }

    Kambites::~Kambites() = default;

    ////////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - non-pure virtual methods - public
    ////////////////////////////////////////////////////////////////////////////

    tril Kambites::const_contains(word_type const& lhs,
                                  word_type const& rhs) const {
      validate_word(lhs);
      validate_word(rhs);
      if (lhs == rhs) {
        return tril::TRUE;
      } else if (_k->small_overlap_class() < 3) {
        return tril::unknown;
      }

      std::string u, v;
      detail::word_to_string(_k->alphabet(), lhs, u);
      detail::word_to_string(_k->alphabet(), rhs, v);
      return (_k->equal_to(u, v) ? tril::TRUE : tril::FALSE);
    }

    bool Kambites::is_quotient_obviously_finite_impl() {
      return false;
    }

    bool Kambites::contains(word_type const& lhs, word_type const& rhs) {
      return const_contains(lhs, rhs) == tril::TRUE;
    }

    bool Kambites::is_quotient_obviously_infinite_impl() {
      return kambites().is_obviously_infinite();
    }

    ////////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - pure virtual methods - private
    ////////////////////////////////////////////////////////////////////////////

    word_type Kambites::class_index_to_word_impl(class_index_type i) {
      return _k->froidure_pin()->minimal_factorisation(i);
    }

    size_t Kambites::number_of_classes_impl() {
      run();  // required so that the state of this is correctly set.
      return _k->size();
    }

    std::shared_ptr<FroidurePinBase> Kambites::quotient_impl() {
      return _k->froidure_pin();
    }

    class_index_type Kambites::word_to_class_index_impl(word_type const& word) {
      auto   S   = static_cast<froidure_pin_type*>(_k->froidure_pin().get());
      size_t pos = S->position(detail::KE(*_k, word));
      LIBSEMIGROUPS_ASSERT(pos != UNDEFINED);
      return pos;
    }

    void Kambites::run_impl() {
      auto stppd = [this]() -> bool { return stopped(); };
      _k->run_until(stppd);
      report_why_we_stopped();
    }

    bool Kambites::finished_impl() const {
      return _k->finished();
    }

    ////////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - non-pure virtual methods - private
    ////////////////////////////////////////////////////////////////////////////

    void Kambites::add_pair_impl(word_type const& u, word_type const& v) {
      _k->add_rule(u, v);
    }

    void Kambites::set_number_of_generators_impl(size_t n) {
      if (_k->alphabet().empty()) {
        _k->set_alphabet(n);
      }
    }

  }  // namespace congruence

  template <>
  word_type FroidurePin<
      detail::KE,
      FroidurePinTraits<detail::KE, fpsemigroup::Kambites<std::string>>>::
      factorisation(detail::KE const& x) {
    return x.word(*state());
  }

  template <>
  word_type FroidurePin<
      detail::KE,
      FroidurePinTraits<detail::KE,
                        fpsemigroup::Kambites<detail::MultiStringView>>>::
      factorisation(detail::KE const& x) {
    return x.word(*state());
  }

  template <>
  tril FroidurePin<
      detail::KE,
      FroidurePinTraits<detail::KE, fpsemigroup::Kambites<std::string>>>::
      is_finite() const {
    return tril::FALSE;
  }

  template <>
  tril FroidurePin<
      detail::KE,
      FroidurePinTraits<detail::KE,
                        fpsemigroup::Kambites<detail::MultiStringView>>>::
      is_finite() const {
    return tril::FALSE;
  }
}  // namespace libsemigroups
