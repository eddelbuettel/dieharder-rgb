/*
 * See copyright in copyright.h and the accompanying file COPYING
 */

/*
 *========================================================================
 * This is the Diehard QPSO test, rewritten from the description
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
 *    OQSO means Overlapping-Quadruples-Sparse-Occupancy        ::
 *  The test OQSO is similar, except that it considers 4-letter ::
 *  words from an alphabet of 32 letters, each letter determined  ::
 *  by a designated string of 5 consecutive bits from the test    ::
 *  file, elements of which are assumed 32-bit random integers.   ::
 *  The mean number of missing words in a sequence of 2^21 four-  ::
 *  letter words,  (2^21+3 "keystrokes"), is again 141909, with   ::
 *  sigma = 295.  The mean is based on theory; sigma comes from   ::
 *  extensive simulation.                                         ::
 *
 *  Note 2^21 = 2097152
 *
 *========================================================================
 */


#include "dieharder.h"

double diehard_oqso()
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
   help_diehard_oqso();
   printf("# Random number generator tested: %s\n",gsl_rng_name(rng));
   printf("# Number of rands required is around 2^28 for 100 samples.\n");
 }

 kspi = 0;  /* Always zero first */
 pks = sample((void *)diehard_oqso_test);

 /*
  * Display histogram of ks p-values (optional)
  */
 if(hist_flag){
   histogram(ks_pvalue,psamples,0.0,1.0,10,"p-values");
 }
 printf("# p = %8.6f for diehard_oqso test from Kuiper Kolmogorov-Smirnov\n",pks);
 printf("#     test on %u pvalues.\n",kspi);
 if(pks < 0.0001){
   printf("# Generator %s FAILS at 0.01%% for diehard_oqso.\n",gsl_rng_name(rng));
 }

 /*
  * Put back tsamples
  */
 tsamples = tempsamples;

 return(pks);

}

void diehard_oqso_test()
{

 uint i,j,k,l,t,boffset;
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
   /*
    * Get two "letters" (indices into w)
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

 printf("\n\
#==================================================================\n\
#                Diehard \"OQSO\" test.\n\
#  OQSO means Overlapping-Quadruples-Sparse-Occupancy.        \n\
#  The test OQSO is similar, except that it considers 4-letter \n\
#  words from an alphabet of 32 letters, each letter determined  \n\
#  by a designated string of 5 consecutive bits from the test    \n\
#  file, elements of which are assumed 32-bit random integers.   \n\
#  The mean number of missing words in a sequence of 2^21 four-  \n\
#  letter words,  (2^21+3 \"keystrokes\"), is again 141909, with   \n\
#  sigma = 295.  The mean is based on theory; sigma comes from   \n\
#  extensive simulation.                                         \n\
# \n\
#  Note 2^21 = 2097152\n\
#==================================================================\n");

}
