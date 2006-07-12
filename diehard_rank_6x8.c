/*
 * $Id$
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *
 */

/*
 *========================================================================
 * This is the Diehard BINARY RANK 6x8 test, rewritten from the 
 * description in tests.txt on George Marsaglia's diehard site.
 *
 * This is the BINARY RANK TEST for 6x8 matrices.  From each of  ::
 * six random 32-bit integers from the generator under test, a   ::
 * specified byte is chosen, and the resulting six bytes form a  ::
 * 6x8 binary matrix whose rank is determined.  That rank can be ::
 * from 0 to 6, but ranks 0,1,2,3 are rare; their counts are     ::
 * pooled with those for rank 4. Ranks are found for 100,000     ::
 * random matrices, and a chi-square test is performed on        ::
 * counts for ranks 6,5 and <=4.                                 ::
 *
 *                          Comments
 *===================================================================
 */


#include "dieharder.h"


double diehard_rank_6x8()
{

 double pks;
 uint tempsamples;
 int i;
 
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
  * Save tsamples and use diehard standard values if doing -a(ll).
  */
 if(all == YES){
   tempsamples = tsamples;
   tsamples = 100000;  /* Standard value from diehard */
 }

 /*
  * Allocate space for ks_pvalue.  Free it below
  */
 ks_pvalue = (double *)malloc((size_t) psamples*sizeof(double));

 if(!quiet){
   help_diehard_rank_6x8();
   printf("# Random number generator tested: %s\n",gsl_rng_name(rng));
   printf("#                        Run Details\n");
   if(strncmp("file_input",gsl_rng_name(rng),10) == 0){
     printf("# Random number generator tested: %s\n",gsl_rng_name(rng));
     printf("# File %s contains %u rands of %c type.\n",filename,filecount,filetype);
   } else {
     printf("# Random number generator tested: %s\n",gsl_rng_name(rng));
   }
   printf("# 6x8 samples per test run = %u.  diehard value is 100000.\n",tsamples);
   printf("# Test run %u times to cumulate p-values for KS test.\n",psamples);
 }

 /*
  * start by dimensioning rand_mtx
  */
 rand_mtx = (uint **)malloc(6*sizeof(uint *));
 for(i=0;i<6;i++) rand_mtx[i] = (uint *)malloc(8*sizeof(uint));
 
 kspi = 0;  /* Always zero first */
 pks = sample((void *)diehard_rank_6x8_test);

 /*
  * Display histogram of ks p-values (optional)
  */
 if(hist_flag){
   histogram(ks_pvalue,psamples,0.0,1.0,10,"p-values");
 }
 if(!quiet){
   if(strncmp("file_input",gsl_rng_name(rng),10) == 0){
     printf("# %u rands were used in this test\n",file_input_get_rtot(rng));
     printf("# The file %s was rewound %u times\n",gsl_rng_name(rng),file_input_get_rewind_cnt(rng));
   }
   printf("#==================================================================\n");
 }
 printf("#                          Results\n");
 printf("# p = %8.6f for diehard 6x8 binary rank test from\n",pks);
 printf("#      Kuiper Kolmogorov-Smirnov test on %u pvalues.\n",kspi);
 /* Work through some ranges here */
 if(pks < 0.0001){
   printf("# Generator %s FAILED at < 0.01%% for diehard_rank_6x8.\n",gsl_rng_name(rng));
 } else if(pks < 0.01){
   printf("# Generator %s POOR at < 1%% for diehard_rank_6x8.\n",gsl_rng_name(rng));
   printf("# Recommendation:  Repeat test to verify failure.\n");
 } else if(pks < 0.05){
   printf("# Generator %s POSSIBLY WEAK at < 5%% for diehard_rank_6x8.\n",gsl_rng_name(rng));
   printf("# Recommendation:  Repeat test to verify failure.\n");
 } else {
   printf("# Generator %s PASSED at > 5%% for diehard_rank_6x8.\n",gsl_rng_name(rng));
 }

 /*
  * Put back tsamples, free ks_pvalue, free rand_int
  */
 if(all == YES){
   tsamples = tempsamples;
 }
 free(ks_pvalue);
 free(rand_int);

 return(pks);

}

void diehard_rank_6x8_test()
{

 int i,j,k,t,rank,offset;
 double r,smax,s;
 
 uint bitstring,rmask,mask;
 Btest btest;

 Btest_create(&btest,7,"diehard_rank_6x8",gsl_rng_name(rng));
 for(i=0;i<2;i++){
   btest.x[0] = 0.0;
   btest.y[0] = 0.0;
   btest.sigma[0] = 0.0;
 }
 btest.x[2] = 0.0;
 btest.y[2] = tsamples*0.149858E-06;
 btest.sigma[2] = 0.0;
 btest.x[3] = 0.0;
 btest.y[3] = tsamples*0.808926E-04;
 btest.sigma[3] = 0.0;
 btest.x[4] = 0.0;
 btest.y[4] = tsamples*0.936197E-02;
 btest.sigma[4] = 0.0;
 btest.x[5] = 0.0;
 btest.y[5] = tsamples*0.217439E+00;
 btest.sigma[5] = 0.0;
 btest.x[6] = 0.0;
 btest.y[6] = tsamples*0.773118E+00;
 btest.sigma[6] = 0.0;
 
 for(t=0;t<tsamples;t++){

   /*
    * We generate 6 random rmax_bits-bit integers and put a
    * randomly chosen byte into the LEFTMOST byte position
    * of the row/slot of rand_mtx.
    */
   if(verbose == D_DIEHARD_RANK_6x8 || verbose == D_ALL){
     printf("# diehard_rank_6x8(): Input random matrix = \n");
   }
   for(i=0;i<6;i++){
     if(verbose == D_DIEHARD_RANK_6x8 || verbose == D_ALL){
       printf("# ");
     }
     /*
      * random offset from 0 to rmax_bits-1
      */
     offset = gsl_rng_uniform_int(rng,rmax_bits);
     /*
      * Get a random integer
      */
     bitstring = gsl_rng_get(rng);
     /*
      * Get one byte starting at the random offset
      */
     rand_mtx[i][0] = get_bit_ntuple(&bitstring,1,8,offset);
     /*
      * Shift it left until it is lined up in the leftmost
      * byte.
     rand_mtx[i][0] <<= 24;
      */
     if(verbose == D_DIEHARD_RANK_6x8 || verbose == D_ALL){
       dumpbits(rand_mtx[i],32);
     }
   }

   rank = binary_rank(rand_mtx,6,8);
   if(verbose == D_DIEHARD_RANK_6x8 || verbose == D_ALL){
     printf("binary rank = %d\n",rank);
   }

   if(rank <= 2){
     btest.x[2]++;
   } else {
     btest.x[rank]++;
   }
 }

 /* for(i=0;i<33;i++) printf("btest.x[%d] =  %f\n",i,btest.x[i]); */

 Btest_eval(&btest);
 ks_pvalue[kspi] = btest.pvalue;
 if(verbose == D_DIEHARD_RANK_6x8 || verbose == D_ALL){
   printf("# diehard_rank_6x8(): ks_pvalue[%u] = %10.5f\n",kspi,ks_pvalue[kspi]);
 }
 kspi++;

 Btest_destroy(&btest);

}

void help_diehard_rank_6x8()
{

 printf("\n\
#==================================================================\n\
#             Diehard 6x8 \"Binary Rank\" test\n\
# This is the BINARY RANK TEST for 6x8 matrices.  From each of\n\
# six random 32-bit integers from the generator under test, a\n\
# specified byte is chosen, and the resulting six bytes form a\n\
# 6x8 binary matrix whose rank is determined.  That rank can be\n\
# from 0 to 6, but ranks 0,1,2,3 are rare; their counts are\n\
# pooled with those for rank 4. Ranks are found for 100,000\n\
# random matrices, and a chi-square test is performed on\n\
# counts for ranks 6,5 and <=4.\n\
#\n\
# As always, the test is repeated and a KS test applied to the\n\
# resulting p-values to verify that they are approximately uniform.\n\
#==================================================================\n");

}


