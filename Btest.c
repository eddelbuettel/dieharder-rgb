/*
 *========================================================================
 * $Id$
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *========================================================================
 */

/*
 *========================================================================
 * Btest.c contains a set of routines for creating, destroying, and
 * evaluating binned statistical results obtained in a vector.  It
 * is assumed that each bin contains an integer count of some event, and
 * that the expected value for this count (of "events") is known.
 *
 * The appropriate test for this sort of data is the chisq test, which
 * yields the probability that a given set of binned data could have
 * been drawn randomly from the expected distribution.  The number of
 * degrees of freedom in the chisq distribution is tricky if the expected
 * distribution has long "tails" -- bins where the expected value is
 * approximately zero.  The binned data only approaches the chisq
 * distribution for bins that are have an expected value greater than (say)
 * 10.  The probability is then the incomplete gamma function for
 * the observed chisq and number of degrees of freedom - 1 (there is a
 * constraint used to normalize the expected value to the total number of
 * observed events).
 *
 * We do have one final choice -- to use an input sigma to make chisq
 * or to use Pearson's chisq, which only requires the expected value.
 * We'll leave a sigma vector in the struct for the time being but use
 * Pearson as more general and a bit simpler (they're empirically nearly
 * the same for binomials, as they should be).
 *
 * There is an associated "object" struct containing the various
 * histograms, the name of the test they support, and so forth.  We
 * therefore have creator and destructor routines as well as fairly
 * standardized routines for taking the histogram contents and converting
 * them into a p-value.
 *========================================================================
 */

#include "dieharder.h"

void Btest_create(Btest *btest, unsigned int bins)
{

 int i;
 if(verbose == D_BTEST || verbose == D_ALL){
   printf("Btest_create(): Creating test struct for %u bins.\n",bins);
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
   printf("Btest_create(): Created test structure for %d points.\n",btest->bins);
 }


}

void Btest_destroy(Btest *btest)
{

 free(btest->x);
 free(btest->y);
 free(btest->sigma);

}

void Btest_eval(Btest *btest)
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
   printf("Evaluted pvalue = %6.4f in Btest_eval().\n",btest->pvalue);
 }
 /* verbose=0; */

}

