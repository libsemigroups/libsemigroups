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

// This file contains methods for enumerating a congruence by attempting to
// find all pairs of related elements using brute force. This does not work
// very well in most cases, due to the high complexity of the approach.

#ifndef LIBSEMIGROUPS_INCLUDE_CONG_PAIR_HPP_
#define LIBSEMIGROUPS_INCLUDE_CONG_PAIR_HPP_

#include <queue>

#include "internal/stl.hpp"
#include "internal/uf.hpp"

#include "cong-base.hpp"
#include "fpsemi-base.hpp"
#include "froidure-pin.hpp"
#include "kbe.hpp"
#include "wrap.hpp"

namespace libsemigroups {
  namespace congruence {
    // Implemented in cong-pair-impl.hpp
    template <typename TElementType  = Element const*,
              typename TElementHash  = hash<TElementType>,
              typename TElementEqual = equal_to<TElementType>,
              class TTraits
              = TraitsHashEqual<TElementType, TElementHash, TElementEqual>>
    class P : public CongBase, protected TTraits {
     protected:
      using froidure_pin_type
          = FroidurePin<TElementType, TElementHash, TElementEqual, TTraits>;

     protected:
      ////////////////////////////////////////////////////////////////////////
      // P - typedefs - protected
      ////////////////////////////////////////////////////////////////////////

      using const_reference       = typename froidure_pin_type::const_reference;
      using internal_element_type = typename TTraits::internal_element_type;
      using internal_const_element_type =
          typename TTraits::internal_const_element_type;

      using internal_equal_to = typename TTraits::internal_equal_to;
      using internal_hash     = typename TTraits::internal_hash;

      using product = ::libsemigroups::product<internal_element_type>;

      ////////////////////////////////////////////////////////////////////////
      // P - constructor - protected
      ////////////////////////////////////////////////////////////////////////
      // This is protected because it is not possible to create a P object
      // without it being defined over a parent semigroup.

      explicit P(congruence_type);

     public:
      ////////////////////////////////////////////////////////////////////////
      // P - constructor + destructor - public
      ////////////////////////////////////////////////////////////////////////

      P(congruence_type, FroidurePinBase*);
      P(congruence_type, FroidurePinBase&);
      ~P();

      ////////////////////////////////////////////////////////////////////////
      // Runner - overridden pure virtual methods - public
      ////////////////////////////////////////////////////////////////////////

      void run() override;

      ////////////////////////////////////////////////////////////////////////
      // CongBase - overridden pure virtual methods - public
      ////////////////////////////////////////////////////////////////////////

      void             add_pair(word_type const&, word_type const&) override;
      word_type        class_index_to_word(class_index_type) override;
      FroidurePinBase* quotient_semigroup() override;
      size_t           nr_classes() override;
      class_index_type word_to_class_index(word_type const& w) override;

     protected:
      ////////////////////////////////////////////////////////////////////////
      // CongBase - overridden non-pure virtual methods - protected
      ////////////////////////////////////////////////////////////////////////

      class_index_type
           const_word_to_class_index(word_type const&) const override;
      void init_non_trivial_classes() override;

      ////////////////////////////////////////////////////////////////////////
      // P - methods - protected
      ////////////////////////////////////////////////////////////////////////

      void internal_add_pair(internal_const_element_type,
                             internal_const_element_type);

     private:
      ////////////////////////////////////////////////////////////////////////
      // P - methods - private
      ////////////////////////////////////////////////////////////////////////

      size_t add_index(internal_element_type) const;
      void   delete_tmp_storage();
      size_t get_index(internal_const_element_type) const;
      void   init();

      ////////////////////////////////////////////////////////////////////////
      // P - inner structs - private
      ////////////////////////////////////////////////////////////////////////

      struct PHash {
       public:
        size_t
        operator()(std::pair<internal_const_element_type,
                             internal_const_element_type> const& pair) const {
          return internal_hash()(pair.first)
                 + 17 * internal_hash()(pair.second);
        }
      };

      struct PEqual {
        size_t operator()(std::pair<internal_const_element_type,
                                    internal_const_element_type> pair1,
                          std::pair<internal_const_element_type,
                                    internal_const_element_type> pair2) const {
          return internal_equal_to()(pair1.first, pair2.first)
                 && internal_equal_to()(pair1.second, pair2.second);
        }
      };

      ////////////////////////////////////////////////////////////////////////
      // P - data - private
      ////////////////////////////////////////////////////////////////////////

      mutable std::vector<class_index_type> _class_lookup;
      std::unordered_set<
          std::pair<internal_element_type, internal_element_type>,
          PHash,
          PEqual>
                 _found_pairs;
      bool       _init_done;
      mutable UF _lookup;
      mutable std::unordered_map<internal_const_element_type,
                                 size_t,
                                 internal_hash,
                                 internal_equal_to>
                               _map;
      mutable size_t           _map_next;
      mutable class_index_type _next_class;
      size_t                   _nr_non_trivial_classes;
      size_t                   _nr_non_trivial_elemnts;
      std::queue<std::pair<internal_element_type, internal_element_type>>
                                                 _pairs_to_mult;
      mutable std::vector<internal_element_type> _reverse_map;
      internal_element_type                      _tmp1;
      internal_element_type                      _tmp2;
    };

    //////////////////////////////////////////////////////////////////////////
    // The next class is for calculating congruences on a finitely presented
    // semigroup using KnuthBendix on the fp semigroup and then the pairs
    // algorithm to compute the congruence. Implemented in cong-pair.cpp.
    //////////////////////////////////////////////////////////////////////////

    class KBP : public P<KBE,
                         hash<KBE>,
                         equal_to<KBE>,
                         TraitsHashEqual<KBE, hash<KBE>, equal_to<KBE>>> {

      ////////////////////////////////////////////////////////////////////////
      // KBP - typedefs - private
      ////////////////////////////////////////////////////////////////////////

      using p_type = P<KBE,
                       hash<KBE>,
                       equal_to<KBE>,
                       TraitsHashEqual<KBE, hash<KBE>, equal_to<KBE>>>;

     public:
      ////////////////////////////////////////////////////////////////////////
      // KBP - constructors - public
      ////////////////////////////////////////////////////////////////////////

      KBP(congruence_type, fpsemigroup::KnuthBendix*);
      KBP(congruence_type, fpsemigroup::KnuthBendix&);

      ////////////////////////////////////////////////////////////////////////
      // P - overridden virtual methods - public
      ////////////////////////////////////////////////////////////////////////

      void run() override;

      // Override the method for the class P to avoid having to know the parent
      // semigroup (found as part of KBP::run) to add a pair.
      void add_pair(word_type const&, word_type const&) override;

     protected:
      ////////////////////////////////////////////////////////////////////////
      // CongBase - overridden non-pure virtual methods - private
      ////////////////////////////////////////////////////////////////////////

      using p_type::init_non_trivial_classes;

      ////////////////////////////////////////////////////////////////////////
      // KBP - data - private
      ////////////////////////////////////////////////////////////////////////

      fpsemigroup::KnuthBendix* _kb;
    };

  }  // namespace congruence

  namespace fpsemigroup {
    template <typename TElementType  = Element const*,
              typename TElementHash  = hash<TElementType>,
              typename TElementEqual = equal_to<TElementType>,
              class TTraits
              = TraitsHashEqual<TElementType, TElementHash, TElementEqual>>
    using P = WrappedCong<
        congruence::P<TElementType, TElementHash, TElementEqual, TTraits>,
        false>;
    // The false in the template of the previous line, is so that we do not add
    // the rules from any underlying semigroup to the P.
  }  // namespace fpsemigroup
}  // namespace libsemigroups

#include "cong-pair-impl.hpp"
#endif  // LIBSEMIGROUPS_INCLUDE_CONG_PAIR_HPP_
