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

 int i,j,k;
 unsigned int num_ones,num_zeros,num_bits;
 unsigned int mask;
 double onefrac;

 /*
  * m the number of samples we will run, each n bits long.  We will
  *   start by setting m = samples so it can be passed in via the
  *   existing command line.
  * n is sizeof(unsigned int)*size.  It is therefore likely to be
  *   a multiple of 32 (or eventually 64).  We could always make
  *   this an argument, as well, and just grab the next n bits out of
  *   our random vector for arbitrary n.  However, I doubt that this
  *   will be necessary, certainly not at first.
  * count[k] is an accumulator to be used to count the frequency with
  *   which we get k 1's in n bits, over the m runs.  count[k]/m is thus
  *   (if you like) an estimator for p(n,k,1/2).
  * expected[k] is a matching vector filled with m*p(n,k,1/2), and is
  *   what count is expected to be.
  * del_expected[k] is filled with (m*p(n,k,1/2)*(1.0 - p(n,k,1/2)))^1/2,
  *   the (exact) standard error in expected[k].
  * chisq is where we accumulate sum_k (expected[k] - count[k])^2/del_ex...
  *   and use it to compute a p-value.
  */

  
 /*
  * A loop to collect monobit run samples.
  */
 for(i=0;i<samples;i++){
   if((i%25) == 0){
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
   printf("%u\t%u\t%u\t\t%10.8f%% ones\n",num_ones,num_zeros,num_bits,onefrac);
 }

}

