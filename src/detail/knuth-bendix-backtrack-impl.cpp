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

// This file contains the implementation of the KnuthBendixBacktrack class

#include "libsemigroups/detail/knuth-bendix-backtrack-impl.hpp"

#include <algorithm>  // for equal, swap, fill, min

#include "libsemigroups/exception.hpp"  // for LIBSEMIGROUPS_ASSERT

namespace libsemigroups {
  namespace detail {

    //////////////////////////////////////////////////////////////////////
    // RewritingSystemBacktrack
    //////////////////////////////////////////////////////////////////////

    bool RewritingSystemBacktrack::rewrite(native_word_type& word,
                                           size_t max_rewrite_depth) const {
      if (word.size() == 0) {
        return true;
      }

      // position of the start of the unread suffix of the input word
      size_t pos = 0;

      size_t num_rewrites = 0;

      while (pos < word.size()) {
        LIBSEMIGROUPS_ASSERT(pos >= 0);
        ++pos;
        for (auto start = word.begin(); start < word.begin() + pos; ++start) {
          // This is linear in the size of the stack, so using a map might have
          // better complexity; however, the size of the stack is bounded above
          // by the depth of the backtrack which will always be quite small, so
          // this seems to actually be quicker.
          auto it = std::find_if(
              _rule_stack.begin(), _rule_stack.end(), [&](auto const& rule) {
                return std::equal(rule.first.begin(),
                                  rule.first.end(),
                                  start,
                                  word.begin() + pos);
              });
          if (it != _rule_stack.end()) {
            if (num_rewrites == max_rewrite_depth) {
              return false;
            }
            LIBSEMIGROUPS_ASSERT(std::equal(start,
                                            word.begin() + pos,
                                            it->first.cbegin(),
                                            it->first.cend()));
            size_t diff = it->first.size();
            pos -= diff;
            word.erase(word.begin() + pos, word.begin() + pos + diff);
            word.insert(
                word.begin() + pos, it->second.begin(), it->second.end());
            ++num_rewrites;
            break;
          }
        }
      }

      return true;
    }

    //////////////////////////////////////////////////////////////////////
    // KnuthBendixBacktrack - Constructors
    //////////////////////////////////////////////////////////////////////

    KnuthBendixBacktrack::KnuthBendixBacktrack()
        : _max_queue_size{},
          _max_rewriting_depth{},
          _orientations{},
          _output_presentation{},
          _rule_index{UNDEFINED},
          _rules{},
          _rws{0},
          _should_backtrack{},
          _state_history{} {};

    KnuthBendixBacktrack::KnuthBendixBacktrack(
        Presentation<native_word_type> const& p,
        size_t                                max_depth,
        size_t                                max_queue_size)
        : _max_queue_size{max_queue_size},
          _max_rewriting_depth{max_depth},
          _orientations{p.rules.size() / 2, Orientation::original},
          _output_presentation{p},
          _rule_index{0},
          _rules{},
          _rws{max_queue_size},
          _should_backtrack{false},
          _state_history{} {
      LIBSEMIGROUPS_ASSERT(p.rules.size() / 2 <= _max_queue_size);
      _rules.reserve(_max_queue_size);
      _orientations.reserve(_max_queue_size);
      _state_history.reserve(_max_queue_size);
      for (size_t i = 0; i < p.rules.size(); i += 2) {
        _rules.emplace_back(p.rules[i], p.rules[i + 1]);
      }
      operator++();
    }

    //////////////////////////////////////////////////////////////////////
    // KnuthBendixBacktrack - Public member functions
    //////////////////////////////////////////////////////////////////////

    // prefix increment
    KnuthBendixBacktrack const& KnuthBendixBacktrack::operator++() {
      if (_should_backtrack) {
        _should_backtrack = false;
        if (!backtrack()) {
          return *this;
        }
      }
      while (_rule_index < _rules.size()) {
        auto [new_lhs, new_rhs] = _rules[_rule_index];

        if (!rewrite_pair(new_lhs, new_rhs)) {
          if (!backtrack()) {
            return *this;
          } else {
            continue;
          }
        }

        if (new_lhs != new_rhs) {
          _rws.add_rule(new_lhs, new_rhs);
          _state_history.emplace_back(_rule_index, _rules.size());

          // Find critical pairs
          bool processed_overlaps = true;

          for (auto const& [old_lhs, old_rhs] : _rws.rules()) {
            processed_overlaps
                = process_overlaps(new_lhs, new_rhs, old_lhs, old_rhs);
            if (!processed_overlaps) {
              if (!backtrack()) {
                return *this;
              } else {
                break;
              }
            }
          }
          // If it was not possible to process the overlaps and backtracking
          // didn't fail
          if (!processed_overlaps) {
            continue;
          }
        }

        _rule_index++;
      }
      _should_backtrack = true;
      set_output_presentation();
      return *this;
    }

    //////////////////////////////////////////////////////////////////////
    // KnuthBendixBacktrack - Private member functions
    //////////////////////////////////////////////////////////////////////

    bool KnuthBendixBacktrack::backtrack() {
      if (_state_history.empty()) {
        _rule_index = UNDEFINED;
        return false;
      }
      std::pair<size_t, size_t> last_unflipped_rule_state;
      bool                      found_unflipped_rule = false;

      // Find the last rule that we have processed and only tried one way round.
      while (!_state_history.empty()) {
        last_unflipped_rule_state = _state_history.back();
        _state_history.pop_back();
        _rws.pop_rule();
        _rule_index = last_unflipped_rule_state.first;
        if (_orientations[_rule_index] == Orientation::original) {
          found_unflipped_rule = true;
          break;
        }
      }

      // All rules have been tried both ways round
      if (!found_unflipped_rule) {
        _rule_index = UNDEFINED;
        return false;
      }

      // Flip the rule
      LIBSEMIGROUPS_ASSERT(_orientations[_rule_index] == Orientation::original);
      _orientations[_rule_index] = Orientation::flipped;
      std::swap(_rules[_rule_index].first, _rules[_rule_index].second);

      // Reset to the state to be as if we are just about to process the newly
      // flipped rule
      _rules.resize(last_unflipped_rule_state.second);
      _orientations.resize(last_unflipped_rule_state.second);
      std::fill(_orientations.begin() + _rule_index + 1,
                _orientations.end(),
                Orientation::original);
      return true;
    }

    bool KnuthBendixBacktrack::rewrite_pair(native_word_type& lhs,
                                            native_word_type& rhs) const {
      if (_rws.rewrite(lhs, _max_rewriting_depth)
          && _rws.rewrite(rhs, _max_rewriting_depth)) {
        return true;
      }

      return false;
    }

    bool KnuthBendixBacktrack::add_pending_rule(native_word_type&& lhs,
                                                native_word_type&& rhs) {
      if (lhs == rhs) {
        return true;
      }
      if (_rules.size() >= _max_queue_size) {
        return false;
      }

      _rules.emplace_back(std::move(lhs), std::move(rhs));
      _orientations.emplace_back(Orientation::original);
      return true;
    }

    bool KnuthBendixBacktrack::process_single_overlap(
        native_word_type const&                u_lhs,
        native_word_type const&                u_rhs,
        native_word_type const&                v_lhs,
        native_word_type const&                v_rhs,
        native_word_type::const_iterator const it) {
      // Add rules of the form AY -> XC where:
      //    U = AB -> X
      //    V = BC -> Y
      // TODO(0): multiview?
      native_word_type x(u_lhs.cbegin(), it);  // A
      x.append(v_rhs.cbegin(), v_rhs.cend());  // Y

      native_word_type y(u_rhs.cbegin(), u_rhs.cend());  // X
      y.append(v_lhs.cbegin() + std::distance(it, u_lhs.cend()),
               v_lhs.cend());  // C
      // TODO(1) This could be augmented by a call to some "oracle" function
      // that can tell us if we know our rewriting system will/won't terminate.
      // This could be some incremental version of du_narendran_rusinowitch.

      if (!rewrite_pair(x, y)) {
        return false;
      }
      return add_pending_rule(std::move(x), std::move(y));
    }

    bool KnuthBendixBacktrack::process_subword_overlap(
        native_word_type const&                u_lhs,
        native_word_type const&                u_rhs,
        native_word_type const&                v_lhs,
        native_word_type const&                v_rhs,
        native_word_type::const_iterator const start) {
      // Add rules of the form Y -> AXC where:
      //    U = B -> X
      //    V = ABC -> Y
      native_word_type x(v_rhs.cbegin(), v_rhs.cend());  // Y

      native_word_type y(v_lhs.cbegin(), start);     // A
      y.append(u_rhs.cbegin(), u_rhs.cend());        // X
      y.append(start + u_lhs.size(), v_lhs.cend());  // C

      if (!rewrite_pair(x, y)) {
        return false;
      }
      return add_pending_rule(std::move(x), std::move(y));
    }

    bool KnuthBendixBacktrack::process_onesided_overlaps(
        native_word_type const& u_lhs,
        native_word_type const& u_rhs,
        native_word_type const& v_lhs,
        native_word_type const& v_rhs) {
      // Find overlaps of the form:
      //    U = AB -> X
      //    V = BC -> Y
      // where
      //    ABC -> AY, and
      //    ABC -> XC.
      auto const lower_limit
          = u_lhs.cend() - std::min(u_lhs.size(), v_lhs.size());

      for (auto it = u_lhs.cend() - 1; it > lower_limit; --it) {
        if (std::equal(it, u_lhs.cend(), v_lhs.cbegin())) {
          if (!process_single_overlap(u_lhs, u_rhs, v_lhs, v_rhs, it)) {
            return false;
          }
        }
      }
      return true;
    }

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
    bool KnuthBendixBacktrack::process_overlaps(native_word_type const& u_lhs,
                                                native_word_type const& u_rhs,
                                                native_word_type const& v_lhs,
                                                native_word_type const& v_rhs) {
      // U on the left
      if (!process_onesided_overlaps(u_lhs, u_rhs, v_lhs, v_rhs)) {
        return false;
      }

      if (u_lhs != v_lhs && u_rhs != v_rhs) {
        // U on the right, only if U and V are different
        if (!process_onesided_overlaps(v_lhs, v_rhs, u_lhs, u_rhs)) {
          return false;
        }
      }

      // U a subword
      if (u_lhs.size() < v_lhs.size()) {
        size_t size_difference = v_lhs.size() - u_lhs.size();
        for (auto start = v_lhs.begin();
             start <= v_lhs.begin() + size_difference;
             ++start) {
          if (std::equal(u_lhs.cbegin(), u_lhs.cend(), start)) {
            if (!process_subword_overlap(u_lhs, u_rhs, v_lhs, v_rhs, start)) {
              return false;
            }
          }
        }
      }
      return true;
    }

    void KnuthBendixBacktrack::set_output_presentation() {
      _output_presentation.rules.clear();
      for (auto const& [lhs, rhs] : _rws.rules()) {
        presentation::add_rule_no_checks(_output_presentation, lhs, rhs);
      }
    }

  }  // namespace detail
}  // namespace libsemigroups
