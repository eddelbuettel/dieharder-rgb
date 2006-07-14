/*
* $Id$
*
* See copyright in copyright.h and the accompanying file COPYING
*
*/

/*
 *========================================================================
 * This is the Diehard BINARY RANK 31x31 test, rewritten from the 
 * description in tests.txt on George Marsaglia's diehard site.
 *
 * This is the BINARY RANK TEST for 31x31 matrices. The leftmost ::
 * 31 bits of 31 random integers from the test sequence are used ::
 * to form a 31x31 binary matrix over the field {0,1}. The rank  ::
 * is determined. That rank can be from 0 to 31, but ranks< 28   ::
 * are rare, and their counts are pooled with those for rank 28. ::
 * Ranks are found for 40,000 such random matrices and a chisqua-::
 * re test is performed on counts for ranks 31,30,29 and <=28.   ::
 *
 *                          Comments
 *========================================================================
 */


#include "dieharder.h"
/*
 * Test specific data
 */
#include "diehard_rank_32x32.h"


double diehard_rank_32x32()
{

 int i;
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
 if(ks_pvalue) free(ks_pvalue);
 ks_pvalue  = (double *)malloc((size_t) psamples*sizeof(double));
 /*
  * Note that this assumes that 8*sizeof(uint) >= 32!
  */
 if(rand_mtx) {
   for(i=0;i<32;i++){
     if(rand_mtx[i]) free(rand_mtx[i]);
   }
   free(rand_mtx);
 }
 rand_mtx = (uint **)malloc(32*sizeof(uint*));
 for(i=0;i<32;i++)rand_mtx[i] = (uint *)malloc(sizeof(uint));


 test_header(dtest);
 /*
  * Any custom test header output lines go here.  They should be
  * used VERY sparingly.
  */

 /*
  * This is now the standard test call.
  */
 kspi = 0;  /* Always zero first */
 pks = sample((void *)diehard_rank_32x32_test);

 /*
  * Test Results, standard form.
  */
 test_footer(dtest,pks,ks_pvalue,"Diehard 32x32 Rank Test");

 /*
  * Put back tsamples
  */
 if(all == YES){
   tsamples = ts_save;
   psamples = ps_save;
 }

 free(ks_pvalue);
 if(rand_mtx) {
   for(i=0;i<32;i++){
     if(rand_mtx[i]) free(rand_mtx[i]);
   }
   free(rand_mtx);
 }


 return(pks);

}

void diehard_rank_32x32_test()
{

 int i,j,k,t,rank;
 double r,smax,s;
 uint bitstring,rmask,mask;
 Btest btest;

 Btest_create(&btest,33,"diehard_rank_32x32",gsl_rng_name(rng));
 for(i=0;i<29;i++){
   btest.x[0] = 0.0;
   btest.y[0] = 0.0;
   btest.sigma[0] = 0.0;
 }
 btest.x[29] = 0.0;
 btest.y[29] = tsamples*0.0052854502e+00;
 btest.sigma[29] = 0.0;
 btest.x[30] = 0.0;
 btest.y[30] = tsamples*0.1283502644e+00;
 btest.sigma[30] = 0.0;
 btest.x[31] = 0.0;
 btest.y[31] = tsamples*0.5775761902e+00;
 btest.sigma[31] = 0.0;
 btest.x[32] = 0.0;
 btest.y[32] = tsamples*0.2887880952e+00;
 btest.sigma[32] = 0.0;
 
 for(t=0;t<tsamples;t++){

   /*
    * We generate 32 x 32 random integers and put their leftmost
    * (most significant) bit into each bit slot of the rand_mtx.
    */
   if(verbose == D_DIEHARD_RANK_32x32 || verbose == D_ALL){
     printf("# diehard_rank_32x32(): Input random matrix = \n");
   }
   for(i=0;i<32;i++){
     if(verbose == D_DIEHARD_RANK_32x32 || verbose == D_ALL){
       printf("# ");
     }
     rand_mtx[i][0] = 0;
     for(j=0;j<32;j++){
       bitstring = gsl_rng_get(rng);
       bitstring = get_bit_ntuple(&bitstring,1,1,31);
       rand_mtx[i][0] <<= 1;
       rand_mtx[i][0] += bitstring;
       mask <<= 1;
     }
     if(verbose == D_DIEHARD_RANK_32x32 || verbose == D_ALL){
       dumpbits(rand_mtx[i],32);
     }
   }

   rank = binary_rank(rand_mtx,32,32);
   if(verbose == D_DIEHARD_RANK_32x32 || verbose == D_ALL){
     printf("binary rank = %d\n",rank);
   }

   if(rank <= 29){
     btest.x[29]++;
   } else {
     btest.x[rank]++;
   }
 }

 /* for(i=0;i<33;i++) printf("btest.x[%d] =  %f\n",i,btest.x[i]); */

 Btest_eval(&btest);
 ks_pvalue[kspi] = btest.pvalue;
 if(verbose == D_DIEHARD_RANK_32x32 || verbose == D_ALL){
   printf("# diehard_rank_32x32(): ks_pvalue[%u] = %10.5f\n",kspi,ks_pvalue[kspi]);
 }
 kspi++;

 Btest_destroy(&btest);

}

void help_diehard_rank_32x32()
{

 printf("%s",dtest->description);

}


