/*
* $Id$
*
* See copyright in copyright.h and the accompanying file COPYING
* See also accompanying file STS.COPYING
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

int chisq_poisson(uint *svec,double *pvec,uint nvec,double lambda)
{

}

