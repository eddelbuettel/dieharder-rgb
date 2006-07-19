
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

#include "dieharder.h"
/*
 * Test specific data
 */
#include "sts_monobit.h"

double sts_monobit()
{

 double pks;
 uint ps_save,ts_save;

 /*
  * Do a standard test if -a(ll) is selected.
  * ALSO use standard values if tsamples or psamples are 0
  */
 if(all == YES){
   ts_save = tsamples;
   tsamples = dtest->tsamples_std;
   ps_save = psamples;
   psamples = dtest->psamples_std;
 }
 if(tsamples == 0){
   tsamples = dtest->tsamples_std;
 }
 if(psamples == 0){
   psamples = dtest->psamples_std;
 }
 
 /*
  * Allocate memory for THIS test's ks_pvalues, etc.  Make sure that
  * any missed prior allocations are freed.
  */
 if(ks_pvalue) nullfree(ks_pvalue);
 ks_pvalue  = (double *)malloc((size_t) psamples*sizeof(double));

 /*
  * Reseed FILE random number generators once per individual test.
  * This correctly resets the rewind counter per test.
  */
 if(strncmp("file_input",gsl_rng_name(rng),10) == 0){
   gsl_rng_set(rng,1);
 }

 test_header(dtest);
 /*
  * Any custom test header output lines go here.  They should be
  * used VERY sparingly.
  */

 /*
  * This is the standard test call.
  */
 kspi = 0;  /* Always zero first */
 pks = sample((void *)sts_monobit_test);

 /*
  * Test Results, standard form.
  */
 test_footer(dtest,pks,ks_pvalue,"STS Monobit Test");

 /*
  * Put back tsamples
  */
 if(all == YES){
   tsamples = ts_save;
   psamples = ps_save;
 }

 if(ks_pvalue) nullfree(ks_pvalue);

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

 /*
  * NOTE WELL:  This can also be done by reading in a file!  Note
  * that if -b bits is specified, size will be "more than enough".
  */
 if(verbose == D_RGB_BITDIST || verbose == D_ALL){
   printf("# rgb_bitdist(): Generating %u bits in bitstring",tsamples*sizeof(uint)*8);
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

void help_sts_monobit()
{

 printf("%s",dtest->description);

}
