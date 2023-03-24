//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022 James D. Mitchell
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

#ifndef LIBSEMIGROUPS_STEPHEN_HPP_
#define LIBSEMIGROUPS_STEPHEN_HPP_

#include <chrono>       // for high_resolution_clock
#include <cstddef>      // for size_t
#include <cstdint>      // for size_t
#include <type_traits>  // for decay_t, is_base_of
#include <utility>      // for forward
#include <vector>       // for vector

#include "constants.hpp"             // for PositiveInfinity
#include "digraph-with-sources.hpp"  // for DigraphWithSources
#include "digraph.hpp"               // for ActionDigraph, Act...
#include "make-present.hpp"          // for make
#include "paths.hpp"                 // for Paths
#include "present.hpp"               // for Presentation
#include "runner.hpp"                // for Runner
#include "todd-coxeter-digraph.hpp"  // for StephenDigraph
#include "types.hpp"                 // for word_type

namespace libsemigroups {

  //! Defined in ``stephen.hpp``.
  //!
  //! On this page we describe the functionality in ``libsemigroups`` relating
  //! to Stephen's procedure for finitely presented semigroups. This class
  //! implements Stephen's procedure for (possibly) constructing the word graph
  //! (ActionDigraph) corresponding to the left factors of a word in a finitely
  //! presented semigroup. The algorithm implemented in this class is closely
  //! related to the Todd-Coxeter algorithm  (as implemented in \ref
  //! congruence::ToddCoxeter) and originates in
  //! [Applications of automata theory to presentations of monoids and inverse
  //! monoids](https://rb.gy/brsuvc) by J. B. Stephen.
  class Stephen : public Runner {
    using internal_digraph_type = StephenDigraph;

   public:
    //! The return type of the function \ref word_graph.
    using digraph_type = ActionDigraph<size_t>;

    //! The type of the nodes of a \ref digraph_type.
    using node_type = typename digraph_type::node_type;

   private:
    // Data members
    bool                    _finished;
    node_type               _accept_state;
    Presentation<word_type> _presentation;
    word_type               _word;
    internal_digraph_type   _word_graph;

   public:
    //! Default constructor.
    //!
    //! Default constructs an empty instance, use \ref init and \ref set_word
    //! to specify the presentation and the word, respectively.
    Stephen();

    //! Construct from a presentation.
    //!
    //! Construct an instance for the presentation \p p.
    //!
    //! \tparam P a type derived from PresentationBase
    //!
    //! \param p the presentation.
    //!
    //! \throws LibsemigroupsException if `p.validate()` throws.
    //! \throws LibsemigroupsException if `p.alphabet().size()` is `0`.
    template <typename P,
              typename = std::enable_if_t<
                  std::is_base_of<PresentationBase, std::decay_t<P>>::value>>
    explicit Stephen(P&& p);

    //! Default copy constructor
    Stephen(Stephen const&) = default;

    //! Default move constructor
    Stephen(Stephen&&) = default;

    //! Default copy assignment operator
    Stephen& operator=(Stephen const&) = default;

    //! Default move assignment operator
    Stephen& operator=(Stephen&&) = default;

    ~Stephen() = default;

    //! Initialize from a presentation.
    //!
    //! Replaces the current value (if any) returned by \ref presentation by
    //! the argument, and the state of the object is the same as if it had been
    //! newly constructed from the presentation \p p.
    //!
    //! \tparam P a type derived from PresentationBase
    //!
    //! \param p the presentation.
    //!
    //! \returns A reference to \c this.
    //!
    //! \throws LibsemigroupsException if `p.validate()` throws.
    //! \throws LibsemigroupsException if `p.alphabet().size()` is `0`.
    template <typename P>
    Stephen& init(P&& p);

    //! The input presentation.
    //!
    //! Returns a const reference to the input presentation.
    //!
    //! \param (None)
    //!
    //! \returns A const reference to a Presentation<word_type>.
    //!
    //! \exceptions
    //! \noexcept
    Presentation<word_type> const& presentation() const noexcept {
      return _presentation;
    }

    //! Set the word.
    //!
    //! This function can be used to set the word whose left factors, or
    //! equivalent words, are sought. The input word is copied.
    //!
    //! \param w a const reference to the input word.
    //!
    //! \returns A reference to \c this.
    //!
    //! \throws LibsemigroupsException if the letters in \p do not all belong
    //! to the alphabet of the \ref presentation.
    Stephen& set_word(word_type const& w);

    //! Set the word.
    //!
    //! This function can be used to set the word whose left factors, or
    //! equivalent words, are sought.
    //!
    //! \param w an rvalue reference to the input word.
    //!
    //! \returns A reference to \c this.
    //!
    //! \throws LibsemigroupsException if the letters in \p do not all belong
    //! to the alphabet of the \ref presentation.
    Stephen& set_word(word_type&& w);

    //! The word.
    //!
    //! Returns a const reference to the word set by \ref set_word.
    //!
    //! \param (None)
    //!
    //! \returns A const reference to a \ref word_type.
    //!
    //! \exceptions
    //! \noexcept
    word_type const& word() const noexcept {
      return _word;
    }

    //! The word graph.
    //!
    //! Returns a const reference to the word graph in its present state. The
    //! algorithm implemented in this class is not triggered by calls to this
    //! function.
    //!
    //! \param (None)
    //!
    //! \returns A const reference to a \ref digraph_type.
    //!
    //! \exceptions
    //! \noexcept
    digraph_type const& word_graph() const noexcept {
      return _word_graph;
    }

    //! The accept state of the word graph.
    //!
    //! This function triggers the algorithm implemented in this class (if it
    //! hasn't been triggered already), and then returns the accept state of
    //! the produced word graph.
    //!
    //! \param (None)
    //!
    //! \returns A \ref node_type.
    //!
    //! \throws LibsemigroupsException if no presentation was set at
    //! construction or with \ref init.
    //!
    //! \warning The problem of determining whether two words are equal in
    //! a finitely presented semigroup is undecidable in general, and this
    //! function may never terminate.
    // Throws if run throws, also this is not in the helper namespace because
    // we cache the return value.
    node_type accept_state();

   private:
    using lvalue_tag     = std::true_type;
    using non_lvalue_tag = std::false_type;

    bool finished_impl() const noexcept {
      return _finished;
    }

    template <typename P>
    void init_impl(P&&, lvalue_tag);
    void init_impl(Presentation<word_type>&&, non_lvalue_tag);

    void report_status(
        std::chrono::high_resolution_clock::time_point const& start_time);
    void reset() noexcept;
    void run_impl();
    void standardize();
    void validate() const;
  };

  template <typename P, typename SFINAE>
  Stephen::Stephen(P&& p) : Stephen() {
    init_impl(std::forward<P>(p), std::is_lvalue_reference<P>());
  }

  template <typename P>
  void Stephen::init_impl(P&& p, lvalue_tag) {
    static_assert(std::is_base_of<PresentationBase, std::decay_t<P>>::value,
                  "the template parameter P must be derived from "
                  "PresentationBase");
    init(std::move(make<Presentation<word_type>>(p)));
  }

  template <typename P>
  Stephen& Stephen::init(P&& p) {
    p.validate();
    init_impl(std::forward<P>(p), std::is_lvalue_reference<P>());
    return *this;
  }

  namespace stephen {
    using node_type = Stephen::node_type;

    //! The return type of \ref cbegin_words_accepted and \ref
    //! cend_words_accepted. This is the same as
    //! \ref ActionDigraph::const_pstislo_iterator.
    using const_iterator_words_accepted = const_pstislo_iterator<node_type>;

    //! The return type of \ref cbegin_left_factors and \ref cend_left_factors.
    //! This is the same as \ref ActionDigraph::const_pislo_iterator.
    using const_iterator_left_factors = const_pislo_iterator<node_type>;

    //! Check if a word is equivalent to Stephen::word.
    //!
    //! This function triggers the algorithm implemented in this class (if it
    //! hasn't been triggered already), and then returns \c true if the input
    //! word \p w is equivalent to Stephen::word in the semigroup defined by
    //! Stephen::presentation. A word is equivalent to Stephen::word if it
    //! labels a path in Stephen::word_graph with source \c 0 and target
    //! Stephen::accept_state.
    //!
    //! \param s the Stephen instance
    //! \param w a const reference to the input word.
    //!
    //! \returns A \c bool.
    //!
    //! \throws LibsemigroupsException if no presentation was set at
    //! the construction of \p s or with Stephen::init.
    //!
    //! \warning The problem of determining whether two words are equal in
    //! a finitely presented semigroup is undecidable in general, and this
    //! function may never terminate.
    bool accepts(Stephen& s, word_type const& w);

    //! Check if a word is a left factor of Stephen::word.
    //!
    //! This function triggers the algorithm implemented in this class (if it
    //! hasn't been triggered already), and then returns \c true if the input
    //! word \p w is a left factor of Stephen::word in the semigroup defined by
    //! Stephen::presentation. A word is a left factor of Stephen::word if it
    //! labels a path in Stephen::word_graph with source \c 0.
    //!
    //! \param s the Stephen instance
    //! \param w a const reference to the input word.
    //!
    //! \returns A \c bool.
    //!
    //! \throws LibsemigroupsException if no presentation was set at
    //! the construction of \p s or with Stephen::init.
    //!
    //! \warning The problem of determining whether a word is a left factor of
    //! another word in a finitely presented semigroup is undecidable in
    //! general, and this function may never terminate.
    bool is_left_factor(Stephen& s, word_type const& w);

    //! Returns an iterator pointing at the first word equivalent to
    //! Stephen::word in short-lex order.
    //!
    //! This function triggers the algorithm implemented in this class (if it
    //! hasn't been triggered already).
    //!
    //! \param s the Stephen instance
    //! \param min the minimum length of an equivalent word (default: 0)
    //! \param max the maximum length of an equivalent word (default:
    //! POSITIVE_INFINITY)
    //!
    //! \returns A \c const_iterator.
    //!
    //! \throws LibsemigroupsException if no presentation was set at
    //! the construction of \p s or with Stephen::init.
    //!
    //! \warning The problem of determining whether two words are equal in
    //! a finitely presented semigroup is undecidable in general, and this
    //! function may never terminate.
    //!
    //! \sa ActionDigraph::cbegin_pstislo for more information about the
    //! iterators returned by this function.
    const_iterator_words_accepted cbegin_words_accepted(Stephen& s,
                                                        size_t   min = 0,
                                                        size_t   max
                                                        = POSITIVE_INFINITY) {
      s.run();
      return cbegin_pstislo(s.word_graph(),
                            static_cast<node_type>(0),
                            s.accept_state(),
                            min,
                            max);
    }

    //! Returns an iterator pointing one past the last word equivalent to
    //! Stephen::word.
    //!
    //! \sa \ref cbegin_words_accepted for more information.
    // Not noexcept because cend_pstislo isn't
    const_iterator_words_accepted cend_words_accepted(Stephen& s) {
      s.run();
      return cend_pstislo(s.word_graph());
    }

    //! Returns an iterator pointing at the first word (in short-lex order)
    //! that is a left factor of Stephen::word.
    //!
    //! This function triggers the algorithm implemented in this class (if it
    //! hasn't been triggered already).
    //!
    //! \param s the Stephen instance
    //! \param min the minimum length of an equivalent word (default: 0)
    //! \param max the maximum length of an equivalent word (default:
    //! POSITIVE_INFINITY)
    //!
    //! \returns A \c const_iterator_left_factors.
    //!
    //! \throws LibsemigroupsException if no presentation was set at
    //! the construction of \p s or with Stephen::init.
    //!
    //! \warning The problem of determining whether a word is a left factor of
    //! another word in a finitely presented semigroup is undecidable in
    //! general, and this function may never terminate.
    //!
    //! \sa ActionDigraph::cbegin_pislo for more information about the
    //! iterators returned by this function.
    // Not noexcept because cbegin_pislo isn't
    const_iterator_left_factors cbegin_left_factors(Stephen& s,
                                                    size_t   min = 0,
                                                    size_t   max
                                                    = POSITIVE_INFINITY) {
      using node_type =
          typename std::decay_t<decltype(s.word_graph())>::node_type;
      s.run();
      return cbegin_pislo(s.word_graph(), node_type(0), min, max);
    }

    //! Returns an iterator pointing one past the last word that is a left
    //! factor of Stephen::word.
    //!
    //! \sa \ref cbegin_left_factors for more information.
    // Not noexcept because cend_pislo isn't
    const_iterator_left_factors cend_left_factors(Stephen& s) {
      s.run();
      return cend_pislo(s.word_graph());
    }

    //! Returns the number of words accepted with length in a given range.
    //!
    //! This function returns the number of words that are equivalent to
    //! Stephen::word in the instance \p s with length between \p min and \p
    //! max. This is the same as the number of paths in Stephen::word_graph (if
    //! Stephen::run has been called) with source \c 0, target
    //! Stephen::accept_state,  and length in the range \p min to \p max.
    //!
    //! \param s the Stephen instance.
    //! \param min the minimum length of a word (default: 0).
    //! \param max one more than the maximum length of a word (default:
    //! POSITIVE_INFINITY).
    //!
    //! \returns A \c uint64_t.
    //!
    //! \throws LibsemigroupsException if no presentation was set at
    //! the construction of \p s or with Stephen::init.
    //!
    //!
    //! \sa ActionDigraph::number_of_paths.
    // Not noexcept because number_of_paths isn't
    uint64_t number_of_words_accepted(Stephen& s,
                                      size_t   min = 0,
                                      size_t   max = POSITIVE_INFINITY) {
      s.run();
      return number_of_paths(s.word_graph(),
                             static_cast<node_type>(0),
                             s.accept_state(),
                             min,
                             max);
    }

    //! Returns the number of left factors with length in a given range.
    //!
    //! This function returns the number of left factors of the Stephen::word
    //! in the instance \p s with length between \p min and \p max. This is the
    //! same as the number of paths in Stephen::word_graph (if Stephen::run has
    //! been called) with source \c 0 and length in the range \p min to \p max.
    //!
    //! \param s the Stephen instance.
    //! \param min the minimum length of a word (default: 0).
    //! \param max one more than the maximum length of a word (default:
    //! POSITIVE_INFINITY).
    //!
    //! \returns A \c uint64_t.
    //!
    //! \throws LibsemigroupsException if no presentation was set at
    //! the construction of \p s or with Stephen::init.
    //!
    //! \sa ActionDigraph::number_of_paths.
    //!
    //! \throws LibsemigroupsException if no presentation was set at
    //! construction or with Stephen::init.
    // Number of words that represent left factors of word()
    // Not noexcept because number_of_paths isn't
    uint64_t number_of_left_factors(Stephen& s,
                                    size_t   min = 0,
                                    size_t   max = POSITIVE_INFINITY) {
      s.run();
      return number_of_paths(s.word_graph(), 0, min, max);
    }
  }  // namespace stephen
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_STEPHEN_HPP_
