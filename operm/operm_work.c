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
gsl_permutation **fperms,**perms,*lperm,*rperm;

void operm_work()
{

 int i,j;
 double **c;

 /*
  * This is operm2, basically, at the moment.
  */
 k = 2;

 n = 2*k-1;
 nperms = gsl_sf_fact(n);
 perms = (gsl_permutation**) malloc(nperms*sizeof(gsl_permutation*));
 for(i=0;i<nperms;i++){
   perms[i] = gsl_permutation_alloc(n);
   if(i == 0){
     gsl_permutation_init(perms[i]);
   } else {
     gsl_permutation_memcpy(perms[i],perms[i-1]);
     gsl_permutation_next(perms[i]);
   }
   gsl_permutation_fprintf(stdout,perms[i],"%u ");
   printf("\n");
 }

 /*
  * We also need to form the permutations of 0 1 and 1 2 separately.
  * They together form the "index" of f, e.g. f_0 selects 0 1 and f_1
  * selects 1 0 (etc) in lexicographical order.  Note that we're
  * shifting to C indices relative to the nilpotent markov chains paper.
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



