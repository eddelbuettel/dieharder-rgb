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

void Ntest_create(Ntest *ntest, unsigned int npts,
                    char *testname, char *rngname)
{

 int i;
 if(verbose){
   printf("Ntest_create(): Creating test struct for %u points.\n",npts);
 }
 ntest->x = (double *) malloc(sizeof(double)*npts);       /* sample results */
 ntest->y = (double *) malloc(sizeof(double)*npts);       /* expected sample results */
 ntest->sigma = (double *) malloc(sizeof(double)*npts);   /* known sample sigma */
 /* zero or set everything */
 for(i=0;i<npts;i++){
   ntest->x[i] = 0.0;
   ntest->y[i] = 0.0;
   ntest->sigma[i] = 0.0;
 }
 ntest->npts = npts;
 ntest->chisq = 0.0;
 ntest->pvalue = 0.0;
 strncpy(ntest->testname,testname,128);
 strncpy(ntest->rngname,rngname,128);
 if(verbose){
   printf("Ntest_create(): Created test structure for %d points.\n",ntest->npts);
 }


}

void Ntest_destroy(Ntest *ntest)
{

 free(ntest->x);
 free(ntest->y);
 free(ntest->sigma);

}

void Ntest_eval(Ntest *ntest)
{

 int i;
 double chi;

 /*
  * This routine evaluates chi-squared, where:
  * ntest->x is the trial vector
  * ntest->y is the exact vector
  * ntest->sigma is the vector of expected error for y
  *              (for the exact/true distribution)
  * npts is the vector length(s).
  *
  * x, y, sigma all must be filled in my the calling routine.
  *
  * We'll have to see how this handles sigma[i] = 0.0.  Probably badly...
  */

 if(verbose){
   printf("Evaluating chisq and pvalue for %d points\n",ntest->npts);
 }

 if(!quiet){
   printf("# Ntest_eval summary\n");
   printf("# %3s %10s %10s %10s %10s %12s\n","bit","    X","    Y"," sigma","del-chisq"," chisq");
   printf("#------------------------------------------------------------------\n");
 }
 ntest->chisq = 0.0;
 for (i=0;i<ntest->npts;i++) {
   chi = (ntest->y[i]-ntest->x[i])/ntest->sigma[i];
   ntest->chisq += chi*chi;
   if(!quiet){
     printf("# %3d %10.1f %10.1f %10.3f %10.3f %12.8f\n",i,ntest->x[i],
           ntest->y[i],ntest->sigma[i],chi*chi,ntest->chisq);
   }
 }

 ntest->pvalue = gsl_sf_gamma_inc_Q((double)ntest->npts/2.0,ntest->chisq/2.0);

}

void Ntest_conclusion(Ntest *ntest)
{
 /*
  * The following decision tree states our conclusion(s).  Disagree
  * with them all you like -- they are at best approximate.
  */
 if(!quiet){
   printf("#==================================================================\n");
   printf("# %s test using the %s generator \n",ntest->testname,ntest->rngname);
   printf("# Results: %12s %10s %12s\n","npoints","chisq","p-value");
   printf("#        %12d   %11.2f %11.5f\n",ntest->npts,ntest->chisq,ntest->pvalue);
   if((ntest->pvalue<=0.05) && (ntest->pvalue>0.01)){
     printf("# Conclusion: Generator %s may be weak. (5%% level)  Run again.\n",ntest->rngname);
   } else if((ntest->pvalue<=0.01) && (ntest->pvalue > 0.0001)){
     printf("# Conclusion: Generator %s likely to be weak! (1%% level)  Run again.\n",ntest->rngname);
   } else if(ntest->pvalue<=0.0001){
     printf("# Conclusion: %s Generator Fails Test! (0.01%% level or less)\n",ntest->rngname);
   } else if(ntest->pvalue>0.9999){
     printf("# Conclusion: %s Generator succeeded! (0.01%% level or more)\n",ntest->rngname);
   } else {
     printf("# Conclusion: No obvious reason to worry about generator %s.\n",ntest->rngname);
   }
   printf("#==================================================================\n");
 }

}

 /*
  * Debugging Cruft:  Given as 0.801252 in STS docs, just making sure that
  * we're using the right gsl incomplete gamma routine.  We are.
 pvalue = gsl_sf_gamma_inc_Q(1.5,0.5);
 printf("rgb_binomial p-value = Q(%f,%f) = %10.6f\n",1.5,0.5,pvalue);
 */

