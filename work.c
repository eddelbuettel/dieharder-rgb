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
   diehard_runs();
   diehard_birthdays();
   diehard_2dsphere();
   diehard_3dsphere();
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
     break;
   case DIEHARD_BDAY:
     diehard_birthdays();
     break;
   case DIEHARD_2DSPHERE:
     diehard_2dsphere();
     break;
   case DIEHARD_3DSPHERE:
     diehard_3dsphere();
     break;
 }

 switch(rgb){
   default:
     break;
   case RGB_PERSIST:
     rgb_persist();
     break;
   case RGB_BITDIST:
     rgb_bitdist();
     break;
 }

 switch(sts){
   default:
     break;
   case STS_MONOBIT:
     sts_monobit();
     break;
   case STS_RUNS:
     sts_runs();
     break;
 }

}
