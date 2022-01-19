// $Id: ubigint.h,v 1.4 2022-01-11 15:58:58-08 - - $
//James Garrett jaagarre
//Nick Kornienko nkornien


#ifndef UBIGINT_H
#define UBIGINT_H

#include <exception>
#include <iostream>
#include <limits>
#include <vector>
#include <utility>
using namespace std;

#include "debug.h"
#include "relops.h"

class ubigint {
   private:
      using ubigvalue_t = vector<uint8_t>;
      void pad_zeros (vector<uint8_t>*, vector<uint8_t>*) const;
      void trim_zeros (vector<uint8_t>*) const;
      bool less(const ubigint&) const;
      bool equal(const ubigint&) const;
   public:
      ubigvalue_t uvalue {};
      void multiply_by_2();
      void divide_by_2();

      ubigint() = default; // Need default ctor as well.
      ubigint (unsigned long);
      ubigint (const string&);

      ubigint operator+ (const ubigint&) const;
      ubigint operator- (const ubigint&) const;
      ubigint operator* (const ubigint&) const;
      ubigint operator/ (const ubigint&) const;
      ubigint operator% (const ubigint&) const;

      bool operator== (const ubigint&) const;
      bool operator<  (const ubigint&) const;

      void print() const;

   friend ostream& operator<< (ostream&, const ubigint&);
   friend ostream& operator<< (ostream&, const ubigint::ubigvalue_t&);
};

#endif

