/*
 * $Id$
 *
 * See copyright in copyright.h and the accompanying file COPYING
 */

/*
 *========================================================================
 * This is the Diehard minimum distance (2d sphere) test, rewritten from 
 * the description in tests.txt on George Marsaglia's diehard site.
 *
 *               THE MINIMUM DISTANCE TEST                       ::
 * It does this 100 times::   choose n=8000 random points in a   ::
 * square of side 10000.  Find d, the minimum distance between   ::
 * the (n^2-n)/2 pairs of points.  If the points are truly inde- ::
 * pendent uniform, then d^2, the square of the minimum distance ::
 * should be (very close to) exponentially distributed with mean ::
 * .995 .  Thus 1-exp(-d^2/.995) should be uniform on [0,1) and  ::
 * a KSTEST on the resulting 100 values serves as a test of uni- ::
 * formity for random points in the square. Test numbers=0 mod 5 ::
 * are printed but the KSTEST is based on the full set of 100    ::
 * random choices of 8000 points in the 10000x10000 square.      ::
 *
 *                      Comment
 * Obviously the same test as 3d Spheres but in 2d, hence the
 * name.
 *========================================================================
 */


#include "dieharder.h"
/*
 * Test specific data
 */
#include "diehard_2dsphere.h"

double diehard_2dsphere()
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
 pks = sample((void *)diehard_2dsphere_test);

 /*
  * Test Results, standard form.
  */
 test_footer(dtest,pks,ks_pvalue,"Diehard Minimum Distance (2d Circle) Test");

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

void diehard_2dsphere_test()
{

 int i,j,k,l,m;
 C3 *c3;
 double r1,r2,r3,rmin,r2min;

 /*
  * This one should be pretty straightforward.  Generate a vector
  * of two random coordinates in the range 0-10000.  Do
  * a simple double loop through to float the smallest separation out.
  * Generate p, save in a sample vector.  Apply KS test.
  */
 c3 = (C3 *)malloc(POINTS*sizeof(C3));

 rmin = 2000.0;
 for(j=0;j<POINTS;j++){
   /*
    * Generate a new point in the 10000x10000 square.
    */
   for(k=0;k<DIM;k++) c3[j].x[k] = 10000.0*gsl_rng_uniform_pos(rng);
   if(verbose == D_DIEHARD_2DSPHERE || verbose == D_ALL){
     printf("%d: (%8.2f,%8.2f)\n",j,c3[j].x[0],c3[j].x[1]);
   }

   /*
    * Now compute the distance between the new point and all previously
    * picked points.
    */
   for(k=j-1;k>=0;k--){
     r2 = 0.0;
     for(l=0;l<DIM;l++){
       r2 += (c3[j].x[l]-c3[k].x[l])*(c3[j].x[l]-c3[k].x[l]);
     }
     r1 = sqrt(r2);
     if(r1<rmin) {
       rmin = r1;
       r2min = r2;
     }
   }
 }

 if(verbose == D_DIEHARD_2DSPHERE || verbose == D_ALL){
   printf("Found rmin = %f  (r^2 = %f)\n",rmin,r2min);
 }
 ks_pvalue[kspi] = 1.0 - exp(-r2min/0.995);
 if(verbose == D_DIEHARD_2DSPHERE || verbose == D_ALL){
   printf("p-value[%d] = %f\n",kspi,ks_pvalue[kspi]);
 }
 kspi++;

}

void help_diehard_2dsphere()
{

 printf("%s",dtest->description);

}
