/*
 *========================================================================
 * $Id: OPERM_work.c 14 2001-09-12 16:09:15Z rgb $
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *========================================================================
 */

/*
 *========================================================================
 *  Project work template.
 *========================================================================
 */

#include "operm.h"

double f(int ip,gsl_permutation *ap);

int k,kperms,n,nperms;
gsl_permutation **fperms,**operms,**lperms,**rperms;

void operm_work()
{

 int i,j,l,m;
 double **c;

 /*
  * This is operm2, basically, at the moment.
  */
 k = 2;

 /*
  * We first form the permutations of 0,1,...,k-1 in lexicographic order.
  * This order determines the "index" argument to f, e.g. f(0,ap) determines
  * if the permutation sent to it matches the one indexed by 0 or not and
  * returns the centered statistic I_\pi - 1/k! either way.  This index
  * is the index that matches the indices in the covariance matrix c[i][j].
  */
 kperms = gsl_sf_fact(k);
 fperms = (gsl_permutation**) malloc(kperms*sizeof(gsl_permutation*));
 for(i=0;i<kperms;i++){
   fperms[i] = gsl_permutation_alloc(k);
   if(i == 0){
     gsl_permutation_init(fperms[i]);
   } else {
     gsl_permutation_memcpy(fperms[i],fperms[i-1]);
     gsl_permutation_next(fperms[i]);
   }
   gsl_permutation_fprintf(stdout,fperms[i],"%u ");
   printf("\n");
 }


 /*
  * Create c (covariance matrix for operm test) and test
  * f() -- end by zeroing c so we can build it correctly.
  */
 printf("Matrix of f values: \n");
 c = (double **)malloc(k*sizeof(double*));
 for(i=0;i<k;i++){
  c[i] = (double *)malloc(k*sizeof(double));
  for(j = 0;j<k;j++){
     c[i][j] = f(i,fperms[j]);
     printf("f[%d][%d] = %8.4f\n",i,j,c[i][j]);
     c[i][j] = 0.0;   /* Eventually this will just zero c the hard way */
  }
 }

 /*
  * OK, here's how it works.  We have an ordered list of arguments
  * to f in fperms[], so fperms[i] corresponds to a specific ordered
  * permutation of k objects.  We now have to process the nperms
  * permutations into "left" permutations and "right" permutations.
  * Really we may have to do "operms" (which works even better;-) and
  * do it for the various overlaps:
  *  k = 2
  *    operm
  *    0 1 2
  *     / \
  *  0 1   1 2
  * lperm rperm
  *
  *    operm
  *    0 2 1
  *     / \
  *  0 1   2 1
  * lperm rperm
  *
  * ... (note we preserve order of 0 1 on left, 1 2 on right).
  * These would get summed over permutations into:
  *   c[i][j] = (1/nperm) * sum_operm(f(i,lperm)*f(j,rperm))
  * (where yeah, this has to be turned into real code)
  */

 /*
  * First we make the overlapping permutations.  This part isn't
  * quite right because when we get to k = 3 we can overlap:
  *   0 1 2
  *     1 2 3
  *       2 3 4
  * so we must loop over this, once we figure out the algorithm for
  * k = 2 (which has a unique overlap).
  */
 n = 2*k-1;
 nperms = gsl_sf_fact(n);
 operms = (gsl_permutation**) malloc(nperms*sizeof(gsl_permutation*));
 for(i=0;i<nperms;i++){
   operms[i] = gsl_permutation_alloc(n);
   if(i == 0){
     gsl_permutation_init(operms[i]);
   } else {
     gsl_permutation_memcpy(operms[i],operms[i-1]);
     gsl_permutation_next(operms[i]);
   }
   gsl_permutation_fprintf(stdout,operms[i],"%u ");
   printf("\n");
 }

 /*
  * Now sequentially sort them into left and right sides.
  */
 lperms = (gsl_permutation**) malloc(nperms*sizeof(gsl_permutation*));
 rperms = (gsl_permutation**) malloc(nperms*sizeof(gsl_permutation*));
 for(i=0;i<nperms;i++){
   lperms[i] = gsl_permutation_alloc(k);
   rperms[i] = gsl_permutation_alloc(k);
   j = 0;
   l = 0;
   m = 0;
   while(j<n){
     if(operms[i]->data[j] < k) lperms[i]->data[l++] = operms[i]->data[j];
     if(operms[i]->data[j] > 0) rperms[i]->data[m++] = operms[i]->data[j] - 1;
     j++;
   }
   printf("lperms[%d] = ",i);
   gsl_permutation_fprintf(stdout,lperms[i],"%u ");
   printf("rperms[%d] = ",i);
   gsl_permutation_fprintf(stdout,rperms[i],"%u ");
   printf("\n");
 }

 printf("covariance = \n");
 for(i=0;i<k;i++){
   for(j=0;j<k;j++){
     for(l=0;l<nperms;l++){
       /* 
       printf("f(%d,%d) = %f\n",i,l,f(i,lperms[l]));
       printf("f(%d,%d) = %f\n",j,l,f(j,lperms[l]));
       */
       c[i][j] += f(i,lperms[l])*f(j,rperms[l]);
     }
     c[i][j] /= nperms;
     printf("%8.4f  ",c[i][j]);
   }
   printf("\n");
 }
  
}

double f(int ip,gsl_permutation *ap)
{

 double fret;
 /* printf("Executing f\n"); */
 if(memcmp(fperms[ip]->data,ap->data,k*sizeof(size_t)) == 0){
   /* printf("Permutations match: f = ");
   gsl_permutation_fprintf(stdout,fperms[ip],"%u ");
   printf(" = a = ");
   gsl_permutation_fprintf(stdout,ap,"%u ");
   printf("\n"); */
   fret = (double) (kperms-1.0)/kperms;
 } else {
   /* printf("Permutations don't match: f = ");
   gsl_permutation_fprintf(stdout,fperms[ip],"%u ");
   printf(" = a = ");
   gsl_permutation_fprintf(stdout,ap,"%u ");
   printf("\n"); */
   fret = (double) (-1.0)/kperms;
 }

 return(fret);

}



