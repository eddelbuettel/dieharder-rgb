/*
* $Id$
*
* See copyright in copyright.h and the accompanying file COPYING
* See also accompanying file STS.COPYING
*
*/

/*
 *========================================================================
 * Given a vector of real numbers supposedly distributed according to
 * some known distribution (e.g. uniform, for a vector of p-values),
 * obtain an unbiased estimator for whether the vector could have been
 * drawn from the hypothesized distribution.  This function itself
 * returns a p-value, but it is a p-value that states global conclusions
 * over many trials, similar to what one might get from a Pearson
 * chisq goodness of fit performed on binned p-values as suggested by
 * the STS documentation.  Ultimately, I think we'll do several of these
 * tests -- I want some way of arriving at an absolute accept/reject
 * decision on the hypothesis.
 *========================================================================
 */


#include "rand_rate.h"


int kstest()
{

}
