/*
 *========================================================================
 * $Id$
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *========================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#define BUFLEN 1024

char **allocate_fields(size_t maxfields,size_t maxfieldlength)
{

 int i;

 char **outfields;
 
 outfields = (char **) malloc((size_t) (maxfields*sizeof(char*)));
 for(i = 0;i < maxfields;i++) {
   outfields[i] = (char *) malloc((size_t)(maxfieldlength*sizeof(char)));
 }

 return(outfields);
 
}


/*
 * parse() is a reusable routine to break up a string into char[32]
 * fields.  Anything past 32 characters is truncated.
 */
int parse(char *inbuffer,char **outfields,int maxfields,int maxfieldlength)
{

 char delim[8],*nextval;
 int i = 0;
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
 delim[6] = '=';		/* : needed to parse field = value */
 delim[7] = (char) NULL;        /* terminator */

 nextval = strtok(inbuffer,delim);
 /* fprintf(OUTFP,"nextval = %s\n",nextval); */
 if(nextval == (char *)NULL) return 0;
 strncpy(outfields[i++],nextval,maxfieldlength);
 /* fprintf(OUTFP,"outfields[%d] = %s\n",i-1,outfields[i-1]); */

 while(i < maxfields){
   nextval = strtok((char *) NULL, delim);
   /* fprintf(OUTFP,"nextval = %s\n",nextval); */
   if(nextval == (char *)NULL) break;
   strncpy(outfields[i++], nextval,maxfieldlength);
   /* fprintf(OUTFP,"outfields[%d] = %s\n",i-1,outfields[i-1]); */
 }

 return i;

}

/*
 * split() is a reusable routine to break up a string into fields.
 * One has to pass it the string to be split, a vector of strings
 * to fill in, a null-terminated vector of split characters, the 
 * maximum number of fields one can produce, and the maximum length 
 * of the fields one can produce.  We can probably do better, but 
 * this will do for now.
 */
int split(char *inbuffer,char **outfields,char *delim,
               int maxfields,int maxfieldlength)
{

 char *nextval;
 int i = 0;
 
 nextval = strtok(inbuffer,delim);
 if(nextval == (char *)NULL) return 0;
 strncpy(outfields[i++],nextval,maxfieldlength);

 while(i < maxfields){
   nextval = strtok((char *) NULL, delim);
   if(nextval == (char *)NULL) break;
   strncpy(outfields[i++], nextval,maxfieldlength);
 }

 return i;

}

