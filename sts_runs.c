/*
 * $Id$
 *
 * See copyright in copyright.h and the accompanying file COPYING
 * See also accompanying file STS.COPYING
 *
 */

/*
 *========================================================================
 * This is a the monobit test, rewritten from the description in the
 * STS suite.
 *
 * Rewriting means that I can standardize the interface to gsl-encapsulated
 * routines more easily.  It also makes this my own code.
 *========================================================================
 */

#include "dieharder.h"
/*
 * Test specific data
 */
#include "sts_runs.h"

double sts_runs()
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
  * Allocate memory for THIS test's ks_pvalues, etc.  Make sure that
  * any missed prior allocations are freed.
  */
 if(ks_pvalue) nullfree(ks_pvalue);
 ks_pvalue  = (double *)malloc((size_t) psamples*sizeof(double));

 test_header(dtest);
 /*
  * Any custom test header output lines go here.  They should be
  * used VERY sparingly.
  */

 /*
  * This is the standard test call.
  */
 kspi = 0;  /* Always zero first */
 pks = sample((void *)sts_runs_test);

 /*
  * Test Results, standard form.
  */
 test_footer(dtest,pks,ks_pvalue,"STS Runs Test");

 /*
  * Put back tsamples
  */
 if(all == YES){
   tsamples = ts_save;
   psamples = ps_save;
 }

 if(ks_pvalue) nullfree(ks_pvalue);

 return(pks);

}

void sts_runs_test()
{

 int i,b,t;
 unsigned int nbits,bitstring,value;
 Xtest mtest;
 double pks_monobit,pones,c00,c01,c10,c11;;

 /*
  * Start by running monobit, and quitting if it fails
 pks_monobit = sts_monobit();
 if(pks_monobit < 0.0001) return;
  */

 /*
  * Number of total bits from -t tsamples = size of rand_int[]
  */
 bits = rmax_bits*tsamples;
 mtest.x = 0.0;
 strncpy(mtest.testname,"sts_runs",128);
 strncpy(mtest.rngname,gsl_rng_name(rng),128);

 /*
  * Create entire bitstring to be tested
  */
 for(t=0;t<tsamples;t++){
   rand_int[t] = gsl_rng_get(rng);
 }

 /*
  * Fill vector of "random" integers with selected generator.
  * NOTE WELL:  This can also be done by reading in a file!
  */
 pones = 0.0;
 c00 = 0.0;
 c01 = 0.0;
 c10 = 0.0;  /* Equal to c01 by obvious periodic symmetry */
 c11 = 0.0;
 for(b=0;b<bits;b++){
   /*
    * This gets the integer value of the ntuple at index position
    * n in the current bitstring, from a window with cyclic wraparound.
    */
   value = get_bit_ntuple(rand_int,tsamples,2,b);
   switch(value){
     case 0:   /* 00 no new ones */
       c00++;
       break;
     case 1:   /* 01 no new ones */
       c01++;
       mtest.x++;
       break;
     case 2:   /* 10 one new one (from the left) */
       c10++;
       mtest.x++;
       pones++;
       break;
     case 3:   /* 11 one new one (from the left) */
       c11++;
       pones++;
       break;
   }
   if(verbose == D_STS_RUNS || verbose == D_ALL){
     printf("# sts_runs(): mtest.x = %f, pone = %f\n",mtest.x,pones);
   }
 }
 /*
  * form the probability of getting a one in the entire sample
  */
 pones /= (double) tsamples*rmax_bits;
 c00 /= (double) tsamples*rmax_bits;
 c01 /= (double) tsamples*rmax_bits;
 c10 /= (double) tsamples*rmax_bits;
 c11 /= (double) tsamples*rmax_bits;
 /* printf(" p = %f c00 = %f c01 = %f c10 = %f c11 = %f\n",pones,c00,c01,c10,c11); */
 mtest.y = 2.0*bits*pones*(1.0-pones);
 mtest.sigma = 2.0*sqrt(bits)*pones*(1.0-pones);
 Xtest_eval(&mtest);
 ks_pvalue[kspi] = mtest.pvalue;
 if(verbose == D_STS_MONOBIT || verbose == D_ALL){
   printf("# sts_monobit(): ks_pvalue[%u] = %10.5f\n",kspi,ks_pvalue[kspi]);
 }
 kspi++;
 
}

void help_sts_runs()
{

 printf("%s",dtest->description);

}
