#ifndef CSE_MATH_RANLIB_H
#define CSE_MATH_RANLIB_H
/*****************************************************************************|
*  FILE:    ranlib.h
*
*  AUTHOR:  Andrew Hall
*
*  CREATED: January 15, 2003
*
*  LAST MODIFIED:  
*             BY:  
*             TO:  
*
*  SUMMARY:
*
*    This is the ranlib library from Netlib by Brown, Lovato, and Russell,
*    based on work by L'Ecuyer and Cote from ACM TOMS.  This file is simply
*    an import of the original ranlib.h with this comment, include guards,
*    and the extern "C" concession to C++.
*
*  NOTES:
*
*
*  TO DO:
*
*
*******************************************************************************|
*        1         2         3         4         5         6         7         8
*2345678901234567890123456789012345678901234567890123456789012345678901234567890
*/



/* Prototypes for all user accessible RANLIB routines */

#ifdef __cplusplus
extern "C" {
#endif


  /* Prototypes for all user accessible RANLIB routines */

  extern void advnst(long k);
  extern float genbet(float aa,float bb);
  extern float genchi(float df);
  extern float genexp(float av);
  extern float genf(float dfn, float dfd);
  extern float gengam(float a,float r);
  extern void genmn(float *parm,float *x,float *work);
  extern void genmul(long n,float *p,long ncat,long *ix);
  extern float gennch(float df,float xnonc);
  extern float gennf(float dfn, float dfd, float xnonc);
  extern float gennor(float av,float sd);
  extern void genprm(long *iarray,int larray);
  extern float genunf(float low,float high);
  extern void getsd(long *iseed1,long *iseed2);
  extern void gscgn(long getset,long *g);
  extern long ignbin(long n,float pp);
  extern long ignnbn(long n,float p);
  extern long ignlgi(void);
  extern long ignpoi(float mu);
  extern long ignuin(long low,long high);
  extern void initgn(long isdtyp);
  extern long mltmod(long a,long s,long m);
  extern void phrtsd(char* phrase,long* seed1,long* seed2);
  extern float ranf(void);
  extern void setall(long iseed1,long iseed2);
  extern void setant(long qvalue);
  extern void setgmn(float *meanv,float *covm,long p,float *parm);
  extern void setsd(long iseed1,long iseed2);
  extern float sexpo(void);
  extern float sgamma(float a);
  extern float snorm(void);
  
#ifdef __cplusplus
}
#endif

#endif /* CSE_MATH_RANLIB_H */
