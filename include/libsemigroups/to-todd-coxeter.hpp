//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023 James D. Mitchell
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
//
// This file contains declarations for overloads of the "to" function for
// outputting a ToddCoxeter object.

#ifndef LIBSEMIGROUPS_TO_TODD_COXETER_HPP_
#define LIBSEMIGROUPS_TO_TODD_COXETER_HPP_

#include <type_traits>  // for is_same_v, enable_if_t

#include "constants.hpp"           // for POSITIVE_INFINITY
#include "exception.hpp"           // for LIBSEMIGROUPS_EXCEPTION
#include "froidure-pin.hpp"        // for FroidurePin
#include "knuth-bendix-class.hpp"  // for KnuthBendix
#include "to-froidure-pin.hpp"     // for to<FroidurePin>
#include "todd-coxeter-class.hpp"  // for ToddCoxeter

namespace libsemigroups {

  // TODO(1):
  // * to<ToddCoxeter> for FroidurePin<TCE> just return the original
  // ToddCoxeter instance.
  // * to<ToddCoxeter<std::string>>(ToddCoxeter<word_type> const&)
  // * to<ToddCoxeter<std::string>>(KnuthBendix<word_type> const&)

  // TODO(0) doc
  template <typename Result, typename Node>
  auto to(congruence_kind knd, FroidurePinBase& fpb, WordGraph<Node> const& wg)
      -> std::enable_if_t<
          std::is_same_v<ToddCoxeter<typename Result::native_word_type>,
                         Result>,
          Result>;

  // TODO(0) doc
  template <template <typename...> typename Thing,
            typename Word,
            typename Rewriter,
            typename ReductionOrder>
  auto to(congruence_kind knd, KnuthBendix<Word, Rewriter, ReductionOrder>& kb)
      -> std::enable_if_t<std::is_same_v<ToddCoxeter<Word>, Thing<Word>>,
                          ToddCoxeter<Word>>;

}  // namespace libsemigroups

#include "to-todd-coxeter.tpp"
#endif  // LIBSEMIGROUPS_TO_TODD_COXETER_HPP_
