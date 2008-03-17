/*
 * See copyright in copyright.h and the accompanying file COPYING
 */

/*
 *========================================================================
 * This is a test that checks to see if the rng generates bit patterns
 * (n-tuples) that are distributed correctly (binomially).  For example,
 * for 2-tuples (bit pairs) there are four possibilities: 00, 01, 10, 11.
 * Each should occur with a probability of 1/4, hence the count of (say) K
 * 00 bitpairs out of N trials should be distributed (over M samples)
 * according to the binomial probability p = binomial(N,K,0.25) -- that
 * is, the expected count for k 00's is M*p(N,K,0.25).
 *
 * This test should be more sensitive than just doing a large number of
 * trials and aggregating all the 00's over all the samples and comparing
 * the result to the expected mean, as there can be distributions that
 * have the expected mean of 0.25 that are >>not<< binomially distributed.
 *
 * By making a single program capable of doing any n-tuple, we gain
 * immediate benefit.  Both STS and Diehard, for example, contain
 * tests that validate bit distribution frequencies; however, they typically
 * do so for specific n-tuples, e.g. 5 in several diehard tests.  There
 * is nothing terribly special about 5, and no point in looking too
 * hard at e.g. intervals between patterns at 5, since testing pure
 * frequency at 6 bits simultaneously ensures that all 6 bit patterns have
 * the correct frequency (since they are sub-patterns of 6 bit patterns
 * that have the correct frequency, and if they had the INcorrect frequency
 * the six-bit patterns would as well by inheritance) and (less obviously,
 * but I'm pretty sure I can prove it) that intervals between the five
 * bit patterns must also be correct IF the test is satisfied robustly
 * for various arbitrary test string lengths and sampling counts.
 *
 * Anyway, I'm giving this one a shot as it may end up being a one-size
 * fits all tool for validating bitlevel randomness for any generator,
 * up to some n-tuple.  I expect that 2004/2005 computers will easily
 * be able to validate through n=8 without really warming up or taking
 * terribly long (validating the frequency and distribution for e.g.
 * the entire ascii alphabet at the byte level) and MAY be able to do
 * actual frequency validation on 16-bit integers.  Note that it only takes
 * a few seconds to generate millions of 32 bit integers with most of
 * the GSL routines.  To get statistically valid results, one needs to
 * accumulate order of 100 trials per outcome, or test order of millions
 * of integers and accumulate the results in an outcome vector 64K long.
 * both of these are well within reach using at most hours of CPU time,
 * and partitioned on a beowulf might take only minutes or even seconds.
 *
 * The latest version of the actual test (below) tests only ONE ntuple,
 * the value set in the global variable sts_serial_ntuple which must
 * be a positive integer.  The calling program is responsible for e.g.
 * testing a range of ntuples.
 *========================================================================
 */

/*
 *                      Overlapping Test
 *==================================================================
 * We fill input with inlen uints. The test rules are that
 * nb < |log_2 (inlen*32) | - 2.  Or inverting (which is
 * more useful) inlen = 2^(nb+2)/sizeof(uint).  For example, if
 * nb=8, inlen = 1024/32 = 32.  This is obviously very convervative.
 * Even if nb=16 (testing 65K number) it is only 2^18/2^5 = 2^13 or
 * around 8 MB of uints.
 *
 * To make this test fast, we should very likely just use
 * inlen = 2^nb UINTS, which is 8x the minimum recommended and easy
 * to compute, e.g. for nb = 16 we have input[65536], containing
 * 2^24 = 16 Mbits.  We then allocate one extra uint at the end,
 * fill it, copy the 0 uint to the end for periodic wrap, and
 * simply rip a uint window along it with the inline shift trick
 * to generate the overlapping samples.  The code we develop can
 * likely be backported to bits.c as being a bit more efficient.
 */

#include <dieharder/libdieharder.h>

#include "static_get_bits.c"

/*
 * This is a buffer of uint length (2^nb)+1 that we will fill with rands
 * to be tested, once per test.
 */
uint *uintbuf;
char *charbuf;

void sts_serial(Test **test,int irun)
{

 uint bsize;       /* number of bits/samples in uintbuf */
 uint nb;          /* number of bits in a tested ntuple */
 uint value_max;   /* 2^{nb}, basically (max value of nb bit word + 1) */
 uint value;       /* value of sampled ntuple (as a uint) */
 uint mask;        /* mask in only nb bits */
 uint bi;          /* bit offset relative to window */

 /* Look for cruft below */

 uint i,j,n,m,t;            /* generic loop indices */
 uint **freq,*psi2,ctotal;  /* count of any ntuple per bitstring */
 uint window;  /* uint window into uintbuf, slide along a byte at at time. */

 double pvalue; /* standard return */

 /*
  * Sample a bitstring of rgb_bitstring_ntuple in length (exactly).  Note
  * that I'm going to force nb>=2, nb<=16 for the moment.  The routine
  * might "work" for nb up to 24 or even (on a really big memory machine)
  * 32, but memory requirements in uints are around 2^(nb+1) + n where n
  * is order unity, maybe worse, so I think it would take a 12 to 16 GB
  * machine to be able to just hold the memory, and then it would take a
  * very long time.  However, the testing process might parallelize
  * decently if we could farm out the blocks of bits to be run efficiently
  * over a network relative to the time required to generate them.
  */
 if(sts_serial_ntuple > 1 && sts_serial_ntuple < 17){
   nb = sts_serial_ntuple;        /* To save typing, mostly... */
   tsamples = test[0]->tsamples;  /* ditto */
   MYDEBUG(D_STS_SERIAL){
     printf("#==================================================================\n");
     printf("# Starting sts_serial.\n");
     printf("# sts_serial: Testing ntuple = %u\n",nb);
   }
 } else {
   printf("#==================================================================\n");
   printf("# Starting sts_serial.\n");
   fprintf(stderr,"Error:  sts_serial_ntuple must be in [2,16].  Exiting.\n");
   exit(0);
 }

 /*
  * We need a vector of freq vectors, and a smaller vector of psi^2
  * accumulators in order to implement the STS test.  We make each
  * frequency counter vector only as large as it needs to be.  We
  * zero each accumulator as we make it.
  */
 freq = (uint **) malloc(17*sizeof(uint *));
 for(m = 1;m < 17;m++) {
   freq[m] = (uint *)malloc(pow(2,m)*sizeof(uint));
   memset(freq[m],0,pow(2,m)*sizeof(uint));
 }

 /*
  * psi2 is where we accumulate the statistics when we're done.  We
  * zero it here to save time later.
  */
 psi2 = (uint *) malloc(17*sizeof(uint *));
 memset(psi2,0,17*sizeof(uint));

 /*
  * uintbuf is the one true vector of rands processed during this test.
  * It needs to be at LEAST 2^16 uints in length, 2^20 is much better.
  * We'll default this from tsamples, though.
  */
 uintbuf = (uint *)malloc((tsamples+1)*sizeof(uint));

 /*
  * The largest integer for this ntuple is 2^nb-1 (they range from 0 to
  * 2^nb - 1).  However, this is used to size count and limit loops, so
  * we use 2^nb and start indices from 0 as usual.
  */
 value_max = (uint) pow(2,nb);
 MYDEBUG(D_STS_SERIAL){
   printf("# sts_serial(): value_max = %u\n",value_max);
 }

 /*
  * If uintbuf[test[0]->tsamples] is allocated (plus one for wraparound)
  * then we need to count the number of bits, which is the number of
  * OVERLAPPING samples we will pull.
  */
 bsize = tsamples*sizeof(uint)*CHAR_BIT;
 MYDEBUG(D_STS_SERIAL){
   printf("# sts_serial(): bsize = %u\n",bsize);
 }

 /*
  * We start by filling testbuf with rands and cloning the first into
  * the last slot for cyclic wrap.  This initial effort just uses the
  * fast gsl_rng_get() call, but we'll need to use the inline from the
  * static_get_rng routines in production.
  */
 for(t=0;t<tsamples;t++){
   /* uintbuf[t] = get_rand_bits_uint(32,0xFFFFFFFF,rng); */
   uintbuf[t] = gsl_rng_get(rng);
   MYDEBUG(D_STS_SERIAL){
     printf("# sts_serial(): %u:  ",t);
     dumpuintbits(&uintbuf[t],1);
     printf("\n");
   }
 }
 uintbuf[tsamples] = uintbuf[0];   /* Periodic wraparound */
 MYDEBUG(D_STS_SERIAL){
   printf("# sts_serial(): %u:  ",tsamples);
   dumpuintbits(&uintbuf[tsamples],1);
   printf("\n");
 }

 /*
  * We now in ONE PASS loop over all of the possible values of m from
  * 1 to 16.
  */
 for(m=1;m<17;m++){
 
   /*
    * Set the mask for bits to be returned.  This will work fine for
    * m in the allowed range, and will fail if m = 32 if that ever
    * happens in the future.
    */
   mask = 0;
   for(i = 0; i < m; i++){
      mask |= (1u << m);
   }
   mask = ((1u << m) - 1);

   /* Initialize index and count total */
   j = 0;
   ctotal = 0;
   MYDEBUG(D_STS_SERIAL){
     printf("looping bsize = %u times\n",bsize);
   }
   for(t=0;t<bsize;t++){
     /*
      * Offset of current sample, relative to left boundary of window.
      */
     if((t%32) == 0){
       window = uintbuf[j];  /* start with window = testbuf = charbuf */
       MYDEBUG(D_STS_SERIAL){
         printf("uintbuf[%u] = %08x = ",j,window);
         dumpuintbits(&window,1);
         printf("\n");
       }
       j++;
     }
     bi = t%16;
     value = (window >> (32 - m - bi)) & mask;
     MYDEBUG(D_STS_SERIAL){
       dumpbitwin(value,nb);
       printf("\n");
     }
     freq[m][value]++;
     ctotal++;
     if(bi == 15){
       window = (window << 16);
       window += (uintbuf[j] >> 16);
       MYDEBUG(D_STS_SERIAL){
         printf("window[%u] = %08x = ",j,window);
         dumpuintbits(&window,1);
         printf("\n");
       }
     }
   }

   MYDEBUG(D_STS_SERIAL){
     printf("# sts_serial():=====================================================\n");
     printf("# sts_serial():                  Count table\n");
     printf("# sts_serial():\tbits\tvalue\tcount\tprob\n");
     for(i = 0; i<pow(2,m); i++){
       printf("# sts_serial():   ");
       dumpbitwin(i,m);
       printf("\t%u\t%u\t%f\n",i,freq[m][i],(double) freq[m][i]/ctotal);
     }
     printf("# sts_serial(): Total count = %u, target probability = %f\n",ctotal,1.0/pow(2,m));
   }

 } /* End of m loop */

 /*
  * Now it is time to implement the statistic from STS SP800 whatever.
  */
  

 exit(0);

}

