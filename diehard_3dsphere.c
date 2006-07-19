/*
 * $Id$
 *
 * See copyright in copyright.h and the accompanying file COPYING
 */

/*
 *========================================================================
 * This is the Diehard 3d spheres test, rewritten from the description
 * in tests.txt on George Marsaglia's diehard site.
 *
 * Basically, we choose 4000 points in a cube of side 1000.  Compute the
 * smallest nearest neighbor distance (radius R of the smallest sphere
 * that doesn't overlap any neighboring point). R^3 is exponentially
 * distributed with an empirical exponential distribution with mean 30.
 * Thus p = 1.0 - exp(-R^3/30.0) should be a uniform distribution.  Run
 * a KS test on a vector of independent samples of this entire test to
 * find out.
 *========================================================================
 */


#include "dieharder.h"
/*
 * Test specific data
 */
#include "diehard_3dsphere.h"

double diehard_3dsphere()
{


 double pks;
 uint ps_save,ts_save;

 /*
  * Do a standard test if -a(ll) is selected.
  * Note that this test ignores tsamples, so we always save it
  * and restore it at the end.
  */
 ts_save = tsamples;
 tsamples = dtest->tsamples_std;
 if(all == YES){
   ps_save = psamples;
   psamples = dtest->psamples_std;
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
 pks = sample((void *)diehard_3dsphere_test);

 /*
  * Test Results, standard form.
  */
 test_footer(dtest,pks,ks_pvalue,"Diehard 3d Sphere (Minimum Distance) Test");

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

void diehard_3dsphere_test()
{

 int i,j,k,l,m;
 C3 *c3;
 double r1,r2,r3,rmin,r3min;
 double xdelta,ydelta,zdelta;

 /*
  * This one should be pretty straightforward.  Generate a vector
  * of three random coordinates in the range 0-1000 (check the
  * diehard code to see what "in" a 1000^3 cube means, but I'm assuming
  * real number coordinates greater than 0 and less than 1000).  Do
  * a simple double loop through to float the smallest separation out.
  * Generate p, save in a sample vector.  Apply KS test.
  */
 c3 = (C3 *)malloc(POINTS*sizeof(C3));

 rmin = 2000.0;
 for(j=0;j<POINTS;j++){
   /*
    * Generate a new point in the cube.
    */
   for(k=0;k<DIM;k++) c3[j].x[k] = 1000.0*gsl_rng_uniform_pos(rng);
   if(verbose == D_DIEHARD_3DSPHERE || verbose == D_ALL){
     printf("%d: (%8.2f,%8.2f,%8.2f)\n",j,c3[j].x[0],c3[j].x[1],c3[j].x[2]);
   }

   /*
    * Now compute the distance between the new point and all previously
    * picked points.
    */
   for(k=j-1;k>=0;k--){
     xdelta = c3[j].x[0]-c3[k].x[0];
     ydelta = c3[j].x[1]-c3[k].x[1];
     zdelta = c3[j].x[2]-c3[k].x[2];
     r2 = xdelta*xdelta + ydelta*ydelta + zdelta*zdelta;
     r1 = sqrt(r2);
     r3 = r2*r1;
     if(verbose == D_DIEHARD_3DSPHERE || verbose == D_ALL){
       printf("%d-%d: |(%6.2f,%6.2f,%6.2f)| = r1 = %f rmin = %f, \n",
          j,k,xdelta,ydelta,zdelta,r1,rmin);
     }
     if(r1<rmin) {
       rmin = r1;
       r3min = r3;
     }
   }
 }

 if(verbose == D_DIEHARD_3DSPHERE || verbose == D_ALL){
   printf("Found rmin = %f  (r^3 = %f)\n",rmin,r3min);
 }
 ks_pvalue[kspi] = 1.0 - exp(-r3min/30.0);
 if(verbose == D_DIEHARD_3DSPHERE || verbose == D_ALL){
   printf("p-value[%d] = %f\n",kspi,ks_pvalue[kspi]);
 }
 kspi++;

}

void help_diehard_3dsphere()
{

 printf("%s",dtest->description);

}
