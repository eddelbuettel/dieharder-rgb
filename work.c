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
   case RGB_BINOMIAL:
     rgb_binomial();
     break;
   case STS_MONOBIT:
     sts_monobit();
     break;
   case STS_RUNS:
     sts_runs();
     break;
 }

}
