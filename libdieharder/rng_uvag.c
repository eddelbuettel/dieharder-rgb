/*
 *  rng_uvag.c
 * 
 *  Copyright(c) 2004 by Alex Hay - zenjew@hotmail.com
 *
 *  From:
 *
 *      Private communication, 6/5/07.
 *
 *  GSL packaged for dieharder by Robert G. Brown 6/5/07
 *
 *  Alex has agree to make this available under the GSL so it can be
 *  integrated with the dieharder package.
 *
 *  Cellular automaton random number generator
 *  Uses a 256-state automaton to generate random sequences of
 *  32-bit unsigned integers.
 *
 *========================================================================
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <dieharder/libdieharder.h>

/*
 * This is a wrapping of the uvag rng
 */

static unsigned long int uvag_get (void *vstate);
static double uvag_get_double (void *vstate);
static void uvag_set (void *vstate, unsigned long int s);

typedef struct
  {
  }
uvag_state_t;

/*
 * UVAG specific defines.
 *
 * UVAG provides globally as *rndint, an effectively infinite sequence of
 * TYPEs, uniformly distributed (0, 2**(8*sizeof(TYPE))-1).  We need to
 * return uints in our GSL-compatible wrapper, at least at the moment.
 */

/* #define TYPE long long || char || short || int || long */
#define TYPE uint
#define WORD sizeof(TYPE)
#define BYTESOUT 11000000

/*
 * Global variables and data for UVAG
 */

TYPE *rp, rndint;
unsigned char sindex, svec[255 + WORD];  /* 256 overlapping TYPE seeds */

static inline unsigned long int
uvag_get (void *vstate)
{
  /*
   * Returns a 32-bit unsigned integer produced by the UVAG
   */
  rp = svec + svec[sindex+=1];
  *rp += rndint;
  rndint += *rp;
  return rndint;
/*  rndint += *rp += rndint;
  return *rndint; */

}

static double
uvag_get_double (void *vstate)
{
  return uvag_get (vstate) / (double) UINT_MAX;
}

static void
uvag_set (void *vstate, unsigned long int s) {

 /* Initialize automaton using specified seed. */
 uvag_state_t *state = (uvag_state_t *) vstate;
 
 uint i, array_len = 255 + WORD, tot, seed_seed, tmp8;
 unsigned char key[256], *kp, temp;
 gsl_rng *seed_rng;    /* random number generator used to seed uvag */

 /*
  * Preload the array with 1-byte integers
  */
 for (i=0; i<array_len; i++){
   svec[i]=i;
 }

 /*
  * OK, here we have to modify Alex's algorithm.  The GSL requires a
  * single seed, unsigned long int in type.  Alex requires a key 
  * string 256 characters long (that is, 64 uints long).  We therefore
  * have to bootstrap off of an existing, deterministic GSL RNG to fill
  * the seed string from a single permitted seed.  Note that type 12 is
  * the mt19937 generator, basically one of the best in the world -- not
  * that it matters.
  */
 seed_rng = gsl_rng_alloc(types[12]);
 seed_seed = s;
 gsl_rng_set(seed_rng,seed_seed);
 random_max = gsl_rng_max(seed_rng);
 rmax = random_max;
 rmax_bits = 0;
 rmax_mask = 0;
 while(rmax){
   rmax >>= 1;
   rmax_mask = rmax_mask << 1;
   rmax_mask++;
   rmax_bits++;
 }
 for(i=0;i<256;i++){
   /* if(i%32 == 0) printf("\n"); */
   get_rand_bits(&tmp8,sizeof(uint),8,seed_rng);
   if(i!=255){
     key[i] = tmp8;
   } else {
     key[i] = 0;
   }
   /* printf("%02x",key[i]); */
 }
 /* printf("\n"); */

 kp = key;
 tot = 0;
 for(i=0; i<array_len; i++) {        /* shuffle seeds */
   tot += *kp++;
   tot%=array_len;
   temp = svec[tot];
   svec[tot] = svec[i];
   svec[i] = temp;
   /* wrap around key[] */
   if(*kp == '\0') {
     kp = key;
   }
 }
/* For debugging of the original load'n'shuffle
 printf("svec = ");
 for(i=0;i<256;i++){
   if(i%32 == 0) printf("\n");
   printf("%02x|",svec[i]);
 }
 printf("\n");
 */

 sindex = 0;
 rndint = 0;

}

static const gsl_rng_type uvag_type =
{"uvag",                          /* name */
 UINT_MAX,			/* RAND_MAX */
 0,				/* RAND_MIN */
 sizeof (uvag_state_t),
 &uvag_set,
 &uvag_get,
 &uvag_get_double};

const gsl_rng_type *gsl_rng_uvag = &uvag_type;
