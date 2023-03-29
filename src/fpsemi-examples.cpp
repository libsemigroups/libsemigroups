//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022 Murray T. Whyte
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

#include "libsemigroups/fpsemi-examples.hpp"

#include <algorithm>  // for max, for_each
#include <cstdint>    // for int64_t
#include <cstdlib>    // for abs
#include <numeric>    // for iota

#include "libsemigroups/exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/present.hpp"
#include "libsemigroups/report.hpp"  // for magic_enum support
#include "libsemigroups/types.hpp"   // for word_type, relation_type
#include "libsemigroups/words.hpp"   // for operator""_w

#include <rx/ranges.hpp>

namespace libsemigroups {
  using literals::operator""_w;
  namespace {

    word_type range(size_t first, size_t last, size_t step = 1) {
      return rx::seq<letter_type>(first, step) | rx::take(last - first)
             | rx::to_vector();
    }
    word_type range(size_t n) {
      return range(0, n, 1);
    }

    template <typename T>
    std::vector<T> concat(std::vector<T> lhs, const std::vector<T>& rhs) {
      lhs.insert(lhs.end(), rhs.begin(), rhs.end());
      return lhs;
    }
    std::vector<size_t> max_elt_B(size_t i) {
      std::vector<size_t> t(0);
      for (int end = i; end >= 0; end--) {
        for (int k = 0; k <= end; k++) {
          t.push_back(k);
        }
      }
      return t;
    }
    std::vector<size_t> max_elt_D(size_t i, int g) {
      // g est 0 ou 1 : 0 pour f et 1 pour e
      std::vector<size_t> t(0);
      int                 parity = g;
      for (int end = i; end > 0; end--) {
        t.push_back(parity);
        for (int k = 2; k <= end; k++) {
          t.push_back(k);
        }
        parity = (parity + 1) % 2;
      }
      return t;
    }
    word_type operator^(word_type const& w, size_t exp) {
      word_type result;
      for (size_t i = 0; i < exp; ++i) {
        result.insert(result.end(), w.cbegin(), w.cend());
      }
      return result;
    }

    word_type operator*(word_type const& lhs, word_type const& rhs) {
      word_type result(lhs);
      result.insert(result.end(), rhs.cbegin(), rhs.cend());
      return result;
    }

    void add_monoid_relations(std::vector<word_type> const& alphabet,
                              word_type                     id,
                              std::vector<relation_type>&   relations) {
      for (size_t i = 0; i < alphabet.size(); ++i) {
        if (alphabet[i] != id) {
          relations.emplace_back(alphabet[i] * id, alphabet[i]);
          relations.emplace_back(id * alphabet[i], alphabet[i]);
        } else {
          relations.emplace_back(id * id, id);
        }
      }
    }

    void add_idempotent_rules(std::vector<relation_type>& relations,
                              word_type const&            letters) {
      using presentation::operator+;
      for (auto x : letters) {
        word_type w = {x};
        relations.emplace_back(w + w, w);
      }
    }

    void add_commutes_rules(std::vector<relation_type>& relations,
                            word_type const&            letters) {
      using presentation::operator+;
      size_t const        n = letters.size();

      for (size_t i = 0; i < n - 1; ++i) {
        word_type u = {letters[i]};
        for (size_t j = i + 1; j < n; ++j) {
          word_type v = {letters[j]};
          relations.emplace_back(u + v, v + u);
        }
      }
    }

    void add_commutes_rules(std::vector<relation_type>&      relations,
                            word_type const&                 letters1,
                            std::initializer_list<word_type> letters2) {
      using presentation::operator+;
      size_t const        m = letters1.size();
      size_t const        n = letters2.size();
      // TODO so far this assumes that letters1 and letters2 have empty
      // intersection

      for (size_t i = 0; i < m; ++i) {
        word_type u = {letters1[i]};
        for (size_t j = 0; j < n; ++j) {
          word_type v = *(letters2.begin() + j);
          relations.emplace_back(u + v, v + u);
        }
      }
    }

    void
    add_full_transformation_monoid_relations(std::vector<relation_type>& result,
                                             size_t                      n,
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

      if (n < 4) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 1st argument to be at least 4, found %llu", uint64_t(n));
      }
      if (e12_value >= pi_start && e12_value <= pi_start + n - 2) {
        LIBSEMIGROUPS_EXCEPTION("e12 must not lie in the range [pi_start, "
                                "pi_start + n - 2], found %llu",
                                uint64_t(e12_value));
      }

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
          return pi[0] * e12 * pi[0];
        } else if (i == 1) {
          return pi[0] * pi[j - 2] * pi[0] * e12 * pi[0] * pi[j - 2] * pi[0];
        } else if (j == 2) {
          return pi[i - 2] * e12 * pi[i - 2];
        } else if (j == 1) {
          return pi[0] * pi[i - 2] * e12 * pi[i - 2] * pi[0];
        } else if (i == 2) {
          return pi[j - 2] * pi[0] * e12 * pi[0] * pi[j - 2];
        }
        return pi[i - 2] * pi[0] * pi[j - 2] * pi[0] * e12 * pi[0] * pi[j - 2]
               * pi[0] * pi[i - 2];
      };

      auto transp = [&pi](size_t i, size_t j) -> word_type {
        LIBSEMIGROUPS_ASSERT(i != j);
        if (i > j) {
          std::swap(i, j);
        }
        if (i == 1) {
          return pi[j - 2];
        }
        return pi[i - 2] * pi[j - 2] * pi[i - 2];
      };

      // Relations a
      for (size_t i = 1; i <= n; ++i) {
        for (size_t j = 1; j <= n; ++j) {
          if (j == i) {
            continue;
          }
          // Relations (k)
          result.emplace_back(transp(i, j) * eps(i, j), eps(i, j));
          // Relations (j)
          result.emplace_back(eps(j, i) * eps(i, j), eps(i, j));
          // Relations (i)
          result.emplace_back(eps(i, j) * eps(i, j), eps(i, j));
          // Relations (d)
          result.emplace_back(transp(i, j) * eps(i, j) * transp(i, j),
                              eps(j, i));
          for (size_t k = 1; k <= n; ++k) {
            if (k == i || k == j) {
              continue;
            }
            // Relations (h)
            result.emplace_back(eps(k, j) * eps(i, j), eps(k, j));
            // Relations (g)
            result.emplace_back(eps(k, i) * eps(i, j),
                                transp(i, j) * eps(k, j));
            // Relations (f)
            result.emplace_back(eps(j, k) * eps(i, j), eps(i, j) * eps(i, k));
            result.emplace_back(eps(j, k) * eps(i, j), eps(i, k) * eps(i, j));
            // Relations (c)
            result.emplace_back(transp(k, i) * eps(i, j) * transp(k, i),
                                eps(k, j));
            // Relations (b)
            result.emplace_back(transp(j, k) * eps(i, j) * transp(j, k),
                                eps(i, k));
            for (size_t l = 1; l <= n; ++l) {
              if (l == i || l == j || l == k) {
                continue;
              }
              // Relations (e)
              result.emplace_back(eps(l, k) * eps(i, j), eps(i, j) * eps(l, k));
              // Relations (a)
              result.emplace_back(transp(k, l) * eps(i, j) * transp(k, l),
                                  eps(i, j));
            }
          }
        }
      }
    }

  }  // namespace

  namespace fpsemigroup {

    using literals::operator""_w;
    using presentation::pow;
    using presentation::prod;
    using presentation::operator+;
    using presentation::operator+=;

    std::vector<relation_type> stellar_monoid(size_t l) {
      if (l < 2) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected argument to be at least 2, found %llu", uint64_t(l));
      }
      std::vector<size_t> pi;
      for (size_t i = 0; i < l; ++i) {
        pi.push_back(i);  // 0 est \pi_0
      }

      std::vector<relation_type> rels{};
      std::vector<size_t>        t{pi[0]};
      for (int i = 1; i < static_cast<int>(l); ++i) {
        t.insert(t.begin(), pi[i]);
        rels.push_back({concat(t, {pi[i]}), t});
      }
      return rels;
    }

    std::vector<relation_type> fibonacci_semigroup(size_t r, size_t n) {
      if (n == 0) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 2nd argument to be strictly positive, found %llu",
            uint64_t(n));
      } else if (r == 0) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 1st argument to be strictly positive, found %llu",
            uint64_t(r));
      }
      std::vector<relation_type> result;
      for (size_t i = 0; i < n; ++i) {
        word_type lhs(r, 0);
        std::iota(lhs.begin(), lhs.end(), i);
        std::for_each(lhs.begin(), lhs.end(), [&n](size_t& x) { x %= n; });
        result.emplace_back(lhs, word_type({(i + r) % n}));
      }
      return result;
    }

    std::vector<relation_type> plactic_monoid(size_t n) {
      if (n < 2) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected argument to be at least 2, found %llu", uint64_t(n));
      }
      std::vector<relation_type> result;
      for (size_t c = 0; c < n; ++c) {
        for (size_t b = 0; b < c; ++b) {
          for (size_t a = 0; a < b; ++a) {
            result.emplace_back(word_type({b, a, c}), word_type({b, c, a}));
            result.emplace_back(word_type({a, c, b}), word_type({c, a, b}));
          }
        }
      }
      for (size_t b = 0; b < n; ++b) {
        for (size_t a = 0; a < b; ++a) {
          result.emplace_back(word_type({b, a, a}), word_type({a, b, a}));
          result.emplace_back(word_type({b, b, a}), word_type({b, a, b}));
        }
      }
      return result;
    }

    std::vector<relation_type> stylic_monoid(size_t n) {
      if (n < 2) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected argument to be at least 2, found %llu", uint64_t(n));
      }
      std::vector<relation_type> result = plactic_monoid(n);
      for (size_t a = 0; a < n; ++a) {
        result.emplace_back(word_type({a, a}), word_type({a}));
      }
      return result;
    }

    std::vector<relation_type> symmetric_group(size_t n,
                                               author val,
                                               size_t index) {
      if (n < 4) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 1st argument to be at least 4, found %llu", uint64_t(n));
      }
      if (val == author::Carmichael) {
        if (index != 0) {
          LIBSEMIGROUPS_EXCEPTION("expected 3rd argument to be 0 when 2nd "
                                  "argument is author::Carmichael, found %llu",
                                  uint64_t(index));
        }
        // Exercise 9.5.2, p172 of
        // https://link.springer.com/book/10.1007/978-1-84800-281-4
        std::vector<word_type> pi;
        for (size_t i = 0; i <= n - 2; ++i) {
          pi.push_back({i});
        }
        std::vector<relation_type> result;

        for (size_t i = 0; i <= n - 2; ++i) {
          result.emplace_back(pi[i] ^ 2, word_type({}));
        }
        for (size_t i = 0; i < n - 2; ++i) {
          result.emplace_back((pi[i] * pi[i + 1]) ^ 3, word_type({}));
        }
        result.emplace_back((pi[n - 2] * pi[0]) ^ 3, word_type({}));
        for (size_t i = 0; i < n - 2; ++i) {
          for (size_t j = 0; j < i; ++j) {
            result.emplace_back((pi[i] * pi[i + 1] * pi[i] * pi[j]) ^ 2,
                                word_type({}));
          }
          for (size_t j = i + 2; j <= n - 2; ++j) {
            result.emplace_back((pi[i] * pi[i + 1] * pi[i] * pi[j]) ^ 2,
                                word_type({}));
          }
        }
        for (size_t j = 1; j < n - 2; ++j) {
          result.emplace_back((pi[n - 2] * pi[0] * pi[n - 2] * pi[j]) ^ 2,
                              word_type({}));
        }
        return result;
      } else if (val == author::Coxeter + author::Moser) {
        // From Chapter 3, Proposition 1.2 in https://bit.ly/3R5ZpKW (Ruskuc
        // thesis)
        if (index != 0) {
          LIBSEMIGROUPS_EXCEPTION(
              "expected 3rd argument to be 0 when 2nd argument is "
              "author::Coxeter + author::Moser, found %llu",
              uint64_t(index));
        }

        std::vector<word_type> a;
        for (size_t i = 0; i < n - 1; ++i) {
          a.push_back({i});
        }
        std::vector<relation_type> result;

        for (size_t i = 0; i < n - 1; i++) {
          result.emplace_back(a[i] ^ 2, word_type({}));
        }

        for (size_t j = 0; j < n - 2; ++j) {
          result.emplace_back((a[j] * a[j + 1]) ^ 3, word_type({}));
        }
        for (size_t l = 2; l < n - 1; ++l) {
          for (size_t k = 0; k <= l - 2; ++k) {
            result.emplace_back((a[k] * a[l]) ^ 2, word_type({}));
          }
        }
        return result;

      } else if (val == author::Moore) {
        if (index == 0) {
          // From Chapter 3, Proposition 1.1 in https://bit.ly/3R5ZpKW (Ruskuc
          // thesis)
          word_type const e = {};
          word_type const a = {0};
          word_type const b = {1};

          std::vector<relation_type> result;
          result.emplace_back(a ^ 2, e);
          result.emplace_back(b ^ n, e);
          result.emplace_back((a * b) ^ (n - 1), e);
          result.emplace_back((a * (pow(b, n - 1)) * a * b) ^ 3, e);
          for (size_t j = 2; j <= n - 2; ++j) {
            result.emplace_back((a * ((pow(b, n - 1)) ^ j) * a * (b ^ j)) ^ 2,
                                e);
          }
          return result;
        } else if (index == 1) {
          // From https://core.ac.uk/download/pdf/82378951.pdf
          // TODO: Get proper DOI of this paper

          std::vector<relation_type> result;
          std::vector<word_type>     s;

          for (size_t i = 0; i <= n - 2; ++i) {
            s.push_back({i});
          }

          for (size_t i = 0; i <= n - 2; ++i) {
            result.emplace_back(s[i] ^ 2, word_type({}));
          }

          for (size_t i = 0; i <= n - 4; ++i) {
            for (size_t j = i + 2; j <= n - 2; ++j) {
              result.emplace_back(s[i] * s[j], s[j] * s[i]);
            }
          }

          for (size_t i = 1; i <= n - 2; ++i) {
            result.emplace_back(s[i] * s[i - 1] * s[i],
                                s[i - 1] * s[i] * s[i - 1]);
          }
          return result;
        }
        LIBSEMIGROUPS_EXCEPTION("expected 3rd argument to be 0 or 1 when 2nd "
                                "argument is author::Moore, found %llu",
                                uint64_t(index));
      } else if (val == author::Burnside + author::Miller) {
        // See Eq 2.6 of 'Presentations of finite simple groups: A quantitative
        // approach' J. Amer. Math. Soc. 21 (2008), 711-774
        if (index != 0) {
          LIBSEMIGROUPS_EXCEPTION(
              "expected 3rd argument to be 0 when 2nd argument is "
              "author::Burnside + author::Miller, found %llu",
              uint64_t(index));
        }
        std::vector<word_type> a;
        for (size_t i = 0; i <= n - 2; ++i) {
          a.push_back({i});
        }

        std::vector<relation_type> result;

        for (size_t i = 0; i <= n - 2; ++i) {
          result.emplace_back(a[i] ^ 2, word_type({}));
        }

        for (size_t i = 0; i <= n - 2; ++i) {
          for (size_t j = 0; j <= n - 2; ++j) {
            if (i == j) {
              continue;
            }
            result.emplace_back((a[i] * a[j]) ^ 3, word_type({}));
          }
        }

        for (size_t i = 0; i <= n - 2; ++i) {
          for (size_t j = 0; j <= n - 2; ++j) {
            if (i == j) {
              continue;
            }
            for (size_t k = 0; k <= n - 2; ++k) {
              if (k == i || k == j) {
                continue;
              }
              result.emplace_back((a[i] * a[j] * a[i] * a[k]) ^ 2,
                                  word_type({}));
            }
          }
        }
        return result;
      } else if (val
                 == author::Guralnick + author::Kantor + author::Kassabov
                        + author::Lubotzky) {
        // See Section 2.2 of 'Presentations of finite simple groups: A
        // quantitative approach' J. Amer. Math. Soc. 21 (2008), 711-774
        std::vector<word_type> a;
        for (size_t i = 0; i <= n - 2; ++i) {
          a.push_back({i});
        }

        std::vector<relation_type> result;

        for (size_t i = 0; i <= n - 2; ++i) {
          result.emplace_back(a[i] ^ 2, word_type({}));
        }

        for (size_t i = 0; i <= n - 2; ++i) {
          for (size_t j = 0; j <= n - 2; ++j) {
            if (i != j) {
              result.emplace_back((a[i] * a[j]) ^ 3, word_type({}));
            }
          }
        }

        for (size_t i = 0; i <= n - 2; ++i) {
          for (size_t j = 0; j <= n - 2; ++j) {
            if (i == j) {
              continue;
            }
            for (size_t k = 0; k <= n - 2; ++k) {
              if (k != i && k != j) {
                result.emplace_back((a[i] * a[j] * a[k]) ^ 4, word_type({}));
              }
            }
          }
        }

        return result;

      } else {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 2nd argument to be one of: author::Burnside + "
            "author::Miller, "
            "author::Carmichael, author::Coxeter + author::Moser, or "
            "author::Moore, found %s",
            detail::to_string(val).c_str());
      }
    }

    std::vector<relation_type> alternating_group(size_t n, author val) {
      if (n < 4) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 1st argument to be at least 4, found %llu", uint64_t(n));
      }
      if (val == author::Moore) {
        std::vector<relation_type> result;
        std::vector<word_type>     a;

        for (size_t i = 0; i <= n - 3; ++i) {
          a.push_back(word_type({i}));
        }

        result.emplace_back(a[0] ^ 3, word_type({}));

        for (size_t j = 1; j <= n - 3; ++j) {
          result.emplace_back(a[j] ^ 2, word_type({}));
        }

        for (size_t i = 1; i <= n - 3; ++i) {
          result.emplace_back((a[i - 1] * a[i]) ^ 3, word_type({}));
        }

        for (size_t k = 2; k <= n - 3; ++k) {
          for (size_t j = 0; j <= k - 2; ++j) {
            result.emplace_back((a[j] * a[k]) ^ 2, word_type({}));
          }
        }

        return result;
      }
      LIBSEMIGROUPS_EXCEPTION(
          "expected 2nd argument to be author::Moore, found %s",
          detail::to_string(val).c_str());
    }

    // From https://core.ac.uk/reader/33304940
    std::vector<relation_type> dual_symmetric_inverse_monoid(size_t n,
                                                             author val) {
      if (n < 3) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 1st argument to be at least 3, found %llu", uint64_t(n));
      }
      if (val == author::Easdown + author::East + author::FitzGerald) {
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
        for (size_t i = 0; i < n + 1; ++i) {
          alphabet.push_back({i});
        }
        auto                       x = alphabet.back();
        auto                       e = alphabet.front();
        std::vector<relation_type> result;
        add_monoid_relations(alphabet, e, result);
        auto s = alphabet.cbegin();

        // R1
        for (size_t i = 1; i <= n - 1; ++i) {
          for (size_t j = 1; j <= n - 1; ++j) {
            result.emplace_back((s[i] * s[j]) ^ mij(i, j), e);
          }
        }
        // R2
        result.emplace_back(x ^ 3, x);
        // R3
        result.emplace_back(x * s[1], x);
        result.emplace_back(s[1] * x, x);
        // R4
        result.emplace_back(x * s[2] * x, x * s[2] * x * s[2]);
        result.emplace_back(x * s[2] * x * s[2], s[2] * x * s[2] * x);
        result.emplace_back(s[2] * x * s[2] * x, x * s[2] * (x ^ 2));
        result.emplace_back(x * s[2] * (x ^ 2), (x ^ 2) * s[2] * x);
        if (n == 3) {
          return result;
        }
        // R5
        word_type const sigma = s[2] * s[3] * s[1] * s[2];
        result.emplace_back((x ^ 2) * sigma * (x ^ 2) * sigma,
                            sigma * (x ^ 2) * sigma * (x ^ 2));
        result.emplace_back(sigma * (x ^ 2) * sigma * (x ^ 2),
                            x * s[2] * s[3] * s[2] * x);
        // R6
        std::vector<word_type> l = {{}, {}, x * s[2] * s[1]};
        for (size_t i = 3; i <= n - 1; ++i) {
          l.push_back(s[i] * l[i - 1] * s[i] * s[i - 1]);
        }
        std::vector<word_type> y = {{}, {}, {}, x};
        for (size_t i = 4; i <= n; ++i) {
          y.push_back(l[i - 1] * y[i - 1] * s[i - 1]);
        }
        for (size_t i = 3; i <= n - 1; ++i) {
          result.emplace_back(y[i] * s[i] * y[i], s[i] * y[i] * s[i]);
        }
        if (n == 4) {
          return result;
        }
        // R7
        for (size_t i = 4; i <= n - 1; ++i) {
          result.emplace_back(x * s[i], s[i] * x);
        }
        return result;
      } else {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 2nd argument to be author::Easdown + author::East + "
            "author::FitzGerald, found %s",
            detail::to_string(val).c_str());
      }
    }

    std::vector<relation_type> uniform_block_bijection_monoid(size_t n,
                                                              author val) {
      if (n < 3) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 1st argument to be at least 3, found %llu", uint64_t(n));
      }
      if (val == author::FitzGerald) {
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
        for (size_t i = 0; i < n + 1; ++i) {
          alphabet.push_back({i});
        }
        auto                       t = alphabet.back();
        auto                       e = alphabet.front();
        std::vector<relation_type> result;
        add_monoid_relations(alphabet, e, result);
        auto s = alphabet.cbegin();

        // S in Theorem 3 (same as dual_symmetric_inverse_monoid)
        for (size_t i = 1; i <= n - 1; ++i) {
          for (size_t j = 1; j <= n - 1; ++j) {
            result.emplace_back((s[i] * s[j]) ^ mij(i, j), e);
          }
        }

        // F2
        result.emplace_back(t ^ 2, t);

        // F3
        result.emplace_back(t * s[1], t);
        result.emplace_back(s[1] * t, t);

        // F4
        for (size_t i = 3; i <= n - 1; ++i) {
          result.emplace_back(s[i] * t, t * s[i]);
        }

        // F5
        result.emplace_back(s[2] * t * s[2] * t, t * s[2] * t * s[2]);

        // F6
        result.emplace_back(
            s[2] * s[1] * s[3] * s[2] * t * s[2] * s[3] * s[1] * s[2] * t,
            t * s[2] * s[1] * s[3] * s[2] * t * s[2] * s[3] * s[1] * s[2]);

        return result;
      } else {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 2nd argument to be author::FitzGerald, found %s",
            detail::to_string(val).c_str());
      }
    }

    // From Theorem 41 in doi:10.1016/j.jalgebra.2011.04.008
    std::vector<relation_type> partition_monoid(size_t n, author val) {
      if (val == author::Machine) {
        if (n != 2 && n != 3) {
          LIBSEMIGROUPS_EXCEPTION_V3(
              "the 1st argument (size_t) must be 2 or 3 when the "
              "2nd argument is author::Machine, found {}",
              n);
        }
      } else if (val == author::East) {
        if (n < 4) {
          LIBSEMIGROUPS_EXCEPTION_V3(
              "the 1st argument (degree) must be at least 4 "
              "when the 2nd argument is author::East, found {}",
              n);
        }
      } else {
        LIBSEMIGROUPS_EXCEPTION_V3("the 2nd argument must be author::Machine "
                                   "or author::East, found {}",
                                   val);
      }

      std::vector<relation_type> result;

      if (val == author::Machine && n == 2) {
        result.emplace_back(01_w, 1_w);
        result.emplace_back(10_w, 1_w);
        result.emplace_back(02_w, 2_w);
        result.emplace_back(20_w, 2_w);
        result.emplace_back(03_w, 3_w);
        result.emplace_back(30_w, 3_w);
        result.emplace_back(11_w, 0_w);
        result.emplace_back(13_w, 3_w);
        result.emplace_back(22_w, 2_w);
        result.emplace_back(31_w, 3_w);
        result.emplace_back(33_w, 3_w);
        result.emplace_back(232_w, 2_w);
        result.emplace_back(323_w, 3_w);
        result.emplace_back(1212_w, 212_w);
        result.emplace_back(2121_w, 212_w);
        return result;
      }
      if (val == author::Machine && n == 3) {
        result.emplace_back(00_w, 0_w);
        result.emplace_back(01_w, 1_w);
        result.emplace_back(02_w, 2_w);
        result.emplace_back(03_w, 3_w);
        result.emplace_back(04_w, 4_w);
        result.emplace_back(10_w, 1_w);
        result.emplace_back(20_w, 2_w);
        result.emplace_back(22_w, 0_w);
        result.emplace_back(24_w, 4_w);
        result.emplace_back(30_w, 3_w);
        result.emplace_back(33_w, 3_w);
        result.emplace_back(40_w, 4_w);
        result.emplace_back(42_w, 4_w);
        result.emplace_back(44_w, 4_w);
        result.emplace_back(111_w, 0_w);
        result.emplace_back(112_w, 21_w);
        result.emplace_back(121_w, 2_w);
        result.emplace_back(211_w, 12_w);
        result.emplace_back(212_w, 11_w);
        result.emplace_back(214_w, 114_w);
        result.emplace_back(312_w, 123_w);
        result.emplace_back(343_w, 3_w);
        result.emplace_back(412_w, 411_w);
        result.emplace_back(434_w, 4_w);
        result.emplace_back(1131_w, 232_w);
        result.emplace_back(1132_w, 231_w);
        result.emplace_back(1231_w, 32_w);
        result.emplace_back(1232_w, 31_w);
        result.emplace_back(1234_w, 314_w);
        result.emplace_back(1323_w, 313_w);
        result.emplace_back(1414_w, 414_w);
        result.emplace_back(2131_w, 132_w);
        result.emplace_back(2132_w, 131_w);
        result.emplace_back(2134_w, 1314_w);
        result.emplace_back(2313_w, 1313_w);
        result.emplace_back(2314_w, 1134_w);
        result.emplace_back(2323_w, 323_w);
        result.emplace_back(3132_w, 313_w);
        result.emplace_back(3143_w, 123_w);
        result.emplace_back(3232_w, 323_w);
        result.emplace_back(4114_w, 414_w);
        result.emplace_back(4132_w, 4131_w);
        result.emplace_back(4141_w, 414_w);
        result.emplace_back(13113_w, 3213_w);
        result.emplace_back(13414_w, 4134_w);
        result.emplace_back(23113_w, 3113_w);
        result.emplace_back(23213_w, 13213_w);
        result.emplace_back(23413_w, 13413_w);
        result.emplace_back(23414_w, 14134_w);
        result.emplace_back(31141_w, 11413_w);
        result.emplace_back(31311_w, 3213_w);
        result.emplace_back(32311_w, 3113_w);
        result.emplace_back(34113_w, 123_w);
        result.emplace_back(34143_w, 11413_w);
        result.emplace_back(41134_w, 4314_w);
        result.emplace_back(41311_w, 13114_w);
        result.emplace_back(41313_w, 4313_w);
        result.emplace_back(41314_w, 4134_w);
        result.emplace_back(41341_w, 4134_w);
        result.emplace_back(41432_w, 41431_w);
        result.emplace_back(113413_w, 31413_w);
        result.emplace_back(114134_w, 3414_w);
        result.emplace_back(131143_w, 43213_w);
        result.emplace_back(131313_w, 31313_w);
        result.emplace_back(131413_w, 3413_w);
        result.emplace_back(143114_w, 43114_w);
        result.emplace_back(231143_w, 143213_w);
        result.emplace_back(311341_w, 31413_w);
        result.emplace_back(311431_w, 114313_w);
        result.emplace_back(313131_w, 31313_w);
        result.emplace_back(313141_w, 3413_w);
        result.emplace_back(314113_w, 3_w);
        result.emplace_back(414311_w, 43114_w);
        result.emplace_back(414314_w, 414_w);
        result.emplace_back(431314_w, 13114_w);
        result.emplace_back(1143131_w, 311432_w);
        result.emplace_back(1143213_w, 31143_w);
        result.emplace_back(1313413_w, 313413_w);
        result.emplace_back(3114321_w, 31143_w);
        result.emplace_back(3131341_w, 313413_w);
        result.emplace_back(4311432_w, 4143131_w);
        result.emplace_back(31143231_w, 3114323_w);
        result.emplace_back(311432341_w, 114313413_w);
        return result;
      }

      // author::East and n >= 4_w
      word_type s  = {0};
      word_type c  = {1};
      word_type e  = {2};
      word_type t  = {3};
      word_type id = {4};

      std::vector<word_type> alphabet = {s, c, e, t, id};
      add_monoid_relations(alphabet, id, result);

      // V1
      result.emplace_back(c ^ n, id);
      result.emplace_back((s * c) ^ (n - 1), id);
      result.emplace_back(s * s, id);
      for (size_t i = 2; i <= n / 2; ++i) {
        result.emplace_back(((c ^ i) * s * (c ^ (n - i)) * s) ^ 2, id);
      }

      // V2
      result.emplace_back(e * e, e);
      result.emplace_back(e * t * e, e);
      result.emplace_back(s * c * e * (c ^ (n - 1)) * s, e);
      result.emplace_back(c * s * (c ^ (n - 1)) * e * c * s * (c ^ (n - 1)), e);

      // V3
      result.emplace_back(t * t, t);
      result.emplace_back(t * e * t, t);
      result.emplace_back(t * s, t);
      result.emplace_back(s * t, t);
      result.emplace_back(
          (c ^ 2) * s * (c ^ (n - 2)) * t * (c ^ 2) * s * (c ^ (n - 2)), t);
      result.emplace_back((c ^ (n - 1)) * s * c * s * (c ^ (n - 1)) * t * c * s
                              * (c ^ (n - 1)) * s * c,
                          t);

      // V4
      result.emplace_back(s * e * s * e, e * s * e);
      result.emplace_back(e * s * e * s, e * s * e);

      // V5
      result.emplace_back(t * c * t * (c ^ (n - 1)), c * t * (c ^ (n - 1)) * t);

      // V6
      result.emplace_back(t * (c ^ 2) * t * (c ^ (n - 2)),
                          (c ^ 2) * t * (c ^ (n - 2)) * t);
      // V7
      result.emplace_back(t * (c ^ 2) * e * (c ^ (n - 2)),
                          (c ^ 2) * e * (c ^ (n - 2)) * t);
      return result;
    }

    // From Theorem 5 in 10.21136/MB.2007.134125
    // https://dml.cz/bitstream/handle/10338.dmlcz/134125/MathBohem_132-2007-3_6.pdf

    std::vector<relation_type> singular_brauer_monoid(size_t n) {
      if (n < 3) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected argument to be at least 3, found %llu", uint64_t(n));
      }
      std::vector<std::vector<word_type>> t;
      size_t                              val = 0;
      for (size_t i = 0; i < n; ++i) {
        t.push_back({});
        for (size_t j = 0; j < n; ++j) {
          if (i != j) {
            t.back().push_back({val});
            val++;
          } else {
            t.back().push_back({0});
          }
        }
      }

      std::vector<relation_type> result;
      // (3) + (4)
      for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
          if (i != j) {
            result.emplace_back(t[i][j], t[j][i]);
            result.emplace_back(t[i][j] ^ 2, t[i][j]);
          }
        }
      }

      // (6) + (7)
      for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
          for (size_t k = 0; k < n; ++k) {
            if (i != j && j != k && i != k) {
              result.emplace_back(t[i][j] * t[i][k] * t[j][k],
                                  t[i][j] * t[j][k]);
              result.emplace_back(t[i][j] * t[j][k] * t[i][j], t[i][j]);
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
                result.emplace_back(t[i][j] * t[j][k] * t[k][l],
                                    t[i][j] * t[i][l] * t[k][l]);
                result.emplace_back(t[i][j] * t[k][l] * t[i][k],
                                    t[i][j] * t[j][l] * t[i][k]);
                result.emplace_back(t[i][j] * t[k][l], t[k][l] * t[i][j]);
              }
            }
          }
        }
      }
      return result;
    }

    // From https://doi.org/10.1007/s10012-000-0001-1
    std::vector<relation_type> orientation_preserving_monoid(size_t n) {
      if (n < 3) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected argument to be at least 3, found %llu", uint64_t(n));
      }
      word_type                  b = {0};
      word_type                  u = {1};
      word_type                  e = {};
      std::vector<relation_type> result;

      result.emplace_back(b ^ n, e);
      result.emplace_back(u ^ 2, u);
      result.emplace_back((u * b) ^ n, u * b);
      result.emplace_back(b * ((u * (pow(b, n - 1))) ^ (n - 1)),
                          (u * (pow(b, n - 1))) ^ (n - 1));
      for (size_t i = 2; i <= n - 1; ++i) {
        result.emplace_back(u * (b ^ i) * ((u * b) ^ (n - 1)) * (pow(b, n - i)),
                            (b ^ i) * ((u * b) ^ (n - 1)) * (pow(b, n - i))
                                * u);
      }
      return result;
    }

    // Also from https://doi.org/10.1007/s10012-000-0001-1
    std::vector<relation_type> orientation_reversing_monoid(size_t n) {
      if (n < 3) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected argument to be at least 3, found %llu", uint64_t(n));
      }
      word_type                  e = {};
      word_type                  b = {0};
      word_type                  u = {1};
      word_type                  c = {2};
      std::vector<relation_type> result;

      result.emplace_back(b ^ n, e);
      result.emplace_back(u ^ 2, u);
      result.emplace_back((u * b) ^ n, u * b);
      result.emplace_back(b * ((u * (pow(b, n - 1))) ^ (n - 1)),
                          (u * (pow(b, n - 1))) ^ (n - 1));
      for (size_t i = 2; i <= n - 1; ++i) {
        result.emplace_back(u * (b ^ i) * ((u * b) ^ (n - 1)) * (pow(b, n - i)),
                            (b ^ i) * ((u * b) ^ (n - 1)) * (pow(b, n - i))
                                * u);
      }
      result.emplace_back(c ^ 2, e);
      result.emplace_back(b * c, c * (pow(b, n - 1)));
      result.emplace_back(u * c, c * ((b * u) ^ (n - 1)));
      result.emplace_back(c * (u * (pow(b, n - 1)) ^ (n - 2)),
                          (pow(b, n - 2)) * ((u * (pow(b, n - 1))) ^ (n - 2)));

      return result;
    }

    // From Theorem 2.2 in https://doi.org/10.1093/qmath/haab001
    std::vector<relation_type> temperley_lieb_monoid(size_t n) {
      if (n < 3) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected argument to be at least 3, found %llu", uint64_t(n));
      }
      std::vector<word_type> e(n, word_type());
      for (size_t i = 0; i < n - 1; ++i) {
        e[i + 1] = {i};
      }
      std::vector<relation_type> result;

      // E1
      for (size_t i = 1; i <= n - 1; ++i) {
        result.emplace_back(e[i] ^ 2, e[i]);
      }
      // E2 + E3
      for (size_t i = 1; i <= n - 1; ++i) {
        for (size_t j = 1; j <= n - 1; ++j) {
          auto d = std::abs(static_cast<int64_t>(i) - static_cast<int64_t>(j));
          if (d > 1) {
            result.emplace_back(e[i] * e[j], e[j] * e[i]);
          } else if (d == 1) {
            result.emplace_back(e[i] * e[j] * e[i], e[i]);
          }
        }
      }

      return result;
    }

    // From Theorem 3.1 in
    // https://link.springer.com/content/pdf/10.2478/s11533-006-0017-6.pdf
    std::vector<relation_type> brauer_monoid(size_t n) {
      word_type const e = {0};

      std::vector<word_type> sigma(n);
      std::vector<word_type> theta(n);

      std::vector<word_type> alphabet = {e};
      for (size_t i = 1; i <= n - 1; ++i) {
        sigma[i] = {i};
        alphabet.push_back(sigma[i]);
      }
      for (size_t i = 1; i <= n - 1; ++i) {
        theta[i] = {i + n - 1};
        alphabet.push_back(theta[i]);
      }
      std::vector<relation_type> result;

      add_monoid_relations(alphabet, e, result);

      // E1
      for (size_t i = 1; i <= n - 1; ++i) {
        result.emplace_back(sigma[i] ^ 2, e);
        result.emplace_back(theta[i] ^ 2, theta[i]);
        result.emplace_back(theta[i] * sigma[i], sigma[i] * theta[i]);
        result.emplace_back(sigma[i] * theta[i], theta[i]);
      }

      // E2 + E3
      for (size_t i = 1; i <= n - 1; ++i) {
        for (size_t j = 1; j <= n - 1; ++j) {
          auto d = std::abs(static_cast<int64_t>(i) - static_cast<int64_t>(j));
          if (d > 1) {
            result.emplace_back(sigma[i] * sigma[j], sigma[j] * sigma[i]);
            result.emplace_back(theta[i] * theta[j], theta[j] * theta[i]);
            result.emplace_back(theta[i] * sigma[j], sigma[j] * theta[i]);
          } else if (d == 1) {
            result.emplace_back(sigma[i] * sigma[j] * sigma[i],
                                sigma[j] * sigma[i] * sigma[j]);
            result.emplace_back(theta[i] * theta[j] * theta[i], theta[i]);
            result.emplace_back(sigma[i] * theta[j] * theta[i],
                                sigma[j] * theta[i]);
            result.emplace_back(theta[i] * theta[j] * sigma[i],
                                theta[i] * sigma[j]);
          }
        }
      }

      return result;
    }

    // From Proposition 4.2 in
    // https://link.springer.com/content/pdf/10.1007/s002339910016.pdf
    std::vector<relation_type> rectangular_band(size_t m, size_t n) {
      if (m == 0) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 1st argument to be strictly positive, found %llu",
            uint64_t(m));
      }
      if (n == 0) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 2nd argument to be strictly positive, found %llu",
            uint64_t(n));
      }

      std::vector<word_type> a(m);
      std::vector<word_type> b(n);
      for (size_t i = 0; i < m; ++i) {
        a[i] = {i};
      }
      for (size_t i = 0; i < n; ++i) {
        b[i] = {i + m};
      }

      std::vector<relation_type> result;
      result.emplace_back(a[0], b[0]);

      // (7)
      for (size_t i = 1; i < m; ++i) {
        result.emplace_back(a[i - 1] * a[i], a[i - 1]);
      }

      result.emplace_back(a[m - 1] * a[0], a[m - 1]);

      // (8)
      for (size_t i = 1; i < n; ++i) {
        result.emplace_back(b[i - 1] * b[i], b[i]);
      }

      result.emplace_back(b[n - 1] * b[0], b[0]);

      for (size_t i = 1; i < m; ++i) {
        for (size_t j = 1; j < n; ++j) {
          result.emplace_back(b[j] * a[i], a[0]);
        }
      }

      return result;
    }

    std::vector<relation_type> full_transformation_monoid(size_t n,
                                                          author val) {
      if (n < 4) {
        LIBSEMIGROUPS_EXCEPTION(
            "the 1st argument (size_t) must be at least 4, found %llu",
            uint64_t(n));
      }
      if (val == author::Aizenstat) {
        // From Proposition 1.7 in https://bit.ly/3R5ZpKW
        auto result = symmetric_group(n, author::Moore);

        word_type const a = {0};
        word_type const b = {1};
        word_type const t = {2};

        result.emplace_back(a * t, t);
        result.emplace_back((pow(b, n - 2)) * a * (b ^ 2) * t * (pow(b, n - 2))
                                * a * (b ^ 2),
                            t);
        result.emplace_back(b * a * (pow(b, n - 1)) * a * b * t
                                * (pow(b, n - 1)) * a * b * a * (pow(b, n - 1)),
                            t);
        result.emplace_back((t * b * a * (pow(b, n - 1))) ^ 2, t);
        result.emplace_back(((pow(b, n - 1)) * a * b * t) ^ 2,
                            t * (pow(b, n - 1)) * a * b * t);

        result.emplace_back((t * (pow(b, n - 1)) * a * b) ^ 2,
                            t * (pow(b, n - 1)) * a * b * t);
        result.emplace_back((t * b * a * (pow(b, n - 2)) * a * b) ^ 2,
                            (b * a * (pow(b, n - 2)) * a * b * t) ^ 2);
        return result;
      } else if (val == author::Iwahori) {
        // From Theorem 9.3.1, p161-162, (Ganyushkin + Mazorchuk)
        // using Theorem 9.1.4 to express presentation in terms
        // of the pi_i and e_12.
        // https://link.springer.com/book/10.1007/978-1-84800-281-4
        auto result = symmetric_group(n, author::Carmichael);
        add_full_transformation_monoid_relations(result, n, 0, n - 1);
        return result;
      }
      LIBSEMIGROUPS_EXCEPTION(
          "expected 2nd argument to be author::Aizenstat or "
          "author::Iwahori, found %s",
          detail::to_string(val).c_str());
    }

    std::vector<relation_type> partial_transformation_monoid(size_t n,
                                                             author val) {
      if (n < 3) {
        LIBSEMIGROUPS_EXCEPTION(
            "the 1st argument (size_t) must be at least 3, found %llu",
            uint64_t(n));
      } else if (val == author::Machine) {
        if (n != 3) {
          LIBSEMIGROUPS_EXCEPTION("the 1st argument must be 3 where the 2nd "
                                  "argument is author::Machine, found %llu",
                                  uint64_t(n));
        }
        return {{{0, 0}, {}},
                {{0, 3}, {3}},
                {{2, 2}, {2}},
                {{2, 3}, {2}},
                {{3, 2}, {3}},
                {{3, 3}, {3}},
                {{0, 1, 0}, {1, 1}},
                {{0, 1, 1}, {1, 0}},
                {{1, 0, 1}, {0}},
                {{1, 1, 0}, {0, 1}},
                {{1, 1, 1}, {}},
                {{1, 1, 3}, {0, 1, 3}},
                {{2, 0, 1}, {0, 1, 2}},
                {{3, 0, 2}, {2, 0, 2}},
                {{3, 1, 2}, {2, 1, 2}},
                {{0, 1, 2, 0}, {2, 1, 1}},
                {{0, 1, 2, 1}, {2, 1, 0}},
                {{0, 2, 0, 2}, {2, 0, 2}},
                {{0, 2, 1, 0}, {1, 2, 1}},
                {{0, 2, 1, 1}, {1, 2, 0}},
                {{0, 2, 1, 2}, {2, 1, 2}},
                {{1, 2, 1, 0}, {0, 2, 1}},
                {{1, 2, 1, 1}, {0, 2, 0}},
                {{1, 2, 1, 3}, {0, 2, 1, 3}},
                {{1, 3, 1, 3}, {3, 1, 3}},
                {{2, 0, 2, 0}, {2, 0, 2}},
                {{2, 0, 2, 1}, {2, 1, 2}},
                {{2, 1, 2, 1}, {2, 1, 2, 0}},
                {{2, 1, 3, 1}, {2, 1, 3, 0}},
                {{3, 0, 1, 2}, {3, 0, 1}},
                {{3, 0, 1, 3}, {3, 0, 1}},
                {{3, 1, 3, 1}, {3, 1, 3, 0}},
                {{1, 0, 2, 1, 3}, {3, 1, 0, 2}},
                {{1, 1, 2, 0, 2}, {2, 1, 1, 2}},
                {{1, 1, 2, 1, 2}, {2, 1, 0, 2}},
                {{1, 3, 1, 0, 2}, {2, 1, 3}},
                {{2, 1, 0, 2, 1}, {2, 1, 0, 2, 0}},
                {{2, 1, 1, 2, 0}, {2, 1, 1, 2}},
                {{2, 1, 1, 2, 1}, {2, 1, 0, 2}},
                {{2, 1, 3, 0, 1}, {1, 3, 1, 1, 2}},
                {{3, 1, 0, 2, 1}, {3, 1, 0, 2, 0}},
                {{3, 1, 1, 2, 0}, {3, 1, 1, 2}},
                {{3, 1, 1, 2, 1}, {3, 1, 0, 2}},
                {{1, 2, 1, 2, 0, 2}, {2, 1, 2, 0, 2}}};
      } else if (val == author::Sutov) {
        // From Theorem 9.4.1, p169, (Ganyushkin + Mazorchuk)
        // https://link.springer.com/book/10.1007/978-1-84800-281-4
        if (n < 4) {
          LIBSEMIGROUPS_EXCEPTION(
              "the 1st argument must be at least 4 when the "
              "2nd argument is author::Sutov, found %llu",
              uint64_t(n));
        }
        auto result = symmetric_inverse_monoid(n, author::Sutov);

        add_full_transformation_monoid_relations(result, n, 0, n);
        word_type              e12 = {n};
        std::vector<word_type> epsilon
            = {{n - 1}, {0, n - 1, 0}, {1, n - 1, 1}};
        result.emplace_back(e12 * epsilon[1], e12);
        result.emplace_back(epsilon[1] * e12, epsilon[1]);

        result.emplace_back(e12 * epsilon[0], epsilon[1] * epsilon[0] * e12);
        result.emplace_back(e12 * epsilon[2], epsilon[2] * e12);

        return result;
      }
      LIBSEMIGROUPS_EXCEPTION("expected 2nd argument to be author::Machine or "
                              "author::Sutov, found %s",
                              detail::to_string(val).c_str());
    }

    // From Theorem 9.2.2, p156
    // https://link.springer.com/book/10.1007/978-1-84800-281-4 (Ganyushkin +
    // Mazorchuk)
    std::vector<relation_type> symmetric_inverse_monoid(size_t n, author val) {
      if (val == author::Sutov) {
        if (n < 4) {
          LIBSEMIGROUPS_EXCEPTION(
              "the 1st argument must be at least 4 when the "
              "2nd argument is author::Sutov, found %llu",
              uint64_t(n));
        }
        auto result = symmetric_group(n, author::Carmichael);

        std::vector<word_type> pi;
        for (size_t i = 0; i <= n - 2; ++i) {
          pi.push_back({i});
        }
        std::vector<word_type> epsilon;
        epsilon.push_back({n - 1});
        for (size_t i = 0; i <= n - 2; ++i) {
          epsilon.push_back(pi[i] * epsilon[0] * pi[i]);
        }

        result.emplace_back(epsilon[0] ^ 2, epsilon[0]);
        result.emplace_back(epsilon[0] * epsilon[1], epsilon[1] * epsilon[0]);

        for (size_t k = 1; k <= n - 2; ++k) {
          result.emplace_back(epsilon[1] * pi[k], pi[k] * epsilon[1]);
          result.emplace_back(epsilon[k + 1] * pi[0], pi[0] * epsilon[k + 1]);
        }
        result.emplace_back(epsilon[1] * epsilon[0] * pi[0],
                            epsilon[1] * epsilon[0]);
        return result;
      }
      LIBSEMIGROUPS_EXCEPTION(
          "expected 2nd argument to be author::Sutov, found %s",
          detail::to_string(val).c_str());
    }

    // Chinese monoid
    // See: The Chinese Monoid - Cassaigne, Espie, Krob, Novelli and Hivert,
    // 2001
    std::vector<relation_type> chinese_monoid(size_t n) {
      if (n < 2) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected argument to be at least 2, found %llu", uint64_t(n));
      }
      std::vector<relation_type> result;
      for (size_t a = 0; a < n; a++) {
        for (size_t b = a; b < n; b++) {
          for (size_t c = b; c < n; c++) {
            if (a != b) {
              result.emplace_back(word_type({c, b, a}), word_type({c, a, b}));
            }
            if (b != c) {
              result.emplace_back(word_type({c, b, a}), word_type({b, c, a}));
            }
          }
        }
      }
      return result;
    }

    std::vector<relation_type> monogenic_semigroup(size_t m, size_t r) {
      std::vector<relation_type> result;
      if (r == 0) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 2nd argument to be strictly positive, found %llu",
            uint64_t(r));
      }
      result.emplace_back(word_type({0}) ^ (m + r), word_type({0}) ^ m);
      return result;
    }

    Presentation<word_type> order_preserving_monoid(size_t n) {
      if (n < 3) {
        LIBSEMIGROUPS_EXCEPTION_V3(
            "expected argument to be at least 3, found {}", n);
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
        presentation::add_rule(p, v[n - 2 - i] + u[i], u[i] + v[n - 1 - i]);
        // relations 2
        presentation::add_rule(p, u[n - 2 - i] + v[i], v[i] + u[n - 1 - i]);
      }

      for (size_t i = 0; i <= n - 2; ++i) {
        // relations 3
        presentation::add_rule(p, v[n - 2 - i] + u[i], u[i]);
        // relations 4
        presentation::add_rule(p, u[n - 2 - i] + v[i], v[i]);
      }

      // relations 5
      for (size_t i = 0; i <= n - 2; ++i) {
        for (size_t j = 0; j <= n - 2; ++j) {
          if (j != (n - 2) - i && j != n - i - 1) {
            presentation::add_rule(p, u[i] + v[j], v[j] + u[i]);
          }
        }
      }

      // relation 6
      presentation::add_rule(p, u[0] + u[1] + u[0], u[0] + u[1]);

      // relation 7
      presentation::add_rule(p, v[0] + v[1] + v[0], v[0] + v[1]);

      p.alphabet_from_rules();

      return p;
    }

    std::vector<relation_type> cyclic_inverse_monoid(size_t n,
                                                     author val,
                                                     size_t index) {
      if (val != author::Fernandes) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 2nd argument to be author::Fernandes, found %s",
            detail::to_string(val).c_str());
      }
      if (n < 3) {
        LIBSEMIGROUPS_EXCEPTION(
            "the 1st argument must be at least 3 when the 2nd argument is "
            "author::Fernandes, found %llu",
            uint64_t(n));
      } else if (index != 0 && index != 1) {
        LIBSEMIGROUPS_EXCEPTION("the 3rd argument must be 0 or 1 when the 2nd "
                                "argument is author::Fernandes, found %llu",
                                uint64_t(n));
      }

      std::vector<relation_type> result;
      auto                       g = 0_w;
      // See Theorem 2.6 of https://arxiv.org/pdf/2211.02155.pdf
      if (index == 0) {
        word_type e = range(1, n + 1);

        // R1
        result.emplace_back(pow(g, n), ""_w);

        // R2
        add_idempotent_rules(result, e);

        // R3
        add_commutes_rules(result, e);

        // R4
        result.emplace_back(g + e[0], e[n - 1] + g);
        for (size_t i = 0; i < n - 1; ++i) {
          result.emplace_back(g + e[i + 1], e[i] + g);
        }

        // R5
        result.emplace_back(g + e, e);

      } else if (index == 1) {
        // See Theorem 2.7 of https://arxiv.org/pdf/2211.02155.pdf
        auto e = 1_w;

        result.emplace_back(pow(g, n), ""_w);  // relation Q1
        result.emplace_back(pow(e, 2), e);     // relation Q2

        // relations Q3
        for (size_t j = 2; j <= n; ++j) {
          for (size_t i = 1; i < j; ++i) {
            result.emplace_back(e + pow(g, n - j + i) + e + pow(g, n - i + j),
                                pow(g, n - j + i) + e + pow(g, n - i + j) + e);
          }
        }
        result.emplace_back(g + pow(e + pow(g, n - 1), n),
                            pow(e + pow(g, n - 1), n));  // relation Q4
      }
      return result;
    }

    // See Theorem 2.17 of https://arxiv.org/pdf/2211.02155.pdf
    // See Theorem 2.17 of https://arxiv.org/pdf/2211.02155.pdf
    std::vector<relation_type>
    order_preserving_cyclic_inverse_monoid(size_t n) {
      if (n < 3) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected argument to be at least 3, found %llu", uint64_t(n));
      }

      std::vector<relation_type> result;

      word_type x = 0_w, y = 1_w, e = range(2, n);

      // relations V1
      add_idempotent_rules(result, e);

      // relations V2
      result.emplace_back(x + y + x, x);
      result.emplace_back(y + x + y, y);

      // relations V3
      result.emplace_back(y + pow(x, 2) + y, x + pow(y, 2) + x);

      // relations V4
      add_commutes_rules(result, e);
      add_commutes_rules(result, e, {x + y});
      add_commutes_rules(result, e, {y + x});

      // relations V6
      for (size_t i = 0; i <= n - 4; ++i) {
        result.emplace_back(x + e[i + 1], e[i] + x);
      }

      // relations V7
      result.emplace_back(pow(x, 2) + y, e[n - 3] + x);
      result.emplace_back(y + pow(x, 2), x + e[0]);

      // relations V8
      result.emplace_back(y + x + prod(e, 0, n - 2, 1) + x + y,
                          x + prod(e, 0, n - 2, 1) + x + y);

      return result;
    }

    // See Theorem 2.8 of https://arxiv.org/pdf/2205.02196.pdf
    std::vector<relation_type> partial_isometries_cycle_graph_monoid(size_t n) {
      if (n < 3) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected argument to be at least 3, found %llu", uint64_t(n));
      }

      std::vector<relation_type> result;

      word_type g = {0};
      word_type h = {1};
      word_type e = {2};

      // Q1
      result.emplace_back(pow(g, n), ""_w);
      result.emplace_back(pow(h, 2), ""_w);
      result.emplace_back(h + g, pow(g, n - 1) + h);

      // Q2
      result.emplace_back(pow(e, 2), e);
      result.emplace_back(g + h + e + g + h, e);

      // Q3

      for (size_t j = 2; j <= n; ++j) {
        for (size_t i = 1; i < j; ++i) {
          result.emplace_back(e + pow(g, j - i) + e + pow(g, n - j + i),
                              pow(g, j - i) + e + pow(g, n - j + i) + e);
        }
      }

      if (n % 2 == 1) {
        // Q4
        result.emplace_back(h + g + pow(e + g, n - 2) + e,
                            pow(e + g, n - 2) + e);
      } else {
        // Q5
        result.emplace_back(
            h + g + pow(e + g, n / 2 - 1) + g + pow(e + g, n / 2 - 2) + e,
            pow(e + g, n / 2 - 1) + g + pow(e + g, n / 2 - 2) + e);
        result.emplace_back(h + pow(e + g, n - 1) + e, pow(e + g, n - 1) + e);
      }

      return result;
    }

    std::vector<relation_type> not_symmetric_group(size_t n, author val) {
      if (n < 4) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 1st argument to be at least 4, found %llu", uint64_t(n));
      }
      if (val
          == author::Guralnick + author::Kantor + author::Kassabov
                 + author::Lubotzky) {
        // See Section 2.2 of 'Presentations of finite simple groups: A
        // quantitative approach' J. Amer. Math. Soc. 21 (2008), 711-774

        std::vector<relation_type> result;

        for (size_t i = 0; i <= n - 2; ++i) {
          result.emplace_back(pow({i}, 2), ""_w);
        }

        for (size_t i = 0; i <= n - 2; ++i) {
          for (size_t j = 0; j <= n - 2; ++j) {
            if (i != j) {
              result.emplace_back(pow({i, j}, 3), ""_w);
            }
          }
        }

        for (size_t i = 0; i <= n - 2; ++i) {
          for (size_t j = 0; j <= n - 2; ++j) {
            if (i != j) {
              for (size_t k = 0; k <= n - 2; ++k) {
                if (k != i && k != j) {
                  result.emplace_back(pow({i, j, k}, 4), ""_w);
                }
              }
            }
          }
        }

        return result;

      } else {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 2nd argument to be author::Guralnick + author::Kantor",
            " + author::Kassabov + author::Lubotzky found %s",
            detail::to_string(val).c_str());
      }
    }

    // The remaining presentation functions are currently undocumented, as we
    // are not completely sure what they are.

    std::vector<relation_type> rook_monoid(size_t l, int q) {
      if (l < 2) {
        LIBSEMIGROUPS_EXCEPTION(
            "the 1st argument (size_t) must at least 2, found %llu",
            uint64_t(l));
      } else if (q != 0 && q != 1) {
        LIBSEMIGROUPS_EXCEPTION(
            "the 2nd argument (int) must be 0 or 1, found %llu", uint64_t(q));
      }

      std::vector<size_t> s;
      for (size_t i = 0; i < l; ++i) {
        s.push_back(i);  // 0 est \pi_0
      }

      // identity relations
      size_t                     id   = l;
      std::vector<relation_type> rels = {relation_type({id, id}, {id})};
      for (size_t i = 0; i < l; ++i) {
        rels.push_back({{s[i], id}, {s[i]}});
        rels.push_back({{id, s[i]}, {s[i]}});
      }

      switch (q) {
        case 0:
          for (size_t i = 0; i < l; ++i)
            rels.push_back({{s[i], s[i]}, {s[i]}});
          break;
        case 1:
          rels.push_back({{s[0], s[0]}, {s[0]}});
          for (size_t i = 1; i < l; ++i)
            rels.push_back({{s[i], s[i]}, {id}});
          break;
        default: {
        }
      }
      for (int i = 0; i < static_cast<int>(l); ++i) {
        for (int j = 0; j < static_cast<int>(l); ++j) {
          if (std::abs(i - j) >= 2) {
            rels.push_back({{s[i], s[j]}, {s[j], s[i]}});
          }
        }
      }

      for (size_t i = 1; i < l - 1; ++i) {
        rels.push_back({{s[i], s[i + 1], s[i]}, {s[i + 1], s[i], s[i + 1]}});
      }

      rels.push_back({{s[1], s[0], s[1], s[0]}, {s[0], s[1], s[0], s[1]}});
      rels.push_back({{s[1], s[0], s[1], s[0]}, {s[0], s[1], s[0]}});

      return rels;
    }

    std::vector<relation_type> renner_common_type_B_monoid(size_t l, int q) {
      // q is supposed to be 0 or 1
      std::vector<size_t> s;
      std::vector<size_t> e;
      for (size_t i = 0; i < l; ++i) {
        s.push_back(i);
      }
      for (size_t i = l; i < 2 * l + 1; ++i) {
        e.push_back(i);
      }
      size_t id = 2 * l + 1;

      std::vector<relation_type> rels = {relation_type({id, id}, {id})};
      // identity relations
      for (size_t i = 0; i < l; ++i) {
        rels.push_back({{s[i], id}, {s[i]}});
        rels.push_back({{id, s[i]}, {s[i]}});
        rels.push_back({{id, e[i]}, {e[i]}});
        rels.push_back({{e[i], id}, {e[i]}});
      }
      rels.push_back({{id, e[l]}, {e[l]}});
      rels.push_back({{e[l], id}, {e[l]}});

      switch (q) {
        case 0:
          for (size_t i = 0; i < l; ++i)
            rels.push_back({{s[i], s[i]}, {s[i]}});
          break;
        case 1:
          for (size_t i = 0; i < l; ++i)
            rels.push_back({{s[i], s[i]}, {id}});
          break;
        default: {
        }
      }
      for (int i = 0; i < static_cast<int>(l); ++i) {
        for (int j = 0; j < static_cast<int>(l); ++j) {
          if (std::abs(i - j) >= 2) {
            rels.push_back({{s[i], s[j]}, {s[j], s[i]}});
          }
        }
      }

      for (size_t i = 1; i < l - 1; ++i) {
        rels.push_back({{s[i], s[i + 1], s[i]}, {s[i + 1], s[i], s[i + 1]}});
      }

      rels.push_back({{s[1], s[0], s[1], s[0]}, {s[0], s[1], s[0], s[1]}});

      for (size_t i = 1; i < l; ++i) {
        for (size_t j = 0; j < i; ++j) {
          rels.push_back({{s[i], e[j]}, {e[j], s[i]}});
        }
      }

      for (size_t i = 0; i < l; ++i) {
        for (size_t j = i + 1; j < l + 1; ++j) {
          rels.push_back({{s[i], e[j]}, {e[j], s[i]}});
          rels.push_back({{s[i], e[j]}, {e[j]}});
        }
      }

      for (size_t i = 0; i < l + 1; ++i) {
        for (size_t j = 0; j < l + 1; ++j) {
          rels.push_back({{e[i], e[j]}, {e[j], e[i]}});
          rels.push_back({{e[i], e[j]}, {e[std::max(i, j)]}});
        }
      }

      for (size_t i = 0; i < l; ++i) {
        rels.push_back({{e[i], s[i], e[i]}, {e[i + 1]}});
      }

      return rels;
    }

    std::vector<relation_type> renner_type_B_monoid(size_t l,
                                                    int    q,
                                                    author val) {
      if (val == author::Godelle) {
        std::vector<size_t> s;
        std::vector<size_t> e;
        for (size_t i = 0; i < l; ++i) {
          s.push_back(i);
        }
        for (size_t i = l; i < 2 * l + 1; ++i) {
          e.push_back(i);
        }

        std::vector<relation_type> rels = renner_common_type_B_monoid(l, q);

        if (l >= 2)
          rels.push_back({{e[0], s[0], s[1], s[0], e[0]}, {e[2]}});

        return rels;
      } else {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 2nd argument to be author::Godelle, found %s",
            detail::to_string(val).c_str());
      }
    }

    std::vector<relation_type> RennerTypeBMonoid(size_t l, int q) {
      std::vector<size_t> s;
      std::vector<size_t> e;
      for (size_t i = 0; i < l; ++i) {
        s.push_back(i);
      }
      for (size_t i = l; i < 2 * l + 1; ++i) {
        e.push_back(i);
      }

      std::vector<relation_type> rels = renner_common_type_B_monoid(l, q);

      for (size_t i = 1; i < l; i++) {
        std::vector<size_t> new_rel = max_elt_B(i);
        new_rel.push_back(e[0]);
        new_rel.insert(new_rel.begin(), e[0]);
        rels.push_back({new_rel, {e[i + 1]}});
      }

      return rels;
    }

    std::vector<relation_type> renner_common_type_D_monoid(size_t l, int q) {
      // q is supposed to be 0 or 1
      std::vector<size_t> s;
      std::vector<size_t> e;
      for (size_t i = 0; i < l; ++i) {
        s.push_back(i);  // 0 est \pi_1^f et 1 est \pi_1^e
      }
      for (size_t i = l; i < 2 * l + 1; ++i) {
        e.push_back(i);
      }
      size_t f  = 2 * l + 1;
      size_t id = 2 * l + 2;

      std::vector<relation_type> rels = {relation_type({id, id}, {id})};
      // identity relations
      for (size_t i = 0; i < l; ++i) {
        rels.push_back({{s[i], id}, {s[i]}});
        rels.push_back({{id, s[i]}, {s[i]}});
        rels.push_back({{id, e[i]}, {e[i]}});
        rels.push_back({{e[i], id}, {e[i]}});
      }
      rels.push_back({{id, e[l]}, {e[l]}});
      rels.push_back({{e[l], id}, {e[l]}});
      rels.push_back({{id, f}, {f}});
      rels.push_back({{f, id}, {f}});

      switch (q) {
        case 0:
          for (size_t i = 0; i < l; ++i)
            rels.push_back({{s[i], s[i]}, {s[i]}});
          break;
        case 1:
          for (size_t i = 0; i < l; ++i)
            rels.push_back({{s[i], s[i]}, {id}});
          break;
        default: {
        }
      }
      for (int i = 1; i < static_cast<int>(l); ++i) {  // tout sauf \pi_1^f
        for (int j = 1; j < static_cast<int>(l); ++j) {
          if (std::abs(i - j) >= 2) {
            rels.push_back({{s[i], s[j]}, {s[j], s[i]}});
          }
        }
      }
      for (int i = 3; i < static_cast<int>(l);
           ++i) {  // \pi_1^f avec les \pi_i pour i>2
        rels.push_back({{s[0], s[i]}, {s[i], s[0]}});
      }
      rels.push_back({{s[0], s[1]}, {s[1], s[0]}});  // \pi_1^f avec \pi_1^e

      for (size_t i = 1; i < l - 1; ++i) {  // tresses
        rels.push_back({{s[i], s[i + 1], s[i]}, {s[i + 1], s[i], s[i + 1]}});
      }
      if (l > 2) {
        rels.push_back(
            {{s[0], s[2], s[0]}, {s[2], s[0], s[2]}});  // tresse de \pi_1^f
      }

      for (size_t i = 2; i < l; ++i) {  // commutation, attention début à 2
        for (size_t j = 0; j < i; ++j) {
          rels.push_back({{s[i], e[j]}, {e[j], s[i]}});
        }
        rels.push_back({{s[i], f}, {f, s[i]}});
      }
      rels.push_back({{s[0], f}, {f, s[0]}});        // commutation \pi_1^f et f
      rels.push_back({{s[1], e[0]}, {e[0], s[1]}});  // commutation \pi_1^e et e

      for (size_t i = 1; i < l; ++i) {  // absorption
        for (size_t j = i + 1; j < l + 1; ++j) {
          rels.push_back({{s[i], e[j]}, {e[j], s[i]}});
          rels.push_back({{s[i], e[j]}, {e[j]}});
          if (i == 1) {  // cas particulier \pi_1^f
            rels.push_back({{s[0], e[j]}, {e[j], s[0]}});
            rels.push_back({{s[0], e[j]}, {e[j]}});
          }
        }
      }

      for (size_t i = 0; i < l + 1; ++i) {  // produit e_i
        for (size_t j = 0; j < l + 1; ++j) {
          rels.push_back({{e[i], e[j]}, {e[j], e[i]}});
          rels.push_back({{e[i], e[j]}, {e[std::max(i, j)]}});
        }
        if (i > 1) {
          rels.push_back({{f, e[i]}, {e[i], f}});
          rels.push_back({{f, e[i]}, {e[i]}});
        }
      }
      rels.push_back({{f, f}, {f}});
      rels.push_back({{f, e[0]}, {e[1]}});
      rels.push_back({{e[0], f}, {e[1]}});

      for (size_t i = 2; i < l; ++i) {
        rels.push_back({{e[i], s[i], e[i]}, {e[i + 1]}});
      }
      rels.push_back({{e[0], s[0], e[0]}, {e[2]}});
      rels.push_back({{f, s[1], f}, {e[2]}});

      return rels;
    }

    std::vector<relation_type> renner_type_D_monoid(size_t l,
                                                    int    q,
                                                    author val) {
      if (val == author::Godelle) {
        std::vector<size_t> s;
        std::vector<size_t> e;
        for (size_t i = 0; i < l; ++i) {
          s.push_back(i);
        }
        for (size_t i = l; i < 2 * l + 1; ++i) {
          e.push_back(i);
        }
        size_t f = 2 * l + 1;

        std::vector<relation_type> rels = renner_common_type_D_monoid(l, q);

        if (l >= 3) {
          rels.push_back({{e[0], s[0], s[2], s[1], f}, {e[3]}});
          rels.push_back({{f, s[1], s[2], s[0], e[0]}, {e[3]}});
        }
        return rels;
      } else {
        LIBSEMIGROUPS_EXCEPTION(
            "expected 2nd argument to be author::Godelle, found %s",
            detail::to_string(val).c_str());
      }
    }

    std::vector<relation_type> RennerTypeDMonoid(size_t l, int q) {
      std::vector<size_t> s;
      std::vector<size_t> e;
      for (size_t i = 0; i < l; ++i) {
        s.push_back(i);
      }
      for (size_t i = l; i < 2 * l + 1; ++i) {
        e.push_back(i);
      }
      size_t f = 2 * l + 1;

      std::vector<relation_type> rels = renner_common_type_D_monoid(l, q);

      for (size_t i = 2; i < l; i++) {
        std::vector<size_t> new_rel_f = max_elt_D(i, 0);
        std::vector<size_t> new_rel_e = max_elt_D(i, 1);

        if (i % 2 == 0) {
          new_rel_e.insert(new_rel_e.begin(), f);
          new_rel_e.push_back(e[0]);
          rels.push_back({new_rel_e, {e[i + 1]}});

          new_rel_f.insert(new_rel_f.begin(), e[0]);
          new_rel_f.push_back(f);
          rels.push_back({new_rel_f, {e[i + 1]}});
        } else {
          new_rel_e.insert(new_rel_e.begin(), f);
          new_rel_e.push_back(f);
          rels.push_back({new_rel_e, {e[i + 1]}});

          new_rel_f.insert(new_rel_f.begin(), e[0]);
          new_rel_f.push_back(e[0]);
          rels.push_back({new_rel_f, {e[i + 1]}});
        }
      }
      return rels;
    }

    std::ostringstream& operator<<(std::ostringstream& oss, author val) {
      std::string sep = "";
      if (static_cast<uint64_t>(val) & static_cast<uint64_t>(author::Machine)) {
        oss << sep << "author::Machine";
        sep = " + ";
      }
      if (static_cast<uint64_t>(val)
          & static_cast<uint64_t>(author::Aizenstat)) {
        oss << sep << "author::Aizenstat";
        sep = " + ";
      }
      if (static_cast<uint64_t>(val)
          & static_cast<uint64_t>(author::Burnside)) {
        oss << sep << "author::Burnside";
        sep = " + ";
      }
      if (static_cast<uint64_t>(val)
          & static_cast<uint64_t>(author::Carmichael)) {
        oss << sep << "author::Carmichael";
        sep = " + ";
      }
      if (static_cast<uint64_t>(val) & static_cast<uint64_t>(author::Coxeter)) {
        oss << sep << "author::Coxeter";
        sep = " + ";
      }
      if (static_cast<uint64_t>(val) & static_cast<uint64_t>(author::Easdown)) {
        oss << sep << "author::Easdown";
        sep = " + ";
      }
      if (static_cast<uint64_t>(val)
          & static_cast<uint64_t>(author::FitzGerald)) {
        oss << sep << "author::FitzGerald";
        sep = " + ";
      }
      if (static_cast<uint64_t>(val) & static_cast<uint64_t>(author::Godelle)) {
        oss << sep << "author::Godelle";
        sep = " + ";
      }
      if (static_cast<uint64_t>(val)
          & static_cast<uint64_t>(author::Guralnick)) {
        oss << sep << "author::Guralnick";
        sep = " + ";
      }
      if (static_cast<uint64_t>(val) & static_cast<uint64_t>(author::Iwahori)) {
        oss << sep << "author::Iwahori";
        sep = " + ";
      }
      if (static_cast<uint64_t>(val) & static_cast<uint64_t>(author::Kantor)) {
        oss << sep << "author::Kantor";
        sep = " + ";
      }
      if (static_cast<uint64_t>(val)
          & static_cast<uint64_t>(author::Kassabov)) {
        oss << sep << "author::Kassabov";
        sep = " + ";
      }
      if (static_cast<uint64_t>(val)
          & static_cast<uint64_t>(author::Lubotzky)) {
        oss << sep << "author::Lubotsky";
        sep = " + ";
      }
      if (static_cast<uint64_t>(val) & static_cast<uint64_t>(author::Miller)) {
        oss << sep << "author::Miller";
        sep = " + ";
      }
      if (static_cast<uint64_t>(val) & static_cast<uint64_t>(author::Moore)) {
        oss << sep << "author::Moore";
        sep = " + ";
      }
      if (static_cast<uint64_t>(val) & static_cast<uint64_t>(author::Moser)) {
        oss << sep << "author::Moser";
        sep = " + ";
      }
      if (static_cast<uint64_t>(val) & static_cast<uint64_t>(author::Sutov)) {
        oss << sep << "author::Sutov";
        sep = " + ";
      }
      return oss;
    }
  }  // namespace fpsemigroup
}  // namespace libsemigroups
