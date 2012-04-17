/****************************************************************
             narrow_mult.c


   This uses Qun Ju's Implementation of narrowing for multiplication
   (see her dissertation, 1998, Brandeis U., CS Dept.)

   2000 Copyright Timothy J. Hickey
   2003 Copyright David K. Wittenberg
   licence: http://interval.sourceforge.net/licence.txt, zlib/png 

 ****************************************************************/

/*            CHANGELOG

2003-03-11  changed __BYTE_ORDER to BYTE_ORDER for portability      dkw
2003-12-12  removed includes duplicated by smath                    dkw

*/

            
/*  if narrowing fails, return(FAIL) else return(TRUE)  */



#include "smath.h"



#if BYTE_ORDER == BIG_ENDIAN
static NEWDOUBLE
    ZERO          = {{0x0000, 0x0000, 0x0000, 0x0000}},
    NEG_ZERO      = {{0x8000, 0x0000, 0x0000, 0x0000}},
    POS_INF       = {{0x7ff0, 0x0000, 0x0000, 0x0000}},
    NEG_INF       = {{0xfff0, 0x0000, 0x0000, 0x0000}};
#endif

#if BYTE_ORDER == LITTLE_ENDIAN
static NEWDOUBLE
    ZERO          = {{0x0000,0x0000, 0x0000, 0x0000 }},
    NEG_ZERO      = {{0x0000,0x0000, 0x0000, 0x8000 }},
    POS_INF       = {{0x0000,0x0000, 0x0000, 0x7ff0 }},
    NEG_INF       = {{0x0000,0x0000, 0x0000, 0xfff0 }};
#endif

extern int FAIL_FLAG;

int interval_mul(double x1, double x2, double y1, double y2, double* low, double* high);
int interval_div(double x1, double x2, double y1, double y2, double* low, double* high);
int interval_div4(double x1, double x2, double y1, double y2, double* low, double* high);
int interval_mul4(double x1, double x2, double y1, double y2, double* low, double* high);

/* ****************************************************************
          intersection(x1,x2, y)
		intersect *y with [x1,x2]
   ************************************************************** */


static int intersection(double x1,double x2, INTERVAL *y)
{
    if (x1 > y->lo)   y->lo = x1; 
    if (x2 < y->hi)   y->hi = x2; 
    return(y->lo <= y->hi);
}


int intersect_mulIII(INTERVAL a, INTERVAL b, INTERVAL *x) {
    /* this implements the constraint "*x in a/b" */
    double  tmp1, tmp2;

    interval_mul(a.lo, a.hi, b.lo, b.hi, &tmp1, &tmp2);
    return(intersection(tmp1,tmp2, x));
}


int intersect_divIII(INTERVAL z, INTERVAL x, INTERVAL *y) {
    /* This implements the constraint: "*y in z/x" */
    double  tmp1, tmp2;

    if (!interval_div(z.lo,z.hi, x.lo, x.hi, &tmp1,&tmp2)) /*two interval*/
      { if ((y->lo>tmp1) && (y->hi<tmp2))   
          {y->lo = POS_INF.d; y->hi = NEG_INF.d;return(FAIL);}
        else if ((y->lo>tmp1) || ((y->lo==tmp1)&&(tmp1==0.0)))
          intersection(tmp2, POS_INF.d, y);
        else if ((y->hi<tmp2) || ((y->hi==tmp2)&&(tmp2==0.0)))
          intersection(NEG_INF.d, tmp1, y);
      }
    else 
      { if (!intersection(tmp1,tmp2, y)) return(FAIL);
      }
   return(TRUE);
}

/* The case like [1,4]/[0,0] case won't appear */
int narrow_mul(INTERVAL *x,INTERVAL *y, INTERVAL *z)
{
    double  tmp1, tmp2;


    interval_mul(x->lo, x->hi, y->lo, y->hi, &tmp1, &tmp2);
    if (!intersection(tmp1,tmp2, z)) return(FAIL);


               /* Even if x properly contains 0,  we still have a chance
                  to narrow y without splitting */
    if (!interval_div(z->lo,z->hi, x->lo, x->hi, &tmp1,&tmp2)) /*two interval*/
      { if ((y->lo>tmp1) && (y->hi<tmp2))  return(FAIL);
        if ((y->lo>tmp1) || ((y->lo==tmp1)&&(tmp1==0.0)))
          intersection(tmp2, POS_INF.d, y);
        else if ((y->hi<tmp2) || ((y->hi==tmp2)&&(tmp2==0.0)))
          intersection(NEG_INF.d, tmp1, y);
      }
    else { if (!intersection(tmp1,tmp2, y)) return(FAIL); }



                /* after y has passed the narrowing, y stil proper
                   contains 0;  then we know there is no need to narrow x */
    if ((y->lo>=0.0) || (y->hi<=0.0))    
      { interval_div(z->lo, z->hi, y->lo, y->hi, &tmp1, &tmp2);
        if (!intersection(tmp1,tmp2,x)) return(FAIL);
      }

    return(TRUE);
}


int interval_mul(x1,x2, y1,y2,low, high)
double  x1, x2, y1, y2, *low, *high;
{   return(interval_mul4(x1,x2, y1,y2,low, high)); }

int interval_div(x1,x2, y1,y2,low, high)
double  x1, x2, y1, y2, *low, *high;
{   return(interval_div4(x1,x2, y1,y2,low, high)); }



/***************************************************************************
  ASSUMPTION: 0*inf = 0 or nothing (see the notes in the code)

  IDEA :   try to make only one call to mul_lohi(), div_lohi().

***************************************************************************/
int interval_mul4(x1,x2, y1, y2, low, high)
double     x1, x2, y1, y2, *low, *high;
{                                        
    double    lo, hi;

 /*** If we don't define: 0*inf=0, then we need add the following code: ***/
    if (((x1==0.0) && (x2==0.0)) || ((y1==0.0) && (y2==0.0)))
      { *low = 0.0; *high = 0.0;  return 0; }


    if (x1>=0.0) {
       if (y1>=0.0)                                              /* + + */
          { *low=mul_lo(x1, y1);   *high=mul_hi(x2,y2); return 0; }
       if (y2<=0.0)                                              /* + - */
          { *low=mul_lo(x2, y1);   *high=mul_hi(x1,y2); return 0; }
       *low=mul_lo(x2, y1);   *high=mul_hi(x2,y2); return 0;       /* + s */
     }

    if (x2<=0.0) {
       if (y1>=0.0)                                              /* - + */
          { *low=mul_lo(x1, y2);   *high=mul_hi(x2,y1); return 0; }
       if (y2<=0.0)                                              /* - - */
          { *low=mul_lo(x2, y2);   *high=mul_hi(x1,y1); return 0; }
       *low=mul_lo(x1, y2);   *high=mul_hi(x1,y1); return 0;       /* - s */
      }

    if (y1>=0.0)                                                 /* s + */
      { *low=mul_lo(x1, y2);   *high=mul_hi(x2,y2); return 0; }
    if (y2<=0.0)                                                 /* s + */
      { *low=mul_lo(x2, y1);   *high=mul_hi(x1,y1); return 0; }
   
    *low = mul_lo(x2,y1); lo = mul_lo(x1,y2);                    /* s s */
    *high = mul_hi(x1,y1); hi = mul_hi(x2,y2);
    if (lo<*low)   *low=lo;
    if (hi>*high)  *high=hi;
    return 0;
}
 

int interval_div4(x1,x2, y1, y2, low, high)
double     x1, x2, y1, y2, *low, *high;
{                                        
    //double    lo, hi;

    if ((y1<0.0) && (y2>0.0))  /* return two split points  for +s, -s  */
      { if (x1>0.0)                                              /* + s */
            { *low = div_hi(x1,y1);  *high = div_lo(x1,y2);  return(0); }
        if (x2<0.0)                                              /* - s */
            { *low = div_hi(x2,y2);  *high = div_lo(x2,y1);  return(0); }
        *low = NEG_INF.d;  *high = POS_INF.d;  return(1);            /* s s */
      }       

/*** 0/0 case (more than "s s" case). It concerns the following cases 
     to avoid div_lohi(0,0) appear:
     [0,0]/[0,0],  [0,0]/[0,a], [0,0]/[a,0], [0,a]/[0,0], [a,0]/[0,0] 
***/
    if (((x1<=0.0) && (x2>=0.0)) && ((y1<=0.0) && (y2>=0.0)))
        { *low = NEG_INF.d;  *high = POS_INF.d;  return(1);}

    if (y1==0.0) y1=ZERO.d;       /* 1/+0 = +inf,   1/-0 = -inf */
    if (y2==0.0) y2=NEG_ZERO.d;

    if  (x1>=0.0)  {
      if (y1>=0.0)                                               /* + + */
        { *low=div_lo(x1, y2);   *high=div_hi(x2,y1);  return(1); }; 
      *low=div_lo(x2, y2);   *high=div_hi(x1,y1);  return(1);    /* + - */
     }

    if  (x2<=0.0)  {
      if (y1>=0.0)                                               /* - + */
       { *low=div_lo(x1, y1);   *high=div_hi(x2,y2);  return(1); }; 
      *low=div_lo(x2, y1);   *high=div_hi(x1,y2);  return(1);    /* - - */
     } 

    if (y1>=0.0)                                                 /* s + */
       { *low=div_lo(x1, y1);  *high=div_hi(x2,y1);  return(1); }
    *low = div_lo(x2, y2);  *high = div_hi(x1,y2);  return(1);   /* s - */
}

