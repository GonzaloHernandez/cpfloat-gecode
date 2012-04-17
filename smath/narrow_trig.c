/* 
     trig.c
        -- narrowing and interval functions for trig fns.

   2000 Copyright Timothy J. Hickey
   2003 Copyright David K. Wittenberg
   licence: http://interval.sourceforge.net/licence.txt, zlib/png 

*/

/*            CHANGELOG

2003-03-11  changed __BYTE_ORDER to BYTE_ORDER for portability      dkw
2003-12-12  removed some includes duplicated by smath               dkw 

*/



#include "smath.h"
#include <stdio.h>


   /*
   ****************************************************************
              CONSTANTS used in this module

     pi/2 = 
     1.921F B544 42D1 8469 898C C517 01B8 39A2 5204 9C11 ...
     base 16
   ****************************************************************
   */

#if BYTE_ORDER == BIG_ENDIAN
static NEWDOUBLE
  //TWOPI_A              =  {{0x4019, 0x21fb, 0x5430, 0x0000}},
  //TWOPI_B              =  {{0x3e14, 0x2d18, 0x4680, 0x0000}},
  HALFPI_A             =  {{0x3ff9, 0x21fb, 0x5300, 0x0000}},  /* 24 trailing zero bits */
  HALFPI_B             =  {{0x3e34, 0x42d1, 0x8300, 0x0000}},
  //HALFPI_LO            =  {{0x3ff9, 0x21fb, 0x5444, 0x2d18}},
  HALFPI_HI            =  {{0x3ff9, 0x21fb, 0x5444, 0x2d19}},
  //PI_LO                =  {{0x4009, 0x21fb, 0x5444, 0x2d18}},
  PI_HI                =  {{0x4009, 0x21fb, 0x5444, 0x2d19}};
  //TWOPI_LO             =  {{0x4019, 0x21fb, 0x5444, 0x2d18}},
  //TWOPI_HI             =  {{0x4019, 0x21fb, 0x5444, 0x2d19}};

static NEWDOUBLE
    POS_INF       = {{0x7ff0, 0x0000, 0x0000, 0x0000}},
    NEG_INF       = {{0xfff0, 0x0000, 0x0000, 0x0000}};

static NEWINTERVAL
  PN_INF           =
    {{0xfff0, 0x0000, 0x0000, 0x0000,
      0x7ff0, 0x0000, 0x0000, 0x0000}},
  HALFPIC       =
    {{0x3c76, 0x9898, 0xcc51, 0x701b,
      0x3c76, 0x9898, 0xcc51, 0x701c}},
  //TWOPIC       =
    //{{0x3c18, 0x98cc, 0x5170, 0x1b83,
      //0x3c18, 0x98cc, 0x5170, 0x1b84}},
  //TWOPIB       =
    //{{0x3e14, 0x2d18, 0x4698, 0x98cc,
      //0x3e14, 0x2d18, 0x4698, 0x98cd}},
  HALFPI       =
    {{0x3ff9, 0x21fb, 0x5444, 0x2d18,
      0x3ff9, 0x21fb, 0x5444, 0x2d19}};
  //PI           =
    //{{0x4009, 0x21fb, 0x5444, 0x2d18,
      //0x4009, 0x21fb, 0x5444, 0x2d19}},
  //TWOPI        =
    //{{0x4019, 0x21fb, 0x5444, 0x2d18,
      //0x4019, 0x21fb, 0x5444, 0x2d19}};
#endif


#if BYTE_ORDER == LITTLE_ENDIAN
static NEWDOUBLE
  //TWOPI_A              =  {{0x0000,0x5430, 0x21fb, 0x4019 }},
  //TWOPI_B              =  {{0x0000,0x4680, 0x2d18, 0x3e14 }},
  HALFPI_A             =  {{0x0000,0x5300, 0x21fb, 0x3ff9 }},  /* 24 trailing zero bits */
  HALFPI_B             =  {{0x0000,0x8300, 0x42d1, 0x3e34 }},
  //HALFPI_LO            =  {{0x2d18,0x5444, 0x21fb, 0x3ff9 }},
  HALFPI_HI            =  {{0x2d19,0x5444, 0x21fb, 0x3ff9 }},
  //PI_LO                =  {{0x2d18,0x5444, 0x21fb, 0x4009 }},
  PI_HI                =  {{0x2d19,0x5444, 0x21fb, 0x4009 }};
  //TWOPI_LO             =  {{0x2d18,0x5444, 0x21fb, 0x4019 }},
  //TWOPI_HI             =  {{0x2d19,0x5444, 0x21fb, 0x4019 }};

static NEWDOUBLE
    POS_INF       = {{0x0000,0x0000, 0x0000, 0x7ff0 }},
    NEG_INF       = {{0x0000,0x0000, 0x0000, 0xfff0 }};

static NEWINTERVAL
  PN_INF           =
    {{0x0000,0x0000, 0x0000, 0xfff0 ,
      0x0000,0x0000, 0x0000, 0x7ff0 }},
  HALFPIC       =
    {{0x701b,0xcc51, 0x9898, 0x3c76 ,
      0x701c,0xcc51, 0x9898, 0x3c76 }},
  //TWOPIC       =
    //{{0x1b83,0x5170, 0x98cc, 0x3c18 ,
      //0x1b84,0x5170, 0x98cc, 0x3c18 }},
  //TWOPIB       =
    //{{0x98cc,0x4698, 0x2d18, 0x3e14 ,
      //0x98cd,0x4698, 0x2d18, 0x3e14 }},
  HALFPI       =
    {{0x2d18,0x5444, 0x21fb, 0x3ff9 ,
      0x2d19,0x5444, 0x21fb, 0x3ff9 }};
  //PI           =
    //{{0x2d18,0x5444, 0x21fb, 0x4009 ,
      //0x2d19,0x5444, 0x21fb, 0x4009 }},
  //TWOPI        =
    //{{0x2d18,0x5444, 0x21fb, 0x4019 ,
      //0x2d19,0x5444, 0x21fb, 0x4019 }};
#endif

int intersectionII(INTERVAL a,  INTERVAL *x);

static void new_print_double(char *s,double f) {
unsigned int * u;
  u = (unsigned int *) &f;
  printf("%20s %8x %8x %.20g\n",s,*u,*(u+1),f);
}

static void new_print_interval(char *s, INTERVAL p) {
  printf("%s\n",s);
  new_print_double("lo = ",p.lo);
  new_print_double("hi = ",p.hi);
  printf("\n");
}


   /*
   ****************************************************************
          INTERVAL periodicfnII(          
                   INTERVAL X, 
                        int (*reduce)(), 
                   INTERVAL (*range)(),
                     double (*f)(), 
                   INTERVAL Y,
                        int K
          )


   SOUND EVALUATION OF A PERIODIC FUNCTION ON AN INTERVAL X
   this procedure has the following arguments which describe
   everything that is needed to know to narrow a periodic function.


   int reduce(int hilo, double x, double *N, int *J, double *Z)
      this computes an double Z such that
             x in W + a_J + N*period, 
      where W = [Z,inf] if hilo = LO and [-inf,Z] if hilo = HI
      and where a_(J-1) < (Z + a_J) < a_(J+1)
      and where the fundamental domain [0,p] of the period is decomposed 
      into a sequence of intervals whose endpoints are
        0 = a_0 < a_1 < a_2 < ... < a_K < p=a_{K+1}
      We assume that F is monotone on each of the K+1 subintervals.
      and to increase precision we suggest that the a_i include
      all zeroes of F, all extrema, and all discontinuities.

      If it is not able to make such a decomposition it returns 0
      otherwise it returns 1.
     
   INTERVAL range(int i,int w)
      This returns the interval [F(-a_i),F(+a_{i+w}))]

   double f(int left_right, int j, double x)
      This is a function which returns upper/lower bounds
      on the value f(x+a_j). the first argument specifies
      whether we are evaluating f on a left or right endpoint
      of an interval, and based on that and the slope of the
      function at x+a_j we return either an upper or lower
      bound. More precisely, for increasing functions we
      return lower-bounds for left endpoints and upper for right.
      For decreasing functions, we do the reverse.
    

   INTERVAL Y;
      this is the value of F([-inf,inf])

   int K;
      this is the number of points decomposing a period,
      and we assume that K >= 2, else F is a constant function.


   The algorithm we use for narrowing is to 
     1) reduce X.lo and X.hi to get
            X.lo in Zlo + a_Jlo + Nlo*period
            X.hi in Zhi + a_Jhi + Nhi*period
     2) compute the largest nlo and smallest nhi such that 
            a_(mlo)<=X.lo<=X.hi<=a_(mhi)
     3) based on the sign of F on I_(mlo) and I_(mhi)
        compute appropriate upper and/or lower bounds
        a of F(X.lo) and b of F(X.hi), 
        and let I0 = union([a],[b]),
     4) if Nlo = Nhi and nlo=nhi, return I0
     5) else compute the interval I = range(nlo+1,nlo+1+width)
        and return union(union([a],[b]),I)
     
   For sin,cos,tan,cot,sin2pi,cos2pi,tan2pi,cot2pi this will
   work well. It could also work for f(x) = x-floor(x)

   We can improve the performance to 2ULP worst case by
   modifying reduce to return an extended precision result
   and modifying f to use an extended precision argument,
   otherwise the reduction will sometimes produce a 2ULP error
   and evaluating the function of each of the two endpoints will
   produce a range of 2 ULP, which means we can not avoid some
   4 ULP errors, simply because of the tablemaker's  problem
   as it arises in reduction mod pi/2. 

   ****************************************************************
   */


INTERVAL periodicfnII(          
         INTERVAL X, 
              int (*reduce)(), 
         INTERVAL (*range)(),
           double (*f)(), 
         INTERVAL Y,
              int K
) {

  int Jlo,Jhi,mlo,mhi,width;
  double Nlo,Nhi,Zlo,Zhi,y1,y2;
  int debug=0;

  if (debug) {
    print_interval("periodicfnII: X",X);
    print_interval("Y :",Y); 
  }

  if (!(reduce(0, X.lo, &Nlo, &Jlo, &Zlo) 
         &&
        reduce(1, X.hi, &Nhi, &Jhi, &Zhi)))
    return(Y);

  mlo = (Zlo>=0)?Jlo:Jlo-1;
  mhi = (Zhi<=0)?Jhi:Jhi+1;

  width = (mhi- mlo+K*(Nhi-Nlo));
  if (width > K) return(Y);

  y1 = f(0,Jlo,Zlo);
  y2 = f(1,Jhi,Zhi);

  Y = unionDDI(y1,y2);

  if (debug) {
    new_print_double("Nlo",Nlo);
    new_print_double("Jlo",Jlo*1.0);
    new_print_double("Zlo",Zlo);
    new_print_double("mlo",mlo*1.0);

    new_print_double("Nhi",Nhi);
    new_print_double("Jhi",Jhi*1.0);
    new_print_double("Zhi",Zhi);
    new_print_double("mhi",mhi*1.0);

    print_interval("new Y :",Y); 
  }
  if (width <= 1)
    return(Y);
  else
    return(unionIII(Y,range(mlo+1,width-2)));
}


   /*
   ****************************************************************
   int reduce_trig2pi(int hilo, double x, double *N, int *J, double *Z)

   This attempts to find integers J and N and a double Z such that
       x = Z + J*(1/4) + N
   where abs(Z)<1/4. If it is unable to find such a triple (because
   of the limited precision of N), then it returns 0 otherwise 1.
   Notice that J+1 -(Z<0) gives the quadrant (1,2,3, or 4) of the
   trig function.
   ****************************************************************
   */
int reduce_trig2pi(int hilo, double x, double *N, int *J, double *Z) {
  double M;

  M = floor(4*x+0.5);
  *J = M - 4*floor(M/4);
  if (hilo == 1)
    *Z = sub_hi(x, M/4.0);
  else 
    *Z = sub_lo(x, M/4.0);

  *N = floor(M/4.0);

  return((-0.25 < *Z) && (*Z < 0.25));
}


/*
  This computes (x - N*pi/2) with last digit
  accuracy, assuming 
    N = round(div(x/(pi/2))) and
    abs(x)< 2^24
*/

INTERVAL rem_mod_halfpiN(double x, double N){ 
  INTERVAL U,V,W,Y,Y2; //X,Z;
  INTERVAL NHPA,NHPB,NHPC;
  int debug=0;

  if ((N > 1024*1024*4) || (N < -1024*1024*4))
    return(makeDDI(NEG_INF.d,POS_INF.d));

  NHPA = mulDDI(N,HALFPI_A.d);
  NHPB = mulDDI(N,HALFPI_B.d);
  NHPC = mulDII(N,HALFPIC.i);

  /* U is a first approx to x mod pi/2 */
  U =  subDII(x,NHPA);


  /* V is almost exactly divisibly by pi/2  and x in U.lo + V */
  V =  subDDI(x,U.lo);  

  /* W contains the remainder of V mod pi/2 */
  W = subIII(subIII(subIII(V,NHPA),NHPB),NHPC);

  /* Y contains the remainder of x mod pi/2 */
  Y = addDII(U.lo,W);

  if ((-0.01< Y.lo) && (Y.hi < 0.01)) {
  Y2 = subIII(subIII(subDII(x,NHPA),NHPB),NHPC);
    Y = intersectIII(Y,Y2);
  }

 if (debug) {
  new_print_double("HALFPI_A",HALFPI_A.d);
  new_print_double("HALFPI_B",HALFPI_B.d);
  print_interval("HALFPIC",HALFPIC.i);

  new_print_double("x",x);
  new_print_interval("mulDDI(N,HALFPI_A.d)",mulDDI(N,HALFPI_A.d));
  new_print_interval("x- ..", subDII(x,mulDDI(N,HALFPI_A.d)));
  new_print_interval("N*PI_B",mulDDI(N,HALFPI_B.d));
  new_print_interval(".. - N*PB",subIII(subDII(x,mulDDI(N,HALFPI_A.d)),mulDDI(N,HALFPI_B.d)));
  new_print_interval("N*PI_C",mulDII(N,HALFPIC.i));
  new_print_interval(".. - N*PC",subIII(subIII(subDII(x,mulDDI(N,HALFPI_A.d)),mulDDI(N,HALFPI_B.d)),mulDII(N,HALFPIC.i)));

  new_print_interval("U",U);
  new_print_interval("V",V);
  new_print_interval("W",W);
  new_print_interval("Y",Y);
 }



  return(Y);
}


/*
   this computes  Rlo < x - N*pi/2
*/

int div_precise_halfpi_lo(double x, double *N,double *Rlo) {
  INTERVAL I,R;

  I = divDII(x,HALFPI.i);
  *N = floor(I.lo+0.5);

  R  = rem_mod_halfpiN(x,*N);

  if (R.lo > HALFPI.i.hi) {
    *N = floor(I.lo)+1;
    R  = rem_mod_halfpiN(x,*N);
  }
    
  *Rlo = R.lo;

  return((-HALFPI.i.lo < *Rlo) && (*Rlo <= HALFPI.i.lo));
}

/*
   this computes  Rhi > x - N*pi/2
*/

int div_precise_halfpi_hi(double x, double *N,double *Rhi) {
  INTERVAL I,R;

  I = divDII(x,HALFPI.i);
  *N = floor(I.lo+0.5);
  R  = rem_mod_halfpiN(x,*N);

  if (R.hi > HALFPI.i.hi) {
    *N = floor(I.lo)+1;
    R  = rem_mod_halfpiN(x,*N);
    /* PROBLEM: we could now have R.hi < 0 !!!! */
  }

  *Rhi = R.hi;

  return((-HALFPI.i.lo < *Rhi) && (*Rhi <= HALFPI.i.lo));
}


   /*
   ****************************************************************
   This attempts to find integers J and N and a double Z such that
       x = Z + J*(pi/2) + N*2*pi, J in {0,1,2,3}, |Z|<pi/2
   This is only meant to be used with |x| < 2^20
   ****************************************************************
   */
int reduce_trig(int hilo, double x, double *N, int *J, double *Z) {
  double M;

  if (hilo==0) {
    if (!div_precise_halfpi_lo(x,&M,Z)) return(0);
    *N = floor(M/4);
    *J = M - 4*(*N);
  }
  else {
    if (!div_precise_halfpi_hi(x,&M,Z)) return(0);
    *N = floor(M/4);
    *J = M - 4*(*N);
  }

  return((-HALFPI.i.lo < *Z) && (*Z < HALFPI.i.lo));
}


   /*
   ****************************************************************
   This is a sound inverse to the reduce_trig2pi operation listed above.
   ****************************************************************
   */
double translate_trig2pi(int hilo, double N, int J, double Z) {
  //double M;

  if (hilo==0) 
    return(add_lo(add_lo(Z,J/4.0), N));
  else
    return(add_hi(add_hi(Z,J/4.0),N));
}



   /*
   ****************************************************************
   This is a sound inverse to the reduce_trig operation listed above.
   ****************************************************************
   */
double translate_trig(int hilo, double N, int J, double Z) {
  INTERVAL A;
  
  A = addDII(Z,
      addIII(mulDII(J,HALFPI.i),
      addIII(mulDDI(4*N,HALFPI_A.d),
      addIII(mulDDI(4*N,HALFPI_B.d),
             mulDII(4*N,HALFPIC.i)))));

  if (hilo==0) 
    return(A.lo);
  else
    return(A.hi);
}


   /*
   ****************************************************************
              INTERVAL range_sin(int a, int width)

   This returns the range of the sin function on the
   interval [a/4,(a+width)/4].
   ****************************************************************
   */
INTERVAL range_sin(int a, int width) {
/*      a0=0, a1=1/4, a2=1/2, a3=3/4, a4=1
  sin = 0     1       0       -1      0
  a could be 0,1,2,3,4, width could be 0,1,2,3
*/

  if (width>=3) return(makeDDI(-1,1));
  else if (width == 2) {
    if ((a==0)||(a==4)) return(makeDDI(0,1));
    else if (a==2) return(makeDDI(-1,0));
    else return(makeDDI(-1,1));
  }
  else if (width == 1) {
    if ((a==0)||(a==1)||(a==4)) return(makeDDI(0,1)); 
    else return(makeDDI(-1,0));
  }
  else if (width==0) {
    if ((a==0)||(a==2)||(a==4)) return(makeDDI(0.0,0.0));
    else if (a==1) return(makeDDI(1.0,1.0));
    else return(makeDDI(-1.0,-1.0));
  }
  else return(makeDDI(NEG_INF.d,POS_INF.d));


}

  /* this computes the range of cos in a similar fashion */
INTERVAL range_cos(int a, int width) {
  return(range_sin((a+1)%4,width));
}

   /*
   ****************************************************************
           double eval_sin2pi(int hilo, int j, double x)

   This returns an upper/lower bound on sin2pi(x+j/4) for
   j in {0,1,2,3} and x in [-0.25,0.25] 
   ****************************************************************
   */
double eval_sin2pi(int hilo, int j, double x) {
  if(hilo==1) {
    switch(j) {
    case 0: return(sin2pi0_hi(x));
    case 1: if (x<0) return(cos2pi0_hi(x)); else return(cos2pi0_lo(x));
    case 2: return(-sin2pi0_hi(x));
    case 3: if (x<0) return(-cos2pi0_hi(x)); else return(-cos2pi0_lo(x));
    }
  }
  else {
    switch(j) {
    case 0: return(sin2pi0_lo(x));
    case 1: if (x<0) return(cos2pi0_lo(x)); else return(cos2pi0_hi(x));
    case 2: return(-sin2pi0_lo(x));
    case 3: if (x<0) return(-cos2pi0_lo(x)); else return(-cos2pi0_hi(x));
    }
  }
  return 0;
}

   /* this evaluates cosine in the same fashion */
double eval_cos2pi(int hilo, int j, double x) {
  return( eval_sin2pi(hilo,(j+1)%4,x));
}


   /*
   ****************************************************************
           double eval_sin(int hilo, int j, double x)

   This returns an upper/lower bound on sin2pi(x+j*pi/2) for
   j in {0,1,2,3} and x in [-pi/2,pi/2]
   ****************************************************************
   */
double eval_sin(int hilo, int j, double x) {
  if(hilo==1) {
    switch(j) {
    case 0: return(sin0_hi(x));
    case 1: if (x<0) return(cos0_hi(x)); else return(cos0_lo(x));
    case 2: return(-sin0_hi(x));
    case 3: if (x<0) return(-cos0_hi(x)); else return(-cos0_lo(x));
    }
  }
  else {
    switch(j) {
    case 0: return(sin0_lo(x));
    case 1: if (x<0) return(cos0_lo(x)); else return(cos0_hi(x));
    case 2: return(-sin0_lo(x));
    case 3: if (x<0) return(-cos0_lo(x)); else return(-cos0_hi(x));
    }
  }
  return 0;
}

   /* this evaluates cosine in the same fashion */
double eval_cos(int hilo, int j, double x) {
  return( eval_sin(hilo,(j+1)%4,x));
}



   /*
   ****************************************************************
   int eval_asincos2pi(int offset,INTERVAL Z, INTERVAL *W, int *nonempty)

   This procedure computes W = f_j^{-1}(Z) intersected with R_s, 
   where R_0 = [-0.125,0], R_12 = [0,0.125]
   and f_j(z) = sin(z+(j+offset)/4)
   ****************************************************************
   */

int eval_asincos2pi(int offset,INTERVAL Z, INTERVAL *W, int *nonempty) {
  INTERVAL U; //,V;
  int k,j,s;
  int debug=0;
 
  if ((Z.hi < -1) || (1 < Z.lo)) return(FAIL);

  U = asin2piII(Z);   /* U in [-0.25, 0.25] */
  //V = subDII(0.25,U); /* U in [ 0.00, 0.50] = acos2piII(Z) */

  for (k=0;k<8;k++) {
    j = ((k+offset)/2)%4; s = ((k+offset)%2);

    if (s == 0)
      switch(j) {
      case 0: 
          *(W+k)=U;               /* =  inv_sin( Z) */
          nonempty[k] = (intersectionII(makeDDI(-0.125,0),W+k)); break;
      case 1: 
          *(W+k)=subIDI(U,0.25);  /* =  inv_sin(Z) - 0.25 = - inv_cos(Z) */
          nonempty[k]=  (intersectionII(makeDDI(-0.125,0),W+k)); break;
      case 2:         
          *(W+k)=negII(U);        /* =  inv_sin(Z) - 0.50 = inv_sin(-Z)*/
          nonempty[k]=(intersectionII(makeDDI(-0.125,0),W+k)); break;
      case 3:
          *(W+k)=subDII(-0.25,U); /* =  inv_sin(Z) - 0.75  = -inv_cos(-Z) */
          nonempty[k]=(intersectionII(makeDDI(-0.125,0),W+k)); break;
        }
    else
    switch(j) {
    case 0: 
        *(W+k)=U;               /* =  inv_sin(Z) */
        nonempty[k]=(intersectionII(makeDDI(0, 0.125),W+k)); break;
    case 1: 
        *(W+k)=subDII(0.25,U);  /* =  inv_sin(Z) - 0.25 = inv_cos(Z) */
        nonempty[k]=(intersectionII(makeDDI(0, 0.125),W+k)); break;
    case 2:
        *(W+k)=negII(U);        /* =  inv_sin(Z) - 0.50 = inv_sin(-Z)*/
        nonempty[k]=(intersectionII(makeDDI(0, 0.125),W+k)); break;
    case 3:
        *(W+k)=addDII(0.25,U);  /* =  inv_sin(Z) - 0.75  = inv_cos(-Z)*/
        nonempty[k]=(intersectionII(makeDDI(0, 0.125),W+k)); break;
      }


    if (debug) {
      printf("nonempty = %d, inv_trig2pi for k=%d s=%d ",nonempty[k],k,s);
      print_interval(" is ",W[k]);
    }
  }

  return(TRUE);
}


   /*
   ****************************************************************
   int eval_atancot2pi(int offset,INTERVAL Z, INTERVAL *W, int *nonempty)

   This procedure computes W = f_j^{-1}(Z) intersected with R_s, 
   where R_0 = [-0.125,0], R_12 = [0,0.125]
   and f_j(z) = tan(z+(j+offset)/4)
   ****************************************************************
   */

int eval_atancot2pi(int offset,INTERVAL Z, INTERVAL *W, int *nonempty) {
  INTERVAL U;
  int k,j,s;
  int debug=0;
 
  U = atan2piII(Z);   /* U in [-0.25, 0.25] */

  for (k=0;k<8;k++) {
    j = ((k+offset)/2)%4; s = ((k+offset)%2);

    if (s == 0)
      switch(j) {
      case 0: 
      case 2:
          *(W+k)=U;
          nonempty[k] = (intersectionII(makeDDI(-0.125,0),W+k)); break;
      case 1: 
      case 3:
          *(W+k)=subIDI(U,0.25);
          nonempty[k]=  (intersectionII(makeDDI(-0.125,0),W+k)); break;
        }
    else
    switch(j) {
    case 0: 
    case 2:
        *(W+k)=U;           
        nonempty[k]=(intersectionII(makeDDI(0, 0.125),W+k)); break;
    case 1: 
    case 3:
        *(W+k)=addIDI(U,0.25);
        nonempty[k]=(intersectionII(makeDDI(0, 0.125),W+k)); break;
      }

    if (debug) {
      printf("nonempty = %d, inv_trig2pi for k=%d s=%d ",nonempty[k],k,s);
      print_interval(" is ",W[k]);
    }
  }

  return(TRUE);
}


int eval_asin2pi(INTERVAL Z, INTERVAL *W, int *nonempty) {
    return(eval_asincos2pi(0,Z, W, nonempty));
}
int eval_acos2pi(INTERVAL Z, INTERVAL *W, int *nonempty) {
    return(eval_asincos2pi(2,Z, W, nonempty));
}
int eval_atan2pi(INTERVAL Z, INTERVAL *W, int *nonempty) {
    return(eval_atancot2pi(0,Z, W, nonempty));
}


   /*
   ****************************************************************
   int eval_asincos(int offset,INTERVAL Z, INTERVAL *W, int *nonempty)

   This procedure computes W = f_j^{-1}(Z) intersected with R_s, 
   where R_0 = [-pi/2,0], R_12 = [0,pi/2]
   and f_j(z) = sin(z+(j+offset)*pi/2)
   ****************************************************************
   */

int eval_asincos(int offset,INTERVAL Z, INTERVAL *W, int *nonempty) {
  INTERVAL U;
  int k,j,s;
  int debug=0;
 
  if ((Z.hi < -1) || (1 < Z.lo)) return(FAIL);

  U = asinII(Z);   /* U in [-0.25, 0.25]2pi */

  for (k=0;k<8;k++) {
    j = ((k+offset)/2)%4; s = ((k+offset)%2);

    if (s == 0)
      switch(j) {
      case 0: 
          *(W+k)=U;               /* =  inv_sin( Z) */
          nonempty[k] = (intersectionII(makeDDI(-HALFPI.i.hi/2,0),W+k)); break;
      case 1: 
          *(W+k)=subIII(U,HALFPI.i);  /* =  inv_sin(Z) - 0.25 = - inv_cos(Z) */
          nonempty[k]=  (intersectionII(makeDDI(-HALFPI.i.hi/2,0),W+k)); break;
      case 2:         
          *(W+k)=negII(U);        /* =  inv_sin(Z) - 0.50 = inv_sin(-Z)*/
          nonempty[k]=(intersectionII(makeDDI(-HALFPI.i.hi/2,0),W+k)); break;
      case 3:
          *(W+k)=subIII(negII(HALFPI.i),U); /* =  inv_sin(Z) - 0.75  = -inv_cos(-Z) */
          nonempty[k]=(intersectionII(makeDDI(-HALFPI.i.hi/2,0),W+k)); break;
        }
    else
    switch(j) {
    case 0: 
        *(W+k)=U;               /* =  inv_sin(Z) */
        nonempty[k]=(intersectionII(makeDDI(0, HALFPI.i.hi/2),W+k)); break;
    case 1: 
        *(W+k)=subIII(HALFPI.i,U);  /* =  inv_sin(Z) - 0.25 = inv_cos(Z) */
        nonempty[k]=(intersectionII(makeDDI(0, HALFPI.i.hi/2),W+k)); break;
    case 2:
        *(W+k)=negII(U);        /* =  inv_sin(Z) - 0.50 = inv_sin(-Z)*/
        nonempty[k]=(intersectionII(makeDDI(0, HALFPI.i.hi/2),W+k)); break;
    case 3:
        *(W+k)=addIII(HALFPI.i,U);  /* =  inv_sin(Z) - 0.75  = inv_cos(-Z)*/
        nonempty[k]=(intersectionII(makeDDI(0, HALFPI.i.hi/2),W+k)); break;
      }


    if (debug) {
      printf("nonempty = %d, inv_trig2pi for k=%d s=%d ",nonempty[k],k,s);
      print_interval(" is ",W[k]);
    }
  }

  return(TRUE);
}



   /*
   ****************************************************************
   int eval_atancot(int offset,INTERVAL Z, INTERVAL *W, int *nonempty)

   This procedure computes W = f_j^{-1}(Z) intersected with R_s, 
   where R_0 = [-pi/2,0], R_12 = [0,pi/2]
   and f_j(z) = tan(z+(j+offset)*pi/2)
   ****************************************************************
   */

int eval_atancot(int offset,INTERVAL Z, INTERVAL *W, int *nonempty) {
  INTERVAL U;
  int k,j,s;
  int debug=0;
 
  U = atanII(Z);            /* in [-pi/2,pi/2] */

  for (k=0;k<8;k++) {
    j = ((k+offset)/2)%4; s = ((k+offset)%2);

    if (s == 0)
      switch(j) {
      case 0: 
      case 2:
          *(W+k)=U;               
          nonempty[k] = (intersectionII(makeDDI(-HALFPI.i.hi/2,0),W+k)); break;
      case 1: 
      case 3:
          *(W+k)=subIII(U,HALFPI.i); 
          nonempty[k]=  (intersectionII(makeDDI(-HALFPI.i.hi/2,0),W+k)); break;
        }
    else
    switch(j) {
    case 0: 
    case 2:
        *(W+k)=U;
        nonempty[k]=(intersectionII(makeDDI(0, HALFPI.i.hi/2),W+k)); break;
    case 1: 
    case 3:
        *(W+k)=addIII(HALFPI.i,U);
        nonempty[k]=(intersectionII(makeDDI(0, HALFPI.i.hi/2),W+k)); break;
      }

    if (debug) {
      printf("nonempty = %d, inv_trig2pi for k=%d s=%d ",nonempty[k],k,s);
      print_interval(" is ",W[k]);
    }
  }

  return(TRUE);
}




int eval_asin(INTERVAL Z, INTERVAL *W, int *nonempty) {
    return(eval_asincos(0,Z, W, nonempty));
}
int eval_acos(INTERVAL Z, INTERVAL *W, int *nonempty) {
    return(eval_asincos(2,Z, W, nonempty));
}
int eval_atan(INTERVAL Z, INTERVAL *W, int *nonempty) {
  return(eval_atancot(0,Z,W,nonempty));
}



   /* 
   ****************************************************************
   Interval extension of the trig functions
   ****************************************************************
   */ 



INTERVAL sin2piII(INTERVAL x) {
  return(periodicfnII(x,reduce_trig2pi,range_sin,eval_sin2pi,makeDDI(-1,1),4));
}


INTERVAL cos2piII(INTERVAL x) {
  return(periodicfnII(x,reduce_trig2pi,range_cos,eval_cos2pi,makeDDI(-1,1),4));
}


INTERVAL tan2piII(INTERVAL x) {
  return(divIII(sin2piII(x),cos2piII(x)));  /* we should implement this directly */
}


INTERVAL sinII(INTERVAL x) {
  return(periodicfnII(x,reduce_trig,range_sin,eval_sin,makeDDI(-1,1),4));
}


INTERVAL cosII(INTERVAL x) {
  return(periodicfnII(x,reduce_trig,range_cos,eval_cos,makeDDI(-1,1),4));
}


INTERVAL tanII(INTERVAL x) {
  return(divIII(sinII(x),cosII(x)));
}







/*
        int intersect_inv_periodicfnII(          
                 INTERVAL Z, 
                 INTERVAL *X, 
                      int (*reduce)(), 
                      int (*inv_f)(), 
                   double (*translate)(),
                 INTERVAL Y,
                      int K
        )
        


  This is a general procedure for soundly and precisely
  computing the smallest   interval containing 
  the intersection of X and inverse(f)(Z)
  for a periodic function f which is represented
  as described in the periodicfnII procedure comments.
  The other arguments are

  int INTERVAL inv_f(INTERVAL Z, INTERVAL *W,int *empty)
     this computes the array W of Intervals where
       W[s+2*J] = intersect(K_j,inv_f(Z),R_s)-a_j
     where K_j+a_j is the jth overlapping subinterval
     centered at a_j (for j=0,1,2,...), and
     R_1 = [0,inf], R_0 = [-inf,0]. Thus, we divide
     the fundamental domain into 2K translations of
     subintervals I_b (centered at zero) and we compute 
     the inverse of  f_(2j+u)(x) = f(x + a_j) on I_(2j+u)

     The reason we compute such a complicated gadget is that
     we want our results to be relatively small offsets 
     from the critical points a_j and we want to reduce to
     the case of computing the inverse of the function on 
     subintervals where it is monotonic.

     We also need to apply the inverse operation to 
     range reduction, using the following procedure:

  int reduce(int hilo, double x, double *N, int *J, double *Z, INTERVAL *W)
     if (hilo=LO) this returns N,J,Z such that
         Z + a_J + N*P < x
g     where P is the period of the function, if hilo=HI
     it returns an Z,J,N such that  x < Z + a_J + N*P


  double translate(int hilo,double N, int J, double W)
     if (hilo==LO) this returns a double x such that
         x < W + a_J + N*P
     and if (hilo=HI) then we have W + a_J + N*P < x


  double Y. This is the image of the function

  int K. this is the number of points a_J

*/


int intersect_inv_periodicfnII(          
         INTERVAL Z, 
         INTERVAL *X, 
              int (*reduce)(), 
              int (*inv_f)(), 
           double (*translate)(),
         INTERVAL Y,
              int K
) {
  int i,j,k; //,s;
  int nonempty[32];
  INTERVAL W[32];
  double tmp,Nlo,Nhi,XXlo,XXhi;
  int Jlo,Jhi;

  if ((Y.hi < Z.lo) || (Z.hi < Y.lo))
    return(FAIL);

  /* compute inv_f */
  if (!inv_f(Z,W,nonempty))
    return(FAIL);
   

  /* narrow X->lo */
  if (reduce(0, X->lo, &Nlo, &Jlo, &XXlo)) {
     for(j=0;j<2*K;j++) {
        i = j/2; /*s = j % 2*/; k = (j + 2*Jlo)%(2*K); 
        if (nonempty[k]) {  /* (inv_f((i+Jlo)%K,s,Z,&W)) {*/
           if (X->lo < (tmp = translate(0,Nlo,Jlo+i,W[k].lo))) 
              {X->lo = tmp;  break;}
           else if (X->lo <= (tmp = translate(1,Nlo,Jlo+i,W[k].hi))) {
               break;}
	 }
     }
     if (X->lo > X->hi) return(FAIL);
     }

  /* narrow X->hi */
  if (reduce(1, X->hi, &Nhi, &Jhi, &XXhi)) {
     for(j=0;j<2*K;j++) {
        i=j/2; /*s= 1-(j%2)*/; k = (2*Jhi+1-j+2*K)%(2*K); 
        if (nonempty[k]) {  /* inv_f((Jhi+K-i)%K,s,Z,&W)) {*/
           if (X->hi > (tmp = translate(1,Nhi,Jhi-i,W[k].hi))) 
              {X->hi = tmp;   break;}
           else if (X->hi >= (tmp = translate(0,Nhi,Jhi-i,W[k].lo))) {
               break;}
	 }
     }

     if (X->lo > X->hi) return(FAIL);
   }

  return(TRUE);
}



   /* 
   ****************************************************************
   Interval extension of the arc-trig functions
   Since these arc-trig functions are monotone, we can 
   compute their interval extensions simply by soundly
   evaluating them at the endpoints of the interval.
   ****************************************************************
   */ 



INTERVAL asin2piII(INTERVAL X){
  INTERVAL S,T;
  T = intersectIII(X,makeDDI(-1,1));
  S.lo = asin2pi_lo(T.lo);
  S.hi = asin2pi_hi(T.hi);
  return(S);
}

INTERVAL acos2piII(INTERVAL X){
  INTERVAL S,T;
  T = intersectIII(X,makeDDI(-1,1));
  S.lo = acos2pi_lo(T.hi);
  S.hi = acos2pi_hi(T.lo);
  return(S);
}

INTERVAL atan2piII(INTERVAL X){
  INTERVAL S;
  S.lo = atan2pi_lo(X.lo);
  S.hi = atan2pi_hi(X.hi);
  return(S);
}


INTERVAL asinII(INTERVAL X)
{
  INTERVAL S,T;
  T = intersectIII(X,makeDDI(-1,1));
  S.lo = asin_lo(T.lo);
  S.hi = asin_hi(T.hi);
  return(S);
}


INTERVAL acosII(INTERVAL X)
{
  INTERVAL S,T;
  T = intersectIII(X,makeDDI(-1,1));
  S.lo = acos_lo(T.hi);
  S.hi = acos_hi(T.lo);
  return(S);
}


INTERVAL atanII(INTERVAL X)
{
  INTERVAL S;
  S.lo = atan_lo(X.lo);
  S.hi = atan_hi(X.hi);
  return(S);
}






/* ****************************************************************
      Semi-narrowing operators:

   ************************************************************** */
int intersectionII(INTERVAL a,  INTERVAL *x)
{
  if (a.lo > x->lo) x->lo = a.lo;
  if (a.hi < x->hi) x->hi = a.hi;
  return(x->lo <= x->hi);
}


int intersect_sin2piII(INTERVAL x, INTERVAL *z) 
{ INTERVAL y;
  y = sin2piII(x);
  return(intersectionII(y, z));
}


int intersect_cos2piII(INTERVAL x, INTERVAL *z) 
{ INTERVAL y;
  y = cos2piII(x);
  return(intersectionII(y, z));
}


int intersect_tan2piII(INTERVAL x, INTERVAL *z) 
{ INTERVAL y;
  y = tan2piII(x);
  return(intersectionII(y, z));
}



int intersect_inv_sin2piII(INTERVAL Z, INTERVAL *X) {
   return(
     intersect_inv_periodicfnII(Z,X,
        reduce_trig2pi,
        eval_asin2pi,
        translate_trig2pi,
        makeDDI(-1,1),
        4)
   );
} 

int intersect_inv_cos2piII(INTERVAL Z, INTERVAL *X) {
   return(
     intersect_inv_periodicfnII(Z,X,
        reduce_trig2pi,
        eval_acos2pi,
        translate_trig2pi,
        makeDDI(-1,1),
        4)
   );
} 

int intersect_inv_tan2piII(INTERVAL z, INTERVAL *x) {
   return(
     intersect_inv_periodicfnII(z,x,
        reduce_trig2pi,
        eval_atan2pi,
        translate_trig2pi,
        PN_INF.i,
        4)
   );

}



int intersect_sinII(INTERVAL x, INTERVAL *z)
{ INTERVAL y;
  y = sinII(x);
  return(intersectionII(y, z));
}

int intersect_cosII(INTERVAL x, INTERVAL *z)
{ INTERVAL y;
  y = cosII(x);
  return(intersectionII(y, z));
}

int intersect_tanII(INTERVAL x, INTERVAL *z)
{ INTERVAL y;
  y = tanII(x);
  return(intersectionII(y, z));
}



int intersect_inv_sinII(INTERVAL Z, INTERVAL *X) {
   return(
     intersect_inv_periodicfnII(Z,X,
        reduce_trig,
        eval_asin,
        translate_trig,
        makeDDI(-1,1),
        4)
   );
} 


int intersect_inv_cosII(INTERVAL z, INTERVAL *x) {
   return(
     intersect_inv_periodicfnII(z,x,
        reduce_trig,
        eval_acos,
        translate_trig,
        makeDDI(-1,1),
        4)
   );
}

int intersect_inv_tanII(INTERVAL z, INTERVAL *x) {
   return(
     intersect_inv_periodicfnII(z,x,
        reduce_trig,
        eval_atan,
        translate_trig,
        PN_INF.i,
        4)
   );

}





/* ****************************************************************
   Narrowing Procedures for the trigonometric functions

   ************************************************************** */
int narrow_sin2pi(INTERVAL *x, INTERVAL *z)   /* sin(2pi*x)=z */
{  return(intersect_sin2piII(*x, z) && intersect_inv_sin2piII(*z, x)); }


int narrow_cos2pi(INTERVAL *x, INTERVAL *z)   /* sin(2pi*x)=z */
{  return(intersect_cos2piII(*x, z) && intersect_inv_cos2piII(*z, x)); }


int narrow_tan2pi(INTERVAL *x, INTERVAL *z)   /* tan(2pi*x)=z */
{  return(intersect_tan2piII(*x, z) && intersect_inv_tan2piII(*z, x)); }






int narrow_sin(INTERVAL *x, INTERVAL *z)   /* sin(x)=z */
{  return(intersect_sinII(*x, z) && intersect_inv_sinII(*z, x)); }


int narrow_cos(INTERVAL *x, INTERVAL *z)     /* cos(x)=z */
{  return(intersect_cosII(*x, z) && intersect_inv_cosII(*z, x)); }


int narrow_tan(INTERVAL *x, INTERVAL *z)     /* tan(x)=z */
{  return(intersect_tanII(*x, z) && intersect_inv_tanII(*z, x)); }

/* **************************************************************** */





/* ****************************************************************
   Narrowing Procedures for the inverse trigonometric functions

   ************************************************************** */

int narrow_asin2pi(INTERVAL *x,INTERVAL *y){ /* asin(2pi*x)=z */

  int err;
  INTERVAL clip;
  INTERVAL X,Y;

  clip.lo = -.25;
  clip.hi =  .25;
  
  Y = intersectIII(*y,clip); // clip Y to ran(asin) first

  // x and dom(asin) disjoint? => no narrowing
  if (x->hi < -1. || x->lo > 1.)
    return 1;
  
  if (x->lo < -1. && x->hi > 1.)
    return 1;

  // x includes values >1.0 (not defined for asin)
  if (x->hi > 1.){
    X.hi = 1.;
    X.lo = x->lo;
    
    if (intersect_sin2piII(Y,&X))
      x->lo = X.lo;
    else
      x->lo = 1.0;
    
    return 1;
  }

  // x includes values < -1.0 (not defined for asin)
  if (x->lo < -1.){
    X.lo = -1.;
    X.hi = x->hi;
    
    if (intersect_sin2piII(Y,&X))
      x->hi = X.hi;
    else
      x->hi = -1.0;
    
    return 1;
  }

  // asin defined for all values in x, i.e. x->lo >= -1.0 and x-> hi <= 1.0
  X = *x;

  // all values in X are defined and Y is clipped => default narrowing
  err = intersect_inv_sin2piII(X,&Y) && intersect_sin2piII(Y,&X);
  
  if (!err) // X or Y is the empty set
    return 0;
  else { // both X and Y are not the empty set
    *y=Y;
    *x=X;
    return 1;
  }
}


int narrow_acos2pi(INTERVAL *x,INTERVAL *y){ /* acos(2pi*x)=z */

  int err;
  INTERVAL clip;
  INTERVAL X,Y;

  clip.lo = 0.0;
  clip.hi = 0.5;

  Y=intersectIII(clip,*y); // clip Y to ran(acos)

  // x and dom(acos) disjoint? => no narrowing
  if (x->hi < -1. || x->lo > 1.)
    return 1;
  
  if (x->lo < -1. && x->hi > 1.)
    return 1;

  // x includes values >1.0 (not defined for acos)
  if (x->hi > 1.){
    X.hi = 1.;
    X.lo = x->lo;

    if (intersect_cos2piII(Y,&X))
      x->lo = X.lo;
    else
      x->lo = 1.0;
    
    return 1;
  }

  // x includes values <-1.0 (not defined for acos)
  if (x->lo < -1.){
    X.lo = -1.;
    X.hi = x->hi;

    if (intersect_cos2piII(Y,&X))
      x->hi = X.hi;
    else
      x->hi = -1.0;
    
    return 1;
  }

  // acos defined for all values in x, i.e. x->lo >= -1.0 and x-> hi <= 1.0
  X = *x;

  // all values in X are defined and Y is clipped => default narrowing
  err = intersect_inv_cos2piII(X,&Y) && intersect_cos2piII(Y,&X);
  
  if (!err) // X or Y is the empty set
    return 0;
  else { // both X and Y are not the empty set
    *y=Y;
    *x=X;
    return 1;
  }

}



int narrow_atan2pi(INTERVAL *x,INTERVAL *y){ /* atan(2pi*x)=z */

  //int err;
  //double buffer;
  INTERVAL clip,Y;
  clip.lo = -.25;
  clip.hi = .25;

  Y = intersectIII(*y,clip); // clipping Y to ran(atan)
  
  if (Y.lo > Y.hi)
    return 0; // Y and ran(atan) disjoint => no solution
  else { // default narrowing with clipped y
    *y=Y;
    return intersect_inv_tan2piII(*x,y)
      && intersect_tan2piII(*y,x);
  }
  
}



int narrow_asin(INTERVAL *x,INTERVAL *y){   /* asin(x)=z */

  int err;
  INTERVAL clip;
  INTERVAL X,Y;

  clip.lo = -HALFPI_HI.d;
  clip.hi =  HALFPI_HI.d;
  
  Y = intersectIII(*y,clip); // clip Y to ran(asin) first

  // x and dom(asin) disjoint? => no narrowing
  if (x->hi < -1. || x->lo > 1.)
    return 1;
  
  if (x->lo < -1. && x->hi > 1.)
    return 1;

  // x includes values >1.0 (not defined for asin)
  if (x->hi > 1.){
    X.hi = 1.;
    X.lo = x->lo;
    
    if (intersect_sinII(Y,&X))
      x->lo = X.lo;
    else
      x->lo = 1.0;
    
    return 1;
  }

  // x includes values <-1.0 (not defined for asin)
  if (x->lo < -1.){
    X.lo = -1.;
    X.hi = x->hi;
    
    if (intersect_sinII(Y,&X))
      x->hi = X.hi;
    else
      x->hi = -1.0;
    
    return 1;
  }

  // asin defined for all values in x, i.e. x->lo >= -1.0 and x-> hi <= 1.0
  X = *x;

  // all values in X are defined and Y is clipped => default narrowing
  err = intersect_inv_sinII(X,&Y) && intersect_sinII(Y,&X);
  
  if (!err) // X or Y is the empty set
    return 0;
  else { // both X and Y are not the empty set
    *y=Y;
    *x=X;
    return 1;
  }
}



int narrow_acos(INTERVAL *x,INTERVAL *y){   /* acos(x)=z */

  int err;
  INTERVAL clip;
  INTERVAL X,Y;

  clip.lo = 0.0;
  clip.hi = PI_HI.d;

  Y=intersectIII(clip,*y); // clip Y to ran(acos)

  // x and dom(acos) disjoint? => no narrowing
  if (x->hi < -1. || x->lo > 1.)
    return 1;
  
  if (x->lo < -1. && x->hi > 1.)
    return 1;

  // x includes values >1.0 (not defined for acos)
  if (x->hi > 1.){
    X.hi = 1.;
    X.lo = x->lo;

    if (intersect_cosII(Y,&X))
      x->lo = X.lo;
    else
      x->lo = 1.0;
    
    return 1;
  }

  // x includes values <-1.0 (not defined for acos)
  if (x->lo < -1.){
    X.lo = -1.;
    X.hi = x->hi;

    if (intersect_cosII(Y,&X))
      x->hi = X.hi;
    else
      x->hi = -1.0;
    
    return 1;
  }

  // acos defined for all values in x, i.e. x->lo >= -1.0 and x-> hi <= 1.0
  X = *x;

  // all values in X are defined and Y is clipped => default narrowing
  err = intersect_inv_cosII(X,&Y) && intersect_cosII(Y,&X);
  
  if (!err) // X or Y is the empty set
    return 0;
  else { // both X and Y are not the empty set
    *y=Y;
    *x=X;
    return 1;
  }

}



int narrow_atan(INTERVAL *x,INTERVAL *y){  /* atan(x)=z */

  //int err;
  //double buffer;
  INTERVAL clip,Y;
  clip.lo = -HALFPI_HI.d;
  clip.hi = HALFPI_HI.d;

  Y = intersectIII(*y,clip); // clipping Y to ran(atan)
  
  if (Y.lo > Y.hi)
    return 0; // Y and ran(atan) disjoint => no solution
  else { // default narrowing with clipped y
    *y=Y;
    return intersect_inv_tanII(*x,y)
      && intersect_tanII(*y,x);
  }

}


/* **************************************************************** */
