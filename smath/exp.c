/* 
     exp.c

   2000 Copyright Timothy J. Hickey
   licence: http://interval.sourceforge.net/licence.txt, zlib/png 

*/


/* #include <ieeefp.h> */
#include "smath.h"



double exp_lo(double x) {
  INTERVAL Y;
  Y = expDI(x);
  return(Y.lo);
}

double exp_hi(double x) {
  INTERVAL Y;
  Y = expDI(x);
  return(Y.hi);
}


double log_lo(double x) {
  INTERVAL Y;
  Y = logDI(x);
  return(Y.lo);
}

double log_hi(double x) {
  INTERVAL Y;
  Y = logDI(x);
  return(Y.hi);
}



int narrow_exp(INTERVAL *A,INTERVAL *B) {
  INTERVAL X,Y;
  X.hi = A->hi;X.lo = A->lo;
  Y.hi = B->hi;Y.lo = B->lo;
  Y = intersectIII(Y,expII(X));
  X = intersectIII(X,logII(Y));
  A->hi = X.hi; A->lo = X.lo;
  B->hi = Y.hi; B->lo = Y.lo;
  if ((X.hi < X.lo) || (Y.hi < Y.lo))
    return(0);
  else return(1);  
}



int narrow_log(INTERVAL *x,INTERVAL *y)
{
  INTERVAL X;

   // no element in x is in dom(LOG):
  if (x->hi <= 0)
    return 1;


  // LOG is not defined for all values in x:
  if (x->lo <= 0){
    X = *x;
    X = intersectIII(X,expII(*y));
    x->hi = (X.hi < X.lo) ? 0 : X.hi;
    return 1;
  }

  // LOG defined for all values in x:
  *y = intersectIII(*y,logII(*x));
  *x = intersectIII(*x,expII(*y));
  return (x->hi >= x->lo && y->hi >= y->lo);
  
}


