//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023 James D. Mitchell
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

// This file contains the implementation of the Kambites class implementing the
// algorithm described in:
//
// Kambites, M. (2009). Small overlap monoids. I. The word problem. J. Algebra,
// 321(8), 2187–2205.
//
// for solving the word problem in small overlap monoids, and a novel algorithm
// for computing normal forms in small overlap monoids, by Maria Tsalakou.

namespace libsemigroups {
  // Data structure for caching the regularly accessed parts of the
  // relation words.
  template <typename Word>
  struct Kambites<Word>::RelationWords {
    using internal_type          = typename Kambites<Word>::internal_type;
    bool          is_initialized = false;
    internal_type X;
    internal_type Y;
    internal_type Z;
    internal_type XY;
    internal_type YZ;
    internal_type XYZ;
  };

  template <typename Word>
  class Kambites<Word>::Complements {
   public:
    Complements() = default;

    Complements& init() {
      _complements.clear();
      _lookup.clear();
      return *this;
    }

    void init(std::vector<value_type> const&);

    std::vector<size_t> const& of(size_t i) const {
      LIBSEMIGROUPS_ASSERT(i < _lookup.size());
      LIBSEMIGROUPS_ASSERT(_lookup[i] < _complements.size());
      return _complements[_lookup[i]];
    }

   private:
    std::vector<std::vector<size_t>> _complements;
    std::vector<size_t>              _lookup;
  };

  ////////////////////////////////////////////////////////////////////////
  // Kambites - constructor and destructor impl - public
  ////////////////////////////////////////////////////////////////////////
  ///
  template <typename Word>
  Kambites<Word>::Kambites() : CongruenceInterface(congruence_kind::twosided) {
    init();
  }

  template <typename Word>
  Kambites<Word>& Kambites<Word>::init() {
    CongruenceInterface::init();
    // Mutable
    _class = UNDEFINED;
    _complements.init();
    _have_class = false;
    _XYZ_data.clear();
    // Non-mutable
    _suffix_tree.init();
    return *this;
  }

  template <typename Word>
  Kambites<Word>::Kambites(Kambites const&) = default;

  template <typename Word>
  Kambites<Word>::~Kambites() = default;

  ////////////////////////////////////////////////////////////////////////
  // CongruenceInterface - pure virtual functions impl - public
  ////////////////////////////////////////////////////////////////////////

  template <typename Word>
  bool Kambites<Word>::contains(word_type const& u, word_type const& v) {
    validate_small_overlap_class();
    // Words aren't validated, the below returns false if they contain
    // letters not in the alphabet.
    if constexpr (std::is_same_v<internal_type, word_type>) {
      return wp_prefix(internal_type(u), internal_type(v), internal_type());
    } else {
      std::string uu = to_string(presentation(), u);
      std::string vv = to_string(presentation(), v);
      return wp_prefix(internal_type(uu), internal_type(vv), internal_type());
    }
  }

  template <typename Word>
  template <typename SFINAE>
  auto Kambites<Word>::contains(value_type const& u, value_type const& v)
      -> std::enable_if_t<!std::is_same_v<value_type, word_type>, SFINAE> {
    validate_small_overlap_class();
    // Words aren't validated, the below returns false if they contain
    // letters not in the alphabet.
    return wp_prefix(internal_type(u), internal_type(v), internal_type());
  }

  template <typename Word>
  typename Kambites<Word>::value_type
  Kambites<Word>::normal_form(value_type const& w0) {
    validate_small_overlap_class();
    using words:: operator+;
    using words:: operator+=;
    size_t        r = UNDEFINED;
    internal_type v, w(w0);
    while (!w.empty()) {
      if (r == UNDEFINED) {
        normal_form_inner(r, v, w);
        continue;
      }

      size_t rb
          = Z_active_proper_complement(r, w.cbegin() + Z(r).size(), w.cend());
      if (rb == UNDEFINED || Z(r) == Z(rb)) {
        normal_form_inner(r, v, w);
        continue;
      }
      LIBSEMIGROUPS_ASSERT(detail::is_prefix(w, Z(r)));
      size_t                 s;
      internal_type_iterator it_zrb, it_wp;
      std::tie(s, it_zrb, it_wp)
          = p_active(Z(rb), w.cbegin() + Z(r).size(), w.cend());
      LIBSEMIGROUPS_ASSERT(s < _presentation.rules.size());
      // we could just use w.erase(w.begin(), it_wp); if not for the
      // if-statement in the next line requiring the old w.
      internal_type wp(it_wp, w.cend());
      if (!wp_prefix(wp, wp, Z(s))) {
        normal_form_inner(r, v, w);
        continue;
      }
      size_t sb = prefix_of_complement(s, it_zrb, Z(rb).cend());
      if (sb != UNDEFINED) {
        // line 10
        replace_prefix(wp, Z(s));
        wp.erase(wp.begin(), wp.begin() + Z(s).size());
        internal_type b(X(sb).cbegin() + (Z(rb).cend() - it_zrb),
                        X(sb).cend());  // b
        if (wp_prefix(internal_type(w0),
                      v + Z(r) + b + YZ(sb) + wp,
                      internal_type())) {
          // line 12
          v += Z(r);
          v += b;
          v += Y(sb);
          w = Z(sb);
          w += wp;
          r = sb;
          continue;
        }
      }
      // line 16
      v += Z(r);
      append(v, w.cbegin() + Z(r).size(), it_wp);
      std::swap(w, wp);
      r = s;
    }
    return v;
  }

  template <typename Word>
  template <typename SFINAE>
  auto Kambites<Word>::normal_form(word_type const& w)
      -> std::enable_if_t<!std::is_same_v<value_type, word_type>, SFINAE> {
    // Words aren't validated, the below returns false if they contain
    // letters not in the alphabet.
    std::string uu = to_string(presentation(), w);
    return to_word(presentation(), normal_form(uu));
  }

  ////////////////////////////////////////////////////////////////////////
  // Kambites - member functions impl - public
  ////////////////////////////////////////////////////////////////////////

  template <typename Word>
  size_t Kambites<Word>::small_overlap_class() {
    run();
    return _class;
  }

  ////////////////////////////////////////////////////////////////////////
  // Kambites - init functions - private
  ////////////////////////////////////////////////////////////////////////

  template <typename Word>
  Kambites<Word>&
  Kambites<Word>::private_init_from_presentation(bool call_init) {
    if (call_init) {
      init();
    }
    ukkonen::add_words_no_checks(
        _suffix_tree, _presentation.rules.cbegin(), _presentation.rules.cend());
    return *this;
  }

  ////////////////////////////////////////////////////////////////////////
  // Kambites - validation functions impl - private
  ////////////////////////////////////////////////////////////////////////

  template <typename Word>
  void Kambites<Word>::validate_small_overlap_class() {
    if (small_overlap_class() < 4) {
      LIBSEMIGROUPS_EXCEPTION(
          "small overlap class must be at least 4, but found {}",
          small_overlap_class());
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // Kambites - XYZ functions impl - private
  ////////////////////////////////////////////////////////////////////////

  template <typename Word>
  void Kambites<Word>::really_init_XYZ_data(size_t i) const {
    auto const X_end = ukkonen::maximal_piece_prefix_no_checks(
        _suffix_tree,
        _presentation.rules[i].cbegin(),
        _presentation.rules[i].cend());
    auto const Z_begin = ukkonen::maximal_piece_suffix_no_checks(
        _suffix_tree,
        _presentation.rules[i].cbegin(),
        _presentation.rules[i].cend());

    _XYZ_data[i].is_initialized = true;
    _XYZ_data[i].X   = internal_type(_presentation.rules[i].cbegin(), X_end);
    _XYZ_data[i].Y   = internal_type(X_end, Z_begin);
    _XYZ_data[i].Z   = internal_type(Z_begin, _presentation.rules[i].cend());
    _XYZ_data[i].XY  = internal_type(_presentation.rules[i].cbegin(), Z_begin);
    _XYZ_data[i].YZ  = internal_type(X_end, _presentation.rules[i].cend());
    _XYZ_data[i].XYZ = internal_type(_presentation.rules[i]);
  }

  ////////////////////////////////////////////////////////////////////////
  // Kambites - helpers impl - private
  ////////////////////////////////////////////////////////////////////////

  // O(number of relation words * (last - first))
  // See explanation above.
  //
  // TODO(later): we tried multiple things here to try and improve this, but
  // none of them were better than the current function. Two things that we
  // never tried or didn't get to work were:
  // 1) Binary search
  // 2) Using the original suffix tree (without the unique characters)
  template <typename Word>
  size_t
  Kambites<Word>::relation_prefix(internal_type_iterator const& first,
                                  internal_type_iterator const& last) const {
    for (size_t i = 0; i < _presentation.rules.size(); ++i) {
      if (detail::is_prefix(first,
                            last,
                            _presentation.rules[i].cbegin(),
                            _presentation.rules[i].cend() - Z(i).size())) {
        return i;
      }
    }
    return UNDEFINED;
  }

  // See explanation above.
  // Complexity: O(max|Y| * (last - first))
  template <typename Word>
  size_t Kambites<Word>::clean_overlap_prefix(
      internal_type_iterator const& first,
      internal_type_iterator const& last) const {
    size_t i = relation_prefix(first, last);
    if (i == UNDEFINED) {
      return UNDEFINED;
    }
    for (auto it = first + X(i).size() + 1; it < first + XY(i).size(); ++it) {
      if (relation_prefix(it, last) != UNDEFINED) {
        return UNDEFINED;
      }
    }
    return i;
  }

  // See explanation above.
  template <typename Word>
  std::pair<size_t, size_t>
  Kambites<Word>::clean_overlap_prefix_mod(internal_type const& w,
                                           size_t               n) const {
    size_t     i = 0, j = 0;
    auto       first = w.cbegin();
    auto const last  = w.cend();
    for (; i < n; ++i) {
      j = clean_overlap_prefix(first, last);
      if (j != UNDEFINED) {
        break;
      }
      first++;
    }
    return std::make_pair(i, j);
  }

  template <typename Word>
  std::tuple<size_t,
             typename Kambites<Word>::internal_type_iterator,
             typename Kambites<Word>::internal_type_iterator>
  Kambites<Word>::p_active(internal_type const&          x,
                           internal_type_iterator const& first,
                           internal_type_iterator const& last) const {
    // The following should hold but can't be checked when internal_type is
    // MultiStringView.
    // LIBSEMIGROUPS_ASSERT(x.cend() < first || x.cbegin() >= last);
    internal_type y = x;
    append(y, first, last);
    for (auto it = y.cbegin(); it < y.cbegin() + x.size(); ++it) {
      size_t i = relation_prefix(it, y.cend());
      if (i != UNDEFINED) {
        size_t n = it - y.cbegin();
        y.erase(y.begin() + x.size(), y.end());
        return std::make_tuple(
            i, x.cbegin() + n, first + (XY(i).size() - (x.size() - n)));
      }
    }
    return std::make_tuple(UNDEFINED, x.cend(), last);
  }

  // See explanation above.
  template <typename Word>
  void Kambites<Word>::replace_prefix(internal_type&       w,
                                      internal_type const& p) const {
    LIBSEMIGROUPS_ASSERT(wp_prefix(w, w, p));
    using words::operator+=;
    if (detail::is_prefix(w, p)) {
      return;
    }

    size_t i, j;
    std::tie(i, j) = clean_overlap_prefix_mod(w, p.size());
    LIBSEMIGROUPS_ASSERT(j != UNDEFINED);

    internal_type u(w.cbegin() + i + XY(j).size(), w.cend());
    replace_prefix(u, Z(j));
    LIBSEMIGROUPS_ASSERT(detail::is_prefix(u, Z(j)));
    u.erase(u.begin(), u.begin() + Z(j).size());

    size_t k = prefix_of_complement(j, p.cbegin() + i, p.cend());
    LIBSEMIGROUPS_ASSERT(k != UNDEFINED);

    w.erase(w.begin() + i, w.end());
    w += XYZ(k);
    w += u;
    LIBSEMIGROUPS_ASSERT(detail::is_prefix(w, p));
  }

  ////////////////////////////////////////////////////////////////////////
  // Kambites - complement helpers impl - private
  ////////////////////////////////////////////////////////////////////////

  template <typename Word>
  size_t Kambites<Word>::prefix_of_complement(
      size_t                        i,
      internal_type_iterator const& first,
      internal_type_iterator const& last) const {
    // TODO(later) use binary_search instead
    for (auto const& j : _complements.of(i)) {
      if (detail::is_prefix(XYZ(j).cbegin(), XYZ(j).cend(), first, last)) {
        return j;
      }
    }
    return UNDEFINED;
  }

  template <typename Word>
  size_t Kambites<Word>::complementary_XY_prefix(size_t               i,
                                                 internal_type const& w) const {
    // TODO(later) use binary_search instead
    for (auto const& j : _complements.of(i)) {
      if (detail::is_prefix(w, XY(j))) {
        return j;
      }
    }
    return UNDEFINED;
  }

  template <typename Word>
  size_t Kambites<Word>::Z_active_complement(size_t               i,
                                             internal_type const& w) const {
    auto const first = w.cbegin();
    auto const last  = w.cend();

    for (auto const& j : _complements.of(i)) {
      if (std::get<0>(p_active(Z(j), first, last)) != UNDEFINED) {
        return j;
      }
    }
    return UNDEFINED;
  }

  template <typename Word>
  size_t Kambites<Word>::Z_active_proper_complement(
      size_t                        i,
      internal_type_iterator const& first,
      internal_type_iterator const& last) const {
    for (auto const& j : _complements.of(i)) {
      if (i != j && std::get<0>(p_active(Z(j), first, last)) != UNDEFINED) {
        return j;
      }
    }
    return UNDEFINED;
  }

  ////////////////////////////////////////////////////////////////////////
  // Kambites - main functions impl - private
  ////////////////////////////////////////////////////////////////////////

  template <typename Word>
  bool Kambites<Word>::wp_prefix(internal_type u,
                                 internal_type v,
                                 internal_type p) const {
    using detail::is_prefix;
    using words::operator+;

    _complements.init(_presentation.rules);

    while (!u.empty() && !v.empty()) {
      size_t i = clean_overlap_prefix(u);
      if (i == UNDEFINED) {
        if (u[0] != v[0] || (!p.empty() && u[0] != p[0])) {
          return false;
        }
        pop_front(u);
        pop_front(v);
        if (!p.empty()) {
          pop_front(p);
        }
      } else {
        if (prefix_of_complement(i, p) == UNDEFINED) {  // line 18
          return false;
        }
        size_t j = complementary_XY_prefix(i, v);
        if (j == UNDEFINED) {  // line 20
          return false;
        }
        // At this point u = X_iY_iu' and v = X_jY_jv'
        bool up_start_Z = is_prefix(
            u.cbegin() + XY(i).size(), u.cend(), Z(i).cbegin(), Z(i).cend());
        bool vp_start_Z = is_prefix(
            v.cbegin() + XY(j).size(), v.cend(), Z(j).cbegin(), Z(j).cend());

        if (up_start_Z && vp_start_Z) {  // line 22
          u.erase(u.begin(), u.begin() + XYZ(i).size());
          auto k = Z_active_complement(i, u);
          k      = (k == UNDEFINED ? i : k);
          u      = Z(k) + u;
          v.erase(v.begin(), v.begin() + XYZ(j).size());
          v = Z(k) + v;
          p.clear();
          // line 23
        } else if (i == j) {  // line 26
          if (is_prefix(X(i), p)) {
            p.clear();
          } else {
            p = Z(i);
          }
          u.erase(u.begin(), u.begin() + XY(i).size());
          v.erase(v.begin(), v.begin() + XY(i).size());
        } else if (vp_start_Z) {  // line 30
          u.erase(u.begin(), u.begin() + XY(i).size());
          v.erase(v.begin(), v.begin() + XYZ(j).size());
          v = Z(i) + v;
          p.clear();
        } else if (up_start_Z) {  // line 32
          u.erase(u.begin(), u.begin() + XYZ(i).size());
          u = Z(j) + u;
          v.erase(v.begin(), v.begin() + XY(j).size());
          p.clear();
        } else {  // line 34
          p = detail::maximum_common_suffix(Z(i), Z(j));
          if (!is_prefix(u.cbegin() + XY(i).size(),
                         u.cend(),
                         Z(i).cbegin(),
                         Z(i).cend() - p.size())
              || !is_prefix(v.cbegin() + XY(j).size(),
                            v.cend(),
                            Z(j).cbegin(),
                            Z(j).cend() - p.size())) {
            return false;
          } else {
            u.erase(u.begin(), u.begin() + XYZ(i).size() - p.size());
            v.erase(v.begin(), v.begin() + XYZ(j).size() - p.size());
          }
        }
      }
    }

    return u.empty() && v.empty() && p.empty();
  }

  template <typename Word>
  void Kambites<Word>::normal_form_inner(size_t&        r,
                                         internal_type& v,
                                         internal_type& w) const {
    using words::operator+=;
    size_t       i, j;
    std::tie(i, j) = clean_overlap_prefix_mod(w, w.size());
    if (j == UNDEFINED) {
      // line 39
      v += w;
      w.clear();
      return;
    }

    internal_type wp(w.cbegin() + i + XY(j).size(), w.cend());
    append(v, w.cbegin(), w.cbegin() + i);  // a

    if (!wp_prefix(wp, wp, Z(j))) {
      // line 23
      r = UNDEFINED;
      v += XY(j);
      w = std::move(wp);
    } else {
      // line 27
      r = _complements.of(j).front();
      replace_prefix(wp, Z(j));
      v += XY(r);
      w = Z(r);
      append(w, wp.cbegin() + Z(j).size(), wp.cend());
    }
  }
  template <typename Word>
  void Kambites<Word>::run_impl() {
    if (!_have_class) {
      if constexpr (std::is_same_v<value_type, word_type>) {
        auto const& pairs = generating_pairs();
        _presentation.rules.insert(
            _presentation.rules.end(), pairs.cbegin(), pairs.cend());
        ukkonen::add_words_no_checks(
            _suffix_tree, pairs.cbegin(), pairs.cend());
      } else {
        auto pairs = (rx::iterator_range(generating_pairs().cbegin(),
                                         generating_pairs().cend())
                      | ToStrings(_presentation.alphabet()) | rx::to_vector());
        ukkonen::add_words_no_checks(
            _suffix_tree, pairs.cbegin(), pairs.cend());
        _presentation.rules.insert(_presentation.rules.end(),
                                   std::make_move_iterator(pairs.begin()),
                                   std::make_move_iterator(pairs.end()));
      }

      size_t result = POSITIVE_INFINITY;
      for (auto const& w : _presentation.rules) {
        result = std::min(result,
                          ukkonen::number_of_pieces_no_checks(
                              _suffix_tree, w.cbegin(), w.cend()));
      }
      _have_class = true;
      _class      = result;
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // Kambites - inner classes - private
  ////////////////////////////////////////////////////////////////////////

  template <typename Word>
  void Kambites<Word>::Complements::init(
      std::vector<value_type> const& relation_words) {
    if (relation_words.empty()) {
      return;
    }
    // Initialise the _complements data structure
    if (_complements.empty()) {
      detail::Duf<>                      duf;
      std::unordered_map<size_t, size_t> map;
      duf.resize(relation_words.size());
      for (size_t i = 0; i < relation_words.size() - 1; ++i) {
        if (i % 2 == 0) {
          duf.unite(i, i + 1);
        }
        for (size_t j = i + 1; j < relation_words.size(); ++j) {
          if (relation_words[i] == relation_words[j]) {
            duf.unite(i, j);
          }
        }
      }
      size_t next = 0;
      _complements.resize(duf.number_of_blocks());
      _lookup.resize(relation_words.size());
      for (size_t i = 0; i < relation_words.size(); ++i) {
        std::unordered_map<size_t, size_t>::iterator it;
        bool                                         inserted;
        std::tie(it, inserted) = map.emplace(duf.find(i), next);
        _lookup[i]             = it->second;
        _complements[it->second].push_back(i);
        next += inserted;
      }
      for (auto& v : _complements) {
        std::sort(v.begin(), v.end(), [&relation_words](size_t i, size_t j) {
          return lexicographical_compare(relation_words[i], relation_words[j]);
        });
      }
    }
  }

  template <typename Word>
  typename Kambites<Word>::internal_type const&
  Kambites<Word>::X(size_t i) const {
    LIBSEMIGROUPS_ASSERT(i < _presentation.rules.size());
    LIBSEMIGROUPS_ASSERT(finished_impl());
    init_XYZ_data(i);
    return _XYZ_data[i].X;
  }

  template <typename Word>
  typename Kambites<Word>::internal_type const&
  Kambites<Word>::Y(size_t i) const {
    LIBSEMIGROUPS_ASSERT(i < _presentation.rules.size());
    LIBSEMIGROUPS_ASSERT(finished_impl());
    init_XYZ_data(i);
    return _XYZ_data[i].Y;
  }

  template <typename Word>
  typename Kambites<Word>::internal_type const&
  Kambites<Word>::Z(size_t i) const {
    LIBSEMIGROUPS_ASSERT(i < _presentation.rules.size());
    LIBSEMIGROUPS_ASSERT(finished_impl());
    init_XYZ_data(i);
    return _XYZ_data[i].Z;
  }

  template <typename Word>
  typename Kambites<Word>::internal_type const&
  Kambites<Word>::XY(size_t i) const {
    LIBSEMIGROUPS_ASSERT(i < _presentation.rules.size());
    LIBSEMIGROUPS_ASSERT(finished_impl());
    init_XYZ_data(i);
    return _XYZ_data[i].XY;
  }

  template <typename Word>
  typename Kambites<Word>::internal_type const&
  Kambites<Word>::YZ(size_t i) const {
    LIBSEMIGROUPS_ASSERT(i < _presentation.rules.size());
    LIBSEMIGROUPS_ASSERT(finished_impl());
    init_XYZ_data(i);
    return _XYZ_data[i].YZ;
  }

  template <typename Word>
  typename Kambites<Word>::internal_type const&
  Kambites<Word>::XYZ(size_t i) const {
    LIBSEMIGROUPS_ASSERT(i < _presentation.rules.size());
    LIBSEMIGROUPS_ASSERT(finished_impl());
    init_XYZ_data(i);
    return _XYZ_data[i].XYZ;
  }
}  // namespace libsemigroups
