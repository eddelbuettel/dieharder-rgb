/*
* $Id$
*
* See copyright in copyright.h and the accompanying file COPYING
* See also accompanying file STS.COPYING
*
*/

/*
 *========================================================================
 * This code determines if a vector of p-values is "uniform" as far as
 * as the number of values that fall below a 1% cutoff.  We'll likely
 * put in a better test of uniformity but this is for starters.
 *========================================================================
 */


#include "dieharder.h"

int confidence(double *pvec,int m,double alpha)
{

 int i,j,k;
 double phat,pcount,proportion,interval,confplus,confminus;

 pcount = 0.0;
 for(i=0;i<m;i++){
   /* printf("pvalue = %f\n",pvec[i]); */
   if(pvec[i]>alpha) pcount++;
 }
 proportion = pcount/(double) m;
 /* printf("pcount = %f, proportion = %f\n",pcount,proportion); */
 phat = 1.0 - alpha;
 interval = 3.0*sqrt(phat*(1.0-phat)/(double)m);
 confplus  = phat+interval;
 confminus = phat-interval;
 printf("# %4.2f%% confidence interval for %d samples = (%f,%f)\n",
   phat*100.0,m,confminus,confplus);
 if(proportion > confplus){
   printf("Failure:  proportion = %f\n",proportion);
 } else if(proportion < confminus){
   printf("Failure:  proportion = %f\n",proportion);
 } else {
   printf("Success:  proportion = %f\n",proportion);
 }
 
}

