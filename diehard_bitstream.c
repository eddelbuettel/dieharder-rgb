/*
 * See copyright in copyright.h and the accompanying file COPYING
 */

/*
 *========================================================================
 * This is the Diehard OPSO test, rewritten from the description
 * in tests.txt on George Marsaglia's diehard site.
 *
 * Rewriting means that I can standardize the interface to
 * gsl-encapsulated routines more easily.  It also makes this
 * my own code.  Finally, since the C versions Marsaglia provides
 * are the result of f2c running on Fortran sources, they are really
 * ugly code and the rewrite should be much more manageable.
 *
 * Here is the test description from diehard_tests.txt:
 *
 *                   THE BITSTREAM TEST                          ::
 * The file under test is viewed as a stream of bits. Call them  ::
 * b1,b2,... .  Consider an alphabet with two "letters", 0 and 1 ::
 * and think of the stream of bits as a succession of 20-letter  ::
 * "words", overlapping.  Thus the first word is b1b2...b20, the ::
 * second is b2b3...b21, and so on.  The bitstream test counts   ::
 * the number of missing 20-letter (20-bit) words in a string of ::
 * 2^21 overlapping 20-letter words.  There are 2^20 possible 20 ::
 * letter words.  For a truly random string of 2^21+19 bits, the ::
 * number of missing words j should be (very close to) normally  ::
 * distributed with mean 141,909 and sigma 428.  Thus            ::
 *  (j-141909)/428 should be a standard normal variate (z score) ::
 * that leads to a uniform [0,1) p value.  The test is repeated  ::
 * twenty times.                                                 ::
 *
 *                         Comment
 * The tests BITSTREAM, OPSO, OQSO and DNA are all closely related.
 * They all measure global distribution properties of extended
 * bit combinations in projections of varying dimension.  I
 * believe that they are closely related to the Knuth tests that
 * will code when diehard is finished that look for N-dimensional
 * hyperplanes (known to be a problem with pretty much all
 * linear congruential generators, for example).  I would LIKE to
 * have a series of tests like this that is more systematic and
 * not quite so creatively named to emphasize the relationship
 * between the tests, and I suspect that either Knuth or a homebrew
 * RGB test (like bitdist, but looking at projections of different
 * dimension) would be the way to go here.  Note that this test is
 * also closely related to rgb_bitdist, which measures (as much as
 * possible) the uniformity of the distribution of ntuples of bits.
 *
 * However, ALL rng's fail rgb_bitdist long before they get to
 * 20 bit strings -- the distributions of ntuples stop being random
 * (uniform to the extent required by true randomness) by the time you
 * hit six bit ntuples even with a good rng.  This is an area of
 * future research, as a "proper" test procedure -- in my opinion --
 * would a) determine test dependencies -- if test X is failed, then
 * test Y will always be failed, for example (more generally, the
 * covariance of test failure); b) determine a SYSTEMATIC series of
 * tests that make specific, useful statements about where and how
 * a failure occurs -- basically, tests that determine the MOMENT of
 * the failures in a generalized sense in a suitable series.
 *
 * There may well yet remain specific "oddball" tests that are failed
 * even when moment tests are passed -- in principle, of course, one
 * could test EVERY known distribution that can be generated from
 * uniform deviates and measure the extent to which the generated
 * distribution differs from the ideal -- but I suspect that functional
 * analysis applied to the underlying distributions ultimately connects
 * most failures to an observed failure pattern in a moment study via
 * projective covariance in functional expansion space.
 *========================================================================
 */


#include "dieharder.h"

#define M 1048576

double diehard_bitstream()
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
  * This test requires a fixed number of tsamples, alas.  Deviation
  * not permitted, whether or not we are running a single test and
  * trying to set -t whatever.
  */
 tempsamples = tsamples;
 tsamples = 2097152;  /* Standard value from diehard */

 if(!quiet){
   help_diehard_bitstream();
   printf("# Random number generator tested: %s\n",gsl_rng_name(rng));
   printf("# Number of rands required is around 2^26 for 100 samples.\n");
 }

 kspi = 0;  /* Always zero first */
 pks = sample((void *)diehard_bitstream_test);

 /*
  * Display histogram of ks p-values (optional)
  */
 if(hist_flag){
   histogram(ks_pvalue,psamples,0.0,1.0,10,"p-values");
 }
 printf("# p = %8.6f for diehard_bitstream test from Kuiper Kolmogorov-Smirnov\n",pks);
 printf("#     test on %u pvalues.\n",kspi);
 if(pks < 0.0001){
   printf("# Generator %s FAILS at 0.01%% for diehard_bitstream.\n",gsl_rng_name(rng));
 }

 /*
  * Put back tsamples
  */
 tsamples = tempsamples;

 return(pks);

}

void diehard_bitstream_test()
{

 uint i,t,boffset;
 Xtest ptest;
 char *w;

 /*
  * p = 141909, with sigma 428, for tsamples 2^21 20 bit ntuples.
  * tsamples cannot be varied unless one figures out the actual
  * expected "missing works" count as a function of sample size.  SO:
  *
  * ptest.x = number of "missing ntuples" given 2^21 trials
  * ptest.y = 141909
  * ptest.sigma = 428
  */
 ptest.y = 141909.0;
 ptest.sigma = 428.0;

 /*
  * We now make tsamples measurements, as usual, to generate the
  * missing statistic.  The easiest way to proceed is to just increment
  * a vector of length 2^20 using the generated ntuples as the indices
  * of the slot being incremented.  Then we zip through the vector
  * counting the remaining zeros.
  */

 w = (char *)malloc(M*sizeof(char));
 memset(w,0,M*sizeof(char));

/*
 printf("w is allocated and zero'd\n");
 printf("About to generate %u samples\n",tsamples);
 */

 /*
  * To minimize the number of rng calls, we use each j and k mod 32
  * to determine the offset of the 10-bit long string (with
  * periodic wraparound) to be used for the next iteration.  We
  * therefore have to "seed" the process with a random k.
  */
 i = gsl_rng_get(rng);
 for(t=0;t<tsamples;t++){
   /*
    * Get a 20-bit ntuple as an index into w.  Use each (presumed
    * random) value to determine the uint offset for the next
    * 20-bit window.
    */
   boffset = i%32;
   i = gsl_rng_get(rng);
   i = get_bit_ntuple(&i,1,20,boffset);
   w[i]++;
 }

 /*
  * Now we count the holes, so to speak
  */
 ptest.x = 0;
 for(i=0;i<M;i++){
   if(w[i] == 0){
     ptest.x++;
     /* printf("ptest.x = %f  Hole: w[%u] = %u\n",ptest.x,i,w[i]); */
   }
 }
 if(verbose == D_DIEHARD_BITSTREAM || verbose == D_ALL){
   printf("%f %f %f\n",ptest.y,ptest.x,ptest.x-ptest.y);
 }

 Xtest_eval(&ptest);
 ks_pvalue[kspi] = ptest.pvalue;

 if(verbose == D_DIEHARD_BITSTREAM || verbose == D_ALL){
   printf("# diehard_bitstream(): ks_pvalue[%u] = %10.5f\n",kspi,ks_pvalue[kspi]);
 }

 kspi++;

 /*
  * Don't forget to free or we'll leak.  Hate to have to wear
  * depends...
  */
 free(w);

}

void help_diehard_bitstream()
{

 printf("\n\
#==================================================================\n\
#                Diehard \"BITSTREAM\" test.\n\
# The file under test is viewed as a stream of bits. Call them  \n\
# b1,b2,... .  Consider an alphabet with two \"letters\", 0 and 1 \n\
# and think of the stream of bits as a succession of 20-letter  \n\
# \"words\", overlapping.  Thus the first word is b1b2...b20, the \n\
# second is b2b3...b21, and so on.  The bitstream test counts   \n\
# the number of missing 20-letter (20-bit) words in a string of \n\
# 2^21 overlapping 20-letter words.  There are 2^20 possible 20 \n\
# letter words.  For a truly random string of 2^21+19 bits, the \n\
# number of missing words j should be (very close to) normally  \n\
# distributed with mean 141,909 and sigma 428.  Thus            \n\
#  (j-141909)/428 should be a standard normal variate (z score) \n\
# that leads to a uniform [0,1) p value.  The test is repeated  \n\
# twenty times.                                                 \n\
# \n\
# Note that of course we do not \"restart file\", when using gsl \n\
# generators, we just crank out the next random number. \n\
# We also do not bother to overlap the words.  rands are cheap. \n\
# Finally, we repeat the test (usually) more than twenty time.\n\
#==================================================================\n");

}
