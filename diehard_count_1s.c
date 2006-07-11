/*
 * See copyright in copyright.h and the accompanying file COPYING
 */

/*
 *========================================================================
 * This is the Diehard Count a Stream of 1's test, rewritten from the
 * description in tests.txt on George Marsaglia's diehard site.
 *
 * Rewriting means that I can standardize the interface to
 * gsl-encapsulated routines more easily.  It also makes this
 * my own code.  Finally, since the C versions Marsaglia provides
 * are the result of f2c running on Fortran sources, they are really
 * ugly code and the rewrite should be much more manageable.
 *
 * Here is the test description from diehard_tests.txt:
 *
 *     This is the COUNT-THE-1's TEST on a stream of bytes.      ::
 * Consider the file under test as a stream of bytes (four per   ::
 * 32 bit integer).  Each byte can contain from 0 to 8 1's,      ::
 * with probabilities 1,8,28,56,70,56,28,8,1 over 256.  Now let  ::
 * the stream of bytes provide a string of overlapping  5-letter ::
 * words, each "letter" taking values A,B,C,D,E. The letters are ::
 * determined by the number of 1's in a byte::  0,1,or 2 yield A,::
 * 3 yields B, 4 yields C, 5 yields D and 6,7 or 8 yield E. Thus ::
 * we have a monkey at a typewriter hitting five keys with vari- ::
 * ous probabilities (37,56,70,56,37 over 256).  There are 5^5   ::
 * possible 5-letter words, and from a string of 256,000 (over-  ::
 * lapping) 5-letter words, counts are made on the frequencies   ::
 * for each word.   The quadratic form in the weak inverse of    ::
 * the covariance matrix of the cell counts provides a chisquare ::
 * test::  Q5-Q4, the difference of the naive Pearson sums of    ::
 * (OBS-EXP)^2/EXP on counts for 5- and 4-letter cell counts.    ::
 *========================================================================
 */


#include "dieharder.h"

double diehard_count_1s_stream()
{

 double *pvalue,pks;
 uint tempsamples;

 /*
  * This is the merest shell to set any test-specific variables, call
  * the main test routine (which fills one or more slots in ks_pvalue[]
  * and increments kspi accordingly), and run a Kuiper Kolmogorov-Smirnov
  * test on the vector of pvalues produced and turn it into a single,
  * cumulative p-value (pks) for the entire test.  If the test parameters
  * are set properly, this will USUALLY yield an unambiguous signal of
  * failure.
  */

 /*
  * If this test is run by itself, we can ignore tsamples.  If it is
  * part of a "standard run", we have to use specific values.  Either
  * way, we have to adjust the sizes of e.g. the list of integers to
  * be generated and sampled, and (re)allocate memory accordingly.
  * Then at the bottom, we have to put it all back.
  */
 if(all == YES){
   tempsamples = tsamples;
   tsamples = 100000;  /* Standard value from diehard */
 }

 if(!quiet){
   help_diehard_count_1s_stream();
   printf("# Random number generator tested: %s\n",gsl_rng_name(rng));
   printf("# Number of rands required is around 3x10^8 for 100 samples.\n");
 }

 kspi = 0;  /* Always zero first */
 pks = sample((void *)diehard_count_1s_stream_test);

 /*
  * Display histogram of ks p-values (optional)
  */
 if(hist_flag){
   histogram(ks_pvalue,psamples,0.0,1.0,10,"p-values");
 }
 printf("# p = %8.6f for diehard_count_1s_stream test (mean) from Kuiper Kolmogorov-Smirnov\n",pks);
 printf("#     test on %u pvalues.\n",kspi);
 if(pks < 0.0001){
   printf("# Generator %s FAILS at 0.01%% for diehard_count_1s_stream.\n",gsl_rng_name(rng));
 }

 /*
  * Put back tsamples
  */
 if(all == YES){
   tsamples = tempsamples;
 }

 return(pks);

}

void diehard_count_1s_stream_test()
{

 int i,j,k;
 Ptest ptest;  /* I think this is a ptest.  We'll see. */

 /*
  * Count a Stream of 1's is very strange.  Basically, take a byte outta
  * the random stream.  Turn it into a digit base 5 (0-4) based on the
  * number of 1's in its binary representation (!).  Create a five digit
  * number (base 5) out of five successive such bytes.  Increment the
  * frequency counter of the referenced slot of a vector with indices that
  * run from 00000 to 44444.  Then at the end we have to do some black magic
  * that I don't understand yet to turn it into a statistic whose expected
  * value we know and hence into a p-value.
  *
  * So, looks like we need several things:
  *   a) A vector (built once at the beginning) that maps base_256(byte) to
  * base_5 ("letter").
  *   b) A simple loop that does e.g. index = (5*index + new_base_5) to
  * effectively shift the base_5 digits left one slot and add the next
  * transformed byte AS AN INTEGER INDEX.  
  *   c) Use index and increment counter slot.
  *   d) Repeat for tsamples (+5) rands -- To do an "overlapping" test
  * (something we really don't need to do any more) we keep doing the
  * left shift + new_base_5 trick and use %625 to discard the leftmost digit.
  *   e) Then (if I understand this correctly) we use the remaing FOUR
  * base_5 digits as an index and increment a SECOND counter slot in the
  * range 0000 to 4444 (base 5).
  *   f) We turn each of these frequency counts into a number, subtract
  * the number from the four digit counts from the number from the five
  * digit counts, and FINALLY use the expected value of this difference and
  * the variance of the difference to create a p-value.
  */

 /*
  * We begin, therefore, by generating a lookup table.  I can even print
  * the generated table and read it in as permanent data.
  */

 

 ks_pvalue[kspi] = ptest.pvalue;
 if(verbose == D_RGB_BITDIST || verbose == D_ALL){
   printf("# diehard_count_1s_stream_freq(): ks_pvalue[%u] = %10.5f\n",kspi,ks_pvalue[kspi]);
 }
 kspi++;

}

void help_diehard_count_1s_stream()
{

 printf("\n\
#==================================================================\n\
#            Diehard \"count_1s_stream\" test (modified).\n\
# Consider the file under test as a stream of bytes (four per   \n\
# 32 bit integer).  Each byte can contain from 0 to 8 1's,      \n\
# with probabilities 1,8,28,56,70,56,28,8,1 over 256.  Now let  \n\
# the stream of bytes provide a string of overlapping  5-letter \n\
# words, each "letter" taking values A,B,C,D,E. The letters are \n\
# determined by the number of 1's in a byte::  0,1,or 2 yield A,\n\
# 3 yields B, 4 yields C, 5 yields D and 6,7 or 8 yield E. Thus \n\
# we have a monkey at a typewriter hitting five keys with vari- \n\
# ous probabilities (37,56,70,56,37 over 256).  There are 5^5   \n\
# possible 5-letter words, and from a string of 256,000 (over-  \n\
# lapping) 5-letter words, counts are made on the frequencies   \n\
# for each word.   The quadratic form in the weak inverse of    \n\
# the covariance matrix of the cell counts provides a chisquare \n\
# test::  Q5-Q4, the difference of the naive Pearson sums of    \n\
# (OBS-EXP)^2/EXP on counts for 5- and 4-letter cell counts.    \n\
#==================================================================\n");

}
