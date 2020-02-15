/* Testing Code */

#include <limits.h>
#include <math.h>

/* Routines used by floation point test code */

/* Convert from bit level representation to floating point number */
// static double u2f(unsigned long long u)
// {
//   union {
//     unsigned long long u;
//     double f;
//   } v;
//   v.u = u;
//   return v.f;
// }

/* Convert from floating point number to bit-level representation */
unsigned long long d2u(double f)
{
  union {
    unsigned long long u;
    double f;
  } v;
  v.u = 0;
  v.f = f;
  return v.u;
}

double u2d(unsigned long long u)
{
  union {
    unsigned long long u;
    double f;
  } v;
  v.u = u;
  return v.f;
}

/* Convert from bit level representation to floating point number */
float u2f(unsigned u) {
  union {
    unsigned u;
    float f;
  } a;
  a.u = u;
  return a.f;
}

/* Convert from floating point number to bit-level representation */
unsigned f2u(float f) {
  union {
    unsigned u;
    float f;
  } a;
  a.f = f;
  return a.u;
}


/* Copyright (C) 1991-2016 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */
/* This header is separate from features.h so that the compiler can
   include it implicitly at the start of every compilation.  It must
   not itself include <features.h> or any other header that includes
   <features.h> because the implicit include comes before any feature
   test macros that may be defined in a source file before it first
   explicitly includes a system header.  GCC knows the name of this
   header in order to preinclude it.  */
/* glibc's intent is to support the IEC 559 math functionality, real
   and complex.  If the GCC (4.9 and later) predefined macros
   specifying compiler intent are available, use them to determine
   whether the overall intent is to support these features; otherwise,
   presume an older compiler has intent to support these features and
   define these macros by default.  */
/* wchar_t uses Unicode 8.0.0.  Version 8.0 of the Unicode Standard is
   synchronized with ISO/IEC 10646:2014, plus Amendment 1 (published
   2015-05-15).  */
/* We do not support C11 <threads.h>.  */
int test_halfAdd(int x, int y) {
    return x + y;
}
int test_sameSign(int x, int y) {
  return (x >= 0) && (y >= 0) || (x < 0) && (y < 0);
}
int test_oneMoreThan(int x, int y) {
  return (y - x == 1) && (x != 0x7FFFFFFF);
}
int test_fullAdd(int x, int y) {
    return (x + y) & 0xF;
}
int test_testAndSet(int x, int y, int z) {
    int lx = x & 0xFFFF;
    int hx = x & 0xFFFF0000;
    z <<= 16;
    x = lx | (lx == y ? z : hx);
    return x;
}
int test_zeroByte(int x)
{
  char p1 = x;
  char p2 = x >> 8;
  char p3 = x >> 16;
  char p4 = x >> 24;
  return (p1 == 0) || (p2 == 0) || (p3 == 0) || (p4 == 0);
}
int test_isTmin(int x) {
    return x == 0x80000000;
}
int test_negate(int x) {
  return -x;
}
int test_subOK(int x, int y)
{
  long long ldiff = (long long) x - y;
  return ldiff == (int) ldiff;
}
int test_negPerByte(int x) {
    char b1 = x;
    char b2 = x >> 8;
    char b3 = x >> 16;
    char b4 = x >> 24;
    b1 = -b1;
    b2 = -b2;
    b3 = -b3;
    b4 = -b4;
    x = ((int)b1 & 0xFF) | (((int)b2 & 0xFF) << 8) |
        (((int)b3 & 0xFF) << 16) | (((int)b4 & 0xFF) << 24);
    return x;
}
int test_isGreater(int x, int y)
{
  return x > y;
}
int test_modThree(int x) {
    return x % 3;
}
int test_howManyBits(int x) {
    unsigned int a, cnt;
    x = x<0 ? -x-1 : x;
    a = (unsigned int)x;
    for (cnt=0; a; a>>=1, cnt++)
        ;
    return (int)(cnt + 1);
}
int test_twos2ones(int x)
{
    return x - (x<0?1:0);
}
int test_ones2twos(int x)
{
    return x + (x<0?1:0);
}
unsigned long long test_float2double(unsigned long long uf) {
  unsigned long long mask_frac_float = 0x007fffff;
  unsigned long long mask_exp_float = 0x7f800000;
  unsigned long long mask_sign_float = 0x80000000;
  unsigned long long frac_float = 0;
  unsigned long long frac_double = 0;
  unsigned long long exp_float = 0;
  unsigned long long exp_double = 0;
  unsigned long long sign = 0;
  unsigned long long res = 0;
  frac_float = uf & mask_frac_float;
  frac_double = frac_float << 29;
  exp_float = uf & mask_exp_float;
  exp_double = ((exp_float >> 23) + (1024-128)) << 52;
  sign = (uf & mask_sign_float) << 32;
  // nan/inf
  if(exp_float == mask_exp_float){
    exp_double = 0x7ff00000;
    exp_double <<= 32;
  }
  res = sign | exp_double | frac_double;
  return res;
}
unsigned long long test_double_abs(unsigned long long uf) {
    double d = u2d(uf);
    unsigned long long unf = d2u(-d);
    if(isnan(d))
        return uf;
  /* An unfortunate hack to get around a limitation of the BDD Checker */
  if ((long long) uf < 0)
      return unf;
  else
      return uf;
}
unsigned long long test_double_neg(unsigned long long uf) {
    double d = u2d(uf);
    if(isnan(d))
        return uf;
    return d2u(-d);
}
unsigned long long test_double_half(unsigned long long uf) {
    double d = u2d(uf);
    double hd = 0.5*d;
    if (isnan(d))
        return uf;
    else
        return d2u(hd);
}
unsigned long long test_double_twice(unsigned long long uf) {
    double d = u2d(uf);
    double td = 2*d;
    if(isnan(d))
        return uf;
    else
        return d2u(td);
}
unsigned long long test_double_pwr2(long long x) {
    double result = 1.0;
    double p2 = 2.0;
    long long recip = (x < 0);
    unsigned long long one = 1;
    unsigned long long tmin = one << 63;
    /* treat tmin specially */
    if((unsigned long long)x == tmin)
        return 0;
    if(recip) {
        x = -x;
        p2 = 0.5;
    }
    while(x > 0) {
        if(x & 0x1)
            result = result * p2;
        p2 = p2 * p2;
        x >>= 1;
    }
    return d2u(result);
}
unsigned long long test_double_negpwr2(long long x) {
    double result = 1.0;
    double p2 = 0.5;
    unsigned long long one = 1;
    unsigned long long tmin = one << 63;
    unsigned long long inf = 0x7ff;
    long long recip = (x < 0);
    inf = inf << 52;
    if((unsigned long long)x == tmin)
        return inf;
    if (recip) {
        x = -x;
        p2 = 2.0;
    }
    while (x > 0) {
        if (x & 0x1)
            result = result * p2;
        p2 = p2 * p2;
        x >>= 1;
    }
    return d2u(result);
}
unsigned long long test_double_add(unsigned long long f1, unsigned long long f2) {
    // clear sign bit
    unsigned long long one = 1;
    f1 = f1 & ((one << 63) - 1);
    f2 = f2 & ((one << 63) - 1);
    double d1 = u2d(f1), d2 = u2d(f2);
    unsigned long long mask = 0x7ff;
    mask = mask << 52;
    // if either NaN or oo
    if (f1 >= mask || f2 >= mask)
        return 0;
    return d2u(d1 + d2);
}
