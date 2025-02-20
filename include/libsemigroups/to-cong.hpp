//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022 James D. Mitchell
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

#ifndef LIBSEMIGROUPS_TO_CONG_HPP_
#define LIBSEMIGROUPS_TO_CONG_HPP_

// TODO(0) iwyu

#include "cong-class.hpp"
#include "froidure-pin-base.hpp"
#include "to-todd-coxeter.hpp"
#include "types.hpp"
#include "word-graph.hpp"

namespace libsemigroups {

  //! \brief Re-initialize from congruence_kind, FroidurePin, and WordGraph.
  //!
  //! This function re-initializes a Congruence instance as if it had been
  //! newly constructed over the FroidurePin instance \p S representing a 1-
  //! or 2-sided congruence according to \p knd.
  //!
  //! \tparam Node the type of the nodes in the 3rd argument \p wg (word
  //! graph).
  //!
  //! \param knd the kind (onesided or twosided) of the congruence.
  //! \param S a reference to the FroidurePin over which the congruence
  //! is being defined.
  //! \param wg the left or right Cayley graph of S.
  //!
  //! \returns A reference to `*this`.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \warning This constructor does not check its arguments.
  template <typename Result, typename Node>
  auto to(congruence_kind knd, FroidurePinBase& fpb, WordGraph<Node> const& wg)
      -> std::enable_if_t<
          std::is_same_v<Congruence<typename Result::native_word_type>, Result>,
          Result>;

}  // namespace libsemigroups

#include "to-cong.tpp"

#endif  // LIBSEMIGROUPS_TO_CONG_HPP_
