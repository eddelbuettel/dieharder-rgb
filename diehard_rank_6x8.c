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
 printf("# p = %8.6f for diehard 6x8 binary rank test from Kuiper Kolmogorov-Smirnov\n",pks);
 printf("#     test on %u pvalues.\n",kspi);
 if(pks < 0.0001){
   printf("# Generator %s FAILS at 0.01%% for diehard_rank_6x8.\n",gsl_rng_name(rng));
 }

 return(pks);

}

void diehard_rank_6x8_test()
{

 int i,j,k,t,rank;
 double r,smax,s;
 

 /*
  * In this test, we generate a random integer matrix in A, do a singular
  * value decomposition into A, S and V, and compute the rank from S.
  * The rank is then binned, and the bins compared to the expected
  * distribution of rank.  Note that although we generate uints
  * (ideally 32 bit integers, but in any event rmax_bits integers) we
  * must store them into A as doubles, as SVD works with double precision
  * matrices and variables only.  Note also that due to floating point
  * roundoff, we may need to apply a tolerance test to the contents of
  * S to determine the rank -- zeros may not be EXACTLY zero.
  */

 for(t=0;t<tsamples;t++){

   printf("# diehard_rank_6x8(): Input random matrix = \n");
   for(i=0;i<6;i++){
     printf("# ");
     for(j=0;j<8;j++){
       rand_mtx[i][j] = gsl_rng_get(rng);
       printf("%10u ",rand_mtx[i][j]);
     }
     printf("\n");
   }

   rank = binary_rank(rand_mtx,6,8);

   printf("Sample %u: rank = %d\n",t,rank);

 }
   

}

void help_diehard_rank_6x8()
{

 printf("\n\
#==================================================================\n\
#             Diehard 6x8 \"Binary Rank\" test\n\
# This is the BINARY RANK TEST for 6x8 matrices. The leftmost \n\
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


