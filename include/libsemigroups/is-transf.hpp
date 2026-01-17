//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025-2026 James D. Mitchell
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

// This file contains some functions for checking whether or not a container
// defines a partial transformation, full transformation, partial permutation,
// or permutation. These are used in transf.hpp for argument checking, and also
// in hpcombi.hpp for the same.

#ifndef LIBSEMIGROUPS_IS_TRANSF_HPP_
#define LIBSEMIGROUPS_IS_TRANSF_HPP_

#include <algorithm>      // for none_of, find_if
#include <cstddef>        // for size_t
#include <iterator>       // for distance
#include <string_view>    // for string_view
#include <type_traits>    // for decay_t, is_unsigned_v
#include <unordered_map>  // for unordered_map
#include <utility>        // for declval, pair

#include "constants.hpp"  // for UNDEFINED

#include "detail/print.hpp"  // for to_printable

namespace libsemigroups {

  namespace detail {

    // Returns "{it, pos}" where "it" is an iterator point to the first
    // repeated element and "pos" is the position of the first occurrence of
    // that element. Returns {last, std::distance(first, last)} if not
    // duplicates.
    template <typename Iterator>
    std::pair<Iterator, size_t> find_duplicates(
        Iterator                                                    first,
        Iterator                                                    last,
        std::unordered_map<std::decay_t<decltype(*first)>, size_t>& seen);

    template <typename Iterator>
    [[nodiscard]] std::pair<Iterator, size_t> find_duplicates(Iterator first,
                                                              Iterator last) {
      std::unordered_map<std::decay_t<decltype(*first)>, size_t> seen;
      return find_duplicates(first, last, seen);
    }

    template <typename Iterator>
    [[nodiscard]] bool has_duplicates(Iterator first, Iterator last) {
      return find_duplicates(first, last).first != last;
    }

    template <typename Iterator>
    void throw_if_duplicates(
        Iterator                                                    first,
        Iterator                                                    last,
        std::unordered_map<std::decay_t<decltype(*first)>, size_t>& seen,
        std::string_view                                            where);

    template <typename Iterator>
    void throw_if_duplicates(Iterator         first,
                             Iterator         last,
                             std::string_view where) {
      std::unordered_map<std::decay_t<decltype(*first)>, size_t> seen;
      throw_if_duplicates(first, last, seen, where);
    }

    template <typename Iterator, typename Func>
    void throw_if_value_out_of_range(Iterator         first,
                                     Iterator         last,
                                     Func&&           func,
                                     std::string_view where);

    template <typename Iterator>
    void throw_if_not_ptransf(Iterator first, Iterator last, size_t deg);

    template <typename Iterator>
    void throw_if_not_ptransf(Iterator first, Iterator last) {
      throw_if_not_ptransf(first, last, std::distance(first, last));
    }

    template <typename Iterator>
    void throw_if_not_ptransf(Iterator dom_first,
                              Iterator dom_last,
                              Iterator img_first,
                              Iterator img_last,
                              size_t   deg);

    template <typename Iterator>
    void throw_if_not_transf(Iterator first, Iterator last, size_t deg);

    template <typename Iterator>
    void throw_if_not_transf(Iterator first, Iterator last) {
      throw_if_not_transf(first, last, std::distance(first, last));
    }

    template <typename Iterator>
    void throw_if_not_perm(Iterator first, Iterator last, size_t deg) {
      throw_if_not_transf(first, last, deg);
      throw_if_duplicates(first, last, "image");
    }

    template <typename Iterator>
    void throw_if_not_perm(Iterator first, Iterator last) {
      throw_if_not_perm(first, last, std::distance(first, last));
    }

    template <typename Iterator>
    void throw_if_not_pperm(Iterator first, Iterator last, size_t deg) {
      throw_if_not_ptransf(first, last, deg);
      throw_if_duplicates(first, last, "image");
    }

    template <typename Iterator>
    void throw_if_not_pperm(Iterator first, Iterator last) {
      throw_if_not_ptransf(first, last);
      throw_if_duplicates(first, last, "image");
    }

    template <typename Iterator>
    void throw_if_not_pperm(Iterator dom_first,
                            Iterator dom_last,
                            Iterator img_first,
                            Iterator img_last,
                            size_t   deg) {
      throw_if_not_ptransf(dom_first, dom_last, img_first, img_last, deg);
      throw_if_duplicates(img_first, img_last, "image");
    }
  }  // namespace detail

  //! \ingroup transf_group
  //!
  //! \brief Returns whether or not the argument describes a
  //! partial transformation.
  //!
  //! Defined in `is-transf.hpp`.
  //!
  //! This function returns `true` if every value in the range from \p first to
  //! \p last is less than \p deg or equal to UNDEFINED; `false` is
  //! returned otherwise.
  //!
  //! \tparam Iterator the type of the iterators \p first and \p last.
  //!
  //! \param first iterator pointing a the start of the possible partial
  //! transformation.
  //!
  //! \param last iterator pointing one beyond the end of the
  //! possible partial transformation.
  //!
  //! \param deg the degree.
  //!
  //! \returns Whether or not \p first to \p last describes a partial
  //! transformation.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \note The values pointed at by \p Iterator instances must be unsigned
  //! integers, or this won't compile.
  template <typename Iterator>
  [[nodiscard]] bool is_ptransf(Iterator first, Iterator last, size_t deg) {
    static_assert(
        std::is_unsigned_v<std::decay_t<decltype(*std::declval<Iterator>())>>);
    return std::none_of(first, last, [&deg](auto val) {
      return val >= deg && val != UNDEFINED;
    });
  }

  //! \ingroup transf_group
  //!
  //! \brief Returns whether or not the argument describes a
  //! partial transformation.
  //!
  //! Defined in `is-transf.hpp`.
  //!
  //! This function calls the 3-argument version of \ref is_ptransf where the
  //! 3rd argument \c deg is set to `std::distance(first, last)`.
  //!
  //! \tparam Iterator the type of the iterators \p first and \p last.
  //!
  //! \param first iterator pointing a the start of the possible partial
  //! transformation. \param last iterator pointing one beyond the end of the
  //! possible partial transformation.
  //!
  //! \returns Whether or not \p first to \p last describes a partial
  //! transformation.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \note The values pointed at by \p Iterator instances must be unsigned
  //! integers, or this won't compile.
  template <typename Iterator>
  [[nodiscard]] bool is_ptransf(Iterator first, Iterator last) {
    return is_ptransf(first, last, std::distance(first, last));
  }

  //! \ingroup transf_group
  //!
  //! \brief Returns whether or not the argument describes a transformation.
  //!
  //! Defined in `is-transf.hpp`.
  //!
  //! This function returns `true` if every value in the range from \p first to
  //! \p last is less than \p deg; and `false` otherwise.
  //!
  //! \tparam Iterator the type of the iterators \p first and \p last.
  //!
  //! \param first iterator pointing a the start of the possible transformation.
  //!
  //! \param last iterator pointing one beyond the end of the possible
  //! transformation.
  //!
  //! \param deg the degree.
  //!
  //! \returns Whether or not \p first to \p last describes a transformation.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \note The values pointed at by \p Iterator instances must be unsigned
  //! integers, or this won't compile.
  template <typename Iterator>
  [[nodiscard]] bool is_transf(Iterator first, Iterator last, size_t deg) {
    static_assert(
        std::is_unsigned_v<std::decay_t<decltype(*std::declval<Iterator>())>>);
    return std::none_of(first, last, [&deg](auto val) { return val >= deg; });
  }

  //! \ingroup transf_group
  //!
  //! \brief Returns whether or not the argument describes a transformation.
  //!
  //! Defined in `is-transf.hpp`.
  //!
  //! This function calls the 3-argument version of \ref is_ptransf where the
  //! 3rd argument \c deg is set to `std::distance(first, last)`.
  //!
  //! \tparam Iterator the type of the iterators \p first and \p last.
  //!
  //! \param first iterator pointing a the start of the possible
  //! transformation.
  //! \param last iterator pointing one beyond the end of the
  //! possible transformation.
  //!
  //! \returns Whether or not \p first to \p last describes a transformation.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \note The values pointed at by \p Iterator instances must be unsigned
  //! integers, or this won't compile.
  template <typename Iterator>
  [[nodiscard]] bool is_transf(Iterator first, Iterator last) {
    return is_transf(first, last, std::distance(first, last));
  }

  //! \ingroup transf_group
  //!
  //! \brief Returns whether or not the argument describes a partial
  //! permutation.
  //!
  //! Defined in `is-transf.hpp`.
  //!
  //! This function returns `true` if \p first to \p last describes an
  //! injective partial transformation.
  //!
  //! \tparam Iterator the type of the iterators \p first and \p last.
  //!
  //! \param first iterator pointing a the start of the possible partial
  //! permutation.
  //!
  //! \param last iterator pointing one beyond the end of the
  //! possible partial permutation.
  //!
  //! \param deg the degree.
  //!
  //! \returns Whether or not \p first to \p last describes a partial
  //! permutation.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \note The values pointed at by \p Iterator instances must be unsigned
  //! integers, or this won't compile.
  template <typename Iterator>
  [[nodiscard]] bool is_pperm(Iterator first, Iterator last, size_t deg) {
    return is_ptransf(first, last, deg) && !detail::has_duplicates(first, last);
  }

  //! \ingroup transf_group
  //!
  //! \brief Returns whether or not the argument describes a partial
  //! permutation.
  //!
  //! Defined in `is-transf.hpp`.
  //!
  //! This function calls the 3-argument version of \ref is_pperm where the
  //! 3rd argument \c deg is set to `std::distance(first, last)`.
  //!
  //! \tparam Iterator the type of the iterators \p first and \p last.
  //!
  //! \param first iterator pointing a the start of the possible
  //! partial permutation.
  //!
  //! \param last iterator pointing one beyond the end of the
  //! possible partial permutation.
  //!
  //! \returns Whether or not \p first to \p last describes a partial
  //! permutation.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \note The values pointed at by \p Iterator instances must be unsigned
  //! integers, or this won't compile.
  template <typename Iterator>
  [[nodiscard]] bool is_pperm(Iterator first, Iterator last) {
    return is_pperm(first, last, std::distance(first, last));
  }

  //! \ingroup transf_group
  //!
  //! \brief Returns whether or not the argument describes a permutation.
  //!
  //! Defined in `is-transf.hpp`.
  //!
  //! This function returns `true` if \p first to \p last describes an
  //! injective permutation.
  //!
  //! \param first iterator pointing a the start of the possible
  //! permutation.
  //!
  //! \param last iterator pointing one beyond the end of the
  //! possible permutation.
  //!
  //! \param deg the degree.
  //!
  //! \returns Whether or not \p first to \p last describes a permutation.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \note The values pointed at by \p Iterator instances must be unsigned
  //! integers, or this won't compile.
  template <typename Iterator>
  [[nodiscard]] bool is_perm(Iterator first, Iterator last, size_t deg) {
    return is_transf(first, last, deg) && !detail::has_duplicates(first, last);
  }

  //! \ingroup transf_group
  //!
  //! \brief Returns whether or not the argument describes a permutation.
  //!
  //! Defined in `is-transf.hpp`.
  //!
  //! This function calls the 3-argument version of \ref is_perm where the
  //! 3rd argument \c deg is set to `std::distance(first, last)`.
  //!
  //! \tparam Iterator the type of the iterators \p first and \p last.
  //!
  //! \param first iterator pointing a the start of the possible
  //! permutation.
  //!
  //! \param last iterator pointing one beyond the end of the
  //! possible permutation.
  //!
  //! \returns Whether or not \p first to \p last describes a permutation.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \note The values pointed at by \p Iterator instances must be unsigned
  //! integers, or this won't compile.
  template <typename Iterator>
  [[nodiscard]] bool is_perm(Iterator first, Iterator last) {
    return is_perm(first, last, std::distance(first, last));
  }
}  // namespace libsemigroups

#include "is-transf.tpp"

#endif  // LIBSEMIGROUPS_IS_TRANSF_HPP_
