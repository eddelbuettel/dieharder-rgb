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

double sts_runs()
{

 int i,j,k;
 double pks;

 if(!quiet){
   printf("#==================================================================\n");
   printf("#                    sts_runs Test Description\n");
   printf("# Counts the total number of 0 runs + total number of 1 runs across\n");
   printf("# a sample of bits.  Note that a 0 run must begin with 10 and end\n");
   printf("# with 01.  Note that a 1 run must begin with 01 and end with a 10.\n");
   printf("# This test, run on a bitstring with cyclic boundary conditions, is\n");
   printf("# absolutely equivalent to just counting the 01 + 10 bit pairs.\n");
   printf("# It is therefore totally redundant with but not as good as the\n");
   printf("# rgb_bitdist() test for 2-tuples, which looks beyond the means to the\n");
   printf("# moments, testing an entire histogram  of 00, 01, 10, and 11 counts\n");
   printf("# to see if it is binomially distributed with p = 0.25.\n");
   printf("#\n");
   printf("# random number generator: %s\n",gsl_rng_name(rng));
   printf("# p-samples = %u   number of sampled ints/test = %u\n",psamples,tsamples);
   printf("#==================================================================\n");
 }

 /*
  * This part should ALMOST be standardized enough to make a
  * single unified call.  For the moment we'll do one per test, as each
  * test has a few quirks that would otherwise have to go in e.g. work(),
  * making it complicated.
  */
 kspi = 0;  /* Always zero first */
 pks = sample((void *)sts_runs_test);
 printf("p = %6.3f for sts_runs test from Kuiper Komogorov-Smirnov test on %u pvalues.\n",pks,cntrl.rgb_bitdist_ntuple,kspi);
 if(pks < 0.0001){
   printf("Generator %s fails for sts_runs.\n",gsl_rng_name(rng));
 }
 return(pks);

}

void sts_runs_test()
{

 int i,j,k,t;
 unsigned int nbits,firstbit,lastbit,thisbit;
 Xtest mtest;

 /*
  * Start by running monobit, and quitting if it fails
  */
 if(sts_monobit() < 0.0001) return(0);

 /*
  * Number of total bits (from EITHER -b bits OR -s size, -b overrides -s)
  */
 nbits = rmax_bits*tsamples;
 mtest.y = nbits/2.0;
 mtest.sigma = sqrt((double)nbits/2.0);
 mtest.npts = nbits;
 strncpy(mtest.testname,"sts_runs",128);
 strncpy(mtest.rngname,gsl_rng_name(rng),128);

 /*
  * Fill vector of "random" integers with selected generator.
  * NOTE WELL:  This can also be done by reading in a file!
  */
 for(j=0;j<size;j++) rand_int[j] = gsl_rng_get(rng);

 if(verbose) printf("# Tested bitstring:\n#");
 /*
  * start by initializing lastbit and firstbit.
  */
 lastbit = firstbit = get_bit(0);
 if(firstbit) {
   if(verbose) printf("1");
 } else {
   if(verbose) printf("0");
 }
 mtest.x = 0;
 for(j=1;j<nbits;j++){
   thisbit = get_bit(j);
   /* For every 01 pair, there is a 10 pair. See NOTES */
   if(lastbit == 0 && thisbit == 1) {
     mtest.x += 2;
   }
   if(thisbit) {
     if(verbose) printf("1");
   } else {
     if(verbose) printf("0");
   }
   lastbit = thisbit;
 }
 /*
  * Now we handle cyclic wraparound to get exactly n counts.
  */
 if(lastbit == 0 && firstbit == 1) mtest.x += 2;
 if(verbose) printf("\n");
 Xtest_eval(&mtest);
 Xtest_conclusion(&mtest);

 printf("# Full sts_runs test summary:\n");
 printf("#==================================================================\n");
 printf("# %11s  %12s  %12s  %12s  %8s\n","Test Name","Generator",
   "# samples","# of bits","p-value");
 printf("  %11s  %12s  %12d  %11d  %8.4f\n",mtest.testname,mtest.rngname,
   psamples,nbits,mtest.pvalue);

 if(mtest.pvalue<0.01){
   return(0);
 } else {
   return(1);
 }

}

