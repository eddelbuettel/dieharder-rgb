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

void rgb_persist()
{

 int i,j,nbits;
 unsigned int mask;
 static unsigned last_mask,and_mask,last_rand;

 free(rand_int);
 rand_int = (unsigned int*)malloc(samples * sizeof(unsigned int));

 /*
  * Fill rand_int with a string of random numbers
  */
 for(i=0;i<samples;i++) rand_int[i] = gsl_rng_get(random);

 /*
  * Now go through the list and dump the numbers several ways.
  */
 if(bits > 32) {
   nbits = 32;
 } else {
   nbits = bits;
 }
 last_rand = rand_int[0];  /* to start it */
 and_mask = ~(last_rand ^ rand_int[0]);
 for(i=0;i<samples;i++){
   mask = 1;
   printf("rand_int[%d] = %u = ",i,rand_int[i]);
   for(j=0;j<nbits;j++){
     if(mask & rand_int[i]){
       printf("1");
     } else {
       printf("0");
     }
     mask = mask << 1;
   }
   printf("\n");

   /*
    * Now we make a mask of bits that coincide. Logic 41, where are you?
    */
   and_mask = and_mask & (~(last_rand ^ rand_int[i]));
   mask = 1;
   printf("and_mask = %u = ",and_mask);
   for(j=0;j<nbits;j++){
     if(mask & and_mask){
       printf("1");
     } else {
       printf("0");
     }
     mask = mask << 1;
   }
   printf("\n");

 }

}

