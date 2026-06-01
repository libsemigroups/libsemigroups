//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2026 James D. Mitchell
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

namespace libsemigroups {
  namespace matrix {
    template <typename Mat>
    Mat pow(Mat const& x, typename Mat::scalar_type e) {
      using scalar_type = typename Mat::scalar_type;

      if constexpr (std::is_signed<scalar_type>::value) {
        if (e < 0) {
          LIBSEMIGROUPS_EXCEPTION(
              "negative exponent, expected value >= 0, found {}", e);
        }
      }

      throw_if_not_square(x);

      typename Mat::semiring_type const* sr = nullptr;

      if constexpr (IsMatWithSemiring<Mat>) {
        sr = x.semiring();
      }

      if (e == 0) {
        return x.one();
      }

      auto y = Mat(x);
      if (e == 1) {
        return y;
      }
      auto z = (e % 2 == 0 ? x.one() : y);

      Mat tmp(sr, x.number_of_rows(), x.number_of_cols());
      while (e > 1) {
        tmp.product_inplace_no_checks(y, y);
        std::swap(y, tmp);
        e /= 2;
        if (e % 2 == 1) {
          tmp.product_inplace_no_checks(z, y);
          std::swap(z, tmp);
        }
      }
      return z;
    }

    template <typename Mat, size_t R, size_t C, typename Container>
    void bitset_rows(Container&&                          views,
                     detail::StaticVector1<BitSet<C>, R>& result) {
      using RowView    = typename Mat::RowView;
      using value_type = typename std::decay_t<Container>::value_type;
      // std::vector<bool> is used as value_type in the benchmarks
      static_assert(IsBMat<Mat>, "IsBMat<Mat> must be true!");
      static_assert(std::is_same_v<value_type, RowView>
                        || std::is_same_v<value_type, std::vector<bool>>,
                    "Container::value_type must equal Mat::RowView or "
                    "std::vector<bool>!!");
      static_assert(R <= BitSet<1>::max_size(),
                    "R must be at most BitSet<1>::max_size()!");
      static_assert(C <= BitSet<1>::max_size(),
                    "C must be at most BitSet<1>::max_size()!");
      LIBSEMIGROUPS_ASSERT(views.size() <= R);
      LIBSEMIGROUPS_ASSERT(views.empty() || views[0].size() <= C);
      for (auto const& v : views) {
        result.emplace_back(v.cbegin(), v.cend());
      }
    }

    template <typename Mat, size_t R, size_t C, typename Container>
    auto bitset_rows(Container&& views) {
      using RowView    = typename Mat::RowView;
      using value_type = typename std::decay_t<Container>::value_type;
      static_assert(IsBMat<Mat>, "IsBMat<Mat> must be true!");
      static_assert(std::is_same_v<value_type, RowView>
                        || std::is_same_v<value_type, std::vector<bool>>,
                    "Container::value_type must equal Mat::RowView or "
                    "std::vector<bool>!!");
      static_assert(R <= BitSet<1>::max_size(),
                    "R must be at most BitSet<1>::max_size()!");
      static_assert(C <= BitSet<1>::max_size(),
                    "C must be at most BitSet<1>::max_size()!");
      LIBSEMIGROUPS_ASSERT(views.size() <= R);
      LIBSEMIGROUPS_ASSERT(views.empty() || views[0].size() <= C);
      detail::StaticVector1<BitSet<C>, R> result;
      bitset_rows<Mat>(std::forward<Container>(views), result);
      return result;
    }

    template <typename Mat, size_t R, size_t C>
    void bitset_rows(Mat const&                           x,
                     detail::StaticVector1<BitSet<C>, R>& result) {
      static_assert(IsBMat<Mat>, "IsBMat<Mat> must be true!");
      static_assert(R <= BitSet<1>::max_size(),
                    "R must be at most BitSet<1>::max_size()!");
      static_assert(C <= BitSet<1>::max_size(),
                    "C must be at most BitSet<1>::max_size()!");
      LIBSEMIGROUPS_ASSERT(x.number_of_cols() <= C);
      LIBSEMIGROUPS_ASSERT(x.number_of_rows() <= R);
      bitset_rows<Mat>(rows(x), result);
    }

    template <typename Mat>
    auto bitset_rows(Mat const& x) {
      static_assert(IsBMat<Mat>, "IsBMat<Mat> must be true!");
      LIBSEMIGROUPS_ASSERT(x.number_of_rows() <= BitSet<1>::max_size());
      LIBSEMIGROUPS_ASSERT(x.number_of_cols() <= BitSet<1>::max_size());
      size_t const M = detail::BitSetCapacity<Mat>::value;
      return bitset_rows<Mat, M, M>(rows(x));
    }

    template <typename Mat, typename Container>
    void bitset_row_basis(Container&& rows, std::decay_t<Container>& result) {
      using value_type = typename std::decay_t<Container>::value_type;
      static_assert(IsBMat<Mat>, "IsBMat<Mat> must be true!");
      static_assert(IsBitSet<value_type> || detail::IsStdBitSet<value_type>,
                    "Container::value_type must be BitSet or std::bitset");
      LIBSEMIGROUPS_ASSERT(rows.size() <= BitSet<1>::max_size());
      LIBSEMIGROUPS_ASSERT(rows.empty()
                           || rows[0].size() <= BitSet<1>::max_size());

      std::sort(rows.begin(), rows.end(), detail::LessBitSet());
      // Remove duplicates
      rows.erase(std::unique(rows.begin(), rows.end()), rows.end());
      for (size_t i = 0; i < rows.size(); ++i) {
        value_type cup;
        cup.reset();
        for (size_t j = 0; j < i; ++j) {
          if ((rows[i] & rows[j]) == rows[j]) {
            cup |= rows[j];
          }
        }
        for (size_t j = i + 1; j < rows.size(); ++j) {
          if ((rows[i] & rows[j]) == rows[j]) {
            cup |= rows[j];
          }
        }
        if (cup != rows[i]) {
          result.push_back(rows[i]);
        }
      }
    }

    template <typename Mat, typename Container>
    std::decay_t<Container> bitset_row_basis(Container&& rows) {
      using value_type = typename std::decay_t<Container>::value_type;
      static_assert(IsBMat<Mat>, "IsBMat<Mat> must be true!");
      static_assert(IsBitSet<value_type> || detail::IsStdBitSet<value_type>,
                    "Container::value_type must be BitSet or std::bitset");
      LIBSEMIGROUPS_ASSERT(rows.size() <= BitSet<1>::max_size());
      LIBSEMIGROUPS_ASSERT(rows.empty()
                           || rows[0].size() <= BitSet<1>::max_size());
      std::decay_t<Container> result;
      bitset_row_basis<Mat>(std::forward<Container>(rows), result);
      return result;
    }

    template <typename Mat, size_t M>
    detail::StaticVector1<BitSet<M>, M> bitset_row_basis(Mat const& x) {
      static_assert(IsBMat<Mat>, "IsBMat<Mat> must be true!");
      LIBSEMIGROUPS_ASSERT(x.number_of_rows() <= BitSet<1>::max_size());
      LIBSEMIGROUPS_ASSERT(x.number_of_cols() <= BitSet<1>::max_size());
      detail::StaticVector1<BitSet<M>, M> result;
      bitset_row_basis<Mat>(bitset_rows(x), result);
      return result;
    }

    template <typename Mat, typename Container>
    void bitset_row_basis(Mat const& x, Container& result) {
      using value_type = typename Container::value_type;
      static_assert(IsBMat<Mat>, "IsBMat<Mat> must be true!");
      static_assert(IsBitSet<value_type> || detail::IsStdBitSet<value_type>,
                    "Container::value_type must be BitSet or std::bitset");
      LIBSEMIGROUPS_ASSERT(x.number_of_rows() <= BitSet<1>::max_size());
      LIBSEMIGROUPS_ASSERT(x.number_of_cols() <= BitSet<1>::max_size());
      bitset_row_basis<Mat>(bitset_rows(x), result);
    }

    template <typename Mat, typename Container>
    std::enable_if_t<IsMaxPlusTruncMat<Mat>>
    row_basis(Container&& views, std::decay_t<Container>& result) {
      using value_type = typename std::decay_t<Container>::value_type;
      static_assert(std::is_same_v<value_type, typename Mat::RowView>,
                    "Container::value_type must be Mat::RowView");
      using scalar_type = typename Mat::scalar_type;
      using Row         = typename Mat::Row;

      if (views.empty()) {
        return;
      }

      LIBSEMIGROUPS_ASSERT(result.empty());

      std::sort(views.begin(), views.end());
      Row tmp1(views[0]);

      for (size_t r1 = 0; r1 < views.size(); ++r1) {
        if (r1 == 0 || views[r1] != views[r1 - 1]) {
          std::fill(tmp1.begin(), tmp1.end(), tmp1.scalar_zero());
          for (size_t r2 = 0; r2 < r1; ++r2) {
            scalar_type max_scalar = matrix::threshold(tmp1);
            for (size_t c = 0; c < tmp1.number_of_cols(); ++c) {
              if (views[r2][c] == tmp1.scalar_zero()) {
                continue;
              }
              if (views[r1][c] >= views[r2][c]) {
                if (views[r1][c] != matrix::threshold(tmp1)) {
                  max_scalar
                      = std::min(max_scalar, views[r1][c] - views[r2][c]);
                }
              } else {
                max_scalar = tmp1.scalar_zero();
                break;
              }
            }
            if (max_scalar != tmp1.scalar_zero()) {
              tmp1 += views[r2] * max_scalar;
            }
          }
          if (tmp1 != views[r1]) {
            result.push_back(views[r1]);
          }
        }
      }
    }

    template <typename Mat, typename Container>
    std::enable_if_t<IsBMat<Mat>> row_basis(Container&&              views,
                                            std::decay_t<Container>& result) {
      using RowView    = typename Mat::RowView;
      using value_type = typename std::decay_t<Container>::value_type;
      // std::vector<bool> is used as value_type in the benchmarks
      static_assert(std::is_same_v<value_type, RowView>
                        || std::is_same_v<value_type, std::vector<bool>>,
                    "Container::value_type must equal Mat::RowView or "
                    "std::vector<bool>!!");

      if (views.empty()) {
        return;
      }

      // Convert RowViews to BitSets
      size_t const M = detail::BitSetCapacity<Mat>::value;
      // if views is an rvalue reference and we forward it here, then we cannot
      // use it later in this function!
      auto br           = bitset_rows<Mat, M, M>(views);
      using bitset_type = typename decltype(br)::value_type;

      // Map for converting bitsets back to row views
      std::unordered_map<bitset_type, size_t> lookup;
      LIBSEMIGROUPS_ASSERT(br.size() == views.size());
      for (size_t i = 0; i < br.size(); ++i) {
        lookup.insert({br[i], i});
      }

      // Compute rowbasis using bitsets + convert back to rowviews
      for (auto const& bs : bitset_row_basis<Mat>(br)) {
        auto it = lookup.find(bs);
        LIBSEMIGROUPS_ASSERT(it != lookup.end());
        result.push_back(views[it->second]);
      }
    }

    template <typename Mat, typename Container, typename>
    std::decay_t<Container> row_basis(Container&& rows) {
      using value_type = typename std::decay_t<Container>::value_type;
      static_assert(IsMatrix<Mat>, "IsMatrix<Mat> must be true!");
      static_assert(std::is_same_v<value_type, typename Mat::RowView>,
                    "Container::value_type must be Mat::RowView");

      std::decay_t<Container> result;
      row_basis<Mat>(std::forward<Container>(rows), result);
      return result;
    }

    template <typename Mat, typename Container, typename>
    void row_basis_rows(Mat const& x, Container& result) {
      LIBSEMIGROUPS_ASSERT(result.empty());
      for (auto y : row_basis<Mat>(x)) {
        result.push_back(typename Mat::Row(y));
      }
    }

    template <typename Mat, typename Container, typename>
    detail::StaticVector1<typename Mat::Row, Mat::nr_rows>
    row_basis_rows(Mat const& x) {
      detail::StaticVector1<typename Mat::Row, Mat::nr_rows> container;
      row_basis_rows(x, container);
      return container;
    }

    template <typename Mat, typename Container, typename>
    detail::StaticVector1<typename Mat::Row, Mat::nr_rows>
    row_basis_rows(Container&& rows) {
      using value_type = typename std::decay_t<Container>::value_type;
      static_assert(IsMatrix<Mat>, "IsMatrix<Mat> must be true!");
      static_assert(std::is_same_v<value_type, typename Mat::Row>,
                    "Container::value_type must be Mat::Row");

      detail::StaticVector1<typename Mat::RowView, Mat::nr_rows> rvs;
      std::unordered_map<typename Mat::scalar_type*, size_t>     lookup;

      for (size_t i = 0; i < rows.size(); ++i) {
        auto rv = typename Mat::RowView(rows[i]);
        rvs.push_back(rv);
        lookup.insert({&(*rv.begin()), i});
      }
      std::decay_t<Container> result;
      for (auto rv : row_basis<Mat>(rvs)) {
        auto&& row = rows[lookup.at(&(*rv.begin()))];
        result.push_back(std::forward<decltype(row)>(row));
      }
      return result;
    }

    // TODO: merge this with the above function
    template <typename Mat, typename Container, typename>
    std::vector<typename Mat::Row> row_basis_rows(Container&& rows) {
      using value_type = typename std::decay_t<Container>::value_type;
      static_assert(IsMatrix<Mat>, "IsMatrix<Mat> must be true!");
      static_assert(std::is_same_v<value_type, typename Mat::Row>,
                    "Container::value_type must be Mat::Row");

      std::vector<typename Mat::RowView>                     rvs;
      std::unordered_map<typename Mat::scalar_type*, size_t> lookup;

      for (size_t i = 0; i < rows.size(); ++i) {
        auto rv = typename Mat::RowView(rows[i]);
        rvs.push_back(rv);
        lookup.insert({&(*rv.begin()), i});
      }
      std::decay_t<Container> result;
      for (auto rv : row_basis<Mat>(rvs)) {
        auto&& row = rows[lookup.at(&(*rv.begin()))];
        result.push_back(std::forward<decltype(row)>(row));
      }
      return result;
    }

    template <typename Mat, typename>
    size_t row_space_size(Mat const& x) {
      size_t const M = detail::BitSetCapacity<Mat>::value;
      auto         bitset_row_basis_
          = bitset_row_basis<Mat>(bitset_rows<Mat, M, M>(rows(x)));

      std::unordered_set<BitSet<M>> st;
      st.insert(bitset_row_basis_.cbegin(), bitset_row_basis_.cend());
      std::vector<BitSet<M>> orb(bitset_row_basis_.cbegin(),
                                 bitset_row_basis_.cend());
      for (size_t i = 0; i < orb.size(); ++i) {
        for (auto& row : bitset_row_basis_) {
          auto cup = orb[i];
          for (size_t j = 0; j < x.number_of_rows(); ++j) {
            cup.set(j, cup[j] || row[j]);
          }
          if (st.insert(cup).second) {
            // cup is a copy of orb[i] so ok to move
            orb.push_back(std::move(cup));
          }
        }
      }
      return orb.size();
    }

  }  // namespace matrix
}  // namespace libsemigroups
