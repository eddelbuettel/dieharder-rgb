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


static char bitdist_description[]="
#==================================================================
#                        rgb_bitdist
# Here we create a vector of length bits, and increment each bit
# slot by the contents of the bit (0 or 1).  The statistic
# in the end is an extension of rgb_persist, but sensitive to cases
# where a bit changes but is biased towards either 0 or 1.  We basically
# compute the expected number of bits per slot (samples/2) and the
# sigma of same (sqrt(samples)/2) and form a chisq of the measured 
# deviations in in the vector and turn it into a p-value.
#
# However, this p-value is NOT the primary result of interest from
# this test -- far more interesting should be the actual output
# frequency histogram, which should be flat within sqrt(samples) noise.
# Whereever it is NOT flat it provides us with evidence of non-randomness,
# quite possibly in ways that on average satisfy the expectations for
# the total number of bits (one can systematically have balanced
# surfeits and deficits of ones in certain bitslots).
#
#==================================================================
";
										    

void rgb_bitdist()
{

 int i,j,k,nbits,npairs;
 int first_bit,current_bit,pairmask = 3,current_pair;
 Ntest btest;   /* For bitlevel binomial test results */
 Ntest *ctest;   /* For bitpair binomial test results */

 /*
  * btest.x[k] is an accumulator to be used to count the frequency with
  *   which we get k 1's in num_bits, over the m runs.
  * btest.y[k] contains the expected binomial result, samples*p_1
  * btest.sigma[k] contains the standard binomial error of btest.y[k],
  *   sqrt(samples)*p_1*(1-p_1) (in both cases with p_0 = p_1 = 0.5).
  *
  * We start by creating all three and filling y[k] and sigma[k]
  * to support the formation of chisq as we run.
  *
  * We use rand_int as usual, and the standard code for extracting the
  * output from particular bits in series.
  */

 nbits = bits;
 Ntest_create(&btest,nbits,"rgb_bitdist",gsl_rng_name(random));
 for(k=0;k<nbits;k++){
   btest.x[k] = 0.0;
   btest.y[k] = (double) samples*0.5;
   btest.sigma[k] = sqrt((double) samples)*0.25;
 }

 /*
  * We're going to test every successive pair AND wrap the final bit
  * back to the first bit for the final bitpair, so there are as many
  * bitpairs as their are bits.  There are exactly four distinct binary
  * bitpairs, 00, 01, 10, 11 (0-3).  We'll have to do the malloc here
  * since Ntest_create wasn't designed to do a vector of Ntests...
  *
  * Note that each pair has p=0.25, and (1-p) = 0.75.
  */
 npairs = 4;
 ctest = (Ntest *) malloc(npairs*sizeof(Ntest));
 for(i = 0;i < npairs;i++){
   Ntest_create(&ctest[i],nbits,"rgb_bitpairdist",gsl_rng_name(random));
   for(k=0;k<nbits;k++){
     ctest[i].x[k] = 0.0;
     ctest[i].y[k] = (double) samples*0.25;
     ctest[i].sigma[k] = sqrt((double) samples)*0.25*0.75;
   }
 }

 for(i=0;i<samples;i++){
   if(reseed_flag){
     seed = random_seed();
     gsl_rng_set(random,seed);
   }

   /*
    * Fill rand_int with a string of random numbers long enough to
    * provide all requested bits (which could be quite long -- we
    * have no real restrictions on number of bits in this test).
    */
   for(j=0;j<size;j++) {
     rand_int[j] = gsl_rng_get(random);
     if(verbose){
       printf("rand_int[%d] = %u\n",j,rand_int[j]);
       dumpbits(&rand_int[j],32);
     }
   }
   current_pair = 0;
   for(j=0;j<nbits;j++){
     current_bit = get_bit(j);
     /* Do the single bit histogram */
     btest.x[j] += current_bit;
     /*
      * Save the first bit for periodic wraparound at the end.  Also
      * initialize current pair with the first bit.
      */
     if(j==0) {
       first_bit = current_bit;
       current_pair = current_bit;
     } else {
     /*
      * For all bits AFTER the first bit, we left-shift current_pair,
      * add the current bit, & with pairmask (discarding bits that
      * have shifted out of the first two slots).  Note that this
      * general methodology can be used for ANY number of bits, provided
      * that we dynamically generate the mask.  Our eventual design
      * goal here is to put all of this in a single loop over the
      * size of the bit subsequence being studied, although we'll have to
      * work a bit to manage the periodic wraparound at the end for
      * the general case.
      *
      * current pair ends up being the INDEX of the ctest to be incremented.
      */
       current_pair <<= 1;
       current_pair += current_bit;
       current_pair = current_pair & pairmask;
       ctest[current_pair].x[j-1]++;
     }
   }
   /*
    * Finally, we have to deal with the last/first bit.
    */
   current_pair <<= 1;
   current_pair += first_bit;
   current_pair = current_pair & pairmask;
   ctest[current_pair].x[j-1]++;
 }
 /*
  * RESULTS:  We output chisq, the number of points compared, and a p-value
  * that is the probability of getting this result if the random number
  * generator is good.  Hence values less than 0.01 suggest a "bad" rng.
  * Results MUCH less than 0.01 should Never Happen and we'll say so.
  * If I understand things correctly, p-value from chisq is
  *   p = incomplete_gamma(N/2,chisq/2) (gsl Q version, not P version).
  */
 Ntest_eval(&btest);
 if(!quiet) printf("%s",bitdist_description);
 /*
  * Then binomial test results from the same data.
  */
 Ntest_conclusion(&btest);
 printf("#==================================================================\n");

 printf("# %10s   %12s    %12s  %12s  %10s\n","Test Name","Generator",
   "# samples","# of bits","p-value");
 printf(" %13s  %12s  %12d  %11d  %13.7f\n",btest.testname,btest.rngname,
   samples,nbits,btest.pvalue);

 Ntest_destroy(&btest);

 for(i=0;i<=3;i++){
   Ntest_eval(&ctest[i]);
   Ntest_conclusion(&ctest[i]);
   Ntest_destroy(&ctest[i]);
 }

 free(ctest);

}

