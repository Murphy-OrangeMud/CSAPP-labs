#include <stdio.h>
#include <stdlib.h>
#include <math.h>


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

/* Fix any NaN to be NaN value 0x7FC00000 */
double fix_nan(double f)
{
  unsigned long long uf = d2u(f);
  unsigned long long unf = d2u(-f);
  unsigned long long exp_test = 0x7FF00000;
  exp_test <<= 32;
  unsigned long long exp_num = (uf & exp_test) >> 52;
  if (exp_num == 0x7ff)
    return u2d(exp_test);
  else
    return f;
}

unsigned long long double_negpwr2(long long x) {
	long long exp, val;
	unsigned long long xx = 1;
	if (x > 1074) return 0;
	else if (x > 1022) {
		val = (xx<<52)>>(-1022+x);
	}
	else if (x < -1023) {
		exp = 2047; val = 0;
	}
	else {
		exp = -x + 1023;val = 0;
	}
    return val|(exp << 52);
 
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


int main()
{
    unsigned long long check_point = 0;
    unsigned long long count = 0;
    unsigned long long max_num = ((unsigned long long)1) << 26;
    
    while(1)
    {
        unsigned long long eps = ((unsigned long long)1) << 38 | 1 << 10;
        check_point += (eps);
        count++;
        if(count >= max_num)
            break;

        long long val1 = double_negpwr2(check_point);
        long long val2 = test_double_negpwr2(check_point);
        if(val1 != val2)
        {
            return -1;
        }
    }
    return 0;
}
