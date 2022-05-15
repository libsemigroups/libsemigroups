//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 Florent Hivert
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

#include "fpsemi-examples.hpp"

#include <algorithm>  // for max, for_each
#include <cstdint>    // for int64_t
#include <cstdlib>    // for abs
#include <numeric>    // for iota

#include "libsemigroups/exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/types.hpp"      // for word_type, relation_type

namespace libsemigroups {
  namespace {
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
    void add_group_relations(std::vector<word_type> const& alphabet,
                             word_type                     id,
                             std::vector<word_type> const& inverse,
                             std::vector<relation_type>&   relations) {
      for (size_t i = 0; i < alphabet.size(); ++i) {
        relations.emplace_back(alphabet[i] * inverse[i], id);
        if (alphabet[i] != inverse[i]) {
          relations.emplace_back(inverse[i] * alphabet[i], id);
        }
        if (alphabet[i] != id) {
          relations.emplace_back(alphabet[i] * id, alphabet[i]);
          relations.emplace_back(id * alphabet[i], alphabet[i]);
        }
      }
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
  }  // namespace

  std::vector<relation_type> RookMonoid(size_t l, int q) {
    if (l < 2) {
      LIBSEMIGROUPS_EXCEPTION(
          "the 1st argument (size_t) must at least 2, found %llu", uint64_t(l));
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

  std::vector<relation_type> RennerCommonTypeBMonoid(size_t l, int q) {
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

  std::vector<relation_type> EGTypeBMonoid(size_t l, int q) {
    std::vector<size_t> s;
    std::vector<size_t> e;
    for (size_t i = 0; i < l; ++i) {
      s.push_back(i);
    }
    for (size_t i = l; i < 2 * l + 1; ++i) {
      e.push_back(i);
    }

    std::vector<relation_type> rels = RennerCommonTypeBMonoid(l, q);

    if (l >= 2)
      rels.push_back({{e[0], s[0], s[1], s[0], e[0]}, {e[2]}});

    return rels;
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

    std::vector<relation_type> rels = RennerCommonTypeBMonoid(l, q);

    for (size_t i = 1; i < l; i++) {
      std::vector<size_t> new_rel = max_elt_B(i);
      new_rel.push_back(e[0]);
      new_rel.insert(new_rel.begin(), e[0]);
      rels.push_back({new_rel, {e[i + 1]}});
    }

    return rels;
  }

  std::vector<relation_type> RennerCommonTypeDMonoid(size_t l, int q) {
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

  std::vector<relation_type> EGTypeDMonoid(size_t l, int q) {
    std::vector<size_t> s;
    std::vector<size_t> e;
    for (size_t i = 0; i < l; ++i) {
      s.push_back(i);
    }
    for (size_t i = l; i < 2 * l + 1; ++i) {
      e.push_back(i);
    }
    size_t f = 2 * l + 1;

    std::vector<relation_type> rels = RennerCommonTypeDMonoid(l, q);

    if (l >= 3) {
      rels.push_back({{e[0], s[0], s[2], s[1], f}, {e[3]}});
      rels.push_back({{f, s[1], s[2], s[0], e[0]}, {e[3]}});
    }
    return rels;
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

    std::vector<relation_type> rels = RennerCommonTypeDMonoid(l, q);

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

  std::vector<relation_type> Stell(size_t l) {
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

  std::vector<relation_type> Fibonacci(size_t r, size_t n) {
    std::vector<relation_type> result;
    for (size_t i = 0; i < n; ++i) {
      word_type lhs(r, 0);
      std::iota(lhs.begin(), lhs.end(), i);
      std::for_each(lhs.begin(), lhs.end(), [&n](size_t& x) { x %= n; });
      result.emplace_back(lhs, word_type({(i + r) % n}));
    }
    return result;
  }

  std::vector<relation_type> Plactic(size_t n) {
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

  std::vector<relation_type> Stylic(size_t n) {
    std::vector<relation_type> result = Plactic(n);
    for (size_t a = 0; a < n; ++a) {
      result.emplace_back(word_type({a, a}), word_type({a}));
    }
    return result;
  }

  std::vector<relation_type> SymmetricGroup1(size_t n) {
    word_type const e = {0};
    word_type const a = {1};
    word_type const b = {2};
    word_type const B = {3};

    std::vector<relation_type> result;
    add_group_relations({e, a, b, B}, e, {e, a, B, b}, result);
    result.emplace_back(b ^ n, e);
    result.emplace_back((a * b) ^ (n - 1), e);
    result.emplace_back((a * B * a * b) ^ 3, e);
    for (size_t j = 2; j <= n - 2; ++j) {
      result.emplace_back((a * (B ^ j) * a * (b ^ j)) ^ 2, e);
    }
    return result;
  }

  std::vector<relation_type> SymmetricGroup2(size_t n) {
    std::vector<word_type> alphabet = {{0}};
    for (size_t i = 0; i < n; ++i) {
      alphabet.push_back({i});
    }
    std::vector<relation_type> result;
    add_group_relations(alphabet, {0}, alphabet, result);

    for (size_t j = 1; j <= n - 2; ++j) {
      result.emplace_back(word_type({j, j + 1, j, j + 1, j, j + 1}),
                          word_type({0}));
    }
    for (size_t l = 3; l <= n - 1; ++l) {
      for (size_t k = 1; k <= l - 2; ++k) {
        result.emplace_back(word_type({k, l, k, l}), word_type({0}));
      }
    }
    return result;
  }

  // From https://core.ac.uk/reader/33304940
  std::vector<relation_type> DualSymmetricInverseMonoidEEF(size_t n) {
    if (n < 3) {
      LIBSEMIGROUPS_EXCEPTION("expected value of at least 3");
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
  }

  std::vector<relation_type> UniformBlockBijectionMonoidF(size_t n) {
    if (n < 3) {
      LIBSEMIGROUPS_EXCEPTION("expected value of at least 3");
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
    for (size_t i = 0; i < n + 1; ++i) {
      alphabet.push_back({i});
    }
    auto                       t = alphabet.back();
    auto                       e = alphabet.front();
    std::vector<relation_type> result;
    add_monoid_relations(alphabet, e, result);
    auto s = alphabet.cbegin();

    // S in Theorem 3 (same as DualSymmetricInverseMonoidEEF)
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
  }

  // From Theorem 41 in doi:10.1016/j.jalgebra.2011.04.008
  std::vector<relation_type> PartitionMonoidEast41(size_t n) {
    if (n < 4) {
      LIBSEMIGROUPS_EXCEPTION(
          "the 1st argument (size_t) must be at least 4, found %llu",
          uint64_t(n));
    }
    word_type s  = {0};
    word_type c  = {1};
    word_type e  = {2};
    word_type t  = {3};
    word_type id = {4};

    std::vector<word_type>     alphabet = {s, c, e, t, id};
    std::vector<relation_type> result;
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

  std::vector<relation_type> SingularBrauer(size_t n) {
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
            result.emplace_back(t[i][j] * t[i][k] * t[j][k], t[i][j] * t[j][k]);
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
  std::vector<relation_type> OrientationPreserving(size_t n) {
    word_type                  b = {0};
    word_type                  u = {1};
    word_type                  e = {2};
    std::vector<relation_type> result;

    add_monoid_relations({b, u, e}, e, result);

    result.emplace_back(b ^ n, e);
    result.emplace_back(u ^ 2, u);
    result.emplace_back((u * b) ^ n, u * b);
    result.emplace_back(b * ((u * (b ^ (n - 1))) ^ (n - 1)),
                        (u * (b ^ (n - 1))) ^ (n - 1));
    for (size_t i = 2; i <= n - 1; ++i) {
      result.emplace_back(u * (b ^ i) * ((u * b) ^ (n - 1)) * (b ^ (n - i)),
                          (b ^ i) * ((u * b) ^ (n - 1)) * (b ^ (n - i)) * u);
    }
    return result;
  }

  // Also from https://doi.org/10.1007/s10012-000-0001-1
  std::vector<relation_type> OrientationReversing(size_t n) {
    word_type                  e = {0};
    word_type                  b = {1};
    word_type                  u = {2};
    word_type                  c = {3};
    std::vector<relation_type> result;

    add_monoid_relations({e, b, u, c}, e, result);

    result.emplace_back(b ^ n, e);
    result.emplace_back(u ^ 2, u);
    result.emplace_back((u * b) ^ n, u * b);
    result.emplace_back(b * ((u * (b ^ (n - 1))) ^ (n - 1)),
                        (u * (b ^ (n - 1))) ^ (n - 1));
    for (size_t i = 2; i <= n - 1; ++i) {
      result.emplace_back(u * (b ^ i) * ((u * b) ^ (n - 1)) * (b ^ (n - i)),
                          (b ^ i) * ((u * b) ^ (n - 1)) * (b ^ (n - i)) * u);
    }
    result.emplace_back(c ^ 2, e);
    result.emplace_back(b * c, c * (b ^ (n - 1)));
    result.emplace_back(u * c, c * ((b * u) ^ (n - 1)));
    result.emplace_back(c * (u * (b ^ (n - 1)) ^ (n - 2)),
                        (b ^ (n - 2)) * ((u * (b ^ (n - 1))) ^ (n - 2)));

    return result;
  }

  // From Theorem 2.2 in https://doi.org/10.1093/qmath/haab001
  std::vector<relation_type> TemperleyLieb(size_t n) {
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
  std::vector<relation_type> Brauer(size_t n) {
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

}  // namespace libsemigroups
