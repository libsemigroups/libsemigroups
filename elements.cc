/*******************************************************************************
 * Semigroups++
 *
 * This file contains classes for creating elements of a semigroup.
 *
 *******************************************************************************/

#include "elements.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// BooleanMat
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

size_t BooleanMat::complexity () const {
  return pow(this->degree(), 3);
}

size_t BooleanMat::degree () const {
  return sqrt(_vector->size());
}

size_t BooleanMat::hash_value () const {
  size_t seed = 0;
  for (size_t i = 0; i < _vector->size(); i++) {
    seed = ((seed << 1) + _vector->at(i));
  }
  return seed;
}

Element* BooleanMat::identity () const {
  std::vector<bool>* matrix(new std::vector<bool>());
  matrix->resize(_vector->size(), false);
  for (size_t i = 0; i < this->degree(); i++) {
    matrix->at(i * this->degree() + i) =  true;
  }
  return new BooleanMat(matrix);
}

// multiply x and y into this
void BooleanMat::redefine (Element const* x,
                           Element const* y) {
  assert(x->degree() == y->degree());
  assert(x->degree() == this->degree());
  
  size_t k;
  size_t dim = this->degree();
  std::vector<bool>* xx(static_cast<BooleanMat const*>(x)->_vector);
  std::vector<bool>* yy(static_cast<BooleanMat const*>(y)->_vector);

  for (size_t i = 0; i < dim; i++) {
    for (size_t j = 0; j < dim; j++) {
      for (k = 0; k < dim; k++) {
        if (xx->at(i * dim + k) && yy->at(k * dim + j)) {
          break;
        }
      }
      _vector->at(i * dim + j) =  (k < dim);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Bipartition
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

u_int32_t Bipartition::block (size_t pos) const {
  return _vector->at(pos);
}

size_t Bipartition::complexity () const {
  return pow(_vector->size(), 2);
}

size_t Bipartition::degree () const {
  return _vector->size() / 2;
}

size_t Bipartition::hash_value () const {
  size_t seed = 0;
  for (size_t i = 0; i < _vector->size(); i++) {
    seed = ((seed * _vector->size()) + _vector->at(i));
  }
  return seed;
}

// the identity of this
Element* Bipartition::identity () const {
  std::vector<u_int32_t>* blocks(new std::vector<u_int32_t>());
  blocks->reserve(this->_vector->size());
  for (size_t j = 0; j < 2; j++) {
    for (u_int32_t i = 0; i < this->degree(); i++) {
      blocks->push_back(i);
    }
  }
  return new Bipartition(blocks);
}

// multiply x and y into this
void Bipartition::redefine (Element const* x, Element const* y) {
  assert(x->degree() == y->degree());
  assert(x->degree() == this->degree());
  u_int32_t n = this->degree();

  Bipartition const* xx = static_cast<Bipartition const*>(x);
  Bipartition const* yy = static_cast<Bipartition const*>(y);

  std::vector<u_int32_t>* xblocks(xx->_vector);
  std::vector<u_int32_t>* yblocks(yy->_vector);

  u_int32_t nrx(xx->nrblocks());
  u_int32_t nry(yy->nrblocks());

  std::vector<u_int32_t> fuse;
  std::vector<u_int32_t> lookup;
  fuse.reserve(nrx + nry);
  lookup.reserve(nrx + nry);

  // maybe this should be pointer to local data, may slow down hashing
  // but speed up redefinition?
  for (size_t i = 0; i < nrx + nry; i++) {
    fuse.push_back(i);
    lookup.push_back(-1);
  }

  for (size_t i = 0; i < n; i++) {
    u_int32_t j = fuseit(fuse, xblocks->at(i + n));
    u_int32_t k = fuseit(fuse, yblocks->at(i) + nrx);
    if (j != k) {
      if (j < k) {
        fuse.at(k) = j;
      } else {
        fuse.at(j) = k;
      }
    }
  }

  u_int32_t next = 0;

  for (size_t i = 0; i < n; i++) {
    u_int32_t j = fuseit(fuse, xblocks->at(i));
    if (lookup.at(j) == (u_int32_t) -1) {
      lookup.at(j) = next;
      next++;
    }
    this->_vector->at(i) = lookup.at(j);
  }

  for (size_t i = n; i < 2 * n; i++) {
    u_int32_t j = fuseit(fuse, yblocks->at(i) + nrx);
    if (lookup.at(j) == (u_int32_t) -1) {
      lookup.at(j) = next;
      next++;
    }
    this->_vector->at(i) = lookup.at(j);
  }
}

inline u_int32_t Bipartition::fuseit (std::vector<u_int32_t> const& fuse, 
                                      u_int32_t pos                      ) {
  while (fuse.at(pos) < pos) {
    pos = fuse.at(pos);
  }
  return pos;
}

// nr blocks

u_int32_t Bipartition::nrblocks () const {
  size_t nr = 0;
  for (auto x: *_vector) {
    if (x > nr) {
      nr = x;
    }
  }
  return nr + 1;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Matrices over semirings
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Semiring* MatrixOverSemiring::semiring () const {
  return _semiring;
}

void MatrixOverSemiring::set_semiring (Semiring* semiring) {
  assert(_semiring == nullptr);
 _semiring = semiring;
}

size_t MatrixOverSemiring::complexity () const {
  return pow(this->degree(), 3);
}

size_t MatrixOverSemiring::degree () const {
  return sqrt(_vector->size());
}

size_t MatrixOverSemiring::hash_value () const {
  size_t seed = 0;
  for (size_t i = 0; i < _vector->size(); i++) {
    seed = ((seed << 4) + _vector->at(i));
  }
  return seed;
}

// the identity
Element* MatrixOverSemiring::identity () const {
  std::vector<long>* matrix(new std::vector<long>());
  matrix->resize(_vector->size(), _semiring->zero());

  size_t n = this->degree();
  for (size_t i = 0; i < n; i++) {
    matrix->at(i * n + i) = _semiring->one();
  }
  return new MatrixOverSemiring(matrix, _semiring);
}

Element* MatrixOverSemiring::really_copy (size_t increase_degree_by) const {
  MatrixOverSemiring*
    copy(static_cast<MatrixOverSemiring*>(
          ElementWithVectorData::really_copy(increase_degree_by)));
  copy->set_semiring(_semiring);
  return copy;
}

void MatrixOverSemiring::redefine (Element const* x,
                                   Element const* y) {

  MatrixOverSemiring const* xx(static_cast<MatrixOverSemiring const*>(x));
  MatrixOverSemiring const* yy(static_cast<MatrixOverSemiring const*>(y));

  assert(xx->degree() == yy->degree());
  assert(xx->degree() == this->degree());
  size_t deg = this->degree();

  for (size_t i = 0; i < deg; i++) {
    for (size_t j = 0; j < deg; j++) {
      long v = _semiring->zero();
      for (size_t k = 0; k < deg; k++) {
        v = _semiring->plus(v, _semiring->prod(xx->at(i * deg + k), 
                                               yy->at(k * deg + j)));
      }
      _vector->at(i * deg + j) = v;
    }
  }
  after(); // post process this
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Projective max-plus matrices
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

size_t ProjectiveMaxPlusMatrix::hash_value () const {

  size_t seed = 0;
  for (size_t i = 0; i < _vector->size(); i++) {
    seed = ((seed << 4) + _vector->at(i));
  }
  return seed;
}

void ProjectiveMaxPlusMatrix::after () {
  long   norm = *std::max_element(_vector->begin(), _vector->end());
  size_t deg  = pow(this->degree(), 2);

  for (size_t i = 0; i < deg; i++) {
    if (_vector->at(i) != LONG_MIN) {
      _vector->at(i) -= norm;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Partitioned binary relations (PBRs)
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

size_t PBR::complexity () const {
  return pow((2 * this->degree()), 3);
}

size_t PBR::degree () const {
  return _vector->size() / 2;
}

size_t PBR::hash_value () const {
  size_t seed = 0;
  size_t pow = 101;
  for (size_t i = 0; i < this->degree(); i++) {
    for (size_t j = 0; j < this->at(i).size(); j++) { 
      seed = (seed * pow) + this->at(i).at(j);
    }
  }
  return seed;
}

Element* PBR::identity () const {
  std::vector<std::vector<u_int32_t> >* 
    adj(new std::vector<std::vector<u_int32_t> >());
  size_t n = this->degree();
  adj->reserve(2 * n);
  for (u_int32_t i = 0; i < 2 * n; i++) {
    adj->push_back(std::vector<u_int32_t>());
  }
  for (u_int32_t i = 0; i < n; i++) {
    adj->at(i).push_back(i + n);
    adj->at(i + n).push_back(i);
  }
  return new PBR(adj);
}

//FIXME this allocates lots of memory on every call, maybe better to keep
//the data in the class and overwrite it.
//FIXME also we repeatedly search in the same part of the graph, and so
//there is probably a lot of repeated work in the dfs. Better use some version
//of Floyd-Warshall
void PBR::redefine (Element const* xx, 
                    Element const* yy ) {
  assert(xx->degree() == yy->degree());
  assert(xx->degree() == this->degree());

  PBR const* x(static_cast<PBR const*>(xx));
  PBR const* y(static_cast<PBR const*>(yy));
   
  u_int32_t n = this->degree();

  for (size_t i = 0; i < 2 * n; i++) {
    (*_vector)[i].clear();
  }

  std::vector<bool> x_seen;
  std::vector<bool> y_seen;
  x_seen.resize(2 * n, false);
  y_seen.resize(2 * n, false);

  for (size_t i = 0; i < n; i++) {
    x_dfs(n, i, i, x_seen, y_seen, x, y);
    for (size_t j = 0; j < 2 * n; j++) {
      x_seen.at(j) = false;
      y_seen.at(j) = false;
    }
  }

  for (size_t i = n; i < 2 * n; i++) {
    y_dfs(n, i, i, x_seen, y_seen, x, y);
    for (size_t j = 0; j < 2 * n; j++) {
      x_seen.at(j) = false;
      y_seen.at(j) = false;
    }
  }
}

// add vertex2 to the adjacency of vertex1
void PBR::add_adjacency (size_t vertex1, size_t vertex2) {
  auto it = std::lower_bound(_vector->at(vertex1).begin(),
                             _vector->at(vertex1).end(), 
                             vertex2);
  if (it == _vector->at(vertex1).end()) {
    _vector->at(vertex1).push_back(vertex2);
  } else if ((*it) != vertex2) {
    _vector->at(vertex1).insert(it, vertex2);
  }
}

void PBR::x_dfs (u_int32_t          n,
                 u_int32_t          i, 
                 u_int32_t          v,         // the vertex we're currently doing
                 std::vector<bool>& x_seen,
                 std::vector<bool>& y_seen,
                 PBR const*         x, 
                 PBR const*         y      ) {
  
  if (!x_seen.at(i)) {
    x_seen.at(i) = true;
    for (auto j: x->at(i)) {
      if (j < n) {
        add_adjacency(v, j);
      } else {
        y_dfs(n, j - n, v, x_seen, y_seen, x, y);
      }
    }
  }
}

void PBR::y_dfs (u_int32_t          n,
                 u_int32_t          i, 
                 u_int32_t          v,         // the vertex we're currently doing
                 std::vector<bool>& x_seen,
                 std::vector<bool>& y_seen,
                 PBR const*         x, 
                 PBR const*         y      ) {

  if (!y_seen.at(i)) {
    y_seen.at(i) = true;
    for (auto j: y->at(i)) {
      if (j >= n) {
        add_adjacency(v, j);
      } else {
        x_dfs(n, j + n, v, x_seen, y_seen, x, y);
      }
    }
  }
}
