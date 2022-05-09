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

#ifndef LIBSEMIGROUPS_TESTS_FPSEMI_EXAMPLES_HPP_
#define LIBSEMIGROUPS_TESTS_FPSEMI_EXAMPLES_HPP_

#include <cctype>         // for toupper
#include <cstddef>        // for size_t
#include <string>         // for basic_string, operator+
#include <unordered_set>  // for unordered_set
#include <vector>         // for vector

#include "libsemigroups/present.hpp"  // for Presentation
#include "libsemigroups/types.hpp"    // for relation_type

namespace libsemigroups {
  std::vector<relation_type> RookMonoid(size_t l, int q);
  std::vector<relation_type> RennerCommonTypeBMonoid(size_t l, int q);
  std::vector<relation_type> RennerTypeBMonoid(size_t l, int q);
  std::vector<relation_type> EGTypeBMonoid(size_t l, int q);
  std::vector<relation_type> RennerCommonTypeDMonoid(size_t l, int q);
  std::vector<relation_type> EGTypeDMonoid(size_t l, int q);
  std::vector<relation_type> RennerTypeDMonoid(size_t l, int q);
  std::vector<relation_type> Stell(size_t l);
  std::vector<relation_type> Fibonacci(size_t r, size_t n);
  std::vector<relation_type> Plactic(size_t n);
  std::vector<relation_type> Stylic(size_t n);
  std::vector<relation_type> SymmetricGroup1(size_t n);
  std::vector<relation_type> SymmetricGroup2(size_t n);
  std::vector<relation_type> DualSymmetricInverseMonoidEEF(size_t n);
  std::vector<relation_type> UniformBlockBijectionMonoidF(size_t n);
  std::vector<relation_type> PartitionMonoidEast41(size_t n);
  std::vector<relation_type> SingularBrauer(size_t n);
  std::vector<relation_type> OrientationPreserving(size_t n);
  std::vector<relation_type> OrientationReversing(size_t n);
  std::vector<relation_type> TemperleyLieb(size_t n);
  std::vector<relation_type> Brauer(size_t n);

  template <typename T, typename F, typename... Args>
  void setup(T& tc, size_t num_gens, F func, Args... args) {
    tc.set_number_of_generators(num_gens);
    for (auto const& w : func(args...)) {
      tc.add_pair(w.first, w.second);
    }
  }

  template <typename T>
  T make_group(T& M) {
    std::string              id = "e";
    std::unordered_set<char> self_inverse;
    self_inverse.emplace('e');

    for (auto it = M.cbegin_rules(); it != M.cend_rules(); ++it) {
      if (it->second == id && it->first.size() == 2
          && it->first[0] == it->first[1]) {
        // Remove self-inverse elements
        self_inverse.emplace(it->first[0]);
      } else if (it->first == id && it->second.size() == 2
                 && it->second[0] == it->second[1]) {
        // Remove self-inverse elements
        self_inverse.emplace(it->second[0]);
      }
    }

    std::string alphabet;
    std::string inverse;

    for (auto const& letter : self_inverse) {
      alphabet += letter;
      inverse += letter;
    }

    for (auto const& letter : M.alphabet()) {
      if (self_inverse.find(letter) == self_inverse.cend()) {
        alphabet += letter;
        alphabet += std::toupper(letter);
        inverse += std::toupper(letter);
        inverse += letter;
      }
    }

    T G;
    G.set_alphabet(alphabet);
    G.set_identity(id);
    G.set_inverses(inverse);
    for (auto it = M.cbegin_rules(); it < M.cend_rules(); ++it) {
      if ((it->second != id || it->first.size() != 2
           || it->first[0] != it->first[1])
          && (it->first != id || it->second.size() != 2
              || it->second[0] != it->second[1])) {
        G.add_rule(*it);
      }
    }
    return G;
  }

  template <typename T>
  T AlternatingGroupMoore(size_t n) {
    static const std::string alphabet = "abcdfghijklmnopqrstuvwxyz";
    T                        x;
    std::string              id = "e";
    x.set_alphabet(id
                   + std::string(alphabet.cbegin(), alphabet.cbegin() + n - 2));

    std::string const& a = x.alphabet();
    x.add_rule(std::string({a[1]}) + a[1] + a[1], id);

    for (size_t i = 2; i <= n - 2; ++i) {
      x.add_rule(std::string({a[i - 1]}) + a[i] + a[i - 1] + a[i] + a[i - 1]
                     + a[i],
                 id);
    }

    for (size_t j = 1; j <= n - 4; ++j) {
      x.add_rule(std::string({a[j]}) + a[j], id);
    }

    for (size_t k = 3; k <= n - 3; ++k) {
      for (size_t j = 1; j < k - 1; ++j) {
        x.add_rule(std::string({a[j]}) + a[k] + a[j] + a[k], id);
      }
    }
    return make_group(x);
  }

  template <typename T,
            typename
            = std::enable_if_t<std::is_base_of<PresentationBase, T>::value>>
  T make(std::vector<relation_type> const& rels) {
    Presentation<word_type> p;
    for (auto const& rel : rels) {
      p.add_rule(rel.first.cbegin(),
                 rel.first.cend(),
                 rel.second.cbegin(),
                 rel.second.cend());
    }
    p.alphabet_from_rules();
    p.validate();
    return p;
  }

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_TESTS_FPSEMI_EXAMPLES_HPP_
