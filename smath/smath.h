/*
    smath.h

   2000,2003  Copyright Timothy J. Hickey, David K. Wittenberg


   licence: http://interval.sourceforge.net/licence.txt, zlib/png

    Data structures and prototypes for the sound math library.
    The library has several types of sound math operations:
      * narrowing operators
      * semi-narrowing intersection operators
      * interval arithmetic operators
      * soundly rounded functions


    More precisely, the smath library implements the
    following four types of functions:

    I. Narrowing functions.
       These function have the form narrow_op(A1,...,An)
       where the Ai are pointers to intervals, and "op" is
       a function or relation. Thus,
         narrow_add(&x,&y,&z) will narrow the x,y,z intervals
       without removing any solutions to x+y=z. The narrowing
       functions return false if any of the argument intervals
       become narrowed to the empty set, i.e., if the relation
       op(x1,...,xn) has been shown to have no solutions with
       (x1,...,xn) in (A1,...,An)


    II. Semi-narrowing functions.
       These functions have the form intersect_op(A1,...,An,X)
       where the Ai and *X are intervals, and "op" is a function.
       They implement the following operation:
               X = (X intersect op(A1,...,An))
       and they are especially useful when op(A1,...,An) is a
       union of intervals. If the intersection is empty, then
       intersect_op returns FAIL, otherwise it returns TRUE.

       Thus, intersect_inv_sin(*a,*b) will intersect b with an
       enclosure of sin^(-1)(a), but will have no effect on a.
       Notice that we can implement narrow_sin(*x,*y) as
        return((intersect_sin(x,y)&&intersect_inv_sin(y,x)));
       and indeed, any narrowing function can be implemented
       by "and"ing together the semi-narrowing functions for
       its component "functions".

    III. Interval extensions of functions.
      These have the form opII(A1) where A1 is an interval,
      and op is a function, and opII(A1) is an interval.
      Thus, expII(x) returns an interval y which contains
      all points exp(t) with t in x.

    IV. Soundly rounded functions.
      These have the form op_lo(x) (resp. op_hi(x)) and they
      return lower (resp. upper) bounds on op(x). Thus,
         exp_lo(x) <= exp(x) <= exp_hi(x),    for all x

*/
/*               CHANGELOG

2003-12-12    moved includes here from .c files        dkw
*/

/****************************************************
              Includes
*******************************************/


#include <math.h>


/*  This test should be for macos, but I couldn't find that #def */
#ifdef __APPLE__ 
  #ifdef __MACH__
    #include <machine/endian.h>
  #endif
#else
  #include <endian.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif


    /*
    ****************************************************************
              smath data structures
    ****************************************************************
    */


typedef struct interval{
  double lo;
  double hi;
} INTERVAL, *INTERVALP;


    /*
    ****************************************************************
    All of the following data structures are used to initialize
    double precision fields in data structures
    ****************************************************************
    */

typedef union ldouble {
  long int u;
  double d;
} LDOUBLE;


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
    ****************************************************************
    Some convenient I/O functions for debugging
    ****************************************************************
    */

void print_interval(char *s, INTERVAL p);
void print_fp(double x);


    /*
    ****************************************************************
    The Narrowing Functions
    These return 1 for success and 0 for failure.
    ****************************************************************
    */

/* arithmetic */
int narrow_add(INTERVAL *x,INTERVAL *y, INTERVAL *z);
int narrow_mul(INTERVAL *x,INTERVAL *y, INTERVAL *z);
int narrow_u_minus(INTERVAL *x, INTERVAL *y);
int narrow_square(INTERVAL *x, INTERVAL *y);

/* exponential functions */
int narrow_exp(INTERVAL *x, INTERVAL *y);
int narrow_log(INTERVAL *x, INTERVAL *y);

/* equality, inequality, and disequality relations */
int narrow_eq(INTERVAL *x,INTERVAL *y);
int narrow_lt(INTERVAL *x,INTERVAL *y);
int narrow_le(INTERVAL *x,INTERVAL *y);
int narrow_ne(INTERVAL *x,INTERVAL *y);

/* type functions */
int narrow_integer(INTERVAL *x);
int narrow_bool(INTERVAL *x);

/* miscellaneous real functions */
int narrow_abs(INTERVAL *x,INTERVAL *y);
int narrow_sgn(INTERVAL *x,INTERVAL *y);
int narrow_min(INTERVAL *x,INTERVAL *y, INTERVAL *z);
int narrow_max(INTERVAL *x,INTERVAL *y, INTERVAL *z);
int narrow_flr(INTERVAL *x,INTERVAL *y);
int narrow_ceil(INTERVAL *x,INTERVAL *y);

/* boolean functions */
int narrow_or(INTERVAL *x,INTERVAL *y, INTERVAL *z);
int narrow_and(INTERVAL *x,INTERVAL *y, INTERVAL *z);
int narrow_xor(INTERVAL *x,INTERVAL *y, INTERVAL *z);
int narrow_not(INTERVAL *x,INTERVAL *y);
int narrow_imp(INTERVAL *x,INTERVAL *y, INTERVAL *z);
int narrow_if(INTERVAL *x,INTERVAL *y, INTERVAL *z);

/* comparison functions */
int narrow_lessfn(INTERVAL *x,INTERVAL *y, INTERVAL *z);
int narrow_leqfn(INTERVAL *x,INTERVAL *y, INTERVAL *z);
int narrow_eqfn(INTERVAL *x,INTERVAL *y, INTERVAL *z);

/* trig functions */
int narrow_sin(INTERVAL *x,INTERVAL *y);
int narrow_cos(INTERVAL *x,INTERVAL *y);
int narrow_tan(INTERVAL *x,INTERVAL *y);

int narrow_asin(INTERVAL *x,INTERVAL *y);
int narrow_acos(INTERVAL *x,INTERVAL *y);
int narrow_atan(INTERVAL *x,INTERVAL *y);

int narrow_sin2pi(INTERVAL *x,INTERVAL *y);
int narrow_cos2pi(INTERVAL *x,INTERVAL *y);
int narrow_tan2pi(INTERVAL *x,INTERVAL *y);

int narrow_asin2pi(INTERVAL *x,INTERVAL *y);
int narrow_acos2pi(INTERVAL *x,INTERVAL *y);
int narrow_atan2pi(INTERVAL *x,INTERVAL *y);

/* the subset operation */
int narrow_subset(INTERVAL *x,INTERVAL *y);

/* power narrowing procedures */
int narrow_pow_odd(INTERVAL *x, INTERVAL *y, INTERVAL *z);
int narrow_pow_even(INTERVAL *x, INTERVAL *y, INTERVAL *z);


    /*
    ****************************************************************
    THE INTERSECTION OPERATORS, a.k.a the SEMI-NARROWING FUNCTIONS
    These operators have the form  intersect_f(A,...,*X)
    They are essentially half of the narrowing operation
          X=f(A,...)
    In that they intersect X with the image f(A,...).
    If the intersection is empty, then the functions returns FAIL.
    ****************************************************************
    */
int intersect_mulIII(INTERVAL a, INTERVAL b, INTERVAL *x);
int intersect_divIII(INTERVAL a, INTERVAL b, INTERVAL *x);

int intersect_sin2piII(INTERVAL z, INTERVAL *x);
int intersect_cos2piII(INTERVAL z, INTERVAL *x);
int intersect_tan2piII(INTERVAL z, INTERVAL *x);
int intersect_inv_sin2piII(INTERVAL z, INTERVAL *x);
int intersect_inv_cos2piII(INTERVAL z, INTERVAL *x);
int intersect_inv_tan2piII(INTERVAL z, INTERVAL *x);

int intersect_sinII(INTERVAL z, INTERVAL *x);
int intersect_cosII(INTERVAL z, INTERVAL *x);
int intersect_tanII(INTERVAL z, INTERVAL *x);
int intersect_inv_sinII(INTERVAL z, INTERVAL *x);
int intersect_inv_cosII(INTERVAL z, INTERVAL *x);
int intersect_inv_tanII(INTERVAL z, INTERVAL *x);





    /*
    ****************************************************************
    The INTERVAL ARITHMETIC OPERATORS
    ****************************************************************
    */

/* constructors */
INTERVAL cnstDI(double a);
INTERVAL makeDDI(double lo,double hi);

/* pure interval arithmetic operators */
INTERVAL addIII(INTERVAL a, INTERVAL b);
INTERVAL subIII(INTERVAL a, INTERVAL b);
INTERVAL mulIII(INTERVAL a, INTERVAL b);
INTERVAL divIII(INTERVAL a, INTERVAL b);
INTERVAL negII(INTERVAL a);
INTERVAL sqrtII(INTERVAL A);
INTERVAL squareII(INTERVAL A);

INTERVAL intersectIII(INTERVAL a, INTERVAL b);
INTERVAL unionIII(INTERVAL A, INTERVAL B);

INTERVAL absII(INTERVAL a);
INTERVAL sgnII(INTERVAL a);
INTERVAL floorII(INTERVAL a);
INTERVAL ceilII(INTERVAL a);

INTERVAL maxIII(INTERVAL a,INTERVAL b);
INTERVAL minIII(INTERVAL a,INTERVAL b);

INTERVAL two_to_nDI(int n);

INTERVAL expII(INTERVAL X);
INTERVAL logII(INTERVAL X);


INTERVAL sin2piII(INTERVAL X);
INTERVAL cos2piII(INTERVAL X);
INTERVAL tan2piII(INTERVAL X);
INTERVAL asin2piII(INTERVAL X);
INTERVAL acos2piII(INTERVAL X);
INTERVAL atan2piII(INTERVAL X);

INTERVAL sinII(INTERVAL X);
INTERVAL cosII(INTERVAL X);
INTERVAL tanII(INTERVAL X);
INTERVAL asinII(INTERVAL X);
INTERVAL acosII(INTERVAL X);
INTERVAL atanII(INTERVAL X);


/* mixed interval/double arithmetic operators */
/* these are convenience functions */
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

INTERVAL sin2pi0DI(double x);
INTERVAL cos2pi0DI(double x);
INTERVAL tan2pi0DI(double x);
INTERVAL asin2piDI(double x);
INTERVAL acos2piDI(double x);
INTERVAL atan2piDI(double x);

INTERVAL sin0DI(double x);
INTERVAL cos0DI(double x);
INTERVAL tan0DI(double x);
INTERVAL asinDI(double x);
INTERVAL acosDI(double x);
INTERVAL atanDI(double x);

INTERVAL unionDDI(double a, double b);

    /*
    ****************************************************************
    SOUNDLY ROUNDED FUNCTIONS
    ****************************************************************
    */


double next_fp(double x);
double prev_fp(double x);

double add_hi(double x,double y);
double add_lo(double x,double y);

double sub_hi(double x,double y);
double sub_lo(double x,double y);

double mul_hi(double x,double y);
double mul_lo(double x,double y);

double div_hi(double x,double y);
double div_lo(double x,double y);

double exp_hi(double x);
double exp_lo(double x);

double log_hi(double x);
double log_lo(double x);

double sqrt_hi(double x);
double sqrt_lo(double x);



double sin2pi0_lo(double x);
double sin2pi0_hi(double x);

double cos2pi0_lo(double x);
double cos2pi0_hi(double x);

double tan2pi0_lo(double x);
double tan2pi0_hi(double x);

double asin2pi_lo(double x);
double asin2pi_hi(double x);

double acos2pi_lo(double x);
double acos2pi_hi(double x);

double atan2pi_lo(double x);
double atan2pi_hi(double x);


double sin0_lo(double x);
double sin0_hi(double x);

double cos0_lo(double x);
double cos0_hi(double x);

double tan0_lo(double x);
double tan0_hi(double x);

double asin_lo(double x);
double asin_hi(double x);

double acos_lo(double x);
double acos_hi(double x);

double atan_lo(double x);
double atan_hi(double x);


    /*
    ****************************************************************
      Macros for fast soundly rounded arithmetic.

      The following definitions are meant to be used
      when the rounding mode is set to round
      toward minus infinity at all times.
      In that case, we have
        OP_0(x,y) = OP_lo(x,y)
        OP_1(x,y) = OP_hi(x,y)
      and we can compute with both rounding
      modes without having to make an explicit
      operating system call to change the
      rounding mode.
    ****************************************************************
    */

#define ADD_0(x,y)   (( x)+( y))
#define SUB_0(x,y)   (( x)-( y))
#define MUL_0(x,y)   (( x)*( y))
#define DIV_0(x,y)   (( x)/( y))

#define ADD_1(x,y) (-((-(x))+(-(y))))
#define SUB_1(x,y) (-((  y )-(  x )))
#define MUL_1(x,y) (-((-(x))*(  y )))
#define DIV_1(x,y) (-((-(x))/(  y )))

#ifdef __cplusplus
}
#endif



