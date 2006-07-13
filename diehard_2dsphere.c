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
#define POINTS 8000
#define DIM 2

typedef struct {
  double x[DIM];
} C3;
 
double diehard_2dsphere()
{

 double pks;
 
 /*
  * Allocate space for ks_pvalue.  Free it below
  */
 ks_pvalue = (double *)malloc((size_t) psamples*sizeof(double));

 if(!quiet){
   help_diehard_2dsphere();
   printf("#                        Run Details\n");
   if(strncmp("file_input",gsl_rng_name(rng),10) == 0){
     printf("# Random number generator tested: %s\n",gsl_rng_name(rng));
     printf("# File %s contains %u rands of %c type.\n",filename,filecount,filetype);
   } else {
     printf("# Random number generator tested: %s\n",gsl_rng_name(rng));
   }
   printf("# Samples per test FIXED at %u.\n",POINTS);
   printf("# Test run %u times to cumulate p-values for KS test.\n",psamples);
 }

 kspi = 0;  /* Always zero first */
 pks = sample((void *)diehard_2dsphere_test);

 /*
  * Display histogram of ks p-values (optional)
  */
 if(hist_flag){
   histogram(ks_pvalue,psamples,0.0,1.0,10,"p-values");
 }
 if(!quiet){
   if(strncmp("file_input",gsl_rng_name(rng),10) == 0){
     printf("# %u rands were used in this test\n",file_input_get_rtot(rng));
     printf("# The file %s was rewound %u times\n",gsl_rng_name(rng),file_input_get_rewind_cnt(rng));
   }
 }
 printf("#                          Results\n");
 printf("# p = %8.6f for diehard_2dsphere test from\n",pks);
 printf("#     Kuiper Kolmogorov-Smirnov test on %u pvalues.\n",kspi);
 /* Work through some ranges here */
 if(pks < 0.0001){
   printf("# Generator %s FAILED at < 0.01%% for diehard_2dsphere.\n",gsl_rng_name(rng));
 } else if(pks < 0.01){
   printf("# Generator %s POOR at < 1%% for diehard_2dsphere.\n",gsl_rng_name(rng));
   printf("# Recommendation:  Repeat test to verify failure.\n");
 } else if(pks < 0.05){
   printf("# Generator %s POSSIBLY WEAK at < 5%% for diehard_2dsphere.\n",gsl_rng_name(rng));
   printf("# Recommendation:  Repeat test to verify failure.\n");
 } else {
   printf("# Generator %s PASSED at > 5%% for diehard_2dsphere.\n",gsl_rng_name(rng));
 }
 printf("#==================================================================\n");

 /*
  * free ks_pvalue.
  */
 free(ks_pvalue);

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

 printf("#==================================================================\n");
 printf("#         Diehard \"Minimum Distance\" test (modified).\n");
 printf("# Generate 8000 points in a 10000^2 square.  Determine the\n");
 printf("# the shortest nearest neighbor distance R.  This should generate\n");
 printf("# p = 1.0 - exp(-R^2/0.995).  Repeat for lots of samples, apply a\n");
 printf("# KS test to see if p is uniform.\n");
 printf("#==================================================================\n");

}
