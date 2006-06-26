/* file_input
 * 
 * By Daniel Summerhays
 * Mar. 10, 2005
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

#include "dieharder.h"

/*
 * This is a wrapping for random numbers from a file
 */

static unsigned long int file_input_get (void *vstate);
static double file_input_get_double (void *vstate);
static void file_input_set (void *vstate, unsigned long int s);

typedef struct
  {
    FILE *fp;
    char filenumtype;
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
        break;
      case 'o':
        if(0 == fscanf(state->fp,"%lo",&j)){
              fprintf(stderr,"Error: reading %s failed.  Exiting.\n", filename);
              exit(0);
        }
        break;
      case 'x':
        if(0 == fscanf(state->fp,"%lx",&j)){
              fprintf(stderr,"Error: reading %s failed.  Exiting.\n", filename);
              exit(0);
        }
        break;
      case 'b':
        /* This case is incomplete. */
        fprintf(stderr,"Warning: binary integer file reading not implemented\n");
        exit(0);
        break;
    }
    if(verbose){
       fprintf(stdout,"Got int = %lu\n",j);
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

static void
file_input_set (void *vstate, unsigned long int s)
{

 int i;  /* loop variable */
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
   fprintf(stdout,"Opened %s for the first time at %x\n", filename,state->fp);
 } else {
   /* Close, then reopen */
   fclose(state->fp);
   if ((state->fp = fopen(filename,"r")) == NULL) {
     fprintf(stderr,"Error: Cannot open %s, exiting.\n", filename);
     exit(0);
   }
   fprintf(stdout,"Reopened %s at %x\n", filename,state->fp);
 }

 if(verbose){
    fprintf(stdout,"state->fp is %08x\n",state->fp);
    fprintf(stdout,"Opened %s.\n", filename);
    fprintf(stdout,"about to read inbuf\n");
 }
 if(state->fp != NULL) {
    fgets(inbuf,K,state->fp);
 }
 if(verbose){
    fprintf(stdout,"inbuf read, contains %s\n",inbuf);
 }
 numfields = parse(inbuf,fields,MAXFIELDNUMBER,K);
 if(numfields < 2){
      fprintf(stderr,"Error: Too few fields: format is 'type: b/o/d/x' for binary/octal/dec/hex resp.\n");
      exit(0);
 }
 if(verbose){
    fprintf(stdout,"fields[0] = %s fields[1] = %s\n",fields[0],fields[1]);
 }
 if(strncmp(fields[0],"type",4)){
    fprintf(stderr,"Error: Header incorrect. Need rand size declared as 'type: b/o/d/x' for binary/octal/dec/hex format resp.\n");
    exit(0);
 }				/* set input as binary/octal/dec/hex */
 switch(fields[1][0]){
   case 'b':
     state->filenumtype = 'b';
     break;
   case '0':
     state->filenumtype = 'o';
     break;
   case 'd':
     state->filenumtype = 'd';
     break;
   case 'x':
     state->filenumtype = 'x';
     break;
   default:
     fprintf(stderr,"Error: Header incorrect. Need rand size declared as 'type: (b/o/d/x)'");
     break;
 }
 if(verbose){ 
    fprintf(stdout,"filenumtype set to %c\n",state->filenumtype);
 }
 if(state->fp != NULL) {
    fgets(inbuf,K,state->fp);
 }
 numfields = parse(inbuf,fields,MAXFIELDNUMBER,K);
 if(numfields < 2){
      fprintf(stderr,"Error: Too few fields: format is 'count: <dec int>' for count integers\n");
      exit(0);
 }
 if(strncmp(fields[0],"count",5)){
    fprintf(stderr,"Error: Header incorrect. Need number of rands declared as 'count: <dec int>' for count integers");
    exit(0);
 }
 if(verbose){
    fprintf(stdout,"fields[0] = %s fields[1] = %s\n",fields[0],fields[1]);
 }
 filecount = atoi(fields[1]);
 if(verbose){
    fprintf(stdout,"filecount set to %i\n",filecount);
 }
  if(state->fp != NULL) {
    fgets(inbuf,K,state->fp);
 }
 numfields = parse(inbuf,fields,MAXFIELDNUMBER,K);
 if(numfields < 2){
      fprintf(stderr,"Error: Too few fields: format is 'numbit: <dec int>' for numbit-bit integers");
      exit(0);
 }
 if(strncmp(fields[0],"numbit",6)){
    fprintf(stderr,"Error: Header incorrect. Need number of bits declared as 'numbit: <dec int>' for numbit-bit integers");
    exit(0);
 }
 if(verbose){
    fprintf(stdout,"fields[0] = %s fields[1] = %s\n",fields[0],fields[1]);
 }
 filenumbits = atoi(fields[1]);
 if(verbose){
    fprintf(stdout,"filenumbits set to %i\n",filenumbits);
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
