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
  unsigned long int j;
  if(state->fp != NULL) {
    switch(state->filenumtype){
      case 'd':
        if(0 == fscanf(state->fp,"%lu",&j)){
              fprintf(stderr,"Error: reading %s failed.  Exiting.\n", filename);
              exit(0);
        }
	state->rptr++;
        break;
      case 'o':
        if(0 == fscanf(state->fp,"%lo",&j)){
              fprintf(stderr,"Error: reading %s failed.  Exiting.\n", filename);
              exit(0);
        }
	state->rptr++;
        break;
      case 'x':
        if(0 == fscanf(state->fp,"%lx",&j)){
              fprintf(stderr,"Error: reading %s failed.  Exiting.\n", filename);
              exit(0);
        }
	state->rptr++;
        break;
      case 'b':
        /* This case is incomplete. */
        fprintf(stderr,"Warning: binary integer file reading not implemented\n");
        exit(0);
	state->rptr++;
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
     fprintf(stdout,"Opened %s for the first time at %x\n", filename,state->fp);
   }
   state->rptr = 0;  /* Point this at the first record */
 } else {
   /*
    * Rewinding is a problem.  Rather, it is easy, but it seriously
    * reduces the size of the space being explored.  Basically, we
    * absolutely need to compute how many rands we will need and
    * bitch like hell the FIRST time we run out in a test.  For
    * the moment we just do it, though -- complaining comes later.
    */
   if(state->rptr >= filecount){
     rewind(state->fp);
     fprintf(stdout,"# file_input(): rptr = %lu\n",state->rptr);
     state->rptr = 0;
     fprintf(stdout,"# file_input() Warning: Rewound %s, set rptr = %lu\n", filename,state->rptr);
   } else {
     return;
   }
 }

 if(verbose){
    fprintf(stdout,"# file_input(): state->fp is %08x\n",state->fp);
    fprintf(stdout,"# file_input(): Opened %s.\n", filename);
    fprintf(stdout,"# file_input(): Parsing header:\n");
 }
 /*
  * We MUST have precisely three data lines at the beginning after
  * any comments.
  */
 cnt = 0;
 while(cnt < 3){
   if(state->fp != NULL) {
     fgets(inbuf,K,state->fp);
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
