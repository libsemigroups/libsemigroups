//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2023 James D. Mitchell
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

// This file contains a base class for congruence-like classes.

#ifndef LIBSEMIGROUPS_CONG_INTF_HPP_
#define LIBSEMIGROUPS_CONG_INTF_HPP_

#include <cstddef>           // for size_t
#include <initializer_list>  // for initializer_list
#include <iterator>
#include <vector>  // for vector, operator==, vector<>::const_iter...

#include "exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "ranges.hpp"     // for is_input_or_sink_v
#include "runner.hpp"     // for Runner
#include "types.hpp"      // for word_type, relation_type, letter_type, tril

namespace libsemigroups {
  class Congruence;
  class ToddCoxeter;

  //! \defgroup cong_intf_group The congruence interface
  //!
  //! This page contains links to the documentation of the common functionality
  //! that for all  the classes in ``libsemigroups`` that represent congruences
  //! on semigroups and monoids. These classes are:
  //! * \ref Congruence
  //! * \ref Kambites
  //! * \ref KnuthBendix
  //! * \ref todd_coxeter_class_group "ToddCoxeter"

  //! \ingroup cong_intf_group
  //! Defined in ``cong-intf.hpp``.
  //!
  //! Every class for representing a congruence in ``libsemigroups`` is derived
  //! from CongruenceInterface, which holds the member functions and data that
  //! are common to all its derived classes.
  //!
  //! These classes are:
  //! * \ref Congruence
  //! * \ref Kambites
  //! * \ref KnuthBendix
  //! * \ref todd_coxeter_class_group "ToddCoxeter"
  class CongruenceInterface : public Runner {
    /////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - data members - private
    /////////////////////////////////////////////////////////////////////////

    std::vector<word_type> _generating_pairs;
    congruence_kind        _type;

   protected:
    ////////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - constructors + destructor - protected
    ////////////////////////////////////////////////////////////////////////////

    // Constructors + initializers are protected to prevent construction of
    // useless CongruenceInterface objects
    CongruenceInterface() = default;

    CongruenceInterface& init() {
      _generating_pairs.clear();
      Runner::init();
      return *this;
    }

    explicit CongruenceInterface(congruence_kind type) : CongruenceInterface() {
      init(type);
    }

    CongruenceInterface& init(congruence_kind type) {
      init();
      _type = type;
      return *this;
    }

    CongruenceInterface(CongruenceInterface const&)            = default;
    CongruenceInterface(CongruenceInterface&&)                 = default;
    CongruenceInterface& operator=(CongruenceInterface const&) = default;
    CongruenceInterface& operator=(CongruenceInterface&&)      = default;

    ~CongruenceInterface();

    ////////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - validation - protected
    ////////////////////////////////////////////////////////////////////////////

    template <typename Subclass, typename Iterator1, typename Iterator2>
    void throw_if_letter_out_of_bounds(Iterator1 first, Iterator2 last) const {
      static_cast<Subclass const*>(this)->throw_if_letter_out_of_bounds(first,
                                                                        last);
    }

   public:
    ////////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - public member functions
    ////////////////////////////////////////////////////////////////////////////

    //! \brief Returns the number of generating pairs.
    //!
    //! This function returns the number of generating pairs, which is the size
    //! of \ref generating_pairs divided by \f$2\f$.
    //!
    //! \returns
    //! The number of generating pairs.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] size_t number_of_generating_pairs() const noexcept {
      return _generating_pairs.size() / 2;
    }

    //! \brief Returns a const reference to the std::vector of generating
    //! pairs.
    //!
    //! This function returns the generating pairs of the congruence
    //! represented by any derived class of a CongruenceInterface. This is
    //! always a std::vector of \ref word_type, regardless of the
    //! \ref native_presentation_type of the derived class.
    //!
    //! \returns
    //! A const reference to the generating pairs.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    // TODO(0) should implement this in the derived classes, so that the return
    // type is native_word_type, not always word_type.
    [[nodiscard]] std::vector<word_type> const&
    generating_pairs() const noexcept {
      return _generating_pairs;
    }

    //! The handedness of the congruence (left, right, or 2-sided).
    //!
    //! \returns A \ref congruence_kind.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] congruence_kind kind() const noexcept {
      return _type;
    }

   protected:
    // This is protected so that it is not possible to change the kind
    // arbitrarily.
    CongruenceInterface& kind(congruence_kind knd) {
      _type = knd;
      return *this;
    }

    ////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - add_generating_pair
    ////////////////////////////////////////////////////////////////////////

    // The functions in this section are used as aliases in the derived
    // classes, and so can be protected here.
    template <typename Subclass,
              typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    Subclass& add_generating_pair_no_checks(Iterator1 first1,
                                            Iterator2 last1,
                                            Iterator3 first2,
                                            Iterator4 last2) {
      _generating_pairs.emplace_back(first1, last1);
      _generating_pairs.emplace_back(first2, last2);
      return static_cast<Subclass&>(*this);
    }

    template <typename Subclass,
              typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    Subclass& add_generating_pair(Iterator1 first1,
                                  Iterator2 last1,
                                  Iterator3 first2,
                                  Iterator4 last2) {
      throw_if_started();
      throw_if_letter_out_of_bounds<Subclass>(first1, last1);
      throw_if_letter_out_of_bounds<Subclass>(first2, last2);
      return add_generating_pair_no_checks<Subclass>(
          first1, last1, first2, last2);
    }

    ////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - contains
    ////////////////////////////////////////////////////////////////////////

    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    [[nodiscard]] tril currently_contains(Iterator1 first1,
                                          Iterator2 last1,
                                          Iterator3 first2,
                                          Iterator4 last2) const {
      throw_if_letter_out_of_bounds(first1, last1);
      throw_if_letter_out_of_bounds(first2, last2);
      return currently_contains_no_checks(first1, last1, first2, last2);
    }

    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    [[nodiscard]] bool contains(Iterator1 first1,
                                Iterator2 last1,
                                Iterator3 first2,
                                Iterator4 last2) {
      throw_if_letter_out_of_bounds(first1, last1);
      throw_if_letter_out_of_bounds(first2, last2);
      return contains_no_checks(first1, last1, first2, last2);
    }

   private:
    void throw_if_started() const;
  };

  namespace congruence_interface {
    //! \defgroup cong_intf_helpers_group Generic congruence helpers
    //! \ingroup cong_intf_group
    //!
    //! \brief Helper functions for subclasses of \ref CongruenceInterface.
    //!
    //! This page contains documentation for helper functions for the classes
    //! Congruence, Kambites, KnuthBendix, and \ref todd_coxeter_class_group
    //! "ToddCoxeter". The functions documented on this page belong to all of
    //! the namespaces ``congruence_interface``, ``congruence``, ``kambites``,
    //! ``knuth_bendix``, and ``todd_coxeter``.
    //!
    //! @{

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - add_generating_pair
    ////////////////////////////////////////////////////////////////////////

    //! \brief Helper for adding a generating pair of words.
    template <typename Subclass, typename Word>
    Subclass& add_generating_pair_no_checks(Subclass&   ci,
                                            Word const& u,
                                            Word const& v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return ci.add_generating_pair_no_checks(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    // TODO(0) doc
    template <typename Subclass, typename Word>
    Subclass& add_generating_pair_no_checks(Subclass& ci, Word&& u, Word&& v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return ci.add_generating_pair_no_checks(
          std::make_move_iterator(std::begin(u)),
          std::make_move_iterator(std::end(u)),
          std::make_move_iterator(std::begin(v)),
          std::make_move_iterator(std::end(v)));
    }

    // TODO(doc)
    template <typename Subclass, typename Int>
    Subclass&
    add_generating_pair_no_checks(Subclass&                         ci,
                                  std::initializer_list<Int> const& u,
                                  std::initializer_list<Int> const& v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return add_generating_pair_no_checks<Subclass, std::vector<Int>>(
          ci, u, v);
    }

    // TODO(doc)
    template <typename Subclass>
    inline Subclass& add_generating_pair_no_checks(Subclass&   ci,
                                                   char const* u,
                                                   char const* v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return ci.add_generating_pair_no_checks(
          u, u + std::strlen(u), v, v + std::strlen(v));
    }

    // TODO(doc)
    template <typename Subclass, typename Word>
    Subclass& add_generating_pair(Subclass& ci, Word const& u, Word const& v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return ci.add_generating_pair(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    // TODO(doc)
    template <typename Subclass, typename Word>
    Subclass& add_generating_pair(Subclass& ci, Word&& u, Word&& v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return ci.add_generating_pair(std::make_move_iterator(std::begin(u)),
                                    std::make_move_iterator(std::end(u)),
                                    std::make_move_iterator(std::begin(v)),
                                    std::make_move_iterator(std::end(v)));
    }

    // TODO(doc)
    template <typename Subclass, typename Int>
    Subclass& add_generating_pair(Subclass&                         ci,
                                  std::initializer_list<Int> const& u,
                                  std::initializer_list<Int> const& v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return add_generating_pair<Subclass, std::vector<Int>>(ci, u, v);
    }

    // TODO(doc)
    template <typename Subclass>
    Subclass& add_generating_pair(Subclass& ci, char const* u, char const* v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return ci.add_generating_pair(
          u, u + std::strlen(u), v, v + std::strlen(v));
    }

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - currently_contains_no_checks
    ////////////////////////////////////////////////////////////////////////

    // TODO(0) doc
    template <typename Subclass, typename Word>
    [[nodiscard]] tril currently_contains_no_checks(Subclass const& ci,
                                                    Word const&     u,
                                                    Word const&     v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return ci.currently_contains_no_checks(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    // This version of the function catches the cases when u & v are not of the
    // same type but both convertible to string_view
    template <typename Subclass>
    [[nodiscard]] tril currently_contains_no_checks(Subclass const&  ci,
                                                    std::string_view u,
                                                    std::string_view v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return ci.currently_contains_no_checks(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    template <typename Subclass>
    [[nodiscard]] tril currently_contains_no_checks(Subclass const& ci,
                                                    char const*     u,
                                                    char const*     v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return ci.currently_contains_no_checks(
          u, u + std::strlen(u), v, v + std::strlen(v));
    }

    template <typename Subclass,
              typename Int = typename Subclass::native_letter_type>
    [[nodiscard]] tril
    currently_contains_no_checks(Subclass const&                   ci,
                                 std::initializer_list<Int> const& u,
                                 std::initializer_list<Int> const& v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return currently_contains_no_checks<Subclass, std::initializer_list<Int>>(
          ci, u, v);
    }

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - currently_contains
    ////////////////////////////////////////////////////////////////////////

    // TODO(0) doc
    template <typename Subclass, typename Word>
    [[nodiscard]] tril currently_contains(Subclass const& ci,
                                          Word const&     u,
                                          Word const&     v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return ci.currently_contains(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    // This version of the function catches the cases when u & v are not of the
    // same type but both convertible to string_view
    template <typename Subclass>
    [[nodiscard]] tril currently_contains(Subclass const&  ci,
                                          std::string_view u,
                                          std::string_view v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return ci.currently_contains(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    template <typename Subclass>
    [[nodiscard]] tril currently_contains(Subclass const& ci,
                                          char const*     u,
                                          char const*     v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return ci.currently_contains_no_checks(
          u, u + std::strlen(u), v, v + std::strlen(v));
    }

    // TODO(0) doc
    template <typename Subclass,
              typename Int = typename Subclass::native_letter_type>
    [[nodiscard]] tril currently_contains(Subclass const&                   ci,
                                          std::initializer_list<Int> const& u,
                                          std::initializer_list<Int> const& v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return currently_contains<Subclass, std::initializer_list<Int>>(ci, u, v);
    }

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - contains_no_checks
    ////////////////////////////////////////////////////////////////////////

    // TODO(0) doc
    template <typename Subclass, typename Word>
    [[nodiscard]] bool contains_no_checks(Subclass&   ci,
                                          Word const& u,
                                          Word const& v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return ci.contains_no_checks(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    // This version of the function catches the cases when u & v are not of the
    // same type but both convertible to string_view
    template <typename Subclass>
    [[nodiscard]] bool contains_no_checks(Subclass&        ci,
                                          std::string_view u,
                                          std::string_view v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return ci.contains_no_checks(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    template <typename Subclass>
    [[nodiscard]] bool contains_no_checks(Subclass&   ci,
                                          char const* u,
                                          char const* v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return ci.contains_no_checks(
          u, u + std::strlen(u), v, v + std::strlen(v));
    }

    // TODO(0) doc
    template <typename Subclass,
              typename Int = typename Subclass::native_letter_type>
    [[nodiscard]] bool contains_no_checks(Subclass&                         ci,
                                          std::initializer_list<Int> const& u,
                                          std::initializer_list<Int> const& v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return contains_no_checks<Subclass, std::initializer_list<Int>>(ci, u, v);
    }

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - contains
    ////////////////////////////////////////////////////////////////////////

    // TODO(0) doc
    template <typename Subclass, typename Word>
    [[nodiscard]] bool contains(Subclass& ci, Word const& u, Word const& v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return ci.contains(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    // This version of the function catches the cases when u & v are not of the
    // same type but both convertible to string_view
    template <typename Subclass>
    [[nodiscard]] bool contains(Subclass&        ci,
                                std::string_view u,
                                std::string_view v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return ci.contains(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    template <typename Subclass>
    [[nodiscard]] bool contains(Subclass& ci, char const* u, char const* v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return ci.contains(u, u + std::strlen(u), v, v + std::strlen(v));
    }

    // TODO(0) doc
    template <typename Subclass,
              typename Int = typename Subclass::native_letter_type>
    [[nodiscard]] bool contains(Subclass&                         ci,
                                std::initializer_list<Int> const& u,
                                std::initializer_list<Int> const& v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return contains<Subclass, std::initializer_list<Int>>(ci, u, v);
    }

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - reduce_no_run_no_checks
    ////////////////////////////////////////////////////////////////////////

    // TODO(0) doc
    template <typename Subclass,
              typename InputWord,
              typename OutputWord = InputWord>
    [[nodiscard]] OutputWord reduce_no_run_no_checks(Subclass const&  ci,
                                                     InputWord const& w) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      OutputWord result;
      ci.reduce_no_run_no_checks(
          std::back_inserter(result), std::begin(w), std::end(w));
      return result;
    }

    // No string_view version is required because there is only a single "word"
    // parameter, and so the first template will catch every case except
    // initializer_list and char const*

    // TODO(0) doc
    template <typename Subclass, typename Int = size_t>
    [[nodiscard]] auto
    reduce_no_run_no_checks(Subclass const&                   ci,
                            std::initializer_list<Int> const& w) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      if constexpr (std::is_same_v<Subclass, ToddCoxeter>) {
        // We have a special case here for ToddCoxeter because ToddCoxeter
        // reverses the output words, in its reduce_no_run_no_checks, which does
        // not have the template parameter Subclass, so we call the 2-template
        // parameter overload of this function for ToddCoxeter. I.e. if we
        // called the 3-template param version as in the "else" case below, then
        // we'd just be calling the function above, which doesn't reverse the
        // words, and we'd get incorrect output.
        return reduce_no_run_no_checks<std::initializer_list<Int>,
                                       std::vector<Int>>(ci, w);
      } else {
        return reduce_no_run_no_checks<Subclass,
                                       std::initializer_list<Int>,
                                       std::vector<Int>>(ci, w);
      }
    }

    // TODO(0) doc
    template <typename Subclass>
    [[nodiscard]] auto reduce_no_run_no_checks(Subclass const& ci,
                                               char const*     w) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      if constexpr (std::is_same_v<Subclass, ToddCoxeter>) {
        return reduce_no_run_no_checks<std::string, std::string>(ci, w);
      } else {
        return reduce_no_run_no_checks<Subclass, std::string, std::string>(ci,
                                                                           w);
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - reduce_no_run
    ////////////////////////////////////////////////////////////////////////

    // TODO(0) doc
    template <typename Subclass,
              typename InputWord,
              typename OutputWord = InputWord>
    [[nodiscard]] OutputWord reduce_no_run(Subclass const&  ci,
                                           InputWord const& w) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      OutputWord result;
      ci.reduce_no_run(std::back_inserter(result), std::begin(w), std::end(w));
      return result;
    }

    // No string_view version is required because there is only a single "word"
    // parameter, and so the first template will catch every case except
    // initializer_list and char const*

    // TODO(0) doc
    template <typename Subclass, typename Int = size_t>
    [[nodiscard]] auto reduce_no_run(Subclass const&                   ci,
                                     std::initializer_list<Int> const& w) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      if constexpr (std::is_same_v<Subclass, ToddCoxeter>) {
        // See the comment above about why there's a special case here.
        return reduce_no_run<std::initializer_list<Int>, std::vector<Int>>(ci,
                                                                           w);
      } else {
        return reduce_no_run<Subclass,
                             std::initializer_list<Int>,
                             std::vector<Int>>(ci, w);
      }
    }

    // TODO(0) doc
    template <typename Subclass>
    [[nodiscard]] auto reduce_no_run(Subclass const& ci, char const* w) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      if constexpr (std::is_same_v<Subclass, ToddCoxeter>) {
        // See the comment above about why there's a special case here.
        return reduce_no_run<std::string, std::string>(ci, w);
      } else {
        return reduce_no_run<Subclass, std::string, std::string>(ci, w);
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - reduce_no_checks
    ////////////////////////////////////////////////////////////////////////

    // TODO(0) doc
    template <typename Subclass,
              typename InputWord,
              typename OutputWord = InputWord>
    [[nodiscard]] OutputWord reduce_no_checks(Subclass&        ci,
                                              InputWord const& w) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      OutputWord result;
      ci.reduce_no_checks(
          std::back_inserter(result), std::begin(w), std::end(w));
      return result;
    }

    // No string_view version is required because there is only a single "word"
    // parameter, and so the first template will catch every case except
    // initializer_list and char const*

    // TODO(0) doc
    template <typename Subclass, typename Int = size_t>
    [[nodiscard]] auto reduce_no_checks(Subclass&                         ci,
                                        std::initializer_list<Int> const& w) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      if constexpr (std::is_same_v<Subclass, ToddCoxeter>) {
        // See the comment above about why there's a special case here.
        return reduce_no_checks<std::initializer_list<Int>, std::vector<Int>>(
            ci, w);
      } else {
        return reduce_no_checks<Subclass,
                                std::initializer_list<Int>,
                                std::vector<Int>>(ci, w);
      }
    }

    // TODO(0) doc
    template <typename Subclass>
    [[nodiscard]] auto reduce_no_checks(Subclass& ci, char const* w) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      if constexpr (std::is_same_v<Subclass, ToddCoxeter>) {
        // See the comment above about why there's a special case here.
        return reduce_no_checks<std::string, std::string>(ci, w);
      } else {
        return reduce_no_checks<Subclass, std::string, std::string>(ci, w);
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - reduce
    ////////////////////////////////////////////////////////////////////////

    // TODO(0) doc
    template <typename Subclass,
              typename InputWord,
              typename OutputWord = InputWord>
    [[nodiscard]] OutputWord reduce(Subclass& ci, InputWord const& w) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      OutputWord result;
      ci.reduce(std::back_inserter(result), std::begin(w), std::end(w));
      return result;
    }

    // No string_view version is required because there is only a single "word"
    // parameter, and so the first template will catch every case except
    // initializer_list and char const*

    // TODO(0) doc
    template <typename Subclass, typename Int = size_t>
    [[nodiscard]] auto reduce(Subclass&                         ci,
                              std::initializer_list<Int> const& w) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      if constexpr (std::is_same_v<Subclass, ToddCoxeter>) {
        // See the comment above about why there's a special case here.
        return reduce<std::initializer_list<Int>, std::vector<Int>>(ci, w);
      } else {
        return reduce<Subclass, std::initializer_list<Int>, std::vector<Int>>(
            ci, w);
      }
    }

    // TODO(0) doc
    template <typename Subclass>
    [[nodiscard]] auto reduce(Subclass& ci, char const* w) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      if constexpr (std::is_same_v<Subclass, ToddCoxeter>) {
        return reduce<std::string, std::string>(ci, w);
      } else {
        return reduce<Subclass, std::string, std::string>(ci, w);
      }
    }

    //! @}

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - normal_forms
    ////////////////////////////////////////////////////////////////////////

    // There's nothing in common to implement in this file.

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - partition
    ////////////////////////////////////////////////////////////////////////

    // TODO Doc
    // TODO tpp file
    // TODO to congruence namespace
    template <typename Subclass, typename Range>
    [[nodiscard]] std::vector<
        std::vector<std::decay_t<typename Range::output_type>>>
    partition(Subclass& kb, Range r) {
      // Congruence + ToddCoxeter have their own overloads for this
      static_assert(!std::is_same_v<Subclass, ToddCoxeter>
                    && !std::is_same_v<Subclass, Congruence>);

      using output_type = std::decay_t<typename Range::output_type>;
      using return_type = std::vector<std::vector<output_type>>;

      if (!r.is_finite) {
        LIBSEMIGROUPS_EXCEPTION("the 2nd argument (a range) must be finite, "
                                "found an infinite range");
      }

      return_type result;

      std::unordered_map<output_type, size_t> map;
      size_t                                  index = 0;

      while (!r.at_end()) {
        auto next = r.get();
        if (kb.presentation().contains_empty_word() || !next.empty()) {
          auto next_nf        = congruence_interface::reduce(kb, next);
          auto [it, inserted] = map.emplace(next_nf, index);
          if (inserted) {
            result.emplace_back();
            index++;
          }
          size_t index_of_next_nf = it->second;
          result[index_of_next_nf].push_back(next);
        }
        r.next();
      }
      return result;
    }

    // TODO Doc
    // TODO tpp file
    template <
        typename Subclass,
        typename Iterator1,
        typename Iterator2,
        typename Word = std::decay_t<
            typename rx::iterator_range<Iterator1, Iterator2>::output_type>>
    std::vector<std::vector<Word>> partition(Subclass& ci,
                                             Iterator1 first,
                                             Iterator2 last) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return partition(ci, rx::iterator_range(first, last));
    }

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - non_trivial_classes
    ////////////////////////////////////////////////////////////////////////

    template <typename Subclass,
              typename Range,
              typename Word = std::decay_t<typename Range::output_type>,
              typename      = std::enable_if_t<rx::is_input_or_sink_v<Range>>>
    std::vector<std::vector<Word>> non_trivial_classes(Subclass& ci, Range r) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      auto result = partition(ci, r);
      result.erase(
          std::remove_if(result.begin(),
                         result.end(),
                         [](auto const& x) -> bool { return x.size() <= 1; }),
          result.end());
      return result;
    }

    template <
        typename Subclass,
        typename Iterator1,
        typename Iterator2,
        typename Word = std::decay_t<
            typename rx::iterator_range<Iterator1, Iterator2>::output_type>>
    std::vector<std::vector<Word>> non_trivial_classes(Subclass& ci,
                                                       Iterator1 first,
                                                       Iterator2 last) {
      return non_trivial_classes(ci, rx::iterator_range(first, last));
    }
  }  // namespace congruence_interface
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_CONG_INTF_HPP_

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// old doc follows TODO use it or lose it
////////////////////////////////////////////////////////////////////////////
// CongruenceInterface - non-pure virtual functions - public
////////////////////////////////////////////////////////////////////////////

//! Check if a pair of words belongs to the congruence.
//!
//! \param u a word (vector of integers) over the generators of the
//! semigroup.
//! \param v a word (vector of integers) over the generators of
//! the semigroup.
//!
//! \returns \c true if the words \p u and \p v belong to the
//! same congruence class, and \c false otherwise.
//!
//! \throws LibsemigroupsException if \p u or \p v contains a letter that
//! is out of bounds.
//!
//! \throws std::bad_alloc if the (possibly infinite) computation uses all
//! the available memory.
//!
//! \complexity
//! See warning.
//!
//! \warning The problem of determining the return value of this function
//! is undecidable in general, and this function may never terminate.
// virtual bool contains(word_type const& u, word_type const& v) {
//   return u == v || word_to_class_index(u) == word_to_class_index(v);
// }

// bool contains_with_arg_checks(word_type const& lhs,
//                               word_type const& rhs) {
//   validate_word(lhs);
//   validate_word(rhs);
//   return contains(lhs, rhs);
// }

// Same as the above but only uses the so far computed information to
// answer. In particular, does not call this->run().
//! Check if a pair of words is known to belong to the congruence.
//!
//! \param u a word (vector of integers) over the generators of the
//! semigroup.
//! \param v a word (vector of integers) over the generators of
//! the semigroup.
//!
//! \returns
//! * tril::TRUE if the words \p u and \p v
//! are known to belong to the same congruence class
//!
//! * tril::FALSE if the words are known to not belong
//! to the same congruence class
//!
//! * tril::unknown otherwise.
//!
//! \throws LibsemigroupsException if \p u or \p v contains a letter that
//! is out of bounds.
//!
//! \complexity
//! Linear in `u.size() + v.size()`.
// virtual tril const_contains(word_type const& u, word_type const& v)
// const;

//! Compare the indices of the classes containing two words.
//!
//! This function returns \c true if the congruence class of \p v is less
//! than the class of \p v in a total ordering of congruence classes.
//!
//! \param u a word (vector of integers) over the generators of the
//! semigroup.
//! \param v a word (vector of integers) over the generators of
//! the semigroup.
//!
//! \returns \c true if the class of \p u is less than that of \p .
//!
//! \throws LibsemigroupsException if \p u or \p v contains a letter that
//! is out of bounds.
//!
//! \throws std::bad_alloc if the (possibly infinite) computation uses all
//! the available memory.
//!
//! \complexity
//! See warning.
//!
//! \warning The problem of determining the return value of this function
//! is undecidable in general, and this function may never terminate.
//!
//! \par Possible Implementation
//! \code
//! bool less(word_type const& u, word_type const& v) {
//!   return word_to_class_index(u) < word_to_class_index(v);
//! }
//! \endcode
// virtual bool less(word_type const& u, word_type const& v) {
//   return word_to_class_index(u) < word_to_class_index(v);
// }

/////////////////////////////////////////////////////////////////////////
// CongruenceInterface - non-virtual functions - public
/////////////////////////////////////////////////////////////////////////

//! Get a canonical representative of the \c i-th class.
//!
//! If the congruence, that an object of this type represents, is defined
//! over a semigroup with generators \f$A\f$, then this function defines a
//! injective function from \f$\{0, 1, \ldots, n -  1\}\f$, where \f$n\f$
//! is the number of classes, or to the non-negative integers
//! \f$\{0, 1, \ldots\}\f$ if \c this has infinitely many classes, to a
//! fixed set of words over \f$A\f$ representing distinct congruences
//! classes.
//!
//! \param i the index of the class whose representative we want to find,
//! a value of type \ref word_type.
//!
//! \returns The word representing the \p i-th class of the congruence
//!
//! \throws LibsemigroupsException if the specified class index \p i
//! exceeds the total number of classes.
//!
//! \throws std::bad_alloc if the (possibly infinite) computation uses all
//! the available memory.
//!
//! \complexity
//! See warning.
//!
//! \warning The function for finding the structure of a congruence may be
//! non-deterministic, or undecidable, and this function may never return
//! a result.
//!
//! \note
//! word_to_class_index() and class_index_to_word() are mutually inverse
//! functions.
// word_type class_index_to_word(class_index_type i);

//! Convert a word into the index of the class containing it.
//!
//! If the congruence, that an object of this type represents, is defined
//! over a semigroup with generators \f$A\f$, then this function defines a
//! surjective function from the set of all words over \f$A\f$ to either
//! \f$\{0, 1, \ldots, n -  1\}\f$, where \f$n\f$ is the number of
//! classes, or to the non-negative integers \f$\{0, 1, \ldots\}\f$ if \c
//! this has infinitely many classes.
//!
//! \param w the word whose class index we want to find. The parameter \p
//! w must be a \ref word_type consisting of indices of the
//! generators of the semigroup over which \c this is defined.
//!
//! \returns The index of the congruence class corresponding to \p word.
//!
//! \throws LibsemigroupsException if \p w contains a letter that is out
//! of bounds, or the object has not been fully initialised.
//!
//! \throws std::bad_alloc if the (possibly infinite) computation uses all
//! the available memory.
//!
//! \complexity
//! See warning.
//!
//! \warning The function for finding the structure of a congruence may be
//! non-deterministic, or undecidable, and this function may never return
//! a result.
//!
//! \note
//! word_to_class_index() and class_index_to_word() are mutually inverse
//! functions.
// class_index_type word_to_class_index(word_type const& w);

//! Returns a const iterator pointing to the first non-singleton class.
//!
//! \returns
//! A \ref non_trivial_class_iterator pointing to a
//! std::vector<word_type>.
//!
//! \throws LibsemigroupsException if has_parent_froidure_pin() returns
//! `false`.
//!
//! \complexity
//! See warnings.
//!
//! \warning The problem of determining the return value of this function
//! is undecidable in general, and this function may never terminate.
//!
// TODO copy doc

//! Returns a const iterator pointing one-past-the-end of the last
//! non-singleton class.
//!
//! \returns
//! A \ref non_trivial_class_iterator pointing to a
//! std::vector<word_type>.
//!
//! \throws LibsemigroupsException if has_parent_froidure_pin() returns
//! `false`.
//!
//! \complexity
//! See warnings.
//!
//! \warning The problem of determining the return value of this function
//! is undecidable in general, and this function may never terminate.
//!
// TODO copy doc

// TODO copy doc?
//! The number of non-singleton classes.
//!
//! \returns The number of non-singleton classes of the congruence.
//!
//! \throws LibsemigroupsException if has_parent_froidure_pin() returns \c
//! false.
//!
//! \complexity
//! See warning.
//!
//! \warning The problem of determining the return value of this function
//! is undecidable in general, and this function may never terminate.
//!

// TODO copy doc?
//! Returns a semigroup represented as an instance of a derived class of
//! FroidurePinBase that is isomorphic to the quotient of the parent
//! semigroup of \c this by the 2-sided congruence that \c this
//! represents.
//!
//! \returns A \shared_ptr to FroidurePinBase.
//!
//! \throws LibsemigroupsException if any of the following hold:
//! * the congruence is not 2-sided, `side() !=
//! congruence_kind::twosided`
//! * the quotient semigroup is known (or can be easily be shown to be)
//! infinite
//! * the implementation throws.
//!
//! \throws std::bad_alloc if the (possibly infinite) computation uses all
//! the available memory.
//!
//! \warning The problem of determining the return value of this function
//! is undecidable in general, and this function may never terminate.
//!
//! \note
//! The returned FroidurePin instance satisfies `FroidurePin::immutable()
//! == true` and so certain of its member functions (those that change
//! the underlying mathematical object) are disabled.
//!

// TODO copy doc?
//! Check if the quotient semigroup has been computed.
//!
//! Returns \c true if the congruence represented by this object knows an
//! isomorphic quotient semigroup represented by an instance of
//! FroidurePin.
//!
//! \returns A `bool`.
//!
//! \exceptions
//! \noexcept
//!
//! \complexity
//! Constant.
