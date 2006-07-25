/*
 *========================================================================
 * $Id$
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *========================================================================
 */

/*
 *========================================================================
 * Vtest.c contains a set of routines for evaluating a p-value, the
 * probability that a given test result was obtained IF the underlying
 * random number generator was a "good" one (the null hypothesis), given a
 * vector x of observed results and a vector y of expected results.  It
 * determines the p-value using Pearson's chisq, which does not require
 * the independent input of the expected sigma for each "bin" (vector
 * position).
 *========================================================================
 */
#include "dieharder.h"

void Vtest_create(Vtest *btest, unsigned int bins)
{

 int i;
 if(verbose == D_BTEST || verbose == D_ALL){
   printf("Vtest_create(): Creating test struct for %u bins.\n",bins);
 }
 btest->x = (double *) malloc(sizeof(double)*bins);       /* sample results */
 btest->y = (double *) malloc(sizeof(double)*bins);       /* expected sample results */
 btest->sigma = (double *) malloc(sizeof(double)*bins);   /* known sample sigma */
 /* zero or set everything */
 for(i=0;i<bins;i++){
   btest->x[i] = 0.0;
   btest->y[i] = 0.0;
   btest->sigma[i] = 0.0;
 }
 btest->bins = bins;
 btest->chisq = 0.0;
 btest->pvalue = 0.0;
 if(verbose == D_BTEST || verbose == D_ALL){
   printf("Vtest_create(): Created test structure for %d points.\n",btest->bins);
 }


}

void Vtest_destroy(Vtest *btest)
{

 free(btest->x);
 free(btest->y);
 free(btest->sigma);

}

void Vtest_eval(Vtest *btest)
{

 uint i,ndof;
 double delchisq,chisq;
 double x_tot,y_tot;


 /*
  * This routine evaluates chi-squared, where:
  * btest->x is the trial vector
  * btest->y is the exact vector
  * btest->sigma is the vector of expected error for y
  *              (for the exact/true distribution)
  * bins is the vector length(s).
  *
  * x, y, sigma all must be filled in my the calling routine.
  *
  * We'll have to see how this handles sigma[i] = 0.0.  Probably badly
  * but smoothly.
  */
 /* verbose=1; */
 if(verbose == D_BTEST || verbose == D_ALL){
   printf("Evaluating chisq and pvalue for %d points\n",btest->bins);
 }

 chisq = 0.0;
 x_tot = 0.0;
 y_tot = 0.0;
 ndof = 0;
 if(verbose == D_BTEST || verbose == D_ALL){
   printf("# %7s   %3s      %3s %10s      %10s %10s %9s\n",
           "bit/bin","DoF","X","Y","sigma","del-chisq","chisq");
   printf("#==================================================================\n");
 }
 for (i=0;i<btest->bins;i++) {
   if(btest->x[i] > 10.0){
     x_tot += btest->x[i];
     y_tot += btest->y[i];
     delchisq = (btest->x[i] - btest->y[i])*(btest->x[i] - btest->y[i])/btest->y[i];
     /*  Alternative way of evaluating chisq for binomial only.
     delchisq = (btest->x[i] - btest->y[i])/btest->sigma[i];
     delchisq *= delchisq;
     */
     chisq += delchisq;
     if(verbose == D_BTEST || verbose == D_ALL){
       printf("# %5u\t%3u\t%12.4f\t%12.4f\t%8.4f\t%10.4f\n",
                  i,ndof,btest->x[i],btest->y[i],delchisq,chisq);
     }
     ndof++;
   }
 }

 if(verbose == D_BTEST || verbose == D_ALL){
   printf("Total:  %10.4f  %10.4f\n",x_tot,y_tot);
   printf("#==================================================================\n");
   printf("Evaluated chisq = %f for %u degrees of freedom\n",chisq,ndof);
 }
 btest->chisq = chisq;

 /*
  * Now evaluate the corresponding pvalue.  The only real question
  * is what is the correct number of degrees of freedom.  I'd argue we
  * did use a constraint when we set expected = binomial*nsamp, so we'll
  * go for ndof (count of bins tallied) - 1.
  */
 ndof--;
 btest->pvalue = gsl_sf_gamma_inc_Q((double)(ndof)/2.0,chisq/2.0);
 if(verbose == D_BTEST || verbose == D_ALL){
   printf("Evaluted pvalue = %6.4f in Vtest_eval().\n",btest->pvalue);
 }
 /* verbose=0; */

}

