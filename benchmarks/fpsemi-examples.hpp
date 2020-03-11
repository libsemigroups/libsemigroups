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

#ifndef LIBSEMIGROUPS_BENCHMARKS_FPSEMI_EXAMPLES_HPP_
#define LIBSEMIGROUPS_BENCHMARKS_FPSEMI_EXAMPLES_HPP_

#include <cstddef>  // for size_t
#include <string>   // for string
#include <vector>   // for vector

namespace libsemigroups {

  struct StringPresentation {
    size_t                                           id;
    std::string                                      name;
    std::string                                      A;
    std::vector<std::pair<std::string, std::string>> R;
  };

  std::vector<StringPresentation> const& string_infinite_examples();
  StringPresentation const&              string_infinite_examples(size_t);

}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_BENCHMARKS_FPSEMI_EXAMPLES_HPP_
