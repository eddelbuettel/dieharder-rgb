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


double diehard_rank_6x8()
{

 double pks;
 int i;
 
 if(!quiet){
   help_diehard_rank_6x8();
   printf("# Random number generator tested: %s\n",gsl_rng_name(rng));
   printf("# Number of random 6x8 bitwise matrices tested = %u\n",tsamples);
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
 printf("# p = %8.6f for diehard 6x8 binary rank test from Kuiper Kolmogorov-Smirnov\n",pks);
 printf("#     test on %u pvalues.\n",kspi);
 if(pks < 0.0001){
   printf("# Generator %s FAILS at 0.01%% for diehard_rank_6x8.\n",gsl_rng_name(rng));
 }

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


