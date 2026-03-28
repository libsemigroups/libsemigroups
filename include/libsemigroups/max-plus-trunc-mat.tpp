
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2021-2026 Finn Smith
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
  template <typename Mat>
  void ImageRightAction<Mat,
                        typename LambdaValue<Mat>::type,
                        std::enable_if_t<IsMaxPlusTruncMat<Mat>>>::
       operator()(result_type& res, result_type const& pt, Mat const& x) const {
    using scalar_type = typename Mat::scalar_type;
    res.clear();
    // TODO this is bad but I don't see any good ways around it
    const_cast<Mat*>(&x)->transpose();
    auto        rows = matrix::rows(x);
    result_type prod_rows;

    for (size_t r = 0; r < pt.size(); ++r) {
      typename Mat::Row row;
      for (size_t c = 0; c < Mat::nr_cols; ++c) {
        row(0, c) = std::inner_product(
            pt[r].cbegin(),
            pt[r].cend(),
            rows[c].cbegin(),
            MaxPlusZero<scalar_type>()(),
            MaxPlusPlus<scalar_type>(),
            MaxPlusTruncProd<detail::IsTruncMatHelper<Mat>::threshold,
                             scalar_type>());
      }
      prod_rows.emplace_back(std::move(row));
    }
    const_cast<Mat*>(&x)->transpose();
    res = std::move(matrix::row_basis_rows<Mat>(prod_rows));
  }

  template <typename Mat>
  size_t Rank<Mat, RankState<Mat>, std::enable_if_t<IsMaxPlusTruncMat<Mat>>>::
         operator()(Mat const& x) const {
    using row_type = typename Mat::Row;
    auto row_views = matrix::rows(x);
    RightAction<row_type, row_type, matrix::RowSum<row_type>> orb;
    row_type                                                  seed;
    for (auto it = seed.begin(); it != seed.end(); ++it) {
      *it = MaxPlusZero<typename Mat::scalar_type>()();
    }
    orb.add_seed(seed);
    std::unordered_set<row_type, Hash<row_type>> gens;
    for (auto const& row : row_views) {
      for (typename Mat::scalar_type i = 0;
           i <= detail::IsTruncMatHelper<Mat>::threshold;
           ++i) {
        gens.insert(row_type(row * i));
      }
    }
    for (auto& gen : gens) {
      orb.add_generator(gen);
    }
    orb.run();
    return orb.size();
  }
}  // namespace libsemigroups
