/*
 * $Id: rgb_operm.c 252 2006-10-10 13:17:36Z rgb $
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *
 */

/*
 *========================================================================
 * This is the revised Overlapping Permutations test.  It directly
 * simulates the covariance matrix of overlapping permutations.  The way
 * this works below (tentatively) is:
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

/*
 * Some globals that will eventually go in the test include where they
 * arguably belong.
 */
double fcentral(int pind,size_t *data,int len,int offset,int nkp);
uint iperm(size_t *data,int len,int offset);
void make_ctarget();
void make_cexpt();
double **ctarget,**cexpt;


void rgb_operm(Test **test,int irun)
{

 int i,j,n,nb,noperms,nperms;
 uint csamples;   /* rgb_operm_k^2 is vector size of cov matrix */
 uint *count,ctotal; /* counters */
 uint size;
 double pvalue,ntuple_prob,pbin;  /* probabilities */
 Vtest *vtest;   /* Chisq entry vector */

 /*
  * For a given n = ntuple size in bits, there are n! bit orderings
  */
 MYDEBUG(D_RGB_OPERM){
   printf("#==================================================================\n");
   printf("# rgb_operm: Running rgb_operm verbosely for k = %d.\n",rgb_operm_k);
   printf("# rgb_operm: Use -v = %d to focus.\n",D_RGB_OPERM);
   printf("# rgb_operm: ======================================================\n");
 }
 nperms = gsl_sf_fact(rgb_operm_k);
 csamples = rgb_operm_k*rgb_operm_k;
 n = 2*rgb_operm_k - 1;
 noperms = gsl_sf_fact(n);
 MYDEBUG(D_RGB_OPERM){
   printf("# rgb_operm: Forming overlapping permutations of %d integers.\n",rgb_operm_k);
 }
 make_ctarget();


 if(rgb_operm_k>0){
   nb = rgb_operm_k;
   MYDEBUG(D_RGB_OPERM){
     printf("# rgb_operm: Testing nb = %u (CRUFT)\n",nb);
   }
 } else {
   printf("Error:  rgb_operm_k must be a positive integer.  Exiting.\n");
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

 make_cexpt();

 exit(0);

}

void make_ctarget()
{

 int i,j,k,n,off1,off2;
 int nperms,noperms,tnoperms;
 double fi,fj;
 gsl_permutation **fperms,**operms,**lperms,**rperms;

 /*
  * We first form the permutations of 0,1,...,k-1 in lexicographic order.
  * This order determines the "index" argument to f, e.g. f(0,ap) determines
  * if the permutation sent to it matches the one indexed by 0 or not and
  * returns the centered statistic I_\pi - 1/k! either way.  This index
  * is the index that matches the indices in the covariance matrix c[i][j].
  */
 MYDEBUG(D_RGB_OPERM){
   printf("#==================================================================\n");
   printf("# rgb_operm: Running ctarget()\n");
   printf("# rgb_operm: Running operm verbosely for rgb_operm_k = %d.\n",rgb_operm_k);
   printf("# rgb_operm: Forming basic permutations of %d integers.\n",rgb_operm_k);
   printf("# rgb_operm: fperms[] = \n");
 }
 nperms = gsl_sf_fact(rgb_operm_k);
 fperms = (gsl_permutation**) malloc(nperms*sizeof(gsl_permutation*));
 for(i=0;i<nperms;i++){
   fperms[i] = gsl_permutation_alloc(rgb_operm_k);
   if(i == 0){
     gsl_permutation_init(fperms[i]);
   } else {
     gsl_permutation_memcpy(fperms[i],fperms[i-1]);
     gsl_permutation_next(fperms[i]);
   }
 }

 for(i=0;i<nperms;i++){
   MYDEBUG(D_RGB_OPERM){
     printf("# rgb_operm:  kperm = %d  i = %d: ",iperm(fperms[i]->data,rgb_operm_k,0),i);
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
       printf(") = %12.6f\n",fcentral(i,fperms[j]->data,rgb_operm_k,0,nperms));
     }
   }

   /*
    * Create c (covariance matrix for operm test) and zero it.
    */
   printf("# rgb_operm: Creating and zeroing c[][]:\n");

 }

 ctarget = (double **)malloc(nperms*sizeof(double*));
 for(i=0;i<nperms;i++){
   ctarget[i] = (double *)malloc(rgb_operm_k*sizeof(double));
   for(j = 0;j<nperms;j++){
     ctarget[i][j] = 0.0;
   }
 }

 /*
  * Now we make the complete set of overlapping permutations.  There
  * are 2*rgb_operm_ntuple - 1 elements, permuted.
  */
 MYDEBUG(D_RGB_OPERM){
   printf("# rgb_operm: Forming overlapping permutations out to n = %d.\n",2*rgb_operm_k-1);
 }
 n = 2*rgb_operm_k - 1;
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
         for(off2=1;off2<rgb_operm_k;off2++){
           fi = fcentral(i,operms[k]->data,rgb_operm_k,0,nperms);
           fj = fcentral(j,operms[k]->data,rgb_operm_k,off2,nperms);
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

void make_cexpt()
{

 int i,j,k,n,off1,off2;
 int nperms,noperms,tnoperms;
 double fi,fj;
 uint testv[19];  /* easier than malloc etc, but beware length */
 uint *testw;     /* the sliding test window pointer */

 gsl_permutation **fperms,**operms,**lperms,**rperms;

 /*
  * This routine should more or less recapitluate make_ctarget() EXCEPT
  * that instead of computing the values of c, they are simulated from
  * a stream of data X_tsamples = {x_1, x_2, ... x_{tsamples+rgb_operm_k}}.
  * For the moment these integers are pulled from the rng as 32 bit uints
  * (which makes the test the symmetric inverse of testing floats generated
  * from the uints by division, and much faster).
  *
  * We then start at offset i=1 and pull a set s_i of rgb_operm_k numbers
  * from the prefilled (for speed) vector X, e.g. s_i = x_i,x_i+1 for
  * rgb_operm_k = 2.
  *
  * We compute the permutation number of this set: p_i = permno(s_i).  This
  * corresponds to the index of a counter, which we increment, accumulating
  * the relative frequency of that permutation in X.  We then increment i,
  * form s_i+1 = x_i+1,x_i+2 (NOTE WELL the overlap!) form p_i+1, increment
  * its counter, and loop over all tsamples s_i contained in the vector.
  *
  * If we divide the counters by tsamples, the resulting vector is the
  * measured relative frequency of the occurence of each permutation.  This
  * number can be used in a manner identical to the computed probability to
  * form the experimental permutation matrix C.  The difference between the
  * computed and observed permutation matrices is a vector that should be
  * distributed according to the chisq distribution with rgb_operm_k! - 1
  * degrees of freedom, or something like that.  This part I've got to
  * work out by trial and error once I generate the experimental relative
  * frequency (measured probability) of the permutations.
  *
  * Note that to convert the integers into a "permutation" we have to
  * map them into a set of 0 to rgb_operm_k-1 integers that indicate
  * the rank order of the x_i.  For example, for rgb_operm_k=2 and the
  * following string of 8-bit numbers:
  *  X = {17 242 99 112 48 47 200 85}
  *
  *   s_1 = {17 242} -> {0 1} -> p_1 = 1
  *   s_2 = {242 17} -> {1 0} 
  *     17 242 99 -> 0 2 1 = p_0 p_1 p_2
  *     112 48 47 -> 2 1 0 = p_0 p_1 p_2
  *     200 85 142 -> 2 0 1 = p_0 p_1 p_2
  *     127 78 78 -> 2 0 1 OR 2 1 0 with a toggle (this will be rare
  *   as the number of bits increases...
  *
  * We then form f_i(y_s), f_j(y_{s+1}),... f_k(y_{s+n-1}) for all
  * i,j,k... in (0...n-1).  Finally,
  *    c[i][j] = 1/n_samp \sum_n_samp { f_i(y_s) f_j(y_{s+1}) +
  *      f_i(y_s) f_j(y_{s+2}) + ...f_i(y_s) f_j(y_{s+n-1}) }
  *
  * This SHOULD turn out to be (in the limit) the same as c_target[i][j],
  * and we can at least hope that their difference is a nice \chisq
  * target.
  */
 MYDEBUG(D_RGB_OPERM){
   printf("#==================================================================\n");
   printf("# rgb_operm: Entering make_cexpt()\n");
   printf("# rgb_operm: Size of sliding window = %d.\n",rgb_operm_k);
   printf("# rgb_operm: Forming basic permutations of %d integers.\n",rgb_operm_k-1);
   printf("# rgb_operm: fperms[] = \n");
 }
 nperms = gsl_sf_fact(rgb_operm_k);
 fperms = (gsl_permutation**) malloc(nperms*sizeof(gsl_permutation*));
 for(i=0;i<nperms;i++){
   fperms[i] = gsl_permutation_alloc(rgb_operm_k);
   if(i == 0){
     gsl_permutation_init(fperms[i]);
   } else {
     gsl_permutation_memcpy(fperms[i],fperms[i-1]);
     gsl_permutation_next(fperms[i]);
   }
 }

 for(i=0;i<nperms;i++){
   MYDEBUG(D_RGB_OPERM){
     printf("# rgb_operm:  kperm = %d  i = %d: ",iperm(fperms[i]->data,rgb_operm_k,0),i);
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
       printf(") = %12.6f\n",fcentral(i,fperms[j]->data,rgb_operm_k,0,nperms));
     }
   }

   /*
    * Create c (covariance matrix for operm test) and zero it.
    */
   printf("# rgb_operm: Creating and zeroing c[][]:\n");

 }

 cexpt = (double **)malloc(nperms*sizeof(double*));
 for(i=0;i<nperms;i++){
   cexpt[i] = (double *)malloc(rgb_operm_k*sizeof(double));
   for(j = 0;j<nperms;j++){
     ctarget[i][j] = 0.0;
   }
 }

 /*
  * All of the above is pretty much the same for both routines.  Here,
  * though, we have to pull the 2*rgb_operm_ntuple - 1 random uints (for
  * example) and transform them into a permutation.  The question is,
  * how do we do that?
  */
 MYDEBUG(D_RGB_OPERM){
   printf("# rgb_operm: Forming overlapping permutations out to n = %d.\n",2*rgb_operm_k-1);
 }
 n = 2*rgb_operm_k - 1;
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
  * We now evaluate this by sampling.  In a nutshell, this involves
  *   a) Filling a vector 2*rgb_operm_k - 1 rands.
  *   b) Filling a vector of length rgb_operm_k from a sliding window on
  *      the first vector (can be done with a pointer instead of actual
  *      data movement).
  *   c) Passing this vector/pointer to a routine that transforms it into
  *      the order indices in a permutation object, then returns its
  *      permutation index.  I'm guessing/hoping that there is
  *      a clever recursion that does this efficiently, or perhaps a
  *      canned subroutine call from the GSL.
  *   d) Cumulate C_expt(pi,pi') of the permutation number from these
  *      samples
  *   e) tsamples times
  *   f) and then divide by tsamples or whatever to make its normalization
  *      match that of C_theoretical.
  *
  * We'll figure out what to do then, to evaluate a suitable chisq p-value.
  */
 for(t=0;t<tsamples;t++){
   for(k=0;k<2*rgb_operm_k;k++) testv[k] = gsl_rng_get(rng);
   testw = testv;
   i = permind(rgb_operm_k,testw);
   for(k=1;k<rgb_operm_k;k++){
     j = permind(rgb_operm_k,testw+k);
IAMHERE IAMHERE   
 
 }

 for(i=0;i<nperms;i++){
   for(j=0;j<nperms;j++){
     /* Sum over permutations */
     for(k=0;k<noperms;k++){
       /* and sum over all overlapping offsets in the permutations */
       for(off1=0;off1<1;off1++){
         for(off2=1;off2<rgb_operm_k;off2++){
           fi = fcentral(i,operms[k]->data,rgb_operm_k,0,nperms);
           fj = fcentral(j,operms[k]->data,rgb_operm_k,off2,nperms);
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

