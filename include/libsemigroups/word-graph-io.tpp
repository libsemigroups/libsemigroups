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

// Template implementations of KBMAG serialization.

#include <cstdint>  // for uint64_t
#include <limits>   // for numeric_limits
#include <sstream>  // for istringstream, ostringstream
#include <vector>   // for vector

#include "detail/fmt.hpp"    // for fmt::format
#include "detail/kbmag.hpp"  // for KbmagReader, KbmagWriter
#include "exception.hpp"     // for LIBSEMIGROUPS_EXCEPTION

namespace libsemigroups {
  namespace word_graph {
    template <typename Node>
    void write_kbmag(std::ostream&          output,
                     WordGraph<Node> const& wg,
                     kbmagformat            format) {
      if (format != kbmagformat::dense && format != kbmagformat::sparse) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected dense or sparse KBMAG format, found {}",
            static_cast<int>(format));
      }
      auto const n   = wg.number_of_nodes();
      auto const d   = wg.out_degree();
      auto const max = std::numeric_limits<Node>::max();
      if (n > max || d > max) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected KBMAG node count and out-degree at most {}, found {} "
            "nodes and out-degree {}",
            max,
            n,
            d);
      }
      libsemigroups::detail::KbmagWriter writer(output);
      writer.write(
          "word_graph := rec(\n  isFSA := true,\n  alphabet := rec(type := "
          "\"simple\", size := ");
      writer.write(static_cast<uint64_t>(d));
      writer.write("),\n  states := rec(type := \"simple\", size := ");
      writer.write(static_cast<uint64_t>(n));
      writer.write("),\n  flags := [\"DFA\"],\n  initial := ");
      writer.write(n == 0 ? "[]" : "[1]");
      writer.write(",\n  accepting := [],\n  table := rec(format := ");
      writer.write(format == kbmagformat::dense ? "\"dense deterministic\""
                                                : "\"sparse\"");
      writer.write(", numTransitions := ");
      writer.write(static_cast<uint64_t>(wg.number_of_edges()));
      writer.write(", transitions := [");
      for (size_t s = 0; s < n; ++s) {
        writer.write(s == 0 ? "\n[" : ",\n[");
        bool first = true;
        for (size_t a = 0; a < d; ++a) {
          auto const t
              = wg.target_no_checks(static_cast<Node>(s), static_cast<Node>(a));
          if (t != UNDEFINED && t >= n) {
            LIBSEMIGROUPS_EXCEPTION(
                "expected target less than {} at source {}, label {}, found {}",
                n,
                s,
                a,
                t);
          }
          if (format == kbmagformat::sparse && t == UNDEFINED) {
            continue;
          }
          if (!first) {
            writer.write(",");
          }
          first = false;
          if (format == kbmagformat::sparse) {
            writer.write("[");
            writer.write(static_cast<uint64_t>(a) + 1);
            writer.write(",");
          }
          writer.write(t == UNDEFINED ? uint64_t{0}
                                      : static_cast<uint64_t>(t) + 1);
          if (format == kbmagformat::sparse) {
            writer.write("]");
          }
        }
        writer.write("]");
      }
      writer.write("\n])\n);\n");
      writer.flush();
    }

    template <typename Node>
    std::string to_kbmag_string(WordGraph<Node> const& wg, kbmagformat format) {
      std::ostringstream output;
      write_kbmag(output, wg, format);
      return output.str();
    }
  }  // namespace word_graph

  template <typename Return>
  std::enable_if_t<is_specialization_of_v<Return, WordGraph>, Return>
  make(std::istream& input) {
    using node_type = typename Return::node_type;
    detail::KbmagReader reader(input);
    auto const          n   = reader.number_of_nodes();
    auto const          d   = reader.out_degree();
    auto const          max = std::numeric_limits<node_type>::max();
    if (n > max || d > max) {
      reader.fail(
          fmt::format("expected node count and alphabet size at most {}, "
                      "found {} nodes and alphabet size {}",
                      max,
                      n,
                      d));
    }
    auto const max_size = std::vector<node_type>().max_size();
    if (n > std::numeric_limits<size_t>::max()
        || d > std::numeric_limits<size_t>::max()
        || (d != 0 && n > max_size / d)) {
      reader.fail(
          fmt::format("the {} by {} transition table exceeds the maximum "
                      "storage size of {} entries",
                      n,
                      d,
                      max_size));
    }
    Return result(static_cast<size_t>(n), static_cast<size_t>(d));
    for (size_t s = 0; s < n; ++s) {
      auto const& row = reader.read_row();
      for (size_t a = 0; a < d; ++a) {
        if (row[a] != 0) {
          result.target_no_checks(static_cast<node_type>(s),
                                  static_cast<node_type>(a),
                                  static_cast<node_type>(row[a] - 1));
        }
      }
    }
    reader.finish();
    return result;
  }

  template <typename Return>
  std::enable_if_t<is_specialization_of_v<Return, WordGraph>, Return>
  make(std::string const& input) {
    std::istringstream stream(input);
    return make<Return>(stream);
  }
}  // namespace libsemigroups
