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

    template <typename Subclass, typename InputWord, typename OutputWord>
    OutputWord reduce_no_run_no_checks(Subclass const& ci, InputWord const& w) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      OutputWord result;
      ci.reduce_no_run_no_checks(
          std::back_inserter(result), std::begin(w), std::end(w));
      return result;
    }

    template <typename Subclass, typename Int, typename OutputWord>
    OutputWord reduce_no_run_no_checks(Subclass const&                   ci,
                                       std::initializer_list<Int> const& w) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      if constexpr (std::is_base_of_v<ToddCoxeterBase, Subclass>) {
        // We have a special case here for ToddCoxeterBase because
        // ToddCoxeterBase reverses the output words, in its
        // reduce_no_run_no_checks, which does not have the template parameter
        // Subclass, so we call the 2-template parameter overload of this
        // function for ToddCoxeterBase. I.e. if we called the 3-template param
        // version as in the "else" case below, then we'd just be calling the
        // function above, which doesn't reverse the words, and we'd get
        // incorrect output.
        return reduce_no_run_no_checks<OutputWord, std::initializer_list<Int>>(
            ci, w);
      } else {
        return reduce_no_run_no_checks<Subclass,
                                       std::initializer_list<Int>,
                                       OutputWord>(ci, w);
      }
    }

    template <typename Subclass, typename OutputWord>
    auto reduce_no_run_no_checks(Subclass const& ci, char const* w) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      if constexpr (std::is_base_of_v<ToddCoxeterBase, Subclass>) {
        return reduce_no_run_no_checks<OutputWord, std::string_view>(ci, w);
      } else {
        return reduce_no_run_no_checks<Subclass, std::string_view, OutputWord>(
            ci, w);
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - reduce_no_run
    ////////////////////////////////////////////////////////////////////////

    template <typename Subclass, typename InputWord, typename OutputWord>
    OutputWord reduce_no_run(Subclass const& ci, InputWord const& w) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      OutputWord result;
      ci.reduce_no_run(std::back_inserter(result), std::begin(w), std::end(w));
      return result;
    }

    template <typename Subclass, typename Int, typename OutputWord>
    OutputWord reduce_no_run(Subclass const&                   ci,
                             std::initializer_list<Int> const& w) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      if constexpr (std::is_base_of_v<ToddCoxeterBase, Subclass>) {
        // See the comment above about why there's a special case here.
        return reduce_no_run<OutputWord, std::initializer_list<Int>>(ci, w);
      } else {
        return reduce_no_run<Subclass, std::initializer_list<Int>, OutputWord>(
            ci, w);
      }
    }

    template <typename Subclass, typename OutputWord>
    OutputWord reduce_no_run(Subclass const& ci, char const* w) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      if constexpr (std::is_base_of_v<ToddCoxeterBase, Subclass>) {
        // See the comment above about why there's a special case here.
        return reduce_no_run<OutputWord, std::string_view>(ci, w);
      } else {
        return reduce_no_run<Subclass, std::string_view, OutputWord>(ci, w);
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - reduce_no_checks
    ////////////////////////////////////////////////////////////////////////

    template <typename Subclass, typename InputWord, typename OutputWord>
    OutputWord reduce_no_checks(Subclass& ci, InputWord const& w) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      OutputWord result;
      ci.reduce_no_checks(
          std::back_inserter(result), std::begin(w), std::end(w));
      return result;
    }

    template <typename Subclass, typename Int, typename OutputWord>
    OutputWord reduce_no_checks(Subclass&                         ci,
                                std::initializer_list<Int> const& w) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      if constexpr (std::is_base_of_v<ToddCoxeterBase, Subclass>) {
        // See the comment above about why there's a special case here.
        return reduce_no_checks<OutputWord, std::initializer_list<Int>>(ci, w);
      } else {
        return reduce_no_checks<Subclass,
                                std::initializer_list<Int>,
                                OutputWord>(ci, w);
      }
    }

    template <typename Subclass, typename OutputWord>
    OutputWord reduce_no_checks(Subclass& ci, char const* w) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      if constexpr (std::is_base_of_v<ToddCoxeterBase, Subclass>) {
        // See the comment above about why there's a special case here.
        return reduce_no_checks<OutputWord, std::string_view>(ci, w);
      } else {
        return reduce_no_checks<Subclass, std::string_view, OutputWord>(ci, w);
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - reduce
    ////////////////////////////////////////////////////////////////////////

    template <typename Subclass, typename InputWord, typename OutputWord>
    OutputWord reduce(Subclass& ci, InputWord const& w) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      OutputWord result;
      ci.reduce(std::back_inserter(result), std::begin(w), std::end(w));
      return result;
    }

    template <typename Subclass, typename Int, typename OutputWord>
    OutputWord reduce(Subclass& ci, std::initializer_list<Int> const& w) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      if constexpr (std::is_base_of_v<ToddCoxeterBase, Subclass>) {
        // See the comment above about why there's a special case here.
        return reduce<OutputWord, std::initializer_list<Int>>(ci, w);
      } else {
        return reduce<Subclass, std::initializer_list<Int>, OutputWord>(ci, w);
      }
    }

    template <typename Subclass, typename OutputWord>
    auto reduce(Subclass& ci, char const* w) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      if constexpr (std::is_base_of_v<ToddCoxeterBase, Subclass>) {
        // See the comment above about why there's a special case here.
        return reduce<OutputWord, std::string_view>(ci, w);
      } else {
        return reduce<Subclass, std::string_view, OutputWord>(ci, w);
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - partition
    ////////////////////////////////////////////////////////////////////////

    template <typename Subclass, typename Range, typename OutputWord, typename>
    std::vector<std::vector<OutputWord>> partition(Subclass& ci, Range r) {
      // Congruence + ToddCoxeter have their own overloads for this
      static_assert(!std::is_base_of_v<ToddCoxeterBase, Subclass>
                    && !std::is_base_of_v<CongruenceBase, Subclass>);

      if (!r.is_finite) {
        LIBSEMIGROUPS_EXCEPTION("the 2nd argument (a range) must be finite, "
                                "found an infinite range");
      }

      std::vector<std::vector<OutputWord>> result;

      std::unordered_map<OutputWord, size_t> map;
      size_t                                 index = 0;

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

    template <typename Subclass, typename Range, typename OutputWord, typename>
    std::vector<std::vector<OutputWord>> non_trivial_classes(Subclass& ci,
                                                             Range     r) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
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
