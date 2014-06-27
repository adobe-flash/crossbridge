/*
 * main.c -- Expression tree constructors and main program for gawk. 
 */

/* 
 * Copyright (C) 1986, 1988, 1989 the Free Software Foundation, Inc.
 * 
 * This file is part of GAWK, the GNU implementation of the
 * AWK Progamming Language.
 * 
 * GAWK is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 1, or (at your option)
 * any later version.
 * 
 * GAWK is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GAWK; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "gawk.h"
#include "patchlevel.h"
#include <signal.h>

extern int yyparse();
extern void do_input();
extern int close_io();
extern void init_fields();
extern int getopt();
extern int re_set_syntax();
extern NODE *node();

static void usage();
static void set_fs();
static void init_vars();
static void init_args();
static NODE *spc_var();
static void pre_assign();
static void copyleft();

/* These nodes store all the special variables AWK uses */
NODE *FS_node, *NF_node, *RS_node, *NR_node;
NODE *FILENAME_node, *OFS_node, *ORS_node, *OFMT_node;
NODE *FNR_node, *RLENGTH_node, *RSTART_node, *SUBSEP_node;
NODE *ENVIRON_node, *IGNORECASE_node;
NODE *ARGC_node, *ARGV_node;

/*
 * The parse tree and field nodes are stored here.  Parse_end is a dummy item
 * used to free up unneeded fields without freeing the program being run 
 */
int errcount = 0;	/* error counter, used by yyerror() */

/* The global null string */
NODE *Nnull_string;

/* The name the program was invoked under, for error messages */
char *myname;

/* A block of AWK code to be run before running the program */
NODE *begin_block = 0;

/* A block of AWK code to be run after the last input file */
NODE *end_block = 0;

int exiting = 0;		/* Was an "exit" statement executed? */
int exit_val = 0;		/* optional exit value */

#ifdef DEBUG
/* non-zero means in debugging is enabled.  Probably not very useful */
int debugging = 0;
extern int yydebug;
#endif

int tempsource = 0;		/* source is in a temp file */
char **sourcefile = NULL;	/* source file name(s) */
int numfiles = -1;		/* how many source files */

int strict = 0;			/* turn off gnu extensions */

int output_is_tty = 0;		/* control flushing of output */

NODE *expression_value;

/*
 * for strict to work, legal options must be first
 *
 * Unfortunately, -a and -e are orthogonal to -c.
 */
#define EXTENSIONS	8	/* where to clear */
#ifdef DEBUG
char awk_opts[] = "F:f:v:caeCVdD";
#else
char awk_opts[] = "F:f:v:caeCV";
#endif

char * version_string = "GAWK for MallocBench";

int
main(argc, argv)
int argc;
char **argv;
{
#ifdef DEBUG
	/* Print out the parse tree.   For debugging */
	register int dotree = 0;
#endif
#if 0
/* LLVM - Don't need this */
	extern char *version_string;
#endif
	FILE *fp;
	int c;
	extern int opterr, optind;
	extern char *optarg;
 	extern char *strrchr();
 	extern char *tmpnam();
	extern SIGTYPE catchsig();
	int i;
	int nostalgia;
#ifdef somtime_in_the_future
	int regex_mode = RE_SYNTAX_POSIX_EGREP;
#else
	int regex_mode = RE_SYNTAX_AWK;
#endif

	(void) signal(SIGFPE, catchsig);
	(void) signal(SIGSEGV, catchsig);

#ifdef BWGC
	{
	    extern gc_init();
/*	    gc_init();
*/
	}
#endif BWGC
	
	if (strncmp(version_string, "@(#)", 4) == 0)
		version_string += 4;

	myname = strrchr(argv[0], '/');
	if (myname == NULL)
		myname = argv[0];
	else
		myname++;
	if (argc < 2)
		usage();

	/* initialize the null string */
	Nnull_string = make_string("", 0);
	Nnull_string->numbr = 0.0;
	Nnull_string->type = Node_val;
	Nnull_string->flags = (PERM|STR|NUM|NUMERIC);

	/* Set up the special variables */

	/*
	 * Note that this must be done BEFORE arg parsing else -F
	 * breaks horribly 
	 */
	init_vars();

	/* worst case */
	emalloc(sourcefile, char **, argc * sizeof(char *), "main");


#ifdef STRICT	/* strict new awk compatibility */
	strict = 1;
	awk_opts[EXTENSIONS] = '\0';
#endif

#ifndef STRICT
	/* undocumented feature, inspired by nostalgia, and a T-shirt */
	nostalgia = 0;
	for (i = 1; i < argc && argv[i][0] == '-'; i++) {
		if (argv[i][1] == '-')		/* -- */
			break;
		else if (argv[i][1] == 'c') {	/* compatibility mode */
			nostalgia = 0;
			break;
		} else if (STREQ(&argv[i][1], "nostalgia"))
			nostalgia = 1;
			/* keep looping, in case -c after -nostalgia */
	}
	if (nostalgia) {
		fprintf (stderr, "awk: bailing out near line 1\n");
		abort();
	}
#endif
		
	while ((c = getopt (argc, argv, awk_opts)) != EOF) {
		switch (c) {
#ifdef DEBUG
		case 'd':
			debugging++;
			dotree++;
			break;

		case 'D':
			debugging++;
			yydebug = 2;
			break;
#endif

#ifndef STRICT
		case 'c':
			strict = 1;
			break;
#endif

		case 'F':
			set_fs(optarg);
			break;

		case 'f':
			/*
			 * a la MKS awk, allow multiple -f options.
			 * this makes function libraries real easy.
			 * most of the magic is in the scanner.
			 */
			sourcefile[++numfiles] = optarg;
			break;

		case 'v':
			pre_assign(optarg);
			break;

		case 'V':
			fprintf(stderr, "%s, patchlevel %d\n",
					version_string, PATCHLEVEL);
			break;

		case 'C':
			copyleft();
			break;

		case 'a':	/* use old fashioned awk regexps */
			regex_mode = RE_SYNTAX_AWK;
			break;

		case 'e':	/* use egrep style regexps, per Posix */
			regex_mode = RE_SYNTAX_POSIX_EGREP;
			break;

		case '?':
		default:
			/* getopt will print a message for us */
			/* S5R4 awk ignores bad options and keeps going */
			break;
		}
	}

	/* Tell the regex routines how they should work. . . */
	(void) re_set_syntax(regex_mode);

#ifdef DEBUG
	setbuf(stdout, (char *) NULL);	/* make debugging easier */
#endif
	if (isatty(fileno(stdout)))
		output_is_tty = 1;
	/* No -f option, use next arg */
	/* write to temp file and save sourcefile name */
	if (numfiles == -1) {
		int i;

		if (optind > argc - 1)	/* no args left */
			usage();
		numfiles++;
		i = strlen (argv[optind]);
		if (i == 0) {	/* sanity check */
			fprintf(stderr, "%s: empty program text\n", myname);
			usage();
			/* NOTREACHED */
		}
		sourcefile[0] = tmpnam((char *) NULL);
		if ((fp = fopen (sourcefile[0], "w")) == NULL)
			fatal("could not save source prog in temp file (%s)",
			strerror(errno));
		if (fwrite (argv[optind], 1, i, fp) == 0)
			fatal(
			"could not write source program to temp file (%s)",
			strerror(errno));
		if (argv[optind][i-1] != '\n')
			putc ('\n', fp);
		(void) fclose (fp);
		tempsource++;
		optind++;
	}
	init_args(optind, argc, myname, argv);

	/* Read in the program */
	if (yyparse() || errcount)
		exit(1);

#ifdef DEBUG
	if (dotree)
		print_parse_tree(expression_value);
#endif
	/* Set up the field variables */
	init_fields();

	if (begin_block)
		(void) interpret(begin_block);
	if (!exiting && (expression_value || end_block))
		do_input();
	if (end_block)
		(void) interpret(end_block);
	if (close_io() != 0 && exit_val == 0)
		exit_val = 1;
	
	exit(exit_val);
	/* NOTREACHED */
	return exit_val;
}

static void
usage()
{
	char *opt1 = " -f progfile [--]";
	char *opt2 = " [--] 'program'";
#ifdef STRICT
	char *regops = " [-ae] [-F fs] [-v var=val]"
#else
	char *regops = " [-aecCV] [-F fs] [-v var=val]";
#endif

	fprintf(stderr, "usage: %s%s%s file ...\n       %s%s%s file ...\n",
		myname, regops, opt1, myname, regops, opt2);
	exit(11);
}

/* Generate compiled regular expressions */
struct re_pattern_buffer *
make_regexp(s, ignorecase)
NODE *s;
int ignorecase;
{
	struct re_pattern_buffer *rp;
	char *err;

	emalloc(rp, struct re_pattern_buffer *, sizeof(*rp), "make_regexp");
	memset((char *) rp, 0, sizeof(*rp));
	emalloc(rp->buffer, char *, 16, "make_regexp");
	rp->allocated = 16;
	emalloc(rp->fastmap, char *, 256, "make_regexp");

	if (! strict && ignorecase)
		rp->translate = casetable;
	else
		rp->translate = NULL;
	if ((err = re_compile_pattern(s->stptr, s->stlen, rp)) != NULL)
		fatal("%s: /%s/", err, s->stptr);
	free_temp(s);
	return rp;
}

struct re_pattern_buffer *
mk_re_parse(s, ignorecase)
char *s;
int ignorecase;
{
	char *src;
	register char *dest;
	register int c;
	int in_brack = 0;

	for (dest = src = s; *src != '\0';) {
		if (*src == '\\') {
			c = *++src;
			switch (c) {
			case '/':
			case 'a':
			case 'b':
			case 'f':
			case 'n':
			case 'r':
			case 't':
			case 'v':
			case 'x':
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
				c = parse_escape(&src);
				if (c < 0)
					cant_happen();
				*dest++ = (char)c;
				break;
			default:
				*dest++ = '\\';
				*dest++ = (char)c;
				src++;
				break;
			}
		} else if (*src == '/' && ! in_brack)
			break;
		else {
			if (*src == '[')
				in_brack = 1;
			else if (*src == ']')
				in_brack = 0;

			*dest++ = *src++;
		}
	}
	return make_regexp(tmp_string(s, dest-s), ignorecase);
}

static void
copyleft ()
{
	extern char *version_string;
	char *cp;
	static char blurb[] =
"Copyright (C) 1989, Free Software Foundation.\n\
GNU Awk comes with ABSOLUTELY NO WARRANTY.  This is free software, and\n\
you are welcome to distribute it under the terms of the GNU General\n\
Public License, which covers both the warranty information and the\n\
terms for redistribution.\n\n\
You should have received a copy of the GNU General Public License along\n\
with this program; if not, write to the Free Software Foundation, Inc.,\n\
675 Mass Ave, Cambridge, MA 02139, USA.\n";

	fprintf (stderr, "%s, patchlevel %d\n", version_string, PATCHLEVEL);
	fputs(blurb, stderr);
	fflush(stderr);
}

static void
set_fs(str)
char *str;
{
	register NODE **tmp;

	tmp = get_lhs(FS_node, 0);
	/*
	 * Only if in full compatibility mode check for the stupid special
	 * case so -F\t works as documented in awk even though the shell
	 * hands us -Ft.  Bleah!
	 */
	if (strict && str[0] == 't' && str[1] == '\0')
		str[0] = '\t';
	*tmp = make_string(str, 1);
	do_deref();
}

static void
init_args(argc0, argc, argv0, argv)
int argc0, argc;
char *argv0;
char **argv;
{
	int i, j;
	NODE **aptr;

	ARGV_node = spc_var("ARGV", Nnull_string);
	aptr = assoc_lookup(ARGV_node, tmp_number(0.0));
	*aptr = make_string(argv0, strlen(argv0));
	for (i = argc0, j = 1; i < argc; i++) {
		aptr = assoc_lookup(ARGV_node, tmp_number((AWKNUM) j));
		*aptr = make_string(argv[i], strlen(argv[i]));
		j++;
	}
	ARGC_node = spc_var("ARGC", make_number((AWKNUM) j));
}

/*
 * Set all the special variables to their initial values.
 */
static void
init_vars()
{
	extern char **environ;
	char *var, *val;
	NODE **aptr;
	int i;

	FS_node = spc_var("FS", make_string(" ", 1));
	NF_node = spc_var("NF", make_number(-1.0));
	RS_node = spc_var("RS", make_string("\n", 1));
	NR_node = spc_var("NR", make_number(0.0));
	FNR_node = spc_var("FNR", make_number(0.0));
	FILENAME_node = spc_var("FILENAME", make_string("-", 1));
	OFS_node = spc_var("OFS", make_string(" ", 1));
	ORS_node = spc_var("ORS", make_string("\n", 1));
	OFMT_node = spc_var("OFMT", make_string("%.6g", 4));
	RLENGTH_node = spc_var("RLENGTH", make_number(0.0));
	RSTART_node = spc_var("RSTART", make_number(0.0));
	SUBSEP_node = spc_var("SUBSEP", make_string("\034", 1));
	IGNORECASE_node = spc_var("IGNORECASE", make_number(0.0));

	ENVIRON_node = spc_var("ENVIRON", Nnull_string);
	for (i = 0; environ[i]; i++) {
		static char nullstr[] = "";

		var = environ[i];
		val = strchr(var, '=');
		if (val)
			*val++ = '\0';
		else
			val = nullstr;
		aptr = assoc_lookup(ENVIRON_node, tmp_string(var, strlen (var)));
		*aptr = make_string(val, strlen (val));

		/* restore '=' so that system() gets a valid environment */
		if (val != nullstr)
			*--val = '=';
	}
}

/* Create a special variable */
static NODE *
spc_var(name, value)
char *name;
NODE *value;
{
	register NODE *r;

	if ((r = lookup(variables, name)) == NULL)
		r = install(variables, name, node(value, Node_var, (NODE *) NULL));
	return r;
}

static void
pre_assign(v)
char *v;
{
	char *cp;

	cp = strchr(v, '=');
	if (cp != NULL) {
		*cp++ = '\0';
		variable(v)->var_value = make_string(cp, strlen(cp));
	} else {
		fprintf (stderr,
			"%s: '%s' argument to -v not in 'var=value' form\n",
				myname, v);
		usage();
	}
}

SIGTYPE
catchsig(sig, code)
int sig, code;
{
#ifdef lint
	code = 0; sig = code; code = sig;
#endif
	if (sig == SIGFPE) {
		fatal("floating point exception");
	} else if (sig == SIGSEGV) {
		msg("fatal error: segmentation fault");
		/* fatal won't abort() if not compiled for debugging */
		abort();
	} else
		cant_happen();
	/* NOTREACHED */
}
