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


double diehard_binary_rank()
{

 double pks;
 
 if(!quiet){
   help_diehard_binary_rank();
   printf("# Random number generator tested: %s\n",gsl_rng_name(rng));
   printf("# Number of random matrices tested = %u\n",tsamples);
 }

 kspi = 0;  /* Always zero first */
 pks = sample((void *)diehard_binary_rank_test);
 printf("# p = %8.6f for diehard binary rank test from Kuiper Kolmogorov-Smirnov\n",pks);
 printf("#     test on %u pvalues.\n",kspi);
 if(pks < 0.0001){
   printf("# Generator %s FAILS at 0.01%% for diehard_binary_rank.\n",gsl_rng_name(rng));
 }

 return(pks);

}

void diehard_binary_rank_test()
{

}

void help_diehard_binary_rank()
{

 printf("\n\
#==================================================================\n\
#                    Diehard \"Binary Rank\" test\n\
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


