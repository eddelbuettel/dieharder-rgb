/*
 * $Id$
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *
 */

/*
 *========================================================================
 * This is the Diehard OPERM5 test, rewritten from the description
 * in tests.txt on George Marsaglia's diehard site.
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
 *                       Comment
 * Good test.  Just about everything fails it.  It is validated
 * to the extent humanly possible against the last c port of
 * diehard that I could find (die.c) on the remaining diehard
 * mirror but it is complicated enough (and poorly documented
 * enough) that I worry.  One lousy error in the r[][], s[][]
 * or map[] vector data and we'd get slightly wrong answers
 * and never know it, but we get the SAME answers right now
 * either way from the same data.
 *========================================================================
 */


#include "dieharder.h"
/*
 * Test specific data
 */
#include "diehard_operm5.h"

double diehard_operm5()
{

 double pks;
 uint ps_save,ts_save;

 uint est_num_rands;

 /*
  * Do a standard test if -a(ll) is selected.
  * ALSO use standard values if tsamples or psamples are 0
  */
 if(all == YES){
   ts_save = tsamples;
   tsamples = dtest->tsamples_std;
   ps_save = psamples;
   psamples = dtest->psamples_std;
 }
 if(tsamples == 0){
   tsamples = dtest->tsamples_std;
 }
 if(psamples == 0){
   psamples = dtest->psamples_std;
 }
 
 /*
  * Allocate memory for THIS test's ks_pvalues, etc.  Make sure that
  * any missed prior allocations are freed.
  */
 if(ks_pvalue) nullfree(ks_pvalue);
 ks_pvalue  = (double *)malloc((size_t) psamples*sizeof(double));

 /*
  * Reseed FILE random number generators once per individual test.
  * This correctly resets the rewind counter per test.
  */
 if(strncmp("file_input",gsl_rng_name(rng),10) == 0){
   gsl_rng_set(rng,1);
 }

 test_header(dtest);
 printf("# Number of rands required is around 2^28 for 100 samples.\n");

 /*
  * This is now the standard test call.
  */
 kspi = 0;  /* Always zero first */
 pks = sample((void *)diehard_operm5_test);

 /*
  * Test Results, standard form.
  */
 test_footer(dtest,pks,ks_pvalue,"Diehard Overlapping 5-permutations Test");

 /*
  * Put back tsamples
  */
 if(all == YES){
   tsamples = ts_save;
   psamples = ps_save;
 }

 if(ks_pvalue) nullfree(ks_pvalue);

 return(pks);

}

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
 } else {
   for(i=0;i<5;i++){
     v[i] = gsl_rng_get(rng);
   }
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
     /*
      * This is the other way to get the same result as above.  It
      * yields the exact same numbers.  This strongly suggests that
      * there is nothing wrong with the code, overlapping or not,
      * relative to diehard.
     for(i=0;i<4;i++){
       v[i] = v[i+1];
     }
     v[4] = gsl_rng_get(rng);
      */
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
 /*
  * The absolute value "shouldn't" be necessary but it is -- every
  * few hundred runs we get a negative chisq, which seems very
  * plausible (actually) given the numbers and wierd chisq in the
  * first place.  The other possibility (alas a very reasonable one)
  * is that some of the numbers in r[][], s[][] or map[] are incorrect.
  * Noting well that they AGREE with an INDEPENDENT PORT of diehard
  * to C to 12 significant figures when run on identical binary files
  * and flagged to use the same algorithm...
  *
  * It would be nice, so nice, to have SOME clue how to actually generate
  * the matrices and other numbers since even a simple sign error on
  * a single number could make the test useless and (incidentally) cause
  * it to sometimes return a negative chisq.
  *
  * In the meantime, negative chisq causes the incomplete gamma function
  * routine to crash, so we protect it with the fabs() call.
  */
 chisq = fabs(chisq / norm);
 ndof = 99;
 if(verbose){
   printf("# diehard_operm5(): chisq[%u] = %10.5f\n",kspi,chisq);
 }
 pvalue = gsl_sf_gamma_inc_Q((double)(ndof)/2.0,chisq/2.0);
 ks_pvalue[kspi] = pvalue;
 if(verbose == D_DIEHARD_OPERM5 || verbose == D_ALL){
   printf("# diehard_operm5(): ks_pvalue[%u] = %10.5f\n",kspi,ks_pvalue[kspi]);
 }

 kspi++;

}

void help_diehard_operm5()
{

 printf("%s",dtest->description);

}
