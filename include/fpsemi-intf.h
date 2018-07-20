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

// This file contains an interface for fp semigroup like classes.

#ifndef LIBSEMIGROUPS_INCLUDE_FPSEMI_INTF_H_
#define LIBSEMIGROUPS_INCLUDE_FPSEMI_INTF_H_

#include <string>

#include "internal/runner.h"

#include "semigroup-base.h"
#include "types.h"

namespace libsemigroups {
  class FpSemigroup;
  class FpSemiIntf : public Runner {
    friend class libsemigroups::FpSemigroup;

   public:
    static const std::string NO_ALPHABET;

    // Pure methods (for defining an f.p. semigroup)
    virtual void               set_nr_generators(size_t) = 0;
    virtual size_t             nr_generators() const     = 0;
    virtual void               set_alphabet(std::string) = 0;
    virtual std::string const& alphabet() const          = 0;
    // TODO - nr_relations
    //      - next_relation (can't do cbegin/cend cos the data types are
    //      different in the subclasses)

   private:
    virtual bool validate_word(word_type const&) const           = 0;
    virtual bool validate_word(std::string const&) const         = 0;
    virtual void set_isomorphic_non_fp_semigroup(SemigroupBase*) = 0;
    virtual void internal_add_relation(word_type, word_type)     = 0;

   public:
    // The arguments add_relations are not const& since they must be copied
    // into any instance of a derived class.
    virtual void add_relation(word_type, word_type)     = 0;
    virtual void add_relation(std::string, std::string) = 0;

    // Pure methods (attributes of an f.p. semigroup)
    virtual bool           is_obviously_finite() const       = 0;
    virtual bool           is_obviously_infinite() const     = 0;
    virtual size_t         size()                            = 0;
    virtual SemigroupBase* isomorphic_non_fp_semigroup()     = 0;
    virtual bool           has_isomorphic_non_fp_semigroup() = 0;

    // Pure methods (for elements of fp semigroups)
    virtual bool        equal_to(word_type const&, word_type const&)     = 0;
    virtual bool        equal_to(std::string const&, std::string const&) = 0;
    virtual word_type   normal_form(word_type const&)                    = 0;
    virtual std::string normal_form(std::string const&)                  = 0;

    // Non-pure methods (these all use the pure methods, and are mostly
    // syntactic sugar).
    void add_relation(std::initializer_list<size_t>,
                      std::initializer_list<size_t>);
    void add_relations(SemigroupBase*);
    void add_relations(std::vector<relation_type> const&);
    void add_relation(relation_type);
    void add_relation(std::pair<std::string, std::string>);
    bool validate_relation(relation_type const&) const;
    bool validate_relation(word_type const&, word_type const&) const;
    bool validate_relation(std::pair<std::string, std::string> const&) const;
    bool validate_relation(std::string const&, std::string const&) const;
    bool equal_to(std::initializer_list<size_t> const&,
                  std::initializer_list<size_t> const&);
    word_type normal_form(std::initializer_list<size_t> const&);
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_FPSEMI_INTF_H_
