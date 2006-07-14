/*
 * $Id$
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *
 */

/*
 *========================================================================
 * This is the Diehard BINARY RANK 6x8 test, rewritten from the 
 * description in tests.txt on George Marsaglia's diehard site.
 *
 * This is the BINARY RANK TEST for 6x8 matrices.  From each of  ::
 * six random 32-bit integers from the generator under test, a   ::
 * specified byte is chosen, and the resulting six bytes form a  ::
 * 6x8 binary matrix whose rank is determined.  That rank can be ::
 * from 0 to 6, but ranks 0,1,2,3 are rare; their counts are     ::
 * pooled with those for rank 4. Ranks are found for 100,000     ::
 * random matrices, and a chi-square test is performed on        ::
 * counts for ranks 6,5 and <=4.                                 ::
 *
 *                          Comments
 *===================================================================
 */


#include "dieharder.h"
/*
 * Test specific data
 */
#include "diehard_rank_6x8.h"


double diehard_rank_6x8()
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
 if(ks_pvalue) nullfree(ks_pvalue);
 ks_pvalue  = (double *)malloc((size_t) psamples*sizeof(double));
 if(rand_mtx) {
   fprintf(stderr,"dieharder_rand_6x8(): rand_mtx not NULL.  Cannot happen.\n");
   exit(0);
 }
 rand_mtx = (uint **)malloc(6*sizeof(uint *));
 for(i=0;i<6;i++) rand_mtx[i] = (uint *)malloc(8*sizeof(uint));

 test_header(dtest);

 /*
  * This is the standard test call.
  */
 kspi = 0;  /* Always zero first */
 pks = sample((void *)diehard_rank_6x8_test);

 /*
  * Test Results, standard form.
  */
 test_footer(dtest,pks,ks_pvalue,"Diehard 6x8 Binary Rank Test");

 /*
  * Put back tsamples
  */
 if(all == YES){
   tsamples = ts_save;
   psamples = ps_save;
 }

 if(ks_pvalue) nullfree(ks_pvalue);
 if(rand_mtx){
   for(i=0;i<6;i++) nullfree(rand_mtx[i]);
   nullfree(rand_mtx);
 }

 return(pks);

}

void diehard_rank_6x8_test()
{

 int i,j,k,t,rank,offset;
 double r,smax,s;
 
 uint bitstring,rmask,mask;
 Btest btest;

 Btest_create(&btest,7,"diehard_rank_6x8",gsl_rng_name(rng));
 for(i=0;i<2;i++){
   btest.x[0] = 0.0;
   btest.y[0] = 0.0;
   btest.sigma[0] = 0.0;
 }
 btest.x[2] = 0.0;
 btest.y[2] = tsamples*0.149858E-06;
 btest.sigma[2] = 0.0;
 btest.x[3] = 0.0;
 btest.y[3] = tsamples*0.808926E-04;
 btest.sigma[3] = 0.0;
 btest.x[4] = 0.0;
 btest.y[4] = tsamples*0.936197E-02;
 btest.sigma[4] = 0.0;
 btest.x[5] = 0.0;
 btest.y[5] = tsamples*0.217439E+00;
 btest.sigma[5] = 0.0;
 btest.x[6] = 0.0;
 btest.y[6] = tsamples*0.773118E+00;
 btest.sigma[6] = 0.0;
 
 for(t=0;t<tsamples;t++){

   /*
    * We generate 6 random rmax_bits-bit integers and put a
    * randomly chosen byte into the LEFTMOST byte position
    * of the row/slot of rand_mtx.
    */
   if(verbose == D_DIEHARD_RANK_6x8 || verbose == D_ALL){
     printf("# diehard_rank_6x8(): Input random matrix = \n");
   }
   for(i=0;i<6;i++){
     if(verbose == D_DIEHARD_RANK_6x8 || verbose == D_ALL){
       printf("# ");
     }
     /*
      * random offset from 0 to rmax_bits-1
      */
     offset = gsl_rng_uniform_int(rng,rmax_bits);
     /*
      * Get a random integer
      */
     bitstring = gsl_rng_get(rng);
     /*
      * Get one byte starting at the random offset
      */
     rand_mtx[i][0] = get_bit_ntuple(&bitstring,1,8,offset);
     /*
      * Shift it left until it is lined up in the leftmost
      * byte.
     rand_mtx[i][0] <<= 24;
      */
     if(verbose == D_DIEHARD_RANK_6x8 || verbose == D_ALL){
       dumpbits(rand_mtx[i],32);
     }
   }

   rank = binary_rank(rand_mtx,6,8);
   if(verbose == D_DIEHARD_RANK_6x8 || verbose == D_ALL){
     printf("binary rank = %d\n",rank);
   }

   if(rank <= 2){
     btest.x[2]++;
   } else {
     btest.x[rank]++;
   }
 }

 /* for(i=0;i<33;i++) printf("btest.x[%d] =  %f\n",i,btest.x[i]); */

 Btest_eval(&btest);
 ks_pvalue[kspi] = btest.pvalue;
 if(verbose == D_DIEHARD_RANK_6x8 || verbose == D_ALL){
   printf("# diehard_rank_6x8(): ks_pvalue[%u] = %10.5f\n",kspi,ks_pvalue[kspi]);
 }
 kspi++;

 Btest_destroy(&btest);

}

void help_diehard_rank_6x8()
{

 printf("%s",dtest->description);

}


