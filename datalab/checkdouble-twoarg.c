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

unsigned long long double_add(unsigned long long f1, unsigned long long f2) {
	unsigned long long xx = 1;
	unsigned long long yyy = 0xfffff;
	unsigned long long yy = ((yyy<<32)|0xffffffff);
	//printf("%0llx\n", yy);
	long long exp1;
	long long exp2;
	long long val1;
	long long val2;
	int subexp;
	int guard;
	int round;
	int sticky;
	int x;
	f1 &= (~(xx<<63)); f2 &= (~(xx<<63));
//printf("before exchange: %lld %lld\n", f1, f2);
	if (((((f1>>52)&0x7ff)^0x7ff) == 0)||((((f2>>52)&0x7ff)^0x7ff) == 0)) return 0;
	else {
		
		if (f1 < f2)
		{
			f1 = f1^f2;
			f2 = f1^f2;
			f1 = f1^f2;
			//printf("after exchange: %lld %lld\n", f1, f2);
		}
		
		exp1 = (f1>>52)&0x7ff;
		exp2 = (f2>>52)&0x7ff;
		//printf("exp %lld %lld\n",exp1,exp2);
		val1 = f1 & yy;
		val2 = f2 & yy;
		if(exp1) val1 += (xx << 52);
		if(exp2) val2 += (xx << 52);
		if (exp1 > exp2){
			subexp = exp1 - exp2;
			//guard = (val2 >> subexp)&1;
			round = (val2 >> (subexp-1))&1;
			sticky = ((val2>>(subexp-2))&1);
			for (x = subexp-3; x >= 0; --x)
			{
				sticky = sticky | ((val2 >> x)&1);
			}
			val2 = val2 >> (subexp);
			val1 += val2;
			guard = (val1&1);
			//printf("%d %d %d %d\n", subexp, guard, round, sticky);
			if ((val1>>53)&1){
				//printf("if\n");
				val1 >>= 1;
				exp1 += 1;
				if (guard&(round|sticky|(val1&1)))
				{
					//printf("if and if\n");					
					val1 += 1;
					if ((val1>>53)&1)
					{	
						//printf("if and if and if\n");	
						val1 >>= 1;
						exp1 += 1;
					}
				}
				if ((val1 >> 52)&1) val1 -= (xx<<52);
			}
			else {
				//printf("else\n");
				if (round & (guard|sticky)) {
					//printf("else and if\n");
					val1 += 1;
					if ((val1>>53)&1)
					{	
						//printf("else and if and if\n");
						val1 >>= 1;
						exp1 += 1;
					}			
				}
				if ((val1 >> 52)&1) val1 -= (xx<<52);
			}
		}
		else if (exp1 == exp2)
		{
			val1 += val2;
			if (((val1>>53)&1)&&exp1)
			{
				guard = (val1>>1) & 1;
				round = val1 & 1;
				//printf("else if and if\n");
				val1 >>= 1;
				if (guard&round) {
					//printf("round\n");
					val1 += 1;
				}
				exp1 += 1;
			}
			if ((val1 >> 52)&1) {
				//printf("52\n");
				val1 -= (xx<<52);
			}
		}
	//printf("val %lld %lld\n",val1,val2);
	//printf("result %lld\n",((exp1<<52)|val1));
	return (exp1<<52)|val1;
	}
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
	//test
	//unsigned long long d = d2u(d1 + d2);
	//unsigned long long *p = &d;
	//printf("testres %lf\n",*(double*)p);
	//getchar();
    return d2u(d1 + d2);
}


int main()
{
    unsigned long long check_point = 0, check_point2 = 0;
    unsigned long long count = 0;
    unsigned long long max_num = ((unsigned long long)1) << 26;
    
    while(1)
    {
        unsigned long long eps = ((unsigned long long)1) << 38 | 1 << 10;
        check_point += (eps);
        check_point2 = ((unsigned long long)1) << 63 - check_point;
        count++;
        if(count >= max_num)
            break;

        long long val1 = double_add(check_point, check_point2);
        long long val2 = test_double_add(check_point, check_point2);
        if(val1 != val2)
        {
            return -1;
        }
    }
    return 0;
}
