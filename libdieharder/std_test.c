/*
 * $Id: rgb_bitdist.c 225 2006-08-17 13:15:00Z rgb $
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *
 */

#include <dieharder/libdieharder.h>

/*
 * A standard test returns a single-pass p-value as an end result.
 * The "size" of the single pass test is controlled by the tsamples
 * parameter, where possible (some tests use statistics evaluated only
 * for very particular values of tsamples.
 *
 * The test is, in general, executed psamples times and the p-values
 * for each run are in turn accumulated in a vector.  This can all be
 * done in a very standard loop.
 *
 * These values SHOULD be uniformly distributed from [0-1].  A
 * Kuiper Kolmogorov-Smirnov test is performed on the final distribution
 * of p to generate a p-value for the entire test series, containing
 * (typically) tsamples*psamples actual samples.
 *
 * Some tests generate more than one p-value per single pass.  Others
 * are designed to be iterated over yet another integer-indexed control
 * parameter.  To facilitate the consistent pass-back of test results to
 * a UI and allow test reuse without leaks, it is important to be able
 * to create "a standard test" and destroy/free it when done.  For this
 * reason the API for the library standard test function is quite
 * object-oriented in its implementation.
 *
 * It is strongly suggested that this object oriented design be reused
 * whenever possible when designing new tests.  This also permits the
 * maximal reuse of code in the UI or elsewhere.
 */

/*
 * Create a new test that will return nkps p-values per single pass,
 * for psamples passes.  dtest is a pointer to a struct containing
 * the test description and default values for tsamples and psamples.
 * This should be called before a test is started in the UI.
 */
Test **create_test(Dtest *dtest, uint tsamples,uint psamples)
{

 int i,j,k;
 Test **newtest;

 MYDEBUG(D_STD_TEST){
   fprintf(stdout,"# create_test(): About to create test %s\n",dtest->sname);
 }

 /*
  * Here we have to create a vector of tests of length nkps
  */
 /* printf("Allocating vector of pointers to Test structs of length %d\n",dtest->nkps); */
 newtest = (Test **)malloc((size_t) dtest->nkps*sizeof(Test *));
 for(i=0;i<dtest->nkps;i++){
   /* printf("Allocating the actual test struct for the %d th test\n",i); */
   newtest[i] = (Test *)malloc(sizeof(Test));
 }

 /*
  * Initialize the newtests
  */
 for(i=0;i<dtest->nkps;i++){

   /*
    * Do a standard test if -a(ll) is selected no matter what people enter
    * for tsamples or psamples.  ALSO use standard values if tsamples or
    * psamples are 0 (not initialized).
    */
   if(all == YES || tsamples == 0){
     newtest[i]->tsamples = dtest->tsamples_std;
   } else {
     newtest[i]->tsamples = tsamples;
   }
   if(all == YES || psamples == 0){
     newtest[i]->psamples = dtest->psamples_std;
   } else {
     newtest[i]->psamples = psamples;
   }
     
   /*
    * Now we can malloc space for the pvalues vector, and a
    * single (80-column) LINE for labels for the pvalues.  We default
    * the label to a line of #'s.
    */
   newtest[i]->pvalues = (double *)malloc((size_t)newtest[i]->psamples*sizeof(double));
   newtest[i]->pvlabel = (char *)malloc((size_t)LINE*sizeof(char));
   snprintf(newtest[i]->pvlabel,LINE,"##################################################################\n");
   for(j=0;j<newtest[i]->psamples;j++){
     newtest[i]->pvalues[j] = 0.0;
   }

   /*
    * Finally, we initialize ks_pvalue "just because".
    */
   newtest[i]->ks_pvalue = 0.0;

   /*
   printf("Allocated and set newtest->tsamples = %d\n",newtest[i]->tsamples);
   printf("Allocated and set newtest->psamples = %d\n",newtest[i]->psamples);
   printf("Allocated a vector of pvalues at %0x\n",newtest[i]->pvalues);
   */

 }

 /* printf("Allocated complete test struct at %0x\n",newtest); */
 return(newtest);

}

/*
 * Destroy (free) a test created with create_test without leaking.
 * This should be called as soon as a test is finished in the UI.
 */
void destroy_test(Dtest *dtest, Test **test)
{

 int i;

 /*
  * To destroy a test one has to first free its allocated contents
  * or leak.
  */
 /*
 printf("Destroying test %s\n",dtest->name);
 printf("Looping over %d test pvalue vectors\n",dtest->nkps);
 */
 for(i=0;i<dtest->nkps;i++){
   free(test[i]->pvalues);
   free(test[i]->pvlabel);
 }
 /* printf("Freeing all the test structs\n"); */
 for(i=0;i<dtest->nkps;i++){
   free(test[i]);
 }
 /* printf("Freeing the toplevel test struct at %0x\n",test); */
 free(test);

}

void std_test(Dtest *dtest, Test **test)
{

 int i,j;

 /*
  * This is very simple.  Run the test psamples times, using the value
  * set for psamples when the test was created.
  */
 for(i=0;i<test[0]->psamples;i++){

   dtest->test(test,i);

 }

 /*
  * Then evaluate the final test p-values for each individual test
  * statistic computed during the one run of nkps trials.  Note
  * that we could use kstest_kuiper or kstest (anderson-darling)
  * here, and that we REALLY should enable choosing one or the
  * other with a flag.
  */
 for(j = 0;j < dtest->nkps;j++){
   if(ks_test == 1){
     /* This can be selected with -k 1 from the command line */
     test[j]->ks_pvalue = kstest(test[j]->pvalues,test[j]->psamples);
   } else {
     /* This is default */
     test[j]->ks_pvalue = kstest_kuiper(test[j]->pvalues,test[j]->psamples);
   }
 }

}

