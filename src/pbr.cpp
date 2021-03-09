//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 James D. Mitchell
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

// This file contains implementations of the PBR class.

#include "libsemigroups/pbr.hpp"

#include <algorithm>  // for all_of, fill
#include <ostream>    // for operator<<, cha...
#include <string>     // for operator+, char...
#include <thread>     // for thread

#include "libsemigroups/containers.hpp"  // for DynamicArray2
#include "libsemigroups/debug.hpp"       // for LIBSEMIGROUPS_A...
#include "libsemigroups/exception.hpp"   // for LIBSEMIGROUPS_E...
#include "libsemigroups/string.hpp"      // for to_string

namespace libsemigroups {

  namespace {
    std::vector<std::vector<uint32_t>>
    process_left_right(std::vector<std::vector<int32_t>> const& left,
                       std::vector<std::vector<int32_t>> const& right) {
      size_t                             n = left.size();
      std::vector<std::vector<uint32_t>> out;
      std::vector<uint32_t>              v;

      if (n != right.size()) {
        LIBSEMIGROUPS_EXCEPTION("the two vectors must have the same length");
      }
      if (n > 0x40000000) {
        LIBSEMIGROUPS_EXCEPTION("too many points!");
      }
      for (std::vector<int32_t> vec : left) {
        v = std::vector<uint32_t>();
        for (int32_t x : vec) {
          if (x == 0 || x < -static_cast<int32_t>(n)
              || x > static_cast<int32_t>(n)) {
            LIBSEMIGROUPS_EXCEPTION(
                "value out of bounds in the 1st argument, expected values in "
                "[%d, -1] or [1, %d] but found %d",
                -n,
                n,
                x);
          }
          if (x > 0) {
            v.push_back(static_cast<uint32_t>(x - 1));
          }
        }
        // We have to go backwards through the vector to add the negative
        // entries, so that users can input those negative entries in the
        // natural order
        for (auto it = vec.rbegin(); it < vec.rend(); ++it) {
          if (*it < 0) {
            v.push_back(static_cast<uint32_t>(n - *it - 1));
          }
        }
        out.push_back(v);
      }
      for (std::vector<int32_t> vec : right) {
        v = std::vector<uint32_t>();
        for (int32_t x : vec) {
          if (x == 0 || x < -static_cast<int32_t>(n)
              || x > static_cast<int32_t>(n)) {
            LIBSEMIGROUPS_EXCEPTION(
                "value out of bounds in the 1st argument, expected values in "
                "[%d, -1] or [1, %d] but found %d",
                -n,
                n,
                x);
          }
          if (x > 0) {
            v.push_back(static_cast<uint32_t>(x - 1));
          }
        }
        for (auto it = vec.rbegin(); it < vec.rend(); ++it) {
          if (*it < 0) {
            v.push_back(static_cast<uint32_t>(n - *it - 1));
          }
        }
        out.push_back(v);
      }
      return out;
    }

    void unite_rows(detail::DynamicArray2<bool>& out,
                    detail::DynamicArray2<bool>& tmp,
                    size_t const&                i,
                    size_t const&                j) {
      for (size_t k = 0; k < out.number_of_cols(); k++) {
        out.set(i, k, (out.get(i, k) || tmp.get(j, k + 1)));
      }
    }

    void y_dfs(std::vector<bool>&,
               std::vector<bool>&,
               detail::DynamicArray2<bool>&,
               uint32_t const&,
               uint32_t const&,
               PBR const* const,
               PBR const* const,
               size_t const&);

    void x_dfs(std::vector<bool>&           x_seen,
               std::vector<bool>&           y_seen,
               detail::DynamicArray2<bool>& tmp,
               uint32_t const&              n,
               uint32_t const&              i,
               PBR const* const             x,
               PBR const* const             y,
               size_t const&                adj) {
      if (!x_seen[i]) {
        x_seen[i] = true;
        for (auto const& j : (*x)[i]) {
          if (j < n) {
            tmp.set(adj, j + 1, true);
          } else {
            y_dfs(x_seen, y_seen, tmp, n, j - n, x, y, adj);
          }
        }
      }
    }

    void y_dfs(std::vector<bool>&           x_seen,
               std::vector<bool>&           y_seen,
               detail::DynamicArray2<bool>& tmp,
               uint32_t const&              n,
               uint32_t const&              i,
               PBR const* const             x,
               PBR const* const             y,
               size_t const&                adj) {
      if (!y_seen[i]) {
        y_seen[i] = true;
        for (auto const& j : (*y)[i]) {
          if (j >= n) {
            tmp.set(adj, j + 1, true);
          } else {
            x_dfs(x_seen, y_seen, tmp, n, j + n, x, y, adj);
          }
        }
      }
    }

  }  // namespace

  // TODO(later) Should be defined for all of the new element types
  PBR operator*(PBR const& x, PBR const& y) {
    PBR xy(x.degree());
    xy.product_inplace(x, y);
    return xy;
  }

  // TODO(later) Should be defined for all of the new element types
  bool operator!=(PBR const& x, PBR const& y) {
    return !(x == y);
  }

  void validate(PBR const& x) {
    size_t n = x._vector.size();
    if (n % 2 == 1) {
      LIBSEMIGROUPS_EXCEPTION("expected argument of even length");
    }
    for (size_t u = 0; u < n; ++u) {
      for (auto const& v : x._vector.at(u)) {
        if (v >= n) {
          LIBSEMIGROUPS_EXCEPTION(
              "entry out of bounds, vertex " + detail::to_string(u)
              + " is adjacent to " + detail::to_string(v)
              + ", should be less than " + detail::to_string(n));
        }
      }
    }
    for (size_t u = 0; u < n; ++u) {
      if (!std::is_sorted(x._vector.at(u).cbegin(), x._vector.at(u).cend())) {
        LIBSEMIGROUPS_EXCEPTION("the adjacencies of vertex ",
                                detail::to_string(u).c_str(),
                                " are unsorted");
      }
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // Partitioned binary relations (PBRs)
  ////////////////////////////////////////////////////////////////////////

  PBR::PBR(std::vector<std::vector<uint32_t>> const& vec) : _vector(vec) {}

  PBR::PBR(std::initializer_list<std::vector<uint32_t>> const& vec)
      : _vector(vec) {}

  PBR::PBR(size_t degree)
      : PBR(std::vector<std::vector<uint32_t>>(degree * 2,
                                               std::vector<uint32_t>())) {}

  PBR::PBR(std::initializer_list<std::vector<int32_t>> const& left,
           std::initializer_list<std::vector<int32_t>> const& right)
      : PBR(process_left_right(left, right)) {}

  std::ostringstream& operator<<(std::ostringstream& os, PBR const& pbr) {
    if (pbr.degree() == 0) {
      os << "{}";
      return os;
    }
    os << "{";
    for (size_t i = 0; i < pbr.degree() * 2 - 1; ++i) {
      os << "{";
      if (!pbr[i].empty()) {
        for (size_t j = 0; j + 1 < pbr[i].size(); ++j) {
          os << pbr[i][j] << ", ";
        }
        os << detail::to_string(pbr[i].back());
      }
      os << "}, ";
    }

    os << "{";
    if (!pbr[2 * pbr.degree() - 1].empty()) {
      for (size_t j = 0; j + 1 < pbr[2 * pbr.degree() - 1].size(); ++j) {
        os << pbr[2 * pbr.degree() - 1][j] << ", ";
      }
      os << detail::to_string(pbr[2 * pbr.degree() - 1].back());
    }
    os << "}}";
    return os;
  }

  std::ostream& operator<<(std::ostream& os, PBR const& pbr) {
    os << detail::to_string(pbr);
    return os;
  }

  size_t PBR::degree() const noexcept {
    return _vector.size() / 2;
  }

  PBR PBR::identity() const {
    std::vector<std::vector<uint32_t>> adj;
    size_t                             n = this->degree();
    adj.reserve(2 * n);
    for (uint32_t i = 0; i < 2 * n; i++) {
      adj.push_back(std::vector<uint32_t>());
    }
    for (uint32_t i = 0; i < n; i++) {
      adj[i].push_back(i + n);
      adj[i + n].push_back(i);
    }
    return PBR(adj);
  }

  PBR PBR::identity(size_t n) {
    std::vector<std::vector<uint32_t>> adj;
    adj.reserve(2 * n);
    for (uint32_t i = 0; i < 2 * n; i++) {
      adj.push_back(std::vector<uint32_t>());
    }
    for (uint32_t i = 0; i < n; i++) {
      adj[i].push_back(i + n);
      adj[i + n].push_back(i);
    }
    return PBR(adj);
  }

  void PBR::product_inplace(PBR const& xx, PBR const& yy, size_t thread_id) {
    LIBSEMIGROUPS_ASSERT(xx.degree() == yy.degree());
    LIBSEMIGROUPS_ASSERT(xx.degree() == this->degree());
    LIBSEMIGROUPS_ASSERT(&xx != this && &yy != this);

    static std::vector<std::vector<bool>> _x_seen(
        std::thread::hardware_concurrency() + 1);
    static std::vector<std::vector<bool>> _y_seen(
        std::thread::hardware_concurrency() + 1);
    static std::vector<detail::DynamicArray2<bool>> _out(
        std::thread::hardware_concurrency() + 1);
    static std::vector<detail::DynamicArray2<bool>> _tmp(
        std::thread::hardware_concurrency() + 1);

    PBR const& x = static_cast<PBR const&>(xx);
    PBR const& y = static_cast<PBR const&>(yy);

    uint32_t const n = this->degree();

    std::vector<bool>&           x_seen = _x_seen.at(thread_id);
    std::vector<bool>&           y_seen = _y_seen.at(thread_id);
    detail::DynamicArray2<bool>& tmp    = _tmp.at(thread_id);
    detail::DynamicArray2<bool>& out    = _out.at(thread_id);

    if (x_seen.size() != 2 * n) {
      x_seen.clear();
      x_seen.resize(2 * n, false);
      y_seen.clear();
      y_seen.resize(2 * n, false);
      out.clear();
      out.add_cols(2 * n);
      out.add_rows(2 * n);
      tmp.clear();
      tmp.add_cols(2 * n + 1);
    } else {
      std::fill(x_seen.begin(), x_seen.end(), false);
      std::fill(y_seen.begin(), y_seen.end(), false);
      std::fill(out.begin(), out.end(), false);
      std::fill(tmp.begin(), tmp.end(), false);
    }

    for (size_t i = 0; i < n; i++) {
      for (auto const& j : x[i]) {
        if (j < n) {
          out.set(i, j, true);
        } else if (j < tmp.number_of_rows() && tmp.get(j, 0)) {
          unite_rows(out, tmp, i, j);
        } else {
          if (j >= tmp.number_of_rows()) {
            tmp.add_rows(j - tmp.number_of_rows() + 1);
          }
          tmp.set(j, 0, true);
          x_seen[i] = true;
          y_dfs(x_seen, y_seen, tmp, n, j - n, &x, &y, j);
          unite_rows(out, tmp, i, j);
          std::fill(x_seen.begin(), x_seen.end(), false);
          std::fill(y_seen.begin(), y_seen.end(), false);
        }
        if (std::all_of(out.begin_row(i), out.end_row(i), [](bool val) {
              return val;
            })) {
          break;
        }
      }
    }

    for (size_t i = n; i < 2 * n; i++) {
      for (auto const& j : y[i]) {
        if (j >= n) {
          out.set(i, j, true);
        } else if (j < tmp.number_of_rows() && tmp.get(j, 0)) {
          unite_rows(out, tmp, i, j);
        } else {
          if (j >= tmp.number_of_rows()) {
            tmp.add_rows(j - tmp.number_of_rows() + 1);
          }
          tmp.set(j, 0, true);
          y_seen[i] = true;
          x_dfs(x_seen, y_seen, tmp, n, j + n, &x, &y, j);
          unite_rows(out, tmp, i, j);
          std::fill(x_seen.begin(), x_seen.end(), false);
          std::fill(y_seen.begin(), y_seen.end(), false);
        }
        if (std::all_of(out.begin_row(i), out.end_row(i), [](bool val) {
              return val;
            })) {
          break;
        }
      }
    }

    for (size_t i = 0; i < 2 * n; i++) {
      _vector[i].clear();
      for (size_t j = 0; j < 2 * n; j++) {
        if (out.get(i, j)) {
          _vector[i].push_back(j);
        }
      }
    }
  }

}  // namespace libsemigroups
