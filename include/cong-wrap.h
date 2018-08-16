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

// This file contains TODO

#ifndef LIBSEMIGROUPS_INCLUDE_CONG_WRAP_H_
#define LIBSEMIGROUPS_INCLUDE_CONG_WRAP_H_

#include "internal/libsemigroups-exception.h"
#include "internal/stl.h"

#include "cong-intf.h"
#include "fpsemi-intf.h"
#include "semigroup-base.h"
#include "types.h"

namespace libsemigroups {
  namespace fpsemigroup {
    template <class TWrappedCong, bool TAddRules = true>
    class WrappedCong : public FpSemiIntf {
     public:
      using wrapped_type = TWrappedCong;

      ////////////////////////////////////////////////////////////////////////////
      // WrappedCong - constructors - public
      ////////////////////////////////////////////////////////////////////////////

      WrappedCong()
          : _nr_rules(0),
            _wrapped_cong(
                make_unique<wrapped_type>(congruence_type::TWOSIDED)) {}

      // FIXME avoid code duplication here
      explicit WrappedCong(SemigroupBase* S)
          : _nr_rules(0),
            _wrapped_cong(
                make_unique<wrapped_type>(congruence_type::TWOSIDED, S)) {
        FpSemiIntf::set_alphabet(S->nr_generators());
        if (TAddRules) {
          add_rules(S);
        }
        // FIXME set_isomorphic_non_fp_semigroup??
      }

      explicit WrappedCong(std::string const& lphbt) : WrappedCong() {
        set_alphabet(lphbt);
      }

      explicit WrappedCong(SemigroupBase& S) : WrappedCong(&S) {}

      ////////////////////////////////////////////////////////////////////////////
      // Runner - overridden pure virtual method - public
      ////////////////////////////////////////////////////////////////////////////

      void run() override {
        _wrapped_cong->run_until(
            [this]() -> bool { return dead() || timed_out(); });
      }

      ////////////////////////////////////////////////////////////////////////////
      // FpSemiIntf - overridden pure virtual methods - public
      ////////////////////////////////////////////////////////////////////////////

      void add_rule(std::string const& lhs, std::string const& rhs) override {
        if (!is_alphabet_defined()) {
          throw LIBSEMIGROUPS_EXCEPTION(
              "cannot add rules before an alphabet is defined");
        }
        // We perform these checks here because string_to_word fails if lhs/rhs
        // are not valid, and string_to_word does not checks.
        validate_word(lhs);
        validate_word(rhs);
        _nr_rules++;
        _wrapped_cong->add_pair(string_to_word(lhs), string_to_word(rhs));
      }

      bool is_obviously_finite() override {
        return _wrapped_cong->is_quotient_obviously_finite();
      }

      bool is_obviously_infinite() override {
        return _wrapped_cong->is_quotient_obviously_infinite();
      }

      size_t size() override {
        return _wrapped_cong->nr_classes();
      }

      bool equal_to(std::string const& lhs, std::string const& rhs) override {
        return _wrapped_cong->contains(string_to_word(lhs),
                                       string_to_word(rhs));
      }

      // TODO improve the many copies etc in:
      // string -> word_type -> class_index_type -> word_type -> string
      std::string normal_form(std::string const& w) override {
        return word_to_string(_wrapped_cong->class_index_to_word(
            _wrapped_cong->word_to_class_index(string_to_word(w))));
      }

      SemigroupBase* isomorphic_non_fp_semigroup() override {
        return _wrapped_cong->quotient_semigroup();
      }

      size_t nr_rules() const noexcept override {
        return _nr_rules;
      }

      ////////////////////////////////////////////////////////////////////////////
      // Runner - overridden non-pure virtual method - protected
      ////////////////////////////////////////////////////////////////////////////

      bool finished_impl() const override {
        return _wrapped_cong->finished();
      }

      ////////////////////////////////////////////////////////////////////////////
      // FpSemiIntf - overridden non-pure virtual methods - public
      ////////////////////////////////////////////////////////////////////////////

      // We override FpSemiIntf::add_rule to avoid unnecessary conversion from
      // word_type -> string.
      void add_rule(word_type const& lhs, word_type const& rhs) override {
        if (lhs.empty() || rhs.empty()) {
          throw LIBSEMIGROUPS_EXCEPTION("rules must be non-empty");
        }
        validate_word(lhs);
        validate_word(rhs);
        _wrapped_cong->add_pair(lhs, rhs);
      }

      // We override FpSemiIntf::equal_to to avoid unnecessary conversion from
      // word_type -> string.
      bool equal_to(word_type const& lhs, word_type const& rhs) override {
        return _wrapped_cong->contains(lhs, rhs);
      }

      // We override FpSemiIntf::normal_form to avoid unnecessary conversion
      // from word_type -> string.
      word_type normal_form(word_type const& w) override {
        return _wrapped_cong->class_index_to_word(
            _wrapped_cong->word_to_class_index(w));
      }

      // We override FpSemiIntf::set_alphabet so that we can set the number of
      // generators in _wrapped_cong.
      void set_alphabet(std::string const& lphbet) override {
        FpSemiIntf::set_alphabet(lphbet);
        _wrapped_cong->set_nr_generators(lphbet.size());
      }

      // We override FpSemiIntf::set_alphabet so that we can set the number of
      // generators in _wrapped_cong.
      void set_alphabet(size_t nr_letters) override {
        FpSemiIntf::set_alphabet(nr_letters);
        _wrapped_cong->set_nr_generators(nr_letters);
      }

      void add_rules(SemigroupBase* S) override {
        // TODO improve this method to avoid unnecessary conversions
        FpSemiIntf::add_rules(S);
        _nr_rules += S->nr_rules();
        // TODO something like the following
        // if (S->nr_rules() == this->nr_rules()) {
        //   set_isomorphic_non_fp_semigroup(S);
        // }
      }

      ////////////////////////////////////////////////////////////////////////////
      // WrappedCong - methods - public
      ////////////////////////////////////////////////////////////////////////////
      // TODO use shared_ptr
      TWrappedCong const* congruence() const {
        return _wrapped_cong.get();
      }

     private:
      size_t                        _nr_rules;
      std::unique_ptr<TWrappedCong> _wrapped_cong;
    };
  }  // namespace fpsemigroup
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_CONG_WRAP_H_
