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
#include "matrix.h"
#include "block.h"

/*
 * We'll assume that anything that this is ever linked to will likely
 * have a verbose variable, at least if one wishes/needs to debug
 * it.  However, we will want very much to make this code bulletproof
 * so that it never needs active debugging!
 */
extern int verbose;

new_gsl_matrix *matrix_alloc(size_t dim, int signature, int *lower, int *upper)
{

 new_gsl_matrix *matrix;
 int i,i0,i1,i2,i3,i4,i5,i6;
 int vdisp;

 /*
  * One little sanity check here.
  */
 if(dim > MATRIX_MAX_DIM){
   fprintf(stderr,"matrix_alloc() error.  Cannot allocate matrix of more than %d dimensions.\n",MATRIX_MAX_DIM);
   exit(0);
 }
 if(verbose){
   printf("matrix_alloc:\n");
 }
 matrix = (new_gsl_matrix *)malloc(sizeof(new_gsl_matrix));
 if(verbose){
   printf("matrix_alloc: matrix struct created at %x.\n",matrix);
 }

 /*
  * Set the length(s) of each matrix dimension.  This isn't as efficient
  * as a memcpy, but it is perhaps safer, as somebody might have passed
  * a vector LONGER than dim as lower[] and upper[].
  */
 matrix->dim = dim;
 matrix->signature = signature;
 if(verbose){
   printf("matrix_alloc: matrix->dim = %d\n",matrix->dim);
   printf("matrix_alloc: matrix->signature = %d\n",matrix->signature);
 }
 matrix->lower = (int *) malloc((size_t)(matrix->dim*sizeof(int)));
 if(verbose){
   printf("matrix_alloc: matrix->lower allocated at %x.\n",matrix->lower);
 }
 matrix->upper = (int *) malloc((size_t)(matrix->dim*sizeof(int)));
 if(verbose){
   printf("matrix_alloc: matrix->upper allocated at %x.\n",matrix->upper);
 }
 matrix->length = (int *) malloc((size_t)(matrix->dim*sizeof(int)));
 if(verbose){
   printf("matrix_alloc: matrix->length allocated at %x.\n",matrix->length);
 }
 for(i=0;i<matrix->dim;i++){
   matrix->lower[i] = lower[i];
   matrix->upper[i] = upper[i];
   matrix->length[i] = upper[i] - lower[i] + 1;
   if(verbose){
     printf("matrix_alloc: matrix->lower[%2d] = %d\n",i,matrix->lower[i]);
     printf("matrix_alloc: matrix->upper[%2d] = %d\n",i,matrix->upper[i]);
     printf("matrix_alloc: matrix->length[%2d] = %d\n",i,matrix->length[i]);
   }
 }
 if(verbose){
   printf("matrix_alloc: Now we start the fancy mptr stuff.\n");
 }

 /*
  * The following code does the following.  It starts at the MATRIX_MAX_DIM
  * pointer level and allocates matrix_row_ptr's for each level UP to
  * dim-1.  dim-1 is the (only) level that points to actual addresses in
  * a typed memory block, and it is set to NULL to remind us (via a
  * segment violation if necessary) to pack the matrix with addresses
  * from the data block being dereferenced.
  */
 if(matrix->dim == 1) {
  matrix->mptr = (void ********) 0;
  if(verbose){
   printf("matrix_alloc: matrix->dim == 1.\n");
   printf("matrix_alloc: matrix->mptr at %x.\n",matrix->mptr);
  }
 } else {
  /*
   * Create 1st dimension
   */
  matrix->mptr = (void ********) matrix_row_alloc(matrix->length[0],matrix->lower[0]);
  if(verbose){
   printf("matrix_alloc: matrix->mptr of length %d allocated at %x.\n",matrix->length[0]*sizeof(void*),matrix->mptr);
  }
  for(i0 = matrix->lower[0];i0<=matrix->upper[0];i0++){
   if(matrix->dim == 2) {
    matrix->mptr[i0] = (void *******) 0;
    if(verbose){
     printf("matrix_alloc: matrix->dim == 2.\n");
     printf("matrix_alloc: matrix->mptr[%2d] at %x.\n",i0,matrix->mptr[i0]);
    }
   } else {
    /*
     * Create 2nd dimension
     */
    matrix->mptr[i0] = (void *******) matrix_row_alloc(matrix->length[1],matrix->lower[1]);
    if(verbose){
     printf("matrix_alloc: matrix->mptr[%2d] allocated at %x.\n",i0,matrix->mptr[i0]);
    }
    for(i1 = matrix->lower[1];i1<=matrix->upper[1];i1++){
     if(matrix->dim == 3) {
      matrix->mptr[i0][i1] = (void ******) 0;
      if(verbose){
       printf("matrix_alloc: matrix->dim == 3 done.\n");
      }
     } else {
      /*
       * Create 3rd dimension
       */
      matrix->mptr[i0][i1] = (void ******) matrix_row_alloc(matrix->length[2],matrix->lower[2]);
      for(i2 = matrix->lower[2];i2<=matrix->upper[2];i2++){
       if(matrix->dim == 4) {
        matrix->mptr[i0][i1][i2] = (void *****) 0;
        if(verbose){
         printf("matrix_alloc: matrix->dim == 4 done.\n");
        }
       } else {
        /*
         * Create 4th dimension
         */
        matrix->mptr[i0][i1][i2] = (void *****) matrix_row_alloc(matrix->length[3],matrix->lower[3]);
        for(i3 = matrix->lower[3];i3<=matrix->upper[3];i3++){
         if(matrix->dim == 5) {
          matrix->mptr[i0][i1][i2][i3] = (void ****) 0;
          if(verbose){
           printf("matrix_alloc: matrix->dim == 5 done.\n");
          }
         } else {
          /*
           * Create 5th dimension
           */
          matrix->mptr[i0][i1][i2][i3] = (void ****) matrix_row_alloc(matrix->length[4],matrix->lower[4]);
          for(i4 = matrix->lower[4];i4<=matrix->upper[4];i4++){
           if(matrix->dim == 6) {
            matrix->mptr[i0][i1][i2][i3][i4] = (void ***) 0;
            if(verbose){
             printf("matrix_alloc: matrix->dim == 6 done.\n");
            }
           } else {
            /*
             * Create 6th dimension
             */
            matrix->mptr[i0][i1][i2][i3][i4] = (void ***) matrix_row_alloc(matrix->length[5],matrix->lower[5]);
            for(i5 = matrix->lower[5];i5<=matrix->upper[5];i5++){
             if(matrix->dim == 7) {
              matrix->mptr[i0][i1][i2][i3][i4][i5] = (void **) 0;
              if(verbose){
               printf("matrix_alloc: matrix->dim == 7 done.\n");
              }
             } else {
              /*
               * Create 7th dimension
               */
              matrix->mptr[i0][i1][i2][i3][i4][i5] = (void **) matrix_row_alloc(matrix->length[6],matrix->lower[6]);
              for(i6 = matrix->lower[6];i6<=matrix->upper[6];i6++){
               if(matrix->dim == 8) {
                matrix->mptr[i0][i1][i2][i3][i4][i5][i6] = (void *) 0;
                if(verbose){
                 printf("matrix_alloc: matrix->dim == 8 done.\n");
                }
               } else {
                fprintf(stderr,"matrix_alloc() error.  Cannot allocate matrix of more than %d dimensions.\n",MATRIX_MAX_DIM);
                exit(0);
               }
              }
             }
            }
           }
          }
         }
        }
       }
      }
     }
    }
   }
  }
 }
 if(verbose){
   printf("matrix_alloc:  Returning matrix at %x.\n",matrix);
 }
 return(matrix);
}

int matrix_free(new_gsl_matrix *matrix)
{

 int i,i0,i1,i2,i3,i4,i5,i6;
 int vdisp;

 /*
  * Freeing is the inverse of creating.  We have to go through all the
  * loops but IN REVERSE ORDER, freeing the deepest surviving pointer row
  * BEFORE any of the shallower rows that are part of its reference, or
  * we'll leak like a sieve.  * There are two gotchas in here.  One is
  * that we don't free the data level here at all.  For example, if
  * matrix->dim = 1 (vector) we don't free anything as matrix->mptr points
  * directly at the data block.  If matrix->dim = 2 (2d matrix) we do free
  * matrix->mptr (which has a malloc'd block of void* addresses for the
  * first row, the CONTENTS of which point to the data).  Ditto for higher
  * dim -- we free only the void* pointers in the hierarchy, one level
  * LESS than the matrix dimension.  * The other is that we displaced the
  * pointer addresses to allow for the lower index!  So when we free, we
  * need to invert the displacement!  However, free KNOWS the size of the
  * addresses, so we invert the displacement with no sizeof(void*) (even
  * though we had to use the sizeof(void*) to scale the original return).
  * Go figure!
  */
 if(matrix->dim > 1) {
  if(matrix->dim > 2) {
   for(i0 = matrix->lower[0];i0<=matrix->upper[0];i0++){
    if(matrix->dim > 3) {
     for(i1 = matrix->lower[1];i1<=matrix->upper[1];i1++){
      if(matrix->dim > 4) {
       for(i2 = matrix->lower[2];i2<=matrix->upper[2];i2++){
        if(matrix->dim > 5) {
         for(i3 = matrix->lower[3];i3<=matrix->upper[3];i3++){
          if(matrix->dim > 6) {
           for(i4 = matrix->lower[4];i4<=matrix->upper[4];i4++){
            if(matrix->dim > 7) {
             for(i5 = matrix->lower[5];i5<=matrix->upper[5];i5++){
              if(matrix->dim > 8) {
               fprintf(stderr,"matrix_free() error.  Cannot deallocate matrix of more than %d dimensions.\n",MATRIX_MAX_DIM);
               exit(0);
	      }
              /* Free row of pointers only, not the data to which they point */
              if(verbose){
                printf("matrix_free: Freeing matrix->mptr[%2d][%2d][%2d][%2d][%2d][%2d] at %x\n",
                  i0,i1,i2,i3,i4,i5,(void *)(matrix->mptr[i0][i1][i2][i3][i4][i5] + matrix->lower[6]));
              }
              free(matrix->mptr[i0][i1][i2][i3][i4][i5] + matrix->lower[6]);
             }
	    }
            /* Free row of pointers only, not the data to which they point */
            if(verbose){
              printf("matrix_free: Freeing matrix->mptr[%2d][%2d][%2d][%2d][%2d] at %x\n",
                i0,i1,i2,i3,i4,(void *)(matrix->mptr[i0][i1][i2][i3][i4] + matrix->lower[5]));
            }
            free(matrix->mptr[i0][i1][i2][i3][i4] + matrix->lower[5]);
           }
	  }
          /* Free row of pointers only, not the data to which they point */
          if(verbose){
            printf("matrix_free: Freeing matrix->mptr[%2d][%2d][%2d][%2d] at %x\n",
                i0,i1,i2,i3,(void *)(matrix->mptr[i0][i1][i2][i3] + matrix->lower[4]));
          }
          free(matrix->mptr[i0][i1][i2][i3] + matrix->lower[4]);
         }
        }
        /* Free row of pointers only, not the data to which they point */
        if(verbose){
          printf("matrix_free: Freeing matrix->mptr[%2d][%2d][%2d] at %x\n",
              i0,i1,i2,(void*)(matrix->mptr[i0][i1][i2] + matrix->lower[3]));
        }
        free(matrix->mptr[i0][i1][i2] + matrix->lower[3]);
       }
      }
      /* Free row of pointers only, not the data to which they point */
      if(verbose){
        printf("matrix_free: Freeing matrix->mptr[%2d][%2d] at %x\n",
            i0,i1,(void*)(matrix->mptr[i0][i1]+matrix->lower[2]));
      }
      free(matrix->mptr[i0][i1] + matrix->lower[2]);
     }
    }
    /* Free row of pointers only, not the data to which they point */
    if(verbose){
      printf("matrix_free: Freeing matrix->mptr[%2d] at %x\n",
          i0,(void*)(matrix->mptr[i0] + matrix->lower[1]));
    }
    free((void*) (matrix->mptr[i0] + matrix->lower[1]));
   }
  }
  if(verbose){
    printf("matrix_free: Freeing matrix->mptr at %x\n",
        (void*)(matrix->mptr + matrix->lower[0]));
  }
  free(matrix->mptr + matrix->lower[0]);
 }
 /*
  * At this point (only) we're free with the upper/lower/length subvectors.
  */
 if(verbose){
   printf("matrix_free: Freeing matrix->lower at %x\n",matrix->lower);
 }
 free(matrix->lower);
 if(verbose){
   printf("matrix_free: Freeing matrix->upper at %x\n",matrix->upper);
 }
 free(matrix->upper);
 if(verbose){
   printf("matrix_free: Freeing matrix->length at %x\n",matrix->length);
 }
 free(matrix->length);
 /*
  * And at the VERY end, with matrix itself.
  */
 if(verbose){
   printf("matrix_free: Freeing matrix struc at %x\n",matrix);
 }
 free(matrix);

}


int matrix_pack(new_gsl_matrix *matrix,new_gsl_block *block)
{

 int i,i0,i1,i2,i3,i4,i5,i6,i7;
 int vdisp;

 /*
  * Packing goes on in the identical fashion to allocating, except that
  * in the innermost loop (where there are dim-1 indices) we fill in
  * the appropriately displaced addresses from the actual data block.
  * If we were good dogs, we'd check the block to make sure that it
  * has room first, so that the matrix doesn't ever go out of bounds.
  */
 if(verbose){
   printf("matrix_pack: Packing addresses from block->data starting at %x\n",block->data);
 }
 if(matrix->dim == 1) {
  vdisp = -matrix->lower[0];
  vdisp *= block->stride;
  if(verbose){
    printf("matrix_pack: vdisp(%d) = %d\n",matrix->lower[0],vdisp);
  }
  matrix->mptr = (void ********) (block->data + vdisp);
  if(verbose){
    printf("matrix_pack: matrix->mptr points at data block %x\n",matrix->mptr);
  }
 } else {
  for(i0 = matrix->lower[0];i0<=matrix->upper[0];i0++){
   if(matrix->dim == 2) {
    vdisp = ((i0-matrix->lower[0])*matrix->length[1]
               - matrix->lower[1]);
    vdisp *= block->stride;
    if(verbose){
      printf("matrix_pack: vdisp(%d,%d) = %d\n",i0,matrix->lower[1],vdisp);
    }
    matrix->mptr[i0] = (void *******)(block->data + vdisp);
    if(verbose){
      printf("matrix_pack: matrix->mptr[%d] points at data block %x\n",i0,matrix->mptr[i0]);
    }
   } else {
    for(i1 = matrix->lower[1];i1<=matrix->upper[1];i1++){
     if(matrix->dim == 3) {
      vdisp = ( ((i0-matrix->lower[0])*matrix->length[1]
                + (i1-matrix->lower[1]))*matrix->length[2]
                -  matrix->lower[2] );
      vdisp *= block->stride;
      if(verbose){
        printf("matrix_pack: vdisp(%d,%d,%d) = %d\n",i0,i1,matrix->lower[2],vdisp);
      }
      matrix->mptr[i0][i1] = (void ******) (block->data + vdisp);
      if(verbose){
        printf("matrix_pack: matrix->mptr[%d][%d] points at data block %x\n",i0,i1,matrix->mptr[i0][i1]);
      }
     } else {
      for(i2 = matrix->lower[2];i2<=matrix->upper[2];i2++){
       if(matrix->dim == 4) {
        vdisp = ( (((i0-matrix->lower[0])*matrix->length[1]
                  + (i1-matrix->lower[1]))*matrix->length[2]
                  + (i2-matrix->lower[2]))*matrix->length[3]
                  -  matrix->lower[3] );
        vdisp *= block->stride;
        if(verbose){
          printf("matrix_pack: vdisp(%d,%d,%d,%d) = %d\n",i0,i1,i2,matrix->lower[3],vdisp);
        }
        matrix->mptr[i0][i1][i2] = (void *****) (block->data + vdisp);
        if(verbose){
          printf("matrix_pack: matrix->mptr[%d][%d][%d] points at data block %x\n",i0,i1,i2,matrix->mptr[i0][i1][i2]);
        }
       } else {
        for(i3 = matrix->lower[3];i3<=matrix->upper[3];i3++){
         if(matrix->dim == 5) {
          vdisp = ( ((((i0-matrix->lower[0])*matrix->length[1]
                    + (i1-matrix->lower[1]))*matrix->length[2]
                    + (i2-matrix->lower[2]))*matrix->length[3]
                    + (i3-matrix->lower[3]))*matrix->length[4]
                    -  matrix->lower[4] );
          vdisp *= block->stride;
          if(verbose){
            printf("matrix_pack: vdisp(%d,%d,%d,%d,%d) = %d\n",i0,i1,i2,i3,matrix->lower[4],vdisp);
          }
          matrix->mptr[i0][i1][i2][i3] = (void ****) (block->data + vdisp);
          if(verbose){
            printf("matrix_pack: matrix->mptr[%d][%d][%d][%d] points at data block %x\n",i0,i1,i2,i3,matrix->mptr[i0][i1][i2][i3]);
          }
         } else {
          for(i4 = matrix->lower[4];i4<=matrix->upper[4];i4++){
           if(matrix->dim == 6) {
            vdisp = ( (((((i0-matrix->lower[0])*matrix->length[1]
                      + (i1-matrix->lower[1]))*matrix->length[2]
                      + (i2-matrix->lower[2]))*matrix->length[3]
                      + (i3-matrix->lower[3]))*matrix->length[4]
                      + (i4-matrix->lower[4]))*matrix->length[5]
                      -  matrix->lower[5] );
            vdisp *= block->stride;
            if(verbose){
              printf("matrix_pack: vdisp(%d,%d,%d,%d,%d,%d) = %d\n",i0,i1,i2,i3,i4,matrix->lower[5],vdisp);
            }
            matrix->mptr[i0][i1][i2][i3][i4] = (void ***) (block->data + vdisp);
            if(verbose){
              printf("matrix_pack: matrix->mptr[%d][%d][%d][%d][%d] points at data block %x\n",i0,i1,i2,i3,i4,matrix->mptr[i0][i1][i2][i3][i4]);
            }
           } else {
            for(i5 = matrix->lower[5];i5<=matrix->upper[5];i5++){
             if(matrix->dim == 7) {
              vdisp = ( ((((((i0-matrix->lower[0])*matrix->length[1]
                        + (i1-matrix->lower[1]))*matrix->length[2]
                        + (i2-matrix->lower[2]))*matrix->length[3]
                        + (i3-matrix->lower[3]))*matrix->length[4]
                        + (i4-matrix->lower[4]))*matrix->length[5]
                        + (i5-matrix->lower[5]))*matrix->length[6]
                        -  matrix->lower[6] );
              vdisp *= block->stride;
              if(verbose){
                printf("matrix_pack: vdisp(%d,%d,%d,%d,%d,%d,%d) = %d\n",i0,i1,i2,i3,i4,i5,matrix->lower[6],vdisp);
              }
              matrix->mptr[i0][i1][i2][i3][i4][i5] = (void **) (block->data + vdisp);
              if(verbose){
                printf("matrix_pack: matrix->mptr[%d][%d][%d][%d][%d][%d] points at data block %x\n",i0,i1,i2,i3,i4,i5,matrix->mptr[i0][i1][i2][i3][i4][i5]);
              }
             } else {
              for(i6 = matrix->lower[6];i6<=matrix->upper[6];i6++){
               if(matrix->dim == 8) {
                vdisp = ( (((((((i0-matrix->lower[0])*matrix->length[1]
                          + (i1-matrix->lower[1]))*matrix->length[2]
                          + (i2-matrix->lower[2]))*matrix->length[3]
                          + (i3-matrix->lower[3]))*matrix->length[4]
                          + (i4-matrix->lower[4]))*matrix->length[5]
                          + (i5-matrix->lower[5]))*matrix->length[6]
                          + (i6-matrix->lower[6]))*matrix->length[7]
                          -  matrix->lower[7] );
                vdisp *= block->stride;
                if(verbose){
                  printf("matrix_pack: vdisp(%d,%d,%d,%d,%d,%d,%d,%d) = %d\n",i0,i1,i2,i3,i4,i5,i6,matrix->lower[7],vdisp);
                }
                matrix->mptr[i0][i1][i2][i3][i4][i5][i6] = (void *) (block->data + vdisp);
                if(verbose){
                  printf("matrix_pack: matrix->mptr[%d][%d][%d][%d][%d][%d][%d] points at data block %x\n",i0,i1,i2,i3,i4,i5,i6,matrix->mptr[i0][i1][i2][i3][i4][i5][i6]);
                }
               } else {
                fprintf(stderr,"matrix_pack() error: dimension cannot exceed %d\n",MATRIX_MAX_DIM);
		exit(0);
               }
              }
             }
            }
           }
          }
         }
        }
       }
      }
     }
    }
   }
  }
 }

}

void *matrix_row_alloc(size_t length,int lower)
{

  int vdisp;
  size_t size;
  void *rowptr;

  size = sizeof(void *)*length;
  vdisp = lower*sizeof(void *);
  rowptr = (void *)malloc(size);
  if(verbose){
    printf("matrix_row_alloc: row data address = %x, disp = %d, returning %x\n",
      rowptr,vdisp,rowptr-vdisp);
  }
  rowptr -= vdisp;
  return(rowptr);
}
