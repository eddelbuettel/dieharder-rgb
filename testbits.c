/*
 *========================================================================
 * $Id: startup.c 242 2006-10-04 05:54:56Z rgb $
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *========================================================================
 */

/*
 *========================================================================
 * This routine does all the required initialization and startup,
 * including memory allocation and prefilling of vectors.  It is
 * COMPLETELY outside the timing loops.
 *========================================================================
 */

#include "dieharder.h"

void testbits()
{

 uint input[4],output[4];
 uint tuple,off;
 input[0] = 0x88888888;
 dumpbits(&input[0],32);
 input[1] = 0xaaaaaaaa;
 dumpbits(&input[1],32);
 input[2] = 0xcccccccc;
 dumpbits(&input[2],32);
 input[3] = 0xffffffff;
 dumpbits(&input[3],32);
 memset(output,0,16);

 /*
  * The following are different tests we need to run to validate
  * get_ntuple_cyclic (which is a bitch to code).
  */
 /* Get exactly one line.  Terminates rule 2a */ 
 tuple = 32;
 off = 0;
 printf("Getting a %d-tuple from offset = %d\n",tuple,off);
 get_ntuple_cyclic(input,4,output,4,tuple,off);
 dumpbits(&output[0],32);
 dumpbits(&output[1],32);
 dumpbits(&output[2],32);
 dumpbits(&output[3],32);

 /* Terminates rule 1a */ 
 tuple = 24;
 off = 0;
 printf("Getting a %d-tuple from offset = %d\n",tuple,off);
 get_ntuple_cyclic(input,4,output,4,tuple,off);
 dumpbits(&output[0],32);
 dumpbits(&output[1],32);
 dumpbits(&output[2],32);
 dumpbits(&output[3],32);

 /*
  * The following are different tests we need to run to validate
  * get_ntuple_cyclic (which is a bitch to code).
  */
 /* Terminates rule 2a */ 
 tuple = 24;
 off = 8;
 printf("Getting a %d-tuple from offset = %d\n",tuple,off);
 get_ntuple_cyclic(input,4,output,4,tuple,off);
 dumpbits(&output[0],32);
 dumpbits(&output[1],32);
 dumpbits(&output[2],32);
 dumpbits(&output[3],32);

 /*
  * The following are different tests we need to run to validate
  * get_ntuple_cyclic (which is a bitch to code).
  */
 /* Terminates rule 1a (interior) */ 
 tuple = 16;
 off = 8;
 printf("Getting a %d-tuple from offset = %d\n",tuple,off);
 get_ntuple_cyclic(input,4,output,4,tuple,off);
 dumpbits(&output[0],32);
 dumpbits(&output[1],32);
 dumpbits(&output[2],32);
 dumpbits(&output[3],32);

 /* Terminates rule 2c -- one fill from rule R, then 2c */
 tuple = 24;
 off = 24;
 printf("Getting a %d-tuple from offset = %d\n",tuple,off);
 get_ntuple_cyclic(input,4,output,4,tuple,off);
 dumpbits(&output[0],32);
 dumpbits(&output[1],32);
 dumpbits(&output[2],32);
 dumpbits(&output[3],32);

 /* Terminates rule 1 -- alternate R-L R-L R-L 1 */
 tuple = 120;
 off = 0;
 printf("Getting a %d-tuple from offset = %d\n",tuple,off);
 get_ntuple_cyclic(input,4,output,4,tuple,off);
 dumpbits(&output[0],32);
 dumpbits(&output[1],32);
 dumpbits(&output[2],32);
 dumpbits(&output[3],32);

 /* Terminates rule 2b -- R R R 2b */
 tuple = 120;
 off = 8;
 printf("Getting a %d-tuple from offset = %d\n",tuple,off);
 get_ntuple_cyclic(input,4,output,4,tuple,off);
 dumpbits(&output[0],32);
 dumpbits(&output[1],32);
 dumpbits(&output[2],32);
 dumpbits(&output[3],32);

 /* Terminates rule 2c, cycli -- one fill from rule R, then 2c */
 tuple = 24;
 off = 120;
 printf("Getting a %d-tuple from offset = %d\n",tuple,off);
 get_ntuple_cyclic(input,4,output,4,tuple,off);
 dumpbits(&output[0],32);
 dumpbits(&output[1],32);
 dumpbits(&output[2],32);
 dumpbits(&output[3],32);

 /* Terminates rule 1, but wraps cyclic -- R L R L R L 1 */
 tuple = 120;
 off = 32;
 printf("Getting a %d-tuple from offset = %d\n",tuple,off);
 get_ntuple_cyclic(input,4,output,4,tuple,off);
 dumpbits(&output[0],32);
 dumpbits(&output[1],32);
 dumpbits(&output[2],32);
 dumpbits(&output[3],32);
 exit(0);

 /* Terminates rule 2b, cyclic -- R R R 2b */
 tuple = 120;
 off = 40;
 printf("Getting a %d-tuple from offset = %d\n",tuple,off);
 get_ntuple_cyclic(input,4,output,4,tuple,off);
 dumpbits(&output[0],32);
 dumpbits(&output[1],32);
 dumpbits(&output[2],32);
 dumpbits(&output[3],32);

}
