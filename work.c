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

/*
 if(output == YES){
   output_rnds();
 }
 */

 if(all == YES){
   rgb_timing();
   rgb_persist();
   rgb_bitdist();
   diehard_birthdays();
   diehard_operm5();
   diehard_rank_32x32();
   diehard_rank_6x8();
   diehard_bitstream();
   diehard_opso();
   diehard_oqso();
   diehard_dna();
   diehard_count_1s_stream();
   diehard_count_1s_byte();
   diehard_parking_lot();
   diehard_2dsphere();
   diehard_3dsphere();
   diehard_squeeze();
   diehard_sums();
   diehard_runs();
   diehard_craps();
   marsaglia_tsang_gcd();
   marsaglia_tsang_gorilla();
   sts_monobit();
   sts_runs();
   user_template();
   exit(0);
 }

 switch(diehard){
   case DIEHARD_BDAY:
     diehard_birthdays();
     exit(0);
     break;
   case DIEHARD_OPERM5:
     diehard_operm5();
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
   case DIEHARD_BITSTREAM:
     diehard_bitstream();
     exit(0);
     break;
   case DIEHARD_OPSO:
     diehard_opso();
     exit(0);
     break;
   case DIEHARD_OQSO:
     diehard_oqso();
     exit(0);
     break;
   case DIEHARD_DNA:
     diehard_dna();
     exit(0);
     break;
   case DIEHARD_COUNT_1S_STREAM:
     diehard_count_1s_stream();
     exit(0);
     break;
   case DIEHARD_COUNT_1S_BYTE:
     diehard_count_1s_byte();
     exit(0);
     break;
   case DIEHARD_PARKING_LOT:
     diehard_parking_lot();
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
   case DIEHARD_SQUEEZE:
     diehard_squeeze();
     exit(0);
     break;
   case DIEHARD_SUMS:
     diehard_sums();
     exit(0);
     break;
   case DIEHARD_RUNS:
     diehard_runs();
     exit(0);
     break;
   case DIEHARD_CRAPS:
     diehard_craps();
     exit(0);
     break;
   case MARSAGLIA_TSANG_GCD:
     marsaglia_tsang_gcd();
     exit(0);
     break;
   case MARSAGLIA_TSANG_GORILLA:
     marsaglia_tsang_gorilla();
     exit(0);
     break;
   default:
     break;
 }

 switch(rgb){
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
   case RGB_LMN:
     rgb_lmn();
     exit(0);
     break;
   default:
     break;
 }

 switch(sts){
   case STS_MONOBIT:
     sts_monobit();
     exit(0);
     break;
   case STS_RUNS:
     sts_runs();
     exit(0);
     break;
   default:
     break;
 }

 switch(user){
   case USER_TEMPLATE:
     user_template();
     exit(0);
     break;
   default:
     break;
 }

 list_rngs();

}
