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

int sts_runs()
{

 int i,j,k;
 unsigned int nbits,firstbit,lastbit,thisbit;
 Xtest mtest;

 /*
  * Start by running monobit, and quitting if it fails
  */
 if(!sts_monobit()) return(0);

 /*
  * Number of total bits (from EITHER -b bits OR -s size, -b overrides -s)
  */
 if(bits){
   nbits = bits;
 } else {
   nbits = 8*sizeof(unsigned int)*size;
 }
 mtest.y = nbits/2.0;
 mtest.sigma = sqrt((double)nbits/2.0);
 mtest.npts = nbits;
 strncpy(mtest.testname,"sts_runs",128);
 strncpy(mtest.rngname,gsl_rng_name(random),128);

 /*
  * Fill vector of "random" integers with selected generator.
  * NOTE WELL:  This can also be done by reading in a file!
  */
 for(j=0;j<size;j++) rand_int[j] = gsl_rng_get(random);

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

 if(mtest.pvalue<0.01){
   return(0);
 } else {
   return(1);
 }

}

