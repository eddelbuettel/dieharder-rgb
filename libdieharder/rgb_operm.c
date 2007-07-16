/*
 * $Id: rgb_operm.c 252 2006-10-10 13:17:36Z rgb $
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *
 */

/*
 *========================================================================
 * This is the revised Overlapping Permutations test.  It directly
 * simulates the covariance matrix of overlapping permutations.  What
 * this means is:
 *
 *    For a bit ntuple of length N, slide a window of length N to the
 *    right one bit at a time.  Compute the permutation index of the
 *    original ntuple, the permutation index of the window ntuple, and
 *    accumulate the covariance matrix of the two positions.  This
 *    can be directly and precisely computed as well.  The simulated
 *    result should be distributed according to the chisq distribution,
 *    so we subtract the two and feed it into the chisq program as a
 *    vector to compute p.
 *
 * This MAY NOT BE RIGHT.  I'm working from both Marsaglia's limited
 * documentation (in a program that doesn't do ANYTHING like what the
 * documentation says it does) and from Nilpotent Markov Processes.
 * But I confess to not quite understand how to actually perform the
 * test in the latter -- it is very good at describing the construction
 * of the target matrix, not so good at describing how to transform
 * this into a chisq and p.
 *
 * FWIW, as I get something that actually works here, I'm going to
 * THOROUGHLY document it in the book that will accompany the test.
 *========================================================================
 */

#include <dieharder/libdieharder.h>

 double fcentral(int pind,size_t *data,int len,int offset,int nkp);
 uint iperm(size_t *data,int len,int offset);
 void make_ctarget();

 gsl_permutation **fperms,**operms,**lperms,**rperms;
 double **ctarget,**cexpt;


void rgb_operm(Test **test,int irun)
{

 int i,j,n,nb,noperms,nperms;
 uint csamples;   /* rgb_operm_ntuple^2 is vector size of cov matrix */
 uint *count,ctotal; /* counters */
 uint size;
 double pvalue,ntuple_prob,pbin;  /* probabilities */
 Vtest *vtest;   /* Chisq entry vector */

 /*
  * For a given n = ntuple size in bits, there are n! bit orderings
  */
 MYDEBUG(D_RGB_OPERM){
   printf("# rgb_operm: Running rgb_operm verbosely for ntuple = %d.\n",rgb_operm_ntuple);
   printf("# rgb_operm: Forming basic permutations of integers from 0 to %d.\n",rgb_operm_ntuple-1);
   printf("# rgb_operm: fperms[] = \n");
 }

 nperms = gsl_sf_fact(rgb_operm_ntuple);
 csamples = rgb_operm_ntuple*rgb_operm_ntuple;
 n = 2*rgb_operm_ntuple - 1;
 noperms = gsl_sf_fact(n);
 make_ctarget();

 /*
  * OK, this works AFAICT, although I need controls to be sanely
  * mapped and everything.  We're just going to see if we can
  * make it work for rgb_operm_ntuple = 2 (2 bit overlapping patterns) first,
  * THEN go on to more.  Note that IMO this test is completely,
  * utterly, totally redundant with the n=3 bitstream test, because
  * if the distribution of 000, 001, 010, 011, 100, 101, 110, 111 is
  * correct (occupation 1/8 each) then I absolutely positively
  * guarantee that the covariance matrix of of 2 bit patterns is
  * correct...
  *
  * Anyway, let's see what we can do.  Now we have to SIMULATE the
  * SAME covariance matrix.  That is, we have to generate a bit
  * pattern of length 2*rgb_operm_ntuple-1 and add up the frequencies of the
  * different sliding-window permutations, then divide by the number
  * of samples.
  */


 /*
  * Sample a bitstring of rgb_bitstring_ntuple in length (exactly).
  */
 if(rgb_operm_ntuple>0){
   nb = rgb_operm_ntuple;
   MYDEBUG(D_RGB_OPERM){
     printf("# rgb_bitdist: Testing ntuple = %u\n",nb);
   }
 } else {
   printf("Error:  rgb_bitdist_ntuple must be a positive integer.  Exiting.\n");
   exit(0);
 }

 /*
  * Allocate memory for value_max vector of Vtest structs and counts,
  * PER TEST.  Note that we must free both of these when we are done
  * or leak.
  */
 vtest = (Vtest *)malloc(csamples*sizeof(Vtest));
 count = (uint *)malloc(csamples*sizeof(uint));

 Vtest_create(&vtest[i],csamples+1,"rgb_operm",gsl_rng_name(rng));

 exit(0);

}

void make_ctarget()
{

 int i,j,k,n,off1,off2;
 int nperms,noperms,tnoperms;
 double fi,fj;

 /*
  * We first form the permutations of 0,1,...,k-1 in lexicographic order.
  * This order determines the "index" argument to f, e.g. f(0,ap) determines
  * if the permutation sent to it matches the one indexed by 0 or not and
  * returns the centered statistic I_\pi - 1/k! either way.  This index
  * is the index that matches the indices in the covariance matrix c[i][j].
  */
 MYDEBUG(D_RGB_OPERM){
   printf("# rgb_operm: Running operm verbosely for ntuple = %d.\n",rgb_operm_ntuple);
   printf("# rgb_operm: Forming basic permutations of integers from 0 to %d.\n",rgb_operm_ntuple-1);
   printf("# rgb_operm: fperms[] = \n");
 }
 nperms = gsl_sf_fact(rgb_operm_ntuple);
 fperms = (gsl_permutation**) malloc(nperms*sizeof(gsl_permutation*));
 for(i=0;i<nperms;i++){
   fperms[i] = gsl_permutation_alloc(rgb_operm_ntuple);
   if(i == 0){
     gsl_permutation_init(fperms[i]);
   } else {
     gsl_permutation_memcpy(fperms[i],fperms[i-1]);
     gsl_permutation_next(fperms[i]);
   }
 }

 for(i=0;i<nperms;i++){
   MYDEBUG(D_RGB_OPERM){
     printf("# rgb_operm:  kperm = %d  i = %d: ",iperm(fperms[i]->data,rgb_operm_ntuple,0),i);
     gsl_permutation_fprintf(stdout,fperms[i]," %u");
     printf("\n");
   }
 }

 /*
  * Test fcentral().
  */
 MYDEBUG(D_RGB_OPERM){
   printf("# rgb_operm: Testing fcentral(i,nperms,kperm):\n");
   for(i=0;i<nperms;i++){
     for(j = 0;j<nperms;j++){
       printf("# rgb_operm:   fcentral(%d,",i);
       gsl_permutation_fprintf(stdout,fperms[j],"%u ");
       printf(") = %12.6f\n",fcentral(i,fperms[j]->data,rgb_operm_ntuple,0,nperms));
     }
   }

   /*
    * Create c (covariance matrix for operm test) and zero it.
    */
   printf("# rgb_operm: Creating and zeroing c[][]:\n");

 }

 ctarget = (double **)malloc(nperms*sizeof(double*));
 for(i=0;i<nperms;i++){
   ctarget[i] = (double *)malloc(rgb_operm_ntuple*sizeof(double));
   for(j = 0;j<nperms;j++){
     ctarget[i][j] = 0.0;
   }
 }

 /*
  * Now we make the complete set of overlapping permutations.  There
  * are 2*rgb_operm_ntuple - 1 elements, permuted.
  */
 MYDEBUG(D_RGB_OPERM){
   printf("# rgb_operm: Forming overlapping permutations out to n = %d.\n",2*rgb_operm_ntuple-1);
 }
 n = 2*rgb_operm_ntuple - 1;
 noperms = gsl_sf_fact(n);
 operms = (gsl_permutation**) malloc(noperms*sizeof(gsl_permutation*));
 MYDEBUG(D_RGB_OPERM){
   printf("# rgb_operm: Permuting integers from 0 to %d:\n",n-1);
 }
 for(i=0;i<noperms;i++){
   operms[i] = gsl_permutation_alloc(n);
   MYDEBUG(D_RGB_OPERM){
     printf("# rgb_operm: ");
   }
   if(i == 0){
     gsl_permutation_init(operms[i]);
   } else {
     gsl_permutation_memcpy(operms[i],operms[i-1]);
     gsl_permutation_next(operms[i]);
   }
   MYDEBUG(D_RGB_OPERM){
     gsl_permutation_fprintf(stdout,operms[i]," %u");
     printf("\n");
   }
 }

 /*
  * We now apply the rule very directly.
  * with each rgb_operm_ntuple-window onto the permutation from left to right.
  */
 for(i=0;i<nperms;i++){
   for(j=0;j<nperms;j++){
     /* Sum over permutations */
     for(k=0;k<noperms;k++){
       /* and sum over all overlapping offsets in the permutations */
       for(off1=0;off1<1;off1++){
         for(off2=1;off2<rgb_operm_ntuple;off2++){
           fi = fcentral(i,operms[k]->data,rgb_operm_ntuple,0,nperms);
           fj = fcentral(j,operms[k]->data,rgb_operm_ntuple,off2,nperms);
           ctarget[i][j] += fi*fj;
	 }
       }
     }
   }
 }

 MYDEBUG(D_RGB_OPERM){
   printf("# rgb_operm: number of permutations = %d\n",noperms);
   printf("# rgb_operm: target (exact) covariance matrix = \n");
 }
 for(i=0;i<nperms;i++){
   MYDEBUG(D_RGB_OPERM){
     printf("# rgb_operm: ");
   }
   for(j=0;j<nperms;j++){
     ctarget[i][j] /= noperms;
     MYDEBUG(D_RGB_OPERM){
       printf("%10.6f  ",ctarget[i][j]);
     }
   }
   MYDEBUG(D_RGB_OPERM){
     printf("\n");
   }
 }

  
}

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
 MYDEBUG(D_RGB_OPERM){
   printf("# rgb_operm: iperm(): dtmp = ");
   for(i=0;i<len;i++){
     printf("%u ",dtmp[i]);
   }
   printf("\n");
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

