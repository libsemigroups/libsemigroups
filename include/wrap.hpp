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

#ifndef LIBSEMIGROUPS_INCLUDE_WRAP_HPP_
#define LIBSEMIGROUPS_INCLUDE_WRAP_HPP_

#include "internal/libsemigroups-exception.hpp"
#include "internal/stl.hpp"

#include "cong-base.hpp"
#include "fpsemi-base.hpp"
#include "froidure-pin-base.hpp"
#include "types.hpp"

namespace libsemigroups {
  namespace fpsemigroup {
    template <class TWrappedCong, bool TAddRules = true>
    class WrappedCong : public FpSemiBase {
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
      explicit WrappedCong(FroidurePinBase* S)
          : _nr_rules(0),
            _wrapped_cong(
                make_unique<wrapped_type>(congruence_type::TWOSIDED, S)) {
        FpSemiBase::set_alphabet(S->nr_generators());
        if (TAddRules) {
          add_rules(S);
        }
        // FIXME set_isomorphic_non_fp_semigroup??
      }

      explicit WrappedCong(std::string const& lphbt) : WrappedCong() {
        set_alphabet(lphbt);
      }

      explicit WrappedCong(FroidurePinBase& S) : WrappedCong(&S) {}

      ////////////////////////////////////////////////////////////////////////////
      // Runner - overridden pure virtual method - public
      ////////////////////////////////////////////////////////////////////////////

      void run() override {
        _wrapped_cong->run_until(
            [this]() -> bool { return dead() || timed_out(); });
      }

      ////////////////////////////////////////////////////////////////////////////
      // FpSemiBase - overridden pure virtual methods - public
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

      FroidurePinBase* isomorphic_non_fp_semigroup() override {
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
      // FpSemiBase - overridden non-pure virtual methods - public
      ////////////////////////////////////////////////////////////////////////////

      // We override FpSemiBase::add_rule to avoid unnecessary conversion from
      // word_type -> string.
      void add_rule(word_type const& lhs, word_type const& rhs) override {
        if (lhs.empty() || rhs.empty()) {
          throw LIBSEMIGROUPS_EXCEPTION("rules must be non-empty");
        }
        validate_word(lhs);
        validate_word(rhs);
        _wrapped_cong->add_pair(lhs, rhs);
      }

      // We override FpSemiBase::equal_to to avoid unnecessary conversion from
      // word_type -> string.
      bool equal_to(word_type const& lhs, word_type const& rhs) override {
        return _wrapped_cong->contains(lhs, rhs);
      }

      // We override FpSemiBase::normal_form to avoid unnecessary conversion
      // from word_type -> string.
      word_type normal_form(word_type const& w) override {
        return _wrapped_cong->class_index_to_word(
            _wrapped_cong->word_to_class_index(w));
      }

      // We override FpSemiBase::set_alphabet so that we can set the number of
      // generators in _wrapped_cong.
      void set_alphabet(std::string const& lphbet) override {
        FpSemiBase::set_alphabet(lphbet);
        _wrapped_cong->set_nr_generators(lphbet.size());
      }

      // We override FpSemiBase::set_alphabet so that we can set the number of
      // generators in _wrapped_cong.
      void set_alphabet(size_t nr_letters) override {
        FpSemiBase::set_alphabet(nr_letters);
        _wrapped_cong->set_nr_generators(nr_letters);
      }

      void add_rules(FroidurePinBase* S) override {
        // TODO improve this method to avoid unnecessary conversions
        FpSemiBase::add_rules(S);
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
#endif  // LIBSEMIGROUPS_INCLUDE_WRAP_HPP_
