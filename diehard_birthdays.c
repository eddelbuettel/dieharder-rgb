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

 unsigned int i,j,k,m,mnext,ns,nc;
 unsigned int nms;
 unsigned int rmask;
 unsigned int *rand_list,*intervals;
 unsigned int *jsamp;
 double lambda;

 /*
  * Should be 512, but we'll do it this way in case we want to vary it.
  */
 nms = pow(2,BMPOW);

 lambda = (double)nms*nms*nms/pow(2.0,26.0);
 
 /*
  * Forget size for this one; reallocate to nms in length.
  * Also get the rand_list, where we'll put the 24 bit masked result.
  */
 free(rand_int);
 rand_int  = (unsigned int *)malloc(nms*sizeof(unsigned int));
 rand_list = (unsigned int *)malloc(nms*sizeof(unsigned int));
 intervals = (unsigned int *)malloc(nms*sizeof(unsigned int));
 jsamp =     (unsigned int *)malloc(rmax_bits*sizeof(unsigned int));
 rmask = 1;
 for(i=0;i<BBITS-1;i++) {
  rmask = rmask << 1;
  rmask++;
 }
 /*
  * Zero the counter. Note that we only count the cyclic
  * permutations of the rmax_bits signficant bits from the
  * rng, and that we WILL sample each permutation samples
  * times below in each permutation slot.  The permutations
  * are arguably not independent, but each permutations
  * 500 samples are, so the p-values are all independently
  * valid.
  */
 for(nc=0;nc<rmax_bits;nc++){
   jsamp[nc] = 0;
 }

 /*
  * Create samples of the birthday problem.
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
    */
   for(nc=0;nc<rmax_bits;nc++){
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
      * We count the number of times an interval is repeated.
      * I presume that this includes those rare intervals that are
      * tripled, although that is very much not clear.  I'll
      * have to look for a reference to clarify this.
      */
     
     for(m=0;m<nms-1;m++){
       mnext = m+1;
       while(intervals[m] == intervals[mnext]){
          if(verbose){
            printf("intervals[%u] = %u == intervals[%u] = %u\n",
               m,intervals[m],mnext,intervals[mnext]);
	  }
	  if(mnext == m+1){
	    jsamp[nc] += 2;
	  } else {
            jsamp[nc]++;
	  }
	  mnext++;
       }
       if(mnext != m+1) m = mnext;
       
     }
     if(verbose){
       printf("jsamp[%u] = %u\n",ns,jsamp[ns]);
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
  * OK, it is very much time for bed, but the following is a sorted
  * list of counts of birthday intervals that match, with correct
  * handling of triples and higher (so three in a row counts as three,
  * and two in a row counts as two).  So all we have to do is compute
  * chisq for the jsamp vector, following Marsaglia, assuming a poisson
  * distribution.  But TOMORROW, not now... I'm dyin'.
  */
 gsl_sort_uint(jsamp,1,rmax_bits);
 for(nc=0;nc<rmax_bits;nc++){
   printf("jsamp[%u] = %u\n",nc,jsamp[nc]);
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

