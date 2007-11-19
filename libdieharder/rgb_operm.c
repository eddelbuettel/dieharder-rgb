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
#define RGB_OPERM_KMAX 10

/*
 * Some globals that will eventually go in the test include where they
 * arguably belong.
 */
double fpipi(int pi1,int pi2,int nkp);
uint piperm(size_t *data,int len);
void make_cexact();
void make_cexpt();
int nperms,noperms;
double **cexact,**ceinv,**cexpt,**idty;
double *cvexact,*cvein,*cvexpt,*vidty;

void rgb_operm(Test **test,int irun)
{

 int i,j,n,nb,iv,s;
 uint csamples;   /* rgb_operm_k^2 is vector size of cov matrix */
 uint *count,ctotal; /* counters */
 uint size;
 double pvalue,ntuple_prob,pbin;  /* probabilities */
 Vtest *vtest;   /* Chisq entry vector */

 gsl_matrix_view CEXACT,CEINV,CEXPT,IDTY;

 /*
  * For a given n = ntuple size in bits, there are n! bit orderings
  */
 MYDEBUG(D_RGB_OPERM){
   printf("#==================================================================\n");
   printf("# rgb_operm: Running rgb_operm verbosely for k = %d.\n",rgb_operm_k);
   printf("# rgb_operm: Use -v = %d to focus.\n",D_RGB_OPERM);
   printf("# rgb_operm: ======================================================\n");
 }

 /*
  * Sanity check first
  */
 if((rgb_operm_k < 0) || (rgb_operm_k > RGB_OPERM_KMAX)){
   printf("\nError:  rgb_operm_k must be a positive integer <= %u.  Exiting.\n",RGB_OPERM_KMAX);
   exit(0);
 }

 nperms = gsl_sf_fact(rgb_operm_k);
 noperms = gsl_sf_fact(3*rgb_operm_k-2);
 csamples = rgb_operm_k*rgb_operm_k;
 gsl_permutation * p = gsl_permutation_alloc(nperms);

 /*
  * Allocate memory for value_max vector of Vtest structs and counts,
  * PER TEST.  Note that we must free both of these when we are done
  * or leak.
  */
 vtest = (Vtest *)malloc(csamples*sizeof(Vtest));
 count = (uint *)malloc(csamples*sizeof(uint));
 Vtest_create(&vtest[i],csamples+1,"rgb_operm",gsl_rng_name(rng));

 /*
  * We have to allocate and free the cexact and cexpt matrices here
  * or they'll be forgotten when these routines return.
  */
 MYDEBUG(D_RGB_OPERM){
   printf("# rgb_operm: Creating and zeroing cexact[][] and cexpt[][].\n");
 }
 cexact = (double **)malloc(nperms*sizeof(double*));
 ceinv  = (double **)malloc(nperms*sizeof(double*));
 cexpt  = (double **)malloc(nperms*sizeof(double*));
 idty   = (double **)malloc(nperms*sizeof(double*));
 cvexact = (double *)malloc(nperms*nperms*sizeof(double));
 cvein   = (double *)malloc(nperms*nperms*sizeof(double));
 cvexpt  = (double *)malloc(nperms*nperms*sizeof(double));
 vidty   = (double *)malloc(nperms*nperms*sizeof(double));
 for(i=0;i<nperms;i++){
   /* Here we pack addresses to map the matrix addressing onto the vector */
   cexact[i] = &cvexact[i*nperms];
   ceinv[i] = &cvein[i*nperms];
   cexpt[i] = &cvexpt[i*nperms];
   idty[i] = &vidty[i*nperms];
   for(j = 0;j<nperms;j++){
     cexact[i][j] = 0.0;
     ceinv[i][j] = 0.0;
     cexpt[i][j]  = 0.0;
     idty[i][j]   = 0.0;
   }
 }

 make_cexact();
 make_cexpt();

 iv=0;
 for(i=0;i<nperms;i++){
   for(j=0;j<nperms;j++){
     cvexact[iv] = cexact[i][j];
     cvexpt[iv]  = cexpt[i][j];
     vidty[iv]   = 0.0;
   }
 }

 CEXACT = gsl_matrix_view_array(cvexact, nperms, nperms);
 CEINV  = gsl_matrix_view_array(cvein  , nperms, nperms);
 CEXPT  = gsl_matrix_view_array(cvexpt , nperms, nperms);
 IDTY   = gsl_matrix_view_array(vidty  , nperms, nperms);

 /*
  * Hmmm, looks like cexact isn't invertible.  Duh.  So it has eigenvalues.
  * This seems to be important (how, I do not know) so let's find out.
  * Here is the gsl ritual for evaluating eigenvalues etc.
  */

 gsl_vector *eval = gsl_vector_alloc (nperms);
 gsl_matrix *evec = gsl_matrix_alloc (nperms,nperms);
 /*
 gsl_eigen_nonsymm_workspace* w =  gsl_eigen_nonsymmv_alloc(nperms);
 gsl_eigen_nonsymm_params (1,0,w);
 gsl_eigen_nonsymmv(&CEXACT.matrix, eval, evec, w);
 gsl_eigen_nonsymmv_free (w);
 */
 gsl_eigen_symmv_workspace* w =  gsl_eigen_symmv_alloc(nperms);
 gsl_eigen_symmv(&CEXACT.matrix, eval, evec, w);
 gsl_eigen_symmv_free (w);
 gsl_eigen_symmv_sort (eval, evec, GSL_EIGEN_SORT_ABS_ASC);

 {
   int i;

   printf("#==================================================================\n");
   for (i = 0; i < nperms; i++) {
     double eval_i = gsl_vector_get (eval, i);
     gsl_vector_view evec_i = gsl_matrix_column (evec, i);
     printf ("eigenvalue[%u] = %g\n", i, eval_i);
     printf ("eigenvector[%u] = \n",i);
     gsl_vector_fprintf (stdout,&evec_i.vector, "%10.5f");
   }
   printf("#==================================================================\n");
 }

 gsl_vector_free (eval);
 gsl_matrix_free (evec);

/*
 gsl_linalg_LU_decomp(&CEXACT.matrix, p, &s);
 gsl_linalg_LU_invert(&CEXACT, p, &CEINV);
 gsl_permutation_free(p);
 gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, &CEINV.matrix, &CEXPT.matrix, 0.0, &IDTY.matrix);
 printf("#==================================================================\n");
 printf("# Should be inverse of C, assuming it is invertible:\n");
 for(i=0;i<nperms;i++){
   printf("# ");
   for(j = 0;j<nperms;j++){
     printf("%8.3f ",idty[i][j]);
   }
   printf("\n");
 }
 printf("#==================================================================\n");
 printf("#==================================================================\n");
 printf("# Should be normal on identity:\n");
 for(i=0;i<nperms;i++){
   printf("# ");
   for(j = 0;j<nperms;j++){
     printf("%8.3f ",idty[i][j]);
   }
   printf("\n");
 }
 printf("#==================================================================\n");
 */

    

 /*
  * OK, at this point we have two matrices:  cexact[][] is filled with
  * the exact covariance matrix expected for the overlapping permutations.
  * cexpt[][] has been filled numerically by generating strings of random
  * uints or floats, generating sort index permutations, and
  * using them to IDENTICALLY generate an "experimental" version of c[][].
  * The two should correspond, in the limit of large tsamples.  IF I
  * understand Alhakim, Kawczak and Molchanov, then the way to implement
  * the simplest possible chisq test is to evaluate:
  *       cexact^-1 cexpt \approx I
  * where the diagonal terms should form a vector that is chisq distributed?
  * Let's try this...
  */
 
 

 /*
  * Free cexact[][] and cexpt[][]
  * Fix this when we're done so we don't leak; for now to much trouble.
 for(i=0;i<nperms;i++){
   free(cexact[i]);
   free(cexpt[i]);
 }
 free(cexact);
 free(cexpt);
  */
 

 exit(0);

}

void make_cexact()
{

 int i,j,k,ip,t;
 double fi,fj;
 /*
  * This is the test vector.
  */
 double testv[RGB_OPERM_KMAX*2];  /* easier than malloc etc, but beware length */
 /*
  * pi[] is the permutation index of a sample.  ps[] holds the
  * actual sample.
  */
 int pi[RGB_OPERM_KMAX*2],ps[RGB_OPERM_KMAX*2];
 /*
  * We seem to have made a mistake of sorts.  We actually have to sum
  * BOTH the forward AND the backward directions.  That means that the
  * permutation vector has to be of length 3k-1, with the pi=1 term
  * corresponding to the middle.  So for k=2, instead of 0,1,2 we need
  * 0 1 2 3 4 and we'll have to do 23, 34 in the leading direction and
  * 21, 10 in the trailing direction.
  */
 gsl_permutation **operms;

 MYDEBUG(D_RGB_OPERM){
   printf("#==================================================================\n");
   printf("# rgb_operm: Running cexact()\n");
 }

 /*
  * Test fpipi().  This is probably cruft, actually.
 MYDEBUG(D_RGB_OPERM){
   printf("# rgb_operm: Testing fpipi()\n");
   for(i=0;i<nperms;i++){
     for(j = 0;j<nperms;j++){
       printf("# rgb_operm: fpipi(%u,%u,%u) = %f\n",i,j,nperms,fpipi(i,j,nperms));
     }
   }
 }
 */

 MYDEBUG(D_RGB_OPERM){
   printf("#==================================================================\n");
   printf("# rgb_operm: Forming set of %u overlapping permutations\n",noperms);
   printf("# rgb_operm: Permutations\n");
   printf("# rgb_operm:==============================\n");
 }
 operms = (gsl_permutation**) malloc(noperms*sizeof(gsl_permutation*));
 for(i=0;i<noperms;i++){
   operms[i] = gsl_permutation_alloc(3*rgb_operm_k - 2);
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
  * We now form c_exact PRECISELY the same way that we do c_expt[][]
  * below, except that instead of pulling random samples of integers
  * or floats and averaging over the permutations thus represented,
  * we iterate over the complete set of equally weighted permutations
  * to get an exact answer.  Note that we have to center on 2k-1 and
  * go both forwards and backwards.
  */
 for(t=0;t<noperms;t++){
   /*
    * To sort into a perm, test vector needs to be double.
    */
   for(k=0;k<3*rgb_operm_k - 2;k++) testv[k] = (double) operms[t]->data[k];

   /* Not cruft, but quiet...
   MYDEBUG(D_RGB_OPERM){
     printf("#------------------------------------------------------------------\n");
     printf("# Generating offset sample permutation pi's\n");
   }
   */
   for(k=0;k<2*rgb_operm_k-1;k++){
     gsl_sort_index(ps,&testv[k],1,rgb_operm_k);
     pi[k] = piperm(ps,rgb_operm_k);

     /* Not cruft, but quiet...
     MYDEBUG(D_RGB_OPERM){
       printf("# %u: ",k);
       for(ip=k;ip<rgb_operm_k+k;ip++){
         printf("%.1f ",testv[ip]);
       }
       printf("\n# ");
       for(ip=0;ip<rgb_operm_k;ip++){
         printf("%u ",ps[ip]);
       }
       printf(" = %u\n",pi[k]);
     }
     */

   }

   /*
    * This is the business end of things.  The covariance matrix is the
    * the sum of a central function of the permutation indices that yields
    * nperms-1/nperms on diagonal, -1/nperms off diagonal, for all the
    * possible permutations, for the FIRST permutation in a sample (fi)
    * times the sum of the same function over all the overlapping permutations
    * drawn from the same sample.  Quite simple, really.
    */
   for(i=0;i<nperms;i++){
     fi = fpipi(i,pi[rgb_operm_k-1],nperms);
     for(j=0;j<nperms;j++){
       fj = 0.0;
       for(k=1;k<rgb_operm_k;k++){
         fj += fpipi(j,pi[rgb_operm_k - 1 + k],nperms);
         fj += fpipi(j,pi[rgb_operm_k - 1 - k],nperms);
       }
       cexact[i][j] += fi*fj;
     }
   }

 }

 MYDEBUG(D_RGB_OPERM){
   printf("# rgb_operm:==============================\n");
   printf("# rgb_operm: cexact[][] = \n");
 }
 for(i=0;i<nperms;i++){
   MYDEBUG(D_RGB_OPERM){
     printf("# ");
   }
   for(j=0;j<nperms;j++){
     cexact[i][j] /= noperms;
     MYDEBUG(D_RGB_OPERM){
       printf("%10.6f  ",cexact[i][j]);
     }
   }
   MYDEBUG(D_RGB_OPERM){
     printf("\n");
   }
 }

 /*
  * Free operms[]
  */
 for(i=0;i<noperms;i++){
   gsl_permutation_free(operms[i]);
 }
 free(operms);

}

void make_cexpt()
{

 int i,j,k,ip,t;
 double fi,fj;
 /*
  * This is the test vector.
  */
 double testv[RGB_OPERM_KMAX*2];  /* easier than malloc etc, but beware length */
 /*
  * pi[] is the permutation index of a sample.  ps[] holds the
  * actual sample.
  */
 int pi[RGB_OPERM_KMAX*2],ps[RGB_OPERM_KMAX*2];

 MYDEBUG(D_RGB_OPERM){
   printf("#==================================================================\n");
   printf("# rgb_operm: Running cexpt()\n");
 }

 /*
  * We evaluate cexpt[][] by sampling.  In a nutshell, this involves
  *   a) Filling testv[] with 2*rgb_operm_k - 1 random uints or doubles
  * It clearly cannot matter which we use, as long as the probability of
  * exact duplicates in a sample is very low.
  *   b) Using gsl_sort_index the exact same way it was used in make_cexact()
  * to generate the pi[] index, using ps[] as scratch space for the sort
  * indices.
  *   c) Evaluating fi and fj from the SAMPLED result, tsamples times.
  *   d) Normalizing.
  * Note that this is pretty much identical to the way we formed c_exact[][]
  * except that we are determining the relative frequency of each sort order
  * permutation 2*rgb_operm_k-1 long.
  *
  * NOTE WELL!  I honestly think that it is borderline silly to view
  * this as a matrix and to go through all of this nonsense.  The theoretical
  * c_exact[][] is computed from the observation that all the permutations
  * of n objects have equal weight = 1/n!.  Consequently, they should
  * individually be binomially distributed, tending to normal with many
  * samples.  Collectively they should be distributed like a vector of
  * equal binomial probabilities and a p-value should follow either from
  * chisq on n!-1 DoF or for that matter a KS test.  I see no way that
  * making it into a matrix can increase the sensitivity of the test -- if
  * the p-values are well defined in the two cases they can only be equal
  * by their very definition.
  *
  * If you are a statistician reading these words and disagree, please
  * communicate with me and explain why I'm wrong.  I'm still very much
  * learning statistics and would cherish gentle correction.
  */
 for(t=0;t<tsamples;t++){
   /*
    * To sort into a perm, test vector needs to be double.
    */
   for(k=0;k<3*rgb_operm_k - 2;k++) testv[k] = (double) gsl_rng_get(rng);

   /* Not cruft, but quiet...
   MYDEBUG(D_RGB_OPERM){
     printf("#------------------------------------------------------------------\n");
     printf("# Generating offset sample permutation pi's\n");
   }
   */
   for(k=0;k<2*rgb_operm_k-1;k++){
     gsl_sort_index(ps,&testv[k],1,rgb_operm_k);
     pi[k] = piperm(ps,rgb_operm_k);

     /* Not cruft, but quiet...
     MYDEBUG(D_RGB_OPERM){
       printf("# %u: ",k);
       for(ip=k;ip<rgb_operm_k+k;ip++){
         printf("%.1f ",testv[ip]);
       }
       printf("\n# ");
       for(ip=0;ip<rgb_operm_k;ip++){
         printf("%u ",permsample->data[ip]);
       }
       printf(" = %u\n",pi[k]);
     }
     */
   }

   /*
    * This is the business end of things.  The covariance matrix is the
    * the sum of a central function of the permutation indices that yields
    * nperms-1/nperms on diagonal, -1/nperms off diagonal, for all the
    * possible permutations, for the FIRST permutation in a sample (fi)
    * times the sum of the same function over all the overlapping permutations
    * drawn from the same sample.  Quite simple, really.
    */
   for(i=0;i<nperms;i++){
     fi = fpipi(i,pi[rgb_operm_k-1],nperms);
     for(j=0;j<nperms;j++){
       fj = 0.0;
       for(k=1;k<rgb_operm_k;k++){
         fj += fpipi(j,pi[rgb_operm_k - 1 + k],nperms);
         fj += fpipi(j,pi[rgb_operm_k - 1 - k],nperms);
       }
       cexpt[i][j] += fi*fj;
     }
   }

 }

 MYDEBUG(D_RGB_OPERM){
   printf("# rgb_operm:==============================\n");
   printf("# rgb_operm: cexpt[][] = \n");
 }
 for(i=0;i<nperms;i++){
   MYDEBUG(D_RGB_OPERM){
     printf("# ");
   }
   for(j=0;j<nperms;j++){
     cexpt[i][j] /= tsamples;
     MYDEBUG(D_RGB_OPERM){
       printf("%10.6f  ",cexpt[i][j]);
     }
   }
   MYDEBUG(D_RGB_OPERM){
     printf("\n");
   }
 }

}

uint piperm(size_t *data,int len)
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
  * Copy data -> dtmp
  */
 memcpy((void *)dtmp,(void *) data,len*sizeof(size_t));
 MYDEBUG(D_RGB_OPERM){
   /* This isn't exactly cruft, but we don't need to
    * watch just now.  Truthfully, I doubt we need
    * to generate this at all in cexpt().
   printf("# rgb_operm: iperm(): dtmp = ");
   for(i=0;i<len;i++){
     printf("%u ",dtmp[i]);
   }
   printf("\n");
   */
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

double fpipi(int pi1,int pi2,int nkp)
{

 int i;
 double fret;

 /*
  * compute the k-permutation index from iperm for the window
  * at data[offset] of length len.  If it matches pind, return
  * the first quantity, otherwise return the second.
  */
 if(pi1 == pi2){

   fret = (double) (nkp - 1.0)/nkp;
   if(verbose < 0){
     printf(" f(%d,%d) = %10.6f\n",pi1,pi2,fret);
   }
   return(fret);

 } else {

   fret = (double) (-1.0/nkp);
   if(verbose < 0){
     printf(" f(%d,%d) = %10.6f\n",pi1,pi2,fret);
   }
   return(fret);

 }


}




