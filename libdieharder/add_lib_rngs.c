/*
 *========================================================================
 * $Id$
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *========================================================================
 */

#include <dieharder/libdieharder.h>

void add_lib_rngs()
{

 int i = 0;

 /*
  * Get to the end of types
  */
 while(types[i] != NULL){
   i++;
 }

 /*
  * and add the new ones...
  */

 /* gsl_rng_dev_random */
 types[i] = (gsl_rng_dev_random);
 MYDEBUG(D_ADD_MY_TYPES){
   printf("# add_my_types():  Added type %d = %s\n",i,types[i]->name);
 }
 i++;
 /* gsl_rng_dev_arandom */
 types[i] = (gsl_rng_dev_arandom);
 MYDEBUG(D_ADD_MY_TYPES){
   printf("# add_my_types():  Added type %d = %s\n",i,types[i]->name);
 }
 i++;
 /* gsl_rng_dev_urandom */
 types[i] = (gsl_rng_dev_urandom);
 MYDEBUG(D_ADD_MY_TYPES){
   printf("# add_my_types():  Added type %d = %s\n",i,types[i]->name);
 }
 i++;
 /* gsl_rng_file_input */
 types[i] = (gsl_rng_file_input);
 MYDEBUG(D_ADD_MY_TYPES){
   printf("# add_my_types():  Added type %d = %s\n",i,types[i]->name);
 }
 i++;
 /* gsl_rng_file_input_raw */
 types[i] = (gsl_rng_file_input_raw);
 MYDEBUG(D_ADD_MY_TYPES){
   printf("# add_my_types():  Added type %d = %s\n",i,types[i]->name);
 }
 i++;
 /* gsl_rng_ca */
 types[i] = (gsl_rng_ca);
 MYDEBUG(D_ADD_MY_TYPES){
   printf("# add_my_types():  Added type %d = %s\n",i,types[i]->name);
 }
 i++;
 /* gsl_rng_r_wichmann_hill */
 types[i] = (gsl_rng_r_wichmann_hill);
 MYDEBUG(D_ADD_MY_TYPES){
   printf("# add_my_types():  Added type %d = %s\n",i,types[i]->name);
 }
 i++;
 /* gsl_rng_r_marsaglia_mc */
 types[i] = (gsl_rng_r_marsaglia_mc);
 MYDEBUG(D_ADD_MY_TYPES){
   printf("# add_my_types():  Added type %d = %s\n",i,types[i]->name);
 }
 i++;
 /* gsl_rng_r_super_duper */
 types[i] = (gsl_rng_r_super_duper);
 MYDEBUG(D_ADD_MY_TYPES){
   printf("# add_my_types():  Added type %d = %s\n",i,types[i]->name);
 }
 i++;
 /* gsl_rng_r_mersenne_twister */
 types[i] = (gsl_rng_r_mersenne_twister);
 MYDEBUG(D_ADD_MY_TYPES){
   printf("# add_my_types():  Added type %d = %s\n",i,types[i]->name);
 }
 i++;
 /* gsl_rgb_r_knuth_taocp */
 types[i] = (gsl_rng_r_knuth_taocp);
 MYDEBUG(D_ADD_MY_TYPES){
   printf("# add_my_types():  Added type %d = %s\n",i,types[i]->name);
 }
 i++;
 /* gsl_rng_r_knuth_taocp2 */
 types[i] = (gsl_rng_r_knuth_taocp2);
 MYDEBUG(D_ADD_MY_TYPES){
   printf("# add_my_types():  Added type %d = %s\n",i,types[i]->name);
 }
 i++;
 /* gsl_rng_uvag */
 types[i] = (gsl_rng_uvag);
 MYDEBUG(D_ADD_MY_TYPES){
   printf("# add_my_types():  Added type %d = %s\n",i,types[i]->name);
 }
 i++;
 
}
