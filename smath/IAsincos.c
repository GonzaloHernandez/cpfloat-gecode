/*
   IAsincos.c

   This is a library of sound and precise implementations of the
   trigonometric functions, sin, cos, and tan.

   2000 Copyright Timothy J. Hickey
   2003 Copyright David K. Wittenberg
   licence: http://interval.sourceforge.net/licence.txt, zlib/png 

*/


/*            CHANGELOG

2003-03-11  changed __BYTE_ORDER to BYTE_ORDER for portability      dkw
2003-12-12  removed include math.h, as it's in smath                dkw

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
  //PI_OVER_4_A      =  {{0x3fe9, 0x21fb, 0x5440, 0x0000}},
  //PI_OVER_2_LO     =  {{0x3ff9, 0x21fb, 0x5444, 0x2d18}},
  PI_OVER_2_A      =  {{0x3ff9, 0x21fb, 0x5440, 0x0000}};
  //PI_OVER_2_B      =  {{0x3dd0, 0xB461, 0x1A60, 0x0000}}, 
  //PI_OVER_2_B_LO   =  {{0x3dd0, 0xB461, 0x1A62, 0x6331}}, 
  //PI_OVER_2_B_HI   =  {{0x3dd0, 0xB461, 0x1A62, 0x6332}}, 
  //PI_OVER_2_C_LO   =  {{0x3ba3, 0x198A, 0x2E03, 0x7073}}, 
  //PI_OVER_2_C_HI   =  {{0x3ba3, 0x198A, 0x2E03, 0x7074}};


static NEWINTERVAL
  //PNINF          =
    //{{0xfff0, 0x0000, 0x0000, 0x0000,
      //0x7ff0, 0x0000, 0x0000, 0x0000}},
  //PIOVER4B       =
    //{{0x3dc0, 0xB461, 0x1A62, 0x6331,
      //0x3dc0, 0xB461, 0x1A62, 0x6332}}, 
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
  //PI_OVER_4_A      =  {{0x0000,0x5440, 0x21fb, 0x3fe9 }},
  //PI_OVER_2_LO     =  {{0x2d18,0x5444, 0x21fb, 0x3ff9 }},
  PI_OVER_2_A      =  {{0x0000,0x5440, 0x21fb, 0x3ff9 }};
  //PI_OVER_2_B      =  {{0x0000,0x1A60, 0xB461, 0x3dd0 }}, 
  //PI_OVER_2_B_LO   =  {{0x6331,0x1A62, 0xB461, 0x3dd0 }}, 
  //PI_OVER_2_B_HI   =  {{0x6332,0x1A62, 0xB461, 0x3dd0 }}, 
  //PI_OVER_2_C_LO   =  {{0x7073,0x2E03, 0x198A, 0x3ba3 }}, 
  //PI_OVER_2_C_HI   =  {{0x7074,0x2E03, 0x198A, 0x3ba3 }};


static NEWINTERVAL
  //PNINF          =
    //{{0x0000,0x0000, 0x0000, 0xfff0 ,
      //0x0000,0x0000, 0x0000, 0x7ff0 }},
  //PIOVER4B       =
    //{{0x6331,0x1A62, 0xB461, 0x3dc0 ,
      //0x6332,0x1A62, 0xB461, 0x3dc0 }}, 
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

   /* 
   **************************************************************** 
   ****************************************************************
   ****************************************************************
   ****************************************************************
   ****************************************************************

        SOUND EVALUATION OF THE TRIGONOMETRIC FUNCTIONS

   **************************************************************** 
   ****************************************************************
   ****************************************************************
   ****************************************************************
   ****************************************************************
   */




   /* 
   ****************************************************************
       INTERVAL sin_hot(INTERVAL A2)

   This computes an interval R that contains f(A2),
   where
     f(x^2) = (sin(x)-x)/(x^3/6),
   i.e., 
     sin(x) = x + x^3/6*f(x^2)

   We assume that 
      A2 < [0,pi/4+epsilon] < [0,1]
   ****************************************************************
   */

INTERVAL sin_hot(INTERVAL A2) {
 int j,deg4 = 5;
 INTERVAL R;

 R = makeDDI(-1,1);

    for (j=4*deg4;j>=8;j -= 4) {
       /* y = sin(a), R = (1+a^2/((j-1)*(j-2))*(-1 + a^2/(j*(j+1))*R)) */
       R= addDII(-1.0,divIDI(mulIII(A2,R),(j+1)*j));
       R= addDII( 1.0,divIDI(mulIII(A2,R),(j-1)*(j-2)));
       if (debug){ printf("  j=%d  ",j); print_interval("R=",R); }
     }
    R = addDII(-1.0,divIDI(mulIII(A2,R),20.0));
       if (debug){ printf("  j=%d  ",j); print_interval("R=",R); }

     if (debug) print_interval("sin_hot=",R);

  return(R);

}




   /*
   ****************************************************************
      INTERVAL cos_hot(INTERVAL A2)

   This computes an interval R that contains g(A2),
   where
     g(x^2) = (cos(x) - 1 + x^2/2)/(x^4/12)
   i.e.,
     cos(x) = 1 - x^2/2 + x^4/12*g(x^2)
   ****************************************************************
   */

INTERVAL cos_hot(INTERVAL A2) {
 int j,deg4 = 5;
 INTERVAL R;

 R = makeDDI(-1,1);

    for (j=4*deg4;j>=8;j -= 4) {
       /* y = cos(a), R = (1+a^2/((j-3)*(j-2))*(-1 + a^2/((j-1)*j)*R)) */
       R= addDII(-1.0,divIDI(mulIII(A2,R),(j-1)*j));
       R= addDII(+1.0,divIDI(mulIII(A2,R),(j-3)*(j-2)));
      if (debug){ printf("  j=%d  ",j); print_interval("R=",R); }
     }

     if (debug) print_interval("cos_hot=",R);

  return(R);
}


   /*
   ****************************************************************
   this computes sin(a+W) assuming that
   a+W is in the range [0,0.80], using the formula
      sin(a+W) =
          a + 
          W +
          A2*A*sin_hot(A2)/6
   where 
      A = a+W
      A2 = (a+W)^2 = (a^2 + W*(2*a+W))
   and care must be taken computing A2 so that the width
   of A2 stays small, especially when a is near 0.80
   ****************************************************************
   */

INTERVAL sin0(double a, INTERVAL W) {
      INTERVAL A,A2,a2,W2,y;

      A = addDII(a,W);
      a2 = mulDDI(a,a);
      W2 = mulIII(W,addIII(W,mulDDI(2,a))); 
      A2=addIII(a2,W2);
      /* we need to compute A2 carefully so as to keep its
         width small. This is only a problem when a is near
         its maximum since a^3/6*sin_hot is about 1/12 and
         a is about 3/4. Thus, we only get 3 free bits
         hence A2^3*sin_hot(A2)/6 has to be correct to 8 ULP
      */    


      y = addDII(a,
          addIII(W,
                 divIDI(mulIII(A,mulIII(A2,sin_hot(A2))),
                        6.0)));

     if (debug) print_interval("a=",cnstDI(a));
     if (debug) print_interval("W=",W);
     if (debug) print_interval("A=",A);
     if (debug) print_interval("A2=",A2);
     if (debug) print_interval("A*A2=",mulIII(A,A2));
     if (debug) print_interval("a^3/6*R=",
                 divIDI(mulIII(A,mulIII(A2,sin_hot(A2))),
                        6.0));
     if (debug) print_interval("y=",y);
   return(y);

}


   /* 
   ****************************************************************
   this computes cos(a+W) assuming that
   a+W is in the range [0,0.80]
   We use the following formula, with X=a+W
   cos(X) =
         1
      + -A2/2
      + -W2/2
      +  A4/24.0*cos_hot(A2)
   where
     A2 = (a+W)^2 = (a^2 + W*(2*a+W))
     A4 = A2*A2
   and care must be taken computing 1 - (a+W)^2/2 + (a+W)^4*R/24
   Since "a" is actually a single precision number, it has
   23 stored bits, thus, squaring "a" requires 47 stored bits,
   and this leaves 5 spare bits. So if a2 > 2^(-5), then
   1-a2/2 can be computed with no roundoff error. 

   ****************************************************************
   */

INTERVAL cos0(double a, INTERVAL W) {
      INTERVAL R,A2,A4,a2,W2; //A

       //A = addDII(a,W);

       a2 = mulDDI(a,a);
       W2 = mulIII(W,addIII(W,mulDDI(2,a))); 
       A2=addIII(a2,W2);

       A4 = mulIII(A2,A2);

       /* (a+W)^2 in a2 + W2 */
       R= addDII(1.0,
          addIII(negII(divIDI(a2,2.0)),
          addIII(divIDI(negII(W2),2.0),
                 divIDI(mulIII(A4,cos_hot(A2)),24.0))));


     if (debug) print_interval("a2=",a2);
     if (debug) print_interval("W2=",W2);
     if (debug) print_interval("1-a2/2=",
                               subDII(+1.0,divIDI(a2,2.0)));
     if (debug) print_interval("y=",R);

  return(R);
}




INTERVAL sin0DI(double x) {
  double y; INTERVAL W,Z; //Y 

  if (x < 0) 
     return(negII(sin0DI(-x)));
  else if (x <= 0.125*TWOPI.i.hi) {
     Z = sin0(x,cnstDI(0));
     return(Z);
   }
  else {
     y = PIOVER2.i.hi - x;
     W = addIII(subIDI(subDDI(PI_OVER_2_A.d,x),y),PIOVER2B.i);
     Z = cos0(y,W);
     return(Z);
  }
}



INTERVAL cos0DI(double x) {
  double y; INTERVAL W,Z; //Y

  if (x < 0) 
     return(cos0DI(-x));
  else if (x <= 0.125*TWOPI.i.hi) {
     Z = cos0(x,cnstDI(0));
     return(Z);
   }
  else {
     y = PIOVER2.i.hi - x;
     W = addIII(subIDI(subDDI(PI_OVER_2_A.d,x),y),PIOVER2B.i);
     Z = sin0(y,W);
     return(Z);
  }
}

INTERVAL tan0DI(double x) {
  return(divIII(sin0DI(x),cos0DI(x)));
}



INTERVAL sin2pi0DI(double x) {
/*
  Let x*2pi = y+W,  (1/4-x)*2pi = z+U
  if (x<1/8)
        sin2pi0DI(x) = sin0(y,W)
  else
        sin2pi0DI(x) = cos0(z+U)
*/
  double y; INTERVAL W,Y,Z; 

  //int s;

  if (x < 0) 
    return(negII(sin2pi0DI(-x)));
  else if (x <= 0.125) {
     Y = mulDII(x,TWOPI.i);
     y = (Y.lo + Y.hi)/2.0;
     W = subIDI(Y,y);
     Z = sin0(y,W);
     return(Z);
   }
  else {
     Y = mulIII(subDDI(0.25,x),TWOPI.i);
     y = (Y.lo + Y.hi)/2.0;
     W = subIDI(Y,y);
     Z = cos0(y,W);
     return(Z);
  }
}


INTERVAL cos2pi0DI(double x) {
/*
  Let x*2pi = y+W,  (1/4-x)*2pi = z+U
  if (x<1/8)
        sin2pi0DI(x) = sin0(y,W)
  else
        sin2pi0DI(x) = cos0(z+U)
*/
  double y; INTERVAL W,Y,Z; 

  if (x < 0) x = -x;

  if (x <= 0.125) {
     Y = mulDII(x,TWOPI.i);
     y = (Y.lo + Y.hi)/2.0;
     W = subIDI(Y,y);
     Z = cos0(y,W);
     return(Z);
   }
  else {
     Y = mulIII(subDDI(0.25,x),TWOPI.i);
     y = (Y.lo + Y.hi)/2.0;
     W = subIDI(Y,y);
     Z = sin0(y,W);
     return(Z);
  }
}

INTERVAL tan2pi0DI(double x) {
  return(divIII(sin2pi0DI(x),cos2pi0DI(x)));
}


