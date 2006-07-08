/*
 * See copyright in copyright.h and the accompanying file COPYING
 */

/*
 *========================================================================
 * This is the Diehard DNA test, rewritten from the description
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
 *   The DNA test considers an alphabet of 4 letters::  C,G,A,T,::
 * determined by two designated bits in the sequence of random   ::
 * integers being tested.  It considers 10-letter words, so that ::
 * as in OPSO and OQSO, there are 2^20 possible words, and the   ::
 * mean number of missing words from a string of 2^21  (over-    ::
 * lapping)  10-letter  words (2^21+9 "keystrokes") is 141909.   ::
 * The standard deviation sigma=339 was determined as for OQSO   ::
 * by simulation.  (Sigma for OPSO, 290, is the true value (to   ::
 * three places), not determined by simulation.                  ::
 *
 *  Note 2^21 = 2097152
 *
 *========================================================================
 */


#include "dieharder.h"

double diehard_dna()
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
   help_diehard_dna();
   printf("# Random number generator tested: %s\n",gsl_rng_name(rng));
   printf("# Number of rands required is around 2^28 for 100 samples.\n");
 }

 kspi = 0;  /* Always zero first */
 pks = sample((void *)diehard_dna_test);

 /*
  * Display histogram of ks p-values (optional)
  */
 if(hist_flag){
   histogram(ks_pvalue,psamples,0.0,1.0,10,"p-values");
 }
 printf("# p = %8.6f for diehard_dna test from Kuiper Kolmogorov-Smirnov\n",pks);
 printf("#     test on %u pvalues.\n",kspi);
 if(pks < 0.0001){
   printf("# Generator %s FAILS at 0.01%% for diehard_dna.\n",gsl_rng_name(rng));
 }

 /*
  * Put back tsamples
  */
 tsamples = tempsamples;

 return(pks);

}

void diehard_dna_test()
{

 uint i,j,k,l,m,n,o,p,q,r,t,boffset;
 Xtest ptest;
 char **********w;

 /*
  * p = 141909, with sigma 339, FOR tsamples 2^21+1 2 letter words.
  * These cannot be varied unless one figures out the actual
  * expected "missing works" count as a function of sample size.  SO:
  *
  * ptest.x = number of "missing words" given 2^21+1 trials
  * ptest.y = 141909
  * ptest.sigma = 339
  */
 ptest.y = 141909.0;
 ptest.sigma = 339.0;

 /*
  * We now make tsamples measurements, as usual, to generate the
  * missing statistic.  Wow!  10 dimensions!  I don't think
  * even matrix() will do that...;-)
  */

 w = (char **********)malloc(4*sizeof(char *********));
 for(i=0;i<4;i++){
   w[i] = (char *********)malloc(4*sizeof(char ********));
   for(j=0;j<4;j++){
     w[i][j] = (char ********)malloc(4*sizeof(char *******));
     for(k=0;k<4;k++){
       w[i][j][k] = (char *******)malloc(4*sizeof(char******));
       for(l=0;l<4;l++){
         w[i][j][k][l] = (char ******)malloc(4*sizeof(char*****));
         for(m=0;m<4;m++){
           w[i][j][k][l][m] = (char *****)malloc(4*sizeof(char****));
           for(n=0;n<4;n++){
             w[i][j][k][l][m][n] = (char ****)malloc(4*sizeof(char***));
             for(o=0;o<4;o++){
               w[i][j][k][l][m][n][o] = (char ***)malloc(4*sizeof(char**));
               for(p=0;p<4;p++){
                 w[i][j][k][l][m][n][o][p] = (char **)malloc(4*sizeof(char*));
                 for(q=0;q<4;q++){
                   w[i][j][k][l][m][n][o][p][q] = (char *)malloc(4*sizeof(char));
                   /* Zero the column */
                   memset(w[i][j][k][l][m][n][o][p][q],0,4*sizeof(char));
		 }
	       }
	     }
	   }
	 }
       }
     }
   }
 }

 /*
  * To minimize the number of rng calls, we use each j and k mod 32
  * to determine the offset of the 10-bit long string (with
  * periodic wraparound) to be used for the next iteration.  We
  * therefore have to "seed" the process with a random l
  */
 q = gsl_rng_get(rng);
 for(t=0;t<tsamples;t++){
   /*
    * Get two "letters" (indices into w)
    */
   boffset = q%32;
   i = gsl_rng_get(rng);
   i = get_bit_ntuple(&i,1,2,boffset);
   boffset = i%32;
   j = gsl_rng_get(rng);
   j = get_bit_ntuple(&j,1,2,boffset);
   boffset = j%32;
   k = gsl_rng_get(rng);
   k = get_bit_ntuple(&k,1,2,boffset);
   boffset = k%32;
   l = gsl_rng_get(rng);
   l = get_bit_ntuple(&l,1,2,boffset);
   boffset = l%32;
   m = gsl_rng_get(rng);
   m = get_bit_ntuple(&m,1,2,boffset);
   boffset = m%32;
   n = gsl_rng_get(rng);
   n = get_bit_ntuple(&n,1,2,boffset);
   boffset = n%32;
   o = gsl_rng_get(rng);
   o = get_bit_ntuple(&o,1,2,boffset);
   boffset = o%32;
   p = gsl_rng_get(rng);
   p = get_bit_ntuple(&p,1,2,boffset);
   boffset = p%32;
   q = gsl_rng_get(rng);
   q = get_bit_ntuple(&q,1,2,boffset);
   boffset = q%32;
   r = gsl_rng_get(rng);
   r = get_bit_ntuple(&r,1,2,boffset);
   w[i][j][k][l][m][n][o][p][q][r]++;
 }
 /*
  * Now we count the holes, so to speak
  */
 ptest.x = 0;
 for(i=0;i<4;i++){
   for(j=0;j<4;j++){
     for(k=0;k<4;k++){
       for(l=0;l<4;l++){
         for(m=0;m<4;m++){
           for(n=0;n<4;n++){
             for(o=0;o<4;o++){
               for(p=0;p<4;p++){
                 for(q=0;q<4;q++){
                   for(r=0;r<4;r++){
                     if(w[i][j][k][l][m][n][o][p][q][r] == 0){
                       ptest.x += 1.0;
                       /* printf("ptest.x = %f  Hole: w[%u][%u][%u][%u] = %u\n",ptest.x,i,j,k,l,w[i][j][k][l]); */
		     }
		   }
		 }
	       }
	     }
	   }
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
 for(i=0;i<4;i++){
   for(j=0;j<4;j++){
     for(k=0;k<4;k++){
       for(l=0;l<4;l++){
         for(m=0;m<4;m++){
           for(n=0;n<4;n++){
             for(o=0;o<4;o++){
               for(p=0;p<4;p++){
                 for(q=0;q<4;q++){
                   free(w[i][j][k][l][m][n][o][p][q]);
		 }
		 free(w[i][j][k][l][m][n][o][p]);
	       }
               free(w[i][j][k][l][m][n][o]);
	     }
             free(w[i][j][k][l][m][n]);
	   }
           free(w[i][j][k][l][m]);
	 }
         free(w[i][j][k][l]);
       }
       free(w[i][j][k]);
     }
     free(w[i][j]);
   }
   free(w[i]);
 }
 free(w);

}

void help_diehard_dna()
{

 printf("\n\
#==================================================================\n\
#                  Diehard \"DNA\" test.\n\
# \n\
#   The DNA test considers an alphabet of 4 letters::  C,G,A,T,\n\
# determined by two designated bits in the sequence of random   \n\
# integers being tested.  It considers 10-letter words, so that \n\
# as in OPSO and OQSO, there are 2^20 possible words, and the   \n\
# mean number of missing words from a string of 2^21  (over-    \n\
# lapping)  10-letter  words (2^21+9 \"keystrokes\") is 141909.   \n\
# The standard deviation sigma=339 was determined as for OQSO   \n\
# by simulation.  (Sigma for OPSO, 290, is the true value (to   \n\
# three places), not determined by simulation.                  \n\
# \n\
# Note 2^21 = 2097152\n\
# Note also that we don't bother with overlapping keystrokes \n\
# (and sample more rands -- rands are now cheap). \n\
#==================================================================\n");

}
