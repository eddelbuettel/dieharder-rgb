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

double chisq_pearson(double *observed,double *expected,int kmax)
{

 uint i,j,k;
 double delchisq,chisq,pvalue;

 /*
  * Compute Pearson's chisq for this vector of the data.
  */
 chisq = 0.0;
 for(k = 0;k < kmax;k++){
   delchisq = (observed[k] - expected[k])*
      (observed[k] - expected[k])/expected[k];
   chisq += delchisq;
   if(verbose){
     printf("%u:  observed = %f,  expected = %f, delchisq = %f, chisq = %f\n",
        k,observed[k],expected[k],delchisq,chisq);
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
   printf("pvalue = %f in chisq_pearson.\n",pvalue);
 }

 return(pvalue);

}

double chisq_binomial(double *observed,uint kmax,uint nsamp)
{

 uint n,nmax;
 double expected,sigma,chi,chisq,pvalue,bprob,obstotal,exptotal,sigtotal;

 chisq = 0.0;
 obstotal = 0.0;
 exptotal = 0.0;
 sigtotal = 0.0;
 nmax = kmax;
 if(!quiet){
   printf("# %8s %3s %10s %10s %10s %10s  %7s\n",
           "bit/bin","X","Y","bprob","sigma","del-chisq","chisq");
   printf("#==================================================================\n");
 }
 for(n = 0;n <= nmax;n++){
   bprob = binomial(nmax,n,0.5);
   expected = nsamp*bprob;
   sigma = sqrt(expected*(1.0 - bprob));
   obstotal += observed[n];
   exptotal += expected;
   sigtotal += sigma;
   chi = (observed[n] - expected)/sigma;
   chisq += chi*chi;
   if(!quiet){
     printf("# %3d %10.1f %10.1f %10.8f %10.8f %8.3f %10.4f\n",n,observed[n],
           expected,bprob,sigma,chi*chi,chisq);
   }
 }
 printf("Total:  %10f  %10f  %10f\n",
        obstotal,exptotal,sigtotal);

 if(verbose == 0){
   printf("Evaluated chisq = %f for %u n values\n",chisq,nmax);
 }

 /*
  * Now evaluate the corresponding pvalue.  The only real question
  * is what is the correct number of degrees of freedom.  This is
  * a tough one.
  */
 pvalue = gsl_sf_gamma_inc_Q((double)(nmax)/2.0,chisq/2.0);
 if(verbose == 0){
   printf("pvalue = %f in chisq_binomial.\n",pvalue);
 }

 exit(0);
 return(pvalue);

}

