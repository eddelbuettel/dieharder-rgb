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

 int i,j,nbits,csamples;
 unsigned int *rand_uint;
 static unsigned and_mask,cumulative_mask,last_rand;

 /*
  * We arbitrarily choose 256 successive random numbers as enough.
  * This should be plenty -- the probability of any bit slot not
  * changing by CHANCE in 256 tries is 2^-256 or almost 10^-26,
  * so even with 32 bit slots, repeatedly, we aren't horribly likely
  * to see it in our lifetime unless we run this test continuously for
  * months at a time (yes, a dumb idea).
  */
 rand_uint = (unsigned int*)malloc(256 * sizeof(unsigned int));

 /*
  * Now go through the list and dump the numbers several ways.
  */
 if(bits > 32) {
   nbits = 32;
 } else {
   nbits = bits;
 }

 if(reseed_flag) {
   csamples = samples;
 } else {
   csamples = 1;
 }

 cumulative_mask = 0;
 for(j=0;j<csamples;j++){
   seed = random_seed();
   gsl_rng_set(rng,seed);
   /*
    * Fill rand_int with a string of random numbers
    */
   for(i=0;i<256;i++) rand_uint[i] = gsl_rng_get(rng);
   last_rand = rand_uint[0];  /* to start it */
   and_mask = ~(last_rand ^ rand_uint[0]);
   for(i=0;i<256;i++){
     if(verbose){
       printf("rand_uint[%d] = %u = ",i,rand_uint[i]);
       dumpbits(&rand_uint[i],nbits);
     }

     /*
      * Now we make a mask of bits that coincide. Logic 41, where are you?
      */
     and_mask = and_mask & (~(last_rand ^ rand_uint[i]));
     if(verbose){
       printf("and_mask = %u = ",and_mask);
       dumpbits(&and_mask,nbits);
     }

   }
   and_mask = and_mask & rmax_mask;
   cumulative_mask = cumulative_mask | and_mask;
 }
 printf("#==================================================================\n");
 printf("#                        rgb_persist\n");
 printf("# This test generates 256 sequential samples of an random unsigned\n");
 printf("# integer from the given rng.  Successive integers are logically\n");
 printf("# processed to extract a mask with 1's whereever bits do not\n");
 printf("# change.  Since bits will NOT change when filling e.g. unsigned\n");
 printf("# ints with 16 bit ints, this mask logically &'d with the maximum\n");
 printf("# random number returned by the rng.  All the remaining 1's in the\n");
 printf("# resulting mask are therefore significant -- they represent bits\n");
 printf("# that never change over the length of the test.  These bits are\n");
 printf("# very likely the reason that certain rng's fail the monobit\n");
 printf("# test -- extra persistent e.g. 1's or 0's inevitably bias the\n");
 printf("# total bitcount.  In many cases the particular bits repeated\n");
 printf("# appear to depend on the seed.  If the -i flag is given, the\n");
 printf("# entire test is repeated with the rng reseeded to generate a mask\n");
 printf("# and the extracted mask cumulated to show all the possible bit\n");
 printf("# positions that might be repeated for different seeds.\n");
 printf("#==================================================================\n");
 printf("# Results for %s rng, using its %d valid bits:\n",gsl_rng_name(rng),rmax_bits);
 if(cumulative_mask){
   printf("rgb_persist test FAILED (bits repeat)\n");
 } else {
   printf("rgb_persist test PASSED (no bits repeat)\n");
 }
 if(reseed_flag){
   printf("cumulated_mask = %u = ",cumulative_mask);
   dumpbits(&cumulative_mask,nbits);
 } else {
   printf("and_mask = %u = ",and_mask);
   dumpbits(&and_mask,nbits);
 }
 printf("randm_mask = %u = ",rmax_mask);
 dumpbits(&rmax_mask,nbits);
 printf("random_max = %u = ",random_max);
 dumpbits(&random_max,nbits);

 free(rand_uint);

}

