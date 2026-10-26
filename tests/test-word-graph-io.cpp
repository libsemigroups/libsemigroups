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

#include <cstdint>    // for uint8_t, uint16_t, uint32_t, uint64_t
#include <iomanip>    // for hex, showbase
#include <ios>        // for ios_base
#include <istream>    // for istream
#include <locale>     // for locale, numpunct
#include <ostream>    // for ostream
#include <sstream>    // for istringstream, ostringstream
#include <streambuf>  // for streambuf
#include <string>     // for string
#include <utility>    // for pair
#include <vector>     // for vector

#include "test-main.hpp"

#include "libsemigroups/exception.hpp"
#include "libsemigroups/word-graph.hpp"

namespace libsemigroups {
  namespace {
    std::string record(std::string const& rows   = "[[2,2],[1,2]]",
                       std::string const& format = "dense deterministic",
                       std::string const& nodes  = "2",
                       std::string const& degree = "2") {
      return "rec(isFSA:=true,alphabet:=rec(type:=\"simple\",size:=" + degree
             + "),states:=rec(type:=\"simple\",size:=" + nodes
             + "),flags:=[\"DFA\"],initial:=" + (nodes == "0" ? "[]" : "[1]")
             + ",accepting:=[],table:=rec(format:=\"" + format
             + "\",transitions:=" + rows + "))";
    }

    std::string replace(std::string        text,
                        std::string const& from,
                        std::string const& to) {
      auto pos = text.find(from);
      REQUIRE(pos != std::string::npos);
      text.replace(pos, from.size(), to);
      return text;
    }

    template <typename Node = uint32_t>
    void check_invalid(std::string const& input, std::string const& reason) {
      CAPTURE(input);
      REQUIRE_THROWS_AS(make<WordGraph<Node>>(input), LibsemigroupsException);
      try {
        (void) make<WordGraph<Node>>(input);
      } catch (LibsemigroupsException const& e) {
        auto message = chomp(e.what());
        REQUIRE(message.find("invalid KBMAG input at line ") == 0);
        REQUIRE(message.find(", column ") != std::string::npos);
        REQUIRE(message.find(reason) != std::string::npos);
      }
    }

    class ChunkedInputBuffer : public std::streambuf {
     public:
      explicit ChunkedInputBuffer(std::string const& text) : _text(text) {}

     protected:
      int_type underflow() override {
        if (_pos == _text.size()) {
          return traits_type::eof();
        }
        _char = _text[_pos++];
        setg(&_char, &_char, &_char + 1);
        return traits_type::to_int_type(_char);
      }

     private:
      std::string const& _text;
      size_t             _pos  = 0;
      char               _char = 0;
    };

    class FailingOutputBuffer : public std::streambuf {
     protected:
      std::streamsize xsputn(char const*, std::streamsize n) override {
        return n == 0 ? 0 : n - 1;
      }
    };

    class GroupedNumbers : public std::numpunct<char> {
     protected:
      char do_thousands_sep() const override {
        return '_';
      }
      std::string do_grouping() const override {
        return "\1";
      }
    };
  }  // namespace

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("WordGraphIO",
                                   "000",
                                   "KBMAG example",
                                   "[quick][word-graph-io]",
                                   uint8_t,
                                   uint16_t,
                                   uint32_t,
                                   uint64_t) {
    auto graph = make<WordGraph<TestType>>(2, {{1, 1}, {0, 1}});
    REQUIRE(make<WordGraph<TestType>>(record()) == graph);
    auto text = word_graph::to_kbmag_string(graph);
    REQUIRE(text
            == "word_graph := rec(\n"
               "  isFSA := true,\n"
               "  alphabet := rec(type := \"simple\", size := 2),\n"
               "  states := rec(type := \"simple\", size := 2),\n"
               "  flags := [\"DFA\"],\n"
               "  initial := [1],\n"
               "  accepting := [],\n"
               "  table := rec(format := \"dense deterministic\", "
               "numTransitions := 4, transitions "
               ":= [\n"
               "[2,2],\n[1,2]\n])\n);\n");
    REQUIRE(make<WordGraph<TestType>>(text) == graph);
    auto sparse
        = word_graph::to_kbmag_string(graph, word_graph::kbmagformat::sparse);
    REQUIRE(sparse.find("[[1,2],[2,2]],\n[[1,1],[2,2]]") != std::string::npos);
    REQUIRE(make<WordGraph<TestType>>(sparse) == graph);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraphIO",
                          "001",
                          "KBMAG dimensions and labels",
                          "[quick][word-graph-io]") {
    for (uint32_t n = 0; n < 8; ++n) {
      for (uint32_t d = 0; d < 7; ++d) {
        WordGraph<uint32_t> graph(n, d);
        for (uint32_t s = 0; s < n; ++s) {
          for (uint32_t a = 0; a < d; ++a) {
            if ((s + 3 * a) % 4 != 0) {
              graph.target(s, a, (n + 2 * s - (a % n)) % n);
            }
          }
        }
        for (auto format : {word_graph::kbmagformat::dense,
                            word_graph::kbmagformat::sparse}) {
          auto text    = word_graph::to_kbmag_string(graph, format);
          auto decoded = make<WordGraph<uint32_t>>(text);
          REQUIRE(decoded == graph);
          REQUIRE(decoded.number_of_nodes() == n);
          REQUIRE(decoded.out_degree() == d);
          REQUIRE(text.find(n == 0 ? "initial := []" : "initial := [1]")
                  != std::string::npos);
        }
      }
    }
    // Isolated nodes and an entirely unused alphabet must not disappear.
    WordGraph<uint8_t> empty_edges(4, 7);
    REQUIRE(make<WordGraph<uint8_t>>(word_graph::to_kbmag_string(
                empty_edges, word_graph::kbmagformat::sparse))
            == empty_edges);
    WordGraph<uint16_t> parallel(1, 300);
    for (uint16_t a = 0; a < 300; ++a) {
      parallel.target(0, a, 0);
    }
    REQUIRE(make<WordGraph<uint16_t>>(word_graph::to_kbmag_string(
                parallel, word_graph::kbmagformat::sparse))
            == parallel);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraphIO",
                          "002",
                          "KBMAG syntax and sparse defaults",
                          "[quick][word-graph-io]") {
    auto        graph = make<WordGraph<uint32_t>>(2, {{1, 1}, {0, 1}});
    std::string text  = R"( # Fields may contain GAP comments and whitespace.
_RWS.wa := rec(
  isFSA := true,
  states := rec(type := "simple", size := 2),
  alphabet := rec(type := "simple", size := 2),
  extra := rec(values := ["comma, close) and # inside a string", [1, 2]]),
  flags := ["DFA", "dense"],
  accepting := [],
  initial := [1..1],
  table := rec(format := "sparse", numTransitions := 4,
               defaultTarget := 2, transitions := [[], [[1,1]]]),
  trailing := ["escaped \" quote", rec(a := true)]
); # End of record.
)";
    REQUIRE(make<WordGraph<uint32_t>>(text) == graph);
    auto continued = replace(record(), "isFSA", "isF\\\nSA");
    continued      = replace(continued, "size:=2", "size:=\\\r\n2");
    continued      = replace(continued, "simple", "sim\\\r\nple");
    REQUIRE(make<WordGraph<uint32_t>>(continued) == graph);
    auto partial = make<WordGraph<uint32_t>>(record("[[,2],[1,-7]]"));
    REQUIRE(partial.target(0, 0) == UNDEFINED);
    REQUIRE(partial.target(0, 1) == 1);
    REQUIRE(partial.target(1, 0) == 0);
    REQUIRE(partial.target(1, 1) == UNDEFINED);
    REQUIRE(make<WordGraph<uint32_t>>(
                record("[[2,2],[1,2]]", "dense deterministic"))
            == graph);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraphIO",
                          "003",
                          "KBMAG stream handling",
                          "[quick][word-graph-io]") {
    auto               text = record();
    ChunkedInputBuffer buffer(text);
    std::istream       input(&buffer);
    auto               graph = make<WordGraph<uint32_t>>(input);
    REQUIRE(graph == make<WordGraph<uint32_t>>(2, {{1, 1}, {0, 1}}));
    std::ostringstream output;
    output.imbue(std::locale(std::locale::classic(), new GroupedNumbers));
    output << std::hex << std::showbase;
    output.width(30);
    auto flags = output.flags();
    word_graph::write_kbmag(output, graph);
    REQUIRE(output.str() == word_graph::to_kbmag_string(graph));
    REQUIRE(output.flags() == flags);
    REQUIRE(output.width() == 30);
    FailingOutputBuffer failed_buffer;
    std::ostream        failed_output(&failed_buffer);
    REQUIRE_EXCEPTION_MSG(word_graph::write_kbmag(failed_output, graph),
                          "failed to write KBMAG output to the stream");
    failed_output.clear();
    failed_output.exceptions(std::ios::badbit);
    REQUIRE_THROWS_AS(word_graph::write_kbmag(failed_output, graph),
                      std::ios_base::failure);
    std::istringstream failed_input(text);
    failed_input.setstate(std::ios::badbit);
    REQUIRE_THROWS_AS(make<WordGraph<uint32_t>>(failed_input),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraphIO",
                          "004",
                          "KBMAG malformed records",
                          "[quick][word-graph-io]") {
    check_invalid("", "expected 'rec' or a GAP variable assignment");
    check_invalid(replace(record(), "isFSA:=true", "isFSA:=false"),
                  "expected 'true'");
    check_invalid(replace(record(), "isFSA:=true,", ""),
                  "missing required field 'isFSA'");
    check_invalid(replace(record(), "isFSA:=true", "isFSA:=true,isFSA:=true"),
                  "duplicate record field 'isFSA'");
    check_invalid(
        replace(record(), "type:=\"simple\"", "type:=\"identifiers\""),
        "expected alphabet.type 'simple', found 'identifiers'");
    check_invalid(replace(record(), "type:=\"simple\",", ""),
                  "requires both 'type' and 'size'");
    check_invalid(replace(record(), "flags:=[\"DFA\"]", "flags:=[]"),
                  "include 'DFA'");
    check_invalid(replace(record(), "\"DFA\"", "\"NFA\""),
                  "expected a DFA, found flag 'NFA'");
    check_invalid(replace(record(), "initial:=[1]", "initial:=[2]"),
                  "expected initial state 1 (WordGraph node 0), found 2");
    check_invalid(replace(record(), "initial:=[1]", "initial:=[1,2]"),
                  "expected initial list [1]");
    check_invalid(replace(record(), "accepting:=[]", "accepting:=[1]"),
                  "word graphs have no accepting states");
    check_invalid(record("[]", "dense nondeterministic"),
                  "expected table format");
    check_invalid(
        replace(record(), "transitions:=[[2,2],[1,2]]", "filename:=\"x\""),
        "external transition files are unsupported");
    check_invalid(replace(record(), "format:=\"dense deterministic\",", ""),
                  "expected table 'format'");
    check_invalid(record() + " trailing", "expected end of input");
    check_invalid(
        replace(record(), "accepting:=[]", "accepting:=[],extra:=([)]"),
        "unmatched delimiter");
    check_invalid(record() + "\"unfinished", "unterminated quoted string");
    // Truncation at every byte must either identify the error or be a complete
    // record (only the full input is complete here).
    auto text = record();
    for (size_t i = 0; i < text.size(); ++i) {
      CAPTURE(i);
      REQUIRE_THROWS_AS(make<WordGraph<uint32_t>>(text.substr(0, i)),
                        LibsemigroupsException);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraphIO",
                          "005",
                          "KBMAG transition diagnostics",
                          "[quick][word-graph-io]") {
    check_invalid(
        record("[[3,2],[1,2]]"),
        "expected dense target at most 2 at source 0, label 0, found 3");
    check_invalid(record("[[1],[1,2]]"),
                  "expected 2 dense entries at source 0, found 1");
    check_invalid(record("[[1,2,1],[1,2]]"),
                  "expected 2 dense entries at source 0; found additional");
    check_invalid(record("[[1,2]]"), "expected 2 transition rows, found 1");
    check_invalid(record("[[1,2],[1,2],[1,2]]"),
                  "expected 2 transition rows; found an additional row");
    check_invalid(record("[[[0,1]],[]]", "sparse"),
                  "expected sparse label in [1, 2] at source 0, found 0");
    check_invalid(record("[[[3,1]],[]]", "sparse"),
                  "expected sparse label in [1, 2] at source 0, found 3");
    check_invalid(
        record("[[[1,3]],[]]", "sparse"),
        "expected sparse target in [1, 2] at source 0, label 1, found 3");
    check_invalid(record("[[[1,0]],[]]", "sparse"),
                  "expected sparse target in [1, 2]");
    check_invalid(record("[[[1,1],[1,2]],[]]", "sparse"),
                  "repeated sparse label 1 at source 0");
    check_invalid(
        replace(record(), "transitions:=", "numTransitions:=3,transitions:="),
        "numTransitions declares 3 edges, but the table has 4");
    check_invalid(
        replace(record(), "transitions:=", "defaultTarget:=1,transitions:="),
        "defaultTarget is only supported after format := 'sparse'");
    check_invalid(replace(record("[[],[]]", "sparse"),
                          "transitions:=",
                          "defaultTarget:=3,transitions:="),
                  "expected defaultTarget in [1, 2], found 3");
    auto invalid = WordGraph<uint32_t>(2, 1);
    invalid.target_no_checks(1, 0, 2);
    REQUIRE_EXCEPTION_MSG(
        (void) word_graph::to_kbmag_string(invalid),
        "expected target less than 2 at source 1, label 0, found 2");
    REQUIRE_EXCEPTION_MSG(
        (void) word_graph::to_kbmag_string(
            WordGraph<uint32_t>(), static_cast<word_graph::kbmagformat>(9)),
        "expected dense or sparse KBMAG format, found 9");
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraphIO",
                          "006",
                          "KBMAG bounds before allocation",
                          "[quick][word-graph-io]") {
    check_invalid<uint8_t>(record("[]", "dense deterministic", "256", "1"),
                           "expected node count and alphabet size at most 255, "
                           "found 256 nodes and alphabet size 1");
    check_invalid<uint8_t>(record("[]", "dense deterministic", "1", "256"),
                           "expected node count and alphabet size at most 255, "
                           "found 1 nodes and alphabet size 256");
    check_invalid(
        record("[]", "dense deterministic", "2", "18446744073709551616"),
        "64-bit integer range");
    check_invalid(record("[]", "dense deterministic", "2", "-1"),
                  "alphabet.size to be a nonnegative integer");
    check_invalid<uint64_t>(
        record("[]", "dense deterministic", "4294967296", "4294967296"),
        "exceeds the maximum storage size");
    REQUIRE_EXCEPTION_MSG(
        (void) word_graph::to_kbmag_string(WordGraph<uint8_t>(256, 0)),
        "expected KBMAG node count and out-degree at most 255, found 256 nodes "
        "and out-degree 0");
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraphIO",
                          "007",
                          "KBMAG buffered output",
                          "[quick][word-graph-io]") {
    WordGraph<uint32_t> graph(7000, 5);
    for (uint32_t s = 0; s < 7000; ++s) {
      for (uint32_t a = 0; a < 5; ++a) {
        graph.target(s, a, (3 * s + a) % 7000);
      }
    }
    for (auto format :
         {word_graph::kbmagformat::dense, word_graph::kbmagformat::sparse}) {
      auto text = word_graph::to_kbmag_string(graph, format);
      REQUIRE(text.size() > 65536);
      REQUIRE(make<WordGraph<uint32_t>>(text) == graph);
    }
  }
}  // namespace libsemigroups
