/* Token-reader for Bison's input parser,
   Copyright (C) 1984, 1986 Bob Corbett and Free Software Foundation, Inc.

BISON is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY.  No author or distributor accepts responsibility to anyone
for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.
Refer to the BISON General Public License for full details.

Everyone is granted permission to copy, modify and redistribute BISON,
but only under the conditions described in the BISON General Public
License.  A copy of this license is supposed to have been given to you
along with BISON so you can know your rights and responsibilities.  It
should be in a file named COPYING.  Among other things, the copyright
notice and this notice must be preserved on all copies.

 In other words, you are welcome to use, share and improve this program.
 You are forbidden to forbid anyone else to use, share and improve
 what you give them.   Help stamp out software-hoarding!  */

/* 
   lex() is the entry point.  It is called from reader.c.
   It returns one of the token-type codes defined in lex.h.
   When an identifier is seen, the code IDENTIFIER is returned
   and the name is looked up in the symbol table using symtab.c;
   symval is set to a pointer to the entry found.  */

#include <stdio.h>
#include <ctype.h>
#include "files.h"
#include "symtab.h"
#include "lex.h"


extern int lineno;
extern int translations;


char token_buffer[MAXTOKEN + 1];
bucket *symval;
int numval;

static int unlexed;		/* these two describe a token to be reread */
static bucket *unlexed_symval;	/* by the next call to lex */

extern void fatals(char *fmt,int x1,int x2,int x3,int x4,int x5,int x6,
                   int x7,int x8);
extern void fatal(char *s);

int parse_percent_token(void);

void init_lex(void)
{
  unlexed = -1;
}



int skip_white_space(void)
{
  register int c;
  register int inside;

  c = getc(finput);

  for (;;)
    {
      switch (c)
	{
	case '/':
	  c = getc(finput);
	  if (c != '*')
	    fatals("unexpected '/%c' found",c,0,0,0,0,0,0,0);

	  c = getc(finput);

	  inside = 1;
	  while (inside)
	    {
	      if (c == '*')
		{
		  while (c == '*')
		    c = getc(finput);

		  if (c == '/')
		    {
		      inside = 0;
		      c = getc(finput);
		    }
		}
	      else if (c == '\n')
		{
		  lineno++;
		  c = getc(finput);
		}
	      else if (c == EOF)
		fatal("unterminated comment");
	      else
		c = getc(finput);
	    }

	  break;

	case '\n':
	  lineno++;

	case ' ':
	case '\t':
	case '\f':
	  c = getc(finput);
	  break;

	default:
	  return (c);
	}
    }
}

void unlex(int token)
{
  unlexed = token;
  unlexed_symval = symval;
}

int lex(void)
{
  register int c;
  register char *p;

  if (unlexed >= 0)
    {
      symval = unlexed_symval;
      c = unlexed;
      unlexed = -1;
      return (c);
    }

  c = skip_white_space();

  switch (c)
    {
    case EOF:
      return (ENDFILE);

    case 'A':  case 'B':  case 'C':  case 'D':  case 'E':
    case 'F':  case 'G':  case 'H':  case 'I':  case 'J':
    case 'K':  case 'L':  case 'M':  case 'N':  case 'O':
    case 'P':  case 'Q':  case 'R':  case 'S':  case 'T':
    case 'U':  case 'V':  case 'W':  case 'X':  case 'Y':
    case 'Z':
    case 'a':  case 'b':  case 'c':  case 'd':  case 'e':
    case 'f':  case 'g':  case 'h':  case 'i':  case 'j':
    case 'k':  case 'l':  case 'm':  case 'n':  case 'o':
    case 'p':  case 'q':  case 'r':  case 's':  case 't':
    case 'u':  case 'v':  case 'w':  case 'x':  case 'y':
    case 'z':
    case '.':  case '_':
      p = token_buffer;
      while (isalnum(c) || c == '_' || c == '.')
	{
	  if (p < token_buffer + MAXTOKEN)
	    *p++ = c;
	  c = getc(finput);
	}

      *p = 0;
      ungetc(c, finput);
      symval = getsym(token_buffer);
      return (IDENTIFIER);

    case '0':  case '1':  case '2':  case '3':  case '4':
    case '5':  case '6':  case '7':  case '8':  case '9':
      {
	numval = 0;

	while (isdigit(c))
	  {
	    numval = numval*10 + c - '0';
	    c = getc(finput);
	  }
	ungetc(c, finput);
	return (NUMBER);
      }

    case '\'':
      translations = -1;

      /* parse the literal token and compute character code in  code  */

      c = getc(finput);
      {
	register int code = 0;

	if (c == '\\')
	  {
	    c = getc(finput);

	    if (c <= '7' && c >= '0')
	      {
		while (c <= '7' && c >= '0')
		  {
		    code = (code * 8) + (c - '0');
		    c = getc(finput);
		  }
		if (code >= 128 || code<0)/* JF this said if(c>=128) */
		  fatals("malformatted literal token '\\%03o'",code,0,0,0,0,0,
                         0,0);
	      }
	    else
	      {
		if (c == 't')
		  code = '\t';
		else if (c == 'n')
		  code = '\n';
		else if (c == 'r')
		  code = '\r';
		else if (c == 'f')
		  code = '\f';
		else if (c == 'b')
		  code = '\b';
		else if (c == '\\')
		  code = '\\';
		else if (c == '\'')
		  code = '\'';
		else if (c == '\"')	/* JF this is a good idea */
		  code = '\"';
		else fatals("invalid literal token '\\%c'",c,0,0,0,0,0,0,0);
		c = getc(finput);
	      }
	  }
	else
	  {
	    code = c;
	    c = getc(finput);
	  }
	if (c != '\'')
	  fatal("multicharacter literal tokens NOT supported");

	/* now fill token_buffer with the canonical name for this character
	   as a literal token.  Do not use what the user typed,
	   so that '\012' and '\n' can be interchangeable.  */

	p = token_buffer;
	*p++ = '\'';
	if (code == '\\')
	  {
	    p = token_buffer + 1;
	    *p++ = '\\';
	    *p++ = '\\';
	  }
	else if (code == '\'')
	  {
	    p = token_buffer + 1;
	    *p++ = '\\';
	    *p++ = '\'';
	  }
	else if (code >= 040 && code != 0177)
	  *p++ = code;
	else if (code == '\t')
	  {
	    p = token_buffer + 1;
	    *p++ = '\\';
	    *p++ = 't';
	  }
	else if (code == '\n')
	  {
	    p = token_buffer + 1;
	    *p++ = '\\';
	    *p++ = 'n';
	  }
	else if (code == '\r')
	  {
	    p = token_buffer + 1;
	    *p++ = '\\';
	    *p++ = 'r';
	  }
	else if (code == '\b')
	  {
	    p = token_buffer + 1;
	    *p++ = '\\';
	    *p++ = 'b';
	  }
	else if (code == '\f')
	  {
	    p = token_buffer + 1;
	    *p++ = '\\';
	    *p++ = 'f';
	  }
        else
	  {
	    *p++ = code / 0100 + '0';
	    *p++ = ((code / 010) & 07) + '0';
	    *p++ = (code & 07) + '0';
	  }
	*p++ = '\'';
	*p = 0;
	symval = getsym(token_buffer);
	symval->class = STOKEN;
	if (! symval->user_token_number)
	  symval->user_token_number = code;
	return (IDENTIFIER);
      }

    case ',':
      return (COMMA);

    case ':':
      return (COLON);

    case ';':
      return (SEMICOLON);

    case '|':
      return (BAR);

    case '{':
      return (LEFT_CURLY);

    case '=':
      do
	{
	  c = getc(finput);
	  if (c == '\n') lineno++;
	}
      while(c==' ' || c=='\n' || c=='\t');

      if (c == '{')
      	return(LEFT_CURLY);
      else
	{
	  ungetc(c, finput);
	  return(ILLEGAL);
	}

    case '<':
      p = token_buffer;
      c = getc(finput);
      while (c != '>')
	{
	  if (c == '\n' || c == EOF)
	    fatal("unterminated type name");

	  if (p >= token_buffer + MAXTOKEN - 1)
	    fatals("type name too long (%d max)",MAXTOKEN-1,0,0,0,0,0,0,0);

	  *p++ = c;
	  c = getc(finput);
	}
      *p = 0;
      return (TYPENAME);
	    

    case '%':
      return (parse_percent_token());

    default:
      return (ILLEGAL);
    }
}


/* parse a token which starts with %.  Assumes the % has already been read and discarded.  */

int parse_percent_token(void)
{
  register int c;
  register char *p;

  p = token_buffer;
  c = getc(finput);

  switch (c)
    {
    case '%':
      return (TWO_PERCENTS);

    case '{':
      return (PERCENT_LEFT_CURLY);

    case '<':
      return (LEFT);

    case '>':
      return (RIGHT);

    case '2':
      return (NONASSOC);

    case '0':
      return (TOKEN);

    case '=':
      return (PREC);
    }
  if (!isalpha(c))
    return (ILLEGAL);

  while (isalpha(c) || c == '_')
    {
      if (p < token_buffer + MAXTOKEN)
	*p++ = c;
      c = getc(finput);
    }

  ungetc(c, finput);

  *p = 0;

  if (strcmp(token_buffer, "token") == 0
      ||
      strcmp(token_buffer, "term") == 0)
    return (TOKEN);
  else if (strcmp(token_buffer, "nterm") == 0)
    return (NTERM);
  else if (strcmp(token_buffer, "type") == 0)
    return (TYPE);
  else if (strcmp(token_buffer, "guard") == 0)
    return (GUARD);
  else if (strcmp(token_buffer, "union") == 0)
    return (UNION);
  else if (strcmp(token_buffer, "expect") == 0)
    return (EXPECT);
  else if (strcmp(token_buffer, "start") == 0)
    return (START);
  else if (strcmp(token_buffer, "left") == 0)
    return (LEFT);
  else if (strcmp(token_buffer, "right") == 0)
    return (RIGHT);
  else if (strcmp(token_buffer, "nonassoc") == 0
	   ||
	   strcmp(token_buffer, "binary") == 0)
    return (NONASSOC);
  else if (strcmp(token_buffer, "semantic_parser") == 0)
    return (SEMANTIC_PARSER);
  else if (strcmp(token_buffer, "pure_parser") == 0)
    return (PURE_PARSER);
  else if (strcmp(token_buffer, "prec") == 0)
    return (PREC);
  else return (ILLEGAL);
}
