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

// This file contains helper functions for the class CongruenceInterface and
// its derived classes. This is a separate file so that we can declare all the
// derived classes of CongruenceInterface prior to declaring the functions in
// this file.

#ifndef LIBSEMIGROUPS_CONG_INTF_HELPERS_HPP_
#define LIBSEMIGROUPS_CONG_INTF_HELPERS_HPP_

#include <algorithm>         // for remove_if
#include <cstring>           // for strlen, size_t
#include <initializer_list>  // for initializer_list, begin, end
#include <iterator>          // for begin, end, back_inserter
#include <string>            // for string
#include <string_view>       // for basic_string_view, string_view
#include <type_traits>       // for is_base_of_v, decay_t, is_same_v
#include <unordered_map>     // for unordered_map
#include <vector>            // for vector

#include "exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/todd-coxeter-base.hpp"
#include "ranges.hpp"  // for is_input_or_sink_v, iterator_...

namespace libsemigroups {

  // Forward decls
  class CongruenceInterface;

  struct CongruenceBase;

  template <typename Word>
  class Congruence;

  class ToddCoxeterBase;

  template <typename Word>
  class ToddCoxeter;

  enum class tril;

  namespace congruence_interface {

    //! \defgroup cong_intf_helpers_group Generic congruence helpers
    //! \ingroup cong_all_group
    //!
    //! \brief Helper functions for subclasses of \ref CongruenceInterface.
    //!
    //! This page contains documentation for helper functions for the classes
    //! Congruence, Kambites, KnuthBendixBase, and \ref todd_coxeter_class_group
    //! "ToddCoxeterBase". The functions documented on this page belong to all
    //! of the namespaces \ref cong_intf_helpers_group "congruence_interface",
    //! `congruence`, \ref kambites, \ref knuth_bendix, and \ref
    //! todd_coxeter_helpers_group "todd_coxeter".

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - add_generating_pair
    ////////////////////////////////////////////////////////////////////////

    //! \defgroup cong_intf_helpers_add_pair_group Add generating pairs
    //! \ingroup cong_intf_helpers_group
    //!
    //! \brief Add a generating pair using objects instead of iterators.
    //!
    //! This page contains the documentation of the functions
    //! \ref add_generating_pair and \ref add_generating_pair_no_checks
    //! which can be invoked with a variety of different argument types.
    //!
    //! @{

    //! \brief Helper for adding a generating pair of words.
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! This function can be used to add a generating pair to the subclass \p
    //! ci of \ref CongruenceInterface using the objects themselves rather
    //! than using iterators.
    //!
    //! \tparam Subclass the type of the first parameter.
    //! \tparam Word the type of the second and third parameters.
    //!
    //! \param ci the subclass of \ref CongruenceInterface.
    //! \param u the left hand side of the pair to add.
    //! \param v the right hand side of the pair to add.
    //!
    //! \return A reference to \p ci.
    //!
    //! \cong_intf_warn_assume_letters_in_bounds
    template <typename Subclass, typename Word>
    Subclass& add_generating_pair_no_checks(Subclass&   ci,
                                            Word const& u,
                                            Word const& v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return ci.add_generating_pair_no_checks(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    //! \brief Helper for adding a generating pair of words
    //! (std::initializer_list).
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! See \ref
    //! add_generating_pair_no_checks(Subclass&, Word const&, Word const&)
    //! for details.
    template <typename Subclass, typename Int>
    Subclass&
    add_generating_pair_no_checks(Subclass&                         ci,
                                  std::initializer_list<Int> const& u,
                                  std::initializer_list<Int> const& v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return add_generating_pair_no_checks<Subclass, std::vector<Int>>(
          ci, u, v);
    }

    //! \brief Helper for adding a generating pair of words
    //! (string literals).
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! See \ref
    //! add_generating_pair_no_checks(Subclass&, Word const&, Word const&)
    //! for details.
    template <typename Subclass>
    Subclass& add_generating_pair_no_checks(Subclass&   ci,
                                            char const* u,
                                            char const* v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return ci.add_generating_pair_no_checks(
          u, u + std::strlen(u), v, v + std::strlen(v));
    }

    // This version of the function catches the cases when u & v are not of
    // the same type but both convertible to string_view
    //! \brief Helper for adding a generating pair of words
    //! (std::string_view).
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! See \ref
    //! add_generating_pair_no_checks(Subclass&, Word const&, Word const&)
    //! for details.
    template <typename Subclass>
    Subclass& add_generating_pair_no_checks(Subclass&        ci,
                                            std::string_view u,
                                            std::string_view v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return ci.add_generating_pair_no_checks(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    //! \brief Helper for adding a generating pair of words.
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! This function can be used to add a generating pair to the subclass \p
    //! ci of \ref CongruenceInterface using the objects themselves rather
    //! than using iterators.
    //!
    //! \tparam Subclass the type of the first parameter.
    //! \tparam Word the type of the second and third parameters.
    //!
    //! \param ci the subclass of \ref CongruenceInterface.
    //! \param u the left hand side of the pair to add.
    //! \param v the right hand side of the pair to add.
    //!
    //! \return A reference to \p ci.
    //!
    //! \cong_intf_throws_if_letters_out_of_bounds
    template <typename Subclass, typename Word>
    Subclass& add_generating_pair(Subclass& ci, Word const& u, Word const& v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return ci.add_generating_pair(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    //! \brief Helper for adding a generating pair of words
    //! (std::initializer_list).
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! See \ref
    //! add_generating_pair(Subclass&, Word const&, Word const&)
    //! for details.
    template <typename Subclass, typename Int>
    Subclass& add_generating_pair(Subclass&                         ci,
                                  std::initializer_list<Int> const& u,
                                  std::initializer_list<Int> const& v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return add_generating_pair<Subclass, std::vector<Int>>(ci, u, v);
    }

    //! \brief Helper for adding a generating pair of words
    //! (string literals).
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! See \ref
    //! add_generating_pair(Subclass&, Word const&, Word const&)
    //! for details.
    template <typename Subclass>
    Subclass& add_generating_pair(Subclass& ci, char const* u, char const* v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return ci.add_generating_pair(
          u, u + std::strlen(u), v, v + std::strlen(v));
    }

    // This version of the function catches the cases when u & v are not of
    // the same type but both convertible to string_view
    //! \brief Helper for adding a generating pair of words
    //! (std::string_view).
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! See \ref
    //! add_generating_pair(Subclass&, Word const&, Word const&)
    //! for details.
    template <typename Subclass>
    Subclass& add_generating_pair(Subclass&        ci,
                                  std::string_view u,
                                  std::string_view v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return ci.add_generating_pair_no_checks(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    //! @}

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - currently_contains_no_checks
    ////////////////////////////////////////////////////////////////////////

    //! \defgroup cong_intf_helpers_contains_group Containment
    //! \ingroup cong_intf_helpers_group
    //!
    //! \brief Check containment of a pair of words in a congruence.
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! This page contains the documentation of the functions
    //! \ref currently_contains_no_checks; \ref currently_contains;
    //! \ref contains_no_checks; and \ref contains which can be invoked with a
    //! variety of different argument types.
    //!
    //! Functions with the prefix `currently_` do not perform any
    //! enumeration of the \ref CongruenceInterface derived class instances;
    //! and those with the suffix `_no_checks` do not check that the input
    //! words are valid.
    //!
    //! @{

    //! \brief Check containment of a pair of words.
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! This function checks whether or not the words \p u and \p v are
    //! already known to be contained in the congruence represented by a \ref
    //! CongruenceInterface derived class instance \p ci. This function
    //! performs no enumeration of \p ci, so it is possible for the words to
    //! be contained in the congruence, but that this is not currently known.
    //!
    //! \tparam Subclass the type of the first parameter.
    //! \tparam Word the type of the second and third parameters.
    //!
    //! \param ci the subclass of \ref CongruenceInterface.
    //! \param u the left hand side of the pair to add.
    //! \param v the right hand side of the pair to add.
    //!
    //! \returns
    //! * tril::TRUE if the words are known to belong to the congruence;
    //! * tril::FALSE if the words are known to not belong to the congruence;
    //! * tril::unknown otherwise.
    //!
    //! \cong_intf_warn_assume_letters_in_bounds
    template <typename Subclass, typename Word>
    [[nodiscard]] tril currently_contains_no_checks(Subclass const& ci,
                                                    Word const&     u,
                                                    Word const&     v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return ci.currently_contains_no_checks(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    // This version of the function catches the cases when u & v are not of
    // the same type but both convertible to string_view
    //! \brief Helper for checking containment of a pair of words
    //! (std::string_view).
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! See \ref
    //! currently_contains_no_checks(Subclass const&, Word const&, Word
    //! const&) for details.
    template <typename Subclass>
    [[nodiscard]] tril currently_contains_no_checks(Subclass const&  ci,
                                                    std::string_view u,
                                                    std::string_view v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return ci.currently_contains_no_checks(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    //! \brief Helper for checking containment of a pair of words
    //! (string literal).
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! See \ref
    //! currently_contains_no_checks(Subclass const&, Word const&, Word
    //! const&) for details.
    template <typename Subclass>
    [[nodiscard]] tril currently_contains_no_checks(Subclass const& ci,
                                                    char const*     u,
                                                    char const*     v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return ci.currently_contains_no_checks(
          u, u + std::strlen(u), v, v + std::strlen(v));
    }

    //! \brief Helper for checking containment of a pair of words
    //! (std::initializer_list).
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! See \ref
    //! currently_contains_no_checks(Subclass const&, Word const&, Word
    //! const&) for details.
    template <typename Subclass,
              typename Int = typename Subclass::native_letter_type>
    [[nodiscard]] tril
    currently_contains_no_checks(Subclass const&                   ci,
                                 std::initializer_list<Int> const& u,
                                 std::initializer_list<Int> const& v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return currently_contains_no_checks<Subclass, std::initializer_list<Int>>(
          ci, u, v);
    }

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - currently_contains
    ////////////////////////////////////////////////////////////////////////

    //! \brief Check containment of a pair of words.
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! This function checks whether or not the words \p u and \p v are
    //! already known to be contained in the congruence represented by a \ref
    //! CongruenceInterface derived class instance \p ci. This function
    //! performs no enumeration of \p ci, so it is possible for the words to
    //! be contained in the congruence, but that this is not currently known.
    //!
    //! \tparam Subclass the type of the first parameter.
    //! \tparam Word the type of the second and third parameters.
    //!
    //! \param ci the subclass of \ref CongruenceInterface.
    //! \param u the left hand side of the pair to add.
    //! \param v the right hand side of the pair to add.
    //!
    //! \returns
    //! * tril::TRUE if the words are known to belong to the congruence;
    //! * tril::FALSE if the words are known to not belong to the congruence;
    //! * tril::unknown otherwise.
    //!
    //! \cong_intf_throws_if_letters_out_of_bounds
    template <typename Subclass, typename Word>
    [[nodiscard]] tril currently_contains(Subclass const& ci,
                                          Word const&     u,
                                          Word const&     v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return ci.currently_contains(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    // This version of the function catches the cases when u & v are not of
    // the same type but both convertible to string_view
    //! \brief Helper for checking containment of a pair of words
    //! (std::string_view).
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! See \ref
    //! currently_contains(Subclass const&, Word const&, Word const&)
    //! for details.
    template <typename Subclass>
    [[nodiscard]] tril currently_contains(Subclass const&  ci,
                                          std::string_view u,
                                          std::string_view v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return ci.currently_contains(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    //! \brief Helper for checking containment of a pair of words
    //! (string literal).
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! See \ref
    //! currently_contains(Subclass const&, Word const&, Word const&)
    //! for details.
    template <typename Subclass>
    [[nodiscard]] tril currently_contains(Subclass const& ci,
                                          char const*     u,
                                          char const*     v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return ci.currently_contains_no_checks(
          u, u + std::strlen(u), v, v + std::strlen(v));
    }

    //! \brief Helper for checking containment of a pair of words
    //! (std::initializer_list).
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! See \ref
    //! currently_contains(Subclass const&, Word const&, Word const&)
    //! for details.
    template <typename Subclass,
              typename Int = typename Subclass::native_letter_type>
    [[nodiscard]] tril currently_contains(Subclass const&                   ci,
                                          std::initializer_list<Int> const& u,
                                          std::initializer_list<Int> const& v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return currently_contains<Subclass, std::initializer_list<Int>>(ci, u, v);
    }

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - contains_no_checks
    ////////////////////////////////////////////////////////////////////////

    //! \brief Check containment of a pair of words.
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! This function checks whether or not the words \p u and \p v are
    //! contained in the congruence represented by the instance \p ci of a
    //! derived class of \ref CongruenceInterface. This function triggers a
    //! full enumeration of \p ci, which may never terminate.
    //!
    //! \tparam Subclass the type of the first parameter.
    //! \tparam Word the type of the second and third parameters.
    //!
    //! \param ci the subclass of \ref CongruenceInterface.
    //! \param u the left hand side of the pair to add.
    //! \param v the right hand side of the pair to add.
    //!
    //! \returns Whether or not the pair belongs to the congruence.
    //!
    //! \cong_intf_warn_assume_letters_in_bounds
    template <typename Subclass, typename Word>
    [[nodiscard]] bool contains_no_checks(Subclass&   ci,
                                          Word const& u,
                                          Word const& v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return ci.contains_no_checks(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    // This version of the function catches the cases when u & v are not of
    // the same type but both convertible to string_view
    //! \brief Helper for checking containment of a pair of words
    //! (std::string_view).
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! See \ref
    //! contains_no_checks(Subclass&, Word const&, Word const&)
    //! for details.
    template <typename Subclass>
    [[nodiscard]] bool contains_no_checks(Subclass&        ci,
                                          std::string_view u,
                                          std::string_view v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return ci.contains_no_checks(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    //! \brief Helper for checking containment of a pair of words
    //! (string literal).
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! See \ref
    //! contains_no_checks(Subclass&, Word const&, Word const&)
    //! for details.
    template <typename Subclass>
    [[nodiscard]] bool contains_no_checks(Subclass&   ci,
                                          char const* u,
                                          char const* v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return ci.contains_no_checks(
          u, u + std::strlen(u), v, v + std::strlen(v));
    }

    //! \brief Helper for checking containment of a pair of words
    //! (std::initializer_list).
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! See \ref
    //! contains_no_checks(Subclass&, Word const&, Word const&)
    //! for details.
    template <typename Subclass,
              typename Int = typename Subclass::native_letter_type>
    [[nodiscard]] bool contains_no_checks(Subclass&                         ci,
                                          std::initializer_list<Int> const& u,
                                          std::initializer_list<Int> const& v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return contains_no_checks<Subclass, std::initializer_list<Int>>(ci, u, v);
    }

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - contains
    ////////////////////////////////////////////////////////////////////////

    //! \brief Check containment of a pair of words.
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! This function checks whether or not the words \p u and \p v are
    //! contained in the congruence represented by the instance \p ci of a
    //! derived class of \ref CongruenceInterface. This function triggers a
    //! full enumeration of \p ci, which may never terminate.
    //!
    //! \tparam Subclass the type of the first parameter.
    //! \tparam Word the type of the second and third parameters.
    //!
    //! \param ci the subclass of \ref CongruenceInterface.
    //! \param u the left hand side of the pair to add.
    //! \param v the right hand side of the pair to add.
    //!
    //! \returns Whether or not the pair belongs to the congruence.
    //!
    //! \cong_intf_throws_if_letters_out_of_bounds
    template <typename Subclass, typename Word>
    [[nodiscard]] bool contains(Subclass& ci, Word const& u, Word const& v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return ci.contains(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    // This version of the function catches the cases when u & v are not of
    // the same type but both convertible to string_view
    //! \brief Helper for checking containment of a pair of words
    //! (std::string_view).
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! See \ref
    //! contains(Subclass&, Word const&, Word const&)
    //! for details.
    template <typename Subclass>
    [[nodiscard]] bool contains(Subclass&        ci,
                                std::string_view u,
                                std::string_view v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return ci.contains(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    //! \brief Helper for checking containment of a pair of words
    //! (string literal).
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! See \ref
    //! contains(Subclass&, Word const&, Word const&)
    //! for details.
    template <typename Subclass>
    [[nodiscard]] bool contains(Subclass& ci, char const* u, char const* v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return ci.contains(u, u + std::strlen(u), v, v + std::strlen(v));
    }

    //! \brief Helper for checking containment of a pair of words
    //! (std::initializer_list).
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! See \ref
    //! contains_no_checks(Subclass&, Word const&, Word const&)
    //! for details.
    template <typename Subclass,
              typename Int = typename Subclass::native_letter_type>
    [[nodiscard]] bool contains(Subclass&                         ci,
                                std::initializer_list<Int> const& u,
                                std::initializer_list<Int> const& v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return contains<Subclass, std::initializer_list<Int>>(ci, u, v);
    }
    //! @}

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - reduce_no_run_no_checks
    ////////////////////////////////////////////////////////////////////////

    //! \defgroup cong_intf_helpers_reduce_group Reduce a word
    //! \ingroup cong_intf_helpers_group
    //!
    //! \brief Check containment of a pair of words in a congruence.
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! This page contains the documentation of the functions \ref
    //! reduce_no_run_no_checks; \ref reduce_no_run; \ref reduce_no_checks;
    //! and
    //! \ref reduce which can be invoked with a variety of different argument
    //! types.
    //!
    //! Functions with the suffix `_no_run` do not perform any enumeration
    //! of the \ref CongruenceInterface derived class instances; and those
    //! with the suffix `_no_checks` do not check that the input words are
    //! valid.
    //!
    //! @{

    //! \brief Reduce a word with no enumeration or checks.
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! This function returns a reduced word equivalent to the input word \p w
    //! in the congruence represented by an instance of a derived class of
    //! \ref CongruenceInterface. This function triggers no enumeration. The
    //! word output by this function is equivalent to the input word in the
    //! congruence. If \p ci is `finished`, then the output word is a normal
    //! form for the input word. If the \p ci is not `finished`, then it
    //! might be that equivalent input words produce different output words.
    //!
    //! \tparam Subclass the type of the first parameter.
    //! \tparam InputWord the type of the second parameter.
    //! \tparam OutputWord the type of word to be returned (defaults to \p
    //! InputWord).
    //!
    //! \param ci the subclass of \ref CongruenceInterface.
    //! \param w the word to reduce.
    //!
    //! \returns An irreducible word equivalent to \p w.
    //!
    //! \cong_intf_warn_assume_letters_in_bounds
    template <typename Subclass,
              typename InputWord,
              typename OutputWord = InputWord>
    [[nodiscard]] OutputWord reduce_no_run_no_checks(Subclass const&  ci,
                                                     InputWord const& w);

    // No string_view version is required because there is only a single
    // "word" parameter, and so the first template will catch every case
    // except initializer_list and char const*

    //! \brief Reduce a word (std::initializer_list).
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! See \ref
    //! reduce_no_run_no_checks(Subclass const&, InputWord const&)
    //! for details.
    template <typename Subclass,
              typename Int        = size_t,
              typename OutputWord = std::vector<Int>>
    [[nodiscard]] OutputWord
    reduce_no_run_no_checks(Subclass const&                   ci,
                            std::initializer_list<Int> const& w);

    //! \brief Reduce a word (string literal).
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! See \ref
    //! reduce_no_run_no_checks(Subclass const&, InputWord const&)
    //! for details.
    template <typename Subclass, typename OutputWord = std::string>
    [[nodiscard]] auto reduce_no_run_no_checks(Subclass const& ci,
                                               char const*     w);

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - reduce_no_run
    ////////////////////////////////////////////////////////////////////////

    //! \brief Reduce a word with no enumeration.
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! This function returns a reduced word equivalent to the input word \p w
    //! in the congruence represented by an instance of a derived class of
    //! \ref CongruenceInterface. This function triggers no enumeration. The
    //! word output by this function is equivalent to the input word in the
    //! congruence. If \p ci is `finished`, then the output word is a normal
    //! form for the input word. If the \p ci is not `finished`, then it
    //! might be that equivalent input words produce different output words.
    //!
    //! \tparam Subclass the type of the first parameter.
    //! \tparam InputWord the type of the second parameter.
    //! \tparam OutputWord the type of word to be returned (defaults to \p
    //! InputWord).
    //!
    //! \param ci the subclass of \ref CongruenceInterface.
    //! \param w the word to reduce.
    //!
    //! \returns An irreducible word equivalent to \p w.
    //!
    //! \cong_intf_throws_if_letters_out_of_bounds
    template <typename Subclass,
              typename InputWord,
              typename OutputWord = InputWord>
    [[nodiscard]] OutputWord reduce_no_run(Subclass const&  ci,
                                           InputWord const& w);

    // No string_view version is required because there is only a single
    // "word" parameter, and so the first template will catch every case
    // except initializer_list and char const*

    //! \brief Reduce a word (std::initializer_list).
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! See \ref
    //! reduce_no_run(Subclass const&, InputWord const&)
    //! for details.
    template <typename Subclass,
              typename Int        = size_t,
              typename OutputWord = std::vector<Int>>
    [[nodiscard]] OutputWord reduce_no_run(Subclass const&                   ci,
                                           std::initializer_list<Int> const& w);

    //! \brief Reduce a word (string literal).
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! See \ref
    //! reduce_no_run(Subclass const&, InputWord const&)
    //! for details.
    template <typename Subclass, typename OutputWord = std::string>
    [[nodiscard]] OutputWord reduce_no_run(Subclass const& ci, char const* w);

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - reduce_no_checks
    ////////////////////////////////////////////////////////////////////////

    //! \brief Reduce a word with no checks.
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! This function returns a reduced word equivalent to the input word \p w
    //! in the congruence represented by an instance of a derived class of
    //! \ref CongruenceInterface. This function triggers a full enumeration.
    //! The word output by this function is equivalent to the input word in
    //! the congruence. If \p ci is `finished`, then the output word is a
    //! normal form for the input word. If the \p ci is not `finished`, then
    //! it might be that equivalent input words produce different output
    //! words.
    //!
    //! \tparam Subclass the type of the first parameter.
    //! \tparam InputWord the type of the second parameter.
    //! \tparam OutputWord the type of word to be returned (defaults to \p
    //! InputWord).
    //!
    //! \param ci the subclass of \ref CongruenceInterface.
    //! \param w the word to reduce.
    //!
    //! \returns An irreducible word equivalent to \p w.
    //!
    //! \cong_intf_warn_assume_letters_in_bounds
    template <typename Subclass,
              typename InputWord,
              typename OutputWord = InputWord>
    [[nodiscard]] OutputWord reduce_no_checks(Subclass& ci, InputWord const& w);

    // No string_view version is required because there is only a single
    // "word" parameter, and so the first template will catch every case
    // except initializer_list and char const*

    //! \brief Reduce a word (std::initializer_list).
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! See \ref
    //! reduce_no_checks(Subclass&, InputWord const&)
    //! for details.
    template <typename Subclass,
              typename Int        = size_t,
              typename OutputWord = std::vector<Int>>
    [[nodiscard]] OutputWord
    reduce_no_checks(Subclass& ci, std::initializer_list<Int> const& w);

    //! \brief Reduce a word (string literal).
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! See \ref
    //! reduce_no_checks(Subclass&, InputWord const&)
    //! for details.
    template <typename Subclass, typename OutputWord = std::string>
    [[nodiscard]] OutputWord reduce_no_checks(Subclass& ci, char const* w);

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - reduce
    ////////////////////////////////////////////////////////////////////////

    //! \brief Reduce a word.
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! This function returns a reduced word equivalent to the input word \p w
    //! in the congruence represented by an instance of a derived class of
    //! \ref CongruenceInterface. This function triggers a full enumeration.
    //! The word output by this function is equivalent to the input word in
    //! the congruence. If \p ci is `finished`, then the output word is a
    //! normal form for the input word. If the \p ci is not `finished`, then
    //! it might be that equivalent input words produce different output
    //! words.
    //!
    //! \tparam Subclass the type of the first parameter.
    //! \tparam InputWord the type of the second parameter.
    //! \tparam OutputWord the type of word to be returned (defaults to \p
    //! InputWord).
    //!
    //! \param ci the subclass of \ref CongruenceInterface.
    //! \param w the word to reduce.
    //!
    //! \returns An irreducible word equivalent to \p w.
    //!
    //! \cong_intf_throws_if_letters_out_of_bounds
    template <typename Subclass,
              typename InputWord,
              typename OutputWord = InputWord>
    [[nodiscard]] OutputWord reduce(Subclass& ci, InputWord const& w);

    // No string_view version is required because there is only a single
    // "word" parameter, and so the first template will catch every case
    // except initializer_list and char const*

    //! \brief Reduce a word (std::initializer_list).
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! See \ref reduce(Subclass&, InputWord const&) for details.
    template <typename Subclass,
              typename Int        = size_t,
              typename OutputWord = std::vector<Int>>
    [[nodiscard]] OutputWord reduce(Subclass&                         ci,
                                    std::initializer_list<Int> const& w);

    //! \brief Reduce a word (string literal).
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! See \ref reduce(Subclass&, InputWord const&) for details.
    template <typename Subclass, typename OutputWord = std::string>
    [[nodiscard]] auto reduce(Subclass& ci, char const* w);

    //! @}

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - normal_forms
    ////////////////////////////////////////////////////////////////////////

    // There's nothing in common to implement in this file.

    //! \defgroup cong_intf_helpers_partition_group Partitioning
    //! \ingroup cong_intf_helpers_group
    //!
    //! \brief Partition a range of words by a congruence.
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! This page contains the documentation of the functions \ref partition
    //! and \ref non_trivial_classes for partitioning a range of words by a
    //! congruence.
    //!
    //! @{

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - partition
    ////////////////////////////////////////////////////////////////////////

    // Forward decls defined in todd-coxeter-helpers.tpp and cong-helpers.tpp
    // TODO(0) static assert that Word is Range::output_type
    template <typename Word,
              typename Range,
              typename = std::enable_if_t<rx::is_input_or_sink_v<Range>>>
    [[nodiscard]] std::vector<std::vector<Word>>
    partition(ToddCoxeter<Word>& ci, Range r);

    // TODO(0) static assert that Word is Range::output_type
    template <typename Word,
              typename Range,
              typename = std::enable_if_t<rx::is_input_or_sink_v<Range>>>
    [[nodiscard]] std::vector<std::vector<Word>> partition(Congruence<Word>& ci,
                                                           Range             r);

    //! \brief Partition a range of words.
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! This function returns the partition of the words in the range \p r
    //! induced by the instance \p ci of a derived class of \ref
    //! CongruenceInterface. This function triggers a full enumeration of \p
    //! ci.
    //!
    //! \tparam Subclass the type of the first parameter.
    //! \tparam Range the type of the input range of words.
    //! \tparam OutputWord the type of the words in the output (defaults to
    //! the type of the words in the input range).
    //!
    //! \param ci the derived class of \ref CongruenceInterface.
    //! \param r the input range of words.
    //!
    //! \returns The partition of the input range.
    //!
    //! \throws LibsemigroupsException if the input range of words is
    //! infinite.
    template <typename Subclass,
              typename Range,
              typename OutputWord = std::decay_t<typename Range::output_type>,
              typename = std::enable_if_t<rx::is_input_or_sink_v<Range>>>
    [[nodiscard]] std::vector<std::vector<OutputWord>> partition(Subclass& ci,
                                                                 Range     r);

    //! \brief Partition a range of words (via iterators)
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! See \ref partition(Subclass&, Range) for details.
    template <
        typename Subclass,
        typename Iterator1,
        typename Iterator2,
        typename OutputWord = std::decay_t<
            typename rx::iterator_range<Iterator1, Iterator2>::output_type>>
    std::vector<std::vector<OutputWord>> partition(Subclass& ci,
                                                   Iterator1 first,
                                                   Iterator2 last) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return partition<Subclass,
                       rx::iterator_range<Iterator1, Iterator2>,
                       OutputWord>(ci, rx::iterator_range(first, last));
    }

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - non_trivial_classes
    ////////////////////////////////////////////////////////////////////////

    //! \brief Find the non-trivial classes in the partition of a range of
    //! words.
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! This function returns the classes with size at least \f$2\f$ in the
    //! partition of the words in the range \p r according to \p ci.
    //! This function triggers a full enumeration of \p ci.
    //!
    //! \tparam Subclass the type of the first parameter.
    //! \tparam Range the type of the input range of words.
    //! \tparam OutputWord the type of the words in the output (defaults to
    //! the type of the words in the input range).
    //!
    //! \param ci a derived class of CongruenceInterface.
    //! \param r the input range of words.
    //!
    //! \returns The partition of the input range.
    //!
    //! \throws LibsemigroupsException if the input range of words is
    //! infinite.
    template <typename Subclass,
              typename Range,
              typename OutputWord = std::decay_t<typename Range::output_type>,
              typename = std::enable_if_t<rx::is_input_or_sink_v<Range>>>
    [[nodiscard]] std::vector<std::vector<OutputWord>>
    non_trivial_classes(Subclass& ci, Range r);

    //! \brief Partition a range of words into non-trivial classes (via
    //! iterators).
    //!
    //! Defined in `cong-intf.hpp`.
    //!
    //! See \ref non_trivial_classes(Subclass&, Range) for details.
    template <
        typename Subclass,
        typename Iterator1,
        typename Iterator2,
        typename OutputWord = std::decay_t<
            typename rx::iterator_range<Iterator1, Iterator2>::output_type>>
    std::vector<std::vector<OutputWord>> non_trivial_classes(Subclass& ci,
                                                             Iterator1 first,
                                                             Iterator2 last) {
      return non_trivial_classes<Subclass,
                                 rx::iterator_range<Iterator1, Iterator2>,
                                 OutputWord>(ci,
                                             rx::iterator_range(first, last));
    }

    //! @}
  }  // namespace congruence_interface
}  // namespace libsemigroups

#include "cong-intf-helpers.tpp"

#endif  // LIBSEMIGROUPS_CONG_INTF_HELPERS_HPP_
