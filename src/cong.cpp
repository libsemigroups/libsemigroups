//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 James D. Mitchell
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

// This file contains stuff for creating congruence over FroidurePinBase objects
// or over FpSemigroup objects.

#include "libsemigroups/cong.hpp"

#include <memory>  // for shared_ptr

#include "libsemigroups/cong-pair.hpp"  // for KnuthBendixCongruenceByPairs
#include "libsemigroups/debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/fpsemi.hpp"     // for FpSemigroup
#include "libsemigroups/froidure-pin-base.hpp"  // for FroidurePinBase
#include "libsemigroups/kambites.hpp"           // for Kambites
#include "libsemigroups/knuth-bendix.hpp"       // for KnuthBendix_
#include "libsemigroups/to-presentation.hpp"    // for to_presentation
#include "libsemigroups/to-todd-coxeter.hpp"    // for to_todd_coxeter
#include "libsemigroups/todd-coxeter.hpp"       // for congruence::ToddCoxeter

namespace libsemigroups {

  using KnuthBendix_ = congruence::KnuthBendix;
  //  using Kambites         = congruence::Kambites;
  using class_index_type = CongruenceInterface::class_index_type;
  using options          = congruence::ToddCoxeter::options;

  //////////////////////////////////////////////////////////////////////////
  // Congruence - constructors - public
  //////////////////////////////////////////////////////////////////////////

  Congruence::Congruence(congruence_kind type)
      : CongruenceInterface(type), _race() {}

  Congruence::Congruence(congruence_kind                  type,
                         std::shared_ptr<FroidurePinBase> S)
      : Congruence(type) {
    auto tc1
        = std::make_shared<ToddCoxeter>(type, to_presentation<word_type>(*S));
    _race.add_runner(tc1);

    // TODO if necessary make a runner that tries to S.run(), then get the
    // Cayley graph and use that in the ToddCoxeter, at present that'll happen
    // here in the constructor
    auto tc2 = std::make_shared<ToddCoxeter>(type, to_todd_coxeter(type, *S));
    _race.add_runner(tc2);
  }

  Congruence::Congruence(congruence_kind type, Presentation<word_type> const& p)
      : Congruence(type) {
    _race.max_threads(POSITIVE_INFINITY);
    if (type == congruence_kind::twosided) {
      _race.add_runner(std::make_shared<Kambites<detail::MultiStringView>>(p));
      _race.add_runner(std::make_shared<KnuthBendix>(p));
    }
    _race.add_runner(std::make_shared<ToddCoxeter>(type, p));
    // TODO add a Runner that tries to create a ToddCoxeter using the Cayley
    // graph of some FroidurePin
    LIBSEMIGROUPS_ASSERT(!_race.empty());
  }
}  // namespace libsemigroups
