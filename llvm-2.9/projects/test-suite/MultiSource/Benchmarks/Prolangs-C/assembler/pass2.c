/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* ============================ pass2.c =================================== */ 
/* Drives the pass2 process, but lets other routines do the real work.      */
#include <stdio.h>
#include <string.h>
#include "stringI.h"
#include "scan_line.h"
#include "convert.h"
#include "sym_tab.h"
#include "record.h"
#include "assem.h"
#include "constants.h"
#include "buffer.h"
#include "code.h"

/* -------------------------- PASS2 ---------------------------------------- */
/* Drives pass2                                                              */
void PASS2(FILE *INPUT_STREAM,FILE *OBJECT_STREAM,FILE *LISTING_STREAM,
           SYMBOL_TABLE *SYM_TAB,int *ERROR)
{
  char TAG_FIELD;                    /* First character to each line of      */
                                     /*    output is TAG determining what    */
                                     /*    information that line contains.   */
  char *REST_OF_LINE;                /* Input line minus the tag field.      */
  struct SYMBOL_TABLE_ENTRY *TABLE_ENTRY;
                                     /* Entry in symbol table for a label    */
  char LABEL_NAME[LABEL_SIZE_1+1];   /* Place to store a label.              */
  int ERROR_SINCE_LAST_STATEMENT     /* Flag for error detection             */
    = FALSE_1;

/*  --------------------------- Reset module name and NEXT_MISSING_LABEL     */
/*                              to their values at the start of pass1        */
  MODULE_NAME[0] = '_';         
  MODULE_NAME[1] = '\0';
  RESET_MISSING_LABEL_NAME();

  INITIALIZE_TEXT_RECORD();

  while (!feof(INPUT_STREAM)) {
    TAG_FIELD = (char) getc(INPUT_STREAM);
    switch (TAG_FIELD) {
    case 'p':
/* what follows is straight from the source file. Assemble it.              */
      if (ERROR_SINCE_LAST_STATEMENT) (void) fprintf(LISTING_STREAM,"\n");
      ERROR_SINCE_LAST_STATEMENT = FALSE_1;
      CODE(INPUT_STREAM,SYM_TAB,OBJECT_STREAM,LISTING_STREAM,
	   &ERROR_SINCE_LAST_STATEMENT);
      (*ERROR) = ((*ERROR) || ERROR_SINCE_LAST_STATEMENT);
      OUTPUT_BUFFER(&ERROR_REC_BUF,LISTING_STREAM,2);
      break;

    case 'e':
/* What follows is an error message from pass1. Pass it to listing file.     */
      ERROR_SINCE_LAST_STATEMENT = TRUE_1;
      GET_LINE(&REST_OF_LINE,INPUT_STREAM);
      (void) fprintf(LISTING_STREAM,"%s\n",REST_OF_LINE);
      (*ERROR) = TRUE_1;
      break;

    case 't':
      {
/* What follows is a text record (do to BYTE or WORD) generated in pass1     */
/* Handle approperiately. (i.e. Let the text record handler deal with it).   */
	char TEMP_CH;
	int LOCATION;

	(void) fscanf(INPUT_STREAM,"%d%c",&LOCATION,&TEMP_CH);
	GET_LINE(&REST_OF_LINE,INPUT_STREAM); 
	if ( TEMP_CH == 'W')
	  ADD_TO_TEXT_RECORD(REST_OF_LINE,LOCATION,OBJECT_STREAM);
	else {
	  char TEMP[HEX_CHAR_PER_BYTE_1+1];
	  int DELTA = 0;

	  for (TEMP[HEX_CHAR_PER_BYTE_1]='\0';!eoln(*REST_OF_LINE);
	       REST_OF_LINE+=HEX_CHAR_PER_BYTE_1) {
	    (void) strncpy(TEMP,REST_OF_LINE,HEX_CHAR_PER_BYTE_1);
	    ADD_TO_TEXT_RECORD(TEMP,(LOCATION+DELTA),OBJECT_STREAM);
	    DELTA ++;
	  }
	}
      }
      break;

    case 'E': 
/* What follows is an end record... pass it and the 'E' to the object file   */
/* After printing any text records and modification records that may be      */
/* buffered.                                                                 */
      PRT_TEXT_RECORD(OBJECT_STREAM);
      INITIALIZE_TEXT_RECORD();
      OUTPUT_BUFFER(&MOD_REC_BUF,OBJECT_STREAM,2);

      if (!strcmp(MAIN_ROUTINE,MODULE_NAME)) {
	char ADDR[MEM_ADDR_SIZE_1/BITS_PER_HALFBYTE_1+2];

	if (START_ADDRESS == -1)
	  START_ADDRESS = (*LOOK_UP_SYMBOL(MODULE_NAME,MODULE_NAME,
					   SYM_TAB)).LOCATION;

	NUM_TO_STR(START_ADDRESS,16,MEM_ADDR_SIZE_1/BITS_PER_HALFBYTE_1+1,
		   ADDR);
	(void) fprintf(OBJECT_STREAM,"E%s\n",ADDR);
      } else (void) fprintf(OBJECT_STREAM,"E\n");
      GET_LINE(&REST_OF_LINE,INPUT_STREAM);
      break;

    case 'R':
/* What follows is a refer record. Pass it and the 'R' to the object file.   */
      GET_LINE(&REST_OF_LINE,INPUT_STREAM);
      (void) fprintf(OBJECT_STREAM,"%c%s\n",TAG_FIELD,REST_OF_LINE);
      break;

    case 'M':
/* What follows is a modification record. Put it on the modification record  */
/* buffer.                                                                   */
      GET_LINE(&REST_OF_LINE,INPUT_STREAM);
      ADD_TO_END_OF_BUFFER(&MOD_REC_BUF,REST_OF_LINE);
      break;

    case 'd':
      {
/* What follows is a define record. Pass1 didn't know the locations, just    */
/* the names. It made sure that the the defs will fit in one record though,  */
/* so here, must just put the locations after the names as output to the     */
/* object file.                                                              */
	char TEMP[LABEL_SIZE_1+1];
	int AT;

	GET_LINE(&REST_OF_LINE,INPUT_STREAM); 
	(void) fprintf(OBJECT_STREAM,"D");
	
	for (TEMP[LABEL_SIZE_1]='\0';!eoln(*REST_OF_LINE);
	     REST_OF_LINE+=LABEL_SIZE_1) {
	  (void) strncpy(TEMP,REST_OF_LINE,LABEL_SIZE_1);
	  (void) fprintf(OBJECT_STREAM,"%s",TEMP);
	  for (AT=LABEL_SIZE_1-1;((AT > 0) && (TEMP[AT] == ' '));AT--);
	  TEMP[AT+1] = '\0';
	  TABLE_ENTRY = LOOK_UP_SYMBOL(MODULE_NAME,TEMP,SYM_TAB);

	  if (TABLE_ENTRY == NULL) {
	    (void) fprintf(LISTING_STREAM,
			   "ERROR[41]: %s Undefined label in EXTDEF.\n",
			   TEMP);
	    (void) fprintf(OBJECT_STREAM,"000000");
	    (*ERROR) = 1;

	  } else if ((*TABLE_ENTRY).TYPE != RELATIVE) {
	    (void) fprintf(LISTING_STREAM,
   "ERROR[42]: %s is wrong type. Expected LABEL, found EXTREF or CONSTANT.\n",
			   TEMP);
	    (void) fprintf(OBJECT_STREAM,"000000");
	    (*ERROR) = 1;

	  } else PRT_NUM( (*TABLE_ENTRY).LOCATION,16,6,OBJECT_STREAM);
	}
	(void) fprintf(OBJECT_STREAM,"\n");
      }
      break;

    case 's':
/* what follows is a start record. Means must output a Header record to     */
/* object file.                                                             */
      GET_LINE(&REST_OF_LINE,INPUT_STREAM);
      BLANK_STR(LABEL_NAME);
      (void) strncpy(LABEL_NAME,REST_OF_LINE,strlen(REST_OF_LINE));
      (void) strcpy(MODULE_NAME,REST_OF_LINE);
      (void) fprintf(OBJECT_STREAM,"H%s",LABEL_NAME);
      TABLE_ENTRY = LOOK_UP_SYMBOL(REST_OF_LINE,REST_OF_LINE,SYM_TAB);
      PRT_NUM((*TABLE_ENTRY).LOCATION,16,6,OBJECT_STREAM);
      PRT_NUM((*TABLE_ENTRY).LENGTH,16,6,OBJECT_STREAM);
      (void) fprintf(OBJECT_STREAM,"\n");
      break;

    default:
/* Something put out by pass1 that shouldn't be here.                        */
      if (!feof(INPUT_STREAM)) {

	GET_LINE(&REST_OF_LINE,INPUT_STREAM);
	(void) printf(
	      "TEMP FILE build incorrectly. This should never happen.\n");
	(void) printf("%c%s\n",TAG_FIELD,REST_OF_LINE);
      }
      break;
    } /* END switch (TAG_FIELD) */
  } /* END   while (!feof(INPUT_STREAM)) */
  PRT_TEXT_RECORD(OBJECT_STREAM);
}
