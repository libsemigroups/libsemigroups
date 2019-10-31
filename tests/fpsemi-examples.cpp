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

#include <cstdlib>  // for size_t, abs

namespace libsemigroups {
  std::vector<relation_type> RookMonoid(size_t l, int q) {
    // q is supposed to be 0 or 1

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
        // default: assert(FALSE)
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
        // default: assert(FALSE)
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

  std::vector<size_t> max_elt_B(size_t i) {
    std::vector<size_t> t(0);
    for (int end = i; end >= 0; end--) {
      for (int k = 0; k <= end; k++) {
        t.push_back(k);
      }
    }
    return t;
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
        // default: assert(FALSE)
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
}  // namespace libsemigroups
