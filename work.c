/*
 *========================================================================
 * $Id$
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *========================================================================
 */

/*
 *========================================================================
 * This should be a nice, big case switch where we add EACH test
 * we might want to do and either just configure and do it or
 * prompt for input (if absolutely necessary) and then do it.
 *========================================================================
 */

#include "dieharder.h"

void work()
{

 if(all == YES){
   rgb_timing();
   diehard_runs();
   diehard_birthdays();
   diehard_2dsphere();
   diehard_3dsphere();
   diehard_rank_32x32();
   diehard_rank_6x8();
   rgb_persist();
   rgb_bitdist();
   sts_monobit();
   sts_runs();
   exit(0);
 }

 switch(diehard){
   default:
     break;
   case DIEHARD_RUNS:
     diehard_runs();
     exit(0);
     break;
   case DIEHARD_BDAY:
     diehard_birthdays();
     exit(0);
     break;
   case DIEHARD_2DSPHERE:
     diehard_2dsphere();
     exit(0);
     break;
   case DIEHARD_3DSPHERE:
     diehard_3dsphere();
     exit(0);
     break;
   case DIEHARD_RANK_32x32:
     diehard_rank_32x32();
     exit(0);
     break;
   case DIEHARD_RANK_6x8:
     diehard_rank_6x8();
     exit(0);
     break;
 }

 switch(rgb){
   default:
     break;
   case RGB_TIMING:
     rgb_timing();
     exit(0);
     break;
   case RGB_PERSIST:
     rgb_persist();
     exit(0);
     break;
   case RGB_BITDIST:
     rgb_bitdist();
     exit(0);
     break;
 }

 switch(sts){
   default:
     break;
   case STS_MONOBIT:
     sts_monobit();
     exit(0);
     break;
   case STS_RUNS:
     sts_runs();
     exit(0);
     break;
 }

 list_rngs();

}
