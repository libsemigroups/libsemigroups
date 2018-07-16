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

#ifndef LIBSEMIGROUPS_SRC_SEMIGROUP_BASE_H_
#define LIBSEMIGROUPS_SRC_SEMIGROUP_BASE_H_

#include <atomic>

#include "constants.h"
#include "recvec.h"
#include "types.h"

namespace libsemigroups {
  class SemigroupBase {
   public:
    //! Type used for indexing elements in a Semigroup, use this when not
    //! specifically referring to a position in _elements. It should be possible
    //! to change this type and everything will just work, provided the size of
    //! the semigroup is less than the maximum value of this type of integer.
    using size_type = size_t;

    //! Type for the position of an element in an instance of Semigroup. The
    //! size of the semigroup being enumerated must be at most
    //! std::numeric_limits<element_index_type>::max()
    using element_index_type = size_type;

    //! Type for a left or right Cayley graph of a semigroup.
    using cayley_graph_type = RecVec<element_index_type>;

    virtual ~SemigroupBase() {}
    virtual element_index_type word_to_pos(word_type const&) const    = 0;
    virtual size_t             current_max_word_length() const        = 0;
    virtual size_t             degree() const                         = 0;
    virtual size_t             nrgens() const                         = 0;
    virtual bool               is_done() const                        = 0;
    virtual bool               is_begun() const                       = 0;
    virtual size_t             current_size() const                   = 0;
    virtual size_t             current_nrrules() const                = 0;
    virtual element_index_type prefix(element_index_type) const       = 0;
    virtual element_index_type suffix(element_index_type) const       = 0;
    virtual letter_type        first_letter(element_index_type) const = 0;
    virtual letter_type        final_letter(element_index_type) const = 0;
    virtual size_t             batch_size() const                     = 0;
    virtual size_t             length_const(element_index_type) const = 0;
    virtual size_t             length_non_const(element_index_type)   = 0;
    virtual element_index_type
                               product_by_reduction(element_index_type,
                                                    element_index_type) const =
                               0;
    virtual element_index_type fast_product(element_index_type,
                                            element_index_type) const = 0;
    virtual element_index_type letter_to_pos(letter_type) const       = 0;
    virtual size_t             size()                                 = 0;
    virtual size_t             nridempotents()                        = 0;
    virtual bool               is_idempotent(element_index_type)      = 0;
    virtual size_t             nrrules()                              = 0;
    virtual void               set_batch_size(size_t)                 = 0;
    virtual void               reserve(size_t)                        = 0;
    virtual element_index_type position_to_sorted_position(element_index_type)
        = 0;
    virtual element_index_type right(element_index_type, letter_type)       = 0;
    virtual cayley_graph_type* right_cayley_graph_copy()                    = 0;
    virtual element_index_type left(element_index_type, letter_type)        = 0;
    virtual cayley_graph_type* left_cayley_graph_copy()                     = 0;
    virtual void      minimal_factorisation(word_type&, element_index_type) = 0;
    virtual word_type minimal_factorisation(element_index_type)             = 0;
    virtual void      factorisation(word_type&, element_index_type)         = 0;
    virtual word_type factorisation(element_index_type)                     = 0;
    virtual void      reset_next_relation()                                 = 0;
    virtual void      next_relation(word_type&)                             = 0;
    virtual void      enumerate(std::atomic<bool>&, size_t)                 = 0;
    virtual void      enumerate(size_t)                                     = 0;
    virtual void      set_max_threads(size_t)                               = 0;
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_SRC_SEMIGROUP_BASE_H_
