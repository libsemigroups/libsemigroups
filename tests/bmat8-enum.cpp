#include <iostream>
#include <unordered_set>  // for unordered_set
#include <vector>         // for vector
#include <x86intrin.h>

#include "catch.hpp"
#include "test-main.hpp"

#include "bmat8.hpp"
#include "timer.hpp"

namespace bmat8_enum {
  using namespace libsemigroups;
  static size_t                            n;
  static size_t                            max;
  static std::vector<uint8_t>              rows;
  static std::unordered_set<BMat8>         set;
  static std::vector<BMat8>                out;
  static std::array<bool, 256>             row_seen;
  static std::vector<std::vector<uint8_t>> row_orb_by_row;
  static size_t                            first_row;
  static int                               min_ones;

  BMat8 BMat8_from_rows(std::vector<uint8_t> const& rows) {
    LIBSEMIGROUPS_ASSERT(rows.size() <= 8);
    LIBSEMIGROUPS_ASSERT(0 < rows.size());
    size_t out = 0;
    size_t i   = 0;
    for (uint8_t x : rows){
      out |= (static_cast<size_t>(x) << 8 * (7 - i));
      ++i;
    }
    return BMat8(out);
  }

  BMat8 canonical_bmat8(BMat8 bm, size_t n) {
    return bm;
  }

  bool is_row_reduced(BMat8 bm) {
    return bm.nr_rows() == bm.row_space_basis().nr_rows();
  }

  bool is_col_reduced(BMat8 bm) {
    return is_row_reduced(bm.transpose());
  }

  int nr_ones(uint8_t x) {
    return __builtin_popcount(x);
  }
  
  void dive(size_t k) {
    LIBSEMIGROUPS_ASSERT(k > 0);
    if (k < n - 1) {
      for (size_t row = rows[k - 1]; row < max; ++row) {
        if (!row_seen[row] && nr_ones(row) >= min_ones) {
          rows[k] = row;

          // UPDATE ROW ORB
          for (size_t i = first_row; i < k; ++i) {
            for (uint8_t old_row : row_orb_by_row[i]) {
              uint8_t new_row = old_row | row;
              if (!row_seen[row]) {
                row_orb_by_row[k].push_back(row);
                row_seen[row] = true;
              }
            }
          }

          // DIVE
          dive(k + 1);

          // RESET ROW ORB
          for (uint8_t row : row_orb_by_row[k]) {
            row_seen[row] = false;
          }
          row_orb_by_row[k].clear();
        }
      }
    } else if (k == n - 1) {
      for (uint8_t row = rows[k - 1]; row <= max; ++row) {
        if (!row_seen[row] && nr_ones(row) >= min_ones) {
          rows[k] = row;
          BMat8 bm = BMat8_from_rows(rows);
          // move the matrix to the right place
          bm = BMat8(bm.to_int() << 4);
          if (is_col_reduced(bm)) {
            std::cout << bm << std::endl;
            BMat8 canon = canonical_bmat8(bm, n);
            if (set.find(canon) == set.end()) {
              set.insert(canon);
              out.push_back(canon);
            }
          } else {
          }
        }
      }
      rows[k] = 0;
    }
  }

  std::vector<BMat8>& bmat_enum(size_t dim) {
    n        = dim;
    max      = (1 << n) - 1;
    rows     = std::vector<uint8_t>(n, 0);
    set      = std::unordered_set<BMat8>();
    out      = std::vector<BMat8>();
    row_seen = std::array<bool, 256>();
    for (bool& x : row_seen) {
      x = false;
    }
    row_seen[0]    = true;
    row_orb_by_row
        = std::vector<std::vector<uint8_t>>(8, std::vector<uint8_t>());

    for (size_t i = 0; i < n - 1; ++i) {
      first_row = i;
      for (size_t j = 1; j < n; ++j) {
        uint8_t row = (1 << j) - 1;
        rows[i]     = row;
        min_ones     = nr_ones(row);
        row_orb_by_row[i].clear();
        row_orb_by_row[i].push_back(0);
        row_orb_by_row[i].push_back(row);
        row_seen[row] = true;

        dive(i + 1);

        row_seen[row] = false;
      }
      rows[i] = 0;
    }
    out.push_back(BMat8(1));
    out.push_back(BMat8(0));
    return out;
  }
}  // namespace bmat8_enum

namespace libsemigroups {
  using namespace bmat8_enum;
  LIBSEMIGROUPS_TEST_CASE("BMat8 enum", "001", "size 4", "[quick]") {
    REQUIRE(bmat_enum(4).size() == 363); 
  }
} // namespace libsemigroups
