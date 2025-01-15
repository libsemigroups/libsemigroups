//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2024 James D. Mitchell + Maria Tsalakou
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

// This file contains the declaration of the Kambites class implementing the
// algorithm described in:
//
// Kambites, M. (2009). Small overlap monoids. I. The word problem. J. Algebra,
// 321(8), 2187–2205.
//
// for solving the word problem in small overlap monoids, and a novel algorithm
// for computing normal forms in small overlap monoids, by Maria Tsalakou.

#ifndef LIBSEMIGROUPS_KAMBITES_HELPERS_HPP_
#define LIBSEMIGROUPS_KAMBITES_HELPERS_HPP_

#include "cong-intf-helpers.hpp"  // for helper declarations in congruence_interface namespace
#include "kambites-class.hpp"  // for Kambites

#include "detail/kambites-nf.hpp"  // for KambitesNormalFormRange

namespace libsemigroups {

  namespace congruence_interface {
    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - normal_forms
    ////////////////////////////////////////////////////////////////////////

    //! \brief Returns a range object containing normal forms.
    //!
    //! Defined in \c kambites.hpp.
    //!
    //! This function returns a range object containing short-lex normal forms
    //! of the classes of the congruence represented by a Kambites instance.
    //!
    //! \tparam Word the type of the words contained in the parameter \p k.
    //!
    //! \param k the \ref Kambites instance.
    //!
    //! \returns A range object.
    //!
    //! \throws LibsemigroupsException if the Kambites::small_overlap_class of
    //! \p k is not at least \f$4\f$.
    //!
    //! \warning The returned range object is always infinite.
    // TODO(1) should be allowed to specify the OutputWord here too, this
    // requires changes to KambitesNormalFormRange however.
    template <typename Word>
    auto normal_forms(Kambites<Word>& k) {
      k.throw_if_not_C4();
      return detail::KambitesNormalFormRange(k);
    }
  }  // namespace congruence_interface

  //! \ingroup kambites_group
  //!
  //! \brief Helper functions for the \ref Kambites class template.
  //!
  //! Defined in \c kambites.hpp.
  //!
  //! This page contains documentation for many helper functions for the \ref
  //! Kambites class template. In particular, these functions include versions
  //! of several of the member functions of \ref KnuthBendix (that accept
  //! iterators) whose parameters are not iterators, but objects instead. The
  //! helpers documented on this page all belong to the namespace `kambites`.
  //!
  //! \sa \ref cong_intf_helpers_group
  //!
  //! @{
  namespace kambites {
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
    // Interface helpers - normal_forms
    ////////////////////////////////////////////////////////////////////////

    using congruence_interface::normal_forms;

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - partition
    ////////////////////////////////////////////////////////////////////////

    using congruence_interface::partition;

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - non_trivial_classes
    ////////////////////////////////////////////////////////////////////////

    using congruence_interface::non_trivial_classes;

    // There's no non_trivial_classes(Kambites k1, Kambites k2) because it's
    // unclear how this could be computed (because they always define infinite
    // semigroups/monoids), so we can't just do non_trivial_classes(k1,
    // kambites::normal_forms(k2)) (as in ToddCoxeterBase) because there are
    // infinitely many normal_forms.

    //! @}
  }  // namespace kambites
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_KAMBITES_HELPERS_HPP_
