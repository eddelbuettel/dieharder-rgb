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

#include "dieharder.h"

void Xtest_eval(Xtest *xtest)
{

 int i;
 double chi;

 /*
  * This routine evaluates the p-value from the xtest data.
  * x, y, sigma all must be filled in by the calling routine.
  */
 xtest->pvalue =
     gsl_sf_erfc(fabs(xtest->y - xtest->x)/(sqrt(2.0)*xtest->sigma));

 if(verbose == D_XTEST || verbose == D_ALL){
   printf("# Xtest_eval(): x = %10.5f  y = %10.5f  sigma = %10.5f\n",
     xtest->x, xtest->y, xtest->sigma);
   printf("# Xtest_eval(): p-value = %10.5f\n",xtest->pvalue);
 }

}

