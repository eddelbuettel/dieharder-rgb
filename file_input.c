/*
 * file_input
 * 
 * By Daniel Summerhays
 * Mar. 10, 2005
 * 
 * See copyright in copyright.h and the accompanying file COPYING
 *
 */

#include "dieharder.h"

/*
 * This is a wrapper for getting random numbers from a file
 */

static unsigned long int file_input_get (void *vstate);
static double file_input_get_double (void *vstate);
static void file_input_set (void *vstate, unsigned long int s);

typedef struct
  {
    FILE *fp;
    char filenumtype;
    unsigned long int rptr;
  } file_input_state_t;

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
     * Increment the pointer/count of rands read so far.
     */
    state->rptr++;

    /*
     * Convert the STRING input above into a uint according to
     * the "type" (basically matching scanf type).
     */
    switch(state->filenumtype){
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
       fprintf(stdout,"# file_input(): %lu -> %lu\n",state->rptr,j);
    }
    /*
     * This basically rewinds the file and resets state->rptr to 0
     */
    if(state->rptr == filecount) file_input_set(vstate, 0);
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
 if(verbose){
    fprintf(stdout,"entering file_input_set\n");
 }

 file_input_state_t *state = (file_input_state_t *) vstate;
 
 if (state->fp == NULL){
   /* If never opened, open */
   if ((state->fp = fopen(filename,"r")) == NULL) {
     fprintf(stderr,"Error: Cannot open %s, exiting.\n", filename);
     exit(0);
   }
   if(verbose){
     fprintf(stdout,"# file_input(): Opened %s for the first time at %x\n", filename,state->fp);
     fprintf(stdout,"# file_input(): state->fp is %08x\n",state->fp);
     fprintf(stdout,"# file_input(): Parsing header:\n");
   }
   state->rptr = 0;  /* Point this at the first record */
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
     fprintf(stderr,"# file_input(): Warning rptr = %lu equals filecount\n",state->rptr);
     state->rptr = 0;
     fprintf(stderr,"# file_input():   Rewinding %s, resetting rptr = %lu\n", filename,state->rptr);
     fprintf(stderr,"# file_input(): Parsing header:\n");
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
       fprintf(stderr,"Error: Wrong number of fields: format is 'fieldname: value'\n");
       exit(0);
     }
     if(strncmp(splitbuf[0],"type",4) == 0){
       state->filenumtype = splitbuf[1][0];
       cnt++;
       if(verbose){
         fprintf(stdout,"#file_input(): cnt = %d\n",cnt);
         fprintf(stdout,"#file_input(): filenumtype set to %c\n",state->filenumtype);
       }
     }
     if(strncmp(splitbuf[0],"count",5) == 0){
       filecount = atoi(splitbuf[1]);
       cnt++;
       if(verbose){ 
         fprintf(stdout,"#file_input(): cnt = %d\n",cnt);
         fprintf(stdout,"filecount set to %i\n",filecount);
       }
     }
     if(strncmp(splitbuf[0],"numbit",6) == 0){
       filenumbits = atoi(splitbuf[1]);
       cnt++;
       if(verbose){ 
         fprintf(stdout,"#file_input(): cnt = %d\n",cnt);
         fprintf(stdout,"filenumbits set to %i\n",filenumbits);
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
 &file_input_get_double};

const gsl_rng_type *gsl_rng_file_input = &file_input_type;
