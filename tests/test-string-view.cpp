// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2021 James D. Mitchell + Maria Tsalakou
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

// TODO(later)
// - Write tests for maximum_common_suffix

#include "catch.hpp"      // for REQUIRE, REQUIRE_NOTHROW, REQUIRE_THROWS_AS
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include <cstddef>      // for size_t
#include <cstdint>      // for int64_t, uint64_t
#include <iterator>     // for distance, advance, operator!=
#include <string>       // for string
#include <string>       // for basic_string, operator==
#include <type_traits>  // for move

#include "libsemigroups/string-view.hpp"  // for MultiStringView, StringView...

namespace libsemigroups {
  using detail::MultiStringView;
  struct LibsemigroupsException;  // forward decl

  LIBSEMIGROUPS_TEST_CASE("MultiStringView",
                          "000",
                          "catch all",
                          "[quick][multistringview]") {
    // REQUIRE(sizeof(detail::MultiStringView) == 32);
    // REQUIRE(sizeof(uint64_t) == 8);
    std::string             s = "abcdefghijkl";
    detail::MultiStringView msv(s.cbegin(), s.cbegin() + 3);
    REQUIRE(msv.size() == 3);
    msv.append(s.cbegin(), s.cend());
    REQUIRE(msv.size() == s.size() + 3);
    REQUIRE(msv.cbegin() != msv.cbegin() + 3);
    REQUIRE(*msv.cbegin() == *(msv.cbegin() + 3));
    REQUIRE(msv.cbegin() + 1 != msv.cbegin() + 4);
    REQUIRE(*(msv.cbegin() + 1) == *(msv.cbegin() + 4));
    REQUIRE(msv.cbegin() + 2 != msv.cbegin() + 5);
    REQUIRE(*(msv.cbegin() + 2) == *(msv.cbegin() + 5));
    REQUIRE((msv.cbegin() + 3) != (msv.cbegin() + 6));
    REQUIRE(*(msv.cbegin() + 3) != *(msv.cbegin() + 6));
    REQUIRE(std::string(msv) == "abcabcdefghijkl");
    REQUIRE(((msv.cbegin() + 3) - msv.cbegin()) == 3);
    REQUIRE((msv.cbegin() - (msv.cbegin() + 3)) == -3);
    REQUIRE(static_cast<size_t>(msv.cend() - msv.cbegin()) == msv.size());
    REQUIRE(msv.cbegin() - msv.cend() == -static_cast<int64_t>(msv.size()));
    REQUIRE(msv.cbegin() + msv.size() == msv.cend());
    std::string t = "";
    for (auto it = msv.cbegin(); it != msv.cend(); ++it) {
      t += *it;
    }
    REQUIRE(t == "abcabcdefghijkl");
    REQUIRE(msv.number_of_views() == 2);
    REQUIRE(msv == msv);
    REQUIRE(msv.cbegin()[10] == 'h');
    detail::MultiStringView msv2(s.cbegin(), s.cbegin() + 3);
    msv2.append(s.cbegin(), s.cbegin() + 3);
    msv2.append(s.cbegin() + 3, s.cend());

    REQUIRE(msv == msv);
    REQUIRE(msv == msv2);
    REQUIRE(msv2 == msv);
    msv2.append(s.cbegin(), s.cbegin() + 3);
    REQUIRE(msv2 != msv);
    REQUIRE(msv != msv2);
    REQUIRE(msv < msv2);
    REQUIRE(msv <= msv2);
    REQUIRE(msv2 >= msv);
    REQUIRE(msv2 > msv);
    t = "";
    for (auto it = msv2.crbegin(); it != msv2.crend(); ++it) {
      t += *it;
    }
    REQUIRE(t == "cbalkjihgfedcbacba");

    REQUIRE(msv2.number_of_views() == 3);
    msv2.append(s.cbegin() + 1, s.cbegin() + 1);
    REQUIRE(msv2.number_of_views() == 3);

    detail::MultiStringView msv3;
    REQUIRE(msv3.empty());
    REQUIRE(std::string(msv3) == "");

    msv += msv2;
    REQUIRE(std::string(msv) == "abcabcdefghijklabcabcdefghijklabc");
    REQUIRE(msv.number_of_views() == 5);
    auto it1 = msv.cbegin();
    auto it2 = msv.cbegin();
    ++++++++++++it1;
    ++++++++++++++++++++++++++++++it2;
    REQUIRE(it2 - it1 == 9);
    auto it3 = (msv.cbegin() + 6);
    REQUIRE(it3 == it1);
    it3 = (msv.cbegin() + 6);
    std::advance(it3, 9);
    REQUIRE(*it3 == 'a');
    REQUIRE(it3 - msv.cbegin() == 15);
    REQUIRE(it3 == (msv.cbegin() + 15));
    REQUIRE((msv.cbegin() + 15) - (msv.cbegin() + 6) == 9);

    t = "";
    for (auto it = msv.cbegin() + 6; it != msv.cbegin() + 15; ++it) {
      t += *it;
    }

    REQUIRE(t == "defghijkl");

    REQUIRE(msv3.number_of_views() == 0);
    msv3.append(msv.cbegin() + 6, msv.cbegin() + 15);
    REQUIRE(std::string(msv3) == "defghijkl");

    msv3.clear();
    REQUIRE(msv3.empty());

    msv3.append(msv.cbegin() + 6, msv.cbegin() + 16);
    REQUIRE(std::string(msv3) == "defghijkla");
  }

  LIBSEMIGROUPS_TEST_CASE("MultiStringView",
                          "001",
                          "is_prefix",
                          "[quick][multistringview]") {
    using detail::MultiStringView;
    std::string     s = "abcdefghijkl";
    MultiStringView m1(s.cbegin(), s.cend() - 4);
    REQUIRE(std::string(m1) == "abcdefgh");
    REQUIRE(m1.number_of_views() == 1);
    m1.append(s.begin(), s.cend());
    REQUIRE(std::string(m1) == "abcdefghabcdefghijkl");
    REQUIRE(m1.number_of_views() == 2);
    m1.append(s.begin(), s.cend());
    REQUIRE(m1.number_of_views() == 3);
    REQUIRE(std::string(m1) == "abcdefghabcdefghijklabcdefghijkl");
    m1.append(s.begin(), s.cend());
    REQUIRE(m1.number_of_views() == 4);

    MultiStringView m2(s.cbegin(), s.cend() - 4);
    m2.append(s.begin(), s.cend());

    REQUIRE(detail::is_prefix(m1, m2));
  }

  LIBSEMIGROUPS_TEST_CASE("MultiStringView",
                          "002",
                          "erase",
                          "[quick][multistringview]") {
    std::string     s = "abcdefghijkl";
    MultiStringView m(s.cbegin(), s.cend() - 4);
    m.append(s.begin(), s.cend());
    m.append(s.begin(), s.cend());
    m.append(s.begin(), s.cend());

    REQUIRE(std::string(m) == "abcdefghabcdefghijklabcdefghijklabcdefghijkl");
    // Both string_type::iterators in the same range not at the beginning or
    // end.
    m.erase(m.cbegin() + 1, m.cbegin() + 2);
    REQUIRE(std::string(m) == "acdefghabcdefghijklabcdefghijklabcdefghijkl");
    m.erase(m.cbegin(), m.cbegin() + 3);
    REQUIRE(std::string(m) == "efghabcdefghijklabcdefghijklabcdefghijkl");
    m.erase(m.cbegin() + 1, m.cbegin() + 4);
    REQUIRE(std::string(m) == "eabcdefghijklabcdefghijklabcdefghijkl");
    m.erase(m.cbegin() + 5, m.cend());
    REQUIRE(std::string(m) == "eabcd");

    REQUIRE(m.size() == 5);
    m.erase(m.cend(), m.cbegin());
    REQUIRE(m.size() == 5);
  }

  LIBSEMIGROUPS_TEST_CASE("MultiStringView",
                          "003",
                          "iterators",
                          "[quick][multistringview]") {
    std::string     s = "abcdefghijkl";
    MultiStringView m(s.cbegin(), s.cend() - 4);
    m.append(s.begin(), s.cend());
    m.append(s.begin(), s.cend());
    m.append(s.begin(), s.cend());
    auto it = m.cbegin();
    it += 0;
    REQUIRE(it == m.cbegin());
    REQUIRE(m.cbegin() == it);
    it += 3;
    REQUIRE(it > m.cbegin());
    REQUIRE(it - m.cbegin() == 3);

    MultiStringView mm;
    REQUIRE(mm.empty());
    REQUIRE(mm.cbegin() == mm.cend());
  }

  LIBSEMIGROUPS_TEST_CASE("MultiStringView",
                          "004",
                          "constructors (long)",
                          "[quick][multistringview]") {
    std::string     s = "abcdefghijkl";
    MultiStringView m(s.cbegin(), s.cend() - 4);
    m.append(s.begin(), s.cend());
    m.append(s.begin(), s.cend());
    m.append(s.begin(), s.cend());
    REQUIRE(!m.empty());

    MultiStringView mm(m);
    REQUIRE(m == mm);

    MultiStringView mmm(std::move(mm));
    REQUIRE(m == mmm);

    m.erase(m.cbegin() + 1, m.cbegin() + 4);
    REQUIRE(std::string(m) == "aefghabcdefghijklabcdefghijklabcdefghijkl");

    mm = m;
    REQUIRE(mm == m);
    REQUIRE(mm != mmm);

    mmm = std::move(mm);
    REQUIRE(mmm == m);

    REQUIRE(m.size() == 41);
    REQUIRE(std::distance(m.cbegin() + 4, m.cend()) == 37);
    REQUIRE((m.cend() - 11) - (m.cbegin() + 4) == 26);
    MultiStringView mmmm(m.cbegin() + 4, m.cend() - 11);

    REQUIRE(std::string(m) == "aefghabcdefghijklabcdefghijklabcdefghijkl");
    REQUIRE(std::string(mmmm) == "habcdefghijklabcdefghijkla");
    REQUIRE(mmmm == MultiStringView("habcdefghijklabcdefghijkla"));
    REQUIRE(!mmmm.empty());
  }

  LIBSEMIGROUPS_TEST_CASE("MultiStringView",
                          "005",
                          "constructors (short)",
                          "[quick][multistringview]") {
    std::string     s = "abcdefghijkl";
    MultiStringView m(s.cbegin(), s.cend() - 4);
    m.append(s.begin(), s.cend());

    MultiStringView mm(m);
    REQUIRE(m == mm);

    MultiStringView mmm(std::move(mm));
    REQUIRE(m == mmm);

    m.erase(m.cbegin() + 1, m.cbegin() + 4);
    REQUIRE(std::string(m) == "aefghabcdefghijkl");

    mm = m;
    REQUIRE(mm == m);
    REQUIRE(mm != mmm);

    mmm = std::move(mm);
    REQUIRE(mmm == m);

    REQUIRE(m.size() == 17);
    MultiStringView mmmm(m.cbegin() + 4, m.cend() - 11);

    REQUIRE(std::string(m) == "aefghabcdefghijkl");
    REQUIRE(std::string(mmmm) == "ha");
    REQUIRE(mmmm == MultiStringView("ha"));
  }

  LIBSEMIGROUPS_TEST_CASE("MultiStringView",
                          "006",
                          "copy assignment (short assigned to long)",
                          "[quick][multistringview]") {
    std::string     s = "abcdefghijkl";
    MultiStringView m(s.cbegin(), s.cend() - 4);
    m.append(s.begin(), s.cend());
    REQUIRE(m.size() == 20);

    MultiStringView mm(s.cbegin(), s.cend() - 4);
    mm.append(s.begin(), s.cend());
    mm.append(s.begin(), s.cend());
    REQUIRE(mm.size() == 32);

    mm = m;

    REQUIRE(mm.size() == 20);
    REQUIRE(mm == m);
    REQUIRE(mm.number_of_views() == 2);
    REQUIRE(std::string(mm) == std::string(m));
    REQUIRE(std::string(mm) == "abcdefghabcdefghijkl");
  }

  LIBSEMIGROUPS_TEST_CASE("MultiStringView",
                          "007",
                          "copy assignment (short assigned to short)",
                          "[quick][multistringview]") {
    std::string     s = "abcdefghijkl";
    MultiStringView m(s.cbegin(), s.cend() - 4);
    m.append(s.begin(), s.cend());
    REQUIRE(m.size() == 20);

    MultiStringView mm(s.cbegin(), s.cend() - 4);
    mm.append(s.begin(), s.cend() - 1);
    REQUIRE(mm.size() == 19);
    REQUIRE(m != mm);

    mm = m;

    REQUIRE(mm.size() == 20);
    REQUIRE(mm == m);
    REQUIRE(mm.number_of_views() == 2);
    REQUIRE(std::string(mm) == std::string(m));
    REQUIRE(std::string(mm) == "abcdefghabcdefghijkl");
  }

  LIBSEMIGROUPS_TEST_CASE("MultiStringView",
                          "008",
                          "move assignment (short assigned to long)",
                          "[quick][multistringview]") {
    std::string     s = "abcdefghijkl";
    MultiStringView m(s.cbegin(), s.cend() - 4);
    m.append(s.begin(), s.cend());
    REQUIRE(m.size() == 20);

    MultiStringView mm(s.cbegin(), s.cend() - 4);
    mm.append(s.begin(), s.cend());
    mm.append(s.begin(), s.cend());
    REQUIRE(mm.size() == 32);

    mm = std::move(m);

    REQUIRE(mm.size() == 20);
    REQUIRE(mm == m);
    REQUIRE(mm.number_of_views() == 2);
    REQUIRE(std::string(mm) == std::string(m));
    REQUIRE(std::string(mm) == "abcdefghabcdefghijkl");
  }

  LIBSEMIGROUPS_TEST_CASE("MultiStringView",
                          "009",
                          "move assignment (short assigned to short)",
                          "[quick][multistringview]") {
    std::string     s = "abcdefghijkl";
    MultiStringView m(s.cbegin(), s.cend() - 4);
    m.append(s.begin(), s.cend());
    REQUIRE(m.size() == 20);

    MultiStringView mm(s.cbegin(), s.cend() - 4);
    mm.append(s.begin(), s.cend() - 1);
    REQUIRE(mm.size() == 19);
    REQUIRE(m != mm);

    mm = std::move(m);

    REQUIRE(mm.size() == 20);
    REQUIRE(mm == m);
    REQUIRE(mm.number_of_views() == 2);
    REQUIRE(std::string(mm) == std::string(m));
    REQUIRE(std::string(mm) == "abcdefghabcdefghijkl");
  }

  LIBSEMIGROUPS_TEST_CASE("MultiStringView",
                          "010",
                          "code coverage",
                          "[quick][stringview]") {
    std::string                 s = "abcdefghijkl";
    detail::StringViewContainer m;
    m.emplace_back(s.cbegin() + 1, s.cbegin() + 5);
    m.emplace_back(s.cbegin() + 2, s.cbegin() + 6);

    REQUIRE(m.size(0) == 4);
    REQUIRE(m.size(1) == 4);
    REQUIRE(m.number_of_views() == 2);
    auto foo = [](detail::StringViewContainer const& mm) { return mm.size(1); };
    REQUIRE(foo(m) == 4);

    m.pop_back();
    REQUIRE(m.number_of_views() == 1);
    m.pop_back();
    REQUIRE(m.number_of_views() == 0);
    REQUIRE(m.size() == 0);
    REQUIRE(m.empty());
    m.pop_back();  // does nothing
    REQUIRE(m.number_of_views() == 0);
    REQUIRE(m.size() == 0);
    REQUIRE(m.empty());

    m.insert(0, detail::StringView(s.cbegin(), s.cend()));
    REQUIRE(m.number_of_views() == 1);
    REQUIRE(m.size() == 12);
    m.insert(0, detail::StringView(s.cend() - 2, s.cend()));
    REQUIRE(m.number_of_views() == 2);
    REQUIRE(m.size(0) == 2);
    REQUIRE(m.size(1) == 12);
    REQUIRE(m.size() == 14);
    m.erase(0, 1);
    REQUIRE(m.number_of_views() == 1);
    REQUIRE(m.size() == 12);
    m.insert(1, detail::StringView(s.cend() - 2, s.cend()));
    REQUIRE(m.number_of_views() == 2);
    REQUIRE(m.size(1) == 2);
    REQUIRE(m.size(0) == 12);
    m.erase(0, 2);
    REQUIRE(m.empty());
    m.erase(0, 2);  // does nothing
    REQUIRE(m.empty());

    m.emplace_back(s.cbegin() + 1, s.cbegin() + 6);
    m.emplace_back(s.cbegin() + 2, s.cbegin() + 6);
    REQUIRE(m.size(0) == 5);
    REQUIRE(m.size(1) == 4);
    REQUIRE(m.size() == 9);
    REQUIRE(m.number_of_views() == 2);

    m.erase(1, 2);
    REQUIRE(m.size(0) == 5);
    REQUIRE(m.size() == 5);
    REQUIRE(m.number_of_views() == 1);
    m.erase(0, 1);
    REQUIRE(m.size() == 0);
    REQUIRE(m.number_of_views() == 0);
    REQUIRE(m.empty());

    m.emplace_back(s.cbegin() + 1, s.cbegin() + 6);
    m.emplace_back(s.cbegin() + 2, s.cbegin() + 6);
    m.emplace_back(s.cbegin() + 2, s.cbegin() + 6);
    m.emplace_back(s.cbegin() + 2, s.cbegin() + 6);
    REQUIRE(!m.empty());
    REQUIRE(m.size() == 17);
    m.pop_back();
    REQUIRE(m.size() == 13);
    m.clear();
    REQUIRE(m.empty());
  }

  LIBSEMIGROUPS_TEST_CASE("MultiStringView",
                          "011",
                          "pop_front",
                          "[quick][multistringview]") {
    std::string     s = "abcdefghijkl";
    MultiStringView m(s.cbegin(), s.begin() + 4);
    m.append(s.begin() + 1, s.begin() + 3);
    REQUIRE(m.number_of_views() == 2);
    REQUIRE(m.size() == 6);
    REQUIRE(std::string(m) == "abcdbc");

    REQUIRE(m.pop_front() == 'a');
    REQUIRE(std::string(m) == "bcdbc");
    REQUIRE(m.number_of_views() == 2);

    REQUIRE(m.pop_front() == 'b');
    REQUIRE(std::string(m) == "cdbc");
    REQUIRE(m.number_of_views() == 2);

    REQUIRE(m.pop_front() == 'c');
    REQUIRE(std::string(m) == "dbc");
    REQUIRE(m.number_of_views() == 2);

    REQUIRE(m.pop_front() == 'd');
    REQUIRE(std::string(m) == "bc");
    REQUIRE(m.number_of_views() == 1);

    REQUIRE(m.pop_front() == 'b');
    REQUIRE(std::string(m) == "c");
    REQUIRE(m.number_of_views() == 1);

    REQUIRE(m.pop_front() == 'c');
    REQUIRE(std::string(m) == "");
    REQUIRE(m.number_of_views() == 0);

    REQUIRE_THROWS_AS(m.pop_front(), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("MultiStringView",
                          "012",
                          "append",
                          "[quick][multistringview]") {
    std::string     s = "abcdefghijkl";
    MultiStringView m(s.cbegin() + 3, s.begin() + 6);
    REQUIRE(std::string(m) == "def");
    m.append(m.cbegin(), m.cbegin() + 1);
    REQUIRE(std::string(m) == "defd");
    m.append(m.cbegin(), m.cend());
    REQUIRE(std::string(m) == "defddefd");
    m.append(m.cbegin() + 3, m.cbegin() + 5);
    REQUIRE(std::string(m) == "defddefddd");
    REQUIRE(m.size() == 10);
    m.append(m.cbegin(), m.cend());
    REQUIRE(m.size() == 20);
    // Check the case when we append an interval adjacent to the end of the
    // current view
    m.clear();
    REQUIRE(m.number_of_views() == 0);
    m.append(s.cbegin(), s.cbegin() + 5);
    REQUIRE(m.number_of_views() == 1);
    m.append(s.cbegin() + 5, s.cend());
    REQUIRE(m.number_of_views() == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("MultiStringView",
                          "013",
                          "operators",
                          "[quick][multistringview]") {
    std::string     s = "abcdefghijkl";
    MultiStringView m(s.cbegin() + 3, s.begin() + 6);
    m.append(m.cbegin(), m.cbegin() + 1);
    m.append(m.cbegin(), m.cend());
    m.append(m.cbegin() + 3, m.cbegin() + 5);
    REQUIRE(std::string(m) == "defddefddd");
    REQUIRE(m[0] == 'd');
    REQUIRE(m[1] == 'e');
    REQUIRE(m[2] == 'f');
    REQUIRE(m[3] == 'd');
    REQUIRE(m[4] == 'd');
    REQUIRE(m[5] == 'e');
    REQUIRE(m[6] == 'f');
    REQUIRE(m[7] == 'd');
    REQUIRE(m[8] == 'd');
    REQUIRE(m[9] == 'd');

    MultiStringView mm(s);
    REQUIRE(std::string(mm + m) == "abcdefghijkldefddefddd");
  }

  LIBSEMIGROUPS_TEST_CASE("MultiStringView",
                          "014",
                          "maximum_common_suffix",
                          "[quick][multistringview]") {
    std::string     s = "abcdefghijkl";
    MultiStringView m(s.cbegin() + 3, s.begin() + 6);
    m.append(m.cbegin(), m.cbegin() + 1);
    m.append(m.cbegin(), m.cend());
    m.append(m.cbegin() + 3, m.cbegin() + 5);
    REQUIRE(std::string(m) == "defddefddd");

    MultiStringView mm = m + m;
    REQUIRE(maximum_common_suffix(mm, m) == m);
  }

  LIBSEMIGROUPS_TEST_CASE("MultiStringView",
                          "015",
                          "deep tests",
                          "[quick][multistringview]") {
    std::string     s = "bcdabcd";
    MultiStringView m(s.cbegin(), s.begin() + 2);
    m.append(s.cbegin() + 2, s.end());

    MultiStringView ww(m.cbegin(), m.cbegin() + 2);
    REQUIRE(ww.size() == 2);
  }

}  // namespace libsemigroups
