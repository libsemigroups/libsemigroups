//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022-2025 Murray T. Whyte
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

// This file contains some implementations of functions that produce fp
// semigroups for testing purposes.

#include "libsemigroups/presentation-examples.hpp"

#include <algorithm>         // for max, for_each
#include <cmath>             // for abs
#include <cstdint>           // for int64_t
#include <cstdlib>           // for abs
#include <initializer_list>  // for initializer_list
#include <numeric>           // for iota
#include <unordered_map>     // for operator==
#include <utility>           // for move, swap

#include "libsemigroups/constants.hpp"     // for operator==
#include "libsemigroups/debug.hpp"         // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/exception.hpp"     // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/presentation.hpp"  // for add_rule_no_checks, Presen...
#include "libsemigroups/ranges.hpp"        // for operator|, to_vector, enum...
#include "libsemigroups/types.hpp"         // for word_type, letter_type
#include "libsemigroups/word-range.hpp"    // for operator""_w, operator+, pow

#include "libsemigroups/detail/fmt.hpp"  // for format, format_string

namespace libsemigroups {
  using literals::operator""_w;
  using words::   operator+;

  namespace {

    word_type range(size_t first, size_t last, size_t step = 1) {
      return rx::seq<letter_type>(first, step) | rx::take(last - first)
             | rx::to_vector();
    }

    word_type range(size_t last) {
      return range(0, last, 1);
    }

    void add_full_transformation_monoid_relations(Presentation<word_type>& p,
                                                  size_t                   n,
                                                  size_t pi_start,
                                                  size_t e12_value) {
      // This function adds the full transformation monoid relations due to
      // Iwahori, from Section 9.3, p161-162, (Ganyushkin + Mazorchuk),
      // expressed in terms of the generating set {pi_2, ..., pi_n,
      // epsilon_{12}} using the notation of that chapter.
      // https://link.springer.com/book/10.1007/978-1-84800-281-4

      // The argument n specifies the degree of the full transformation monoid.
      // The generators corresponding to the pi_i will always constitute n - 2
      // consecutive integers, starting from the argument pi_start. The argument
      // m specifies the value which will represent the idempotent e12.

      // When adding these relations for the full transformation
      // monoid presentation (Iwahori) in this file, we want e12_value = n - 1.
      // For the partial transformation monoid presentation, we want e12_value =
      // n.

      LIBSEMIGROUPS_ASSERT(n >= 4);
      LIBSEMIGROUPS_ASSERT(e12_value < pi_start
                           || e12_value >= pi_start + n - 2);

      word_type              e12 = {e12_value};
      std::vector<word_type> pi;
      for (size_t i = pi_start; i <= pi_start + n - 2; ++i) {
        pi.push_back({i});
      }

      // The following expresses the epsilon idempotents in terms of the
      // generating set
      auto eps = [&e12, &pi](size_t i, size_t j) -> word_type {
        LIBSEMIGROUPS_ASSERT(i != j);
        if (i == 1 && j == 2) {
          return e12;
        } else if (i == 2 && j == 1) {
          return pi[0] + e12 + pi[0];
        } else if (i == 1) {
          return pi[0] + pi[j - 2] + pi[0] + e12 + pi[0] + pi[j - 2] + pi[0];
        } else if (j == 2) {
          return pi[i - 2] + e12 + pi[i - 2];
        } else if (j == 1) {
          return pi[0] + pi[i - 2] + e12 + pi[i - 2] + pi[0];
        } else if (i == 2) {
          return pi[j - 2] + pi[0] + e12 + pi[0] + pi[j - 2];
        }
        return pi[i - 2] + pi[0] + pi[j - 2] + pi[0] + e12 + pi[0] + pi[j - 2]
               + pi[0] + pi[i - 2];
      };

      auto transp = [&pi](size_t i, size_t j) -> word_type {
        LIBSEMIGROUPS_ASSERT(i != j);
        if (i > j) {
          std::swap(i, j);
        }
        if (i == 1) {
          return pi[j - 2];
        }
        return pi[i - 2] + pi[j - 2] + pi[i - 2];
      };

      // Relations a
      for (size_t i = 1; i <= n; ++i) {
        for (size_t j = 1; j <= n; ++j) {
          if (j == i) {
            continue;
          }
          // Relations (k)
          presentation::add_rule_no_checks(
              p, transp(i, j) + eps(i, j), eps(i, j));
          // Relations (j)
          presentation::add_rule_no_checks(p, eps(j, i) + eps(i, j), eps(i, j));
          // Relations (i)
          presentation::add_rule_no_checks(p, eps(i, j) + eps(i, j), eps(i, j));
          // Relations (d)
          presentation::add_rule_no_checks(
              p, transp(i, j) + eps(i, j) + transp(i, j), eps(j, i));
          for (size_t k = 1; k <= n; ++k) {
            if (k == i || k == j) {
              continue;
            }
            // Relations (h)
            presentation::add_rule_no_checks(
                p, eps(k, j) + eps(i, j), eps(k, j));
            // Relations (g)
            presentation::add_rule_no_checks(
                p, eps(k, i) + eps(i, j), transp(i, j) + eps(k, j));
            // Relations (f)
            presentation::add_rule_no_checks(
                p, eps(j, k) + eps(i, j), eps(i, j) + eps(i, k));
            presentation::add_rule_no_checks(
                p, eps(j, k) + eps(i, j), eps(i, k) + eps(i, j));
            // Relations (c)
            presentation::add_rule_no_checks(
                p, transp(k, i) + eps(i, j) + transp(k, i), eps(k, j));
            // Relations (b)
            presentation::add_rule_no_checks(
                p, transp(j, k) + eps(i, j) + transp(j, k), eps(i, k));
            for (size_t l = 1; l <= n; ++l) {
              if (l == i || l == j || l == k) {
                continue;
              }
              // Relations (e)
              presentation::add_rule_no_checks(
                  p, eps(l, k) + eps(i, j), eps(i, j) + eps(l, k));
              // Relations (a)
              presentation::add_rule_no_checks(
                  p, transp(k, l) + eps(i, j) + transp(k, l), eps(i, j));
            }
          }
        }
      }
    }

    void add_renner_type_B_common(Presentation<word_type>& p, size_t l, int q) {
      LIBSEMIGROUPS_ASSERT(q == 0 || q == 1);
      auto s = range(l);
      auto e = range(l, 2 * l + 1);

      if (q == 0) {
        presentation::add_idempotent_rules_no_checks(p, s);
      } else {
        presentation::add_involution_rules_no_checks(p, s);
      }

      for (size_t i = 0; i < l; ++i) {
        for (size_t j = i + 2; j < l; ++j) {
          presentation::add_rule_no_checks(p, {s[i], s[j]}, {s[j], s[i]});
        }
      }

      for (size_t i = 1; i < l - 1; ++i) {
        presentation::add_rule_no_checks(
            p, {s[i], s[i + 1], s[i]}, {s[i + 1], s[i], s[i + 1]});
      }

      presentation::add_rule_no_checks(
          p, {s[1], s[0], s[1], s[0]}, {s[0], s[1], s[0], s[1]});

      for (size_t i = 1; i < l; ++i) {
        for (size_t j = 0; j < i; ++j) {
          presentation::add_rule_no_checks(p, {s[i], e[j]}, {e[j], s[i]});
        }
      }

      for (size_t i = 0; i < l; ++i) {
        for (size_t j = i + 1; j < l + 1; ++j) {
          presentation::add_rule_no_checks(p, {s[i], e[j]}, {e[j], s[i]});
          presentation::add_rule_no_checks(p, {s[i], e[j]}, {e[j]});
        }
      }

      for (size_t i = 0; i < l + 1; ++i) {
        for (size_t j = 0; j < l + 1; ++j) {
          presentation::add_rule_no_checks(p, {e[i], e[j]}, {e[j], e[i]});
          presentation::add_rule_no_checks(
              p, {e[i], e[j]}, {e[std::max(i, j)]});
        }
      }

      for (size_t i = 0; i < l; ++i) {
        presentation::add_rule_no_checks(p, {e[i], s[i], e[i]}, {e[i + 1]});
      }
    }

    void add_renner_type_D_common(Presentation<word_type>& p, size_t l, int q) {
      LIBSEMIGROUPS_ASSERT(q == 0 || q == 1);
      // q is supposed to be 0 or 1
      auto        s = range(l);
      auto        e = range(l, 2 * l + 1);
      letter_type f = 2 * l + 1;

      if (q == 0) {
        presentation::add_idempotent_rules_no_checks(p, s);
      } else {
        presentation::add_involution_rules_no_checks(p, s);
      }

      for (size_t i = 1; i < l; ++i) {  // tout sauf \pi_1^f
        for (size_t j = i + 2; j < l; ++j) {
          presentation::add_rule_no_checks(p, {s[i], s[j]}, {s[j], s[i]});
        }
      }
      for (size_t i = 3; i < l; ++i) {  // \pi_1^f avec les \pi_i pour i>2
        presentation::add_rule_no_checks(p, {s[0], s[i]}, {s[i], s[0]});
      }
      presentation::add_rule_no_checks(
          p, {s[0], s[1]}, {s[1], s[0]});  // \pi_1^f avec \pi_1^e

      for (size_t i = 1; i < l - 1; ++i) {  // tresses
        presentation::add_rule_no_checks(
            p, {s[i], s[i + 1], s[i]}, {s[i + 1], s[i], s[i + 1]});
      }
      if (l > 2) {
        presentation::add_rule_no_checks(
            p, {s[0], s[2], s[0]}, {s[2], s[0], s[2]});  // tresse de \pi_1^f
      }

      for (size_t i = 2; i < l; ++i) {  // commutation, attention début à 2
        for (size_t j = 0; j < i; ++j) {
          presentation::add_rule_no_checks(p, {s[i], e[j]}, {e[j], s[i]});
        }
        presentation::add_rule_no_checks(p, {s[i], f}, {f, s[i]});
      }
      presentation::add_rule_no_checks(
          p, {s[0], f}, {f, s[0]});  // commutation \pi_1^f et f
      presentation::add_rule_no_checks(
          p, {s[1], e[0]}, {e[0], s[1]});  // commutation \pi_1^e et e

      for (size_t i = 1; i < l; ++i) {  // absorption
        for (size_t j = i + 1; j < l + 1; ++j) {
          presentation::add_rule_no_checks(p, {s[i], e[j]}, {e[j], s[i]});
          presentation::add_rule_no_checks(p, {s[i], e[j]}, {e[j]});
          if (i == 1) {  // cas particulier \pi_1^f
            presentation::add_rule_no_checks(p, {s[0], e[j]}, {e[j], s[0]});
            presentation::add_rule_no_checks(p, {s[0], e[j]}, {e[j]});
          }
        }
      }

      for (size_t i = 0; i < l + 1; ++i) {  // produit e_i
        for (size_t j = 0; j < l + 1; ++j) {
          presentation::add_rule_no_checks(p, {e[i], e[j]}, {e[j], e[i]});
          presentation::add_rule_no_checks(
              p, {e[i], e[j]}, {e[std::max(i, j)]});
        }
        if (i > 1) {
          presentation::add_rule_no_checks(p, {f, e[i]}, {e[i], f});
          presentation::add_rule_no_checks(p, {f, e[i]}, {e[i]});
        }
      }
      presentation::add_rule_no_checks(p, {f, f}, {f});
      presentation::add_rule_no_checks(p, {f, e[0]}, {e[1]});
      presentation::add_rule_no_checks(p, {e[0], f}, {e[1]});

      for (size_t i = 2; i < l; ++i) {
        presentation::add_rule_no_checks(p, {e[i], s[i], e[i]}, {e[i + 1]});
      }
      presentation::add_rule_no_checks(p, {e[0], s[0], e[0]}, {e[2]});
      presentation::add_rule_no_checks(p, {f, s[1], f}, {e[2]});
    }

    void add_coxeter_common(Presentation<word_type>& p, size_t n) {
      for (letter_type i = 0; i < n - 2; ++i) {
        presentation::add_rule_no_checks(p, {i, i + 1, i}, {i + 1, i, i + 1});
      }
      for (letter_type i = 0; i < n - 1; ++i) {
        for (letter_type j = i + 2; j < n - 1; ++j) {
          presentation::add_rule_no_checks(p, {i, j}, {j, i});
        }
      }
    }

    void add_rook_monoid_common(Presentation<word_type>& p, size_t n) {
      using words::pow;
      presentation::add_commutes_rules_no_checks(p, {n - 1}, range(1, n - 1));
      presentation::add_rule_no_checks(
          p, pow({0, n - 1}, 2), pow({n - 1, 0}, 2));
      presentation::add_rule_no_checks(
          p, pow({0, n - 1}, 2), {n - 1, 0, n - 1});
    }

    word_type max_elt_B(size_t i) {
      word_type t(0);
      for (int end = i; end >= 0; end--) {
        for (int k = 0; k <= end; k++) {
          t.push_back(k);
        }
      }
      return t;
    }

    word_type max_elt_D(size_t i, int g) {
      // g est 0 ou 1 : 0 pour f et 1 pour e
      word_type t(0);
      int       parity = g;
      for (int end = i; end > 0; end--) {
        t.push_back(parity);
        for (int k = 2; k <= end; k++) {
          t.push_back(k);
        }
        parity = (parity + 1) % 2;
      }
      return t;
    }
  }  // namespace

  namespace presentation::examples {

    using literals::operator""_w;
    using words::pow;
    using words::prod;
    using words::operator+;
    using words::operator+=;

    Presentation<word_type> stellar_monoid_GH19(size_t l) {
      if (l < 2) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 1st argument to be at least 2, found {}", l);
      }
      auto p = zero_rook_monoid(l);

      word_type t = {l - 1};
      for (size_t i = 0; i < l - 1; ++i) {
        t.insert(t.begin(), i);
        presentation::add_rule_no_checks(p, t + word_type({i}), t);
      }
      p.contains_empty_word(true);
      return p;
    }

    Presentation<word_type> fibonacci_semigroup_CRRT94(size_t r, size_t n) {
      if (r == 0) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 1st argument to be strictly positive, found {}", r);
      } else if (n == 0) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 2nd argument to be strictly positive, found {}", n);
      }
      Presentation<word_type> p;

      for (size_t i = 0; i < n; ++i) {
        word_type lhs(r, 0);
        std::iota(lhs.begin(), lhs.end(), i);
        std::for_each(lhs.begin(), lhs.end(), [&n](size_t& x) { x %= n; });
        presentation::add_rule_no_checks(p, lhs, word_type({(i + r) % n}));
      }
      p.alphabet_from_rules();
      return p;
    }

    // Theorem 6 of https://doi.org/10.2140/pjm.1970.34.709 (Knuth), and Section
    // 3 of https://doi.org/10.1007/s00233-022-10285-3 (Abram, Reutenauer)
    Presentation<word_type> plactic_monoid_Knu70(size_t n) {
      if (n < 1) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 1st argument to be at least 1, found {}", n);
      }
      Presentation<word_type> p;
      p.alphabet(n);
      if (n == 1) {
        return p;
      }
      for (size_t c = 0; c < n; ++c) {
        for (size_t b = 0; b < c; ++b) {
          for (size_t a = 0; a < b; ++a) {
            presentation::add_rule_no_checks(p, {b, a, c}, {b, c, a});
            presentation::add_rule_no_checks(p, {a, c, b}, {c, a, b});
          }
        }
      }
      for (size_t b = 0; b < n; ++b) {
        for (size_t a = 0; a < b; ++a) {
          presentation::add_rule_no_checks(p, {b, a, a}, {a, b, a});
          presentation::add_rule_no_checks(p, {b, b, a}, {b, a, b});
        }
      }
      p.contains_empty_word(true);
      return p;
    }

    Presentation<word_type> stylic_monoid_AR22(size_t n) {
      if (n < 2) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 1st argument to be at least 2, found {}", n);
      }
      auto p = plactic_monoid_Knu70(n);
      presentation::add_idempotent_rules_no_checks(p, range(n));
      p.contains_empty_word(true);
      return p;
    }

    // This presentation is due to Burnside [p. 464] and Miller [p. 366] in 1911
    // See Eq 2.6 of 'Presentations of finite simple groups: A quantitative
    // approach' J. Amer. Math. Soc. 21 (2008), 711-774
    Presentation<word_type> symmetric_group_Bur12(size_t n) {
      if (n < 2) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 1st argument to be at least 2, found {}", n);
      }
      Presentation<word_type> p;
      presentation::add_involution_rules_no_checks(p, range(n - 1));

      for (size_t i = 0; i <= n - 2; ++i) {
        for (size_t j = 0; j <= n - 2; ++j) {
          if (i != j) {
            presentation::add_rule_no_checks(p, pow({i, j}, 3), {});
          }
        }
      }

      for (size_t i = 0; i <= n - 2; ++i) {
        for (size_t j = 0; j <= n - 2; ++j) {
          if (i != j) {
            for (size_t k = 0; k <= n - 2; ++k) {
              if (k != i && k != j) {
                presentation::add_rule_no_checks(p, pow({i, j, i, k}, 2), {});
              }
            }
          }
        }
      }
      p.alphabet_from_rules();
      return p;
    }

    // Carmichael pg. 169 and Exercise 9.5.2, p172 of
    // https://link.springer.com/book/10.1007/978-1-84800-281-4
    Presentation<word_type> symmetric_group_Car56(size_t n) {
      if (n < 2) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 1st argument to be at least 2, found {}", n);
      }
      Presentation<word_type> p;
      presentation::add_involution_rules_no_checks(p, range(n - 1));
      for (size_t i = 0; i < n - 2; ++i) {
        presentation::add_rule_no_checks(p, pow({i, i + 1}, 3), {});
      }
      if (n != 2) {
        presentation::add_rule_no_checks(p, pow({n - 2, 0}, 3), {});
      }
      for (size_t i = 0; i < n - 2; ++i) {
        for (size_t j = 0; j < i; ++j) {
          presentation::add_rule_no_checks(p, pow({i, i + 1, i, j}, 2), {});
        }
        for (size_t j = i + 2; j <= n - 2; ++j) {
          presentation::add_rule_no_checks(p, pow({i, i + 1, i, j}, 2), {});
        }
      }
      for (size_t i = 1; i < n - 2; ++i) {
        presentation::add_rule_no_checks(p, pow({n - 2, 0, n - 2, i}, 2), {});
      }
      p.alphabet_from_rules();
      return p;
    }

    // From Theorm A in Moore 1897
    Presentation<word_type> symmetric_group_Moo97_a(size_t n) {
      if (n < 2) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 1st argument to be at least 2, found {}", n);
      }
      Presentation<word_type> p;
      presentation::add_involution_rules_no_checks(p, range(n - 1));
      add_coxeter_common(p, n);
      p.alphabet_from_rules();
      return p;
    }

    // From Theorm A' in Moore 1897
    Presentation<word_type> symmetric_group_Moo97_b(size_t n) {
      if (n < 2) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 1st argument to be at least 2, found {}", n);
      }
      Presentation<word_type> p;
      presentation::add_rule_no_checks(p, 00_w, {});
      presentation::add_rule_no_checks(p, pow(1_w, n), {});
      presentation::add_rule_no_checks(p, pow(01_w, n - 1), {});
      presentation::add_rule_no_checks(
          p, pow(0_w + pow(1_w, n - 1) + 01_w, 3), {});
      for (size_t j = 2; j <= n - 2; ++j) {
        presentation::add_rule_no_checks(
            p, pow(0_w + pow(pow(1_w, n - 1), j) + 0_w + pow(1_w, j), 2), {});
      }
      p.alphabet_from_rules();
      return p;
    }

    Presentation<word_type> alternating_group_Moo97(size_t n) {
      if (n < 4) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 1st argument to be at least 4, found {}", n);
      }
      Presentation<word_type> p;

      presentation::add_rule_no_checks(p, 000_w, {});

      presentation::add_involution_rules_no_checks(p, range(1, n - 2));

      for (size_t i = 1; i <= n - 3; ++i) {
        presentation::add_rule_no_checks(p, pow({i - 1, i}, 3), {});
      }

      for (size_t k = 2; k <= n - 3; ++k) {
        for (size_t j = 0; j <= k - 2; ++j) {
          presentation::add_rule_no_checks(p, {j, k, j, k}, {});
        }
      }
      p.alphabet_from_rules();
      return p;
    }

    Presentation<word_type> dual_symmetric_inverse_monoid_EEF07(size_t n) {
      if (n < 3) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 1st argument to be at least 3, found {}", n);
      }
      auto mij = [](size_t i, size_t j) {
        if (i == j) {
          return 1;
        } else if (std::abs(static_cast<int64_t>(i) - static_cast<int64_t>(j))
                   == 1) {
          return 3;
        } else {
          return 2;
        }
      };

      std::vector<word_type> alphabet;
      for (size_t i = 0; i < n; ++i) {
        alphabet.push_back({i});
      }
      auto                    x = alphabet.back();
      auto                    e = ""_w;
      Presentation<word_type> result;
      result.alphabet(n).contains_empty_word(true);

      auto s = alphabet.cbegin();

      // R1
      for (size_t i = 0; i < n - 1; ++i) {
        for (size_t j = 0; j < n - 1; ++j) {
          presentation::add_rule_no_checks(
              result, pow((s[i] + s[j]), mij(i, j)), e);
        }
      }
      // R2
      presentation::add_rule_no_checks(result, pow(x, 3), x);
      // R3
      presentation::add_rule_no_checks(result, x + s[0], x);
      presentation::add_rule_no_checks(result, s[0] + x, x);
      // R4
      presentation::add_rule_no_checks(
          result, x + s[1] + x, x + s[1] + x + s[1]);
      presentation::add_rule_no_checks(
          result, x + s[1] + x + s[1], s[1] + x + s[1] + x);
      presentation::add_rule_no_checks(
          result, s[1] + x + s[1] + x, x + s[1] + x + x);
      presentation::add_rule_no_checks(
          result, x + s[1] + x + x, x + x + s[1] + x);
      if (n == 3) {
        return result;
      }
      // R5
      word_type const sigma = s[1] + s[2] + s[0] + s[1];
      presentation::add_rule_no_checks(result,
                                       (x + x) + sigma + (x + x) + sigma,
                                       sigma + (x + x) + sigma + (x + x));
      presentation::add_rule_no_checks(result,
                                       sigma + (x + x) + sigma + (x + x),
                                       x + s[1] + s[2] + s[1] + x);
      // R6
      std::vector<word_type> l = {{}, {}, x + s[1] + s[0]};
      for (size_t i = 2; i < n - 1; ++i) {
        l.push_back(s[i] + l[i] + s[i] + s[i - 1]);
      }
      std::vector<word_type> y = {{}, {}, {}, x};
      for (size_t i = 3; i < n; ++i) {
        y.push_back(l[i] + y[i] + s[i - 1]);
      }
      for (size_t i = 2; i < n - 1; ++i) {
        presentation::add_rule_no_checks(
            result, y[i + 1] + s[i] + y[i + 1], s[i] + y[i + 1] + s[i]);
      }
      if (n == 4) {
        return result;
      }
      // R7
      for (size_t i = 3; i < n - 1; ++i) {
        presentation::add_rule_no_checks(result, x + s[i], s[i] + x);
      }
      return result;
    }

    Presentation<word_type> uniform_block_bijection_monoid_Fit03(size_t n) {
      if (n < 3) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 1st argument to be at least 3, found {}", n);
      }

      word_type t = {n - 1};

      Presentation<word_type> p;
      p.alphabet(n);
      p.contains_empty_word(true);

      // S in Theorem 3 (same as dual_symmetric_inverse_monoid)
      // TODO factor out the common part
      for (size_t i = 0; i <= n - 2; ++i) {
        presentation::add_rule_no_checks(p, {i, i}, {});
        if (i != n - 2) {
          presentation::add_rule_no_checks(p, pow({i, i + 1}, 3), {});
        }
        if (i != 0) {
          presentation::add_rule_no_checks(p, pow({i - 1, i}, 3), {});
          for (size_t j = 0; j < i - 1; ++j) {
            presentation::add_rule_no_checks(p, pow({i, j}, 2), {});
          }
        }
        for (size_t j = i + 2; j < n - 1; ++j) {
          presentation::add_rule_no_checks(p, pow({i, j}, 2), {});
        }
      }

      // F2
      presentation::add_rule_no_checks(p, pow(t, 2), t);

      // F3
      presentation::add_rule_no_checks(p, t + 0_w, t);
      presentation::add_rule_no_checks(p, 0_w + t, t);

      // F4
      presentation::add_commutes_rules_no_checks(p, range(2, n - 1), t);

      // F5
      presentation::add_rule_no_checks(p, 1_w + t + 1_w + t, t + 1_w + t + 1_w);

      // F6
      presentation::add_rule_no_checks(
          p, 1021_w + t + 1201_w + t, t + 1021_w + t + 1201_w);

      return p;
    }

    Presentation<word_type> partition_monoid_HR05(size_t n) {
      if (n < 1) {
        LIBSEMIGROUPS_EXCEPTION(
            "the 1st argument (size_t) must be at least 1, found {}", n);
      }

      Presentation<word_type> p;

      std::vector<word_type> s(n), r(n), q(n);
      for (size_t i = 0; i < n; ++i) {
        if (i != n - 1) {
          s[i] = {i};
          q[i] = {i + n - 1};
        }
        r[i] = {i + 2 * n - 2};
      }

      p.alphabet(3 * n - 2);
      p.contains_empty_word(true);

      for (size_t i = 0; i < n - 1; ++i) {
        presentation::add_rule_no_checks(p, pow(s[i], 2), {});    // (Q1)
        presentation::add_rule_no_checks(p, pow(r[i], 2), r[i]);  // (Q4)
        presentation::add_rule_no_checks(
            p, s[i] + r[i], r[i + 1] + s[i]);  // (Q7)
        presentation::add_rule_no_checks(
            p, r[i] + r[i + 1] + s[i], r[i] + r[i + 1]);                // (Q8)
        presentation::add_rule_no_checks(p, pow(q[i], 2), q[i]);        // (Q9)
        presentation::add_rule_no_checks(p, q[i] + s[i], s[i] + q[i]);  // (Q13)
        presentation::add_rule_no_checks(p, q[i] + s[i], q[i]);         // (Q13)
        presentation::add_rule_no_checks(p, q[i] + r[i] + q[i], q[i]);  // (Q15)
        presentation::add_rule_no_checks(
            p, q[i] + r[i + 1] + q[i], q[i]);                           // (Q15)
        presentation::add_rule_no_checks(p, r[i] + q[i] + r[i], r[i]);  // (Q16)
        presentation::add_rule_no_checks(
            p, r[i + 1] + q[i] + r[i + 1], r[i + 1]);  // (Q16)
      }
      presentation::add_rule_no_checks(p, pow(r[n - 1], 2), r[n - 1]);  // (Q4)

      int64_t m = n;
      for (int64_t i = 0; i < m - 1; ++i) {
        for (int64_t j = 0; j < m - 1; ++j) {
          auto d = std::abs(i - j);
          if (d > 1) {
            presentation::add_rule_no_checks(
                p, s[i] + s[j], s[j] + s[i]);  // (Q2)
            presentation::add_rule_no_checks(
                p, s[i] + q[j], q[j] + s[i]);  // (Q11)
          } else if (d == 1) {
            presentation::add_rule_no_checks(
                p, s[i] + s[j] + s[i], s[j] + s[i] + s[j]);  // (Q3)
            presentation::add_rule_no_checks(
                p, s[i] + s[j] + q[i], q[j] + s[i] + s[j]);  // (Q12)
          }
          if ((j != i) && (j != i + 1)) {
            presentation::add_rule_no_checks(
                p, s[i] + r[j], r[j] + s[i]);  // (Q6)
            presentation::add_rule_no_checks(
                p, q[i] + r[j], r[j] + q[i]);  // (Q14)
          }
          if (j != i) {
            presentation::add_rule_no_checks(
                p, r[i] + r[j], r[j] + r[i]);  // (Q5)
            presentation::add_rule_no_checks(
                p, q[i] + q[j], q[j] + q[i]);  // (Q10)
          }
        }
        if ((m - 1 != i) && (m - 1 != i + 1)) {
          presentation::add_rule_no_checks(
              p, s[i] + r[n - 1], r[n - 1] + s[i]);  // (Q6)
          presentation::add_rule_no_checks(
              p, q[i] + r[n - 1], r[n - 1] + q[i]);  // (Q14)
        }
        presentation::add_rule_no_checks(
            p, r[i] + r[n - 1], r[n - 1] + r[i]);  // (Q5)
      }
      return p;
    }

    Presentation<word_type> partition_monoid_Eas11(size_t n) {
      if (n < 4) {
        LIBSEMIGROUPS_EXCEPTION(
            "the 1st argument (degree) must be at least 4, found {}", n);
      }

      Presentation<word_type> p;

      p.alphabet(4);
      p.contains_empty_word(true);

      // V1
      presentation::add_rule_no_checks(p, pow(1_w, n), {});
      presentation::add_rule_no_checks(p, pow(01_w, n - 1), {});
      presentation::add_rule_no_checks(p, 00_w, {});
      for (size_t i = 2; i <= n / 2; ++i) {
        presentation::add_rule_no_checks(
            p, pow(pow(1_w, i) + 0_w + pow(1_w, n - i) + 0_w, 2), {});
      }

      // V2
      presentation::add_rule_no_checks(p, 22_w, 2_w);
      presentation::add_rule_no_checks(p, 232_w, 2_w);
      presentation::add_rule_no_checks(p, 012_w + pow(1_w, n - 1) + 0_w, 2_w);
      presentation::add_rule_no_checks(
          p, 10_w + pow(1_w, n - 1) + 210_w + pow(1_w, n - 1), 2_w);

      // V3
      presentation::add_rule_no_checks(p, 33_w, 3_w);
      presentation::add_rule_no_checks(p, 323_w, 3_w);
      presentation::add_rule_no_checks(p, 30_w, 3_w);
      presentation::add_rule_no_checks(p, 03_w, 3_w);
      presentation::add_rule_no_checks(
          p, 110_w + pow(1_w, n - 2) + 3110_w + pow(1_w, n - 2), 3_w);
      presentation::add_rule_no_checks(p,
                                       pow(1_w, n - 1) + 010_w + pow(1_w, n - 1)
                                           + 310_w + pow(1_w, n - 1) + 01_w,
                                       3_w);

      // V4
      presentation::add_rule_no_checks(p, 0202_w, 202_w);
      presentation::add_rule_no_checks(p, 2020_w, 202_w);

      // V5
      presentation::add_rule_no_checks(
          p, 313_w + pow(1_w, n - 1), 13_w + pow(1_w, n - 1) + 3_w);

      // V6
      presentation::add_rule_no_checks(
          p, 3113_w + pow(1_w, n - 2), 113_w + pow(1_w, n - 2) + 3_w);
      // V7
      presentation::add_rule_no_checks(
          p, 3112_w + pow(1_w, n - 2), 112_w + pow(1_w, n - 2) + 3_w);
      return p;
    }

    // From Theorem 5 in 10.21136/MB.2007.134125
    // https://dml.cz/bitstream/handle/10338.dmlcz/134125/MathBohem_132-2007-3_6.pdf
    Presentation<word_type> singular_brauer_monoid_MM07(size_t n) {
      if (n < 3) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 1st argument to be at least 3, found {}", n);
      }
      std::vector<std::vector<word_type>> t;
      size_t                              value = 0;
      for (size_t i = 0; i < n; ++i) {
        t.push_back({});
        for (size_t j = 0; j < n; ++j) {
          if (i != j) {
            t.back().push_back({value});
            value++;
          } else {
            t.back().push_back({0});
          }
        }
      }
      Presentation<word_type> p;

      // (3) + (4)
      for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
          if (i != j) {
            presentation::add_rule_no_checks(p, t[i][j], t[j][i]);
            presentation::add_rule_no_checks(p, pow(t[i][j], 2), t[i][j]);
          }
        }
      }

      // (6) + (7)
      for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
          for (size_t k = 0; k < n; ++k) {
            if (i != j && j != k && i != k) {
              presentation::add_rule_no_checks(
                  p, t[i][j] + t[i][k] + t[j][k], t[i][j] + t[j][k]);
              presentation::add_rule_no_checks(
                  p, t[i][j] + t[j][k] + t[i][j], t[i][j]);
            }
          }
        }
      }

      // (5) + (8) + (9)
      for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
          for (size_t k = 0; k < n; ++k) {
            for (size_t l = 0; l < n; ++l) {
              if (i != j && j != k && i != k && i != l && j != l && k != l) {
                presentation::add_rule_no_checks(p,
                                                 t[i][j] + t[j][k] + t[k][l],
                                                 t[i][j] + t[i][l] + t[k][l]);
                presentation::add_rule_no_checks(p,
                                                 t[i][j] + t[k][l] + t[i][k],
                                                 t[i][j] + t[j][l] + t[i][k]);
                presentation::add_rule_no_checks(
                    p, t[i][j] + t[k][l], t[k][l] + t[i][j]);
              }
            }
          }
        }
      }
      p.alphabet_from_rules();
      p.contains_empty_word(true);
      return p;
    }

    // From https://doi.org/10.1007/s10012-000-0001-1
    Presentation<word_type> orientation_preserving_monoid_AR00(size_t n) {
      if (n < 3) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 1st argument to be at least 3, found {}", n);
      }
      Presentation<word_type> p;
      p.alphabet(2);
      p.contains_empty_word(true);

      presentation::add_rule_no_checks(p, pow(0_w, n), {});
      presentation::add_rule_no_checks(p, 11_w, 1_w);
      presentation::add_rule_no_checks(p, pow(10_w, n), 10_w);
      presentation::add_rule_no_checks(p,
                                       0_w + pow(1_w + pow(0_w, n - 1), n - 1),
                                       pow(1_w + pow(0_w, n - 1), n - 1));
      for (size_t i = 2; i <= n - 1; ++i) {
        presentation::add_rule_no_checks(
            p,
            1_w + pow(0_w, i) + pow(10_w, n - 1) + pow(0_w, n - i),
            pow(0_w, i) + pow(10_w, n - 1) + pow(0_w, n - i) + 1_w);
      }
      return p;
    }

    // Also from https://doi.org/10.1007/s10012-000-0001-1
    Presentation<word_type>
    orientation_preserving_reversing_monoid_AR00(size_t n) {
      if (n < 3) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 1st argument to be at least 3, found {}", n);
      }
      Presentation<word_type> p;
      p.alphabet(3);
      p.contains_empty_word(true);

      presentation::add_rule_no_checks(p, pow(0_w, n), {});
      presentation::add_rule_no_checks(p, 11_w, 1_w);
      presentation::add_rule_no_checks(p, pow(10_w, n), 10_w);
      presentation::add_rule_no_checks(p,
                                       0_w + pow(1_w + pow(0_w, n - 1), n - 1),
                                       pow(1_w + pow(0_w, n - 1), n - 1));

      for (size_t i = 2; i <= n - 1; ++i) {
        presentation::add_rule_no_checks(
            p,
            1_w + pow(0_w, i) + pow(10_w, n - 1) + pow(0_w, n - i),
            pow(0_w, i) + pow(10_w, n - 1) + pow(0_w, n - i) + 1_w);
      }
      presentation::add_rule_no_checks(p, 22_w, {});
      presentation::add_rule_no_checks(p, 02_w, 2_w + pow(0_w, n - 1));
      presentation::add_rule_no_checks(p, 12_w, 2_w + pow(01_w, n - 1));

      presentation::add_rule_no_checks(p,
                                       2_w + pow(1_w + pow(0_w, n - 1), n - 2),
                                       pow(0_w, n - 2)
                                           + pow(1_w + pow(0_w, n - 1), n - 2));

      return p;
    }

    // From Theorem 2.2 in https://doi.org/10.1093/qmath/haab001
    Presentation<word_type> temperley_lieb_monoid_Eas21(size_t n) {
      if (n < 3) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 1st argument to be at least 3, found {}", n);
      }

      Presentation<word_type> p;
      p.alphabet(n - 1);
      p.contains_empty_word(true);

      // E1
      presentation::add_idempotent_rules_no_checks(p, range(0, n - 1));

      int64_t m = n;
      // E2 + E3
      for (int64_t i = 0; i < m - 1; ++i) {
        for (int64_t j = 0; j < m - 1; ++j) {
          auto d = std::abs(i - j);
          if (d > 1) {
            presentation::add_rule_no_checks(p, {i, j}, {j, i});
          } else if (d == 1) {
            presentation::add_rule_no_checks(p, {i, j, i}, {i});
          }
        }
      }
      return p;
    }

    // From Theorem 3.1 in
    // https://link.springer.com/content/pdf/10.2478/s11533-006-0017-6.pdf
    Presentation<word_type> brauer_monoid_KM07(size_t n) {
      if (n < 1) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 1st argument to be at least 1, found {}", n);
      }
      std::vector<word_type> s(n), t(n);
      for (size_t i = 0; i < n - 1; ++i) {
        s[i] = {i};
        t[i] = {i + n - 1};
      }

      Presentation<word_type> p;
      p.alphabet(2 * n - 2);
      p.contains_empty_word(true);

      // E1
      for (size_t i = 0; i < n - 1; ++i) {
        presentation::add_rule_no_checks(p, pow(s[i], 2), {});
        presentation::add_rule_no_checks(p, pow(t[i], 2), t[i]);
        presentation::add_rule_no_checks(p, t[i] + s[i], s[i] + t[i]);
        presentation::add_rule_no_checks(p, s[i] + t[i], t[i]);
      }

      // E2 + E3
      int64_t m = n;
      for (int64_t i = 0; i < m - 1; ++i) {
        for (int64_t j = 0; j < m - 1; ++j) {
          auto d = std::abs(i - j);
          if (d > 1) {
            presentation::add_rule_no_checks(p, s[i] + s[j], s[j] + s[i]);
            presentation::add_rule_no_checks(p, t[i] + t[j], t[j] + t[i]);
            presentation::add_rule_no_checks(p, t[i] + s[j], s[j] + t[i]);
          } else if (d == 1) {
            presentation::add_rule_no_checks(
                p, s[i] + s[j] + s[i], s[j] + s[i] + s[j]);
            presentation::add_rule_no_checks(p, t[i] + t[j] + t[i], t[i]);
            presentation::add_rule_no_checks(
                p, s[i] + t[j] + t[i], s[j] + t[i]);
            presentation::add_rule_no_checks(
                p, t[i] + t[j] + s[i], t[i] + s[j]);
          }
        }
      }

      return p;
    }

    // From Theorem 5.1 in
    // https://link.springer.com/content/pdf/10.2478/s11533-006-0017-6.pdf
    Presentation<word_type> partial_brauer_monoid_KM07(size_t n) {
      if (n < 1) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 1st argument to be at least 1, found {}", n);
      }

      std::vector<word_type> s(n), t(n), v(n);

      for (size_t i = 0; i < n - 1; ++i) {
        s[i] = {i};
        t[i] = {i + n - 1};
        v[i] = {i + 2 * n - 2};
      }
      v[n - 1] = {3 * n - 3};

      Presentation<word_type> p = brauer_monoid_KM07(n);
      p.alphabet(3 * n - 2);

      // 15 from p14
      for (size_t i = 0; i < n; ++i) {
        presentation::add_rule_no_checks(p, v[i] + v[i], v[i]);
        for (size_t j = 0; j < n; ++j) {
          if (j != i) {
            presentation::add_rule_no_checks(p, v[i] + v[j], v[j] + v[i]);
          }
        }
      }

      // 16 from p14
      for (size_t i = 0; i < n - 1; ++i) {
        presentation::add_rule_no_checks(p, s[i] + v[i], v[i + 1] + s[i]);
      }

      for (size_t i = 0; i < n - 1; ++i) {
        for (size_t j = 0; j < n; ++j) {
          if (j != i && j != i + 1) {
            presentation::add_rule_no_checks(p, s[i] + v[j], v[j] + s[i]);
            // 17 on p15
            presentation::add_rule_no_checks(p, t[i] + v[j], v[j] + t[i]);
          }
        }
      }

      // 16 on p14
      for (size_t i = 0; i < n - 1; ++i) {
        presentation::add_rule_no_checks(
            p, v[i] + s[i] + v[i], v[i] + v[i + 1]);
      }

      for (size_t i = 0; i < n - 1; ++i) {
        // 18
        presentation::add_rule_no_checks(p, t[i] + v[i], t[i] + v[i + 1]);
        // 18
        presentation::add_rule_no_checks(p, v[i] + t[i], v[i + 1] + t[i]);
        // 19
        presentation::add_rule_no_checks(p, t[i] + v[i] + t[i], t[i]);
        // 19
        presentation::add_rule_no_checks(
            p, v[i] + t[i] + v[i], v[i] + v[i + 1]);
      }
      return p;
    }

    // From Theorem 4.1 in
    // https://arxiv.org/pdf/1301.4518
    Presentation<word_type> motzkin_monoid_PHL13(size_t n) {
      if (n < 1) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 1st argument to be at least 1, found {}", n);
      }

      Presentation<word_type> p;

      if (n == 1) {
        p.alphabet(1);
        p.contains_empty_word(true);
        p.rules = {00_w, 0_w};
        return p;
      }

      std::vector<word_type> t(n), r(n), l(n);
      for (size_t i = 0; i < n - 1; ++i) {
        t[i] = {i};
        r[i] = {i + n - 1};
        l[i] = {i + 2 * n - 2};
      }

      p.alphabet(3 * n - 3);
      p.contains_empty_word(true);

      for (size_t i = 0; i < n - 1; ++i) {
        // eqn (1)
        presentation::add_rule_no_checks(p, pow(r[i], 3), pow(r[i], 2));
        presentation::add_rule_no_checks(p, pow(l[i], 3), pow(l[i], 2));
        presentation::add_rule_no_checks(p, pow(r[i], 2), pow(l[i], 2));
        // eqn (3)
        presentation::add_rule_no_checks(p, l[i] + r[i] + l[i], l[i]);
        presentation::add_rule_no_checks(p, r[i] + l[i] + r[i], r[i]);
        // eqn (7)
        presentation::add_rule_no_checks(p, pow(t[i], 2), t[i]);
        // eqn (9)
        presentation::add_rule_no_checks(p, t[i] + l[i], t[i] + r[i]);
        presentation::add_rule_no_checks(p, l[i] + t[i], r[i] + t[i]);
        // eqn (12)
        presentation::add_rule_no_checks(p, t[i] + l[i] + t[i], t[i]);
        // Fix for the paper, to make Lemma 4.10 work
        presentation::add_rule_no_checks(p, r[i] + t[i] + l[i], r[i] + r[i]);
      }

      for (size_t i = 0; i < n - 2; ++i) {
        // eqn (2)
        presentation::add_rule_no_checks(
            p, l[i] + l[i + 1] + l[i], l[i] + l[i + 1]);
        presentation::add_rule_no_checks(
            p, l[i + 1] + l[i] + l[i + 1], l[i] + l[i + 1]);
        presentation::add_rule_no_checks(
            p, r[i] + r[i + 1] + r[i], r[i + 1] + r[i]);
        presentation::add_rule_no_checks(
            p, r[i + 1] + r[i] + r[i + 1], r[i + 1] + r[i]);
        // eqn (4)
        presentation::add_rule_no_checks(
            p, l[i + 1] + r[i] + l[i], l[i + 1] + r[i]);
        presentation::add_rule_no_checks(
            p, r[i] + l[i + 1] + r[i + 1], r[i] + l[i + 1]);
        presentation::add_rule_no_checks(
            p, r[i] + l[i] + r[i + 1], l[i] + r[i + 1]);
        presentation::add_rule_no_checks(
            p, l[i + 1] + r[i + 1] + l[i], r[i + 1] + l[i]);
        // eqn (5)
        presentation::add_rule_no_checks(p, l[i] + r[i], r[i + 1] + l[i + 1]);
        // eqn (10)
        presentation::add_rule_no_checks(
            p, t[i] + r[i + 1], t[i] + t[i + 1] + l[i]);
        presentation::add_rule_no_checks(
            p, l[i + 1] + t[i], r[i] + t[i + 1] + t[i]);
        // eqn (11)
        presentation::add_rule_no_checks(
            p, r[i] + r[i + 1] + t[i], t[i + 1] + r[i] + r[i + 1]);
        presentation::add_rule_no_checks(
            p, t[i] + l[i + 1] + l[i], l[i + 1] + l[i] + t[i + 1]);
      }

      int64_t m = n;
      for (int64_t i = 0; i < m - 1; ++i) {
        for (int64_t j = 0; j < m - 1; ++j) {
          auto d = std::abs(i - j);
          if (d > 1) {
            // eqn (6)
            presentation::add_rule_no_checks(p, r[i] + l[j], l[j] + r[i]);
            presentation::add_rule_no_checks(p, r[i] + r[j], r[j] + r[i]);
            presentation::add_rule_no_checks(p, l[i] + l[j], l[j] + l[i]);
            presentation::add_rule_no_checks(p, t[i] + r[j], r[j] + t[i]);
            presentation::add_rule_no_checks(p, t[i] + l[j], l[j] + t[i]);
            presentation::add_rule_no_checks(p, t[i] + t[j], t[j] + t[i]);
          } else if (d == 1) {
            // eqn (8)
            presentation::add_rule_no_checks(p, t[i] + t[j] + t[i], t[i]);
          }
        }
      }
      return p;
    }

    // From Proposition 4.2 in
    // https://link.springer.com/content/pdf/10.1007/s002339910016.pdf
    Presentation<word_type> rectangular_band_ACOR00(size_t m, size_t n) {
      if (m == 0) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 1st argument to be strictly positive, found {}", m);
      }
      if (n == 0) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 2nd argument to be strictly positive, found {}", n);
      }

      std::vector<word_type> a(m);
      std::vector<word_type> b(n);
      for (size_t i = 0; i < m; ++i) {
        a[i] = {i};
      }
      for (size_t i = 0; i < n; ++i) {
        b[i] = {i + m};
      }

      Presentation<word_type> p;
      presentation::add_rule_no_checks(p, a[0], b[0]);
      p.alphabet(m + n);

      // (7)
      for (size_t i = 1; i < m; ++i) {
        presentation::add_rule_no_checks(p, a[i - 1] + a[i], a[i - 1]);
      }

      presentation::add_rule_no_checks(p, a[m - 1] + a[0], a[m - 1]);

      // (8)
      for (size_t i = 1; i < n; ++i) {
        presentation::add_rule_no_checks(p, b[i - 1] + b[i], b[i]);
      }

      presentation::add_rule_no_checks(p, b[n - 1] + b[0], b[0]);

      for (size_t i = 1; i < m; ++i) {
        for (size_t j = 1; j < n; ++j) {
          presentation::add_rule_no_checks(p, b[j] + a[i], a[0]);
        }
      }
      return p;
    }

    // From Section 5, Theorem 2 Aizenstat (Russian) or Proposition 1.7 in
    // https://bit.ly/3R5ZpKW (English)
    Presentation<word_type> full_transformation_monoid_Aiz58(size_t n) {
      if (n < 4) {
        LIBSEMIGROUPS_EXCEPTION(
            "the 1st argument (degree) must be at least 4, found {}", n);
      }
      Presentation<word_type> p;
      p = symmetric_group_Moo97_b(n);

      presentation::add_rule_no_checks(p, 02_w, 2_w);
      presentation::add_rule_no_checks(
          p, pow(1_w, n - 2) + 0112_w + pow(1_w, n - 2) + 011_w, 2_w);
      presentation::add_rule_no_checks(p,
                                       10_w + pow(1_w, n - 1) + 012_w
                                           + pow(1_w, n - 1) + 010_w
                                           + pow(1_w, n - 1),
                                       2_w);
      presentation::add_rule_no_checks(p, pow(210_w + pow(1_w, n - 1), 2), 2_w);
      presentation::add_rule_no_checks(
          p, pow(pow(1_w, n - 1) + 012_w, 2), 2_w + pow(1_w, n - 1) + 012_w);
      presentation::add_rule_no_checks(p,
                                       pow(2_w + pow(1_w, n - 1) + 01_w, 2),
                                       2_w + pow(1_w, n - 1) + 012_w);
      presentation::add_rule_no_checks(
          p,
          pow(210_w + pow(1_w, n - 2) + 01_w, 2),
          pow(10_w + (pow(1_w, n - 2)) + 012_w, 2));
      p.alphabet_from_rules();
      p.contains_empty_word(true);
      return p;
    }

    // From Theorem 9.3.1, p161-162, (Ganyushkin + Mazorchuk), proved in
    // Iwahori, using Theorem 9.1.4 to express presentation in terms of the
    // pi_i and e_12.
    // https://link.springer.com/book/10.1007/978-1-84800-281-4
    Presentation<word_type> full_transformation_monoid_II74(size_t n) {
      if (n < 4) {
        LIBSEMIGROUPS_EXCEPTION(
            "the 1st argument (degree) must be at least 4, found {}", n);
      }
      Presentation<word_type> p;
      p = symmetric_group_Car56(n);
      add_full_transformation_monoid_relations(p, n, 0, n - 1);
      p.alphabet_from_rules();
      p.contains_empty_word(true);
      return p;
    }

    Presentation<word_type> full_transformation_monoid(size_t n) {
      if (n < 2) {
        LIBSEMIGROUPS_EXCEPTION(
            "the 1st argument (degree) must be at least 2, found {}", n);
      } else if (n < 4) {
        Presentation<word_type> p;
        if (n == 2) {
          presentation::add_rule_no_checks(p, 00_w, ""_w);
          presentation::add_rule_no_checks(p, 01_w, 1_w);
          presentation::add_rule_no_checks(p, 11_w, 1_w);
          p.alphabet_from_rules();
          p.contains_empty_word(true);
          return p;
        } else if (n == 3) {
          presentation::add_rule_no_checks(p, 00_w, ""_w);
          presentation::add_rule_no_checks(p, 11_w, ""_w);
          presentation::add_rule_no_checks(p, 010_w, 101_w);
          presentation::add_rule_no_checks(p, 02_w, 2_w);
          presentation::add_rule_no_checks(p, 2121_w, 2_w);
          presentation::add_rule_no_checks(p, pow(0102_w, 3) + 010_w, 20102_w);
          p.alphabet_from_rules();
          p.contains_empty_word(true);
          return p;
        }
      }
      return full_transformation_monoid_MW24_a(n);
    }

    // From Theorem 1.5 of arXiv:2406.19294
    Presentation<word_type> full_transformation_monoid_MW24_a(size_t n) {
      if (n < 4) {
        LIBSEMIGROUPS_EXCEPTION(
            "the 1st argument (degree) must be at least 4, found {}", n);
      }

      auto p = symmetric_group_Car56(n);

      // From Theorem 1.5 of arXiv:2406.19294

      // Relation T1
      presentation::add_rule_no_checks(p, {n - 1, 1, n - 1, 1}, {n - 1});

      // Relation T3
      presentation::add_rule_no_checks(p, {1, 2, 1, n - 1}, {n - 1, 1, 2, 1});

      // Relation T7
      presentation::add_rule_no_checks(
          p,
          {n - 2, 0, 1, 0, n - 1, n - 2, 0, 1, 0, n - 1},
          {n - 1, n - 2, 0, 1, 0, n - 1, n - 2, 0, 1, 0});

      // Relation T8
      std::vector<size_t> gens(n - 1);  // list of generators to use prod on
      std::iota(gens.begin(), gens.end(), 0);
      presentation::add_rule_no_checks(
          p,
          prod(gens, 1, n - 1, 1) + word_type({1, 0, n - 1}),
          word_type({n - 1}) + prod(gens, 1, n - 1, 1) + word_type({1}));

      // Relation T9
      presentation::add_rule_no_checks(
          p,
          {0, 1, 0, n - 1, 0, 1, 0, n - 1, 0, 1, 0, n - 1, 0, 1, 0},
          {n - 1, 0, 1, 0, n - 1});

      p.alphabet_from_rules();
      p.contains_empty_word(true);
      return p;
    }

    // From Theorem 1.5 of arXiv:2406.19294
    Presentation<word_type> full_transformation_monoid_MW24_b(size_t n) {
      if (n < 5) {
        LIBSEMIGROUPS_EXCEPTION(
            "the 1st argument (degree) must be at least 5, found {}", n);
      } else if (n % 2 != 1) {
        LIBSEMIGROUPS_EXCEPTION(
            "the 1st argument (degree) must be odd, found {}", n);
      }
      auto p = symmetric_group_Car56(n);

      // Relation T7
      presentation::add_rule_no_checks(
          p,
          {n - 2, 0, 1, 0, n - 1, n - 2, 0, 1, 0, n - 1},
          {n - 1, n - 2, 0, 1, 0, n - 1, n - 2, 0, 1, 0});

      // Relation T9
      presentation::add_rule_no_checks(
          p,
          {0, 1, 0, n - 1, 0, 1, 0, n - 1, 0, 1, 0, n - 1, 0, 1, 0},
          {n - 1, 0, 1, 0, n - 1});

      // Relation T10
      presentation::add_rule_no_checks(
          p, {1, 2, 1, n - 1, 1, 2, 1}, {n - 1, 1, n - 1, 1});

      // Relation T8
      std::vector<size_t> gens(n - 1);  // list of generators to use prod on
      std::iota(gens.begin(), gens.end(), 0);
      presentation::add_rule_no_checks(
          p,
          word_type({1}) + prod(gens, n - 2, 0, -1) + word_type({n - 1})
              + prod(gens, 1, n - 1, 1) + word_type({1}),
          {0, n - 1, 1, n - 1, 1});

      p.alphabet_from_rules();
      p.contains_empty_word(true);
      return p;
    }

    // From Theorem 1.6 of https://doi.org/10.48550/arXiv.2406.19294
    Presentation<word_type> partial_transformation_monoid_MW24(size_t n) {
      if (n < 2) {
        LIBSEMIGROUPS_EXCEPTION(
            "the 1st argument (size_t) must be at least 2, found {}", n);
      }
      Presentation<word_type> p;

      p = symmetric_group_Car56(n);

      // Relation I3
      std::vector<size_t> gens(n - 1);  // list of generators to use prod on
      std::iota(gens.begin(), gens.end(), 0);
      presentation::add_rule_no_checks(
          p,
          prod(gens, 0, n - 1, 1) + word_type({0, n - 1}),
          word_type({n - 1}) + prod(gens, 0, n - 1, 1) + word_type({0}));

      // Relation T3
      presentation::add_rule_no_checks(p, {1, 2, 1, n}, {n, 1, 2, 1});

      // Relation T7
      presentation::add_rule_no_checks(p,
                                       {n - 2, 0, 1, 0, n, n - 2, 0, 1, 0, n},
                                       {n, n - 2, 0, 1, 0, n, n - 2, 0, 1, 0});

      // Relation T8
      presentation::add_rule_no_checks(
          p,
          prod(gens, 1, n - 1, 1) + word_type({1, 0, n}),
          word_type({n}) + prod(gens, 1, n - 1, 1) + word_type({1}));

      // Relation T9
      presentation::add_rule_no_checks(
          p, {0, 1, 0, n, 0, 1, 0, n, 0, 1, 0, n, 0, 1, 0}, {n, 0, 1, 0, n});

      // Relation P1
      presentation::add_rule_no_checks(p, {n, 0, n - 1, 0}, {n});

      // Relation P5
      presentation::add_rule_no_checks(p, {1, n - 1, 1, n}, {n, 1, n - 1, 1});

      // Relation P6
      presentation::add_rule_no_checks(
          p, {0, 1, 0, n - 1, 0, 1, 0}, {n - 1, 0, n, 0});

      // Relation P7
      presentation::add_rule_no_checks(p, {0, n - 1, 0, n - 1, 0}, {n, n - 1});

      p.alphabet_from_rules();
      return p;
    }

    // Due to Shutov 1960, as in Theorem 9.4.1, p169, (Ganyushkin + Mazorchuk)
    // https://link.springer.com/book/10.1007/978-1-84800-281-4
    Presentation<word_type> partial_transformation_monoid_Shu60(size_t n) {
      if (n < 4) {
        LIBSEMIGROUPS_EXCEPTION("the 1st argument must be at least 4, found {}",
                                n);
      }
      Presentation<word_type> p;
      p = symmetric_inverse_monoid_Shu60(n);
      p.alphabet(n + 1);
      add_full_transformation_monoid_relations(p, n, 0, n);
      presentation::add_rule_no_checks(p, {n, 0, n - 1, 0}, {n});
      presentation::add_rule_no_checks(p, {0, n - 1, 0, n}, {0, n - 1, 0});
      presentation::add_rule_no_checks(p, {n, n - 1}, {0, n - 1, 0, n - 1, n});
      presentation::add_rule_no_checks(p, {n, 1, n - 1, 1}, {1, n - 1, 1, n});
      p.alphabet_from_rules();
      return p;
    }

    Presentation<word_type> symmetric_inverse_monoid_Gay18(size_t n) {
      if (n < 2) {
        LIBSEMIGROUPS_EXCEPTION("the 1st argument must be at least 2, found {}",
                                n);
      }
      Presentation<word_type> p;
      p = symmetric_group_Moo97_a(n);
      p.alphabet(n);
      presentation::add_idempotent_rules_no_checks(p, {n - 1});
      add_rook_monoid_common(p, n);
      p.alphabet_from_rules();
      p.contains_empty_word(true);
      return p;
    }

    // From Theorem 9.2.2, p156
    // https://link.springer.com/book/10.1007/978-1-84800-281-4 (Ganyushkin +
    // Mazorchuk)
    Presentation<word_type> symmetric_inverse_monoid_Shu60(size_t n) {
      if (n < 4) {
        LIBSEMIGROUPS_EXCEPTION("the 1st argument must be at least 4, found {}",
                                n);
      }
      Presentation<word_type> p;
      p = symmetric_group_Car56(n);

      std::vector<word_type> pi, epsilon = {{n - 1}};
      for (size_t i = 0; i <= n - 2; ++i) {
        pi.push_back({i});
        epsilon.push_back(pi[i] + epsilon[0] + pi[i]);
      }

      presentation::add_rule_no_checks(
          p, pow(epsilon[0], 2), word_type(epsilon[0]));
      presentation::add_rule_no_checks(
          p, epsilon[0] + epsilon[1], epsilon[1] + epsilon[0]);

      for (size_t k = 1; k <= n - 2; ++k) {
        presentation::add_rule_no_checks(
            p, epsilon[1] + pi[k], pi[k] + epsilon[1]);
        presentation::add_rule_no_checks(
            p, epsilon[k + 1] + pi[0], pi[0] + epsilon[k + 1]);
      }
      presentation::add_rule_no_checks(
          p, epsilon[1] + epsilon[0] + pi[0], epsilon[1] + epsilon[0]);
      p.alphabet_from_rules();
      p.alphabet_from_rules();
      p.contains_empty_word(true);
      return p;
    }

    // From Theorem 1.4 of https://doi.org/10.48550/arXiv.2406.19294
    Presentation<word_type> symmetric_inverse_monoid_MW24(size_t n) {
      if (n < 4) {
        LIBSEMIGROUPS_EXCEPTION("the 1st argument must be at least 4, found {}",
                                n);
      }
      Presentation<word_type> p;

      p = symmetric_group_Car56(n);

      // Relation I2
      presentation::add_rule_no_checks(p, {0, 1, 0, n - 1}, {n - 1, 0, 1, 0});

      // Relation I6
      std::vector<size_t> gens(n - 1);  // list of generators to use prod on
      std::iota(gens.begin(), gens.end(), 0);
      presentation::add_rule_no_checks(
          p,
          prod(gens, 0, n - 1, 1) + word_type({0, n - 1}),
          word_type({n - 1, n - 1}) + prod(gens, 0, n - 1, 1) + word_type({0}));

      // Relation I7
      presentation::add_rule_no_checks(
          p, {0, n - 1, 0, n - 1, 0, n - 1, 0}, {n - 1, 0, n - 1});
      p.alphabet_from_rules();
      p.contains_empty_word(true);
      return p;
    }

    // Chinese monoid
    // See: The Chinese Monoid - Cassaigne, Espie, Krob, Novelli and Hivert,
    // 2001
    Presentation<word_type> chinese_monoid_CEKNH01(size_t n) {
      if (n < 2) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 1st argument to be at least 2, found {}", n);
      }
      Presentation<word_type> p;
      p.alphabet(n);
      p.contains_empty_word(true);
      for (size_t a = 0; a < n; a++) {
        for (size_t b = a; b < n; b++) {
          for (size_t c = b; c < n; c++) {
            if (a != b) {
              presentation::add_rule_no_checks(
                  p, word_type({c, b, a}), word_type({c, a, b}));
            }
            if (b != c) {
              presentation::add_rule_no_checks(
                  p, word_type({c, b, a}), word_type({b, c, a}));
            }
          }
        }
      }
      return p;
    }

    Presentation<word_type> monogenic_semigroup(size_t m, size_t r) {
      if (r == 0) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 2nd argument to be strictly positive, found {}", r);
      }
      Presentation<word_type> p;
      presentation::add_rule_no_checks(p, pow(0_w, m + r), pow(0_w, m));
      p.alphabet_from_rules();
      if (m == 0) {
        p.contains_empty_word(true);
      } else {
        p.contains_empty_word(false);
      }
      return p;
    }

    Presentation<word_type> order_preserving_monoid_AR00(size_t n) {
      if (n < 3) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 1st argument to be at least 3, found {}", n);
      }
      std::vector<word_type> u, v;

      for (size_t i = 0; i <= n - 2; ++i) {
        u.push_back({i});
        v.push_back({n - 1 + i});
      }

      Presentation<word_type> p;
      p.contains_empty_word(true);

      // relations 1
      for (size_t i = 1; i <= n - 2; ++i) {
        // relations 1
        presentation::add_rule_no_checks(
            p, v[n - 2 - i] + u[i], u[i] + v[n - 1 - i]);
        // relations 2
        presentation::add_rule_no_checks(
            p, u[n - 2 - i] + v[i], v[i] + u[n - 1 - i]);
      }

      for (size_t i = 0; i <= n - 2; ++i) {
        // relations 3
        presentation::add_rule_no_checks(p, v[n - 2 - i] + u[i], u[i]);
        // relations 4
        presentation::add_rule_no_checks(p, u[n - 2 - i] + v[i], v[i]);
      }

      // relations 5
      for (size_t i = 0; i <= n - 2; ++i) {
        for (size_t j = 0; j <= n - 2; ++j) {
          if (j != (n - 2) - i && j != n - i - 1) {
            presentation::add_rule_no_checks(p, u[i] + v[j], v[j] + u[i]);
          }
        }
      }

      // relation 6
      presentation::add_rule_no_checks(p, u[0] + u[1] + u[0], u[0] + u[1]);

      // relation 7
      presentation::add_rule_no_checks(p, v[0] + v[1] + v[0], v[0] + v[1]);

      p.alphabet_from_rules();

      return p;
    }

    // See Theorem 2.6 of https://arxiv.org/pdf/2211.02155.pdf
    Presentation<word_type> cyclic_inverse_monoid_Fer22_a(size_t n) {
      if (n < 3) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 1st argument to be at least 3, found {}", n);
      }
      Presentation<word_type> p;
      p.contains_empty_word(true);
      p.alphabet(n + 1);

      // R1
      presentation::add_rule_no_checks(p, pow(0_w, n), {});

      // R2
      presentation::add_idempotent_rules_no_checks(p, range(1, n + 1));

      // R3
      presentation::add_commutes_rules_no_checks(p, range(1, n + 1));

      // R4
      presentation::add_rule_no_checks(p, 01_w, word_type({n}) + 0_w);
      for (size_t i = 0; i < n - 1; ++i) {
        presentation::add_rule_no_checks(
            p, 0_w + word_type({i + 2}), word_type({i + 1}) + 0_w);
      }

      // R5
      presentation::add_rule_no_checks(p, range(0, n + 1), range(1, n + 1));
      return p;
    }

    // See Theorem 2.7 of https://arxiv.org/pdf/2211.02155.pdf
    Presentation<word_type> cyclic_inverse_monoid_Fer22_b(size_t n) {
      if (n < 3) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 1st argument to be at least 3, found {}", n);
      }
      Presentation<word_type> p;
      p.contains_empty_word(true);
      p.alphabet(2);

      presentation::add_rule_no_checks(p, pow(0_w, n), ""_w);  // relation Q1
      presentation::add_rule_no_checks(p, 11_w, 1_w);          // relation Q2

      // relations Q3
      for (size_t j = 2; j <= n; ++j) {
        for (size_t i = 1; i < j; ++i) {
          presentation::add_rule_no_checks(
              p,
              1_w + pow(0_w, n - j + i) + 1_w + pow(0_w, n - i + j),
              pow(0_w, n - j + i) + 1_w + pow(0_w, n - i + j) + 1_w);
        }
      }
      presentation::add_rule_no_checks(
          p,
          0_w + pow(1_w + pow(0_w, n - 1), n),
          pow(1_w + pow(0_w, n - 1), n));  // relation Q4
      return p;
    }

    // See Theorem 2.17 of https://arxiv.org/pdf/2211.02155.pdf
    Presentation<word_type>
    order_preserving_cyclic_inverse_monoid_Fer22(size_t n) {
      if (n < 3) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 1st argument to be at least 3, found {}", n);
      }

      Presentation<word_type> p;
      p.alphabet(n).contains_empty_word(true);

      word_type e = range(2, n);

      // relations V1
      presentation::add_idempotent_rules_no_checks(p, e);

      // relations V2
      presentation::add_rule_no_checks(p, 010_w, 0_w);
      presentation::add_rule_no_checks(p, 101_w, 1_w);

      // relations V3
      presentation::add_rule_no_checks(
          p, 1_w + pow(0_w, 2) + 1_w, 0_w + pow(1_w, 2) + 0_w);

      // relations V4
      presentation::add_commutes_rules_no_checks(p, e);
      presentation::add_commutes_rules_no_checks(p, e, {01_w});
      presentation::add_commutes_rules_no_checks(p, e, {10_w});

      // relations V6
      for (size_t i = 0; i <= n - 4; ++i) {
        presentation::add_rule_no_checks(p, 0_w + e[i + 1], e[i] + 0_w);
      }

      // relations V7
      presentation::add_rule_no_checks(p, pow(0_w, 2) + 1_w, e[n - 3] + 0_w);
      presentation::add_rule_no_checks(p, 1_w + pow(0_w, 2), 02_w);

      // relations V8
      presentation::add_rule_no_checks(p,
                                       10_w + prod(e, 0, n - 2, 1) + 01_w,
                                       0_w + prod(e, 0, n - 2, 1) + 01_w);

      return p;
    }

    // See Theorem 2.8 of https://arxiv.org/pdf/2205.02196.pdf
    Presentation<word_type>
    partial_isometries_cycle_graph_monoid_FP22(size_t n) {
      if (n < 3) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 1st argument to be at least 3, found {}", n);
      }

      Presentation<word_type> p;
      p.contains_empty_word(true);
      p.alphabet(3);

      // Q1
      presentation::add_rule_no_checks(p, pow(0_w, n), {});
      presentation::add_rule_no_checks(p, pow(1_w, 2), {});
      presentation::add_rule_no_checks(p, 10_w, pow(0_w, n - 1) + 1_w);

      // Q2
      presentation::add_rule_no_checks(p, pow(2_w, 2), 2_w);
      presentation::add_rule_no_checks(p, 01201_w, 2_w);

      // Q3

      for (size_t j = 2; j <= n; ++j) {
        for (size_t i = 1; i < j; ++i) {
          presentation::add_rule_no_checks(
              p,
              2_w + pow(0_w, j - i) + 2_w + pow(0_w, n - j + i),
              pow(0_w, j - i) + 2_w + pow(0_w, n - j + i) + 2_w);
        }
      }

      if (n % 2 == 1) {
        // Q4
        presentation::add_rule_no_checks(
            p, 10_w + pow(20_w, n - 2) + 2_w, pow(20_w, n - 2) + 2_w);
      } else {
        // Q5
        presentation::add_rule_no_checks(
            p,
            10_w + pow(20_w, n / 2 - 1) + 0_w + pow(20_w, n / 2 - 2) + 2_w,
            pow(20_w, n / 2 - 1) + 0_w + pow(20_w, n / 2 - 2) + 2_w);
        presentation::add_rule_no_checks(
            p, 1_w + pow(20_w, n - 1) + 2_w, pow(20_w, n - 1) + 2_w);
      }

      return p;
    }

    // See Section 2.2 of 'Presentations of finite simple groups: A quantitative
    // approach' J. Amer. Math. Soc. 21 (2008), 711-774
    Presentation<word_type> not_symmetric_group_GKKL08(size_t n) {
      if (n < 4) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 1st argument to be at least 4, found {}", n);
      }

      Presentation<word_type> p;
      p.alphabet(n - 1).contains_empty_word(true);

      for (size_t i = 0; i <= n - 2; ++i) {
        presentation::add_rule_no_checks(p, pow({i}, 2), {});
      }

      for (size_t i = 0; i <= n - 2; ++i) {
        for (size_t j = 0; j <= n - 2; ++j) {
          if (i != j) {
            presentation::add_rule_no_checks(p, pow({i, j}, 3), {});
          }
        }
      }

      for (size_t i = 0; i <= n - 2; ++i) {
        for (size_t j = 0; j <= n - 2; ++j) {
          if (i != j) {
            for (size_t k = 0; k <= n - 2; ++k) {
              if (k != i && k != j) {
                presentation::add_rule_no_checks(p, pow({i, j, k}, 4), {});
              }
            }
          }
        }
      }

      return p;
    }

    // n should be an odd prime for this presentation to actually defined the
    // claimed group. See Theorem 4 in https://doi.org/10.1112/blms/12.1.17
    Presentation<word_type> special_linear_group_2_CR80(size_t q) {
      if (q < 3) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 1st argument to be at least 3, found {}", q);
      }
      Presentation<word_type> p;
      p.alphabet(4);
      p.contains_empty_word(true);
      presentation::add_inverse_rules(p, 1032_w);
      presentation::add_rule(p, 00_w + pow(31_w, 3), {});
      presentation::add_rule(
          p,
          pow(0_w + pow(2_w, 4) + 0_w + pow(2_w, (q + 1) / 2), 2) + pow(2_w, q)
              + pow(0_w, 2 * (q / 3)),
          {});
      return p;
    }

    // See Definition 4.2 of https://doi.org/10.1016/S0012-365X(99)00270-8
    Presentation<word_type> hypo_plactic_monoid_Nov00(size_t n) {
      auto result = plactic_monoid_Knu70(n);

      for (letter_type a = 0; a < n; ++a) {
        for (letter_type b = a; b < n; ++b) {
          for (letter_type c = b + 1; c < n; ++c) {
            for (letter_type d = c; d < n; ++d) {
              result.rules.push_back({c, a, d, b});
              result.rules.push_back({a, c, b, d});
            }
          }
        }
      }
      for (letter_type a = 0; a < n; ++a) {
        for (letter_type b = a + 1; b < n; ++b) {
          for (letter_type c = b; c < n; ++c) {
            for (letter_type d = c + 1; d < n; ++d) {
              result.rules.push_back({b, d, a, c});
              result.rules.push_back({d, b, c, a});
            }
          }
        }
      }
      return result;
    }

    Presentation<word_type>
    sigma_plactic_monoid_AHMNT24(std::vector<size_t> const& sigma) {
      auto p = plactic_monoid_Knu70(sigma.size());
      p.contains_empty_word(true);
      for (auto [a, e] : rx::enumerate(sigma)) {
        presentation::add_rule(p, pow({a}, e), {a});
      }
      return p;
    }

    Presentation<word_type> zero_rook_monoid_Gay18(size_t n) {
      if (n < 2) {
        LIBSEMIGROUPS_EXCEPTION(
            "the 1st argument (degree) must at least 2, found {}", n);
      }

      Presentation<word_type> p;
      add_coxeter_common(p, n);
      presentation::add_idempotent_rules_no_checks(p, range(n));
      add_rook_monoid_common(p, n);
      p.alphabet_from_rules().contains_empty_word(true);
      return p;
    }

    Presentation<word_type> not_renner_type_B_monoid_Gay18(size_t l, int q) {
      if (q != 0 && q != 1) {
        LIBSEMIGROUPS_EXCEPTION("the 2nd argument must be 0 or 1, found {}", q);
      }

      Presentation<word_type> p;
      add_renner_type_B_common(p, l, q);

      auto s = range(l);
      auto e = range(l, 2 * l + 1);
      if (l >= 2) {
        presentation::add_rule_no_checks(
            p, {e[0], s[0], s[1], s[0], e[0]}, {e[2]});
      }
      p.alphabet_from_rules().contains_empty_word(true);
      return p;
    }

    Presentation<word_type> renner_type_B_monoid_Gay18(size_t l, int q) {
      if (q != 0 && q != 1) {
        LIBSEMIGROUPS_EXCEPTION("the 2nd argument must be 0 or 1, found {}", q);
      }

      Presentation<word_type> p;
      add_renner_type_B_common(p, l, q);

      auto s = range(l);
      auto e = range(l, 2 * l + 1);

      for (size_t i = 1; i < l; i++) {
        std::vector<size_t> new_rel = max_elt_B(i);
        new_rel.push_back(e[0]);
        new_rel.insert(new_rel.begin(), e[0]);
        presentation::add_rule_no_checks(p, new_rel, {e[i + 1]});
      }
      p.alphabet_from_rules().contains_empty_word(true);
      return p;
    }

    Presentation<word_type> not_renner_type_D_monoid_God09(size_t l, int q) {
      if (q != 0 && q != 1) {
        LIBSEMIGROUPS_EXCEPTION("the 2nd argument must be 0 or 1, found {}", q);
      }
      Presentation<word_type> p;
      add_renner_type_D_common(p, l, q);

      auto        s = range(l);
      auto        e = range(l, 2 * l + 1);
      letter_type f = 2 * l + 1;

      if (l >= 3) {
        presentation::add_rule_no_checks(
            p, {e[0], s[0], s[2], s[1], f}, {e[3]});
        presentation::add_rule_no_checks(
            p, {f, s[1], s[2], s[0], e[0]}, {e[3]});
      }
      p.alphabet_from_rules().contains_empty_word(true);
      return p;
    }

    Presentation<word_type> renner_type_D_monoid_Gay18(size_t l, int q) {
      if (q != 0 && q != 1) {
        LIBSEMIGROUPS_EXCEPTION("the 2nd argument must be 0 or 1, found {}", q);
      }
      Presentation<word_type> p;
      add_renner_type_D_common(p, l, q);

      auto        s = range(l);
      auto        e = range(l, 2 * l + 1);
      letter_type f = 2 * l + 1;

      for (size_t i = 2; i < l; i++) {
        word_type new_rel_f = max_elt_D(i, 0);
        word_type new_rel_e = max_elt_D(i, 1);

        if (i % 2 == 0) {
          new_rel_e.insert(new_rel_e.begin(), f);
          new_rel_e.push_back(e[0]);
          presentation::add_rule_no_checks(p, new_rel_e, {e[i + 1]});

          new_rel_f.insert(new_rel_f.begin(), e[0]);
          new_rel_f.push_back(f);
          presentation::add_rule_no_checks(p, new_rel_f, {e[i + 1]});
        } else {
          new_rel_e.insert(new_rel_e.begin(), f);
          new_rel_e.push_back(f);
          presentation::add_rule_no_checks(p, new_rel_e, {e[i + 1]});
          new_rel_f.insert(new_rel_f.begin(), e[0]);
          new_rel_f.push_back(e[0]);
          presentation::add_rule_no_checks(p, new_rel_f, {e[i + 1]});
        }
      }
      p.alphabet_from_rules().contains_empty_word(true);
      return p;
    }

  }  // namespace presentation::examples
}  // namespace libsemigroups
