/*
   IAexp.c

   This is a library of sound implementations of the
   exponential function and natural logarithm.
   The library uses interval arithmetic.

   2000 Copyright Timothy J. Hickey
   2003 Copyright David K. Wittenberg
   licence: http://interval.sourceforge.net/licence.txt, zlib/png 

*/


/*            CHANGELOG

2003-03-11  changed __BYTE_ORDER to BYTE_ORDER for portability      dkw
2003-12-12  removed some includes, as they're in smath.h           dkw

*/

#include <stdio.h>
#include <stdlib.h>
/* #include <ieeefp.h> */
#include "smath.h"

#if BYTE_ORDER == BIG_ENDIAN
static NEWDOUBLE
    MAX_REAL      = {{0x7fef, 0xffff, 0xffff, 0xffff}},
    POS_INF       = {{0x7ff0, 0x0000, 0x0000, 0x0000}},
    //NEG_INF       = {{0xfff0, 0x0000, 0x0000, 0x0000}},
    LOGMAXREAL    = {{0x4086, 0x2e42, 0xfefa, 0x39ef}},
    LOGMINREAL    = {{0xc087, 0x4385, 0x446d, 0x71c4}},
    LN2_LO        = {{0x3fe6, 0x2e42, 0xfefa, 0x39ef}}, 
    //LN2_HI        = {{0x3fe6, 0x2e42, 0xfefa, 0x39f0}}, 
    LN2_A         = {{0x3fe6, 0x2e42, 0xfef0, 0x0000}};
    //LN2_B_LO      = {{0x3dd4, 0x73de, 0x6af2, 0x78ec}},
    //LN2_B_HI      = {{0x3dd4, 0x73de, 0x6af2, 0x78ed}};

static NEWDOUBLE3 log_val[] = {
{{{0x0000, 0x0000, 0x0000, 0x0000}},
 {{0x0000, 0x0000, 0x0000, 0x0000}},
 {{0x0000, 0x0000, 0x0000, 0x0000}}},
{{{0x3fbe, 0x2707, 0x6e28, 0x0000}},   /* l(1 + 1/8) */
 {{0x3d87, 0x972f, 0x4f54, 0x3fff}},
 {{0x3d87, 0x972f, 0x4f54, 0x4000}}},
{{{0x3fcc, 0x8ff7, 0xc798, 0x0000}},   /* l(1+2/8)  */
 {{0x3d94, 0xd10d, 0x612e, 0xc0f7}},
 {{0x3d94, 0xd10d, 0x612e, 0xc0f8}}},
{{{0x3fd4, 0x618b, 0xc218, 0x0000}}, /* ln(1+3/8) */
 {{0x3db1, 0x7b09, 0xf42d, 0xecde}},
 {{0x3db1, 0x7b09, 0xf42d, 0xecdf}}},
{{{0x3fd9, 0xf323, 0xecb8, 0x0000}}, /* ln(1+4/8) */
 {{0x3dbe, 0x612f, 0xcada, 0x35d9}},
 {{0x3dbe, 0x612f, 0xcada, 0x35da}}},
{{{0x3fdf, 0x128f, 0x5fa8, 0x0000}}, /* ln(1+5/8) */
 {{0x3dbc, 0x1bb2, 0xcd72, 0x0ec4}},
 {{0x3dbc, 0x1bb2, 0xcd72, 0x0ec5}}},
{{{0x3fe1, 0xe85f, 0x5e70, 0x0000}}, /* ln(1+6/8) */
 {{0x3d80, 0x340f, 0x7b16, 0x697c}},
 {{0x3d80, 0x340f, 0x7b16, 0x697d}}},
{{{0x3fe4, 0x1d8f, 0xe840, 0x0000}}, /* ln(1+7/8) */
 {{0x3dc9, 0xcab9, 0x9192, 0xf30b}},
 {{0x3dc9, 0xcab9, 0x9192, 0xf30c}}},
{{{0x3fe6, 0x2e42, 0xfef8, 0x0000}}, /* ln(1+8/8) */
 {{0x3db1, 0xcf79, 0xabc9, 0xe3b3}},
 {{0x3db1, 0xcf79, 0xabc9, 0xe3b4}}}
};


//static EXT_PREC log_tab[] = {
//{{{0x0000, 0x0000, 0x0000, 0x0000}},
 //{{0x0000, 0x0000, 0x0000, 0x0000,
   //0x0000, 0x0000, 0x0000, 0x0000}}},
//{{{0x3fbe, 0x2707, 0x6e28, 0x0000}},   /* l(1 + 1/8) */
 //{{0x3d87, 0x972f, 0x4f54, 0x3fff,
   //0x3d87, 0x972f, 0x4f54, 0x4000}}},
//{{{0x3fcc, 0x8ff7, 0xc798, 0x0000}},   /* l(1+2/8)  */
 //{{0x3d94, 0xd10d, 0x612e, 0xc0f7,
   //0x3d94, 0xd10d, 0x612e, 0xc0f8}}},
//{{{0x3fd4, 0x618b, 0xc218, 0x0000}}, /* ln(1+3/8) */
 //{{0x3db1, 0x7b09, 0xf42d, 0xecde,
   //0x3db1, 0x7b09, 0xf42d, 0xecdf}}},
//{{{0x3fd9, 0xf323, 0xecb8, 0x0000}}, /* ln(1+4/8) */
 //{{0x3dbe, 0x612f, 0xcada, 0x35d9,
   //0x3dbe, 0x612f, 0xcada, 0x35da}}},
//{{{0x3fdf, 0x128f, 0x5fa8, 0x0000}}, /* ln(1+5/8) */
 //{{0x3dbc, 0x1bb2, 0xcd72, 0x0ec4,
   //0x3dbc, 0x1bb2, 0xcd72, 0x0ec5}}},
//{{{0x3fe1, 0xe85f, 0x5e70, 0x0000}}, /* ln(1+6/8) */
 //{{0x3d80, 0x340f, 0x7b16, 0x697c,
   //0x3d80, 0x340f, 0x7b16, 0x697d}}},
//{{{0x3fe4, 0x1d8f, 0xe840, 0x0000}}, /* ln(1+7/8) */
 //{{0x3dc9, 0xcab9, 0x9192, 0xf30b,
   //0x3dc9, 0xcab9, 0x9192, 0xf30c}}},
//{{{0x3fe6, 0x2e42, 0xfef8, 0x0000}}, /* ln(1+8/8) */
 //{{0x3db1, 0xcf79, 0xabc9, 0xe3b3,
   //0x3db1, 0xcf79, 0xabc9, 0xe3b4}}}
//};






static NEWINTERVAL
  FAIL_INT =
     {{0x7ff0, 0x0000, 0x0000, 0x0000,
       0xfff0, 0x0000, 0x0000, 0x0000}},
  NEAR_POS_INF  =
     {{0x7fef, 0xffff, 0xffff, 0xffff,
       0x7ff0, 0x0000, 0x0000, 0x0000}},
  NEAR_NEG_INF  =
     {{0xfff0, 0x0000, 0x0000, 0x0000,
       0xffef, 0xffff, 0xffff, 0xffff}},
  NEAR_POS_ZERO =
     {{0x0000, 0x0000, 0x0000, 0x0000,
       0x0000, 0x0000, 0x0000, 0x0001}},
  LN2B          =
     {{0x3dd4, 0x73de, 0x6af2, 0x78ec,
       0x3dd4, 0x73de, 0x6af2, 0x78ed}};


#endif


#if BYTE_ORDER == LITTLE_ENDIAN
static NEWDOUBLE
    MAX_REAL      = {{0xffff,0xffff, 0xffff, 0x7fef }},
    POS_INF       = {{0x0000,0x0000, 0x0000, 0x7ff0 }},
    //NEG_INF       = {{0x0000,0x0000, 0x0000, 0xfff0 }},
    LOGMAXREAL    = {{0x39ef,0xfefa, 0x2e42, 0x4086 }},
    LOGMINREAL    = {{0x71c4,0x446d, 0x4385, 0xc087 }},
    LN2_LO        = {{0x39ef,0xfefa, 0x2e42, 0x3fe6 }}, 
    //LN2_HI        = {{0x39f0,0xfefa, 0x2e42, 0x3fe6 }}, 
    LN2_A         = {{0x0000,0xfef0, 0x2e42, 0x3fe6 }};
    //LN2_B_LO      = {{0x78ec,0x6af2, 0x73de, 0x3dd4 }},
    //LN2_B_HI      = {{0x78ed,0x6af2, 0x73de, 0x3dd4 }};

//static NEWDOUBLE3 log_val[] = {
//{
 //{{0x0000,0x0000, 0x0000, 0x0000 }},
 //{{0x0000,0x0000, 0x0000, 0x0000 }},
 //{{0x0000,0x0000, 0x0000, 0x0000 }}},
//{
 //{{0x0000,0x6e28, 0x2707, 0x3fbe }},   /* l(1 + 1/8) */
 //{{0x3fff,0x4f54, 0x972f, 0x3d87 }},
 //{{0x4000,0x4f54, 0x972f, 0x3d87 }}},
//{
 //{{0x0000,0xc798, 0x8ff7, 0x3fcc }},   /* l(1+2/8)  */
 //{{0xc0f7,0x612e, 0xd10d, 0x3d94 }},
 //{{0xc0f8,0x612e, 0xd10d, 0x3d94 }}},
//{
 //{{0x0000,0xc218, 0x618b, 0x3fd4 }}, /* ln(1+3/8) */
 //{{0xecde,0xf42d, 0x7b09, 0x3db1 }},
 //{{0xecdf,0xf42d, 0x7b09, 0x3db1 }}},
//{
 //{{0x0000,0xecb8, 0xf323, 0x3fd9 }}, /* ln(1+4/8) */
 //{{0x35d9,0xcada, 0x612f, 0x3dbe }},
 //{{0x35da,0xcada, 0x612f, 0x3dbe }}},
//{
 //{{0x0000,0x5fa8, 0x128f, 0x3fdf }}, /* ln(1+5/8) */
 //{{0x0ec4,0xcd72, 0x1bb2, 0x3dbc }},
 //{{0x0ec5,0xcd72, 0x1bb2, 0x3dbc }}},
//{
 //{{0x0000,0x5e70, 0xe85f, 0x3fe1 }}, /* ln(1+6/8) */
 //{{0x697c,0x7b16, 0x340f, 0x3d80 }},
 //{{0x697d,0x7b16, 0x340f, 0x3d80 }}},
//{
 //{{0x0000,0xe840, 0x1d8f, 0x3fe4 }}, /* ln(1+7/8) */
 //{{0xf30b,0x9192, 0xcab9, 0x3dc9 }},
 //{{0xf30c,0x9192, 0xcab9, 0x3dc9 }}},
//{
 //{{0x0000,0xfef8, 0x2e42, 0x3fe6 }}, /* ln(1+8/8) */
 //{{0xe3b3,0xabc9, 0xcf79, 0x3db1 }},
 //{{0xe3b4,0xabc9, 0xcf79, 0x3db1 }}}
//};


static EXT_PREC log_tab[] = {
{
 {{0x0000,0x0000, 0x0000, 0x0000 }},
 {{0x0000,0x0000, 0x0000, 0x0000 ,
   0x0000,0x0000, 0x0000, 0x0000 }}},
{
 {{0x0000,0x6e28, 0x2707, 0x3fbe }},   /* l(1 + 1/8) */
 {{0x3fff,0x4f54, 0x972f, 0x3d87 ,
   0x4000,0x4f54, 0x972f, 0x3d87 }}},
{
 {{0x0000,0xc798, 0x8ff7, 0x3fcc }},   /* l(1+2/8)  */
 {{0xc0f7,0x612e, 0xd10d, 0x3d94 ,
   0xc0f8,0x612e, 0xd10d, 0x3d94 }}},
{
 {{0x0000,0xc218, 0x618b, 0x3fd4 }}, /* ln(1+3/8) */
 {{0xecde,0xf42d, 0x7b09, 0x3db1 ,
   0xecdf,0xf42d, 0x7b09, 0x3db1 }}},
{
 {{0x0000,0xecb8, 0xf323, 0x3fd9 }}, /* ln(1+4/8) */
 {{0x35d9,0xcada, 0x612f, 0x3dbe ,
   0x35da,0xcada, 0x612f, 0x3dbe }}},
{
 {{0x0000,0x5fa8, 0x128f, 0x3fdf }}, /* ln(1+5/8) */
 {{0x0ec4,0xcd72, 0x1bb2, 0x3dbc ,
   0x0ec5,0xcd72, 0x1bb2, 0x3dbc }}},
{
 {{0x0000,0x5e70, 0xe85f, 0x3fe1 }}, /* ln(1+6/8) */
 {{0x697c,0x7b16, 0x340f, 0x3d80 ,
   0x697d,0x7b16, 0x340f, 0x3d80 }}},
{
 {{0x0000,0xe840, 0x1d8f, 0x3fe4 }}, /* ln(1+7/8) */
 {{0xf30b,0x9192, 0xcab9, 0x3dc9 ,
   0xf30c,0x9192, 0xcab9, 0x3dc9 }}},
{
 {{0x0000,0xfef8, 0x2e42, 0x3fe6 }}, /* ln(1+8/8) */
 {{0xe3b3,0xabc9, 0xcf79, 0x3db1 ,
   0xe3b4,0xabc9, 0xcf79, 0x3db1 }}}
};






static NEWINTERVAL
  FAIL_INT =
     {{0x0000,0x0000, 0x0000, 0x7ff0 ,
       0x0000,0x0000, 0x0000, 0xfff0 }},
  NEAR_POS_INF  =
     {{0xffff,0xffff, 0xffff, 0x7fef ,
       0x0000,0x0000, 0x0000, 0x7ff0 }},
  NEAR_NEG_INF  =
     {{0x0000,0x0000, 0x0000, 0xfff0 ,
       0xffff,0xffff, 0xffff, 0xffef }},
  NEAR_POS_ZERO =
     {{0x0000,0x0000, 0x0000, 0x0000 ,
       0x0001,0x0000, 0x0000, 0x0000 }},
  LN2B          =
     {{0x78ec,0x6af2, 0x73de, 0x3dd4 ,
       0x78ed,0x6af2, 0x73de, 0x3dd4 }};


#endif


INTERVAL expDI(double x) {
  double n;   INTERVAL A,R;   int i,deg=15;

    /* RANGE REDUCTION to [ln(minreal),ln(maxreal)] */
  if      (x > LOGMAXREAL.d)  return(NEAR_POS_INF.i);
  else if (x < LOGMINREAL.d)  return(NEAR_POS_ZERO.i);
    /* RANGE REDUCTION to A in [-ln(2)/2, ln(2)/2] */
    /* x \in A + n*ln(2),  A = (x-n*ln2a) - n*ln2B */
  n = floor(x/LN2_LO.d+0.5);          
  A = subIII(subDII(x,mulDDI(n,LN2_A.d)),mulDII(n,LN2B.i));

    /* TAYLOR SERIES EVALUATION of exp(A) */
  R = makeDDI(0.5,2); /* R = [1,2] */
  for(i=deg;i>=3;i--) 
    R = addDII(1,divIDI(mulIII(A,R),i));  /* R = 1+A*R/i */
    /* R = 1 + (A + A*(A*R)/2) */
  R = addDII(1.0,addIII(A, divIDI(mulIII(A,mulIII(A,R)),2.0)));

    /* APPLY ADDITION LAW : exp(x) = exp(a+n*ln(2)) = R*2^n */
  R = mulIII(R,two_to_nDI(n)); 
  return(R);
}



INTERVAL expII(INTERVAL X) {
  INTERVAL Y,T;
  T = expDI(X.lo);
  Y.lo = T.lo;
  T = expDI(X.hi);
  Y.hi = T.hi;
  return(Y);
}






INTERVAL two_to_nDI(int n)
{
  double t;
  unsigned int *u, *v;
  int m;

   if (n>1023)
     return(makeDDI(MAX_REAL.d,POS_INF.d));
   else if (n > -1023) {
     t=0;
#if BYTE_ORDER == BIG_ENDIAN
     u=(unsigned int *) &t;
     v=u+1;
#endif     
#if BYTE_ORDER == LITTLE_ENDIAN
     v=(unsigned int *) &t;
     u=v+1;
#endif
     *u = ((n+1023)<<20);
     return(cnstDI(t));
   }
   else {
    t=0;
#if BYTE_ORDER == BIG_ENDIAN
     u=(unsigned int *) &t;
     v=u+1;
#endif     
#if BYTE_ORDER == LITTLE_ENDIAN
     v=(unsigned int *) &t;
     u=v+1;
#endif
    m = -1022-n;
    if (m <= 20) {
       *u=(1<<(20-m));
       return(cnstDI(t));
     }
    else if (m <= 52) {
        *v=(1<<(52-m));
        return(cnstDI(t));
      }
    else {
      *v=1;
      return(makeDDI(0,t));
    }
  }
}

//static double get_exp_part(double x, int *n, double *a) {
  //double t;
  //unsigned int *u,*v;

  //t = x;

//#if BYTE_ORDER == BIG_ENDIAN
     //u=(unsigned int *) &t;
     //v=u+1;
//#endif     
//#if BYTE_ORDER == LITTLE_ENDIAN
     //v=(unsigned int *) &t;
     //u=v+1;
//#endif

  //*u = ((*u) & 0x7ff00000);
  //*v = 0;

  //*n = (((*u)>>20) - 1023);
  //*a = x/t;
  //return(x);
//}


    /*
    ****************************************************************
    This should bind a and n to numbers such that
    x = a*2^n, where 1<=a<2
    ****************************************************************
    */
static void get_bits_exp(double x, double *a, int *n) {
  double t,t2;
  unsigned int *u,*v;

  t2 = t = x;

#if BYTE_ORDER == BIG_ENDIAN
     u=(unsigned int *) &t2;
     v=u+1;
#endif     
#if BYTE_ORDER == LITTLE_ENDIAN
     v=(unsigned int *) &t2;
     u=v+1;
#endif

  *u = ((*u) & 0x7ff00000);
  if (*u == 0) { /* denormalized case, shift up 60 bits */ 
    t2 = t2*1024*1024*1024*1024*1024*1024;
    *u = ((*u) & 0x7ff00000);
    *v = 0;
    *n = (((*u)>>20) - 1023)-60;
    t2 = t2/(1024.0*1024*1024*1024*1024*1024);
    *a = t/t2;
  }
  else {
    *v = 0;
    *n = (((*u)>>20) - 1023);
    *a = t/t2;
  }
}


INTERVAL logDI(double x) {
  double a,a1;
  int n,j,i,deg=40;
  INTERVAL A1,R;
    /* Range reduction to [1, 1+1/8] */
    /* x = a*2^n,  1<=a < 2 */
    /* a in (1+i/8)*(1 + A1) */

  if (x==POS_INF.d) return(NEAR_POS_INF.i);
  if (x==0.0) return(NEAR_NEG_INF.i);

  get_bits_exp(x,&a,&n);
  i = floor(a*8)-8;
  if (i <= 1) i = 0;
  a1 = i/8.0;
  A1 = divIDI(subDDI(a,(1+a1)),(1+a1));

    /* Taylor evaluation of log(1+A1) */
  R = makeDDI(0,1);
  for (j=deg;j>=2;j--) 
    R = subIII(divDDI(1.0,j),mulIII(A1,R));
  R = subIII(A1,mulIII(A1,mulIII(A1,R)));

    /* Use addition law: 
       ln((1+a1)*(1+A1)*2^n) = ln(1+a1)+ln(1+A1)+n*ln(2) */
  R = addIII(mulDDI(n,LN2_A.d),
      addDII(log_tab[i].a.d,
      addIII(R,
      addIII(mulDII(n,LN2B.i),
             log_tab[i].b.i))));

  return(R);  
}




INTERVAL logII(INTERVAL X) {
  INTERVAL T,Y;
  if (X.hi < 0) return(FAIL_INT.i);
  if (X.lo < 0) X.lo = 0;
  T = logDI(X.lo);
  Y.lo = T.lo;
  T = logDI(X.hi);
  Y.hi = T.hi;
  return(Y);
}  


/*
void narrow_expII(INTERVAL *X,INTERVAL *Y) {
  *Y = intersectIII(*Y,expII(*X));
  *X = intersectIII(*X,logII(*Y));
}
*/




