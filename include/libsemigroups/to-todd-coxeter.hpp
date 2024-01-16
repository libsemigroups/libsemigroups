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

#include "knuth-bendix.hpp"
#include "todd-coxeter.hpp"

namespace libsemigroups {

  class FroidurePinBase;
  // TODO uncomment  class KnuthBendix;
  class ToddCoxeter;
  enum class congruence_kind;

  ToddCoxeter to_todd_coxeter(congruence_kind knd, FroidurePinBase& fp);

  template <typename Rewriter, typename ReductionOrder>
  ToddCoxeter to_todd_coxeter(congruence_kind                        knd,
                              KnuthBendix<Rewriter, ReductionOrder>& kb) {
    if (kb.number_of_classes() == POSITIVE_INFINITY) {
      LIBSEMIGROUPS_EXCEPTION(
          "cannot construct a ToddCoxeter instance using the Cayley graph of "
          "an infinite KnuthBendix<> object, maybe try ToddCoxeter({}, "
          "kb.presentation()) instead?",
          kb.kind());
    }
    // TODO why are we doing this? Why not just use the active rules of kb?
    auto fp = to_froidure_pin(kb);
    return to_todd_coxeter(knd, fp);
  }

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_TO_TODD_COXETER_HPP_
