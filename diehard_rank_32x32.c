/*
* $Id$
*
* See copyright in copyright.h and the accompanying file COPYING
*
*/

/*
 *========================================================================
 *========================================================================
 */


#include "dieharder.h"


double diehard_rank_32x32()
{

 int i;
 double pks;
 
 if(!quiet){
   help_diehard_rank_32x32();
   printf("# Random number generator tested: %s\n",gsl_rng_name(rng));
   printf("# Number of 32x32 random matrices tested = %u\n",tsamples);
 }
 /*
  * Note that this assumes that 8*sizeof(uint) >= 32!
  */
 rand_mtx = (uint **)malloc(32*sizeof(uint*));
 for(i=0;i<32;i++)rand_mtx[i] = (uint *)malloc(sizeof(uint));

 kspi = 0;  /* Always zero first */
 pks = sample((void *)diehard_rank_32x32_test);
 printf("# p = %8.6f for diehard 32x32 binary rank test from Kuiper Kolmogorov-Smirnov\n",pks);
 printf("#     test on %u pvalues.\n",kspi);
 if(pks < 0.0001){
   printf("# Generator %s FAILS at 0.01%% for diehard_rank_32x32.\n",gsl_rng_name(rng));
 }

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
 btest.x[29] = 0.0052854502e+00;
 btest.y[29] = 0.0;
 btest.sigma[29] = 0.0;
 btest.x[30] = 0.1283502644e+00;
 btest.y[30] = 0.0;
 btest.sigma[30] = 0.0;
 btest.x[31] = 0.5775761902e+00;
 btest.y[31] = 0.0;
 btest.sigma[31] = 0.0;
 btest.x[32] = 0.2887880952e+00;
 btest.y[32] = 0.0;
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

 Btest_eval(&btest);
 ks_pvalue[kspi] = btest.pvalue;
 if(verbose == D_DIEHARD_RANK_32x32 || verbose == D_ALL){
   printf("# diehard_rank_32x32(): ks_pvalue[%u] = %10.5f\n",kspi,ks_pvalue[kspi]);
 }
 printf("# diehard_rank_32x32(): ks_pvalue[%u] = %10.5f\n",kspi,ks_pvalue[kspi]);
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


