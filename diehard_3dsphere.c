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
#define POINTS 4000
#define DIM 3

typedef struct {
  double x[DIM];
} C3;
 
double diehard_3dsphere()
{

 double pks;
 
 if(!quiet){
   printf("#==================================================================\n");
   printf("#              Diehard \"3d Sphere\" test (modified).\n");
   printf("# Generate 4000 points in a 1000^3 integer cube.  Determine the\n");
   printf("# the shortest nearest neighbor distance R.  This should generate\n");
   printf("# p = 1.0 - exp(-R^3/30).  Repeat for lots of samples, apply a\n");
   printf("# KS test to see if p is uniform.\n");
   printf("#==================================================================\n");
   printf("# Random number generator tested: %s\n",gsl_rng_name(rng));
   printf("# Number of points tested = %u\n",POINTS);
 }

 kspi = 0;  /* Always zero first */
 pks = sample((void *)diehard_3dsphere_test);
 printf("p = %8.6f for diehard_3dsphere test from Kuiper Kolmogorov-Smirnov test\n",pks);
 if(pks < 0.0001){
   printf("Generator %s fails for diehard_3dsphere.\n",gsl_rng_name(rng));
 }

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

