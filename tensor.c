/*
 * $Id$
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include "tensor.h"

/*
 * We'll assume that anything that this is ever linked to will likely
 * have a verbose variable, at least if one wishes/needs to debug
 * it.  However, we will want very much to make this code bulletproof
 * so that it never needs active debugging!
 */
extern int verbose;

new_gsl_tensor *tensor_alloc(size_t dim, size_t stride, int owner, int signature, int *lower, int *upper)
{

 int i;
 size_t blocksize;
 new_gsl_tensor *tensor;

 /*
  * At this point I think all there is to it is allocating tensor,
  * allocating matrix, callocating block, packing matrix.
  */
 if(verbose){
   printf("tensor_alloc:\n");
 }
 tensor = (new_gsl_tensor *)malloc(sizeof(tensor));
 if(tensor == 0){
   fprintf(stderr,"tensor_alloc() error: Not enough memory.\n");
   exit(0);
 }
 if(verbose){
   printf("tensor_alloc: tensor struct created at %x.\n",tensor);
 }
 tensor->matrix = matrix_alloc(dim, signature, lower, upper);

 if(verbose){
   printf("tensor_alloc: tensor->matrix created at %x\n",tensor->matrix);
   printf("tensor_alloc: computing size of %d dimensional data block.\n",tensor->matrix->dim);
 }
 blocksize = 1;
 for(i=0;i<tensor->matrix->dim;i++){
   blocksize *= tensor->matrix->length[i];
   if(verbose){
     printf("tensor_alloc: blocksize[%d] = %d\n",i,blocksize);
   }
 }
 blocksize *= stride;
 if(verbose){
   printf("tensor_alloc: Creating data block of length %d, stride %d, owner %d.\n",blocksize,stride,owner);
 }
 tensor->block = block_alloc(blocksize,stride,owner);
 if(verbose){
   printf("tensor_alloc: packing tensor->block addresses into tensor->matrix.\n");
 }
 matrix_pack(tensor->matrix,tensor->block);
 return(tensor);
   
}

new_gsl_tensor *tensor_calloc(size_t dim, size_t stride, int owner, int signature, int *lower, int *upper)
{

 int i;
 size_t blocksize;
 new_gsl_tensor *tensor;

 /*
  * At this point I think all there is to it is allocating tensor,
  * allocating matrix, callocating block, packing matrix.
  */
 tensor = (new_gsl_tensor *)malloc(sizeof(tensor));
 if(tensor == 0){
   fprintf(stderr,"tensor_alloc() error: Not enough memory.\n");
   exit(0);
 }
 tensor->matrix = matrix_alloc(dim, signature, lower, upper);
 for(i=0;i<dim;i++){
   blocksize += tensor->matrix->length[i];
 }
 blocksize *= stride;
 tensor->block = block_calloc(blocksize,stride,owner);

 /*
  * Now pack it.  Matrix knows how it should be packed, and block is the
  * actual data block whose addresses should be packed.
  */
 matrix_pack(tensor->matrix,tensor->block);

 
 return(tensor);
   
}

int tensor_free(new_gsl_tensor *tensor)
{

 if(verbose){
   printf("tensor_free: free tensor->matrix at %x\n",tensor->matrix);
 }
 matrix_free(tensor->matrix);
 if(verbose){
   printf("tensor_free: free tensor->block at %x\n",tensor->block);
 }
 block_free(tensor->block);
 if(verbose){
   printf("tensor_free: free tensor at %x\n",tensor);
 }
 free(tensor);

}

