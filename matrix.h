/*
* $Id$
*
* See copyright in copyright.h and the accompanying file COPYING
*
*/

#define MATRIX_MAX_DIM 8

/*
 * MATRIX The matrix is a struct that contains the generalized, packed set
 * of pointers and parameters required to create the tensor itself and
 * pack a "matrix view" of it according to those parameters.  The matrix
 * does NOT contain a direct reference to the data block itself as there
 * may be a number of matrices packed from a single block -- for example,
 * in relativity a 2nd rank 4x4 tensor might have the embedded 3x3
 * submatrix for the space-like piece accessible by a different name.  A
 * matrix can also be allocated independent of a tensor (straight from an
 * already existing data block).  Freeing a matrix does NOT free the
 * underlying tensor data (unless done with a routine that does both).
 *
 * As noted above, the matrices themselves are of void type and hence
 * CANNOT be used to directly address the data.  They MUST be cast to a
 * pointer of the matching rank and appropriate type.  There is, however,
 * no reason not to make a bunch of shell subroutines to do the requisite
 * cast for you.
 */
typedef struct
{
  int dim;                /* Must be less than TENSOR_MAX_DIM */
  int signature;          /* 0=covariant; 1=contravariant */
  /*
   * This permits several ways of generating a matrix out of a block.
   * Note that all of these are basically vectors with indices that
<?xml version="1.0"?>
* run from 0-dim.  At least two of them (typically lower/upper
   * or lower/length) are used to generate the matrix view.
   */
  int *lower;
  int *upper;
  int *length;
  /*
   * the number of *'s here MUST be greater than or equal to
   * MATRIX_MAX_DIM.
   */
  void ********mptr;
} new_gsl_matrix;

/*
 * Very, very simple.
 *  matrix_alloc allocates a matrix struct and fills it in (a complicated process!)
 *  matrix_free frees it (but NOT the data block it references!)
 */
new_gsl_matrix *matrix_alloc(size_t dim, int signature, int *lower, int *upper);
int matrix_free(new_gsl_matrix *matrix);

/*
 * matrix_row_alloc allocates space for a matrix row of POINTERS.
 * The space is returned with void pointer type, and is presumed
 * to be filled only with memory addresses when the matrix is fully
 * packed.  As noted above, the entire matrix is cast to an appropriate
 * type before referencing it, so all pointers dereference correctly.
 */
void *matrix_row_alloc(size_t length, int lower_index);
int matrix_row_free(void *matrix_row);

