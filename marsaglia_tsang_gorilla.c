/*
 * $Id: diehard_craps.c 191 2006-07-13 08:23:50Z rgb $
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *
 */

/*
 *========================================================================
 *========================================================================
 */

#include "dieharder.h"
/*
 * Test specific data
 */
#include "marsaglia_tsang_gorilla.h"

double marsaglia_tsang_gorilla()
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

 /*
  * Reseed FILE random number generators once per individual test.
  * This correctly resets the rewind counter per test.
  */
 if(strncmp("file_input",gsl_rng_name(rng),10) == 0){
   gsl_rng_set(rng,1);
 }

 test_header(dtest);
 /*
  * Any custom test header output lines go here.  They should be
  * used VERY sparingly.
  */

 /*
  * This is the standard test call.
  */
 kspi = 0;  /* Always zero first */
 pks = sample((void *)marsaglia_tsang_gorilla_test);

 /*
  * Test Results, standard form.
  */
 test_footer(dtest,pks,ks_pvalue,"Maraglia and Tsang Gorilla Test");

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

void marsaglia_tsang_gorilla_test()
{

 uint t,i,lag;
 Xtest ptest;

 /*
  * ptest.x = actual sum of tsamples lagged samples from rng
  * ptest.y = tsamples*0.5 is the expected mean value of the sum
  * ptest.sigma = sqrt(tsamples/12.0) is the standard deviation
  */
 ptest.x = 0.0;  /* Initial value */
 ptest.y = (double) tsamples*0.5;
 ptest.sigma = sqrt(tsamples/12.0);

 /*
  * sample only every lag returns from the rng, discard the rest.
  * We have to get the (float) value from the user input and set
  * a uint 
  */
 if(x_user){
   lag = x_user;
 } else {
   lag = 2; /* Why not?  Anything but 0, really... */
 }

 if(verbose == D_USER_TEMPLATE || verbose == D_ALL){
   printf("# marsaglia_tsang_gorilla(): Doing a test on lag %u\n",lag);
 }

 for(t=0;t<tsamples;t++){

   /*
    * A VERY SIMPLE test (probably not too sensitive)
    */

   /* Throw away lag-1 per sample */
   for(i=0;i<(lag-1);i++) gsl_rng_uniform(rng);

   /* sample only every lag numbers, reset counter */
   ptest.x += gsl_rng_uniform(rng);

 }

 Xtest_eval(&ptest);
 ks_pvalue[kspi] = ptest.pvalue;

 if(verbose == D_USER_TEMPLATE || verbose == D_ALL){
   printf("# marsaglia_tsang_gorilla(): ks_pvalue[%u] = %10.5f\n",kspi,ks_pvalue[kspi]);
 }

 kspi++;

}

void help_marsaglia_tsang_gorilla()
{

  printf("%s",dtest->description);

}
