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

// This file contains a base class for f.p. semigroup like classes.

#ifndef LIBSEMIGROUPS_INCLUDE_FPSEMI_BASE_HPP_
#define LIBSEMIGROUPS_INCLUDE_FPSEMI_BASE_HPP_

#include <stddef.h>       // for size_t
#include <string>         // for string
#include <unordered_map>  // for unordered_map
#include <utility>        // for pair
#include <vector>         // for vector

#include "internal/runner.hpp"  // for Runner

#include "types.hpp"  // for word_type, letter_type, relation_type

namespace libsemigroups {
  class FpSemigroup;      // Forward declaration
  class FroidurePinBase;  // Forward declaration

  class FpSemiBase : public Runner {
    friend class FpSemigroup;

   public:
    //////////////////////////////////////////////////////////////////////////////
    // FpSemiBase - constructor + destructor - public
    //////////////////////////////////////////////////////////////////////////////

    // FpSemiBase is non-copyable, and non-movable.
    FpSemiBase(FpSemiBase const&) = delete;
    FpSemiBase& operator=(FpSemiBase const&) = delete;
    FpSemiBase(FpSemiBase&&)                 = delete;
    FpSemiBase& operator=(FpSemiBase&&) = delete;

    FpSemiBase();
    virtual ~FpSemiBase();

    //////////////////////////////////////////////////////////////////////////////
    // FpSemiBase - pure virtual methods - public
    //////////////////////////////////////////////////////////////////////////////

    // Pure methods (attributes of an f.p. semigroup)
    virtual bool   is_obviously_finite()   = 0;
    virtual bool   is_obviously_infinite() = 0;
    virtual size_t size()                  = 0;

    // Pure methods (for elements of fp semigroups)
    virtual bool        equal_to(std::string const&, std::string const&) = 0;
    virtual std::string normal_form(std::string const&)                  = 0;

    virtual FroidurePinBase* isomorphic_non_fp_semigroup() = 0;

    virtual size_t nr_rules() const noexcept = 0;

    //////////////////////////////////////////////////////////////////////////////
    // FpSemiBase - non-pure virtual methods - public
    //////////////////////////////////////////////////////////////////////////////

    virtual bool      equal_to(word_type const&, word_type const&);
    virtual word_type normal_form(word_type const&);

    //////////////////////////////////////////////////////////////////////////////
    // FpSemiBase - non-virtual methods - public
    //////////////////////////////////////////////////////////////////////////////

    void               set_alphabet(std::string const&);
    void               set_alphabet(size_t);
    std::string const& alphabet() const noexcept;

    void add_rule(std::string const&, std::string const&);
    void add_rule(word_type const&, word_type const&);
    void add_rule(std::initializer_list<size_t>, std::initializer_list<size_t>);
    void add_rule(relation_type rel);
    void add_rule(std::pair<std::string, std::string>);

    void add_rules(FroidurePinBase*);
    void add_rules(std::vector<std::pair<std::string, std::string>> const&);

    bool               has_isomorphic_non_fp_semigroup() const noexcept;
    word_type          normal_form(std::initializer_list<letter_type>);
    bool               equal_to(std::initializer_list<letter_type>,
                  std::initializer_list<letter_type>);

    // Set the char in alphabet() to be the identity.
    void set_identity(std::string const&);
    void set_identity(letter_type);

    word_type   string_to_word(std::string const&) const;
    std::string word_to_string(word_type const&) const;

   protected:
    //////////////////////////////////////////////////////////////////////////////
    // FpSemiBase - non-virtual methods - protected
    //////////////////////////////////////////////////////////////////////////////

    size_t char_to_uint(char) const;
    char   uint_to_char(size_t) const noexcept;

    FroidurePinBase* get_isomorphic_non_fp_semigroup() const noexcept;
    void             set_isomorphic_non_fp_semigroup(FroidurePinBase*) noexcept;

    bool validate_letter(char) const;
    bool validate_letter(letter_type) const;

    void validate_word(std::string const&) const;
    void validate_word(word_type const&) const;

    void validate_relation(std::string const&, std::string const&) const;
    void validate_relation(std::pair<std::string, std::string> const&) const;
    void validate_relation(relation_type const&) const;
    void validate_relation(word_type const&, word_type const&) const;

   private:
    //////////////////////////////////////////////////////////////////////////////
    // FpSemiBase - pure virtual methods - private
    //////////////////////////////////////////////////////////////////////////////

    virtual void add_rule_impl(std::string const&, std::string const&) = 0;

    //////////////////////////////////////////////////////////////////////////////
    // FpSemiBase - non-pure virtual methods - private
    //////////////////////////////////////////////////////////////////////////////

    virtual void set_alphabet_impl(std::string const&);
    virtual void set_alphabet_impl(size_t);
    virtual void add_rule_impl(word_type const&, word_type const&);
    virtual void add_rules_impl(FroidurePinBase*);

    //////////////////////////////////////////////////////////////////////////////
    // FpSemiBase - non-virtual methods - private
    //////////////////////////////////////////////////////////////////////////////

    void reset() noexcept;

    //////////////////////////////////////////////////////////////////////////////
    // FpSemiBase - data - private
    //////////////////////////////////////////////////////////////////////////////
    std::string _alphabet;
    // TODO The unordered_map could be an array if this is too slow
    std::unordered_map<char, letter_type> _alphabet_map;
    FroidurePinBase*                      _isomorphic_non_fp_semigroup;
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_FPSEMI_BASE_HPP_
