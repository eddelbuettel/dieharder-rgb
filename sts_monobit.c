/*
* $Id$
*
* See copyright in copyright.h and the accompanying file COPYING
* See also accompanying file STS.COPYING
*
*/

/*
 *========================================================================
 * This is a the monobit test, rewritten from the description in the
 * STS suite.
 *
 * Rewriting means that I can standardize the interface to gsl-encapsulated
 * routines more easily.  It also makes this my own code.
 *========================================================================
 */

#include "rand_rate.h"

void sts_monobit()
{

 int i,j,k;
 unsigned int num_ones,num_zeros,num_bits;
 unsigned int mask;
 double onefrac;

 /*
  * I don't know for sure what this will need.  At a guess:
  *  a) A loop to run "samples" monobit tests.
  *  b) The test itself, which will be something like:
  *    i) Fill vector
  *   ii) Test vector, producing e.g. P-value for the sample
  *  c) Save/eval max-p, min-p, mean-p and maybe stddev-p
  *  d) If max,min,mean,stddev all "expected", conclude good.
  *  e) If not, conclude bad.
  */

 /*
  * A loop to collect monobit run samples.
  */
 for(i=0;i<samples;i++){
   if((i%25) == 0){
     printf("ones\t\tzeros\t\ttotal bits\t\tPercentage of ones\n");
     printf("------------------------------------------------------------------\n");
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
       num_ones += mask&rand_int[j];
       mask<<1;
     }
   }
   num_zeros = num_bits - num_ones;
   onefrac = 100.0*(double)num_ones/(double)num_bits;
   printf("%u\t%u\t%u\t\t%10.8f%% ones\n",num_ones,num_zeros,num_bits,onefrac);
 }

}

