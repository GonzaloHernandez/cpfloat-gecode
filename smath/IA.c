/*
  IA.c

   This is a simple interval arithmetic library
   which implements the standard arithmetic 
   operators: +,-,*,/ as interval-valued functions
   defined on intervals and doubles.

   2000 Copyright Timothy J. Hickey
   2003 Copyright David K. Wittenberg
   licence: http://interval.sourceforge.net/licence.txt, zlib/png 

*/

/*       Change Log

2003-03-11  changed __BYTE_ORDER to BYTE_ORDER for portability      dkw
2003-03-14  changed NAN to CLIP_NAN to avoid conflict with OS X names dkw
2003-12-12  removed include endian, math, those are in smath       dkw
*/

#include <stdio.h>
#include <stdlib.h>
/* #include <ieeefp.h> */
#include "smath.h"


#define IS_NEG_ZERO(x) ((x==0) && (( *((unsigned short *) &x) & 0x8000) != 0))


//#if BYTE_ORDER == BIG_ENDIAN
//static NEWDOUBLE
  //POS_INF  ={{0x7ff0, 0x0000,0x0000,0x0000}}, 
  //NEG_INF  ={{0xfff0, 0x0000,0x0000,0x0000}}, 
  //NEG_ZERO ={{0x8000, 0x0000,0x0000,0x0000}}, 
  //POS_ZERO ={{0x0000, 0x0000,0x0000,0x0000}}, 
  //CLIP_NAN ={{0x7ff1, 0x0000,0x0000,0x0000}};
//#endif
//#if BYTE_ORDER == LITTLE_ENDIAN
//static NEWDOUBLE
  //POS_INF  ={{0x0000, 0x0000,0x0000,0x7fff}}, 
  //NEG_INF  ={{0x0000, 0x0000,0x0000,0xfff0}}, 
  //NEG_ZERO ={{0x0000, 0x0000,0x0000,0x8000}},
  //POS_ZERO ={{0x0000, 0x0000,0x0000,0x0000}}, 
  //CLIP_NAN ={{0x0000, 0x0000,0x0000,0x7ff1}};
//#endif

int interval_mul4(double x1, double x2, double y1, double y2, double* low, double* high);
int interval_div4(double x1, double x2, double y1, double y2, double* low, double* high);

void new_print_double(char *s,double f) {
unsigned int * u;
  u = (unsigned int *) &f;
  printf("%20s %8x %8x %.20g\n",s,*u,*(u+1),f);
}

double fnext(double x) {
 UDOUBLE r;
    r.d= x;
#if BYTE_ORDER ==  BIG_ENDIAN
    r.u[0] = (r.u[0] & 0x7ff00000) - 0x03400000;
    r.u[1] = 0;
#endif
#if BYTE_ORDER == LITTLE_ENDIAN
    r.u[1] = (r.u[1] & 0x7ff00000) - 0x03400000;
    r.u[0] = 0;
#endif
  new_print_double("x = ",x);
  new_print_double("r = ",r.d);
  new_print_double("x+r.d = ",x+r.d);
  return(x+r.d);
}

void print_interval(char *s, INTERVAL p) {
  printf("%s\n",s);
  new_print_double("lo = ",p.lo);
  new_print_double("hi = ",p.hi);
  printf("\n");
}



/*
  Below are the interval arithmetic functions
  The suffixes tell the argument and return type
    I = interval
    D = double
  with the return type being at the end of the list:
  Thus, makeDDI accepts two doubles and returns an interval. 
  and addIII accepts two intervals and returns an interval.

*/

INTERVAL cnstDI(double a) {
  INTERVAL c;
  c.hi = c.lo = a;
  return(c);
}


INTERVAL makeDDI(double lo,double hi) {
  INTERVAL c;
  c.lo = lo;
  c.hi = hi;
  return(c);
}


INTERVAL addIII(INTERVAL a, INTERVAL b) {
  INTERVAL c;  
  c.lo = add_lo(a.lo,b.lo);
  c.hi = add_hi(a.hi,b.hi);
  return(c);
}

INTERVAL subIII(INTERVAL a, INTERVAL b) {
  INTERVAL c;  
  c.lo = sub_lo(a.lo,b.hi);
  c.hi = sub_hi(a.hi,b.lo);
  return(c);
}

INTERVAL mulIII(INTERVAL a, INTERVAL b) {
  INTERVAL c;  
  interval_mul4(a.lo,a.hi,b.lo,b.hi,&(c.lo),&(c.hi));
  return(c);
}

INTERVAL divIII(INTERVAL a, INTERVAL b) {
  INTERVAL c;  
  interval_div4(a.lo,a.hi,b.lo,b.hi,&c.lo,&c.hi);
  return(c);
}

INTERVAL negII(INTERVAL a) {
  INTERVAL c;
  c.lo = -a.hi;
  c.hi = -a.lo;
  return(c);
}

INTERVAL intersectIII(INTERVAL a, INTERVAL b) {
  INTERVAL c;
  c.lo = (a.lo > b.lo)? a.lo : b.lo;
  c.hi = (a.hi < b.hi)? a.hi : b.hi;
  return(c);
}


/*
  Mixed double/interval operators returning intervals
*/

INTERVAL addIDI(INTERVAL a, double b) {
  INTERVAL c;  
  c.lo = add_lo(a.lo, b);
  c.hi = add_hi(a.hi, b);
  return(c);
}

INTERVAL addDII(double a, INTERVAL b) {
  INTERVAL c;  
  c.lo = add_lo(a, b.lo);
  c.hi = add_hi(a, b.hi);
  return(c);
}

INTERVAL addDDI(double a, double b) {
  INTERVAL c;  
  c.lo = add_lo(a, b);
  c.hi = add_hi(a, b);
  return(c);
}


INTERVAL subIDI(INTERVAL a, double b) {
  INTERVAL c;  
  c.lo = sub_lo(a.lo, b);
  c.hi = sub_hi(a.hi, b);
  return(c);
}

INTERVAL subDII(double a, INTERVAL b) {
  INTERVAL c;  
  c.lo = sub_lo(a, b.hi);
  c.hi = sub_hi(a, b.lo);
  return(c);
}

INTERVAL subDDI(double a, double b) {
  INTERVAL c;  
  c.lo = sub_lo(a, b);
  c.hi = sub_hi(a, b);
  return(c);
}


INTERVAL mulIDI(INTERVAL a, double b) {
  INTERVAL c;  
  interval_mul4(a.lo,a.hi,b,b,&(c.lo),&(c.hi));
  return(c);
  /*

  double tmp,min,max;

  if ((a.lo==0) && (a.hi==0)) {
    c.lo = c.hi = 0; return(c);}

  if (b == 0) {
    c.lo = c.hi = 0; return(c);}

  min = mul_lo(a.lo,b);
  if ((tmp = mul_lo(a.hi,b)) < min) min = tmp;
  c.lo = min;

  max = mul_hi(a.lo,b);
  if ((tmp = mul_hi(a.hi,b)) > max) max = tmp;
  c.hi = max;

  return(c);
  */
}

INTERVAL mulDII(double a, INTERVAL b) {
  INTERVAL c;  
  interval_mul4(a,a,b.lo,b.hi,&(c.lo),&(c.hi));
  return(c);
  /*

  double tmp,min,max;

  if (a==0) {
    c.lo = c.hi = 0; return(c);}

  if ((b.lo==0) && (b.hi==0)) {
    c.lo = c.hi = 0; return(c);}

  min = mul_lo(a,b.lo);
  if ((tmp = mul_lo(a,b.hi)) < min) min = tmp;
  c.lo = min;

  max = mul_hi(a,b.lo);
  if ((tmp = mul_hi(a,b.hi)) > max) max = tmp;
  c.hi = max;

  return(c);
  */
}

INTERVAL mulDDI(double a, double b) {
  INTERVAL c;  

  if ((a==0) || (b == 0)) {
    c.lo = c.hi = 0; return(c);}

  c.lo = mul_lo(a,b);
  c.hi = mul_hi(a,b);
  return(c);

}




INTERVAL divIDI(INTERVAL a, double b) {
  INTERVAL c;  
  interval_div4(a.lo,a.hi,b,b,&(c.lo),&(c.hi));
  return(c);
  /*
  double tmp,min,max;

  if (b==0) {
    c.lo = NEG_INF.d; c.hi = POS_INF.d; return(c);}

  if ((a.lo==0) && (a.hi==0)) {
    c.lo = c.hi = 0; return(c);}

  min = div_lo(a.lo,b);
  if ((tmp = div_lo(a.hi,b)) < min) min = tmp;
  c.lo = min;

  max = div_hi(a.lo,b);
  if ((tmp = div_hi(a.hi,b)) > max) max = tmp;
  c.hi = max;

  return(c);
  */
}

INTERVAL divDII(double a, INTERVAL b) {
  INTERVAL c;  
  interval_div4(a,a,b.lo,b.hi,&(c.lo),&(c.hi));
  return(c);
  /*
  double tmp,min,max;

  if ((b.lo==0) && (b.hi==0)) {
    c.lo = NEG_INF.d; c.hi = POS_INF.d; return(c);}

  if (a==0) {
    c.lo = c.hi = 0; return(c);}

  if ((b.lo<0) && (b.hi>0)) {
    c.lo = NEG_INF.d; c.hi = POS_INF.d; return(c);}

  if (b.hi == 0) b.hi = NEG_ZERO.d;
  if (b.lo == 0) b.lo = POS_ZERO.d;

  min = div_lo(a,b.lo);
  if ((tmp = div_lo(a,b.hi)) < min) min = tmp;
  c.lo = min;

  max = div_hi(a,b.lo);
  if ((tmp = div_hi(a,b.hi)) > max) max = tmp;
  c.hi = max;

  return(c);
  */
}


INTERVAL divDDI(double a, double b) {
  INTERVAL c;  
  interval_div4(a,a,b,b,&(c.lo),&(c.hi));
  return(c);
  /*

  if (b==0) {
    c.lo = NEG_INF.d; c.hi = POS_INF.d; return(c);}

  if (a==0) {
    c.lo = c.hi = 0; return(c);}

  c.lo = div_lo(a,b);
  c.hi = div_hi(a,b);
  return(c);
  */
}

INTERVAL sqrtII(INTERVAL A) {
  /* note: IEEE 754 sqrt is subject to the
     rounding modes!
  */
  INTERVAL B;
  //double x;

  B.lo = sqrt_lo(A.lo);
  B.hi = sqrt_hi(A.hi);

  return(B);
}






void printSI(char *s, INTERVAL a) {
  printf("%s [%.20g,%.20g]\n",s,a.lo,a.hi);
} 



/* 
  Next we have the procedural forms of the
  interval arithmetic operators
*/

/*
static double max4(double a, double b, double c, double d) {
  double m;
  m=a;
  if (m<b) m=b;
  if (m<c) m=c;
  if (m<d) m=d;
  return(m);
}


static double min4(double a, double b, double c, double d) {
  double m;
  m=a;
  if (m>b) m=b;
  if (m>c) m=c;
  if (m>d) m=d;
  return(m);
}

static double max2(double a, double b) {
  if (a>b) return(a); else return(b);
}

static double min2(double a, double b) {
  if (a<b) return(a); else return(b);
}
*/


INTERVAL unionIII(INTERVAL A, INTERVAL B) {
  INTERVAL C;

  if (A.lo < B.lo) C.lo = A.lo; else C.lo = B.lo;
  if (A.hi > B.hi) C.hi = A.hi; else C.hi = B.hi;

  return(C);
}

INTERVAL unionDDI(double a, double b) {
  INTERVAL C;

  if (a<b) 
     {C.lo = a;C.hi = b;}
   else 
     {C.lo = b;C.hi = a;}

  return(C);
}





