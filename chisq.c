/*
 * $Id$
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *
 */

/*
 *========================================================================
 * This code evaluates a vector of chisq's and p-values from a vector
 * of sampled results.  I believe that it assumes a Poissonian
 * distribution in the vector, not a normal one.  If so, we'll rename
 * this appropriately.
 *========================================================================
 */


#include "rand_rate.h"


/*
 * This routine computes chisq on a vector of nvec values drawn
 * presumably from a known discrete distribution (Poisson).
 * lambda is the expected mean, and pvec is a vector
 * where a list of associated p-values is returned.
 *
 * Note that a Poisson distribution with mean lambda is:
 *
 *  p(k) = (lambda^k/k!) exp(-lambda)
 *
 * and that it can be computed in a single pass by:
 *
 *  p(k) = gsl_ran_poisson_pdf(uint k,double lambda);
 *
 * All we need to do, then, is compute p(k) and use it to
 * determine a vector of estimates for the interval frequency,
 * then compute Pearson's chisq in a straightforward way.
 * The only "tricky" part of all of this is going to be a)
 * scaling the data by the number of independent samples; and
 * b) converting into p-values, which requires a knowledge of
 * the number of degrees of freedom of the fit.
 */

double chisq_poisson(uint *observed,double lambda,int kmax)
{

 uint i,j,k;
 double *expected;
 double delchisq,chisq,pvalue;

 expected = (double *)malloc(kmax*sizeof(double));
 for(k = 0;k<kmax;k++){
   expected[k] = samples*gsl_ran_poisson_pdf(k,lambda);
 }

 /*
  * Compute Pearson's chisq for this vector of the data.
  */
 chisq = 0.0;
 for(k = 0;k < kmax;k++){
   delchisq = ((double) observed[k] - expected[k])*
      ((double) observed[k] - expected[k])/expected[k];
   chisq += delchisq;
   if(verbose){
     printf("%u:  observed = %f,  expected = %f, delchisq = %f, chisq = %f\n",
        k,(double)observed[k],expected[k],delchisq,chisq);
   }
 }

 if(verbose){
   printf("Evaluated chisq = %f for %u k values\n",chisq,kmax);
 }

 /*
  * Now evaluate the corresponding pvalue.  The only real question
  * is what is the correct number of degrees of freedom.  We have
  * kmax bins, so it should be kmax-1.
  */
 pvalue = gsl_sf_gamma_inc_Q((double)(kmax-1)/2.0,chisq/2.0);
 if(verbose){
   printf("pvalue = %f in chisq_poisson.\n",pvalue);
 }

 return(pvalue);

}

