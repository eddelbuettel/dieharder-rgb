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

#include "rand_rate.h"

void rand_rate_work()
{

 if(testnum<0){
   measure_rate();
   diehard_runs();
   rgb_persist();
   rgb_binomial();
   rgb_bitdist();
   sts_runs();
   exit(0);
 }

 switch(testnum){
   default:
   case LIST_RNGS:
     list_rngs();
     break;
   case LIST_RAND:
     list_rand();
     break;
   case BOGORATE:
     measure_rate();
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
   case RGB_PERSIST:
     rgb_persist();
     break;
   case RGB_BINOMIAL:
     rgb_binomial();
     break;
   case RGB_BIT2:
     rgb_bit2();
     break;
   case RGB_BITDIST:
     rgb_bitdist();
     break;
   case STS_MONOBIT:
     sts_monobit();
     break;
   case STS_RUNS:
     sts_runs();
     break;
 }

}
