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
#include "cong-common-helpers.hpp"  // for partition, add_gener...
#include "exception.hpp"          // for LIBSEMIGROUPS_EXCEPTION
#include "kambites-class.hpp"     // for Kambites
#include "todd-coxeter-base.hpp"  // for ToddCoxeterBase
#include "types.hpp"              // for word_type
//
#include "detail/knuth-bendix-base.hpp"  // for KnuthBendixBase

namespace libsemigroups {

  // forward decl
  template <typename Word>
  class Presentation;

  namespace congruence_common {
    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - normal_forms
    ////////////////////////////////////////////////////////////////////////

    // TODO(1) implement (see tpp file)

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - partition
    ////////////////////////////////////////////////////////////////////////

    // The function template:
    //
    // template <typename Word,
    //           typename Range,
    //           typename = std::enable_if_t<rx::is_input_or_sink_v<Range>>>
    // [[nodiscard]] std::vector<std::vector<Word>> partition(Congruence<Word>&
    // ci, Range r);
    //
    // is forward declared in cong-common-helpers.hpp

  }  // namespace congruence_common

  //! \ingroup cong_all_group
  //!
  //! Helper functions for the Congruence class template
  //!
  //! Defined in \c cong-helpers.hpp.
  //!
  //! This page contains documentation for many helper functions for the
  //! Congruence class template. In particular, these functions include versions
  //! of several of the member functions of the Congruence class template (that
  //! accept iterators) whose parameters are not iterators, but the underlying
  //! objects. The helpers documented on this page all belong to the namespace
  //! \ref congruence and \ref cong_intf_helpers_group "congruence_common".
  namespace congruence {

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - add_generating_pair
    ////////////////////////////////////////////////////////////////////////

    using congruence_common::add_generating_pair;
    using congruence_common::add_generating_pair_no_checks;

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - contains
    ////////////////////////////////////////////////////////////////////////

    using congruence_common::contains;
    using congruence_common::contains_no_checks;
    using congruence_common::currently_contains;
    using congruence_common::currently_contains_no_checks;

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - reduce
    ////////////////////////////////////////////////////////////////////////

    using congruence_common::reduce;
    using congruence_common::reduce_no_checks;
    using congruence_common::reduce_no_run;
    using congruence_common::reduce_no_run_no_checks;

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - partitioning
    ////////////////////////////////////////////////////////////////////////

    using congruence_common::non_trivial_classes;
    using congruence_common::partition;

  }  // namespace congruence
}  // namespace libsemigroups

#include "cong-helpers.tpp"
#endif  // LIBSEMIGROUPS_CONG_HELPERS_HPP_
