/*
 *========================================================================
 * $Id$
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *========================================================================
 */

/*
 *========================================================================
 * This should be a nice, big case switch where we add EACH test
 * we might want to do and either just configure and do it or
 * prompt for input (if absolutely necessary) and then do it.
 *========================================================================
 */

#include "rand_rate.h"

double chisq_eval(double *x,double *y,double *sigma, unsigned int n)
{

 int i;
 double chi,chisq;
 chisq=0.0;
 /*
  * This routine evaluates chi-squared, where:
  * x is the trial vector
  * y is the exact vector
  * sigma is the vector of expected error for y (the exact true distribution)
  * n is the vector length(s).
  *
  * We'll have to see how this handles sigma[i] = 0.0.  Probably badly...
  */
 for (i=0;i<n;i++) {
   chi = (y[i]-x[i])/sigma[i];
   chisq += chi*chi;
 }

 return(chisq);

}
