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
 printf("p = %6.3f for sts_runs test from Kuiper Komogorov-Smirnov test on %u pvalues.\n",pks,kspi);
 if(pks < 0.0001){
   printf("Generator %s fails for sts_runs.\n",gsl_rng_name(rng));
 }
 return(pks);

}

void sts_runs_test()
{

 int i,b,t;
 unsigned int nbits,bitstring,value;
 Xtest mtest;
 double pks_monobit,pones,c00,c01,c10,c11;;

 /*
  * Start by running monobit, and quitting if it fails
 pks_monobit = sts_monobit();
 if(pks_monobit < 0.0001) return;
  */

 /*
  * Number of total bits from -t tsamples = size of rand_int[]
  */
 bits = rmax_bits*tsamples;
 mtest.x = 0.0;
 strncpy(mtest.testname,"sts_runs",128);
 strncpy(mtest.rngname,gsl_rng_name(rng),128);

 /*
  * Create entire bitstring to be tested
  */
 for(t=0;t<tsamples;t++){
   rand_int[t] = gsl_rng_get(rng);
 }

 /*
  * Fill vector of "random" integers with selected generator.
  * NOTE WELL:  This can also be done by reading in a file!
  */
 pones = 0.0;
 c00 = 0.0;
 c01 = 0.0;
 c10 = 0.0;  /* Equal to c01 by obvious periodic symmetry */
 c11 = 0.0;
 for(b=0;b<bits;b++){
   /*
    * This gets the integer value of the ntuple at index position
    * n in the current bitstring, from a window with cyclic wraparound.
    */
   value = get_bit_ntuple(rand_int,tsamples,2,b);
   switch(value){
     case 0:   /* 00 no new ones */
       c00++;
       break;
     case 1:   /* 01 no new ones */
       c01++;
       mtest.x++;
       break;
     case 2:   /* 10 one new one (from the left) */
       c10++;
       mtest.x++;
       pones++;
       break;
     case 3:   /* 11 one new one (from the left) */
       c11++;
       pones++;
       break;
   }
   if(verbose == D_STS_RUNS || verbose == D_ALL){
     printf("# sts_runs(): mtest.x = %f, pone = %f\n",mtest.x,pones);
   }
 }
 /*
  * form the probability of getting a one in the entire sample
  */
 pones /= (double) tsamples*rmax_bits;
 c00 /= (double) tsamples*rmax_bits;
 c01 /= (double) tsamples*rmax_bits;
 c10 /= (double) tsamples*rmax_bits;
 c11 /= (double) tsamples*rmax_bits;
 /* printf(" p = %f c00 = %f c01 = %f c10 = %f c11 = %f\n",pones,c00,c01,c10,c11); */
 mtest.y = 2.0*bits*pones*(1.0-pones);
 mtest.sigma = 2.0*sqrt(2.0*bits)*pones*(1.0-pones);
 Xtest_eval(&mtest);
 ks_pvalue[kspi] = mtest.pvalue;
 if(verbose == D_STS_MONOBIT || verbose == D_ALL){
   printf("# sts_monobit(): ks_pvalue[%u] = %10.5f\n",kspi,ks_pvalue[kspi]);
 }
 kspi++;
 
}

