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

#ifndef LIBSEMIGROUPS_TO_CONG_HPP_
#define LIBSEMIGROUPS_TO_CONG_HPP_

#include <type_traits>  // for enable_if_t, is_same_v

#include "cong-class.hpp"         // for Congruence
#include "froidure-pin-base.hpp"  // for FroidurePinBase
#include "to-todd-coxeter.hpp"    // for to<ToddCoxeter>
#include "types.hpp"              // for congruence_kind
#include "word-graph.hpp"         // for WordGraph

namespace libsemigroups {

  //! \defgroup to_cong_group to<Congruence>
  //! \ingroup congruence_group
  //!
  //! \brief Convert to Congruence instance.
  //!
  //! This page contains documentation related to converting a `libsemigroups`
  //! objects into a Congruence instances.
  //!
  //! \sa \ref to_group for an overview of possible conversions between
  //! `libsemigroups` types.

  //! \ingroup to_cong_group
  //!
  //! \brief Convert a \ref FroidurePin object to a \ref_congruence.
  //!
  //! Defined in \c to-cong.hpp
  //!
  //! Despite the hideous signature, this function should be invoked as follows:
  //!
  //! \code_no_test
  //! to<Congruence<Word>>(knd, fpb, wg);
  //! \end_code_no_test
  //!
  //! This function converts the \ref FroidurePin object \p fpb into a
  //! \ref_congruence object using the WordGraph \p wg (which should be either
  //! the \ref FroidurePinBase::left_cayley_graph or the
  //! \ref FroidurePinBase::right_cayley_graph of \p fpb).
  //!
  //! \tparam Result used for SFINAE, the return type of this function, must be
  //! `Congruence<Word>` for some type \c Word.
  //! \tparam Node the type of the nodes in the WordGraph \p wg.
  //!
  //! \param knd the kind of the congruence being constructed.
  //! \param fpb the FroidurePin instance to be converted.
  //! \param wg the left or right Cayley graph of \p fpb.
  //!
  //! \returns A \ref_congruence object representing the trivial congruence
  //! over the semigroup defined by \p fpb.
  //!
  //! \throws LibsemigroupsException if \p wg is not the
  //! \ref FroidurePinBase::left_cayley_graph or the
  //! \ref FroidurePinBase::right_cayley_graph of \p fpb.
  template <typename Result, typename Node>
  auto to(congruence_kind knd, FroidurePinBase& fpb, WordGraph<Node> const& wg)
      -> std::enable_if_t<
          std::is_same_v<Congruence<typename Result::native_word_type>, Result>,
          Result>;

  //! \ingroup to_cong_group
  //!
  //! \brief Convert a \ref WordGraph object to a \ref_congruence.
  //!
  //! Defined in \c to-cong.hpp
  //!
  //! Despite the hideous signature, this function should be invoked as follows:
  //!
  //! \code_no_test
  //! to<Congruence<Word>>(knd, wg);
  //! \end_code_no_test
  //!
  //! This function converts the \ref WordGraph object \p wg into a
  //! \ref_congruence object by constructing a \ref_todd_coxeter instance from
  //! \p wg and adding that as a runner to the \ref_congruence.
  //!
  //! \tparam Result used for SFINAE, the return type of this function, must be
  //! `Congruence<Word>` for some type \c Word.
  //! \tparam Node the type of the nodes in the \ref WordGraph \p wg.
  //!
  //! \param knd the kind of the congruence being constructed.
  //! \param wg the word graph \p wg.
  //!
  //! \returns A \ref_congruence object representing the trivial congruence
  //! over \p wg.
  //!
  //! \warning The \ref WordGraph \p wg is added as is to the constructed
  //! \ref_congruence, and no checks are performed that the constructed
  //! \ref_todd_coxeter or \ref_congruence object are valid.
  template <typename Result, typename Node>
  auto to(congruence_kind knd, WordGraph<Node> const& wg) -> std::enable_if_t<
      std::is_same_v<Congruence<typename Result::native_word_type>, Result>,
      Result>;

}  // namespace libsemigroups

#include "to-cong.tpp"

#endif  // LIBSEMIGROUPS_TO_CONG_HPP_
