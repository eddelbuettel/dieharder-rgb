/*
 *========================================================================
 * $Id$
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *========================================================================
 */

#include "benchmaster.h"

/*
 * parse() is a reusable routine to break up a string into fields.
 * Anything past maxfieldlength characters is truncated.
 *
 * To use this, allocate a maxfields x maxfieldlength char **array
 * e.g. field and then split a line held in buf with:
 *
 *  nf = parse(buf,fields,maxfields,maxfieldlength);
 *
 * at which point you can look at the fields pretty much the same way
 * you look at **argv from the command line:
 *
 *  for(i=0;i<nf;i++) printf("field[%d] = %s",i,field[i]);
 *  printf("\n");
 *
 * If buf contains e.g.
 *
 *  count = 100
 *
 * then the count can be extracted with something like:
 *
 *  cnt = atoi(field[2]);
 *
 * assuming cnt is an int.  More generally use conversion tools like
 * strtoul (uint) or strtod (double) to maintain fine grained control of
 * typing as atoi and atof are macros for these tools anyway but hide
 * their typing and some of the controls.
 *
 * Note that the use of strtok() is somewhat deprecated.  Tough.  It is
 * a damn useful tool and the alternatives all involve rewriting it.
 */

int parse(char *inbuffer,char **outfields,int maxfields,int maxfieldlength)
{

 char delim[7],*nextval;
 int i = 0;

 if((verbose == VERBOSE) || (verbose == V_INIT)){
   printf("V_INIT: Starting parse().  Use -v %d to focus.\n",V_INIT);
 }



/* 
 * Permit blank, tab, or comma separators anywhere we need to parse
 * a line.
 */
 delim[0] = ' ';                /* blanks */
 delim[1] = (char) 9;           /* tab */
 delim[2] = ',';                /* comma */
 delim[3] = (char) 10;		/* LF */
 delim[4] = (char) 13;		/* CR */
 delim[5] = ':';		/* : needed to parse /proc/net/dev or passwd */
 delim[6] = (char) NULL;        /* terminator */

 
 nextval = strtok(inbuffer,delim);
 if(nextval == (char *)NULL) return 0;
 strncpy(outfields[i++],nextval,maxfieldlength);
 if((verbose == VERBOSE) || (verbose == V_INIT)){
   printf("V_INIT: Parsed field[%d] = %s.\n",i-1,outfields[i-1]);
 }

 while(i < maxfields-1){
   nextval = strtok((char *) NULL, delim);
   if(nextval == (char *)NULL) break;
   strncpy(outfields[i++], nextval,maxfieldlength);
   if((verbose == VERBOSE) || (verbose == V_INIT)){
     printf("V_INIT: Parsed field[%d] = %s.\n",i-1,outfields[i-1]);
   }
 }

 /* Null the last field */
 bzero(outfields[i],maxfieldlength);
 if((verbose == VERBOSE) || (verbose == V_INIT){
   printf("V_INIT: Terminated field[%d] = %s.\n",i,outfields[i]);
 }

 return i;

}

