/*
* $Id$
*
* See copyright in copyright.h and the accompanying file COPYING
*/

/*
 *========================================================================
 * This is the Kolmogorov-Smirnov test for uniformity on the interval
 * [0,1).  p-values from a (large) number of independent trials should
 * be uniformly distributed on the interval [0,1) if the underlying
 * result is consistent with the hypothesis that the rng in use is
 * not correlated.  Deviation from uniformity is a sign of the failure
 * of the hypothesis that the generator is "good".  Here we simply
 * input a vector of p-values and a count, and output an overall
 * p-value for the set of tests.
 *========================================================================
 */


#include "rand_rate.h"


double kstest(double *pvalue,int count)
{

 int i,j,k;
 double y,d,dmax,csqrt;
 double p;

 /*
  * We start by sorting the list of pvalues.
  */
 gsl_sort(pvalue,1,count);

 /*
  * Here's the test.  For each (sorted) pvalue, its index is the
  * number of values cumulated to the left of it.  d is the distance
  * between that number and the straight line representing a uniform
  * accumulation.  We save the maximum d across all cumulated samples
  * and transform it into a p-value at the end.
  */
 dmax = 0.0;
 printf("    p       y       d       dmax\n");
 for(i=0;i<count;i++){
   y = (double) i/count;
   d = fabs(pvalue[i] - y);
   printf("%8.3f   %8.3f    %8.3f   %8.3f\n",pvalue[i],y,d,dmax);
   if(d > dmax) dmax = d;
 }

 csqrt = sqrt(count);
 p = q_ks((csqrt + 0.12 + 0.11/csqrt)*d);

 return(p);

}


double q_ks(double x)
{

 int i,sign;
 double qsum = 0;

 sign = -1;
 for(i=1;i<100;i++){
   sign *= -1;
   qsum += sign*exp(-2.0*i*i*x*x);
   /* printf("Q_ks %d: %f\n",i,qsum); */
 }

 /* printf("Q_ks returning %f\n",qsum); */
 return(2.0*qsum);

}
   
   
