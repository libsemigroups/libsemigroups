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

#ifndef LIBSEMIGROUPS_SRC_SEMIGROUPS_BASE_H_
#define LIBSEMIGROUPS_SRC_SEMIGROUPS_BASE_H_

#include <atomic>

#include "recvec.h"

namespace libsemigroups {
  //! Type for the index of a generator of a semigroup.
  typedef size_t letter_t;

  //! Type for a word over the generators of a semigroup.
  typedef std::vector<letter_t> word_t;

  //! Type for a pair of word_t (a *relation*) of a semigroup.
  typedef std::pair<word_t, word_t> relation_t;

  class SemigroupBase {
   protected:
    // Type used for indexing elements in a Semigroup, use this when not
    // specifically referring to a position in _elements. It should be possible
    // to change this type and everything will just work, provided the size of
    // the semigroup is less than the maximum value of this type of integer.
    typedef size_t index_t;

   public:
    //! Type for the position of an element in an instance of Semigroup. The
    //! size of the semigroup being enumerated must be at most
    //! std::numeric_limits<element_index_t>::max()
    typedef index_t element_index_t;

    //! This variable is used to indicate that a value is undefined, such as,
    //! for example, the position of an element that does not belong to a
    //! semigroup.
    static index_t const UNDEFINED;

    //! This variable is used to indicate the maximum possible limit that can
    //! be used with Semigroup::enumerate.
    static index_t const LIMIT_MAX;

    //! Type for a left or right Cayley graph of a semigroup.
    typedef RecVec<element_index_t> cayley_graph_t;

    virtual ~SemigroupBase() {}
    virtual element_index_t word_to_pos(word_t const&) const                = 0;
    virtual size_t          current_max_word_length() const                 = 0;
    virtual size_t          degree() const                                  = 0;
    virtual size_t          nrgens() const                                  = 0;
    virtual bool            is_done() const                                 = 0;
    virtual bool            is_begun() const                                = 0;
    virtual size_t          current_size() const                            = 0;
    virtual size_t          current_nrrules() const                         = 0;
    virtual element_index_t prefix(element_index_t) const                   = 0;
    virtual element_index_t suffix(element_index_t) const                   = 0;
    virtual letter_t        first_letter(element_index_t) const             = 0;
    virtual letter_t        final_letter(element_index_t) const             = 0;
    virtual size_t          batch_size() const                              = 0;
    virtual size_t          length_const(element_index_t) const             = 0;
    virtual size_t          length_non_const(element_index_t)               = 0;
    virtual element_index_t product_by_reduction(element_index_t,
                                                 element_index_t) const     = 0;
    virtual element_index_t fast_product(element_index_t,
                                         element_index_t) const             = 0;
    virtual element_index_t letter_to_pos(letter_t) const                   = 0;
    virtual size_t          size()                                          = 0;
    virtual size_t          nridempotents()                                 = 0;
    virtual bool            is_idempotent(element_index_t)                  = 0;
    virtual size_t          nrrules()                                       = 0;
    virtual void            set_batch_size(size_t)                          = 0;
    virtual void            reserve(size_t)                                 = 0;
    virtual element_index_t position_to_sorted_position(element_index_t)    = 0;
    virtual element_index_t right(element_index_t, letter_t)                = 0;
    virtual cayley_graph_t* right_cayley_graph_copy()                       = 0;
    virtual element_index_t left(element_index_t, letter_t)                 = 0;
    virtual cayley_graph_t* left_cayley_graph_copy()                        = 0;
    virtual void            minimal_factorisation(word_t&, element_index_t) = 0;
    virtual word_t*         minimal_factorisation(element_index_t)          = 0;
    virtual void            factorisation(word_t&, element_index_t)         = 0;
    virtual word_t*         factorisation(element_index_t)                  = 0;
    virtual void            reset_next_relation()                           = 0;
    virtual void            next_relation(word_t&)                          = 0;
    virtual void            enumerate(std::atomic<bool>&, size_t)           = 0;
    virtual void            enumerate(size_t)                               = 0;
    virtual void            set_report(bool) const                          = 0;
    virtual void            set_max_threads(size_t)                         = 0;
  };

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_SRC_SEMIGROUPS_BASE_H_
