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

int diehard_runs()
{

 int i,j,k;
 unsigned int ucount,dcount;
 double ptmp;
 Ntest utest,dtest;

 if(verbose){
   printf("Creating utest\n");
 }
 Ntest_create(&utest,RUN_MAX,"diehard_runs(up)",gsl_rng_name(rng));
 if(verbose){
   printf("Initializing utest\n");
 }
 for(k=0;k<RUN_MAX;k++){
   /*
    * Here is what I don't know -- what the expected probability
    * and sigma is for each possible run length from a sample.
    * Let's assume something like 1/2^i.  The rest we'll leave
    * for the moment.
    */
   ptmp = pow(0.5,k+1);
   utest.y[k] = samples*ptmp;
   utest.sigma[k] = sqrt(utest.y[k]*(1.0 - ptmp));
 }

 if(verbose){
   printf("Creating dtest\n");
 }
 Ntest_create(&dtest,RUN_MAX,"diehard_runs(down)",gsl_rng_name(rng));
 for(k=0;k<RUN_MAX;k++){
   ptmp = pow(0.5,k+1);
   dtest.y[k] = samples*ptmp;
   dtest.sigma[k] = sqrt(dtest.y[k]*(1.0 - ptmp));
 }


 /*
  * Fill vector of "random" integers with selected generator.
  * Observe that this test does NOT not convert to floats but
  * counts up down and down up on an integer compare.
  */
 for(i=0;i<samples;i++){
   ucount = dcount = 0;
   for(j=0;j<size;j++) {
     rand_int[j] = gsl_rng_get(rng);
     if(verbose){
       printf("%d:  %u\n",j,rand_int[j]);
     }
     if(j>0){
       /*
        * Check to see if this int is larger, if so increment ucount.  We'll
        * handle boundary cases (equal integers) as larger.  We then must check
        * to see if the PREVIOUS pair was running down (in which case dcount
	* will not be zero).  If so, we increment dtest->x[dcount] and clear
	* dcount.
        */
       if(rand_int[j] >= rand_int[j-1]){
         ucount++;
         if(verbose){
           printf("->ucount = %u\n",ucount);
	 }
	 if(dcount){
	   if(dcount > RUN_MAX) {
	     fprintf(stderr,"diehard_run run-time error, down run larger than %d\n",RUN_MAX);
	     fprintf(stderr,"Increase RUN_MAX and recompile, or use smaller sample size.\n");
	     exit(0);
	   }
	   dtest.x[dcount-1]++;
           if(verbose){
             printf("->old dcount = %u binned\n",dcount);
           }
	   dcount = 0;
	 }
       } else {
         dcount++;
         if(verbose){
           printf("->dcount = %u\n",dcount);
	 }
	 if(ucount){
	   if(ucount >= RUN_MAX) {
	     fprintf(stderr,"diehard_run run-time error, up run larger than %d\n",RUN_MAX);
	     fprintf(stderr,"Increase RUN_MAX and recompile, or use smaller sample size.\n");
	     exit(0);
	   }
	   utest.x[ucount-1]++;
           if(verbose){
             printf("->old ucount = %u binned\n",ucount);
           }
	   ucount = 0;
	 }
       }
     }
   }
   /*
    * Manage end case separately (periodic/cyclic wraparound) so
    * we check exactly size cases per sample.
    */
   if(rand_int[size] >= rand_int[0]){
     ucount++;
     if(verbose){
       printf("->ucount = %u\n",ucount);
     }
     if(dcount){
       if(dcount > RUN_MAX) {
         fprintf(stderr,"diehard_run run-time error, down run larger than %d\n",RUN_MAX);
         fprintf(stderr,"Increase RUN_MAX and recompile, or use smaller sample size.\n");
         exit(0);
       }
       dtest.x[dcount-1]++;
       if(verbose){
         printf("->old dcount = %u binned\n",dcount);
       }
       dcount = 0;
     }
     utest.x[ucount-1]++;
     if(verbose){
       printf("->final ucount = %u binned\n",ucount);
     }
     ucount = 0;
   } else {
     dcount++;
     if(verbose){
       printf("->dcount = %u\n",dcount);
     }
     if(ucount){
       if(ucount >= RUN_MAX) {
         fprintf(stderr,"diehard_run run-time error, up run larger than %d\n",RUN_MAX);
         fprintf(stderr,"Increase RUN_MAX and recompile, or use smaller sample size.\n");
          exit(0);
       }
       utest.x[ucount-1]++;
       if(verbose){
         printf("->old ucount = %u binned\n",ucount);
       }
       ucount = 0;
     }
     dtest.x[dcount-1]++;
     if(verbose){
       printf("->final dcount = %u binned\n",dcount);
     }
     dcount = 0;
   }
   /*
    * This ends a single sample.
    */
 }

 /*
  * Compute the test statistics up an down.
  */

 b[0] = 0.16666666666666666;
 b[1] = 0.20833333333333334;
 b[2] = 0.09166666666666666;
 b[3] = 0.026388888888888889;
 b[4] = 0.0057539682539682543;
 b[5] = 0.0011904761904761906;
 up = 0.0;
 down = 0.0;
 for(i=0;i<RUN_MAX;i++) {
   for(j=0;j<RUN_MAX;j++) {
     up   += (utest.x[i] - rn*b[i])*(utest.x[j] - rn*b[j])*a[i][j];
     down += (dtest.x[i] - rn*b[i])*(dtest.x[j] - rn*b[j])*a[i][j];
   }
 }
 up /= rn;
 down /= rn;


 
 /*
  * This ends all sampling.  Now we have to analyze the result.
  */
 Ntest_eval(&utest);
 Ntest_eval(&dtest);
   
}

