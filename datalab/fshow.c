/* Display structure of double precision floating-point numbers */

#include <stdio.h>
#include <stdlib.h>

/* Declarations of some nonstandard library functions */
double strtod(const char *nptr, char **endptr);
long long strtoll(const char *s, char **ptr, int base);
char *strdup(const char *s);


#define DOUBLE_SIZE 64
#define FRAC_SIZE 52
#define EXP_SIZE 11
#define BIAS (((long long)1<<(EXP_SIZE-1))-1)
#define FRAC_MASK (((long long)1<<FRAC_SIZE)-1)
#define EXP_MASK (((long long)1<<EXP_SIZE)-1)

/* double point helpers */
unsigned long long f2u(double f)
{
  union {
    unsigned long long u;
    double f;
  } v;
  v.u = 0;
  v.f = f;
  return v.u;
}

double u2f(unsigned long long u)
{
  union {
    unsigned long long u;
    double f;
  } v;
  v.u = u;
  return v.f;
}

/* Get exponent */
unsigned long long get_exp(unsigned long long uf)
{
  return (uf>>FRAC_SIZE) & EXP_MASK;
}

/* Get fraction */
unsigned long long get_frac(unsigned long long uf)
{
  return uf & FRAC_MASK;
}

/* Get sign */
unsigned long long get_sign(unsigned long long uf)
{
  return (uf>>(DOUBLE_SIZE-1)) & 0x1;
}

void show_double(unsigned long long uf)
{
  double f = u2f(uf);
  unsigned long long exp = get_exp(uf);
  unsigned long long frac = get_frac(uf);
  unsigned long long sign = get_sign(uf);

  printf("\nDouble Precision Floating point value %.10g\n", f);
  printf("Bit Representation 0x%.8llx, sign = %llx, exponent = 0x%.2llx, fraction = 0x%.6llx\n",
	 uf, sign, exp, frac);
  if (exp == EXP_MASK) {
    if (frac == 0) {
      printf("%cInfinity\n", sign ? '-' : '+');
    } else
      printf("Not-A-Number\n");
  } else {
    long long denorm = (exp == 0);
    long long uexp = denorm ? 1-BIAS : exp - BIAS;
    long long mantissa = denorm ? frac : frac + ((unsigned long long)1<<FRAC_SIZE);
    double fman = (double) mantissa / (double) ((unsigned long long)1<<FRAC_SIZE);
    printf("%s.  %c%.10f X 2^(%lld)\n",
	   denorm ? "Denormalized" : "Normalized",
	   sign ? '-' : '+',
	   fman, uexp);
  }
}

/* Extract hex/decimal/or double value from string */
static long long get_num_val(char *sval, unsigned long long *valp) {
  char *endp;
  /* See if it's an integer or doubleing point */
  int ishex = 0;
  int isdouble = 0;
  int i;
  for (i = 0; sval[i]; i++) {
    switch (sval[i]) {
    case 'x':
    case 'X':
      ishex = 1;
      break;
    case 'e':
    case 'E':
      if (!ishex)
	isdouble = 1;
      break;
    case '.':
      isdouble = 1;
      break;
    default:
      break;
    }
  }
  if (isdouble) {
    double fval = strtod(sval, &endp);
    if (!*endp) {
      *valp = *(unsigned long long *) &fval;
      return 1;
    }
    return 0;
  } else {
    long long llval = strtoll(sval, &endp, 0);
    long long upperbits = llval >> 31;
    /* will give -1 for negative, 0 or 1 for positive */
    if (valp && (upperbits == 0 || upperbits == -1 || upperbits == 1)) {
      *valp = (unsigned long long) llval;
      return 1;
    }
    return 0;
  }
}


void usage(char *fname) {
  printf("Usage: %s val1 val2 ...\n", fname);
  printf("Values may be given as hex patterns or as doubleing point numbers\n");
  exit(0);
}


int main(int argc, char *argv[])
{
  int i;
  unsigned long long uf;
  if (argc < 2)
    usage(argv[0]);
  for (i = 1; i < argc; i++) {
    char *sval = argv[i];
    if (get_num_val(sval, &uf)) {
      show_double(uf);
    } else {
      printf("Invalid 32-bit number: '%s'\n", sval);
      usage(argv[0]);
    }
  }
  return 0;
}


