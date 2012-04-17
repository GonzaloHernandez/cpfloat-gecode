   /*
   ****************************************************************
       test_precision.c

   2000 Copyright Timothy J. Hickey
   2003 Copyright David K. Wittenberg
   licence: http://interval.sourceforge.net/licence.txt, zlib/png 

   This program tests the precision of the smath library.

   ****************************************************************
   */


/*            CHANGELOG

2003-03-11  changed __BYTE_ORDER to BYTE_ORDER for portability      dkw
2003-12-12  removed some includes duplicated by smath               dkw

*/



#include <stdlib.h>
#include <stdio.h>
#include "smath.h"


#if BYTE_ORDER == BIG_ENDIAN
static NEWDOUBLE
  //TWOPI_LO          =  {{0x4019, 0x21fb, 0x5444, 0x2d18}},
  //TWOPI_HI          =  {{0x4019, 0x21fb, 0x5444, 0x2d19}},
  TWOPI             =  {{0x4019, 0x21fb, 0x5444, 0x2d18}},
  PIOVER2_A         =  {{0x3ff9, 0x21fb, 0x5440, 0x0000}},
  PIOVER2_B         =  {{0x3dd0, 0xB461, 0x1A62, 0x6331}};
#endif

#if BYTE_ORDER == LITTLE_ENDIAN
static NEWDOUBLE
  //TWOPI_LO          =  {{0x2d18,0x5444, 0x21fb, 0x4019 }},
  //TWOPI_HI          =  {{0x2d19,0x5444, 0x21fb, 0x4019 }},
  TWOPI             =  {{0x2d18,0x5444, 0x21fb, 0x4019 }},
  PIOVER2_A         =  {{0x0000,0x5440, 0x21fb, 0x3ff9 }},
  PIOVER2_B         =  {{0x6331,0x1A62, 0xB461, 0x3dd0 }};
#endif


int tight_lo=1,tight_hi=1;
int tabular_form = 0;


int width[1024];


double (*nexta)(void);
double (*flo)(double x);
double (*fmid)(double x);
double (*fhi)(double x);


static void print_double(char *s,double f) {
unsigned int * u;
  u = (unsigned int *) &f;
#if BYTE_ORDER == BIG_ENDIAN
  printf("%20s %8x %8x %.20g\n",s,*u,*(u+1),f);
#endif
#if BYTE_ORDER == LITTLE_ENDIAN
  printf("%20s %8x %8x %.20g\n",s,*(u+1),*u,f);
#endif
}


double dummy(double x) {
  tight_lo = 1;
  tight_hi = 1;
  return(x);
}

int debug=0;

int autogen = 1;

double t52 = 1024.0*1024.0*1024.0*1024.0*1024.0*4.0;

void auto_test(int count)
{
  unsigned int *v,*v2,*w,*w2,j;
  int width[102],i,jj,c;
  double max=0,sum=0,
         count_t=0,count_tlo=0,count_thi=0,count_thilo=0,
         num=1;
  double err,err2=0,maxerr2lo=0.0,maxerr2hi=0.0,maxerr2=0.0;

   num = c = count;

  for(i=0;i<102;i++) width[i]=0;

  while(count--) {
   double a,blo,b,bhi;


  if (autogen==1) 
    a = (*nexta)();
  else {
    printf("Enter a:\n");
    scanf("%lf",&a);
  }

  blo = (*flo)(a);
  /*  fpsetround(FP_RN); */
//  roundnr();
  b   = (*fmid)(a);
  /*  fpsetround(FP_RM); */
//  rounddn();
  bhi = (*fhi)(a);

#if BYTE_ORDER == BIG_ENDIAN
  w = (unsigned int *) &blo;
  w2=w+1;
  v = (unsigned int *) &bhi;
  v2=v+1;
#endif

#if BYTE_ORDER == LITTLE_ENDIAN
  w2 = (unsigned int *) &blo;
  w=w2+1;
  v2 = (unsigned int *) &bhi;
  v=v2+1;
#endif

  if (bhi>0) 
      j = jj = *(v2) - *(w2);
  else
      j = jj = *(w2) - *(v2);

  if ((j == 0) && (blo != bhi)) {
    if (bhi>0) 
      j = jj = (*v-*w)* 1024.0*1024.0*1024.0*4.0;
    else
      j = jj = (*w-*v)* 1024.0*1024.0*1024.0*4.0;
  }
  /* this measures the distance between blo and bhi
     in terms of the number of ULP between them. It
     is not always correct however */ 


  err = (*v-*w)*1024.0  *1024.0  *1024.0  *4  + j;

  err2 = 0;
  if (b < blo) {
     err2 = (blo-b)/blo;
     err2 = (err2 < 0)? (-err2):err2;
     if (err2> maxerr2lo) maxerr2lo = err2;
   }
  if (b > bhi) {
     err2 = (b-bhi)/bhi;
     err2 = (err2 < 0)? (-err2):err2;
     if (err2> maxerr2hi) maxerr2hi = err2;
   }
  if (err2 > maxerr2) maxerr2 = err2;








/*
  if (j >= 3) {
    printf("j = %d ", j);
    print_double("a = ",a);
  }
*/


  if ( /* (blo > b) || (bhi < b)  ||  */
       /* (err2 * t52 > 1.0) || */
       /* (err2 > 2.2E-16) ||*/
       /* (j >= 3) ||*/
      (autogen==0)) { /* || (((j>1) && (tight_lo || tight_hi)))) {*/

     debug = 1; 
     blo=(*flo)(a); 
     bhi=(*fhi)(a);
     debug = 0;

     printf("\n\nj=%d tight = %d %d\n",j,tight_lo,tight_hi);
     print_double("a = ",a);
     print_double("blo = ",blo);
     print_double("  b = ",b);
     print_double("bhi = ",bhi);
     printf("\n");
     printf("\n");
   }

  count_t += (1 - tight_lo*tight_hi);
  count_tlo += (1-tight_lo);
  count_thi += (1-tight_hi);
  count_thilo += ((1-tight_hi)*(1-tight_lo));

  sum = sum + err*err;

  if (err > max) max = err;

  if ((jj < 0) || (j >= 3)) 
 {  /* printf("error in auto test: j = %d\n",j);*/
    j=99;
  }

  width[j]++;

  if ((blo > b)) width[100]+= 1;
  if ((bhi < b)) width[101]+= 1;

 }
  if (tabular_form) {
/*         "rmserr maxerr   1 ULP    2 ULP    >3 ULP  b<blo   b>bhi
           "xx.xxx  xx.xxx  xx.xxx%  xx.xxx%  xx.xxx% xx.xxx% xx.xxx%  
*/
   printf("%7.4g  %5.2g  %7.2f%%  %7.2f%%  %7.2f%%  %10.2f, %10.2f, %6.3f%%  %6.3f%%\n",
           sqrt(sum/num),max,
           width[1]/((double) c)*100,
           width[2]/((double) c)*100,
           width[99]/((double) c)*100,
           maxerr2lo*1024.0*1024.0*1024.0*1024.0*1024.0*4.0,
           maxerr2hi*1024.0*1024.0*1024.0*1024.0*1024.0*4.0,
           width[100]/((double) c)*100,
           width[101]/((double) c)*100);

  }
  else {
     printf("\n\n error table\n\n");

     for(i=0;i<100;i++) 
        if (width[i]!=0) 
            printf("width[%d] = %10d/%d = %6.3f%%\n",
              i,width[i],c,width[i]/(double)c*100);
     if (width[100] != 0) 
       printf("num with b < blo is %10d/%d = %6.3f%%\n",
               width[100],c,width[100]/(double)c*100);
     if (width[101] != 0) 
       printf("num with b > bhi is %10d/%d = %6.3f%%\n",
             width[101],c,width[101]/(double)c*100);
    
     if (maxerr2lo > 0)
       printf("max rel error of b wrt [blo,*] is %g ULP\n",
       maxerr2lo*(1024.0*1024.0*1024.0*1024.0*1024.0*4.0));

     if (maxerr2hi > 0)
       printf("max rel error of b wrt [*,bhi] is %g ULP\n",
       maxerr2hi*(1024.0*1024.0*1024.0*1024.0*1024.0*4.0));
/*
  printf("Tightness exceptions:\n");
  printf("high  or low: %f\n",count_t);
  printf("        high: %f\n",count_thi);
  printf("         low: %f\n",count_tlo);
  printf("high and low: %f\n",count_thilo);
  printf("\n\n");
*/

      printf("max error = %g\n",max);
      printf("rms error = %g\n\n",sqrt(sum/num));

   }
}


double exp_domain(void) {
 return(709*(2*drand48()-1)); 
}

double log_domain(void) {
 return(1/drand48()); 
}

double trig_range = 1e6;
double trig_start = -1e6,
       trig_end = 1e6;

double trig_domain(void) {
 double t;
 t = drand48();
 return(t*trig_start + (1-t)*trig_end);
}

double old_trig_domain(void) {
 return((2*drand48()-1)*trig_range);
}


int asd = 0;
double asin_domain(void) {
/* 0, 0.5, 0.707, 0.866 1.0 */

/*  return(asd/32.0+drand48()/32.0);*/
  switch(asd) {
  case 0:  return((2*drand48()-1)); break; /* error 2 ULP */
  case 1:  return((0.5)*drand48());    break;  /* error 2 ULP */
  case 2:  return(0.5 + (0.708- 0.5)*drand48());    break; /* error 3 ULP */
  case 3:  return(0.707 + (0.867-0.707)*drand48());    break; /* error 3 ULP */
  case 4:  return(0.866 + (0.95-0.866)*drand48());    break; /* error 4 ULP */
  case 5:  return(0.95 + (1-0.95)*drand48());    break;   /* error 2ULP */
  case 6:  return(drand48()); break;
  case 7:  return(-1*drand48()); break;
  case 8:  return(0.866*drand48()); break;
  default:  return((2*floor(drand48()+0.5)-1) *
                   (floor(((drand48())*8))/8.0 + drand48()/asd));
  }

}

double asec_domain(void) {
  return(1/(2*drand48()-1));
}

double atan_domain(void) {
/*  return(drand48());*/
 if (drand48()>0.5) {
   if (drand48()>0.5)
     return(1/drand48());
   else
     return(drand48()); 
 }
 else {
   if (drand48()>0.5)
     return(-1/drand48());
   else
     return(-1*drand48()); 
 }
}

typedef struct funset {
  char *name;
  double (*gen)(void);
  double (*hi)(double x);
  double (*mid)(double x);
  double (*lo)(double x);
} FUNSET;

  
double nop_domain(void) {
  return(drand48());
}

double nop(double x) {
  return(x=1.0);
}


double sin_lo(double x) {
  INTERVAL Y;
  Y = sinII(cnstDI(x));
  return(Y.lo);
}
double sin_hi(double x) {
  INTERVAL Y;
  Y = sinII(cnstDI(x));
  return(Y.hi);
}
double cos_lo(double x) {
  INTERVAL Y;
  Y = cosII(cnstDI(x));
  return(Y.lo);
}
double cos_hi(double x) {
  INTERVAL Y;
  Y = cosII(cnstDI(x));
  return(Y.hi);
}
double tan_lo(double x) {
  INTERVAL Y;
  Y = tanII(cnstDI(x));
  return(Y.lo);
}
double tan_hi(double x) {
  INTERVAL Y;
  Y = tanII(cnstDI(x));
  return(Y.hi);
}
double sin2pi_lo(double x) {
  INTERVAL Y;
  Y = sin2piII(cnstDI(x));
  return(Y.lo);
}
double sin2pi_hi(double x) {
  INTERVAL Y;
  Y = sin2piII(cnstDI(x));
  return(Y.hi);
}
double cos2pi_lo(double x) {
  INTERVAL Y;
  Y = cos2piII(cnstDI(x));
  return(Y.lo);
}
double cos2pi_hi(double x) {
  INTERVAL Y;
  Y = cos2piII(cnstDI(x));
  return(Y.hi);
}
double tan2pi_lo(double x) {
  INTERVAL Y;
  Y = tan2piII(cnstDI(x));
  return(Y.lo);
}
double tan2pi_hi(double x) {
  INTERVAL Y;
  Y = tan2piII(cnstDI(x));
  return(Y.hi);
}
  



double sin2pi(double x) {
  x = x - floor(x);
  return(sin(4*(x*PIOVER2_A.d + x*PIOVER2_B.d)));
}
double cos2pi(double x) {
  x = x - floor(x);
  return(cos(x*TWOPI.d));
}
double tan2pi(double x) {
  x = x - floor(x);
  return(tan(x*TWOPI.d));
}


double asin2pi(double x) {
  return(asin(x)/TWOPI.d);
}
double acos2pi(double x) {
  return(acos(x)/TWOPI.d);
}
double atan2pi(double x) {
  return(atan(x)/TWOPI.d);
}


FUNSET f[] = {
  {"sin",&trig_domain,&sin_hi,&sin,&sin_lo},
  {"cos",&trig_domain,&cos_hi,&cos,&cos_lo},
  {"tan",&trig_domain,&tan_hi,&tan,&tan_lo},

  {"asin",&asin_domain,&asin_hi,&asin,&asin_lo},
  {"acos",&asin_domain,&acos_hi,&acos,&acos_lo},
  {"atan",&atan_domain,&atan_hi,&atan,&atan_lo},

  {"sin2pi",&trig_domain,&sin2pi_hi,&sin2pi,&sin2pi_lo},
  {"cos2pi",&trig_domain,&cos2pi_hi,&cos2pi,&cos2pi_lo},
  {"tan2pi",&trig_domain,&tan2pi_hi,&tan2pi,&tan2pi_lo},

  {"asin2pi",&asin_domain,&asin2pi_hi,&asin2pi,&asin2pi_lo},
  {"acos2pi",&asin_domain,&acos2pi_hi,&acos2pi,&acos2pi_lo},
  {"atan2pi",&atan_domain,&atan2pi_hi,&atan2pi,&atan2pi_lo},

  {"exp",&exp_domain,&exp_hi,&exp,&exp_lo},
  {"log",&log_domain,&log_hi,&log,&log_lo},

  {"sin3",&nop_domain,&sin,&sin,&sin},
  {"exp3",&nop_domain,&exp,&exp,&exp},
  {"asin3",&nop_domain,&asin,&asin,&asin},
  {"nop",&nop_domain,&nop,&nop,&nop}
};


int main(void) {
  int count,i;

// rounddn();
 /*  fpsetround(FP_RM);*/
 printf("Debug mode? (yes = 1   no = 0 )\n");
 scanf("%d",&debug);

 printf("Auto test? (yes = 1   no = 0 )\n");
 scanf("%d",&autogen);

 printf("asin domain: (0,1,2,3,4,5)\n");
 scanf("%d",&asd);


 trig_range = 1000000.0;

/*
 printf("What range for the trig functions (-x to x)\n enter x:\n");
 scanf("%lg",&trig_range);
 printf(
  "What range for the trig functions (LO to HI)\n enter LO HI:\n");
 scanf("%lg%lg",&trig_start,&trig_end);
*/

 count = 1;
 while( count != -2) {
   printf("Evaluate which function?\n");
   scanf("%d",&count);
   if (count == -1) {
     /* test all functions */
      tabular_form = 1;

      printf("number of iterations of all functions:\n");
      scanf("%d",&count); 

      printf("\n\nname   rmserr  maxerr   1 ULP     2 UL P    >3 ULP   b<blo    b>bhi\n");
      while(count > 0) {
        for (i=0;i<14;i++) {
           nexta = f[i].gen;
           flo   = f[i].lo;
           fmid  = f[i].mid;
           fhi   = f[i].hi;
           printf("%8s ",f[i].name);
           auto_test(count);
	 }
        printf("number of iterations of all functions:\n");
        scanf("%d",&count); 

      }
    }
   else if (count == -3) {
     /* test all functions */
      tabular_form = 1;

      printf("number of iterations of all functions:\n");
      scanf("%d",&count); 

      printf("\n\nname   rmserr  maxerr   1 ULP     2 UL P    >3 ULP   b<blo    b>bhi\n");
      while(count > 0) {
        for (i=0;i<14;i++) {
           if (i==6) i=12;  
           nexta = f[i].gen;
           flo   = f[i].lo;
           fmid  = f[i].mid;
           fhi   = f[i].hi;
           printf("%8s ",f[i].name);
           auto_test(count);
	 }
        printf("number of iterations of all functions:\n");
        scanf("%d",&count); 

      }
    }
   else {
     tabular_form = 0;

     nexta = f[count].gen;
     flo = f[count].lo;
     fmid = f[count].mid;
     fhi = f[count].hi;

     printf("number of iterations of %s:\n",f[count].name);
     scanf("%d",&count); 
     while(count > 0) {
       auto_test(count);

/*
       printf("asin domain: (0,1,2,3,4,5)\n");
       scanf("%d",&asd);
*/
       printf("number of iterations:\n");
       scanf("%d",&count); 
     }
   }
 }
 return 0;
}


