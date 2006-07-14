/*
 * $Id$
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *
 */

/*
 *========================================================================
 * This is the Diehard Birthdays test, rewritten from the description
 * in tests.txt on George Marsaglia's diehard site.
 *
 *               This is the BIRTHDAY SPACINGS TEST        
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
 *                            Comment
 *
 * I'm modifying this test several ways.  First, I'm running it on all 32
 * 24 bit strings implicit in the variables.  We'll do this by rotating
 * each variable by one bit position in between a simple run of the test.
 * A full run will therefore be 32 simple (rotated) runs on bits 1-24, and
 * we can do -p psamples runs to get a final set of p-values to evaluate.
 *========================================================================
 */


#include "dieharder.h"
/*
 * Test specific stuff
 */
#include "diehard_birthdays.h"

static double lambda;
static unsigned int *intervals;
static unsigned int nms,nbits,kmax;

double diehard_birthdays()
{

 double pks;
 uint ps_save,ts_save;

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
  * From Marsaglia, nms should just be "512", but this CAN be varied and
  * all his formulae still work.  Similarly, nbits should be "24", but we
  * can really make it anything we want that's less than or equal to
  * rmax_bits;
  */
 if(x_user != 0.0) {
   nms = (int)x_user;
 } else {
   nms = 512;  /* default value */
 }
 if(y_user != 0.0) {
   nbits = (int)y_user;
 } else {
   nbits = 24;
 }
 if(nbits>rmax_bits) nbits = rmax_bits;

 /*
  * This is the one thing that matters.  We're going to make the
  * exact poisson distribution we expect below, and lambda has to
  * be right.  lambda = nms^3/4n where n = 2^nbits, which is:
  *   lambda = (2^9)^3/(2^2 * 2^24) = 2^27/2^26 = 2.0
  * for Marsaglia's defaults.  Small changes in nms make big changes
  * in lambda, but we can easily pick e.g. nbits = 30, nms = 2048
  *   lambda =  (2^11)^3/2^32 = 2.0
  * and get the same test, but with a lot more samples and perhaps a
  * slightly smoother result.
  */
 lambda = (double)nms*nms*nms/pow(2.0,(double)nbits+2.0);

 /*
  * Allocate memory for THIS test's ks_pvalues, etc.  Make sure that
  * any missed prior allocations are freed.
  */
 if(ks_pvalue) nullfree(ks_pvalue);
 ks_pvalue  = (double *)malloc((size_t) psamples*sizeof(double));
 if(rand_int) nullfree(rand_int);
 rand_int = (unsigned int *)malloc(nms*sizeof(unsigned int));
 intervals = (unsigned int *)malloc(nms*sizeof(unsigned int));

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

 test_header(dtest);
 printf("# %u samples drawn from %u-bit integers masked out of a\n",nms,nbits);
 printf("# %u bit random integer.  lambda = %f, kmax = %u, tsamples = %u\n",rmax_bits,lambda,kmax,tsamples);
 if(tsamples > 1000){
   printf("#==================================================================\n");
   printf("# WARNING WARNING WARNING!  This test rarely requires more than\n");
   printf("#   -t 300 to make nearly any generator fail, and will take a\n");
   printf("#   very long time to run even there.  Consider restarting.\n");
   printf("#==================================================================\n");
 }


 /*
  * This is now the standard test call.
  */
 kspi = 0;  /* Always zero first */
 pks = sample((void *)diehard_birthdays_test);

 /*
  * Results of the mean test
  */
 test_footer(dtest,pks,ks_pvalue,"Diehard Birthdays Test");

 /*
  * Put back tsamples
  */
 if(all == YES){
   tsamples = ts_save;
   psamples = ps_save;
 }

 if(ks_pvalue) nullfree(ks_pvalue);
 if(rand_int) nullfree(rand_int);
 if(intervals) nullfree(intervals);

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

void help_diehard_birthdays()
{

 printf("%s",dtest->description);

}
