/*
 *========================================================================
 * $Id$
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *========================================================================
 */

/*
 *========================================================================
 * Ntest.c contains a set of routines for creating, destroying, and
 * evaluating statistical results obtained in a vector or histogram.  It
 * is assumed that each bin's expected contents and normal sigma are
 * known, and chisq for each bin is evaluated relative to this expectation
 * and cumulated.
 *
 * There is an associated "object" struct containing the various
 * histograms, the name of the test they support, and so forth.  We
 * therefore have creator and destructor routines as well as fairly
 * standardized routines for taking the histogram contents and converting
 * them into a p-value.
 *========================================================================
 */

#include "rand_rate.h"

void Ntest_create(Ntest *ntest, unsigned int npts,
                    char *testname, char *rngname)
{

 int i;
 if(verbose > 10){
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
 if(verbose > 10){
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

 uint i,nvalid;
 double chi,chisq;
 double x_tot,y_tot;
 double *pvalue,pks;

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
  * We'll have to see how this handles sigma[i] = 0.0.  Probably badly
  * but smoothly.
  */

 if(verbose > 10){
   printf("Evaluating chisq and pvalue for %d points\n",ntest->npts);
 }

 pvalue = (double *)malloc(ntest->npts*sizeof(double));

 if(!quiet){
   printf("# Ntest_eval summary\n");
   printf("# %7s %3s       %3s   %10s   %10s\n",
           "bit/bin","X","Y","sigma","p-value");
   printf("#==================================================================\n");
 }
 ntest->chisq = 0.0;
 x_tot = 0.0;
 y_tot = 0.0;
 /*
  * We've been doing this all wrong.  Each value in ntest->x[i] should
  * be distributed normally.  Each value therefore should generate a p-value.
  * This VECTOR of pvalues should satisfy a kolmogorov-smirnov test, EXCEPT
  * that we should only include points where the expected result is
  * (say) >=5, if not higher.
  */
 nvalid = 0;
 for (i=0;i<ntest->npts;i++) {
   if(ntest->x[i] > 30.0){
     pvalue[nvalid] = 0.5*
       gsl_sf_erfc((ntest->y[i] - ntest->x[i])/(sqrt(2.0)*ntest->sigma[i]));
     if(!quiet){
       printf("# %3d %10.4f %10.4f %10.4f %10.4f\n",i,ntest->x[i],
             ntest->y[i],ntest->sigma[i],pvalue[nvalid]);
     }
     nvalid++;
   }
 }

 ntest->pvalue = kstest_kuiper(pvalue,nvalid);
 if(!quiet){
   printf("Kuiper KS p value for series = %f\n",ntest->pvalue);
 }

 free(pvalue);

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

