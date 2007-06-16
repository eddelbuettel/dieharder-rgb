/*
 * $Id: rgb_operm.c 252 2006-10-10 13:17:36Z rgb $
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *
 */

/*
 *========================================================================
 *========================================================================
 */

#include <dieharder/libdieharder.h>

 double fcentral(int pind,size_t *data,int len,int offset,int nkp);
 uint iperm(size_t *data,int len,int offset);

 int kcov;
 gsl_permutation **fperms,**operms,**lperms,**rperms;
 double **c;


uint iperm(size_t *data,int len,int offset)
{

 uint i,j,k,max;
 static size_t *dtmp = 0;
 uint pindex,uret,tmp;

 /*
  * Allocate space for the temporary copy only the first time
  */
 if(dtmp == 0){
   dtmp = (size_t *)malloc((len+1)*sizeof(size_t));
 }

 /*
  * Copy a chunk of length len from data+offset -> dtmp
  */
 memcpy((void *)dtmp,(void *) &data[offset],len*sizeof(size_t));
 if(verbose>1){
   printf("iperm: dtmp = ");
   for(i=0;i<len;i++){
     printf("%u ",dtmp[i]);
   }
 }

 pindex = 0;
 for(i=len-1;i>0;i--){
   max = dtmp[0];
   k = 0;
   for(j=1;j<=i;j++){
     if(max <= dtmp[j]){
       max = dtmp[j];
       k = j;
     }
   }
   pindex = (i+1)*pindex + k;
   tmp = dtmp[i];
   dtmp[i] = dtmp[k];
   dtmp[k] = tmp;
 }

 return(pindex);

}

void rgb_operm(Test **test,int irun)
{

}

void operm_work()
{

 int i,j,k,n,off1,off2;
 int nkperm,noperm,tnoperm;
 double fi,fj;

 /*
  * We first form the permutations of 0,1,...,k-1 in lexicographic order.
  * This order determines the "index" argument to f, e.g. f(0,ap) determines
  * if the permutation sent to it matches the one indexed by 0 or not and
  * returns the centered statistic I_\pi - 1/k! either way.  This index
  * is the index that matches the indices in the covariance matrix c[i][j].
  */
 if(verbose){
   printf("Running operm verbosely with k = %d.\n",kcov);
   printf("Forming basic permutations of integers from 0 to %d.\n",kcov-1);
   printf("fperms[] = \n");
 }
 nkperm = gsl_sf_fact(kcov);
 fperms = (gsl_permutation**) malloc(nkperm*sizeof(gsl_permutation*));
 for(i=0;i<nkperm;i++){
   fperms[i] = gsl_permutation_alloc(kcov);
   if(i == 0){
     gsl_permutation_init(fperms[i]);
   } else {
     gsl_permutation_memcpy(fperms[i],fperms[i-1]);
     gsl_permutation_next(fperms[i]);
   }
 }

 for(i=0;i<nkperm;i++){
   if(verbose){
     printf(" kperm = %d  i = %d: ",iperm(fperms[i]->data,kcov,0),i);
     gsl_permutation_fprintf(stdout,fperms[i]," %u");
     printf("\n");
   }
 }

 /*
  * Test fcentral().
  */
 if(verbose){
   printf("Testing fcentral(i,nkperm,kperm):\n");
 }
 for(i=0;i<nkperm;i++){
   for(j = 0;j<nkperm;j++){
     printf("  fcentral(%d,",i);
     gsl_permutation_fprintf(stdout,fperms[j],"%u ");
     printf(") = %12.6f\n",fcentral(i,fperms[j]->data,kcov,0,nkperm));
   }
 }

 /*
  * Create c (covariance matrix for operm test) and zero it.
  */
 if(verbose){
   printf("Creating and zeroing c[][]:\n");
 }
 c = (double **)malloc(nkperm*sizeof(double*));
 for(i=0;i<nkperm;i++){
   c[i] = (double *)malloc(kcov*sizeof(double));
   for(j = 0;j<nkperm;j++){
     c[i][j] = 0.0;   /* Eventually this will just zero c the hard way */
     if(verbose){
       printf(" %u",c[i][j]);
     }
   }
   if(verbose){
     printf("\n");
   }
 }

 /*
  * Now we make the complete set of overlapping permutations.  There
  * are 2*kcov - 1 elements, permuted.
  */
 if(verbose){
   printf("Forming overlapping permutations out to n = %d.\n",2*kcov-1);
 }
 n = 2*kcov - 1;
 noperm = gsl_sf_fact(n);
 operms = (gsl_permutation**) malloc(noperm*sizeof(gsl_permutation*));
 if(verbose){
   printf("Permuting integers from 0 to %d:\n",n-1);
 }
 for(i=0;i<noperm;i++){
   operms[i] = gsl_permutation_alloc(n);
   if(i == 0){
     gsl_permutation_init(operms[i]);
   } else {
     gsl_permutation_memcpy(operms[i],operms[i-1]);
     gsl_permutation_next(operms[i]);
   }
   if(verbose){
     gsl_permutation_fprintf(stdout,operms[i]," %u");
     printf("\n");
   }
 }

 /*
  * We now apply the rule very directly.
  * with each kcov-window onto the permutation from left to right.
  */
 for(i=0;i<nkperm;i++){
   for(j=0;j<nkperm;j++){
     /* Sum over permutations */
     for(k=0;k<noperm;k++){
       /* and sum over all overlapping offsets in the permutations */
       for(off1=0;off1<1;off1++){
         for(off2=1;off2<kcov;off2++){
           fi = fcentral(i,operms[k]->data,kcov,0,nkperm);
           fj = fcentral(j,operms[k]->data,kcov,off2,nkperm);
           c[i][j] += fi*fj;
	 }
       }
     }
   }
 }

 if(verbose){
   printf("noperm = %d\n",noperm);
   printf("covariance = \n");
 }
 for(i=0;i<nkperm;i++){
   for(j=0;j<nkperm;j++){
     c[i][j] /= noperm;
     printf("%10.6f  ",c[i][j]);
   }
   printf("\n");   
 }

  
}

double fcentral(int pind,size_t *data,int len,int offset,int nkp)
{

 int i;
 double fret;

 /*
  * compute the k-permutation index from iperm for the window
  * at data[offset] of length len.  If it matches pind, return
  * the first quantity, otherwise return the second.
  */
 if(iperm(data,len,offset) == pind){

   fret = (double) (nkp - 1.0)/nkp;
   if(verbose>1){
     printf(" f(%d,",pind);
     for(i=0;i<len;i++){
       printf("%u ",data[offset+i]);
     }
     printf(",%d,%d) = %10.6f\n",len,offset,fret);
   }
   return(fret);

 } else {

   fret = (double) (-1.0/nkp);
   if(verbose>1){
     printf(" f(%d,",pind);
     for(i=0;i<len;i++){
       printf("%u ",data[offset+i]);
     }
     printf(",%d,%d) = %10.6f\n",len,offset,fret);
   }
   return(fret);

 }


}

