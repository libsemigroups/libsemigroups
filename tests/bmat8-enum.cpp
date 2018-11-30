#include <bitset>
#include <iostream>
#include <fstream>
#include <unordered_set>  // for unordered_set
#include <vector>         // for vector

#include "catch.hpp"
#include "test-main.hpp"

#include "bmat8.hpp"
#include "element.hpp"
#include "froidure-pin.hpp"
#include "timer.hpp"

#include "../extern/bliss-0.73/graph.hh"

namespace bmat8_enum {
  using namespace libsemigroups;
  using bliss_digraph = typename bliss_digraphs::Digraph; 
  static size_t                            n;
  static size_t                            max;
  static std::vector<uint8_t>              rows;
  static std::unordered_set<BMat8>         set;
  static std::vector<BMat8>                out;
  static std::array<bool, 256>             row_seen;
  static std::vector<std::vector<uint8_t>> row_orb_by_row;
  static size_t                            first_row;
  static int                               min_ones;
  bliss_digraphs::Stats                    stats;

  BMat8 BMat8_from_rows(std::vector<uint8_t> const& rows) {
    LIBSEMIGROUPS_ASSERT(rows.size() <= 8);
    LIBSEMIGROUPS_ASSERT(0 < rows.size());
    size_t out = 0;
    for (size_t i = 0; i < rows.size(); ++i){
      out = (out << 8) | rows[i];
    }
    out = out << 8 * (8 - rows.size());
    return BMat8(out);
  }
  
  BMat8 BMat8_from_rows_old(std::vector<uint8_t> const& rows) {
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
  
  template <size_t N>
  BMat8 BMat8_from_perm(typename Perm<N>::type x) {
    LIBSEMIGROUPS_ASSERT(N <= 8);
    std::vector<uint8_t> rows;
    for (size_t i = 0; i < N; ++i) {
      rows.push_back(1 << (7 - x[i]));
    }
    return BMat8_from_rows(rows);
  }

  bliss_digraph bliss_digraph_from_BMat8(BMat8 bm, size_t dim = 8) {
    bliss_digraph out = bliss_digraph(2 * dim);
    size_t x = bm.to_int();
    for (size_t i = 0; i < dim; ++ i) {
      out.change_color(i, 0);
      out.change_color(dim + i, 1);
      for (size_t j = 0; j < dim; ++ j) {
        if ((x >> (63 - 8 * i - j)) & 1) {
          out.add_edge(i, dim + j);
        }
      }
    }
    return out;
  }

  void bliss_hook_function(void*               param,
                           const unsigned int  n,
                           const unsigned int* aut) {
  }

  BMat8 permuted_BMat8(BMat8 bm, size_t dim, const unsigned int* perm) {
    std::vector<uint8_t> bm_rows = bm.rows();
    std::vector<uint8_t> perm_rows(dim, 0);
    for (size_t i = 0; i < dim; ++ i) {
      perm_rows[perm[i]] = bm_rows[i];      
    }
    BMat8 row_perm_bm = BMat8_from_rows(perm_rows);
 
    bm_rows = row_perm_bm.transpose().rows();
    for (size_t i = 0; i < dim; ++ i) {
      perm_rows[perm[i + dim] - dim] = bm_rows[i];      
    }
    return BMat8_from_rows(perm_rows).transpose();
  }

  BMat8 canonical_BMat8(BMat8 bm, size_t dim) {
    bliss_digraph dg = bliss_digraph_from_BMat8(bm, dim);
    return permuted_BMat8(
        bm, dim, dg.canonical_form(stats, &bliss_hook_function, nullptr));
  }

  bool is_row_reduced(BMat8 bm) {
    return bm.nr_rows() == bm.row_space_basis().nr_rows();
  }

  bool is_col_reduced(BMat8 bm) {
    return is_row_reduced(bm.transpose());
  }

  bool is_row_trim(BMat8 bm, size_t dim = 8) {
    std::vector<uint8_t> rows = bm.rows();
    for (size_t i = 0; i < dim; ++i) {
      for (size_t j = 0; j < dim; ++j) {
        if (rows[i] && (i != j) && ((rows[i] | rows[j]) == rows[j])) {
          return false;
        }
      }
    }
    return true;
  }
  
  std::bitset<256> row_space_bitset(BMat8 bm) {
    std::bitset<256> lookup;
    std::vector<uint8_t> row_vec = bm.row_space_basis().rows();
    auto                 last = std::remove(row_vec.begin(), row_vec.end(), 0);
    row_vec.erase(last, row_vec.end());
    for (uint8_t x : row_vec) {
      lookup.set(x);
    }
    lookup.set(0);
    std::vector<uint8_t> row_space(row_vec.begin(), row_vec.end());
    for (size_t i = 0; i < row_space.size(); ++i) {
      for (uint8_t row : row_vec) {
        uint8_t x = row_space[i] | row;
        if (!lookup[x]) {
          row_space.push_back(x);
          lookup.set(x);
        }
      }
    }
    return lookup;
  }
  
  bool is_col_trim(BMat8 bm, size_t dim = 8) {
    return is_row_trim(bm.transpose(), dim);
  }

  bool is_trim(BMat8 bm, size_t dim) {
    return is_row_trim(bm, dim) && is_col_trim(bm, dim);
  }

  int nr_ones(uint8_t x) {
    return __builtin_popcount(x);
  }
  
  void dive(size_t k, bool trim = false) {
    LIBSEMIGROUPS_ASSERT(k > 0);
    if (k < n - 1) {
      for (size_t row = rows[k - 1]; row < max; ++row) {
        if (!row_seen[row] && nr_ones(row) >= min_ones) {
          if (trim) {
            bool trim_fail = false;
            for (size_t i = first_row; i < k; ++i) {
              if (rows[i] && ((rows[i] | row) == row)) {
                trim_fail = true;
                break;
              }
            }
            if (trim_fail) {
              continue;
            }
          }
          rows[k] = row;


          // UPDATE ROW ORB
          for (size_t i = first_row; i < k; ++i) {
            for (uint8_t old_row : row_orb_by_row[i]) {
              uint8_t new_row = old_row | row;
              if (!row_seen[new_row]) {
                row_orb_by_row[k].push_back(new_row);
                row_seen[new_row] = true;
              }
            }
          }

          // DIVE
          dive(k + 1, trim);

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
          if (trim) {
            bool trim_fail = false;
            for (size_t i = first_row; i < k; ++i) {
              if (rows[i] && ((rows[i] | row) == row)) {
                trim_fail = true;
                break;
              }
            }
            if (trim_fail) {
              continue;
            }
          }
          rows[k] = row;
          BMat8 bm = BMat8_from_rows(rows);
          // move the matrix to the right place
          bm = BMat8(bm.to_int() << (8 - n));
          if (is_col_reduced(bm) && (!trim || is_col_trim(bm, n))) {
            BMat8 canon = canonical_BMat8(bm, n);
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

  std::vector<BMat8>& bmat_enum(size_t dim, bool trim = false) {
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

        dive(i + 1, trim);

        row_seen[row] = false;
      }
      rows[i] = 0;
    }
    out.push_back(BMat8(1 << (9 * (8 - dim))));
    out.push_back(BMat8(0));
    return out;
  }
}  // namespace bmat8_enum

namespace libsemigroups {
  using namespace bmat8_enum;
  LIBSEMIGROUPS_TEST_CASE("BMat8 enum", "001", "size 4", "[extreme]") {
    BMat8 x = BMat8({{0, 0, 0}, {0, 0, 1}, {0, 1, 1}});
    BMat8 y = BMat8({{0, 0, 0}, {0, 0, 1}, {1, 0, 1}});
    bliss_digraph dgx = bliss_digraph_from_BMat8(x, 3);
    bliss_digraph dgy = bliss_digraph_from_BMat8(y, 3);

    dgx.write_dot("dgx.dot");

    bliss_digraph dgx2 = bliss_digraph(6);
    dgx2.change_color(3, 1);
    dgx2.change_color(4, 1);
    dgx2.change_color(5, 1);
    dgx2.add_edge(1, 5);
    dgx2.add_edge(2, 4);
    dgx2.add_edge(2, 5);

    REQUIRE(dgx.cmp(dgx2) == 0);

    BMat8 canonx = permuted_BMat8(
        x, 3, dgx.canonical_form(stats, &bliss_hook_function, nullptr));
    BMat8 canony = permuted_BMat8(
        y, 3, dgy.canonical_form(stats, &bliss_hook_function, nullptr));

    REQUIRE(canonx == canony);

    std::ofstream o;


    //TODO : check all the trim sizes

    std::vector<BMat8> bmat_enum_4 = bmat_enum(4);
    REQUIRE(bmat_enum_4.size() == 60);
    o.open("bmat_enum_4.txt", std::ios::out | std::ios::trunc);
    for (BMat8 i : bmat_enum_4) {
      o << i.to_int() << "\n";
    }
    o.close();
   
    /*
    std::vector<BMat8> bmat_trim_enum_4 = bmat_enum(4, true);
    REQUIRE(bmat_trim_enum_4.size() == 10);
    o.open("bmat_trim_enum_4.txt", std::ios::out | std::ios::trunc);
    for (BMat8 i : bmat_trim_enum_4) {
      o << i.to_int() << "\n";
    }
    o.close();

    std::vector<BMat8> bmat_enum_5 = bmat_enum(5);
    REQUIRE(bmat_enum_5.size() == 877);
    o.open("bmat_enum_5.txt", std::ios::out | std::ios::trunc);
    for (BMat8 i : bmat_enum_5) {
      o << i.to_int() << "\n";
    }
    o.close();

    std::vector<BMat8> bmat_trim_enum_5 = bmat_enum(5, true);
    REQUIRE(bmat_trim_enum_5.size() == 32);
    o.open("bmat_trim_enum_5.txt", std::ios::out | std::ios::trunc);
    for (BMat8 i : bmat_trim_enum_5) {
      o << i.to_int() << "\n";
    }
    o.close();
*/
    /*
    std::vector<BMat8> bmat_enum_6 = bmat_enum(6);
    REQUIRE(bmat_enum_6.size() == 42944);
    o.open("bmat_enum_6.txt", std::ios::out | std::ios::trunc);
    for (BMat8 i : bmat_enum_6) {
      o << i.to_int() << "\n";
    }
    o.close();

    std::vector<BMat8> bmat_trim_enum_6 = bmat_enum(6, true);
    REQUIRE(bmat_trim_enum_6.size() == 394);
    o.open("bmat_trim_enum_6.txt", std::ios::out | std::ios::trunc);
    for (BMat8 i : bmat_trim_enum_6) {
      o << i.to_int() << "\n";
    }
    o.close();
    */
    /*
    std::vector<BMat8> bmat_enum_7 = bmat_enum(7);
    REQUIRE(bmat_enum_7.size() == 7339704);
    o.open("bmat_enum_7.txt", std::ios::out | std::ios::trunc);
    for (BMat8 i : bmat_enum_7) {
      o << i.to_int() << "\n";
    }
    o.close();
    */

    /*
    std::vector<BMat8> bmat_trim_enum_7 = bmat_enum(7, true);
    REQUIRE(bmat_trim_enum_7.size() == 34014);
    o.open("bmat_trim_enum_7.txt", std::ios::out | std::ios::trunc);
    for (BMat8 i : bmat_trim_enum_7) {
      o << i.to_int() << "\n";
    }
    o.close();
    
    std::vector<BMat8> bmat_trim_enum_8 = bmat_enum(8, true);
    std::cout << bmat_trim_enum_8.size() << std::endl;
    //REQUIRE(bmat_trim_enum_8.size() == 34014);
    o.open("bmat_trim_enum_8.txt", std::ios::out | std::ios::trunc);
    for (BMat8 i : bmat_trim_enum_8) {
      o << i.to_int() << "\n";
    }
    o.close();
    */
  }
  
  LIBSEMIGROUPS_TEST_CASE("BMat8 enum", "002", "filter 6", "[extreme]") {
    std::vector<BMat8> bmat6_enum;
    std::ifstream f("bmat_trim_enum_6.txt");
    std::string line;
    while (std::getline(f, line)) {
      bmat6_enum.push_back(BMat8(std::stoul(line)));
    }
    f.close();
    REQUIRE(bmat6_enum.size() == 394);

    bmat6_enum.push_back(BMat8({{1, 1, 0, 0, 0, 0},
                                {0, 1, 0, 0, 0, 0},
                                {0, 0, 1, 0, 0, 0},
                                {0, 0, 0, 1, 0, 0},
                                {0, 0, 0, 0, 1, 0},
                                {0, 0, 0, 0, 0, 1}}));

    
    using Perm = typename Perm<6>::type;
    const std::vector<Perm> S6_gens
        = {Perm({1, 2, 3, 4, 5, 0}), Perm({1, 0, 2, 3, 4, 5})};

    FroidurePin<Perm> S6(S6_gens);
    REQUIRE(S6.size() == 720);

    std::vector<BMat8> S6_bmats;
    for (Perm p : S6) {
      S6_bmats.push_back(BMat8_from_perm<6>(p));
    }

    std::vector<std::vector<std::bitset<256>>> row_spaces(
        65, std::vector<std::bitset<256>>());
    for (BMat8 x : bmat6_enum) {
      for (BMat8 y : S6_bmats) {
        std::bitset<256> bitset = row_space_bitset(x * y);
        row_spaces[bitset.count()].push_back(bitset);
      }
    }
    size_t sum = 0;
    for (std::vector<std::bitset<256>> vec : row_spaces) {
      sum += vec.size();
    }
    REQUIRE(sum == bmat6_enum.size() * 720);
    std::vector<BMat8> filtered;
    for (BMat8 bm : bmat6_enum) {
      std::bitset<256> bitset = row_space_bitset(bm);
      bool found = false;
      for (size_t i = bitset.count() + 1; i < 64; ++i) {
        for (size_t j = 0; j < row_spaces[i].size(); ++j) {
          if ((bitset | row_spaces[i][j]) == row_spaces[i][j]) {
            // TODO: not this
            found = true;
            i = 257;
            break;
          }
        }
      }
      if (!found) {
        filtered.push_back(bm);
      }
    }
    
    REQUIRE(filtered.size() == 66);

    std::ofstream o;
    o.open("bmat_gens_6.txt", std::ios::out | std::ios::trunc);
    for (BMat8 i : filtered) {
      o << i.to_int() << "\n";
    }
    for (Perm p : S6_gens) {
      o << BMat8_from_perm<6>(p).to_int() << "\n";
    }
    o.close();
  }


  LIBSEMIGROUPS_TEST_CASE("BMat8 enum", "003", "filter 7", "[extreme]") {
    std::vector<BMat8> bmat7_enum;
    std::ifstream f("bmat_trim_enum_7.txt");
    std::string line;
    while (std::getline(f, line)) {
      bmat7_enum.push_back(BMat8(std::stoul(line)));
    }
    f.close();
    REQUIRE(bmat7_enum.size() == 34014);

    std::cout << "finished reading!" << std::endl;

    bmat7_enum.push_back(BMat8({{1, 1, 0, 0, 0, 0, 0},
                                {0, 1, 0, 0, 0, 0, 0},
                                {0, 0, 1, 0, 0, 0, 0},
                                {0, 0, 0, 1, 0, 0, 0},
                                {0, 0, 0, 0, 1, 0, 0},
                                {0, 0, 0, 0, 0, 1, 0},
                                {1, 0, 0, 0, 0, 0, 1}}));

    using Perm = typename Perm<7>::type;
    const std::vector<Perm> S7_gens
        = {Perm({1, 2, 3, 4, 5, 6, 0}), Perm({1, 0, 2, 3, 4, 5, 6})};

    FroidurePin<Perm> S7(S7_gens);
    REQUIRE(S7.size() == 5040);
    std::cout << "finished computing S7!" << std::endl;

    std::vector<BMat8> S7_bmats;
    for (Perm p : S7) {
      S7_bmats.push_back(BMat8_from_perm<7>(p));
    }

    std::vector<std::vector<std::bitset<256>>> row_spaces(
        129, std::vector<std::bitset<256>>(0));
    size_t count = 0;
    for (BMat8 x : bmat7_enum) {
      for (BMat8 y : S7_bmats) {
        std::bitset<256> bitset = row_space_bitset(x * y);
        row_spaces[bitset.count()].push_back(bitset);
      }
      std::cout << count << std::endl;
      count++;
    }
    std::cout << "got here!" << std::endl;
    std::vector<BMat8> filtered;
    size_t i = 0;
    for (BMat8 bm : bmat7_enum) {
      std::cout << i << std::endl;
      std::bitset<256> bitset = row_space_bitset(bm);
      bool found = false;
      for (size_t i = bitset.count() + 1; i < 128; ++i) {
        for (size_t j = 0; j < row_spaces[i].size(); ++j) {
          if ((bitset | row_spaces[i][j]) == row_spaces[i][j]) {
            // TODO: not this
            found = true;
            i = 257;
            break;
          }
        }
      }
      if (!found) {
        filtered.push_back(bm);
      }
      ++i;
    }
    std::ofstream o;
    o.open("bmat_gens_7.txt", std::ios::out | std::ios::trunc);
    for (BMat8 i : filtered) {
      o << i.to_int() << "\n";
    }
    for (Perm p : S7_gens) {
      o << BMat8_from_perm<6>(p).to_int() << "\n";
    }
    o.close();
    REQUIRE(filtered.size() == 421); 
    
  }
} // namespace libsemigroups
