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

void Xtest_conclusion(Xtest *xtest)
{
 /*
  * The following decision tree states our conclusion(s).  Disagree
  * with them all you like -- they are at best approximate.
  */
 if(!quiet){
   printf("#==================================================================\n");
   printf("# %s test using the %s generator \n",xtest->testname,xtest->rngname);
   printf("# Results: %12s\n","p-value");
   printf("#        %11.5f\n",xtest->pvalue);
   if((xtest->pvalue<=0.05) && (xtest->pvalue>0.01)){
     printf("# Conclusion: Generator %s may be weak. (5%% level)  Run again.\n",xtest->rngname);
   } else if((xtest->pvalue<=0.01) && (xtest->pvalue > 0.0001)){
     printf("# Conclusion: Generator %s likely to be weak! (1%% level)  Run again.\n",xtest->rngname);
   } else if(xtest->pvalue<=0.0001){
     printf("# Conclusion: %s Generator Fails Test! (0.01%% level or less)\n",xtest->rngname);
   } else if(xtest->pvalue>0.9999){
     printf("# Conclusion: %s Generator succeeded! (0.01%% level or more)\n",xtest->rngname);
   } else {
     printf("# Conclusion: No obvious reason to worry about generator %s.\n",xtest->rngname);
   }
   printf("#==================================================================\n");
 }

}

