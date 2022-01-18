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

ostream &operator<<(ostream &out, const ubigint::ubigvalue_t &uvalue)
{
   for (const auto &digit : uvalue)
      out << static_cast<int>(digit);
   return out;
}

ubigint::ubigint(unsigned long that) : 
uvalue((ubigint(to_string(that))).uvalue)
{
}

ubigint::ubigint(const string &that) : uvalue(0)
{
   for (char digit : that)
   {
      if (not isdigit(digit))
      {
         throw invalid_argument("ubigint::ubigint(" + that + ")");
      }
      uvalue.push_back(digit - '0');
   }
}

void ubigint::pad_zeros(vector<uint8_t> *vec1,
                        vector<uint8_t> *vec2) const
{
   if (vec1->size() == vec2->size())
   {
      return;
   }

   int diff = abs(static_cast<int>(vec1->size()) -
                  static_cast<int>(vec2->size()));
   if (vec1->size() > vec2->size())
   {
      for (int i = 0; i < diff; i++)
      {
         vec2->push_back(0);
      }
   }
   else
   {
      for (int i = 0; i < diff; i++)
      {
         vec1->push_back(0);
      }
   }
}

void ubigint::trim_zeros(vector<uint8_t> *vec) const
{
   while (vec->size() > 0 && vec->back() == 0)
   {
      vec->pop_back();
   }
}

ubigint ubigint::operator+(const ubigint &that) const
{
   vector<uint8_t> vecL = this->uvalue;
   vector<uint8_t> vecR = that.uvalue;
   vector<uint8_t> result;
   reverse(vecL.begin(), vecL.end());
   reverse(vecR.begin(), vecR.end());

   pad_zeros(&vecL, &vecR);

   result.push_back(0);
   for (size_t i = 0; i < vecL.size(); i++)
   {
      uint8_t sum = result[i] + vecL[i] + vecR[i];
      uint8_t digit = sum % 10;
      uint8_t carry = sum / 10;

      result[i] = digit;
      result.push_back(carry);
   }

   trim_zeros(&result);
   reverse(result.begin(), result.end());
   ubigint retval;
   retval.uvalue = result;

   return retval;
}

ubigint ubigint::operator-(const ubigint &that) const
{
   vector<uint8_t> vecL = this->uvalue;
   vector<uint8_t> vecR = that.uvalue;
   vector<uint8_t> result;
   reverse(vecL.begin(), vecL.end());
   reverse(vecR.begin(), vecR.end());

   pad_zeros(&vecL, &vecR);

   result.push_back(0);
   bool borrow = false;

   for (size_t i = 0; i < vecL.size(); i++)
   {
      int sum = ((borrow) ? -1 : 0) + vecL[i] - vecR[i];
      int digit;

      if (sum < 0)
      {
         digit = sum + 10;
         borrow = true;
      }
      else
      {
         digit = sum;
         borrow = false;
      }

      result[i] = digit;
      result.push_back(0);
   }

   trim_zeros(&result);
   reverse(result.begin(), result.end());
   ubigint retval;
   retval.uvalue = result;

   return retval;
}

ubigint ubigint::operator*(const ubigint &that) const
{
   DEBUGF('u', *this << "+" << that);
   vector<uint8_t> vecL = this->uvalue;
   vector<uint8_t> vecR = that.uvalue;
   vector<uint8_t> result;
   reverse(vecL.begin(), vecL.end());
   reverse(vecR.begin(), vecR.end());

   result.resize(static_cast<int>(vecL.size()) +
                 static_cast<int>(vecR.size()));

   for (size_t i = 0; i < vecL.size(); i++)
   {
      int carry = 0;

      for (size_t j = 0; j < vecR.size(); j++)
      {
         int digit = result[i + j] + vecL[i] * vecR[j] + carry;
         result[i + j] = digit % 10;
         carry = digit / 10;
      }

      result[i + static_cast<int>(vecR.size())] = carry;
   }

   trim_zeros(&result);
   reverse(result.begin(), result.end());

   ubigint retval;
   retval.uvalue = result;
   return retval;
}

void ubigint::multiply_by_2()
{
   ubigint two;
   two.uvalue.push_back(2);
   *this = *this * two;
}

void ubigint::divide_by_2()
{
   ubigint two;
   two.uvalue.push_back(2);
   reverse(this->uvalue.begin(), this->uvalue.end());

   for (size_t i = 0; i < this->uvalue.size(); i++)
   {
      this->uvalue[i] = this->uvalue[i] / 2;
      if ((i != this->uvalue.size()) &&
          ((this->uvalue[i + 1] % 2) != 0))
      {
         this->uvalue[i] += 5;
      }
   }

   trim_zeros(&this->uvalue);
   reverse(this->uvalue.begin(), this->uvalue.end());
}

struct quo_rem
{
   ubigint quotient;
   ubigint remainder;
};
quo_rem udivide(const ubigint &dividend, const ubigint &divisor_)
{
   // NOTE: udivide is a non-member function.
   ubigint divisor{divisor_};
   ubigint zero{0};
   if (divisor == zero)
      throw domain_error("udivide by zero");
   ubigint power_of_2{1};
   ubigint quotient{0};
   ubigint remainder{dividend}; // left operand, dividend

   while (divisor < remainder)
   {
      divisor.multiply_by_2();
      power_of_2.multiply_by_2();
   }
   while (power_of_2 > zero)
   {
      if (divisor <= remainder)
      {
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
      }
      divisor.divide_by_2();
      power_of_2.divide_by_2();
   }
   DEBUGF('/', "quotient = " << quotient);
   DEBUGF('/', "remainder = " << remainder);
   return {.quotient = quotient, .remainder = remainder};
}

ubigint ubigint::operator/(const ubigint &that) const
{
   return udivide(*this, that).quotient;
}

ubigint ubigint::operator%(const ubigint &that) const
{
   return udivide(*this, that).remainder;
}

bool ubigint::less(const ubigint &that) const
{
   for (size_t i = 0; i < this->uvalue.size(); i++)
   {
      if (this->uvalue[i] < that.uvalue[i])
      {
         return true;
      }

      if (this->uvalue[i] > that.uvalue[i])
      {
         return false;
      }
   }
   return false;
}

bool ubigint::equal(const ubigint &that) const
{
   for (size_t i = 0; i < this->uvalue.size(); i++)
   {
      if (this->uvalue[i] != that.uvalue[i])
      {
         return false;
      }
   }
   return true;
}

bool ubigint::operator==(const ubigint &that) const
{
   if (this->uvalue.size() != that.uvalue.size())
   {
      return false;
   }
   return this->equal(that);
}

bool ubigint::operator<(const ubigint &that) const
{
   if (this->uvalue.size() < that.uvalue.size())
   {
      return true;
   }
   if (this->uvalue.size() > that.uvalue.size())
   {
      return false;
   }
   return this->less(that);
}

void ubigint::print() const
{
   DEBUGF('p', this << " -> " << *this);
   for (size_t i = 0; i < uvalue.size(); i++)
   {
      cout << static_cast<int>(uvalue[i]);
      if (i != 0 && i % 68 == 0)
      {
         cout << "\\"
              << "\n";
      }
   }
}

ostream &operator<<(ostream &out, const ubigint &that)
{
   return out << "ubigint(" << that.uvalue << ")";
}
