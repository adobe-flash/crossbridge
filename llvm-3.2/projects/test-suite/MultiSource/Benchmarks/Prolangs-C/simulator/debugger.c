/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */

/* =============================== debugger.c =============================== */
/* Drives the whole simulatation/debugging process. Contains the debugging   */
/* Routines and calls the simulation routines (else where defined).          */
#include <string.h>
#include "memory.h"
#include "boolean.h"
#include "constants.h"
#include "sym_tab.h"
#include "sim_debug.h"
#include "machine.h"
#include "stringI.h"
/* ---------------------------------- Globals ------------------------------ */
/* MAIN_ROUTINE                       Name of (program) module declared with */
/*                                       a START (main routine).             */
char MAIN_ROUTINE[LABEL_SIZE_1+1];

/* START_ADDRESS                      Location where execution begins        */
int START_ADDRESS = 0;

/* SYM_TAB                            The symbol table                       */
SYMBOL_TABLE SYM_TAB;

/* RETURN_STATUS                      Status of machine when control returned*/
int RETURN_STATUS = HALT_1;        /* to debugger.                           */

/* TYPE_OUT_MODE                      Remembers the type out mode            */
char TYPE_OUT_MODE[6] = "Hex";

/* ----------------------------- Non-Globals ------------------------------- */
#define MAX_NUM_BREAK_POINTS         16
int BREAK_POINTS[MAX_NUM_BREAK_POINTS+1];
int SORTED_BREAK_POINTS[MAX_NUM_BREAK_POINTS+1];
int NUM_BREAK_POINTS = 0;
BOOLEAN TRACE = FALSE_1;

/* ---------------------------- Procedures --------------------------------- */

extern int CHAR_TO_DIGIT(char CH,int NUM_BASE);

/* -------------------------- IS_PREFIX_TO (local) ------------------------- */
/* Returns TRUE iff PREFIX is a prefix of WORD                               */
BOOLEAN IS_PREFIX_TO(char *PREFIX,char *WORD)
{
  int COUNT;
  int LEN;

  if ((LEN = strlen(PREFIX)) > strlen(WORD)) return FALSE_1;
  for (COUNT=0; COUNT < LEN; COUNT ++)
    if (PREFIX[COUNT] != WORD[COUNT]) return FALSE_1;
  return TRUE_1;
}

/* ---------------------------- REMOVE_WS (local) ------------------------- */
/* Line is a pointer to a string. Changes LINE to point to the string that  */
/* starts at the first non-blank/tab character in *LINE.                    */
void REMOVE_WS(char **LINE)
{
/* Skip              SPACEs and             TABs                             */
  while (((**LINE) == ' ') || ((**LINE) == '	'))
    (*LINE) ++;
}

/* ------ Different types of things need to type into the debugger.          */
#define ADDRESS_0          0
#define ADDR_REG_0         1
#define NUMBER_0           2
#define VALUE_0            3
/* ----------------------------- TYPE_IN (local) --------------------------- */
/* Return a value specified by a type in mode. Assumes:                      */
/*   No $ prefix       Hexidecimal input                                     */
/*   $D prefix         Decimal input                                         */
/*   $C prefix         Character input.                                      */
/*   $S prefix         Address of a LABEL returned.                          */
int TYPE_IN(char **COMMAND,BOOLEAN *LEGAL,int RESULT_TYPE)
{
  int VALUE = 0;
  int LOOP;

  (*LEGAL) = TRUE_1;
  REMOVE_WS(COMMAND);
  if ( (**COMMAND) == '$') 
    {
      (*COMMAND) ++;
      switch (**COMMAND) {
/* -------------------------------- $D prefix ------------------------------ */
      case 'D':
	(*COMMAND) ++;
	if (CHAR_TO_DIGIT((**COMMAND),10) == -1) {
	  (void) printf("Expected an decimal, found '%s'. Ignoring command.\n",
			*COMMAND);
	  (*LEGAL) = FALSE_1;
	} else {
	  while (CHAR_TO_DIGIT((**COMMAND),10) != -1) {
	    VALUE = VALUE * 10 + CHAR_TO_DIGIT((**COMMAND),10);
	    (*COMMAND) ++;
	  }
	}
	break;

/* -------------------------------- $C prefix ------------------------------ */
      case 'C':
	if (RESULT_TYPE != VALUE_0) (*LEGAL) = FALSE_1; 
	else {
	  (*COMMAND) ++;
	  VALUE =  (int) **COMMAND;
	  if ( (**COMMAND != '\0')) (*COMMAND) ++;
	}
	break;

/* -------------------------------- $S prefix ------------------------------ */
      case 'S':	
	if (RESULT_TYPE == NUMBER_0) (*LEGAL) = FALSE_1; 
	else {
	  (*COMMAND) ++;
	  REMOVE_WS(COMMAND);
	  if (!( ((**COMMAND) >= 'A') && ((**COMMAND) <= 'Z'))) {
	    (void) printf(
		   "Expected an identifier, found '%s'. Ignoring command.\n",
			  *COMMAND);
	    (*LEGAL) = FALSE_1;
	    return 0;
	  } else {
	    char SYMBOL[LABEL_SIZE_1+1];
	    struct SYMBOL_TABLE_ENTRY *SYMB;
	    
	    for (LOOP=0; ( (LOOP < LABEL_SIZE_1) && 
			  ( ((**COMMAND) >= 'A') && ((**COMMAND) <= 'Z')));
		 LOOP ++) {
	      SYMBOL[LOOP] = **COMMAND;
	      (*COMMAND) ++;
	    }
	    for (;LOOP<LABEL_SIZE_1;LOOP++)
	      SYMBOL[LOOP] = ' ';
	    SYMBOL[LABEL_SIZE_1] = '\0';
	    SYMB = LOOK_UP_SYMBOL(GLOBAL_1,SYMBOL,&SYM_TAB);
	    if (SYMB == NULL) {
	      (void) printf(
			 "Unknown identifier, found '%s'. Ignoring command.\n",
			    SYMBOL);
	      (*LEGAL) = FALSE_1;
	      return 0;
	    }
	    VALUE = (*SYMB).LOCATION;
	  }
	}
	break;
      default: 
	(*LEGAL) = FALSE_1;
      }

      if (!(*LEGAL)) {
	switch (RESULT_TYPE) {
	case ADDRESS_0:
	  (void) printf("Expected an type-in mode specifier ($D or $S).\n");
	  break;
	case NUMBER_0:
	  (void) printf("Expected an type-in mode specifier ($D).\n");
	  break;
	case ADDR_REG_0:
	  (void) printf(
	       "Expected an type-in mode specifier ($D, $S or $R).\n");
	  break;
	case VALUE_0:
	  (void) printf(
	       "Expected an type-in mode specifier ($C, $D, or $S).\n");
	  break;
	default: ;
	}
	(void) printf ("   Found $%c. Ignoring command.\n",(**COMMAND));
      }
    }
  else 
/* --------------------------NO $ prefix (hex) ----------------------------- */
    if (CHAR_TO_DIGIT((**COMMAND),16) == -1) {

      (void) printf("Expected an hexidecimal, found '%s'. Ignoring command.\n",
		    *COMMAND);
      (*LEGAL) = FALSE_1;
    } else {
      while (CHAR_TO_DIGIT((**COMMAND),16) != -1) {
	VALUE = VALUE * 16 + CHAR_TO_DIGIT((**COMMAND),16);
	(*COMMAND) ++;
	}
    }

  return VALUE;
}

/* ---------------------------- EXAMINE (local) ---------------------------- */
/* DEBUGGER COMMAND.                                                         */
/* Print of the contents of a register of memory location or store into a    */
/* Register or memory location.                                              */
void EXAMINE(char *COMMAND,BOOLEAN *LEGAL)
{
  int ADDRESS;                /* Register number of memory address dealing   */
                              /*   with.                                     */
  int VALUE;                  /* Value to store (if necessary)               */
  char *START;                /* START of address/register in command.       */
  char *END;                  /* END of address/register in command.         */
  BOOLEAN NO_ERROR;           /* TRUE iff NO_ERROR has been found.           */
  BOOLEAN STORE = FALSE_1;    /* TRUE if a store command, else FALSE         */

  (*LEGAL) = TRUE_1;
  REMOVE_WS(&COMMAND);
  START = (COMMAND);
  if (strlen(COMMAND) <2) ADDRESS = TYPE_IN(&COMMAND,&NO_ERROR,ADDR_REG_0);
  else if ((COMMAND[0] != '$') || (COMMAND[1] != 'R')) 
     ADDRESS = TYPE_IN(&COMMAND,&NO_ERROR,ADDR_REG_0);
   else {
/* ~~~~~~~~~~~~~~~~~~~~~ Get register number if $R type in mode.             */
     COMMAND += 2;
     switch (*COMMAND) {
     case 'A':
       ADDRESS = 0; break;
     case 'X':
       ADDRESS = 1; break;
     case 'L':
       ADDRESS = 2; break;
     case 'B':
       ADDRESS = 3; break;
     case 'S':
       if (*(COMMAND+1) == 'W') {
	 (COMMAND) ++;
	 ADDRESS = 9;
       } else ADDRESS = 4;
       break;
     case 'T':
       ADDRESS = 5; break;
     case 'P':
       if (*(COMMAND+1) == 'C') {
	 COMMAND ++;
	 ADDRESS = 8;
       } else ADDRESS = -1;
       break;
     default:
       ADDRESS = -1; break;
     }
     if (ADDRESS == -1) { 
       (void) printf(
	     "Expected a valid register name (A,X,L,B,S,T,SW, or PC).\n");
       (void) printf("   Found %s. Ignoring command.\n",COMMAND);
       return;
     } 
/* ~~~~~~~~~~~~~~~~~~~~~ Handle registers.                                   */
     COMMAND++;
     END = COMMAND;
     REMOVE_WS(&COMMAND);
/* --------------------- if '=' then a store command                         */
     if ((*COMMAND) == '=') {
       COMMAND++;
       VALUE = TYPE_IN(&COMMAND,&NO_ERROR,VALUE_0);
       if (!NO_ERROR) return;
       REMOVE_WS(&COMMAND);
       STORE = TRUE_1;
     }
     if ((*COMMAND) != '\0') {
       (void) printf(
	      "Expected end of statement, found '%s'. Ignoring command.\n"
		     ,COMMAND);		   
       return;
     }
     else {
/* ------------------------ Do what is supposed to be done.                  */
       (*END) = '\0';
       if (STORE) {
	 REGISTER[ADDRESS] = VALUE;
	 (void) printf("Set ");
       }
       (void) printf("Register %s = ",START);
       (void) PRINT_CONSTANT("",REGISTER[ADDRESS],".\n");
       return;
     }
   }
  {
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~ Handle memory address                        */
    END = COMMAND;
    REMOVE_WS(&COMMAND);
    if (NO_ERROR)
      if ( (0 > ADDRESS) || (MEM_SIZE_1 <= ADDRESS+2))
	PRINT_CONSTANT("",ADDRESS," is a legal memory [word] address.\n");
      else {
/* --------------------- if '=' then a store command                         */
	if ((*COMMAND) == '=') {
	  COMMAND++;
	  VALUE = TYPE_IN(&COMMAND,&NO_ERROR,VALUE_0);
	  if (!NO_ERROR) return;
	  REMOVE_WS(&COMMAND);
	  STORE = TRUE_1;
	}

	if ((*COMMAND) != '\0')
	  (void) printf(
		"Expected end of statement, found '%s'. Ignoring command.\n"
			,COMMAND);
	else {

	  (*END) = '\0';
/* ------------ Take the appropriate action (STORE/EXAMINE)                  */
	  if (STORE) {
	    REGISTER[ADDRESS] = VALUE;
	    MEMORY[ADDRESS] = VALUE/(256*256);
	    MEMORY[ADDRESS+1] = (VALUE - INT(MEMORY[ADDRESS])*256*256)/256;
	    MEMORY[ADDRESS+2] = (VALUE - INT(MEMORY[ADDRESS+1])*256);
	    (void) printf("Set ");
	  } 
	  VALUE = ((INT(MEMORY[ADDRESS])*256 + INT(MEMORY[ADDRESS+1]))*256+
		   INT(MEMORY[ADDRESS+2]));
	  (void) printf("Location %s = ",START);
	  if (TYPE_OUT_MODE[0] != 'I')
	    PRINT_ADDRESS("",VALUE,".\n");
	  else {
	    (void) printf("\n");
	    PRINT_INSTRUCTION(ADDRESS,TRUE_1,&VALUE);
	  }
	}
      }
  }
}  

/* ---------------------------- LIST_BP (local) ---------------------------- */
/* DEBUGGER COMMAND.                                                         */
/* Print out all the currently set break points.                             */
void LIST_BP(char *COMMAND,BOOLEAN *LEGAL)
{
  int COUNT;                  /* A loop counter.                             */

  REMOVE_WS(&COMMAND);
  (*LEGAL) = TRUE_1;

  if ((*COMMAND) != '\0')
    (void) printf("Expected end of statement, found '%s'. Ignoring command.\n"
		  ,COMMAND);
  else 
    if (NUM_BREAK_POINTS == 0)
      (void) printf("No break points set.\n");
    else for (COUNT=0;COUNT<MAX_NUM_BREAK_POINTS;COUNT++) 
      if (BREAK_POINTS[COUNT] != -1) {
	PRINT_CONSTANT("Set B",COUNT,"= ");
	PRINT_ADDRESS("",BREAK_POINTS[COUNT],"\n");
      }
}

/* ---------------------------- SET_BP (local) ----------------------------- */
/* DEBUGGER COMMAND.                                                         */
/* Check if list breakpoint command. If so call LIST_BP, other wise set a    */
/* break point. A sorted version of the breakpoints is kept so that binary   */
/* search can be used on them.                                               */
void SET_BP(char *COMMAND,BOOLEAN *LEGAL)
{
  BOOLEAN NO_ERROR;           /* TRUE iff NO_ERROR has been found.           */
  int ADDR;
  int OPEN_SPOT;
  int COUNT;

  if ((*COMMAND) == '?') {
    LIST_BP(&(COMMAND[1]),LEGAL); 
    return;
  }
  (*LEGAL) = TRUE_1;
  ADDR = TYPE_IN(&COMMAND,&NO_ERROR,ADDRESS_0);
  REMOVE_WS(&COMMAND);
  if (NO_ERROR)
    if ( (0 > ADDR) || (MEM_SIZE_1 <= ADDR)) 
      PRINT_CONSTANT(
  "Address specified ",ADDR," out of range. No break point set.\n");
    else if (NUM_BREAK_POINTS == MAX_NUM_BREAK_POINTS)
      PRINT_CONSTANT("All ",MAX_NUM_BREAK_POINTS," breakpoints are in use.\n");
    else if ((*COMMAND) != '\0') 
     (void) printf("Expected end of statement, found '%s'. Ignoring command.\n"
		    ,COMMAND);
    else {
      for (OPEN_SPOT=0;BREAK_POINTS[OPEN_SPOT] != -1; OPEN_SPOT++);
      BREAK_POINTS[OPEN_SPOT] = ADDR;

      for (COUNT=NUM_BREAK_POINTS;
	   ((BREAK_POINTS[SORTED_BREAK_POINTS[COUNT]] > ADDR) &&
	    (COUNT > 0));
	   COUNT --)
	SORTED_BREAK_POINTS[COUNT+1] = SORTED_BREAK_POINTS[COUNT];
      SORTED_BREAK_POINTS[COUNT+1] = OPEN_SPOT;
      NUM_BREAK_POINTS ++;
      PRINT_CONSTANT("Set B",OPEN_SPOT,"= ");
      PRINT_ADDRESS("",BREAK_POINTS[OPEN_SPOT],"\n");
    }
}

/* ----------------------------- RESET (local) ----------------------------- */
/* DEBUGGER COMMAND.                                                         */
/* Remove the specified break point.                                         */
void RESET_BP(char *COMMAND,BOOLEAN *LEGAL)
{
  BOOLEAN NO_ERROR;           /* TRUE iff NO_ERROR has been found.           */
  int NUM;                    /* NUM of the break point to remove.           */
  int BP;                     /* Loop counter, steps through the break points*/

  (*LEGAL) = TRUE_1;
  NUM = TYPE_IN(&COMMAND,&NO_ERROR,NUMBER_0);
  REMOVE_WS(&COMMAND);
  if (NO_ERROR)
    if ( (0 > NUM) || (NUM_BREAK_POINTS <= NUM)) {
      PRINT_CONSTANT("",NUM," is not a legal break point.\n");
      PRINT_CONSTANT("Expected a number 0 - ",MAX_NUM_BREAK_POINTS,
		     ". Command ignored.\n");
    }
    else if ((*COMMAND) != '\0') 
     (void) printf("Expected end of statement, found '%s'. Ignoring command.\n"
		    ,COMMAND);
    else 
      if (BREAK_POINTS[NUM] == -1) 
	PRINT_CONSTANT("Break point ",NUM," is not set. Command ignored.\n");
      else {
	for (BP=1;
	     ((SORTED_BREAK_POINTS[BP] != NUM) && (BP <= NUM_BREAK_POINTS));
	     BP++);
	BREAK_POINTS[NUM] = -1;
	for (;BP <= NUM_BREAK_POINTS; BP ++)
	  SORTED_BREAK_POINTS[BP] = SORTED_BREAK_POINTS[BP+1];
	NUM_BREAK_POINTS --;
	PRINT_CONSTANT("Removed break point ",NUM,".\n");
      }
}

/* ------------------------------ AT_BREAK_POINT (local) ------------------- */
/* returns TRUE iff PC points to an instruction that has a byte in it that   */
/* has been set as a break point.                                            */
BOOLEAN AT_BREAK_POINT(void)
{
/* Binary Searches the sorted array of break_points for the breakpoint       */
  int MID;
  int HIGH;
  int LOW = 1;
  int RANGE;                      /* How many bytes the instruction is       */

  if (NUM_BREAK_POINTS == 0) return FALSE_1;
  HIGH = NUM_BREAK_POINTS;
  PRINT_INSTRUCTION(PC_0,FALSE_1,&RANGE);
  do {
    MID = (LOW + HIGH)/2;
    if ((BREAK_POINTS[SORTED_BREAK_POINTS[MID]] >= PC_0) &&
	(BREAK_POINTS[SORTED_BREAK_POINTS[MID]] <= PC_0 + RANGE - 1))
      return TRUE_1;
    if (BREAK_POINTS[SORTED_BREAK_POINTS[MID]] <= PC_0)
      LOW = MID + 1;
    else HIGH = MID - 1;
  } while (LOW <= HIGH);
  return FALSE_1;
}

/* ---------------------------- RUN_IT (local) ----------------------------- */
/* DEBUGGER COMMAND.                                                         */
/* Set up the machine for execution of the proceedure and keep executing     */
/* instructions until it stops or hits a break point.                        */
void RUN_IT(char *COMMAND,BOOLEAN *LEGAL)
{
  REMOVE_WS(&COMMAND);
  (*LEGAL) = TRUE_1;

  if ((*COMMAND) != '\0')
    (void) printf("Expected end of statement, found '%s'. Ignoring command.\n"
		  ,COMMAND);
  else {
    int DONT_CARE;

    RETURN_STATUS = EXECUTING_1;
    L_0 = RETURN_TO_OS_1;
    PC_0 = START_ADDRESS;
    RESET_DEVICES();

    SW_0 = 61440;          /* 00000001111000000000000 binary = 61440 decimal */
  /* User mode, Running, PID = 0, CC = 0, MASK = 1111, ICODE = 0 */
    if (TRACE) PRINT_INSTRUCTION(PC_0,TRUE_1,&DONT_CARE);
    while ((RETURN_STATUS == EXECUTING_1) && !AT_BREAK_POINT()) {
      EXEC(TRUE_1,TRACE);
      if ((TRACE) && (RETURN_STATUS == EXECUTING_1)) {
	(void) printf("\n");
	PRINT_INSTRUCTION(PC_0,TRUE_1,&DONT_CARE);
      }
    }
    if (RETURN_STATUS == EXECUTING_1)
      PRINT_ADDRESS("Break point encountered at ",PC_0,".\n");
  }
}

/* ---------------------------- PROCEED (local) ---------------------------- */
/* DEBUGGER COMMAND.                                                         */
/* Continue execution a program from a break point until the program         */
/* terminates or another break point is found.                               */
void PROCEED(char *COMMAND,BOOLEAN *LEGAL)
{
  int DONT_CARE;

  REMOVE_WS(&COMMAND);
  (*LEGAL) = TRUE_1;

  if ((*COMMAND) != '\0')
    (void) printf("Expected end of statement, found '%s'. Ignoring command.\n"
		  ,COMMAND);
  else {

    if (RETURN_STATUS != EXECUTING_1)
      (void) printf("Can not continue. Instruction ignored.\n");
    else
      do {
	EXEC(TRUE_1,TRACE);
	if ((TRACE) && (RETURN_STATUS == EXECUTING_1)) {
	  (void) printf("\n");
	  PRINT_INSTRUCTION(PC_0,TRUE_1,&DONT_CARE);
	}
      } while ((RETURN_STATUS == EXECUTING_1) && !AT_BREAK_POINT());

    if (RETURN_STATUS == EXECUTING_1)
      PRINT_ADDRESS("Break point encountered at ",PC_0,".\n");
  }
}

/* ---------------------------- EXAMINE (local) ---------------------------- */
/* DEBUGGER COMMAND.                                                         */
/* Execute on instruction, printing the result and the next instruction to   */
/* execute.                                                                  */
void STEP(char *COMMAND,BOOLEAN *LEGAL)
{
  int DONT_CARE;
  
  REMOVE_WS(&COMMAND);
  (*LEGAL) = TRUE_1;

  if ((*COMMAND) != '\0')
    (void) printf("Expected end of statement, found '%s'. Ignoring command.\n"
		  ,COMMAND);
  else {

    if (RETURN_STATUS != EXECUTING_1)
      (void) printf("Can not continue. Instruction ignored.\n");
    else {
      EXEC(TRUE_1,TRUE_1);
      (void) printf("\n");
      if (RETURN_STATUS == EXECUTING_1)
	PRINT_INSTRUCTION(PC_0,TRUE_1,&DONT_CARE);
    }
    
  }
}

/* ------------------------------- HELP (local) ---------------------------- */
/* DEBUGGER COMMAND.                                                         */
/* Prints out the debuggers help message.                                    */
void HELP(char *COMMAND,BOOLEAN *LEGAL)
{
  REMOVE_WS(&COMMAND);
  (*LEGAL) = TRUE_1;
  if ((*COMMAND) != '\0')
    (void) printf("Expected end of statement, found '%s'. Ignoring command.\n"
		  ,COMMAND);
  else {
   (void) printf("List of commands:\n");
   (void) printf("   Short      Long            Function\n\n");
   (void) printf("   B<addr>    BREAK<addr>     Set break point at <addr>.\n");
   (void) printf("   B?         BREAK?          List out the break points.\n");
   (void) printf("   H          HELP            Displays this table.\n");
   (void) printf(
"   L<loc>     LOC<loc>        Examines contents of memory and registers.\n");
   (void) printf(
"   L<loc>=<v> LOC<loc>=<v>    Examines contents of memory and registers.\n");
   (void) printf("   M<mode>    MODE<mode>      Set type out mode.\n");
   (void) printf("   P          PROCEED         Proceed from break point.\n");
   (void) printf("   Q          QUIT            Exit Debugger.\n");
   (void) printf(
	    "   R          RUN             Start execution at entry point.\n");
   (void) printf("   RB<num>    RESET<num>      Remve break point <num>.\n");
   (void) printf("   S          STEP            Execute one instruction.\n");
   (void) printf("   T          TRACE           Trace the execution.\n");  
   (void) printf(
	   "   U          UNTRACE         Stop tracing the execution.\n");  
   (void) printf("\n");
   (void) printf("<addr> = memory address or label.\n");
   (void) printf("<loc>  = memory address, label or register.\n");
   (void) printf(
       "<mode> = H (hexidecimal), D (decimal), or I (instruction).\n");
   (void) printf(
       "<v>    = hexidecimal number, decimal number, or 1 character.\n\n");
 }
}

/* ---------------------------- EXAMINE (local) ---------------------------- */
/* DEBUGGER COMMAND.                                                         */
/* Tells the debugger that the user wish to terminate the debugging session. */
void QUIT(char *COMMAND,BOOLEAN *LEGAL)
{
  (*LEGAL) = TRUE_1;
  REMOVE_WS(&COMMAND);
  if ((*COMMAND) != '\0')
    (void) printf("Expected end of statement, found '%s'. Ignoring command.\n"
		  ,COMMAND);
  else {
    RETURN_STATUS = QUIT_1;
  }
}

/* ---------------------------- TRACE_P (local) ---------------------------- */
/* DEBUGGER COMMAND.                                                         */
/* Turns on the tracing mode.                                                */
void TRACE_P(char *COMMAND,BOOLEAN *LEGAL)
{
  (*LEGAL) = TRUE_1;
  REMOVE_WS(&COMMAND);
  if ((*COMMAND) != '\0')
    (void) printf("Expected end of statement, found '%s'. Ignoring command.\n"
		  ,COMMAND);
  else 
    if (TRACE) (void) printf("Trace is already on.\n");
    else {
      TRACE = TRUE_1;
      (void) printf("Trace is now on.\n");
    }
}

/* --------------------------- UNTRACE_P (local) --------------------------- */
/* DEBUGGER COMMAND.                                                         */
/* Turns off tracing mode.                                                   */
void UNTRACE(char *COMMAND,BOOLEAN *LEGAL)
{
  (*LEGAL) = TRUE_1;
  REMOVE_WS(&COMMAND);
  if ((*COMMAND) != '\0')
    (void) printf("Expected end of statement, found '%s'. Ignoring command.\n"
		  ,COMMAND);
  else 
    if (!TRACE) (void) printf("Trace is already off.\n");
    else {
      TRACE = FALSE_1;
      (void) printf("Trace is now off.\n");
    }
}

/* ------------------------------ MODE (local) ----------------------------- */
/* DEBUGGER COMMAND.                                                         */
/* Set the type out mode [(H)exidecimal, (D)ecimal, (I)nstruction]           */
void MODE(char *COMMAND,BOOLEAN *LEGAL)
{
  (*LEGAL) = TRUE_1;
  REMOVE_WS(&COMMAND);
  if (*COMMAND == '\0')
    (void) printf("Unexpected end of command. Ignoring command.\n");
  else {
    if (*(COMMAND+1) == '\0')
      switch (*COMMAND) {
      case 'H':
	(void) strcpy(TYPE_OUT_MODE,"Hex");
	return;
	break;
      case 'D':
	(void) strcpy(TYPE_OUT_MODE,"Dec");
	return;
	break;
      case 'I':
	(void) strcpy(TYPE_OUT_MODE,"Instr");
	return;
	break;
      default: ;
      }

    (void) printf(
	  "Expected valid mode (H,D, or I). Found %s. Ignoring command.\n",
		  COMMAND);
  }
}

/* ---------------------- CAPITALIZE_STRING -------------------------------- */
/* Make all lower case letters in the string STR upper case letters          */
void CAPITALIZE_STRING(char STR[])
{
  int I;
  for (I=0; I<= (strlen(STR)); I++)
    if ((STR[I] >= 'a') && (STR[I] <= 'z'))
      STR[I] = STR[I] - 'a' + 'A';
}


#define STEP0 1
#define SET_BP0 2
#define EXAMINE0 3
#define RESET_BP0 4
#define RUN_IT0 5
#define HELP0 6
#define MODE0 7
#define PROCEED0 8
#define TRACE_P0 9
#define UNTRACE0 10
#define QUIT0 11

/* ~~~~~~~~~~~~~~~~~~~~~~ Table with the debugger commands                   */
#define COM_TAB_SIZE_1             11
struct COMMAND_TYPE {
  char *SHORT;                     /* Short version of command.              */
  char *LONG;                      /* Long version of the command.           */
  int FUNCTION;
} COMMANDS[COM_TAB_SIZE_1] =
  {{"S","STEP",STEP0},
   {"B","BREAK",SET_BP0},
   {"L","LOC",EXAMINE0},
   {"RB","RESET",RESET_BP0},
   {"R","RUN",RUN_IT0},
   {"H","HELP",HELP0},
   {"M","MODE",MODE0},
   {"P","PROCEED",PROCEED0},
   {"T","TRACE",TRACE_P0},
   {"U","UNTRACE",UNTRACE0},
   {"Q","QUIT",QUIT0}};

void CALL_FUNCTION(int FUNC,char *COMMAND,BOOLEAN *LEGAL)
{
    switch(FUNC) {
    case STEP0: STEP(COMMAND,LEGAL);
	break;
    case SET_BP0: SET_BP(COMMAND,LEGAL);
	break;
    case EXAMINE0: EXAMINE(COMMAND,LEGAL);
	break;
    case RESET_BP0: RESET_BP(COMMAND,LEGAL);
	break;
    case RUN_IT0: RUN_IT(COMMAND,LEGAL);
	break;
    case HELP0: HELP(COMMAND,LEGAL);
	break;
    case MODE0: MODE(COMMAND,LEGAL);
	break;
    case PROCEED0: PROCEED(COMMAND,LEGAL);
	break;
    case TRACE_P0: TRACE_P(COMMAND,LEGAL);
	break;
    case UNTRACE0: UNTRACE(COMMAND,LEGAL);
	break;
    case QUIT0: QUIT(COMMAND,LEGAL);
	break;
    }
}
/* -------------------------------- DEBUGGER ------------------------------- */
/* Drives the entire simulatation/debugging process.                         */
void DEBUGGER(BOOLEAN DEBUG_MODE)
{
  char *COMMAND;              /* The debugging command.                      */
  BOOLEAN LEGAL_COMMAND;      /* Is this a recognized command?               */
  int COUNT;                  /* Loop counter.                               */

  for (COUNT=0; COUNT <= MAX_NUM_BREAK_POINTS; COUNT ++) 
    BREAK_POINTS[COUNT] = -1;

  if (!DEBUG_MODE) {
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ No Debugging, just execute it                */
    RETURN_STATUS = EXECUTING_1;
    L_0 = RETURN_TO_OS_1;
    PC_0 = START_ADDRESS;

    SW_0 = 61440;          /* 00000001111000000000000 binary = 61440 decimal */
  /* User mode, Running, PID = 0, CC = 0, MASK = 1111, ICODE = 0             */
    EXEC(FALSE_1,FALSE_1);
  } else
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Debug the program.                           */
    while (RETURN_STATUS != QUIT_1) {
      LEGAL_COMMAND = FALSE_1;
      (void) printf("%s> ",TYPE_OUT_MODE);
      GET_LINE(&COMMAND,stdin);
      REMOVE_WS(&COMMAND);
      if ((*COMMAND) != '\0') {
	CAPITALIZE_STRING(COMMAND);
      
	for (COUNT=0; (COUNT < COM_TAB_SIZE_1) && !LEGAL_COMMAND; COUNT++) 
	  if (IS_PREFIX_TO(COMMANDS[COUNT].LONG,COMMAND))
	    CALL_FUNCTION(COMMANDS[COUNT].FUNCTION,
			  &(COMMAND[strlen(COMMANDS[COUNT].LONG)]),
				     &LEGAL_COMMAND);
	  else if (IS_PREFIX_TO(COMMANDS[COUNT].SHORT,COMMAND))
	    CALL_FUNCTION(COMMANDS[COUNT].FUNCTION,
			  &(COMMAND[strlen(COMMANDS[COUNT].SHORT)]),
				     &LEGAL_COMMAND);
	
	if (!LEGAL_COMMAND) {
	  (void) printf("'%s' is not a legal command. Ignoring it.\n",COMMAND);
	  (void) printf("Type HELP for the list of legal commands.\n");
	}
      }
      if (RETURN_STATUS == ABNORMAL_1) {
	int CODE;

	RETURN_STATUS = HALT_1;
	CODE = SW_0 - (SW_0/256)*256;
	PRINT_CONSTANT("error code =",CODE,"    ");
	switch (CODE) {
	case 0: (void) printf("Illegal instruction.\n"); break;
	case 1: (void) printf("Privileged instruction in user mode.\n"); break;
	case 2: (void) printf("Address out of range.\n"); break;
	case 3: (void) printf("Memory-protection violation.\n"); break;
	case 4: (void) printf("Aritmetic overflow.\n"); break;
	case 10: (void) printf("Page fault.\n"); break;
	case 11: (void) printf("Segment fault.\n"); break;
	case 12: (void) printf("Segment-protection violation.\n"); break;
	case 13: (void) printf("Segment length exceeded.\n"); break;
	default: (void) printf("\n"); break;
	}
      }
    }
}
