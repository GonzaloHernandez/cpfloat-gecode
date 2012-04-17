/*
       trig2pi.c

   2000 Copyright Timothy J. Hickey
   2003 Copyright David K. Wittenberg
   licence: http://interval.sourceforge.net/licence.txt, zlib/png 

  This contains the code for the trigonometric functions mod 2pi.
  restricted to the range [0,1/4].

*/

/*            CHANGELOG

2003-03-11  changed __BYTE_ORDER to BYTE_ORDER for portability      dkw
2003-12-12  removed some includes duplicated by smath.h             dkw

*/


#include "smath.h"

#if BYTE_ORDER == BIG_ENDIAN
static NEWDOUBLE
    POS_INF       = {{0x7ff0, 0x0000, 0x0000, 0x0000}},
    NEG_INF       = {{0xfff0, 0x0000, 0x0000, 0x0000}};
#endif

#if BYTE_ORDER == LITTLE_ENDIAN
static NEWDOUBLE
    POS_INF       = {{0x0000,0x0000, 0x0000, 0x7ff0 }},
    NEG_INF       = {{0x0000,0x0000, 0x0000, 0xfff0 }};
#endif



double sin2pi0_lo(double x) 
/* x in [-1/4,1/4], result in [-1,1] */
{ INTERVAL Y;
  if (x==POS_INF.d) return(-1.0);
  if (x==NEG_INF.d) return(-1.0);
  Y = sin2pi0DI(x);
  return(Y.lo);
}


double sin2pi0_hi(double x) 
/* x in [-1/4,1/4], result in [-1,1] */
{ INTERVAL Y;
  if (x==POS_INF.d) return(1.0);
  if (x==NEG_INF.d) return(1.0);
  Y = sin2pi0DI(x);
  return(Y.hi);
}


double cos2pi0_lo(double x) 
/* x in [-1/4,1/4], result in [0,1] */
{ INTERVAL Y;
  if (x==POS_INF.d) return(-1.0);
  if (x==NEG_INF.d) return(-1.0);
  Y = cos2pi0DI(x);
  return(Y.lo);
}


double cos2pi0_hi(double x) 
/* x in [-1/4,1/4], result in [0,1] */
{ INTERVAL Y;
  if (x==POS_INF.d) return(1.0);
  if (x==NEG_INF.d) return(1.0);
  Y = cos2pi0DI(x);
  return(Y.hi);
}


double tan2pi0_lo(double x) {
/* x in [0,1/4], result in [0,infty] */
  INTERVAL Y;
  Y = tan2pi0DI(x);
  return(Y.lo);
}

double tan2pi0_hi(double x) {
/* x in [0,1/4], result in [0,infty] */
  INTERVAL Y;
  Y = tan2pi0DI(x);
  return(Y.hi);
}


/* this is a lower bound on asin(x)/(2pi) */
double asin2pi_lo(double x)
/* x in [0,1], result in [0,1/4] */
{ INTERVAL Y;
  Y = asin2piDI(x);
  return(Y.lo);
}

double asin2pi_hi(double x)
/* x in [0,1], result in [0,1/4] */
{ INTERVAL Y;
  Y = asin2piDI(x);
  return(Y.hi);
}


double acos2pi_lo(double x) {
  INTERVAL Y;
  Y = acos2piDI(x);
  return(Y.lo);
}

double acos2pi_hi(double x) {
  INTERVAL Y;
  Y = acos2piDI(x);
  return(Y.hi);
}


double atan2pi_lo(double x) {
  INTERVAL Y;
  Y = atan2piDI(x);
  return(Y.lo);
}

double atan2pi_hi(double x) {
  INTERVAL Y;
  Y = atan2piDI(x);
  return(Y.hi);
}







double sin0_lo(double x) 
/* x in [-1/4,1/4], result in [-1,1] */
{ INTERVAL Y;
  if (x==POS_INF.d) return(-1.0);
  if (x==NEG_INF.d) return(-1.0);
  Y = sin0DI(x);
  return(Y.lo);
}


double sin0_hi(double x) 
/* x in [-1/4,1/4], result in [-1,1] */
{ INTERVAL Y;
  if (x==POS_INF.d) return(1.0);
  if (x==NEG_INF.d) return(1.0);
  Y = sin0DI(x);
  return(Y.hi);
}


double cos0_lo(double x) 
/* x in [-1/4,1/4], result in [0,1] */
{ INTERVAL Y;
  if (x==POS_INF.d) return(-1.0);
  if (x==NEG_INF.d) return(-1.0);
  Y = cos0DI(x);
  return(Y.lo);
}


double cos0_hi(double x) 
/* x in [-1/4,1/4], result in [0,1] */
{ INTERVAL Y;
  if (x==POS_INF.d) return(1.0);
  if (x==NEG_INF.d) return(1.0);
  Y = cos0DI(x);
  return(Y.hi);
}


double tan0_lo(double x) {
  INTERVAL Y;
  Y = tan0DI(x);
  return(Y.lo);
}

double tan0_hi(double x) {
  INTERVAL Y;
  Y = tan0DI(x);
  return(Y.hi);
}


/* this is a lower bound on asin(x)/(2pi) */
double asin_lo(double x)
{ INTERVAL Y;
  Y = asinDI(x);
  return(Y.lo);
}

double asin_hi(double x)
{ INTERVAL Y;
  Y = asinDI(x);
  return(Y.hi);
}


double acos_lo(double x)
{ INTERVAL Y;
  Y = acosDI(x);
  return(Y.lo);
}

double acos_hi(double x)
{ INTERVAL Y;
  Y = acosDI(x);
  return(Y.hi);
}

double atan_lo(double x) {
  INTERVAL X;
  if (x==POS_INF.d) return(-1.0);
  if (x==NEG_INF.d) return(-1.0);
  X = atanDI(x);
  return(X.lo);
}

double atan_hi(double x) {
  INTERVAL X;
  if (x==POS_INF.d) return(1.0);
  if (x==NEG_INF.d) return(1.0);
  X = atanDI(x);
  return(X.hi);
}


































