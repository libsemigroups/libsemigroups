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

#ifndef LIBSEMIGROUPS_TO_KNUTH_BENDIX_HPP_
#define LIBSEMIGROUPS_TO_KNUTH_BENDIX_HPP_

#include <type_traits>

#include "knuth-bendix-class.hpp"  // for KnuthBendix
#include "presentation.hpp"        // for Presentation
#include "to-presentation.hpp"     // for to<Presentation>

namespace libsemigroups {

  ////////////////////////////////////////////////////////////////////////
  // Forward decls
  ////////////////////////////////////////////////////////////////////////

  class FroidurePinBase;

  template <typename Word>
  class ToddCoxeter;

  enum class congruence_kind;

  //! \defgroup to_knuth_bendix_group to<KnuthBendix>
  //! \ingroup knuth_bendix_group
  //!
  //! \brief Convert to \ref_knuth_bendix instance.
  //!
  //! This page contains documentation related to converting `libsemigroups`
  //! objects into \ref_knuth_bendix instances.
  //!
  //!  \sa \ref to_group for an overview of possible conversions between
  //! `libsemigroups` types.

  //! \ingroup to_knuth_bendix_group
  //!
  //! \brief Convert a \ref FroidurePin object to a \ref_knuth_bendix object.
  //!
  //! Defined in \c to-knuth-bendix.hpp
  //!
  //! Despite the hideous signature, this function should be invoked as follows:
  //!
  //! \code
  //! \skip-test
  //! to<KnuthBendix<Word, Rewriter>>(knd, fpb);
  //! \endcode
  //!
  //! This function converts the \ref FroidurePin object \p fpb into a
  //! \ref_knuth_bendix object using the presentation obtained using
  //! \ref to<Presentation<Word>>(FroidurePinBase&).
  //!
  //! \tparam Result used for SFINAE, the return type of this function, must be
  //! `KnuthBendix<Word, Rewriter>` for some type \c Word and \c Rewriter.
  //!
  //! \param knd the kind of the congruence being constructed.
  //! \param fpb the FroidurePin instance to be converted.
  //!
  //! \returns A \ref_knuth_bendix object representing the trivial congruence
  //! over the semigroup defined by \p fpb.
  template <typename Result>
  auto to(congruence_kind knd, FroidurePinBase& fpb) -> std::enable_if_t<
      std::is_same_v<KnuthBendix<typename Result::native_word_type,
                                 typename Result::rewriter_type>,
                     Result>,
      Result> {
    using Word     = typename Result::native_word_type;
    using Rewriter = typename Result::rewriter_type;
    return KnuthBendix<Word, Rewriter>(knd, to<Presentation<Word>>(fpb));
  }

  //! \ingroup to_knuth_bendix_group
  //!
  //! \brief Convert a \ref_todd_coxeter object to a \ref_knuth_bendix object.
  //!
  //! Defined in \c to-knuth-bendix.hpp
  //!
  //! Despite the hideous signature, this function should be invoked as follows:
  //!
  //! \code
  //! \skip-test
  //! to<KnuthBendix>(knd, tc);
  //! \endcode
  //!
  //! This function converts the \ref_todd_coxeter object \p tc into a
  //! \ref_knuth_bendix object using ToddCoxeter::presentation.
  //!
  //! \tparam Thing used for SFINAE, must be \ref_knuth_bendix.
  //! \tparam Word the type of the words used in relations in \p kb.
  //!
  //! \param knd the kind of the congruence being constructed.
  //! \param tc the \ref_todd_coxeter object being converted.
  //!
  //! \returns A \ref_knuth_bendix object representing the trivial congruence
  //! over the semigroup defined by \p tc.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  template <template <typename...> typename Thing, typename Word>
  auto to(congruence_kind knd, ToddCoxeter<Word>& tc)
      -> std::enable_if_t<std::is_same_v<Thing<Word>, KnuthBendix<Word>>,
                          KnuthBendix<Word>> {
    // TODO(1) could do a version where we convert tc to FroidurePin, then use
    // the rules of that instead?
    return KnuthBendix<Word>(knd, tc.presentation());
  }

  //! \ingroup to_knuth_bendix_group
  //!
  //! \brief Convert a \ref_todd_coxeter object to a \ref_knuth_bendix object.
  //!
  //! Defined in \c to-knuth-bendix.hpp
  //!
  //! Despite the hideous signature, this function should be invoked as follows:
  //!
  //! \code
  //! \skip-test
  //! to<KnuthBendix<Word, Rewriter>>(knd, tc);
  //! \endcode
  //!
  //! This function converts the \ref_todd_coxeter object \p tc into a
  //! \ref_knuth_bendix object using ToddCoxeter::presentation.
  //!
  //! \tparam Result used for SFINAE, the return type of this function, must be
  //! `KnuthBendix<Word, Rewriter>` for some type \c Word and \c Rewriter.
  //!
  //! \param knd the kind of the congruence being constructed.
  //! \param tc the \ref_todd_coxeter object being converted.
  //!
  //! \returns A \ref_knuth_bendix object representing the trivial congruence
  //! over the semigroup defined by \p tc.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  template <typename Result>
  auto to(congruence_kind                                 knd,
          ToddCoxeter<typename Result::native_word_type>& tc)
      -> std::enable_if_t<
          std::is_same_v<KnuthBendix<typename Result::native_word_type,
                                     typename Result::rewriter_type>,
                         Result>,
          Result> {
    using Word     = typename Result::native_word_type;
    using Rewriter = typename Result::rewriter_type;
    return KnuthBendix<Word, Rewriter>(knd, tc.presentation());
  }

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_TO_KNUTH_BENDIX_HPP_
