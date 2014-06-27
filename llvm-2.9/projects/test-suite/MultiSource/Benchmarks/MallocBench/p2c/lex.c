/* "p2c", a Pascal to C translator.
   Copyright (C) 1989, 1990, 1991 Free Software Foundation.
   Author's address: daveg@csvax.caltech.edu; 256-80 Caltech/Pasadena CA 91125.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation (any version).

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to
the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. */



#define PROTO_LEX_C
#include "trans.h"


/* Define LEXDEBUG for a token trace */
#define LEXDEBUG




#define EOFMARK 1


Static char dollar_flag, lex_initialized;
Static int if_flag, if_skip;
Static int commenting_flag;
Static char *commenting_ptr;
Static int skipflag;
Static char modulenotation;
Static short inputkind;
Static Strlist *instrlist;
Static char inbuf[300];
Static char *oldinfname, *oldctxname;
Static Strlist *endnotelist;



#define INP_FILE     0
#define INP_INCFILE  1
#define INP_STRLIST  2

Static struct inprec {
    struct inprec *next;
    short kind;
    char *fname, *inbufptr;
    int lnum;
    FILE *filep;
    Strlist *strlistp, *tempopts;
    Token curtok, saveblockkind;
    Symbol *curtoksym;
    Meaning *curtokmeaning;
    char *curtokbuf, *curtokcase;
} *topinput;






char *fixpascalname(name)
char *name;
{
    char *cp, *cp2;

    if (pascalsignif > 0) {
        name = format_ds("%.*s", pascalsignif, name);
        if (!pascalcasesens)
            upc(name);
	else if (pascalcasesens == 3)
	    lwc(name);
    } else if (!pascalcasesens)
        name = strupper(name);
    else if (pascalcasesens == 3)
	name = strlower(name);
    if (ignorenonalpha) {
	for (cp = cp2 = name; *cp; cp++)
	    if (isalnum(*cp))
		*cp2++ = *cp;
    }
    return name;
}



Static void makekeyword(name)
char *name;
{
    Symbol *sym;

    if (*name) {
        sym = findsymbol(name);
        sym->flags |= AVOIDNAME;
    }
}


Static void makeglobword(name)
char *name;
{
    Symbol *sym;

    if (*name) {
        sym = findsymbol(name);
        sym->flags |= AVOIDGLOB;
    }
}



Static void makekeywords()
{
    makekeyword("auto");
    makekeyword("break");
    makekeyword("char");
    makekeyword("continue");
    makekeyword("default");
    makekeyword("defined");   /* is this one really necessary? */
    makekeyword("double");
    makekeyword("enum");
    makekeyword("extern");
    makekeyword("float");
    makekeyword("int");
    makekeyword("long");
    makekeyword("noalias");
    makekeyword("register");
    makekeyword("return");
    makekeyword("short");
    makekeyword("signed");
    makekeyword("sizeof");
    makekeyword("static");
    makekeyword("struct");
    makekeyword("switch");
    makekeyword("typedef");
    makekeyword("union");
    makekeyword("unsigned");
    makekeyword("void");
    makekeyword("volatile");
    makekeyword("asm");
    makekeyword("fortran");
    makekeyword("entry");
    makekeyword("pascal");
    if (cplus != 0) {
        makekeyword("class");
        makekeyword("delete");
        makekeyword("friend");
        makekeyword("inline");
        makekeyword("new");
        makekeyword("operator");
        makekeyword("overload");
        makekeyword("public");
        makekeyword("this");
        makekeyword("virtual");
    }
    makekeyword(name_UCHAR);
    makekeyword(name_SCHAR);    /* any others? */
    makekeyword(name_BOOLEAN);
    makekeyword(name_PROCEDURE);
    makekeyword(name_ESCAPE);
    makekeyword(name_ESCIO);
    makekeyword(name_CHKIO);
    makekeyword(name_SETIO);
    makeglobword("main");
    makeglobword("vextern");     /* used in generated .h files */
    makeglobword("argc");
    makeglobword("argv");
    makekeyword("TRY");
    makekeyword("RECOVER");
    makekeyword("RECOVER2");
    makekeyword("ENDTRY");
}



Static Symbol *Pkeyword(name, tok)
char *name;
Token tok;
{
    Symbol *sp = NULL;

    if (pascalcasesens != 2) {
	sp = findsymbol(strlower(name));
	sp->kwtok = tok;
    }
    if (pascalcasesens != 3) {
	sp = findsymbol(strupper(name));
	sp->kwtok = tok;
    }
    return sp;
}


Static Symbol *Pkeywordposs(name, tok)
char *name;
Token tok;
{
    Symbol *sp = NULL;

    if (pascalcasesens != 2) {
	sp = findsymbol(strlower(name));
	sp->kwtok = tok;
	sp->flags |= KWPOSS;
    }
    if (pascalcasesens != 3) {
	sp = findsymbol(strupper(name));
	sp->kwtok = tok;
	sp->flags |= KWPOSS;
    }
    return sp;
}


Static void makePascalwords()
{
    Pkeyword("AND", TOK_AND);
    Pkeyword("ARRAY", TOK_ARRAY);
    Pkeywordposs("ANYVAR", TOK_ANYVAR);
    Pkeywordposs("ABSOLUTE", TOK_ABSOLUTE);
    Pkeyword("BEGIN", TOK_BEGIN);
    Pkeywordposs("BY", TOK_BY);
    Pkeyword("CASE", TOK_CASE);
    Pkeyword("CONST", TOK_CONST);
    Pkeyword("DIV", TOK_DIV);
    Pkeywordposs("DEFINITION", TOK_DEFINITION);
    Pkeyword("DO", TOK_DO);
    Pkeyword("DOWNTO", TOK_DOWNTO);
    Pkeyword("ELSE", TOK_ELSE);
    Pkeywordposs("ELSIF", TOK_ELSIF);
    Pkeyword("END", TOK_END);
    Pkeywordposs("EXPORT", TOK_EXPORT);
    Pkeyword("FILE", TOK_FILE);
    Pkeyword("FOR", TOK_FOR);
    Pkeywordposs("FROM", TOK_FROM);
    Pkeyword("FUNCTION", TOK_FUNCTION);
    Pkeyword("GOTO", TOK_GOTO);
    Pkeyword("IF", TOK_IF);
    Pkeywordposs("IMPLEMENT", TOK_IMPLEMENT);
    Pkeywordposs("IMPLEMENTATION", TOK_IMPLEMENT);
    Pkeywordposs("IMPORT", TOK_IMPORT);
    Pkeyword("IN", TOK_IN);
    Pkeywordposs("INLINE", TOK_INLINE);
    Pkeywordposs("INTERFACE", TOK_EXPORT);
    Pkeywordposs("INTERRUPT", TOK_INTERRUPT);
    Pkeyword("LABEL", TOK_LABEL);
    Pkeywordposs("LOOP", TOK_LOOP);
    Pkeyword("MOD", TOK_MOD);
    Pkeywordposs("MODULE", TOK_MODULE);
    Pkeyword("NIL", TOK_NIL);
    Pkeyword("NOT", TOK_NOT);
    Pkeyword("OF", TOK_OF);
    Pkeyword("OR", TOK_OR);
    Pkeywordposs("ORIGIN", TOK_ORIGIN);
    Pkeywordposs("OTHERWISE", TOK_OTHERWISE);
    Pkeywordposs("OVERLAY", TOK_SEGMENT);
    Pkeyword("PACKED", TOK_PACKED);
    Pkeywordposs("POINTER", TOK_POINTER);
    Pkeyword("PROCEDURE", TOK_PROCEDURE);
    Pkeyword("PROGRAM", TOK_PROGRAM);
    Pkeywordposs("QUALIFIED", TOK_QUALIFIED);
    Pkeyword("RECORD", TOK_RECORD);
    Pkeywordposs("RECOVER", TOK_RECOVER);
    Pkeywordposs("REM", TOK_REM);
    Pkeyword("REPEAT", TOK_REPEAT);
    Pkeywordposs("RETURN", TOK_RETURN);
    if (which_lang == LANG_UCSD)
	Pkeyword("SEGMENT", TOK_SEGMENT);
    else
	Pkeywordposs("SEGMENT", TOK_SEGMENT);
    Pkeyword("SET", TOK_SET);
    Pkeywordposs("SHL", TOK_SHL);
    Pkeywordposs("SHR", TOK_SHR);
    Pkeyword("THEN", TOK_THEN);
    Pkeyword("TO", TOK_TO);
    Pkeywordposs("TRY", TOK_TRY);
    Pkeyword("TYPE", TOK_TYPE);
    Pkeyword("UNTIL", TOK_UNTIL);
    Pkeywordposs("USES", TOK_IMPORT);
    Pkeywordposs("UNIT", TOK_MODULE);
    if (which_lang == LANG_VAX)
	Pkeyword("VALUE", TOK_VALUE);
    else
	Pkeywordposs("VALUE", TOK_VALUE);
    Pkeyword("VAR", TOK_VAR);
    Pkeywordposs("VARYING", TOK_VARYING);
    Pkeyword("WHILE", TOK_WHILE);
    Pkeyword("WITH", TOK_WITH);
    Pkeywordposs("XOR", TOK_XOR);
    Pkeyword("__MODULE", TOK_MODULE);
    Pkeyword("__IMPORT", TOK_IMPORT);
    Pkeyword("__EXPORT", TOK_EXPORT);
    Pkeyword("__IMPLEMENT", TOK_IMPLEMENT);
}



Static void deterministic(name)
char *name;
{
    Symbol *sym;

    if (*name) {
        sym = findsymbol(name);
        sym->flags |= DETERMF;
    }
}


Static void nosideeff(name)
char *name;
{
    Symbol *sym;

    if (*name) {
        sym = findsymbol(name);
        sym->flags |= NOSIDEEFF;
    }
}



Static void recordsideeffects()
{
    deterministic("abs");
    deterministic("acos");
    deterministic("asin");
    deterministic("atan");
    deterministic("atan2");
    deterministic("atof");
    deterministic("atoi");
    deterministic("atol");
    deterministic("ceil");
    deterministic("cos");
    deterministic("cosh");
    deterministic("exp");
    deterministic("fabs");
    deterministic("feof");
    deterministic("feoln");
    deterministic("ferror");
    deterministic("floor");
    deterministic("fmod");
    deterministic("ftell");
    deterministic("isalnum");
    deterministic("isalpha");
    deterministic("isdigit");
    deterministic("islower");
    deterministic("isspace");
    deterministic("isupper");
    deterministic("labs");
    deterministic("ldexp");
    deterministic("log");
    deterministic("log10");
    deterministic("memcmp");
    deterministic("memchr");
    deterministic("pow");
    deterministic("sin");
    deterministic("sinh");
    deterministic("sqrt");
    deterministic("strchr");
    deterministic("strcmp");
    deterministic("strcspn");
    deterministic("strlen");
    deterministic("strncmp");
    deterministic("strpbrk");
    deterministic("strrchr");
    deterministic("strspn");
    deterministic("strstr");
    deterministic("tan");
    deterministic("tanh");
    deterministic("tolower");
    deterministic("toupper");
    deterministic(setequalname);
    deterministic(subsetname);
    deterministic(signextname);
}





void init_lex()
{
    int i;

    inputkind = INP_FILE;
    inf_lnum = 0;
    inf_ltotal = 0;
    *inbuf = 0;
    inbufptr = inbuf;
    keepingstrlist = NULL;
    tempoptionlist = NULL;
    switch_strpos = 0;
    dollar_flag = 0;
    if_flag = 0;
    if_skip = 0;
    commenting_flag = 0;
    skipflag = 0;
    inbufindent = 0;
    modulenotation = 1;
    notephase = 0;
    endnotelist = NULL;
    for (i = 0; i < SYMHASHSIZE; i++)
        symtab[i] = 0;
    C_lex = 0;
    lex_initialized = 0;
}


void setup_lex()
{
    lex_initialized = 1;
    if (!strcmp(language, "MODCAL"))
        sysprog_flag = 2;
    else
        sysprog_flag = 0;
    if (shortcircuit < 0)
        partial_eval_flag = (which_lang == LANG_TURBO ||
			     which_lang == LANG_VAX ||
			     which_lang == LANG_OREGON ||
			     modula2 ||
			     hpux_lang);
    else
        partial_eval_flag = shortcircuit;
    iocheck_flag = 1;
    range_flag = 1;
    ovflcheck_flag = 1;
    stackcheck_flag = 1;
    fixedflag = 0;
    withlevel = 0;
    makekeywords();
    makePascalwords();
    recordsideeffects();
    topinput = 0;
    ignore_directives = 0;
    skipping_module = 0;
    blockkind = TOK_END;
    gettok();
}




int checkeatnote(msg)
char *msg;
{
    Strlist *lp;
    char *cp;
    int len;

    for (lp = eatnotes; lp; lp = lp->next) {
	if (!strcmp(lp->s, "1")) {
	    echoword("[*]", 0);
	    return 1;
	}
	if (!strcmp(lp->s, "0"))
	    return 0;
	len = strlen(lp->s);
	cp = msg;
	while (*cp && (*cp != lp->s[0] || strncmp(cp, lp->s, len)))
	    cp++;
	if (*cp) {
	    cp = lp->s;
	    if (*cp != '[')
		cp = format_s("[%s", cp);
	    if (cp[strlen(cp)-1] != ']')
		cp = format_s("%s]", cp);
	    echoword(cp, 0);
	    return 1;
	}
    }
    return 0;
}



void beginerror()
{
    end_source();
    if (showprogress) {
        fprintf(stderr, "\r%60s\r", "");
        clearprogress();
    } else
	echobreak();
}


void counterror()
{
    if (maxerrors > 0) {
	if (--maxerrors == 0) {
	    fprintf(outf, "\n/* Translation aborted: Too many errors. */\n");
	    fprintf(outf,   "-------------------------------------------\n");
	    if (outf != stdout)
		printf("Translation aborted: Too many errors.\n");
	    if (verbose)
		fprintf(logf, "Translation aborted: Too many errors.\n");
	    closelogfile();
	    exit(EXIT_FAILURE);
	}
    }
}


void error(msg)     /* does not return */
char *msg;
{
    flushcomments(NULL, -1, -1);
    beginerror();
    fprintf(outf, "/* %s, line %d: %s */\n", infname, inf_lnum, msg);
    fprintf(outf, "/* Translation aborted. */\n");
    fprintf(outf, "--------------------------\n");
    if (outf != stdout) {
        printf("%s, line %d/%d: %s\n", infname, inf_lnum, outf_lnum, msg);
        printf("Translation aborted.\n");
    }
    if (verbose) {
	fprintf(logf, "%s, line %d/%d: %s\n",
		infname, inf_lnum, outf_lnum, msg);
	fprintf(logf, "Translation aborted.\n");
    }
    closelogfile();
    exit(EXIT_FAILURE);
}


void interror(proc, msg)      /* does not return */
char *proc, *msg;
{
    error(format_ss("Internal error in %s: %s", proc, msg));
}


void warning(msg)
char *msg;
{
    if (checkeatnote(msg)) {
	if (verbose)
	    fprintf(logf, "%s, %d/%d: Omitted warning: %s\n",
		    infname, inf_lnum, outf_lnum, msg);
	return;
    }
    beginerror();
    addnote(format_s("Warning: %s", msg), curserial);
    counterror();
}


void intwarning(proc, msg)
char *proc, *msg;
{
    if (checkeatnote(msg)) {
	if (verbose)
	    fprintf(logf, "%s, %d/%d: Omitted internal error in %s: %s\n",
		    infname, inf_lnum, outf_lnum, proc, msg);
	return;
    }
    beginerror();
    addnote(format_ss("Internal error in %s: %s", proc, msg), curserial);
    if (error_crash)
        exit(EXIT_FAILURE);
    counterror();
}




void note(msg)
char *msg;
{
    if (blockkind == TOK_IMPORT || checkeatnote(msg)) {
	if (verbose)
	    fprintf(logf, "%s, %d/%d: Omitted note: %s\n",
		    infname, inf_lnum, outf_lnum, msg);
	return;
    }
    beginerror();
    addnote(format_s("Note: %s", msg), curserial);
    counterror();
}



void endnote(msg)
char *msg;
{
    if (blockkind == TOK_IMPORT || checkeatnote(msg)) {
	if (verbose)
	    fprintf(logf, "%s, %d/%d: Omitted end-note: %s\n",
		    infname, inf_lnum, outf_lnum, msg);
	return;
    }
    if (verbose)
	fprintf(logf, "%s, %d/%d: Recorded end-note: %s\n",
		infname, inf_lnum, outf_lnum, msg);
    (void) strlist_add(&endnotelist, msg);
}


void showendnotes()
{
    while (initialcalls) {
	if (initialcalls->value)
	    endnote(format_s("Remember to call %s in main program [215]",
			     initialcalls->s));
	strlist_eat(&initialcalls);
    }
    if (endnotelist) {
	end_source();
	while (endnotelist) {
	    if (outf != stdout) {
		beginerror();
		printf("Note: %s\n", endnotelist->s);
	    }
	    fprintf(outf, "/* p2c: Note: %s */\n", endnotelist->s);
	    outf_lnum++;
	    strlist_eat(&endnotelist);
	}
    }
}







char *tok_name(tok)
Token tok;
{
    if (tok == TOK_END && inputkind == INP_STRLIST)
	return "end of macro";
    if (tok == curtok && tok == TOK_IDENT)
        return format_s("'%s'", curtokcase);
    if (!modulenotation) {
        switch (tok) {
            case TOK_MODULE:    return "UNIT";
            case TOK_IMPORT:    return "USES";
            case TOK_EXPORT:    return "INTERFACE";
            case TOK_IMPLEMENT: return "IMPLEMENTATION";
	    default:		break;
        }
    }
    return toknames[(int) tok];
}



void expected(msg)
char *msg;
{
    error(format_ss("Expected %s, found %s", msg, tok_name(curtok)));
}


void expecttok(tok)
Token tok;
{
    if (curtok != tok)
        expected(tok_name(tok));
}


void needtok(tok)
Token tok;
{
    if (curtok != tok)
        expected(tok_name(tok));
    gettok();
}


int wexpected(msg)
char *msg;
{
    warning(format_ss("Expected %s, found %s [227]", msg, tok_name(curtok)));
    return 0;
}


int wexpecttok(tok)
Token tok;
{
    if (curtok != tok)
        return wexpected(tok_name(tok));
    else
	return 1;
}


int wneedtok(tok)
Token tok;
{
    if (wexpecttok(tok)) {
	gettok();
	return 1;
    } else
	return 0;
}


void alreadydef(sym)
Symbol *sym;
{
    warning(format_s("Symbol '%s' was already defined [220]", sym->name));
}


void undefsym(sym)
Symbol *sym;
{
    warning(format_s("Symbol '%s' is not defined [221]", sym->name));
}


void symclass(sym)
Symbol *sym;
{
    warning(format_s("Symbol '%s' is not of the appropriate class [222]", sym->name));
}


void badtypes()
{
    warning("Type mismatch [223]");
}


void valrange()
{
    warning("Value range error [224]");
}



void skipparens()
{
    Token begintok;

    if (curtok == TOK_LPAR) {
        gettok();
        while (curtok != TOK_RPAR)
            skipparens();
    } else if (curtok == TOK_LBR) {
        gettok();
        while (curtok != TOK_RBR)
            skipparens();
    } else if (curtok == TOK_BEGIN || curtok == TOK_RECORD ||
	       curtok == TOK_CASE) {
	begintok = curtok;
        gettok();
        while (curtok != TOK_END)
	    if (curtok == TOK_CASE && begintok == TOK_RECORD)
		gettok();
	    else
		skipparens();
    }
    gettok();
}


void skiptotoken2(tok1, tok2)
Token tok1, tok2;
{
    while (curtok != tok1 && curtok != tok2 &&
	   curtok != TOK_END && curtok != TOK_RPAR &&
	   curtok != TOK_RBR && curtok != TOK_EOF)
	skipparens();
}


void skippasttoken2(tok1, tok2)
Token tok1, tok2;
{
    skiptotoken2(tok1, tok2);
    if (curtok == tok1 || curtok == tok2)
	gettok();
}


void skippasttotoken(tok1, tok2)
Token tok1, tok2;
{
    skiptotoken2(tok1, tok2);
    if (curtok == tok1)
	gettok();
}


void skiptotoken(tok)
Token tok;
{
    skiptotoken2(tok, tok);
}


void skippasttoken(tok)
Token tok;
{
    skippasttoken2(tok, tok);
}



int skipopenparen()
{
    if (wneedtok(TOK_LPAR))
	return 1;
    skiptotoken(TOK_SEMI);
    return 0;
}


int skipcloseparen()
{
    if (curtok == TOK_COMMA)
	warning("Too many arguments for built-in routine [225]");
    else
	if (wneedtok(TOK_RPAR))
	    return 1;
    skippasttotoken(TOK_RPAR, TOK_SEMI);
    return 0;
}


int skipcomma()
{
    if (curtok == TOK_RPAR)
	warning("Too few arguments for built-in routine [226]");
    else
	if (wneedtok(TOK_COMMA))
	    return 1;
    skippasttotoken(TOK_RPAR, TOK_SEMI);
    return 0;
}





char *findaltname(name, num)
char *name;
int num;
{
    char *cp;

    if (num <= 0)
        return name;
    if (num == 1 && *alternatename1)
        return format_s(alternatename1, name);
    if (num == 2 && *alternatename2)
        return format_s(alternatename2, name);
    if (*alternatename)
        return format_sd(alternatename, name, num);
    cp = name;
    if (*alternatename1) {
        while (--num >= 0)
	    cp = format_s(alternatename1, cp);
    } else {
	while (--num >= 0)
	    cp = format_s("%s_", cp);
    }
    return cp;
}




Symbol *findsymbol_opt(name)
char *name;
{
    register int i;
    register unsigned int hash;
    register char *cp;
    register Symbol *sp;

    hash = 0;
    for (cp = name; *cp; cp++)
        hash = hash*3 + *cp;
    sp = symtab[hash % SYMHASHSIZE];
    while (sp && (i = strcmp(sp->name, name)) != 0) {
        if (i < 0)
            sp = sp->left;
        else
            sp = sp->right;
    }
    return sp;
}



Symbol *findsymbol(name)
char *name;
{
    register int i;
    register unsigned int hash;
    register char *cp;
    register Symbol **prev, *sp;

    hash = 0;
    for (cp = name; *cp; cp++)
        hash = hash*3 + *cp;
    prev = symtab + (hash % SYMHASHSIZE);
    while ((sp = *prev) != 0 &&
           (i = strcmp(sp->name, name)) != 0) {
        if (i < 0)
            prev = &(sp->left);
        else
            prev = &(sp->right);
    }
    if (!sp) {
        sp = ALLOCV(sizeof(Symbol) + strlen(name), Symbol, symbols);
        sp->mbase = sp->fbase = NULL;
        sp->left = sp->right = NULL;
        strcpy(sp->name, name);
        sp->flags = 0;
	sp->kwtok = TOK_NONE;
        sp->symbolnames = NULL;
        *prev = sp;
    }
    return sp;
}




void clearprogress()
{
    oldinfname = NULL;
}


void progress()
{
    char *ctxname;
    int needrefr;
    static int prevlen;

    if (showprogress) {
        if (!curctx || curctx == nullctx || curctx->kind == MK_MODULE ||
            !strncmp(curctx->name, "__PROCPTR", 9) || blockkind == TOK_IMPORT)
            ctxname = "";
        else
            ctxname = curctx->name;
        needrefr = (inf_lnum & 15) == 0;
        if (oldinfname != infname || oldctxname != ctxname) {
	    if (oldinfname != infname)
		prevlen = 60;
            fprintf(stderr, "\r%*s", prevlen + 2, "");
            oldinfname = infname;
            oldctxname = ctxname;
            needrefr = 1;
        }
        if (needrefr) {
            fprintf(stderr, "\r%5d %s  %s", inf_lnum, infname, ctxname);
	    prevlen = 8 + strlen(infname) + strlen(ctxname);
        } else {
            fprintf(stderr, "\r%5d", inf_lnum);
	    prevlen = 5;
	}
    }
}



void p2c_getline()
{
    char *cp, *cp2;

    switch (inputkind) {

        case INP_FILE:
        case INP_INCFILE:
            inf_lnum++;
	    inf_ltotal++;
            if (fgets(inbuf, 300, inf)) {
                cp = inbuf + strlen(inbuf);
                if (*inbuf && cp[-1] == '\n')
                    cp[-1] = 0;
		if (inbuf[0] == '#' && inbuf[1] == ' ' && isdigit(inbuf[2])) {
		    cp = inbuf + 2;    /* in case input text came */
		    inf_lnum = 0;      /*  from the C preprocessor */
		    while (isdigit(*cp))
			inf_lnum = inf_lnum*10 + (*cp++) - '0';
		    inf_lnum--;
		    while (isspace(*cp)) cp++;
		    if (*cp == '"' && (cp2 = my_strchr(cp+1, '"')) != NULL) {
			cp++;
			infname = stralloc(cp);
			infname[cp2 - cp] = 0;
		    }
		    p2c_getline();
		    return;
		}
		if (copysource && *inbuf) {
		    start_source();
		    fprintf(outf, "%s\n", inbuf);
		}
                if (keepingstrlist) {
                    strlist_append(keepingstrlist, inbuf)->value = inf_lnum;
                }
                if (showprogress && inf_lnum % showprogress == 0)
                    progress();
            } else {
                if (showprogress)
                    fprintf(stderr, "\n");
                if (inputkind == INP_INCFILE) {
                    pop_input();
                    p2c_getline();
                } else
                    strcpy(inbuf, "\001");
            }
            break;

        case INP_STRLIST:
            if (instrlist) {
                strcpy(inbuf, instrlist->s);
                if (instrlist->value)
                    inf_lnum = instrlist->value;
                else
                    inf_lnum++;
                instrlist = instrlist->next;
            } else
                strcpy(inbuf, "\001");
            break;
    }
    inbufptr = inbuf;
    inbufindent = 0;
}




Static void push_input()
{
    struct inprec *inp;

    inp = ALLOC(1, struct inprec, inprecs);
    inp->kind = inputkind;
    inp->fname = infname;
    inp->lnum = inf_lnum;
    inp->filep = inf;
    inp->strlistp = instrlist;
    inp->inbufptr = stralloc(inbufptr);
    inp->curtok = curtok;
    inp->curtoksym = curtoksym;
    inp->curtokmeaning = curtokmeaning;
    inp->curtokbuf = stralloc(curtokbuf);
    inp->curtokcase = stralloc(curtokcase);
    inp->saveblockkind = TOK_NIL;
    inp->next = topinput;
    topinput = inp;
    inbufptr = inbuf + strlen(inbuf);
}



void push_input_file(fp, fname, isinclude)
FILE *fp;
char *fname;
int isinclude;
{
    push_input();
    inputkind = (isinclude == 1) ? INP_INCFILE : INP_FILE;
    inf = fp;
    inf_lnum = 0;
    infname = fname;
    *inbuf = 0;
    inbufptr = inbuf;
    topinput->tempopts = tempoptionlist;
    tempoptionlist = NULL;
    if (isinclude != 2)
        gettok();
}


void include_as_import()
{
    if (inputkind == INP_INCFILE) {
	if (topinput->saveblockkind == TOK_NIL)
	    topinput->saveblockkind = blockkind;
	blockkind = TOK_IMPORT;
    } else
	warning(format_s("%s ignored except in include files [228]",
			 interfacecomment));
}


void push_input_strlist(sp, fname)
Strlist *sp;
char *fname;
{
    push_input();
    inputkind = INP_STRLIST;
    instrlist = sp;
    if (fname) {
        infname = fname;
        inf_lnum = 0;
    } else
        inf_lnum--;     /* adjust for extra p2c_getline() */
    *inbuf = 0;
    inbufptr = inbuf;
    gettok();
}



void pop_input()
{
    struct inprec *inp;

    if (inputkind == INP_FILE || inputkind == INP_INCFILE) {
	while (tempoptionlist) {
	    undooption(tempoptionlist->value, tempoptionlist->s);
	    strlist_eat(&tempoptionlist);
	}
	tempoptionlist = topinput->tempopts;
	if (inf)
	    fclose(inf);
    }
    inp = topinput;
    topinput = inp->next;
    if (inp->saveblockkind != TOK_NIL)
	blockkind = inp->saveblockkind;
    inputkind = inp->kind;
    infname = inp->fname;
    inf_lnum = inp->lnum;
    inf = inp->filep;
    curtok = inp->curtok;
    curtoksym = inp->curtoksym;
    curtokmeaning = inp->curtokmeaning;
    strcpy(curtokbuf, inp->curtokbuf);
    FREE(inp->curtokbuf);
    strcpy(curtokcase, inp->curtokcase);
    FREE(inp->curtokcase);
    strcpy(inbuf, inp->inbufptr);
    FREE(inp->inbufptr);
    inbufptr = inbuf;
    instrlist = inp->strlistp;
    FREE(inp);
}




int undooption(i, name)
int i;
char *name;
{
    char kind = rctable[i].kind;

    switch (kind) {

        case 'S':
	case 'B':
	    if (rcprevvalues[i]) {
                *((short *)rctable[i].ptr) = rcprevvalues[i]->value;
                strlist_eat(&rcprevvalues[i]);
                return 1;
            }
            break;

        case 'I':
        case 'D':
            if (rcprevvalues[i]) {
                *((int *)rctable[i].ptr) = rcprevvalues[i]->value;
                strlist_eat(&rcprevvalues[i]);
                return 1;
            }
            break;

        case 'L':
            if (rcprevvalues[i]) {
                *((long *)rctable[i].ptr) = rcprevvalues[i]->value;
                strlist_eat(&rcprevvalues[i]);
                return 1;
            }
            break;

	case 'R':
	    if (rcprevvalues[i]) {
		*((double *)rctable[i].ptr) = atof(rcprevvalues[i]->s);
		strlist_eat(&rcprevvalues[i]);
		return 1;
	    }
	    break;

        case 'C':
        case 'U':
            if (rcprevvalues[i]) {
                strcpy((char *)rctable[i].ptr, rcprevvalues[i]->s);
                strlist_eat(&rcprevvalues[i]);
                return 1;
            }
            break;

        case 'A':
            strlist_remove((Strlist **)rctable[i].ptr, name);
            return 1;

        case 'X':
            if (rctable[i].def == 1) {
                strlist_remove((Strlist **)rctable[i].ptr, name);
                return 1;
            }
            break;

    }
    return 0;
}




void badinclude()
{
    warning("Can't handle an \"include\" directive here [229]");
    inputkind = INP_INCFILE;     /* expand it in-line */
    gettok();
}



int handle_include(fn)
char *fn;
{
    FILE *fp = NULL;
    Strlist *sl;

    for (sl = includedirs; sl; sl = sl->next) {
	fp = fopen(format_s(sl->s, fn), "r");
	if (fp) {
	    fn = stralloc(format_s(sl->s, fn));
	    break;
	}
    }
    if (!fp) {
        perror(fn);
        warning(format_s("Could not open include file %s [230]", fn));
        return 0;
    } else {
        if (!quietmode && !showprogress)
	    if (outf == stdout)
		fprintf(stderr, "Reading include file \"%s\"\n", fn);
	    else
		printf("Reading include file \"%s\"\n", fn);
	if (verbose)
	    fprintf(logf, "Reading include file \"%s\"\n", fn);
        if (expandincludes == 0) {
            push_input_file(fp, fn, 2);
            curtok = TOK_INCLUDE;
            strcpy(curtokbuf, fn);
        } else {
            push_input_file(fp, fn, 1);
        }
        return 1;
    }
}



int turbo_directive(closing, after)
char *closing, *after;
{
    char *cp, *cp2;
    int i, result;

    if (!strcincmp(inbufptr, "$double", 7)) {
	cp = inbufptr + 7;
	while (isspace(*cp)) cp++;
	if (cp == closing) {
	    inbufptr = after;
	    doublereals = 1;
	    return 1;
	}
    } else if (!strcincmp(inbufptr, "$nodouble", 9)) {
	cp = inbufptr + 9;
	while (isspace(*cp)) cp++;
	if (cp == closing) {
	    inbufptr = after;
	    doublereals = 0;
	    return 1;
	}
    }
    switch (inbufptr[2]) {

        case '+':
        case '-':
            result = 1;
            cp = inbufptr + 1;
            for (;;) {
                if (!isalpha(*cp++))
                    return 0;
                if (*cp != '+' && *cp != '-')
                    return 0;
                if (++cp == closing)
                    break;
                if (*cp++ != ',')
                    return 0;
            }
            cp = inbufptr + 1;
            do {
                switch (*cp++) {

                    case 'b':
                    case 'B':
                        if (shortcircuit < 0 && which_lang != LANG_MPW)
                            partial_eval_flag = (*cp == '-');
                        break;

                    case 'i':
                    case 'I':
                        iocheck_flag = (*cp == '+');
                        break;

                    case 'r':
                    case 'R':
                        if (*cp == '+') {
                            if (!range_flag)
                                note("Range checking is ON [216]");
                            range_flag = 1;
                        } else {
                            if (range_flag)
                                note("Range checking is OFF [216]");
                            range_flag = 0;
                        }
                        break;

                    case 's':
                    case 'S':
                        if (*cp == '+') {
                            if (!stackcheck_flag)
                                note("Stack checking is ON [217]");
                            stackcheck_flag = 1;
                        } else {
                            if (stackcheck_flag)
                                note("Stack checking is OFF [217]");
                            stackcheck_flag = 0;
                        }
                        break;

                    default:
                        result = 0;
                        break;
                }
                cp++;
            } while (*cp++ == ',');
            if (result)
                inbufptr = after;
            return result;

	case 'c':
	case 'C':
	    if (toupper(inbufptr[1]) == 'S' &&
		(inbufptr[3] == '+' || inbufptr[3] == '-') &&
		inbufptr + 4 == closing) {
		if (shortcircuit < 0)
		    partial_eval_flag = (inbufptr[3] == '+');
		inbufptr = after;
		return 1;
	    }
	    return 0;

        case ' ':
            switch (inbufptr[1]) {

                case 'i':
                case 'I':
                    if (skipping_module)
                        break;
                    cp = inbufptr + 3;
                    while (isspace(*cp)) cp++;
                    cp2 = cp;
                    i = 0;
                    while (*cp2 && cp2 != closing)
                        i++, cp2++;
                    if (cp2 != closing)
                        return 0;
                    while (isspace(cp[i-1]))
                        if (--i <= 0)
                            return 0;
                    inbufptr = after;
                    cp2 = ALLOC(i + 1, char, strings);
                    strncpy(cp2, cp, i);
                    cp2[i] = 0;
                    if (handle_include(cp2))
			return 2;
		    break;

		case 's':
		case 'S':
		    cp = inbufptr + 3;
		    outsection(minorspace);
		    if (cp == closing) {
			output("#undef __SEG__\n");
		    } else {
			output("#define __SEG__ ");
			while (*cp && cp != closing)
			    cp++;
			if (*cp) {
			    i = *cp;
			    *cp = 0;
			    output(inbufptr + 3);
			    *cp = i;
			}
			output("\n");
		    }
		    outsection(minorspace);
		    inbufptr = after;
		    return 1;

            }
            return 0;

	case '}':
	case '*':
	    if (inbufptr + 2 == closing) {
		switch (inbufptr[1]) {
		    
		  case 's':
		  case 'S':
		    outsection(minorspace);
		    output("#undef __SEG__\n");
		    outsection(minorspace);
		    inbufptr = after;
		    return 1;

		}
	    }
	    return 0;

        case 'f':   /* $ifdef etc. */
        case 'F':
            if (toupper(inbufptr[1]) == 'I' &&
                ((toupper(inbufptr[3]) == 'O' &&
                  toupper(inbufptr[4]) == 'P' &&
                  toupper(inbufptr[5]) == 'T') ||
                 (toupper(inbufptr[3]) == 'D' &&
                  toupper(inbufptr[4]) == 'E' &&
                  toupper(inbufptr[5]) == 'F') ||
                 (toupper(inbufptr[3]) == 'N' &&
                  toupper(inbufptr[4]) == 'D' &&
                  toupper(inbufptr[5]) == 'E' &&
                  toupper(inbufptr[6]) == 'F'))) {
                note("Turbo Pascal conditional compilation directive was ignored [218]");
            }
            return 0;

    }
    return 0;
}




extern Strlist *addmacros;

void defmacro(name, kind, fname, lnum)
char *name, *fname;
long kind;
int lnum;
{
    Strlist *defsl, *sl, *sl2;
    Symbol *sym, *sym2;
    Meaning *mp;
    Expr *ex;

    defsl = NULL;
    sl = strlist_append(&defsl, name);
    C_lex++;
    if (fname && !strcmp(fname, "<macro>") && curtok == TOK_IDENT)
        fname = curtoksym->name;
    push_input_strlist(defsl, fname);
    if (fname)
        inf_lnum = lnum;
    switch (kind) {

        case MAC_VAR:
            if (!wexpecttok(TOK_IDENT))
		break;
	    for (mp = curtoksym->mbase; mp; mp = mp->snext) {
		if (mp->kind == MK_VAR)
		    warning(format_s("VarMacro must be defined before declaration of variable %s [231]", curtokcase));
	    }
            sl = strlist_append(&varmacros, curtoksym->name);
            gettok();
            if (!wneedtok(TOK_EQ))
		break;
            sl->value = (long)pc_expr();
            break;

        case MAC_CONST:
            if (!wexpecttok(TOK_IDENT))
		break;
	    for (mp = curtoksym->mbase; mp; mp = mp->snext) {
		if (mp->kind == MK_CONST)
		    warning(format_s("ConstMacro must be defined before declaration of variable %s [232]", curtokcase));
	    }
            sl = strlist_append(&constmacros, curtoksym->name);
            gettok();
            if (!wneedtok(TOK_EQ))
		break;
            sl->value = (long)pc_expr();
            break;

        case MAC_FIELD:
            if (!wexpecttok(TOK_IDENT))
		break;
            sym = curtoksym;
            gettok();
            if (!wneedtok(TOK_DOT))
		break;
            if (!wexpecttok(TOK_IDENT))
		break;
	    sym2 = curtoksym;
            gettok();
	    if (!wneedtok(TOK_EQ))
		break;
            funcmacroargs = NULL;
            sym->flags |= FMACREC;
            ex = pc_expr();
            sym->flags &= ~FMACREC;
	    for (mp = sym2->fbase; mp; mp = mp->snext) {
		if (mp->rectype && mp->rectype->meaning &&
		    mp->rectype->meaning->sym == sym)
		    break;
	    }
	    if (mp) {
		mp->constdefn = ex;
	    } else {
		sl = strlist_append(&fieldmacros, 
				    format_ss("%s.%s", sym->name, sym2->name));
		sl->value = (long)ex;
	    }
            break;

        case MAC_FUNC:
            if (!wexpecttok(TOK_IDENT))
		break;
            sym = curtoksym;
            if (sym->mbase &&
		(sym->mbase->kind == MK_FUNCTION ||
		 sym->mbase->kind == MK_SPECIAL))
                sl = NULL;
            else
                sl = strlist_append(&funcmacros, sym->name);
            gettok();
            funcmacroargs = NULL;
            if (curtok == TOK_LPAR) {
                do {
                    gettok();
		    if (curtok == TOK_RPAR && !funcmacroargs)
			break;
                    if (!wexpecttok(TOK_IDENT)) {
			skiptotoken2(TOK_COMMA, TOK_RPAR);
			continue;
		    }
                    sl2 = strlist_append(&funcmacroargs, curtoksym->name);
                    sl2->value = (long)curtoksym;
                    curtoksym->flags |= FMACREC;
                    gettok();
                } while (curtok == TOK_COMMA);
                if (!wneedtok(TOK_RPAR))
		    skippasttotoken(TOK_RPAR, TOK_EQ);
            }
            if (!wneedtok(TOK_EQ))
		break;
            if (sl)
                sl->value = (long)pc_expr();
            else
                sym->mbase->constdefn = pc_expr();
            for (sl2 = funcmacroargs; sl2; sl2 = sl2->next) {
                sym2 = (Symbol *)sl2->value;
                sym2->flags &= ~FMACREC;
            }
            strlist_empty(&funcmacroargs);
            break;

    }
    if (curtok != TOK_EOF)
        warning(format_s("Junk (%s) at end of macro definition [233]", tok_name(curtok)));
    pop_input();
    C_lex--;
    strlist_empty(&defsl);
}



void check_unused_macros()
{
    Strlist *sl;

    if (warnmacros) {
        for (sl = varmacros; sl; sl = sl->next)
            warning(format_s("VarMacro %s was never used [234]", sl->s));
        for (sl = constmacros; sl; sl = sl->next)
            warning(format_s("ConstMacro %s was never used [234]", sl->s));
        for (sl = fieldmacros; sl; sl = sl->next)
            warning(format_s("FieldMacro %s was never used [234]", sl->s));
        for (sl = funcmacros; sl; sl = sl->next)
            warning(format_s("FuncMacro %s was never used [234]", sl->s));
    }
}





#define skipspc(cp)   while (isspace(*cp)) cp++

Static int parsecomment(p2c_only, starparen)
int p2c_only, starparen;
{
    char namebuf[302];
    char *cp, *cp2 = namebuf, *closing, *after;
    char kind, chgmode, upcflag;
    long val, oldval, sign;
    double dval;
    int i, tempopt, hassign;
    Strlist *sp;
    Symbol *sym;

    if (if_flag)
        return 0;
    if (!p2c_only) {
        if (!strncmp(inbufptr, noskipcomment, strlen(noskipcomment)) &&
	     *noskipcomment) {
            inbufptr += strlen(noskipcomment);
	    if (skipflag < 0) {
		if (skipflag < -1) {
		    skipflag++;
		} else {
		    curtok = TOK_ENDIF;
		    skipflag = 1;
		    return 2;
		}
	    } else {
		skipflag = 1;
		return 1;
	    }
        }
    }
    closing = inbufptr;
    while (*closing && (starparen
			? (closing[0] != '*' || closing[1] != ')')
			: (closing[0] != '}')))
	closing++;
    if (!*closing)
	return 0;
    after = closing + (starparen ? 2 : 1);
    cp = inbufptr;
    while (cp < closing && (*cp != '#' || cp[1] != '#'))
	cp++;    /* Ignore comments */
    if (cp < closing) {
	while (isspace(cp[-1]))
	    cp--;
	*cp = '#';   /* avoid skipping spaces past closing! */
	closing = cp;
    }
    if (!p2c_only) {
        if (!strncmp(inbufptr, "DUMP-SYMBOLS", 12) &&
	     closing == inbufptr + 12) {
            wrapup();
            inbufptr = after;
            return 1;
        }
        if (!strncmp(inbufptr, fixedcomment, strlen(fixedcomment)) &&
	     *fixedcomment &&
	     inbufptr + strlen(fixedcomment) == closing) {
            fixedflag++;
            inbufptr = after;
            return 1;
        }
        if (!strncmp(inbufptr, permanentcomment, strlen(permanentcomment)) &&
	     *permanentcomment &&
	     inbufptr + strlen(permanentcomment) == closing) {
            permflag = 1;
            inbufptr = after;
            return 1;
        }
        if (!strncmp(inbufptr, interfacecomment, strlen(interfacecomment)) &&
	     *interfacecomment &&
	     inbufptr + strlen(interfacecomment) == closing) {
            inbufptr = after;
	    curtok = TOK_INTFONLY;
            return 2;
        }
        if (!strncmp(inbufptr, skipcomment, strlen(skipcomment)) &&
	     *skipcomment &&
	     inbufptr + strlen(skipcomment) == closing) {
            inbufptr = after;
	    skipflag--;
	    if (skipflag == -1) {
		skipping_module++;    /* eat comments in skipped portion */
		do {
		    gettok();
		} while (curtok != TOK_ENDIF);
		skipping_module--;
	    }
            return 1;
        }
	if (!strncmp(inbufptr, signedcomment, strlen(signedcomment)) &&
	     *signedcomment && !p2c_only &&
	     inbufptr + strlen(signedcomment) == closing) {
	    inbufptr = after;
	    gettok();
	    if (curtok == TOK_IDENT && curtokmeaning &&
		curtokmeaning->kind == MK_TYPE &&
		curtokmeaning->type == tp_char) {
		curtokmeaning = mp_schar;
	    } else
		warning("{SIGNED} applied to type other than CHAR [314]");
	    return 2;
	}
	if (!strncmp(inbufptr, unsignedcomment, strlen(unsignedcomment)) &&
	     *unsignedcomment && !p2c_only &&
	     inbufptr + strlen(unsignedcomment) == closing) {
	    inbufptr = after;
	    gettok();
	    if (curtok == TOK_IDENT && curtokmeaning &&
		curtokmeaning->kind == MK_TYPE &&
		curtokmeaning->type == tp_char) {
		curtokmeaning = mp_uchar;
	    } else if (curtok == TOK_IDENT && curtokmeaning &&
		       curtokmeaning->kind == MK_TYPE &&
		       curtokmeaning->type == tp_integer) {
		curtokmeaning = mp_unsigned;
	    } else if (curtok == TOK_IDENT && curtokmeaning &&
		       curtokmeaning->kind == MK_TYPE &&
		       curtokmeaning->type == tp_int) {
		curtokmeaning = mp_uint;
	    } else
		warning("{UNSIGNED} applied to type other than CHAR or INTEGER [313]");
	    return 2;
	}
        if (*inbufptr == '$') {
            i = turbo_directive(closing, after);
            if (i)
                return i;
        }
    }
    tempopt = 0;
    cp = inbufptr;
    if (*cp == '*') {
        cp++;
        tempopt = 1;
    }
    if (!isalpha(*cp))
        return 0;
    while ((isalnum(*cp) || *cp == '_') && cp2 < namebuf+300)
        *cp2++ = toupper(*cp++);
    *cp2 = 0;
    i = numparams;
    while (--i >= 0 && strcmp(rctable[i].name, namebuf)) ;
    if (i < 0)
        return 0;
    kind = rctable[i].kind;
    chgmode = rctable[i].chgmode;
    if (chgmode == ' ')    /* allowed in p2crc only */
        return 0;
    if (chgmode == 'T' && lex_initialized) {
        if (cp == closing || *cp == '=' || *cp == '+' || *cp == '-')
            warning(format_s("%s works only at top of program [235]",
                             rctable[i].name));
    }
    if (cp == closing) {
        if (kind == 'S' || kind == 'I' || kind == 'D' || kind == 'L' ||
	    kind == 'R' || kind == 'B' || kind == 'C' || kind == 'U') {
            undooption(i, "");
            inbufptr = after;
            return 1;
        }
    }
    switch (kind) {

        case 'S':
        case 'I':
        case 'L':
            val = oldval = (kind == 'L') ? *(( long *)rctable[i].ptr) :
                           (kind == 'S') ? *((short *)rctable[i].ptr) :
                                           *((  int *)rctable[i].ptr);
            switch (*cp) {

                case '=':
                    skipspc(cp);
		    hassign = (*++cp == '-' || *cp == '+');
                    sign = (*cp == '-') ? -1 : 1;
		    cp += hassign;
                    if (isdigit(*cp)) {
                        val = 0;
                        while (isdigit(*cp))
                            val = val * 10 + (*cp++) - '0';
                        val *= sign;
			if (kind == 'D' && !hassign)
			    val += 10000;
                    } else if (toupper(cp[0]) == 'D' &&
                               toupper(cp[1]) == 'E' &&
                               toupper(cp[2]) == 'F') {
                        val = rctable[i].def;
                        cp += 3;
                    }
                    break;

                case '+':
                case '-':
                    if (chgmode != 'R')
                        return 0;
                    for (;;) {
                        if (*cp == '+')
                            val++;
                        else if (*cp == '-')
                            val--;
                        else
                            break;
                        cp++;
                    }
                    break;

            }
            skipspc(cp);
            if (cp != closing)
                return 0;
            strlist_insert(&rcprevvalues[i], "")->value = oldval;
            if (tempopt)
                strlist_insert(&tempoptionlist, "")->value = i;
            if (kind == 'L')
                *((long *)rctable[i].ptr) = val;
            else if (kind == 'S')
                *((short *)rctable[i].ptr) = val;
            else
                *((int *)rctable[i].ptr) = val;
            inbufptr = after;
            return 1;

	case 'D':
            val = oldval = *((int *)rctable[i].ptr);
	    if (*cp++ != '=')
		return 0;
	    skipspc(cp);
	    if (toupper(cp[0]) == 'D' &&
		toupper(cp[1]) == 'E' &&
		toupper(cp[2]) == 'F') {
		val = rctable[i].def;
		cp += 3;
	    } else {
                cp2 = namebuf;
                while (*cp && cp != closing && !isspace(*cp))
                    *cp2++ = *cp++;
		*cp2 = 0;
		val = parsedelta(namebuf, -1);
		if (!val)
		    return 0;
	    }
	    skipspc(cp);
            if (cp != closing)
                return 0;
            strlist_insert(&rcprevvalues[i], "")->value = oldval;
            if (tempopt)
                strlist_insert(&tempoptionlist, "")->value = i;
            *((int *)rctable[i].ptr) = val;
            inbufptr = after;
            return 1;

        case 'R':
	    if (*cp++ != '=')
		return 0;
	    skipspc(cp);
	    if (toupper(cp[0]) == 'D' &&
		toupper(cp[1]) == 'E' &&
		toupper(cp[2]) == 'F') {
		dval = rctable[i].def / 100.0;
		cp += 3;
	    } else {
		cp2 = cp;
		while (isdigit(*cp) || *cp == '-' || *cp == '+' ||
		       *cp == '.' || toupper(*cp) == 'E')
		    cp++;
		if (cp == cp2)
		    return 0;
		dval = atof(cp2);
	    }
	    skipspc(cp);
	    if (cp != closing)
		return 0;
	    sprintf(namebuf, "%g", *((double *)rctable[i].ptr));
            strlist_insert(&rcprevvalues[i], namebuf);
            if (tempopt)
                strlist_insert(&tempoptionlist, namebuf)->value = i;
	    *((double *)rctable[i].ptr) = dval;
            inbufptr = after;
            return 1;

        case 'B':
	    if (*cp++ != '=')
		return 0;
	    skipspc(cp);
	    if (toupper(cp[0]) == 'D' &&
		toupper(cp[1]) == 'E' &&
		toupper(cp[2]) == 'F') {
		val = rctable[i].def;
		cp += 3;
	    } else {
		val = parse_breakstr(cp);
		while (*cp && cp != closing && !isspace(*cp))
		    cp++;
	    }
	    skipspc(cp);
	    if (cp != closing || val == -1)
		return 0;
            strlist_insert(&rcprevvalues[i], "")->value =
		*((short *)rctable[i].ptr);
            if (tempopt)
                strlist_insert(&tempoptionlist, "")->value = i;
	    *((short *)rctable[i].ptr) = val;
            inbufptr = after;
            return 1;

        case 'C':
        case 'U':
            if (*cp == '=') {
                cp++;
                skipspc(cp);
                for (cp2 = cp; cp2 != closing && !isspace(*cp2); cp2++)
                    if (!*cp2 || cp2-cp >= rctable[i].def)
                        return 0;
                cp2 = (char *)rctable[i].ptr;
                sp = strlist_insert(&rcprevvalues[i], cp2);
                if (tempopt)
                    strlist_insert(&tempoptionlist, "")->value = i;
                while (cp != closing && !isspace(*cp2))
                    *cp2++ = *cp++;
                *cp2 = 0;
                if (kind == 'U')
                    upc((char *)rctable[i].ptr);
                skipspc(cp);
                if (cp != closing)
                    return 0;
                inbufptr = after;
                if (!strcmp(rctable[i].name, "LANGUAGE") &&
                    !strcmp((char *)rctable[i].ptr, "MODCAL"))
                    sysprog_flag |= 2;
                return 1;
            }
            return 0;

        case 'F':
        case 'G':
            if (*cp == '=' || *cp == '+' || *cp == '-') {
                upcflag = (kind == 'F' && !pascalcasesens);
                chgmode = *cp++;
                skipspc(cp);
                cp2 = namebuf;
                while (isalnum(*cp) || *cp == '_' || *cp == '$' || *cp == '%')
                    *cp2++ = *cp++;
                *cp2++ = 0;
		if (!*namebuf)
		    return 0;
                skipspc(cp);
                if (cp != closing)
                    return 0;
                if (upcflag)
                    upc(namebuf);
                sym = findsymbol(namebuf);
		if (rctable[i].def & FUNCBREAK)
		    sym->flags &= ~FUNCBREAK;
                if (chgmode == '-')
                    sym->flags &= ~rctable[i].def;
                else
                    sym->flags |= rctable[i].def;
                inbufptr = after;
                return 1;
           }
           return 0;

        case 'A':
            if (*cp == '=' || *cp == '+' || *cp == '-') {
                chgmode = *cp++;
                skipspc(cp);
                cp2 = namebuf;
                while (cp != closing && !isspace(*cp) && *cp)
                    *cp2++ = *cp++;
                *cp2++ = 0;
                skipspc(cp);
                if (cp != closing)
                    return 0;
                if (chgmode != '+')
                    strlist_remove((Strlist **)rctable[i].ptr, namebuf);
                if (chgmode != '-')
                    sp = strlist_insert((Strlist **)rctable[i].ptr, namebuf);
                if (tempopt)
                    strlist_insert(&tempoptionlist, namebuf)->value = i;
                inbufptr = after;
                return 1;
            }
            return 0;

        case 'M':
            if (!isspace(*cp))
                return 0;
            skipspc(cp);
            if (!isalpha(*cp))
                return 0;
            for (cp2 = cp; *cp2 && cp2 != closing; cp2++) ;
            if (cp2 > cp && cp2 == closing) {
                inbufptr = after;
                cp2 = format_ds("%.*s", (int)(cp2-cp), cp);
                if (tp_integer != NULL) {
                    defmacro(cp2, rctable[i].def, NULL, 0);
                } else {
                    sp = strlist_append(&addmacros, cp2);
                    sp->value = rctable[i].def;
                }
                return 1;
            }
            return 0;

        case 'X':
            switch (rctable[i].def) {

                case 1:     /* strlist with string values */
                    if (!isspace(*cp) && *cp != '=' && 
                        *cp != '+' && *cp != '-')
                        return 0;
                    chgmode = *cp++;
                    skipspc(cp);
                    cp2 = namebuf;
                    while (isalnum(*cp) || *cp == '_' ||
			   *cp == '$' || *cp == '%' ||
			   *cp == '.' || *cp == '-' ||
			   (*cp == '\'' && cp[1] && cp[2] == '\'' &&
			    cp+1 != closing && cp[1] != '=')) {
			if (*cp == '\'') {
			    *cp2++ = *cp++;
			    *cp2++ = *cp++;
			}			    
                        *cp2++ = *cp++;
		    }
                    *cp2++ = 0;
                    if (chgmode == '-') {
                        skipspc(cp);
                        if (cp != closing)
                            return 0;
                        strlist_remove((Strlist **)rctable[i].ptr, namebuf);
                    } else {
                        if (!isspace(*cp) && *cp != '=')
                            return 0;
                        skipspc(cp);
                        if (*cp == '=') {
                            cp++;
                            skipspc(cp);
                        }
                        if (chgmode == '=' || isspace(chgmode))
                            strlist_remove((Strlist **)rctable[i].ptr, namebuf);
                        sp = strlist_append((Strlist **)rctable[i].ptr, namebuf);
                        if (tempopt)
                            strlist_insert(&tempoptionlist, namebuf)->value = i;
                        cp2 = namebuf;
                        while (*cp && cp != closing && !isspace(*cp))
                            *cp2++ = *cp++;
                        *cp2++ = 0;
                        skipspc(cp);
                        if (cp != closing)
                            return 0;
                        sp->value = (long)stralloc(namebuf);
                    }
                    inbufptr = after;
                    if (lex_initialized)
                        handle_nameof();        /* as good a place to do this as any! */
                    return 1;

                case 3:     /* Synonym parameter */
		    if (isspace(*cp) || *cp == '=' ||
			*cp == '+' || *cp == '-') {
			chgmode = *cp++;
			skipspc(cp);
			cp2 = namebuf;
			while (isalnum(*cp) || *cp == '_' ||
			       *cp == '$' || *cp == '%')
			    *cp2++ = *cp++;
			*cp2++ = 0;
			if (!*namebuf)
			    return 0;
			skipspc(cp);
			if (!pascalcasesens)
			    upc(namebuf);
			sym = findsymbol(namebuf);
			if (chgmode == '-') {
			    if (cp != closing)
				return 0;
			    sym->flags &= ~SSYNONYM;
			    inbufptr = after;
			    return 1;
			}
			if (*cp == '=') {
			    cp++;
			    skipspc(cp);
			}
			cp2 = namebuf;
			while (isalnum(*cp) || *cp == '_' ||
			       *cp == '$' || *cp == '%')
			    *cp2++ = *cp++;
			*cp2++ = 0;
			skipspc(cp);
			if (cp != closing)
			    return 0;
			sym->flags |= SSYNONYM;
			if (!pascalcasesens)
			    upc(namebuf);
			if (*namebuf)
			    strlist_append(&sym->symbolnames, "===")->value =
				(long)findsymbol(namebuf);
			else
			    strlist_append(&sym->symbolnames, "===")->value=0;
			inbufptr = after;
			return 1;
		    }
		    return 0;

            }
            return 0;

    }
    return 0;
}



Static void comment(starparen)
int starparen;    /* 0={ }, 1=(* *), 2=C comments*/
{
    register char ch;
    int nestcount = 1, startlnum = inf_lnum, wasrel = 0, trailing;
    int i, cmtindent, cmtindent2, saveeat = eatcomments;
    char *cp;

    if (!strncmp(inbufptr, embedcomment, strlen(embedcomment)) &&
	*embedcomment)
	eatcomments = 0;
    cp = inbuf;
    while (isspace(*cp))
	cp++;
    trailing = (*cp != '{' && ((*cp != '(' && *cp != '/') || cp[1] != '*'));
    cmtindent = inbufindent;
    cmtindent2 = cmtindent + 1 + (starparen != 0);
    cp = inbufptr;
    while (isspace(*cp))
	cmtindent2++, cp++;
    cp = curtokbuf;
    for (;;) {
        ch = *inbufptr++;
        switch (ch) {

            case '}':
                if ((!starparen || nestedcomments == 0) &&
		    starparen != 2 &&
                    --nestcount <= 0) {
                    *cp = 0;
		    if (wasrel && !strcmp(curtokbuf, "\003"))
			*curtokbuf = '\002';
		    if (!commenting_flag)
			commentline(trailing ? CMT_TRAIL : CMT_POST);
		    eatcomments = saveeat;
                    return;
                }
                break;

            case '{':
                if (nestedcomments == 1 && starparen != 2)
                    nestcount++;
                break;

            case '*':
                if ((*inbufptr == ((starparen == 2) ? '/' : ')') &&
		     (starparen || nestedcomments == 0)) &&
                    --nestcount <= 0) {
                    inbufptr++;
                    *cp = 0;
		    if (wasrel && !strcmp(curtokbuf, "\003"))
			*curtokbuf = '\002';
		    if (!commenting_flag)
			commentline(trailing ? CMT_TRAIL : CMT_POST);
		    eatcomments = saveeat;
                    return;
                }
                break;

            case '(':
                if (*inbufptr == '*' && nestedcomments == 1 &&
		    starparen != 2) {
		    *cp++ = ch;
		    ch = *inbufptr++;
                    nestcount++;
		}
                break;

            case 0:
                *cp = 0;
	        if (commenting_flag)
		    saveinputcomment(inbufptr-1);
		else
		    commentline(CMT_POST);
		trailing = 0;
                p2c_getline();
		i = 0;
		for (;;) {
		    if (*inbufptr == ' ') {
			inbufptr++;
			i++;
		    } else if (*inbufptr == '\t') {
			inbufptr++;
			i++;
			if (intabsize)
			    i = (i / intabsize + 1) * intabsize;
		    } else
			break;
		}
		cp = curtokbuf;
		if (*inbufptr) {
		    if (i == cmtindent2 && !starparen)
			cmtindent--;
		    cmtindent2 = -1;
		    if (i >= cmtindent && i > 0) {
			*cp++ = '\002';
			i -= cmtindent;
			wasrel = 1;
		    } else {
			*cp++ = '\003';
		    }
		    while (--i >= 0)
			*cp++ = ' ';
		} else
		    *cp++ = '\003';
                continue;

            case EOFMARK:
                error(format_d("Runaway comment from line %d", startlnum));
		eatcomments = saveeat;
                return;     /* unnecessary */

        }
        *cp++ = ch;
    }
}



char *getinlinepart()
{
    char *cp, *buf;

    for (;;) {
        if (isspace(*inbufptr)) {
            inbufptr++;
        } else if (!*inbufptr) {
            p2c_getline();
        } else if (*inbufptr == '{') {
            inbufptr++;
            comment(0);
        } else if (*inbufptr == '(' && inbufptr[1] == '*') {
            inbufptr += 2;
            comment(1);
        } else
            break;
    }
    cp = inbufptr;
    while (isspace(*cp) || isalnum(*cp) ||
           *cp == '_' || *cp == '$' || 
           *cp == '+' || *cp == '-' ||
           *cp == '<' || *cp == '>')
        cp++;
    if (cp == inbufptr)
        return "";
    while (isspace(cp[-1]))
        cp--;
    buf = format_s("%s", inbufptr);
    buf[cp-inbufptr] = 0;     /* truncate the string */
    inbufptr = cp;
    return buf;
}




Static int getflag()
{
    int res = 1;

    gettok();
    if (curtok == TOK_IDENT) {
        res = (strcmp(curtokbuf, "OFF") != 0);
        gettok();
    }
    return res;
}




char getchartok()
{
    if (!*inbufptr) {
        warning("Unexpected end of line [236]");
        return ' ';
    }
    if (isspace(*inbufptr)) {
        warning("Whitespace not allowed here [237]");
        return ' ';
    }
    return *inbufptr++;
}



char *getparenstr(buf)
char *buf;
{
    int count = 0;
    char *cp;

    if (inbufptr < buf)    /* this will get most bad cases */
        error("Can't handle a line break here");
    while (isspace(*buf))
        buf++;
    cp = buf;
    for (;;) {
        if (!*cp)
            error("Can't handle a line break here");
        if (*cp == '(')
            count++;
        if (*cp == ')')
            if (--count < 0)
                break;
        cp++;
    }
    inbufptr = cp + 1;
    while (cp > buf && isspace(cp[-1]))
        cp--;
    return format_ds("%.*s", (int)(cp - buf), buf);
}



void leadingcomments()
{
    for (;;) {
        switch (*inbufptr++) {

            case 0:
                p2c_getline();
                break;

            case ' ':
            case '\t':
            case 26:
                /* ignore whitespace */
                break;

            case '{':
                if (!parsecomment(1, 0)) {
                    inbufptr--;
                    return;
                }
                break;

	    case '(':
		if (*inbufptr == '*') {
		    inbufptr++;
		    if (!parsecomment(1, 1)) {
			inbufptr -= 2;
			return;
		    }
		    break;
		}
		/* fall through */

            default:
                inbufptr--;
                return;

        }
    }
}




void get_C_string(term)
int term;
{
    char *cp = curtokbuf;
    char ch;
    int i;

    while ((ch = *inbufptr++)) {
        if (ch == term) {
            *cp = 0;
            curtokint = cp - curtokbuf;
            return;
        } else if (ch == '\\') {
            if (isdigit(*inbufptr)) {
                i = (*inbufptr++) - '0';
                if (isdigit(*inbufptr))
                    i = i*8 + (*inbufptr++) - '0';
                if (isdigit(*inbufptr))
                    i = i*8 + (*inbufptr++) - '0';
                *cp++ = i;
            } else {
                ch = *inbufptr++;
                switch (tolower(ch)) {
                    case 'n':
                        *cp++ = '\n';
                        break;
                    case 't':
                        *cp++ = '\t';
                        break;
                    case 'v':
                        *cp++ = '\v';
                        break;
                    case 'b':
                        *cp++ = '\b';
                        break;
                    case 'r':
                        *cp++ = '\r';
                        break;
                    case 'f':
                        *cp++ = '\f';
                        break;
                    case '\\':
                        *cp++ = '\\';
                        break;
                    case '\'':
                        *cp++ = '\'';
                        break;
                    case '"':
                        *cp++ = '"';
                        break;
                    case 'x':
                        if (isxdigit(*inbufptr)) {
                            if (isdigit(*inbufptr))
                                i = (*inbufptr++) - '0';
                            else
                                i = (toupper(*inbufptr++)) - 'A' + 10;
                            if (isdigit(*inbufptr))
                                i = i*16 + (*inbufptr++) - '0';
                            else if (isxdigit(*inbufptr))
                                i = i*16 + (toupper(*inbufptr++)) - 'A' + 10;
                            *cp++ = i;
                            break;
                        }
                        /* fall through */
                    default:
                        warning("Strange character in C string [238]");
                }
            }
        } else
            *cp++ = ch;
    }
    *cp = 0;
    curtokint = cp - curtokbuf;
    warning("Unterminated C string [239]");
}





void begincommenting(cp)
char *cp;
{
    if (!commenting_flag) {
	commenting_ptr = cp;
    }
    commenting_flag++;
}


void saveinputcomment(cp)
char *cp;
{
    if (commenting_ptr)
	sprintf(curtokbuf, "%.*s", (int)(cp - commenting_ptr), commenting_ptr);
    else
	sprintf(curtokbuf, "\003%.*s", (int)(cp - inbuf), inbuf);
    commentline(CMT_POST);
    commenting_ptr = NULL;
}


void endcommenting(cp)
char *cp;
{
    commenting_flag--;
    if (!commenting_flag) {
	saveinputcomment(cp);
    }
}




int peeknextchar()
{
    char *cp;

    cp = inbufptr;
    while (isspace(*cp))
	cp++;
    return *cp;
}




#ifdef LEXDEBUG
Static void zgettok();
void gettok()
{
    zgettok();
    if (tokentrace) {
        printf("gettok() found %s", tok_name(curtok));
        switch (curtok) {
            case TOK_HEXLIT:
            case TOK_OCTLIT:
            case TOK_INTLIT:
            case TOK_MININT:
                printf(", curtokint = %d", curtokint);
                break;
            case TOK_REALLIT:
            case TOK_STRLIT:
                printf(", curtokbuf = %s", makeCstring(curtokbuf, curtokint));
                break;
	    default:
		break;
        }
        putchar('\n');
    }
}
Static void zgettok()
#else
void gettok()
#endif
{
    register char ch;
    register char *cp;
    char ch2;
    char *startcp;
    int i;

    debughook();
    for (;;) {
        switch ((ch = *inbufptr++)) {

            case 0:
	        if (commenting_flag)
		    saveinputcomment(inbufptr-1);
                p2c_getline();
		cp = curtokbuf;
		for (;;) {
		    inbufindent = 0;
		    for (;;) {
			if (*inbufptr == '\t') {
			    inbufindent++;
			    if (intabsize)
				inbufindent = (inbufindent / intabsize + 1) * intabsize;
			} else if (*inbufptr == ' ')
			    inbufindent++;
			else if (*inbufptr != 26)
			    break;
			inbufptr++;
		    }
		    if (!*inbufptr && !commenting_flag) {   /* blank line */
			*cp++ = '\001';
			p2c_getline();
		    } else
			break;
		}
		if (cp > curtokbuf) {
		    *cp = 0;
		    commentline(CMT_POST);
		}
                break;

            case '\t':
            case ' ':
            case 26:    /* ignore ^Z's in Turbo files */
                while (*inbufptr++ == ch) ;
                inbufptr--;
                break;

            case '$':
		if (dollar_idents)
		    goto ident;
                if (dollar_flag) {
                    dollar_flag = 0;
                    curtok = TOK_DOLLAR;
                    return;
		}
		startcp = inbufptr-1;
		while (isspace(*inbufptr))
		    inbufptr++;
		cp = inbufptr;
		while (isxdigit(*cp))
		    cp++;
		if (cp > inbufptr && cp <= inbufptr+8 && !isalnum(*cp)) {
		    while (isspace(*cp))
			cp++;
		    if (!isdigit(*cp) && *cp != '\'') {
			cp = curtokbuf;    /* Turbo hex constant */
			while (isxdigit(*inbufptr))
			    *cp++ = *inbufptr++;
			*cp = 0;
			curtok = TOK_HEXLIT;
			curtokint = my_strtol(curtokbuf, NULL, 16);
			return;
		    }
                }
		dollar_flag++;     /* HP Pascal compiler directive */
		do {
		    gettok();
		    if (curtok == TOK_IF) {             /* $IF expr$ */
			Expr *ex;
			Value val;
			if (!skipping_module) {
			    if (!setup_complete)
				error("$IF$ not allowed at top of program");

			    /* Even though HP Pascal doesn't let these nest,
			       there's no harm in supporting it. */
			    if (if_flag) {
				skiptotoken(TOK_DOLLAR);
				if_flag++;
				break;
			    }
			    gettok();
			    ex = p_expr(tp_boolean);
			    val = eval_expr_consts(ex);
			    freeexpr(ex);
			    i = (val.type == tp_boolean && val.i);
			    free_value(&val);
			    if (!i) {
				if (curtok != TOK_DOLLAR) {
				    warning("Syntax error in $IF$ expression [240]");
				    skiptotoken(TOK_DOLLAR);
				}
				begincommenting(startcp);
				if_flag++;
				while (if_flag > 0)
				    gettok();
				endcommenting(inbufptr);
			    }
			} else {
			    skiptotoken(TOK_DOLLAR);
			}
		    } else if (curtok == TOK_END) {     /* $END$ */
			if (if_flag) {
			    gettok();
			    if (!wexpecttok(TOK_DOLLAR))
				skiptotoken(TOK_DOLLAR);
			    curtok = TOK_ENDIF;
			    if_flag--;
			    return;
			} else {
			    gettok();
			    if (!wexpecttok(TOK_DOLLAR))
				skiptotoken(TOK_DOLLAR);
			}
		    } else if (curtok == TOK_IDENT) {
			if (!strcmp(curtokbuf, "INCLUDE") &&
			     !if_flag && !skipping_module) {
			    char *fn;
			    gettok();
			    if (curtok == TOK_IDENT) {
				fn = stralloc(curtokcase);
				gettok();
			    } else if (wexpecttok(TOK_STRLIT)) {
				fn = stralloc(curtokbuf);
				gettok();
			    } else
				fn = "";
			    if (!wexpecttok(TOK_DOLLAR)) {
				skiptotoken(TOK_DOLLAR);
			    } else {
				if (handle_include(fn))
				    return;
			    }
			} else if (ignore_directives ||
				   if_flag ||
				   !strcmp(curtokbuf, "SEARCH") ||
				   !strcmp(curtokbuf, "REF") ||
				   !strcmp(curtokbuf, "DEF")) {
			    skiptotoken(TOK_DOLLAR);
			} else if (!strcmp(curtokbuf, "SWITCH_STRPOS")) {
			    switch_strpos = getflag();
			} else if (!strcmp(curtokbuf, "SYSPROG")) {
			    if (getflag())
				sysprog_flag |= 1;
			    else
				sysprog_flag &= ~1;
			} else if (!strcmp(curtokbuf, "MODCAL")) {
			    if (getflag())
				sysprog_flag |= 2;
			    else
				sysprog_flag &= ~2;
			} else if (!strcmp(curtokbuf, "PARTIAL_EVAL")) {
			    if (shortcircuit < 0)
				partial_eval_flag = getflag();
			} else if (!strcmp(curtokbuf, "IOCHECK")) {
			    iocheck_flag = getflag();
			} else if (!strcmp(curtokbuf, "RANGE")) {
			    if (getflag()) {
				if (!range_flag)
				    note("Range checking is ON [216]");
				range_flag = 1;
			    } else {
				if (range_flag)
				    note("Range checking is OFF [216]");
				range_flag = 0;
			    }
			} else if (!strcmp(curtokbuf, "OVFLCHECK")) {
			    if (getflag()) {
				if (!ovflcheck_flag)
				    note("Overflow checking is ON [219]");
				ovflcheck_flag = 1;
			    } else {
				if (ovflcheck_flag)
				    note("Overflow checking is OFF [219]");
				ovflcheck_flag = 0;
			    }
			} else if (!strcmp(curtokbuf, "STACKCHECK")) {
			    if (getflag()) {
				if (!stackcheck_flag)
				    note("Stack checking is ON [217]");
				stackcheck_flag = 1;
			    } else {
				if (stackcheck_flag)
				    note("Stack checking is OFF [217]");
				stackcheck_flag = 0;
			    }
			}
			skiptotoken2(TOK_DOLLAR, TOK_COMMA);
		    } else {
			warning("Mismatched '$' signs [241]");
			dollar_flag = 0;    /* got out of sync */
			return;
		    }
		} while (curtok == TOK_COMMA);
                break;

            case '"':
		if (C_lex) {
		    get_C_string(ch);
		    curtok = TOK_STRLIT;
		    return;
		}
		goto stringLiteral;

            case '#':
		if (modula2) {
		    curtok = TOK_NE;
		    return;
		}
		cp = inbufptr;
		while (isspace(*cp)) cp++;
		if (!strcincmp(cp, "INCLUDE", 7)) {
		    char *cp2, *cp3;
		    cp += 7;
		    while (isspace(*cp)) cp++;
		    cp2 = cp + strlen(cp) - 1;
		    while (isspace(*cp2)) cp2--;
		    if ((*cp == '"' && *cp2 == '"' && cp2 > cp) ||
			(*cp == '<' && *cp2 == '>')) {
			inbufptr = cp2 + 1;
			cp3 = stralloc(cp + 1);
			cp3[cp2 - cp - 1] = 0;
			if (handle_include(cp3))
			    return;
			else
			    break;
		    }
		}
		/* fall through */

            case '\'':
                if (C_lex && ch == '\'') {
                    get_C_string(ch);
                    if (curtokint != 1)
                        warning("Character constant has length != 1 [242]");
                    curtokint = *curtokbuf;
                    curtok = TOK_CHARLIT;
                    return;
                }
	      stringLiteral:
                cp = curtokbuf;
		ch2 = (ch == '"') ? '"' : '\'';
                do {
                    if (ch == ch2) {
                        while ((ch = *inbufptr++) != '\n' &&
                               ch != EOF) {
                            if (ch == ch2) {
                                if (*inbufptr != ch2 || modula2)
                                    break;
                                else
                                    inbufptr++;
                            }
                            *cp++ = ch;
                        }
                        if (ch != ch2)
                            warning("Error in string literal [243]");
                    } else {
                        ch = *inbufptr++;
                        if (isdigit(ch)) {
                            i = 0;
                            while (isdigit(ch)) {
                                i = i*10 + ch - '0';
                                ch = *inbufptr++;
                            }
                            inbufptr--;
                            *cp++ = i;
                        } else {
                            *cp++ = ch & 0x1f;
                        }
                    }
                    while (*inbufptr == ' ' || *inbufptr == '\t')
                        inbufptr++;
                } while ((ch = *inbufptr++) == ch2 || ch == '#');
                inbufptr--;
                *cp = 0;
                curtokint = cp - curtokbuf;
                curtok = TOK_STRLIT;
                return;

            case '(':
                if (*inbufptr == '*' && !C_lex) {
                    inbufptr++;
		    switch (commenting_flag ? 0 : parsecomment(0, 1)) {
		        case 0:
                            comment(1);
			    break;
		        case 2:
			    return;
		    }
                    break;
                } else if (*inbufptr == '.') {
                    curtok = TOK_LBR;
                    inbufptr++;
                } else {
                    curtok = TOK_LPAR;
                }
                return;

            case '{':
                if (C_lex || modula2) {
                    curtok = TOK_LBRACE;
                    return;
                }
                switch (commenting_flag ? 0 : parsecomment(0, 0)) {
                    case 0:
                        comment(0);
                        break;
                    case 2:
                        return;
                }
                break;

            case '}':
                if (C_lex || modula2) {
                    curtok = TOK_RBRACE;
                    return;
                }
		if (skipflag > 0) {
		    skipflag = 0;
		} else
		    warning("Unmatched '}' in input file [244]");
                break;

            case ')':
                curtok = TOK_RPAR;
                return;

            case '*':
		if (*inbufptr == (C_lex ? '/' : ')')) {
		    inbufptr++;
		    if (skipflag > 0) {
			skipflag = 0;
		    } else
			warning("Unmatched '*)' in input file [245]");
		    break;
		} else if (*inbufptr == '*' && !C_lex) {
		    curtok = TOK_STARSTAR;
		    inbufptr++;
		} else
		    curtok = TOK_STAR;
                return;

            case '+':
                if (C_lex && *inbufptr == '+') {
                    curtok = TOK_PLPL;
                    inbufptr++;
                } else
                    curtok = TOK_PLUS;
                return;

            case ',':
                curtok = TOK_COMMA;
                return;

            case '-':
                if (C_lex && *inbufptr == '-') {
                    curtok = TOK_MIMI;
                    inbufptr++;
                } else if (*inbufptr == '>') {
                    curtok = TOK_ARROW;
                    inbufptr++;
                } else
                    curtok = TOK_MINUS;
                return;

            case '.':
                if (*inbufptr == '.') {
                    curtok = TOK_DOTS;
                    inbufptr++;
                } else if (*inbufptr == ')') {
                    curtok = TOK_RBR;
                    inbufptr++;
                } else
                    curtok = TOK_DOT;
                return;

            case '/':
		if (C_lex && *inbufptr == '*') {
		    inbufptr++;
		    comment(2);
		    break;
		}
                curtok = TOK_SLASH;
                return;

            case ':':
                if (*inbufptr == '=') {
                    curtok = TOK_ASSIGN;
                    inbufptr++;
		} else if (*inbufptr == ':') {
                    curtok = TOK_COLONCOLON;
                    inbufptr++;
                } else
                    curtok = TOK_COLON;
                return;

            case ';':
                curtok = TOK_SEMI;
                return;

            case '<':
                if (*inbufptr == '=') {
                    curtok = TOK_LE;
                    inbufptr++;
                } else if (*inbufptr == '>') {
                    curtok = TOK_NE;
                    inbufptr++;
                } else if (*inbufptr == '<') {
                    curtok = TOK_LTLT;
                    inbufptr++;
                } else
                    curtok = TOK_LT;
                return;

            case '>':
                if (*inbufptr == '=') {
                    curtok = TOK_GE;
                    inbufptr++;
                } else if (*inbufptr == '>') {
                    curtok = TOK_GTGT;
                    inbufptr++;
                } else
                    curtok = TOK_GT;
                return;

            case '=':
		if (*inbufptr == '=') {
		    curtok = TOK_EQEQ;
		    inbufptr++;
		} else
		    curtok = TOK_EQ;
                return;

            case '[':
                curtok = TOK_LBR;
                return;

            case ']':
                curtok = TOK_RBR;
                return;

            case '^':
                curtok = TOK_HAT;
                return;

            case '&':
                if (*inbufptr == '&') {
                    curtok = TOK_ANDAND;
                    inbufptr++;
                } else
                    curtok = TOK_AMP;
                return;

            case '|':
                if (*inbufptr == '|') {
                    curtok = TOK_OROR;
                    inbufptr++;
                } else
                    curtok = TOK_VBAR;
                return;

            case '~':
                curtok = TOK_TWIDDLE;
                return;

            case '!':
                if (*inbufptr == '=') {
                    curtok = TOK_BANGEQ;
                    inbufptr++;
                } else
                    curtok = TOK_BANG;
                return;

            case '%':
		if (C_lex) {
		    curtok = TOK_PERC;
		    return;
		}
		goto ident;

            case '?':
                curtok = TOK_QM;
                return;

            case '@':
		curtok = TOK_ADDR;
                return;

            case EOFMARK:
                if (curtok == TOK_EOF) {
                    if (inputkind == INP_STRLIST)
                        error("Unexpected end of macro");
                    else
                        error("Unexpected end of file");
                }
                curtok = TOK_EOF;
                return;

            default:
                if (isdigit(ch)) {
		    cp = inbufptr;
		    while (isxdigit(*cp))
			cp++;
		    if (*cp == '#' && isxdigit(cp[1])) {
			i = atoi(inbufptr-1);
			inbufptr = cp+1;
		    } else if (toupper(cp[-1]) == 'B' ||
			       toupper(cp[-1]) == 'C') {
                        inbufptr--;
			i = 8;
		    } else if (toupper(*cp) == 'H') {
                        inbufptr--;
			i = 16;
		    } else if ((ch == '0' && toupper(*inbufptr) == 'X' &&
				isxdigit(inbufptr[1]))) {
			inbufptr++;
			i = 16;
		    } else {
			i = 10;
		    }
		    if (i != 10) {
                        curtokint = 0;
                        while (isdigit(*inbufptr) ||
			       (i > 10 && isxdigit(*inbufptr))) {
                            ch = toupper(*inbufptr++);
                            curtokint *= i;
                            if (ch <= '9')
                                curtokint += ch - '0';
                            else
                                curtokint += ch - 'A' + 10;
                        }
                        sprintf(curtokbuf, "%ld", curtokint);
			if ((toupper(*inbufptr) == 'B' && i == 8) ||
			    (toupper(*inbufptr) == 'H' && i == 16))
			    inbufptr++;
			if (toupper(*inbufptr) == 'C' && i == 8) {
			    inbufptr++;
			    curtok = TOK_STRLIT;
			    curtokbuf[0] = curtokint;
			    curtokbuf[1] = 0;
			    curtokint = 1;
			    return;
			}
                        if (toupper(*inbufptr) == 'L') {
                            strcat(curtokbuf, "L");
                            inbufptr++;
                        }
                        curtok = (i == 8) ? TOK_OCTLIT : TOK_HEXLIT;
                        return;
                    }
                    cp = curtokbuf;
                    i = 0;
                    while (ch == '0')
                        ch = *inbufptr++;
                    if (isdigit(ch)) {
                        while (isdigit(ch)) {
                            *cp++ = ch;
                            ch = *inbufptr++;
                        }
                    } else
                        *cp++ = '0';
                    if (ch == '.') {
                        if (isdigit(*inbufptr)) {
                            *cp++ = ch;
                            ch = *inbufptr++;
                            i = 1;
                            while (isdigit(ch)) {
                                *cp++ = ch;
                                ch = *inbufptr++;
                            }
                        }
                    }
                    if (ch == 'e' || ch == 'E' ||
			ch == 'd' || ch == 'D' ||
			ch == 'q' || ch == 'Q') {
                        ch = *inbufptr;
                        if (isdigit(ch) || ch == '+' || ch == '-') {
                            *cp++ = 'e';
                            inbufptr++;
                            i = 1;
                            do {
                                *cp++ = ch;
                                ch = *inbufptr++;
                            } while (isdigit(ch));
                        }
                    }
                    inbufptr--;
                    *cp = 0;
                    if (i) {
                        curtok = TOK_REALLIT;
                        curtokint = cp - curtokbuf;
                    } else {
                        if (cp >= curtokbuf+10) {
                            i = strcmp(curtokbuf, "2147483648");
                            if (cp > curtokbuf+10 || i > 0) {
				curtok = TOK_REALLIT;
				curtokint = cp - curtokbuf + 2;
				strcat(curtokbuf, ".0");
				return;
			    }
                            if (i == 0) {
                                curtok = TOK_MININT;
                                curtokint = -2147483648;
                                return;
                            }
                        }
                        curtok = TOK_INTLIT;
                        curtokint = atol(curtokbuf);
                        if (toupper(*inbufptr) == 'L') {
                            strcat(curtokbuf, "L");
                            inbufptr++;
                        }
                    }
                    return;
                } else if (isalpha(ch) || ch == '_') {
ident:
                    {
                        register char *cp2;
                        curtoksym = NULL;
                        cp = curtokbuf;
                        cp2 = curtokcase;
			*cp2++ = symcase ? ch : tolower(ch);
			*cp++ = pascalcasesens ? ch : toupper(ch);
			while (isalnum((ch = *inbufptr++)) ||
			       ch == '_' ||
			       (ch == '%' && !C_lex) ||
			       (ch == '$' && dollar_idents)) {
			    *cp2++ = symcase ? ch : tolower(ch);
			    if (!ignorenonalpha || isalnum(ch))
				*cp++ = pascalcasesens ? ch : toupper(ch);
			}
                        inbufptr--;
                        *cp2 = 0;
                        *cp = 0;
			if (pascalsignif > 0)
			    curtokbuf[pascalsignif] = 0;
                    }
		    if (*curtokbuf == '%') {
			if (!strcicmp(curtokbuf, "%INCLUDE")) {
			    char *cp2 = inbufptr;
			    while (isspace(*cp2)) cp2++;
			    if (*cp2 == '\'')
				cp2++;
			    cp = curtokbuf;
			    while (*cp2 && *cp2 != '\'' &&
				   *cp2 != ';' && !isspace(*cp2)) {
				*cp++ = *cp2++;
			    }
			    *cp = 0;
			    cp = my_strrchr(curtokbuf, '/');
			    if (cp && (!strcicmp(cp, "/LIST") ||
				       !strcicmp(cp, "/NOLIST")))
				*cp = 0;
			    if (*cp2 == '\'')
				cp2++;
			    while (isspace(*cp2)) cp2++;
			    if (*cp2 == ';')
				cp2++;
			    while (isspace(*cp2)) cp2++;
			    if (!*cp2) {
				inbufptr = cp2;
				(void) handle_include(stralloc(curtokbuf));
				return;
			    }
			} else if (!strcicmp(curtokbuf, "%TITLE") ||
				   !strcicmp(curtokbuf, "%SUBTITLE")) {
			    gettok();   /* string literal */
			    break;
			} else if (!strcicmp(curtokbuf, "%PAGE")) {
			    /* should store a special page-break comment? */
			    break;   /* ignore token */
			} else if ((i = 2, !strcicmp(curtokbuf, "%B")) ||
				   (i = 8, !strcicmp(curtokbuf, "%O")) ||
				   (i = 16, !strcicmp(curtokbuf, "%X"))) {
			    while (isspace(*inbufptr)) inbufptr++;
			    if (*inbufptr == '\'') {
				inbufptr++;
				curtokint = 0;
				while (*inbufptr && *inbufptr != '\'') {
				    ch = toupper(*inbufptr++);
				    if (isxdigit(ch)) {
					curtokint *= i;
					if (ch <= '9')
					    curtokint += ch - '0';
					else
					    curtokint += ch - 'A' + 10;
				    } else if (!isspace(ch))
					warning("Bad digit in literal [246]");
				}
				if (*inbufptr)
				    inbufptr++;
				sprintf(curtokbuf, "%ld", curtokint);
				curtok = (i == 8) ? TOK_OCTLIT : TOK_HEXLIT;
				return;
			    }
                        }
		    }
                    {
                        register unsigned int hash;
                        register Symbol *sp;

                        hash = 0;
                        for (cp = curtokbuf; *cp; cp++)
                            hash = hash*3 + *cp;
                        sp = symtab[hash % SYMHASHSIZE];
                        while (sp && (i = strcmp(sp->name, curtokbuf)) != 0) {
                            if (i < 0)
                                sp = sp->left;
                            else
                                sp = sp->right;
                        }
                        if (!sp)
                            sp = findsymbol(curtokbuf);
			if (sp->flags & SSYNONYM) {
			    i = 100;
			    while (--i > 0 && sp && (sp->flags & SSYNONYM)) {
				Strlist *sl;
				sl = strlist_find(sp->symbolnames, "===");
				if (sl)
				    sp = (Symbol *)sl->value;
				else
				    sp = NULL;
			    }
			    if (!sp)
				break;    /* ignore token */
			}
			if (sp->kwtok && !(sp->flags & KWPOSS) &&
			    (pascalcasesens != 2 || !islower(*curtokbuf)) &&
			    (pascalcasesens != 3 || !isupper(*curtokbuf))) {
			    curtok = sp->kwtok;
			    return;
			}
			curtok = TOK_IDENT;
                        curtoksym = sp;
                        if ((i = withlevel) != 0 && sp->fbase) {
                            while (--i >= 0) {
                                curtokmeaning = sp->fbase;
                                while (curtokmeaning) {
                                    if (curtokmeaning->rectype == withlist[i]) {
                                        curtokint = i;
                                        return;
                                    }
                                    curtokmeaning = curtokmeaning->snext;
                                }
                            }
                        }
                        curtokmeaning = sp->mbase;
                        while (curtokmeaning && !curtokmeaning->isactive)
                            curtokmeaning = curtokmeaning->snext;
			if (!curtokmeaning)
			    return;
			while (curtokmeaning->kind == MK_SYNONYM)
			    curtokmeaning = curtokmeaning->xnext;
			/* look for unit.ident notation */
                        if (curtokmeaning->kind == MK_MODULE ||
			    curtokmeaning->kind == MK_FUNCTION) {
                            for (cp = inbufptr; isspace(*cp); cp++) ;
                            if (*cp == '.') {
                                for (cp++; isspace(*cp); cp++) ;
                                if (isalpha(*cp)) {
                                    Meaning *mp = curtokmeaning;
                                    Symbol *sym = curtoksym;
                                    char *saveinbufptr = inbufptr;
                                    gettok();
                                    if (curtok == TOK_DOT)
					gettok();
				    else
					curtok = TOK_END;
                                    if (curtok == TOK_IDENT) {
					curtokmeaning = curtoksym->mbase;
					while (curtokmeaning &&
					       curtokmeaning->ctx != mp)
					    curtokmeaning = curtokmeaning->snext;
					if (!curtokmeaning &&
					    !strcmp(sym->name, "SYSTEM")) {
					    curtokmeaning = curtoksym->mbase;
					    while (curtokmeaning &&
						   curtokmeaning->ctx != nullctx)
						curtokmeaning = curtokmeaning->snext;
					}
				    } else
					curtokmeaning = NULL;
                                    if (!curtokmeaning) {
                                        /* oops, was probably funcname.field */
                                        inbufptr = saveinbufptr;
                                        curtokmeaning = mp;
                                        curtoksym = sym;
                                    }
                                }
                            }
                        }
                        return;
                    }
                } else {
                    warning(format_d("Unrecognized character 0%o in file [247]",
				     ch));
                }
        }
    }
}



void checkkeyword(tok)
Token tok;
{
    if (curtok == TOK_IDENT &&
	curtoksym->kwtok == tok) {
	curtoksym->flags &= ~KWPOSS;
	curtok = tok;
    }
}


void checkmodulewords()
{
    if (modula2) {
	checkkeyword(TOK_FROM);
	checkkeyword(TOK_DEFINITION);
	checkkeyword(TOK_IMPLEMENT);
	checkkeyword(TOK_MODULE);
	checkkeyword(TOK_IMPORT);
	checkkeyword(TOK_EXPORT);
    } else if (curtok == TOK_IDENT &&
	       (curtoksym->kwtok == TOK_MODULE ||
		curtoksym->kwtok == TOK_IMPORT ||
		curtoksym->kwtok == TOK_EXPORT ||
		curtoksym->kwtok == TOK_IMPLEMENT)) {
	if (!strcmp(curtokbuf, "UNIT") ||
	    !strcmp(curtokbuf, "USES") ||
	    !strcmp(curtokbuf, "INTERFACE") ||
	    !strcmp(curtokbuf, "IMPLEMENTATION")) {
	    modulenotation = 0;
	    findsymbol("UNIT")->flags &= ~KWPOSS;
	    findsymbol("USES")->flags &= ~KWPOSS;
	    findsymbol("INTERFACE")->flags &= ~KWPOSS;
	    findsymbol("IMPLEMENTATION")->flags &= ~KWPOSS;
	} else {
	    modulenotation = 1;
	    findsymbol("MODULE")->flags &= ~KWPOSS;
	    findsymbol("EXPORT")->flags &= ~KWPOSS;
	    findsymbol("IMPORT")->flags &= ~KWPOSS;
	    findsymbol("IMPLEMENT")->flags &= ~KWPOSS;
	}
	curtok = curtoksym->kwtok;
    }
}












/* End. */



