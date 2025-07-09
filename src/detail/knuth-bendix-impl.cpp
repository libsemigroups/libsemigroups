//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025 James D. Mitchell
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

#include "libsemigroups/detail/knuth-bendix-impl.hpp"

namespace libsemigroups {
  namespace detail {
    void prefixes_string(std::unordered_map<u8string,
                                            size_t,
                                            Hash<u8string>,
                                            std::equal_to<u8string>>& st,
                         u8string const&                              x,
                         size_t&                                      n) {
      for (auto it = x.cbegin() + 1; it < x.cend(); ++it) {
        auto w   = u8string(x.cbegin(), it);
        auto wit = st.find(w);
        if (wit == st.end()) {
          st.emplace(w, n);
          n++;
        }
      }
    }
  }  // namespace detail

}  // namespace libsemigroups
