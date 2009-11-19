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

 uint i,j,k;
 uint pcutoff;
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
  * Initialize the newtests.  The implementation of TTD (test to
  * destruction) modes makes this inevitably a bit complex.  In
  * particular, we have to malloc the GREATER of Xoff and psamples in
  * newtest[i]->pvalues to make room for more pvalues right up to the
  * Xoff cutoff.
  */
 for(i=0;i<dtest->nkps;i++){

   /*
    * Do a standard test if -a(ll) is selected no matter what people enter
    * for tsamples or psamples.  ALSO use standard values if tsamples or
    * psamples are 0 (not initialized).  HOWEVER, note well the new control
    * for psamples that permits one to scale the standard number of psamples
    * in an -a(ll) run by multiply_p.
    */
   if(all == YES || tsamples == 0){
     newtest[i]->tsamples = dtest->tsamples_std;
   } else {
     newtest[i]->tsamples = tsamples;
   }
   if(all == YES || psamples == 0){
     newtest[i]->psamples = dtest->psamples_std*multiply_p;
	 if (newtest[i]->psamples < 1) newtest[i]->psamples = 1;
   } else {
     newtest[i]->psamples = psamples;
   }

   /* Give ntuple an initial value of zero; most tests will set it. */
   newtest[i]->ntuple = 0;

   /*
    * Now we can malloc space for the pvalues vector, and a
    * single (80-column) LINE for labels for the pvalues.  We default
    * the label to a line of #'s.
    */
   if(Xtrategy != 0 && Xoff > newtest[i]->psamples){
     pcutoff = Xoff;
   } else {
     pcutoff = newtest[i]->psamples;
   }
   newtest[i]->pvalues = (double *)malloc((size_t)pcutoff*sizeof(double));
   newtest[i]->pvlabel = (char *)malloc((size_t)LINE*sizeof(char));
   snprintf(newtest[i]->pvlabel,LINE,"##################################################################\n");
   for(j=0;j<pcutoff;j++){
     newtest[i]->pvalues[j] = 0.0;
   }

   /*
    * Finally, we initialize ks_pvalue "just because".
    */
   newtest[i]->ks_pvalue = 0.0;

   MYDEBUG(D_STD_TEST){
     printf("Allocated and set newtest->tsamples = %d\n",newtest[i]->tsamples);
     printf("Xtrategy = %u -> pcutoff = %u\n",Xtrategy,pcutoff);
     printf("Allocated and set newtest->psamples = %d\n",newtest[i]->psamples);
   }

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
  * A standard test is just a bit complex due to the introduction of
  * Xtrategies.  If Xtrategy = 0, we do the usual/normal thing and
  * just run the test psamples times.  If it is NONzero, though, we
  * do some moderately complicated stuff -- either run until Xoff is
  * reached OR failure occurs
  * This is very simple.  Run the test psamples times, using the value
  * set for psamples when the test was created.
  */
 for(i=0;i<test[0]->psamples;i++){

   dtest->test(test,i);

 }

 /*
  *========================================================================
  * Then evaluate the final test p-values for each individual test
  * statistic computed during the one run of nkps trials.  The default
  * method for generating test pvalues is kstest, not kuiper, since
  * David Bauer's suggested fix (plus verification with rgb_kstest_test)
  * now positively demonstrates that it leads to the correct distribution
  * of p for sets of 100 uniform deviates.
  *========================================================================
  */
 for(j = 0;j < dtest->nkps;j++){
   if(ks_test >= 3){
     /*
      * This (Kuiper KS) can be selected with -k 3 from the command line.
      * Generally it is ignored.  All smaller values of ks_test are passed
      * through to kstest() and control its precision (and speed!).
      */
     test[j]->ks_pvalue = kstest_kuiper(test[j]->pvalues,test[j]->psamples);
   } else {
     /* This is (symmetrized Kolmogorov-Smirnov) is the default */
     test[j]->ks_pvalue = kstest(test[j]->pvalues,test[j]->psamples);
   }
 }

}

void ttd_test(Dtest *dtest, Test **test)
{

 uint i,j,k,need_more_p;
 double psofar;

 /*
  * This is the new "test to destruction" mode, which I'm going to write
  * before, and separate from, the "resolve ambiguity" test.  It has to be
  * a separate call to make it easy to wrap in an "execute_test" routine
  * in the CLI or GUI or R.  It basically adds psamples 100 at a time
  * until the final pvalue is less than the Xtrategy cutoff OR the Xoff
  * cutoff in number of psamples is reached.  It starts by doing at
  * least test[0]->psamples runs, though.
  */

 /*
  * run the first batch of samples.
  */
 for(i=0;i<test[0]->psamples;i++){
   dtest->test(test,i);
 }

 /*
  * Run their pvalues to see if we are done.
  */
 need_more_p = YES;
 for(j = 0;j < dtest->nkps;j++){
   if(ks_test >= 3){
     /*
      * This (Kuiper KS) can be selected with -k 3 from the command line.
      * Generally it is ignored.  All smaller values of ks_test are passed
      * through to kstest() and control its precision (and speed!).
      */
     test[j]->ks_pvalue = kstest_kuiper(test[j]->pvalues,test[j]->psamples);
     if(test[j]->ks_pvalue < Xtreme) need_more_p = NO;
   } else {
     /* This is (symmetrized Kolmogorov-Smirnov) is the default */
     test[j]->ks_pvalue = kstest(test[j]->pvalues,test[j]->psamples);
     if(test[j]->ks_pvalue < Xtreme) need_more_p = NO;
   }
 }

 /*
  * Now we add more p in blocks of 100 until we die or hit the cutoff.
  */
 while(need_more_p){
   /* Another block of 100 */
   for(k=0;k<100;k++){
     i++;
     dtest->test(test,i);
   }
   for(j = 0;j < dtest->nkps;j++){
     /* Don't forget to count them */
     test[j]->psamples += 100;
     /* Now BOTH find the ks_pvalue AND test it against the Xtreme cutoff */
     if(ks_test >= 3){
       /*
        * This (Kuiper KS) can be selected with -k 3 from the command line.
        * Generally it is ignored.  All smaller values of ks_test are passed
        * through to kstest() and control its precision (and speed!).
        */
       test[j]->ks_pvalue = kstest_kuiper(test[j]->pvalues,test[j]->psamples);
       if(test[j]->ks_pvalue < Xtreme) need_more_p = NO;
     } else {
       /* This is (symmetrized Kolmogorov-Smirnov) is the default */
       test[j]->ks_pvalue = kstest(test[j]->pvalues,test[j]->psamples);
       if(test[j]->ks_pvalue < Xtreme) need_more_p = NO;
     }
   }
   if(test[0]->psamples >= Xoff) need_more_p = NO;
 }

 /* When we fall through to here, we are done. */

}
   
/*
 * It looks like I'm going to need to break the test routines down a bit
 * to make them modular.  This routine JUST adds count samples to the test
 * and runs a new kstest on them, starting to add at the old
 * test[0]->psamples value.  It should only be called AFTER std_test
 * has been called one time.  It makes no decisions about quitting or
 * continuing -- that has to be handled at the UI level.  It does check
 * to make sure that we won't overrun Xoff, though, and truncates at Xoff
 * if we would.
 */
void add_2_test(Dtest *dtest, Test **test, int count)
{

 uint i,j,k,imax;


 /*
  * Will count carry us over Xoff?  If it will, stop at Xoff and
  * adjust count to match.  test[0]->psamples is the running total
  * of how many samples we have at the end of it all.
  */
 imax = test[0]->psamples + count;
 if(imax > Xoff) imax = Xoff;
 count = imax - test[0]->psamples;
 for(i = test[0]->psamples; i < imax; i++){
   dtest->test(test,i);
 }

 for(j = 0;j < dtest->nkps;j++){
   /*
    * Don't forget to count the new number of samples and use it in the
    * new KS test.
    */
   test[j]->psamples += count;

   if(ks_test >= 3){
     /*
      * This (Kuiper KS) can be selected with -k 3 from the command line.
      * Generally it is ignored.  All smaller values of ks_test are passed
      * through to kstest() and control its precision (and speed!).
      */
     test[j]->ks_pvalue = kstest_kuiper(test[j]->pvalues,test[j]->psamples);
   } else {
     /* This is (symmetrized Kolmogorov-Smirnov) is the default */
     test[j]->ks_pvalue = kstest(test[j]->pvalues,test[j]->psamples);
   }

 }

}
   
      
