// Copyright (c) Jens Kubacki

// make:
// g++ -I../include -o test_np test_main.cpp
// clang++-3.9 -I../include -o test_np test_main.cpp -std=c++14

#include <iostream>
#include <memory>
#include <number-pairings.hpp>

using namespace std;
using namespace number_pairings;

typedef uint64_t int_t;

int main() {

  auto pairings = {
    shared_ptr<pairing<int_t>>(new Cantor<int_t>),
    shared_ptr<pairing<int_t>>(new elegant<int_t>),
    shared_ptr<pairing<int_t>>(new poto<int_t>),
    shared_ptr<pairing<int_t>>(new half<int_t>),
    shared_ptr<pairing<int_t>>(new field<int_t>( 10 + 1, 34 + 1 )),
    shared_ptr<pairing<int_t>>(new stack_x<int_t>( 50 )),
    shared_ptr<pairing<int_t>>(new stack_y<int_t>( 50 )),
    select<int_t>( 0, 0 ),
    select<int_t>( 0, 50 ),
    select<int_t>( 50, 0),
    select<int_t>( 50, 50 ),
    shared_ptr<pairing<int_t>>(new composition<int_t>( { 0, 0 } )),
    shared_ptr<pairing<int_t>>(new composition<int_t>( { 0, 50 } )),
    shared_ptr<pairing<int_t>>(new composition<int_t>( { 50, 0 } )),
    shared_ptr<pairing<int_t>>(new composition<int_t>( { 50, 50 } ))
  };

  for( auto p : pairings ) {
    int_t x = 10, y = 34;
    int_t z = p->join( { x, y } );
    auto xy = p->split( z );
    cout << x << " " << y << " => " << z << " => " << xy[0] << " " <<  xy[1] << endl;
  }

  auto c = shared_ptr<pairing<int_t>>(new composition<int_t>( { 2, 3, 4, 5, 6, 7 } ));
  // auto b = c->bounds();
  // for( auto _b : b ) {
  //  cout <<  _b << " ";
  // }
  cout << c->join( { 0, 1, 2, 3, 4, 5 } ) << endl;
  auto s = c->split( 4166 );
  cout << s[0] << " " << s[1] << " " << s[2] << " " << s[3] << " " << s[4] << " " << s[5] << endl;
  cout << c->join( { 1, 0, 0, 0, 0, 0 } ) << endl;
  cout << c->join( { 0, 1, 0, 0, 0, 0 } ) << endl;
  cout << c->join( { 0, 0, 1, 0, 0, 0 } ) << endl;
  cout << c->join( { 0, 0, 0, 1, 0, 0 } ) << endl;
  cout << c->join( { 0, 0, 0, 0, 1, 0 } ) << endl;
  cout << c->join( { 0, 0, 0, 0, 0, 1 } ) << endl;
  return 0;
}
