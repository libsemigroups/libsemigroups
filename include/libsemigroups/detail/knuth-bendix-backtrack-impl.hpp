//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2026 Joseph Edwards
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

// This file contains the declaration of the KnuthBendixBacktrack class

// TODO:
// * Reporting
// * Replace RewritingSystemBacktrack with something that derives from
//   RewritingSystemBase

#ifndef LIBSEMIGROUPS_DETAIL_KNUTH_BENDIX_BACKTRACK_IMPL_HPP_
#define LIBSEMIGROUPS_DETAIL_KNUTH_BENDIX_BACKTRACK_IMPL_HPP_

#include <cstddef>        // for ptrdiff_t
#include <iterator>       // for forward_iterator_tag, distance
#include <unordered_map>  // for unordered map
#include <utility>        // for pair, move
#include <vector>         // for vector

#include "libsemigroups/presentation.hpp"  // for Presentaiton

#include "rules.hpp"  // for Rule::native_word_type

namespace libsemigroups {
  namespace detail {

    enum class Orientation : bool { original, flipped };

    class RewritingSystemBacktrack {
     public:
      using native_word_type = typename Rule::native_word_type;
      using rule_container_type =
          typename std::unordered_map<native_word_type, native_word_type>;

      // TODO(0): Change this to use a stack, so rules can be popped easier?
      explicit RewritingSystemBacktrack(size_t max_num_rules)
          : _lookup{max_num_rules}, _rules{} {
        _rules.reserve(max_num_rules);
      }

      void add_rule(native_word_type const& lhs,
                    native_word_type const& rhs,
                    size_t                  index);

      void pop_rule(size_t index);

      bool rewrite(native_word_type& word, size_t max_rewrite_depth) const;

      rule_container_type const& rules() const noexcept {
        return _rules;
      }

     private:
      std::vector<native_word_type> _lookup;
      rule_container_type           _rules;
    };

    // TODO: Make max_number_of_rules a template parameter?
    class KnuthBendixBacktrack {
     public:
      //////////////////////////////////////////////////////////////////////
      // Public aliases
      //////////////////////////////////////////////////////////////////////

      using native_word_type =
          typename RewritingSystemBacktrack::native_word_type;
      using value_type        = Presentation<native_word_type>;
      using reference         = value_type&;
      using const_reference   = value_type const&;
      using difference_type   = std::ptrdiff_t;
      using size_type         = size_t;
      using const_pointer     = value_type const*;
      using pointer           = value_type*;
      using iterator_category = std::forward_iterator_tag;

      //////////////////////////////////////////////////////////////////////
      // Constructors
      //////////////////////////////////////////////////////////////////////

      // Default constructed KnuthBendixBacktrack represents the end iterator
      KnuthBendixBacktrack();

      KnuthBendixBacktrack(KnuthBendixBacktrack const&)            = default;
      KnuthBendixBacktrack(KnuthBendixBacktrack&&)                 = default;
      KnuthBendixBacktrack& operator=(KnuthBendixBacktrack const&) = default;
      KnuthBendixBacktrack& operator=(KnuthBendixBacktrack&&)      = default;

      ~KnuthBendixBacktrack() = default;

      KnuthBendixBacktrack(Presentation<native_word_type> const& p,
                           size_t                                max_depth,
                           size_t max_queue_size);

      //////////////////////////////////////////////////////////////////////
      // Public member functions
      //////////////////////////////////////////////////////////////////////

      // This is a weak comparison that is mainly intended to check if an
      // iterator is at the end
      [[nodiscard]] bool
      operator==(KnuthBendixBacktrack const& that) const noexcept {
        return _rule_index == that._rule_index;
      }

      [[nodiscard]] bool
      operator!=(KnuthBendixBacktrack const& that) const noexcept {
        return !(operator==(that));
      }

      [[nodiscard]] const_reference operator*() const noexcept {
        return _output_presentation;
      }

      [[nodiscard]] const_pointer operator->() const noexcept {
        return &(_output_presentation);
      }

      // prefix increment
      KnuthBendixBacktrack const& operator++();

      // postfix - not noexcept because the prefix increment isn't
      KnuthBendixBacktrack operator++(int) {
        return detail::default_postfix_increment<KnuthBendixBacktrack>(*this);
      }

     private:
      bool backtrack();

      bool rewrite_pair(native_word_type& lhs, native_word_type& rhs) const;

      bool add_pending_rule(native_word_type&& lhs, native_word_type&& rhs);

      bool process_single_overlap(native_word_type const&                u_lhs,
                                  native_word_type const&                u_rhs,
                                  native_word_type const&                v_lhs,
                                  native_word_type const&                v_rhs,
                                  native_word_type::const_iterator const it);

      bool
      process_subword_overlap(native_word_type const&                u_lhs,
                              native_word_type const&                u_rhs,
                              native_word_type const&                v_lhs,
                              native_word_type const&                v_rhs,
                              native_word_type::const_iterator const start);

      bool process_onesided_overlaps(native_word_type const& u_lhs,
                                     native_word_type const& u_rhs,
                                     native_word_type const& v_lhs,
                                     native_word_type const& v_rhs);

      // Find and process overlaps of the form
      // 1. U on the left:
      //    U = AB -> X
      //    V = BC -> Y
      //    XC <- ABC -> AY
      // 2. U on the right:
      //    U = BC -> X
      //    V = AB -> Y
      //    YB <- ABC -> AX
      // 3. U a subword:
      //    U = B -> X
      //    V = ABC -> Y
      //    Y <- ABC -> AXC
      // We don't need to check that V is a subword because U is already reduced
      // with respect to V.
      //
      // Return false if, at any stage, rewriting fails
      bool process_overlaps(native_word_type const& u_lhs,
                            native_word_type const& u_rhs,
                            native_word_type const& v_lhs,
                            native_word_type const& v_rhs);

      void set_output_presentation();

      size_t                         _max_queue_size;
      size_t                         _max_rewriting_depth;
      std::vector<Orientation>       _orientations;
      Presentation<native_word_type> _output_presentation;
      size_t                         _rule_index;
      std::vector<std::pair<native_word_type, native_word_type>> _rules;
      RewritingSystemBacktrack                                   _rws;
      bool                                   _should_backtrack;
      std::vector<std::pair<size_t, size_t>> _state_history;
    };

    inline KnuthBendixBacktrack begin_knuth_bendix_backtrack(
        Presentation<KnuthBendixBacktrack::native_word_type> const& p,
        size_t                                                      max_depth,
        size_t max_queue_size) {
      return KnuthBendixBacktrack(p, max_depth, max_queue_size);
    }

    inline KnuthBendixBacktrack end_knuth_bendix_backtrack(
        Presentation<KnuthBendixBacktrack::native_word_type> const&,
        size_t,
        size_t) {
      return KnuthBendixBacktrack();
    }
  }  // namespace detail
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_DETAIL_KNUTH_BENDIX_BACKTRACK_IMPL_HPP_
