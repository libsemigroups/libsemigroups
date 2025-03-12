//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2024-2025 James D. Mitchell
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

// This file contains an implementation of a range object for producing normal
// forms for a KnuthBendixImpl object.

#ifndef LIBSEMIGROUPS_DETAIL_KNUTH_BENDIX_NF_HPP_
#define LIBSEMIGROUPS_DETAIL_KNUTH_BENDIX_NF_HPP_

#include <cstdint>  // for uint32_t

#include "libsemigroups/paths.hpp"            // for Paths
#include "libsemigroups/to-froidure-pin.hpp"  // for to_froidure_pin
#include "libsemigroups/types.hpp"            // for word_type

namespace libsemigroups {

  template <typename Word, typename Rewriter, typename ReductionOrder>
  class KnuthBendix;

  namespace detail {

    template <typename Word, typename Rewriter, typename ReductionOrder>
    class KnuthBendixNormalFormRange : public Paths<uint32_t> {
      using Paths_ = Paths<uint32_t>;

      mutable Word                                 _current;
      KnuthBendix<Word, Rewriter, ReductionOrder>* _kb;

     public:
      using size_type   = typename Paths_::size_type;
      using output_type = Word const&;

      explicit KnuthBendixNormalFormRange(
          KnuthBendix<Word, Rewriter, ReductionOrder>& kb)
          : Paths(kb.gilman_graph()), _current(), _kb(&kb) {
        // It's possible that the gilman graph is empty, so the call to
        // source_no_checks(0) is technically invalid, but nothing goes wrong,
        // so we just go with it. This is slightly smelly.
        Paths_::source_no_checks(0);
        if (!kb.presentation().contains_empty_word()) {
          Paths_::next();
        }
      }

      KnuthBendixNormalFormRange();
      KnuthBendixNormalFormRange(KnuthBendixNormalFormRange const&);
      KnuthBendixNormalFormRange(KnuthBendixNormalFormRange&&);
      KnuthBendixNormalFormRange& operator=(KnuthBendixNormalFormRange const&);
      KnuthBendixNormalFormRange& operator=(KnuthBendixNormalFormRange&&);

      ~KnuthBendixNormalFormRange();

      output_type get() const {
        word_type const& w = Paths_::get();
        _current.clear();
        for (auto c : w) {
          _current.push_back(_kb->presentation().letter_no_checks(c));
        }
        return _current;
      }

      KnuthBendixNormalFormRange& min(size_type val) noexcept {
        Paths_::min(val);
        return *this;
      }

      KnuthBendixNormalFormRange& max(size_type val) noexcept {
        Paths_::max(val);
        return *this;
      }

      using Paths_::at_end;
      using Paths_::count;
      using Paths_::max;
      using Paths_::min;
      using Paths_::next;
      using Paths_::size_hint;

      static constexpr bool is_finite     = true;  // this isn't always true!
      static constexpr bool is_idempotent = true;
    };  // class KnuthBendixNormalFormRange

    template <typename Word, typename Rewriter, typename ReductionOrder>
    KnuthBendixNormalFormRange<Word, Rewriter, ReductionOrder>::
        KnuthBendixNormalFormRange()
        = default;

    template <typename Word, typename Rewriter, typename ReductionOrder>
    KnuthBendixNormalFormRange<Word, Rewriter, ReductionOrder>::
        KnuthBendixNormalFormRange(KnuthBendixNormalFormRange const&)
        = default;

    template <typename Word, typename Rewriter, typename ReductionOrder>
    KnuthBendixNormalFormRange<Word, Rewriter, ReductionOrder>::
        KnuthBendixNormalFormRange(KnuthBendixNormalFormRange&&)
        = default;

    template <typename Word, typename Rewriter, typename ReductionOrder>
    KnuthBendixNormalFormRange<Word, Rewriter, ReductionOrder>&
    KnuthBendixNormalFormRange<Word, Rewriter, ReductionOrder>::operator=(
        KnuthBendixNormalFormRange const&)
        = default;

    template <typename Word, typename Rewriter, typename ReductionOrder>
    KnuthBendixNormalFormRange<Word, Rewriter, ReductionOrder>&
    KnuthBendixNormalFormRange<Word, Rewriter, ReductionOrder>::operator=(
        KnuthBendixNormalFormRange&&)
        = default;

    template <typename Word, typename Rewriter, typename ReductionOrder>
    KnuthBendixNormalFormRange<Word, Rewriter, ReductionOrder>::
        ~KnuthBendixNormalFormRange()
        = default;
  }  // namespace detail
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_DETAIL_KNUTH_BENDIX_NF_HPP_
