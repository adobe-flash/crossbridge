/* Input parser for bison
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

/* read in the grammar specification and record it in the format described in gram.h.
  All guards are copied into the fguard file and all actions into faction,
  in each case forming the body of a C function (yyguard or yyaction)
  which contains a switch statement to decide which guard or action to execute.

The entry point is reader().  */

#include <stdio.h>
#include <ctype.h>
#include "files.h"
#include "new.h"
#include "symtab.h"
#include "lex.h"
#include "gram.h"


static const char *mybasename(const char *str) {
  const char *base = strrchr(str, '/');
  return base ? base+1 : str;
}

#define	LTYPESTR	"\n#ifndef YYLTYPE\ntypedef\n  struct yyltype\n\
    {\n      int first_line;\n      int first_column;\n\
      int last_line;\n      int last_column;\n    }\n\
  yyltype;\n\n#define YYLTYPE yyltype\n#endif\n\n"

/* Number of slots allocated (but not necessarily used yet) in `rline'  */
int rline_allocated;

extern int definesflag;
extern int nolinesflag;
extern bucket *symval;
extern int numval;
extern int failure;
extern int expected_conflicts;

typedef
  struct symbol_list
    {
      struct symbol_list *next;
      bucket *sym;
      bucket *ruleprec;
    }
  symbol_list;



int lineno;
symbol_list *grammar;
int start_flag;
bucket *startval;
char **tags;

static int typed;  /* nonzero if %union has been seen.  */

static int lastprec;  /* incremented for each %left, %right or %nonassoc seen */

static int gensym_count;  /* incremented for each generated symbol */

static bucket *errtoken;

/* Nonzero if any action or guard uses the @n construct.  */
static int yylsp_needed;

extern void init_lex(void);
extern void tabinit(void);
extern void output_headers(void);
extern void output_trailers(void);
extern void free_symtab(void);
extern int skip_white_space(void);
extern int parse_percent_token(void);
extern void open_extra_files(void);
extern void fatal(char *s);
extern void fatals(char *fmt,int x1,int x2,int x3,int x4,int x5,int x6,
                   int x7,int x8);
extern int lex(void);
extern void done(int k);

void read_declarations(void);
void output_ltype(void);
void readgram(void);
void packsymbols(void);
void packgram(void);
void copy_definition(void);
void parse_token_decl(int what_is,int what_is_not);
void parse_type_decl(void);
void parse_start_decl(void);
void parse_union_decl(void);
void parse_expect_decl(void);
void parse_assoc_decl(int assoc);
int read_signed_integer(FILE *stream);
void copy_action(symbol_list *rule,int stack_offset);
void unlex(int token);
int get_type(void);
void record_rule_line(void);
void output_token_defines(FILE *file);

void reader(void)
{

  start_flag = 0;
  startval = NULL;  /* start symbol not specified yet. */

  translations = 0;  /* initially assume token number translation not needed.  */

  nsyms = 1;
  nvars = 0;
  nrules = 0;
  nitems = 0;
  rline_allocated = 10;
  rline = NEW2(rline_allocated, short);

  typed = 0;
  lastprec = 0;

  gensym_count = 0;

  semantic_parser = 0;
  pure_parser = 0;
  yylsp_needed = 0;

  grammar = NULL;

  init_lex();
  lineno = 1;

  /* initialize the symbol table.  */
  tabinit();
  /* construct the error token */
  errtoken = getsym("error");
  errtoken->class = STOKEN;
  /* construct a token that represents all undefined literal tokens. */
  /* it is always token number 2.  */
  getsym("$illegal.")->class = STOKEN;
  /* Read the declaration section.  Copy %{ ... %} groups to ftable and fdefines file.
     Also notice any %token, %left, etc. found there.  */
  fprintf(ftable, "\n/*  A Bison parser, made from %s  */\n\n", mybasename(infile));
  read_declarations();
  /* output the definition of YYLTYPE into the fattrs and fdefines files.  */
  output_ltype();
  /* start writing the guard and action files, if they are needed.  */
  output_headers();
  /* read in the grammar, build grammar in list form.  write out guards and actions.  */
  readgram();
  /* write closing delimiters for actions and guards.  */
  output_trailers();
  if (yylsp_needed)
    fprintf(ftable, "#define YYLSP_NEEDED\n\n");
  /* assign the symbols their symbol numbers.
     Write #defines for the token symbols into fdefines if requested.  */
  packsymbols();
  /* convert the grammar into the format described in gram.h.  */
  packgram();
  /* free the symbol table data structure
     since symbols are now all referred to by symbol number.  */
  free_symtab();
}



/* read from finput until %% is seen.  Discard the %%.
Handle any % declarations,
and copy the contents of any %{ ... %} groups to fattrs.  */

void read_declarations(void)
{
  register int c;
  register int tok;

  for (;;)
    {
      c = skip_white_space();

      if (c == '%')
	{
	  tok = parse_percent_token();

	  switch (tok)
	    {
	    case TWO_PERCENTS:
	      return;

	    case PERCENT_LEFT_CURLY:
	      copy_definition();
	      break;

	    case TOKEN:
	      parse_token_decl (STOKEN, SNTERM);
	      break;
	
	    case NTERM:
	      parse_token_decl (SNTERM, STOKEN);
	      break;
	
	    case TYPE:
	      parse_type_decl();
	      break;
	
	    case START:
	      parse_start_decl();
	      break;
	
	    case UNION:
	      parse_union_decl();
	      break;
	
	    case EXPECT:
	      parse_expect_decl();
	      break;
	
	    case LEFT:
	      parse_assoc_decl(LEFT_ASSOC);
	      break;

	    case RIGHT:
	      parse_assoc_decl(RIGHT_ASSOC);
	      break;

	    case NONASSOC:
	      parse_assoc_decl(NON_ASSOC);
	      break;

	    case SEMANTIC_PARSER:
	      semantic_parser = 1;
	      open_extra_files();
	      break;

	    case PURE_PARSER:
	      pure_parser = 1;
	      break;

	    default:
	      fatal("junk after % in definition section");
	    }
	}
      else if (c == EOF)
        fatal("no input grammar");
      else/* JF changed msg */
        fatals("Unrecognized char '%c' in declaration section",c,0,0,0,0,0,
               0,0);

    }
}


/* copy the contents of a %{ ... %} into the definitions file.
The %{ has already been read.  Return after reading the %}.  */
void copy_definition(void)
{
  register int c;
  register int match;
  register int ended;
  register int after_percent;  /* -1 while reading a character if prev char was % */

  if (!nolinesflag)
    fprintf(fattrs, "#line %d \"%s\"\n", lineno, mybasename(infile));

  after_percent = 0;

  c = getc(finput);

  for (;;)
    {
      switch (c)
	{
	case '\n':
	  putc(c, fattrs);
	  lineno++;
	  break;

	case '%':
          after_percent = -1;
	  break;
	      
	case '\'':
	case '"':
	  match = c;
	  putc(c, fattrs);
	  c = getc(finput);

	  while (c != match)
	    {
	      if (c == EOF || c == '\n')
		fatal("unterminated string");

	      putc(c, fattrs);
	      
	      if (c == '\\')
		{
		  c = getc(finput);
		  if (c == EOF || c == '\n')
		    fatal("unterminated string");
		  putc(c, fattrs);
		  if (c == '\n')
		    lineno++;
		}

	      c = getc(finput);
	    }

	  putc(c, fattrs);
	  break;

	case '/':
	  putc(c, fattrs);
	  c = getc(finput);
	  if (c != '*')
	    continue;

	  putc(c, fattrs);
	  c = getc(finput);

	  ended = 0;
	  while (!ended)
	    {
	      if (c == '*')
		{
		  while (c == '*')
		    {
		      putc(c, fattrs);
		      c = getc(finput);
		    }

		  if (c == '/')
		    {
		      putc(c, fattrs);
		      ended = 1;
		    }
		}
	      else if (c == '\n')
		{
		  lineno++;
		  putc(c, fattrs);
		  c = getc(finput);
		}
	      else if (c == EOF)
		fatal("unterminated comment in %{ definition");
	      else
		{
		  putc(c, fattrs);
		  c = getc(finput);
		}
	    }

	  break;

	case EOF:
	  fatal("unterminated %{ definition");

	default:
	  putc(c, fattrs);
	}

      c = getc(finput);

      if (after_percent)
	{
	  if (c == '}')
	    return;
	  putc('%', fattrs);
	}
      after_percent = 0;

    }

}

/* parse what comes after %token or %nterm.
For %token, what_is is STOKEN and what_is_not is SNTERM.
For %nterm, the arguments are reversed.  */

void parse_token_decl(int what_is,int what_is_not)
{
/*   register int start_lineno; JF */
  register int token = 0;
  register int prev;
  register char *typename = 0;
  int k;
  extern char token_buffer[];

/*   start_lineno = lineno; JF */

  for (;;)
    {
      if(ungetc(skip_white_space(), finput) == '%')
	return;

/*      if (lineno != start_lineno)
	return; JF */

      /* we have not passed a newline, so the token now starting is in this declaration */
      prev = token;

      token = lex();
      if (token == COMMA)
	continue;
      if (token == TYPENAME)
	{
	  k = strlen(token_buffer);
	  if (typename) free (typename);
	  typename = NEW2(k + 1, char);
	  strcpy(typename, token_buffer);
	}
      else if (token == IDENTIFIER)
	{
	  if (symval->class == what_is_not)
	    fatals("symbol %s redefined", symval->tag,0,0,0,0,0,0,0);
	  symval->class = what_is;
	  if (what_is == SNTERM)
	    symval->value = nvars++;

	  if (typename)
	    {
	      if (symval->type_name == NULL)
		symval->type_name = typename;
	      else
		fatals("type redeclaration for %s", symval->tag,0,0,0,0,0,0,0);
	    }
	}
      else if (prev == IDENTIFIER && token == NUMBER)
        {
	  symval->user_token_number = numval;
	  translations = 1;
        }
      else
	fatal("invalid text in %token or %nterm declaration");
    }

}

/* parse what comes after %start */

void parse_start_decl(void)
{
  if (start_flag)
    fatal("multiple %start declarations");
  start_flag = 1;
  if (lex() != IDENTIFIER)
    fatal("invalid %start declaration");
  startval = symval;
}

/* read in a %type declaration and record its information for get_type_name to access */

void parse_type_decl(void)
{
  register int k;
  register char *name;
/*   register int start_lineno; JF */

  extern char token_buffer[];

  if (lex() != TYPENAME)
    fatal("ill-formed %type declaration");

  k = strlen(token_buffer);
  name = NEW2(k + 1, char);
  strcpy(name, token_buffer);

/*   start_lineno = lineno; */

  for (;;)
    {
      register int t;

      if(ungetc(skip_white_space(), finput) == '%')
	return;

/*       if (lineno != start_lineno)
	return; JF */

      /* we have not passed a newline, so the token now starting is in this declaration */

      t = lex();

      switch (t)
	{

	case COMMA:
	  break;

	case IDENTIFIER:
	  if (symval->type_name == NULL)
	    symval->type_name = name;
	  else
	    fatals("type redeclaration for %s", symval->tag,0,0,0,0,0,0,0);

	  break;

	default:
	  fatal("invalid %type declaration");
	}
    }
}

/* read in a %left, %right or %nonassoc declaration and record its information.  */
/* assoc is either LEFT_ASSOC, RIGHT_ASSOC or NON_ASSOC.  */

void parse_assoc_decl(int assoc)
{
  register int k;
  register char *name = NULL;
/*  register int start_lineno; JF */
  register int prev = 0;	/* JF added = 0 to keep lint happy */

  extern char token_buffer[];

  lastprec++;  /* assign a new precedence level.  */

/*   start_lineno = lineno; */

  for (;;)
    {
      register int t;

      if(ungetc(skip_white_space(), finput) == '%')
	return;

      /* if (lineno != start_lineno)
	return; JF */

      /* we have not passed a newline, so the token now starting is in this declaration */

      t = lex();

      switch (t)
	{

	case TYPENAME:
	  k = strlen(token_buffer);
	  name = NEW2(k + 1, char);
	  strcpy(name, token_buffer);
	  break;

	case COMMA:
	  break;

	case IDENTIFIER:
	  symval->prec = lastprec;
	  symval->assoc = assoc;
	  if (symval->class == SNTERM)
	    fatals("symbol %s redefined", symval->tag,0,0,0,0,0,0,0);
	  symval->class = STOKEN;
	  if (name)
	    { /* record the type, if one is specified */
	      if (symval->type_name == NULL)
		symval->type_name = name;
	      else
		fatals("type redeclaration for %s", symval->tag,0,0,0,0,0,0,0);
	    }
	  break;

	case NUMBER:
	  if (prev == IDENTIFIER)
            {
	      symval->user_token_number = numval;
	      translations = 1;
            }
          else	  
	    fatal("invalid text in association declaration");
	  break;

	case SEMICOLON:
	  return;

	default:
	  fatal("malformatted association declaration");
	}

      prev = t;

    }
}



/* copy the union declaration into fattrs (and fdefines),
   where it is made into the
   definition of YYSTYPE, the type of elements of the parser value stack.  */

void parse_union_decl(void)
{
  register int c;
  register int count;
  register int in_comment;

  if (typed)
    fatal("multiple %union declarations");

  typed = 1;

  if (!nolinesflag)
    fprintf(fattrs, "\n#line %d \"%s\"\n", lineno, mybasename(infile));
  else
    fprintf(fattrs, "\n");

  fprintf(fattrs, "typedef union");
  if (fdefines)
    fprintf(fdefines, "typedef union");

  count = 0;
  in_comment = 0;

  c = getc(finput);

  while (c != EOF)
    {
      putc(c, fattrs);
      if (fdefines)
	putc(c, fdefines);

      switch (c)
	{
	case '\n':
	  lineno++;
	  break;

	case '/':
	  c = getc(finput);
	  if (c != '*')
	    ungetc(c, finput);
	  else
	    {
	      putc('*', fattrs);
	      if (fdefines)
		putc('*', fdefines);
	      c = getc(finput);
	      in_comment = 1;
	      while (in_comment)
		{
		  if (c == EOF)
		    fatal("unterminated comment");

		  putc(c, fattrs);
		  if (fdefines)
		    putc(c, fdefines);
		  if (c == '*')
		    {
		      c = getc(finput);
		      if (c == '/')
			{
			  putc('/', fattrs);
			  if (fdefines)
			    putc('/', fdefines);
			  in_comment = 0;
			}
		    }
		  else
		    c = getc(finput);
		}
	    }
	  break;


	case '{':
	  count++;
	  break;

	case '}':
	  count--;
	  if (count == 0)
	    {
	      fprintf(fattrs, " YYSTYPE;\n");
	      if (fdefines)
		fprintf(fdefines, " YYSTYPE;\n");
	      /* JF don't choke on trailing semi */
	      c=skip_white_space();
	      if(c!=';') ungetc(c,finput);
	      return;
	    }
	}

      c = getc(finput);
    }
}

/* parse the declaration %expect N which says to expect N
   shift-reduce conflicts.  */

void parse_expect_decl(void)
{
  register int c;
  register int count;
  char buffer[20];

  c = getc(finput);
  while (c == ' ' || c == '\t')
    c = getc(finput);

  count = 0;
  while (c >= '0' && c <= '9')
    {
      if (count < 20)
	buffer[count++] = c;
      c = getc(finput);
    }

  ungetc (c, finput);

  expected_conflicts = atoi (buffer);
}

/* that's all of parsing the declaration section */

void output_ltype(void)
{
  fprintf(fattrs, LTYPESTR);/* JF added YYABORT() */
  if (fdefines)
    fprintf(fdefines, LTYPESTR);/* JF added YYABORT() */

  fprintf(fattrs, "#define\tYYACCEPT\tgoto yyaccept\n");
  fprintf(fattrs, "#define\tYYABORT\tgoto yyabort\n");
  fprintf(fattrs, "#define\tYYERROR\tgoto yyerrlab\n");
  fprintf(fattrs, "#define\tyytext\t(*_yytext)\nextern char **_yytext;\n");
  fprintf(fattrs, "#define\tyylval\t(*_yylval)\nextern YYSTYPE *_yylval;\n");
  fprintf(fattrs, "#define\tyylloc\t(*_yylloc)\nextern YYLTYPE *_yylloc;\n");
  fprintf(fattrs, "#define\tyyppval\t(*_yyppval)\nextern int *_yyppval;\n");

  if (fdefines)
    {
      fprintf(fdefines, "#define\tYYACCEPT\tgoto yyaccept\n");
      fprintf(fdefines, "#define\tYYABORT\tgoto yyabort\n");
      fprintf(fdefines, "#define\tYYERROR\tgoto yyerrlab\n");
      fprintf(fdefines, "#define\tyytext\t(*_yytext)\nextern char **_yytext;\n");
      fprintf(fdefines, "#define\tyylval\t(*_yylval)\nextern YYSTYPE *_yylval;\n");
      fprintf(fdefines, "#define\tyylloc\t(*_yylloc)\nextern YYLTYPE *_yylloc;\n");
      fprintf(fdefines, "#define\tyyppval\t(*_yyppval)\nextern int *_yyppval;\n");
    }
}



/* Get the data type (alternative in the union) of the value for symbol n in rule rule.  */

char *get_type_name(int n,symbol_list *rule)
{
  static char *msg = "invalid $ value";

  register int i;
  register symbol_list *rp;

  if (n < 0)
    fatal(msg);

  rp = rule;
  i = 0;

  while (i < n)
    {
      rp = rp->next;
      if (rp == NULL || rp->sym == NULL)
	fatal(msg);
      i++;
    }

  return (rp->sym->type_name);
}



/* after %guard is seen in the input file,
copy the actual guard into the guards file.
If the guard is followed by an action, copy that into the actions file.
stack_offset is the number of values in the current rule so far,
which says where to find $0 with respect to the top of the stack,
for the simple parser in which the stack is not popped until after the guard is run.  */

void copy_guard(symbol_list *rule,int stack_offset)
{
  register int c;
  register int n;
  register int count;
  register int match;
  register int ended;
  register char *type_name;
  int brace_flag = 0;

  extern char token_buffer[];

  /* offset is always 0 if parser has already popped the stack pointer */
  if (semantic_parser) stack_offset = 0;

  fprintf(fguard, "\ncase %d:\n", nrules);
  if (!nolinesflag)
    fprintf(fguard, "#line %d \"%s\"\n", lineno, mybasename(infile));
  putc('{', fguard);

  count = 0;
  c = getc(finput);

  while (brace_flag ? (count > 0) : (c != ';'))
    {
      switch (c)
	{
	case '\n':
	  putc(c, fguard);
	  lineno++;
	  break;

	case '{':
	  putc(c, fguard);
	  count++;
	  break;

	case '}':
	  putc(c, fguard);
	  brace_flag = 1;
	  if (count > 0)
	    count--;
	  else
	    fatal("unmatched right brace ('}')");
          break;

	case '\'':
	case '"':
	  match = c;
	  putc(c, fguard);
	  c = getc(finput);

	  while (c != match)
	    {
	      if (c == EOF || c == '\n')
		fatal("unterminated string");

	      putc(c, fguard);
	      
	      if (c == '\\')
		{
		  c = getc(finput);
		  if (c == EOF || c == '\n')
		    fatal("unterminated string");
		  putc(c, fguard);
		  if (c == '\n')
		    lineno++;
		}

	      c = getc(finput);
	    }

	  putc(c, fguard);
	  break;

	case '/':
	  putc(c, fguard);
	  c = getc(finput);
	  if (c != '*')
	    continue;

	  putc(c, fguard);
	  c = getc(finput);

	  ended = 0;
	  while (!ended)
	    {
	      if (c == '*')
		{
		  while (c == '*')
		    {
		      putc(c, fguard);
		      c = getc(finput);
		    }

		  if (c == '/')
		    {
		      putc(c, fguard);
		      ended = 1;
		    }
		}
	      else if (c == '\n')
		{
		  lineno++;
		  putc(c, fguard);
		  c = getc(finput);
		}
	      else if (c == EOF)
		fatal("unterminated comment");
	      else
		{
		  putc(c, fguard);
		  c = getc(finput);
		}
	    }

	  break;

	case '$':
	  c = getc(finput);
	  type_name = NULL;

	  if (c == '<')
	    {
	      register char *cp = token_buffer;

	      while ((c = getc(finput)) != '>' && c > 0)
		*cp++ = c;
	      *cp = 0;
	      type_name = token_buffer;

	      c = getc(finput);
	    }

	  if (c == '$')
	    {
	      fprintf(fguard, "yyval");
	      if (!type_name) type_name = rule->sym->type_name;
	      if (type_name)
		fprintf(fguard, ".%s", type_name);
	      if(!type_name && typed)	/* JF */
		fprintf(stderr,"%s:%d:  warning:  $$ of '%s' has no declared type.\n",infile,lineno,rule->sym->tag);
	    }

	  else if (isdigit(c) || c == '-')
	    {
	      ungetc (c, finput);
	      n = read_signed_integer(finput);
	      c = getc(finput);

	      if (!type_name && n > 0)
		type_name = get_type_name(n, rule);

	      fprintf(fguard, "yyp->yyvsp[%d]", n - stack_offset);
	      if (type_name)
		fprintf(fguard, ".%s", type_name);
	      if(!type_name && typed)	/* JF */
		fprintf(stderr,"%s:%d:  warning:  $%d of '%s' has no declared type.\n",infile,lineno,n,rule->sym->tag);
	      continue;
	    }
	  else
	    fatals("$%c is invalid",c,0,0,0,0,0,0,0);	/* JF changed style */

	  break;

	case '@':
	  c = getc(finput);
	  if (isdigit(c) || c == '-')
	    {
	      ungetc (c, finput);
	      n = read_signed_integer(finput);
	      c = getc(finput);
	    }
	  else
	    fatals("@%c is invalid",c,0,0,0,0,0,0,0);	/* JF changed style */

	  fprintf(fguard, "yyp->yylsp[%d]", n - stack_offset);
	  yylsp_needed = 1;

	  continue;

	case EOF:
	  fatal("unterminated %guard clause");

	default:
	  putc(c, fguard);
	}

      if (c != '}' || count != 0)
	c = getc(finput);
    }

  c = skip_white_space();

  fprintf(fguard, ";\n    break;}");
  if (c == '{')
    copy_action(rule, stack_offset);
  else if (c == '=')
    {
      c = getc(finput);
      if (c == '{')
	copy_action(rule, stack_offset);
    }
  else
    ungetc(c, finput);
}



/* Assuming that a { has just been seen, copy everything up to the matching }
into the actions file.
stack_offset is the number of values in the current rule so far,
which says where to find $0 with respect to the top of the stack.  */

void copy_action(symbol_list *rule,int stack_offset)
{
  register int c;
  register int n;
  register int count;
  register int match;
  register int ended;
  register char *type_name;
  extern char token_buffer[];

  /* offset is always 0 if parser has already popped the stack pointer */
  if (semantic_parser) stack_offset = 0;

  fprintf(faction, "\ncase %d:\n", nrules);
  if (!nolinesflag)
    fprintf(faction, "#line %d \"%s\"\n", lineno, mybasename(infile));
  putc('{', faction);

  count = 1;
  c = getc(finput);

  while (count > 0)
    {
      while (c != '}')
        {
          switch (c)
	    {
	    case '\n':
	      putc(c, faction);
	      lineno++;
	      break;

	    case '{':
	      putc(c, faction);
	      count++;
	      break;

	    case '\'':
	    case '"':
	      match = c;
	      putc(c, faction);
	      c = getc(finput);

	      while (c != match)
		{
		  if (c == EOF || c == '\n')
		    fatal("unterminated string");

		  putc(c, faction);

		  if (c == '\\')
		    {
		      c = getc(finput);
		      if (c == EOF)
			fatal("unterminated string");
		      putc(c, faction);
		      if (c == '\n')
			lineno++;
		    }

		  c = getc(finput);
		}

	      putc(c, faction);
	      break;

	    case '/':
	      putc(c, faction);
	      c = getc(finput);
	      if (c != '*')
		continue;

	      putc(c, faction);
	      c = getc(finput);

	      ended = 0;
	      while (!ended)
		{
		  if (c == '*')
		    {
		      while (c == '*')
		        {
			  putc(c, faction);
			  c = getc(finput);
			}

		      if (c == '/')
			{
			  putc(c, faction);
			  ended = 1;
			}
		    }
		  else if (c == '\n')
		    {
		      lineno++;
		      putc(c, faction);
		      c = getc(finput);
		    }
		  else if (c == EOF)
		    fatal("unterminated comment");
		  else
		    {
		      putc(c, faction);
		      c = getc(finput);
		    }
		}

	      break;

	    case '$':
	      c = getc(finput);
	      type_name = NULL;

	      if (c == '<')
		{
		  register char *cp = token_buffer;

		  while ((c = getc(finput)) != '>' && c > 0)
		    *cp++ = c;
		  *cp = 0;
		  type_name = token_buffer;

		  c = getc(finput);
		}
	      if (c == '$')
		{
		  fprintf(faction, "yyval");
		  if (!type_name) type_name = get_type_name(0, rule);
		  if (type_name)
		    fprintf(faction, ".%s", type_name);
		  if(!type_name && typed)	/* JF */
		    fprintf(stderr,"%s:%d:  warning:  $$ of '%s' has no declared type.\n",infile,lineno,rule->sym->tag);
		}
	      else if (isdigit(c) || c == '-')
		{
		  ungetc (c, finput);
		  n = read_signed_integer(finput);
		  c = getc(finput);

		  if (!type_name && n > 0)
		    type_name = get_type_name(n, rule);

		  fprintf(faction, "yyp->yyvsp[%d]", n - stack_offset);
		  if (type_name)
		    fprintf(faction, ".%s", type_name);
		  if(!type_name && typed)	/* JF */
		    fprintf(stderr,"%s:%d:  warning:  $%d of '%s' has no declared type.\n",infile,lineno,n,rule->sym->tag);
		  continue;
		}
	      else
		fatals("$%c is invalid",c,0,0,0,0,0,0,0);
              /* JF changed format */

	      break;

	    case '@':
	      c = getc(finput);
	      if (isdigit(c) || c == '-')
		{
		  ungetc (c, finput);
		  n = read_signed_integer(finput);
		  c = getc(finput);
		}
	      else
		fatal("invalid @-construct");

	      fprintf(faction, "yyp->yylsp[%d]", n - stack_offset);
	      yylsp_needed = 1;

	      continue;

	    case EOF:
	      fatal("unmatched '{'");

	    default:
	      putc(c, faction);
	    }

          c = getc(finput);
        }

      /* above loop exits when c is '}' */

      if (--count)
        {
	  putc(c, faction);
	  c = getc(finput);
	}
    }

  fprintf(faction, ";\n    break;}");
}



/* generate a dummy symbol, a nonterminal,
whose name cannot conflict with the user's names. */

bucket *gensym(void)
{
  register bucket *sym;

  extern char token_buffer[];
  sprintf (token_buffer, "@%d", ++gensym_count);
  sym = getsym(token_buffer);
  sym->class = SNTERM;
  sym->value = nvars++;
  return (sym);
}

/* Parse the input grammar into a one symbol_list structure.
Each rule is represented by a sequence of symbols: the left hand side
followed by the contents of the right hand side, followed by a null pointer
instead of a symbol to terminate the rule.
The next symbol is the lhs of the following rule.

All guards and actions are copied out to the appropriate files,
labelled by the rule number they apply to.  */

void readgram(void)
{
  register int t;
  register bucket *lhs;
  register symbol_list *p;
  register symbol_list *p1;
  register bucket *bp;

  symbol_list *crule;	/* points to first symbol_list of current rule.  */
			/* its symbol is the lhs of the rule.   */
  symbol_list *crule1;  /* points to the symbol_list preceding crule.  */

  p1 = NULL;

  t = lex();

  while (t != TWO_PERCENTS && t != ENDFILE)
    {
      if (t == IDENTIFIER || t == BAR)
	{
	  register int actionflag = 0;
	  int rulelength = 0;  /* number of symbols in rhs of this rule so far  */
	  int xactions = 0;	/* JF for error checking */
	  bucket *first_rhs = 0;

	  if (t == IDENTIFIER)
	    {
	      lhs = symval;
    
	      t = lex();
	      if (t != COLON)
		fatal("ill-formed rule");
	    }

	  if (nrules == 0)
	    {
	      if (t == BAR)
		fatal("grammar starts with vertical bar");

	      if (!start_flag)
		startval = lhs;
	    }

	  /* start a new rule and record its lhs.  */

	  nrules++;
	  nitems++;

	  record_rule_line ();

	  p = NEW(symbol_list);
	  p->sym = lhs;

	  crule1 = p1;
	  if (p1)
	    p1->next = p;
	  else
	    grammar = p;

	  p1 = p;
	  crule = p;

	  /* mark the rule's lhs as a nonterminal if not already so.  */

	  if (lhs->class == SUNKNOWN)
	    {
	      lhs->class = SNTERM;
	      lhs->value = nvars;
	      nvars++;
	    }
	  else if (lhs->class == STOKEN)
	    fatals("rule given for %s, which is a token", lhs->tag,0,0,0,0,
                   0,0,0);

	  /* read the rhs of the rule.  */

	  for (;;)
	    {
	      t = lex();

	      if (! (t == IDENTIFIER || t == LEFT_CURLY)) break;

	      /* if next token is an identifier, see if a colon follows it.
		 If one does, exit this rule now.  */
	      if (t == IDENTIFIER)
		{
		  register bucket *ssave;
		  register int t1;

		  ssave = symval;
		  t1 = lex();
		  unlex(t1);
		  symval = ssave;
		  if (t1 == COLON) break;

		  if(!first_rhs)	/* JF */
		  	first_rhs = symval;
		  /* not followed by colon => process as part of this rule's rhs.  */
		  if (actionflag)
		    {
		      register bucket *sdummy;

		      /* if this symbol was preceded by an action, */
		      /* make a dummy nonterminal to replace that action in this rule */
		      /* and make another rule to associate the action to the dummy.  */
		      /* Since the action was written out with this rule's number, */
		      /* we must write give the new rule this number */
		      /* by inserting the new rule before it.  */

		      /* make a dummy nonterminal, a gensym.  */
		      sdummy = gensym();

		      /* make a new rule, whose body is empty, before the current one.  */
		      /* so that the action just read can belong to it.  */
		      nrules++;
		      nitems++;
		      record_rule_line ();
		      p = NEW(symbol_list);
		      if (crule1)
			crule1->next = p;
		      else grammar = p;
		      p->sym = sdummy;
		      crule1 = NEW(symbol_list);
		      p->next = crule1;
		      crule1->next = crule;
		      
		      /* insert the dummy generated by that rule into this rule.  */
		      nitems++;
		      p = NEW(symbol_list);
		      p->sym = sdummy;
		      p1->next = p;
		      p1 = p;

		      actionflag = 0;
		    }
		  nitems++;
		  p = NEW(symbol_list);
		  p->sym = symval;
		  p1->next = p;
		  p1 = p;
		}
	      else /* handle an action.  */
		{
		  copy_action(crule, rulelength);
		  actionflag = 1;
		  xactions++;	/* JF */
		}
	      rulelength++;
	    }

	  /* Put an empty link in the list to mark the end of this rule  */
	  p = NEW(symbol_list);
	  p1->next = p;
	  p1 = p;

	  if (t == PREC)
	    {
	      t = lex();
	      crule->ruleprec = symval;
	      t = lex();
	    }
	  if (t == GUARD)
	    {
	      if (! semantic_parser)
		fatal("%guard present but %semantic_parser not specified");

	      copy_guard(crule, rulelength);
	      t = lex();
	    }
	  else if (t == LEFT_CURLY)
	    {
	      if (actionflag) fatal("two actions at end of one rule");
	      copy_action(crule, rulelength);
	      t = lex();
	    }
	  /* JF if we'd end up using default, get a warning */
	  else if(!xactions && first_rhs && lhs->type_name!=first_rhs->type_name) {
	    if(lhs->type_name == 0 || first_rhs->type_name == 0 ||
				      strcmp(lhs->type_name,first_rhs->type_name))
	      fprintf(stderr,"%s:%d:  warning:  type clash ('%s' '%s') on default action\n",
		      infile,
		      lineno,
		      lhs->type_name ? lhs->type_name : "",
		      first_rhs->type_name ? first_rhs->type_name : "");
	  }
	  if (t == SEMICOLON)
	    t = lex();
	}
      /* these things can appear as alternatives to rules.  */
      else if (t == TOKEN)
	{
	  parse_token_decl(STOKEN, SNTERM);
	  t = lex();
	}
      else if (t == NTERM)
	{
	  parse_token_decl(SNTERM, STOKEN);
	  t = lex();
	}
      else if (t == TYPE)
	{
	  t = get_type();
	}
      else if (t == UNION)
	{
	  parse_union_decl();
	  t = lex();
	}
      else if (t == EXPECT)
	{
	  parse_expect_decl();
	  t = lex();
	}
      else if (t == START)
	{
	  parse_start_decl();
	  t = lex();
	}
      else
	fatal("invalid input");
    }

  if (nrules == 0)
    fatal("no input grammar");

  if (typed == 0)/* JF put out same default YYSTYPE as YACC does */
    {
      fprintf(fattrs, "#ifndef YYSTYPE\n#define YYSTYPE int\n#endif\n");
      if (fdefines)
	fprintf(fdefines, "#ifndef YYSTYPE\n#define YYSTYPE int\n#endif\n");
    }

  /* Report any undefined symbols and consider them nonterminals.  */

  for (bp = firstsymbol; bp; bp = bp->next)
    if (bp->class == SUNKNOWN)
      {
	fprintf(stderr, "symbol %s used, not defined as token, and no rules for it\n",
			bp->tag);
	failure = 1;
	bp->class = SNTERM;
	bp->value = nvars++;
      }

  ntokens = nsyms - nvars;
}


void record_rule_line(void)
{
  /* Record each rule's source line number in rline table.  */

  if (nrules >= rline_allocated)
    {
      rline_allocated = nrules * 2;
      rline = (short *) realloc (rline,
				 rline_allocated * sizeof (short));
      if (rline == 0)
	{
	  fprintf (stderr, "bison: memory exhausted\n");
	  done (1);
	}
    }
  rline[nrules] = lineno;
}

/* read in a %type declaration and record its information for get_type_name to access */

int get_type(void)
{
  register int k;
  register int t;
  register char *name;

  extern char token_buffer[];

  t = lex();

  if (t != TYPENAME)
    fatal("ill-formed %type declaration");

  k = strlen(token_buffer);
  name = NEW2(k + 1, char);
  strcpy(name, token_buffer);

  for (;;)
    {
      t = lex();

      switch (t)
	{
	case SEMICOLON:
	  return (lex());

	case COMMA:
	  break;

	case IDENTIFIER:
	  if (symval->type_name == NULL)
	    symval->type_name = name;
	  else
	    fatals("type redeclaration for %s", symval->tag,0,0,0,0,0,0,0);

	  break;

	default:
	  return (t);
	}
    }
}

/* assign symbol numbers, and write definition of token names into fdefines.
Set up vectors tags and sprec of names and precedences of symbols.  */

void packsymbols(void)
{
  register bucket *bp;
  register int tokno = 1;
  register int i;
  register int last_user_token_number;

  /* int lossage = 0; JF set but not used */

  tags = NEW2(nsyms + 1, char *);
  tags[0] = "$";

  sprec = NEW2(nsyms, short);
  sassoc = NEW2(nsyms, short);

  max_user_token_number = 255;
  last_user_token_number = 255;

  for (bp = firstsymbol; bp; bp = bp->next)
    {
      if (bp->class == SNTERM)
	{
	  bp->value += ntokens;
	}
      else
	{
	  if (translations && !(bp->user_token_number))
	    bp->user_token_number = ++last_user_token_number;
	  if (bp->user_token_number > max_user_token_number)
	    max_user_token_number = bp->user_token_number;
	  bp->value = tokno++;
	}

      tags[bp->value] = bp->tag;
      sprec[bp->value] = bp->prec;
      sassoc[bp->value] = bp->assoc;

    }

  if (translations)
    {
      register int i;

      token_translations = NEW2(max_user_token_number+1, short);

      /* initialize all entries for literal tokens to 2,
	 the internal token number for $illegal., which represents all invalid inputs.  */
      for (i = 0; i <= max_user_token_number; i++)
        token_translations[i] = 2;      
    }

  for (bp = firstsymbol; bp; bp = bp->next)
    {
      if (bp->value >= ntokens) continue;
      if (translations)
	{
	  if (token_translations[bp->user_token_number] != 2)
	    {
	    	/* JF made this a call to fatals() */
	      fatals( "tokens %s and %s both assigned number %d",
			      tags[token_translations[bp->user_token_number]],
			      bp->tag,
			      bp->user_token_number,0,0,0,0,0);
	    }
	  token_translations[bp->user_token_number] = bp->value;
	}
    }

  error_token_number = errtoken->value;

  output_token_defines(ftable);

  if (startval->class == SUNKNOWN)
    fatals("the start symbol %s is undefined", startval->tag,0,0,0,0,0,0,0);
  else if (startval->class == STOKEN)
    fatals("the start symbol %s is a token", startval->tag,0,0,0,0,0,0,0);

  start_symbol = startval->value;

  if (definesflag)
    {
      output_token_defines(fdefines);

      if (semantic_parser)
	for (i = ntokens; i < nsyms; i++)
	  {
	    /* don't make these for dummy nonterminals made by gensym.  */
	    if (*tags[i] != '@')
	      fprintf(fdefines, "#define\tNT%s\t%d\n", tags[i], i);
	  }

      fclose(fdefines);
      fdefines = NULL;
    }
}
      
void output_token_defines(FILE *file)
{
  bucket *bp;

  for (bp = firstsymbol; bp; bp = bp->next)
    {
      if (bp->value >= ntokens) continue;

      /* For named tokens, but not literal ones, define the name.  */
      /* The value is the user token number.  */

      if ('\'' != *tags[bp->value] && bp != errtoken)
	{
	  register char *cp = tags[bp->value];
	  register char c;

	  /* Don't #define nonliteral tokens whose names contain periods.  */

	  while ((c = *cp++) && c != '.');
	  if (!c)
	    {
              fprintf(file, "#define\t%s\t%d\n", tags[bp->value],
			    (translations ? bp->user_token_number : bp->value));
	      if (semantic_parser)
                fprintf(file, "#define\tT%s\t%d\n", tags[bp->value],
			      bp->value);
	    }
	}
    }

  putc('\n', file);
}

/* convert the rules into the representation using rrhs, rlhs and ritems.  */

void packgram(void)
{
  register int itemno;
  register int ruleno;
  register symbol_list *p;
/*  register bucket *bp; JF unused */

  bucket *ruleprec;

  ritem = NEW2(nitems + 1, short);
  rlhs = NEW2(nrules, short) - 1;
  rrhs = NEW2(nrules, short) - 1;
  rprec = NEW2(nrules, short) - 1;
  rassoc = NEW2(nrules, short) - 1;

  itemno = 0;
  ruleno = 1;

  p = grammar;
  while (p)
    {
      rlhs[ruleno] = p->sym->value;
      rrhs[ruleno] = itemno;
      ruleprec = p->ruleprec;

      p = p->next;
      while (p && p->sym)
	{
	  ritem[itemno++] = p->sym->value;
	  /* a rule gets the precedence and associativity of the last token in it.  */
          if (p->sym->class == STOKEN)
	    {
	      rprec[ruleno] = p->sym->prec;
	      rassoc[ruleno] = p->sym->assoc;
	    }
	  if (p) p = p->next;
	}

      /* if this rule has a %prec, specified symbol's precedence replaces the default */
      if (ruleprec)
	{
          rprec[ruleno] = ruleprec->prec;
          rassoc[ruleno] = ruleprec->assoc;
	}

      ritem[itemno++] = -ruleno;
      ruleno++;

      if (p) p = p->next;
    }

  ritem[itemno] = 0;
}

/* Read a signed integer from STREAM and return its value.  */

int read_signed_integer(FILE *stream)
{
  register int c = getc(stream);
  register int sign = 1;
  register int n;

  if (c == '-')
    {
      c = getc(stream);
      sign = -1;
    }
  n = 0;
  while (isdigit(c))
    {
      n = 10*n + (c - '0');
      c = getc(stream);
    }

  ungetc(c, stream);

  return n * sign;
}
