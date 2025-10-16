//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020-2025 James D. Mitchell
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
      auto      n = m;
      node_type p;
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
      f.throw_if_not_acyclic();
      f.throw_if_node_out_of_bounds(i);
      path_to_root_no_checks(f, w, i);
    }

    void path_from_root(Forest const& f, word_type& w, Forest::node_type i) {
      f.throw_if_not_acyclic();
      f.throw_if_node_out_of_bounds(i);
      path_from_root_no_checks(f, w, i);
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

  }  // namespace forest
}  // namespace libsemigroups
