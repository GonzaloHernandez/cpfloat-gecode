/*
  File:     "bnr.c"

   2000 Copyright Timothy J. Hickey
   licence: http://interval.sourceforge.net/licence.txt, zlib/png 

*/

#include "smath.h"
#include <stdio.h>

    /*
    ****************************************************************
    Narrowing procedures for odd and even integer powers
    ****************************************************************
    */

int narrow_pow_odd(INTERVAL *x, INTERVAL *y, INTERVAL *z) {
  /* here we use x^y = z => abs(x)^y = abs(z),m for y odd */
  /* this I need to redo, it is pretty ugly! */
  /*

       z = x^y, y and odd integer, 

     which is equivalent to narrowing the following function:

       z = sgn(x)*exp(y*log(|x|))
       x = sgn(z)*exp(1/y*log(|z|))

     Since these are monotone functions (for "fixed" y), they attain
     their max and min at the end points, thus we need only evaluate these
     functions at the endpoints to evaluate them on an interval.
     
  */

 INTERVAL  t1; //,t2;

/*
    t1 = mulIII(sgnII(cnstDI(x->lo)),expII(mulIII(*y,logII(absII(cnstDI(x->lo))))));
    t2 = mulIII(sgnII(cnstDI(x->hi)),expII(mulIII(*y,logII(absII(cnstDI(x->hi))))));
    *z = intersectIII(*z,unionIII(t1,t2));

    t1 = mulIII(sgnII(cnstDI(z->lo)),expII(divIII(logII(absII(cnstDI(z->lo))),*y)));
    t2 = mulIII(sgnII(cnstDI(z->hi)),expII(divIII(logII(absII(cnstDI(z->hi))),*y)));
    *x = intersectIII(*x,unionIII(t1,t2));
*/

    t1 = mulIII(sgnII(*x),expII(mulIII(*y,logII(absII(*x)))));
    *z = intersectIII(*z,t1);

    t1 = mulIII(sgnII(*z),expII(divIII(logII(absII(*z)),*y)));
    *x = intersectIII(*x,t1);

    return((z->hi >= z->lo) && (x->hi >= x->lo));
/*

  INTERVAL lnx, lnz, absx, absz;
  int result;

  lnx = logII(absII(*x));
  lnz = logII(absII(*z));

  printf("Entering narrow_pow_odd\n");
  print_interval("x=",*x);
  print_interval("y=",*y);
  print_interval("z=",*z);
  print_interval("lnx=",lnx);
  print_interval("lnz=",lnz);

  if (!(result = narrow_mul(&lnx,y,&lnz))) {
    absx = expII(lnx);
    absz = expII(lnz);
    result = (narrow_abs(x, &absx) && narrow_abs(z, &absz));
    print_interval("absx=",absx);
    print_interval("absz=",absz);
    print_interval("lnx=",lnx);
    print_interval("lnz=",lnz);
    print_interval("x=",*x);
    print_interval("y=",*y);
    print_interval("z=",*z);
    printf("leaving narrow_pow_odd\n\n");
  }
 return(result);
*/

}  

int narrow_pow_even(INTERVAL *x, INTERVAL *y, INTERVAL *z) {
  /* here we use x^(2y) = z => abs(x)^(2y) = z */
  INTERVAL lnx,lnz,absx;
  int result;
  int debug = 0;

  if (debug) {
    printf("Entering pow_even\n");
    print_interval("x =",*x);
    print_interval("y =",*y);
    print_interval("z =",*z);
  }
  lnx = logII(absII(*x));
  lnz = logII(*z);

  if (debug) {
    printf("computing logs\n");
    print_interval("lnx =",lnx);
    print_interval("lnz =",lnz);
  }
  result = narrow_mul(&lnx,y,&lnz);

  if (debug) {
    printf("narrowing mult -> %d\n",result);
    print_interval("lnx =",lnx);
    print_interval("y =",*y);
    print_interval("lnz =",lnz);
  }
  if (result) {
    absx = expII(lnx);
    *z = intersectIII(*z,expII(lnz));

  if (debug) {
    print_interval("absx =",*x);
    print_interval("z =",*z);
  }
    result = narrow_abs(x, &absx);
  if (debug) {
    printf("narrowing abs = %d\n",result);
    print_interval("  absx =",*x);
    print_interval("  x =",*x);
  }
  }
  if (debug) {
    printf("Leaving pow_even -> %d\n",result);
    print_interval("x =",*x);
    print_interval("y =",*y);
    print_interval("z =",*z);
  }
  return(result);
}  

