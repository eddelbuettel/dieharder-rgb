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
    unsigned int flen;
    unsigned int rptr;
  } file_input_raw_state_t;

static unsigned long int
file_input_raw_get (void *vstate)
{

 file_input_raw_state_t *state = (file_input_raw_state_t *) vstate;
 unsigned int nread,iret;

 /*
  * Check that the file is open (via file_input_raw_set()).
  */
 if(state->fp != NULL) {

   /*
    * We'll hope and assume that the file read is already sensibly
    * buffered and just read in the next one and return it.
    */
   if(state->rptr < state->flen){
     if((nread = fread(&iret,sizeof(uint),1,state->fp)) == 1){
       /* Successful read */
       state->rptr++;
       /*
        * Now we KNOW where the last record lies, so we precisely
        * rewind/reset when we've exhausted the file.
        */
       if(state->rptr == state->flen){
         file_input_raw_set(vstate, 0);
       }
       /* printf("%4u/%4u:\t%9u\n",state->rptr,state->flen,iret); */
       return(iret);
     } else {
       fprintf(stderr,"# file_input_raw(): Error.  This cannot happen.\n");
       exit(0);
     }
   }
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

 static uint first=1;
 struct stat sbuf;

 file_input_raw_state_t *state = (file_input_raw_state_t *) vstate;


 /*
  * Clear this pointer.  We always start over when file_input_raw_set() is
  * called.
  */
 /* printf("Setting state->rptr = %4u to zero.\n",state->rptr); */
 state->rptr = 0;
 
 /*
  * Get and set the file length, check to make sure the file exists,
  * whatever...
  */
 if(first){
   if(verbose){
     fprintf(stdout,"# file_input_raw(): entering file_input_raw_set 1st call.\n");
   }

   if(stat(filename, &sbuf)){
     if(errno == EBADF){
       fprintf(stderr,"# file_input_raw(): Error -- file descriptor %s bad.\n",filename);
       exit(0);
     }
   }
   /*
    * Is this a regular file?  If so, turn its byte length into a 32 bit uint
    * length.
    */
   if(S_ISREG(sbuf.st_mode)){
     state->flen = sbuf.st_size/sizeof(uint);
   } else {
     fprintf(stderr,"# file_input_raw(): Error -- path %s not regular file.\n",filename);
     exit(0);
   }

   /*
    * If we get here, the file exists, is a regular file, and we know its
    * length.  We can now open it.  The test catches all other conditions
    * that might keep the file from reading, e.g. permissions.
    */
   if ((state->fp = fopen(filename,"r")) == NULL) {
     fprintf(stderr,"# file_input_raw(): Error: Cannot open %s, exiting.\n", filename);
     exit(0);
   }

   /*
    * OK, so if we get here, the file is open.
    */
   fprintf(stdout,"# file_input_raw(): Opened %s for the first time.\n", filename);
   fprintf(stdout,"# file_input_raw(): state->fp is %08x, file contains %u unsigned integers.\n",state->fp,state->flen);
   if(verbose){
     fprintf(stdout,"# file_input_raw(): Opened %s for the first time.\n", filename);
     fprintf(stdout,"# file_input_raw(): state->fp is %08x, file contains %u unsigned integers.\n",state->fp,state->flen);
   }
   first = 0;
 } else {
   /* printf("Rewinding %s\n",filename); */
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
