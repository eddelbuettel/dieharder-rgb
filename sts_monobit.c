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

double sts_monobit()
{

 int i,j,k;
 double pks;

 if(!quiet){
   printf("#==================================================================\n");
   printf("#                 sts_monobit Test Description\n");
   printf("# Very simple.  Counts the 1 bits in a long string of random uints.\n");
   printf("# Compares to expected number, generates a p-value directly from\n");
   printf("# erfc().  Very effective at revealing overtly weak generators;\n");
   printf("# Not so good at determining where stronger ones eventually fail.\n");
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
 pks = sample((void *)sts_monobit_test);
 printf("p = %6.3f for sts_monobit test from Kuiper Kolmogorov-Smirnov test on %u pvalues.\n",pks,cntrl.rgb_bitdist_ntuple,kspi);
 if(pks < 0.0001){
   printf("Generator %s fails for sts_monobit.\n",gsl_rng_name(rng));
 }
 return(pks);

}

void sts_monobit_test()
{

 int i,b,bsamples,bit;
 uint bitstring,blens,nbits;
 Xtest mtest;

 /*
  * mtest.x contains n_1's - n_0's = n_1's - (nbits - n_1's)
  *   or mtest.x = 2*n_1's - nbits;
  * mtest.y is the number we expect (2*n_1's = nbits, so mtest.y = 0)
  * mtest.sigma is the expected error, 1/sqrt(nbits).
  *
  * Note that the expected distribution is the "half normal" centered
  * on 0.0.  I need to figure out if this is responsible for the 1/sqrt(2)
  * in the pvalue = erfc(|y - x|/(sqrt(2)*sigma)).
  *
  * Another very useful thing to note is that we don't really need to
  * do "samples" here.  Or rather, we could -- for enough bits, the
  * distribution of means should be normal -- but we don't.
  *
  */
 /*
  * The number of bits per random integer tested.
  */
 blens = rmax_bits;
 nbits = blens*tsamples;
 mtest.y = 0.0;
 mtest.sigma = sqrt((double)nbits);
 mtest.npts = nbits;
 strncpy(mtest.testname,"sts_monobit",128);
 strncpy(mtest.rngname,gsl_rng_name(rng),128);

 /*
  * NOTE WELL:  This can also be done by reading in a file!  Note
  * that if -b bits is specified, size will be "more than enough".
  */
 if(verbose == D_RGB_BITDIST || verbose == D_ALL){
   printf("# rgb_bitdist(): Generating %u bits in rand_int[]\n",size*sizeof(uint)*8);
 }
 mtest.x = 0;
 for(i=0;i<tsamples;i++) {
   bitstring = gsl_rng_get(rng);
   if(verbose == D_RGB_BITDIST || verbose == D_ALL){
     printf("# rgb_bitdist(): rand_int[%d] = %u = ",i,bitstring);
     dumpbits(&bitstring,8*sizeof(uint));
   }
   for(b=0;b<blens;b++){
     /*
      * This gets the integer value of the ntuple at index position
      * n in the current bitstring, from a window with cyclic wraparound.
      */
     bit = get_bit_ntuple(&bitstring,1,1,b);
     mtest.x += bit;
   }
 }

 mtest.x = 2*mtest.x - nbits;
 if(verbose == D_RGB_BITDIST || verbose == D_ALL){
   printf("mtext.x = %10.5f  mtest.sigma = %10.5f\n",mtest.x,mtest.sigma);
 }
 Xtest_eval(&mtest);
 ks_pvalue[kspi] = mtest.pvalue;
 if(verbose == D_STS_MONOBIT || verbose == D_ALL){
   printf("# sts_monobit(): ks_pvalue[%u] = %10.5f\n",kspi,ks_pvalue[kspi]);
 }
 kspi++;
 
}

