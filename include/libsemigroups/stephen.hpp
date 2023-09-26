//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022-2023 James D. Mitchell
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
#include <type_traits>  // for decay_t, is_base_of_v
#include <utility>      // for forward
#include <vector>       // for vector

#include "constants.hpp"                // for PositiveInfinity
#include "paths.hpp"                    // for const_pislo_iterator etc
#include "presentation.hpp"             // for Presentation
#include "runner.hpp"                   // for Runner
#include "to-presentation.hpp"          // for make
#include "types.hpp"                    // for word_type
#include "word-graph-with-sources.hpp"  // for DigraphWithSources
#include "word-graph.hpp"               // for WordGraph, Act...

#include "detail/dot.hpp"                 // for Dot
#include "detail/int-range.hpp"           // for IntegralRange<>::v...
#include "detail/node-managed-graph.hpp"  // for NodeManagedGraph
#include "detail/stl.hpp"                 // for IsStdSharedPtr

// TODO
// * iwyu
// * minimal rep (as per Reinis) (named normal_form?)
// * class_of for inverse Stephen (i.e. all walks in the graph through all
// nodes) (not sure how to do this just yet). This is different than
// words_accepted see Corollary 3.2 in Stephen's "Presentations of inverse
// monoids" paper (not thesis).
// * invert() - just swap the initial and accept states and re-standardize
// * idempotent() - just make the accept state = initial state.
// * update reporting to new standard
// * update so that run_for, run_until work properly (at present basically
//   run_impl starts again from scratch every time)
// * canonical_form (as per Howie's book)
namespace libsemigroups {

  //! Defined in ``stephen.hpp``.
  //!
  //! On this page we describe the functionality in ``libsemigroups`` relating
  //! to Stephen's procedure for finitely presented semigroups. This class
  //! implements Stephen's procedure for (possibly) constructing the word graph
  //! (WordGraph) corresponding to the left factors of a word in a finitely
  //! presented semigroup. The algorithm implemented in this class is closely
  //! related to the Todd-Coxeter algorithm  (as implemented in \ref
  //! ToddCoxeter) and originates in [Applications of automata theory to
  //! presentations of monoids and inverse monoids](https://rb.gy/brsuvc) by J.
  //! B. Stephen.
  template <typename P>
  class Stephen : public Runner {
    template <typename T>
    struct ActualPresentation {
      using type = T;
    };

    template <>
    struct ActualPresentation<std::shared_ptr<Presentation<word_type>>> {
      using type = Presentation<word_type>;
    };

    template <>
    struct ActualPresentation<std::shared_ptr<InversePresentation<word_type>>> {
      using type = InversePresentation<word_type>;
    };

    template <typename Q>
    static constexpr bool is_valid_presentation() {
      using R = typename ActualPresentation<std::decay_t<Q>>::type;
      return std::is_same_v<R, Presentation<word_type>>
             || std::is_same_v<R, InversePresentation<word_type>>;
    }

    static_assert(is_valid_presentation<P>(), "TODO");

   public:
    using presentation_type = typename ActualPresentation<P>::type;

    //! The return type of the function \ref word_graph.
    using word_graph_type = WordGraph<uint32_t>;
    using node_type       = word_graph_type::node_type;

   private:
    class StephenGraph;  // forward decl

    // Data members
    node_type    _accept_state;
    bool         _something_changed;
    bool         _finished;
    P            _presentation;
    word_type    _word;
    StephenGraph _word_graph;

   public:
    //! Default constructor.
    //!
    //! Default constructs an empty instance, use \ref init and \ref set_word
    //! to specify the presentation and the word, respectively.
    Stephen();
    Stephen& init();

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
    Stephen(P&& p);
    Stephen& init(P&& p);

    //! Initialize from a presentation.
    //!
    //! Replaces the current value (if any) returned by \ref presentation by
    //! the argument, and the state of the object is the same as if it had
    //! been newly constructed from the presentation \p p.
    //!
    //! \tparam P a type derived from PresentationBase
    //!
    //! \param p the presentation.
    //!
    //! \returns A reference to \c this.
    //!
    //! \throws LibsemigroupsException if `p.validate()` throws.
    //! \throws LibsemigroupsException if `p.alphabet().size()` is `0`.
    Stephen(P const& p);
    Stephen& init(P const& p);

    //! Default copy constructor
    Stephen(Stephen const& that) = default;

    //! Default move constructor
    Stephen(Stephen&&) = default;

    //! Default copy assignment operator
    Stephen& operator=(Stephen const&) = default;

    //! Default move assignment operator
    Stephen& operator=(Stephen&&) = default;

    ~Stephen() = default;

    template <typename Q>
    Stephen(Q const& q) : Stephen() {
      init(q);
    }

    // TODO to tpp
    template <typename Q>
    Stephen& init(Q const& q) {
      static_assert(((IsInversePresentation<P>) == (IsInversePresentation<Q>) )
                        && IsPresentation<P> && IsPresentation<Q>,
                    "TODO");
      if constexpr (IsInversePresentation<P> && IsInversePresentation<Q>) {
        return init(to_inverse_presentation<word_type>(q));
      } else {
        return init(to_presentation<word_type>(q));
      }
    }

    // TODO make private and hid in tpp file
    template <typename PP>
    static constexpr auto& deref_if_necessary(PP&& p) {
      if constexpr (detail::IsStdSharedPtr<std::decay_t<PP>>) {
        return *p;
      } else {
        return p;
      }
    }

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
    presentation_type const& presentation() const noexcept {
      return deref_if_necessary(_presentation);
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
    //! \returns A const reference to a \ref word_graph_type.
    //!
    //! \exceptions
    //! \noexcept
    // TODO add a warning that if the value of word is set, then run is called,
    // then word is set to another value, then word_graph() is accessed, then
    // the returned value doesn't relate to the currently set value. Or better
    // still don't have this behaviour
    word_graph_type const& word_graph() const noexcept {
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
    node_type                  accept_state();
    static constexpr node_type initial_state() {
      return 0;
    }

    void operator*=(Stephen<P>& y) {
      // TODO if one of this and that is finished, then just tack on the linear
      // graph.
      this->run();
      y.run();
      // FIXME _word_graph has two mem fns number_nodes_active (in NodeManager)
      // and number_active_nodes (in WordGraph), this is super confusing!
      size_t const N = _word_graph.number_of_nodes_active();
      _word_graph.disjoint_union_inplace(y._word_graph);
      _word_graph.merge_nodes_no_checks(accept_state(), y.initial_state() + N);
      _word_graph.template process_coincidences<DoNotRegisterDefs>();
      _accept_state = UNDEFINED;
      _finished     = false;
      _word.insert(_word.end(), y._word.cbegin(), y._word.cend());
      _word_graph.cursor() = initial_state();
    }

   private:
    Stephen& init_after_presentation_set();
    void     throw_if_presentation_empty(presentation_type const&) const;
    void     something_changed() noexcept;

    void run_impl() override;

    bool finished_impl() const noexcept override {
      return _finished;
    }

    void standardize();

    void report_status(
        std::chrono::high_resolution_clock::time_point const& start_time);
  };

  // Deduction guides
  // The following is not a mistake but intentional, if no presentation type is
  // explicitly used, then we use Presentation<word_type>. The only other
  // alternative is to use a std::shared_ptr<Presentation<word_type>> or
  // std::shared_ptr<InversePresentation<word_type>>;
  // Presentation<std::string> is not allowed.
  template <typename Word>
  Stephen(Presentation<Word> const&) -> Stephen<Presentation<word_type>>;

  template <typename Word>
  Stephen(Presentation<Word>&) -> Stephen<Presentation<word_type>>;

  template <typename Word>
  Stephen(Presentation<Word>&&) -> Stephen<Presentation<word_type>>;

  template <typename Word>
  Stephen(InversePresentation<Word> const&)
      -> Stephen<InversePresentation<word_type>>;

  template <typename Word>
  Stephen(InversePresentation<Word>&)
      -> Stephen<InversePresentation<word_type>>;

  template <typename Word>
  Stephen(InversePresentation<Word>&&)
      -> Stephen<InversePresentation<word_type>>;

  template <typename Word>
  Stephen(std::shared_ptr<InversePresentation<Word>>&&)
      -> Stephen<std::shared_ptr<InversePresentation<word_type>>>;
  // TODO other shared_prt guides?

}  // namespace libsemigroups
#include "stephen.tpp"

namespace libsemigroups {
  namespace stephen {
    using const_iterator_words_accepted = const_pstislo_iterator<uint32_t>;

    using const_iterator_left_factors = const_pislo_iterator<uint32_t>;

    template <typename PresentationType>
    bool accepts(Stephen<PresentationType>& s, word_type const& w);

    template <typename PresentationType>
    bool is_left_factor(Stephen<PresentationType>& s, word_type const& w);

    // TODO left_factors range object
    // TODO words_accepted range object

    // TODO to tpp
    template <typename PresentationType>
    const_iterator_words_accepted
    cbegin_words_accepted(Stephen<PresentationType>& s,
                          size_t                     min = 0,
                          size_t                     max = POSITIVE_INFINITY) {
      s.run();
      return cbegin_pstislo(
          s.word_graph(), s.initial_state(), s.accept_state(), min, max);
    }

    template <typename PresentationType>
    const_iterator_words_accepted
    cend_words_accepted(Stephen<PresentationType>& s) {
      s.run();
      return cend_pstislo(s.word_graph());
    }

    template <typename PresentationType>
    auto words_accepted(Stephen<PresentationType>& s) {
      Paths paths(s.word_graph());
      return paths.from(s.initial_state()).to(s.accept_state());
    }

    template <typename PresentationType>
    const_iterator_left_factors
    cbegin_left_factors(Stephen<PresentationType>& s,
                        size_t                     min = 0,
                        size_t                     max = POSITIVE_INFINITY) {
      s.run();
      return cbegin_pislo(s.word_graph(), 0, min, max);
    }

    template <typename PresentationType>
    const_iterator_left_factors
    cend_left_factors(Stephen<PresentationType>& s) {
      s.run();
      return cend_pislo(s.word_graph());
    }

    template <typename PresentationType>
    auto left_factors(Stephen<PresentationType>& s) {
      Paths paths(s.word_graph());
      return paths.from(s.initial_state());
    }

    // TODO to tpp
    template <typename PresentationType>
    uint64_t number_of_words_accepted(Stephen<PresentationType>& s,
                                      size_t                     min = 0,
                                      size_t max = POSITIVE_INFINITY) {
      s.run();
      using node_type = typename Stephen<PresentationType>::node_type;
      return number_of_paths(
          s.word_graph(), node_type(0), s.accept_state(), min, max);
    }

    template <typename PresentationType>
    uint64_t number_of_left_factors(Stephen<PresentationType>& s,
                                    size_t                     min = 0,
                                    size_t max = POSITIVE_INFINITY) {
      s.run();
      return number_of_paths(s.word_graph(), 0, min, max);
    }

    // TODO to tpp
    template <typename P>
    Dot dot(Stephen<P>& s) {
      if constexpr (IsInversePresentation<P>) {
        Dot result;
        result.kind(Dot::Kind::digraph);
        result.add_node("initial").add_node_attr("initial", "style", "invis");
        result.add_node("accept").add_node_attr("accept", "style", "invis");
        for (auto n : s.word_graph().nodes()) {
          result.add_node(n);
          result.add_node_attr(n, "shape", "box");
        }
        result.add_edge("initial", std::to_string(s.initial_state()));
        result.add_edge(std::to_string(s.accept_state()), "accept");

        for (auto n : s.word_graph().nodes()) {
          for (size_t a = 0; a < s.presentation().alphabet().size() / 2; ++a) {
            auto m = s.word_graph().target(n, a);
            if (m != UNDEFINED) {
              result.add_edge(n, m)
                  .add_edge_attr(n, m, "color", result.colors[a])
                  .add_edge_attr(n, m, "label", std::to_string(a))
                  .add_edge_attr(n, m, "minlen", std::to_string(2));
            }
          }
        }
        return result;
      } else {
      }
    }

  }  // namespace stephen

  template <typename PresentationType>
  bool operator==(Stephen<PresentationType> const& x,
                  Stephen<PresentationType> const& y) {
    return stephen::accepts(const_cast<Stephen<PresentationType>&>(x), y.word())
           && stephen::accepts(const_cast<Stephen<PresentationType>&>(y),
                               x.word());
  }

  // TODO to tpp
  template <typename PresentationType>
  std::ostream& operator<<(std::ostream&                    os,
                           Stephen<PresentationType> const& x) {
    std::string word;
    // if (x.word().size() < 10) {
    //   word = detail::to_string(x.word());
    // } else {
    word = " " + std::to_string(x.word().size()) + " letter word, ";
    //}
    // TODO use fmt
    os << std::string("<Stephen for ") << word << " with "
       << x.word_graph().number_of_nodes() << "  nodes, "
       << x.word_graph().number_of_edges() << " edges>";
    return os;
  }

  // TODO reuse the doc from here to end of file
  //! The return type of \ref cbegin_words_accepted and \ref
  //! cend_words_accepted. This is the same as
  //! \ref WordGraph::const_pstislo_iterator.

  //! The return type of \ref cbegin_left_factors and \ref
  //! cend_left_factors. This is the same as \ref
  //! WordGraph::const_pislo_iterator.

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

  //! Check if a word is a left factor of Stephen::word.
  //!
  //! This function triggers the algorithm implemented in this class (if it
  //! hasn't been triggered already), and then returns \c true if the input
  //! word \p w is a left factor of Stephen::word in the semigroup defined
  //! by Stephen::presentation. A word is a left factor of Stephen::word
  //! if it labels a path in Stephen::word_graph with source \c 0.
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
  //! \sa WordGraph::cbegin_pstislo for more information about the
  //! iterators returned by this function.
  //!
  //! Returns an iterator pointing one past the last word equivalent to
  //! Stephen::word.
  //!
  //! \sa \ref cbegin_words_accepted for more information.
  // Not noexcept because cend_pstislo isn't

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
  //! \sa WordGraph::cbegin_pislo for more information about the
  //! iterators returned by this function.
  // Not noexcept because cbegin_pislo isn't

  //! Returns an iterator pointing one past the last word that is a left
  //! factor of Stephen::word.
  //!
  //! \sa \ref cbegin_left_factors for more information.
  // Not noexcept because cend_pislo isn't

  //! Returns the number of words accepted with length in a given range.
  //!
  //! This function returns the number of words that are equivalent to
  //! Stephen::word in the instance \p s with length between \p min and \p
  //! max. This is the same as the number of paths in Stephen::word_graph
  //! (if Stephen::run has been called) with source \c 0, target
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
  //! \sa WordGraph::number_of_paths.
  // Not noexcept because number_of_paths isn't

  //! Returns the number of left factors with length in a given range.
  //!
  //! This function returns the number of left factors of the
  //! Stephen::word in the instance \p s with length between \p min and \p
  //! max. This is the same as the number of paths in Stephen::word_graph
  //! (if Stephen::run has been called) with source \c 0 and length in the
  //! range \p min to \p max.
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
  //! \sa WordGraph::number_of_paths.
  //!
  //! \throws LibsemigroupsException if no presentation was set at
  //! construction or with Stephen::init.
  // Number of words that represent left factors of word()
  // Not noexcept because number_of_paths isn't

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_STEPHEN_HPP_
