/*
 *========================================================================
 * $Id: dieharder.h 176 2006-07-11 21:18:27Z rgb $
 *
 * file_input (GSL compatible).
 * 
 * By Daniel Summerhays
 * Mar. 10, 2005
 *
 * Heavily modifed by rgb, June-July 2006
 * 
 * See copyright in copyright.h and the accompanying file COPYING
 *========================================================================
 */

#include "dieharder.h"

/*
 * This is a wrapper for getting random numbers from a file.  OH MY
 * have we been screwing up the use of state.  Note CAREFULLY how
 * we must proceed to access the state variables inside of a given
 * rng.
 */

/* static unsigned long int file_input_get (gsl_rng *rng); */
static unsigned long int file_input_get (void *vstate);
static double file_input_get_double (void *vstate);
static void file_input_set (void *vstate, unsigned long int s);

/*
 * This struct contains the data maintained on the operation of
 * the file_input rng.
 *  fp is the file pointer
 *  filenumtype is a character used to determine how to read it
 *     (snitched from the associated fscanf type, basically)
 *  rptr is a count of rands returned since last rewind
 *  rtot is a count of rands returned since the file was opened
 *  rewind_cnt is a count of how many times the file was rewound
 *     since its last open.
 */
typedef struct
  {
    FILE *fp;
    uint rptr;
    uint rtot;
    uint rewind_cnt;
  } file_input_state_t;

uint
file_input_get_rewind_cnt(gsl_rng *rng)
{
  file_input_state_t *state = (file_input_state_t *) rng->state;
  return state->rewind_cnt;
}

uint
file_input_get_rtot(gsl_rng *rng)
{
  file_input_state_t *state = (file_input_state_t *) rng->state;
  return state->rtot;
}

void
file_input_set_rtot(gsl_rng *rng,uint value)
{
  file_input_state_t *state = (file_input_state_t *) rng;
  state->rtot = 0;
}

static unsigned long int
file_input_get (void *vstate)
{
  file_input_state_t *state = (file_input_state_t *) vstate;
  int i;
  unsigned long int j,nmask = 0xFFFFFFFF;
  double f;
  char inbuf[K]; /* input buffer */
  if(state->fp != NULL) {
    /*
     * Read in the next random number from the file
     */
    if(fgets(inbuf,K,state->fp) == 0){
      fprintf(stderr,"# file_input(): Error: EOF on %s\n",filename);
      exit(0);
    }
    /*
     * Increment the counter of rands read so far.
     */
    state->rptr++;
    state->rtot++;

    /*
     * Convert the STRING input above into a uint according to
     * the "type" (basically matching scanf type).
     */
    switch(filetype){
      /*
       * 32 bit unsigned int by assumption
       */
      case 'd':
      case 'i':
      case 'u':
        if(0 == sscanf(inbuf,"%lu",&j)){
          fprintf(stderr,"Error: converting %s failed.  Exiting.\n", inbuf);
          exit(0);
        }
        break;
      /*
       * double precision floats get converted to 32 bit uint
       */
      case 'e':
      case 'E':
      case 'f':
      case 'F':
      case 'g':
        if(0 == sscanf(inbuf,"%g",&f)){
          fprintf(stderr,"Error: converting %s failed.  Exiting.\n", inbuf);
          exit(0);
        }
	j = (uint) f*UINT_MAX;
        break;
      /*
       * OK, so octal is really pretty silly, but we got it.  Still uint.
       */
      case 'o':
        if(0 == sscanf(inbuf,"%lo",&j)){
          fprintf(stderr,"Error: converting %s failed.  Exiting.\n", inbuf);
          exit(0);
        }
        break;
      /*
       * hexadecimal is silly too, but we got it.  uint, of course.
       */
      case 'x':
        if(0 == sscanf(inbuf,"%lx",&j)){
          fprintf(stderr,"Error: converting %s failed.  Exiting.\n", inbuf);
          exit(0);
        }
        break;
      case 'X':
        if(0 == sscanf(inbuf,"%lX",&j)){
          fprintf(stderr,"Error: converting %s failed.  Exiting.\n", inbuf);
          exit(0);
        }
        break;
      /*
       * binary is NOT so silly.  Let's do it.  The hard way.  A typical
       * entry should look like:
       *    01110101001010100100111101101110
       */
      case 'b':
        j = bit2uint(inbuf,filenumbits);
	/* Debugging cruft
	printf("j = %u: binary = ",j);
	dumpbits((uint *)&j,32);
	*/
        break;
    }
    if(verbose){
       fprintf(stdout,"# file_input() %u: %u/%u -> %u\n",state->rtot,state->rptr,filecount,j);
    }
    /*
     * This (with seed s == 0) basically rewinds the file and resets
     * state->rptr to 0, but rtot keeps running,
     */
    if(state->rptr == filecount) {
      /*
       * Should be resetting/rewinding files
       */
      file_input_set(vstate, 0);
    }
    return j;
  } else {
    fprintf(stderr,"Error: %s not open.  Exiting.\n", filename);
    exit(0);
  }

}

static double
file_input_get_double (void *vstate)
{
  return file_input_get (vstate) / (double) UINT_MAX;
}


/*
 * file_input_set() is not yet terriby robust.  For example, it
 * cannot cope with missing header info, duplicate header info,
 * impossible header info.  It should work, though, for a well-formed
 * header
 */

static void
file_input_set (void *vstate, unsigned long int s)
{

 int i;  /* loop variable */
 int cnt;
 int numfields;
 char inbuf[K]; /* input buffer */
 file_input_state_t *state = (file_input_state_t *) vstate;

 if(verbose == D_FILE_INPUT || verbose == D_ALL){
   fprintf(stdout,"# file_input(): entering file_input_set\n");
   fprintf(stdout,"# file_input(): state->fp = %0x, seed = %lu\n",state->fp,s);
 }

 /*
  * We use the "seed" to determine whether or not to reopen or
  * rewind.  A seed s == 0 for an open file means rewind; a seed
  * of anything else forces a close (resetting rewind_cnt) followed
  * by a reopen.
  */
 if(state->fp && s ) {
   if(verbose == D_FILE_INPUT || verbose == D_ALL){
     fprintf(stdout,"# file_input(): Closing/reopening/resetting %s\n",filename);
   }
   fclose(state->fp);
   state->fp = NULL;
 }

 if (state->fp == NULL){
   if(verbose == D_FILE_INPUT || verbose == D_ALL){
     fprintf(stdout,"# file_input(): Opening %s\n", filename);
   }
   /* If never opened, open */
   if ((state->fp = fopen(filename,"r")) == NULL) {
     fprintf(stderr,"Error: Cannot open %s, exiting.\n", filename);
     exit(0);
   }
   if(verbose == D_FILE_INPUT || verbose == D_ALL){
     fprintf(stdout,"# file_input(): Opened %s for the first time at %x\n", filename,state->fp);
     fprintf(stdout,"# file_input(): state->fp is %08x\n",state->fp);
     fprintf(stdout,"# file_input(): Parsing header:\n");
   }
   state->rptr = 0;  /* No rands read yet */
   if(s) {
     state->rtot = 0;  /* Only set this if it is a reseed */
   }
   state->rewind_cnt = 0;  /* No rewinds yet */
 } else {
   /*
    * Rewinding is a problem.  Rather, it is easy, but it seriously
    * reduces the size of the space being explored.  We rewind every
    * time our file pointer reaches the end of the file, otherwise
    * we do not actually reopen the file (and of course seed is
    * ignored).
    */
   if(state->rptr >= filecount){
     rewind(state->fp);
     state->rptr = 0;
     state->rewind_cnt++;
     if(verbose == D_FILE_INPUT || verbose == D_ALL){
       fprintf(stderr,"# file_input(): Rewinding %s at rtot = %u\n", filename,state->rtot);
       fprintf(stderr,"# file_input(): Rewind count = %u, resetting rptr = %lu\n",state->rewind_cnt,state->rptr);
     }
   } else {
     return;
   }
 }

 /*
  * We MUST have precisely three data lines at the beginning after
  * any comments.
  */
 cnt = 0;
 while(cnt < 3){
   if(state->fp != NULL) {
     if(fgets(inbuf,K,state->fp) == 0){
       fprintf(stderr,"# file_input(): Error: EOF on %s\n",filename);
       exit(0);
     }
   }
   if(verbose){
     fprintf(stdout,"%d: %s",cnt,inbuf);
   }

   /*
    * Skip comments altogether, whereever they might be.  Also adopt code
    * to use new, improved, more portable "split()" command.
    */
   if(inbuf[0] != '#'){
     /*
      * Just like perl, sorta.  In fact, I'm really liking using
      * perl-derived utility functions for parsing where I can.
      */
     chop(inbuf);
     numfields = split(inbuf);
     if(numfields != 2){
       fprintf(stderr,"# file_input(): Error: Wrong number of fields: format is 'fieldname: value'\n");
       exit(0);
     }
     if(strncmp(splitbuf[0],"type",4) == 0){
       filetype = splitbuf[1][0];
       cnt++;
       if(verbose){
         fprintf(stdout,"# file_input(): cnt = %d\n",cnt);
         fprintf(stdout,"# file_input(): filenumtype set to %c\n",filetype);
       }
     }
     if(strncmp(splitbuf[0],"count",5) == 0){
       filecount = atoi(splitbuf[1]);
       cnt++;
       if(verbose){ 
         fprintf(stdout,"# file_input(): cnt = %d\n",cnt);
         fprintf(stdout,"# file_input(): filecount set to %d\n",filecount);
       }
     }
     if(strncmp(splitbuf[0],"numbit",6) == 0){
       filenumbits = atoi(splitbuf[1]);
       cnt++;
       if(verbose){ 
         fprintf(stdout,"# file_input(): cnt = %d\n",cnt);
         fprintf(stdout,"# file_input(): filenumbits set to %i\n",filenumbits);
       }
     }
   }
 }

 return;

}

static const gsl_rng_type file_input_type =
{"file_input",                        /* name */
 UINT_MAX,                    /* RAND_MAX */
 0,                           /* RAND_MIN */
 sizeof (file_input_state_t),
 &file_input_set,
 &file_input_get,
 &file_input_get_double };

const gsl_rng_type *gsl_rng_file_input = &file_input_type;
