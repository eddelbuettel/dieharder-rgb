/*
* $Id$
*
* See copyright in copyright.h and the accompanying file COPYING
* See also accompanying file STS.COPYING
*
*/

/*
 *========================================================================
 * This is a test that checks to see if the rng generates bits that are
 * binomially distributed.  It checks the actual distribution of bits for
 * relatively small strings (small enough that n! doesn't overflow and
 * 2^-n doesn't underflow, so we can evaluate meaningful probabilities).
 * It >>also<< does the sts_monobit test on the aggregate string across
 * all smaller samples.
 *
 * This (and all further rgb_ tests) are "my own" in that I'm BOTH
 * making them up AND writing them as I go.  If they turn out to be
 * equivalent to STS tests I'll document this and probably convert to
 * the STS versions.  The exercise of conceiving and writing tests is
 * still invaluable.
 *========================================================================
 */

#include "rand_rate.h"

void rgb_binomial()
{

 int i,j,k,nbits;
 unsigned int num_ones,num_zeros,num_bits;
 unsigned int mask;
 double onefrac,chisq,pvalue;
 double *x,*y,*sigma,ptmp;

 /*
  * x[k] is an accumulator to be used to count the frequency with
  *   which we get k 1's in num_bits, over the m runs.
  * y[k] contains the expected/exact result.
  * sigma[k] contains the standard error in y[k].
  *
  * We start by creating all three and filling y[k] and sigma[k]
  * to support the formation of chisq as we run.
  */
 nbits = 8*sizeof(unsigned int)*size;
 x = (double *) malloc(sizeof(double)*(nbits+1));       /* sample results */
 y = (double *) malloc(sizeof(double)*(nbits+1));       /* expected sample results */
 sigma = (double *) malloc(sizeof(double)*(nbits+1));   /* known sample sigma */
 for(k=0;k<=nbits;k++){
   ptmp = binomial(nbits,k,0.5);
   x[k] = 0.0;                          /* zero the accumulator/counter */
   y[k] = samples*ptmp;
   sigma[k] = sqrt(y[k]*(1.0 - ptmp));
 }

 /*
  * Next we run the sampling loop to accumulate num_bit results
  * in x[k].  For the moment we print out each sample as well.
  */
 for(i=0;i<samples;i++){
   if((i%25) == 0 && verbose){
     printf("# ones\t\tzeros\t\ttotal bits\t\tPercentage of ones\n");
     printf("#------------------------------------------------------------------\n");
   }
   /*
    * Fill vector of "random" integers with selected generator.
    * NOTE WELL:  This can also be done by reading in a file!
    */
   for(j=0;j<size;j++) rand_int[j] = gsl_rng_get(random);
   /*
    * Count the 1 bits and the 0 bits
    * We do this by masking out each bit slot, anding the mask with
    * the rand_int and accumulating the ones.  I think.
    */
   num_bits = 0;
   num_ones = 0;
   num_zeros = 0;
   for(j=0;j<size;j++){
     mask = 1;
     for(k=0;k<32;k++){
       num_bits++;
       if((mask & rand_int[j]) != 0) num_ones++;
       mask = mask<<1;
     }
   }
   num_zeros = num_bits - num_ones;
   onefrac = 100.0*(double)num_ones/(double)num_bits;
   if(verbose){
     printf("%u\t%u\t%u\t\t%10.8f%% ones\n",num_ones,num_zeros,num_bits,onefrac);
   }
   /*
    * Increment the counter for the resulting number of ones.
    */
   x[num_ones]++;
 }

 /*
  * RESULTS:  We output chisq, the number of points compared, and a p-value
  * that is the probability of getting this result if the random number
  * generator is good.  Hence values less than 0.01 suggest a "bad" rng.
  * Results MUCH less than 0.01 should Never Happen and we'll say so.
  * If I understand things correctly, p-value from chisq is
  *   p = incomplete_gamma(N/2,chisq/2) (gsl Q version, not P version).
  */
 chisq = chisq_eval(x,y,sigma,nbits);
 pvalue = gsl_sf_gamma_inc_Q((double)nbits/2.0,chisq/2.0);
 if(!quiet){
   printf("#==================================================================\n");
   printf("# rgb_binomial test of bit distribution using the %s generator\n",gsl_rng_name(random));
   printf("# number of 1's in %d strings of %d bits each cumulated and compared\n",samples,nbits);
   printf("# to binomial distribution predictions.\n");
   printf("# Results:  chisq = %12.8f for %d points (bits)\n",chisq,nbits);
   printf("#==================================================================\n");
 }
 printf("p-value = Q(%f,%f) = %10.6f\n",(double)nbits/2.0,chisq/2.0,pvalue);
 if(!quiet){
   if((pvalue<=0.05) && (pvalue>0.01)){
     printf("Conclusion: Generator may be weak. (5%% level)  Run again.\n");
   } else if((pvalue<=0.01) && (pvalue > 0.0001)){
     printf("Conclusion: Generator likely to be weak! (1%% level)  Run again.\n");
   } else if(pvalue<=0.0001){
     printf("Conclusion: Generator Fails Test! (0.01%% level or less)\n");
   } else {
     printf("Conclusion: No obvious reason to worry about generator.\n");
   }
 }

 /*
  * Debugging:  Given as 0.801252 in STS docs, just making sure that
  * we're using the right gsl incomplete gamma routine.  We are.
 pvalue = gsl_sf_gamma_inc_Q(1.5,0.5);
 printf("rgb_binomial p-value = Q(%f,%f) = %10.6f\n",1.5,0.5,pvalue);
 */

}

