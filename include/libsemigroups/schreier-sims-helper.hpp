// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2021 Reinis Cirpons
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

// This file contains an implementation of a backtrack search for computing
// the intersection of two permutations given by Schreier-Sims algorithm.

// TODO
// 1. Implement base change
// 2. Keep track of representatives of phi
// 3. Calculate Omega

#ifndef LIBSEMIGROUPS_SCHREIER_SIMS_HELPER_HPP_
#define LIBSEMIGROUPS_SCHREIER_SIMS_HELPER_HPP_

#include <cstddef>  // for size_t

#include "containers.hpp"     // for Array2
#include "schreier-sims.hpp"  // for SchreierSims, SchreierSims<>::ele...
#include "transf.hpp"         // for LeastPerm
#include "types.hpp"          // for SmallestInteger

namespace libsemigroups {

  template <size_t N,
            typename TPointType   = typename SmallestInteger<N>::type,
            typename TElementType = LeastPerm<N>,
            typename TTraits = SchreierSimsTraits<N, TPointType, TElementType>>
  class SchreierSims;  // forward decl

  namespace schreier_sims_helper {

    template <size_t N>
    using point_type = typename SchreierSims<N>::point_type;

    template <size_t N>
    using element_type = typename SchreierSims<N>::element_type;

    template <size_t N>
    SchreierSims<N> intersection(SchreierSims<N> const& S1,
                                 SchreierSims<N> const& S2) {
      if (S2.number_of_generators() < S1.number_of_generators())
        return intersection(S2, S1);

      SchreierSims<N> T;

      // If N <= 1 then both S1, S2 are trivial.
      if (N <= 1) {
        T.run();
        return T;
      }

      // Note that if N-1 points are fixed then the N-th point is also fixed.
      // So if base contains all N points, then we lose nothing by discarding
      // the last point in the base.
      size_t base_size = S1.base_size();
      if (base_size == N)
        base_size = N - 1;

      SchreierSims<N> S2B;
      S1.run();
      for (size_t depth = 0; depth < base_size; ++depth) {
        S2B.add_base_point(S1.base(depth));
      }
      for (size_t i = 0; i < S2.number_of_generators(); ++i) {
        S2B.add_generator(S2.generator(i));
      }
      S2B.run();

      // Only need to consider points reachable by both groups
      detail::StaticVector2<point_type<N>, N> refined_orbit;
      for (size_t depth = 0; depth < base_size; ++depth) {
        // First point is always base point to make algorithm simpler
        refined_orbit[depth].push_back(S1.base(depth));
        for (point_type<N> pt = 0; pt < N; ++pt)
          if ((pt != S1.base(depth)) && S1.orbit_lookup(depth, pt)
              && S2B.orbit_lookup(depth, pt))
            refined_orbit[depth].push_back(pt);
      }

      // Initially assume that we have traversed the tree to the leaf
      // corresponding to the base and identity element.
      // stab_depth tracks the largest stabiliser we have found thus far.
      size_t                         stab_depth = base_size;
      size_t                         depth      = 0;
      std::array<size_t, N>          state_index;
      std::array<element_type<N>, N> state_elem;
      state_index.fill(0);
      state_elem.fill(S1.One());

      while (stab_depth > 0) {
        for (; depth < base_size; ++depth) {
          // This is a safe memory access as base_size <= N-1, so depth < N-1
          // during the loop and so depth + 1 <= N-1
          LIBSEMIGROUPS_ASSERT(depth + 1 < N);
          S1.Product(state_elem[depth + 1],
                     state_elem[depth],
                     S1.transversal_element(
                         depth, refined_orbit[depth][state_index[depth]]));
        }
        if (S2B.contains(state_elem[depth])) {
          T.add_generator(state_elem[depth]);
          // As soon as we find one, the rest are in a coset of stabiliser, so
// dont need to look at any more nodes.
          depth = stab_depth;
        }
        // If previous if statement passes then depth = stab_depth > 0 by the
        // while loop invariant. If not, then depth = base_size > 0 due to the
        // for loop before the if statement.
        LIBSEMIGROUPS_ASSERT(depth != 0);
        depth--;

        // Find largest depth that has an unvisited node and increment its
        // index. Adjust stabilizer depth as depths are exhausted.
        for (; depth >= 0; --depth) {
          state_index[depth]++;
          if (state_index[depth] < refined_orbit[depth].size())
            break;
          if (depth < stab_depth)
            stab_depth = depth;
          state_index[depth] = 0;
        }
      }

      T.run();
      return T;
    }
  }  // namespace schreier_sims_helper
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_SCHREIER_SIMS__HELPER_HPP_
