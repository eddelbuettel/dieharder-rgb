/*
* $Id$
*
* See copyright in copyright.h and the accompanying file COPYING
*
*/

/*
 *========================================================================
 * timing and utility sources.  tv_start and tv_stop are globals.
 *========================================================================
 */

#include "rand_rate.h"

double binomial(unsigned int n, unsigned int k, double p)
{

 double pnk = gsl_sf_fact(n)*pow(p,(double)k)*pow((1.0-p),(double)(n-k))/
             (gsl_sf_fact(k)*gsl_sf_fact(n-k));

 return(pnk);

}
