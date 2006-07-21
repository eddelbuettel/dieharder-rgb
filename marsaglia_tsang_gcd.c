/*
 * $Id: diehard_craps.c 191 2006-07-13 08:23:50Z rgb $
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *
 */

/*
 *========================================================================
 *                    Marsaglia and Tsang GCD Test
 * 
 * 10^7 tsamples (default) of uint rands u, v are generated and two
 * statistics are generated: their greatest common divisor (GCD) (w)
 * and the number of steps of Euclid's Method required to find it
 * (k).  Two tables of frequencies are thus generated -- one for the
 * number of times each value for k in the range 0 to 41 (with counts
 * greater than this range lumped in with the endpoints).
 * The other table is the frequency of occurrence of each GCD w.
 * k is be distributed approximately binomially, but this is useless for
 * the purposes of performing a stringent test.  Instead four "good"
 * RNGs (gfsr4,mt19937_1999,rndlxs2,taus2) were used to construct a
 * simulated table of high precision probabilities for k (a process that
 * obviously begs the question as to whether or not THESE generators
 * are "good" wrt the test.  At any rate, they produce very similar tables
 * and pass the test with each other's tables (and are otherwise very
 * different RNGs).  The table of probabilities for the gcd distribution is
 * generated dynamically per test (it is easy to compute).  Chisq tests
 * on both of these binned distributions yield two p-values per test,
 * and 100 (default) p-values of each are accumulated and subjected to
 * final KS tests and displayed in a histogram.
 *========================================================================
 */

#include "dieharder.h"
/*
 * Test specific data
 */
#include "marsaglia_tsang_gcd.h"

double marsaglia_tsang_gcd()
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
  * any missed prior allocations are freed.  THIS test generates TWO
  * statistics (like the craps test) so we need to allocate two
  * tables for p-values.
  */
 if(ks_pvalue) nullfree(ks_pvalue);
 ks_pvalue  = (double *)malloc((size_t) psamples*sizeof(double));
 if(ks_pvalue2) nullfree(ks_pvalue2);
 ks_pvalue2  = (double *)malloc((size_t) psamples*sizeof(double));

 /*
  * Reseed FILE random number generators once per individual test.
  * This correctly resets the rewind counter per test.  Also reset
  * the RNG if Seed was specified, as only that will give us a
  * standard test result independent of test order in an -all run.
  */
 if(strncmp("file_input",gsl_rng_name(rng),10) == 0){
   gsl_rng_set(rng,1);
 } else if(Seed){
   gsl_rng_set(rng,Seed);
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
 pks = sample((void *)marsaglia_tsang_gcd_test);

 /*
  * Test Results, standard form.
  */
 test_footer(dtest,pks,ks_pvalue,"Marsaglia and Tsang GCD Test");

 /*
  * the GCD test generates two statistics, not one, so we need an extra
  * KS test on the second vector of p-values.
  */
 pks = kstest_kuiper(ks_pvalue2,kspi);
 test_footer(dtest,pks,ks_pvalue2,"Marsaglia and Tsang GCD Test");

 /*
  * Put back tsamples
  */
 if(all == YES){
   tsamples = ts_save;
   psamples = ps_save;
 }

 if(ks_pvalue) nullfree(ks_pvalue);
 if(ks_pvalue2) nullfree(ks_pvalue2);

 return(pks);

}

/*
 * This determines the number of samples that go into building the kprob[]
 * table.
 */
#define KCNT 10000000000
#define KTBLSIZE 41

void marsaglia_tsang_gcd_test()
{

 unsigned long long int t,ktbl[KTBLSIZE];
 uint i,j,k,u,v,w;
 static uint *gcd = 0;
 static double gnorm = 6.0/(PI*PI);
 static uint gtblsize = 0;
 Btest btest_k,btest_u;

 /* Make data tables for one-time entry -- do not delete.
 uint nbin = 50;
 double pbin = 0.376;
 printf("double kprob[%u] = {\n",KTBLSIZE);
 printf(" %10.8f",gsl_ran_binomial_pdf(0,pbin,nbin));
 for(i=1;i<KTBLSIZE;i++){
   if(i%6 == 0) {
     if((i)%6 == 0) printf(", \n");
     printf(" %10.8f",gsl_ran_binomial_pdf(i,pbin,nbin));
   } else {
     printf(", %10.8f",gsl_ran_binomial_pdf(i,pbin,nbin));
   }
 }
 printf("};\n");
 exit(0);
 */

 /*
  * Zero both tables, set gtblsize so that the expectation of gcd[] > 10
  * (arbitrary cutoff).  We don't free this on exit, but then, we only
  * allocate it once so it should be OK.
  */
 if(gtblsize == 0) {
   gtblsize = sqrt((double)tsamples*gnorm/100.0);
   /* printf("gtblsize = %u\n",gtblsize); */
 }
 if(gcd == 0) gcd = (uint *)malloc(gtblsize*sizeof(uint));
 memset(gcd,0,gtblsize*sizeof(uint));
 memset(ktbl,0,KTBLSIZE*sizeof(unsigned long long int));


 Btest_create(&btest_k,KTBLSIZE);
 Btest_create(&btest_u,gtblsize);

 /* exit(0); */

 if(verbose == D_MARSAGLIA_TSANG_GCD || verbose == D_ALL){
   printf("# user_marsaglia_tsang_gcd(): Beginning gcd test\n");
 }

 /* for(t=0;t<KCNT;t++){ */
 for(t=0;t<tsamples;t++){
   /* Initialize counter for this sample */
   k = 0;
   /* Get nonzero u,v */
   do{
    u = gsl_rng_get(rng);
   } while(u == 0);
   do{
    v = gsl_rng_get(rng);
   } while(v == 0);

   do{
     w = u%v;
     u = v;
     v = w;
     k++;
   } while(v>0);

   /*
    * We just need tsamples*c/u^2 to be greater than about 10, the
    * cutoff built into Btest_eval()  For tsamples = 10^7, turns out that
    * gtblsize < sqrt((double)tsamples*gnorm/10.0) (about 780) should be just
    * about right.  We lump all counts larger than that into "the tail",
    * which MUST be included in the chisq targets down below.
    */
   if(u>=gtblsize) u = gtblsize-1;
   if(u<gtblsize) {
     gcd[u]++;
   }

   /*
    * lump the k's > KTBLSIZE only because that's what we did generating
    * the table...
    */
   k = (k>KTBLSIZE-1)?KTBLSIZE-1:k;
   ktbl[k]++;

 }

 /*
  * This is where I formulate my own probability table, using
  * a mix of the best RNGs I have available.  Of course this ultimately
  * begs many questions...
 printf("double kprob[KTBLSIZE] = {\n");
 for(i=0;i<KTBLSIZE;i++){
   printf(" %10.8f,",(double)ktbl[i]/KCNT);
   if((i+1)%6 == 0) printf("\n");
 }
 printf("};\n");

 return;
  */

 /*
  * Put tabular results into btest_k, normalizing by the number
  * of samples as usual.  Note kprob is preloaded table of
  * target probabilities generated in commented fragement above.
  */
 if(verbose == D_MARSAGLIA_TSANG_GCD || verbose == D_ALL){
   printf(" Binomial probability table for k distribution.\n");
   printf("  i\t  mean\n");
 }
 for(i=0;i<KTBLSIZE;i++){
   btest_k.x[i] = (double)ktbl[i];
   btest_k.y[i] = tsamples*kprob[i];
   btest_k.sigma[i] = 0.0;
   if(verbose == D_MARSAGLIA_TSANG_GCD || verbose == D_ALL){
     printf(" %2u\t%f\t%f\t%f\n",i,btest_k.x[i],btest_k.y[i],btest_k.x[i]-btest_k.y[i]);
   }
 }
 /*
  * We will probably turn this into a table, but it isn't that expensive in the
  * short run as is.
  */
 for(i=0;i<gtblsize;i++){
   if(i>1){
     btest_u.x[i] = (double)gcd[i];
     if(i == gtblsize-1){
       /* This should be close enough to convergence */
       for(j=i;j<100000;j++){
         btest_u.y[i] += tsamples*gnorm/(1.0*j*j);
       }
       /* printf(" %2u\t%f\t%f\t%f\n",i,btest_u.x[i],btest_u.y[i],btest_u.x[i]-btest_u.y[i]); */
     } else {
       btest_u.y[i] = tsamples*gnorm/(i*i);
     }
     btest_u.sigma[i] = 0.0;
   } else {
     btest_u.x[i] = 0.0;
     btest_u.y[i] = 0.0;
     btest_u.sigma[i] = 0.0;
   }
   if(verbose == D_MARSAGLIA_TSANG_GCD || verbose == D_ALL){
     printf(" %2u\t%f\t%f\t%f\n",i,btest_u.x[i],btest_u.y[i],btest_u.x[i]-btest_u.y[i]);
   }
 }

 /*
  * Evaluate test statistics for this run
  */

 Btest_eval(&btest_k);
 ks_pvalue[kspi] = btest_k.pvalue;

 Btest_eval(&btest_u);
 ks_pvalue2[kspi] = btest_u.pvalue;


 Btest_destroy(&btest_k);
 Btest_destroy(&btest_u);

 if(verbose == D_USER_TEMPLATE || verbose == D_ALL){
   printf("# user_marsaglia_tsang_gcd(): ks_pvalue_k[%u] = %10.5f  ks_pvalue_w[%u] = %10.5f\n",kspi,ks_pvalue[kspi],ks_pvalue2[kspi]);
 }

 kspi++;

}

void help_marsaglia_tsang_gcd()
{

  printf("%s",dtest->description);

}
