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

// This file contains some functions for checking whether or not a container
// defines a partial transformation, full transformation, partial permutation,
// or permutation. These are used in transf.hpp for argument checking, and also
// in hpcombi.hpp for the same.

namespace libsemigroups {

  namespace detail {
    template <typename Iterator>
    std::pair<Iterator, size_t> find_duplicates(
        Iterator                                                    first,
        Iterator                                                    last,
        std::unordered_map<std::decay_t<decltype(*first)>, size_t>& seen) {
      seen.clear();
      for (auto it = first; it != last; ++it) {
        if (*it != UNDEFINED) {
          auto [prev_it, inserted] = seen.emplace(*it, seen.size());
          if (!inserted) {
            return std::pair(it, prev_it->second);
          }
        }
      }
      return std::pair(last, seen.size());
    }

    template <typename Iterator>
    void throw_if_duplicates(
        Iterator                                                    first,
        Iterator                                                    last,
        std::unordered_map<std::decay_t<decltype(*first)>, size_t>& seen,
        std::string_view                                            where) {
      auto [it, pos] = find_duplicates(first, last, seen);
      if (it != last) {
        LIBSEMIGROUPS_EXCEPTION(
            "duplicate {} value, found {} in position {}, first "
            "occurrence in position {}",
            where,
            to_printable(*it),
            std::distance(first, it),
            pos);
      }
    }

    template <typename Iterator, typename Func>
    void throw_if_value_out_of_range(Iterator         first,
                                     Iterator         last,
                                     Func&&           func,
                                     std::string_view where) {
      static_assert(std::is_unsigned_v<
                    std::decay_t<decltype(*std::declval<Iterator>())>>);
      auto const M  = std::distance(first, last);
      auto const it = std::find_if(first, last, std::forward<Func>(func));
      if (it != last) {
        LIBSEMIGROUPS_EXCEPTION("{} value out of bounds, expected value in "
                                "[0, {}), found {} in position {}",
                                where,
                                M,
                                *it,
                                std::distance(first, it));
      }
    }

    template <typename Iterator>
    void throw_if_not_ptransf(Iterator first, Iterator last, size_t deg) {
      throw_if_value_out_of_range(
          first,
          last,
          [&deg](auto val) { return val >= deg && val != UNDEFINED; },
          std::string_view("image"));
    }

    template <typename Iterator>
    void throw_if_not_ptransf(Iterator dom_first,
                              Iterator dom_last,
                              Iterator img_first,
                              Iterator img_last,
                              size_t   deg) {
      using Scalar = std::decay_t<decltype(*dom_first)>;
      static_assert(std::is_same_v<Scalar, std::decay_t<decltype(*img_first)>>);

      auto dom_size = std::distance(dom_first, dom_last);
      auto img_size = std::distance(img_first, img_last);
      if (dom_size != img_size) {
        // The next 2 checks just verify that we can safely run the
        // constructor that uses *this[dom[i]] = im[i] for i = 0, ...,
        // dom.size() - 1.
        LIBSEMIGROUPS_EXCEPTION("domain and image size mismatch, domain has "
                                "size {} but image has size {}",
                                dom_size,
                                img_size);
      }
      throw_if_value_out_of_range(
          dom_first,
          dom_last,
          [&deg](auto val) { return val >= deg && val != UNDEFINED; },
          "domain");
      throw_if_value_out_of_range(
          img_first,
          img_last,
          [&deg](auto val) { return val >= deg && val != UNDEFINED; },
          "domain");

      throw_if_duplicates(dom_first, dom_last, "domain");

      auto it = std::find(dom_first, dom_last, UNDEFINED);
      if (it != dom_last) {
        LIBSEMIGROUPS_EXCEPTION(
            "the 1st argument (domain) must not contain UNDEFINED, but found "
            "UNDEFINED (= {}) in position {}",
            static_cast<Scalar>(UNDEFINED),
            std::distance(dom_first, it));
      }
      it = std::find(img_first, img_last, UNDEFINED);
      if (it != img_last) {
        LIBSEMIGROUPS_EXCEPTION(
            "the 2nd argument (image) must not contain UNDEFINED, but found "
            "UNDEFINED (= {}) in position {}",
            static_cast<Scalar>(UNDEFINED),
            std::distance(img_first, it));
      }
      // NOTE: it is ok if deg >= max. value of Scalar, but then necessary that
      // the values in <dom> and <ran> are less than (max. value of Scalar) - 1.
    }

    template <typename Iterator>
    void throw_if_not_transf(Iterator first, Iterator last, size_t deg) {
      static_assert(std::is_unsigned_v<
                    std::decay_t<decltype(*std::declval<Iterator>())>>);
      throw_if_value_out_of_range(
          first,
          last,
          [&deg](auto val) { return val >= deg; },
          std::string_view("image"));
    }

  }  // namespace detail
}  // namespace libsemigroups
