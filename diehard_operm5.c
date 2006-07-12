/*
 * See copyright in copyright.h and the accompanying file COPYING
 */

/*
 *========================================================================
 * This is the Diehard OPERM5 test, rewritten from the description
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
 *          THE OVERLAPPING 5-PERMUTATION TEST                 ::
 * This is the OPERM5 test.  It looks at a sequence of one mill- ::
 * ion 32-bit random integers.  Each set of five consecutive     ::
 * integers can be in one of 120 states, for the 5! possible or- ::
 * derings of five numbers.  Thus the 5th, 6th, 7th,...numbers   ::
 * each provide a state. As many thousands of state transitions  ::
 * are observed,  cumulative counts are made of the number of    ::
 * occurences of each state.  Then the quadratic form in the     ::
 * weak inverse of the 120x120 covariance matrix yields a test   ::
 * equivalent to the likelihood ratio test that the 120 cell     ::
 * counts came from the specified (asymptotically) normal dis-   ::
 * tribution with the specified 120x120 covariance matrix (with  ::
 * rank 99).  This version uses 1,000,000 integers, twice.       ::
 *
 *========================================================================
 */


#include "dieharder.h"

double diehard_operm5()
{

 double *pvalue,pks;
 uint tempsamples;
 uint est_num_rands;

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
  * This test MAY require a fixed number of tsamples.  We'll see.
  */
 if(all == YES){
   tempsamples = tsamples;
   tsamples = 1000000;  /* Standard value from diehard */
 }

 if(!quiet){
   help_diehard_operm5();
   printf("# Random number generator tested: %s\n",gsl_rng_name(rng));
   printf("# Number of rands required is around 2^23 for 100 samples.\n");
 }

 kspi = 0;  /* Always zero first */
 pks = sample((void *)diehard_operm5_test);

 /*
  * Display histogram of ks p-values (optional)
  */
 if(hist_flag){
   histogram(ks_pvalue,psamples,0.0,1.0,10,"p-values");
 }
 printf("# p = %8.6f for diehard_operm5 test from Kuiper Kolmogorov-Smirnov\n",pks);
 printf("#     test on %u pvalues.\n",kspi);
 if(pks < 0.0001){
   printf("# Generator %s FAILS at 0.01%% for diehard_operm5.\n",gsl_rng_name(rng));
 }

 /*
  * Put back tsamples
  */
 tsamples = tempsamples;

 return(pks);

}

/*
 * These are the r and s matrices from diehard, all ready to go
 * with no I/O required or wierd indexing or anything.
 */
#include "diehard_operm5.h"

/*
 * kperm computes the permutation number of a vector of five integers
 * passed to it.
 */
uint kperm(uint v[],uint voffset)
{

 uint i,j,k,max;
 uint w[5];
 uint pindex,uret,tmp;

 /*
  * work on a copy of v, not v itself in case we are using overlapping
  * 5-patterns.
  */
 for(i=0;i<5;i++){
   j = (i+voffset)%5;
   w[i] = v[j];
 }

 if(verbose == -1){
   printf("==================================================================\n");
   printf("%10u %10u %10u %10u %10u\n",w[0],w[1],w[2],w[3],w[4]);
   printf(" Permutations = \n");
 }
 pindex = 0;
 for(i=4;i>0;i--){
   max = w[0];
   k = 0;
   for(j=1;j<=i;j++){
     if(max <= w[j]){
       max = w[j];
       k = j;
     }
   }
   pindex = (i+1)*pindex + k;
   tmp = w[i];
   w[i] = w[k];
   w[k] = tmp;
   if(verbose == -1){
     printf("%10u %10u %10u %10u %10u\n",w[0],w[1],w[2],w[3],w[4]);
   }
 }
 if(pindex < 60 ){
   uret = map[pindex];
 } else {
   uret = pindex;
 }

 if(verbose == -1){
   printf(" => %u\n",pindex);
   printf("map[%u] = %u\n",pindex,uret);
 }

 return uret;
   
}

#include "d_raw.h"

void diehard_operm5_test()
{

 uint i,j,k,kp,t,vind,v[5];
 double count[120];
 double av,norm,x[60],y[60],chisq,ndof,pvalue;

 /*
  * Just read in the raw r half-matrix and s half-matrix from d_raw = perm.txt
 k = 0;
 for(i=0;i<60;i++){
   for(j=i;j<60;j++){
     r[i][j] = d_raw[k++];
   }
 }
 for(i=0;i<60;i++){
   for(j=i;j<60;j++){
     s[i][j] = d_raw[k++];
   }
 }
 for(i=0;i<59;i++){
   for(j=i+1;j<60;j++){
     r[j][i] = r[i][j];
     s[j][i] = s[i][j];
   }
 }
  */

 if(verbose == D_DIEHARD_OPERM5 || verbose == D_ALL){
   printf("int r[][] = {\n");
   for(i=0;i<60;i++){
     printf("{");
     for(j=0;j<59;j++){
       printf("%10d, ",r[i][j]);
     }
     printf("%10d},\n",r[i][59]);
   }
   printf("};\n");
   printf("int s[][] = {\n");
   for(i=0;i<60;i++){
     printf("{");
     for(j=0;j<59;j++){
       printf("%10d, ",s[i][j]);
     }
     printf("%10d},\n",s[i][59]);
   }
   printf("};\n");
 }

 /*
  * Zero count vector, was t(120) in diehard.f90.
  */
 for(i=0;i<120;i++) count[i] = 0.0;
 if(overlap){
   for(i=0;i<5;i++){
     v[i] = gsl_rng_get(rng);
   }
   vind = 0;
 }
 for(t=0;t<tsamples;t++){

   /*
    * OK, now we are ready to generate a list of permutation indices.
    * Basically, we take a vector of 5 integers and transform it into a
    * number with the kperm function.  We will use the overlap flag to
    * determine whether or not to refill the entire v vector or just
    * rotate bytes.
    */
   if(overlap){
     kp = kperm(v,vind);
     /* printf("kp = %u\n",kp); */
     count[kp]++;
     v[vind] = gsl_rng_get(rng);
     vind = (vind+1)%5;
   } else {
     for(i=0;i<5;i++){
       v[i] = gsl_rng_get(rng);
     }
     kp = kperm(v,0);
     count[kp]++;
   }
 }

 if(verbose){
   for(i=0;i<120;i++){
     printf("%u: %f\n",i,count[i]);
   }
 }

 /*
  * Now (at last) we compute the statistic and get a p-value.  The
  * computation is straightforward, but involves the r and s matrices
  * so it doesn't fit the existing X or B models.
  */
 chisq = 0.0;
 /*
  * Not at all sure about this, yet.
  */
 av = 2.0*tsamples/120.0;
 norm = 2.e5*tsamples;
 for(i=0;i<60;i++){
   x[i] = count[i] + count[i+60] - av;
   y[i] = count[i] - count[i+60];
   /* printf("count[%u] = %f  count[%u] = %f  x[%u] = %f  y[%u] = %f\n",i,count[i],i+60,count[i+60],i,x[i],i,y[i]);*/
 }
 for(i=0;i<60;i++){
   for(j=0;j<60;j++){
     chisq = chisq + x[i]*r[i][j]*x[j] + y[i]*s[i][j]*y[j];
   }
 }
 chisq = chisq / norm;
 ndof = 99;
 if(verbose){
   printf("# diehard_operm5(): chisq[%u] = %f\n",kspi,chisq);
 }
 printf("# diehard_operm5(): chisq[%u] = %f\n",kspi,chisq);
 pvalue = gsl_sf_gamma_inc_Q((double)(ndof)/2.0,chisq/2.0);
 ks_pvalue[kspi] = pvalue;
 printf("# diehard_operm5(): ks_pvalue[%u] = %10.5f\n",kspi,ks_pvalue[kspi]);
 if(verbose == D_DIEHARD_OPERM5 || verbose == D_ALL){
   printf("# diehard_operm5(): ks_pvalue[%u] = %10.5f\n",kspi,ks_pvalue[kspi]);
 }

 kspi++;

}

void help_diehard_operm5()
{

 printf("\n\
#==================================================================\n\
#                Diehard \"OPERM5\" test.\n\
# This is the OPERM5 test.  It looks at a sequence of one mill- \n\
# ion 32-bit random integers.  Each set of five consecutive     \n\
# integers can be in one of 120 states, for the 5! possible or- \n\
# derings of five numbers.  Thus the 5th, 6th, 7th,...numbers   \n\
# each provide a state. As many thousands of state transitions  \n\
# are observed,  cumulative counts are made of the number of    \n\
# occurences of each state.  Then the quadratic form in the     \n\
# weak inverse of the 120x120 covariance matrix yields a test   \n\
# equivalent to the likelihood ratio test that the 120 cell     \n\
# counts came from the specified (asymptotically) normal dis-   \n\
# tribution with the specified 120x120 covariance matrix (with  \n\
# rank 99).  This version uses 1,000,000 integers, twice.       \n\
#==================================================================\n");

}
