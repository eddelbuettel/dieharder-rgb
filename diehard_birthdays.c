/*
* $Id$
*
* See copyright in copyright.h and the accompanying file COPYING
* See also accompanying file STS.COPYING
*
*/

/*
 *========================================================================
 * This is the Diehard Birthdays test, rewritten from the description
 * in tests.txt on George Marsaglia's diehard site.
 *
 * Choose m birthdays in a year of n days.  List the spacings
 * between the birthdays.  If j is the number of values that 
 * occur more than once in that list, then j is asymptotically
 * Poisson distributed with mean m^3/(4n).  Experience shows n
 * must be quite large, say n>=2^18, for comparing the results
 * to the Poisson distribution with that mean.  This test uses
 * n=2^24 and m=2^9,  so that the underlying distribution for j
 * is taken to be Poisson with lambda=2^27/(2^26)=2.  A sample
 * of 500 j's is taken, and a chi-square goodness of fit test
 * provides a p value.  The first test uses bits 1-24 (counting
 * from the left) from integers in the specified file.       
 *   Then the file is closed and reopened. Next, bits 2-25 are
 * used to provide birthdays, then 3-26 and so on to bits 9-32.
 * Each set of bits provides a p-value, and the nine p-values
 * provide a sample for a KSTEST.
 *
 * I'm modifying this test several ways.  First, I'm running it on all 32
 * 24 bit strings implicit in the variables.  We'll do this by rotating
 * each variable by one bit position in between a simple run of the test.
 * A full run will therefore be 32 simple (rotated) runs on bits 1-24, and
 * we can do -s samples runs to get a final set of p-values to evaluate.
 * Second, instead of KS I'm using the STS tests -- looking at confidence
 * intervals as well as goodness of fit on a uniform distribution of
 * p-values.
 *========================================================================
 */


#include "rand_rate.h"

#define BLEN  32
#define BBITS 24
/* #define BMPOW 9 */
#define BMPOW 9

int diehard_birthdays()
{

 unsigned int i,j,k,kmax,m,mnext,ns,nc;
 unsigned int nms;
 unsigned int rmask;
 unsigned int *rand_list,*intervals;
 unsigned int **js;
 double lambda,*pvalue;

 /*
  * Should be 512, but we'll do it this way in case we want to vary it.
  */
 nms = pow(2,BMPOW);

 lambda = (double)nms*nms*nms/pow(2.0,26.0);
 /*
  * This should be more than twice as many slots as we really
  * need for the Poissonian tail.
  */
 kmax = 1;
 while(samples*gsl_ran_poisson_pdf(kmax,lambda)>5) kmax++;
 kmax++;   /* and one to grow on...*/
 printf("# Setting number of bins kmax = %u.\n",kmax);
 
 /*
  * Forget size for this one; reallocate to nms in length.
  * Also get the rand_list, where we'll put the 24 bit masked result.
  */
 free(rand_int);
 rand_int  = (unsigned int *)malloc(nms*sizeof(unsigned int));
 rand_list = (unsigned int *)malloc(nms*sizeof(unsigned int));
 intervals = (unsigned int *)malloc(nms*sizeof(unsigned int));
 rmask = 1;
 for(i=0;i<BBITS-1;i++) {
  rmask = rmask << 1;
  rmask++;
 }

 /*
  * js[rmax_bits][kmax] is the histogram we increment using the
  * count of repeated intervals as an index, for each cyclic
  * permutation of bits.
  */
 js = (unsigned int **)malloc(rmax_bits*sizeof(unsigned int *));
 for(i=0;i<rmax_bits;i++){
   js[i] = (unsigned int *)malloc(kmax*sizeof(unsigned int));
   for(j=0;j<kmax;j++) js[i][j] = 0;
 }
 pvalue = (double *)malloc(rmax_bits*sizeof(double));

 /*
  * Each sample uses a unique set of rand_int[]'s, but evaluates
  * the Poissonian statistic for each cyclic rotation of the bits
  * across the 24 bit mask.
  */
 for(ns=0;ns<samples;ns++){

   /*
    * Fill vector of rands to begin.
    */
   for(m=0;m<nms;m++) {
     rand_int[m] = gsl_rng_get(rng);
   }

   /*
    * Cycle over all rmax_bits cyclic bit permutations (which we'll take
    * 24 bits at a time).  We really need to test to make sure
    * that rmax_bits >= 24...
   for(nc=0;nc<1;nc++){
   */
   for(nc=0;nc<rmax_bits;nc++){

     /*
      * k is the interval count for THIS pass through THIS list (what
      * Marsaglia calls j).
      */
     k = 0;
     for(m = 0;m<nms;m++){
       /*
        * Create a list of 24-bit masked rands
        */
       rand_list[m] = rand_int[m]&rmask;
       if(verbose){
         printf("Original int = ");
         dumpbits(&rand_int[m],32);
         printf("  Masked int = ");
         dumpbits(&rand_list[m],32);
       }
     }

     /*
      * The actual test logic goes right here.  We have nms random ints
      * with 24 bits each.  We sort them.
      */
     if(verbose){
       for(m=0;m<nms;m++){
         printf("Before %u:  %u\n",m,rand_list[m]);
       }
     }
     gsl_sort_uint(rand_list,1,nms);
     if(verbose){
       for(m=0;m<nms;m++){
         printf("After %u:  %u\n",m,rand_list[m]);
       }
     }

     /*
      * We create the intervals between entries in the sorted
      * list and sort THEM.
      */
     intervals[0] = rand_list[0];
     for(m=1;m<nms;m++){
       intervals[m] = rand_list[m] - rand_list[m-1];
     }
     gsl_sort_uint(intervals,1,nms);
     if(verbose){
       for(m=0;m<nms;m++){
         printf("Intervals %u:  %u\n",m,intervals[m]);
       }
     }

     /*
      * We count the number of interval values that occur more than
      * once in the list.  Presumably that means that even if an interval
      * occurs 3 or 4 times, it counts only once!
      */
     
     for(m=0;m<nms-1;m++){
       mnext = m+1;
       while(intervals[m] == intervals[mnext]){
          /* There is at least one repeat of this interval */
	  if(mnext == m+1){
	    /* increment the count of repeated intervals */
	    k++;
	  }
          if(verbose){
            printf("intervals[%u] = %u == intervals[%u] = %u\n",
               m,intervals[m],mnext,intervals[mnext]);
	  }
	  mnext++;
       }
       /*
        * Skip all the rest that were identical.
	*/
       if(mnext != m+1) m = mnext;
     }
     /*
      * k now is the total number of intervals that occur more than
      * once in this sample and permutation.  So we increment the
      * sample counter for this permutation in this slot.  If k is
      * bigger than kmax, we simply ignore it -- it is a BAD IDEA
      * to bundle all the points from the tail into the last bin,
      * as a Poisson distribution can have a lot of points out in that
      * tail!
      */
     if(k<kmax) js[nc][k]++;
     if(verbose){
       printf("incremented js[%u][%u] = %u\n",nc,k,js[nc][k]);
     }
       

     /*
      * Done with a test on rand_list[].  Now we cycle each int in our
      * original list one bit to the right (with wraparound).
      */
     for(m = 0;m<nms;m++){
       cycle(&rand_int[m],rmax_bits);
     }
   }
 }

 /*
  * Let's sort the result (for fun) and print it out for each bit
  * position.
  */
 if(verbose){
   for(nc=0;nc<rmax_bits;nc++){
     printf("#==================================================================\n");
     for(k=0;k<kmax;k++){
       printf("js[%u][%u] = %u\n",nc,k,js[nc][k]);
     }
   }
 }


 /*
  * Fine fine fine.  We FINALLY have a distribution per cyclic permutation of
  * the binned repeat interval counts of many samples of 512 numbers drawn
  * from 2^24 (24 bit integer masks).  We should now be able to pass
  * EACH vector of results off to a Pearson chisq computation for the
  * expected Poissonian distribution and generate a p-value for each
  * cyclic permutation of the bits through the 24 bit mask.
  */
 for(nc=0;nc<rmax_bits;nc++){
    pvalue[nc] = chisq_poisson(js[nc],lambda,kmax);
    printf("p-value[%u] = %f\n",nc,pvalue[nc]);    
 }
 

 if(!quiet){
   printf("#==================================================================\n");
   printf("#                Diehard \"Birthdays\" test (modified).\n");
   printf("# Each sample determines the number of matching intervals\n");
   printf("# from 512 \"birthdays\" drawn on a 24-bit \"year\".\n");
   printf("# Cumulated samples are then converted to chisq and p-values.\n");
   printf("#==================================================================\n");
   printf("# Random number generator tested: %s\n",gsl_rng_name(rng));
   printf("# %u samples drawn of %u cyclic permutations.\n",samples,rmax_bits);
   printf("# lambda = %f\n",lambda);
 }

}

