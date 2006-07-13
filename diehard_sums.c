/*
 * $Id: diehard_bitstream.c 185 2006-07-12 22:23:59Z rgb $
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *
 */

/*
 *========================================================================
 * This is the Diehard sums test, rewritten from the description
 * in tests.txt on George Marsaglia's diehard site.
 *
 *             The  OVERLAPPING SUMS test                        ::
 * Integers are floated to get a sequence U(1),U(2),... of uni-  ::
 * form [0,1) variables.  Then overlapping sums,                 ::
 *   S(1)=U(1)+...+U(100), S2=U(2)+...+U(101),... are formed.    ::
 * The S's are virtually normal with a certain covariance mat-   ::
 * rix.  A linear transformation of the S's converts them to a   ::
 * sequence of independent standard normals, which are converted ::
 * to uniform variables for a KSTEST. The  p-values from ten     ::
 * KSTESTs are given still another KSTEST.                       ::
 *
 *                       Comments
 *========================================================================
 */


#include "dieharder.h"

double diehard_sums()
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
  * If this test is run by itself, we can ignore tsamples.  If it is
  * part of a "standard run", we have to use specific values.  Either
  * way, we have to adjust the sizes of e.g. the list of integers to
  * be generated and sampled, and (re)allocate memory accordingly.
  * Then at the bottom, we have to put it all back.
  */
 if(all == YES){
   tempsamples = tsamples;
   tsamples = 100;          /* do 100 separate sums */
 }

 /*
  * Allocate space for ks_pvalue.  Free it below
  */
 ks_pvalue = (double *)malloc((size_t) psamples*sizeof(double));

 if(!quiet){
   help_diehard_sums();
   printf("#                        Run Details\n");
   if(strncmp("file_input",gsl_rng_name(rng),10) == 0){
     printf("# Random number generator tested: %s\n",gsl_rng_name(rng));
     printf("# File %s contains %u rands of %c type.\n",filename,filecount,filetype);
   } else {
     printf("# Random number generator tested: %s\n",gsl_rng_name(rng));
   }
   printf("# Samples per test = %u.  Diehard default is 100\n",tsamples);
   printf("# Test run %u times to cumulate p-values for final KS test.\n",psamples);
   printf("# Note that diehard only used 10 p-values in its final KS test.\n");
 }
 if(tsamples != 100){
   printf("#==================================================================\n");
   printf("# WARNING WARNING WARNING!  This test pretty much requires\n");
   printf("#   that tsamples be 100 to work, although I'm leaving in\n");
   printf("#   the ability to change it.  Consider restarting.\n");
   printf("#==================================================================\n");
 }

 kspi = 0;  /* Always zero first */
 pks = sample((void *)diehard_sums_test);

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
 }
 printf("#                          Results\n");
 printf("# p = %8.6f for diehard_sums test (mean) from\n",pks);
 printf("#     Kuiper Kolmogorov-Smirnov test on %u pvalues.\n",kspi);
 /* Work through some ranges here */
 if(pks < 0.0001){
   printf("# Generator %s FAILED at < 0.01%% for diehard_sums.\n",gsl_rng_name(rng));
 } else if(pks < 0.01){
   printf("# Generator %s POOR at < 1%% for diehard_sums.\n",gsl_rng_name(rng));
   printf("# Recommendation:  Repeat test to verify failure.\n");
 } else if(pks < 0.05){
   printf("# Generator %s POSSIBLY WEAK at < 5%% for diehard_sums.\n",gsl_rng_name(rng));
   printf("# Recommendation:  Repeat test to verify failure.\n");
 } else {
   printf("# Generator %s PASSED at > 5%% for diehard_sums.\n",gsl_rng_name(rng));
 }
 printf("#==================================================================\n");

 /*
  * Put back tsamples, free ks_pvalue.
  */
 if(all == YES){
   tsamples = tempsamples;
 }
 
 free(ks_pvalue);

 return(pks);

}

void diehard_sums_test()
{

 int i,j,k,m,t;
 double a,b,qq,*x,*y,*rand_dbl;
 double pvalue;
 Xtest ptest;

 /*
  * SUMS requires that we evaluate y[i] = \sum_i^{100+i} u[i].  Naturally,
  * the sum (or mean, if you prefer) is supposedly normally distributed
  * according to the central limit theorem.  So each number has an
  * associated p-value -- the probability of that particular number
  * occurring from the random sum.  The vector of p-values can be turned
  * into an overall p-value via a KS test.
  *
  * However, the sums formed from OVERLAPPING vectors are not uncorrelated.
  * They have to be transformed with a matrix to make them independent
  * normal.  Things get all funky then in Marsaglia's description, at least
  * to a poor ignorant physicist like myself.  I quote:
  *
  *   The y's covariance matrix T is Toeplitz with diagonals 100,99,...2,1
  *   A Cholesky factorization of T: V'V = T provides M=V^(-1).  The
  *   covariance of x = yM is M'TM=I.
  *
  *   The columns of M have at most 3 non-zero elements.
  *
  * Ouch!  Toeplitz!  Cholesky factorization!  Off to wikipedia or
  * the wolfram site to figure out what that means in plain English...;-)
  *
  * The greatest tragedy, of course, is that I THINK that he's working
  * this hard to avoid having to use too many rands.  After all, if one
  * uses NON-overlapping sums, they ARE independent already and a
  * simple chisq on the vector yields a p-value.  Is there some reason
  * to expect a generator might fail on the overlapping sums but pass
  * on the much more straightforward test on real non-overlapping ones?
  *
  * We will Find Out Shortly, folks, as I have at my disposal the overlap
  * flag, and plan to implement Marsaglia's test precisely in the -O(verlap)
  * segment and just plain doggone sum independent sequences of uniform
  * rands and see how they distribute around 100*0.5 = 50 with a boring
  * old chisq test...
  *
  * I may make this an rgb test, of course, because several parameters of
  * this process can be varied a lot more easily if I just write the test
  * from scratch from the beginning.
  */

 /*
  * It's a bit more efficient not to malloc this ever pass through,
  * but we can worry about moving it and parsing out m elsewhere
  * later.  Maybe.  If we care.
  */
 rand_dbl = (double *)malloc((size_t) tsamples*sizeof(double));

 /*
  * Initialize and zero x (used in both overlapping and non-overlapping
  * sums below).
  */
 x = (double *)malloc(tsamples*sizeof(double));
 memset(x,0,tsamples*sizeof(double));

 /*
  * Initialize y.  We're going to play a curious little pointer trick
  * with the y-sums.  The following creates y[-1] to y[tsamples-1],
  * inclusive and zeros them all.
  */
 y = (double *)malloc((tsamples+1)*sizeof(double));
 memset(y,0,(tsamples+1)*sizeof(double));
 y = &y[1];

 /*
  * Fill a ring of rands of length m, summing them into y[-1] at the
  * same time.  This will seed the t loop below nicely.
  */
 if(verbose == D_DIEHARD_SUMS || verbose == D_ALL){
   printf("#==================================================================\n");
   printf("# Initializing initial y[-1] and ring of rands\n");
 }
 for(t=0;t<tsamples;t++){
   rand_dbl[t] = gsl_rng_uniform(rng);
   y[-1] += rand_dbl[t];
   if(verbose == D_DIEHARD_SUMS || verbose == D_ALL){
     printf("y[-1] =  %f\n",y[-1]);
   }
 }
   
 if(verbose == D_DIEHARD_SUMS || verbose == D_ALL){
   printf("#==================================================================\n");
   printf("# Now we generate the set of %u overlapping y's\n",tsamples);
 }
 for(t=0;t<tsamples;t++){
   /*
    * This index points into the ring of rands and
    * circulates through 0-(m-1) as t increments.
    */
   /* Subtract the rand at the ring pointer */
   y[t] = y[t-1] - rand_dbl[t];
   /* Get a new rand into the ring at the pointer */
   rand_dbl[t] = gsl_rng_uniform(rng);
   /* Add the new rand at the ring pointer */
   y[t] += rand_dbl[t];
   if(verbose == D_DIEHARD_SUMS || verbose == D_ALL){
     printf("y[%u] =  %f\n",t,y[t]);
   }
 }

 /*
  * Now, for a variety of reasons we need to scale this so
  * that it has mean 0 and unit deviation.
  */
 if(verbose == D_DIEHARD_SUMS || verbose == D_ALL){
   printf("#==================================================================\n");
   printf("# We shift and scale y so it should have mean 0.0\n");
 }
 for(t=0;t<tsamples;t++){
   y[t] = (y[t] - 0.5*tsamples)*sqrt(12.0);
   if(verbose == D_DIEHARD_SUMS || verbose == D_ALL){
     printf("y[%u] =  %f\n",t,y[t]);
   }
 }

 /*
  * We convert these (evaluated either way) to a unit normal and thence
  * to a uniform distribution.
  */
 if(verbose == D_DIEHARD_SUMS || verbose == D_ALL){
   printf("#==================================================================\n");
   printf("# We convert it to a normal distribution of width 1.0\n");
 }
 x[0] = y[0]/sqrt((double)tsamples);
 x[1] = -x[0]*(tsamples-1)/sqrt(2.0*m - 1.0)
             +y[1]*sqrt(tsamples/(2.0*m - 1.0));
 if(verbose == D_DIEHARD_SUMS || verbose == D_ALL){
   printf("x[0] = %f\n",x[0]);
   printf("x[1] = %f\n",x[1]);
 }

 for(t=2;t<tsamples;t++){
   a = 2.0*m + 1.0 - t;
   b = 2.0*a - 2.0;
   x[t] = y[0]/sqrt(a*b)- sqrt((a-1.0)/(b+2.0))*y[t-1] + sqrt(a/b)*y[t];
   if(verbose == D_DIEHARD_SUMS || verbose == D_ALL){
     printf("x[%u] = %f\n",t,x[t]);
   }
 }

 /*
  * Finally, we convert it into a uniform distribution by making
  * it into an Xtest, as it were.
  */
 if(verbose == D_DIEHARD_SUMS || verbose == D_ALL){
   printf("#==================================================================\n");
   printf("# Finally, we turn it into a vector of presumably uniform deviates.\n");
 }
 for(t=0;t<tsamples;t++){
   ptest.x = x[t];
   ptest.y = 0.0;
   ptest.sigma = 1.0;
   Xtest_eval(&ptest);
   x[t] = ptest.pvalue;
   if(verbose == D_DIEHARD_SUMS || verbose == D_ALL){
     printf("x[%u] = %f \n",t,x[t]);
   }
 }
 if(verbose == D_DIEHARD_SUMS || verbose == D_ALL){
   histogram(x,tsamples,0.0,1.0,10,"x-values");
 }

 /*
  * At last, the x[i]'s are a presumably UNIFORM distribution from 0-1, in
  * fact a distribution of tsamples p-values!  We can then do the same old
  * ks test on them that we'll eventually do on ks_pvalue[]!
  */
 pvalue = kstest_kuiper(x,tsamples);
 ks_pvalue[kspi] = pvalue;
 if(verbose == D_DIEHARD_SUMS || verbose == D_ALL){
   printf("# diehard_sums(): ks_pvalue[%u] = %10.5f\n",kspi,ks_pvalue[kspi]);
 }
 kspi++;

 free(rand_dbl);
 free(x);
 free(&y[-1]);

}

void help_diehard_sums()
{

 printf("\n\
#==================================================================\n\
#                Diehard \"sums\" test (modified).\n\
# Integers are floated to get a sequence U(1),U(2),... of uni-  \n\
# form [0,1) variables.  Then overlapping sums,                 \n\
#   S(1)=U(1)+...+U(100), S2=U(2)+...+U(101),... are formed.    \n\
# The S's are virtually normal with a certain covariance mat-   \n\
# rix.  A linear transformation of the S's converts them to a   \n\
# sequence of independent standard normals, which are converted \n\
# to uniform variables for a KSTEST. The  p-values from ten     \n\
# KSTESTs are given still another KSTEST.                       \n\
#\n\
# Note well:  -O causes the old diehard version to be run (more or\n\
# less).  Omitting it causes non-overlapping sums to be used and \n\
# directly tests the overall balance of uniform rands.\n\
#==================================================================\n");

}

