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
 * The real question, then, is whether we can compute a
 * straightforward chisq and p-value from just our data
 * and the computable p(k), or if we have to go through
 * the considerable rigamarole below.
 */

int chisq_poisson(uint *svec,double *pvec,uint nvec,double lambda)
{

 uint i,j,k;
 uint *tmp_vec,ilim;
 double p,s,*ex,*ps,*obs;

 /*
  * Dimension this and that (the internal/tmp variables).
  */
 tmp_vec = (uint *)malloc(nvec*sizeof(uint));
 ps = (double *)malloc(nvec*sizeof(double));

 /* observations? */
 obs = (double *)malloc(nvec*sizeof(double));
 /* expectated values? */
 ex = (double *)malloc(nvec*sizeof(double));

 ilim = lambda + 4.0*lambda;

 /*
  * Zero the accumulator vectors.  I wonder why we don't need nvec of
  * them...
  */
 for(i=0;i<=ilim;i++){
   ex[i] = 0.0;
   tmp_vec[i] = 0.0;
   ps[i] = 0.0;
 }

 p = exp(-lambda);
 s = p*samples;
 j = 0;
 ps[0] = s;
 if(s > 0.5){
   ex[0] = s;
   s = 0.0;
   j = 1;
 }

 for(i=1;i<=ilim;i++){
   p = lambda*p/i;
   ps[i] = ps[i-1] + p*samples;
   s += p*samples;
   tmp_vec[i] = j;
   if(ps[i] > (double)(samples - 5)){
     ex[j] = s + samples - ps[i];
     for(k=i+1;k<=samples;k++){
       tmp_vec[k] = j;
     }
   } else {
     if(s >= 5){
       ex[j] = s;
       j++;
       s = 0.0;
     }
   }
 }

 /*
  * Who knows what this is for.  I should find myself a web-article
  * on poisson chisq and p-values, as I can't believe that this
  * is efficient.  The other thing to do is (yes) look in the GSL
  * and see if any of this stuff is there...
  */
 for(i=0;i<=100;i++){
   obs[i] = 0;
 }

 for(i=1;i<=samples;i++){
   k = tmp_vec[pvec[i]];
   obs[k]++;
 }

 s = 0.0;
 for(i=0;i<=j;i++){
   s += (obs[i]-ex[m])*(obs[i]-ex[m])/ex(m);
 }


 p-value = chisq_(&s, &j);

}

