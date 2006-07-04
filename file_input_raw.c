/*
 * file_input_raw
 * 
 * See copyright in copyright.h and the accompanying file COPYING
 *
 */

#include "dieharder.h"

/*
 * This is a wrapper for getting random numbers in RAW (binary integer)
 * format from a file.  A raw file has no headers -- it is presumed to be
 * a pure bit stream.  We therefore have to read it in a page at a time,
 * realloc its required storage as needed, and count as we go.  In this
 * way we can figure out if e.g. a compressed file is sufficiently
 * "random" to make it likely that the compression is good and so on.
 */

static unsigned long int file_input_raw_get (void *vstate);
static double file_input_raw_get_double (void *vstate);
static void file_input_raw_set (void *vstate, unsigned long int s);

typedef struct
  {
    FILE *fp;
    unsigned long int flen;
    unsigned long int rptr;
  } file_input_raw_state_t;

static unsigned long int
file_input_raw_get (void *vstate)
{

 file_input_raw_state_t *state = (file_input_raw_state_t *) vstate;
 static unsigned int nread=0,cnt=0;
 unsigned int iret,inbuf[K];

 /*
  * Check that the file is opened by file_input_raw_set().
  */
 if(state->fp != NULL) {

   /*
    * It is time to read in a new block of raw uints. n tells us how many
    * uints we actually got.
    */
   if((state->rptr % K) == 0){

     /*
      * Que suck!  We have to FIRST try to read in a new block.
      * This may work, or it may not.  If it doesn't work (returns
      * zero) THEN we test for EOF, rewind, and have to call
      * nread again.  We manage it with a while loop, which
      * (alas) has a few ways to become infinite...
      */
     while( (nread = fread(inbuf,sizeof(uint),K,state->fp)) == 0){
       if(feof(state->fp)){
         /* printf("state->rptr = %u at EOF\n",state->rptr); */
         file_input_raw_set(vstate, 0);
         if(state->flen == 0){
           state->flen = cnt;
           if(verbose){
             printf("# file_input_raw(): There are %u rands in file\n",state->flen);
	   }
	 }
       } else {
         fprintf(stderr,"# file_input_raw(): Error.  This cannot happen.\n");
	 exit(0);
       }
     }

     /*
      * We just read in 0 < n <= K records, so we increment cnt, but only
      * if state->flen still is zero (the first time we count).
      */
     if(state->flen == 0) cnt += nread;
     
   }

   /*
    * OK if we get here, the file is open, the buffer contains at least
    * one unread record and we're not at EOF.  So we prepare to return
    * inbuf[state->rptr] and then do some thinking about what to make
    * state->rptr for the next call.
    */
   iret = inbuf[state->rptr];
   /* fprintf(stdout,"# file_input_raw(): %lu -> %lu\n",state->rptr,iret); */
   if(verbose){
     fprintf(stdout,"# file_input_raw(): %lu -> %lu\n",state->rptr,iret);
   }

   /*
    * Increment state->rptr.  If the INCREMENTED one is K, no problem,
    * the next call will trigger the next buffer.  If the INCREMENTED
    * one is nread, however, we ARE at the EOF (or at any rate the last
    * read was a short one returning less than K records).  We therefore
    * need to set state->rptr to 0 to force the next call to trigger
    * a (presumed) rewind.
    */
    state->rptr++;

    /*
     * Note that nread = K is normal and this works in this case too.
     */
    if(state->rptr == nread) state->rptr = 0;
    if(verbose){
      printf("# file_input_raw(): %u\n",iret);
    }
    return(iret);

 } else {
   fprintf(stderr,"# file_input_raw_get(): Error: %s not open, call file_input_raw_set() first.  Exiting.\n", filename);
   exit(0);
 }

}

static double
file_input_raw_get_double (void *vstate)
{
  return file_input_raw_get (vstate) / (double) UINT_MAX;
}


/*
 * file_input_raw_set() is very simple.  If the file hasn't been opened
 * yet, it opens it and sets flen and rptr to zero.  Otherwise it
 * rewinds it and sets rptr to zero.  Typically it is only called one
 * time per file by the user, although it will be called once per read
 * page by file_input_raw_get().
 */

static void
file_input_raw_set (void *vstate, unsigned long int s)
{

 if(verbose){
    fprintf(stdout,"entering file_input_raw_set\n");
 }

 file_input_raw_state_t *state = (file_input_raw_state_t *) vstate;

 /*
  * Clear this pointer.  This will force a reread of the first data
  * block on a rewind.
  */
 state->rptr = 0;
 
 if (state->fp == NULL){
   /* If never opened, open */
   if ((state->fp = fopen(filename,"r")) == NULL) {
     fprintf(stderr,"Error: Cannot open %s, exiting.\n", filename);
     exit(0);
   }
   if(verbose){
     fprintf(stdout,"# file_input_raw(): Opened %s for the first time.\n", filename);
     fprintf(stdout,"# file_input_raw(): state->fp is %08x\n",state->fp);
   }
 } else {
   rewind(state->fp);
   if(verbose){
     fprintf(stdout,"# file_input_raw():  Rewinding %s, resetting rptr = %lu\n", filename,state->rptr);
   }
 }

}

static const gsl_rng_type file_input_raw_type =
{"file_input_raw",                        /* name */
 UINT_MAX,                    /* RAND_MAX */
 0,                           /* RAND_MIN */
 sizeof (file_input_raw_state_t),
 &file_input_raw_set,
 &file_input_raw_get,
 &file_input_raw_get_double};

const gsl_rng_type *gsl_rng_file_input_raw = &file_input_raw_type;
