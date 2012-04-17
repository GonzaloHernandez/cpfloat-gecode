/* ****************************************************************
   ****************************************************************

                         sound_ops.c

   2000 Copyright Timothy J. Hickey
   2003 Copyright David K. Wittenberg
  licence: http://interval.sourceforge.net/licence.txt, zlib/png 

              PART I:  Low level narrowing procedures

   ****************************************************************
   **************************************************************** */

/*       Change Log
2003-02-27   Ported to Mac OS X on PPC                            dkw

    changed roundup, roundnr, rounddn to macros instead of functions
    defined round* for ppc, #ifdef so appropriate defs are used
    mv'd defs of round* out of if rounding = FPSET  (bugfix)
    changed round*word to pent_round*word for setting rounding bits 
    used round* instead of __FPU__SETCW for portability
   
2003-03-11  changed __BYTE_ORDER to BYTE_ORDER for portability      dkw

2003-03-12  copied relevant piece from fpu_control.h - don't include it  dkw

2003-05-06  added comments about rounding modes and optimization   dkw

2003-12-12  removed some includes duplicated by smath               dkw
 
*/


/* darwin needs gcc 3.1 or later - per gcc web site */


#define FPSET 1
#define NEXTFP 2
#define FAST_FPSET 3


#define ROUNDING_METHOD FPSET

/*
  the three alternatives are:
  #define ROUNDING_METHOD FPSET
  #define ROUNDING_METHOD NEXTFP
  #define ROUNDING_METHOD FAST_FPSET
    only FPSET has been tested carefully
*/


#include "smath.h" 
#include <stdio.h>

/* Setting the rounding mode now relies on mpfr */

#include <mpfr.h>

// GMP_RNDN: round to nearest
// GMP_RNDZ: round towards zero
// GMP_RNDU: round towards plus infinity
// GMP_RNDD: round towards minus infinity

#define  roundup   mpfr_set_default_rounding_mode(GMP_RNDU);

#define roundnr    mpfr_set_default_rounding_mode(GMP_RNDN);

#define  rounddn   mpfr_set_default_rounding_mode(GMP_RNDD);


#if BYTE_ORDER == LITTLE_ENDIAN
static NEWDOUBLE
    MAX_REAL      = {{0xffff, 0xffff, 0xffff, 0x7fef}},
    POS_INF       = {{0x0000, 0x0000, 0x0000, 0x7fff}},
    NEG_INF       = {{0x0000, 0x0000, 0x0000, 0xfff0}};
#endif

#if BYTE_ORDER == BIG_ENDIAN
static NEWDOUBLE
    MAX_REAL      = {{0x7fef, 0xffff, 0xffff, 0xffff}},
    POS_INF       = {{0x7ff0, 0x0000, 0x0000, 0x0000}},
    NEG_INF       = {{0xfff0, 0x0000, 0x0000, 0x0000}};
#endif

void init_real_constants()
{//double x,z,w;
 //int *y;

}

void print_fp(x)
double x;
{

  if (x == POS_INF.d) printf("+inf");
  else if (x == NEG_INF.d) printf("-inf");
  else printf("%30.20g",x);

}


/* ****************************************************************
  These routines will select the next highest and next lowest fp numbers
   **************************************************************** */


double next_fp(double x)
{
  unsigned *y,*z;

  if (x == NEG_INF.d) return(-MAX_REAL.d);

#if BYTE_ORDER ==  BIG_ENDIAN
  y = ((unsigned *) (&x));
  z = y+1;
#endif
#if BYTE_ORDER ==  LITTLE_ENDIAN
  z = ((unsigned *) (&x));
  y = z+1;
#endif

  /* this handles the nan case */
  if (((*y & 0x7ff00000) == 0x7ff00000) && ((*y &0x000fffff) != 0))
    return(x);


  if (x == POS_INF.d)
    return(POS_INF.d);   
  else if (x > 0){
    *z = *z + 1;
    if (*z == 0)
      {
       *y = *y + 1;
     }
  } else if (x < 0){
    x = -x;
    if (*z != 0)
      {*z -= 1;}
    else {
      *z = 0xffffffff;
      *y -= 1;
    }
    x = -x;
  } else {
    *z = 1;
    *y= 0;
  }
  return(x);
}

double prev_fp(double x)
{
  return( - next_fp(-x));
}


/* ****************************************************************
    Sound arithmetic operations

    The version with rounding control causes some bugs to appear
    in the powerseries examples.

   **************************************************************** */


#if ROUNDING_METHOD==FPSET 


double add_hi(double x,double y)
{
  roundup;
  return(x+y);
}

double add_lo(double x,double y)
{
  rounddn;
  return(x+y);
}

double sub_hi(double x,double y)
{
  roundup;
  return(x-y);
}

double sub_lo(double x,double y)
{
  rounddn;
  return(x-y);
}

double mul_hi(double x,double y)
{
  roundup;
  return(x*y);
}

double mul_lo(double x,double y)
{
  rounddn;
  return(x*y);
}

double div_hi(double x,double y)
{
  double z;
  roundup;
  z = x/y;
  return(z);
}

double div_lo(double x, double y)
{
 double z;
  rounddn;
  z = x/y;
  return(z);
}

double sqrt_hi(double x)
{
  if (x==0) return(0.0);
  roundup;
  return(sqrt(x));
}

double sqrt_lo(double x)
{
  if (x==0) return(0.0);
  rounddn;
  return(sqrt(x));
}
/* above here, we assume rounding is fpset */


#elif ROUNDING_METHOD == FAST_FPSET 
/* here we assume that the rounding mode is always 
   set to round toward negative infinity, ie is FP_RM
*/

double add_hi(x,y)
double x,y;
{
  return(-((-x)+(-y)));
}

double add_lo(x,y)
double x,y;
{
  return(x+y);
}

double sub_hi(x,y)
double x,y;
{
  return(-(y-x));
}

double sub_lo(x,y)
double x,y;
{
  return(x-y);
}

double mul_hi(x,y)
double x,y;
{
  return(-((-x)*y));
}

double mul_lo(x,y)
double x,y;
{
  return(x*y);
}

double div_hi(x,y)
double x,y;
{
  double z;
  z = -(-x)/y;
  return(z);
}

double div_lo(x,y)
double x,y;
{
 double z;
  z = x/y;
  return(z);
}

double sqrt_hi(x)
double x;
{
  double a;
  if (x==0) return(0.0);
  roundup;
  a = sqrt(x);
  rounddn;
   return(a);
}

double sqrt_lo(x)
double x;
{
  if (x==0) return(0.0);
  return(sqrt(x));
}

/* this is the end of rounding method is fast_fpset */

#elif ROUNDING_METHOD == NEXTFP

double add_hi(x,y)
double x,y;
{
  return(next_fp(x+y));
}

double add_lo(x,y)
double x,y;
{
  return(prev_fp(x+y));
}

double sub_hi(x,y)
double x,y;
{
  return(next_fp(x-y));
}

double sub_lo(x,y)
double x,y;
{
  return(prev_fp(x-y));
}

double mul_hi(x,y)
double x,y;
{
  return(next_fp(x*y));
}

double mul_lo(x,y)
double x,y;
{
  return(prev_fp(x*y));
}

double div_hi(x,y)
double x,y;
{
  return(next_fp(x/y));
}

double div_lo(x,y)
double x,y;
{
  return(prev_fp(x/y));
}

double sqrt_hi(x)
double x;
{
  if (x==0) return(0.0);
  return(next_fp(sqrt(x)));
}

double sqrt_lo(x)
double x;
{
  if (x==0) return(0.0);
  return(prev_fp(sqrt(x)));
}

#endif  /* rounding method is nextfp */


