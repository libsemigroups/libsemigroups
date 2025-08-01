//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2024-2025 James D. Mitchell
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

// This file contains the declaration of the partial transformation class and
// its subclasses.

namespace libsemigroups {

  namespace detail {

    template <typename Scalar, typename Container>
    void validate(PTransfBase<Scalar, Container> const& f) {
      throw_if_image_value_out_of_range(f);
    }

    template <size_t N, typename Scalar>
    void validate(Transf<N, Scalar> const& f) {
      throw_if_image_value_out_of_range(f);
    }

    template <size_t N, typename Scalar>
    void validate(PPerm<N, Scalar> const& f) {
      throw_if_not_pperm(f);
    }

    template <size_t N, typename Scalar>
    void validate(Perm<N, Scalar> const& f) {
      throw_if_not_perm(f);
    }
  }  // namespace detail

  ////////////////////////////////////////////////////////////////////////
  // PTransfBase
  ////////////////////////////////////////////////////////////////////////

  // STATIC
  template <typename Point, typename Container>
  template <typename Subclass, typename OtherContainer>
  Subclass PTransfBase<Point, Container>::make(OtherContainer&& cont) {
    throw_if_bad_args(cont);

#pragma GCC diagnostic push
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif
    // TODO(1) use move iterator in next line
    Subclass result(std::begin(cont), std::end(cont));
#pragma GCC diagnostic pop
    detail::validate(result);
    return result;
  }

  // STATIC
  template <typename Point, typename Container>
  template <typename Subclass, typename T>
  Subclass PTransfBase<Point, Container>::make(std::initializer_list<T> cont) {
#pragma GCC diagnostic push
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif
    return make<Subclass, std::vector<T>>(cont);
#pragma GCC diagnostic pop
  }

  // STATIC
  template <typename Point, typename Container>
  void PTransfBase<Point, Container>::resize(container_type& c,
                                             size_t          N,
                                             point_type      val) {
    if constexpr (detail::is_array_v<container_type>) {
      std::fill(c.begin() + N, c.end(), val);
    } else {
      c.resize(N, val);
    }
  }

  // STATIC
  template <typename Point, typename Container>
  template <typename T>
  void PTransfBase<Point, Container>::throw_if_bad_args(T const& cont) {
    if constexpr (detail::is_array_v<container_type>) {
      if (cont.size() != std::tuple_size_v<container_type>) {
        LIBSEMIGROUPS_EXCEPTION(
            "incorrect container size, expected {}, found {}",
            std::tuple_size_v<container_type>,
            cont.size());
      }
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // StaticPTransf
  ////////////////////////////////////////////////////////////////////////

  template <size_t N, typename Scalar>
  StaticPTransf<N, Scalar>::StaticPTransf(size_t n) : StaticPTransf() {
    if (n != N) {
      LIBSEMIGROUPS_EXCEPTION("StaticPTransf has fixed degree {}, cannot "
                              "construct a StaticPTransf of degree {}!",
                              N,
                              n);
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // Transf
  ////////////////////////////////////////////////////////////////////////

  template <size_t N, typename Scalar>
  void Transf<N, Scalar>::product_inplace(Transf const& x, Transf const& y) {
    LIBSEMIGROUPS_ASSERT(x.degree() == y.degree());
    LIBSEMIGROUPS_ASSERT(x.degree() == degree());
    LIBSEMIGROUPS_ASSERT(&x != this && &y != this);
    size_t const n = degree();
    for (point_type i = 0; i < n; ++i) {
      (*this)[i] = y[x[i]];
    }
  }

  template <size_t N, typename Scalar>
  void throw_if_image_value_out_of_range(Transf<N, Scalar> const& x) {
    size_t const M = x.degree();
    for (auto const& val : x) {
      if (val >= M) {
        LIBSEMIGROUPS_EXCEPTION("image value out of bounds, expected value in "
                                "[{}, {}), found {}",
                                0,
                                M,
                                val);
      }
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // PPerm
  ////////////////////////////////////////////////////////////////////////

  template <size_t N, typename Scalar>
  template <typename OtherScalar>
  PPerm<N, Scalar>::PPerm(std::vector<OtherScalar> const& dom,
                          std::vector<OtherScalar> const& ran,
                          size_t                          M)
      : PPerm(M) {
    LIBSEMIGROUPS_ASSERT(M >= N);
    LIBSEMIGROUPS_ASSERT(dom.size() <= M);
    LIBSEMIGROUPS_ASSERT(ran.size() <= M);
    LIBSEMIGROUPS_ASSERT(ran.size() <= dom.size());
    for (size_t i = 0; i < dom.size(); ++i) {
      (*this)[dom[i]] = ran[i];
    }
  }

  template <size_t N, typename Scalar>
  void PPerm<N, Scalar>::product_inplace(PPerm const& x, PPerm const& y) {
    LIBSEMIGROUPS_ASSERT(x.degree() == y.degree());
    LIBSEMIGROUPS_ASSERT(x.degree() == degree());
    LIBSEMIGROUPS_ASSERT(&x != this && &y != this);
    size_t const n = degree();
    // Use size_t for the loop variable <i> below because if, for example,
    // n = 256 and point_type = uint8_t, then the i < n is always true in the
    // next line.
    for (size_t i = 0; i < n; ++i) {
      (*this)[i] = (x[i] == UNDEFINED ? UNDEFINED : y[x[i]]);
    }
  }

  template <size_t N, typename Scalar>
  void detail::throw_if_bad_args(std::vector<Scalar> const& dom,
                                 std::vector<Scalar> const& ran,
                                 size_t                     deg) {
    if (N != 0 && deg != N) {
      // Sanity check that the final argument is compatible with the
      // template param N, if we have a static pperm
      LIBSEMIGROUPS_EXCEPTION(
          "the 3rd argument is not valid, expected {}, found {}", N, deg);
    } else if (dom.size() != ran.size()) {
      // The next 2 checks just verify that we can safely run the
      // constructor that uses *this[dom[i]] = im[i] for i = 0, ...,
      // dom.size() - 1.
      LIBSEMIGROUPS_EXCEPTION("domain and range size mismatch, domain has "
                              "size {} but range has size {}",
                              dom.size(),
                              ran.size());
    } else if (!(dom.empty()
                 || deg > *std::max_element(dom.cbegin(), dom.cend()))) {
      LIBSEMIGROUPS_EXCEPTION(
          "domain value out of bounds, found {}, must be less than {}",
          *std::max_element(dom.cbegin(), dom.cend()),
          deg);
    }
    std::unordered_map<Scalar, size_t> seen;
    detail::throw_if_duplicates(dom.cbegin(), dom.cend(), seen);
    detail::throw_if_duplicates(ran.cbegin(), ran.cend(), seen);

    auto it = std::find(dom.cbegin(), dom.cend(), UNDEFINED);
    if (it != dom.cend()) {
      LIBSEMIGROUPS_EXCEPTION(
          "the 1st argument (domain) must not contain UNDEFINED, but found "
          "UNDEFINED (= {}) in position {}",
          static_cast<Scalar>(UNDEFINED),
          std::distance(dom.cbegin(), it));
    }
    it = std::find(ran.cbegin(), ran.cend(), UNDEFINED);
    if (it != ran.cend()) {
      LIBSEMIGROUPS_EXCEPTION(
          "the 2nd argument (range) must not contain UNDEFINED, but found "
          "UNDEFINED (= {}) in position {}",
          static_cast<Scalar>(UNDEFINED),
          std::distance(ran.cbegin(), it));
    }
    // NOTE: it is ok if deg >= max. value of Scalar, but then necessary that
    // the values in <dom> and <ran> are less than (max. value of Scalar) - 1.
  }

  template <typename Return>
  std::enable_if_t<IsPPerm<Return>, Return>
  make(std::vector<typename Return::point_type> const& dom,
       std::vector<typename Return::point_type> const& ran,
       size_t const                                    M) {
    detail::throw_if_bad_args(dom, ran, M);
    Return result(dom, ran, M);
    throw_if_not_pperm(result);
    return result;
  }

  ////////////////////////////////////////////////////////////////////////
  // transf namespace
  ////////////////////////////////////////////////////////////////////////

  template <typename Transf, typename Point>
  void image(Transf const& x, std::vector<Point>& im) {
    im.clear();
    for (size_t i = 0; i < x.degree(); ++i) {
      auto j = x[i];
      if (j != x.undef()) {
        im.push_back(j);
      }
    }
    std::sort(im.begin(), im.end());
    im.erase(std::unique(im.begin(), im.end()), im.end());
  }

  template <typename Transf>
  std::vector<typename Transf::point_type> image(Transf const& x) {
    std::vector<typename Transf::point_type> result;
    image(x, result);
    return result;
  }

  template <typename Transf, typename Point>
  void domain(Transf const& x, std::vector<Point>& dom) {
    dom.clear();
    for (size_t i = 0; i < x.degree(); ++i) {
      auto j = x[i];
      if (j != x.undef()) {
        dom.push_back(i);
      }
    }
    std::sort(dom.begin(), dom.end());
    dom.erase(std::unique(dom.begin(), dom.end()), dom.end());
  }

  template <typename Transf>
  std::vector<typename Transf::point_type> domain(Transf const& x) {
    std::vector<typename Transf::point_type> result;
    domain(x, result);
    return result;
  }

  template <size_t N, typename Scalar>
  PPerm<N, Scalar> right_one(PPerm<N, Scalar> const& x) {
    size_t const     n = x.degree();
    PPerm<N, Scalar> result(n);
    std::fill(result.begin(), result.end(), static_cast<Scalar>(UNDEFINED));
    for (size_t i = 0; i < n; ++i) {
      if (x[i] != UNDEFINED) {
        result[x[i]] = x[i];
      }
    }
    return result;
  }

  template <size_t N, typename Scalar>
  PPerm<N, Scalar> left_one(PPerm<N, Scalar> const& x) {
    size_t const     n = x.degree();
    PPerm<N, Scalar> result(n);
    std::fill(result.begin(), result.end(), static_cast<Scalar>(UNDEFINED));
    for (size_t i = 0; i < n; ++i) {
      if (x[i] != UNDEFINED) {
        result[i] = i;
      }
    }
    return result;
  }

  template <size_t N, typename Scalar>
  void inverse(PPerm<N, Scalar> const& from, PPerm<N, Scalar>& to) {
    if (to.degree() < from.degree()) {
      // Shouldn't be possible to get here if PPerm<N, Scalar> is static
      LIBSEMIGROUPS_ASSERT((IsDynamic<PPerm<N, Scalar>>) );
      to.increase_degree_by(from.degree() - to.degree());
    }
    std::fill(to.begin(), to.end(), static_cast<Scalar>(UNDEFINED));
    for (size_t i = 0; i < from.degree(); ++i) {
      if (from[i] != UNDEFINED) {
        to[from[i]] = i;
      }
    }
  }

  template <size_t N, typename Scalar>
  void inverse(Perm<N, Scalar> const& from, Perm<N, Scalar>& to) {
    if (to.degree() != from.degree()) {
      LIBSEMIGROUPS_EXCEPTION(
          "the arguments must have the same degrees, but found {} != {}",
          to.degree(),
          from.degree());
    }
    std::fill(to.begin(), to.end(), static_cast<Scalar>(UNDEFINED));
    for (size_t i = 0; i < from.degree(); ++i) {
      to[from[i]] = i;
    }
  }

  template <size_t N, typename Scalar>
  Perm<N, Scalar> inverse(Perm<N, Scalar> const& from) {
    Perm<N, Scalar> to(from.degree());
    inverse(from, to);
    return to;
  }

  ////////////////////////////////////////////////////////////////////////
  // Adapters
  ////////////////////////////////////////////////////////////////////////

  template <size_t N, typename Scalar, typename T>
  void ImageRightAction<Transf<N, Scalar>, T>::operator()(
      T&                       res,
      T const&                 pt,
      Transf<N, Scalar> const& x) const {
    res.clear();
    for (auto i : pt) {
      res.push_back(x[i]);
    }
    std::sort(res.begin(), res.end());
    res.erase(std::unique(res.begin(), res.end()), res.end());
  }

  template <size_t N, typename Scalar, typename T>
  void ImageLeftAction<Transf<N, Scalar>, T>::operator()(
      T&                       res,
      T const&                 pt,
      Transf<N, Scalar> const& x) const {
    res.clear();
    res.resize(x.degree());
    static thread_local std::vector<Scalar> buf;
    buf.clear();
    buf.resize(x.degree(), Scalar(UNDEFINED));
    Scalar next = 0;

    for (size_t i = 0; i < res.size(); ++i) {
      if (buf[pt[x[i]]] == UNDEFINED) {
        buf[pt[x[i]]] = next++;
      }
      res[i] = buf[pt[x[i]]];
    }
  }

  template <size_t N, typename Scalar, typename T>
  void
  Lambda<Transf<N, Scalar>, T>::operator()(T&                       res,
                                           Transf<N, Scalar> const& x) const {
    res.clear();
    res.resize(x.degree());
    for (size_t i = 0; i < res.size(); ++i) {
      res[i] = x[i];
    }
    std::sort(res.begin(), res.end());
    res.erase(std::unique(res.begin(), res.end()), res.end());
  }

  template <size_t N, typename Scalar, size_t M>
  void Lambda<Transf<N, Scalar>, BitSet<M>>::operator()(
      BitSet<M>&               res,
      Transf<N, Scalar> const& x) const {
    if (x.degree() > M) {
      LIBSEMIGROUPS_EXCEPTION(
          "expected a transformation of degree at most {}, found {}",
          M,
          x.degree());
    }
    res.reset();
    for (size_t i = 0; i < x.degree(); ++i) {
      res.set(x[i]);
    }
  }

  template <size_t N, typename Scalar, typename T>
  void Rho<Transf<N, Scalar>, T>::operator()(T&                       res,
                                             Transf<N, Scalar> const& x) const {
    res.clear();
    res.resize(x.degree());
    static thread_local std::vector<Scalar> buf;
    buf.clear();
    buf.resize(x.degree(), Scalar(UNDEFINED));
    Scalar next = 0;

    for (size_t i = 0; i < res.size(); ++i) {
      if (buf[x[i]] == UNDEFINED) {
        buf[x[i]] = next++;
      }
      res[i] = buf[x[i]];
    }
  }

  template <size_t N, typename Scalar, typename T>
  void ImageRightAction<PPerm<N, Scalar>, T>::operator()(
      T&                      res,
      T const&                pt,
      PPerm<N, Scalar> const& x) const {
    res.clear();
    for (auto i : pt) {
      if (x[i] != UNDEFINED) {
        res.push_back(x[i]);
      }
    }
    std::sort(res.begin(), res.end());
  }

  template <size_t N, typename Scalar, size_t M>
  void ImageRightAction<PPerm<N, Scalar>, BitSet<M>>::operator()(
      BitSet<M>&              res,
      BitSet<M> const&        pt,
      PPerm<N, Scalar> const& x) const {
    res.reset();
    // Apply the lambda to every set bit in pt
    pt.apply([&x, &res](size_t i) {
      if (x[i] != UNDEFINED) {
        res.set(x[i]);
      }
    });
  }

  template <size_t N, typename Scalar, size_t M>
  void Lambda<PPerm<N, Scalar>, BitSet<M>>::operator()(
      BitSet<M>&              res,
      PPerm<N, Scalar> const& x) const {
    if (x.degree() > M) {
      LIBSEMIGROUPS_EXCEPTION(
          "expected partial perm of degree at most {}, found {}",
          M,
          x.degree());
    }
    res.reset();
    for (size_t i = 0; i < x.degree(); ++i) {
      if (x[i] != UNDEFINED) {
        res.set(x[i]);
      }
    }
  }

  template <size_t N, typename Scalar, size_t M>
  void Rho<PPerm<N, Scalar>, BitSet<M>>::operator()(
      BitSet<M>&              res,
      PPerm<N, Scalar> const& x) const {
    if (x.degree() > M) {
      LIBSEMIGROUPS_EXCEPTION(
          "expected partial perm of degree at most {}, found {}",
          M,
          x.degree());
    }
    static PPerm<N, Scalar> xx(x.degree());
    inverse(x, xx);
    Lambda<PPerm<N, Scalar>, BitSet<M>>()(res, xx);
  }

  namespace detail {
    template <typename Thing>
    std::string transf_to_input_string(Thing const&     x,
                                       std::string_view prefix,
                                       std::string_view braces = "{}") {
      if (braces.size() != 2) {
        LIBSEMIGROUPS_EXCEPTION("the 2nd argument (braces) must have length 2, "
                                "but found {} of length {}",
                                braces,
                                braces.size());
      }
      return fmt::format("{}({}{}{})",
                         prefix,
                         braces[0],
                         fmt::join(x.begin(), x.end(), ", "),
                         braces[1]);
    }
  }  // namespace detail

  template <size_t N, typename Scalar>
  std::string to_input_string(Transf<N, Scalar> const& x,
                              std::string_view         prefix,
                              std::string_view         braces) {
    std::string actual_prefix;
    if (prefix.empty()) {
      // Yes this looks weird, but the point is to keep the string that prefix
      // views alive until after the end of the function call.
      actual_prefix
          = fmt::format("Transf<{}, uint{}_t>", N, sizeof(Scalar) * 8);
      prefix = actual_prefix;
    }
    return detail::transf_to_input_string(x, prefix, braces);
  }

  template <size_t N, typename Scalar>
  std::string to_input_string(Perm<N, Scalar> const& x,
                              std::string_view       prefix,
                              std::string_view       braces) {
    std::string actual_prefix;
    if (prefix.empty()) {
      // Yes this looks weird, but the point is to keep the string that prefix
      // views alive until after the end of the function call.
      actual_prefix = fmt::format("Perm<{}, uint{}_t>", N, sizeof(Scalar) * 8);
      prefix        = actual_prefix;
    }
    return detail::transf_to_input_string(x, prefix, braces);
  }

  template <size_t N, typename Scalar>
  std::string to_input_string(PPerm<N, Scalar> const& x,
                              std::string_view        prefix,
                              std::string_view        braces) {
    if (braces.size() != 2) {
      LIBSEMIGROUPS_EXCEPTION("the 2nd argument (braces) must have length 2, "
                              "but found {} of length {}",
                              braces,
                              braces.size());
    }
    std::string actual_prefix;
    if (prefix.empty()) {
      // Yes this looks weird, but the point is to keep the string that prefix
      // views alive until after the end of the function call.
      actual_prefix = fmt::format("PPerm<{}, uint{}_t>", N, sizeof(Scalar) * 8);
      prefix        = actual_prefix;
    }
    auto dom = domain(x);
    auto im  = dom;
    std::for_each(im.begin(), im.end(), [&x](auto& val) { val = x[val]; });

    return fmt::format("{}({}{}{}, {}{}{}, {})",
                       prefix,
                       braces[0],
                       fmt::join(dom.begin(), dom.end(), ", "),
                       braces[1],
                       braces[0],
                       fmt::join(im.begin(), im.end(), ", "),
                       braces[1],
                       x.degree());
  }

  template <size_t N, typename Scalar>
  std::string to_human_readable_repr(Transf<N, Scalar> const& x,
                                     std::string_view         prefix,
                                     std::string_view         braces,
                                     size_t                   max_width) {
    auto result = to_input_string(x, prefix, braces);
    if (result.size() <= max_width) {
      return result;
    }
    return fmt::format(
        "<transformation of degree {} and rank {}>", x.degree(), x.rank());
  }

  template <size_t N, typename Scalar>
  std::string to_human_readable_repr(Perm<N, Scalar> const& x,
                                     std::string_view       prefix,
                                     std::string_view       braces,
                                     size_t                 max_width) {
    auto result = to_input_string(x, prefix, braces);
    if (result.size() <= max_width) {
      return result;
    }
    return fmt::format("<permutation of degree {}>", x.degree());
  }

  template <size_t N, typename Scalar>
  std::string to_human_readable_repr(PPerm<N, Scalar> const& x,
                                     std::string_view        prefix,
                                     std::string_view        braces,
                                     size_t                  max_width) {
    auto result = to_input_string(x, prefix, braces);
    if (result.size() <= max_width) {
      return result;
    }
    return fmt::format(
        "<partial permutation of degree {} and rank {}>", x.degree(), x.rank());
  }
}  // namespace libsemigroups
