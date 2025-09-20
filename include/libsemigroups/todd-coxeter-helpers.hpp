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

// This file contains the declarations of some helper functions for the
// ToddCoxeter<Word> class.

#ifndef LIBSEMIGROUPS_TODD_COXETER_HELPERS_HPP_
#define LIBSEMIGROUPS_TODD_COXETER_HELPERS_HPP_

#include <algorithm>         // for reverse
#include <chrono>            // for milliseconds
#include <cstring>           // for strlen, size_t
#include <initializer_list>  // for begin, end, initi...
#include <iterator>          // for back_inserter
#include <string>            // for basic_string, string
#include <type_traits>       // for is_integral_v
#include <utility>           // for move
#include <vector>            // for vector

#include "cong-common-helpers.hpp"  // for partition, add_ge...
#include "constants.hpp"            // for UNDEFINED, operat...
#include "exception.hpp"            // for LIBSEMIGROUPS_EXC...
#include "paths.hpp"                // for Paths
#include "presentation.hpp"         // for Presentation::val...
#include "ranges.hpp"               // for seq, operator|
#include "todd-coxeter-class.hpp"   // for ToddCoxeter
#include "types.hpp"                // for word_type, congru...
#include "word-graph.hpp"           // for follow_path_no_ch...

#include "detail/fmt.hpp"             // for format
#include "detail/path-iterators.hpp"  // for const_pilo_iterat...

namespace libsemigroups {
  //! This page contains documentation for everything in the namespace
  //! \ref todd_coxeter. This includes everything from
  //! \ref cong_common_helpers_group and \ref todd_coxeter_helpers_group.
  namespace todd_coxeter {
    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter<Word> helpers
    ////////////////////////////////////////////////////////////////////////

#ifndef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
    // This is just for our convenience here, so not documented.
    using index_type = typename detail::ToddCoxeterImpl::index_type;
#endif

    //! \defgroup todd_coxeter_helpers_group ToddCoxeter helper functions
    //! \ingroup todd_coxeter_group
    //!
    //! \brief Helper functions for the \ref_todd_coxeter class.
    //!
    //! Defined in \c todd-coxeter-helpers.hpp.
    //!
    //! This page contains documentation for many helper functions for the
    //! \ref_todd_coxeter class. In particular, these functions include versions
    //! of several of the member functions of \ref_todd_coxeter (that accept
    //! iterators) whose parameters are not iterators, but objects instead. The
    //! helpers documented on this page all belong to the namespace
    //! \ref todd_coxeter.
    //!
    //! \sa \ref cong_common_helpers_group
    //!
    //! @{

    // TODO(1) this group is a bit hard to look at, it'd be better if all the
    // overloads of a given function were on one page, with a single bit of
    // documentation. This is because the overloads all do the same thing, and
    // so there's no real benefit in repeating the doc over and over again.

    ////////////////////////////////////////////////////////////////////////
    // Possible future interface helpers - word -> index
    ////////////////////////////////////////////////////////////////////////

    //! \brief Returns the current index of the class containing a word.
    //!
    //! Defined in \c todd-coxeter-helpers.hpp.
    //!
    //! This function just calls
    //! \code
    //! tc.current_index_of_no_checks(std::begin(w), std::end(w));
    //! \endcode
    //!
    //! See ToddCoxeter::current_index_of_no_checks for details.
    //!
    //! \tparam Word the type of the second argument \p w.
    //!
    //! \param tc the \ref_todd_coxeter instance.
    //! \param w the word.
    //!
    //! \returns The current index of the class containing the word.
    template <typename Word>
    [[nodiscard]] index_type
    current_index_of_no_checks(ToddCoxeter<Word> const& tc, Word const& w) {
      return tc.current_index_of_no_checks(std::begin(w), std::end(w));
    }

    //! \brief Returns the current index of the class containing a word.
    //!
    //! Defined in \c todd-coxeter-helpers.hpp.
    //!
    //! This function just calls
    //! \code
    //! tc.current_index_of(std::begin(w), std::end(w));
    //! \endcode
    //!
    //! See \ref ToddCoxeter::current_index_of for details.
    //!
    //! \tparam Word the type of the second argument \p w.
    //!
    //! \param tc the  \ref_todd_coxeter instance.
    //! \param w the word.
    //!
    //! \returns The current index of the class containing the word.
    template <typename Word>
    [[nodiscard]] index_type current_index_of(ToddCoxeter<Word> const& tc,
                                              Word const&              w) {
      return tc.current_index_of(std::begin(w), std::end(w));
    }

    //! \brief Returns the index of the class containing a word.
    //!
    //! Defined in \c todd-coxeter-helpers.hpp.
    //!
    //! This function just calls
    //! \code
    //! tc.index_of_no_checks(std::begin(w), std::end(w));
    //! \endcode
    //!
    //! See ToddCoxeter::index_of_no_checks for details.
    //!
    //! \tparam Word the type of the second argument \p w.
    //!
    //! \param tc the  \ref_todd_coxeter instance.
    //! \param w the word.
    //!
    //! \returns The index of the class containing the word.
    template <typename Word>
    [[nodiscard]] index_type index_of_no_checks(ToddCoxeter<Word>& tc,
                                                Word const&        w) {
      return tc.index_of_no_checks(std::begin(w), std::end(w));
    }

    //! \brief Returns the index of the class containing a word.
    //!
    //! Defined in \c todd-coxeter-helpers.hpp.
    //!
    //! This function just calls
    //! \code
    //! tc.index_of(std::begin(w), std::end(w));
    //! \endcode
    //!
    //! See \ref ToddCoxeter::index_of for details.
    //!
    //! \tparam Word the type of the second argument \p w.
    //!
    //! \param tc the  \ref_todd_coxeter instance.
    //! \param w the word.
    //!
    //! \returns The index of the class containing the word.
    template <typename Word>
    [[nodiscard]] index_type index_of(ToddCoxeter<Word>& tc, Word const& w) {
      return tc.index_of(std::begin(w), std::end(w));
    }

    //! \brief Returns the current index of the class containing a word.
    //!
    //! Defined in \c todd-coxeter-helpers.hpp.
    //!
    //! This function just calls
    //! \code
    //! tc.current_index_of_no_checks(std::begin(w), std::end(w));
    //! \endcode
    //!
    //! See ToddCoxeter::current_index_of_no_checks for details.
    //!
    //! \tparam Int the type of items in the second argument \p w.
    //!
    //! \param tc the  \ref_todd_coxeter instance.
    //! \param w the word.
    //!
    //! \returns The current index of the class containing the word.
    template <typename Word, typename Int>
    [[nodiscard]] index_type
    current_index_of_no_checks(ToddCoxeter<Word>&                tc,
                               std::initializer_list<Int> const& w) {
      static_assert(std::is_integral_v<Int>);
      return tc.current_index_of_no_checks(std::begin(w), std::end(w));
    }

    //! \brief Returns the current index of the class containing a word.
    //!
    //! Defined in \c todd-coxeter-helpers.hpp.
    //!
    //! This function just calls
    //! \code
    //! tc.current_index_of(std::begin(w), std::end(w));
    //! \endcode
    //!
    //! See \ref ToddCoxeter::current_index_of for details.
    //!
    //! \tparam Int the type of items in the second argument \p w.
    //!
    //! \param tc the  \ref_todd_coxeter instance.
    //! \param w the word.
    //!
    //! \returns The current index of the class containing the word.
    template <typename Word, typename Int>
    [[nodiscard]] index_type
    current_index_of(ToddCoxeter<Word>&                tc,
                     std::initializer_list<Int> const& w) {
      static_assert(std::is_integral_v<Int>);
      return tc.current_index_of(std::begin(w), std::end(w));
    }

    //! \brief Returns the index of the class containing a word.
    //!
    //! Defined in \c todd-coxeter-helpers.hpp.
    //!
    //! This function just calls
    //! \code
    //! tc.index_of_no_checks(std::begin(w), std::end(w));
    //! \endcode
    //!
    //! See \ref ToddCoxeter::index_of_no_checks for details.
    //!
    //! \tparam Int the type of items in the second argument \p w.
    //!
    //! \param tc the  \ref_todd_coxeter instance.
    //! \param w the word.
    //!
    //! \returns The index of the class containing the word.
    template <typename Word, typename Int>
    [[nodiscard]] index_type
    index_of_no_checks(ToddCoxeter<Word>&                tc,
                       std::initializer_list<Int> const& w) {
      static_assert(std::is_integral_v<Int>);
      return tc.index_of_no_checks(std::begin(w), std::end(w));
    }

    //! \brief Returns the index of the class containing a word.
    //!
    //! Defined in \c todd-coxeter-helpers.hpp.
    //!
    //! This function just calls
    //! \code
    //! tc.index_of(std::begin(w), std::end(w));
    //! \endcode
    //!
    //! See \ref ToddCoxeter::index_of for details.
    //!
    //! \tparam Int the type of items in the second argument \p w.
    //!
    //! \param tc the  \ref_todd_coxeter instance.
    //! \param w the word.
    //!
    //! \returns The index of the class containing the word.
    template <typename Word, typename Int>
    [[nodiscard]] index_type index_of(ToddCoxeter<Word>&                tc,
                                      std::initializer_list<Int> const& w) {
      static_assert(std::is_integral_v<Int>);
      return tc.index_of(std::begin(w), std::end(w));
    }

    //! \brief Returns the current index of the class containing a word.
    //!
    //! Defined in \c todd-coxeter-helpers.hpp.
    //!
    //! This function just calls
    //! \code
    //! tc.current_index_of_no_checks(w, w + std::strlen(w));
    //! \endcode
    //!
    //! See ToddCoxeter::current_index_of_no_checks for details.
    //!
    //! \param tc the  \ref_todd_coxeter instance.
    //! \param w the word.
    //!
    //! \returns The current index of the class containing the word.
    template <typename Word>
    [[nodiscard]] inline index_type
    current_index_of_no_checks(ToddCoxeter<Word>& tc, char const* w) {
      LIBSEMIGROUPS_ASSERT(w != nullptr);
      return tc.current_index_of_no_checks(w, w + std::strlen(w));
    }

    //! \brief Returns the current index of the class containing a word.
    //!
    //! Defined in \c todd-coxeter-helpers.hpp.
    //!
    //! This function just calls
    //! \code
    //! tc.current_index_of(w, w + std::strlen(w));
    //! \endcode
    //!
    //! See ToddCoxeter::current_index_of for details.
    //!
    //! \param tc the  \ref_todd_coxeter instance.
    //! \param w the word.
    //!
    //! \returns The current index of the class containing the word.
    template <typename Word>
    [[nodiscard]] inline index_type current_index_of(ToddCoxeter<Word>& tc,
                                                     char const*        w) {
      detail::throw_if_nullptr(w, "2nd");
      return tc.current_index_of(w, w + std::strlen(w));
    }

    //! \brief Returns the index of the class containing a word.
    //!
    //! Defined in \c todd-coxeter-helpers.hpp.
    //!
    //! This function just calls
    //! \code
    //! tc.index_of_no_checks(w, w + std::strlen(w));
    //! \endcode
    //!
    //! See ToddCoxeter::index_of_no_checks for details.
    //!
    //! \param tc the  \ref_todd_coxeter instance.
    //! \param w the word.
    //!
    //! \returns The current index of the class containing the word.
    template <typename Word>
    [[nodiscard]] inline index_type index_of_no_checks(ToddCoxeter<Word>& tc,
                                                       char const*        w) {
      LIBSEMIGROUPS_ASSERT(w != nullptr);
      return tc.index_of_no_checks(w, w + std::strlen(w));
    }

    //! \brief Returns the index of the class containing a word.
    //!
    //! Defined in \c todd-coxeter-helpers.hpp.
    //!
    //! This function just calls
    //! \code
    //! tc.index_of(w, w + std::strlen(w));
    //! \endcode
    //!
    //! See ToddCoxeter::index_of for details.
    //!
    //! \param tc the  \ref_todd_coxeter instance.
    //! \param w the word.
    //!
    //! \returns The current index of the class containing the word.
    template <typename Word>
    [[nodiscard]] inline index_type index_of(ToddCoxeter<Word>& tc,
                                             char const*        w) {
      detail::throw_if_nullptr(w, "2nd");
      return tc.index_of(w, w + std::strlen(w));
    }

    ////////////////////////////////////////////////////////////////////////
    // Possible future interface helpers - index -> word
    ////////////////////////////////////////////////////////////////////////

    //! \brief Returns a word representing a class with given index.
    //!
    //! Defined in \c todd-coxeter-helpers.hpp.
    //!
    //! See ToddCoxeter::word_of_no_checks for details.
    //!
    //! \tparam Word the type of the returned word (default:
    //! \ref libsemigroups::word_type "word_type").
    //!
    //! \param tc the  \ref_todd_coxeter instance.
    //! \param i the index of the class.
    //!
    //! \returns A representative of the class with given index.
    template <typename Word>
    [[nodiscard]] Word current_word_of_no_checks(ToddCoxeter<Word>& tc,
                                                 index_type         i);

    //! \brief Returns a word representing a class with given index.
    //!
    //! Defined in \c todd-coxeter-helpers.hpp.
    //!
    //! See ToddCoxeter::word_of for details.
    //!
    //! \tparam Word the type of the returned word (default:
    //! \ref libsemigroups::word_type "word_type").
    //!
    //! \param tc the  \ref_todd_coxeter instance.
    //! \param i the index of the class.
    //!
    //! \returns A representative of the class with given index.
    template <typename Word>
    [[nodiscard]] Word current_word_of(ToddCoxeter<Word>& tc, index_type i);

    //! \brief Returns a word representing a class with given index.
    //!
    //! Defined in \c todd-coxeter-helpers.hpp.
    //!
    //! See ToddCoxeter::word_of_no_checks for details.
    //!
    //! \tparam Word the type of the returned word (default:
    //! \ref libsemigroups::word_type "word_type").
    //!
    //! \param tc the \ref_todd_coxeter instance.
    //! \param i the index of the class.
    //!
    //! \returns A representative of the class with given index.
    template <typename Word>
    [[nodiscard]] Word word_of_no_checks(ToddCoxeter<Word>& tc, index_type i);

    //! \brief Returns a word representing a class with given index.
    //!
    //! Defined in \c todd-coxeter-helpers.hpp.
    //!
    //! See ToddCoxeter::word_of for details.
    //!
    //! \tparam Word the type of the returned word (default:
    //! \ref libsemigroups::word_type "word_type").
    //!
    //! \param tc the  \ref_todd_coxeter instance.
    //! \param i the index of the class.
    //!
    //! \returns A representative of the class with given index.
    template <typename Word>
    [[nodiscard]] Word word_of(ToddCoxeter<Word>& tc, index_type i);

    ////////////////////////////////////////////////////////////////////////
    // Possible future interface helpers - class_of
    ////////////////////////////////////////////////////////////////////////

    //! \brief Returns a range object containing every word in the congruence
    //! class with given index.
    //!
    //! Defined in \c todd-coxeter-helpers.hpp.
    //!
    //! This function returns a range object containing every word in belonging
    //! to the class with index \p n in the congruence represented by the
    //!  \ref_todd_coxeter instance \p tc. Calls
    //!  to this function trigger a full
    //! enumeration of \p tc.
    //!
    //! \param tc the  \ref_todd_coxeter
    //! instance.
    //! \param n the index of the class.
    //!
    //! \returns A range object containing the class with index \p n.
    //!
    //! \throws LibsemigroupsException if \p n is greater than or equal to
    //! `tc.number_of_classes()`.
    //!
    //! \cong_common_warn_undecidable{Todd-Coxeter}
    // Can't out of line this because of auto return type
    template <typename Word>
    [[nodiscard]] auto class_by_index(ToddCoxeter<Word>& tc, index_type n) {
      size_t const offset = (tc.presentation().contains_empty_word() ? 0 : 1);
      tc.run();
      // We call run and then current_word_graph, because the word
      // graph does not need to be standardized for this to work.
      // TODO(1) again there are alots of copies here
      // TODO(1) this also has the disadvantage that we can't set the various
      // settings in the Paths object, in particular, the size_hint + count
      // functions!
      return Paths(tc.current_word_graph()).source(0).target(n + offset)
             | rx::transform([&tc](auto const& w) {
                 Word ww;
                 for (auto index : w) {
                   ww.push_back(tc.presentation().letter_no_checks(index));
                 }
                 return ww;
               });
    }

    //! \brief Returns a range object containing every word in the congruence
    //! class with given index.
    //!
    //! Defined in \c todd-coxeter-helpers.hpp.
    //!
    //! This function returns a range object containing every word in belonging
    //! to the class with index \p n in the congruence represented by the
    //!  \ref_todd_coxeter instance \p tc. Calls
    //!  to this function trigger a full
    //! enumeration of \p tc.
    //!
    //! \param tc the  \ref_todd_coxeter
    //! instance.
    //! \param n the index of the class.
    //!
    //! \returns A range object containing the class with index \p n.
    //!
    //! \warning This function does not check its arguments. In particular, it
    //! is assumed that \p n is strictly less than `tc.number_of_classes()`.
    //!
    //! \cong_common_warn_undecidable{Todd-Coxeter}
    // Can't out of line this because of auto return type
    template <typename Word>
    [[nodiscard]] auto class_by_index_no_checks(ToddCoxeter<Word>& tc,
                                                index_type         n) {
      size_t const offset = (tc.presentation().contains_empty_word() ? 0 : 1);
      tc.run();
      // We call run and then current_word_graph, because the word
      // graph does not need to be standardized for this to work.
      // TODO(1) again there are alots of copies here
      // TODO(1) this also has the disadvantage that we can't set the various
      // settings in the Paths object, in particular, the size_hint + count
      // functions!
      return Paths(tc.current_word_graph())
                 .source_no_checks(0)
                 .target_no_checks(n + offset)
             | rx::transform([&tc](auto const& w) {
                 Word ww;
                 for (auto index : w) {
                   ww.push_back(tc.presentation().letter_no_checks(index));
                 }
                 return ww;
               });
    }

    //! \brief Returns a range object containing every word in the congruence
    //! class of a word given by iterators.
    //!
    //! Defined in \c todd-coxeter-helpers.hpp.
    //!
    //! This function returns a range object containing every word in belonging
    //! to the same class as the word (contained in the range from \p first to
    //! \p last) in the congruence represented by the  \ref_todd_coxeter
    //! instance \p tc. Calls to this function trigger a full enumeration of
    //! \p tc.
    //!
    //! \tparam Iterator1 the type of the 2nd argument \p first.
    //! \tparam Iterator2 the type of the 3rd argument \p last.
    //!
    //! \param tc the  \ref_todd_coxeter instance.
    //! \param first iterator pointing at the first letter of the word.
    //! \param last iterator pointing one beyond the last letter of the word.
    //!
    //! \returns A range object containing the words in the class of the input
    //! word.
    //!
    //! \cong_common_throws_if_letters_out_of_bounds
    //!
    //! \cong_common_warn_undecidable{Todd-Coxeter}
    template <typename Word, typename Iterator1, typename Iterator2>
    [[nodiscard]] auto class_of(ToddCoxeter<Word>& tc,
                                Iterator1          first,
                                Iterator2          last) {
      return class_by_index(tc, tc.index_of(first, last));
    }

    //! \brief Returns a range object containing every word in the congruence
    //! class of a word given by iterators.
    //!
    //! Defined in \c todd-coxeter-helpers.hpp.
    //!
    //! This function returns a range object containing every word in belonging
    //! to the same class as the word (contained in the range from \p first to
    //! \p last) in the congruence represented by the  \ref_todd_coxeter
    //! instance \p tc. Calls to this function trigger a full enumeration of
    //! \p tc.
    //!
    //! \tparam Iterator1 the type of the 2nd argument \p first.
    //! \tparam Iterator2 the type of the 3rd argument \p last.
    //!
    //! \param tc the  \ref_todd_coxeter instance.
    //! \param first iterator pointing at the first letter of the word.
    //! \param last iterator pointing one beyond the last letter of the word.
    //!
    //! \returns A range object containing the words in the class of the input
    //! word.
    //!
    //! \cong_common_warn_assume_letters_in_bounds
    //!
    //! \cong_common_warn_undecidable{Todd-Coxeter}
    template <typename Word, typename Iterator1, typename Iterator2>
    [[nodiscard]] auto class_of_no_checks(ToddCoxeter<Word>& tc,
                                          Iterator1          first,
                                          Iterator2          last) {
      return class_by_index_no_checks(tc, tc.index_of_no_checks(first, last));
    }

    //! \brief Returns a range object containing every word in the congruence
    //! class of a given word.
    //!
    //! Defined in \c todd-coxeter-helpers.hpp.
    //!
    //! This function returns a range object containing every word in
    //! belonging to the same class as the input word \p w in the congruence
    //! represented by the  \ref_todd_coxeter
    //! instance
    //! \p tc. Calls to this function trigger a full enumeration of \p tc.
    //!
    //! \tparam Word the type of the 2nd argument \p w.
    //!
    //! \param tc the  \ref_todd_coxeter
    //! instance.
    //! \param w the input word.
    //!
    //! \returns A range object containing the words in the class of the input
    //! word.
    //!
    //! \cong_common_throws_if_letters_out_of_bounds
    //!
    //! \cong_common_warn_undecidable{Todd-Coxeter}
    template <typename Word>
    [[nodiscard]] auto class_of(ToddCoxeter<Word>& tc, Word const& w) {
      return class_of(tc, std::begin(w), std::end(w));
    }

    //! \brief Returns a range object containing every word in the congruence
    //! class of a given word.
    //!
    //! Defined in \c todd-coxeter-helpers.hpp.
    //!
    //! This function returns a range object containing every word in
    //! belonging to the same class as the input word \p w in the congruence
    //! represented by the  \ref_todd_coxeter
    //! instance
    //! \p tc. Calls to this function trigger a full enumeration of \p tc.
    //!
    //! \tparam Word the type of the 2nd argument \p w.
    //!
    //! \param tc the  \ref_todd_coxeter
    //! instance.
    //! \param w the input word.
    //!
    //! \returns A range object containing the words in the class of the input
    //! word.
    //!
    //! \cong_common_warn_assume_letters_in_bounds
    //!
    //! \cong_common_warn_undecidable{Todd-Coxeter}
    template <typename Word>
    [[nodiscard]] auto class_of_no_checks(ToddCoxeter<Word>& tc,
                                          Word const&        w) {
      return class_of_no_checks(tc, std::begin(w), std::end(w));
    }

    //! \brief Returns a range object containing every word in the congruence
    //! class of a given word.
    //!
    //! Defined in \c todd-coxeter-helpers.hpp.
    //!
    //! This function returns a range object containing every word in
    //! belonging to the same class as the input word \p w in the congruence
    //! represented by the  \ref_todd_coxeter
    //! instance
    //! \p tc. Calls to this function trigger a full enumeration of \p tc.
    //!
    //! \tparam Int the type of the letters in the word \p w.
    //!
    //! \param tc the  \ref_todd_coxeter
    //! instance.
    //! \param w the input word.
    //!
    //! \returns A range object containing the words in the class of the input
    //! word.
    //!
    //! \cong_common_throws_if_letters_out_of_bounds
    //!
    //! \cong_common_warn_undecidable{Todd-Coxeter}
    template <typename Word, typename Int>
    [[nodiscard]] auto class_of(ToddCoxeter<Word>&                tc,
                                std::initializer_list<Int> const& w) {
      static_assert(std::is_integral_v<Int>);
      return class_of(tc, std::begin(w), std::end(w));
    }

    //! \brief Returns a range object containing every word in the congruence
    //! class of a given word.
    //!
    //! Defined in \c todd-coxeter-helpers.hpp.
    //!
    //! This function returns a range object containing every word in
    //! belonging to the same class as the input word \p w in the congruence
    //! represented by the  \ref_todd_coxeter
    //! instance
    //! \p tc. Calls to this function trigger a full enumeration of \p tc.
    //!
    //! \tparam Int the type of the letters in the word \p w.
    //!
    //! \param tc the \ref_todd_coxeter
    //! instance.
    //! \param w the input word.
    //!
    //! \returns A range object containing the words in the class of the input
    //! word.
    //!
    //! \cong_common_warn_assume_letters_in_bounds
    //!
    //! \cong_common_warn_undecidable{Todd-Coxeter}
    template <typename Word, typename Int>
    [[nodiscard]] auto class_of_no_checks(ToddCoxeter<Word>&                tc,
                                          std::initializer_list<Int> const& w) {
      static_assert(std::is_integral_v<Int>);
      return class_of_no_checks(tc, std::begin(w), std::end(w));
    }

    //! \brief Returns a range object containing every word in the congruence
    //! class of a given word.
    //!
    //! Defined in \c todd-coxeter-helpers.hpp.
    //!
    //! This function returns a range object containing every word in
    //! belonging to the same class as the input word \p w in the congruence
    //! represented by the  \ref_todd_coxeter
    //! instance
    //! \p tc. Calls to this function trigger a full enumeration of \p tc.
    //!
    //! \param tc the  \ref_todd_coxeter
    //! instance.
    //! \param w pointer to first letter.
    //!
    //! \returns A range object containing the words in the class of the input
    //! word.
    //!
    //! \cong_common_warn_assume_letters_in_bounds
    //!
    //! \cong_common_warn_undecidable{Todd-Coxeter}
    template <typename Word>
    [[nodiscard]] inline auto class_of_no_checks(ToddCoxeter<Word>& tc,
                                                 char const*        w) {
      LIBSEMIGROUPS_ASSERT(w != nullptr);
      return class_of_no_checks(tc, w, w + std::strlen(w));
    }

    //! \brief Returns a range object containing every word in the congruence
    //! class of a given word.
    //!
    //! Defined in \c todd-coxeter-helpers.hpp.
    //!
    //! This function returns a range object containing every word in
    //! belonging to the same class as the input word \p w in the congruence
    //! represented by the  \ref_todd_coxeter
    //! instance
    //! \p tc. Calls to this function trigger a full enumeration of \p tc.
    //!
    //! \param tc the  \ref_todd_coxeter
    //! instance.
    //! \param w pointer to first letter.
    //!
    //! \returns A range object containing the words in the class of the input
    //! word.
    //!
    //! \cong_common_throws_if_letters_out_of_bounds
    //!
    //! \cong_common_warn_undecidable{Todd-Coxeter}
    template <typename Word>
    [[nodiscard]] auto class_of(ToddCoxeter<Word>& tc, char const* w) {
      detail::throw_if_nullptr(w, "2nd");
      LIBSEMIGROUPS_ASSERT(w != nullptr);

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnonnull"
#endif
      return class_of(tc, w, w + std::strlen(w));
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
    }

    ////////////////////////////////////////////////////////////////////////
    // Possible future interface helpers - is_non_trivial
    ////////////////////////////////////////////////////////////////////////

    //! \brief Check if the congruence has more than one class.
    //!
    //! Defined in \c todd-coxeter-helpers.hpp.
    //!
    //! Returns tril::TRUE if it is possible to show that the congruence is
    //! non-trivial; tril::FALSE if the congruence is already known to be
    //! trivial; and tril::unknown if it is not possible to show that the
    //! congruence is non-trivial.
    //!
    //! This function attempts to find a non-trivial congruence containing
    //! the congruence represented by a \ref_todd_coxeter instance by repeating
    //! the following steps on a copy until the enumeration concludes:
    //! 1. running the enumeration for the specified amount of time
    //! 2. repeatedly choosing a random pair of nodes and identifying them,
    //!    until the number of nodes remaining in the quotient is smaller than
    //!    \p threshold times the initial number of nodes for this step.
    //!
    //! If at the end of this process, the  \ref_todd_coxeter instance is
    //! non-trivial, then the original  \ref_todd_coxeter is also non-trivial.
    //! Otherwise, the entire process is repeated again up to a total of
    //! \p tries times.
    //!
    //! \param tc the  \ref_todd_coxeter instance.
    //! \param tries the number of attempts to find non-trivial
    //! super-congruence.
    //! \param try_for the amount of time in millisecond to enumerate the
    //! congruence after choosing a random pair of representatives and
    //! identifying them.
    //! \param threshold the threshold (see description).
    //!
    //! \returns A value of type \ref tril
    [[nodiscard]] tril is_non_trivial(detail::ToddCoxeterImpl&  tc,
                                      size_t                    tries = 10,
                                      std::chrono::milliseconds try_for
                                      = std::chrono::milliseconds(100),
                                      float threshold = 0.99);

    //! \brief Perform a lookbehind.
    //!
    //! Defined in \c todd-coxeter-helpers.hpp.
    //!
    //! This function performs a "lookbehind" on the argument \p tc which is
    //! defined as follows. For every node \c n in the so-far computed
    //! \ref WordGraph (obtained from \ref ToddCoxeter::current_word_graph) we
    //! use the current word graph to rewrite the current short-lex least path
    //! from the initial node to \c n. If this rewritten word is not equal to
    //! the original word, and it also labels a path from the initial node in
    //! the current word graph to a node \c m, then \c m and \c n represent the
    //! same congruence class. Thus we may collapse \c m and \c n (i.e. quotient
    //! the word graph by the least congruence containing the pair \c m and
    //! \c n).
    //!
    //! The intended use case for this function is when you have a large word
    //! graph in a partially enumerated \ref_todd_coxeter instance, and you
    //! would like to minimise this word graph as far as possible.
    //!
    //! For example, if we take the following monoid presentation of B. H.
    //! Neumann for the trivial group:
    //!
    //! \code
    //! Presentation<std::string> p;
    //! p.alphabet("abcdef");
    //! p.contains_empty_word(true);
    //! presentation::add_inverse_rules(p, "defabc");
    //! presentation::add_rule(p, "bbdeaecbffdbaeeccefbccefb", "");
    //! presentation::add_rule(p, "ccefbfacddecbffaafdcaafdc", "");
    //! presentation::add_rule(p, "aafdcdbaeefacddbbdeabbdea", "");
    //! ToddCoxeter tc(congruence_kind.twosided, p);
    //! \endcode
    //!
    //! Then running \p tc will simply grow the underlying word graph until
    //! your computer runs out of memory. The authors of ``libsemigroups`` were
    //! not able to find any combination of the many settings for
    //! \ref_todd_coxeter where running \p tc returned an answer. We also tried
    //! with GAP and ACE but neither of these seemed able to return an answer
    //! either. But doing the following:
    //!
    //! \code
    //! tc.lookahead_extent(options::lookahead_extent::full)
    //!     .lookahead_style(options::lookahead_style::felsch);
    //!
    //! tc.run_for(std::chrono::seconds(1));
    //! tc.perform_lookahead(true);
    //!
    //! todd_coxeter::perform_lookbehind(tc);
    //! tc.run_for(std::chrono::seconds(1));
    //! todd_coxeter::perform_lookbehind(tc);
    //! tc.perform_lookahead(true);
    //! tc.number_of_classes(); // returns 1
    //! \endcode
    //!
    //! returns the correct answer in about 22 seconds (on a 2024 Macbook Pro M4
    //! Pro).
    //!
    //! \param tc the  \ref_todd_coxeter instance.
    //!
    //! \throws LibsemigroupsException if \p tc is a one-sided congruence and
    //! has any generating pairs (because in this case \ref perform_lookbehind
    //! does nothing but still might take some time to run).
    void perform_lookbehind(detail::ToddCoxeterImpl& tc);

    ////////////////////////////////////////////////////////////////////////
    // Possible future interface helpers - redundant_rule
    ////////////////////////////////////////////////////////////////////////

    //! \brief Return an iterator pointing at the left hand side of a redundant
    //! rule.
    //!
    //! Defined in \c todd-coxeter-helpers.hpp.
    //!
    //! Starting with the last rule in the presentation, this function
    //! attempts to run the Todd-Coxeter algorithm on the rules of the
    //! presentation except for a given omitted rule. For every such omitted
    //! rule, Todd-Coxeter is run for the length of time indicated by the
    //! second parameter \p t, and then it is checked if the omitted rule can
    //! be shown to be redundant.
    //!
    //! If the omitted rule can be shown to be redundant in this way, then an
    //! iterator pointing to its left hand side is returned.
    //!
    //! If no rule can be shown to be redundant in this way, then an iterator
    //! pointing to \c p.rules.cend() is returned.
    //!
    //! \tparam Word type of words in the Presentation.
    //! \tparam Time type of the 2nd parameter (time to try running
    //! Todd-Coxeter).
    //!
    //! \param p the presentation.
    //! \param t time to run Todd-Coxeter for every omitted rule.
    //!
    //! \returns An iterator pointing at the left-hand side of a redundant rule
    //! of \c p.rules.cend().
    //!
    //! \warning The progress of the Todd-Coxeter algorithm may differ between
    //! different calls to this function even if the parameters are identical.
    //! As such this function is non-deterministic, and may produce different
    //! results with the same input.
    template <typename Word, typename Time>
    [[nodiscard]] typename std::vector<Word>::const_iterator
    redundant_rule(Presentation<Word> const& p, Time t);

    //! @}

  }  // namespace todd_coxeter

  // This namespace contains implementations of the interface helpers (i.e.
  // specific versions for ToddCoxeterImpl of the helper functions from
  // cong-common.hpp).
  namespace congruence_common {
    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - normal_forms
    ////////////////////////////////////////////////////////////////////////

    //! \brief Returns a range object containing the normal forms.
    //!
    //! Defined in \c todd-coxeter-helpers.hpp.
    //!
    //! This function returns a range object containing normal forms of the
    //! classes of the congruence represented by an instance of
    //! \ref_todd_coxeter. The order of the classes, and the normal form that is
    //! returned, are controlled by ToddCoxeter::standardize(Order). This
    //! function triggers a full enumeration of \p tc.
    //!
    //! \tparam Word the type of the words contained in the output range
    //! (default: \ref libsemigroups::word_type "word_type").
    //!
    //! \param tc the \ref_todd_coxeter
    //! instance.
    //!
    //! \returns A range object.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \cong_common_warn_undecidable{Todd-Coxeter}.
    template <typename Word>
    [[nodiscard]] auto normal_forms(ToddCoxeter<Word>& tc);

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - partition
    ////////////////////////////////////////////////////////////////////////

    //! \brief Partition a range of words.
    //!
    //! Defined in \c todd-coxeter-helpers.hpp.
    //!
    //! This function returns the partition of the words in the range \p r
    //! induced by the \ref_todd_coxeter instance \p tc. This function triggers
    //! a full enumeration of \p tc.
    //!
    //! \tparam Word the type of the words in \p tc, in the range \p r, and in
    //! the output.
    //! \tparam Range the type of the input range of words.
    //!
    //! \param tc the \ref_todd_coxeter instance.
    //! \param r the input range of words.
    //!
    //! \returns The partition of the input range.
    //!
    //! \throws LibsemigroupsException if the number of classes in \p tc is
    //! infinite. In this case, the enumeration of \p tc will not terminate
    //! successfully.
    //!
    //! \throws LibsemigroupsException if the input range of words is infinite.
    //!
    //! \cong_common_warn_undecidable{Todd-Coxeter}.
    // The following function is also declared in cong-common-helpers.hpp:
    template <typename Word, typename Range, typename>
    [[nodiscard]] std::vector<std::vector<Word>>
    partition(ToddCoxeter<Word>& tc, Range r);

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - non_trivial_classes
    ////////////////////////////////////////////////////////////////////////

    //! \brief Find the non-trivial classes in the partition of the normal
    //! forms of one \ref_todd_coxeter instance in another.
    //!
    //! Defined in \c todd-coxeter-helpers.hpp.
    //!
    //! This function returns the classes with size at least \f$2\f$ in the
    //! partition of the normal forms of \p tc2 according to the
    //! \ref_todd_coxeter instance \p tc1. This function triggers a full
    //! enumeration of \p tc1 and \p tc2.
    //!
    //! \tparam Word the type of the words in the output (defaults
    //! \ref libsemigroups::word_type "word_type").
    //!
    //! \param tc1 the \ref_todd_coxeter instance to use for partitioning.
    //! \param tc2 the \ref_todd_coxeter instance to be partitioned.
    //!
    //! \returns The partition of the input range.
    //!
    //! \throws LibsemigroupsException if the number of classes in \p tc1 or
    //! \p tc2 is infinite.
    //!
    //! \cong_common_warn_undecidable{Todd-Coxeter}.
    template <typename Word>
    [[nodiscard]] std::vector<std::vector<Word>>
    non_trivial_classes(ToddCoxeter<Word>& tc1, ToddCoxeter<Word>& tc2);
  }  // namespace congruence_common

  // The todd_coxeter namespace contains all helpers specific to
  // ToddCoxeter<Word> and aliases for the common functionality in the
  // congruence_common namespace.
  namespace todd_coxeter {
    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter<Word> add_generating_pairs helpers
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
    // Interface helpers - reduce[_no_run][_no_checks]
    ////////////////////////////////////////////////////////////////////////

    using congruence_common::reduce;
    using congruence_common::reduce_no_checks;
    using congruence_common::reduce_no_run;
    using congruence_common::reduce_no_run_no_checks;

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - partitioning and normal forms
    ////////////////////////////////////////////////////////////////////////

    using congruence_common::non_trivial_classes;
    using congruence_common::normal_forms;
    using congruence_common::partition;

  }  // namespace todd_coxeter
}  // namespace libsemigroups

#include "todd-coxeter-helpers.tpp"
#endif  // LIBSEMIGROUPS_TODD_COXETER_HELPERS_HPP_
