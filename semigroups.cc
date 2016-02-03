/*******************************************************************************
 * Semigroups++
 *
 * This file contains classes for creating elements of a semigroup.
 *
 *******************************************************************************/

#include "semigroups.h"

void Semigroup::max_word_length_by_rank (std::vector<size_t>& result, bool report) {

  enumerate(-1, report);

  result.resize(_degree, 0);
  size_t pos = _elements->size() - 1;
  size_t len = _lenindex.size() - 1; // not sure if this is right
  while (len > 0 && std::find(result.begin(), result.end(), 0) != result.end()) {
    while (pos >= _lenindex[len]) {
      size_t r = (*_elements)[pos]->rank() - 1;
      if (result[r] == 0) {
        result[r] = len;
      }
      pos--;
    }
    len--;
  }
}
