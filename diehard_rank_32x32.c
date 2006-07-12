/*
* $Id$
*
* See copyright in copyright.h and the accompanying file COPYING
*
*/

/*
 *========================================================================
 * This is the Diehard BINARY RANK 31x31 test, rewritten from the 
 * description in tests.txt on George Marsaglia's diehard site.
 *
 * This is the BINARY RANK TEST for 31x31 matrices. The leftmost ::
 * 31 bits of 31 random integers from the test sequence are used ::
 * to form a 31x31 binary matrix over the field {0,1}. The rank  ::
 * is determined. That rank can be from 0 to 31, but ranks< 28   ::
 * are rare, and their counts are pooled with those for rank 28. ::
 * Ranks are found for 40,000 such random matrices and a chisqua-::
 * re test is performed on counts for ranks 31,30,29 and <=28.   ::
 *
 *                          Comments
 *========================================================================
 */


#include "dieharder.h"


double diehard_rank_32x32()
{

 int i;
 double pks;
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
  * Save tsamples and use diehard standard values if doing -a(ll).
  */
 if(all == YES){
   tempsamples = tsamples;
   tsamples = 40000;  /* Standard value from diehard */
 }

 /*
  * Allocate space for ks_pvalue.  Free it below
  */
 ks_pvalue = (double *)malloc((size_t) psamples*sizeof(double));

 if(!quiet){
   help_diehard_rank_32x32();
   printf("# Random number generator tested: %s\n",gsl_rng_name(rng));
   printf("#                        Run Details\n");
   if(strncmp("file_input",gsl_rng_name(rng),10) == 0){
     printf("# Random number generator tested: %s\n",gsl_rng_name(rng));
     printf("# File %s contains %u rands of %c type.\n",filename,filecount,filetype);
   } else {
     printf("# Random number generator tested: %s\n",gsl_rng_name(rng));
   }
   printf("# 32x32 samples per test run = %u.  diehard value is 40000.\n",tsamples);
   printf("# Test run %u times to cumulate p-values for KS test.\n",psamples);
 }
 /*
  * Note that this assumes that 8*sizeof(uint) >= 32!
  */
 rand_mtx = (uint **)malloc(32*sizeof(uint*));
 for(i=0;i<32;i++)rand_mtx[i] = (uint *)malloc(sizeof(uint));

 kspi = 0;  /* Always zero first */
 pks = sample((void *)diehard_rank_32x32_test);

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
 printf("# p = %8.6f for diehard 32x32 binary rank test from \n",pks);
 printf("#     Kuiper Kolmogorov-Smirnov test on %u pvalues.\n",kspi);
 /* Work through some ranges here */
 if(pks < 0.0001){
   printf("# Generator %s FAILED at < 0.01%% for diehard_rank_32x32.\n",gsl_rng_name(rng));
 } else if(pks < 0.01){
   printf("# Generator %s POOR at < 1%% for diehard_rank_32x32.\n",gsl_rng_name(rng));
   printf("# Recommendation:  Repeat test to verify failure.\n");
 } else if(pks < 0.05){
   printf("# Generator %s POSSIBLY WEAK at < 5%% for diehard_rank_32x32.\n",gsl_rng_name(rng));
   printf("# Recommendation:  Repeat test to verify failure.\n");
 } else {
   printf("# Generator %s PASSED at > 5%% for diehard_rank_32x32.\n",gsl_rng_name(rng));
 }

 /*
  * Put back tsamples, free ks_pvalue, free rand_int
  * Put back tsamples, free ks_pvalue.
  */
 if(all == YES){
   tsamples = tempsamples;
 }
 free(ks_pvalue);
 free(rand_int);

 return(pks);

}

void diehard_rank_32x32_test()
{

 int i,j,k,t,rank;
 double r,smax,s;
 uint bitstring,rmask,mask;
 Btest btest;

 Btest_create(&btest,33,"diehard_rank_32x32",gsl_rng_name(rng));
 for(i=0;i<29;i++){
   btest.x[0] = 0.0;
   btest.y[0] = 0.0;
   btest.sigma[0] = 0.0;
 }
 btest.x[29] = 0.0;
 btest.y[29] = tsamples*0.0052854502e+00;
 btest.sigma[29] = 0.0;
 btest.x[30] = 0.0;
 btest.y[30] = tsamples*0.1283502644e+00;
 btest.sigma[30] = 0.0;
 btest.x[31] = 0.0;
 btest.y[31] = tsamples*0.5775761902e+00;
 btest.sigma[31] = 0.0;
 btest.x[32] = 0.0;
 btest.y[32] = tsamples*0.2887880952e+00;
 btest.sigma[32] = 0.0;
 
 for(t=0;t<tsamples;t++){

   /*
    * We generate 32 x 32 random integers and put their leftmost
    * (most significant) bit into each bit slot of the rand_mtx.
    */
   if(verbose == D_DIEHARD_RANK_32x32 || verbose == D_ALL){
     printf("# diehard_rank_32x32(): Input random matrix = \n");
   }
   for(i=0;i<32;i++){
     if(verbose == D_DIEHARD_RANK_32x32 || verbose == D_ALL){
       printf("# ");
     }
     rand_mtx[i][0] = 0;
     for(j=0;j<32;j++){
       bitstring = gsl_rng_get(rng);
       bitstring = get_bit_ntuple(&bitstring,1,1,31);
       rand_mtx[i][0] <<= 1;
       rand_mtx[i][0] += bitstring;
       mask <<= 1;
     }
     if(verbose == D_DIEHARD_RANK_32x32 || verbose == D_ALL){
       dumpbits(rand_mtx[i],32);
     }
   }

   rank = binary_rank(rand_mtx,32,32);
   if(verbose == D_DIEHARD_RANK_32x32 || verbose == D_ALL){
     printf("binary rank = %d\n",rank);
   }

   if(rank <= 29){
     btest.x[29]++;
   } else {
     btest.x[rank]++;
   }
 }

 /* for(i=0;i<33;i++) printf("btest.x[%d] =  %f\n",i,btest.x[i]); */

 Btest_eval(&btest);
 ks_pvalue[kspi] = btest.pvalue;
 if(verbose == D_DIEHARD_RANK_32x32 || verbose == D_ALL){
   printf("# diehard_rank_32x32(): ks_pvalue[%u] = %10.5f\n",kspi,ks_pvalue[kspi]);
 }
 kspi++;

 Btest_destroy(&btest);

}

void help_diehard_rank_32x32()
{

 printf("\n\
#==================================================================\n\
#              Diehard 32x32 \"Binary Rank\" test\n\
# This is the BINARY RANK TEST for 31x31 matrices. The leftmost \n\
# 31 bits of 31 random integers from the test sequence are used \n\
# to form a 31x31 binary matrix over the field {0,1}. The rank  \n\
# is determined. That rank can be from 0 to 31, but ranks< 28   \n\
# are rare, and their counts are pooled with those for rank 28. \n\
# Ranks are found for (default) 40,000 such random matrices and\n\
# a chisquare test is performed on counts for ranks 31,30,29 and\n\
# <=28.\n\
#\n\
# As always, the test is repeated and a KS test applied to the\n\
# resulting p-values to verify that they are approximately uniform.\n\
#==================================================================\n");

}


