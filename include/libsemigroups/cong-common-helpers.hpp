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

// This file contains helper functions for the derived classes of
// detail::CongruenceCommon. This is a separate file so that we can declare all
// the derived classes of detail::CongruenceCommon prior to declaring the
// functions in this file.
//
// The implementation of all helpers must go into the namespace
// congruence_common, and then be aliased into the, e.g., todd_coxeter
// namespace. This makes it possible to generically use, e.g.,
// congruence_common::normal_forms in, e.g., the python bindings.

#ifndef LIBSEMIGROUPS_CONG_COMMON_HELPERS_HPP_
#define LIBSEMIGROUPS_CONG_COMMON_HELPERS_HPP_

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
#include "ranges.hpp"     // for is_input_or_sink_v, iterator_...

namespace libsemigroups {

  // Forward decls
  template <typename Word>
  class Congruence;

  template <typename Word>
  class ToddCoxeter;

  enum class tril;

  namespace detail {
    class ToddCoxeterBase;
    class CongruenceCommon;
    struct CongruenceBase;

    static inline void throw_if_nullptr(char const*      w,
                                        std::string_view arg = "1st") {
      if (w == nullptr) {
        LIBSEMIGROUPS_EXCEPTION(
            "the {} argument (char const*) must not be nullptr", arg);
      }
    }
  }  // namespace detail

  namespace congruence_common {

    //! \defgroup cong_common_helpers_group Common congruence helpers
    //!
    //! This page contains documentation for helper functions for the classes
    //! Congruence, Kambites, KnuthBendix, and \ref_todd_coxeter. The
    //! functions documented on this page belong to all of the namespaces
    //! \ref cong_common_helpers_group "congruence_common", \ref congruence, \ref
    //! kambites, \ref knuth_bendix, and \ref todd_coxeter.

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - add_generating_pair
    ////////////////////////////////////////////////////////////////////////

    //! \defgroup cong_common_helpers_add_pair_group Add generating pairs
    //! \ingroup cong_common_helpers_group
    //!
    //! \brief Add a generating pair using objects instead of iterators.
    //!
    //! This page contains the documentation of the functions
    //! \ref add_generating_pair and \ref add_generating_pair_no_checks
    //! which can be invoked with a variety of different argument types.
    //!
    //! @{

    //! \anchor add_generating_pairs_no_checks_main
    //! \brief Helper for adding a generating pair of words.
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! This function can be used to add a generating pair to \p thing
    //! using objects themselves rather than iterators.
    //!
    //! \tparam Thing the type of the first parameter must be one of
    //! Kambites, KnuthBendix, \ref_todd_coxeter, or Congruence.
    //!
    //! \param thing the object to add generating pairs to.
    //! \param u the left hand side of the pair to add.
    //! \param v the right hand side of the pair to add.
    //!
    //! \return A reference to \p thing.
    //!
    //! \cong_common_warn_assume_letters_in_bounds
    // NOTE: we use native_word_type and not another template param to avoid
    // unexpected behaviour, if for example we add words which are strings to a
    // ToddCoxeter<word_type>, then unexpected things might happen.
    template <typename Thing>
    Thing&
    add_generating_pair_no_checks(Thing&                                  thing,
                                  typename Thing::native_word_type const& u,
                                  typename Thing::native_word_type const& v) {
      static_assert(std::is_base_of_v<detail::CongruenceCommon, Thing>);
      return thing.add_generating_pair_no_checks(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    //! \brief Helper for adding a generating pair of words
    //! (std::initializer_list).
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! \tparam Int must satisfy `std::integral_type_v<Int>`.
    //!
    //! See \ref add_generating_pairs_no_checks_main
    //! "add_generating_pair_no_checks" for details.
    template <typename Thing, typename Int>
    Thing& add_generating_pair_no_checks(Thing& thing,
                                         std::initializer_list<Int> const& u,
                                         std::initializer_list<Int> const& v) {
      static_assert(std::is_base_of_v<detail::CongruenceCommon, Thing>);
      static_assert(std::is_integral_v<Int>);
      return thing.add_generating_pair_no_checks(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    //! \brief Helper for adding a generating pair of words
    //! (string literals).
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! See \ref add_generating_pairs_no_checks_main
    //! "add_generating_pair_no_checks" for details.
    template <typename Thing>
    Thing& add_generating_pair_no_checks(Thing&      thing,
                                         char const* u,
                                         char const* v) {
      static_assert(std::is_base_of_v<detail::CongruenceCommon, Thing>);
      LIBSEMIGROUPS_ASSERT(u != nullptr);
      LIBSEMIGROUPS_ASSERT(v != nullptr);
      // We could static_assert that Thing::native_word_type == std::string,
      // but it doesn't seem that adding this restriction would gain us
      // anything, so it is not currently done.
      return thing.add_generating_pair_no_checks(
          u, u + std::strlen(u), v, v + std::strlen(v));
    }

    // This version of the function catches the cases when u & v are not of
    // the same type but both convertible to string_view
    //! \brief Helper for adding a generating pair of words (std::string_view).
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! See \ref add_generating_pairs_no_checks_main
    //! "add_generating_pair_no_checks" for details.
    template <typename Thing>
    Thing& add_generating_pair_no_checks(Thing&           thing,
                                         std::string_view u,
                                         std::string_view v) {
      static_assert(std::is_base_of_v<detail::CongruenceCommon, Thing>);
      // We could static_assert that Thing::native_word_type == std::string,
      // but it doesn't seem that adding this restriction would gain us
      // anything, so it is not currently done.
      return thing.add_generating_pair_no_checks(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    //! \anchor add_generating_pairs_main
    //! \brief Helper for adding a generating pair of words.
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! This function can be used to add a generating pair to \p thing
    //! using objects themselves rather than iterators.
    //!
    //! \tparam Thing the type of the first parameter must be one of
    //! Kambites, KnuthBendix, \ref_todd_coxeter, or Congruence.
    //!
    //! \param thing the object to add generating pairs to.
    //! \param u the left hand side of the pair to add.
    //! \param v the right hand side of the pair to add.
    //!
    //! \return A reference to \p thing.
    //!
    //! \cong_common_throws_if_letters_out_of_bounds
    // NOTE: we use native_word_type and not another template param to avoid
    // unexpected behaviour, if for example we add words which are strings to a
    // ToddCoxeter<word_type>, then unexpected things might happen.
    template <typename Thing>
    Thing& add_generating_pair(Thing&                                  thing,
                               typename Thing::native_word_type const& u,
                               typename Thing::native_word_type const& v) {
      static_assert(std::is_base_of_v<detail::CongruenceCommon, Thing>);
      return thing.add_generating_pair(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    //! \brief Helper for adding a generating pair of words
    //! (std::initializer_list).
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! \tparam Int must satisfy `std::integral_type_v<Int>`.
    //!
    //! See \ref add_generating_pairs_main "add_generating_pairs" for details.
    template <typename Thing, typename Int>
    Thing& add_generating_pair(Thing&                            thing,
                               std::initializer_list<Int> const& u,
                               std::initializer_list<Int> const& v) {
      static_assert(std::is_base_of_v<detail::CongruenceCommon, Thing>);
      static_assert(std::is_integral_v<Int>);
      return thing.add_generating_pair(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    //! \brief Helper for adding a generating pair of words
    //! (string literals).
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! See \ref add_generating_pairs_main "add_generating_pairs" for details.
    template <typename Thing>
    Thing& add_generating_pair(Thing& thing, char const* u, char const* v) {
      static_assert(std::is_base_of_v<detail::CongruenceCommon, Thing>);
      detail::throw_if_nullptr(u, "2nd");
      detail::throw_if_nullptr(v, "3rd");
      // We could static_assert that Thing::native_word_type == std::string,
      // but it doesn't seem that adding this restriction would gain us
      // anything, so it is not currently done.
      return thing.add_generating_pair(
          u, u + std::strlen(u), v, v + std::strlen(v));
    }

    // This version of the function catches the cases when u & v are not of
    // the same type but both convertible to string_view
    //! \brief Helper for adding a generating pair of words
    //! (std::string_view).
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! See \ref add_generating_pairs_main "add_generating_pairs" for details.
    template <typename Thing>
    Thing& add_generating_pair(Thing&           thing,
                               std::string_view u,
                               std::string_view v) {
      static_assert(std::is_base_of_v<detail::CongruenceCommon, Thing>);
      // We could static_assert that Thing::native_word_type == std::string,
      // but it doesn't seem that adding this restriction would gain us
      // anything, so it is not currently done.
      return thing.add_generating_pair_no_checks(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    //! @}

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - currently_contains_no_checks
    ////////////////////////////////////////////////////////////////////////

    //! \defgroup cong_common_helpers_contains_group Containment
    //! \ingroup cong_common_helpers_group
    //!
    //! \brief Check containment of a pair of words in a congruence.
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! This page contains the documentation of the functions
    //! \ref currently_contains_no_checks; \ref currently_contains;
    //! \ref contains_no_checks; and \ref contains which can be invoked with a
    //! variety of different argument types.
    //!
    //! These helper functions can be applied to objects of the types:
    //! * Congruence
    //! * Kambites
    //! * KnuthBendix
    //! * \ref_todd_coxeter
    //!
    //! Functions with the prefix `currently_` do not perform any enumeration
    //! of the object representing a congruence; and those with the suffix
    //! `_no_checks` do not check that the input words are valid.
    //!
    //! @{

    //! \anchor currently_contains_no_checks_main
    //! \brief Check containment of a pair of words.
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! This function checks whether or not the words \p u and \p v are already
    //! known to be contained in the congruence represented by \p thing. This
    //! function performs no enumeration of \p thing, so it is possible for the
    //! words to be contained in the congruence, but that this is not currently
    //! known.
    //!
    //! \tparam Thing the type of the first parameter must be one of
    //! Kambites, KnuthBendix, \ref_todd_coxeter, or Congruence.
    //!
    //! \param thing the object to check containment in.
    //! \param u the first word.
    //! \param v the second word.
    //!
    //! \returns
    //! * tril::TRUE if the words are known to belong to the congruence;
    //! * tril::FALSE if the words are known to not belong to the congruence;
    //! * tril::unknown otherwise.
    //!
    //! \cong_common_warn_assume_letters_in_bounds
    // NOTE: we use native_word_type and not another template param to avoid
    // unexpected behaviour, if for example we add words which are strings to a
    // ToddCoxeter<word_type>, then unexpected things might happen.
    template <typename Thing>
    [[nodiscard]] tril
    currently_contains_no_checks(Thing const&                            thing,
                                 typename Thing::native_word_type const& u,
                                 typename Thing::native_word_type const& v) {
      static_assert(std::is_base_of_v<detail::CongruenceCommon, Thing>);
      return thing.currently_contains_no_checks(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    //! \brief Helper for checking containment of a pair of words
    //! (std::initializer_list).
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! \tparam Int must satisfy `std::integral_type_v<Int>`.
    //!
    //! See \ref currently_contains_no_checks_main
    //! "currently_contains_no_checks" for details.
    template <typename Thing, typename Int>
    [[nodiscard]] tril
    currently_contains_no_checks(Thing const&                      thing,
                                 std::initializer_list<Int> const& u,
                                 std::initializer_list<Int> const& v) {
      static_assert(std::is_base_of_v<detail::CongruenceCommon, Thing>);
      static_assert(std::is_integral_v<Int>);
      return thing.currently_contains_no_checks(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    //! \brief Helper for checking containment of a pair of words
    //! (string literal).
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! See \ref currently_contains_no_checks_main
    //! "currently_contains_no_checks" for details.
    template <typename Thing>
    [[nodiscard]] tril currently_contains_no_checks(Thing const& thing,
                                                    char const*  u,
                                                    char const*  v) {
      static_assert(std::is_base_of_v<detail::CongruenceCommon, Thing>);
      LIBSEMIGROUPS_ASSERT(u != nullptr);
      LIBSEMIGROUPS_ASSERT(v != nullptr);
      // We could static_assert that Thing::native_word_type == std::string,
      // but it doesn't seem that adding this restriction would gain us
      // anything, so it is not currently done.
      return thing.currently_contains_no_checks(
          u, u + std::strlen(u), v, v + std::strlen(v));
    }

    //! \brief Helper for checking containment of a pair of words
    //! (std::string_view).
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! See \ref currently_contains_no_checks_main
    //! "currently_contains_no_checks" for details.
    // NOTE: This version of the function catches the cases when u & v are not
    // of the same type but both convertible to string_view
    template <typename Thing>
    [[nodiscard]] tril currently_contains_no_checks(Thing const&     thing,
                                                    std::string_view u,
                                                    std::string_view v) {
      static_assert(std::is_base_of_v<detail::CongruenceCommon, Thing>);
      // We could static_assert that Thing::native_word_type == std::string,
      // but it doesn't seem that adding this restriction would gain us
      // anything, so it is not currently done.
      return thing.currently_contains_no_checks(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - currently_contains
    ////////////////////////////////////////////////////////////////////////

    //! \anchor currently_contains_main
    //! \brief Check containment of a pair of words.
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! This function checks whether or not the words \p u and \p v are already
    //! known to be contained in the congruence represented by \p thing. This
    //! function performs no enumeration of \p thing, so it is possible for the
    //! words to be contained in the congruence, but that this is not currently
    //! known.
    //!
    //! \tparam Thing the type of the first parameter must be one of
    //! Kambites, KnuthBendix, \ref_todd_coxeter, or Congruence.
    //!
    //! \param thing the object to check containment in.
    //! \param u the first word.
    //! \param v the second word.
    //!
    //! \returns
    //! * tril::TRUE if the words are known to belong to the congruence;
    //! * tril::FALSE if the words are known to not belong to the congruence;
    //! * tril::unknown otherwise.
    //!
    //! \cong_common_throws_if_letters_out_of_bounds
    // NOTE: we use native_word_type and not another template param to avoid
    // unexpected behaviour, if for example we add words which are strings to a
    // ToddCoxeter<word_type>, then unexpected things might happen.
    template <typename Thing>
    [[nodiscard]] tril
    currently_contains(Thing const&                            thing,
                       typename Thing::native_word_type const& u,
                       typename Thing::native_word_type const& v) {
      static_assert(std::is_base_of_v<detail::CongruenceCommon, Thing>);
      return thing.currently_contains(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    //! \brief Helper for checking containment of a pair of words
    //! (std::initializer_list).
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! \tparam Int must satisfy `std::integral_type_v<Int>`.
    //!
    //! See \ref currently_contains_main "currently_contains" for details.
    template <typename Thing, typename Int>
    [[nodiscard]] tril currently_contains(Thing const& thing,
                                          std::initializer_list<Int> const& u,
                                          std::initializer_list<Int> const& v) {
      static_assert(std::is_base_of_v<detail::CongruenceCommon, Thing>);
      static_assert(std::is_integral_v<Int>);
      return thing.currently_contains(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    //! \brief Helper for checking containment of a pair of words
    //! (string literal).
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! See \ref currently_contains_main "currently_contains" for details.
    template <typename Thing>
    [[nodiscard]] tril currently_contains(Thing const& thing,
                                          char const*  u,
                                          char const*  v) {
      static_assert(std::is_base_of_v<detail::CongruenceCommon, Thing>);
      detail::throw_if_nullptr(u, "2nd");
      detail::throw_if_nullptr(v, "3rd");
      return thing.currently_contains(
          u, u + std::strlen(u), v, v + std::strlen(v));
    }

    // This version of the function catches the cases when u & v are not of
    // the same type but both convertible to string_view
    //! \brief Helper for checking containment of a pair of words
    //! (std::string_view).
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! See \ref currently_contains_main "currently_contains" for details.
    template <typename Thing>
    [[nodiscard]] tril currently_contains(Thing const&     thing,
                                          std::string_view u,
                                          std::string_view v) {
      static_assert(std::is_base_of_v<detail::CongruenceCommon, Thing>);
      return thing.currently_contains(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - contains_no_checks
    ////////////////////////////////////////////////////////////////////////

    //! \anchor contains_no_checks_main
    //! \brief Check containment of a pair of words.
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! This function checks whether or not the words \p u and \p v are
    //! contained in the congruence represented by \p thing. This function
    //! triggers a full enumeration of \p thing, which may never terminate.
    //!
    //! \tparam Thing the type of the first parameter must be one of
    //! Kambites, KnuthBendix, \ref_todd_coxeter, or Congruence.
    //!
    //! \param thing the object to check containment in.
    //! \param u the left hand side of the pair to add.
    //! \param v the right hand side of the pair to add.
    //!
    //! \returns Whether or not the pair belongs to the congruence.
    //!
    //! \cong_common_warn_assume_letters_in_bounds
    // NOTE: we use native_word_type and not another template param to avoid
    // unexpected behaviour, if for example we add words which are strings to a
    // ToddCoxeter<word_type>, then unexpected things might happen.
    template <typename Thing>
    [[nodiscard]] bool
    contains_no_checks(Thing&                                  thing,
                       typename Thing::native_word_type const& u,
                       typename Thing::native_word_type const& v) {
      static_assert(std::is_base_of_v<detail::CongruenceCommon, Thing>);
      return thing.contains_no_checks(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    //! \brief Helper for checking containment of a pair of words
    //! (std::initializer_list).
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! \tparam Int must satisfy `std::integral_type_v<Int>`.
    //!
    //! See \ref contains_no_checks_main "contains_no_checks" for details.
    template <typename Thing, typename Int>
    [[nodiscard]] bool contains_no_checks(Thing& thing,
                                          std::initializer_list<Int> const& u,
                                          std::initializer_list<Int> const& v) {
      static_assert(std::is_base_of_v<detail::CongruenceCommon, Thing>);
      static_assert(std::is_integral_v<Int>);
      return contains_no_checks<Thing, std::initializer_list<Int>>(thing, u, v);
    }

    //! \brief Helper for checking containment of a pair of words
    //! (string literal).
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! See \ref contains_no_checks_main "contains_no_checks" for details.
    template <typename Thing>
    [[nodiscard]] bool contains_no_checks(Thing&      thing,
                                          char const* u,
                                          char const* v) {
      static_assert(std::is_base_of_v<detail::CongruenceCommon, Thing>);
      LIBSEMIGROUPS_ASSERT(u != nullptr);
      LIBSEMIGROUPS_ASSERT(v != nullptr);
      // We could static_assert that Thing::native_word_type == std::string,
      // but it doesn't seem that adding this restriction would gain us
      // anything, so it is not currently done.
      return thing.contains_no_checks(
          u, u + std::strlen(u), v, v + std::strlen(v));
    }

    // This version of the function catches the cases when u & v are not of
    // the same type but both convertible to string_view
    //! \brief Helper for checking containment of a pair of words
    //! (std::string_view).
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! See \ref contains_no_checks_main "contains_no_checks" for details.
    template <typename Thing>
    [[nodiscard]] bool contains_no_checks(Thing&           thing,
                                          std::string_view u,
                                          std::string_view v) {
      static_assert(std::is_base_of_v<detail::CongruenceCommon, Thing>);
      // We could static_assert that Thing::native_word_type == std::string,
      // but it doesn't seem that adding this restriction would gain us
      // anything, so it is not currently done.
      return thing.contains_no_checks(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - contains
    ////////////////////////////////////////////////////////////////////////

    //! \anchor contains_main
    //! \brief Check containment of a pair of words.
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! This function checks whether or not the words \p u and \p v are
    //! contained in the congruence represented by \p thing. This function
    //! triggers a full enumeration of \p thing, which may never terminate.
    //!
    //! \tparam Thing the type of the first parameter must be one of
    //! Kambites, KnuthBendix, \ref_todd_coxeter, or Congruence.
    //!
    //! \param thing the object to check containment in.
    //! \param u the left hand side of the pair to add.
    //! \param v the right hand side of the pair to add.
    //!
    //! \returns Whether or not the pair belongs to the congruence.
    //!
    //! \cong_common_throws_if_letters_out_of_bounds
    // NOTE: we use native_word_type and not another template param to avoid
    // unexpected behaviour, if for example we add words which are strings to a
    // ToddCoxeter<word_type>, then unexpected things might happen.
    template <typename Thing>
    [[nodiscard]] bool contains(Thing&                                  thing,
                                typename Thing::native_word_type const& u,
                                typename Thing::native_word_type const& v) {
      static_assert(std::is_base_of_v<detail::CongruenceCommon, Thing>);
      return thing.contains(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    //! \brief Helper for checking containment of a pair of words
    //! (std::initializer_list).
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! \tparam Int must satisfy `std::integral_type_v<Int>`.
    //!
    //! See \ref contains_main "contains" for details.
    template <typename Thing, typename Int>
    [[nodiscard]] bool contains(Thing&                            thing,
                                std::initializer_list<Int> const& u,
                                std::initializer_list<Int> const& v) {
      static_assert(std::is_base_of_v<detail::CongruenceCommon, Thing>);
      static_assert(std::is_integral_v<Int>);
      return thing.contains(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    // This version of the function catches the cases when u & v are not of
    // the same type but both convertible to string_view
    //! \brief Helper for checking containment of a pair of words
    //! (std::string_view).
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! See \ref contains_main "contains" for details.
    template <typename Thing>
    [[nodiscard]] bool contains(Thing&           thing,
                                std::string_view u,
                                std::string_view v) {
      static_assert(std::is_base_of_v<detail::CongruenceCommon, Thing>);
      // We could static_assert that Thing::native_word_type == std::string,
      // but it doesn't seem that adding this restriction would gain us
      // anything, so it is not currently done.
      return thing.contains(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    //! \brief Helper for checking containment of a pair of words
    //! (string literal).
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! See \ref contains_main "contains" for details.
    template <typename Thing>
    [[nodiscard]] bool contains(Thing& thing, char const* u, char const* v) {
      static_assert(std::is_base_of_v<detail::CongruenceCommon, Thing>);
      detail::throw_if_nullptr(u, "2nd");
      detail::throw_if_nullptr(v, "3rd");
      // We could static_assert that Thing::native_word_type == std::string,
      // but it doesn't seem that adding this restriction would gain us
      // anything, so it is not currently done.
      return thing.contains(u, u + std::strlen(u), v, v + std::strlen(v));
    }

    //! @}

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - reduce_no_run_no_checks
    ////////////////////////////////////////////////////////////////////////

    //! \defgroup cong_common_helpers_reduce_group Reduce a word
    //! \ingroup cong_common_helpers_group
    //!
    //! \brief Find an irreducible word equivalent to a given word.
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! This page contains the documentation of the functions \ref
    //! reduce_no_run_no_checks; \ref reduce_no_run; \ref reduce_no_checks; and
    //! \ref reduce which can be invoked with a variety of different argument
    //! types.
    //!
    //! These helper functions can be applied to objects of the types:
    //! * Congruence
    //! * Kambites
    //! * KnuthBendix
    //! * \ref_todd_coxeter
    //!
    //! Functions with the suffix `_no_run` do not perform any enumeration of
    //! the object representing a congruence; and those with the suffix
    //! `_no_checks` do not check that the input words are valid.
    //!
    //! @{

    //! \anchor reduce_no_run_no_checks_main
    //! \brief Reduce a word with no enumeration or checks.
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! This function returns a reduced word equivalent to the input word \p w
    //! in the congruence represented by \p thing. This function triggers no
    //! enumeration of \p thing. The word output by this function is equivalent
    //! to the input word in the congruence. If \p thing is \ref
    //! Runner::finished, then the output word is a normal form for the input
    //! word. If \p thing is not \ref Runner::finished, then it might be that
    //! equivalent input words produce different output words.
    //!
    //! \tparam Thing the type of the first parameter must be one of
    //! Kambites, KnuthBendix, \ref_todd_coxeter, or Congruence.
    //!
    //! \param thing the object to reduce words in.
    //! \param w the word to reduce.
    //!
    //! \returns An irreducible word equivalent to \p w.
    //!
    //! \cong_common_warn_assume_letters_in_bounds
    // NOTE: we use native_word_type and not another template param to avoid
    // unexpected behaviour, if for example we add words which are strings to a
    // ToddCoxeter<word_type>, then unexpected things might happen.
    template <typename Thing>
    [[nodiscard]] typename Thing::native_word_type
    reduce_no_run_no_checks(Thing const&                            thing,
                            typename Thing::native_word_type const& w);

    // No string_view version is required because there is only a single
    // "word" parameter, and so the first template will catch every case
    // except initializer_list and char const*

    //! \brief Reduce a word (std::initializer_list).
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! \tparam Int must satisfy `std::integral_type_v<Int>`.
    //!
    //! See \ref reduce_no_run_no_checks_main
    //! "reduce_no_run_no_checks" for details.
    template <typename Thing, typename Int>
    [[nodiscard]] typename Thing::native_word_type
    reduce_no_run_no_checks(Thing const&                      thing,
                            std::initializer_list<Int> const& w);

    //! \brief Reduce a word (string literal).
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! See \ref reduce_no_run_no_checks_main
    //! "reduce_no_run_no_checks" for details.
    template <typename Thing>
    [[nodiscard]] typename Thing::native_word_type
    reduce_no_run_no_checks(Thing const& thing, char const* w);

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - reduce_no_run
    ////////////////////////////////////////////////////////////////////////

    //! \anchor reduce_no_run_main
    //! \brief Reduce a word with no enumeration.
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! This function returns a reduced word equivalent to the input word \p w
    //! in the congruence represented by \p thing. This function triggers no
    //! enumeration of \p thing. The word output by this function is equivalent
    //! to the input word in the congruence. If \p thing is \ref
    //! Runner::finished, then the output word is a normal form for the input
    //! word. If \p thing is not \ref Runner::finished, then it might be that
    //! equivalent input words produce different output words.
    //!
    //! \tparam Thing the type of the first parameter must be one of
    //! Kambites, KnuthBendix, \ref_todd_coxeter, or Congruence.
    //!
    //! \param thing the object to reduce words in.
    //! \param w the word to reduce.
    //!
    //! \returns An irreducible word equivalent to \p w.
    //!
    //! \cong_common_throws_if_letters_out_of_bounds
    // NOTE: we use native_word_type and not another template param to avoid
    // unexpected behaviour, if for example we add words which are strings to a
    // ToddCoxeter<word_type>, then unexpected things might happen.
    template <typename Thing>
    [[nodiscard]] typename Thing::native_word_type
    reduce_no_run(Thing const&                            thing,
                  typename Thing::native_word_type const& w);

    // No string_view version is required because there is only a single
    // "w" parameter, and so the first template will catch every case
    // except initializer_list and char const*

    //! \brief Reduce a word (std::initializer_list).
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! \tparam Int must satisfy `std::integral_type_v<Int>`.
    //!
    //! See \ref reduce_no_run_main "reduce_no_run" for details.
    template <typename Thing, typename Int>
    [[nodiscard]] typename Thing::native_word_type
    reduce_no_run(Thing const& thing, std::initializer_list<Int> const& w);

    //! \brief Reduce a word (string literal).
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! See \ref reduce_no_run_main "reduce_no_run" for details.
    template <typename Thing>
    [[nodiscard]] typename Thing::native_word_type
    reduce_no_run(Thing const& thing, char const* w);

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - reduce_no_checks
    ////////////////////////////////////////////////////////////////////////

    //! \anchor reduce_no_checks_main
    //! \brief Reduce a word with no checks.
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! This function returns a reduced word equivalent to the input word \p w
    //! in the congruence represented by \p thing. This function triggers a full
    //! enumeration of \p thing. The output word is a normal form for the input
    //! word.
    //!
    //! \tparam Thing the type of the first parameter must be one of
    //! Kambites, KnuthBendix, \ref_todd_coxeter, or Congruence.
    //!
    //! \param thing the object to reduce words in.
    //! \param w the word to reduce.
    //!
    //! \returns An irreducible word equivalent to \p w.
    //!
    //! \cong_common_warn_assume_letters_in_bounds
    // NOTE: we use native_word_type and not another template param to avoid
    // unexpected behaviour, if for example we add words which are strings to a
    // ToddCoxeter<word_type>, then unexpected things might happen.
    template <typename Thing>
    [[nodiscard]] typename Thing::native_word_type
    reduce_no_checks(Thing const&                            thing,
                     typename Thing::native_word_type const& w);

    // No string_view version is required because there is only a single
    // "word" parameter, and so the first template will catch every case
    // except initializer_list and char const*

    //! \brief Reduce a word (std::initializer_list).
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! \tparam Int must satisfy `std::integral_type_v<Int>`.
    //!
    //! See \ref reduce_no_checks_main "reduce_no_checks" for details.
    template <typename Thing, typename Int>
    [[nodiscard]] typename Thing::native_word_type
    reduce_no_checks(Thing const& thing, std::initializer_list<Int> const& w);

    //! \brief Reduce a word (string literal).
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! See \ref reduce_no_checks_main "reduce_no_checks" for details.
    template <typename Thing>
    [[nodiscard]] typename Thing::native_word_type
    reduce_no_checks(Thing const& thing, char const* w);

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - reduce
    ////////////////////////////////////////////////////////////////////////
    //! \anchor reduce_main
    //! \brief Reduce a word.
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! This function returns a reduced word equivalent to the input word \p w
    //! in the congruence represented by \p thing. This function triggers a full
    //! enumeration of \p thing. The output word is a normal form for the input
    //! word.
    //!
    //! \tparam Thing the type of the first parameter must be one of
    //! Kambites, KnuthBendix, \ref_todd_coxeter, or Congruence.
    //!
    //! \param thing the object to reduce words in.
    //! \param w the word to reduce.
    //!
    //! \returns An irreducible word equivalent to \p w.
    //!
    //! \cong_common_throws_if_letters_out_of_bounds
    // NOTE: we use native_word_type and not another template param to avoid
    // unexpected behaviour, if for example we add words which are strings to a
    // ToddCoxeter<word_type>, then unexpected things might happen.
    template <typename Thing>
    [[nodiscard]] typename Thing::native_word_type
    reduce(Thing const& thing, typename Thing::native_word_type const& w);

    // No string_view version is required because there is only a single
    // "word" parameter, and so the first template will catch every case
    // except initializer_list and char const*

    //! \brief Reduce a word (std::initializer_list).
    //!
    //! \tparam Int must satisfy `std::integral_type_v<Int>`.
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! See \ref reduce_main "reduce" for details.
    template <typename Thing, typename Int>
    [[nodiscard]] typename Thing::native_word_type
    reduce(Thing const& thing, std::initializer_list<Int> const& w);

    //! \brief Reduce a word (string literal).
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! See \ref reduce_main "reduce" for details.
    template <typename Thing>
    [[nodiscard]] typename Thing::native_word_type reduce(Thing const& thing,
                                                          char const*  w);

    //! @}

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - normal_forms
    ////////////////////////////////////////////////////////////////////////

    // There's nothing in common to implement in this file.

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - partition
    ////////////////////////////////////////////////////////////////////////

    //! \defgroup cong_common_helpers_partition_group Partitioning
    //! \ingroup cong_common_helpers_group
    //!
    //! \brief Partition a range of words by a congruence.
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! This page contains the documentation of the functions \ref
    //! partition(Thing&, Range) and \ref non_trivial_classes for partitioning a
    //! range of words by a congruence.
    //!
    //! @{

#ifndef PARSED_BY_DOXYGEN
    // Forward decls defined in todd-coxeter-helpers.tpp and cong-helpers.tpp
    template <typename Word,
              typename Range,
              typename = std::enable_if_t<rx::is_input_or_sink_v<Range>>>
    [[nodiscard]] std::vector<std::vector<Word>>
    partition(ToddCoxeter<Word>& thing, Range r);

    template <typename Word,
              typename Range,
              typename = std::enable_if_t<rx::is_input_or_sink_v<Range>>>
    [[nodiscard]] std::vector<std::vector<Word>>
    partition(Congruence<Word>& thing, Range r);
#endif

    //! \brief Partition a range of words.
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! This function returns the partition of the words in the range \p r
    //! induced by the object \p thing. This function triggers a full
    //! enumeration of \p thing.
    //!
    //! \tparam Thing the type of the first parameter must be one of
    //! Kambites, KnuthBendix, \ref_todd_coxeter, or Congruence.
    //!
    //! \tparam Range the type of the input range of words, must satisfy
    //! `std::enable_if_t<rx::is_input_or_sink_v<Range>>` and
    //! `Range::output_type` must decay to `Thing::native_word_type`.
    //!
    //! \param thing the object used to partition \p r.
    //! \param r the input range of words.
    //!
    //! \returns The partition of the input range.
    //!
    //! \throws LibsemigroupsException if the input range of words is
    //! infinite.
    template <typename Thing,
              typename Range,
              typename = std::enable_if_t<rx::is_input_or_sink_v<Range>>>
    [[nodiscard]] std::vector<std::vector<typename Thing::native_word_type>>
    partition(Thing& thing, Range r);

    //! \brief Partition a range of words (via iterators)
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! See \ref partition(Thing&, Range) for details.
    template <typename Thing, typename Iterator1, typename Iterator2>
    std::vector<std::vector<typename Thing::native_word_type>>
    partition(Thing& thing, Iterator1 first, Iterator2 last) {
      // static asserts are in done in the next call to partition
      return partition(thing, rx::iterator_range(first, last));
    }

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - non_trivial_classes
    ////////////////////////////////////////////////////////////////////////

    //! \brief Find the non-trivial classes in the partition of a range of
    //! words.
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! This function returns the classes with size at least \f$2\f$ in the
    //! partition of the words in the range \p r according to \p thing.
    //! This function triggers a full enumeration of \p thing.
    //!
    //! \tparam Thing the type of the first parameter must be one of
    //! Kambites, KnuthBendix, \ref_todd_coxeter, or Congruence.
    //!
    //! \tparam Range the type of the input range of words, must satisfy
    //! `std::enable_if_t<rx::is_input_or_sink_v<Range>>` and
    //! `Range::output_type` must decay to `Thing::native_word_type`.
    //!
    //! \param thing the object used to partition \p r.
    //! \param r the input range of words.
    //!
    //! \returns The partition of the input range.
    //!
    //! \throws LibsemigroupsException if the input range of words is
    //! infinite.
    template <typename Thing,
              typename Range,
              typename = std::enable_if_t<rx::is_input_or_sink_v<Range>>>
    [[nodiscard]] std::vector<std::vector<typename Thing::native_word_type>>
    non_trivial_classes(Thing& thing, Range r);

    //! \brief Partition a range of words into non-trivial classes (via
    //! iterators).
    //!
    //! Defined in `cong-common-helpers.hpp`.
    //!
    //! See \ref non_trivial_classes(Thing&, Range) for details.
    template <typename Thing, typename Iterator1, typename Iterator2>
    std::vector<std::vector<typename Thing::native_word_type>>
    non_trivial_classes(Thing& thing, Iterator1 first, Iterator2 last) {
      return non_trivial_classes(thing, rx::iterator_range(first, last));
    }

    //! @}
  }  // namespace congruence_common
}  // namespace libsemigroups

#include "cong-common-helpers.tpp"

#endif  // LIBSEMIGROUPS_CONG_COMMON_HELPERS_HPP_
