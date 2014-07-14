

#include "scanner.h"                /* Useful Macro Definitions */
#include <stdio.h>

/* File I/O Variables and Functions in file stringIO.c */
extern char CH;            /* current input character */
extern char *LEXEME;       /* string read in since the last flush */

extern char *FRONT,*BACK;  /* FRONT: pointer to current input character in
			         LEXEME.
			      BACK: pointer to the first input character in
			      LEXEME, in fact equal to LEXEME */
extern unsigned LEX_LEN;   /* current size of LEXEME buffer. Will change. */

extern void FLUSH();       /* a function that sets LEXEME to null */
extern void GETCHR();      /* a function that gets the next character and 
			      updates LEXEME,FRONT,and BACK appropriately */
extern void BACKUP();      /* a function that undoes the last not undone 
			      GETCHR */

static EOF_FLAG = 1, ERROR_CNT = 0;   /* End Of File Flag and ERROR CouNT */

static void GETSTR(void)   /* read in a string from the input stream */
{
  GETCHR();
  if (CH == ' ') {
    FLUSH();
    GETCHR();
  }
  else {
    ERROR_CNT ++;
    printf("------------>ERROR: Expected a SPACE between Token Code and\n");
    printf("                    the string following it <--------\n");
  }
  while (CH != '\n' && CH != EOF) GETCHR();
  if (CH == '\n')
    BACKUP();
  else
    EOF_FLAG = 0;
}

int main(void)
{
  int CODE;     /* TOKEN CODE */
  int I1,I2;    /* temp storage */
  double D1;    /* temp storage */
/* intialization */
  LEXEME = (char *) calloc(1+LEX_LEN,1);
  FLUSH();

/* main */
  while (EOF_FLAG) {
    scanf("%d",&CODE);

    if (!feof(stdin)) {
      switch (CODE) {

         case ERRORMSG_1:
            GETSTR();
	    printf("Scanner ERROR: %s\n",LEXEME);
	    break;

         case LINE_1:
	    scanf("%d",&I1);
	    printf("===================> LINE NUMBER: %d\n",I1);
	    break;

	 case ADDOP_1:
	    scanf("%d",&I1);
	    printf("   ADDOP: ");
	    switch (I1) {
	       case PLUS_1:
	         printf("+\n");
		 break;
	       case MINUS_1:
		 printf("-\n");
		 break;
	       case OR_1:
		 printf("OR\n");
		 break;
	       default: {
		 ERROR_CNT ++;
		 printf("---------->ERROR: Expected an ADDOP Found: %d\n",I1);
	         }
	       }
	    break;

	 case MULOP_1:
	    scanf("%d",&I1);
	    printf("   MULOP: ");
	    switch (I1) {
	       case TIMES_1:
	         printf("*\n");
		 break;
	       case SLASH_1:
		 printf("/\n");
		 break;
	       case AND_1:
		 printf("AND\n");
		 break;
	       case DIV_1:
		 printf("DIV\n");
		 break;
	       default: {
		 ERROR_CNT ++;
		 printf("---------->ERROR: Expected an MULOP Found: %d\n",I1);
	         }
	       }
	    break;

	 case RELOP_1:
	    scanf("%d",&I1);
	    printf("   RELOP: ");
	    switch (I1) {
	       case EQUAL_1:
	         printf("=\n");
		 break;
	       case NOTEQUAL_1:
		 printf("<>\n");
		 break;
	       case LESSTHANOREQUAL_1:
		 printf("<=\n");
		 break;
	       case LESSTHAN_1:
		 printf("<\n");
		 break;
	       case GREATERTHANOREQUAL_1:
		 printf(">=\n");
		 break;
	       case GREATERTHAN_1:
		 printf(">\n");
		 break;
	       default: {
		 ERROR_CNT ++;
		 printf("---------->ERROR: Expected an RELOP Found: %d\n",I1);
	         }
	       }
	    break;

	 case ASSIGN_1:
	    printf("   ASSIGN (:=) \n");
	    break;

	 case COLON_1:
	    printf("   COLON (:) \n");
	    break;

	 case DOTDOT_1:
	    printf("   DOTDOT (..) \n");
	    break;

	 case DOT_1:
	    printf("   DOT (.) \n");
	    break;

	 case INTEGER_NUMBER_1:
	    scanf("%d",&I1);
	    printf("   Integer: ");
	    if (I1 == ILLEGAL_1)
	      printf("illegal\n");
	    else
	      if (I1 == VALID_1) {
		scanf("%d",&I2);
		printf("%d\n",I2);
	      }
	      else {
		ERROR_CNT ++;
		printf("---------->ERROR: Expected a VALID/ILLEGAL Code \n");
	        printf("                  found: %d\n",I1);
	      }
	    break;

	 case REAL_NUMBER_1:
	    scanf("%d",&I1);
	    printf("   Real: ");
	    if (I1 == ILLEGAL_1)
	      printf("illegal\n");
	    else
	      if (I1 == VALID_1) {
		scanf("%lf",&D1);
		printf("%g\n",D1);
	      }
	      else {
		ERROR_CNT ++;
		printf("---------->ERROR: Expected a VALID/ILLEGAL Code \n");
	        printf("                  found: %d\n",I1);
	      }
	    break;

	 case PROGRAM_1:
	    printf("   PROGRAM \n");
	    break;

	 case VAR_1:
	    printf("   VAR \n");
	    break;

	 case INTEGER_1:
	    printf("   INTEGER (KEYWORD) \n");
	    break;

	 case REAL_1:
	    printf("   REAL (KEYWORD) \n");
	    break;

	 case ARRAY_1:
	    printf("   ARRAY \n");
	    break;

	 case OF_1:
	    printf("   OF \n");
	    break;

	 case PROCEDURE_1:
	    printf("   PROCEDURE \n");
	    break;

	 case FORWARD_1:
	    printf("   FORWARD \n");
	    break;

	 case FUNCTION_1:
	    printf("   FUNCTION \n");
	    break;

	 case BEGIN_1:
	    printf("   BEGIN \n");
	    break;

	 case END_1:
	    printf("   END \n");
	    break;

	 case READ_1:
	    printf("   READ \n");
	    break;

	 case WRITE_1:
	    printf("   WRITE \n");
	    break;

	 case WRITELN_1:
	    printf("   WRITELN \n");
	    break;

	 case IF_1:
	    printf("   IF \n");
	    break;

	 case THEN_1:
	    printf("   THEN \n");
	    break;

	 case ELSE_1:
	    printf("   ELSE \n");
	    break;

	 case WHILE_1:
	    printf("   WHILE \n");
	    break;

	 case DO_1:
	    printf("   DO \n");
	    break;

	 case NOT_1:
	    printf("   NOT \n");
	    break;

	 case ID_1:
	    printf("   Identifier:  ");
	    GETSTR();
	    printf("%s\n",LEXEME);
	    break;

	 case LEFT_PAREN_1:
	    printf("   LEFT PAREN '(' \n");
	    break;

	 case RIGHT_PAREN_1:
	    printf("   RIGHT PAREN ')' \n");
	    break;

	 case LEFT_BRACKET_1:
	    printf("   LEFT BRACKET ([) \n");
	    break;

	 case RIGHT_BRACKET_1:
	    printf("   RIGHT BRACKET (]) \n");
	    break;

	 case COMMA_1:
	    printf("   COMMA (,) \n");
	    break;

	 case SEMICOLON_1:
	    printf("   SEMICOLON (;) \n");
	    break;

         default: 
	    ERROR_CNT ++;
	    printf("------------>ERROR: A Valid TOKEN Code is not the\n");
	    printf("                    first item on the input line.");
	    printf(" <--------\n");
	    GETSTR();
	  }
      GETCHR();
      if (CH != '\n' && EOF_FLAG) {
	ERROR_CNT ++;
	printf("------------>ERROR: Expected a Newline Character <--------\n");
      }
      FLUSH();
    }
    else EOF_FLAG = 0;
  } /* end while (EOF_FLAG) */
  printf("EOF\n");
  if (ERROR_CNT)
    printf("****** %d format errors found in output of your scanner.\n",
	   ERROR_CNT);
  exit(0);
  return 0;
}
