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

static double lambda;
static unsigned int *intervals;
static unsigned int nms,nbits,kmax;

double diehard_birthdays()
{

 double pks;
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
  * From Marsaglia, nms should just be "512", but we'll do it this way in
  * case we want to vary it.  One day it would be useful to scale this
  * whole computation <bam!> up a notch (sorry, Emmeril:-).  Similarly,
  * nbits should be "24", but we can really make it anything we want
  * that's less than rmax_bits;
  */
 nms = 512;
 nbits = 24;
 
 free(rand_int);
 rand_int = (unsigned int *)malloc(nms*sizeof(unsigned int));
 intervals = (unsigned int *)malloc(nms*sizeof(unsigned int));

 if(nbits>rmax_bits) nbits = rmax_bits;

 /*
  * This is the one thing that matters.  We're going to make the
  * exact poisson distribution we expect below, and lambda has to
  * be right.  lambda = nms^3/4n where n = 2^nbits, for the record.
  */
 lambda = (double)nms*nms*nms/pow(2.0,(double)nbits+2.0);

 /*
  * This should be more than twice as many slots as we really
  * need for the Poissonian tail.  We're going to sample tsamples
  * times, and we only want to keep the histogram out to where
  * it has a reasonable number of hits/degrees of freedom, just
  * like we do with all the chisq's built on histograms.
  */
 kmax = 1;
 while(tsamples*gsl_ran_poisson_pdf(kmax,lambda)>5) kmax++;
 kmax++;   /* and one to grow on...*/

 if(testnum < 0){
   tempsamples = tsamples;
   tsamples = 100;  /* Standard value */
 }

 if(!quiet){
   printf("#==================================================================\n");
   printf("#                Diehard \"Birthdays\" test (modified).\n");
   printf("# Each test determines the number of matching intervals\n");
   printf("# from 512 \"birthdays\" drawn on a 24-bit \"year\".  This\n");
   printf("# is repeated and cumulated in a histogram %u times.\n",tsamples);
   printf("# Cumulated samples are then converted to chisq and p-values\n");
   printf("# based on the poisson distribution expectation values.\n");
   printf("#\n");
   printf("# The samples in this test are completely independent with no\n");
   printf("# cyclic/bitwise overlap -- it is therefore stronger than the\n");
   printf("# original (I think).   At any rate, it brings otherwise strong\n");
   printf("# generators to their metaphorical knees at -s 200\n");
   printf("#==================================================================\n");
   printf("# Random number generator tested: %s\n",gsl_rng_name(rng));
   printf("# %u samples drawn of %u-length random integer from inside cyclic\n",tsamples,rmax_bits);
   printf("# window on %u bits.  lambda = %f, kmax = %u\n",rmax_bits,lambda,kmax);
 }

 kspi = 0;  /* Always zero first */
 pks = sample((void *)diehard_birthdays_test);
 printf("p = %8.6f for diehard_birthdays test from Kuiper Komogorov-Smirnov test\n",pks);
 printf("     on %u pvalues.\n",kspi);
 if(pks < 0.0001){
   printf("Generator %s fails for diehard_birthdays.\n",gsl_rng_name(rng));
 }

 /*
  * Put rand_int back, as others might need it.  Free intervals.
  */
 free(rand_int);
 rand_int = (unsigned int *)malloc(size*sizeof(unsigned int));
 free(intervals);

 /*
  * Put back tsamples
  */
 if(testnum < 0){
   tsamples = tempsamples;
 }

 return(pks);

}

void diehard_birthdays_test()
{

 unsigned int i,j,k,t,m,mnext,ns;
 unsigned int bitstring,offset;
 unsigned int *js;
 double *pvalue,pks;

 /*
  * js[kmax] is the histogram we increment using the
  * count of repeated intervals as an index.  Clear it.
  */
 js = (unsigned int *)malloc(kmax*sizeof(unsigned int));
 for(i=0;i<kmax;i++) js[i] = 0;

 /*
  * Each sample uses a unique set of tsample rand_int[]'s, but evaluates
  * the Poissonian statistic for each cyclic rotation of the bits across
  * the 24 bit mask.
  */

 /*
  * Accumulate tsamples of this test into our histogram.
  */
 for(t=0;t<tsamples;t++) {

   /*
    * Create a list of 24-bit masked rands.  This is easy now that
    * we have get_bit_ntuple().  We use a more or less random offset
    * of the bitstring, and use one and only one random number
    * per bitstring, so that our samples are >>independent<<, and average
    * over any particular bit position used as a starting point with
    * cyclic/periodic bit wrap.
    */
   for(m = 0;m<nms;m++){
     bitstring = gsl_rng_get(rng);
     /* more or less random offset in the range of the bitstring */
     offset = bitstring%rmax_bits;
     rand_int[m] = get_bit_ntuple(&bitstring,1,nbits,offset);
     if(verbose == D_DIEHARD_BDAY || verbose == D_ALL){
       printf("  24-bit int = ");
       /* Should count dump from the right, sorry */
       dumpbits(&rand_int[m],32);
     }
   }

   /*
    * The actual test logic goes right here.  We have nms random ints
    * with 24 bits each.  We sort them.
    */
   if(verbose == D_DIEHARD_BDAY || verbose == D_ALL){
     for(m=0;m<nms;m++){
       printf("Before sort %u:  %u\n",m,rand_int[m]);
     }
   }
   gsl_sort_uint(rand_int,1,nms);
   if(verbose == D_DIEHARD_BDAY || verbose == D_ALL){
     for(m=0;m<nms;m++){
       printf("After sort %u:  %u\n",m,rand_int[m]);
     }
   }

   /*
    * We create the intervals between entries in the sorted
    * list and sort THEM.
    */
   intervals[0] = rand_int[0];
   for(m=1;m<nms;m++){
     intervals[m] = rand_int[m] - rand_int[m-1];
   }
   gsl_sort_uint(intervals,1,nms);
   if(verbose == D_DIEHARD_BDAY || verbose == D_ALL){
     for(m=0;m<nms;m++){
       printf("Sorted Intervals %u:  %u\n",m,intervals[m]);
     }
   }

   /*
    * We count the number of interval values that occur more than
    * once in the list.  Presumably that means that even if an interval
    * occurs 3 or 4 times, it counts only once!
    *
    * k is the interval count (Marsaglia calls it j).
    */
   k = 0;
   for(m=0;m<nms-1;m++){
     mnext = m+1;
     while(intervals[m] == intervals[mnext]){
       /* There is at least one repeat of this interval */
       if(mnext == m+1){
         /* increment the count of repeated intervals */
        k++;
       }
       if(verbose == D_DIEHARD_BDAY || verbose == D_ALL){
         printf("repeated intervals[%u] = %u == intervals[%u] = %u\n",
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
    * k now is the total number of intervals that occur more than once in
    * this sample of nms=512 numbers.  We increment the sample counter in
    * this slot.  If k is bigger than kmax, we simply ignore it -- it is a
    * BAD IDEA to bundle all the points from the tail into the last bin,
    * as a Poisson distribution can have a lot of points out in that tail!
    */
   if(k<kmax) js[k]++;
   if(verbose == D_DIEHARD_BDAY || verbose == D_ALL){
     printf("incremented js[%u] = %u\n",k,js[k]);
   }
       
 }


 /*
  * Let's sort the result (for fun) and print it out for each bit
  * position.
  */
 if(verbose == D_DIEHARD_BDAY || verbose == D_ALL){
   printf("#==================================================================\n");
   printf("# This is the repeated interval histogram:\n");
   for(k=0;k<kmax;k++){
     printf("js[%u] = %u\n",k,js[k]);
   }
 }


 /*
  * Fine fine fine.  We FINALLY have a distribution of the binned repeat
  * interval counts of many samples of 512 numbers drawn from 2^24.  We
  * should now be able to pass this vector of results off to a Pearson
  * chisq computation for the expected Poissonian distribution and
  * generate a p-value for each cyclic permutation of the bits through the
  * 24 bit mask.
  */
 ks_pvalue[kspi++] = chisq_poisson(js,lambda,kmax,tsamples);

 free(js);

}

