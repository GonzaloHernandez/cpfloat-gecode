
/* ****************************************************************
   ****************************************************************

                        narrow.c

   2000 Copyright Timothy J. Hickey
   2003 Copyright David K. Wittenberg
   licence: http://interval.sourceforge.net/licence.txt, zlib/png 


         NARROWING ROUTINES FOR INTERVAL ARITHMETIC

   ****************************************************************
   **************************************************************** */

/*            CHANGELOG

2003-03-11  changed __BYTE_ORDER to BYTE_ORDER for portability      dkw
2003-11-14  fixed squareII to solve dependence problem              dkw
2003-12-12  removed some includes which smath already did           dkw
2004-04-20  fixed bug in squareII                                   dkw

*/


#include "smath.h"


//#if BYTE_ORDER == BIG_ENDIAN
//static NEWDOUBLE
    //POS_INF       = {{0x7ff0, 0x0000, 0x0000, 0x0000}},
    //NEG_INF       = {{0xfff0, 0x0000, 0x0000, 0x0000}};
//#endif


//#if BYTE_ORDER == LITTLE_ENDIAN
//static NEWDOUBLE
    //POS_INF       = {{0x0000,0x0000, 0x0000, 0x7ff0 }},
    //NEG_INF       = {{0x0000,0x0000, 0x0000, 0xfff0 }};
//#endif

int FAIL_FLAG=0;

#define MAX(x,y) (((x)>(y))?(x):(y))
#define MIN(x,y) (((x)<(y))?(x):(y))

/****************************************************************/

int narrow_le(INTERVAL *x,INTERVAL *z)
{
	//double vh,vl,xl,xh,zl,zh;

  if (x->hi <= z->lo) return(TRUE);
  if (z->hi < x->lo) return(FAIL);
  if (x->hi > z->hi) x->hi = z->hi;
  if (z->lo < x->lo) z->lo = x->lo;
  return(TRUE);
}

/****************************************************************/

int narrow_lt(INTERVAL *x,INTERVAL *z)
/* this is not optimal if x and/or z are integer arguments */
{
	//double vh,vl,xl,xh,zl,zh;

  if (x->hi < z->lo) return(TRUE);
  if (z->hi <= x->lo) return(FAIL);

  if (x->hi > z->hi) x->hi = z->hi;
  if (z->lo < x->lo) z->lo = x->lo;
  return(TRUE);

}

/****************************************************************/


int narrow_ne(INTERVAL *x, INTERVAL *z)
/* this is not optimal if x and/or z are integer arguments */
{
	//double vh,vl,xl,xh,zl,zh;

  return(!((x->hi == x->lo) && 
           (z->lo == x->lo) && 
           (z->hi == z->lo)));

}


/****************************************************************/

int narrow_eq(INTERVAL *x, INTERVAL *z)
{
	//double vh,vl,xl,xh,zl,zh;

  if (x->hi > z->hi) x->hi = z->hi;
  else if (x->hi < z->hi) z->hi = x->hi;

  if (z->lo < x->lo) z->lo = x->lo;
  else if (z->lo > x->lo) x->lo = z->lo;

  return((x->lo <= x->hi));
}

int intersect_eqII(INTERVAL a,  INTERVAL *x)
{
  if (a.lo > x->lo) x->lo = a.lo;
  if (a.hi < x->hi) x->hi = a.hi;
  return( x->lo <= x->hi);
}



/****************************************************************/

int narrow_abs(INTERVAL *x, INTERVAL *z)
{
  INTERVAL y,t;
  int result;
  double v;

  if (z->lo < 0) z->lo = 0;

  if (x->lo >= 0) return(narrow_eq(x,z));

  if (x->hi <= 0) {
    y.lo = -x->hi; y.hi = -x->lo;
    result = narrow_eq(&y,z); 
    t.lo = -y.hi; t.hi = -y.lo;
    if (x->lo < t.lo) x->lo = t.lo;
    if (x->hi > t.hi) x->hi = t.hi;
    return((result&&(x->lo <= x->hi)));}

  if (x->hi > -(x->lo)) v = x->hi; else v = -(x->lo);

  if (z->hi >= v) {z->hi = v;}

  if (x->hi > z->hi) x->hi = z->hi;
  if (-(x->lo) > z->hi)  x->lo = -(z->hi);

  return((x->lo <= x->hi) && (z->lo <= z->hi));
}

INTERVAL absII(INTERVAL x) {
  INTERVAL y;

  if (x.lo > 0) {y.lo = x.lo; y.hi = x.hi; return(y);}

  if (x.hi < 0) {y.lo = -x.hi; y.hi = -x.lo; return(y);}

  y.lo = 0;
  y.hi = (x.hi > -x.lo)? x.hi: -x.lo;
  return(y);
}

int intersect_abs(INTERVAL x, INTERVAL *z) {
  return(intersect_eqII(absII(x),z));
}

int intersect_inv_abs(INTERVAL z, INTERVAL *xp)
/* this computes *x = *x intersect (z union -z) */
{
  INTERVAL y;

  if (z.lo <= 0) {
    y.hi = (z.hi> -z.lo)?(z.hi):(-z.lo);
    y.lo = -y.hi; 
    return(intersect_eqII(y,xp)); }


       if (xp->lo < -z.hi) xp->lo = -z.hi;
  else if (xp->lo <= -z.lo)             ;
  else if (xp->lo < z.lo)  xp->lo =  z.lo;
  else if (xp->lo <= z.hi)              ;
  else return(FAIL);

       if (xp->hi >   z.hi) xp->hi = z.hi;
  else if (xp->hi >=  z.lo)             ;
  else if (xp->hi >  -z.lo) xp->hi = -z.lo;
  else if (xp->hi >= -z.hi)             ;
  else return(FAIL);

  return(xp->lo <= xp->hi);
}

/****************************************************************/

int narrow_u_minus(INTERVAL *x, INTERVAL *z)
{
  INTERVAL y;
  int result;
	//double vh,vl,xl,xh,zl,zh;

  y = negII(*x);
  result = narrow_eq(&y,z);
  return(result && intersect_eqII(negII(y),x));
}

/****************************************************************/

int narrow_sgn(INTERVAL *x, INTERVAL *z)
{
  //double v;

  if (x->lo > 0) {
     if (z->lo < 1) z->lo = 1;}
  else if (x->hi < 0) {
     if (z->hi> -1) z->hi = -1;}
  else if (z->hi < 0) {
     if (x->hi > 0) x->hi = 0;}
  else if (z->lo > 0) {
     if (x->lo < 0) x->lo = 0;}

  return((x->lo <= x->hi) && (z->lo <= z->hi));
}

INTERVAL sgnII(INTERVAL x) {
  INTERVAL y;

  if (x.hi < 0) {y.lo=y.hi= -1;return(y);}
  else if (x.lo > 0) {y.lo = y.hi = 1; return(y);}
  else {y.lo= -1; y.hi = 1; return(y);}
}

/****************************************************************/

INTERVAL maxIII(INTERVAL x, INTERVAL y) {
  INTERVAL z;

  z.lo = MAX(x.lo,y.lo);
  z.hi = MAX(x.hi,y.hi);
  return(z);
}

int narrow_max(INTERVAL *x, INTERVAL *y, INTERVAL *z)
{
  //double v;
  int result;

  if (x->hi > z->hi) x->hi = z->hi;
  if (y->hi > z->hi) y->hi = z->hi;

  result = intersect_eqII(maxIII(*x,*y),z);

  return(result && (x->lo <= x->hi) && (y->lo <= y->hi));
}



/****************************************************************/

INTERVAL minIII(INTERVAL x, INTERVAL y) {
  INTERVAL z;

  z.lo = MIN(x.lo,y.lo);
  z.hi = MIN(x.hi,y.hi);
  return(z);
}


int narrow_min(INTERVAL *x, INTERVAL *y, INTERVAL *z)
{
  int result;

  if (x->lo < z->lo) x->lo = z->lo;
  if (y->lo < z->lo) y->lo = z->lo;

  result = intersect_eqII(minIII(*x,*y),z);

  return(result && (x->lo <= x->hi) && (y->lo <= y->hi));

}

/****************************************************************/

int narrow_integer(INTERVAL *z)
{
  z->lo = ceil(z->lo);  z->hi = floor(z->hi);
  return(z->lo <= z->hi);
}

/****************************************************************/

int narrow_bool(INTERVAL *z)
{
  //double v;
  if (z->lo < 0) z->lo = 0;
  if (z->hi > 1) z->hi = 1;
  z->lo = ceil(z->lo);  z->hi = floor(z->hi);
  return(z->lo <= z->hi);
}

/****************************************************************/


INTERVAL floorII(INTERVAL x) {
  INTERVAL y;
  y.lo = floor(x.lo);
  y.hi = floor(x.hi);
  return(y);
}

int narrow_flr(INTERVAL *y, INTERVAL *z){
  /* floor(y) = z */
  INTERVAL tmp;
  int result;

  /* narrow z */
  result = intersect_eqII(floorII(*y),z);

  /* narrow y */
  tmp.lo = z->lo; tmp.hi = z->hi+1;
  result = result && intersect_eqII(tmp,y);

  return(result);
}

/****************************************************************/

INTERVAL ceilII(INTERVAL x) {
  INTERVAL y;
  y.lo = ceil(x.lo);
  y.hi = ceil(x.hi);
  return(y);
}

int narrow_ceil(INTERVAL *y, INTERVAL *z){
  /* ceil(y) = z */
  INTERVAL tmp;
  int result;

  /* narrow z */
  result = intersect_eqII(ceilII(*y),z);

  /* narrow y */
  tmp.lo = z->lo-1; tmp.hi = z->hi;
  result = result && intersect_eqII(tmp,y);

  return(result);
}


/****************************************************************/


int narrow_subset(INTERVAL *x, INTERVAL *y)
{
  return(intersect_eqII(*y,x));
}


/* ****************************************************************
		Sound narrowing for addition
	 **************************************************************** */

int narrow_add(INTERVAL *x, INTERVAL *y, INTERVAL *z)
{
	double vh,vl,xl,xh,yl,yh,zl,zh;
        //int result;
/*
	Here we known that x+y=z and x,y,z are in some intervals.
	The strategy is to first compute lower bounds, then compute
	upperbounds.
*/

	xl= x->lo;	 /* xl < +inf */
	yl = y->lo;	/* yl < +inf */
	zl = z->lo;	/* zl < +inf */
	xh=x->hi;
	yh = y->hi;
	zh = z->hi;
	vl = add_lo(xl,yl); /* vl < +inf */
	
	if ((xl==0.0) && (xh==0.0)) {return(narrow_eq(y,z));}
        if ((yl==0.0) && (yh==0.0)) {return(narrow_eq(x,z));}


	if (zl < vl) {
		zl = (z->lo = vl);
	}
	else /* zl >= xl*yl */{
		vl = sub_lo(zl,xh);
		if (yl < vl){
			yl = (y->lo = vl);
		}
		vl = sub_lo(zl,yh);
		if (xl < vl) {
			xl = (x->lo = vl);
			}
	}

	vh = add_hi(xh,yh);
	if (zh > vh){
		z->hi = vh;
	}
	else {
		vh = sub_hi(zh,yl);
		if (xh > vh) {
			x->hi = vh;
		}
		vh = sub_hi(zh,xl);
		if (yh > vh){
			y->hi = vh;
			}
	}

        return( (z->lo <= z->hi) &&
                (x->lo <= x->hi) && 
                (y->lo <= y->hi));
}



/****************************************************************/

INTERVAL squareII(INTERVAL a) {
  INTERVAL b;
  if (a.lo >= 0.0)    /* entire interval is positive */
    {b.lo = mul_lo(a.lo,a.lo); b.hi = mul_hi(a.hi,a.hi);}
 
 else          /* at least part of interval is negative */

    {if (a.hi < 0.0)   /* entire interval is negative */
      {b.lo = mul_lo(a.hi,a.hi); b.hi = mul_hi(a.lo,a.lo);}
				  
    else {  /*  interval spans 0 */
      b.lo = 0.0;
      b.hi =  a.hi > - a.lo ? mul_hi(a.hi, a.hi) : mul_hi (a.lo,a.lo);
	}
    }
  return b;
}

int narrow_square(INTERVAL *x,  INTERVAL *z)
{
	double vh,vl,xl,xh; //,zl,zh;
  if (z->lo < 0.0) {
	z->lo = 0.0;
  }
  if (z->hi < 0.0)
	return(FAIL);

  xl = x->lo;
  xh = x->hi;
  if (xl >= 0) {
	vh = mul_hi(xh,xh);
	if (vh < z->hi) 
		z->hi = vh;
	else {
		vh = sqrt_hi(z->hi);
		if (vh < xh)
			x->hi = vh;
	}
	vl = mul_lo(xl,xl);
	if (vl > z->lo)
		z -> lo = vl;
	else {
		vl = sqrt_lo(z->lo);
	        if (vl > xl)
			x->lo = vl;
	}
  }
  else if (xh <= 0) {
        INTERVAL y;
        y.lo = -x->hi; y.hi = -x->lo;
        narrow_square(&y,z);
        x->lo = -y.hi; x->hi = -y.lo;
  }
  else /* x is split */ {
	vh = mul_hi(xh,xh);
        vl = mul_hi(xl,xl);
        if (vl < vh){
		if (z->hi > vh)
			z->hi = vh;
	   }
        else if (z->hi > vl)
		z->hi = vl;

	vh = sqrt_hi(z->hi);
	if (x->hi > vh)
		x->hi = vh;
	if (x->lo < -vh)
		x->lo = -vh;
	}

  return((x->lo <= x->hi) && (z->lo <= z->hi));

}



