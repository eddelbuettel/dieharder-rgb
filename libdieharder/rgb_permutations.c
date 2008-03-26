/*
 * See copyright in copyright.h and the accompanying file COPYING
 */

/*
 *========================================================================
 * This just counts the permutations of n samples.  They should
 * occur n! times each.  We count them and do a straight chisq.
 *========================================================================
 */

#include <dieharder/libdieharder.h>
#define RGB_PERM_KMAX 10
uint nperms;
uint piperm(size_t *data,int len);
double fpipi(int pi1,int pi2,int nkp);

void rgb_permutations(Test **test,int irun)
{

 uint i,j,permindex,t;
 uint *count;
 gsl_permutation *p;
 Vtest vtest;

 /*
  * This is the test vector.
  */
 double testv[RGB_PERM_KMAX*2];  /* easier than malloc etc, but beware length */
 int ps[4096];

 nperms = gsl_sf_fact(rgb_permutations_k);

 MYDEBUG(D_RGB_OPERM){
   printf("#==================================================================\n");
   printf("# rgb_permutations: Checking frequency of %u permutations\n",nperms);
 }

 p = gsl_permutation_alloc(nperms);

 Vtest_create(&vtest,nperms,"rgb_permutations",gsl_rng_name(rng));
 for(i=0;i<nperms;i++){
   vtest.x[i] = 0.0;
   vtest.y[i] = (double) test[0]->tsamples/nperms;
 }

 /*
  * We count the order permutations in a long string of samples of
  * rgb_permutation_k non-overlapping rands.  This is done by:
  *   a) Filling testv[] with rgb_permutation_k rands.
  *   b) Using gsl_sort_index to generate the permutation index.
  *   c) Incrementing a counter for that index (a-c done tsamples times)
  *   d) Doing a straight chisq on the counter vector with nperms-1 DOF
  *
  * This test should be done with tsamples > 30*nperms, easily met for
  * reasonable rgb_permutation_k
  */
 for(t=0;t<tsamples;t++){
   /*
    * To sort into a perm, test vector needs to be double.
    */
   for(i=0;i<rgb_permutations_k;i++) {
     testv[i] = (double) gsl_rng_get(rng);
   }
   gsl_sort_index(ps,testv,1,rgb_operm_k);
   permindex = piperm(ps,rgb_permutations_k);
   vtest.x[permindex]++;

 }

 MYDEBUG(D_RGB_OPERM){
   printf("# rgb_permutations:==============================\n");
   printf("# rgb_permutations: permutation count = \n");
 }
 for(i=0;i<nperms;i++){
   MYDEBUG(D_RGB_OPERM){
     printf("# count[%u] = %u\n",i,count[i]);
   }
 }

 Vtest_eval(&vtest);
 test[0]->pvalues[irun] = vtest.pvalue;

 MYDEBUG(D_RGB_PERMUTATIONS) {
   printf("# rgb_permutations(): test[0]->pvalues[%u] = %10.5f\n",irun,test[0]->pvalues[irun]);
 }

}






