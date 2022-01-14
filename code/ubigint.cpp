// $Id: ubigint.cpp,v 1.5 2022-01-11 16:36:15-08 - - $

#include <cctype>
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
#include <iostream>
using namespace std;

#include "debug.h"
#include "ubigint.h"

ostream& operator<< (ostream& out, const ubigint::ubigvalue_t& uvalue) {
   for (const auto& digit: uvalue) out << static_cast<int> (digit);
   return out;
}

ubigint::ubigint (unsigned long that): uvalue (that) {
   DEBUGF ('~', this << " -> " << uvalue)
}

ubigint::ubigint (const string& that): uvalue(0) {
   DEBUGF ('~', "that = \"" << that << "\"");
   for (char digit: that) {
      if (not isdigit (digit)) {
         throw invalid_argument ("ubigint::ubigint(" + that + ")");
      }
      uvalue = uvalue * 10 + digit - '0';
   }
}

// ubigint ubigint::pad_zeros (const ubigint& that) const
// {
//    if (this.size() == that.size())
//    {

//    }
//    int diff = abs(this.size() - that.size());
//    if (this.size()  that.size())
// }

ubigint ubigint::operator+ (const ubigint& that) const {
   vector<uint8_t> vect;
   vect.push_back(5);
   // vect.push_back(that.uvalue[0]);
   // vect.insert(vect.end(), that.uvalue.begin(), that.uvalue.end());

   ubigint bigint;
   bigint.uvalue = vect;

   ubigint result;
   result.uvalue = bigint.uvalue;

   return result;
}

ubigint ubigint::operator- (const ubigint& that) const {
   DEBUGF ('u', *this << "+" << that);
   ubigint result;
/***FIXME***
   if (*this < that) throw domain_error ("ubigint::operator-(a<b)");
   return ubigint (uvalue - that.uvalue);
***/
   return result;
}

ubigint ubigint::operator* (const ubigint& that) const {
   DEBUGF ('u', *this << "+" << that);
   ubigint result;
/***FIXME***
   return ubigint (uvalue * that.uvalue);
***/
   return result;
}

void ubigint::multiply_by_2() {
/***FIXME***
   uvalue *= 2;
***/
}

void ubigint::divide_by_2() {
/***FIXME***
   uvalue /= 2;
***/
}


struct quo_rem { ubigint quotient; ubigint remainder; };
quo_rem udivide (const ubigint& dividend, const ubigint& divisor_) {
   // NOTE: udivide is a non-member function.
   ubigint divisor {divisor_};
   ubigint zero {0};
   if (divisor == zero) throw domain_error ("udivide by zero");
   ubigint power_of_2 {1};
   ubigint quotient {0};
   ubigint remainder {dividend}; // left operand, dividend
   while (divisor < remainder) {
      divisor.multiply_by_2();
      power_of_2.multiply_by_2();
   }
   while (power_of_2 > zero) {
      if (divisor <= remainder) {
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
      }
      divisor.divide_by_2();
      power_of_2.divide_by_2();
   }
   DEBUGF ('/', "quotient = " << quotient);
   DEBUGF ('/', "remainder = " << remainder);
   return {.quotient = quotient, .remainder = remainder};
}

ubigint ubigint::operator/ (const ubigint& that) const {
   return udivide (*this, that).quotient;
}

ubigint ubigint::operator% (const ubigint& that) const {
   return udivide (*this, that).remainder;
}

bool ubigint::operator== (const ubigint& that) const {
   return uvalue == that.uvalue;
}

bool ubigint::operator< (const ubigint& that) const {
   return uvalue < that.uvalue;
}

void ubigint::print() const {
   DEBUGF ('p', this << " -> " << *this);
   cout << uvalue;
}

ostream& operator<< (ostream& out, const ubigint& that) { 
   return out << "ubigint(" << that.uvalue << ")";
}

