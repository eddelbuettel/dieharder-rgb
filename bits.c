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

 int i;
 double chi;
 double x_tot,y_tot;

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

 if(verbose > 10){
   printf("Evaluating chisq and pvalue for %d points\n",ntest->npts);
 }

 if(!quiet){
   printf("# Ntest_eval summary\n");
   printf("# %3s %10s %10s %10s %10s %12s\n","bit","    X","    Y"," sigma","del-chisq"," chisq");
   printf("#==================================================================\n");
 }
 ntest->chisq = 0.0;
 x_tot = 0.0;
 y_tot = 0.0;
 for (i=0;i<ntest->npts;i++) {
   chi = (ntest->y[i]-ntest->x[i])/ntest->sigma[i];
   ntest->chisq += chi*chi;
   x_tot += ntest->x[i];
   y_tot += ntest->y[i];
   if(!quiet){
     printf("# %3d %10.1f %10.1f %10.3f %10.3f %12.8f\n",i,ntest->x[i],
           ntest->y[i],ntest->sigma[i],chi*chi,ntest->chisq);
   }
 }
 if(!quiet){
   printf("#==================================================================\n");
   printf("# Tot %10.1f %10.1f\n",x_tot,y_tot);
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

void Xtest_eval(Xtest *xtest)
{

 int i;
 double chi;

 /*
  * This routine evaluates the p-value from the xtest data.
  * x, y, sigma all must be filled in by the calling routine.
  */

 if(!quiet){
   printf("#==================================================================\n");
   printf("#                        Xtest_eval Summary\n");
   printf("# Reference data is observed, expected, error, normalized delta,\n");
   printf("# number of points. p-value is erfc(delta):\n");
   printf("# %5s %10s     %10s %10s %10s\n","X","Y","sigma","delta","Npts");
   printf("#==================================================================\n");
   printf("# %10.1f %10.1f %10.3f %10.5f %10d\n",xtest->x,xtest->y,xtest->sigma,fabs(xtest->y - xtest->x)/(sqrt(2.0)*xtest->sigma),xtest->npts);
 }

 xtest->pvalue =
     gsl_sf_erfc(fabs(xtest->y - xtest->x)/(sqrt(2.0)*xtest->sigma));

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

int get_bit(unsigned int n)
{

 int i;
 unsigned int index,offset,mask;
 static unsigned last_rand;

/*
 * This routine is designed to get the nth bit of the global unsigned int
 * vector rand_int[] (known to be of length size).
 */

 index = (int) (n/(8*sizeof(unsigned int)));
 if(index >= size){
   fprintf(stderr,"Error: bit offset %d exceeds length %d of bitstring in rand[]\n",n,size*sizeof(unsigned int));
   exit(0);
 }

 offset = n%(8*sizeof(unsigned int));
 mask = 1;
 mask = mask<<offset;
 if(mask & rand_int[index]){
   return(1);
 } else {
   return(0);
 }
 

}
