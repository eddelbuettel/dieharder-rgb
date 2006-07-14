/*
 * $Id$
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *
 */

/*
 *========================================================================
 * This is the Diehard Runs test, rewritten from the description
 * in tests.txt on George Marsaglia's diehard site.
 *
 * Here is the test description from diehard_tests.txt:
 *
 * This is the RUNS test. It counts runs up, and runs down,in a sequence
 * of uniform [0,1) variables, obtained by floating the 32-bit integers
 * in the specified file. This example shows how runs are counted:
 *  .123, .357, .789, .425,. 224, .416, .95
 * contains an up-run of length 3, a down-run of length 2 and an up-run
 * of (at least) 2, depending on the next values.  The covariance matrices
 * for the runs-up and runs-down are well-known, leading to chisquare tests
 * for quadratic forms in the weak inverses of the covariance matrices.
 * Runs are counted for sequences of length 10,000.  This is done ten times,
 * then repeated.
 *
 *                            Comment
 *
 * I modify this the following ways. First, I let the sequence length be
 * the variable -t (vector length) instead of fixing it at 10,000.  This
 * lets one test sequences that are much longer (entirely possible with
 * a modern CPU even for a fairly slow RNG).  Second, I repeat this for
 * the variable -s (samples) times, default 100 and not just 10.  Third,
 * because RNG's often have "bad seeds" for which they misbehave, the
 * individual sequences can be optionally -i reseeded for each sample.
 * Because this CAN let bad behavior be averaged out to where
 * it isn't apparent for many samples with few bad seeds, we may need to
 * plot the actual distribution of p-values for this and other tests where
 * this option is used.  Fourth, it is silly to convert integers into floats
 * in order to do this test.  Up sequences in integers are down sequences in
 * floats once one divides by the largest integer available to the generator,
 * period. Integer arithmetic is much faster than float AND one skips the
 * very costly division associated with conversion.
 * *========================================================================
 */


#include "dieharder.h"


/*
 * The following are the definitions and parameters for runs, based on
 * Journal of Applied Statistics v30, Algorithm AS 157, 1981:
 *    The Runs-Up and Runs-Down Tests, by R. G. T. Grafton.
 * (and before that Knuth's The Art of Programming v. 2).
 */

/*
 * Test specific data, defaults
 */
#include "diehard_runs.h"

double diehard_runs()
{

 double pks;
 uint ps_save,ts_save;

 uint tempsamples,temppsamples;

 /*
  * Do a standard test if -a(ll) is selected.
  */
 if(all == YES){
   ts_save = tsamples;
   tsamples = dtest->tsamples_std;
   ps_save = psamples;
   psamples = dtest->psamples_std;
 }

 /*
  * ALSO use standard values if tsamples or psamples are 0
  */
 if(tsamples == 0){
   tsamples = dtest->tsamples_std;
 }
 if(psamples == 0){
   psamples = dtest->psamples_std;
 }
 
 /*
  * Allocate space for ks_pvalue and rand_int, checking to prevent
  * leaks from previous tests.  Free them below.
  *
  * Note that this test generates two samples per call.  Unlike
  * regular diehard, we accumulate the pvalues for up and down runs
  * SEPARATELY and test them SEPARATELY as either one could fail,
  * right?  And failures could easily partially compensate...
  */
 if(ks_pvalue) free(ks_pvalue);
 ks_pvalue = (double *)malloc((size_t) psamples*sizeof(double));
 if(ks_pvalue2) free(ks_pvalue);
 ks_pvalue2 = (double *)malloc((size_t) psamples*sizeof(double));
 if(rand_int) free(rand_int);
 rand_int = (uint *)malloc(tsamples*sizeof(uint));


 test_header(dtest);

 /*
  * This is now the standard test call.
  */
 kspi = 0;  /* Always zero first */
 pks = sample((void *)diehard_runs_test);

 /*
  * Results:
  */
 test_footer(dtest,pks,ks_pvalue,"Runs (up)");

 /*
  * Runs generates two statistics, not one, so we need an extra
  * KS test on the second vector of p-values.
  */
 pks = kstest_kuiper(ks_pvalue2,kspi);
 test_footer(dtest,pks,ks_pvalue2,"Runs (down)");

 /*
  * Put back tsamples
  */
 if(all == YES){
   tsamples = ts_save;
   psamples = ps_save;
 }

 if(ks_pvalue) free(ks_pvalue);
 if(ks_pvalue2) free(ks_pvalue2);
 if(rand_int) free(rand_int);

 return(pks);

}

void diehard_runs_test()
{

 int i,j,k,t,ns;
 unsigned int ucount,dcount,increased;
 int upruns[RUN_MAX],downruns[RUN_MAX];
 double uv,dv,up_pks,dn_pks;
 double *uv_pvalue,*dv_pvalue;

 /*
  * Fill vector of "random" integers with selected generator.
  * Observe that this test does NOT not convert to floats but
  * counts up down and down up on an integer compare.
  */

 /*
  * Clear up and down run bins
  */
 for(k=0;k<RUN_MAX;k++){
   upruns[k] = 0;
   downruns[k] = 0;
 }

 /*
  * Now count up and down runs and increment the bins.  Note
  * that each successive up counts as a run of one down, and
  * each successive down counts as a run of one up.
  */
 ucount = dcount = 1;
 if(verbose){
   printf("j    rand    ucount  dcount\n");
 }
 rand_int[0] = gsl_rng_get(rng);
 for(t=1;t<tsamples;t++) {
   rand_int[t] = gsl_rng_get(rng);
   if(verbose){
     printf("%d:  %10u   %u    %u\n",t,rand_int[t],ucount,dcount);
   }

   /*
    * Did we increase?
    */
   if(rand_int[t] > rand_int[t-1]){
     ucount++;
     if(ucount > RUN_MAX) ucount = RUN_MAX;
     downruns[dcount-1]++;
     dcount = 1;
   } else {
     dcount++;
     if(dcount > RUN_MAX) dcount = RUN_MAX;
     upruns[ucount-1]++;
     ucount = 1;
   }
 }
 if(rand_int[tsamples-1] > rand_int[0]){
   ucount++;
   if(ucount > RUN_MAX) ucount = RUN_MAX;
   downruns[dcount-1]++;
   dcount = 1;
 } else {
   dcount++;
   if(dcount > RUN_MAX) dcount = RUN_MAX;
   upruns[ucount-1]++;
   ucount = 1;
 }

 /*
  * This ends a single sample.
  * Compute the test statistic for up and down runs.
  */
 uv=0.0;
 dv=0.0;
 if(verbose){
   printf(" i      upruns    downruns\n");
 }
 for(i=0;i<RUN_MAX;i++) {
   if(verbose){
     printf("%d:   %7d   %7d\n",i,upruns[i],downruns[i]);
   }
   for(j=0;j<RUN_MAX;j++) {
     uv += ((double)upruns[i]   - tsamples*b[i])*(upruns[j]   - tsamples*b[j])*a[i][j];
     dv += ((double)downruns[i] - tsamples*b[i])*(downruns[j] - tsamples*b[j])*a[i][j];
   }
 }
 uv /= (double)tsamples;
 dv /= (double)tsamples;

 /*
  * This NEEDS WORK!  It isn't right, somehow...
  */
 up_pks = 1.0 - exp ( -0.5 * uv ) * ( 1.0 + 0.5 * uv + 0.125 * uv*uv );
 dn_pks = 1.0 - exp ( -0.5 * dv ) * ( 1.0 + 0.5 * dv + 0.125 * dv*dv );
 
 if(verbose){
   printf("uv = %f   dv = %f\n",uv,dv);
 }
 ks_pvalue[kspi] = gsl_sf_gamma_inc_Q(3.0,uv/2.0);
 ks_pvalue2[kspi] = gsl_sf_gamma_inc_Q(3.0,dv/2.0);
 /* printf("%12.6f\t%12.6f\n",ks_pvalue[kspi-2],ks_pvalue[kspi-1]); */

 kspi++;

}

void help_diehard_runs()
{

 printf("\n\
#==================================================================\n\
#                Diehard \"runs\" test (modified).\n\
# This tests the distribution of increasing and decreasing runs\n\
# of integers.  If called with reasonable parameters e.g. -s 100\n\
# or greater and -n 100000 or greater, it will compute a vector\n\
# of p-values for up and down and verify that the proportion\n\
# of these values less than 0.01 is consistent with a uniform\n\
# distribution.\n\
#==================================================================\n");

}
