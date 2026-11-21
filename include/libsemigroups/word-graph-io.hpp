//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2026 James D. Mitchell
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

// KBMAG serialization of word graphs.

#ifndef LIBSEMIGROUPS_WORD_GRAPH_IO_HPP_
#define LIBSEMIGROUPS_WORD_GRAPH_IO_HPP_

#include <iosfwd>       // for istream, ostream
#include <string>       // for string
#include <type_traits>  // for enable_if_t

#include "is_specialization_of.hpp"  // for is_specialization_of_v
#include "word-graph-class.hpp"      // for WordGraph

namespace libsemigroups {
  namespace word_graph {
    //! \ingroup word_graph_group
    //! \brief The representation of a KBMAG transition table.
    enum class kbmagformat {
      dense,  //!< One target per label, with zero for undefined transitions.
      sparse  //!< A list of (label, target) pairs for each source node.
    };

    //! \ingroup word_graph_group
    //! \brief Writes a word graph as a KBMAG finite-state automaton record.
    //!
    //! Writes a GAP assignment `word_graph := rec(...);`. States and alphabet
    //! letters have type `"simple"`. Node and label numbers are increased by
    //! one; dense tables use zero for undefined transitions, and sparse tables
    //! omit them. The alphabet size records the out-degree even when some
    //! labels have no transitions. Different labels with the same target are
    //! preserved. The initial list is `[1]`, or `[]` for a graph with no nodes,
    //! and the accepting list is always `[]`.
    //!
    //! The algorithm counts the defined transitions, then visits nodes and
    //! labels in their original order and writes the targets directly, using
    //! a bounded output buffer. Including `numTransitions` allows KBMAG tools
    //! to read sparse output using sparse storage. The algorithm does
    //! not sort edges or copy the graph. Output is independent of the stream's
    //! numeric formatting flags and locale.
    //!
    //! \tparam Node the node type of the word graph.
    //! \param output the destination stream.
    //! \param wg the word graph to encode.
    //! \param format the dense or sparse table representation (defaults to
    //! \ref kbmagformat::dense).
    //!
    //! \throws LibsemigroupsException if a target is out of bounds, the
    //! dimensions exceed the node type's range, the format is invalid, or
    //! writing fails without the stream throwing its own exception.
    //! \throws std::ios_base::failure if enabled by the stream.
    //!
    //! \complexity
    //! \f$O(n(d+1)+L)\f$ time and constant additional space, where \f$n\f$
    //! is the number of nodes, \f$d\f$ is the out-degree, and \f$L\f$ is the
    //! output length.
    //!
    //! \note On failure the stream may contain a partial record.
    //!
    //! \sa make(std::istream&), to_kbmag_string(), and the
    //! <a
    //! href="https://github.com/gap-packages/kbmag/blob/master/standalone/doc/fsa_format">
    //! KBMAG/GASP finite-state automaton specification</a>.
    template <typename Node>
    void write_kbmag(std::ostream&          output,
                     WordGraph<Node> const& wg,
                     kbmagformat            format = kbmagformat::dense);

    //! \ingroup word_graph_group
    //! \brief Returns a KBMAG representation of a word graph.
    //!
    //! Uses the same encoding and conventions as write_kbmag(). For large
    //! graphs, write_kbmag() avoids constructing the complete output string.
    //!
    //! \par Example
    //! \code
    //! WordGraph<uint32_t> graph(2, 2);
    //! graph.target(0, 0, 1);
    //! graph.target(0, 1, 1);
    //! auto text = word_graph::to_kbmag_string(graph);
    //! auto copy = make<WordGraph<uint32_t>>(text);
    //! copy == graph; // returns true
    //! \endcode
    //!
    //! \tparam Node the node type of the word graph.
    //! \param wg the word graph to encode.
    //! \param format the dense or sparse table representation (defaults to
    //! \ref kbmagformat::dense).
    //! \returns The KBMAG record as a string.
    //!
    //! \throws LibsemigroupsException if write_kbmag() throws.
    //!
    //! \complexity
    //! \f$O(n(d+1)+L)\f$ time and \f$O(L)\f$ additional space, with the same
    //! notation as write_kbmag().
    template <typename Node>
    [[nodiscard]] std::string to_kbmag_string(WordGraph<Node> const& wg,
                                              kbmagformat            format
                                              = kbmagformat::dense);
  }  // namespace word_graph

  //! \ingroup make_word_graph_group
  //! \brief Constructs a word graph from a KBMAG input stream.
  //!
  //! Reads a KBMAG/GASP record, optionally preceded by a GAP variable
  //! assignment and followed by a semicolon. Both `"dense deterministic"`
  //! and `"sparse"` tables are supported. States and alphabet must have type
  //! `"simple"`, and flags must include `"DFA"`. The initial list must be
  //! `[1]` (or `[]` for no nodes), and the accepting list must be empty.
  //! Nonempty accepting lists and other initial states are rejected rather
  //! than silently discarded.
  //!
  //! The parser accepts whitespace, comments beginning with `#`, backslash
  //! line continuations, and additional record fields containing balanced
  //! GAP expressions. Additional fields are skipped without evaluating GAP
  //! code. Required metadata must precede `table`, and `format` and any
  //! `defaultTarget` must precede `transitions`. External transition files
  //! and nondeterministic tables are not supported.
  //!
  //! Dense rows have exactly the alphabet size entries: a blank or nonpositive
  //! integer denotes an undefined transition. Sparse rows contain distinct
  //! positive labels and positive targets. A sparse `defaultTarget` supplies
  //! omitted transitions. Repeated targets under different labels are valid.
  //! The declared dimensions, including isolated nodes and unused labels,
  //! are preserved. The parser checks one row at a time, converts one-based
  //! indices to zero-based indices, and places targets at their original
  //! labels. It never constructs a second complete transition table.
  //!
  //! \tparam Return the return type, a specialization of WordGraph.
  //! \param input the source stream, consumed through the end of input.
  //! \returns The decoded word graph.
  //!
  //! \throws LibsemigroupsException for invalid or unsupported input, including
  //! dimensions too large for the requested node type or transition storage.
  //! Messages include the line and column, and the invalid value or field.
  //! \throws std::ios_base::failure if enabled by the stream.
  //!
  //! \complexity
  //! \f$O(L+n(d+1))\f$ time and \f$O(d+t+h)\f$ additional space, excluding
  //! the result, where \f$L\f$ is the input length, \f$n\f$ the node count,
  //! \f$d\f$ the out-degree, \f$t\f$ the longest token, and \f$h\f$ the
  //! size of additional metadata and its nesting depth.
  //!
  //! \note An empty graph is encoded with an empty initial list because
  //! there is no node zero. On failure the input may have been partly consumed.
  //!
  //! \sa word_graph::write_kbmag(), word_graph::to_kbmag_string().
  template <typename Return>
  [[nodiscard]] std::enable_if_t<is_specialization_of_v<Return, WordGraph>,
                                 Return>
  make(std::istream& input);

  //! \ingroup make_word_graph_group
  //! \brief Constructs a word graph from a KBMAG string.
  //!
  //! Uses the same parser and conventions as make(std::istream&).
  //!
  //! \tparam Return the return type, a specialization of WordGraph.
  //! \param input the KBMAG record to decode.
  //! \returns The decoded word graph.
  //!
  //! \throws LibsemigroupsException if make(std::istream&) throws.
  //!
  //! \complexity
  //! As for make(std::istream&), with an additional \f$O(L)\f$ space for
  //! the input stream's copy of \p input.
  template <typename Return>
  [[nodiscard]] std::enable_if_t<is_specialization_of_v<Return, WordGraph>,
                                 Return>
  make(std::string const& input);
}  // namespace libsemigroups

#include "word-graph-io.tpp"
#endif  // LIBSEMIGROUPS_WORD_GRAPH_IO_HPP_
