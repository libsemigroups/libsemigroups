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

#ifndef LIBSEMIGROUPS_BENCHMARKS_EXAMPLES_GENERATORS_HPP_
#define LIBSEMIGROUPS_BENCHMARKS_EXAMPLES_GENERATORS_HPP_

#include <cstddef>
#include <string>
#include <vector>

#include "libsemigroups/froidure-pin-base.hpp"
#include "libsemigroups/transf.hpp"

namespace libsemigroups {

  template <typename T>
  struct Generators {
    size_t         id;
    std::string    name;
    std::vector<T> gens;
  };

  template <typename S, typename T, typename SFINAE = S>
  auto make(T const& p) ->
      typename std::enable_if<std::is_base_of<FroidurePinBase, S>::value,
                              SFINAE*>::type {
    auto thing = new S();
    for (auto const& x : p.gens) {
      thing->add_generator(x);
    }
    return thing;
  }

  std::vector<Generators<LeastTransf<16>>> const& transf_examples();
  Generators<LeastTransf<16>> const&              transf_examples(size_t id);

  Generators<LeastTransf<16>> full_transf_monoid(size_t n);

}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_BENCHMARKS_EXAMPLES_GENERATORS_HPP_
