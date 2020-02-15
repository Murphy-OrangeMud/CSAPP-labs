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
int halfAdd(int, int);
int test_halfAdd(int, int);
int sameSign(int, int);
int test_sameSign(int, int);
int oneMoreThan(int, int);
int test_oneMoreThan(int, int);
int fullAdd(int, int);
int test_fullAdd(int, int);
int testAndSet(int, int, int);
int test_testAndSet(int, int, int);
int zeroByte(int);
int test_zeroByte(int);
int isTmin(int);
int test_isTmin(int);
int negate(int);
int test_negate(int);
int subOK(int, int);
int test_subOK(int, int);
int negPerByte(int);
int test_negPerByte(int);
int isGreater(int, int);
int test_isGreater(int, int);
int modThree(int);
int test_modThree(int);
int howManyBits(int);
int test_howManyBits(int);
int twos2ones(int);
int test_twos2ones(int);
int ones2twos(int);
int test_ones2twos(int);
unsigned long long float2double(unsigned long long);
unsigned long long test_float2double(unsigned long long);
unsigned long long double_abs(unsigned long long);
unsigned long long test_double_abs(unsigned long long);
unsigned long long double_neg(unsigned long long);
unsigned long long test_double_neg(unsigned long long);
unsigned long long double_half(unsigned long long);
unsigned long long test_double_half(unsigned long long);
unsigned long long double_twice(unsigned long long);
unsigned long long test_double_twice(unsigned long long);
unsigned long long double_pwr2(long long);
unsigned long long test_double_pwr2(long long);
unsigned long long double_negpwr2(long long);
unsigned long long test_double_negpwr2(long long);
unsigned long long double_add(unsigned long long, unsigned long long);
unsigned long long test_double_add(unsigned long long, unsigned long long);
