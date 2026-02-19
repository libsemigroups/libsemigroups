//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020-2026 James D. Mitchell
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

#include "libsemigroups/forest.hpp"

#include <algorithm>  // for fill, count
#include <cstddef>    // for size_t
#include <vector>     // for vector

#include "libsemigroups/constants.hpp"      // for operator!=, UNDEFINED
#include "libsemigroups/debug.hpp"          // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/detail/string.hpp"  // for group_digits
#include "libsemigroups/exception.hpp"      // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/types.hpp"          // for word_type

namespace libsemigroups {

  ////////////////////////////////////////////////////////////////////////
  // Forest class
  ////////////////////////////////////////////////////////////////////////

  Forest::~Forest() = default;

  Forest& Forest::init(size_t n) {
    _edge_label.resize(n);
    std::fill(std::begin(_edge_label),
              std::end(_edge_label),
              static_cast<uint32_t>(UNDEFINED));
    _parent.resize(n);
    std::fill(std::begin(_parent),
              std::end(_parent),
              static_cast<uint32_t>(UNDEFINED));
    return *this;
  }

  Forest& Forest::add_nodes(size_t n) {
    size_t const old_nr_nodes = number_of_nodes();
    try {
      _edge_label.insert(
          _edge_label.cend(), n, static_cast<uint32_t>(UNDEFINED));
      _parent.insert(_parent.cend(), n, static_cast<uint32_t>(UNDEFINED));
    } catch (...) {
      _edge_label.resize(old_nr_nodes);
      _parent.resize(old_nr_nodes);
      throw;
    }
    return *this;
  }

  void Forest::throw_if_node_out_of_bounds(node_type v) const {
    if (v >= number_of_nodes()) {
      LIBSEMIGROUPS_EXCEPTION("node value out of bounds, expected value in "
                              "the range [0, {}), got {}",
                              number_of_nodes(),
                              v);
    }
  }

  std::string to_human_readable_repr(Forest const& f) {
    using detail::group_digits;
    size_t const num_roots
        = std::count(f.parents().begin(), f.parents().end(), UNDEFINED);
    return fmt::format("<Forest with {} nodes, {} edges, and {} root{}>",
                       group_digits(f.number_of_nodes()),
                       group_digits(f.parents().size()),
                       group_digits(num_roots),
                       num_roots == 1 ? "" : "s");
  }

  ////////////////////////////////////////////////////////////////////////
  // Forest::const_iterator_path nested class
  ////////////////////////////////////////////////////////////////////////

  Forest::const_iterator_path::value_type
  Forest::const_iterator_path::operator*() const {
    if (_current_node != UNDEFINED) {
      return _forest->label_no_checks(_current_node);
    }
    return UNDEFINED;
  }

  Forest::const_iterator_path& Forest::const_iterator_path::operator++() {
    if (_current_node != UNDEFINED
        && !forest::is_root_no_checks(*_forest, _current_node)) {
      // NOTE: This is a bit more complicated than might be expected
      // because we use _current_node == UNDEFINED to denote the end of the
      // range. This way we don't have to know/compute the root of
      // _current_node in cend_path_to_root_no_checks. So, an non-"end"
      // iterator always has _current_node set to a value != UNDEFINED.
      _current_node = _forest->parent_no_checks(_current_node);
    }
    return *this;
  }

  bool Forest::const_iterator_path::operator==(
      const_iterator_path const& that) const {
    if (_forest != that._forest) {
      return false;
    } else if (_current_node == that._current_node) {
      return true;
    } else {
      // NOTE: This is a bit more complicated than might be expected
      // because we use _current_node == UNDEFINED to denote the end of the
      // range. This way we don't have to know/compute the root of
      // _current_node in cend_path_to_root_no_checks. So, an non-"end"
      // iterator always has _current_node set to a value != UNDEFINED. This
      // means that checking equality of (non-"end", non-"end") or ("end",
      // "end") iterators works as expected, but comparing (non-"end",
      // "end") and ("end", non-"end") is the next check. Without something
      // like this UNDEFINED is part of the range defined by
      // [cbegin_path_to_root_no_checks, cend_path_to_root_no_checks).
      return **this == *that;
    }
  }

  void Forest::throw_if_not_acyclic(node_type new_node,
                                    node_type new_parent) const {
    size_t const        not_yet_seen = number_of_nodes();
    std::vector<size_t> seen(number_of_nodes(), not_yet_seen);
    size_t              current = 0;

    for (Forest::node_type m = 0; m != number_of_nodes(); ++m) {
      auto      n      = m;
      node_type p      = UNDEFINED;
      size_t    length = 0;
      while (n != UNDEFINED && seen[n] == not_yet_seen) {
        seen[n] = current;
        p       = n;
        n       = parent(n);
        length++;
      }
      if (n != UNDEFINED && seen[n] == current) {
        std::string cycle;
        if (length == 1) {
          cycle = fmt::format("loop [{}]", n);
        } else if (length == 2) {
          cycle = fmt::format("cycle [{}, {}]", n, parent(n));
        } else {
          cycle = fmt::format(
              "cycle [{}, {}, ..., {}] (length {})", n, parent(n), p, length);
        }
        if (new_node != UNDEFINED && new_parent != UNDEFINED) {
          LIBSEMIGROUPS_EXCEPTION("defining the parent of node {} to be {} "
                                  "creates a {}",
                                  new_node,
                                  new_parent,
                                  cycle);
        } else {
          LIBSEMIGROUPS_EXCEPTION("the Forest object contains the {}"
                                  " and is invalid",
                                  cycle);
        }
      }
      current++;
    }
  }

  Forest& Forest::set_parent_and_label(node_type  node,
                                       node_type  parent,
                                       label_type gen) {
    throw_if_node_out_of_bounds(node);
    throw_if_node_out_of_bounds(parent);

    if (parent == node) {
      LIBSEMIGROUPS_EXCEPTION("a node cannot be its own parent, attempted to "
                              "set {} as the parent of {}",
                              parent,
                              node);
    }

    node_type  previous_parent = parent_no_checks(node);
    label_type previous_label  = label_no_checks(node);
    set_parent_and_label_no_checks(node, parent, gen);

    try {
      throw_if_not_acyclic(node, parent);
    } catch (LibsemigroupsException const& e) {
      set_parent_and_label_no_checks(node, previous_parent, previous_label);
      throw e;
    }
    return *this;
  }

  ////////////////////////////////////////////////////////////////////////
  // Forest helpers
  ////////////////////////////////////////////////////////////////////////

  namespace forest {
    void path_to_root_no_checks(Forest const&     f,
                                word_type&        w,
                                Forest::node_type i) {
      // The next check is a bit too heavy weight, so commented out. Leaving it
      // here in case of thinking about doing this again in the future
      // LIBSEMIGROUPS_ASSERT(is_forest(f));
      LIBSEMIGROUPS_ASSERT(i < f.number_of_nodes());
      f.path_to_root_no_checks(std::back_inserter(w), i);
    }

    void path_from_root_no_checks(Forest const&     f,
                                  word_type&        w,
                                  Forest::node_type i) {
      // The next check is a bit too heavy weight, so commented out. Leaving it
      // here in case of thinking about doing this again in the future
      // LIBSEMIGROUPS_ASSERT(is_forest(f));
      LIBSEMIGROUPS_ASSERT(i < f.number_of_nodes());
      f.path_from_root_no_checks(std::back_inserter(w), i);
    }

    word_type path_to_root_no_checks(Forest const& f, Forest::node_type i) {
      word_type w;
      path_to_root_no_checks(f, w, i);
      return w;
    }

    word_type path_from_root_no_checks(Forest const& f, Forest::node_type i) {
      word_type w;
      path_from_root_no_checks(f, w, i);
      return w;
    }

    void path_to_root(Forest const& f, word_type& w, Forest::node_type i) {
      f.path_to_root(std::back_inserter(w), i);
    }

    void path_from_root(Forest const& f, word_type& w, Forest::node_type i) {
      f.path_from_root(std::back_inserter(w), i);
    }

    [[nodiscard]] word_type path_to_root(Forest const& f, Forest::node_type i) {
      word_type w;
      path_to_root(f, w, i);
      return w;
    }

    [[nodiscard]] word_type path_from_root(Forest const&     f,
                                           Forest::node_type i) {
      word_type w;
      path_from_root(f, w, i);
      return w;
    }

    size_t depth_no_checks(Forest const& f, Forest::node_type i) {
      // The next check is a bit too heavy weight, so commented out. Leaving it
      // here in case of thinking about doing this again in the future
      // LIBSEMIGROUPS_ASSERT(is_forest(f));
      LIBSEMIGROUPS_ASSERT(i < f.number_of_nodes());
      size_t length = 0;
      for (; f.parent_no_checks(i) != UNDEFINED; ++length) {
        LIBSEMIGROUPS_ASSERT(i != f.parent_no_checks(i));
        i = f.parent_no_checks(i);
      }
      return length;
    }

    [[nodiscard]] Forest::label_type max_label(Forest const& f) {
      if (!f.empty()) {
        Forest::label_type max               = 0;
        bool               any_label_defined = false;
        for (auto const& label : f.labels()) {
          if (label != UNDEFINED && label >= max) {
            any_label_defined = true;
            max               = label;
          }
        }
        if (any_label_defined) {
          return max;
        }
      }
      return UNDEFINED;
    }

    bool is_forest(Forest const& f) {
      size_t const        not_yet_seen = f.number_of_nodes();
      std::vector<size_t> seen(f.number_of_nodes(), not_yet_seen);
      size_t              current = 0;

      for (Forest::node_type m = 0; m != f.number_of_nodes(); ++m) {
        auto n = m;
        while (n != UNDEFINED && seen[n] == not_yet_seen) {
          seen[n] = current;
          n       = f.parent(n);
        }
        if (n != UNDEFINED && seen[n] == current) {
          return false;
        }
        current++;
      }
      return true;
    }

    ////////////////////////////////////////////////////////////////////////
    // detail::PathsFromToRootsCommon class
    ////////////////////////////////////////////////////////////////////////

    namespace detail {
      PathsFromToRootsCommon::size_type
      PathsFromToRootsCommon::depth(node_type n) {
        while (!is_root_no_checks(*_forest, n) && _depths[n] == UNDEFINED) {
          _visited.push_back(n);
          n = _forest->parent(n);
        }
        if (is_root_no_checks(*_forest, n)) {
          _depths[n] = 0;
        }
        auto known_depth = _depths[n];
        while (!_visited.empty()) {
          n          = _visited.back();
          _depths[n] = ++known_depth;
          _visited.pop_back();
        }
        return _depths[n];
      }

      PathsFromToRootsCommon::PathsFromToRootsCommon(Forest const& forest)
          : _current_node(0),
            _current_path(),
            _depths(forest.number_of_nodes(),
                    static_cast<size_type>(UNDEFINED)),
            _forest(&forest),
            _visited() {
        LIBSEMIGROUPS_ASSERT(forest::is_forest(*_forest));
      }

      PathsFromToRootsCommon&
      PathsFromToRootsCommon::init(Forest const& forest) {
        _current_node = 0;
        _current_path.clear();
        _depths.resize(forest.number_of_nodes(),
                       static_cast<size_type>(UNDEFINED));
        _forest = &forest;
        LIBSEMIGROUPS_ASSERT(forest::is_forest(*_forest));
        return *this;
      }
    }  // namespace detail

    ////////////////////////////////////////////////////////////////////////
    // PathsFromRoots class
    ////////////////////////////////////////////////////////////////////////

    void PathsFromRoots::next() {
      if (at_end()) {
        return;
      }
      auto next_node = _current_node + 1;
      if (_current_node < _forest->number_of_nodes() - 1) {
        auto   next_anc = next_node, current_anc = _current_node;
        size_t pos = _current_path.size() - 1;

        // We aim to find the least (youngest) common ancestor of
        // _current_node and next_node so that we can reuse the prefix of the
        // _current_path. We start by equalising the depth.
        if (depth(next_anc) > depth(current_anc)) {
          // _current_path will have more letters when complete for
          // next_node, so we resize
          _current_path.resize(_current_path.size() + depth(next_anc)
                               - depth(current_anc));
          // We insert the word path from next_node to next_anc into
          // _current_path from the back to the front, because we are reading
          // it in reverse.
          size_t i = _current_path.size();
          do {
            _current_path[--i] = _forest->label_no_checks(next_anc);
            next_anc           = _forest->parent_no_checks(next_anc);
          } while (depth(next_anc) > depth(current_anc));
        } else if (depth(current_anc) > depth(next_anc)) {
          // _current_path will have fewer letters when complete for
          // next_node, so we erase
          _current_path.erase(_current_path.end() - depth(current_anc)
                                  + depth(next_anc),
                              _current_path.end());
          pos -= depth(current_anc) - depth(next_anc);
          do {
            current_anc = _forest->parent_no_checks(current_anc);
          } while (depth(current_anc) > depth(next_anc));
        }

        while (current_anc != next_anc
               && !is_root_no_checks(*_forest, current_anc)) {
          _current_path[pos--] = _forest->label_no_checks(next_anc);
          current_anc          = _forest->parent_no_checks(current_anc);
          next_anc             = _forest->parent_no_checks(next_anc);
        }
        LIBSEMIGROUPS_ASSERT(_current_path.size() == depth(next_node));
      }
      _current_node = next_node;
    }

    PathsFromRoots& PathsFromRoots::skip_n(size_type n) {
      if (n == 1) {
        next();
      } else if (n != 0) {
        _current_node += n;
        _current_path.clear();
        if (!at_end()) {
          // Can't use next here because it assumes that _current_path
          // contains the path to (_current_node - 1) which it does not.
          _forest->path_from_root_no_checks(std::back_inserter(_current_path),
                                            _current_node);
        }
      }
      return *this;
    }

    ////////////////////////////////////////////////////////////////////////
    // PathsToRoots class
    ////////////////////////////////////////////////////////////////////////

    void PathsToRoots::next() {
      if (at_end()) {
        return;
      }
      auto next_node = _current_node + 1;
      if (_current_node < _forest->number_of_nodes() - 1) {
        auto   next_anc = next_node, current_anc = _current_node;
        size_t pos = 0;

        // We aim to find the least (youngest) common ancestor of
        // _current_node and next_node so that we can reuse the prefix of
        // the _current_path. We start by equalising the depth.
        if (depth(next_anc) > depth(current_anc)) {
          // _current_path will have more letters when complete for
          // next_node, so we resize
          size_t old_size = _current_path.size();
          _current_path.resize(old_size + depth(next_anc) - depth(current_anc));
          std::copy(_current_path.begin(),
                    _current_path.begin() + old_size,
                    _current_path.begin() + depth(next_anc)
                        - depth(current_anc));
          // We insert the word path from next_node to next_anc into
          // _current_path from the front to the back.
          do {
            _current_path[pos++] = _forest->label_no_checks(next_anc);
            next_anc             = _forest->parent_no_checks(next_anc);
          } while (depth(next_anc) > depth(current_anc));
        } else if (depth(current_anc) > depth(next_anc)) {
          // _current_path will have fewer letters when complete for
          // next_node, so we erase
          _current_path.erase(_current_path.begin(),
                              _current_path.begin() + depth(current_anc)
                                  - depth(next_anc));
          do {
            current_anc = _forest->parent_no_checks(current_anc);
          } while (depth(current_anc) > depth(next_anc));
        }
        while (current_anc != next_anc
               && !is_root_no_checks(*_forest, current_anc)) {
          _current_path[pos++] = _forest->label_no_checks(next_anc);
          current_anc          = _forest->parent_no_checks(current_anc);
          next_anc             = _forest->parent_no_checks(next_anc);
        }
        LIBSEMIGROUPS_ASSERT(_current_path.size() == depth(next_node));
      }
      _current_node = next_node;
    }

    PathsToRoots& PathsToRoots::skip_n(size_type n) {
      if (n == 1) {
        next();
      } else if (n != 0) {
        _current_node += n;
        _current_path.clear();
        if (!at_end()) {
          // Can't use next here because it assumes that _current_path
          // contains the path to (_current_node - 1) which it does not.
          _forest->path_to_root_no_checks(std::back_inserter(_current_path),
                                          _current_node);
        }
      }
      return *this;
    }

    ////////////////////////////////////////////////////////////////////////
    // dot functions
    ////////////////////////////////////////////////////////////////////////

    Dot dot(Forest const& f) {
      std::vector<std::string> labels;
      size_t const             N = forest::max_label(f) + 1;
      for (size_t i = 0; i < N; ++i) {
        labels.emplace_back(fmt::format("{}", i));
      }
      return dot(f, labels);
    }

    Dot dot(Forest const& f, std::vector<std::string> const& labels) {
      static std::string const empty = "\u03B5";

      if (!labels.empty() && labels.size() != forest::max_label(f) + 1) {
        LIBSEMIGROUPS_EXCEPTION(
            "the 2nd argument <labels> (a vector of strings) must have "
            "length {} or 0, but found {}",
            forest::max_label(f) + 1,
            labels.size());
      }
      Dot result;
      result.add_attr("rankdir", "BT");
      result.name("Forest").kind(Dot::Kind::digraph);

      for (Forest::node_type n = 0; n < f.number_of_nodes(); ++n) {
        auto& node = result.add_node(n).add_attr("shape", "box");
        if (!labels.empty()) {
          std::string label;
          if (!forest::is_root_no_checks(f, n)) {
            auto first = f.cbegin_path_to_root_no_checks(n),
                 last  = f.cend_path_to_root_no_checks(n);
            for (auto it = first; it != last; ++it) {
              label += labels.at(*it);
            }
          } else {
            label = empty;
          }
          node.add_attr("label", fmt::format("{}: {}", n, label));
        }
      }
      for (Forest::node_type n = 0; n < f.number_of_nodes(); ++n) {
        if (!forest::is_root_no_checks(f, n)) {
          result.add_edge(n, f.parent_no_checks(n))
              .add_attr(
                  "color",
                  result.colors[f.label_no_checks(n) % result.colors.size()]);
        }
      }
      return result;
    }

  }  // namespace forest

  std::string to_human_readable_repr(forest::PathsToRoots const& ptr,
                                     std::string const&          sep) {
    return fmt::format("<forest{}PathsToRoots in {} with current target {}>",
                       sep,
                       to_human_readable_repr(ptr.forest()),
                       ptr.target());
  }

  std::string to_human_readable_repr(forest::PathsFromRoots const& pfr,
                                     std::string const&            sep) {
    return fmt::format("<forest{}PathsFromRoots in {} with current target {}>",
                       sep,
                       to_human_readable_repr(pfr.forest()),
                       pfr.target());
  }

}  // namespace libsemigroups
