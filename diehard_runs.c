/*
* $Id$
*
* See copyright in copyright.h and the accompanying file COPYING
* See also accompanying file STS.COPYING
*
*/

/*
 *========================================================================
 * This is the Diehard RUNS test, rewritten from the description
 * in tests.txt on  * George Marsaglia's diehard site.
 *
 * * Rewriting means that I can standardize the interface to
 * gsl-encapsulated routines more easily.  It also makes this
 * my own code.  Finally, since the C versions Marsaglia provides
 * are the result of f2c running on Fortran sources, they are really
 * ugly code and the rewrite should be much more manageable.
 *
 * From tests.txt:
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
 * I modify this the following ways. First, I let the sequence length be
 * the variable -n (vector length) instead of fixing it at 10,000.  This
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


#include "rand_rate.h"


/*
 * The following are the definitions and parameters for runs, based on
 * Journal of Applied Statistics v30, Algorithm AS 157, 1981:
 *    The Runs-Up and Runs-Down Tests, by R. G. T. Grafton.
 * (and before that Knuth's The Art of Programming v. 2).
 */

#define RUN_MAX 6
/*
 * a_ij
 */
static double a[6][6] = {
 { 4529.4,   9044.9,  13568.0,   18091.0,   22615.0,   27892.0},
 { 9044.9,  18097.0,  27139.0,   36187.0,   45234.0,   55789.0},
 {13568.0,  27139.0,  40721.0,   54281.0,   67852.0,   83685.0},
 {18091.0,  36187.0,  54281.0,   72414.0,   90470.0,  111580.0},
 {22615.0,  45234.0,  67852.0,   90470.0,  113262.0,  139476.0},
 {27892.0,  55789.0,  83685.0,  111580.0,  139476.0,  172860.0}
};
/*
 * b_i
 */
static double b[6];
int diehard_runs()
{

 int i,j,k,ns;
 unsigned int ucount,dcount,increased;
 int upruns[RUN_MAX],downruns[RUN_MAX];
 int ufail,dfail;
 double uv,dv,uv_pvalue,dv_pvalue,uv_pvalue_min,dv_pvalue_min;
 double *up,*down;

 /* Initialize b explicitly */
 b[0] = 1.0/6.0;
 b[1] = 5.0/24.0;
 b[2] = 11.0/120.0;
 b[3] = 19.0/720.0;
 b[4] = 29.0/5040.0;
 b[5] = 1.0/840.0;

 up = (double *)malloc(samples*sizeof(double));
 down = (double *)malloc(samples*sizeof(double));

 uv_pvalue_min = 1.0;
 dv_pvalue_min = 1.0;
 ufail = 0;
 dfail = 0;

 /*
  * Fill vector of "random" integers with selected generator.
  * Observe that this test does NOT not convert to floats but
  * counts up down and down up on an integer compare.
  */
 if(!quiet){
   printf("#==================================================================\n");
   printf("#  up runs p-value      down runs p-value\n");
 }
 for(ns=0;ns<samples;ns++){

   /* Fill vector of rands all at once */
   for(j=0;j<size;j++) {
     rand_int[j] = gsl_rng_get(rng);
   }

   /* Clear up and down run bins */
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
   for(j=1;j<size;j++) {
     if(verbose){
       printf("%d:  %10u   %u    %u\n",j,rand_int[j],ucount,dcount);
     }
     /*
      * Did we increase?
      */
     if(rand_int[j] > rand_int[j-1]){
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
   if(rand_int[size-1] > rand_int[0]){
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
       uv += ((double)upruns[i]   - size*b[i])*(upruns[j]   - size*b[j])*a[i][j];
       dv += ((double)downruns[i] - size*b[i])*(downruns[j] - size*b[j])*a[i][j];
     }
   }
   uv /= (double)size;
   dv /= (double)size;
   if(verbose){
     printf("uv = %f   dv = %f\n",uv,dv);
   }
   uv_pvalue = gsl_sf_gamma_inc_Q(3.0,uv/2.0);
   if(uv_pvalue < uv_pvalue_min) uv_pvalue_min = uv_pvalue;
   if(uv_pvalue < 1.0/samples) ufail++;
   dv_pvalue = gsl_sf_gamma_inc_Q(3.0,dv/2.0);
   if(dv_pvalue < dv_pvalue_min) dv_pvalue_min = dv_pvalue;
   if(dv_pvalue < 1.0/samples) dfail++;

   if(!quiet){
     printf("#    %10.8f                 %10.8f\n",uv_pvalue,dv_pvalue);
   }
/*
 * This distributes uv and dv in the interval 0-1, presumably
 * uniformly, for a KS test.  Once I write one.
   up[ns] = 1.0 -  exp(-0.5*uv)*(1.0 + 0.5*uv  + 0.125*uv*uv);
   down[ns] = 1.0 -  exp(-0.5*dv)*(1.0 + 0.5*dv  + 0.125*dv*dv);
   printf("%d  up = %f   down = %f\n",ns,up[ns],down[ns]);
 */

 }


 if(!quiet){
   printf("#==================================================================\n");
   printf("# Minimum p-values (reference = 1/%u = %f)\n",samples,(double)1.0/samples);
   printf("# (number of p's less than reference) ufail = %d   dfail = %d\n",ufail,dfail);
   printf("#  up min p-value      down min p-value\n");
   printf("#    %10.8f                 %10.8f\n",uv_pvalue_min,dv_pvalue_min);
 }
}

