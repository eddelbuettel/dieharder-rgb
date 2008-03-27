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
uint rgb_perm(size_t *data,int len);
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

 MYDEBUG(D_RGB_PERMUTATIONS){
   printf("#==================================================================\n");
   printf("# rgb_permutations: Debug with %u\n",D_RGB_PERMUTATIONS);
   printf("# rgb_permutations: doing k = %u\n",rgb_permutations_k);
 }

 nperms = gsl_sf_fact(rgb_permutations_k);

 MYDEBUG(D_RGB_PERMUTATIONS){
   printf("# rgb_permutations: Checking frequency of %u permutations\n",nperms);
 }

 /*
  * This creates a permutation MATRIX of 0,1,2,...,rgb_permutation_k
  */
 p = gsl_permutation_alloc(rgb_permutations_k);
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
 for(t=0;t<test[0]->tsamples;t++){
   /*
    * To sort into a perm, test vector needs to be double.
    */
   for(i=0;i<rgb_permutations_k;i++) {
     testv[i] = (double) gsl_rng_get(rng);
     MYDEBUG(D_RGB_PERMUTATIONS){
       printf("# rgb_permutations: testv[%u] = %u\n",i,testv[i]);
     }
   }
   gsl_sort_index(ps,testv,1,rgb_permutations_k);
   MYDEBUG(D_RGB_PERMUTATIONS){
     for(i=0;i<rgb_permutations_k;i++) {
       printf("# rgb_permutations: ps[%u] = %u\n",i,ps[i]);
     }
   }
   permindex = rgb_perm(ps,rgb_permutations_k);
   MYDEBUG(D_RGB_PERMUTATIONS){
     printf("# rgb_permutations: permindex = %u\n",permindex);
   }
   vtest.x[permindex]++;

 }

 MYDEBUG(D_RGB_PERMUTATIONS){
   printf("# rgb_permutations:==============================\n");
   printf("# rgb_permutations: permutation count = \n");
   for(i=0;i<nperms;i++){
     printf("# count[%u] = %u\n",i,vtest.x[i]);
   }
 }

 Vtest_eval(&vtest);
 test[0]->pvalues[irun] = vtest.pvalue;

 MYDEBUG(D_RGB_PERMUTATIONS) {
   printf("# rgb_permutations(): test[0]->pvalues[%u] = %10.5f\n",irun,test[0]->pvalues[irun]);
 }

}

uint rgb_perm(size_t *data,int len)
{

 uint i,j,k,max,min;
 uint pindex,uret,tmp;
 static gsl_permutation** lookup = 0;

 /*
  * Allocate space for lookup table and fill it.
  */
 if(lookup == 0){
   lookup = (gsl_permutation**) malloc(nperms*sizeof(gsl_permutation*));
   MYDEBUG(D_RGB_OPERM){
     printf("# rgb_operm: Allocating piperm lookup table of perms.\n");
   }
   for(i=0;i<nperms;i++){
        lookup[i] = gsl_permutation_alloc(rgb_permutations_k);
   }
   for(i=0;i<nperms;i++){
     if(i == 0){
       gsl_permutation_init(lookup[i]);
     } else {
       gsl_permutation_memcpy(lookup[i],lookup[i-1]);
       gsl_permutation_next(lookup[i]);
     }
   }

   /*
    * This method yields a mirror symmetry in the permutations top to
    * bottom.
   for(i=0;i<nperms/2;i++){
     if(i == 0){
       gsl_permutation_init(lookup[i]);
       for(j=0;j<rgb_permutations_k;j++){
         lookup[nperms-i-1]->data[rgb_permutations_k-j-1] = lookup[i]->data[j];
       }
     } else {
       gsl_permutation_memcpy(lookup[i],lookup[i-1]);
       gsl_permutation_next(lookup[i]);
       for(j=0;j<rgb_permutations_k;j++){
         lookup[nperms-i-1]->data[rgb_permutations_k-j-1] = lookup[i]->data[j];
       }
     }
   }
   */
   MYDEBUG(D_RGB_OPERM){
     for(i=0;i<nperms;i++){
       printf("# rgb_operm: %u => ",i);
       gsl_permutation_fprintf(stdout,lookup[i]," %u");
       printf("\n");
     }
   }

 }

 for(i=0;i<nperms;i++){
   if(memcmp(data,lookup[i]->data,len*sizeof(uint))==0){
     /* Not cruft, but off:
     MYDEBUG(D_RGB_OPERM){
       printf("# piperm(): ");
       gsl_permutation_fprintf(stdout,lookup[i]," %u");
       printf(" = %u\n",i);
     }
     */
     return(i);
   }
 }
 printf("We'd better not get here...\n");

}

