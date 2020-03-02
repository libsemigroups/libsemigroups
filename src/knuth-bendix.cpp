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

#include <cstddef>  // for size_t
#include <string>   // for string

#include "libsemigroups/cong-intf.hpp"  // for CongruenceInterface, CongruenceInterface::...
#include "libsemigroups/fpsemi-intf.hpp"          // for FpSemigroupInterface
#include "libsemigroups/froidure-pin-base.hpp"    // for FroidurePinBase
#include "libsemigroups/froidure-pin.hpp"         // for FroidurePin
#include "libsemigroups/kbe.hpp"                  // for detail::KBE
#include "libsemigroups/knuth-bendix.hpp"         // for KnuthBendix, KnuthBe...
#include "libsemigroups/libsemigroups-debug.hpp"  // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/libsemigroups-exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/obvinf.hpp"  // for IsObviouslyInfinitePairs
#include "libsemigroups/stl.hpp"     // for detail::make_unique
#include "libsemigroups/types.hpp"   // for word_type

#include "knuth-bendix-impl.hpp"

// Include kbe-impl.hpp after knuth-bendix-impl.hpp since detail::KBE depends on
// KnuthBendixImpl.
#include "kbe-impl.hpp"

namespace libsemigroups {
  using froidure_pin_type
      = FroidurePin<detail::KBE,
                    FroidurePinTraits<detail::KBE, fpsemigroup::KnuthBendix>>;
  namespace fpsemigroup {

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendix::Settings - constructor - public
    //////////////////////////////////////////////////////////////////////////

    KnuthBendix::Settings::Settings()
        : _check_confluence_interval(4096),
          _max_overlap(POSITIVE_INFINITY),
          _max_rules(POSITIVE_INFINITY),
          _overlap_policy(policy::overlap::ABC) {}

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendix - setters for Settings - public
    //////////////////////////////////////////////////////////////////////////

    KnuthBendix& KnuthBendix::overlap_policy(policy::overlap p) {
      _impl->set_overlap_policy(p);
      // the next line must be after _impl->set_overlap_policy
      _settings._overlap_policy = p;
      return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendix - constructors and destructor - public
    //////////////////////////////////////////////////////////////////////////

    KnuthBendix::KnuthBendix()
        : FpSemigroupInterface(),
          _impl(detail::make_unique<KnuthBendixImpl>(this)) {}

    KnuthBendix::KnuthBendix(KnuthBendix const& kb) : KnuthBendix() {
      init_from(kb);
    }

    KnuthBendix::~KnuthBendix() = default;

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendix - initialisers - public
    //////////////////////////////////////////////////////////////////////////

    void KnuthBendix::init_from(FroidurePinBase& S) {
      if (S.nr_generators() != 0) {
        if (alphabet().empty()) {
          set_alphabet(S.nr_generators());
        }
        // throws if rules contain letters that are not in the alphabet.
        add_rules(S);
      }
      // Do not set froidure_pin so we are guaranteed that it
      // returns a FroidurePin of detail::KBE's.
    }

    void KnuthBendix::init_from(KnuthBendix const& kb, bool add) {
      // TODO(later)   set confluence if known? Other things?
      if (!kb.alphabet().empty()) {
        if (alphabet().empty()) {
          set_alphabet(kb.alphabet());
        }
        // throws if rules contain letters that are not in the alphabet.
        if (add) {
          add_rules(kb.active_rules());
        }
      }
      // TODO(later) copy other settings
      _settings._overlap_policy = kb._settings._overlap_policy;
    }

    //////////////////////////////////////////////////////////////////////////
    // FpSemigroupInterface - non-pure virtual methods - public
    //////////////////////////////////////////////////////////////////////////

    size_t KnuthBendix::size() {
      if (is_obviously_infinite()) {
        return POSITIVE_INFINITY;
      } else if (alphabet().empty()) {
        return 0;
      } else {
        return froidure_pin()->size();
      }
    }

    size_t KnuthBendix::nr_active_rules() const noexcept {
      return _impl->nr_rules();
    }

    bool KnuthBendix::equal_to(std::string const& u, std::string const& v) {
      validate_word(u);
      validate_word(v);
      return _impl->equal_to(u, v);
    }

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendix public methods for rules and rewriting
    //////////////////////////////////////////////////////////////////////////

    using const_iterator = FpSemigroupInterface::const_iterator;

    // TODO(later)  implement these!
    // const_iterator cbegin_active_rules() const {
    //  return _impl->cbegin_active_rules();
    // }

    // const_iterator cend_active_rules() const {
    //   return _impl->cend_active_rules();
    // }

    std::vector<std::pair<std::string, std::string>>
    KnuthBendix::active_rules() const {
      return _impl->rules();
    }

    std::string* KnuthBendix::rewrite(std::string* w) const {
      return _impl->rewrite(w);
    }

    std::ostream& operator<<(std::ostream& os, KnuthBendix const& kb) {
      os << detail::to_string(kb.active_rules()) << "\n";
      return os;
    }

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendix - main methods - public
    //////////////////////////////////////////////////////////////////////////

    bool KnuthBendix::confluent() const {
      return _impl->confluent();
    }

    void KnuthBendix::run_impl() {
      _impl->knuth_bendix();
      report_why_we_stopped();
    }

    void KnuthBendix::knuth_bendix_by_overlap_length() {
      _impl->knuth_bendix_by_overlap_length();
      report_why_we_stopped();
    }

    //////////////////////////////////////////////////////////////////////////
    // FpSemigroupInterface - pure virtual methods - private
    //////////////////////////////////////////////////////////////////////////

    void KnuthBendix::add_rule_impl(std::string const& p,
                                    std::string const& q) {
      _impl->add_rule(p, q);
    }

    std::shared_ptr<FroidurePinBase> KnuthBendix::froidure_pin_impl() {
      LIBSEMIGROUPS_ASSERT(!alphabet().empty());
      run();
      auto ptr = std::make_shared<froidure_pin_type>(*this);
      for (size_t i = 0; i < alphabet().size(); ++i) {
        ptr->add_generator(detail::KBE(*this, i));
      }
      return ptr;
    }

    bool KnuthBendix::is_obviously_infinite_impl() {
      if (alphabet().size() > nr_rules()) {
        return true;
      }
      detail::IsObviouslyInfinitePairs<char, std::string> ioi(
          alphabet().size());
      ioi.add_rules(cbegin_rules(), cend_rules());
      return ioi.result();
    }

    //////////////////////////////////////////////////////////////////////////
    // FpSemigroupInterface - non-pure virtual methods - private
    //////////////////////////////////////////////////////////////////////////

    void KnuthBendix::set_alphabet_impl(std::string const& lphbt) {
      _impl->set_internal_alphabet(lphbt);
    }

    void KnuthBendix::set_alphabet_impl(size_t) {
      _impl->set_internal_alphabet();
    }

    bool KnuthBendix::validate_identity_impl(std::string const& id) const {
      if (id.length() > 1) {
        LIBSEMIGROUPS_EXCEPTION(
            "invalid identity, found %d letters, should be 0 or 1 letters",
            id.length());
      }
      if (id.length() == 1) {
        validate_letter(id[0]);
        return true;  // Add rules for the identity
      } else {
        return false;  // Don't add rules for the identity
      }
    }
  }  // namespace fpsemigroup

  namespace congruence {
    using class_index_type = CongruenceInterface::class_index_type;

    ////////////////////////////////////////////////////////////////////////////
    // KnuthBendix - constructors - public
    ////////////////////////////////////////////////////////////////////////////

    KnuthBendix::KnuthBendix()
        : CongruenceInterface(congruence_type::twosided),
          _kb(detail::make_unique<fpsemigroup::KnuthBendix>()) {}

    KnuthBendix::KnuthBendix(fpsemigroup::KnuthBendix const& kb)
        : KnuthBendix() {
      _kb->init_from(kb, false);  // false = don't add rules
      if (!_kb->alphabet().empty()) {
        set_nr_generators(_kb->alphabet().size());
      }
      // TODO(later):
      // The following lines don't do anything because _kb does not get the
      // FroidurePin from kb. Could be kb->has_froidure_pin()?
      // if (_kb->has_froidure_pin()) {
      //  set_parent_froidure_pin(_kb->froidure_pin());
      // }
      for (auto const& r : kb.active_rules()) {
        add_pair(kb.string_to_word(r.first), kb.string_to_word(r.second));
      }
    }

    KnuthBendix::KnuthBendix(std::shared_ptr<FroidurePinBase> S)
        : KnuthBendix() {
      _kb->init_from(*S);
      set_nr_generators(S->nr_generators());
      set_parent_froidure_pin(S);
    }

    ////////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - non-pure virtual methods - public
    ////////////////////////////////////////////////////////////////////////////

    tril KnuthBendix::const_contains(word_type const& lhs,
                                     word_type const& rhs) const {
      validate_word(lhs);
      validate_word(rhs);
      if (lhs == rhs) {
        return tril::TRUE;
      }
      std::string u, v;
      u = _kb->rewrite(_kb->word_to_string(lhs));
      v = _kb->rewrite(_kb->word_to_string(rhs));
      if (u == v) {
        return tril::TRUE;
      } else if (_kb->confluent()) {
        return tril::FALSE;
      } else {
        return tril::unknown;
      }
    }

    bool KnuthBendix::is_quotient_obviously_finite_impl() {
      return finished()
             || (has_parent_froidure_pin() && parent_froidure_pin()->finished())
             || (_kb->is_obviously_finite() && nr_generators() != UNDEFINED);
    }

    bool KnuthBendix::contains(word_type const& lhs, word_type const& rhs) {
      if (const_contains(lhs, rhs) == tril::TRUE) {
        return true;
      }
      _kb->run();
      return const_contains(lhs, rhs) == tril::TRUE;
    }

    ////////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - pure virtual methods - private
    ////////////////////////////////////////////////////////////////////////////

    word_type KnuthBendix::class_index_to_word_impl(class_index_type i) {
      return _kb->froidure_pin()->minimal_factorisation(i);
    }

    size_t KnuthBendix::nr_classes_impl() {
      run();  // required so that the state of this is correctly set.
      return _kb->size();
    }

    std::shared_ptr<FroidurePinBase> KnuthBendix::quotient_impl() {
      return _kb->froidure_pin();
    }

    class_index_type
    KnuthBendix::word_to_class_index_impl(word_type const& word) {
      auto   S   = static_cast<froidure_pin_type*>(_kb->froidure_pin().get());
      size_t pos = S->position(detail::KBE(*_kb, word));
      LIBSEMIGROUPS_ASSERT(pos != UNDEFINED);
      return pos;
    }

    void KnuthBendix::run_impl() {
      auto stppd = [this]() -> bool { return stopped(); };
      _kb->run_until(stppd);
      report_why_we_stopped();
    }

    bool KnuthBendix::finished_impl() const {
      return _kb->has_froidure_pin() && _kb->froidure_pin()->finished();
    }

    ////////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - non-pure virtual methods - private
    ////////////////////////////////////////////////////////////////////////////

    void KnuthBendix::add_pair_impl(word_type const& u, word_type const& v) {
      _kb->add_rule(u, v);
    }

    void KnuthBendix::set_nr_generators_impl(size_t n) {
      if (_kb->alphabet().empty()) {
        _kb->set_alphabet(n);
      }
    }
  }  // namespace congruence
}  // namespace libsemigroups
