/*
 *========================================================================
 * $Id: Vtest.c 223 2006-08-17 06:19:38Z rgb $
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *========================================================================
 */

/*
 *========================================================================
 * Vtest.c contains a set of routines for evaluating a p-value, the
 * probability that a given test result was obtained IF the underlying
 * random number generator was a "good" one (the null hypothesis), given a
 * vector x of observed results and a vector y of expected results.  It
 * determines the p-value using Pearson's chisq, which does not require
 * the independent input of the expected sigma for each "bin" (vector
 * position).
 *========================================================================
 */
#include <dieharder/libdieharder.h>

void Vtest_create(Vtest *vtest, unsigned int nvec)
{

 int i;
 MYDEBUG(D_VTEST){
   printf("#==================================================================\n");
   printf("# Vtest_create(): Creating test struct for %u nvec.\n",nvec);
 }
 vtest->x = (double *) malloc(sizeof(double)*nvec);       /* sample results */
 vtest->y = (double *) malloc(sizeof(double)*nvec);       /* expected sample results */
 /* zero or set everything */
 for(i=0;i<nvec;i++){
   vtest->x[i] = 0.0;
   vtest->y[i] = 0.0;
 }
 vtest->nvec = nvec;
 vtest->ndof = 0;  /* The user must enter this, or it will try to compute it */
 vtest->chisq = 0.0;
 vtest->pvalue = 0.0;
 MYDEBUG(D_VTEST){
   printf("# Vtest_create(): Done.\n");
 }


}

void Vtest_destroy(Vtest *vtest)
{

 free(vtest->x);
 free(vtest->y);

}

void Vtest_eval(Vtest *vtest)
{

 uint i,ndof;
 double delchisq,chisq;
 double x_tot,y_tot;


 /*
  * This routine evaluates chi-squared, where:
  * vtest->x is the trial vector
  * vtest->y is the exact vector
  * vtest->sigma is the vector of expected error for y
  *              (for the exact/true distribution)
  * vtest->nvec is the vector length(s).
  * vtest->ndof is the number of degrees of freedom (default nvec-1)
  *
  * x, y, sigma, nvec all must be filled in my the calling routine.
  * Be sure to override the default value of ndof if it is known to
  * the caller.
  *
  * We'll have to see how this handles sigma[i] = 0.0.  Probably badly
  * but smoothly.
  */
 /* verbose=1; */
 MYDEBUG(D_VTEST){
   printf("Evaluating chisq and pvalue for %d points\n",vtest->nvec);
 }

 chisq = 0.0;
 x_tot = 0.0;
 y_tot = 0.0;
 ndof = 0;
 MYDEBUG(D_VTEST){
   printf("# %7s   %3s      %3s %10s      %10s %10s %9s\n",
           "bit/bin","DoF","X","Y","sigma","del-chisq","chisq");
   printf("#==================================================================\n");
 }
 /*
  * If vtest->ndof is nonzero, we use it to compute chisq.  If not, we
  * try to estimate it based on a cutoff that is pretty arbitrary.  So
  * set it to what it should be, if you know.
  */
 for (i=0;i<vtest->nvec;i++) {
   if(vtest->x[i] > 1.0){
     x_tot += vtest->x[i];
     y_tot += vtest->y[i];
     delchisq = (vtest->x[i] - vtest->y[i])*(vtest->x[i] - vtest->y[i])/vtest->y[i];
     /*  Alternative way of evaluating chisq for binomial only.
     delchisq = (vtest->x[i] - vtest->y[i])/vtest->sigma[i];
     delchisq *= delchisq;
     */
     chisq += delchisq;
     MYDEBUG(D_VTEST){
       printf("# %5u\t%3u\t%12.4f\t%12.4f\t%8.4f\t%10.4f\n",
                  i,vtest->ndof,vtest->x[i],vtest->y[i],delchisq,chisq);
     }
     /* increment only if the data is substantial */
     if(vtest->ndof == 0) ndof++;
   }

 }
 if(vtest->ndof == 0) vtest->ndof = ndof-1;

 MYDEBUG(D_VTEST){
   printf("Total:  %10.4f  %10.4f\n",x_tot,y_tot);
   printf("#==================================================================\n");
   printf("Evaluated chisq = %f for %u degrees of freedom\n",chisq,vtest->ndof);
 }
 vtest->chisq = chisq;

 /*
  * Now evaluate the corresponding pvalue.  The only real question
  * is what is the correct number of degrees of freedom.  I'd argue we
  * did use a constraint when we set expected = binomial*nsamp, so we'll
  * go for ndof (count of nvec tallied) - 1.
  */
 vtest->pvalue = gsl_sf_gamma_inc_Q((double)(vtest->ndof)/2.0,chisq/2.0);
 /* printf("Evaluted pvalue = %6.4f in Vtest_eval() with %u ndof.\n",vtest->pvalue,vtest->ndof); */
 MYDEBUG(D_VTEST){
   printf("Evaluted pvalue = %6.4f in Vtest_eval().\n",vtest->pvalue);
 }
 /* verbose=0; */

}

