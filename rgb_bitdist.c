/*
 * $Id$
 *
 * See copyright in copyright.h and the accompanying file COPYING
 * See also accompanying file STS.COPYING
 *
 */

/*
 *========================================================================
 * This is a test that checks to see if the rng generates bit patterns
 * (n-tuples) that are distributed correctly (binomially).  For example,
 * for 2-tuples (bit pairs) there are four possibilities: 00, 01, 10, 11.
 * Each should occur with a probability of 1/4, hence the count of (say) K
 * 00 bitpairs out of N trials should be distributed (over M samples)
 * according to the binomial probability p = binomial(N,K,0.25) -- that
 * is, the expected count for k 00's is M*p(N,K,0.25).
 *
 * This test should be more sensitive than just doing a large number of
 * trials and aggregating all the 00's over all the samples and comparing
 * the result to the expected mean, as there can be distributions that
 * have the expected mean of 0.25 that are >>not<< binomially distributed.
 *
 * By making a single program capable of doing any n-tuple, we gain
 * immediate benefit.  Both STS and Diehard, for example, contain
 * tests that validate bit distribution frequencies; however, they typically
 * do so for specific n-tuples, e.g. 5 in several diehard tests.  There
 * is nothing terribly special about 5, and no point in looking too
 * hard at e.g. intervals between patterns at 5, since testing pure
 * frequency at 6 bits simultaneously ensures that all 6 bit patterns have
 * the correct frequency (since they are sub-patterns of 6 bit patterns
 * that have the correct frequency, and if they had the INcorrect frequency
 * the six-bit patterns would as well by inheritance) and (less obviously,
 * but I'm pretty sure I can prove it) that intervals between the five
 * bit patterns must also be correct IF the test is satisfied robustly
 * for various arbitrary test string lengths and sampling counts.
 *
 * Anyway, I'm giving this one a shot as it may end up being a one-size
 * fits all tool for validating bitlevel randomness for any generator,
 * up to some n-tuple.  I expect that 2004/2005 computers will easily
 * be able to validate through n=8 without really warming up or taking
 * terribly long (validating the frequency and distribution for e.g.
 * the entire ascii alphabet at the byte level) and MAY be able to do
 * actual frequency validation on 16-bit integers.  Note that it only takes
 * a few seconds to generate millions of 32 bit integers with most of
 * the GSL routines.  To get statistically valid results, one needs to
 * accumulate order of 100 trials per outcome, or test order of millions
 * of integers and accumulate the results in an outcome vector 64K long.
 * both of these are well within reach using at most hours of CPU time,
 * and partitioned on a beowulf might take only minutes or even seconds.
 *========================================================================
 */

#include "rand_rate.h"

double rgb_bitdist(int ntuple)
{

 unsigned int b,t,i,j,k,l,m,n;   /* loop indices */
 unsigned int value;         /* value of ntuple (as an integer) */
 unsigned int *count;        /* count of any ntuple per bitstring */
 unsigned int ntuple_max;    /* largest ntuple value */
 double *pvalue,pks,ntuple_prob,pbin;  /* probabilities */
 Btest *btest;               /* A vector of binomial test bins */

 if(!quiet){
   printf("#==================================================================\n");
   printf("#                 rgb_bitdist Test Description\n");
   printf("# Accumulates the frequencies of all n-tuples of bits in a list\n");
   printf("# of random integers and compares the distribution thus generated\n");
   printf("# with the theoretical (binomial) histogram, forming chisq and the\n");
   printf("# associated p-value.\n");
   printf("#==================================================================\n");
 }


 /*
  * Let's see what gsl_ran_binomial_pdf(k,p,n) (for k hits in n trials)
  * returns for large n -- I have difficulty with the raw formula
  * around n = 128, but they may be able to do better.  The following
  * code fragment will prevent over/underflow problems if they can't.
  if(bits > 128){
   nbits = bits = 128;
 } else {
   nbits = bits;
 }
 */


 /*
  * OK, a sample is a string of nbits drawn from the rng, with nbits
  * typically 32 and almost certainly <= 128.  The rng, however,
  * may well only return <32 significant bits (one has to look at
  * RAND_MAX for the generator).  MANY generators only return 31 bits;
  * a few old ones return as few as 16.  We therefore MUST determine
  * the number of rands to generate that will minimally hold a sample's
  * worth of significant/random bits.  rmax_bits should be the number
  * of bits the current rng can generate, so:
  */
 size = 0;
 while(size*rmax_bits < bits) size++;

 /*
  * Largest ntuple value is 2^ntuple-1 (they range from 0 to 2^ntuple - 1).
  * However, this is used to size count and limit loops, so we use
  * 2^ntuple and start indices from 0 as usual.
  */
 ntuple_max = pow(2,ntuple);

 /*
  * Allocate memory for ntuple_max vector of Btest structs and counts.
  */
 btest = (Btest *)malloc(ntuple_max*sizeof(Btest));
 count = (uint *)malloc(ntuple_max*sizeof(uint));

 /*
  * Create vector for bits+1 bins in each btest.  This may not be right;
  * we'll just have to see.  The basic probability for any ntuple is
  * just one over the number of ntuples (equal weight).  We are testing
  * the cumulated number of each ntuple pattern drawn bits times against
  * the binomial prediction, tsamples times.  If that makes sense.
  *
  * To help it make sense, suppose we look at bit 2-tuple 01 in a string
  * of 128 bits.  We count (moving the offset one bit at a time through
  * the string with cyclic wraparound) and find that it occurs 30 times.
  * We therefore increment btest[1][30].  We test a second string, and
  * find that 01 occurs 33 times, so we increment btest[1][33].  In the
  * same two strings we might have counted and incremented 32 00's twice
  * (so btest[0][32] = 2), and counted btest[2][31] = 1, btest[2][32] = 1,
  * btest[3][34] = 1 and btest[3][38] = 1.  We continue for tsamples
  * strings, at which time we SHOULD have a nice histogram of the frequency
  * 00, 01, 10 and 11 occur in tsamples samplings of 128 bit pairs.
  * The expected value for btest[0][32] should equal:
  *   tsamples*gsl_ran_binomial_pdf(32,0.25,128)
  * with a corresponding sigma.  Etc.
  */
 ntuple_prob = 1.0/(double)ntuple_max;
 for(n=0;n<ntuple_max;n++){
   Btest_create(&btest[n],bits+1,"rgb_binomial",gsl_rng_name(rng));
   for(b=0;b<=bits;b++){
     pbin = tsamples*gsl_ran_binomial_pdf(k,ntuple_prob,bits);
     btest[n].x[b] = 0.0;
     btest[n].y[b] = tsamples*pbin;
     btest[n].sigma[b] = sqrt(btest[b].y[b]*(1.0 - pbin));
   }
 }

 /*
  * Now we check tsamples bitstrings of bits in length,
  * counting the 1's.  At the end we increment the result histogram
  * with the bitcount as an index as a trial that generated that
  * bitcount.
  */
 for(t=0;t<tsamples;t++){

   /*
    * Fill vector of "random" integers with selected generator.
    * NOTE WELL:  This can also be done by reading in a file!
    */
   if(verbose){
     printf("Generating %u bits in rand_int[]\n",size*sizeof(uint)*8);
   }
   for(i=0;i<size;i++) {
     rand_int[i] = gsl_rng_get(rng);
     if(verbose){
       printf("rand_int[%d] = %u = ",i,rand_int[i]);
       dumpbits(&rand_int[i],8*sizeof(uint));
     }
   }


   /*
    * Here is where we do the actual counting of the ntuples.
    */
   if(verbose){
     printf("# Counting ntuples in bitstring: ");
   }

   /*
    * Clear the count vector for each ntuple
    */
   for(n=0;n<ntuple_max;n++){
     count[n] = 0;
   }
   for(b=0;b<bits;b++){
     /*
      * This gets the integer value of the ntuple at index position
      * n in the current bitstring, from a window with cyclic wraparound.
      */
     value = get_bit_ntuple((void *)rand_int,size,ntuple,b);
     /*
      * We increment the count of this ntuple for this string of bits
      */
     count[value]++;
     if(verbose){
       printf("count[%u] = %u\n",value,count[value]);
     }
   }
   /*
    * Increment the counter for the resulting numbers of patterns.
    */
   btest[value].x[count[value]]++;
   if(verbose){
     printf("\n# Sample %u: btest[%u].x[%u] = %u\n",t,value,count[value],(uint)btest[value].x[count[value]]);
   }
 }

}

