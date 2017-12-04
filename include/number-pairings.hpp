#ifndef _NUMBER_PAIRINGS_HPP_
#define _NUMBER_PAIRINGS_HPP_

#include <cassert>
#include <cmath>
#include <vector>

// note: this is under construction

namespace number_pairings {

// helpers
template<typename T>
T tn( T x ) { return x * ( x + 1 ) / 2; }
template<typename T>
T tr( T x ) { return floor( ( sqrt( 1 + x * 8 ) - 1 ) / 2 ); }
template<typename T>
T ext( T x ) { return x - tn( tr( x ) ); }

template<typename T>
struct pairing {
  virtual T join( const std::vector<T>& xy ) = 0;
  virtual std::vector<T> split( T z ) = 0;
  virtual std::vector<T> bounds() = 0;
};

template<typename T>
struct Cantor : public pairing<T> {
  T join( const std::vector<T>& xy  ) {
    assert( xy.size() == 2);
    return tn( xy[0] + xy[1] ) + xy[1];
  }
  std::vector<T> split( T z ) {
    T t = tr( z );
    return {
      ( t * ( t + 3 ) / 2 ) - z,
      z - ( ( t * ( t + 1 ) ) / 2 )
    };
  }
  std::vector<T> bounds() { return { 0, 0, 0 }; }
};

template<typename T>
struct elegant : public pairing<T> {
  T join( const std::vector<T>& xy ) {
    assert( xy.size() == 2);
    if ( xy[1] >= xy[0] )
      return xy[1] * ( xy[1] + 1 ) + xy[0];
    else
      return xy[0] * xy[0] + xy[1];
  }
  std::vector<T> split( T z ) {
    T sq_z = T( floor(sqrt( z )) );
    if ( sq_z * sq_z > z ) sq_z--;
    T t = z - sq_z * sq_z;
    if (t < sq_z) return { sq_z, t };
    else return { t - sq_z, sq_z };
  }
  std::vector<T> bounds() { return { 0, 0, 0 }; }
};

// power of two pairing
template<typename T>
struct poto : public pairing<T> {
  T join( const std::vector<T>& xy ) {
    assert( xy.size() == 2);
    return std::pow( 2, xy[0] ) * ( 2 * xy[1] + 1 ) - 1;
  }
  std::vector<T> split( T z ) {
    T _z = z + 1;
    for (T i = 0; i < _z; ++i) {
      T p = T( floor(pow(2, i)) );
      T q = _z / p;
      if (q % 2 == 1) {
        return { i, T( floor(q / 2) ) };
      }
    }
    assert(false);
  }
  std::vector<T> bounds() { return { 0, 0, 0 }; }
};


// half pairing (only x <= y pairs)
template<typename T>
struct half : public pairing<T> {
  T join( const std::vector<T>& xy ) {
    assert( xy.size() == 2);
    return tn( std::max( xy[0], xy[1] ) ) + std::min( xy[0], xy[1] );
  }
  std::vector<T> split( T z ) { return { ext( z ), tr( z ) }; }
  std::vector<T> bounds() { return { 0, 0, 0 }; }
};

// finite field (matrix) of two dimensions
template<typename T>
struct field : public pairing<T> {
  field( T sx, T sy ) {
    _bounds = { sx, sy, sx * sy };
  }
  T join( const std::vector<T>& xy ) {
    assert( xy.size() == 2);
    assert( xy[0] < _bounds[0] && xy[1] < _bounds[1] );
    return _bounds[0] * xy[1] + xy[0] % _bounds[0];
  }
  std::vector<T> split( T z ) {
    assert( z < _bounds[2] );
    return { z % _bounds[0], T( floor( z / _bounds[0] ) ) };
  }
  std::vector<T> bounds() { return _bounds; }
private:
  std::vector<T> _bounds;
};

// x is infinite and y is finite
// sy: size in y
template<typename T>
struct stack_x : public pairing<T> {
  stack_x( T sy ) {
    _bounds = { 0, sy, 0 };
  }
  T join( const std::vector<T>& xy ) {
    assert( xy[1] < _bounds[1] );
    return xy[1] % _bounds[1] + _bounds[1] * xy[0];
  }
  std::vector<T> split( T z ) {
    return { T( floor( z / _bounds[1] ) ), z % _bounds[1] };
  }
  std::vector<T> bounds() { return _bounds; }
private:
  std::vector<T> _bounds;
};

// x is finite y is infinite
// sx: size in x
template<typename T>
struct stack_y : public pairing<T> {
  stack_y( T sx ) {
    _bounds = { sx, 0, 0 };
  }
  T join( const std::vector<T>& xy ) {
    assert( xy[0] < _bounds[0] );
    return _bounds[0] * xy[1] + xy[0] % _bounds[0];
  }
  std::vector<T>split( T z ) {
    return { z % _bounds[0], T( floor( z / _bounds[0] ) ) };
  }
  std::vector<T> bounds() { return _bounds; }
private:
  std::vector<T> _bounds;
};

// selection
template<typename T>
std::shared_ptr<pairing<T>> select(
    T x,
    T y,
    std::shared_ptr<pairing<T>> iip = std::shared_ptr<pairing<T>>(new Cantor<T>) ) {
  if( x == 0 && y == 0 ) return std::shared_ptr<pairing<T>>( iip );
  else if( x == 0 ) return std::shared_ptr<pairing<T>>( new stack_x<T>( y ) );
  else if( y == 0 ) return std::shared_ptr<pairing<T>>( new stack_y<T>( x ) );
  else return std::shared_ptr<pairing<T>>( new field<T>( x, y ) );
}

// composition operator
template<typename T>
struct composition : public pairing<T> {
  composition(
      const std::vector<T>& l,
      std::shared_ptr<pairing<T>> iip = std::shared_ptr<pairing<T>>(new Cantor<T>)
    ) {
    _pairings = { select( l[l.size()-2], l[l.size()-1] ) };
    if( l.size() > 2 ) {
      for( ssize_t i = l.size()-3; i >= 0; i-- ) {
        std::vector<std::shared_ptr<pairing<T>>> new_pairing( 1, select<T>( l[i], _pairings[0]->bounds()[2], iip ) );
        _pairings.insert(_pairings.begin(), new_pairing.begin(), new_pairing.end());
      }
    }
    _bounds = l;
    _bounds.push_back( _pairings[0]->bounds()[2] );
  }
  T join( const std::vector<T>& l ) {
    assert( l.size()==_bounds.size()-1 );
    for( size_t i=0; i<_bounds.size()-1; i++) {
      assert( l[i] < _bounds[i] || _bounds[i] == 0 );
    }
    T n = _pairings[l.size()-2]->join( { l[l.size()-2], l[l.size()-1] } );
    if( l.size() > 2 ) {
      for( ssize_t i=l.size()-3; i>=0; i-- ) {
        n = _pairings[i]->join( { l[i], n } );
      }
    }
    return n;
  }
  std::vector<T> split( T z ) {
    auto xy = _pairings[0]->split( z );
    std::vector<T> l = { xy[0] };
    if( _pairings.size() > 1 ) {
      for( size_t k = 1; k<_pairings.size(); k++ ) {
        xy = _pairings[k]->split( xy[1] );
        l.push_back( xy[0] );
      }
    }
    l.push_back( xy[1] );
    return l;
  }
  std::vector<T> bounds() { return _bounds; }
private:
  std::vector<std::shared_ptr<pairing<T>>> _pairings;
  std::vector<T> _bounds;
};

}

#endif // ifndef _NUMBER_PAIRINGS_HPP_
