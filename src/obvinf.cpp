//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020-2025 James D. Mitchell + Reinis Cirpons
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

#include "libsemigroups/obvinf.hpp"

#include <algorithm>  // for all_of
#include <cstddef>    // for size_t
#include <string>     // for string
#include <utility>    // for pair
#include <vector>     // for vector

#include "libsemigroups/cong.hpp"          // for ToWord
#include "libsemigroups/constants.hpp"     // for UNDEFINED
#include "libsemigroups/debug.hpp"         // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/knuth-bendix.hpp"  // for KnuthBendix
#include "libsemigroups/todd-coxeter.hpp"  // for ToddCoxeterImpl
#include "libsemigroups/word-range.hpp"    // for ToWord

#include "libsemigroups/detail/eigen.hpp"

namespace libsemigroups {

  using const_iterator_word_type =
      typename std::vector<word_type>::const_iterator;
  using const_iterator_pair_string =
      typename std::vector<std::pair<std::string, std::string>>::const_iterator;

  IsObviouslyInfinite::IsObviouslyInfinite(size_t n) : IsObviouslyInfinite() {
    init(n);
  }

  IsObviouslyInfinite& IsObviouslyInfinite::init(size_t n) {
    _empty_word = false;
    _letter_components.init(n);
    _nr_gens         = n;
    _nr_relations    = 0;
    _preserve_length = true;
    _preserve.clear();
    _preserve.resize(n, false);
    _seen.clear();
    _seen.resize(n, false);
    _unique.resize(n, false);
#ifdef LIBSEMIGROUPS_EIGEN_ENABLED
    _matrix = decltype(_matrix)(0, n);
#else
    _matrix           = decltype(_matrix)(n, 0);
#endif
    return *this;
  }

  IsObviouslyInfinite::~IsObviouslyInfinite() = default;

  // First parameter only there for consistency of interface at present
  IsObviouslyInfinite&
  IsObviouslyInfinite::add_rules_no_checks(word_type const&,
                                           const_iterator_word_type first,
                                           const_iterator_word_type last) {
#ifdef LIBSEMIGROUPS_EIGEN_ENABLED
    auto matrix_start = _matrix.rows();
    _matrix.conservativeResize(matrix_start + (last - first) / 2,
                               Eigen::NoChange);
    _matrix.block(matrix_start, 0, (last - first) / 2, _matrix.cols())
        .setZero();
#else
    auto matrix_start = 0;
    std::fill(_matrix.begin(), _matrix.end(), 0);
#endif
    for (auto it = first; it < last; it += 2) {
      private_add_rule(matrix_start + (it - first) / 2, *it, *(it + 1));
    }
    _nr_letter_components = _letter_components.number_of_blocks();
    return *this;
  }

  IsObviouslyInfinite&
  IsObviouslyInfinite::add_rules_no_checks(std::string const&         lphbt,
                                           const_iterator_pair_string first,
                                           const_iterator_pair_string last) {
#ifdef LIBSEMIGROUPS_EIGEN_ENABLED
    auto matrix_start = _matrix.rows();
    _matrix.conservativeResize(matrix_start + (last - first), Eigen::NoChange);
    _matrix.block(matrix_start, 0, (last - first), _matrix.cols()).setZero();
#else
    auto matrix_start = 0;
    std::fill(_matrix.begin(), _matrix.end(), 0);
#endif

    v4::ToWord stw(lphbt);
    word_type  lhs;
    word_type  rhs;
    for (auto it = first; it < last; ++it) {
      stw(lhs, it->first);   // lhs changed in-place
      stw(rhs, it->second);  // rhs changed in-place
      private_add_rule(matrix_start + (it - first), lhs, rhs);
    }
    _nr_letter_components = _letter_components.number_of_blocks();
    return *this;
  }

  bool IsObviouslyInfinite::result() const {
#ifdef LIBSEMIGROUPS_EIGEN_ENABLED
    LIBSEMIGROUPS_ASSERT(_matrix.rows() >= 0);
    LIBSEMIGROUPS_ASSERT(_matrix.cast<float>().colPivHouseholderQr().rank()
                         >= 0);
#endif
    return (_preserve_length
            || (!_empty_word
                && !std::all_of(_unique.begin(),
                                _unique.end(),
                                [](bool v) -> bool { return v; }))
            || !std::all_of(_preserve.begin(),
                            _preserve.end(),
                            [](bool v) -> bool { return v; })
            || (!_empty_word && _nr_letter_components > 1)
#ifdef LIBSEMIGROUPS_EIGEN_ENABLED
            || _nr_relations < _nr_gens
            || size_t(_matrix.cast<float>().colPivHouseholderQr().rank())
                   != _nr_gens);
#else
               || _nr_relations < _nr_gens);
#endif
  }

  void IsObviouslyInfinite::private_add_rule(size_t           row_index,
                                             word_type const& u,
                                             word_type const& v) {
    _nr_relations++;
    if (u.empty() || v.empty()) {
      _empty_word = true;
    }
    std::fill(_seen.begin(), _seen.end(), false);
    plus_letters_in_word(row_index, u);
    if (!_empty_word
        && std::all_of(u.cbegin() + 1, u.cend(), [&u](letter_type i) -> bool {
             return i == u[0];
           })) {
      _unique[u[0]] = true;
    }
    minus_letters_in_word(row_index, v);
    if (!_empty_word && !v.empty()
        && std::all_of(v.cbegin() + 1, v.cend(), [&v](letter_type i) -> bool {
             return i == v[0];
           })) {
      _unique[v[0]] = true;
    }
    for (size_t x = 0; x < _nr_gens; x++) {
      if (matrix(row_index, x) != 0) {
        _preserve[x] = true;
      }
    }
    if (_preserve_length && !matrix_row_sums_to_0(row_index)) {
      _preserve_length = false;
    }
    size_t last_seen = UNDEFINED;
    for (size_t x = 0; x < _nr_gens; ++x) {
      if (_seen[x]) {
        if (last_seen != UNDEFINED) {
          _letter_components.unite(last_seen, x);
        }
        last_seen = x;
      }
    }
  }

  bool is_obviously_infinite(detail::ToddCoxeterImpl const& tc) {
    auto const& d = tc.current_word_graph();
    if (tc.finished()
        || (d.number_of_nodes() != 0
            && v4::word_graph::is_complete(
                d, d.cbegin_active_nodes(), d.cend_active_nodes()))) {
      // TODO(1) Check thoroughly that this works as intended.
      // Without the check on the number of nodes, this doesn't work as expected
      // ATM, since the word graph can sometimes be empty (0 nodes), but with 1
      // active node, so this line can throw (because the range pointed at by
      // d.cbegin_active_nodes(), d.cend_active_nodes() is non-empty but d
      // itself has no nodes) This is an initialization issue for
      // ToddCoxeterImpl, it should always be true the number of nodes >= number
      // of active nodes.
      return false;
    }
    auto                p = tc.internal_presentation();
    IsObviouslyInfinite ioi(p.alphabet().size());
    ioi.add_rules_no_checks(p.alphabet(), p.rules.cbegin(), p.rules.cend());
    ioi.add_rules_no_checks(p.alphabet(),
                            tc.internal_generating_pairs().cbegin(),
                            tc.internal_generating_pairs().cend());
    return ioi.result();
  }

  template <>
  bool is_obviously_infinite(Presentation<std::string> const& p) {
    if (p.alphabet().empty()) {
      return false;
    }
    IsObviouslyInfinite ioi(p.alphabet().size());
    // This function required because of the p.alphabet below!
    ioi.add_rules_no_checks(p.alphabet(), p.rules.cbegin(), p.rules.cend());
    return ioi.result();
  }

}  // namespace libsemigroups
