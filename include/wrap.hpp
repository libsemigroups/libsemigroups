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

// This file contains wrappers for converting an CongBase -> FpSemiBase, the
// resulting FpSemiBase delegates everything to CongBase which it wraps.

#ifndef LIBSEMIGROUPS_INCLUDE_WRAP_HPP_
#define LIBSEMIGROUPS_INCLUDE_WRAP_HPP_

#include <memory>    // for unique_ptr
#include <stddef.h>  // for size_t
#include <string>    // for string

#include "internal/libsemigroups-exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "internal/stl.hpp"                      // for make_unique

#include "cong-base.hpp"          // for congruence_type, con...
#include "fpsemi-base.hpp"        // for FpSemiBase
#include "froidure-pin-base.hpp"  // for FroidurePinBase

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
          : _wrapped_cong(
                make_unique<wrapped_type>(congruence_type::TWOSIDED)) {}

      explicit WrappedCong(FroidurePinBase* S)
          : _wrapped_cong(
                make_unique<wrapped_type>(congruence_type::TWOSIDED, S)) {
        set_alphabet(S->nr_generators());
        if (TAddRules) {
          add_rules(S);
        }
      }

      explicit WrappedCong(FroidurePinBase& S) : WrappedCong(&S) {}

      ////////////////////////////////////////////////////////////////////////////
      // Runner - pure virtual method - public
      ////////////////////////////////////////////////////////////////////////////

      void run() override {
        _wrapped_cong->run_until(
            [this]() -> bool { return dead() || timed_out(); });
      }

      ////////////////////////////////////////////////////////////////////////////
      // FpSemiBase - pure virtual methods - public
      ////////////////////////////////////////////////////////////////////////////

      size_t size() override {
        return _wrapped_cong->nr_classes();
      }

      bool equal_to(std::string const& lhs, std::string const& rhs) override {
        validate_word(lhs);
        validate_word(rhs);
        return _wrapped_cong->contains(string_to_word(lhs),
                                       string_to_word(rhs));
      }

      // TODO improve the many copies etc in:
      // string -> word_type -> class_index_type -> word_type -> string
      std::string normal_form(std::string const& w) override {
        return word_to_string(_wrapped_cong->class_index_to_word(
            _wrapped_cong->word_to_class_index(string_to_word(w))));
      }

      ////////////////////////////////////////////////////////////////////////////
      // Runner - non-pure virtual method - protected
      ////////////////////////////////////////////////////////////////////////////

      bool finished_impl() const override {
        return _wrapped_cong->finished();
      }

      ////////////////////////////////////////////////////////////////////////////
      // FpSemiBase - non-pure virtual methods - public
      ////////////////////////////////////////////////////////////////////////////


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

      ////////////////////////////////////////////////////////////////////////////
      // WrappedCong - methods - public
      ////////////////////////////////////////////////////////////////////////////
      // TODO use shared_ptr
      TWrappedCong const* congruence() const {
        return _wrapped_cong.get();
      }

     private:
      //////////////////////////////////////////////////////////////////////////
      // FpSemiBase - pure virtual methods - private
      //////////////////////////////////////////////////////////////////////////

      void add_rule_impl(std::string const& u, std::string const& v) override {
        // This is only ever called if u and v are valid
        _wrapped_cong->add_pair(string_to_word(u), string_to_word(v));
      }

      FroidurePinBase* isomorphic_non_fp_semigroup_impl() override {
        return &_wrapped_cong->quotient_semigroup();
      }

      //////////////////////////////////////////////////////////////////////////
      // FpSemiBase - non-pure virtual methods - private
      //////////////////////////////////////////////////////////////////////////

      void set_alphabet_impl(std::string const& lphbt) override {
        _wrapped_cong->set_nr_generators(lphbt.size());
      }

      void set_alphabet_impl(size_t nr_letters) override {
        _wrapped_cong->set_nr_generators(nr_letters);
      }

      // We override FpSemiBase::add_rule_impl to avoid unnecessary conversion
      // from word_type -> string.
      void add_rule_impl(word_type const& u, word_type const& v) override {
        // This is only ever called if u and v are valid
        _wrapped_cong->add_pair(u, v);
      }

      void add_rules_impl(FroidurePinBase* S) override {
        relations(*S, [this](word_type lhs, word_type rhs) -> void {
          validate_word(lhs);
          validate_word(rhs);
          add_rule(lhs, rhs);
        });
      }

      bool is_obviously_finite_impl() override {
        return _wrapped_cong->is_quotient_obviously_finite();
      }

      bool is_obviously_infinite_impl() override {
        return _wrapped_cong->is_quotient_obviously_infinite();
      }

      //////////////////////////////////////////////////////////////////////////
      // WrappedCong - data - private
      //////////////////////////////////////////////////////////////////////////

      std::unique_ptr<TWrappedCong> _wrapped_cong;
    };
  }  // namespace fpsemigroup
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_WRAP_HPP_
