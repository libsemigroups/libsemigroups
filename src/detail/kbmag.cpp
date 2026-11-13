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

// Streaming KBMAG parsing and buffered decimal output.

#include "libsemigroups/detail/kbmag.hpp"

#include <algorithm>     // for fill
#include <charconv>      // for from_chars, to_chars
#include <istream>       // for istream
#include <ostream>       // for ostream
#include <system_error>  // for errc

#include "libsemigroups/detail/fmt.hpp"  // for fmt::format
#include "libsemigroups/exception.hpp"   // for LIBSEMIGROUPS_EXCEPTION

namespace libsemigroups {
  namespace detail {
    namespace {
      bool letter(int c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
      }
      bool digit(int c) {
        return c >= '0' && c <= '9';
      }
      bool whitespace(int c) {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f'
               || c == '\v';
      }
    }  // namespace

    [[noreturn]] void KbmagReader::fail(std::string const& reason) const {
      LIBSEMIGROUPS_EXCEPTION("invalid KBMAG input at line {}, column {}: {}",
                              _token_line,
                              _token_column,
                              reason);
    }

    int KbmagReader::peek() {
      auto c = _input.peek();
      if (_input.bad() || (_input.fail() && !_input.eof())) {
        fail("failed to read from the input stream");
      }
      return c;
    }

    int KbmagReader::get() {
      int c = _input.get();
      if (_input.bad() || (_input.fail() && !_input.eof())) {
        fail("failed to read from the input stream");
      }
      if (c == '\n') {
        ++_line;
        _column = 1;
      } else if (c != std::char_traits<char>::eof()) {
        ++_column;
      }
      return c;
    }

    void KbmagReader::next() {
      // Whitespace and GAP comments are insignificant. Backslash-newline
      // continuations are handled below as well as inside individual tokens.
      while (true) {
        while (whitespace(peek())) {
          get();
        }
        if (peek() == '#') {
          while (peek() != '\n' && peek() != std::char_traits<char>::eof()) {
            get();
          }
        } else if (peek() == '\\') {
          get();
          if (peek() == '\r') {
            get();
          }
          if (get() != '\n') {
            fail("expected a newline after a backslash");
          }
        } else {
          break;
        }
      }
      _token_line   = _line;
      _token_column = _column;
      _token.clear();
      int c = get();
      if (c == std::char_traits<char>::eof()) {
        _kind = kind::end;
        return;
      }
      if (c == '"' || c == '\'') {
        _kind     = kind::string;
        int quote = c;
        while ((c = get()) != quote) {
          if (c == std::char_traits<char>::eof()) {
            fail("unterminated quoted string");
          }
          if (c == '\\') {
            c = get();
            if (c == std::char_traits<char>::eof()) {
              fail("unterminated escape in quoted string");
            }
            if (c == '\r' && peek() == '\n') {
              get();
              continue;
            }
            if (c == '\n') {
              continue;
            }
            if (c == 'n') {
              c = '\n';
            } else if (c == 't') {
              c = '\t';
            } else if (c == 'r') {
              c = '\r';
            }
          }
          _token.push_back(static_cast<char>(c));
        }
        return;
      }
      _token.push_back(static_cast<char>(c));
      if (letter(c) || digit(c) || (c == '-' && digit(peek()))) {
        _kind = letter(c) ? kind::identifier : kind::number;
        while (true) {
          c = peek();
          if (digit(c) || (_kind == kind::identifier && letter(c))) {
            _token.push_back(static_cast<char>(get()));
          } else if (c == '\\') {
            get();
            if (peek() == '\r') {
              get();
            }
            if (get() != '\n') {
              fail("expected a newline after a backslash");
            }
          } else {
            break;
          }
        }
        return;
      }
      _kind = kind::punctuation;
      if ((c == ':' && peek() == '=') || (c == '.' && peek() == '.')) {
        _token.push_back(static_cast<char>(get()));
      }
    }

    bool KbmagReader::is(std::string_view value) const {
      return _kind != kind::string && _kind != kind::end && _token == value;
    }

    bool KbmagReader::accept(std::string_view value) {
      if (!is(value)) {
        return false;
      }
      next();
      return true;
    }

    void KbmagReader::expect(std::string_view value) {
      if (!accept(value)) {
        fail(fmt::format("expected '{}', found {}",
                         value,
                         _kind == kind::end ? "end of input"
                                            : fmt::format("'{}'", _token)));
      }
    }

    uint64_t KbmagReader::number(std::string_view description,
                                 bool             allow_negative) {
      bool negative = !_token.empty() && _token[0] == '-';
      if (_kind != kind::number || (negative && !allow_negative)) {
        fail(fmt::format("expected {} to be a nonnegative integer, found '{}'",
                         description,
                         _token));
      }
      uint64_t    value  = 0;
      auto const* first  = _token.data() + (negative ? 1 : 0);
      auto const* last   = _token.data() + _token.size();
      auto        parsed = std::from_chars(first, last, value);
      if (parsed.ec != std::errc() || parsed.ptr != last) {
        fail(fmt::format("{} '{}' exceeds the supported 64-bit integer range",
                         description,
                         _token));
      }
      next();
      return negative ? 0 : value;
    }

    std::string KbmagReader::string() {
      if (_kind != kind::string) {
        fail(fmt::format("expected a quoted string, found '{}'", _token));
      }
      auto value = _token;
      next();
      return value;
    }

    std::string KbmagReader::field(std::unordered_set<std::string>& seen) {
      if (_kind != kind::identifier) {
        fail(fmt::format("expected a record field name, found '{}'", _token));
      }
      auto name = _token;
      if (!seen.insert(name).second) {
        fail(fmt::format("duplicate record field '{}'", name));
      }
      next();
      expect(":=");
      return name;
    }

    void KbmagReader::skip_value() {
      // Skip an expression without interpreting it. A delimiter stack avoids
      // recursion and does not mistake punctuation in strings for delimiters.
      std::vector<char> closing;
      bool              consumed = false;
      while (_kind != kind::end) {
        if (closing.empty() && (is(",") || is(")"))) {
          break;
        }
        if (is("(") || is("[") || is("{")) {
          closing.push_back(is("(") ? ')' : (is("[") ? ']' : '}'));
        } else if (is(")") || is("]") || is("}")) {
          if (closing.empty() || _token[0] != closing.back()) {
            fail(fmt::format("unmatched delimiter '{}' in additional field",
                             _token));
          }
          closing.pop_back();
        }
        consumed = true;
        next();
      }
      if (!consumed || !closing.empty() || _kind == kind::end) {
        fail("expected a complete value for the additional field");
      }
    }

    uint64_t KbmagReader::read_set(std::string const& name) {
      expect("rec");
      expect("(");
      std::unordered_set<std::string> seen;
      uint64_t                        size  = 0;
      bool                            first = true;
      while (!accept(")")) {
        if (!first) {
          expect(",");
        }
        first    = false;
        auto key = field(seen);
        if (key == "type") {
          auto value = string();
          if (value != "simple") {
            fail(fmt::format(
                "expected {}.type 'simple', found '{}'", name, value));
          }
        } else if (key == "size") {
          size = number(name + ".size");
        } else {
          skip_value();
        }
      }
      if (!seen.count("type") || !seen.count("size")) {
        fail(fmt::format("{} requires both 'type' and 'size' fields", name));
      }
      return size;
    }

    void KbmagReader::read_flags() {
      expect("[");
      bool dfa   = false;
      bool first = true;
      while (!accept("]")) {
        if (!first) {
          expect(",");
        }
        first      = false;
        auto value = string();
        if (value == "NFA" || value == "MIDFA") {
          fail(fmt::format("expected a DFA, found flag '{}'", value));
        }
        dfa = dfa || value == "DFA";
      }
      if (!dfa) {
        fail("expected flags to include 'DFA'");
      }
    }

    void KbmagReader::read_initial() {
      if (!_fields.count("states")) {
        fail("expected 'states' before 'initial'");
      }
      expect("[");
      if (_number_of_nodes != 0) {
        auto value = number("initial state");
        if (value != 1) {
          fail(fmt::format(
              "expected initial state 1 (WordGraph node 0), found {}", value));
        }
        if (accept("..")) {
          expect("1");
        }
      }
      if (!is("]")) {
        fail(
            fmt::format("expected initial list {}, found additional value '{}'",
                        _number_of_nodes == 0 ? "[]" : "[1]",
                        _token));
      }
      expect("]");
    }

    void KbmagReader::read_accepting() {
      expect("[");
      if (!is("]")) {
        fail(fmt::format("expected accepting := [] because word graphs have no "
                         "accepting states, found '{}'",
                         _token));
      }
      expect("]");
    }

    void KbmagReader::require_header() const {
      for (auto name :
           {"isFSA", "alphabet", "states", "flags", "initial", "accepting"}) {
        if (!_fields.count(name)) {
          fail(fmt::format("missing required field '{}' before 'table'", name));
        }
      }
    }

    void KbmagReader::read_table_field(std::string const& name) {
      if (name == "format") {
        auto value = string();
        if (value != "sparse" && value != "dense deterministic") {
          fail(fmt::format("expected table format 'dense deterministic' or "
                           "'sparse', found '{}'",
                           value));
        }
        _sparse = value == "sparse";
      } else if (name == "defaultTarget") {
        if (!_table_fields.count("format") || !_sparse) {
          fail("defaultTarget is only supported after format := 'sparse'");
        }
        _default_target = number("defaultTarget");
        if (_default_target == 0 || _default_target > _number_of_nodes) {
          fail(fmt::format("expected defaultTarget in [1, {}], found {}",
                           _number_of_nodes,
                           _default_target));
        }
      } else if (name == "numTransitions") {
        _expected_transitions = number("numTransitions");
      } else if (name == "filename") {
        fail("external transition files are unsupported; expected "
             "'transitions'");
      } else {
        skip_value();
      }
    }

    KbmagReader::KbmagReader(std::istream& input) : _input(input) {
      next();
      if (!is("rec")) {
        if (_kind != kind::identifier) {
          fail("expected 'rec' or a GAP variable assignment");
        }
        next();
        while (accept(".")) {
          if (_kind != kind::identifier && _kind != kind::number) {
            fail("expected a component name in the GAP variable assignment");
          }
          next();
        }
        expect(":=");
      }
      expect("rec");
      expect("(");
      bool first = true;
      while (!is(")")) {
        if (!first) {
          expect(",");
        }
        first     = false;
        auto name = field(_fields);
        if (name == "isFSA") {
          expect("true");
        } else if (name == "alphabet") {
          _out_degree = read_set(name);
        } else if (name == "states") {
          _number_of_nodes = read_set(name);
        } else if (name == "flags") {
          read_flags();
        } else if (name == "initial") {
          read_initial();
        } else if (name == "accepting") {
          read_accepting();
        } else if (name == "table") {
          require_header();
          expect("rec");
          expect("(");
          bool first_table = true;
          while (!is(")")) {
            if (!first_table) {
              expect(",");
            }
            first_table = false;
            auto key    = field(_table_fields);
            if (key == "transitions") {
              if (!_table_fields.count("format")) {
                fail("expected table 'format' before 'transitions'");
              }
              expect("[");
              return;
            }
            read_table_field(key);
          }
          fail("missing required table field 'transitions'");
        } else {
          skip_value();
        }
      }
      fail("missing required field 'table'");
    }

    std::vector<uint64_t> const& KbmagReader::read_row() {
      if (is("]")) {
        fail(fmt::format("expected {} transition rows, found {} (missing "
                         "row for source {})",
                         _number_of_nodes,
                         _source,
                         _source));
      }
      if (_source != 0) {
        expect(",");
      }
      if (!is("[")) {
        fail(fmt::format("expected transition row for source {}, found '{}'",
                         _source,
                         _token));
      }
      expect("[");
      _row.assign(static_cast<size_t>(_out_degree), _default_target);
      if (_sparse) {
        _seen.assign(static_cast<size_t>(_out_degree), false);
        bool first = true;
        while (!accept("]")) {
          if (!first) {
            expect(",");
          }
          first = false;
          expect("[");
          auto a = number("sparse label");
          if (a == 0 || a > _out_degree) {
            fail(fmt::format("expected sparse label in [1, {}] at source {}, "
                             "found {}",
                             _out_degree,
                             _source,
                             a));
          }
          if (_seen[a - 1]) {
            fail(fmt::format("repeated sparse label {} at source {}; expected "
                             "at most one target per label",
                             a,
                             _source));
          }
          _seen[a - 1] = true;
          expect(",");
          auto t = number("sparse target");
          if (t == 0 || t > _number_of_nodes) {
            fail(fmt::format("expected sparse target in [1, {}] at source {}, "
                             "label {}, found {}",
                             _number_of_nodes,
                             _source,
                             a,
                             t));
          }
          _row[a - 1] = t;
          expect("]");
        }
      } else {
        for (uint64_t a = 0; a < _out_degree; ++a) {
          if (a != 0) {
            if (is("]")) {
              fail(fmt::format("expected {} dense entries at source {}, "
                               "found {}",
                               _out_degree,
                               _source,
                               a));
            }
            expect(",");
          }
          if (is(",") || is("]")) {
            continue;
          }
          auto t = number("dense target", true);
          if (t > _number_of_nodes) {
            fail(fmt::format("expected dense target at most {} at source {}, "
                             "label {}, found {}",
                             _number_of_nodes,
                             _source,
                             a,
                             t));
          }
          _row[a] = t;
        }
        if (!is("]")) {
          fail(fmt::format("expected {} dense entries at source {}; "
                           "found additional value '{}'",
                           _out_degree,
                           _source,
                           _token));
        }
        expect("]");
      }
      for (auto t : _row) {
        _transitions += t != 0;
      }
      ++_source;
      return _row;
    }

    void KbmagReader::finish() {
      if (is(",") || is("[")) {
        fail(fmt::format("expected {} transition rows; found an additional "
                         "row for source {}",
                         _number_of_nodes,
                         _source));
      }
      expect("]");
      while (accept(",")) {
        auto key = field(_table_fields);
        if (key == "format" || key == "defaultTarget") {
          fail(fmt::format("expected '{}' before 'transitions'", key));
        }
        read_table_field(key);
      }
      expect(")");
      while (accept(",")) {
        auto key = field(_fields);
        skip_value();
      }
      expect(")");
      accept(";");
      if (_kind != kind::end) {
        fail(fmt::format("expected end of input, found '{}'", _token));
      }
      if (_table_fields.count("numTransitions")
          && _expected_transitions != _transitions) {
        fail(fmt::format(
            "numTransitions declares {} edges, but the table has {}",
            _expected_transitions,
            _transitions));
      }
    }

    KbmagWriter::KbmagWriter(std::ostream& output) : _output(output) {
      _buffer.reserve(65536);
    }

    void KbmagWriter::write(std::string_view text) {
      _buffer.append(text);
      if (_buffer.size() >= 65536) {
        flush();
      }
    }

    void KbmagWriter::write(uint64_t value) {
      char buffer[20];
      auto end = std::to_chars(buffer, buffer + sizeof(buffer), value).ptr;
      write(std::string_view(buffer, end - buffer));
    }

    void KbmagWriter::flush() {
      _output.write(_buffer.data(),
                    static_cast<std::streamsize>(_buffer.size()));
      if (!_output) {
        LIBSEMIGROUPS_EXCEPTION("failed to write KBMAG output to the stream");
      }
      _buffer.clear();
    }
  }  // namespace detail
}  // namespace libsemigroups
