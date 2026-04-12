//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2026 James Mitchell
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope That it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

namespace libsemigroups::matrix {

  ////////////////////////////////////////////////////////////////////////

  template <typename Mat>
  auto throw_if_bad_entry(Mat const& x) -> std::enable_if_t<IsIntMat<Mat>> {
    using scalar_type = typename Mat::scalar_type;
    auto it           = std::find_if(x.cbegin(), x.cend(), [](scalar_type val) {
      return val == POSITIVE_INFINITY || val == NEGATIVE_INFINITY;
    });
    if (it != x.cend()) {
      auto [r, c] = x.coords(it);
      LIBSEMIGROUPS_EXCEPTION("invalid entry, expected entries to be integers, "
                              "but found {} in entry ({}, {})",
                              detail::entry_repr(*it),
                              r,
                              c);
    }
  }

  // TODO(v4) deprecated delete
  template <typename Mat>
  auto throw_if_bad_entry(Mat const&, typename Mat::scalar_type val)
      -> std::enable_if_t<IsIntMat<Mat>> {
    if (val == POSITIVE_INFINITY || val == NEGATIVE_INFINITY) {
      LIBSEMIGROUPS_EXCEPTION("invalid entry, expected entries to be integers, "
                              "but found {}",
                              detail::entry_repr(val));
    }
  }

  ////////////////////////////////////////////////////////////////////////

  template <typename Mat>
  auto throw_if_bad_entry(Mat const& m) -> std::enable_if_t<IsBMat<Mat>> {
    using scalar_type = typename Mat::scalar_type;
    auto it           = std::find_if_not(
        m.cbegin(), m.cend(), [](scalar_type x) { return x == 0 || x == 1; });
    if (it != m.cend()) {
      auto [r, c] = m.coords(it);
      LIBSEMIGROUPS_EXCEPTION(
          "invalid entry, expected 0 or 1 but found {} in entry ({}, {})",
          detail::entry_repr(*it),
          r,
          c);
    }
  }

  // TODO(v4) deprecated delete
  template <typename Mat>
  auto throw_if_bad_entry(Mat const&, typename Mat::scalar_type val)
      -> std::enable_if_t<IsBMat<Mat>> {
    if (val != 0 && val != 1) {
      LIBSEMIGROUPS_EXCEPTION("invalid entry, expected 0 or 1 but found {} ",
                              detail::entry_repr(val));
    }
  }

  ////////////////////////////////////////////////////////////////////////

  template <typename Mat>
  auto throw_if_bad_entry(Mat const& x) -> std::enable_if_t<IsMaxPlusMat<Mat>> {
    using scalar_type = typename Mat::scalar_type;
    auto it           = std::find_if(x.cbegin(), x.cend(), [](scalar_type val) {
      return val == POSITIVE_INFINITY;
    });
    if (it != x.cend()) {
      auto [r, c] = x.coords(it);
      LIBSEMIGROUPS_EXCEPTION(
          "invalid entry, expected entries to be integers or {} (= {}), "
          "but found {} (= {}) in entry ({}, {})",
          entry_repr(NEGATIVE_INFINITY),
          static_cast<scalar_type>(NEGATIVE_INFINITY),
          entry_repr(POSITIVE_INFINITY),
          static_cast<scalar_type>(POSITIVE_INFINITY),
          r,
          c);
    }
  }

  // TODO(v4) deprecated delete
  template <typename Mat>
  auto throw_if_bad_entry(Mat const&, typename Mat::scalar_type val)
      -> std::enable_if_t<IsMaxPlusMat<Mat>> {
    if (val == POSITIVE_INFINITY) {
      using scalar_type = typename Mat::scalar_type;
      LIBSEMIGROUPS_EXCEPTION("invalid entry, expected entries to be "
                              "integers or {} (= {}) but found {} (= {})",
                              entry_repr(NEGATIVE_INFINITY),
                              static_cast<scalar_type>(NEGATIVE_INFINITY),
                              entry_repr(POSITIVE_INFINITY),
                              static_cast<scalar_type>(POSITIVE_INFINITY));
    }
  }

  ////////////////////////////////////////////////////////////////////////

  template <typename Mat>
  auto throw_if_bad_entry(Mat const& x) -> std::enable_if_t<IsMinPlusMat<Mat>> {
    using scalar_type = typename Mat::scalar_type;
    auto it           = std::find_if(x.cbegin(), x.cend(), [](scalar_type val) {
      return val == NEGATIVE_INFINITY;
    });
    if (it != x.cend()) {
      auto [r, c] = x.coords(it);
      LIBSEMIGROUPS_EXCEPTION(
          "invalid entry, expected entries to be integers or {} (= {}), "
          "but found {} (= {}) in entry ({}, {})",
          entry_repr(POSITIVE_INFINITY),
          static_cast<scalar_type>(POSITIVE_INFINITY),
          entry_repr(NEGATIVE_INFINITY),
          static_cast<scalar_type>(NEGATIVE_INFINITY),
          r,
          c);
    }
  }

  // TODO(v4) deprecated delete
  template <typename Mat>
  auto throw_if_bad_entry(Mat const&, typename Mat::scalar_type val)
      -> std::enable_if_t<IsMinPlusMat<Mat>> {
    if (val == NEGATIVE_INFINITY) {
      using scalar_type = typename Mat::scalar_type;
      LIBSEMIGROUPS_EXCEPTION("invalid entry, expected entries to be "
                              "integers or {} (= {}) but found {} (= {})",
                              entry_repr(POSITIVE_INFINITY),
                              static_cast<scalar_type>(POSITIVE_INFINITY),
                              entry_repr(NEGATIVE_INFINITY),
                              static_cast<scalar_type>(NEGATIVE_INFINITY));
    }
  }

  ////////////////////////////////////////////////////////////////////////

  template <typename Mat>
  auto throw_if_bad_entry(Mat const& m)
      -> std::enable_if_t<IsMaxPlusTruncMat<Mat>> {
    detail::throw_if_semiring_nullptr(m);

    using scalar_type   = typename Mat::scalar_type;
    scalar_type const t = matrix::threshold(m);
    auto it = std::find_if_not(m.cbegin(), m.cend(), [t](scalar_type x) {
      return x == NEGATIVE_INFINITY || (0 <= x && x <= t);
    });
    if (it != m.cend()) {
      auto [r, c] = m.coords(it);
      LIBSEMIGROUPS_EXCEPTION(
          "invalid entry, expected values in {{0, 1, ..., {}, {} (= {})}} "
          "but found {} in entry ({}, {})",
          t,
          entry_repr(NEGATIVE_INFINITY),
          static_cast<scalar_type>(NEGATIVE_INFINITY),
          detail::entry_repr(*it),
          r,
          c);
    }
  }

  // TODO(v4) deprecated delete
  template <typename Mat>
  auto throw_if_bad_entry(Mat const& m, typename Mat::scalar_type val)
      -> std::enable_if_t<IsMaxPlusTruncMat<Mat>> {
    detail::throw_if_semiring_nullptr(m);
    using scalar_type   = typename Mat::scalar_type;
    scalar_type const t = matrix::threshold(m);
    if (val == POSITIVE_INFINITY || 0 > val || val > t) {
      LIBSEMIGROUPS_EXCEPTION(
          "invalid entry, expected values in {{0, 1, ..., {}, -{} (= {})}} "
          "but found {}",
          t,
          entry_repr(NEGATIVE_INFINITY),
          static_cast<scalar_type>(NEGATIVE_INFINITY),
          detail::entry_repr(val));
    }
  }

  ////////////////////////////////////////////////////////////////////////

  template <typename Mat>
  auto throw_if_bad_entry(Mat const& m)
      -> std::enable_if_t<IsMinPlusTruncMat<Mat>> {
    // Check that the semiring pointer isn't the nullptr if it shouldn't be
    detail::throw_if_semiring_nullptr(m);

    using scalar_type   = typename Mat::scalar_type;
    scalar_type const t = matrix::threshold(m);
    auto it = std::find_if_not(m.cbegin(), m.cend(), [t](scalar_type x) {
      return x == POSITIVE_INFINITY || (0 <= x && x <= t);
    });
    if (it != m.cend()) {
      auto [r, c] = m.coords(it);

      LIBSEMIGROUPS_EXCEPTION(
          "invalid entry, expected values in {{0, 1, ..., {}, {} (= {})}} "
          "but found {} in entry ({}, {})",
          t,
          detail::entry_repr(POSITIVE_INFINITY),
          static_cast<scalar_type>(POSITIVE_INFINITY),
          detail::entry_repr(*it),
          r,
          c);
    }
  }

  // TODO(v4) deprecated delete
  template <typename Mat>
  auto throw_if_bad_entry(Mat const& m, typename Mat::scalar_type val)
      -> std::enable_if_t<IsMinPlusTruncMat<Mat>> {
    detail::throw_if_semiring_nullptr(m);

    using scalar_type   = typename Mat::scalar_type;
    scalar_type const t = matrix::threshold(m);
    if (!(val == POSITIVE_INFINITY || (0 <= val && val <= t))) {
      LIBSEMIGROUPS_EXCEPTION(
          "invalid entry, expected values in {{0, 1, ..., {}, {} (= {})}} "
          "but found {}",
          t,
          detail::entry_repr(POSITIVE_INFINITY),
          static_cast<scalar_type>(POSITIVE_INFINITY),
          detail::entry_repr(val));
    }
  }

  ////////////////////////////////////////////////////////////////////////

  template <typename Mat>
  auto throw_if_bad_entry(Mat const& m) -> std::enable_if_t<IsNTPMat<Mat>> {
    detail::throw_if_semiring_nullptr(m);

    using scalar_type   = typename Mat::scalar_type;
    scalar_type const t = matrix::threshold(m);
    scalar_type const p = matrix::period(m);
    auto it = std::find_if_not(m.cbegin(), m.cend(), [t, p](scalar_type x) {
      return (0 <= x && x < p + t);
    });
    if (it != m.cend()) {
      auto [r, c] = m.coords(it);

      LIBSEMIGROUPS_EXCEPTION(
          "invalid entry, expected values in {{0, 1, ..., {}}}, but "
          "found {} in entry ({}, {})",
          p + t,
          detail::entry_repr(*it),
          r,
          c);
    }
  }

  // TODO(v4) deprecated delete
  template <typename Mat>
  auto throw_if_bad_entry(Mat const& m, typename Mat::scalar_type val)
      -> std::enable_if_t<IsNTPMat<Mat>> {
    detail::throw_if_semiring_nullptr(m);
    using scalar_type   = typename Mat::scalar_type;
    scalar_type const t = matrix::threshold(m);
    scalar_type const p = matrix::period(m);
    if (val < 0 || val >= p + t) {
      LIBSEMIGROUPS_EXCEPTION(
          "invalid entry, expected values in {{0, 1, ..., {}}}, but "
          "found {}",
          p + t,
          detail::entry_repr(val));
    }
  }
}  // namespace libsemigroups::matrix
