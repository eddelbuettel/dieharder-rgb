/*
* $Id$
*
* See copyright in copyright.h and the accompanying file COPYING
*
*/

/*
 * BLOCK
 * The block is the basic data object.  It is a wrapper for a pointer
 * to a block of memory and a small amount of attached header data.
 *
 * Note that the data block has void (no) type.  It is a raw pointer to
 * the address of a block of memory of size size (in bytes) and must thus 
 * be handled with care.  Presumably malloc/free can still manage.
 *
 * A gsl_vector was just a block with a stride and owner.  By putting
 * stride and owner into the block itself, tremendous simplification
 * is possible.  If stride=owner=NULL, obviously the block is just
 * a block.  If stride is non-NULL but owner is NULL, it is a typed
 * block.  If stride is NULL but the owner is non-NULL it is an owned
 * (but untyped) block (ownership is as likely to be useful for blocks
 * for anything else, I suppose).  If both are non-NULL, the block is
 * an (old-style) vector, but without the redundant block pointer.
 */
typedef struct
{
 size_t size;
 size_t stride;
 int owner;
 void *data;
} new_gsl_block;


/*
 * Keep it very, very simple.
 *  new_gsl_block_alloc allocates a basic block using malloc.
 *  new_gsl_block_calloc allocates AND zeros the block.
 *  free frees the block.
 */
new_gsl_block *block_alloc(size_t size, size_t stride, int owner);
new_gsl_block *block_calloc(size_t size, size_t stride, int owner);
int block_free(new_gsl_block *block);

