/**********************************************************************
 *  This file is part of Polygon.
 *
 *        Copyright (C) 2018 Alex Eskin
 *        Copyright (C) 2019 Vincent Delecroix
 *        Copyright (C) 2019 Julian Rüth
 *
 *  Polygon is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Polygon is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Polygon. If not, see <https://www.gnu.org/licenses/>.
 *********************************************************************/

#include <complex>
#include <ostream>
#include <vector>
#include <boost/math/constants/constants.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include "libpolygon/defs.h"
#include "libpolygon/number_field.h"
#include "libpolygon/params.h"
#include "libpolygon/two_complex.h"

using std::complex;
using std::ostream;
using std::vector;
using boost::numeric_cast;

namespace polygon {
template <typename T>
poly<T> cyclotomic_poly(int n) {
  vector<T> coeffs;
  poly<T> current, quotient, remainder;

  coeffs.push_back(-1);
  for (int i = 1; i < n; i++) {
    coeffs.push_back(0);
  }
  coeffs.push_back(1);

  //  out_s << coeffs;

  current = poly<T>(coeffs);

  if (n == 1) {
    return (current);
  }

  for (int d = 1; d < n; d++) {
    if (n % d == 0) {
      poly<T> g = cyclotomic_poly<T>(d);
      divide<T>(quotient, remainder, current, g);
      if (not(remainder == poly<T>())) {
        std::cout << "cyclotomic: failed to divide";
      }
      current = quotient;
    }
  }
  return (current);
}

template <typename T>
NumberField<T> *InitCyclotomic(int n) {
  // use the cyclotomic field generated by the primitive n'th roots of unity

  NumberField<T> *F;

  complex<COORD> nth_rootofunity(cos(2 * boost::math::constants::pi<COORD>() / n),
                                 sin(2 * boost::math::constants::pi<COORD>() / n));
  poly<T> cyc = cyclotomic_poly<T>(n);
  vector<T> coeffs = cyc.coefficients;
  F = new NumberField<T>(&coeffs[0], numeric_cast<size_t>(cyc.degree()), nth_rootofunity);
  F->store_conjugate(algebraic<T>(1, F).pow(n - 1));

  return (F);
}

template <typename T>
algebraic<T> alg_t<T>::get_coeff(size_t i) {
  return coeffs[i];
}

template <typename T>
void alg_t<T>::set_coeff(size_t i, const algebraic<T> &a) {
  coeffs[i] = a;
}

template <typename T>
alg_t<T>::alg_t() {
  coeffs.assign(Params::nbr_params() + 1, NumberField<T>::F_zero);
}

template <typename T>
void alg_t<T>::pad_coeffs(int n) {
  for (int i = 0; i < n; i++) {
    coeffs.push_back(algebraic<T>(NumberField<T>::F));  // set coefficients to 0
  }
}

template <typename T>
alg_t<T>::alg_t(vector<algebraic<T> > c) : coeffs(c) {
  if (c.size() != Params::nbr_params() + 1) {
    ERR_RET("alg_t: initialization with wrong length vector");
  }
}

template <typename T>
alg_t<T> alg_t<T>::operator-() const {
  alg_t<T> r(*this);

  for (size_t i = 0; i <= Params::nbr_params(); i++) r.coeffs[i] = -(coeffs[i]);
  return r;
}

template <typename T>
alg_t<T> &alg_t<T>::operator+=(const alg_t<T> &p) {
  for (size_t i = 0; i <= Params::nbr_params(); i++) coeffs[i] += p.coeffs[i];
  return *this;
}

template <typename T>
alg_t<T> &alg_t<T>::operator-=(const alg_t<T> &p) {
  for (size_t i = 0; i <= Params::nbr_params(); i++) coeffs[i] -= p.coeffs[i];
  return *this;
}

template <typename T>
alg_t<T> operator+(alg_t<T> p, const alg_t<T> &q) {
  //  alg_t<T> r = p;
  return p += q;
}

template <typename T>
alg_t<T> operator-(alg_t<T> p, const alg_t<T> &q) {
  //  alg_t<T> r = p;
  return p -= q;
}

template <typename T>
alg_t<T> &alg_t<T>::operator*=(const algebraic<T> &r) {
  for (typename vector<algebraic<T> >::iterator i = coeffs.begin();
       i != coeffs.end(); ++i)
    *i *= r;
  return *this;
}

template <typename T>
alg_t<T> &alg_t<T>::operator*=(int r) {
  for (typename vector<algebraic<T> >::iterator i = coeffs.begin();
       i != coeffs.end(); ++i)
    *i *= r;
  return *this;
}

template <typename T>
alg_t<T> &alg_t<T>::operator/=(const algebraic<T> &r) {
  for (typename vector<algebraic<T> >::iterator i = coeffs.begin();
       i != coeffs.end(); ++i)
    *i /= r;
  return *this;
}

template <typename T>
alg_t<T> &alg_t<T>::operator/=(int r) {
  for (typename vector<algebraic<T> >::iterator i = coeffs.begin();
       i != coeffs.end(); ++i)
    *i /= r;
  return *this;
}

template <typename T>
alg_t<T> operator*(const algebraic<T> &p, alg_t<T> &q) {
  //    alg_t<T> r = q;
  return q *= p;
}

template <typename T>
alg_t<T> operator*(alg_t<T> q, const algebraic<T> &p) {
  //    alg_t<T> r = q;
  return q *= p;
}

template <typename T>
alg_t<T> operator*(int p, alg_t<T> &q) {
  //    alg_t<T> r = q;
  return q *= p;
}

template <typename T>
alg_t<T> operator*(alg_t<T> &q, int p) {
  //    alg_t<T> r = q;
  return q *= p;
}

template <typename T>
alg_t<T> operator/(alg_t<T> &q, const algebraic<T> &p) {
  //    alg_t<T> r = q;
  return q /= p;
}

template <typename T>
alg_t<T> operator/(alg_t<T> &q, int p) {
  //    alg_t<T> r = q;
  return q /= p;
}

template <typename T>
bool operator==(const alg_t<T> &p, const alg_t<T> &q) {
  if (p.coeffs == q.coeffs)
    return true;
  else
    return false;
}

template <typename T>
bool operator!=(const alg_t<T> &p, const alg_t<T> &q) {
  return (!(p == q));
}

template <typename T>
Point alg_t<T>::tocomplex() const {
  Point b = coeffs[0].tocomplex();
  for (size_t i = 1; i <= Params::nbr_params(); i++)
    b += coeffs[i].tocomplex() * Params::params[i - 1];
  return b;
}

template <typename T>
bool alg_t<T>::get_direction(algebraic<T> &direction) {
  algebraic<T> q;
  algebraic<T> zero_F = algebraic<T>(coeffs[0].field());
  for (size_t i = 0; i <= Params::nbr_params(); i++) {
    for (size_t j = i + 1; j <= Params::nbr_params(); j++) {
      q = coeffs[i] * coeffs[j].conjugate() - coeffs[j] * coeffs[i].conjugate();
      if (q != zero_F) {  // q != 0
        return (false);
      }
    }
  }
  for (size_t i = 0; i <= Params::nbr_params(); i++) {
    q = coeffs[i];
    if (q != zero_F) {
      break;
    }
  }
  if (q == zero_F) {
    ERR_RET("alg_t: get_direction called on zero vector");
  }
  direction = q;
  return (true);
}

template <typename T>
bool colinear(const alg_t<T> &p1, const alg_t<T> &p2) {
  algebraic<T> q;  // initialized to 0
  //    algebraic<T> zero_F = algebraic<T>(p1.coeffs[0].field());

  //    cout << "In colinear alg_t, p1 = " << p1 << " p2 = " << p2 << endl;

  for (size_t i = 0; i <= Params::nbr_params(); i++) {
    for (size_t j = i; j <= Params::nbr_params(); j++) {
      q = cross_product(p1.coeffs[i], p2.coeffs[j]);

      //	    q =p1.coeffs[i]*p2.coeffs[j].conjugate()
      //		-p2.coeffs[j]*p1.coeffs[i].conjugate();

      q += cross_product(p1.coeffs[j], p2.coeffs[i]);

      //	    q += p1.coeffs[j]*p2.coeffs[i].conjugate()
      //		-p2.coeffs[i]*p1.coeffs[j].conjugate();

      if (q != NumberField<T>::F_zero) {  // q != 0
        return (false);
      }
    }
  }
  return (true);
}

template <typename T>
ostream &operator<<(ostream &outputStream, const alg_t<T> &u) {
  outputStream << u.coeffs[0];
  for (size_t i = 1; i <= Params::nbr_params(); i++) {
    outputStream << "+" << u.coeffs[i] << "t" << i;
  }
  return outputStream;
}

// bigpoint stuff

template <typename T>
BigPoint<T>::BigPoint() {
  cx = 0;
  //    algt = alg_t<T>();
}

template <typename T>
BigPoint<T>::BigPoint(int a, int b) {
  if (a != 0 || b != 0) {
    ERR_RET("BigPoint: non-zero initialization not implemented");
  }

  cx = 0;
  algt = alg_t<T>();
}

template <typename T>
BigPoint<T>::BigPoint(alg_t<T> u) : algt(u) {
  cx = u.tocomplex();
}

template <typename T>
void BigPoint<T>::SetCxFromAlgebraic() {
  cx = algt.tocomplex();
}

template <typename T>
BigPoint<T> BigPoint<T>::operator-() {
  BigPoint<T> r = *this;
  r.cx = -r.cx;
  r.algt = -r.algt;

  return r;
}

template <typename T>
BigPoint<T> &BigPoint<T>::operator+=(const BigPoint<T> &p) {
  cx += p.cx;
  algt += p.algt;

  return *this;
}

template <typename T>
BigPoint<T> &BigPoint<T>::operator-=(const BigPoint<T> &p) {
  cx -= p.cx;
  algt -= p.algt;

  return *this;
}

template <typename T>
BigPoint<T> operator+(BigPoint<T> p, const BigPoint<T> &q) {
  //  BigPoint<T> r = p;
  return p += q;
}

template <typename T>
BigPoint<T> operator-(BigPoint<T> p, const BigPoint<T> &q) {
  //  BigPoint<T> r = p;
  return p -= q;
}

template <typename T>
BigPoint<T> &BigPoint<T>::operator*=(const algebraic<T> &r) {
  cx = r.tocomplex() * cx;
  algt *= r;
  return *this;
}

template <typename T>
BigPoint<T> &BigPoint<T>::operator*=(int r) {
  cx = static_cast<COORD>(r) * cx;
  algt *= r;
  return *this;
}

template <typename T>
BigPoint<T> &BigPoint<T>::operator/=(const algebraic<T> &r) {
  cx /= r.tocomplex();
  algt /= r;
  return *this;
}

template <typename T>
BigPoint<T> &BigPoint<T>::operator/=(int r) {
  cx /= (1.0 * r);
  algt /= r;
  return *this;
}

template <typename T>
BigPoint<T> operator*(const algebraic<T> &p, BigPoint<T> q) {
  BigPoint<T> r = q;
  return r *= p;
}

template <typename T>
BigPoint<T> operator*(BigPoint<T> q, const algebraic<T> &p) {
  //    BigPoint<T> r = q;
  return q *= p;
}

template <typename T>
BigPoint<T> operator*(int p, BigPoint<T> q) {
  //    BigPoint<T> r = q;
  return q *= p;
}

template <typename T>
BigPoint<T> operator*(BigPoint<T> q, int p) {
  //    BigPoint<T> r = q;
  return q *= p;
}

template <typename T>
BigPoint<T> operator/(BigPoint<T> q, const algebraic<T> &p) {
  //    BigPoint<T> r = q;
  return q /= p;
}

template <typename T>
BigPoint<T> operator/(BigPoint<T> q, int p) {
  //    BigPoint<T> r = q;
  return q /= p;
}

template <typename T>
void BigPoint<T>::Check() {
  if (field_arithmetic && abs(cx - algt.tocomplex()) > EPSILON) {
    ERR_RET("BigPoint<T> Check Failed");
  }
}

template <typename T>
bool operator==(const BigPoint<T> &p, const BigPoint<T> &q) {
  if (field_arithmetic) {
    if (p.algt == q.algt) {
      return true;
    } else {
      return false;
    }
  } else {
    if (norm(p.cx - q.cx) < EPSILON * EPSILON) {
      return true;
    } else {
      return false;
    }
  }
}

template <typename T>
alg_t<T> alg_t<T>::conjugate() const {
  alg_t<T> conj;
  for (size_t i = 0; i <= Params::nbr_params(); i++) {
    conj.coeffs[i] = coeffs[i].conjugate();
  }
  return conj;
}

template <typename T>
ostream &operator<<(ostream &outputStream, const BigPoint<T> &p) {
  outputStream << p.cx << "=" << p.algt;
  return outputStream;
}
}  // namespace polygon

// Explicit template instantiations for mpq
namespace polygon {
template class BigPoint<bigrat>;
template class alg_t<bigrat>;

template poly<bigrat> cyclotomic_poly(int n);
template NumberField<bigrat> *InitCyclotomic(int n);
template alg_t<bigrat> operator+(alg_t<bigrat> p, const alg_t<bigrat> &q);
template alg_t<bigrat> operator-(alg_t<bigrat> p, const alg_t<bigrat> &q);
template alg_t<bigrat> operator*(const algebraic<bigrat> &p, alg_t<bigrat> &q);
template alg_t<bigrat> operator*(alg_t<bigrat> q, const algebraic<bigrat> &p);
template alg_t<bigrat> operator*(int p, alg_t<bigrat> &q);
template alg_t<bigrat> operator*(alg_t<bigrat> &q, int p);
template alg_t<bigrat> operator/(alg_t<bigrat> &q, const algebraic<bigrat> &p);
template alg_t<bigrat> operator/(alg_t<bigrat> &q, int p);
template bool operator==(const alg_t<bigrat> &p, const alg_t<bigrat> &q);
template bool operator!=(const alg_t<bigrat> &p, const alg_t<bigrat> &q);
template bool colinear(const alg_t<bigrat> &p1, const alg_t<bigrat> &p2);
template ostream &operator<<(ostream &outputStream, const alg_t<bigrat> &u);

template BigPoint<bigrat> operator+(BigPoint<bigrat> p,
                                    const BigPoint<bigrat> &q);
template BigPoint<bigrat> operator-(BigPoint<bigrat> p,
                                    const BigPoint<bigrat> &q);
template BigPoint<bigrat> operator*(const algebraic<bigrat> &p,
                                    BigPoint<bigrat> q);
template BigPoint<bigrat> operator*(BigPoint<bigrat> q,
                                    const algebraic<bigrat> &p);
template BigPoint<bigrat> operator*(int p, BigPoint<bigrat> q);
template BigPoint<bigrat> operator*(BigPoint<bigrat> q, int p);
template BigPoint<bigrat> operator/(BigPoint<bigrat> q,
                                    const algebraic<bigrat> &p);
template BigPoint<bigrat> operator/(BigPoint<bigrat> q, int p);
template bool operator==(const BigPoint<bigrat> &p, const BigPoint<bigrat> &q);
template ostream &operator<<(ostream &outputStream, const BigPoint<bigrat> &p);
}  // namespace polygon

// Explicit template instantiations for int64
namespace polygon {
template class BigPoint<int64_t>;
template class alg_t<int64_t>;

template poly<int64_t> cyclotomic_poly(int n);
template NumberField<int64_t> *InitCyclotomic(int n);
template alg_t<int64_t> operator+(alg_t<int64_t> p, const alg_t<int64_t> &q);
template alg_t<int64_t> operator-(alg_t<int64_t> p, const alg_t<int64_t> &q);
template alg_t<int64_t> operator*(const algebraic<int64_t> &p,
                                  alg_t<int64_t> &q);
template alg_t<int64_t> operator*(alg_t<int64_t> q,
                                  const algebraic<int64_t> &p);
template alg_t<int64_t> operator*(int p, alg_t<int64_t> &q);
template alg_t<int64_t> operator*(alg_t<int64_t> &q, int p);
template alg_t<int64_t> operator/(alg_t<int64_t> &q,
                                  const algebraic<int64_t> &p);
template alg_t<int64_t> operator/(alg_t<int64_t> &q, int p);
template bool operator==(const alg_t<int64_t> &p, const alg_t<int64_t> &q);
template bool operator!=(const alg_t<int64_t> &p, const alg_t<int64_t> &q);
template bool colinear(const alg_t<int64_t> &p1, const alg_t<int64_t> &p2);
template ostream &operator<<(ostream &outputStream, const alg_t<int64_t> &u);

template BigPoint<int64_t> operator+(BigPoint<int64_t> p,
                                     const BigPoint<int64_t> &q);
template BigPoint<int64_t> operator-(BigPoint<int64_t> p,
                                     const BigPoint<int64_t> &q);
template BigPoint<int64_t> operator*(const algebraic<int64_t> &p,
                                     BigPoint<int64_t> q);
template BigPoint<int64_t> operator*(BigPoint<int64_t> q,
                                     const algebraic<int64_t> &p);
template BigPoint<int64_t> operator*(int p, BigPoint<int64_t> q);
template BigPoint<int64_t> operator*(BigPoint<int64_t> q, int p);
template BigPoint<int64_t> operator/(BigPoint<int64_t> q,
                                     const algebraic<int64_t> &p);
template BigPoint<int64_t> operator/(BigPoint<int64_t> q, int p);
template bool operator==(const BigPoint<int64_t> &p,
                         const BigPoint<int64_t> &q);
template ostream &operator<<(ostream &outputStream, const BigPoint<int64_t> &p);
}  // namespace polygon
