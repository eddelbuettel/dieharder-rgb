/*
* $Id$
*
* See copyright in copyright.h and the accompanying file COPYING
*
*/

#include "rand_rate.h"

void measure_rate()
{

 unsigned int i,j,k,aindex,aitmp;	/* Loop variables */
 int empty_iter,full_iter,vector_length;

 /* Sundry variables for doing statistics */
 double delta,total_time,total_time2;
 double avg_time_empty,avg_time2_empty,sigma_time_empty;
 double avg_time_full,avg_time2_full,sigma_time_full;

 /* The answers and a normalization term */
 double avg_nanotime,nanotime_norm;
 double avg_megarate;


 /*
  *========================================================================
  * This routine is basically a timing shell.
  *
  * The idea is very simple - measure the empty loop time (adjusting
  * iterations until its time is resolvable).  Then we time the filled loop
  * taking exactly one branch of the case switch and subtract to get a
  * timing result.  A bit of division and we've got our number.
  *========================================================================
  * Note that we do NOT want the kernel
  * to grab a timeslice in between the gettimeofday calls, as it introduces
  * a big nasty random delay two or three orders of magnitude larger than
  * the loop time.  The best way to prevent this seems to be to insert a
  * microsleep right before the timing sample, which allows the kernel to
  * grab a slice if anything is pending.  This vastly reduces the variance
  * of the timing loop, although it means that the measurements don't
  * reflect real-world multitasking competition on an actual running
  * system as well.
  *
  * We start by autotuning the empty timing loop number of iterations
  * until is is large enough to yield a "decent" precision in the
  * measurement of the empty loop time.
  *========================================================================
  */
 delta = 0.0;
 empty_iter = 1;
 while(delta < 1.e-2){	/* We want to spin at least a hundredth of a second */

   empty_iter*=2;	/* double until we get there */
   usleep(1);
   start_timing();
   for(j=1;j<=empty_iter;j++){
     dummy = idiot;	/* pass idiot on to dummy to foil optimizers */
   }
   stop_timing();
   delta = delta_timing();

 }

 /*
  * Now we time the empty loop for many samples to get an accurate
  * value, starting timing right after a microsleep to reduce the
  * chance of an interrupt in the middle (although some will of course
  * occur).
  */
  
 total_time = 0.0;
 total_time2 = 0.0;
 for(k=1;k<=samples;k++){
   usleep(1);
   start_timing();
   for(j=1;j<=empty_iter;j++){
     dummy = idiot;	/* pass it on to dummy to force the case */
   }
   stop_timing();
   delta = delta_timing()/(double)empty_iter;
   if(verbose == 11) printf("delta = %12.8e\n",delta);
   total_time += delta;
   total_time2 += delta*delta;
 }
 avg_time_empty = total_time/samples;
 avg_time2_empty = total_time2/samples;
 if(samples>1){
   sigma_time_empty = sqrt(( avg_time2_empty - avg_time_empty*avg_time_empty )/(samples-1.0));
 } else {
   sigma_time_empty = 1.0e300;
 }
 if(!quiet){
   printf("# ========================================================================\n");
   printf("# Timing \"Empty\" Loop\n");
   printf("# Samples = %d  Loop iterations per sample = %d\n",samples,empty_iter);
   printf("# Time(sec): %12.8e +/- %12.8e\n",avg_time_empty,sigma_time_empty);
   printf("# ========================================================================\n");
 }


 /*
  *========================================================================
  * Next we autotune the full timing loop so that it contains enough
  * iterations to ALSO give a "decent" precision.
  *========================================================================
  */
 delta = 0.0;
 full_iter = 1;	/* I want at LEAST 2 iterations */
 while(delta < 1.0e-2){	/* We want to spin at least a hundredth of a second */

   full_iter*=2;	/* double until we get there */
   usleep(1);
   start_timing();
   for(j=1;j<=full_iter;j++){
     dummy = idiot;	/* pass it on to dummy to force the case */
     for(i=0;i<size;i++){
       rand_int[i] = gsl_rng_get(random);
     }
   }
   stop_timing();
   delta = delta_timing();

 }

 /*
  * Note that iterations is under control at the command line and CAN
  * be replaced if desired.  It's generally best not to though...
  */
 if(iterations>0) full_iter = iterations;

 /*
  *========================================================================
  * Now we are ready to run the actual benchmark.  It runs long enough
  * that the usleep(1) doesn't seem to do much. 
  *========================================================================
  */
 total_time = 0.0;
 total_time2 = 0.0;
 for(k=1;k<=samples;k++){
   usleep(1);
   start_timing();
   for(j=1;j<=full_iter;j++){
     dummy = idiot;	/* pass it on to dummy to force the case */
     for(i=0;i<size;i++){
       rand_int[i] = gsl_rng_get(random);
     }
   }
   stop_timing();
   delta = delta_timing()/(double)full_iter;
   if(verbose == 11) printf("delta = %12.8e\n",delta);
   total_time += delta;
   total_time2 += delta*delta;
 }
 avg_time_full = total_time/samples;
 avg_time2_full = total_time2/samples;
 if(samples>1){
   sigma_time_full = sqrt(( avg_time2_full - avg_time_full*avg_time_full )/(samples-1.0));
 } else {
   sigma_time_full = 1.0e+300;
 }
 if(!quiet){
   printf("# Timing test %d\n",testnum);
   printf("# Time(sec): %12.8e +/- %12.8e\n",avg_time_full,sigma_time_full);
   printf("# Samples = %d  Loop iterations per sample = %d\n",samples,full_iter);
   printf("#========================================================================\n");
 }

 /*
  *========================================================================
  * Finally, we evaluate and publish two numbers, the average time and
  * the average rate in mega-whatevers.
  *========================================================================
  */
  vector_length = size*sizeof(unsigned int);
  nanotime_norm = 1.0;
  avg_nanotime = (avg_time_full - avg_time_empty)*1.e+9/nanotime_norm;
  avg_megarate = 1.0e3/avg_nanotime;
  /* This is a comment */
  if(!quiet){
    printf("# Benchmark of the gsl_rnd_int() for the %s generator:\n",gsl_rng_name(random));
    printf("#    and vector size = %d (%d bytes)\n",size,size*sizeof(unsigned int));
    printf("# Average Time: %6.2f nanoseconds\n",avg_nanotime);
    printf("# BogomegaRate: %6.2f megafloats per second\n",avg_megarate);
  } else {
    printf("%12d (bytes)    %6.2f (bogomflops)\n",size*sizeof(unsigned int),avg_megarate);
  }
}

