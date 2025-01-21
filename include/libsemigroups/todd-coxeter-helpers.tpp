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

// This file contains out-of-line ToddCoxeter<Word> helper function templates.

namespace libsemigroups {
  namespace todd_coxeter {

    ////////////////////////////////////////////////////////////////////////
    // Possible future interface helpers - index -> word
    ////////////////////////////////////////////////////////////////////////

    template <typename Word>
    Word current_word_of_no_checks(ToddCoxeter<Word>& tc, index_type i) {
      Word result;
      tc.current_word_of_no_checks(std::back_inserter(result), i);
      return result;
    }

    template <typename Word>
    Word current_word_of(ToddCoxeter<Word>& tc, index_type i) {
      Word result;
      tc.current_word_of(std::back_inserter(result), i);
      return result;
    }

    template <typename Word>
    Word word_of_no_checks(ToddCoxeter<Word>& tc, index_type i) {
      Word result;
      tc.word_of_no_checks(std::back_inserter(result), i);
      return result;
    }

    template <typename Word>
    Word word_of(ToddCoxeter<Word>& tc, index_type i) {
      Word result;
      tc.word_of(std::back_inserter(result), i);
      return result;
    }

    template <typename Word>
    [[nodiscard]] tril is_non_trivial(ToddCoxeter<Word>&        tc,
                                      size_t                    tries,
                                      std::chrono::milliseconds try_for,
                                      float                     threshold) {
      using detail::node_managed_graph::random_active_node;

      if (is_obviously_infinite(tc)) {
        return tril::TRUE;
      } else if (tc.finished()) {
        return tc.number_of_classes() == 1 ? tril::FALSE : tril::TRUE;
      }

      for (size_t try_ = 0; try_ < tries; ++try_) {
        report_default(
            "trying to show non-triviality: {} / {}\n", try_ + 1, tries);
        ToddCoxeterBase copy(tc);
        copy.save(true);
        while (!copy.finished()) {
          copy.run_for(try_for);
          size_t limit = copy.current_word_graph().number_of_nodes_active();
          while (copy.current_word_graph().number_of_nodes_active()
                     >= threshold * limit
                 && !copy.finished()) {
            auto  c1 = random_active_node(copy.current_word_graph());
            auto  c2 = random_active_node(copy.current_word_graph());
            auto& wg = const_cast<ToddCoxeterBase::word_graph_type&>(
                copy.current_word_graph());
            wg.merge_nodes_no_checks(c1, c2);
            wg.process_coincidences<detail::RegisterDefs>();
            wg.process_definitions();
            copy.run_for(try_for);
          }
        }
        if (copy.number_of_classes() > 1) {
          report_default("successfully showed non-triviality!\n");
          return tril::TRUE;
        }
      }
      report_default("failed to show non-triviality!\n");
      return tril::unknown;
    }

    ////////////////////////////////////////////////////////////////////////
    // Possible future interface helpers - redundant_rule
    ////////////////////////////////////////////////////////////////////////

    template <typename Word, typename Time>
    typename std::vector<Word>::const_iterator
    redundant_rule(Presentation<Word> const& p, Time t) {
      constexpr static congruence_kind twosided = congruence_kind::twosided;

      p.validate();
      Presentation<Word> q;
      q.alphabet(p.alphabet());
      q.contains_empty_word(p.contains_empty_word());
      ToddCoxeter<Word> tc;

      for (auto omit = p.rules.crbegin(); omit != p.rules.crend(); omit += 2) {
        q.rules.clear();
        q.rules.insert(q.rules.end(), p.rules.crbegin(), omit);
        q.rules.insert(q.rules.end(), omit + 2, p.rules.crend());
        tc.init(twosided, q);
        tc.run_for(t);
        auto index1 = current_index_of_no_checks(tc, *omit);
        auto index2 = current_index_of_no_checks(tc, *(omit + 1));
        if (index1 != UNDEFINED && index1 == index2) {
          return (omit + 1).base() - 1;
        }
      }
      return p.rules.cend();
    }

    ////////////////////////////////////////////////////////////////////////
    // Possible future interface helpers - first_equivalent_pair
    ////////////////////////////////////////////////////////////////////////

    // The next function is temporarily removed, to simplify the v3 release
    // TODO(1) range version
    // template <typename Iterator>
    // std::pair<Iterator, Iterator> first_equivalent_pair(ToddCoxeter<Word>&
    // tc,
    //                                                     Iterator     first,
    //                                                     Iterator     last) {
    //   std::unordered_map<ToddCoxeter<Word>::node_type, Iterator> map;
    //   size_t                                               index = 0;
    //   for (auto it = first; it != last; ++it, ++index) {
    //     auto [map_it, inserted]
    //         = map.emplace(todd_coxeter::index_of(tc, *it), it);
    //     if (!inserted) {
    //       return std::pair(map_it->second, it);
    //     }
    //   }
    //   return std::pair(last, last);
    // }

    ////////////////////////////////////////////////////////////////////////
    // Possible future interface helpers - is_traversal
    ////////////////////////////////////////////////////////////////////////

    // The next function is temporarily removed, to simplify the v3 release
    // template <typename Iterator>
    // bool is_traversal(ToddCoxeter<Word>& tc, Iterator first, Iterator last) {
    //   return first_equivalent_pair(tc, first, last) == std::pair(last, last);
    // }

  }  // namespace todd_coxeter

  namespace congruence_interface {
    template <typename Word>
    auto normal_forms(ToddCoxeter<Word>& tc) {
      // TODO(1) avoid allocations here.
      // To do this we'll have to make a custom range object that stores a
      // word_type _word that gets updated by calls to
      // tc.word_of_no_checks(std::back_inserter(_word), i);

      return rx::seq() | rx::take(tc.number_of_classes())
             | rx::transform([&tc](auto i) {
                 return todd_coxeter::word_of_no_checks(tc, i);
               });
    }

    template <typename Word, typename Range, typename>
    std::vector<std::vector<Word>> partition(ToddCoxeter<Word>& tc, Range r) {
      static_assert(
          std::is_same_v<Word, std::decay_t<typename Range::output_type>>);

      using return_type = std::vector<std::vector<Word>>;

      if (tc.number_of_classes() == POSITIVE_INFINITY) {
        LIBSEMIGROUPS_EXCEPTION(
            "the 1st argument defines a congruence with infinitely many "
            "classes, the non-trivial classes cannot be determined!");
        // They really can't be determined because we cannot run
        // ToddCoxeter<Word> at all
      } else if (!r.is_finite) {
        LIBSEMIGROUPS_EXCEPTION("the 2nd argument (a range) must be finite, "
                                "found an infinite range");
      }
      return_type         result;
      std::vector<size_t> lookup;
      size_t              next_index = 0;

      while (!r.at_end()) {
        auto       next  = r.get();
        auto const index = todd_coxeter::index_of(tc, next);
        if (index >= lookup.size()) {
          lookup.resize(index + 1, UNDEFINED);
        }
        if (lookup[index] == UNDEFINED) {
          lookup[index] = next_index++;
          result.emplace_back();
        }
        result[lookup[index]].push_back(std::move(next));
        r.next();
      }
      return result;
    }

    template <typename Word>
    std::vector<std::vector<Word>> non_trivial_classes(ToddCoxeter<Word>& tc1,
                                                       ToddCoxeter<Word>& tc2) {
      return non_trivial_classes(tc1, normal_forms<Word>(tc2));
    }
  }  // namespace congruence_interface
}  // namespace libsemigroups
