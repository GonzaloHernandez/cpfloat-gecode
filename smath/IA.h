/*
  IA.h
   This is the include file for a simple interval arithmetic package.

   2000 Copyright Timothy J. Hickey
   licence: http://interval.sourceforge.net/licence.txt, zlib/png 

*/

typedef struct interval{
  double lo;
  double hi;
} INTERVAL, *INTERVALP;

void print_interval(char *s, INTERVAL p);

typedef union udouble {
  unsigned int u[2];
  double d;
} UDOUBLE;


typedef union newdouble {
  unsigned short int usi[4];
  double d;
} NEWDOUBLE, NEWDOUBLE3[3];

typedef union newinterval {
  unsigned short int v[8];
  INTERVAL i;
} NEWINTERVAL, *NEWINTERVALP;

typedef struct ext_prec {
  NEWDOUBLE   a;
  NEWINTERVAL b;
} EXT_PREC;


#define FAIL 0
#define TRUE 1


/*
  These definitions are meant to be used
  when the rounding mode is set to round
  toward minus infinity at all times.
  In that case, we have
    OP_0(x,y) = OP_lo(x,y)
    OP_1(x,y) = OP_hi(x,y)
  and we can compute with both rounding
  modes without having to make an explicit
  rounding mode call.
*/
#define ADD_0(x,y)   (( x)+( y))
#define SUB_0(x,y)   (( x)-( y))
#define MUL_0(x,y)   (( x)*( y))
#define DIV_0(x,y)   (( x)/( y))

#define ADD_1(x,y) (-((-(x))+(-(y))))
#define SUB_1(x,y) (-((  y )-(  x )))
#define MUL_1(x,y) (-((-(x))*(  y )))
#define DIV_1(x,y) (-((-(x))/(  y )))



/* PURE INTERVAL ARITHMETIC OPERATORS */
INTERVAL cnstDI(double a);
INTERVAL makeDDI(double lo,double hi);
INTERVAL addIII(INTERVAL a, INTERVAL b);
INTERVAL subIII(INTERVAL a, INTERVAL b);
INTERVAL mulIII(INTERVAL a, INTERVAL b);
INTERVAL divIII(INTERVAL a, INTERVAL b);
INTERVAL negII(INTERVAL a);
INTERVAL sqrtII(INTERVAL A);
INTERVAL intersectIII(INTERVAL a, INTERVAL b);

INTERVAL expII(INTERVAL X);
INTERVAL logII(INTERVAL X);

INTERVAL sinII(INTERVAL X);
INTERVAL cosII(INTERVAL X);


/* MIXED INTERVAL/DOUBLE ARITHMETIC OPERATORS */
INTERVAL addIDI(INTERVAL a, double b);
INTERVAL subIDI(INTERVAL a, double b);
INTERVAL mulIDI(INTERVAL a, double b);
INTERVAL divIDI(INTERVAL a, double b);

INTERVAL addDII(double a, INTERVAL b);
INTERVAL subDII(double a, INTERVAL b);
INTERVAL mulDII(double a, INTERVAL b);
INTERVAL divDII(double a, INTERVAL b);

INTERVAL addDDI(double a, double b);
INTERVAL subDDI(double a, double b);
INTERVAL mulDDI(double a, double b);
INTERVAL divDDI(double a, double b);

INTERVAL expDI(double x);
INTERVAL logDI(double x);

INTERVAL sin2piDI(double x);
INTERVAL asin2piDI(double x);

INTERVAL two_to_nDI(int n);

/* SPECIAL CASE OPERATORS */

/* intersect x with a quotient set a/b */
/* Y = intersect((a/b),x) */
INTERVAL intersect_divIII(INTERVAL a, INTERVAL b, INTERVAL x);


