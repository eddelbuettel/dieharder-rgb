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
#define POINTS 8000
#define DIM 2

typedef struct {
  double x[DIM];
} C3;
 
double diehard_2dsphere()
{

 double pks;
 
 if(!quiet){
   printf("#==================================================================\n");
   printf("#         Diehard \"Minimum Distance\" test (modified).\n");
   printf("# Generate 8000 points in a 10000^2 square.  Determine the\n");
   printf("# the shortest nearest neighbor distance R.  This should generate\n");
   printf("# p = 1.0 - exp(-R^2/0.995).  Repeat for lots of samples, apply a\n");
   printf("# KS test to see if p is uniform.\n");
   printf("#==================================================================\n");
   printf("# Random number generator tested: %s\n",gsl_rng_name(rng));
   printf("# Number of points tested = %u\n",POINTS);
 }

 kspi = 0;  /* Always zero first */
 pks = sample((void *)diehard_2dsphere_test);
 printf("p = %8.6f for diehard_2dsphere test from Kuiper Kolmogorov-Smirnov test\n",pks);
 if(pks < 0.0001){
   printf("Generator %s fails for diehard_2dsphere.\n",gsl_rng_name(rng));
 }

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

