/*
* $Id$
*
* See copyright in copyright.h and the accompanying file COPYING
*
*/

#include "cpu_rate.h"

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
  * This routine is basically a timing shell into which we can add any
  * flag-controlled, single indexed tests on global vectors malloc'd
  * elsewhere OR on specific non-indexed command sequences.
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
  * We start by autotuning the empty timing loop iterations and timing it 
  * to a "decent" precision.  Note that as long as we precisely match
  * the switch cases, our execution path should be at least homologous
  * for both the empty and full loops (same number of preceding conditional
  * tests and so forth).
  *========================================================================
  */
 delta = 0.0;
 empty_iter = 1;
 while(delta < 1.e-2){	/* We want to spin at least a hundredth of a second */

   empty_iter*=2;	/* double until we get there */
   usleep(1);
   start_timing();
   for(j=1;j<=empty_iter;j++){
     switch(testnum){
       case 0:
       default:
         dummy = idiot;	/* pass it on to dummy to force the case */
         break;
       case 1:
         dummy = idiot;	/* pass it on to dummy to force the case */
         break;
       case 2:
         dummy = idiot;	/* pass it on to dummy to force the case */
         break;
       case 3:
         dummy = idiot;	/* pass it on to dummy to force the case */
         break;
       case 4:
         dummy = idiot;	/* pass it on to dummy to force the case */
         break;
       case 5:
         dummy = idiot;	/* pass it on to dummy to force the case */
         break;
       case 6:
         dummy = idiot;	/* pass it on to dummy to force the case */
         break;
       case 7:
       case 8:
         /*
	  * The baseline case here also has to include the shell
	  * loop of the wr process, I think, as the loop is not "empty".
	  * That is, the following statement is a vector read all
	  * by itself and cannot be made non-sequential.
	  */
         dummy = idiot;	/* pass it on to dummy to force the case */
         for (i=0; i<size; i+=stride){
           aindex = ai[i];
	 }
         break;
       case 9:
         dummy = idiot;	/* pass it on to dummy to force the case */
         break;
       case 10:
         dummy = idiot;	/* pass it on to dummy to force the case */
         break;
     }
   }
   stop_timing();
   delta = delta_timing();

 }

 total_time = 0.0;
 total_time2 = 0.0;
 for(k=1;k<=samples;k++){
   usleep(1);
   start_timing();
   for(j=1;j<=empty_iter;j++){
     switch(testnum){
       case 0:
       default:
         dummy = idiot;	/* pass it on to dummy to force the case */
         break;
       case 1:
         dummy = idiot;	/* pass it on to dummy to force the case */
         break;
       case 2:
         dummy = idiot;	/* pass it on to dummy to force the case */
         break;
       case 3:
         dummy = idiot;	/* pass it on to dummy to force the case */
         break;
       case 4:
         dummy = idiot;	/* pass it on to dummy to force the case */
         break;
       case 5:
         dummy = idiot;	/* pass it on to dummy to force the case */
         break;
       case 6:
         dummy = idiot;	/* pass it on to dummy to force the case */
         break;
       case 7:
       case 8:
         /*
	  * The baseline case here also has to include the shell
	  * loop of the wr process, I think, as the loop is not "empty".
	  * That is, the following statement is a vector read all
	  * by itself and cannot be made non-sequential.
	  */
         dummy = idiot;	/* pass it on to dummy to force the case */
         for (i=0; i<size; i+=stride){
           aindex = ai[i];
	 }
         break;
       case 9:
         dummy = idiot;	/* pass it on to dummy to force the case */
         break;
       case 10:
         dummy = idiot;	/* pass it on to dummy to force the case */
         break;
     }
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
     switch(testnum){
       case 0:
       default:
         dummy = idiot;	/* pass it on to dummy to force the case */
         break;
       case 1:	/* bogomflops (avg float rate) */
         dummy = idiot;	/* pass it on to dummy to force the case */
         for(i=0;i<size;i++){
           d[i] = (ad + d[i])*(bd - d[i])/d[i];
	 }
         break;
       case 2:	/* vector copy */
         dummy = idiot;	/* pass it on to dummy to force the case */
         for(i=0;i<size;i+=stride){
           d[i] = a[i];
	 }
         break;
       case 3:	/* vector scale */
         dummy = idiot;	/* pass it on to dummy to force the case */
         for(i=0;i<size;i+=stride){
           d[i] = xtest*d[i];
	 }
         break;
       case 4:	/* vector add */
         dummy = idiot;	/* pass it on to dummy to force the case */
         for(i=0;i<size;i+=stride){
           d[i] = a[i] + b[i];
	 }
         break;
       case 5:	/* vector triad */
         dummy = idiot;	/* pass it on to dummy to force the case */
         for(i=0;i<size;i+=stride){
           d[i] = a[i] + xtest*b[i];
	 }
         break;
       case 6:	/* bogomegatrids (avg float rate, no divide) */
         dummy = idiot;	/* pass it on to dummy to force the case */
         for(i=0;i<size;i+=stride){
           d[i] = (ad + bd - cd)*d[i];
	 }
         break;
       case 7:	/* memtest, streaming (7) and random (8) */
       case 8:
         dummy = idiot;	/* pass it on to dummy to force the case */
         for (i=0; i<size; i+=stride){
           aindex = ai[i];
           aitmp = ai[aindex];
           ai[aindex] = aitmp;
	 }
         break;
       case 9:
         dummy = idiot;	/* pass it on to dummy to force the case */
         xtest = tan(atan(exp(log(sqrt(xtest*xtest)))));
         break;
       case 10:
         dummy = idiot;	/* pass it on to dummy to force the case */
         fread(&xtest,sizeof(xtest),1,fp);
         break;
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
     switch(testnum){
       case 0:
       default:
         dummy = idiot;	/* pass it on to dummy to force the case */
         break;
       case 1:
         dummy = idiot;	/* pass it on to dummy to force the case */
         for(i=0;i<size;i++){
           d[i] = (ad + d[i])*(bd - d[i])/d[i];
	 }
         break;
       case 2:	/* vector copy */
         dummy = idiot;	/* pass it on to dummy to force the case */
         for(i=0;i<size;i++){
           d[i] = a[i];
	 }
         break;
       case 3:	/* vector scale */
         dummy = idiot;	/* pass it on to dummy to force the case */
         for(i=0;i<size;i++){
           d[i] = xtest*d[i];
	 }
         break;
       case 4:	/* vector add */
         dummy = idiot;	/* pass it on to dummy to force the case */
         for(i=0;i<size;i++){
           d[i] = a[i] + b[i];
	 }
         break;
       case 5:	/* vector triad */
         dummy = idiot;	/* pass it on to dummy to force the case */
         for(i=0;i<size;i++){
           d[i] = a[i] + xtest*b[i];
	 }
         break;
       case 6:	/* bogomegatrids (avg float rate, no divide) */
         dummy = idiot;	/* pass it on to dummy to force the case */
         for(i=0;i<size;i++){
           d[i] = (ad + bd - cd)*d[i];
	 }
         break;
       /* Only difference is in how ai[i] is preloaded with indices */
       case 7:
       case 8:
         dummy = idiot;	/* pass it on to dummy to force the case */
         for (i=0; i<size; i+=stride){
           aindex = ai[i];
           aitmp = ai[aindex];
           ai[aindex] = aitmp;
	 }
         break;
       case 9:
         dummy = idiot;	/* pass it on to dummy to force the case */
         xtest = tan(atan(exp(log(sqrt(xtest*xtest)))));
         break;
       case 10:
         dummy = idiot;	/* pass it on to dummy to force the case */
         fread(&xtest,sizeof(xtest),1,fp);
         break;
     }
   }
   stop_timing();
   delta = delta_timing()/(double)full_iter;
   if(verbose == 11) printf("delta = %12.8e\n",delta);
   total_time += delta;
   total_time2 += delta*delta;
 }
 /* Do we need to adjust for stride? */
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
 switch(testnum){
   case 0:
   default:
     /*
      *==================================================================
      * testid = 0
      * 
      * This is the "empty test" or "null test".  It basically tests
      * how accurately we've measured the empty test loop time and hence 
      * how accurately we've managed to subtract it out of our filled
      * loop measurement in one of the real tests below.  We don't
      * normalize it at all because we correct the raw time before
      * normalizing it.  This SHOULD return "zero" (much less than the
      * the hardware clock, e.g. << 1 nanosecond).
      *==================================================================
      */
     avg_nanotime = (avg_time_full - avg_time_empty)*1.e+9;
     avg_megarate = 1.0e3/avg_nanotime;
     if(!quiet){
       printf("# Empty loop/Null test validation:\n");
       printf("#   dummy = idiot (basically null command) \n");
       printf("# Average Time: %6.2f nanoseconds\n",avg_nanotime);
       printf("# BogomegaRate: %6.2f megaempties per second\n",avg_megarate);
     } else {
       printf("%12d (loop passes)    %6.2f (bogomnulls)\n",size,avg_megarate);
     }
     break;
   case 1:
     /*
      * The original cpu-rate benchmark (that I wrote back in 1985 or so
      * when I wanted to "just measure floating point rates").
      *
      * We count length to be the size (in bytes) of any one of the
      * ad,bd,d vectors.  Note that nanotime_norm reflects the time for
      * an average (* / + -) instruction FOR streaming vectors of length size
      * at the given stride.
      */
     vector_length = size*sizeof(double);
     nanotime_norm = 4.0*size/(double) stride;
     avg_nanotime = (avg_time_full - avg_time_empty)*1.e+9/nanotime_norm;
     avg_megarate = 1.0e3/avg_nanotime;
     /* This is a comment */
     if(!quiet){
       printf("# Vector Double Precision Float averaged over four operations:\n");
       printf("#    d[i] = (ad + d[i])*(bd - d[i])/d[i]\n");
       printf("#    with d[i] = ad = bd = %12.6f\n",ad);
       printf("#    and vector size = %d (%d bytes)\n",size,size*sizeof(double));
       printf("# Average Time: %6.2f nanoseconds\n",avg_nanotime);
       printf("# BogomegaRate: %6.2f megafloats per second\n",avg_megarate);
     } else {
       printf("%12d (bytes)    %6.2f (bogomflops)\n",size*sizeof(double),avg_megarate);
     }
     break;
   case 2:
     /*
      * This is the stream copy benchmark, except that we malloc the
      * space and permit variable lengths and strides.
      *
      */
     vector_length = size*sizeof(double);
     nanotime_norm = 4.0*size/(double) stride;
     avg_nanotime = (avg_time_full - avg_time_empty)*1.e+9/(double)(size*stride);
     avg_megarate = 1.0e3/avg_nanotime;
     if(!quiet){
       printf("# Stream - Vector copy:\n");
       printf("#    d[i] = a[i]\n");
       printf("#    with a[i] = %12.6f\n",xtest);
       printf("#    and vector size = %d (%d bytes)\n",size,size*sizeof(double));
       printf("# Average Time: %6.2f nanoseconds\n",avg_nanotime);
       printf("# BogomegaRate: %6.2f megacopies per second\n",avg_megarate);
       printf("# Bandwidth:    %6.2f megabytes per second\n",2*sizeof(double)*avg_megarate);
     } else {
       printf("%12d (bytes)    %6.2f (bogomcopies)\n",size*sizeof(double),avg_megarate);
     }
     break;
   case 3:
     /*
      * This is the stream scale benchmark, except that we malloc the
      * space and permit variable lengths and strides.
      *
      */
     vector_length = size*sizeof(double);
     nanotime_norm = 4.0*size/(double) stride;
     avg_nanotime = (avg_time_full - avg_time_empty)*1.e+9/(double)(size*stride);
     avg_megarate = 1.0e3/avg_nanotime;
     if(!quiet){
       printf("#  Vector scale:\n");
       printf("#     d[i] = xtest*d[i]\n");
       printf("#     with d[i] = 0.0 and xtest = %12.6f\n",xtest);
       printf("#     and vector size = %d (%d bytes)\n",size,size*sizeof(double));
       printf("#  Average Time: %6.2f nanoseconds\n",avg_nanotime);
       printf("#  BogomegaRate: %6.2f megascales per second\n",avg_megarate);
       printf("#  Bandwidth:    %6.2f megabytes per second\n",2*sizeof(double)*avg_megarate);
     } else {
       printf("%12d (bytes)    %6.2f (bogomscales)\n",size*sizeof(double),avg_megarate);
     }
     break;
   case 4:
     /*
      * This is the stream add benchmark, except that we malloc the
      * space and permit variable lengths and strides.
      *
      */
     vector_length = size*sizeof(double);
     nanotime_norm = 4.0*size/(double) stride;
     avg_nanotime = (avg_time_full - avg_time_empty)*1.e+9/(double)(size*stride);
     avg_megarate = 1.0e3/avg_nanotime;
     if(!quiet){
       printf("#  Vector add:\n");
       printf("#     d[i] = a[i]+b[i]\n");
       printf("#     with a[i] = b[i] = %12.6f\n",xtest);
       printf("#     and vector size = %d (%d bytes)\n",size,size*sizeof(double));
       printf("#  Average Time: %6.2f nanoseconds\n",avg_nanotime);
       printf("#  BogomegaRate: %6.2f mega-adds per second\n",avg_megarate);
       printf("#  Bandwidth:    %6.2f megabytes per second\n",3*sizeof(double)*avg_megarate);
     } else {
       printf("%12d (bytes)    %6.2f (bogomadds)\n",size*sizeof(double),avg_megarate);
     }
     break;
   case 5:
     /*
      * This is the stream triad benchmark, except that we malloc the
      * space and permit variable lengths and strides.
      *
      */
     vector_length = size*sizeof(double);
     nanotime_norm = 4.0*size/(double) stride;
     avg_nanotime = (avg_time_full - avg_time_empty)*1.e+9/(double)(size*stride);
     avg_megarate = 1.0e3/avg_nanotime;
     if(!quiet){
       printf("#  Vector triad:\n");
       printf("#     d[i] = a[i]+xtest*b[i]\n");
       printf("#     with a[i] = xtest = %12.6f, b[i] = 0.0\n",xtest);
       printf("#     and vector size = %d (%d bytes)\n",size,size*sizeof(double));
       printf("#  Average Time: %6.2f nanoseconds\n",avg_nanotime);
       printf("#  BogomegaRate: %6.2f megatriads per second\n",avg_megarate);
       printf("#  Bandwidth:    %6.2f megabytes per second\n",3*sizeof(double)*avg_megarate);
     } else {
       printf("%12d (bytes)    %6.2f (bogomtriads)\n",size*sizeof(double),avg_megarate);
     }
     break;
   case 6:
     /*
      * This is a new stream-like benchmark that does a scalar add and
      * subtract before doing the scale.  The goal is to get some idea
      * of how fast the system does "arithmetic" without the inclusion
      * division.  It probably needs some work.
      */
     vector_length = size*sizeof(double);
     nanotime_norm = 4.0*size/(double) stride;
     avg_nanotime = (avg_time_full - avg_time_empty)*1.e+9/(3.0*size*stride);
     avg_megarate = 1.0e3/avg_nanotime;
     if(!quiet){
       printf("#  Vector Double Precision Float averaged over +-*:\n");
       printf("#     d[i] = (ad + bd - cd)*d[i]\n");
       printf("#     with ad = 1.0, d[i] = bd = cd = %12.6f\n",bd);
       printf("#     and vector size = %d (%d bytes)\n",size,size*sizeof(double));
       printf("#  Average Time: %6.2f nanoseconds\n",avg_nanotime);
       printf("#  BogomegaRate: %6.2f megatrids per second\n",avg_megarate);
     } else {
       printf("%12d (bytes)    %6.2f (bogomtrids)\n",size*sizeof(double),avg_megarate);
     }
     break;
   case 7:
     /*
      * This is the pretty much my old "memtest" benchmark, wrapped
      * up in this timing engine and fitted with a variable stride.
      * This form uses ai[i] = i, which effectly means that it streams
      * through memory vector style, getting two values from the
      * vector, putting one in j, then putting j back where it came
      * from.
      */
     vector_length = size*sizeof(double);
     nanotime_norm = (double) size/(double) stride;
     avg_nanotime = (avg_time_full - avg_time_empty)*1.e+9/nanotime_norm;
     avg_megarate = 1.0e3/avg_nanotime;
     if(!quiet){
       printf("# Sequential Integer Memory (read/write) Access:\n");
       printf("# stride = %d:\n",stride);
       printf("#   aitmp = ai[aindex]\n");
       printf("#   ai[aindex] = aitmp\n");
       printf("#   where aindex = ai[i] = i initially.\n");
       printf("# Average Time (raw): %6.2f \n",(avg_time_full - avg_time_empty)*1.e-9);
       printf("# Average Time: %6.2f nanoseconds\n",avg_nanotime);
       printf("# BogomegaRate: %6.2f megaseqmem int read/writes per second\n",avg_megarate);
     } else {
       printf("%12d (bytes)    %6.2f (bogomseqs) with stride %d\n",size*sizeof(double),avg_megarate,stride);
     }
     break;
   case 8:
     /*
      * This is the pretty much my old "memtest" benchmark, wrapped
      * up in this timing engine and fitted with a variable stride.
      * This form uses ai[i] = shuffle(0,1...size-1), which effectly 
      * means that it jumps all over the place in memory. ai[i] contains
      * an arbitrary shuffled index, so ai[ai[i]] is an arbitrary location
      * in the vector.  The contents of this location are then read and
      * written just as in the streaming example above.
      *
      * The lovely thing about this is that it SHOULD precisely
      * differentiate vector streaming times from random access times,
      * with random access defeating any stream/caching algorithm.
      */
     vector_length = size*sizeof(double);
     nanotime_norm = (double) size/(double) stride;
     avg_nanotime = (avg_time_full - avg_time_empty)*1.e+9/nanotime_norm;
     avg_megarate = 1.0e3/avg_nanotime;
     if(!quiet){
       printf("# Random Integer Memory (read/write) Access:\n");
       printf("#   aitmp = ai[aindex]\n");
       printf("#   ai[aindex] = aitmp\n");
       printf("#   where aindex = ai[i] = shuffled index in range initially.\n");
       printf("# Average Time (raw): %14.4e \n",(avg_time_full - avg_time_empty)*1.e-9);
       printf("# Average Time: %6.2f nanoseconds\n",avg_nanotime);
       printf("# BogomegaRate: %6.2f megarandmem int read/writes per second\n",avg_megarate);
     } else {
       printf("%12d (bytes)    %6.2f (bogomrans) with stride %d\n",size*sizeof(double),avg_megarate,stride);
     }
     break;
   case 9:
     /*
      * This is the old savage (transcendental) benchmark.  On many
      * systems transcendentals are done in software, in which case
      * savage performance will typically be very closely tied to
      * overall cpu clock and floating point speed.  On others (especially
      * ones with hardware transcendental support) it may be very
      * disproportionate to CPU clock.  The old x86/x87 architecture is
      * a good example of the latter -- the 8087 had hardware transcendentals
      * and run much faster than one expected based on its overall float speed
      * or clock.
      */
     vector_length = size*sizeof(double);
     nanotime_norm = 4.0*size/(double) stride;
     avg_nanotime = (avg_time_full - avg_time_empty)*1.e+9;
     avg_megarate = 5.0*1.0e3/avg_nanotime;
     if(!quiet){
       printf("#  Savage benchmark, averaged over five transcendental calls:\n");
       printf("#     xtest = tan(atan(exp(log(sqrt(xtest*xtest)))))\n");
       printf("#     with xtest = %12.6f\n",xtest);
       printf("#  Average Time: %6.2f nanoseconds\n",avg_nanotime);
       printf("#  BogomegaRate: %6.2f megatrancendentals per second\n",avg_megarate);
     } else {
       printf("%12d (bytes)    %6.2f (bogomtrancendentals)\n",size*sizeof(double),avg_megarate);
     }
     break;
   case 10:
     /*
      * This directly benchmark's the rate at which /dev/random returns
      * a "random number stream".
      */
     vector_length = size*sizeof(double);
     nanotime_norm = 4.0*size/(double) stride;
     avg_nanotime = (avg_time_full - avg_time_empty)*1.e+9;
     avg_megarate = 1.0e3/avg_nanotime;
     if(!quiet){
       printf("#  /dev/random benchmark:\n");
       printf("#     (fp open descriptor to /dev/random)\n");
       printf("#     fread(&xtest,sizeof(xtest),1,fp)\n");
       printf("#     with xtest = %12.6f\n",xtest);
       printf("#  Average Time: %6.2f nanoseconds\n",avg_nanotime);
       printf("#  BogomegaRate: %6.2f megarandoms per second\n",avg_megarate);
     } else {
       printf("%12d (bytes)    %6.2f (bogomrandoms)\n",size*sizeof(double),avg_megarate);
     }
     break;
 }

}
