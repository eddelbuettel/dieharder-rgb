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

 double pnk;

 if(verbose){
   printf("binomial(): Making binomial p(%d,%d,%f)\n",n,k,p);
 }

 pnk = gsl_sf_fact(n)*pow(p,(double)k)*pow((1.0-p),(double)(n-k))/
             (gsl_sf_fact(k)*gsl_sf_fact(n-k));

 if(verbose){
   printf("binomial(): Made binomial p(%d,%d,%f) = %f\n",n,k,p,pnk);
 }

 return(pnk);

}

void Ntest_fill_binomial(Ntest *ntest)
{


}
