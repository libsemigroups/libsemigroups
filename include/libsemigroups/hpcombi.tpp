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

// This file contains implementations of specializations of the class adapters
// in adapters.hpp for the element types in HPCombi.

namespace libsemigroups {

  namespace detail {
    template <typename Container>
    void throw_if_too_big(Container const& cont) {
      if (cont.size() > 16) {
        LIBSEMIGROUPS_EXCEPTION("the argument must have size at "
                                "most 16, but found {}",
                                cont.size());
      }
    }

    template <typename Thing, typename Container>
    Thing make_hpcombi_ptransf(Container const& cont, uint8_t pad) {
      LIBSEMIGROUPS_ASSERT(cont.size() <= 16);
      Thing  result;
      size_t i = 0;
      for (; i < cont.size(); ++i) {
        // Next line is so that this function works with initializer_list
        result[i] = *(cont.begin() + i);
      }
      for (; i < 16; ++i) {
        result[i] = pad;
      }
      return result;
    }
  }  // namespace detail

  template <typename Return, typename Container>
  enable_if_is_same<Return, HPCombi::PTransf16> make(Container&& cont) {
    detail::throw_if_too_big(cont);
    detail::throw_if_not_ptransf(std::begin(cont), std::end(cont), 16);
    auto result = detail::make_hpcombi_ptransf<HPCombi::PTransf16>(
        std::forward<Container>(cont), 255);
    LIBSEMIGROUPS_ASSERT(result.validate());
    return result;
  }

  template <typename Return>
  enable_if_is_same<Return, HPCombi::PTransf16>
  make(std::vector<uint8_t> const& dom,
       std::vector<uint8_t> const& ran,
       size_t                      deg) {
    detail::throw_if_not_ptransf(
        dom.cbegin(), dom.cend(), ran.cbegin(), ran.cend(), 16);
    HPCombi::PTransf16 result(dom, ran, deg);
    LIBSEMIGROUPS_ASSERT(result.validate());
    return result;
  }

  template <typename Return, typename Container>
  enable_if_is_same<Return, HPCombi::Transf16> make(Container&& cont) {
    detail::throw_if_too_big(cont);
    detail::throw_if_not_transf(std::begin(cont), std::end(cont), 16);
    auto pad    = cont.size() == 0 ? 0 : *(std::begin(cont) + cont.size() - 1);
    auto result = detail::make_hpcombi_ptransf<HPCombi::Transf16>(
        std::forward<Container>(cont), pad);
    LIBSEMIGROUPS_ASSERT(result.validate());
    return result;
  }

  template <typename Return, typename Container>
  enable_if_is_same<Return, HPCombi::Perm16> make(Container&& cont) {
    detail::throw_if_too_big(cont);
    detail::throw_if_not_perm(std::begin(cont), std::end(cont), 16);
    HPCombi::Perm16 result;
    size_t          i = 0;
    for (; i < cont.size(); ++i) {
      // Next line is so that this function works with initializer_list
      result[i] = *(cont.begin() + i);
    }
    std::iota(result.begin() + cont.size(), result.end(), cont.size());
    LIBSEMIGROUPS_ASSERT(result.validate());
    return result;
  }

  template <typename Return, typename Container>
  enable_if_is_same<Return, HPCombi::PPerm16> make(Container&& cont) {
    detail::throw_if_too_big(cont);
    detail::throw_if_not_pperm(std::begin(cont), std::end(cont), 16);
    auto result = detail::make_hpcombi_ptransf<HPCombi::PPerm16>(cont, 255);
    LIBSEMIGROUPS_ASSERT(result.validate());
    return result;
  }

  template <typename Return>
  enable_if_is_same<Return, HPCombi::PPerm16>
  make(std::vector<uint8_t> const& dom,
       std::vector<uint8_t> const& ran,
       size_t                      deg) {
    detail::throw_if_not_pperm(
        dom.begin(), dom.end(), ran.begin(), ran.end(), 16);
    HPCombi::PPerm16 result(dom, ran, deg);
    LIBSEMIGROUPS_ASSERT(result.validate());
    return result;
  }

}  // namespace libsemigroups
