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

#include "cong-intf.hpp"

#include <cstddef>  // for size_t
#include <vector>   // for vector

#include "common.hpp"

#include "libsemigroups/exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION

namespace libsemigroups {

  namespace congruence {
    CongIntfArgs const& finite_examples(size_t id) {
      for (auto const& p : finite_examples()) {
        if (p.id == id) {
          return p;
        }
      }
      LIBSEMIGROUPS_EXCEPTION("id %llu unknown!", id);
    }

    std::vector<CongIntfArgs> const& finite_examples() {
      static std::vector<CongIntfArgs> examples
          = {{.id   = 0,
              .name = "Example 6.6 in Sims",
              .A    = 4,
              .R    = {{{0, 0}, {0}},
                    {{1, 0}, {1}},
                    {{0, 1}, {1}},
                    {{2, 0}, {2}},
                    {{0, 2}, {2}},
                    {{3, 0}, {3}},
                    {{0, 3}, {3}},
                    {{1, 1}, {0}},
                    {{2, 3}, {0}},
                    {{2, 2, 2}, {0}},
                    {{1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2}, {0}},
                    {{1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3,
                      1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3},
                     {0}}}}};
      return examples;
    }
  }  // namespace congruence
}  // namespace libsemigroups
