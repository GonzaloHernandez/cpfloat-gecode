/*
   IAasincos.c

   This is a library of sound and precise implementations of the
   inverse trigonometric functions: asin, acos, atan

   2000 Copyright Timothy J. Hickey
   2003 Copyright David K. Wittenberg
   licence: http://interval.sourceforge.net/licence.txt, zlib/png 

*/


/*            CHANGELOG

2003-03-11  changed __BYTE_ORDER to BYTE_ORDER for portability      dkw
2003-12-12  removed include math.h, as smath.h has it               dkw

*/


#include <stdio.h>
#include <stdlib.h>
/* #include <ieeefp.h> */
#include "smath.h"

static int debug = 0;

#if BYTE_ORDER == BIG_ENDIAN
static NEWDOUBLE
  //POS_INF          =  {{0x7ff0, 0x0000, 0x0000, 0x0000}},
  //NEG_INF          =  {{0xfff0, 0x0000, 0x0000, 0x0000}},
  //PI_LO            =  {{0x4009, 0x21fb, 0x5444, 0x2d18}},
  //PI_HI            =  {{0x4009, 0x21fb, 0x5444, 0x2d19}},
  PI_A             =  {{0x4009, 0x21fb, 0x5440, 0x0000}},
  //PI_OVER_4_A      =  {{0x3fe9, 0x21fb, 0x5440, 0x0000}},
  //PI_OVER_2_LO     =  {{0x3ff9, 0x21fb, 0x5444, 0x2d18}},
  PI_OVER_2_A      =  {{0x3ff9, 0x21fb, 0x5440, 0x0000}},
  //PI_OVER_2_B      =  {{0x3dd0, 0xB461, 0x1A60, 0x0000}}, 
  PI_OVER_2_B_LO   =  {{0x3dd0, 0xB461, 0x1A62, 0x6331}}, 
  PI_OVER_2_B_HI   =  {{0x3dd0, 0xB461, 0x1A62, 0x6332}};
  //PI_OVER_2_C_LO   =  {{0x3ba3, 0x198A, 0x2E03, 0x7073}}, 
  //PI_OVER_2_C_HI   =  {{0x3ba3, 0x198A, 0x2E03, 0x7074}};


static NEWINTERVAL
  PNINF          =
    {{0xfff0, 0x0000, 0x0000, 0x0000,
      0x7ff0, 0x0000, 0x0000, 0x0000}},
  PIOVER4B       =
    {{0x3dc0, 0xB461, 0x1A62, 0x6331,
      0x3dc0, 0xB461, 0x1A62, 0x6332}}, 
  PIOVER2B       =
    {{0x3dd0, 0xB461, 0x1A62, 0x6331,
      0x3dd0, 0xB461, 0x1A62, 0x6332}}, 
  PIOVER2        =
    {{0x3ff9, 0x21fb, 0x5444, 0x2d18,
      0x3ff9, 0x21fb, 0x5444, 0x2d19}},
  TWOPI        =
    {{0x4019, 0x21fb, 0x5444, 0x2d18,
      0x4019, 0x21fb, 0x5444, 0x2d19}};

#endif


#if BYTE_ORDER == LITTLE_ENDIAN
static NEWDOUBLE
  //POS_INF          =  {{0x0000,0x0000, 0x0000, 0x7ff0 }},
  //NEG_INF          =  {{0x0000,0x0000, 0x0000, 0xfff0 }},
  //PI_LO            =  {{0x2d18,0x5444, 0x21fb, 0x4009 }},
  //PI_HI            =  {{0x2d19,0x5444, 0x21fb, 0x4009 }},
  PI_A             =  {{0x0000,0x5440, 0x21fb, 0x4009 }},
  //PI_OVER_4_A      =  {{0x0000,0x5440, 0x21fb, 0x3fe9 }},
  //PI_OVER_2_LO     =  {{0x2d18,0x5444, 0x21fb, 0x3ff9 }},
  PI_OVER_2_A      =  {{0x0000,0x5440, 0x21fb, 0x3ff9 }},
  //PI_OVER_2_B      =  {{0x0000,0x1A60, 0xB461, 0x3dd0 }}, 
  PI_OVER_2_B_LO   =  {{0x6331,0x1A62, 0xB461, 0x3dd0 }}, 
  PI_OVER_2_B_HI   =  {{0x6332,0x1A62, 0xB461, 0x3dd0 }}; 
  //PI_OVER_2_C_LO   =  {{0x7073,0x2E03, 0x198A, 0x3ba3 }}, 
  //PI_OVER_2_C_HI   =  {{0x7074,0x2E03, 0x198A, 0x3ba3 }};


static NEWINTERVAL
  PNINF          =
    {{0x0000,0x0000, 0x0000, 0xfff0 ,
      0x0000,0x0000, 0x0000, 0x7ff0 }},
  PIOVER4B       =
    {{0x6331,0x1A62, 0xB461, 0x3dc0 ,
      0x6332,0x1A62, 0xB461, 0x3dc0 }}, 
  PIOVER2B       =
    {{0x6331,0x1A62, 0xB461, 0x3dd0 ,
      0x6332,0x1A62, 0xB461, 0x3dd0 }}, 
  PIOVER2        =
    {{0x2d18,0x5444, 0x21fb, 0x3ff9 ,
      0x2d19,0x5444, 0x21fb, 0x3ff9 }},
  TWOPI        =
    {{0x2d18,0x5444, 0x21fb, 0x4019 ,
      0x2d19,0x5444, 0x21fb, 0x4019 }};

#endif


void print_double(char *s,double f);

/* ****************************************************************
   ****************************************************************
         inverse trig functions

   Here we compute all inverse trig functions by
   reducing to arcsin(x), and we compute the latter
   using the addition law for arcsin and a moderate
   size table of extended precision values.

   ****************************************************************
   **************************************************************** */


/* this computes an extended precision square root
     sqrt(V) in B + U 
*/
void mp_sqrtIDIV(INTERVAL V,double *B, INTERVAL *U) {
  INTERVAL T;
  double b;

  T = sqrtII(V);
  *B = b = (T.hi + T.lo)/2;
  *U = subIDI(T,b);
  if (debug) {
    print_interval("mp_sqrt::b= ",cnstDI(b));
    print_interval("mp_sqrt::U0= ",*U);
  }
  *U = divIII(subIII(subIII(V,mulDDI(b,b)),
                     mulIII(*U,*U)),
              mulDDI(2,b));
  if (debug)  print_interval("mp_sqrt::U1= ",*U);
}






/* this computes the higher order terms when A is
   near zero

   asin_eps_hot(A)  contains  {asin(x)-x | x in A}

*/

INTERVAL asin_eps_hot(INTERVAL A) {
  INTERVAL A2;

#if BYTE_ORDER == BIG_ENDIAN
static NEWDOUBLE 
  R0 = {{0x3c5d, 0x0000, 0x0000, 0x0000}},
  P0 = {{0xbc44, 0xa0b4, 0x9764, 0x226a}},
  P1 = {{0x3fc5, 0x5555, 0x5555, 0x56a8}},
  P2 = {{0xbfbf, 0x762b, 0x4e58, 0x57e9}}, 
  P3 = {{0x3f83, 0x17e4, 0xc2ed, 0x623c}},
  Q0 = {{0x3ff0, 0x0000, 0x0000, 0x0000}},
  Q1 = {{0xbff2, 0xff83, 0x7091, 0xb302}},
  Q2 = {{0x3fd4, 0xa23b, 0x8f06, 0x495a}},
  Q3 = {{0xbf83, 0x17e4, 0xc2ed, 0x623c}};
#endif

#if BYTE_ORDER == LITTLE_ENDIAN
static NEWDOUBLE 
  R0 = {{0x0000,0x0000, 0x0000, 0x3c5d }},
  P0 = {{0x226a,0x9764, 0xa0b4, 0xbc44 }},
  P1 = {{0x56a8,0x5555, 0x5555, 0x3fc5 }},
  P2 = {{0x57e9,0x4e58, 0x762b, 0xbfbf }}, 
  P3 = {{0x623c,0xc2ed, 0x17e4, 0x3f83 }},
  Q0 = {{0x0000,0x0000, 0x0000, 0x3ff0 }},
  Q1 = {{0xb302,0x7091, 0xff83, 0xbff2 }},
  Q2 = {{0x495a,0x8f06, 0xa23b, 0x3fd4 }},
  Q3 = {{0x623c,0xc2ed, 0x17e4, 0xbf83 }};
#endif

  A2 = mulIII(A,A);

  return(
     divIII(
       mulIII(A,
       addIII(addDII(P0.d,makeDDI(-R0.d,R0.d)),
                   mulIII(A2,
       addDII(P1.d,mulIII(A2,
       addDII(P2.d,mulIDI(A2,P3.d))))))),

       addDII(Q0.d,mulIII(A2,
       addDII(Q1.d,mulIII(A2,
       addDII(Q2.d,mulIDI(A2,Q3.d))))))));

}



/* asin_tab[i] = asin(i/asin_tab_size) - i/asin_tab_size */
int asin_tab_size = 32;

/* asin_tab[i] = asin(i/asin_tab_size) - i/asin_tab_size */
/*
   These two arrays store the values 
   of arcsin(i/32) i=0,1,2,3,4,31 via:
      arcsin(i/32) - i/32
       in asin_tab_A[i] + asin_tab_B[i]
*/

#if BYTE_ORDER == BIG_ENDIAN
NEWDOUBLE asin_tab_A[] = {
  {{0x0000, 0x0000, 0x0000, 0x0000}},
  {{0x3ed5, 0x57bc, 0x1739, 0x0000}},
  {{0x3f05, 0x5ef4, 0xa9b0, 0x0000}},
  {{0x3f22, 0x1252, 0x27ca, 0x0000}},
  {{0x3f35, 0x7c18, 0x253c, 0x0000}},
  {{0x3f45, 0x10cb, 0x0114, 0x0000}},
  {{0x3f52, 0x4a76, 0x873c, 0x0000}},
  {{0x3f5d, 0x3789, 0xd574, 0x0000}},
  {{0x3f65, 0xf4e7, 0x05c3, 0x0000}},
  {{0x3f6f, 0x8291, 0xe2ee, 0x0000}},
  {{0x3f75, 0xce4e, 0xe438, 0x0000}},
  {{0x3f7d, 0x5104, 0xca66, 0x0000}},
  {{0x3f83, 0x3e9d, 0xb92c, 0x0000}},
  {{0x3f88, 0xc5fd, 0x8165, 0x0000}},
  {{0x3f8f, 0x5e4e, 0x8c1f, 0x0000}},
  {{0x3f93, 0x958a, 0xecdd, 0x0000}},
  {{0x3f98, 0x2a47, 0x05ae, 0x0000}},
  {{0x3f9d, 0x8461, 0x8e29, 0x0000}},
  {{0x3fa1, 0xdf40, 0xfbd3, 0x0000}},
  {{0x3fa5, 0x7bf3, 0x18fe, 0x0000}},
  {{0x3fa9, 0xaad7, 0x1ced, 0x0000}},
  {{0x3fae, 0x8253, 0x83cc, 0x0000}},
  {{0x3fb2, 0x0ef5, 0xa88a, 0x0000}},
  {{0x3fb5, 0x500d, 0xd5f7, 0x0000}},
  {{0x3fb9, 0x1a98, 0xae34, 0x0000}},
  {{0x3fbd, 0x8be4, 0x24e3, 0x0000}},
  {{0x3fc1, 0x6615, 0x59ed, 0x0000}},
  {{0x3fc4, 0x8aac, 0x8e94, 0x0000}},
  {{0x3fc8, 0x6033, 0x69f3, 0x0000}},
  {{0x3fcd, 0x31a3, 0x0c10, 0x0000}},
  {{0x3fd1, 0xc8b4, 0xbfac, 0x0000}},
  {{0x3fd6, 0x7d2f, 0x44fe, 0x0000}},
  {{0x0000, 0x0000, 0x0000, 0x0000}}
};

NEWINTERVAL asin_tab_B[] = {
{{0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000}},
{{0x3c2d, 0x6d94, 0x551b, 0xe3e9,
  0x3c2d, 0x6d94, 0x551b, 0xe3ea}},
{{0x3cb9, 0x460b, 0x8b71, 0xb77b,
  0x3cb9, 0x460b, 0x8b71, 0xb77c}},
{{0x3cc7, 0x0e7b, 0x58fc, 0xdf7d,
  0x3cc7, 0x0e7b, 0x58fc, 0xdf7e}},
{{0x3cce, 0x0be7, 0x5877, 0x24ea,
  0x3cce, 0x0be7, 0x5877, 0x24eb}},
{{0x3cd1, 0x6ea2, 0x98ae, 0x7f12,
  0x3cd1, 0x6ea2, 0x98ae, 0x7f13}},
{{0x3cff, 0x75de, 0x1ecb, 0x42c1,
  0x3cff, 0x75de, 0x1ecb, 0x42c2}},
{{0x3ce2, 0xa88c, 0x62ef, 0x0174,
  0x3ce2, 0xa88c, 0x62ef, 0x0175}},
{{0x3d1c, 0xd1e5, 0xd09d, 0xa2e0,
  0x3d1c, 0xd1e5, 0xd09d, 0xa2e1}},
{{0x3ce5, 0x853f, 0x099d, 0x28f5,
  0x3ce5, 0x853f, 0x099d, 0x28f6}},
{{0x3d25, 0x5363, 0xec9a, 0x653f,
  0x3d25, 0x5363, 0xec9a, 0x6540}},
{{0x3cfb, 0x9d02, 0x7ed2, 0xbb2e,
  0x3cfb, 0x9d02, 0x7ed2, 0xbb2f}},
{{0x3d26, 0x9827, 0xd232, 0xe036,
  0x3d26, 0x9827, 0xd232, 0xe037}},
{{0x3d24, 0x9397, 0x3f34, 0x0a5e,
  0x3d24, 0x9397, 0x3f34, 0x0a5f}},
{{0x3d3a, 0x7b3a, 0x0450, 0x6550,
  0x3d3a, 0x7b3a, 0x0450, 0x6551}},
{{0x3d4d, 0xe7c0, 0x7d94, 0xd9eb,
  0x3d4d, 0xe7c0, 0x7d94, 0xd9ec}},
{{0x3d3b, 0x2c23, 0x2dd9, 0x9707,
  0x3d3b, 0x2c23, 0x2dd9, 0x9708}},
{{0x3d34, 0xb283, 0xb8ef, 0x2236,
  0x3d34, 0xb283, 0xb8ef, 0x2237}},
{{0x3d2c, 0xd022, 0x1d79, 0xfa39,
  0x3d2c, 0xd022, 0x1d79, 0xfa3a}},
{{0x3d44, 0x5a9e, 0x769b, 0xd298,
  0x3d44, 0x5a9e, 0x769b, 0xd299}},
{{0x3cdd, 0xe4a4, 0xcea9, 0xa16b,
  0x3cdd, 0xe4a4, 0xcea9, 0xa16c}},
{{0x3d40, 0x2c8d, 0x4f63, 0x990f,
  0x3d40, 0x2c8d, 0x4f63, 0x9910}},
{{0x3d57, 0x675b, 0x171d, 0x8e8a,
  0x3d57, 0x675b, 0x171d, 0x8e8b}},
{{0x3d67, 0x5d88, 0x36eb, 0x3c9c,
  0x3d67, 0x5d88, 0x36eb, 0x3c9d}},
{{0x3d1b, 0x8102, 0xb07b, 0x66c1,
  0x3d1b, 0x8102, 0xb07b, 0x66c2}},
{{0x3d62, 0x87f2, 0x1f9e, 0xd8c0,
  0x3d62, 0x87f2, 0x1f9e, 0xd8c1}},
{{0x3d73, 0x2e86, 0xc13c, 0x2169,
  0x3d73, 0x2e86, 0xc13c, 0x216a}},
{{0x3d7d, 0x1268, 0x0779, 0x661f,
  0x3d7d, 0x1268, 0x0779, 0x6620}},
{{0x3d62, 0x636e, 0xd427, 0xbe74,
  0x3d62, 0x636e, 0xd427, 0xbe75}},
{{0x3d65, 0xaf35, 0xd7e2, 0xa7e7,
  0x3d65, 0xaf35, 0xd7e2, 0xa7e8}},
{{0x3d82, 0x63ec, 0xe664, 0x23fa,
  0x3d82, 0x63ec, 0xe664, 0x23fb}},
{{0x3d56, 0x430e, 0x7b0a, 0xba20,
  0x3d56, 0x430e, 0x7b0a, 0xba21}},
{{0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000}}
};
#endif



#if BYTE_ORDER == LITTLE_ENDIAN
NEWDOUBLE asin_tab_A[] = {
  {{0x0000,0x0000, 0x0000, 0x0000 }},
  {{0x0000,0x1739, 0x57bc, 0x3ed5 }},
  {{0x0000,0xa9b0, 0x5ef4, 0x3f05 }},
  {{0x0000,0x27ca, 0x1252, 0x3f22 }},
  {{0x0000,0x253c, 0x7c18, 0x3f35 }},
  {{0x0000,0x0114, 0x10cb, 0x3f45 }},
  {{0x0000,0x873c, 0x4a76, 0x3f52 }},
  {{0x0000,0xd574, 0x3789, 0x3f5d }},
  {{0x0000,0x05c3, 0xf4e7, 0x3f65 }},
  {{0x0000,0xe2ee, 0x8291, 0x3f6f }},
  {{0x0000,0xe438, 0xce4e, 0x3f75 }},
  {{0x0000,0xca66, 0x5104, 0x3f7d }},
  {{0x0000,0xb92c, 0x3e9d, 0x3f83 }},
  {{0x0000,0x8165, 0xc5fd, 0x3f88 }},
  {{0x0000,0x8c1f, 0x5e4e, 0x3f8f }},
  {{0x0000,0xecdd, 0x958a, 0x3f93 }},
  {{0x0000,0x05ae, 0x2a47, 0x3f98 }},
  {{0x0000,0x8e29, 0x8461, 0x3f9d }},
  {{0x0000,0xfbd3, 0xdf40, 0x3fa1 }},
  {{0x0000,0x18fe, 0x7bf3, 0x3fa5 }},
  {{0x0000,0x1ced, 0xaad7, 0x3fa9 }},
  {{0x0000,0x83cc, 0x8253, 0x3fae }},
  {{0x0000,0xa88a, 0x0ef5, 0x3fb2 }},
  {{0x0000,0xd5f7, 0x500d, 0x3fb5 }},
  {{0x0000,0xae34, 0x1a98, 0x3fb9 }},
  {{0x0000,0x24e3, 0x8be4, 0x3fbd }},
  {{0x0000,0x59ed, 0x6615, 0x3fc1 }},
  {{0x0000,0x8e94, 0x8aac, 0x3fc4 }},
  {{0x0000,0x69f3, 0x6033, 0x3fc8 }},
  {{0x0000,0x0c10, 0x31a3, 0x3fcd }},
  {{0x0000,0xbfac, 0xc8b4, 0x3fd1 }},
  {{0x0000,0x44fe, 0x7d2f, 0x3fd6 }},
  {{0x0000,0x0000, 0x0000, 0x0000 }}
};

NEWINTERVAL asin_tab_B[] = {
{{0x0000,0x0000, 0x0000, 0x0000 ,
  0x0000,0x0000, 0x0000, 0x0000 }},
{{0xe3e9,0x551b, 0x6d94, 0x3c2d ,
  0xe3ea,0x551b, 0x6d94, 0x3c2d }},
{{0xb77b,0x8b71, 0x460b, 0x3cb9 ,
  0xb77c,0x8b71, 0x460b, 0x3cb9 }},
{{0xdf7d,0x58fc, 0x0e7b, 0x3cc7 ,
  0xdf7e,0x58fc, 0x0e7b, 0x3cc7 }},
{{0x24ea,0x5877, 0x0be7, 0x3cce ,
  0x24eb,0x5877, 0x0be7, 0x3cce }},
{{0x7f12,0x98ae, 0x6ea2, 0x3cd1 ,
  0x7f13,0x98ae, 0x6ea2, 0x3cd1 }},
{{0x42c1,0x1ecb, 0x75de, 0x3cff ,
  0x42c2,0x1ecb, 0x75de, 0x3cff }},
{{0x0174,0x62ef, 0xa88c, 0x3ce2 ,
  0x0175,0x62ef, 0xa88c, 0x3ce2 }},
{{0xa2e0,0xd09d, 0xd1e5, 0x3d1c ,
  0xa2e1,0xd09d, 0xd1e5, 0x3d1c }},
{{0x28f5,0x099d, 0x853f, 0x3ce5 ,
  0x28f6,0x099d, 0x853f, 0x3ce5 }},
{{0x653f,0xec9a, 0x5363, 0x3d25 ,
  0x6540,0xec9a, 0x5363, 0x3d25 }},
{{0xbb2e,0x7ed2, 0x9d02, 0x3cfb ,
  0xbb2f,0x7ed2, 0x9d02, 0x3cfb }},
{{0xe036,0xd232, 0x9827, 0x3d26 ,
  0xe037,0xd232, 0x9827, 0x3d26 }},
{{0x0a5e,0x3f34, 0x9397, 0x3d24 ,
  0x0a5f,0x3f34, 0x9397, 0x3d24 }},
{{0x6550,0x0450, 0x7b3a, 0x3d3a ,
  0x6551,0x0450, 0x7b3a, 0x3d3a }},
{{0xd9eb,0x7d94, 0xe7c0, 0x3d4d ,
  0xd9ec,0x7d94, 0xe7c0, 0x3d4d }},
{{0x9707,0x2dd9, 0x2c23, 0x3d3b ,
  0x9708,0x2dd9, 0x2c23, 0x3d3b }},
{{0x2236,0xb8ef, 0xb283, 0x3d34 ,
  0x2237,0xb8ef, 0xb283, 0x3d34 }},
{{0xfa39,0x1d79, 0xd022, 0x3d2c ,
  0xfa3a,0x1d79, 0xd022, 0x3d2c }},
{{0xd298,0x769b, 0x5a9e, 0x3d44 ,
  0xd299,0x769b, 0x5a9e, 0x3d44 }},
{{0xa16b,0xcea9, 0xe4a4, 0x3cdd ,
  0xa16c,0xcea9, 0xe4a4, 0x3cdd }},
{{0x990f,0x4f63, 0x2c8d, 0x3d40 ,
  0x9910,0x4f63, 0x2c8d, 0x3d40 }},
{{0x8e8a,0x171d, 0x675b, 0x3d57 ,
  0x8e8b,0x171d, 0x675b, 0x3d57 }},
{{0x3c9c,0x36eb, 0x5d88, 0x3d67 ,
  0x3c9d,0x36eb, 0x5d88, 0x3d67 }},
{{0x66c1,0xb07b, 0x8102, 0x3d1b ,
  0x66c2,0xb07b, 0x8102, 0x3d1b }},
{{0xd8c0,0x1f9e, 0x87f2, 0x3d62 ,
  0xd8c1,0x1f9e, 0x87f2, 0x3d62 }},
{{0x2169,0xc13c, 0x2e86, 0x3d73 ,
  0x216a,0xc13c, 0x2e86, 0x3d73 }},
{{0x661f,0x0779, 0x1268, 0x3d7d ,
  0x6620,0x0779, 0x1268, 0x3d7d }},
{{0xbe74,0xd427, 0x636e, 0x3d62 ,
  0xbe75,0xd427, 0x636e, 0x3d62 }},
{{0xa7e7,0xd7e2, 0xaf35, 0x3d65 ,
  0xa7e8,0xd7e2, 0xaf35, 0x3d65 }},
{{0x23fa,0xe664, 0x63ec, 0x3d82 ,
  0x23fb,0xe664, 0x63ec, 0x3d82 }},
{{0xba20,0x7b0a, 0x430e, 0x3d56 ,
  0xba21,0x7b0a, 0x430e, 0x3d56 }},
{{0x0000,0x0000, 0x0000, 0x0000 ,
  0x0000,0x0000, 0x0000, 0x0000 }}
};
#endif


/*
****************************************************************
              INTERVAL asin_hot(INTERVAL Z)
   this computes asin_hot by doing a range reduction
   based on the addition law:
    asin(z) = asin(y) + asin(z*sqrt(1-y^2)-y*sqrt(1-z^2))
   and using a table to lookup the asin(y) values.

****************************************************************
*/

INTERVAL asin_hot(INTERVAL Z) {
  INTERVAL R,Tmp,T1;
  int i;
  double y;
  INTERVAL Alpha,Beta,Gamma,A,X,X2;

  if (debug) printf("Entering asin_hot([%.20f,%.20f])\n",Z.lo,Z.hi);

  i = floor(Z.lo*asin_tab_size);

  if (Z.hi < 0.125) return(asin_eps_hot(Z));

  y = i/(asin_tab_size*1.0);
  A = subIDI(Z,y);
  X  = sqrtII(mulIII(subDDI(1.0,y),addDDI(1.0,y)));
  T1 = mulIII(subDII(1.0,Z),addDII(1.0,Z));
  T1 = intersectIII(T1,subDII(1.0,mulIII(Z,Z)));
  X2 = sqrtII(subDII(1.0,mulIII(Z,Z)));
  Alpha = addIII(mulIII(Z,X),mulDII(y,X2));
  Beta = divIII(mulIII(A,addIDI(Z,y)),Alpha);

  Gamma = mulIII(subIDI(Z,y),divIII(subIII(addIDI(Z,y),Alpha),Alpha));

  Tmp = asin_eps_hot(Beta);

  R=addDII(asin_tab_A[i].d,
      addIII(asin_tab_B[i].i,
        addIII(Tmp,
               Gamma)));

  return(R);
}




void recenter_ep(double *x, INTERVAL *W)
{ UDOUBLE f;
  INTERVAL Q;

      Q = addDII(*x,*W);
      f.d=(Q.lo+Q.hi)/2.0;
      f.u[1]=0;
      *W=addIII(*W,subDDI(*x,f.d));
      *x=f.d;
}



/*
****************************************************************
       asin_ext(x,W,I) = asin(x+W) + I

  This procedure uses the following reductions:

     asin(x) = asin(y) + asin(beta(x,y)),

  where beta(x,y) = x*sqrt(1-y^2)-x*sqrt(1-x^2))
  If we let asin(v) = v + h(v) then
     h(v) = asin(v) - v and

     asin(x) = y+h(y) + beta + h(beta)
             = x + h(y) + h(beta) + (y + beta - x)

  So the addition law for h is:
     h(x) = h(y) + h(beta(x,y)) + gamma(x,y)
  where
     gamma(x,y) = beta(x,y) - (x - y)

  y is selected to be floor(x*32)/32
  asin(y)= y + A1+A2 
      is computed by table lookup with extended precision for A1,A2
  asin(beta) = beta + B1
      is computed by algebraic approximation
  The result is computed by
   asin(x) = 
   R = x+W + (A1 + (gamma(x,y) + (I + (B1 + A2)))),

  We must exercise care in computing beta(x,y) and gamma(x,y)
  if we want to preserve high precision in our results. The
  problem is that these expressions involve differences of nearly
  equal quantities which can generate large roundoff errors
  if not handled carefully. We also make use of extended
  precision to represent the input, so we compute asin(x+W)
  where W is a small interval and x is a double precision number.
   The algebra we use is:

   to compute asin(x+W)+I for small, narrow intervals W and I,
   let
   Z=x+W
   y = floor(Z.lo*32)/32
   A = (x-y)+W
   X1 = sqrt((1-y)*(1+y))
   T1=  ((1-x)-W)*((1+x)+W)
   X2 = sqrt(T1)
   RR = A*((x-y)-W)/(X1+X2)
   Alpha = y*(2*X1-RR) + A*X1
   Beta = A*((x+y)+W)/Alpha
   Gamma = (Z-y)*((Z+y)-Alpha)/Alpha

   asin(x+W)+I =
       x + W + (asin_0(y) + (Gamma + (I + (asin(Beta) + asin_1(y)))))

   where asin(y) = asin_0(y) + asin_1(y)


   This gets very high precision but it is quite expensive.


****************************************************************
*/

INTERVAL asin_ext(double x,INTERVAL W, INTERVAL I) {
  int i;
  double y;
  INTERVAL Z,R,Alpha,Beta,Gamma,A;

  /* adjust the extended precision representation x+W */
  recenter_ep(&x,&W);

  Z = addDII(x,W);
  i = floor(Z.lo*asin_tab_size);

  if (debug) printf("in asin_ext, i=%d\n",i);

  if (Z.hi < 0.125)
     return(addDII(x,addIII(W,addIII(asin_eps_hot(Z),I))));


  /* Prepare to apply addition law with Z = y + A,  y = i/32.0 */
  y = i/(asin_tab_size*1.0);
  A = addIII(subDDI(x,y),W);

  /* compute alpha(Z,y) and beta(Z,y) and gamma(Z,y) */
  { INTERVAL T1,X1,X2,RR;
    X1  = sqrtII(mulIII(subDDI(1.0,y),addDDI(1.0,y)));
    T1 = mulIII(subIII(subDDI(1.0,x),W),addIII(addDDI(1.0,x),W));
    X2 = sqrtII(T1);
    RR = divIII(mulIII(A,addIII(addDDI(x,y),W)),
                addIII(X1,X2));  
    Alpha = addIII(mulDII(y,subIII(mulDII(2,X1),RR)),
                   mulIII(A,X1));
    Beta = divIII(mulIII(A,addIII(addDDI(x,y),W)),Alpha);
    Gamma = mulIII(subIDI(Z,y),
                 divIII(subIII(addIDI(Z,y),Alpha),
                        Alpha));
  }

  /* now we use the addition formula:
    asin(x+W)+I =
       x + W + asin_0(y) + Gamma + I + asin(Beta) + asin_1(y)
    where the terms are in generally decreasing order
  */

  { INTERVAL Tmp,Tmp1,Tmp2,Tmp3;
    Tmp = asin_eps_hot(Beta);
    Tmp1 = addIII(I,addIII(Tmp,asin_tab_B[i].i));
    Tmp2 = addIII(Gamma,Tmp1);
    Tmp3=addDII(asin_tab_A[i].d, Tmp2);
    R=addDII(x, addIII(W,Tmp3));
  }
  return(R);
}


   /* 
   ****************************************************************
   ****************************************************************

           INTERVAL asinDI(double x)

    Here we compute an interval Y containing asin(x)
    for a double x. We use the following properties to
    reduce the domain to [0,0.5], 
       where S3=sqrt(3/4), S2=sqrt(1/2)

    asin(x) = -asin(-x)                            ,if x < 0

    asin(x) = pi/2 - 2*asin(sqrt((1-x)/2))         ,if x > 0.875 

    asin(x) = x + asin_hot(x)                      ,else

   ****************************************************************
   ****************************************************************
   */

INTERVAL asinDI(double x) {
  INTERVAL T,Y;


  if (x < 0.0)
     Y = negII(asinDI(-x));
  else if (x > 0.875){
   T= sqrtII(mulIDI(subDDI(1.0,x),0.5));
   Y = 
         subDII(PI_OVER_2_A.d,
            mulDII(2.0,asin_ext(0.0,T,negII(PIOVER4B.i))));
  }
  else if (x >= 0.0) {
    Y = asin_ext(x,cnstDI(0.0),cnstDI(0.0));
  }

  Y = intersectIII(Y,makeDDI(-PIOVER2.i.hi,PIOVER2.i.hi));


  return(Y);
}


/*
INTERVAL acosDI(double x) {
  return(subIII(PIOVER2.i,asinDI(x)));
}
*/


  /* 
  ****************************************************************
  ****************************************************************

          INTERVAL acosDI(double x)

  This uses the following reduction formulae:

  acos(x) = pi   - 2*asin(sqrt((1+x)/2)) ,if x in [-1.0,  -0.875] 
  acos(x) = pi/2 + asin(-x)              ,if x in [-0.875, 0.0]
  acos(x) = pi/2 - asin(x)               ,if x in [ 0.0,   0.875]
  acos(x) =        2*asin(sqrt((1-x)/2)) ,if x in [ 0.875, 1.0]

  ****************************************************************
  ****************************************************************
  */

INTERVAL acosDI(double x) {
  INTERVAL T,S,Y;


  if (x < -1.0) {
    return(PNINF.i);
  }
  else if (x < -0.500) {
       T= sqrtII(mulIDI(addDDI(1.0,x),0.5));
       Y = 
         subDII(PI_A.d,
            mulDII(2.0,asin_ext(0.0,T,negII(PIOVER2B.i))));
     }
  else if (x < 0)
      Y = addDII(PI_OVER_2_A.d,
          addIII(asinDI(-x),makeDDI(PI_OVER_2_B_LO.d,PI_OVER_2_B_HI.d)));
  else if (x <= 0.500)
    Y =  
      subDII(PI_OVER_2_A.d,
          asin_ext(x,cnstDI(0.0),negII(PIOVER2B.i)));
  else if (x <= 1.0) {
    INTERVAL U; double b;
    S = divIDI(subDDI(1.0,x),2.0);
    mp_sqrtIDIV(S,&b,&U);
    Y = mulDII(2.0,asin_ext(b,U,cnstDI(0.0)));
  }
  else {/* error case */
    return(PNINF.i);
  }

  Y = intersectIII(Y,makeDDI(0,2*PIOVER2.i.hi));

  if (debug) {
    printf("acosDI(x)=Y\n");
    print_interval("x=",cnstDI(x));
    print_interval("Y=",Y);
  }

  return(Y);
}






INTERVAL atanDI(double x) {
  INTERVAL X;

  if (x < 0)
    return(negII(atanDI(-x)));
  else if (x < 1) {
    X = divDII(x,sqrtII(addDII(1,mulDDI(x,x))));
    return(asinII(X));
  }
  else {
/*
    print_interval("1/x",divDDI(1,x));
    print_interval("atan(1/x)",atanII(divDDI(1,x)));
    print_interval("pi/2-atan(1/x)",subIII(PIOVER2.i,atanII(divDDI(1,x))));
*/
    X=divDII(1,sqrtII(addDII(1,mulDDI(x,x))));
    return(subIII(PIOVER2.i,asinII(X)));

    /*
    return(subIII(PIOVER2.i,atanII(divDDI(1,x))));
    */
/*
    X = sqrtII(divDII(0.5,
                      addDII(1,mulIII(mulDDI(x,x),
                         addDII(1,divIDI(sqrtII(addDII(1,mulDDI(x,x))),x))))));
    return(subIII(PIOVER2.i,mulDII(2,asinII(X))));
*/
  }
}


INTERVAL asin2piDI(double x) {
  return(divIII(asinDI(x),TWOPI.i));
}

INTERVAL acos2piDI(double x) {
  return(divIII(acosDI(x),TWOPI.i));
}

INTERVAL atan2piDI(double x) {
  return(divIII(atanDI(x),TWOPI.i));
}


