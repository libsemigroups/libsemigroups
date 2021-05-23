
//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020 James D. Mitchell
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

#ifndef LIBSEMIGROUPS_BENCHMARKS_EXAMPLES_CONG_INTF_HPP_
#define LIBSEMIGROUPS_BENCHMARKS_EXAMPLES_CONG_INTF_HPP_
#include <cstddef>  // for size_t
#include <string>   // for string
#include <vector>   // for vector

#include "libsemigroups/cong-intf.hpp"
#include "libsemigroups/types.hpp"

namespace libsemigroups {
  struct CongIntfArgs {
    size_t                                       id;
    std::string                                  name;
    size_t                                       A;
    std::vector<std::pair<word_type, word_type>> R;
  };

  namespace congruence {
    std::vector<CongIntfArgs> const& finite_examples();
    CongIntfArgs const&              finite_examples(size_t);
  }  // namespace congruence

  template <typename S, typename T, typename SFINAE = S>
  auto make(T const& p) ->
      typename std::enable_if<std::is_base_of<CongruenceInterface, S>::value,
                              SFINAE*>::type {
    auto thing = new S(congruence_kind::twosided);
    thing->set_number_of_generators(p.A);
    for (auto const& x : p.R) {
      thing->add_pair(x.first, x.second);
    }
    return thing;
  }
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_BENCHMARKS_EXAMPLES_CONG_INTF_HPP_
