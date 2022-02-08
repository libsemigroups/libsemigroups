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
    SchreierSims<N> intersection(SchreierSims<N> const& S1,
                                 SchreierSims<N> const& S2) {
      if (S2.number_of_generators() < S1.number_of_generators())
        return intersection(S2, S1);

      SchreierSims<N> S2B;

      // Base change logic.
      // TODO: Make this a separate function?
      for (size_t i = 0; i < S2.number_of_generators(); ++i) {
        S2B.add_generator(S2.generator(i));
      }
      S1.run();
      for (size_t i = 0; i < S1.base_size(); ++i) {
        S2B.add_base_point(S1.base_point(i));
      }
      S2B.run();

      SchreierSims<N> T;


      T.run();
      return T;
    }
  }  // namespace schreier_sims_helper
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_SCHREIER_SIMS__HELPER_HPP_
