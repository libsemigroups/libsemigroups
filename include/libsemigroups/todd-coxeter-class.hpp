//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025 James D. Mitchell
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

// This file contains the declaration of the ToddCoxeter class. This class
// exists mostly to wrap ToddCoxeterBase (where the action happens) to make it
// more user friendly. In particular, ToddCoxeter presentation()
// and generating_pairs() return the input presentation and generating pairs,
// whatever their type, and not the normalized Presentation<word_type> required
// by ToddCoxeterBase.

#ifndef LIBSEMIGROUPS_TODD_COXETER_CLASS_HPP_
#define LIBSEMIGROUPS_TODD_COXETER_CLASS_HPP_

#include "todd-coxeter-base.hpp"
// TODO(0) iwyu, doc

namespace libsemigroups {

  template <typename Word>
  class ToddCoxeter : public ToddCoxeterBase {
   private:
    std::vector<Word>  _generating_pairs;
    Presentation<Word> _presentation;

    // The following is a class for wrapping iterators. This is used by the
    // member functions that accept iterators (that point at words that might
    // not be the word_type used by ToddCoxeterBase) to convert the values
    // pointed at into word_types, and in the class itow, to allow assignment of
    // these values too.
    // CITOW = const_iterator_to_word
    template <typename Iterator>
    class citow {
     protected:
      Iterator           _it;
      ToddCoxeter const* _tc;

     public:
      using internal_iterator_type = Iterator;
      using value_type             = letter_type;
      using reference              = letter_type;
      using const_reference        = value_type;
      using const_pointer          = value_type const*;
      using pointer                = value_type*;

      using size_type         = size_t;
      using difference_type   = std::ptrdiff_t;
      using iterator_category = std::bidirectional_iterator_tag;

      citow(ToddCoxeter const* tc, Iterator it) : _it(it), _tc(tc) {}

      reference operator*() const {
        return _tc->_presentation.index_no_checks(*_it);
      }

      // TODO(1) operator-> ??

      bool operator==(citow<Iterator> that) const noexcept {
        return _it == that._it;
      }

      bool operator!=(citow<Iterator> that) const noexcept {
        return _it != that._it;
      }

      bool operator<=(citow<Iterator> that) const noexcept {
        return _it <= that._it;
      }

      bool operator>=(citow<Iterator> that) const noexcept {
        return _it >= that._it;
      }

      bool operator<(citow<Iterator> that) const noexcept {
        return _it < that._it;
      }

      bool operator>(citow<Iterator> that) const noexcept {
        return _it > that._it;
      }

      citow& operator++() {
        ++_it;
        return *this;
      }

      citow& operator+=(size_type val) noexcept {
        _it += val;
        return *this;
      }

      citow operator+(size_type val) const noexcept {
        citow result(*this);
        result += val;
        return result;
      }

      citow& operator--() {
        --_it;
        return *this;
      }

      citow& operator-=(size_type val) noexcept {
        _it -= val;
        return *this;
      }

      citow operator-(size_type val) const noexcept {
        citow result(*this);
        result -= val;
        return result;
      }

      [[nodiscard]] Iterator get() const noexcept {
        return _it;
      }
    };  // class citow

    // itow only differs from citow in the dereference member function
    // returning a (non-const) reference. A proxy is returned instead which
    // permits assignment to an output iterator.
    template <typename Iterator>
    class itow : public citow<Iterator> {
      // Proxy class for reference to the returned values
      class proxy_ref {
       private:
        Iterator           _it;
        ToddCoxeter const* _tc;

       public:
        // Constructor from ToddCoxeter and iterator
        proxy_ref(ToddCoxeter const* tc, Iterator it) noexcept
            : _it(it), _tc(tc) {}

        // Assignment operator to allow setting the value via the proxy
        Iterator operator=(letter_type i) noexcept {
          *_it = _tc->_presentation.letter_no_checks(i);
          return _it;
        }

        // Conversion operator to obtain the letter corresponding to the
        // letter_type
        [[nodiscard]] operator letter_type() const noexcept {
          return _tc->_presentation.index_no_checks(*_it);
        }
      };  // class proxy_ref

     public:
      using internal_iterator_type = Iterator;
      using value_type             = letter_type;
      using reference              = proxy_ref;
      using const_reference        = value_type;

      // TODO(1) use proxy for pointers too?
      using const_pointer = value_type const*;
      using pointer       = value_type*;

      using size_type         = size_t;
      using difference_type   = std::ptrdiff_t;
      using iterator_category = std::bidirectional_iterator_tag;

      using citow<Iterator>::citow;

      reference operator*() {
        return reference(this->_tc, this->_it);
      }
    };  // class itow

    // Helpers for constructing citow + itow
    template <typename Iterator>
    citow<Iterator> make_citow(Iterator it) const {
      return citow<Iterator>(this, it);
    }

    template <typename Iterator>
    itow<Iterator> make_itow(Iterator it) const {
      return itow<Iterator>(this, it);
    }

   public:
    using native_word_type = Word;
    // TODO(0) native_letter_type, native_presentation_type

    ToddCoxeter() = default;

    ToddCoxeter& init() {
      _generating_pairs.clear();
      _presentation.init();
      ToddCoxeterBase::init();
    }

    ToddCoxeter(ToddCoxeter const&)            = default;
    ToddCoxeter(ToddCoxeter&&)                 = default;
    ToddCoxeter& operator=(ToddCoxeter const&) = default;
    ToddCoxeter& operator=(ToddCoxeter&&)      = default;

    ToddCoxeter(congruence_kind knd, Presentation<Word>&& p) : ToddCoxeter() {
      init(knd, std::move(p));
    }

    // TODO(0) to tpp
    ToddCoxeter& init(congruence_kind knd, Presentation<Word>&& p) {
      if constexpr (!std::is_same_v<Word, word_type>) {
        // to_presentation throws in the next line if p isn't valid
        ToddCoxeterBase::init(knd, to_presentation<word_type>(p));
        _presentation = std::move(p);
      } else {
        p.validate();
        _presentation = p;  // copy p in to _presentation
        presentation::normalize_alphabet(p);
        ToddCoxeterBase::init(knd, std::move(p));
      }
      return *this;
    }

    ToddCoxeter(congruence_kind knd, Presentation<Word> const& p)
        // call the rval ref constructor
        : ToddCoxeter(knd, Presentation<Word>(p)) {}

    ToddCoxeter& init(congruence_kind knd, Presentation<Word> const& p) {
      // call the rval ref init
      return init(knd, Presentation<Word>(p));
    }

    ToddCoxeter(congruence_kind knd, ToddCoxeter const& tc)
        : ToddCoxeterBase(knd, tc), _presentation(tc.presentation()) {}

    ToddCoxeter& init(congruence_kind knd, ToddCoxeter const& tc) {
      ToddCoxeterBase::init(knd, tc);
      _presentation = tc.presentation();
    }

    template <typename Node>
    ToddCoxeter(congruence_kind knd, WordGraph<Node> const& wg)
        : ToddCoxeter() {
      init(knd, wg);
    }

    template <typename Node>
    ToddCoxeter& init(congruence_kind knd, WordGraph<Node> const& wg);

    // Used in Sims
    // TODO(1) could this and the next function be removed, and replaced
    // with something else?
    template <typename Node>
    ToddCoxeter(congruence_kind           knd,
                Presentation<Word> const& p,
                WordGraph<Node> const&    wg) {
      init(knd, p, wg);
    }

    // TODO(1) a to_todd_coxeter variant that throws if p is not valid
    // TODO(0) to tpp
    template <typename Node>
    ToddCoxeter& init(congruence_kind           knd,
                      Presentation<Word> const& p,
                      WordGraph<Node> const&    wg) {
      if constexpr (!std::is_same_v<Word, word_type>) {
        // to_presentation throws in the next line if p isn't valid
        ToddCoxeterBase::init(knd, to_presentation<word_type>(p), wg);
        _presentation = p;
      } else {
        p.validate();
        _presentation = p;  // copy p in to _presentation
        auto copy     = p;
        presentation::normalize_alphabet(copy);
        ToddCoxeterBase::init(knd, copy, wg);
      }
      return *this;
    }

    template <typename Iterator1, typename Iterator2>
    void throw_if_letter_out_of_bounds(Iterator1 first, Iterator2 last) const {
      presentation().validate_word(first, last);
    }

    //! \brief Get the generating pairs of the congruence.
    //!
    //! This function returns the generating pairs of the congruence. The words
    //! comprising the generating pairs are converted to \ref native_word_type
    //! as they are added via \ref add_generating_pair. This function returns
    //! the std::vector of these \ref native_word_type.
    //!
    //! \returns The std::vector of generating pairs.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] std::vector<Word> const& generating_pairs() const noexcept {
      return _generating_pairs;
    }

    //! \brief Get the presentation used to define a \ref
    //! todd_coxeter_class_group "ToddCoxeter" instance (if any).
    //!
    //! If a \ref todd_coxeter_class_group "ToddCoxeterBase" instance is
    //! constructed or initialised using a presentation, then a const reference
    //! to the \ref native_presentation_type version of this presentation is
    //! returned by this function.
    //!
    //! If the \ref todd_coxeter_class_group "ToddCoxeterBase" instance was
    //! constructed or initialised from a WordGraph, then this presentation will
    //! be empty.
    //!
    //! \returns A const reference to the presentation.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] Presentation<Word> const& presentation() const noexcept {
      return _presentation;
    }

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - interface requirements - add_generating_pair
    ////////////////////////////////////////////////////////////////////////
    // TODO(0) to tpp
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    ToddCoxeter& add_generating_pair_no_checks(Iterator1 first1,
                                               Iterator2 last1,
                                               Iterator3 first2,
                                               Iterator4 last2) {
      // Add the input iterators to _generating_pairs
      _generating_pairs.emplace_back(first1, last1);
      _generating_pairs.emplace_back(first2, last2);
      ToddCoxeterBase::add_generating_pair_no_checks(make_citow(first1),
                                                     make_citow(last1),
                                                     make_citow(first2),
                                                     make_citow(last2));
      return *this;
    }

    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    ToddCoxeter& add_generating_pair(Iterator1 first1,
                                     Iterator2 last1,
                                     Iterator3 first2,
                                     Iterator4 last2) {
      // Call CongruenceInterface version so that we perform bound checks in
      // ToddCoxeter and not ToddCoxeterBase
      return CongruenceInterface::add_generating_pair<ToddCoxeter>(
          first1, last1, first2, last2);
    }

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - interface requirements - contains
    ////////////////////////////////////////////////////////////////////////

    //! \brief Check containment of a pair of words via iterators.
    //!
    //! This function checks whether or not the words represented by the ranges
    //! \p first1 to \p last1 and \p first2 to \p last2 are already known to be
    //! contained in the congruence represented by a \ref
    //! todd_coxeter_class_group "ToddCoxeterBase" instance. This function
    //! performs no enumeration, so it is possible for the words to be contained
    //! in the congruence, but that this is not currently known.
    //!
    //! \cong_intf_params_contains
    //!
    //! \returns
    //! * tril::TRUE if the words are known to belong to the congruence;
    //! * tril::FALSE if the words are known to not belong to the congruence;
    //! * tril::unknown otherwise.
    //!
    //! \cong_intf_warn_assume_letters_in_bounds
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    tril currently_contains_no_checks(Iterator1 first1,
                                      Iterator2 last1,
                                      Iterator3 first2,
                                      Iterator4 last2) const {
      return ToddCoxeterBase::currently_contains_no_checks(make_citow(first1),
                                                           make_citow(last1),
                                                           make_citow(first2),
                                                           make_citow(last2));
    }

    //! \brief Check containment of a pair of words via iterators.
    //!
    //! This function checks whether or not the words represented by the ranges
    //! \p first1 to \p last1 and \p first2 to \p last2 are already known to be
    //! contained in the congruence represented by a \ref
    //! todd_coxeter_class_group "ToddCoxeterBase" instance. This function
    //! performs no enumeration, so it is possible for the words to be contained
    //! in the congruence, but that this is not currently known.
    //!
    //! \cong_intf_params_contains
    //!
    //! \returns
    //! * tril::TRUE if the words are known to belong to the congruence;
    //! * tril::FALSE if the words are known to not belong to the congruence;
    //! * tril::unknown otherwise.
    //!
    //! \cong_intf_throws_if_letters_out_of_bounds
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    tril currently_contains(Iterator1 first1,
                            Iterator2 last1,
                            Iterator3 first2,
                            Iterator4 last2) const {
      // Call CongruenceInterface version so that we perform bound checks in
      // ToddCoxeter and not ToddCoxeterBase
      return CongruenceInterface::currently_contains<ToddCoxeter>(
          first1, last1, first2, last2);
    }

    //! \brief Check containment of a pair of words via iterators.
    //!
    //! This function checks whether or not the words represented by the ranges
    //! \p first1 to \p last1 and \p first2 to \p last2 are contained in the
    //! congruence represented by a \ref todd_coxeter_class_group
    //! "ToddCoxeterBase" instance. This function triggers a full enumeration,
    //! which may never terminate.
    //!
    //! \cong_intf_params_contains
    //!
    //! \returns Whether or not the pair belongs to the congruence.
    //!
    //! \cong_intf_warn_undecidable{Todd-Coxeter}
    //!
    //! \cong_intf_warn_assume_letters_in_bounds
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    bool contains_no_checks(Iterator1 first1,
                            Iterator2 last1,
                            Iterator3 first2,
                            Iterator4 last2) {
      return ToddCoxeterBase::contains_no_checks(make_citow(first1),
                                                 make_citow(last1),
                                                 make_citow(first2),
                                                 make_citow(last2));
    }

    //! \brief Check containment of a pair of words via iterators.
    //!
    //! This function checks whether or not the words represented by the ranges
    //! \p first1 to \p last1 and \p first2 to \p last2 are contained in the
    //! congruence represented by a \ref todd_coxeter_class_group
    //! "ToddCoxeterBase" instance. This function triggers a full enumeration,
    //! which may never terminate.
    //!
    //! \cong_intf_params_contains
    //!
    //! \returns Whether or not the pair belongs to the congruence.
    //!
    //! \cong_intf_warn_undecidable{Todd-Coxeter}
    //!
    //! \cong_intf_throws_if_letters_out_of_bounds
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    bool contains(Iterator1 first1,
                  Iterator2 last1,
                  Iterator3 first2,
                  Iterator4 last2) {
      // TODO(1) remove when is_free is implemented
      if (presentation().rules.empty() && generating_pairs().empty()
          && current_word_graph().number_of_nodes_active() == 1) {
        return std::equal(first1, last1, first2, last2);
      }
      // Call CongruenceInterface version so that we perform bound checks in
      // ToddCoxeter and not ToddCoxeterBase
      return CongruenceInterface::contains<ToddCoxeter>(
          first1, last1, first2, last2);
    }

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - interface requirements - reduce
    ////////////////////////////////////////////////////////////////////////

    //! \brief Reduce a word with no enumeration or checks.
    //!
    //! This function writes a reduced word equivalent to the input word
    //! described by the iterator \p first and \p last to the output iterator \p
    //! d_first. This function triggers no enumeration. The word output by
    //! this function is equivalent to the input word in the congruence defined
    //! by a \ref todd_coxeter_class_group "ToddCoxeterBase" instance. If the
    //!  \ref todd_coxeter_class_group "ToddCoxeterBase" instance is \ref
    //!  finished, then the output word is a normal
    //! form for the input word. If the  \ref todd_coxeter_class_group
    //! "ToddCoxeterBase" instance is not \ref finished, then it might be that
    //! equivalent input words produce different output words.
    //!
    //! \cong_intf_params_reduce
    //!
    //! \returns An \p OutputIterator pointing one beyond the last letter
    //! inserted into \p d_first.
    //!
    //! \cong_intf_warn_assume_letters_in_bounds
    //!
    //! \todd_coxeter_note_reverse
    template <typename OutputIterator,
              typename InputIterator1,
              typename InputIterator2>
    OutputIterator reduce_no_run_no_checks(OutputIterator d_first,
                                           InputIterator1 first,
                                           InputIterator2 last) const {
      return ToddCoxeterBase::reduce_no_run_no_checks(
                 make_itow(d_first), make_citow(first), make_citow(last))
          .get();
    }

    //! \brief Reduce a word with no enumeration.
    //!
    //! This function writes a reduced word equivalent to the input word
    //! described by the iterator \p first and \p last to the output iterator \p
    //! d_first. This function triggers no enumeration. The word output by
    //! this function is equivalent to the input word in the congruence defined
    //! by a \ref todd_coxeter_class_group "ToddCoxeterBase" instance. If the
    //!  \ref todd_coxeter_class_group "ToddCoxeterBase" instance is \ref
    //!  finished, then the output word is a normal
    //! form for the input word. If the  \ref todd_coxeter_class_group
    //! "ToddCoxeterBase" instance is not \ref finished, then it might be that
    //! equivalent input words produce different output words.
    //!
    //! \cong_intf_params_reduce
    //!
    //! \returns An \p OutputIterator pointing one beyond the last letter
    //! inserted into \p d_first.
    //!
    //! \cong_intf_throws_if_letters_out_of_bounds
    //!
    //! \todd_coxeter_note_reverse
    template <typename OutputIterator,
              typename InputIterator1,
              typename InputIterator2>
    OutputIterator reduce_no_run(OutputIterator d_first,
                                 InputIterator1 first,
                                 InputIterator2 last) const {
      // Do not call CongruenceInterface version because we need the words to be
      // reversed (i.e. must call ToddCoxeterBase version)
      return CongruenceInterface::reduce_no_run<ToddCoxeter>(
          d_first, first, last);
    }

    //! \brief Reduce a word with no checks.
    //!
    //! This function triggers a full enumeration and then writes a reduced
    //! word equivalent to the input word described by the iterator \p first and
    //! \p last to the output iterator \p d_first. The word output by this
    //! function is equivalent to the input word in the congruence defined by a
    //! \ref todd_coxeter_class_group "ToddCoxeterBase" instance. In other
    //! words, the output word is a normal form for the input word or
    //! equivalently a canconical representative of its congruence class.
    //!
    //! \cong_intf_params_reduce
    //!
    //! \returns An \p OutputIterator pointing one beyond the last letter
    //! inserted into \p d_first.
    //!
    //! \cong_intf_warn_assume_letters_in_bounds
    //!
    //! \cong_intf_warn_undecidable{Todd-Coxeter}
    //!
    //! \todd_coxeter_note_reverse
    template <typename OutputIterator,
              typename InputIterator1,
              typename InputIterator2>
    OutputIterator reduce_no_checks(OutputIterator d_first,
                                    InputIterator1 first,
                                    InputIterator2 last) {
      return ToddCoxeterBase::reduce_no_checks(
                 make_itow(d_first), make_citow(first), make_citow(last))
          .get();
    }

    //! \brief Reduce a word.
    //!
    //! This function triggers a full enumeration and then writes a reduced
    //! word equivalent to the input word described by the iterator \p first and
    //! \p last to the output iterator \p d_first. The word output by this
    //! function is equivalent to the input word in the congruence defined by a
    //! \ref todd_coxeter_class_group "ToddCoxeterBase" instance. In other
    //! words, the output word is a normal form for the input word or
    //! equivalently a canconical representative of its congruence class.
    //!
    //! \cong_intf_params_reduce
    //!
    //! \returns An \p OutputIterator pointing one beyond the last letter
    //! inserted into \p d_first.
    //!
    //! \cong_intf_throws_if_letters_out_of_bounds
    //!
    //! \cong_intf_warn_undecidable{Todd-Coxeter}
    //!
    //! \todd_coxeter_note_reverse
    template <typename OutputIterator,
              typename InputIterator1,
              typename InputIterator2>
    OutputIterator reduce(OutputIterator d_first,
                          InputIterator1 first,
                          InputIterator2 last) {
      // Call CongruenceInterface version so that we perform bound checks in
      // ToddCoxeter and not ToddCoxeterBase
      return CongruenceInterface::reduce<ToddCoxeter>(d_first, first, last);
    }

    //! @}

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeterBase - word -> index
    ////////////////////////////////////////////////////////////////////////

    //! \defgroup todd_coxeter_class_word_index_group Word to class index
    //! \ingroup todd_coxeter_class_group
    //!
    //! \brief Member functions for converting a word into the index of a class
    //! in a \ref todd_coxeter_class_group "ToddCoxeterBase" instance.
    //!
    //! This page contains documentation for the member functions of \ref
    //! todd_coxeter_class_group "ToddCoxeterBase" that can be used to convert
    //! a word into the index of congruence class.
    //!
    //! \sa \ref todd_coxeter_class_index_word_group for the inverses of the
    //! functions described on this page.
    //!
    //! @{

    //! \brief Returns the current index of the class containing a word.
    //!
    //! This function returns the current index of the class containing the word
    //! described by the iterators \p first and \p last. No enumeration is
    //! triggered by calls to this function. Unless \ref finished returns \c
    //! true, the index returned by this function is essentially arbitrary, and
    //! can only really be used to check whether or not two words are currently
    //! known to belong to the congruence.
    //!
    //! The returned index is obtained by following the path in \ref
    //! current_word_graph from node \c 0 labelled by the word given by the
    //! arguments \p first and \p last. If there is no such path, then \ref
    //! UNDEFINED is returned.
    //!
    //! \tparam Iterator1 the type of first argument \p first.
    //! \tparam Iterator2 the type of second argument \p last.
    //!
    //! \param first iterator pointing at the first letter of the word.
    //! \param last iterator pointing one beyond the last letter of the word.
    //!
    //! \returns The current index of the class containing the word.
    //!
    //! \cong_intf_warn_assume_letters_in_bounds
    //!
    // NOTE THAT: the graph contains one more node than there are element if
    // the underlying presentation does not contain the empty word
    template <typename Iterator1, typename Iterator2>
    index_type current_index_of_no_checks(Iterator1 first,
                                          Iterator2 last) const {
      return ToddCoxeterBase::current_index_of_no_checks(make_citow(first),
                                                         make_citow(last));
    }

    //! \brief Returns the current index of the class containing a word.
    //!
    //! This function returns the current index of the class containing the word
    //! described by the iterators \p first and \p last. No enumeration is
    //! triggered by calls to this function. Unless \ref finished returns \c
    //! true, the index returned by this function is essentially arbitrary, and
    //! can only really be used to check whether or not two words are currently
    //! known to belong to the congruence.
    //!
    //! The returned index is obtained by following the path in \ref
    //! current_word_graph from node \c 0 labelled by the word given by the
    //! arguments \p first and \p last. If there is no such path, then \ref
    //! UNDEFINED is returned.
    //!
    //! \tparam Iterator1 the type of first argument \p first.
    //! \tparam Iterator2 the type of second argument \p last.
    //!
    //! \param first iterator pointing at the first letter of the word.
    //! \param last iterator pointing one beyond the last letter of the word.
    //!
    //! \returns The current index of the class containing the word.
    //!
    //! \cong_intf_throws_if_letters_out_of_bounds
    template <typename Iterator1, typename Iterator2>
    index_type current_index_of(Iterator1 first, Iterator2 last) const {
      throw_if_letter_out_of_bounds(first, last);
      return current_index_of_no_checks(first, last);
    }

    //! \brief Returns the index of the class containing a word.
    //!
    //! This function returns the index of the class containing the word
    //! described by the iterators \p first and \p last. A full enumeration is
    //! triggered by calls to this function. If the \ref current_word_graph has
    //! not already been standardized, then this function first standardizes it
    //! with respect to Order::shortlex; otherwise the existing standardization
    //! order is used.
    //!
    //! The returned index is obtained by following the path in \ref
    //! current_word_graph from node \c 0 labelled by the word given by the
    //! arguments \p first and \p last. Since a full enumeration is triggered
    //! by calls to this function, the word graph is complete, and so the return
    //! value is never \ref UNDEFINED.
    //!
    //! \tparam Iterator1 the type of first argument \p first.
    //! \tparam Iterator2 the type of second argument \p last.
    //!
    //! \param first iterator pointing at the first letter of the word.
    //! \param last iterator pointing one beyond the last letter of the word.
    //!
    //! \returns The index of the class containing the word.
    //!
    //! \cong_intf_warn_assume_letters_in_bounds
    template <typename Iterator1, typename Iterator2>
    index_type index_of_no_checks(Iterator1 first, Iterator2 last) {
      return ToddCoxeterBase::index_of_no_checks(make_citow(first),
                                                 make_citow(last));
    }

    //! \brief Returns the index of the class containing a word.
    //!
    //! This function returns the index of the class containing the word
    //! described by the iterators \p first and \p last. A full enumeration is
    //! triggered by calls to this function. If the \ref current_word_graph has
    //! not already been standardized, then this function first standardizes it
    //! with respect to Order::shortlex; otherwise the existing standardization
    //! order is used.
    //!
    //! The returned index is obtained by following the path in \ref
    //! current_word_graph from node \c 0 labelled by the word given by the
    //! arguments \p first and \p last. Since a full enumeration is triggered
    //! by calls to this function, the word graph is complete, and so the return
    //! value is never \ref UNDEFINED.
    //!
    //! \tparam Iterator1 the type of first argument \p first.
    //! \tparam Iterator2 the type of second argument \p last.
    //!
    //! \param first iterator pointing at the first letter of the word.
    //! \param last iterator pointing one beyond the last letter of the word.
    //!
    //! \returns The index of the class containing the word.
    //!
    //! \cong_intf_throws_if_letters_out_of_bounds
    template <typename Iterator1, typename Iterator2>
    index_type index_of(Iterator1 first, Iterator2 last) {
      throw_if_letter_out_of_bounds(first, last);
      return index_of_no_checks(first, last);
    }

    //! @}

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - index -> word
    ////////////////////////////////////////////////////////////////////////

    //! \defgroup todd_coxeter_class_index_word_group Class index to word
    //! \ingroup todd_coxeter_class_group
    //!
    //! \brief Member functions for converting the index of a class
    //! into a word in a \ref todd_coxeter_class_group "ToddCoxeterBase"
    //! instance.
    //!
    //! This page contains documentation for the member functions of
    //!  \ref todd_coxeter_class_group "ToddCoxeterBase" that can be used to
    //!  convert the index of a congruence class
    //! to a representative word belonging to that congruence class.
    //!
    //! \sa \ref todd_coxeter_class_word_index_group for the inverses of the
    //! functions described on this page.
    //!
    //! @{

    //! \brief Insert a current word representing a class with given index into
    //! an output iterator.
    //!
    //! This function appends a current word representing the class with index
    //! \p i to the output iterator \p d_first. No enumeration is triggered by
    //! calls to this function, but \ref current_word_graph is standardized
    //! (using Order::shortlex) if it is not already standardized.
    //!
    //! The word appended to \p d_first is obtained by following a path in \ref
    //! current_spanning_tree from the node corresponding to index \c i back to
    //! the root of that tree.
    //!
    //! \tparam OutputIterator the type of the first argument.
    //!
    //! \param d_first output iterator pointing at the first letter of the
    //! destination.
    //! \param i the index of the class.
    //!
    //! \returns An output iterator pointing one beyond the last letter of the
    //! output word.
    //!
    //! \warning This function does not check its arguments. In particular, it
    //! is assumed that \p i is a valid index of a current class.
    //!
    //! \todd_coxeter_note_reverse
    // NOTE THAT: the graph contains one more node than there are element if
    // the underlying presentation does not contain the empty word
    template <typename OutputIterator>
    OutputIterator current_word_of_no_checks(OutputIterator d_first,
                                             index_type     i) const {
      return ToddCoxeterBase::current_word_of_no_checks(make_itow(d_first), i)
          .get();
    }

    //! \brief Insert a current word representing a class with given index into
    //! an output iterator.
    //!
    //! This function appends a current word representing the class with index
    //! \p i to the output iterator \p d_first. No enumeration is triggered by
    //! calls to this function, but \ref current_word_graph is standardized
    //! (using Order::shortlex) if it is not already standardized.
    //!
    //! The word appended to \p d_first is obtained by following a path in \ref
    //! current_spanning_tree from the node corresponding to index \c i back to
    //! the root of that tree.
    //!
    //! \tparam OutputIterator the type of the first argument.
    //!
    //! \param d_first output iterator pointing at the first letter of the
    //! destination.
    //! \param i the index of the class.
    //!
    //! \returns An output iterator pointing one beyond the last letter of the
    //! output word.
    //!
    //! \throws LibsemigroupsException if \p i is out of bounds.
    //!
    //! \todd_coxeter_note_reverse
    template <typename OutputIterator>
    OutputIterator current_word_of(OutputIterator d_first, index_type i) const {
      return ToddCoxeterBase::current_word_of(make_itow(d_first), i).get();
    }

    //! \brief Insert the word representing a class with given index into
    //! an output iterator.
    //!
    //! This function appends the word representing the class with index
    //! \p i to the output iterator \p d_first. A full enumeration is triggered
    //! by calls to this function, and \ref current_word_graph is standardized.
    //!
    //! The word appended to \p d_first is obtained by following a path in \ref
    //! current_spanning_tree from the node corresponding to index \c i back to
    //! the root of that tree.
    //!
    //! \tparam OutputIterator the type of the first argument.
    //!
    //! \param d_first output iterator pointing at the first letter of the
    //! destination.
    //! \param i the index of the class.
    //!
    //! \returns An output iterator pointing one beyond the last letter of the
    //! output word.
    //!
    //! \throws LibsemigroupsException if \p i is out of bounds.
    //!
    //! \todd_coxeter_note_reverse
    template <typename Iterator>
    Iterator word_of_no_checks(Iterator d_first, index_type i) {
      return ToddCoxeterBase::word_of_no_checks(make_itow(d_first), i).get();
    }

    //! \brief Insert the word representing a class with given index into
    //! an output iterator.
    //!
    //! This function appends the word representing the class with index
    //! \p i to the output iterator \p d_first. A full enumeration is triggered
    //! by calls to this function, and \ref current_word_graph is standardized.
    //!
    //! The word appended to \p d_first is obtained by following a path in \ref
    //! current_spanning_tree from the node corresponding to index \c i back to
    //! the root of that tree.
    //!
    //! \tparam OutputIterator the type of the first argument.
    //!
    //! \param d_first output iterator pointing at the first letter of the
    //! destination.
    //! \param i the index of the class.
    //!
    //! \returns An output iterator pointing one beyond the last letter of the
    //! output word.
    //!
    //! \warning This function does not check its arguments. In particular, it
    //! is assumed that \p i is a valid index of a current class.
    //!
    //! \todd_coxeter_note_reverse
    template <typename Iterator>
    Iterator word_of(Iterator d_first, index_type i) {
      return ToddCoxeterBase::word_of(make_itow(d_first), i).get();
    }

    //! @}
  };

  template <typename Word>
  ToddCoxeter(congruence_kind, Presentation<Word> const&) -> ToddCoxeter<Word>;

  template <typename Word>
  ToddCoxeter(congruence_kind, Presentation<Word>&&) -> ToddCoxeter<Word>;

  template <typename Word>
  ToddCoxeter(congruence_kind, ToddCoxeter<Word> const&) -> ToddCoxeter<Word>;

  template <typename Node>
  ToddCoxeter(congruence_kind, WordGraph<Node> const&)
      -> ToddCoxeter<word_type>;

  template <typename Node>
  ToddCoxeter(congruence_kind, WordGraph<Node>&&) -> ToddCoxeter<word_type>;

  template <typename Word>
  std::string to_human_readable_repr(ToddCoxeter<Word> const& tc) {
    return fmt::format("<ToddCoxeter over {} with {}/{} active/nodes>",
                       to_human_readable_repr(tc.presentation()),
                       tc.current_word_graph().number_of_nodes_active(),
                       tc.current_word_graph().number_of_nodes());
  }
}  // namespace libsemigroups

#include "todd-coxeter-class.tpp"
#endif  // LIBSEMIGROUPS_TODD_COXETER_CLASS_HPP_
