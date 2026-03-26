//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2026 Finn Smith
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

namespace libsemigroups {

  inline size_t
  Rank<TwistedBipartition>::operator()(TwistedBipartition const& x) const {
    if (x.is_zero()) {
      return 0;
    }
    return (x.threshold() - x.floating_components() + 1) * x.degree()
           + x.rank();
  }

  inline void Lambda<TwistedBipartition, std::pair<size_t, Blocks>>::operator()(
      std::pair<size_t, Blocks>& res,
      TwistedBipartition const&  x) const noexcept {
    if (x.is_zero()) {
      res.first  = UNDEFINED;
      res.second = Blocks();
      return;
    }

    res.first  = x.floating_components();
    auto* ptr  = x.bipartition().right_blocks_no_checks();
    res.second = std::move(*ptr);
    delete ptr;
    // TODO modify in place instead
  }

  inline void Rho<TwistedBipartition, std::pair<size_t, Blocks>>::operator()(
      std::pair<size_t, Blocks>& res,
      TwistedBipartition const&  x) const noexcept {
    if (x.is_zero()) {
      res.first  = UNDEFINED;
      res.second = Blocks();
      return;
    }

    res.first  = x.floating_components();
    auto* ptr  = x.bipartition().left_blocks_no_checks();
    res.second = std::move(*ptr);
    delete ptr;
    // TODO modify in place instead
  }

  inline void
  ImageRightAction<TwistedBipartition, std::pair<size_t, Blocks>>::operator()(
      std::pair<size_t, Blocks>&       res,
      std::pair<size_t, Blocks> const& pt,
      TwistedBipartition const&        x) const noexcept {
    // TODO: improve - no need to actually form the projection?

    if (pt.first == UNDEFINED || x.is_zero()) {
      res.first  = UNDEFINED;
      res.second = Blocks();
      return;
    }

    Bipartition proj = bipartition::block_projection(pt.second);
    res.first
        = pt.first + x.floating_components()
          + bipartition::number_floating_components(proj, x.bipartition());
    if (res.first > x.threshold()) {
      res.first  = UNDEFINED;
      res.second = Blocks();
    } else {
      auto* ptr  = (proj * x.bipartition()).right_blocks_no_checks();
      res.second = std::move(*ptr);
      delete ptr;
      // TODO modify in place instead
    }
  }

  inline void
  ImageLeftAction<TwistedBipartition, std::pair<size_t, Blocks>>::operator()(
      std::pair<size_t, Blocks>&       res,
      std::pair<size_t, Blocks> const& pt,
      TwistedBipartition const&        x) const noexcept {
    // TODO: improve as right action

    if (pt.first == UNDEFINED || x.is_zero()) {
      res.first  = UNDEFINED;
      res.second = Blocks();
      return;
    }

    Bipartition proj = bipartition::block_projection(pt.second);
    res.first
        = pt.first + x.floating_components()
          + bipartition::number_floating_components(x.bipartition(), proj);
    if (res.first > x.threshold()) {
      res.first  = UNDEFINED;
      res.second = Blocks();
    } else {
      auto* ptr  = (x.bipartition() * proj).left_blocks_no_checks();
      res.second = std::move(*ptr);
      delete ptr;
      // TODO modify in place instead
    }
  }
}  // namespace libsemigroups
