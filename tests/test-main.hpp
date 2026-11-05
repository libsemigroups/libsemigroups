//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2026 James D. Mitchell
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

#ifndef LIBSEMIGROUPS_TESTS_TEST_MAIN_HPP_
#define LIBSEMIGROUPS_TESTS_TEST_MAIN_HPP_

#include <string>       // for string
#include <string_view>  // for string_view
#include <type_traits>  // for is_reference
#include <utility>      // for forward, move
#include <vector>       // for vector

#define CATCH_CONFIG_ENABLE_ALL_STRINGMAKERS

#include "libsemigroups/config.hpp"      // for LIBSEMIGROUPS_CATCH_ALL_HEADER
#include LIBSEMIGROUPS_CATCH_ALL_HEADER  // for TEST_CASE, etc.

#include "libsemigroups/order.hpp"  // for lenlex_cmp

#define STR2(X) #X
#define STR(X) STR2(X)

#define LIBSEMIGROUPS_TEST_NUM "LIBSEMIGROUPS_TEST_NUM="
#define LIBSEMIGROUPS_TEST_PREFIX "LIBSEMIGROUPS_TEST_PREFIX="

// Note that LIBSEMIGROUPS_TEST_NUM and LIBSEMIGROUPS_TEST_PREFIX allow us to
// locate these tags in the listener
#define LIBSEMIGROUPS_TEST_CASE(classname, nr, msg, tags)                 \
  TEST_CASE("[" nr "]: " classname ": " msg,                              \
            "[" LIBSEMIGROUPS_TEST_PREFIX classname " " nr "][" classname \
            " " nr "][" classname "][" nr "][" LIBSEMIGROUPS_TEST_NUM nr  \
            "][" __FILE__ "][" STR(__LINE__) "]" tags)

#define LIBSEMIGROUPS_TEMPLATE_TEST_CASE(classname, nr, msg, tags, ...) \
  TEMPLATE_TEST_CASE("[" nr "]: " classname ": " msg,                   \
                     "[" LIBSEMIGROUPS_TEST_PREFIX classname " " nr     \
                     "][" classname " " nr "][" classname "][" nr       \
                     "][" LIBSEMIGROUPS_TEST_NUM nr "][" __FILE__       \
                     "][" STR(__LINE__) "]" tags,                       \
                     __VA_ARGS__)

// These wrappers retain Catch2's generator behaviour, including inline uses,
// and record the selected values for the listener on every partial run.
// An optional first argument test::prefix("label = ") labels the displayed
// value without changing the type or value returned by the generator.
#define LIBSEMIGROUPS_GENERATE(...)        \
  ::libsemigroups::test::record_generator( \
      GENERATE(__VA_ARGS__), #__VA_ARGS__, {__FILE__, __LINE__})
#define LIBSEMIGROUPS_GENERATE_COPY(...)   \
  ::libsemigroups::test::record_generator( \
      GENERATE_COPY(__VA_ARGS__), #__VA_ARGS__, {__FILE__, __LINE__})
#define LIBSEMIGROUPS_GENERATE_REF(...)    \
  ::libsemigroups::test::record_generator( \
      GENERATE_REF(__VA_ARGS__), #__VA_ARGS__, {__FILE__, __LINE__})

namespace libsemigroups::test {
  struct GeneratorPrefix {
    std::string text;
  };

  inline GeneratorPrefix prefix(std::string text) {
    return {std::move(text)};
  }

  template <typename T>
  struct PrefixedGeneratorValue {
    T           value;
    std::string prefix;
  };

  // Catch2's GENERATE macros call makeGenerators unqualified. A prefix tag
  // selects this overload through argument-dependent lookup; ordinary calls
  // continue to use Catch2's overloads directly.
  template <typename... Args>
  auto makeGenerators(GeneratorPrefix label, Args&&... args) {
    auto source
        = Catch::Generators::makeGenerators(std::forward<Args>(args)...);
    using Source = decltype(source);
    using Value  = typename Source::type;
    // GeneratorWrapper takes ownership of the source generator.
    auto wrapped = Catch::Generators::GeneratorWrapper<Value>(
        new Source(std::move(source)));
    auto labelled = Catch::Generators::map(
        [label = std::move(label)](Value const& value) {
          return PrefixedGeneratorValue<Value>{value, label.text};
        },
        std::move(wrapped));
    return Catch::Generators::makeGenerators(std::move(labelled));
  }

  struct GeneratorInfo {
    std::string           expression;
    std::string           value;
    Catch::SourceLineInfo location;
  };

  std::vector<GeneratorInfo> const& generator_info();
  void                              clear_generator_info();
  void record_generator_info(char const*           expression,
                             std::string           value,
                             Catch::SourceLineInfo location);

  template <typename T>
  T record_generator(T                     value,
                     char const*           expression,
                     Catch::SourceLineInfo location,
                     std::string_view      value_prefix = "") {
    record_generator_info(
        expression, fmt::format("{}{}", value_prefix, value), location);
    return value;
  }

  template <typename T>
  T record_generator(PrefixedGeneratorValue<T> value,
                     char const*               expression,
                     Catch::SourceLineInfo     location) {
    return record_generator(
        std::move(value.value), expression, location, value.prefix);
  }
}  // namespace libsemigroups::test

std::string chomp(std::string_view what);

// TODO(1) make this a function, and allow specifying optionally the type of
// exception
#define REQUIRE_EXCEPTION_MSG(code, expected)      \
  REQUIRE_THROWS_AS(code, LibsemigroupsException); \
  try {                                            \
    code;                                          \
  } catch (LibsemigroupsException const& e) {      \
    REQUIRE(chomp(e.what()) == expected);          \
  }

namespace libsemigroups {

  template <typename T>
  void verify_forward_iterator_requirements(T it) {
    using deref_type = decltype(*it);
    REQUIRE_NOTHROW(*it);
    REQUIRE(std::is_reference<deref_type>::value);
    REQUIRE(
        std::is_const<typename std::remove_reference<deref_type>::type>::value);
    T copy(it);
    REQUIRE(&copy != &it);
    it++;
    auto it_val   = *it;
    auto copy_val = *copy;
    std::swap(it, copy);
    REQUIRE(copy_val == *it);
    REQUIRE(it_val == *copy);

    it.swap(copy);
    REQUIRE(it_val == *it);
    REQUIRE(copy_val == *copy);

    ++copy;
    REQUIRE(*it == *copy);

    ++it;
    copy++;
    REQUIRE(*it == *copy);
  }

  struct weird_cmp {
    template <typename Word>
    bool operator()(std::pair<Word, Word> const& x,
                    std::pair<Word, Word> const& y) const noexcept {
      return lenlex_cmp(x.first, y.first)
             || (x.first == y.first && lenlex_cmp(x.second, y.second));
    }
  };
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_TESTS_TEST_MAIN_HPP_
