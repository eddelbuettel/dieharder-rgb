/*
 *  rng_aes.c
 * 
 *  Copyright 2009 by Robert G. Brown rgb at phy dot duke dot edu
 *
 *  This rng is derived from Jari Ruusu's aespipe, itself built on top
 *  of Brian Gladman's set of aes utilities.  I wrapped the utilities
 *  into a portable link library and contributed the whole thing back
 *  to Jari, where I hope he contemplates adopting this packaging for
 *  his project for the indefinite future, in which case I can just
 *  install libaes as a dependency.
 *
 * AES stands for Advanced Encryption Standard and is a block cipher
 * approved for general cryptographic use by NIST and FIPS.  AES-256 is
 * approved by the government for encrypting top secret documents.
 *
 * Like many encryption programs, AES can be used as a rather good
 * random number generator.  Perfectly encrypted text is by definition
 * perfectly random; there are NO meaningful patterns that can be abstracted
 * from the study of an encrypted file.
 *
 * There are two simple ways to use aes and dieharder together.  One is
 * to use Ruusu's aespipe directly to feed dieharder via its raw interface.
 * That is:
 *
 *    cat /dev/zero | aespipe -P keyfile | dieharder -g 200 -a
 *
 * will encrypt the output of /dev/zero (using a key with at least 20
 * characters in it in keyfile, or entering a key by hand at the
 * command line) and test the result for randomness.  aespipe has
 * further options that control blocksize and keysize.  This method
 * could also be used to test the randomness of an encrypted file,
 * although most files, encrypted, are not long enough to run a full
 * round of dieharder tests on.
 *
 * The second is to use this built-in rng_aes() interface to generate a
 * 256-bit encryption of an >>already random stream<< generated from
 * one of the "good" GSL generators.  In principle, the difference between
 * such a block-cipher encrypted random stream and perfectly random numbers
 * should be very, very difficult to detect.
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
#include <aes/aes.h>
#include <aes/md5.h>
#include <aes/sha512.h>
#include <aes/rmd160.h>



static unsigned long int aes_get (void *vstate);
static double aes_get_double (void *vstate);
static void aes_set (void *vstate, unsigned long int s);

typedef struct
  {
  }
aes_state_t;

/*
 * AES specific defines.  Note that I'm omitting little-endian support.
 * Just don't care.  If your machine is little-endian, shoot it in the
 * head and get a new one.
 */
#define  AES_KEY_LENGTH   32

#if WORDS_BIGENDIAN
# define xcpu_to_le32(x) ({u_int32_t __x=(x);((u_int32_t)((((u_int32_t)(__x)&(u_int32_t)0x000000ffUL)<<24)|(((u_int32_t)(__x)&(u_int32_t)0x0000ff00UL)<<8)|(((u_int32_t)(__x)&(u_int32_t)0x00ff0000UL)>>8)|(((u_int32_t)(__x)&(u_int32_t)0xff000000UL)>>24)));})
#else
# define xcpu_to_le32(x) ((u_int32_t)(x))
#endif

/*
 * Global variables and data for AES
 */
union {
  u_int32_t     hw[16];
  unsigned char hb[4];
} hbu;
aes_context     ctx;
uint iv[4];
uint ringbuf[4];
static int aes_ind = -1;
gsl_rng *seed_rng;    /* random number generator used to seed uvag */
uint retval;

static inline unsigned long int
aes_get (void *vstate)
{

 /*
  * Returns the next 32-bit unsigned integer produced by the AES
  * rng.  Basically, it looks like I need to load iv up with
  * 4 uints of data to be encrypted (from my rng-internal rng),
  * then pass them to aes_encrypt to be encrypted (return in datap?)
  * and then repeat.  Of course if aes_encrypt encrypts in 4x32 = 128
  * bit chunks, I'll have to maintain datap as a ring buffer and
  * only do this work one in four calls.
  */
 if(aes_ind == -1){
   iv[0] = gsl_rng_get(seed_rng);
   iv[1] = gsl_rng_get(seed_rng);
   iv[2] = gsl_rng_get(seed_rng);
   iv[3] = gsl_rng_get(seed_rng);
   aes_encrypt(&ctx, (unsigned char *)(&iv[0]), (unsigned char *)&ringbuf);
 }
 aes_ind++;
 retval = ringbuf[aes_ind];
 if(aes_ind == 3) aes_ind = -1;  /* reload again on next call */
 
 return(retval);

}

static double
aes_get_double (void *vstate)
{
  return aes_get (vstate) / (double) UINT_MAX;
}

static void aes_set (void *vstate, unsigned long int s) {

 /*
  * Here we use a prespecified rng to both seed the aes process
  * (construct the key) and to generate a random stream that is
  * encrypted and made even more random.  This GREATLY REDUCES
  * the size of the key space -- there can obviously be no more
  * than 2^32 distinct seedings in this approach -- but of course
  * the effective period of the result should be nearly infinite
  * for any given seeding since it is at least the product of
  * the periods of the aes process and the underlying rng, and
  * both of those should be extremely large numbers.
  */
 aes_state_t *state = (aes_state_t *) vstate;
 
 uint i, tmp8;
 unsigned char key[AES_KEY_LENGTH], *kp, temp;

 int bits;

 /*
  * We use mt19937_1999
  */
 seed_rng = gsl_rng_alloc(types[14]);
 gsl_rng_set(seed_rng,s);
 for(i=0;i<AES_KEY_LENGTH;i++){
   get_rand_bits(&tmp8,sizeof(uint),8,seed_rng);
   key[i] = tmp8;
 }
 bits = 256;
 sha512_hash_buffer(key,AES_KEY_LENGTH,&hbu.hb[0],32);
 aes_set_key(&ctx, &hbu.hb[0], bits, 0);
 memset(&hbu.hb[0], 0, sizeof(hbu.hb));

}

static const gsl_rng_type aes_type =
{"aes",                          /* name */
 UINT_MAX,			/* RAND_MAX */
 0,				/* RAND_MIN */
 sizeof (aes_state_t),
 &aes_set,
 &aes_get,
 &aes_get_double};

const gsl_rng_type *gsl_rng_aes = &aes_type;
