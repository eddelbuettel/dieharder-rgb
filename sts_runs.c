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

void sts_runs()
{

 int i,j,k;
 unsigned int num_ones,num_zeros,nbits;
 unsigned int mask;
 double onefrac;
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
  * Number of total bits:
  */
 nbits = 8*sizeof(unsigned int)*size;
 mtest.y = 0.0;
 mtest.sigma = sqrt((double)nbits);
 strncpy(mtest.testname,"sts_monobit",128);
 strncpy(mtest.rngname,gsl_rng_name(random),128);

 /*
  * Fill vector of "random" integers with selected generator.
  * NOTE WELL:  This can also be done by reading in a file!
  */
 for(j=0;j<size;j++) rand_int[j] = gsl_rng_get(random);
 /*
  * Count the 1 bits and the 0 bits
  * We do this by masking out each bit slot, anding the mask with
  * the rand_int and accumulating the ones.  I think.
  */
 mtest.x = 0;
 if(verbose) printf("# Tested bitstring:\n#");
 for(j=0;j<size;j++){
   mask = 1;
   for(k=0;k<32;k++){
     if((mask & rand_int[j]) != 0) {
       mtest.x++;
       if(verbose) printf("1");
     } else {
       if(verbose) printf("0");
     }
     mask = mask<<1;
   }
 }
 if(verbose) printf("\n");
 mtest.x = 2*mtest.x - nbits;
 Xtest_eval(&mtest);
 Xtest_conclusion(&mtest);

}

