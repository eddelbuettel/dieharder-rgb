/*
 * $Id$
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include "block.h"

/*
 * We'll assume that anything that this is ever linked to will likely
 * have a verbose variable, at least if one wishes/needs to debug
 * it.  However, we will want very much to make this code bulletproof
 * so that it never needs active debugging!
 */
extern int verbose;

new_gsl_block *block_alloc(size_t size, size_t stride, int owner)
{

 size_t length;
 new_gsl_block *block;

 /*
  * allocate the tensor struct itself and bitch if trouble.
  */
 if(verbose){
   printf("block_alloc:\n");
 }
 block = (new_gsl_block *)malloc(sizeof(new_gsl_block));
 if(block == 0){
   fprintf(stderr,"Error: insufficient memory in new_gsl_block_alloc()\n");
   exit(0);
 }
 if(verbose){
   printf("block_alloc: Allocated block at %x\n",block);
 }

 /*
  * set various elements of the struct
  */
 block->size = size;
 block->stride = stride;
 block->owner = owner;
 if(verbose){
   printf("block_alloc: Set block->size = %u, block->stride = %u, block->owner = %d\n",
     block->size,block->stride,block->owner);
 }
 /*
  * and mallocate it, complaining and dying if we can't.
  */
 block->data = (void *)malloc(block->size);
 if(block->data == 0) {
   fprintf(stderr,"Error: insufficient memory for data block in new_gsl_block_alloc()\n");
   free(block);
   exit(0);
 }
 if(verbose){
   printf("block_alloc: block->data at %x\n",block->data);
 }
 return(block);
   
}

new_gsl_block *block_calloc(size_t size, size_t stride, int owner)
{

 int i;
 new_gsl_block *b;
 b = block_alloc(size,stride,owner);
 memset(b->data,0,size);
 return b;

}

int block_free(new_gsl_block *block)
{
 if(verbose){
   printf("block_free:  free block->data at %x.\n",block->data);
 }
 free(block->data);
 if(verbose){
   printf("block_free:  free block struct at %x.\n",block);
 }
 free(block);
 return(1);

}

