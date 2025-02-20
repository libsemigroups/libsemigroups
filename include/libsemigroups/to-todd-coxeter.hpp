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

#ifndef LIBSEMIGROUPS_TO_TODD_COXETER_HPP_
#define LIBSEMIGROUPS_TO_TODD_COXETER_HPP_

#include "froidure-pin.hpp"
#include "knuth-bendix-class.hpp"
#include "to-froidure-pin.hpp"
#include "todd-coxeter-class.hpp"

namespace libsemigroups {

  class FroidurePinBase;

  // TODO(1) to_todd_coxeter for FroidurePin<TCE> just return the original
  // ToddCoxeter instance.

  // TODO(0) to tpp
  template <typename Result, typename Node>
  auto to(congruence_kind knd, FroidurePinBase& fpb, WordGraph<Node> const& wg)
      -> std::enable_if_t<
          std::is_same_v<ToddCoxeter<typename Result::native_word_type>,
                         Result>,
          Result>;

  // TODO(0) allow template param "word_type" to be specified
  template <typename Result, typename Rewriter, typename ReductionOrder>
  ToddCoxeter<word_type>
  to_todd_coxeter(congruence_kind                                    knd,
                  detail::KnuthBendixImpl<Rewriter, ReductionOrder>& kb) {
    if (kb.number_of_classes() == POSITIVE_INFINITY) {
      LIBSEMIGROUPS_EXCEPTION(
          "cannot construct a ToddCoxeterImpl instance using the Cayley graph "
          "of an infinite KnuthBendixImpl<> object, maybe try "
          "ToddCoxeterImpl({}, kb.presentation()) instead?",
          kb.kind());
    }
    // TODO why are we doing this? Why not just use the active rules of kb?
    auto fp = to<FroidurePin>(kb);
    return to<ToddCoxeter<word_type>>(knd, fp, fp.right_cayley_graph());
  }

}  // namespace libsemigroups

#include "to-todd-coxeter.tpp"
#endif  // LIBSEMIGROUPS_TO_TODD_COXETER_HPP_
