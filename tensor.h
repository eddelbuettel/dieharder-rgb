/*
* $Id$
*
* See copyright in copyright.h and the accompanying file COPYING
*
*/

#include "block.h"
#include "matrix.h"


/*
 * The tensor struct.  A tensor is a block and matrix.  Nuttin' to it.
 */
typedef struct
{

 new_gsl_block *block;
 new_gsl_matrix *matrix;

} new_gsl_tensor;


/*
 * Very, very simple.
 *  alloc allocates everything (block and matrix).
 *  calloc allocates everything AND zeros the block (calls alloc).
 *  free frees everything.
 * Can't get much simpler.
 *  In almost ALL CASES once one has allocated the tensor one will want
 * extract and cast the actual packed matrix to a ***...**ptr of whatever
 * type you are trying to create.  This ptr is never freed, but the tensor
 * (or its underlying block and matrix) must be.
 *  Once this cast is done, the matrix/tensor can be referenced via the ptr
 * like ptr[i][j][k]...[m][n]
 * as long as i,j,k...m,n remain in bounds relative to the lower and upper
 * tensor index vectors passed in the tensor_alloc() call.
 */
new_gsl_tensor *tensor_alloc(size_t dim, size_t stride, int owner, int signature, int *lower, int *upper);
new_gsl_tensor *tensor_calloc(size_t dim, size_t stride, int owner, int signature, int *lower, int *upper);
int tensor_free(new_gsl_tensor *tensor);
