#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define TMin INT_MIN
#define TMax INT_MAX

#include "btest.h"
#include "bits.h"

test_rec test_set[] = {
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
 {"halfAdd", (funct_t) halfAdd, (funct_t) test_halfAdd, 2,
    "~ & ^ | << >>", 7, 1,
  {{0, 1}, {0, 1}, {TMin, TMax}}},
 {"sameSign", (funct_t) sameSign, (funct_t) test_sameSign, 2,
    "~ & ! ^ | + << >>", 5, 2,
  {{TMin, TMax},{TMin,TMax},{TMin,TMax}}},
 {"oneMoreThan", (funct_t) oneMoreThan, (funct_t) test_oneMoreThan, 2,
    "~ & ! ^ | + << >>", 15, 2,
  {{TMin, TMax},{TMin,TMax},{TMin,TMax}}},
 {"fullAdd", (funct_t) fullAdd, (funct_t) test_fullAdd, 2,
    "~ & ^ | << >>", 30, 2,
  {{0, 15}, {0, 15}, {TMin, TMax}}},
 {"testAndSet", (funct_t) testAndSet, (funct_t) test_testAndSet, 3,
    "~ | ^ & << >> + !", 20, 2,
  {{TMin, TMax}, {0x0000, 0x0FFF}, {0x004A, 0x004A}}},
 {"zeroByte", (funct_t) zeroByte, (funct_t) test_zeroByte, 1, "~ & ! | + << >>", 24, 3,
  {{TMin, TMax},{TMin,TMax},{TMin,TMax}}},
 {"isTmin", (funct_t) isTmin, (funct_t) test_isTmin, 1, "! ~ & ^ | +", 10, 1,
  {{TMin, TMax},{TMin,TMax},{TMin,TMax}}},
 {"negate", (funct_t) negate, (funct_t) test_negate, 1,
    "! ~ & ^ | + << >>", 5, 2,
  {{TMin, TMax},{TMin,TMax},{TMin,TMax}}},
 {"subOK", (funct_t) subOK, (funct_t) test_subOK, 2,
    "! ~ & ^ | + << >>", 20, 3,
  {{TMin, TMax},{TMin,TMax},{TMin,TMax}}},
 {"negPerByte", (funct_t) negPerByte, (funct_t) test_negPerByte, 1,
    "~ | ^ & << >> + !", 30, 3,
  {{TMin, TMax}, {TMin, TMax}, {TMin, TMax}}},
 {"isGreater", (funct_t) isGreater, (funct_t) test_isGreater, 2,
    "! ~ & ^ | + << >>", 24, 3,
  {{TMin, TMax},{TMin,TMax},{TMin,TMax}}},
 {"modThree", (funct_t) modThree, (funct_t) test_modThree, 1,
    "~ ! | ^ & << >> +", 60, 4,
  {{TMin, TMax}, {TMin, TMax}, {TMin, TMax}}},
 {"howManyBits", (funct_t) howManyBits, (funct_t) test_howManyBits, 1, "! ~ & ^ | + << >>", 90, 4,
  {{TMin, TMax},{TMin,TMax},{TMin,TMax}}},
 {"twos2ones", (funct_t) twos2ones, (funct_t) test_twos2ones, 1,
    "! ~ & ^ | + << >>", 10, 3,
  {{TMin+1, TMax},{TMin,TMax},{TMin,TMax}}},
 {"ones2twos", (funct_t) ones2twos, (funct_t) test_ones2twos, 1,
    "! ~ & ^ | + << >>", 5, 3,
  {{TMin, TMax},{TMin,TMax},{TMin,TMax}}},
 {"float2double", (funct_t) float2double, (funct_t) test_float2double, 1,
    "$", 15, 2,
     {{1, 1},{1,1},{1,1}}},
 {"double_abs", (funct_t) double_abs, (funct_t) test_double_abs, 1,
    "$", 10, 2,
     {{1, 1},{1,1},{1,1}}},
 {"double_neg", (funct_t) double_neg, (funct_t) test_double_neg, 1,
    "$", 10, 2,
     {{1, 1},{1,1},{1,1}}},
 {"double_half", (funct_t) double_half, (funct_t) test_double_half, 1,
    "$", 30, 4,
     {{1, 1},{1,1},{1,1}}},
 {"double_twice", (funct_t) double_twice, (funct_t) test_double_twice, 1,
    "$", 30, 4,
     {{1, 1},{1,1},{1,1}}},
 {"double_pwr2", (funct_t) double_pwr2, (funct_t) test_double_pwr2, 1,
    "$", 30, 4,
     {{1, 1},{1,1},{1,1}}},
 {"double_negpwr2", (funct_t) double_negpwr2, (funct_t) test_double_negpwr2, 1,
    "$", 30, 4,
     {{1, 1},{1,1},{1,1}}},
 {"double_add", (funct_t) double_add, (funct_t) test_double_add, 2,
    "$", 100, 5,
     {{1, 1},{1,1},{1,1}}},
  {"", NULL, NULL, 0, "", 0, 0,
   {{0, 0},{0,0},{0,0}}}
};
