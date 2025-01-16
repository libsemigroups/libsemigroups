//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2024 James D. Mitchell
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

// This file contains declarations related to the helper functions for the
// Congruence class.

#ifndef LIBSEMIGROUPS_CONG_HELPERS_HPP_
#define LIBSEMIGROUPS_CONG_HELPERS_HPP_

#include <memory>  // for shared_ptr
#include <string>  // for string
#include <vector>  // for vector

#include "cong-class.hpp"         // for Congruence
#include "cong-intf-helpers.hpp"  // for partition, add_gener...
#include "exception.hpp"          // for LIBSEMIGROUPS_EXCEPTION
#include "kambites-class.hpp"     // for Kambites
#include "knuth-bendix-base.hpp"  // for KnuthBendixBase
#include "todd-coxeter-base.hpp"  // for ToddCoxeterBase
#include "types.hpp"              // for word_type

namespace libsemigroups {

  // forward decl
  template <typename Word>
  class Presentation;

  namespace congruence_interface {
    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - normal_forms
    ////////////////////////////////////////////////////////////////////////

    // TODO(1) implement (see tpp file)

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - partition
    ////////////////////////////////////////////////////////////////////////

    // The function template:
    //
    // template <typename Range, typename OutputWord, typename>
    // [[nodiscard]] std::vector<std::vector<OutputWord>>
    // partition(Congruence& cong, Range r);
    //
    // is declared in cong-intf-helpers.hpp

  }  // namespace congruence_interface

  // There's no doc for anything in this section it's covered by the
  // congruence interface helpers.
  namespace congruence {

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - add_generating_pair
    ////////////////////////////////////////////////////////////////////////

    using congruence_interface::add_generating_pair;
    using congruence_interface::add_generating_pair_no_checks;

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - contains
    ////////////////////////////////////////////////////////////////////////

    using congruence_interface::contains;
    using congruence_interface::contains_no_checks;
    using congruence_interface::currently_contains;
    using congruence_interface::currently_contains_no_checks;

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - reduce
    ////////////////////////////////////////////////////////////////////////

    using congruence_interface::reduce;
    using congruence_interface::reduce_no_checks;
    using congruence_interface::reduce_no_run;
    using congruence_interface::reduce_no_run_no_checks;

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - partitioning
    ////////////////////////////////////////////////////////////////////////

    using congruence_interface::non_trivial_classes;
    using congruence_interface::partition;

  }  // namespace congruence
}  // namespace libsemigroups

#include "cong-helpers.tpp"
#endif  // LIBSEMIGROUPS_CONG_HELPERS_HPP_
