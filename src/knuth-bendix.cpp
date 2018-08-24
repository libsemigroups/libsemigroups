//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2018 James D. Mitchell
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

#include <memory>    // for unique_ptr
#include <stddef.h>  // for size_t
#include <string>    // for operator!=, operator==

#include "internal/libsemigroups-debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "internal/libsemigroups-exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "internal/stl.hpp"                      // for make_unique

#include "cong-base.hpp"          // for CongBase, CongBase::...
#include "fpsemi-base.hpp"        // for FpSemiBase
#include "froidure-pin-base.hpp"  // for FroidurePinBase
#include "froidure-pin.hpp"       // for FroidurePin
#include "kbe.hpp"                // for KBE
#include "knuth-bendix.hpp"       // for KnuthBendix, KnuthBe...
#include "types.hpp"              // for word_type

namespace libsemigroups {
  class ReductionOrdering;
}

#include "knuth-bendix-impl.hpp"

// Include kbe-impl.hpp after knuth-bendix-impl.hpp since KBE depends on
// KnuthBendixImpl.
#include "kbe-impl.hpp"

namespace libsemigroups {
  namespace fpsemigroup {

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendix::Settings - constructor - public
    //////////////////////////////////////////////////////////////////////////

    KnuthBendix::Settings::Settings()
        : _check_confluence_interval(4096),
          _max_overlap(POSITIVE_INFINITY),
          _max_rules(POSITIVE_INFINITY),
          _overlap_policy(overlap_policy::ABC) {}

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendix - setters for Settings - public
    //////////////////////////////////////////////////////////////////////////

    void KnuthBendix::set_check_confluence_interval(size_t interval) {
      _settings._check_confluence_interval = interval;
    }

    void KnuthBendix::set_max_overlap(size_t val) {
      _settings._max_overlap = val;
    }

    void KnuthBendix::set_max_rules(size_t val) {
      _settings._max_rules = val;
    }

    void KnuthBendix::set_overlap_policy(overlap_policy p) {
      _settings._overlap_policy = p;
      _impl->set_overlap_policy(p);
    }

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendix - constructors and destructor - public
    //////////////////////////////////////////////////////////////////////////

    KnuthBendix::KnuthBendix()
        : FpSemiBase(), _impl(new KnuthBendixImpl(this)) {}

    KnuthBendix::KnuthBendix(ReductionOrdering* order)
        : FpSemiBase(), _impl(new KnuthBendixImpl(this, order)) {}

    KnuthBendix::KnuthBendix(FroidurePinBase* S) : KnuthBendix() {
      set_alphabet(S->nr_generators());
      // TODO(now) move the call to add_rules to elsewhere, so that it's done
      // in knuth_bendix so that this is done in a thread, and not when
      // KnuthBendix is constructed. If it is moved, then we will have to do
      // add_rules(S) to add_rule() so that we don't lose the relations from S.
      add_rules(S);
      // Do not set isomorphic_non_fp_semigroup so we are guaranteed that it
      // returns a FroidurePin of KBE's.
    }

    KnuthBendix::KnuthBendix(FroidurePinBase& S) : KnuthBendix(&S) {}

    KnuthBendix::KnuthBendix(KnuthBendix const* kb)
        : KnuthBendix(new ReductionOrdering(kb->_impl->order())) {
      // TODO(later) _active_rules.reserve(kb->nr_rules());
      // TODO(now)   set confluence if known?
      set_alphabet(kb->alphabet());
      _settings._overlap_policy = kb->_settings._overlap_policy;
      _impl->add_rules(kb->_impl);
    }

    KnuthBendix::~KnuthBendix() {
      delete _impl;
    }

    //////////////////////////////////////////////////////////////////////////
    // FpSemiBase - overridden non-pure virtual methods - public
    //////////////////////////////////////////////////////////////////////////

    void KnuthBendix::add_rule_impl(std::string const& p, std::string const& q) {
      _impl->add_rule(p, q);
    }

    bool KnuthBendix::is_obviously_finite() {
      return has_isomorphic_non_fp_semigroup()
             && isomorphic_non_fp_semigroup()->finished();
    }

    bool KnuthBendix::is_obviously_infinite() {
      if (is_obviously_finite()) {
        // In this case the semigroup defined by the KnuthBendix is finite.
        return false;
      } else if (alphabet().size() > nr_rules()) {
        return true;
      }

      // TODO(now):
      // - check that every generator i occurs in the lhs of some rule (if not,
      //   then if two words have different numbers of i in them, then they are
      //   not the same.
      //
      // - check that for every generator there is a lhs consisting solely of
      //   that generator (otherwise the generators has infinite order).

      return false;
    }

    size_t KnuthBendix::size() {
      if (is_obviously_infinite()) {
        return POSITIVE_INFINITY;
      } else {
        return isomorphic_non_fp_semigroup()->size();
      }
    }

    size_t KnuthBendix::nr_rules() const noexcept {
      return _impl->nr_rules();
    }

    FroidurePinBase* KnuthBendix::isomorphic_non_fp_semigroup() {
      LIBSEMIGROUPS_ASSERT(!alphabet().empty());
      // TODO(now) check that no generators/rules can be added after this has
      // been called, or if they are that _isomorphic_non_fp_semigroup is reset
      // again
      if (!has_isomorphic_non_fp_semigroup()) {
        run();
        // TODO(later) use 0-param FroidurePin constructor
        auto T = new FroidurePin<KBE>({KBE(*this, 0)});
        for (size_t i = 1; i < alphabet().size(); ++i) {
          T->add_generator(KBE(*this, i));
        }
        set_isomorphic_non_fp_semigroup(T);
      }
      return get_isomorphic_non_fp_semigroup();
    }

    bool KnuthBendix::equal_to(std::string const& u, std::string const& v) {
      validate_word(u);
      validate_word(v);
      return _impl->equal_to(u, v);
    }

    std::string KnuthBendix::normal_form(std::string const& w) {
      validate_word(w);
      run();
      return rewrite(w);
    }

    //////////////////////////////////////////////////////////////////////////
    // Overridden virtual methods from Runner
    //////////////////////////////////////////////////////////////////////////

    void KnuthBendix::run() {
      knuth_bendix();
    }

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendix public methods for rules and rewriting
    //////////////////////////////////////////////////////////////////////////

    std::vector<std::pair<std::string, std::string>>
    KnuthBendix::rules() const {
      return _impl->rules();
    }

    std::string* KnuthBendix::rewrite(std::string* w) const {
      return _impl->rewrite(w);
    }

    std::string KnuthBendix::rewrite(std::string w) const {
      rewrite(&w);
      return w;
    }

    std::ostream& operator<<(std::ostream& os, KnuthBendix const& kb) {
      os << to_string(kb.rules()) << "\n";
      return os;
    }

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendix - main methods - public
    //////////////////////////////////////////////////////////////////////////

    bool KnuthBendix::confluent() const {
      return _impl->confluent();
    }

    void KnuthBendix::knuth_bendix() {
      _impl->knuth_bendix();
      report_why_we_stopped();
    }

    void KnuthBendix::knuth_bendix_by_overlap_length() {
      _impl->knuth_bendix_by_overlap_length();
      report_why_we_stopped();
    }

    //////////////////////////////////////////////////////////////////////////
    // FpSemiBase - non-pure virtual methods - private
    //////////////////////////////////////////////////////////////////////////

    void KnuthBendix::set_alphabet_impl(std::string const& lphbt) {
      _impl->set_internal_alphabet(lphbt);
    }

    void KnuthBendix::set_alphabet_impl(size_t) {
      _impl->set_internal_alphabet();
    }

  }  // namespace fpsemigroup

  namespace congruence {
    using class_index_type = CongBase::class_index_type;

    ////////////////////////////////////////////////////////////////////////////
    // KnuthBendix - constructors - public
    ////////////////////////////////////////////////////////////////////////////

    KnuthBendix::KnuthBendix()
        : CongBase(congruence_type::TWOSIDED),
          _kb(make_unique<fpsemigroup::KnuthBendix>()) {}

    KnuthBendix::KnuthBendix(fpsemigroup::KnuthBendix const* kb)
        // FIXME don't repeat the code here from the 0-param constructor
        : CongBase(congruence_type::TWOSIDED),
          _kb(make_unique<fpsemigroup::KnuthBendix>(kb)) {
      set_nr_generators(kb->alphabet().size());
    }

    KnuthBendix::KnuthBendix(FroidurePinBase& S)
        // FIXME don't repeat the code here from the 0-param constructor
        : CongBase(congruence_type::TWOSIDED),
          _kb(make_unique<fpsemigroup::KnuthBendix>(S)) {
      set_nr_generators(S.nr_generators());
      set_parent_semigroup(&S);
    }

    ////////////////////////////////////////////////////////////////////////////
    // Runner - overridden pure virtual methods - public
    ////////////////////////////////////////////////////////////////////////////

    void KnuthBendix::run() {
      if (stopped()) {
        return;
      }
      auto stppd = [this]() -> bool { return _kb->dead() || _kb->timed_out(); };
      _kb->run_until(stppd);
      // It is essential that we call _kb->run() first and then
      // _kb->isomorphic_non_fp_semigroup(), since this might get killed
      // during _kb->run().
      if (!_kb->dead() && !_kb->timed_out()) {
        auto S = _kb->isomorphic_non_fp_semigroup();
        while (!S->finished() && !_kb->dead() && !_kb->timed_out()) {
          S->run_until(stppd);
        }
      }
      report_why_we_stopped();
    }

    ////////////////////////////////////////////////////////////////////////////
    // Runner - overridden non-pure virtual method - protected
    ////////////////////////////////////////////////////////////////////////////

    bool KnuthBendix::finished_impl() const {
      return _kb->has_isomorphic_non_fp_semigroup()
             && _kb->isomorphic_non_fp_semigroup()->finished();
    }

    ////////////////////////////////////////////////////////////////////////////
    // CongBase - overridden pure virtual methods - public
    ////////////////////////////////////////////////////////////////////////////

    word_type KnuthBendix::class_index_to_word(class_index_type i) {
      // i is checked in minimal_factorisation
      return _kb->isomorphic_non_fp_semigroup()->minimal_factorisation(i);
    }

    size_t KnuthBendix::nr_classes() {
      return _kb->size();
    }

    class_index_type KnuthBendix::word_to_class_index(word_type const& word) {
      validate_word(word);
      auto S
          = static_cast<FroidurePin<KBE>*>(_kb->isomorphic_non_fp_semigroup());
      size_t pos = S->position(KBE(_kb.get(), word));
      LIBSEMIGROUPS_ASSERT(pos != UNDEFINED);
      return pos;
    }

    ////////////////////////////////////////////////////////////////////////////
    // CongBase - overridden non-pure virtual methods - public
    ////////////////////////////////////////////////////////////////////////////

    CongBase::result_type
    KnuthBendix::const_contains(word_type const& lhs,
                                word_type const& rhs) const {
      if (_kb->rewrite(_kb->word_to_string(lhs))
          == _kb->rewrite(_kb->word_to_string(rhs))) {
        return result_type::TRUE;
      } else if (_kb->confluent()) {
        return result_type::FALSE;
      } else {
        return result_type::UNKNOWN;
      }
    }

    bool KnuthBendix::contains(word_type const& lhs, word_type const& rhs) {
      _kb->run();
      return const_contains(lhs, rhs) == result_type::TRUE;
    }

    ////////////////////////////////////////////////////////////////////////////
    // CongBase - overridden pure virtual methods - private
    ////////////////////////////////////////////////////////////////////////////

    void KnuthBendix::add_pair_impl(word_type const& u, word_type const& v) {
      _kb->add_rule(u, v);
    }

    FroidurePinBase* KnuthBendix::quotient_impl() {
      return _kb->isomorphic_non_fp_semigroup();
    }

    ////////////////////////////////////////////////////////////////////////////
    // CongBase - overridden non-pure virtual methods - private
    ////////////////////////////////////////////////////////////////////////////

    void KnuthBendix::set_nr_generators_impl(size_t n) {
      if (_kb->alphabet().empty()) {
        _kb->set_alphabet(n);
      } else if (_kb->alphabet().size() != n) {
        throw LIBSEMIGROUPS_EXCEPTION(
            "incompatible number of generators, should be "
            + to_string(_kb->alphabet().size()) + ", but found "
            + to_string(n));
      }
    }
  }  // namespace congruence
}  // namespace libsemigroups
