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

#include "libsemigroups/siso.hpp"

#include <cstddef>  // for size_t
#include <string>   // for string
#include <utility>  // for make_pair

#include "libsemigroups/wilo.hpp"   // for cbegin_wilo
#include "libsemigroups/wislo.hpp"  // for cbegin_wislo
#include "libsemigroups/word.hpp"   // for StringToWord

namespace libsemigroups {

  const_silo_iterator cbegin_silo(std::string const& alphabet,
                                  size_t const       upper_bound,
                                  std::string const& first,
                                  std::string const& last) {
    StringToWord string_to_word(alphabet);
    return const_silo_iterator(std::make_pair(alphabet, std::string()),
                               cbegin_wilo(alphabet.size(),
                                           upper_bound,
                                           string_to_word(first),
                                           string_to_word(last)));
  }

  const_silo_iterator cend_silo(std::string const& alphabet,
                                size_t const       upper_bound,
                                std::string const& first,
                                std::string const& last) {
    StringToWord string_to_word(alphabet);
    return const_silo_iterator(std::make_pair(alphabet, std::string()),
                               cend_wilo(alphabet.size(),
                                         upper_bound,
                                         string_to_word(first),
                                         string_to_word(last)));
  }

  const_sislo_iterator cbegin_sislo(std::string const& alphabet,
                                    std::string const& first,
                                    std::string const& last) {
    StringToWord string_to_word(alphabet);
    return const_sislo_iterator(std::make_pair(alphabet, std::string()),
                                cbegin_wislo(alphabet.size(),
                                             string_to_word(first),
                                             string_to_word(last)));
  }

  const_sislo_iterator cend_sislo(std::string const& alphabet,
                                  std::string const& first,
                                  std::string const& last) {
    StringToWord string_to_word(alphabet);
    return const_sislo_iterator(std::make_pair(alphabet, std::string()),
                                cend_wislo(alphabet.size(),
                                           string_to_word(first),
                                           string_to_word(last)));
  }
}  // namespace libsemigroups
