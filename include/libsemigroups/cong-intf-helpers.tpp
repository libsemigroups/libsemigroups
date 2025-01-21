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

// This file contains the implementations of helper function templates for the
// CongruenceInterface class.

namespace libsemigroups {

  namespace congruence_interface {
    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - reduce_no_run_no_checks
    ////////////////////////////////////////////////////////////////////////

    template <typename Subclass>
    typename Subclass::native_word_type
    reduce_no_run_no_checks(Subclass const&                            ci,
                            typename Subclass::native_word_type const& w) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      typename Subclass::native_word_type result;
      ci.reduce_no_run_no_checks(
          std::back_inserter(result), std::begin(w), std::end(w));
      return result;
    }

    template <typename Subclass, typename Int>
    typename Subclass::native_word_type
    reduce_no_run_no_checks(Subclass const&                   ci,
                            std::initializer_list<Int> const& w) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      typename Subclass::native_word_type result;
      ci.reduce_no_run_no_checks(
          std::back_inserter(result), std::begin(w), std::end(w));
      return result;
    }

    template <typename Subclass>
    typename Subclass::native_word_type
    reduce_no_run_no_checks(Subclass const& ci, char const* w) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      typename Subclass::native_word_type result;
      ci.reduce_no_run_no_checks(
          std::back_inserter(result), w, w + std::strlen(w));
      return result;
    }

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - reduce_no_run
    ////////////////////////////////////////////////////////////////////////

    template <typename Subclass>
    typename Subclass::native_word_type
    reduce_no_run(Subclass const&                            ci,
                  typename Subclass::native_word_type const& w) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      typename Subclass::native_word_type result;
      ci.reduce_no_run(std::back_inserter(result), std::begin(w), std::end(w));
      return result;
    }

    template <typename Subclass, typename Int>
    typename Subclass::native_word_type
    reduce_no_run(Subclass const& ci, std::initializer_list<Int> const& w) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      typename Subclass::native_word_type result;
      ci.reduce_no_run(std::back_inserter(result), std::begin(w), std::end(w));
      return result;
    }

    template <typename Subclass>
    typename Subclass::native_word_type reduce_no_run(Subclass const& ci,
                                                      char const*     w) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      typename Subclass::native_word_type result;
      ci.reduce_no_run(std::back_inserter(result), w, w + std::strlen(w));
      return result;
    }

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - reduce_no_checks
    ////////////////////////////////////////////////////////////////////////

    template <typename Subclass>
    typename Subclass::native_word_type
    reduce_no_checks(Subclass&                                  ci,
                     typename Subclass::native_word_type const& w) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      typename Subclass::native_word_type result;
      ci.reduce_no_checks(
          std::back_inserter(result), std::begin(w), std::end(w));
      return result;
    }

    template <typename Subclass, typename Int>
    typename Subclass::native_word_type
    reduce_no_checks(Subclass& ci, std::initializer_list<Int> const& w) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      typename Subclass::native_word_type result;
      ci.reduce_no_checks(
          std::back_inserter(result), std::begin(w), std::end(w));
      return result;
    }

    template <typename Subclass>
    typename Subclass::native_word_type reduce_no_checks(Subclass&   ci,
                                                         char const* w) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      typename Subclass::native_word_type result;
      ci.reduce_no_checks(std::back_inserter(result), w, w + std::strlen(w));
      return result;
    }

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - reduce
    ////////////////////////////////////////////////////////////////////////

    template <typename Subclass>
    typename Subclass::native_word_type
    reduce(Subclass& ci, typename Subclass::native_word_type const& w) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      typename Subclass::native_word_type result;
      ci.reduce(std::back_inserter(result), std::begin(w), std::end(w));
      return result;
    }

    template <typename Subclass, typename Int>
    typename Subclass::native_word_type
    reduce(Subclass& ci, std::initializer_list<Int> const& w) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      typename Subclass::native_word_type result;
      ci.reduce(std::back_inserter(result), std::begin(w), std::end(w));
      return result;
    }

    template <typename Subclass>
    typename Subclass::native_word_type reduce(Subclass& ci, char const* w) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      typename Subclass::native_word_type result;
      ci.reduce(std::back_inserter(result), w, w + std::strlen(w));
      return result;
    }

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - partition
    ////////////////////////////////////////////////////////////////////////

    template <typename Subclass, typename Range, typename>
    std::vector<std::vector<typename Subclass::native_word_type>>
    partition(Subclass& ci, Range r) {
      using Word = typename Subclass::native_word_type;

      // Congruence + ToddCoxeter have their own overloads for this
      static_assert(!std::is_base_of_v<ToddCoxeterBase, Subclass>
                    && !std::is_base_of_v<CongruenceBase, Subclass>);
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);

      static_assert(
          std::is_same_v<Word, std::decay_t<typename Range::output_type>>);

      if (!r.is_finite) {
        LIBSEMIGROUPS_EXCEPTION("the 2nd argument (a range) must be finite, "
                                "found an infinite range");
      }

      std::vector<std::vector<Word>> result;

      std::unordered_map<Word, size_t> map;
      size_t                           index = 0;

      while (!r.at_end()) {
        auto next = r.get();
        if (ci.presentation().contains_empty_word() || !next.empty()) {
          auto next_nf        = congruence_interface::reduce(ci, next);
          auto [it, inserted] = map.emplace(next_nf, index);
          if (inserted) {
            result.emplace_back();
            index++;
          }
          size_t index_of_next_nf = it->second;
          result[index_of_next_nf].push_back(next);
        }
        r.next();
      }
      return result;
    }

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - non_trivial_classes
    ////////////////////////////////////////////////////////////////////////

    template <typename Subclass, typename Range, typename>
    std::vector<std::vector<typename Subclass::native_word_type>>
    non_trivial_classes(Subclass& ci, Range r) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      static_assert(std::is_same_v<typename Subclass::native_word_type,
                                   std::decay_t<typename Range::output_type>>);
      auto result = partition(ci, r);
      result.erase(
          std::remove_if(result.begin(),
                         result.end(),
                         [](auto const& x) -> bool { return x.size() <= 1; }),
          result.end());
      return result;
    }

  }  // namespace congruence_interface
}  // namespace libsemigroups
