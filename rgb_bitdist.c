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

 unsigned int i,j,k,l,m,n;   /* loop indices */
 unsigned int value;         /* value of n-tuple (as an integer) */
 double *pvalue,pks,p_binomial;  /* probabilities */
 Btest *btest;               /* A vector of binomial test bins */

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
 nbits = bits;


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
 while(nsize*rmax_bits < nbits) size++;

 


}

