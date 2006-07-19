/*
 * $Id$
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *
 */

/*
 *========================================================================
 * This is the Diehard QPSO test, rewritten from the description
 * in tests.txt on George Marsaglia's diehard site.
 *
 *     OQSO means Overlapping-Quadruples-Sparse-Occupancy        ::
 * The test OQSO is similar, except that it considers 4-letter ::
 * words from an alphabet of 32 letters, each letter determined  ::
 * by a designated string of 5 consecutive bits from the test    ::
 * file, elements of which are assumed 32-bit random integers.   ::
 * The mean number of missing words in a sequence of 2^21 four-  ::
 * letter words,  (2^21+3 "keystrokes"), is again 141909, with   ::
 * sigma = 295.  The mean is based on theory; sigma comes from   ::
 * extensive simulation.                                         ::
 *
 * Note 2^21 = 2097152
 * The tests BITSTREAM, OPSO, OQSO and DNA are all closely related.
 *========================================================================
 */


#include "dieharder.h"
/*
 * Test specific data
 */
#include "diehard_oqso.h"

double diehard_oqso()
{

 double pks;
 uint ps_save,ts_save;

 /*
  * Do a standard test if -a(ll) is selected.
  * ALSO use standard values if tsamples or psamples are 0
  *
  * MUST use 2^21 = 2097152 diehard standard value, no choice.
  */
 ts_save = tsamples;
 tsamples = dtest->tsamples_std;
 if(all == YES){
   ps_save = psamples;
   psamples = dtest->psamples_std;
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
 printf("# Number of rands required is around 2^21 per psample.\n");
 if(overlap){
   printf("# Using overlapping samples (diehard).\n");
 } else {
   printf("# Using non-overlapping samples (default).\n");
 }

 /*
  * This is the standard test call.
  */
 tsamples = 2097152;  /* Standard value from diehard */
 kspi = 0;  /* Always zero first */
 pks = sample((void *)diehard_oqso_test);


 /*
  * Test Results, standard form.
  */
 test_footer(dtest,pks,ks_pvalue,"Diehard OQSO Test");

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

void diehard_oqso_test()
{

 uint i,j,k,l,i0,j0,k0,l0,t,boffset;
 Xtest ptest;
 char ****w;

 /*
  * p = 141909, with sigma 295, FOR tsamples 2^21 2 letter words.
  * These cannot be varied unless one figures out the actual
  * expected "missing works" count as a function of sample size.  SO:
  *
  * ptest.x = number of "missing words" given 2^21 trials
  * ptest.y = 141909
  * ptest.sigma = 295
  */
 ptest.y = 141909.0;
 ptest.sigma = 295.0;

 /*
  * We now make tsamples measurements, as usual, to generate the
  * missing statistic.  We proceed exactly as we did in opso, but
  * with a 4d 32x32x32x32 matrix and 5 bit indices.  This should
  * basically be strongly related to a Knuth hyperplane test in
  * four dimensions.  Equally obviously there is a sequence of
  * tests, all basically identical, that can be done here much
  * as rgb_bitdist tries to do them.  I'll postpone thinking about
  * this in detail until I'm done with diehard and some more of STS
  * and maybe have implemented the REAL Knuth tests from the Art of
  * Programming.
  */

 w = (char ****)malloc(32*sizeof(char ***));
 for(i=0;i<32;i++){
   w[i] = (char ***)malloc(32*sizeof(char **));
   for(j=0;j<32;j++){
     w[i][j] = (char **)malloc(32*sizeof(char *));
     for(k=0;k<32;k++){
       w[i][j][k] = (char *)malloc(32*sizeof(char));
       /* Zero the column */
       memset(w[i][j][k],0,32*sizeof(char));
     }
   }
 }

 /*
  * To minimize the number of rng calls, we use each j and k mod 32
  * to determine the offset of the 10-bit long string (with
  * periodic wraparound) to be used for the next iteration.  We
  * therefore have to "seed" the process with a random l
  */
 l = gsl_rng_get(rng);
 for(t=0;t<tsamples;t++){
   if(overlap){
     /*
      * Let's do this the cheap/easy way first, sliding a 20 bit
      * window along each int for the 32 possible starting
      * positions a la birthdays, before trying to slide it all
      * the way down the whole random bitstring implicit in a
      * long sequence of random ints.  That way we can exit
      * the tsamples loop at tsamples = 2^15...
      */
     if(t%32 == 0) {
       i0 = gsl_rng_get(rng);
       j0 = gsl_rng_get(rng);
       k0 = gsl_rng_get(rng);
       l0 = gsl_rng_get(rng);
       boffset = 0;
     }
     /*
      * Get four "letters" (indices into w)
      */
     i = get_bit_ntuple(&i0,1,5,boffset);
     j = get_bit_ntuple(&j0,1,5,boffset);
     k = get_bit_ntuple(&k0,1,5,boffset);
     l = get_bit_ntuple(&l0,1,5,boffset);
     /* printf("%u:   %u  %u  %u  %u  %u\n",t,i,j,k,l,boffset); */
     w[i][j][k][l]++;
     boffset++;

   } else {
     /*
      * Get four "letters" (indices into w)
      */
     boffset = l%32;
     i = gsl_rng_get(rng);
     i = get_bit_ntuple(&i,1,5,boffset);
     boffset = i%32;
     j = gsl_rng_get(rng);
     j = get_bit_ntuple(&j,1,5,boffset);
     boffset = j%32;
     k = gsl_rng_get(rng);
     k = get_bit_ntuple(&k,1,5,boffset);
     boffset = k%32;
     l = gsl_rng_get(rng);
     l = get_bit_ntuple(&l,1,5,boffset);
     w[i][j][k][l]++;
   }
 }
 /*
  * Now we count the holes, so to speak
  */
 ptest.x = 0;
 for(i=0;i<32;i++){
   for(j=0;j<32;j++){
     for(k=0;k<32;k++){
       for(l=0;l<32;l++){
         if(w[i][j][k][l] == 0){
           ptest.x += 1.0;
           /* printf("ptest.x = %f  Hole: w[%u][%u][%u][%u] = %u\n",ptest.x,i,j,k,l,w[i][j][k][l]); */
	 }
       }
     }
   }
 }
 if(verbose == D_DIEHARD_OQSO || verbose == D_ALL){
   printf("%f %f %f\n",ptest.y,ptest.x,ptest.x-ptest.y);
 }

 Xtest_eval(&ptest);
 ks_pvalue[kspi] = ptest.pvalue;

 if(verbose == D_DIEHARD_OQSO || verbose == D_ALL){
   printf("# diehard_craps(): ks_pvalue[%u] = %10.5f\n",kspi,ks_pvalue[kspi]);
 }

 kspi++;

 /*
  * Don't forget to free w when done, in reverse order
  */
 for(i=0;i<32;i++){
   for(j=0;j<32;j++){
      for(k=0;k<32;k++){
         free(w[i][j][k]);
      }
      free(w[i][j]);
   }
   free(w[i]);
 }
 free(w);

}

void help_diehard_oqso()
{

 printf("%s",dtest->description);

}
