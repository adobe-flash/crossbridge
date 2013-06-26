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



#define PROTO_DECL_C
#include "trans.h"



#define MAXIMPORTS 100



Static struct ptrdesc {
    struct ptrdesc *next;
    Symbol *sym;
    Type *tp;
} *ptrbase;

Static struct ctxstack {
    struct ctxstack *next;
    Meaning *ctx, *ctxlast;
    struct tempvarlist *tempvars;
    int tempvarcount, importmark;
} *ctxtop;

Static struct tempvarlist {
    struct tempvarlist *next;
    Meaning *tvar;
    int active;
} *tempvars, *stmttempvars;

Static int tempvarcount;

Static int stringtypecachesize;
Static Type **stringtypecache;

Static Meaning *importlist[MAXIMPORTS];
Static int firstimport;

Static Type *tp_special_anyptr;

Static int wasaliased;
Static int deferallptrs;
Static int anydeferredptrs;
Static int silentalreadydef;
Static int nonloclabelcount;

Static Strlist *varstructdecllist;




Static Meaning *findstandardmeaning(kind, name)
enum meaningkind kind;
char *name;
{
    Meaning *mp;
    Symbol *sym;

    sym = findsymbol(fixpascalname(name));
    for (mp = sym->mbase; mp && mp->ctx != curctx; mp = mp->snext) ;
    if (mp) {
	if (mp->kind == kind)
	    mp->refcount = 1;
	else
	    mp = NULL;
    }
    return mp;
}


Static Meaning *makestandardmeaning(kind, name)
enum meaningkind kind;
char *name;
{
    Meaning *mp;
    Symbol *sym;

    sym = findsymbol(fixpascalname(name));
    for (mp = sym->mbase; mp && mp->ctx != curctx; mp = mp->snext) ;
    if (!mp) {
        mp = addmeaning(sym, kind);
        strchange(&mp->name, name);
        if (debug < 4)
            mp->dumped = partialdump;     /* prevent irrelevant dumping */
    } else {
        mp->kind = kind;
    }
    mp->refcount = 1;
    return mp;
}


Static Type *makestandardtype(kind, mp)
enum typekind kind;
Meaning *mp;
{
    Type *tp;

    tp = maketype(kind);
    tp->meaning = mp;
    if (mp)
        mp->type = tp;
    return tp;
}




Static Stmt *nullspecialproc(mp)
Meaning *mp;
{
    warning(format_s("Procedure %s not yet supported [118]", mp->name));
    if (curtok == TOK_LPAR)
        skipparens();
    return NULL;
}

Meaning *makespecialproc(name, handler)
char *name;
Stmt *(*handler)();
{
    Meaning *mp;

    if (!handler)
        handler = nullspecialproc;
    mp = makestandardmeaning(MK_SPECIAL, name);
    mp->handler = (Expr *(*)())handler;
    return mp;
}



Static Stmt *nullstandardproc(ex)
Expr *ex;
{
    warning(format_s("Procedure %s not yet supported [118]", ((Meaning *)ex->val.i)->name));
    return makestmt_call(ex);
}

Meaning *makestandardproc(name, handler)
char *name;
Stmt *(*handler)();
{
    Meaning *mp;

    if (!handler)
        handler = nullstandardproc;
    mp = findstandardmeaning(MK_FUNCTION, name);
    if (mp) {
	mp->handler = (Expr *(*)())handler;
	if (mp->isfunction) {
	    warning(format_s("Procedure %s was declared as a function [119]", name));
	    mp->isfunction = 0;
	}
    } else if (debug > 0)
	warning(format_s("Procedure %s was never declared [120]", name));
    return mp;
}



Static Expr *nullspecialfunc(mp)
Meaning *mp;
{
    warning(format_s("Function %s not yet supported [121]", mp->name));
    if (curtok == TOK_LPAR)
        skipparens();
    return makeexpr_long(0);
}

Meaning *makespecialfunc(name, handler)
char *name;
Expr *(*handler)();
{
    Meaning *mp;

    if (!handler)
        handler = nullspecialfunc;
    mp = makestandardmeaning(MK_SPECIAL, name);
    mp->isfunction = 1;
    mp->handler = handler;
    return mp;
}



Static Expr *nullstandardfunc(ex)
Expr *ex;
{
    warning(format_s("Function %s not yet supported [121]", ((Meaning *)ex->val.i)->name));
    return ex;
}

Meaning *makestandardfunc(name, handler)
char *name;
Expr *(*handler)();
{
    Meaning *mp;

    if (!handler)
        handler = nullstandardfunc;
    mp = findstandardmeaning(MK_FUNCTION, name);
    if (mp) {
	mp->handler = handler;
	if (!mp->isfunction) {
	    warning(format_s("Function %s was declared as a procedure [122]", name));
	    mp->isfunction = 1;
	}
    } else if (debug > 0)
	warning(format_s("Function %s was never declared [123]", name));
    return mp;
}




Static Expr *nullspecialvar(mp)
Meaning *mp;
{
    warning(format_s("Variable %s not yet supported [124]", mp->name));
    if (curtok == TOK_LPAR || curtok == TOK_LBR)
        skipparens();
    return makeexpr_var(mp);
}

Meaning *makespecialvar(name, handler)
char *name;
Expr *(*handler)();
{
    Meaning *mp;

    if (!handler)
        handler = nullspecialvar;
    mp = makestandardmeaning(MK_SPVAR, name);
    mp->handler = handler;
    return mp;
}





void setup_decl()
{
    Meaning *mp, *mp2, *mp_turbo_shortint;
    Symbol *sym;
    Type *tp;
    int i;

    numimports = 0;
    firstimport = 0;
    permimports = NULL;
    stringceiling = stringceiling | 1;   /* round up to odd */
    stringtypecachesize = (stringceiling + 1) >> 1;
    stringtypecache = ALLOC(stringtypecachesize, Type *, misc);
    curctxlast = NULL;
    curctx = NULL;   /* the meta-ctx has no parent ctx */
    curctx = nullctx = makestandardmeaning(MK_MODULE, "SYSTEM");
    strlist_add(&permimports, "SYSTEM")->value = (long)nullctx;
    ptrbase = NULL;
    tempvars = NULL;
    stmttempvars = NULL;
    tempvarcount = 0;
    deferallptrs = 0;
    silentalreadydef = 0;
    varstructdecllist = NULL;
    nonloclabelcount = -1;
    for (i = 0; i < stringtypecachesize; i++)
        stringtypecache[i] = NULL;

    tp_integer = makestandardtype(TK_INTEGER, makestandardmeaning(MK_TYPE,
                     (integer16) ? "LONGINT" : "INTEGER"));
    tp_integer->smin = makeexpr_long(MININT);             /* "long" */
    tp_integer->smax = makeexpr_long(MAXINT);

    if (sizeof_int >= 32) {
        tp_int = tp_integer;                              /* "int" */
    } else {
        tp_int = makestandardtype(TK_INTEGER,
                     (integer16 > 1) ? makestandardmeaning(MK_TYPE, "INTEGER")
				     : NULL);
        tp_int->smin = makeexpr_long(min_sshort);
        tp_int->smax = makeexpr_long(max_sshort);
    }
    mp = makestandardmeaning(MK_TYPE, "C_INT");
    mp->type = tp_int;
    if (!tp_int->meaning)
	tp_int->meaning = mp;

    mp_unsigned = makestandardmeaning(MK_TYPE, "UNSIGNED");
    tp_unsigned = makestandardtype(TK_INTEGER, mp_unsigned);
    tp_unsigned->smin = makeexpr_long(0);                 /* "unsigned long" */
    tp_unsigned->smax = makeexpr_long(MAXINT);

    if (sizeof_int >= 32) {
        tp_uint = tp_unsigned;                            /* "unsigned int" */
	mp_uint = mp_unsigned;
    } else {
	mp_uint = makestandardmeaning(MK_TYPE, "C_UINT");
        tp_uint = makestandardtype(TK_INTEGER, mp_uint);
        tp_uint->smin = makeexpr_long(0);
        tp_uint->smax = makeexpr_long(MAXINT);
    }

    tp_sint = makestandardtype(TK_INTEGER, NULL);
    tp_sint->smin = copyexpr(tp_int->smin);               /* "signed int" */
    tp_sint->smax = copyexpr(tp_int->smax);

    tp_char = makestandardtype(TK_CHAR, makestandardmeaning(MK_TYPE, "CHAR"));
    if (unsignedchar == 0) {
	tp_char->smin = makeexpr_long(-128);              /* "char" */
	tp_char->smax = makeexpr_long(127);
    } else {
	tp_char->smin = makeexpr_long(0);
	tp_char->smax = makeexpr_long(255);
    }

    tp_charptr = makestandardtype(TK_POINTER, NULL);      /* "unsigned char *" */
    tp_charptr->basetype = tp_char;
    tp_char->pointertype = tp_charptr;

    mp_schar = makestandardmeaning(MK_TYPE, "SCHAR");     /* "signed char" */
    tp_schar = makestandardtype(TK_CHAR, mp_schar);
    tp_schar->smin = makeexpr_long(-128);
    tp_schar->smax = makeexpr_long(127);

    mp_uchar = makestandardmeaning(MK_TYPE, "UCHAR");     /* "unsigned char" */
    tp_uchar = makestandardtype(TK_CHAR, mp_uchar);
    tp_uchar->smin = makeexpr_long(0);
    tp_uchar->smax = makeexpr_long(255);

    tp_boolean = makestandardtype(TK_BOOLEAN, makestandardmeaning(MK_TYPE, "BOOLEAN"));
    tp_boolean->smin = makeexpr_long(0);                  /* "boolean" */
    tp_boolean->smax = makeexpr_long(1);

    sym = findsymbol("Boolean");
    sym->flags |= SSYNONYM;
    strlist_append(&sym->symbolnames, "===")->value = (long)tp_boolean->meaning->sym;

    tp_real = makestandardtype(TK_REAL, makestandardmeaning(MK_TYPE, "REAL"));
                                                          /* "float" or "double" */
    mp = makestandardmeaning(MK_TYPE, "LONGREAL");
    if (doublereals)
	mp->type = tp_longreal = tp_real;
    else
	tp_longreal = makestandardtype(TK_REAL, mp);

    tp_void = makestandardtype(TK_VOID, NULL);            /* "void" */

    mp = makestandardmeaning(MK_TYPE, "SINGLE");
    if (doublereals)
	makestandardtype(TK_REAL, mp);
    else
	mp->type = tp_real;
    makestandardmeaning(MK_TYPE, "SHORTREAL")->type = mp->type;
    mp = makestandardmeaning(MK_TYPE, "DOUBLE");
    mp->type = tp_longreal;
    mp = makestandardmeaning(MK_TYPE, "EXTENDED");
    mp->type = tp_longreal;   /* good enough */
    mp = makestandardmeaning(MK_TYPE, "QUADRUPLE");
    mp->type = tp_longreal;   /* good enough */

    tp_sshort = makestandardtype(TK_SUBR, makestandardmeaning(MK_TYPE,
                  (integer16 == 1) ? "INTEGER" : "SWORD"));
    tp_sshort->basetype = tp_integer;                     /* "short" */
    tp_sshort->smin = makeexpr_long(min_sshort);
    tp_sshort->smax = makeexpr_long(max_sshort);

    if (integer16) {
	if (integer16 != 2) {
	    mp = makestandardmeaning(MK_TYPE, "SWORD");
	    mp->type = tp_sshort;
	}
    } else {
	mp = makestandardmeaning(MK_TYPE, "LONGINT");
	mp->type = tp_integer;
    }

    tp_ushort = makestandardtype(TK_SUBR, makestandardmeaning(MK_TYPE, modula2 ? "UWORD" : "WORD"));
    tp_ushort->basetype = tp_integer;                     /* "unsigned short" */
    tp_ushort->smin = makeexpr_long(0);
    tp_ushort->smax = makeexpr_long(max_ushort);

    mp = makestandardmeaning(MK_TYPE, "CARDINAL");
    mp->type = (integer16) ? tp_ushort : tp_unsigned;
    mp = makestandardmeaning(MK_TYPE, "LONGCARD");
    mp->type = tp_unsigned;

    if (modula2) {
	mp = makestandardmeaning(MK_TYPE, "WORD");
	mp->type = tp_integer;
    } else {
	makestandardmeaning(MK_TYPE, "UWORD")->type = tp_ushort;
    }

    tp_sbyte = makestandardtype(TK_SUBR, NULL);           /* "signed char" */
    tp_sbyte->basetype = tp_integer;
    tp_sbyte->smin = makeexpr_long(min_schar);
    tp_sbyte->smax = makeexpr_long(max_schar);

    mp_turbo_shortint = (which_lang == LANG_TURBO) ? makestandardmeaning(MK_TYPE, "SHORTINT") : NULL;
    mp = makestandardmeaning(MK_TYPE, "SBYTE");
    if (needsignedbyte || signedchars == 1 || hassignedchar) {
	mp->type = tp_sbyte;
	if (mp_turbo_shortint)
	    mp_turbo_shortint->type = tp_sbyte;
	tp_sbyte->meaning = mp_turbo_shortint ? mp_turbo_shortint : mp;
    } else {
	mp->type = tp_sshort;
	if (mp_turbo_shortint)
	    mp_turbo_shortint->type = tp_sshort;
    }

    tp_ubyte = makestandardtype(TK_SUBR, makestandardmeaning(MK_TYPE, "BYTE"));
    tp_ubyte->basetype = tp_integer;                      /* "unsigned char" */
    tp_ubyte->smin = makeexpr_long(0);
    tp_ubyte->smax = makeexpr_long(max_uchar);

    if (signedchars == 1)
        tp_abyte = tp_sbyte;                              /* "char" */
    else if (signedchars == 0)
        tp_abyte = tp_ubyte;
    else {
        tp_abyte = makestandardtype(TK_SUBR, NULL);
        tp_abyte->basetype = tp_integer;
        tp_abyte->smin = makeexpr_long(0);
        tp_abyte->smax = makeexpr_long(max_schar);
    }

    mp = makestandardmeaning(MK_TYPE, "POINTER");
    mp2 = makestandardmeaning(MK_TYPE, "ANYPTR");
    tp_anyptr = makestandardtype(TK_POINTER, (which_lang == LANG_HP) ? mp2 : mp);
    ((which_lang == LANG_HP) ? mp : mp2)->type = tp_anyptr;
    tp_anyptr->basetype = tp_void;                        /* "void *" */
    tp_void->pointertype = tp_anyptr;

    if (useAnyptrMacros == 1) {
        tp_special_anyptr = makestandardtype(TK_SUBR, NULL);
        tp_special_anyptr->basetype = tp_integer;
        tp_special_anyptr->smin = makeexpr_long(0);
        tp_special_anyptr->smax = makeexpr_long(max_schar);
    } else
        tp_special_anyptr = NULL;

    tp_proc = maketype(TK_PROCPTR);
    tp_proc->basetype = maketype(TK_FUNCTION);
    tp_proc->basetype->basetype = tp_void;
    tp_proc->escale = 1;   /* saved "hasstaticlinks" */

    tp_str255 = makestandardtype(TK_STRING, NULL);             /* "Char []" */
    tp_str255->basetype = tp_char;
    tp_str255->indextype = makestandardtype(TK_SUBR, NULL);
    tp_str255->indextype->basetype = tp_integer;
    tp_str255->indextype->smin = makeexpr_long(0);
    tp_str255->indextype->smax = makeexpr_long(stringceiling);

    tp_strptr = makestandardtype(TK_POINTER, NULL);            /* "Char *" */
    tp_str255->pointertype = tp_strptr;
    tp_strptr->basetype = tp_str255;

    mp_string = makestandardmeaning(MK_TYPE, "STRING");
    tp = makestandardtype(TK_STRING, mp_string);
    tp->basetype = tp_char;
    tp->indextype = tp_str255->indextype;

    tp_smallset = maketype(TK_SMALLSET);
    tp_smallset->basetype = tp_integer;
    tp_smallset->indextype = tp_boolean;

    tp_text = makestandardtype(TK_POINTER, makestandardmeaning(MK_TYPE, "TEXT"));
    tp_text->basetype = makestandardtype(TK_FILE, NULL);       /* "FILE *" */
    tp_text->basetype->basetype = tp_char;
    tp_text->basetype->pointertype = tp_text;

    tp_bigtext = makestandardtype(TK_BIGFILE, makestandardmeaning(MK_TYPE, "BIGTEXT"));
    tp_bigtext->basetype = tp_char;
    tp_bigtext->meaning->name = stralloc("_TEXT");
    tp_bigtext->meaning->wasdeclared = 1;

    tp_jmp_buf = makestandardtype(TK_SPECIAL, NULL);

    mp = makestandardmeaning(MK_TYPE, "INTERACTIVE");
    mp->type = tp_text;

    mp = makestandardmeaning(MK_TYPE, "BITSET");
    mp->type = makesettype(makesubrangetype(tp_integer, makeexpr_long(0),
					    makeexpr_long(setbits-1)));
    mp->type->meaning = mp;

    mp = makestandardmeaning(MK_TYPE, "INTSET");
    mp->type = makesettype(makesubrangetype(tp_integer, makeexpr_long(0),
					    makeexpr_long(defaultsetsize-1)));
    mp->type->meaning = mp;

    mp_input = makestandardmeaning(MK_VAR, "INPUT");
    mp_input->type = tp_text;
    mp_input->name = stralloc("stdin");
    ex_input = makeexpr_var(mp_input);

    mp_output = makestandardmeaning(MK_VAR, "OUTPUT");
    mp_output->type = tp_text;
    mp_output->name = stralloc("stdout");
    ex_output = makeexpr_var(mp_output);

    mp_stderr = makestandardmeaning(MK_VAR, "STDERR");
    mp_stderr->type = tp_text;
    mp_stderr->name = stralloc("stderr");

    mp_escapecode = makestandardmeaning(MK_VAR, "ESCAPECODE");
    mp_escapecode->type = tp_sshort;
    mp_escapecode->name = stralloc(name_ESCAPECODE);

    mp_ioresult = makestandardmeaning(MK_VAR, "IORESULT");
    mp_ioresult->type = tp_integer;
    mp_ioresult->name = stralloc(name_IORESULT);

    mp_false = makestandardmeaning(MK_CONST, "FALSE");
    mp_false->type = mp_false->val.type = tp_boolean;
    mp_false->val.i = 0;

    mp_true = makestandardmeaning(MK_CONST, "TRUE");
    mp_true->type = mp_true->val.type = tp_boolean;
    mp_true->val.i = 1;

    mp_maxint = makestandardmeaning(MK_CONST, "MAXINT");
    mp_maxint->type = mp_maxint->val.type = tp_integer;
    mp_maxint->val.i = MAXINT;
    mp_maxint->name = stralloc((integer16) ? "SHORT_MAX" :
                               (sizeof_int >= 32) ? "INT_MAX" : "LONG_MAX");

    mp = makestandardmeaning(MK_CONST, "MAXLONGINT");
    mp->type = mp->val.type = tp_integer;
    mp->val.i = MAXINT;
    mp->name = stralloc("LONG_MAX");

    mp_minint = makestandardmeaning(MK_CONST, "MININT");
    mp_minint->type = mp_minint->val.type = tp_integer;
    mp_minint->val.i = MININT;
    mp_minint->name = stralloc((integer16) ? "SHORT_MIN" :
                               (sizeof_int >= 32) ? "INT_MIN" : "LONG_MIN");

    mp = makestandardmeaning(MK_CONST, "MAXCHAR");
    mp->type = mp->val.type = tp_char;
    mp->val.i = 127;
    mp->name = stralloc("CHAR_MAX");

    mp = makestandardmeaning(MK_CONST, "MINCHAR");
    mp->type = mp->val.type = tp_char;
    mp->val.i = 0;
    mp->anyvarflag = 1;

    mp = makestandardmeaning(MK_CONST, "BELL");
    mp->type = mp->val.type = tp_char;
    mp->val.i = 7;
    mp->anyvarflag = 1;

    mp = makestandardmeaning(MK_CONST, "TAB");
    mp->type = mp->val.type = tp_char;
    mp->val.i = 9;
    mp->anyvarflag = 1;

    mp_str_hp = mp_str_turbo = NULL;
    mp_val_modula = mp_val_turbo = NULL;
    mp_blockread_ucsd = mp_blockread_turbo = NULL;
    mp_blockwrite_ucsd = mp_blockwrite_turbo = NULL;
    mp_dec_dec = mp_dec_turbo = NULL;
}



/* This makes sure that if A imports B and then C, C's interface is not
   parsed in the environment of B */
int push_imports()
{
    int mark = firstimport;
    Meaning *mp;

    while (firstimport < numimports) {
	if (!strlist_cifind(permimports, importlist[firstimport]->sym->name)) {
	    for (mp = importlist[firstimport]->cbase; mp; mp = mp->cnext)
		mp->isactive = 0;
	}
        firstimport++;
    }
    return mark;
}



void pop_imports(mark)
int mark;
{
    Meaning *mp;

    while (firstimport > mark) {
        firstimport--;
        for (mp = importlist[firstimport]->cbase; mp; mp = mp->cnext)
            mp->isactive = 1;
    }
}



void import_ctx(ctx)
Meaning *ctx;
{
    Meaning *mp;
    int i;

    for (i = firstimport; i < numimports && importlist[i] != ctx; i++) ;
    if (i >= numimports) {
        if (numimports == MAXIMPORTS)
            error(format_d("Maximum of %d simultaneous imports exceeded", MAXIMPORTS));
        importlist[numimports++] = ctx;
    }
    for (mp = ctx->cbase; mp; mp = mp->cnext) {
        if (mp->exported)
            mp->isactive = 1;
    }
}



void perm_import(ctx)
Meaning *ctx;
{
    Meaning *mp;

    /* Import permanently, as in Turbo's "system" unit */
    for (mp = ctx->cbase; mp; mp = mp->cnext) {
        if (mp->exported)
            mp->isactive = 1;
    }
}



void unimport(mark)
int mark;
{
    Meaning *mp;

    while (numimports > mark) {
        numimports--;
	if (!strlist_cifind(permimports, importlist[numimports]->sym->name)) {
	    for (mp = importlist[numimports]->cbase; mp; mp = mp->cnext)
		mp->isactive = 0;
	}
    }
}




void activatemeaning(mp)
Meaning *mp;
{
    Meaning *mp2;

    if (debug>1) fprintf(outf, "Reviving %s\n", curctxlast->name);
    mp->isactive = 1;
    if (mp->sym->mbase != mp) {     /* move to front of symbol list */
        mp2 = mp->sym->mbase;
        for (;;) {
            if (!mp2) {
		/* Not on symbol list: must be a special kludge meaning */
                return;
            }
            if (mp2->snext == mp)
                break;
            mp2 = mp2->snext;
        }
        mp2->snext = mp->snext;
        mp->snext = mp->sym->mbase;
        mp->sym->mbase = mp;
    }
}



void pushctx(ctx)
Meaning *ctx;
{
    struct ctxstack *top;

    top = ALLOC(1, struct ctxstack, ctxstacks);
    top->ctx = curctx;
    top->ctxlast = curctxlast;
    top->tempvars = tempvars;
    top->tempvarcount = tempvarcount;
    top->importmark = numimports;
    top->next = ctxtop;
    ctxtop = top;
    curctx = ctx;
    curctxlast = ctx->cbase;
    if (curctxlast) {
        activatemeaning(curctxlast);
        while (curctxlast->cnext) {
            curctxlast = curctxlast->cnext;
            activatemeaning(curctxlast);
        }
    }
    tempvars = NULL;
    tempvarcount = 0;
    if (blockkind != TOK_IMPORT && blockkind != TOK_EXPORT)
	progress();
}



void popctx()
{
    struct ctxstack *top;
    struct tempvarlist *tv;
    Meaning *mp;

    if (!strlist_cifind(permimports, curctx->sym->name)) {
	for (mp = curctx->cbase; mp; mp = mp->cnext) {
	    if (debug>1) fprintf(outf, "Hiding %s\n", mp->name);
	    mp->isactive = 0;
	}
    }
    top = ctxtop;
    ctxtop = top->next;
    curctx = top->ctx;
    curctxlast = top->ctxlast;
    while (tempvars) {
        tv = tempvars->next;
        FREE(tempvars);
        tempvars = tv;
    }
    tempvars = top->tempvars;
    tempvarcount = top->tempvarcount;
    unimport(top->importmark);
    FREE(top);
    if (blockkind != TOK_IMPORT && blockkind != TOK_EXPORT)
	progress();
}



void forget_ctx(ctx, all)
Meaning *ctx;
int all;
{
    register Meaning *mp, **mpprev, *mp2, **mpp2;

    if (ctx->kind == MK_FUNCTION && ctx->isfunction && ctx->cbase)
	mpprev = &ctx->cbase->cnext;   /* Skip return-value variable */
    else
	mpprev = &ctx->cbase;
    while ((mp = *mpprev) != NULL) {
	if (all ||
	    (mp->kind != MK_PARAM &&
	     mp->kind != MK_VARPARAM)) {
	    *mpprev = mp->cnext;
	    mpp2 = &mp->sym->mbase;
	    while ((mp2 = *mpp2) != NULL && mp2 != mp)
		mpp2 = &mp2->snext;
	    if (mp2)
		*mpp2 = mp2->snext;
	    if (mp->kind == MK_CONST)
		free_value(&mp->val);
	    freeexpr(mp->constdefn);
	    if (mp->cbase)
		forget_ctx(mp, 1);
	    if (mp->kind == MK_FUNCTION && mp->val.i)
		free_stmt((Stmt *)mp->val.i);
	    strlist_empty(&mp->comments);
	    if (mp->name)
		FREE(mp->name);
	    if (mp->othername)
		FREE(mp->othername);
	    FREE(mp);
	} else
	    mpprev = &mp->cnext;
    }
}




void handle_nameof()
{
    Strlist *sl, *sl2;
    Symbol *sp;
    char *cp;

    for (sl = nameoflist; sl; sl = sl->next) {
        cp = my_strchr(sl->s, '.');
        if (cp) {
            sp = findsymbol(fixpascalname(cp + 1));
            sl2 = strlist_add(&sp->symbolnames, 
                              format_ds("%.*s", (int)(cp - sl->s), sl->s));
        } else {
            sp = findsymbol(fixpascalname(sl->s));
            sl2 = strlist_add(&sp->symbolnames, "");
        }
        sl2->value = sl->value;
        if (debug > 0)
            fprintf(outf, "symbol %s gets \"%s\" -> \"%s\"\n",
                          sp->name, sl2->s, sl2->value);
    }
    strlist_empty(&nameoflist);
}



Static void initmeaning(mp)
Meaning *mp;
{
/*    mp->serial = curserial = ++serialcount;    */
    mp->cbase = NULL;
    mp->xnext = NULL;
    mp->othername = NULL;
    mp->type = NULL;
    mp->dtype = NULL;
    mp->needvarstruct = 0;
    mp->varstructflag = 0;
    mp->wasdeclared = 0;
    mp->isforward = 0;
    mp->isfunction = 0;
    mp->istemporary = 0;
    mp->volatilequal = 0;
    mp->constqual = 0;
    mp->warnifused = (warnnames > 0);
    mp->constdefn = NULL;
    mp->val.i = 0;
    mp->val.s = NULL;
    mp->val.type = NULL;
    mp->refcount = 1;
    mp->anyvarflag = 0;
    mp->isactive = 1;
    mp->exported = 0;
    mp->handler = NULL;
    mp->dumped = 0;
    mp->isreturn = 0;
    mp->fakeparam = 0;
    mp->namedfile = 0;
    mp->bufferedfile = 0;
    mp->comments = NULL;
}



int issafename(sp, isglobal, isdefine)
Symbol *sp;
int isglobal, isdefine;
{
    if (isdefine && curctx->kind != MK_FUNCTION) {
	if (sp->flags & FWDPARAM)
	    return 0;
    }
    if ((sp->flags & AVOIDNAME) ||
	(isdefine && (sp->flags & AVOIDFIELD)) ||
        (isglobal && (sp->flags & AVOIDGLOB)))
        return 0;
    else
        return 1;
}



static Meaning *enum_tname;

void setupmeaning(mp, sym, kind, namekind)
Meaning *mp;
Symbol *sym;
enum meaningkind kind, namekind;
{
    char *name, *symfmt, *editfmt, *cp, *cp2;
    int altnum, isglobal, isdefine;
    Symbol *sym2;
    Strlist *sl;

    if (!sym)
	sym = findsymbol("Spam");   /* reduce crashes due to internal errors */
    if (sym->mbase && sym->mbase->ctx == curctx &&
	curctx != NULL && !silentalreadydef)
        alreadydef(sym);
    mp->sym = sym;
    mp->snext = sym->mbase;
    sym->mbase = mp;
    if (sym == curtoksym) {
	sym->kwtok = TOK_NONE;
	sym->flags &= ~KWPOSS;
    }
    mp->ctx = curctx;
    mp->kind = kind;
    if (pascalcasesens && curctx && curctx->sym && kind != MK_SYNONYM &&
	strlist_cifind(permimports, curctx->sym->name)) { /* a built-in name */
	Meaning *mp2;
	if (islower(sym->name[0]))
	    sym2 = findsymbol(strupper(sym->name));
	else
	    sym2 = findsymbol(strlower(sym->name));
	mp2 = addmeaning(sym2, MK_SYNONYM);
	mp2->xnext = mp;
    }
    if (kind == MK_VAR) {
        sl = strlist_find(varmacros, sym->name);
        if (sl) {
            kind = namekind = MK_VARMAC;
            mp->constdefn = (Expr *)sl->value;
            strlist_delete(&varmacros, sl);
        }
    }
    if (kind == MK_FUNCTION || kind == MK_SPECIAL) {
        sl = strlist_find(funcmacros, sym->name);
        if (sl) {
            mp->constdefn = (Expr *)sl->value;
            strlist_delete(&funcmacros, sl);
        }
    }
    if (kind == MK_VAR || kind == MK_VARREF || kind == MK_VARMAC ||
	kind == MK_TYPE || kind == MK_CONST || kind == MK_FUNCTION) {
        mp->exported = (blockkind == TOK_IMPORT || blockkind == TOK_EXPORT);
	if (blockkind == TOK_IMPORT)
	    mp->wasdeclared = 1;   /* suppress future declaration */
    } else
        mp->exported = 0;
    if (sym == curtoksym)
        name = curtokcase;
    else
        name = sym->name;
    isdefine = (namekind == MK_CONST || (namekind == MK_VARIANT && !useenum));
    isglobal = (!curctx ||
		curctx->kind != MK_FUNCTION ||
                namekind == MK_FUNCTION ||
		namekind == MK_TYPE ||
		namekind == MK_VARIANT ||
                isdefine) &&
               (curctx != nullctx);
    mp->refcount = isglobal ? 1 : 0;   /* make sure globals don't disappear */
    if (namekind == MK_SYNONYM)
	return;
    if (!mp->exported || !*exportsymbol)
        symfmt = "";
    else if (*export_symbol && my_strchr(name, '_'))
        symfmt = export_symbol;
    else
        symfmt = exportsymbol;
    wasaliased = 0;
    if (*externalias && !my_strchr(externalias, '%')) {
        register int i;
        name = format_s("%s", externalias);
        i = numparams;
        while (--i >= 0 && strcmp(rctable[i].name, "ALIAS")) ;
        if (i < 0 || !undooption(i, ""))
            *externalias = 0;
        wasaliased = 1;
    } else if (sym->symbolnames) {
        if (curctx) {
            if (debug > 2)
                fprintf(outf, "checking for \"%s\" of %s\n", curctx->name, sym->name);
            sl = strlist_cifind(sym->symbolnames, curctx->sym->name);
            if (sl) {
                if (debug > 2)
                    fprintf(outf, "found \"%s\"\n", sl->value);
                name = (char *)sl->value;
                wasaliased = 1;
            }
        }
        if (!wasaliased) {
            if (debug > 2)
                fprintf(outf, "checking for \"\" of %s\n", sym->name);
            sl = strlist_find(sym->symbolnames, "");
            if (sl) {
                if (debug > 2)
                    fprintf(outf, "found \"%s\"\n", sl->value);
                name = (char *)sl->value;
                wasaliased = 1;
            }
        }
    }
    if (!*symfmt || wasaliased)
	symfmt = "%s";
    altnum = -1;
    do {
        altnum++;
        cp = format_ss(symfmt, name, curctx ? curctx->name : "");
	switch (namekind) {

	  case MK_CONST:
	    editfmt = constformat;
	    break;

	  case MK_MODULE:
	    editfmt = moduleformat;
	    break;

	  case MK_FUNCTION:
	    editfmt = functionformat;
	    break;

	  case MK_VAR:
	  case MK_VARPARAM:
	  case MK_VARREF:
	  case MK_VARMAC:
	  case MK_SPVAR:
	    editfmt = varformat;
	    break;

	  case MK_TYPE:
	    editfmt = typeformat;
	    break;

	  case MK_VARIANT:   /* A true kludge! */
	    editfmt = enumformat;
	    if (!*editfmt)
		editfmt = useenum ? varformat : constformat;
	    break;

	  default:
	    editfmt = "";
	}
	if (!*editfmt)
	    editfmt = symbolformat;
	if (*editfmt)
	    if (editfmt == enumformat)
		cp = format_ss(editfmt, cp,
			       enum_tname ? enum_tname->name : "ENUM");
	    else
		cp = format_ss(editfmt, cp,
			       curctx ? curctx->name : "");
	if (dollar_idents == 2) {
	    for (cp2 = cp; *cp2; cp2++)
		if (*cp2 == '$' || *cp2 == '%')
		    *cp2 = '_';
	}
        sym2 = findsymbol(findaltname(cp, altnum));
    } while (!issafename(sym2, isglobal, isdefine) &&
	     namekind != MK_MODULE && !wasaliased);
    mp->name = stralloc(sym2->name);
    if (sym2->flags & WARNNAME)
        note(format_s("A symbol named %s was defined [100]", mp->name));
    if (isglobal) {
        switch (namekind) {     /* prevent further name conflicts */

            case MK_CONST:
	    case MK_VARIANT:
            case MK_TYPE:
                sym2->flags |= AVOIDNAME;
                break;

            case MK_VAR:
            case MK_VARREF:
            case MK_FUNCTION:
                sym2->flags |= AVOIDGLOB;
                break;

	    default:
		/* name is completely local */
		break;
        }
    }
    if (debug > 4)
	fprintf(outf, "Created meaning %s\n", mp->name);
}



Meaning *addmeaningas(sym, kind, namekind)
Symbol *sym;
enum meaningkind kind, namekind;
{
    Meaning *mp;

    mp = ALLOC(1, Meaning, meanings);
    initmeaning(mp);
    setupmeaning(mp, sym, kind, namekind);
    mp->cnext = NULL;
    if (curctx) {
        if (curctxlast)
            curctxlast->cnext = mp;
        else
            curctx->cbase = mp;
        curctxlast = mp;
    }
    return mp;
}



Meaning *addmeaning(sym, kind)
Symbol *sym;
enum meaningkind kind;
{
    return addmeaningas(sym, kind, kind);
}



Meaning *addmeaningafter(mpprev, sym, kind)
Meaning *mpprev;
Symbol *sym;
enum meaningkind kind;
{
    Meaning *mp;

    if (!mpprev->cnext && mpprev->ctx == curctx)
        return addmeaning(sym, kind);
    mp = ALLOC(1, Meaning, meanings);
    initmeaning(mp);
    setupmeaning(mp, sym, kind, kind);
    mp->ctx = mpprev->ctx;
    mp->cnext = mpprev->cnext;
    mpprev->cnext = mp;
    return mp;
}


void unaddmeaning(mp)
Meaning *mp;
{
    Meaning *prev;

    prev = mp->ctx;
    while (prev && prev != mp)
	prev = prev->cnext;
    if (prev)
	prev->cnext = mp->cnext;
    else
	mp->ctx = mp->cnext;
    if (!mp->cnext && mp->ctx == curctx)
	curctxlast = prev;
}


void readdmeaning(mp)
Meaning *mp;
{
    mp->cnext = NULL;
    if (curctx) {
        if (curctxlast)
            curctxlast->cnext = mp;
        else
            curctx->cbase = mp;
        curctxlast = mp;
    }
}


Meaning *addfield(sym, flast, rectype, tname)
Symbol *sym;
Meaning ***flast;
Type *rectype;
Meaning *tname;
{
    Meaning *mp;
    int altnum;
    Symbol *sym2;
    Strlist *sl;
    char *name, *name2;

    mp = ALLOC(1, Meaning, meanings);
    initmeaning(mp);
    mp->sym = sym;
    if (sym) {
        mp->snext = sym->fbase;
        sym->fbase = mp;
        if (sym == curtoksym)
            name2 = curtokcase;
        else
            name2 = sym->name;
	name = name2;
        if (tname)
            sl = strlist_find(fieldmacros,
                              format_ss("%s.%s", tname->sym->name, sym->name));
        else
            sl = NULL;
        if (sl) {
            mp->constdefn = (Expr *)sl->value;
            strlist_delete(&fieldmacros, sl);
            altnum = 0;
        } else {
            altnum = -1;
            do {
                altnum++;
		if (*fieldformat)
		    name = format_ss(fieldformat, name2,
				     tname && tname->name ? tname->name
				                          : "FIELD");
                sym2 = findsymbol(findaltname(name, altnum));
            } while (!issafename(sym2, 0, 0) ||
		     ((sym2->flags & AVOIDFIELD) && !reusefieldnames));
	    sym2->flags |= AVOIDFIELD;
        }
        mp->kind = MK_FIELD;
        mp->name = stralloc(findaltname(name, altnum));
    } else {
        mp->name = stralloc("(variant)");
        mp->kind = MK_VARIANT;
    }
    mp->cnext = NULL;
    **flast = mp;
    *flast = &(mp->cnext);
    mp->ctx = NULL;
    mp->rectype = rectype;
    mp->val.i = 0;
    return mp;
}





int isfiletype(type, big)
Type *type;
int big;   /* 0=TK_FILE, 1=TK_BIGFILE, -1=either */
{
    return ((type->kind == TK_POINTER &&
	     type->basetype->kind == TK_FILE && big != 1) ||
	    (type->kind == TK_BIGFILE && big != 0));
}


Meaning *isfilevar(ex)
Expr *ex;
{
    Meaning *mp;

    if (ex->kind == EK_VAR) {
	mp = (Meaning *)ex->val.i;
	if (mp->kind == MK_VAR)
	    return mp;
    } else if (ex->kind == EK_DOT) {
	mp = (Meaning *)ex->val.i;
	if (mp && mp->kind == MK_FIELD)
	    return mp;
    }
    return NULL;
}


Type *filebasetype(type)
Type *type;
{
    if (type->kind == TK_BIGFILE)
	return type->basetype;
    else
	return type->basetype->basetype;
}


Expr *filebasename(ex)
Expr *ex;
{
    if (ex->val.type->kind == TK_BIGFILE)
	return makeexpr_dotq(ex, "f", ex->val.type);
    else
	return ex;
}


Expr *filenamepart(ex)
Expr *ex;
{
    Meaning *mp;

    if (ex->val.type->kind == TK_BIGFILE)
	return makeexpr_dotq(copyexpr(ex), "name", tp_str255);
    else if ((mp = isfilevar(ex)) && mp->namedfile)
	return makeexpr_name(format_s(name_FNVAR, mp->name), tp_str255);
    else
	return NULL;
}


int fileisbuffered(ex, maybe)
Expr *ex;
int maybe;
{
    Meaning *mp;

    return (ex->val.type->kind == TK_BIGFILE ||
	    ((mp = isfilevar(ex)) && (maybe || mp->bufferedfile)));
}



Type *findbasetype_(type, flags)
Type *type;
int flags;
{
    long smin, smax;
    static Type typename;

    for (;;) {
	if (type->preserved && (type->kind != TK_POINTER) &&
	    !(flags & ODECL_NOPRES))
	    return type;
        switch (type->kind) {

            case TK_POINTER:
	        if (type->smin)    /* unresolved forward */
		    return type;
                if (type->basetype == tp_void) {     /* ANYPTR */
                    if (tp_special_anyptr)
                        return tp_special_anyptr;   /* write "Anyptr" */
                    if (!voidstar)
                        return tp_abyte;    /* write "char *", not "void *" */
                }
                switch (type->basetype->kind) {

                    case TK_ARRAY:       /* use basetype's basetype: */
                    case TK_STRING:      /* ^array[5] of array[3] of integer */
                    case TK_SET:         /*  => int (*a)[3]; */
		        if (stararrays == 1 ||
			    !(flags & ODECL_FREEARRAY) ||
			    type->basetype->structdefd) {
			    type = type->basetype->basetype;
			    flags &= ~ODECL_CHARSTAR;
			    continue;
			}
                        break;

		    default:
			break;
                }
		if (type->preserved && !(flags & ODECL_NOPRES))
		    return type;
		if (type->fbase && type->fbase->wasdeclared &&
		    (flags & ODECL_DECL)) {
		    typename.meaning = type->fbase;
		    typename.preserved = 1;
		    return &typename;
		}
                break;

            case TK_FUNCTION:
            case TK_STRING:
            case TK_SET:
            case TK_SMALLSET:
            case TK_SMALLARRAY:
                if (!type->basetype)
                    return type;
                break;

            case TK_ARRAY:
                if (type->meaning && type->meaning->kind == MK_TYPE &&
                    type->meaning->wasdeclared)
                    return type;
		if (type->fbase && type->fbase->wasdeclared &&
		    (flags & ODECL_DECL)) {
		    typename.meaning = type->fbase;
		    typename.preserved = 1;
		    return &typename;
		}
                break;

            case TK_FILE:
                return tp_text->basetype;

            case TK_PROCPTR:
		return tp_proc;

	    case TK_CPROCPTR:
		type = type->basetype->basetype;
		continue;

            case TK_ENUM:
                if (useenum)
                    return type;
                else if (!enumbyte ||
			 type->smax->kind != EK_CONST ||
			 type->smax->val.i > 255)
		    return tp_sshort;
		else if (type->smax->val.i > 127)
                    return tp_ubyte;
		else
                    return tp_abyte;

            case TK_BOOLEAN:
                if (*name_BOOLEAN)
                    return type;
                else
                    return tp_ubyte;

            case TK_SUBR:
                if (type == tp_abyte || type == tp_ubyte || type == tp_sbyte ||
                    type == tp_ushort || type == tp_sshort) {
                    return type;
                } else if ((type->basetype->kind == TK_ENUM && useenum) ||
                           type->basetype->kind == TK_BOOLEAN && *name_BOOLEAN) {
                    return type->basetype;
                } else {
                    if (ord_range(type, &smin, &smax)) {
                        if (squeezesubr != 0) {
                            if (smin >= 0 && smax <= max_schar)
                                return tp_abyte;
                            else if (smin >= 0 && smax <= max_uchar)
                                return tp_ubyte;
                            else if (smin >= min_schar && smax <= max_schar &&
				     (signedchars == 1 || hassignedchar))
                                return tp_sbyte;
                            else if (smin >= min_sshort && smax <= max_sshort)
                                return tp_sshort;
                            else if (smin >= 0 && smax <= max_ushort)
                                return tp_ushort;
                            else
                                return tp_integer;
                        } else {
                            if (smin >= min_sshort && smax <= max_sshort)
                                return tp_sshort;
                            else
                                return tp_integer;
                        }
                    } else
                        return tp_integer;
                }

	    case TK_CHAR:
		if (type == tp_schar &&
		    (signedchars != 1 && !hassignedchar)) {
		    return tp_sshort;
		}
		return type;

            default:
                return type;
        }
        type = type->basetype;
    }
}


Type *findbasetype(type, flags)
Type *type;
int flags;
{
    if (debug>1) {
	fprintf(outf, "findbasetype(");
	dumptypename(type, 1);
	fprintf(outf, ",%d) = ", flags);
	type = findbasetype_(type, flags);
	dumptypename(type, 1);
	fprintf(outf, "\n");
	return type;
    }
    return findbasetype_(type, flags);
}



Expr *arraysize(tp, incskipped)
Type *tp;
int incskipped;
{
    Expr *ex, *minv, *maxv;
    int denom;

    ord_range_expr(tp->indextype, &minv, &maxv);
    if (maxv->kind == EK_VAR && maxv->val.i == (long)mp_maxint &&
	!exprdependsvar(minv, mp_maxint)) {
        return NULL;
    } else {
        ex = makeexpr_plus(makeexpr_minus(copyexpr(maxv),
                                          copyexpr(minv)),
                           makeexpr_long(1));
        if (tp->smin && !incskipped) {
            ex = makeexpr_minus(ex, copyexpr(tp->smin));
        }
        if (tp->smax) {
            denom = (tp->basetype == tp_sshort) ? 16 : 8;
            denom >>= tp->escale;
            ex = makeexpr_div(makeexpr_plus(ex, makeexpr_long(denom-1)),
                              makeexpr_long(denom));
        }
        return ex;
    }
}



Type *promote_type(tp)
Type *tp;
{
    Type *tp2;

    if (tp->kind == TK_ENUM) {
	if (promote_enums == 0 ||
	    (promote_enums < 0 &&
	     (useenum)))
	    return tp;
    }
    if (tp->kind == TK_ENUM ||
         tp->kind == TK_SUBR ||
         tp->kind == TK_INTEGER ||
         tp->kind == TK_CHAR ||
         tp->kind == TK_BOOLEAN) {
        tp2 = findbasetype(tp, ODECL_NOPRES);
	if (tp2 == tp_ushort && sizeof_int == 16)
	    return tp_uint;
        else if (tp2 == tp_sbyte || tp2 == tp_ubyte ||
		 tp2 == tp_abyte || tp2 == tp_char ||
		 tp2 == tp_sshort || tp2 == tp_ushort ||
		 tp2 == tp_boolean || tp2->kind == TK_ENUM) {
            return tp_int;
        }
    }
    if (tp == tp_real)
	return tp_longreal;
    return tp;
}


Type *promote_type_bin(t1, t2)
Type *t1, *t2;
{
    t1 = promote_type(t1);
    t2 = promote_type(t2);
    if (t1 == tp_longreal || t2 == tp_longreal)
	return tp_longreal;
    if (t1 == tp_unsigned || t2 == tp_unsigned)
	return tp_unsigned;
    if (t1 == tp_integer || t2 == tp_integer) {
	if ((t1 == tp_uint || t2 == tp_uint) &&
	    sizeof_int > 0 &&
	    sizeof_int < (sizeof_long > 0 ? sizeof_long : 32))
	    return tp_uint;
	return tp_integer;
    }
    if (t1 == tp_uint || t2 == tp_uint)
	return tp_uint;
    return t1;
}



#if 0
void predeclare_varstruct(mp)
Meaning *mp;
{
    if (mp->ctx &&
	 mp->ctx->kind == MK_FUNCTION &&
	 mp->ctx->varstructflag &&
	 (usePPMacros != 0 || prototypes != 0) &&
	 !strlist_find(varstructdecllist, mp->ctx->name)) {
	output("struct ");
	output(format_s(name_LOC, mp->ctx->name));
	output(" ;\n");
	strlist_insert(&varstructdecllist, mp->ctx->name);
    }
}
#endif


Static void declare_args(type, isheader, isforward)
Type *type;
int isheader, isforward;
{
    Meaning *mp = type->fbase;
    Type *tp;
    int firstflag = 0;
    int usePP, dopromote, proto, showtypes, shownames;
    int staticlink;
    char *name;

#if 1   /* This seems to work better! */
    isforward = !isheader;
#endif
    usePP = (isforward && usePPMacros != 0);
    dopromote = (promoteargs == 1 ||
		 (promoteargs < 0 && (usePP || !fullprototyping)));
    if (ansiC == 1 && blockkind != TOK_EXPORT)
	usePP = 0;
    if (usePP)
        proto = (prototypes) ? prototypes : 1;
    else
        proto = (isforward || fullprototyping) ? prototypes : 0;
    showtypes = (proto > 0);
    shownames = (proto == 1 || isheader);
    staticlink = (type->issigned ||
                  (type->meaning &&
                   type->meaning->ctx->kind == MK_FUNCTION &&
                   type->meaning->ctx->varstructflag));
    if (mp || staticlink) {
        if (usePP)
            output(" PP(");
	else if (spacefuncs)
	    output(" ");
        output("(");
        if (showtypes || shownames) {
            firstflag = 0;
            while (mp) {
                if (firstflag++)
		    if (spacecommas)
			output(",\002 ");
		    else
			output(",\002");
                name = (mp->othername && isheader) ? mp->othername : mp->name;
                tp = (mp->othername) ? mp->rectype : mp->type;
                if (!showtypes) {
                    output(name);
                } else {
		    output(storageclassname(varstorageclass(mp)));
		    if (!shownames || (isforward && *name == '_')) {
			out_type(tp, 1);
		    } else {
			if (dopromote)
			    tp = promote_type(tp);
			outbasetype(tp, ODECL_CHARSTAR|ODECL_FREEARRAY);
			output(" ");
			outdeclarator(tp, name,
				      ODECL_CHARSTAR|ODECL_FREEARRAY);
		    }
		}
                if (isheader)
                    mp->wasdeclared = showtypes;
                if (mp->type == tp_strptr && mp->anyvarflag) {     /* VAR STRING parameter */
		    if (spacecommas)
			output(",\002 ");
		    else
			output(",\002");
                    if (showtypes) {
			if (useAnyptrMacros == 1 || useconsts == 2)
			    output("Const ");
			else if (ansiC > 0)
			    output("const ");
                        output("int");
		    }
                    if (shownames) {
                        if (showtypes)
                            output(" ");
                        output(format_s(name_STRMAX, mp->name));
                    }
                }
                mp = mp->xnext;
            }
            if (staticlink) {     /* sub-procedure with static link */
                if (firstflag++)
		    if (spacecommas)
			output(",\002 ");
		    else
			output(",\002");
                if (type->issigned) {
                    if (showtypes)
			if (tp_special_anyptr)
			    output("Anyptr ");
			else if (voidstar)
			    output("void *");
			else
			    output("char *");
                    if (shownames)
                        output("_link");
                } else {
                    mp = type->meaning->ctx;
                    if (showtypes) {
                        output("struct ");
                        output(format_s(name_LOC, mp->name));
                        output(" *");
                    }
                    if (shownames) {
                        output(format_s(name_LINK, mp->name));
                    }
                }
            }
        }
        output(")");
        if (usePP)
            output(")");
    } else {
        if (usePP)
            output(" PV()");
        else {
	    if (spacefuncs)
		output(" ");
	    if (void_args)
		output("(void)");
	    else
		output("()");
	}
    }
}



void outdeclarator(type, name, flags)
Type *type;
char *name;
int flags;
{
    int i, depth, anyptrs, anyarrays;
    Expr *dimen[30];
    Expr *ex, *maxv;
    Type *tp, *functype, *basetype;
    Expr funcdummy;   /* yow */

    anyptrs = 0;
    anyarrays = 0;
    functype = NULL;
    basetype = findbasetype(type, flags);
    for (depth = 0, tp = type; tp && tp != basetype; tp = tp->basetype) {
        switch (tp->kind) {

            case TK_POINTER:
                if (tp->basetype) {
                    switch (tp->basetype->kind) {

		        case TK_VOID:
			    if (tp->basetype == tp_void &&
				tp_special_anyptr) {
				tp = tp_special_anyptr;
				continue;
			    }
			    break;

                        case TK_ARRAY:    /* ptr to array of x => ptr to x */
                        case TK_STRING:   /*                or => array of x */
                        case TK_SET:
			    if (stararrays == 1 ||
				!(flags & ODECL_FREEARRAY) ||
				(tp->basetype->structdefd &&
				 stararrays != 2)) {
				tp = tp->basetype;
				flags &= ~ODECL_CHARSTAR;
			    } else {
				continue;
			    }
                            break;

			default:
			    break;
                    }
                }
                dimen[depth++] = NULL;
                anyptrs++;
		if (tp->kind == TK_POINTER &&
		    tp->fbase && tp->fbase->wasdeclared)
		    break;
                continue;

            case TK_ARRAY:
		flags &= ~ODECL_CHARSTAR;
                if (tp->meaning && tp->meaning->kind == MK_TYPE &&
                    tp->meaning->wasdeclared)
                    break;
		if (tp->structdefd) {    /* conformant array */
		    if (!variablearrays &&
			!(tp->basetype->kind == TK_ARRAY &&
			  tp->basetype->structdefd))   /* avoid mult. notes */
			note("Conformant array code may not work in all compilers [101]");
		}
                ex = arraysize(tp, 1);
                if (!ex)
                    ex = makeexpr_name("", tp_integer);
                dimen[depth++] = ex;
		anyarrays++;
		if (tp->fbase && tp->fbase->wasdeclared)
		    break;
                continue;

            case TK_SET:
                ord_range_expr(tp->indextype, NULL, &maxv);
                maxv = enum_to_int(copyexpr(maxv));
                if (ord_type(maxv->val.type)->kind == TK_CHAR)
                    maxv->val.type = tp_integer;
                dimen[depth++] = makeexpr_plus(makeexpr_div(maxv, makeexpr_setbits()),
                                               makeexpr_long(2));
                break;

            case TK_STRING:
                if ((flags & ODECL_CHARSTAR) && stararrays == 1) {
                    dimen[depth++] = NULL;
                } else {
                    ord_range_expr(tp->indextype, NULL, &maxv);
                    dimen[depth++] = makeexpr_plus(copyexpr(maxv), makeexpr_long(1));
                }
                continue;

            case TK_FILE:
                break;

	    case TK_CPROCPTR:
		dimen[depth++] = NULL;
		anyptrs++;
		if (procptrprototypes)
		    continue;
                dimen[depth++] = &funcdummy;
		break;

            case TK_FUNCTION:
                dimen[depth++] = &funcdummy;
                if (!functype)
                    functype = tp;
                continue;

	    default:
		break;
        }
        break;
    }
    if (!*name && depth && (spaceexprs > 0 ||
                            (spaceexprs != 0 && !dimen[depth-1])))
        output(" ");    /* spacing for abstract declarator */
    if ((flags & ODECL_FUNCTION) && anyptrs)
        output(" ");
    if (anyarrays > 1 && !(flags & ODECL_FUNCTION))
	output("\003");
    for (i = depth; --i >= 0; ) {
        if (!dimen[i])
            output("*");
        if (i > 0 &&
            ((dimen[i] && !dimen[i-1]) ||
             (dimen[i-1] && !dimen[i] && extraparens > 0)))
            output("(");
    }
    if (flags & ODECL_FUNCTION)
        output("\n");
    if (anyarrays > 1 && (flags & ODECL_FUNCTION))
	output("\003");
    output(name);
    for (i = 0; i < depth; i++) {
        if (i > 0 &&
            ((dimen[i] && !dimen[i-1]) ||
             (dimen[i-1] && !dimen[i] && extraparens > 0)))
            output(")");
        if (dimen[i]) {
            if (dimen[i] == &funcdummy) {
		if (lookback(1) == ')')
		    output("\002");
		if (functype)
		    declare_args(functype, (flags & ODECL_HEADER) != 0,
				           (flags & ODECL_FORWARD) != 0);
		else if (spacefuncs)
		    output(" ()");
		else
		    output("()");
            } else {
		if (lookback(1) == ']')
		    output("\002");
                output("[");
                if (!(flags & ODECL_FREEARRAY) || stararrays == 0 || i > 0)
                    out_expr(dimen[i]);
                freeexpr(dimen[i]);
                output("]");
            }
        }
    }
    if (anyarrays > 1)
	output("\004");
}






/* Find out if types t1 and t2 will work out to be the same C type,
   for purposes of type-casting */

Type *canonicaltype(type)
Type *type;
{
    if (type->kind == TK_SUBR || type->kind == TK_ENUM ||
        type->kind == TK_PROCPTR)
        type = findbasetype(type, 0);
    if (type == tp_char)
        return tp_ubyte;
    if (type->kind == TK_POINTER) {
	if (type->smin)
	    return type;
        else if (type->basetype->kind == TK_ARRAY ||
		 type->basetype->kind == TK_STRING ||
		 type->basetype->kind == TK_SET)
            return makepointertype(canonicaltype(type->basetype->basetype));
        else if (type->basetype == tp_void)
            return (voidstar) ? tp_anyptr : makepointertype(tp_abyte);
        else if (type->basetype->kind == TK_FILE)
            return tp_text;
        else
            return makepointertype(canonicaltype(type->basetype));
    }
    return type;
}


int identicaltypes(t1, t2)
Type *t1, *t2;
{
    if (t1 == t2)
	return 1;
    if (t1->kind == t2->kind) {
	if (t1->kind == TK_SUBR)
	    return (identicaltypes(t1->basetype, t2->basetype) &&
		    exprsame(t1->smin, t2->smin, 2) &&
		    exprsame(t1->smax, t2->smax, 2));
	if (t1->kind == TK_SET ||
	    t1->kind == TK_SMALLSET)
	    return (exprsame(t1->indextype->smax,
			     t2->indextype->smax, 2));
	if (t1->kind == TK_ARRAY ||
	    t1->kind == TK_STRING ||
	    t1->kind == TK_SMALLARRAY)
	    return (identicaltypes(t1->basetype, t2->basetype) &&
		    identicaltypes(t1->indextype, t2->indextype) &&
		    t1->structdefd == t2->structdefd &&
		    ((!t1->smin && !t2->smin) ||
		     (t1->smin && t2->smin &&
		      exprsame(t1->smin, t2->smin, 2))) &&
		    ((!t1->smax && !t2->smax) ||
		     (t1->smax && t2->smax &&
		      exprsame(t1->smax, t2->smax, 2) &&
		      t1->escale == t2->escale &&
		      t1->issigned == t2->issigned)));
    }
    return 0;
}


int similartypes(t1, t2)
Type *t1, *t2;
{
    if (debug > 3) { fprintf(outf, "similartypes("); dumptypename(t1,1); fprintf(outf, ","); dumptypename(t2,1); fprintf(outf, ") = %d\n", identicaltypes(t1, t2)); }
    if (identicaltypes(t1, t2))
	return 1;
    t1 = canonicaltype(t1);
    t2 = canonicaltype(t2);
    return (t1 == t2);
}





Static int checkstructconst(mp)
Meaning *mp;
{
    return (mp->kind == MK_VAR &&
	    mp->constdefn &&
            mp->constdefn->kind == EK_CONST &&
            (mp->constdefn->val.type->kind == TK_ARRAY ||
             mp->constdefn->val.type->kind == TK_RECORD));
}


Static int mixable(mp1, mp2, args, flags)
Meaning *mp1, *mp2;
int args, flags;
{
    Type *tp1 = mp1->type, *tp2 = mp2->type;

    if (mixvars == 0)
        return 0;
    if (mp1->kind == MK_FIELD &&
        (mp1->val.i || mp2->val.i) && mixfields == 0)
        return 0;
    if (checkstructconst(mp1) || checkstructconst(mp2))
        return 0;
    if (mp1->comments) {
	if (findcomment(mp1->comments, CMT_NOT | CMT_PRE, -1))
	    return 0;
    }
    if (mp2->comments) {
	if (findcomment(mp2->comments, CMT_PRE, -1))
	    return 0;
    }
    if ((mp1->constdefn && (mp1->kind == MK_VAR || mp1->kind == MK_VARREF)) ||
	(mp2->constdefn && (mp2->kind == MK_VAR || mp2->kind == MK_VARREF))) {
        if (mixinits == 0)
            return 0;
        if (mixinits != 1 &&
            (!mp1->constdefn || !mp2->constdefn))
            return 0;
    }
    if (args) {
        if (mp1->kind == MK_PARAM && mp1->othername)
            tp1 = mp1->rectype;
        if (mp2->kind == MK_PARAM && mp2->othername)
            tp2 = mp2->rectype;
    }
    if (tp1 == tp2)
        return 1;
    switch (mixtypes) {
        case 0:
            return 0;
        case 1:
            return (findbasetype(tp1, flags) == findbasetype(tp2, flags));
        default:
            if (findbasetype(tp1, flags) != findbasetype(tp2, flags))
		return 0;
            while (tp1->kind == TK_POINTER && !tp1->smin && tp1->basetype)
                tp1 = tp1->basetype;
            while (tp2->kind == TK_POINTER && !tp2->smin && tp2->basetype)
                tp2 = tp2->basetype;
            return (tp1 == tp2);
    }
}



void declarefiles(fnames)
Strlist *fnames;
{
    Meaning *mp;
    char *cp;

    while (fnames) {
	mp = (Meaning *)fnames->value;
	if (mp->kind == MK_VAR || mp->kind == MK_FIELD) {
	    if (mp->namedfile) {
		output(storageclassname(varstorageclass(mp)));
		output(format_ss("%s %s", charname,
				 format_s(name_FNVAR, fnames->s)));
		output(format_s("[%s];\n", *name_FNSIZE ? name_FNSIZE : "80"));
	    }
	    if (mp->bufferedfile && *declbufname) {
		cp = format_s("%s", storageclassname(varstorageclass(mp)));
		if (*cp && isspace(cp[strlen(cp)-1]))
		  cp[strlen(cp)-1] = 0;
		if (*cp || !*declbufncname) {
		    output(declbufname);
		    output("(");
		    output(fnames->s);
		    output(",");
		    output(cp);
		} else {
		    output(declbufncname);
		    output("(");
		    output(fnames->s);
		}
		output(",");
		out_type(mp->type->basetype->basetype, 1);
		output(");\n");
	    }
	}
	strlist_eat(&fnames);
    }
}



char *variantfieldname(num)
int num;
{
    if (num >= 0)
        return format_d("U%d", num);
    else
        return format_d("UM%d", -num);
}


int record_is_union(tp)
Type *tp;
{
    return (tp->kind == TK_RECORD &&
	    tp->fbase && tp->fbase->kind == MK_VARIANT);
}


void outfieldlist(mp)
Meaning *mp;
{
    Meaning *mp0;
    int num, only_union, empty, saveindent, saveindent2;
    Strlist *fnames, *fn;

    if (!mp) {
	output("int empty_struct;   /* Pascal record was empty */\n");
	return;
    }
    only_union = (mp && mp->kind == MK_VARIANT);
    fnames = NULL;
    while (mp && mp->kind == MK_FIELD) {
	flushcomments(&mp->comments, CMT_PRE, -1);
	output(storageclassname(varstorageclass(mp) & 0x10));
	if (mp->dtype)
	    output(mp->dtype->name);
	else
	    outbasetype(mp->type, 0);
        output(" \005");
	for (;;) {
	    if (mp->dtype)
		output(mp->name);
	    else
		outdeclarator(mp->type, mp->name, 0);
	    if (mp->val.i && (mp->type != tp_abyte || mp->val.i != 8))
		output(format_d(" : %d", mp->val.i));
	    if (isfiletype(mp->type, 0)) {
		fn = strlist_append(&fnames, mp->name);
		fn->value = (long)mp;
	    }
	    mp->wasdeclared = 1;
	    if (!mp->cnext || mp->cnext->kind != MK_FIELD ||
		mp->dtype != mp->cnext->dtype ||
		varstorageclass(mp) != varstorageclass(mp->cnext) ||
		!mixable(mp, mp->cnext, 0, 0))
		break;
            mp = mp->cnext;
	    if (spacecommas)
		output(",\001 ");
	    else
		output(",\001");
        }
        output(";");
	outtrailcomment(mp->comments, -1, declcommentindent);
	flushcomments(&mp->comments, -1, -1);
        mp = mp->cnext;
    }
    declarefiles(fnames);
    if (mp) {
	saveindent = outindent;
	empty = 1;
        if (!only_union) {
            output("union {\n");
	    moreindent(tabsize);
	    moreindent(structindent);
        }
        while (mp) {
            mp0 = mp->ctx;
            num = ord_value(mp->val);
            while (mp && mp->ctx == mp0)
                mp = mp->cnext;
            if (mp0) {
		empty = 0;
                if (!mp0->cnext && mp0->kind == MK_FIELD) {
		    mp0->val.i = 0;   /* no need for bit fields in a union! */
                    outfieldlist(mp0);
                } else {
                    if (mp0->kind == MK_VARIANT)
                        output("union {\n");
                    else
                        output("struct {\n");
		    saveindent2 = outindent;
		    moreindent(tabsize);
		    moreindent(structindent);
                    outfieldlist(mp0);
		    outindent = saveindent2;
                    output("} ");
                    output(format_s(name_VARIANT, variantfieldname(num)));
                    output(";\n");
                }
		flushcomments(&mp0->comments, -1, -1);
            }
        }
	if (empty)
	    output("int empty_union;   /* Pascal variant record was empty */\n");
        if (!only_union) {
            outindent = saveindent;
            output("} ");
            output(format_s(name_UNION, ""));
            output(";\n");
        }
    }
}



void declarebigfile(type)
Type *type;
{
    output("FILE *f;\n");
    if (!*declbufncname) {
	output(declbufname);
	output("(f,,");
    } else {
	output(declbufncname);
	output("(f,");
    }
    out_type(type->basetype, 1);
    output(");\n");
    output(charname);
    output(format_s(" name[%s];\n", *name_FNSIZE ? name_FNSIZE : "80"));
}



void outbasetype(type, flags)
Type *type;
int flags;
{
    Meaning *mp;
    int saveindent;

    type = findbasetype(type, flags | ODECL_DECL);
    if (type->preserved && type->meaning->wasdeclared) {
	output(type->meaning->name);
	return;
    }
    switch (type->kind) {

        case TK_INTEGER:
            if (type == tp_uint) {
                output("unsigned");
            } else if (type == tp_sint) {
                if (useAnyptrMacros == 1)
                    output("Signed int");
                else if (hassignedchar)
                    output("signed int");
                else
                    output("int");   /* will sign-extend by hand */
            } else if (type == tp_unsigned) {
                output("unsigned long");
            } else if (type != tp_int)
                output(integername);
            else
                output("int");
            break;

        case TK_SUBR:
            if (type == tp_special_anyptr) {
                output("Anyptr");
            } else if (type == tp_abyte) {
                output("char");
            } else if (type == tp_ubyte) {
                output(ucharname);
            } else if (type == tp_sbyte) {
                output(scharname);
                if (signedchars != 1 && !hassignedchar)
                    note("'signed char' may not be valid in all compilers [102]");
            } else {
                if (type == tp_ushort)
                    output("unsigned ");
                output("short");
            }
            break;

        case TK_CHAR:
            if (type == tp_uchar) {
                output(ucharname);
            } else if (type == tp_schar) {
                output(scharname);
                if (signedchars != 1 && !hassignedchar)
                    note("'signed char' may not be valid in all compilers [102]");
	    } else
		output(charname);
            break;

        case TK_BOOLEAN:
            output((*name_BOOLEAN) ? name_BOOLEAN : ucharname);
            break;

        case TK_REAL:
	    if (type == tp_longreal)
		output("double");
	    else
		output("float");
            break;

        case TK_VOID:
            if (ansiC == 0)
                output("int");
            else if (useAnyptrMacros == 1)
                output("Void");
            else
                output("void");
            break;

        case TK_PROCPTR:
	    output(name_PROCEDURE);
	    break;

        case TK_FILE:
            output("FILE");
            break;

	case TK_SPECIAL:
	    if (type == tp_jmp_buf)
		output("jmp_buf");
	    break;

        default:
	    if (type->kind == TK_POINTER && type->smin) {
		note("Forward pointer reference assumes struct type [323]");
		output("struct ");
		output(format_s(name_STRUCT, type->smin->val.s));
	    } else if (type->meaning && type->meaning->kind == MK_TYPE &&
                type->meaning->wasdeclared) {
                output(type->meaning->name);
            } else {
                switch (type->kind) {

                    case TK_ENUM:
                        output("enum {\n");
			saveindent = outindent;
			moreindent(tabsize);
			moreindent(structindent);
                        mp = type->fbase;
                        while (mp) {
                            output(mp->name);
                            mp = mp->xnext;
                            if (mp)
				if (spacecommas)
				    output(",\001 ");
				else
				    output(",\001");
                        }
                        outindent = saveindent;
                        output("\n}");
                        break;

                    case TK_RECORD:
                    case TK_BIGFILE:
                        if (record_is_union(type))
                            output("union ");
                        else
                            output("struct ");
                        if (type->meaning)
                            output(format_s(name_STRUCT, type->meaning->name));
			if (!type->structdefd) {
			    if (type->meaning) {
				type->structdefd = 1;
				output(" ");
			    }
                            output("{\n");
			    saveindent = outindent;
			    moreindent(tabsize);
			    moreindent(structindent);
			    if (type->kind == TK_BIGFILE)
				declarebigfile(type);
			    else
				outfieldlist(type->fbase);
                            outindent = saveindent;
                            output("}");
                        }
			break;

		    default:
			break;

                }
            }
            break;
    }
}



void out_type(type, witharrays)
Type *type;
int witharrays;
{
    if (!witharrays && type->kind == TK_ARRAY)
        type = makepointertype(type->basetype);
    outbasetype(type, 0);
    outdeclarator(type, "", 0);    /* write an "abstract declarator" */
}




int varstorageclass(mp)
Meaning *mp;
{
    int sclass;

    if (mp->kind == MK_PARAM || mp->kind == MK_VARPARAM ||
	mp->kind == MK_FIELD)
	sclass = 0;
    else if (blockkind == TOK_EXPORT)
        if (usevextern)
	    if (mp->constdefn &&
		(mp->kind == MK_VAR ||
		 mp->kind == MK_VARREF))
		sclass = 2;    /* extern */
	    else
		sclass = 1;    /* vextern */
        else
            sclass = 0;                         /* (plain) */
    else if (mp->isfunction && mp->kind != MK_FUNCTION)
	sclass = 2;   /* extern */
    else if (mp->ctx->kind == MK_MODULE &&
	     (var_static != 0 ||
	      (findsymbol(mp->name)->flags & NEEDSTATIC)) &&
	     !mp->exported && !mp->istemporary && blockkind != TOK_END)
        sclass = (useAnyptrMacros) ? 4 : 3;     /* (private) */
    else if (mp->isforward)
        sclass = 3;   /* static */
    else
	sclass = 0;   /* (plain) */
    if (mp->volatilequal)
	sclass |= 0x10;
    if (mp->constqual)
	sclass |= 0x20;
    if (debug>2) fprintf(outf, "varstorageclass(%s) = %d\n", mp->name, sclass);
    return sclass;
}


char *storageclassname(i)
int i;
{
    char *scname;

    switch (i & 0xf) {
        case 1:
            scname = "vextern ";
	    break;
        case 2:
            scname = "extern ";
	    break;
        case 3:
            scname = "static ";
	    break;
        case 4:
            scname = "Static ";
	    break;
        default:
            scname = "";
	    break;
    }
    if (i & 0x10)
	if (useAnyptrMacros == 1)
	    scname = format_s("%sVolatile ", scname);
	else if (ansiC > 0)
	    scname = format_s("%svolatile ", scname);
    if (i & 0x20)
	if (useAnyptrMacros == 1)
	    scname = format_s("%sConst ", scname);
	else if (ansiC > 0)
	    scname = format_s("%sconst ", scname);
    return scname;
}



Static int var_mixable;

void declarevar(mp, which)
Meaning *mp;
int which;    /* 0x1=header, 0x2=body, 0x4=trailer, 0x8=in varstruct */
{
    int isstatic, isstructconst, saveindent, i;
    Strlist *sl;

    isstructconst = checkstructconst(mp);
    isstatic = varstorageclass(mp);
    if (which & 0x8)
	isstatic &= 0x10;   /* clear all but Volatile flags */
    flushcomments(&mp->comments, CMT_PRE, -1);
    if (which & 0x1) {
        if (isstructconst)
            outsection(minorspace);
        output(storageclassname(isstatic));
	if (mp->dtype)
	    output(mp->dtype->name);
        else
	    outbasetype(mp->type, 0);
        output(" \005");
    }
    if (which & 0x2) {
	if (mp->dtype)
	    output(mp->name);
	else
	    outdeclarator(mp->type, mp->name, 0);
        if (mp->constdefn && blockkind != TOK_EXPORT &&
	    (mp->kind == MK_VAR || mp->kind == MK_VARREF)) {
            if (mp->varstructflag) {    /* move init code into function body */
                intwarning("declarevar",
                    format_s("Variable %s initializer not removed [125]", mp->name));
            } else {
                if (isstructconst) {
                    output(" = {\n");
		    saveindent = outindent;
		    moreindent(tabsize);
		    moreindent(structinitindent);
                    out_expr((Expr *)mp->constdefn->val.i);
                    outindent = saveindent;
                    output("\n}");
		    var_mixable = 0;
		} else if (mp->type->kind == TK_ARRAY &&
			   mp->constdefn->val.type->kind == TK_STRING &&
			   !initpacstrings) {
		    if (mp->ctx->kind == MK_MODULE) {
			sl = strlist_append(&initialcalls,
					    format_sss("memcpy(%s,\002 %s,\002 sizeof(%s))",
						       mp->name,
						       makeCstring(mp->constdefn->val.s,
								   mp->constdefn->val.i),
						       mp->name));
			sl->value = 1;
		    } else if (mp->isforward) {
			output(" = {\005");
			for (i = 0; i < mp->constdefn->val.i; i++) {
			    if (i > 0)
				output(",\001");
			    output(makeCchar(mp->constdefn->val.s[i]));
			}
			output("}");
			mp->constdefn = NULL;
			var_mixable = 0;
		    }
                } else {
		    output(" = ");
                    out_expr(mp->constdefn);
		}
            }
        }
    }
    if (which & 0x4) {
        output(";");
	outtrailcomment(mp->comments, -1, declcommentindent);
	flushcomments(&mp->comments, -1, -1);
        if (isstructconst)
            outsection(minorspace);
    }
}




Static int checkvarmacdef(ex, mp)
Expr *ex;
Meaning *mp;
{
    int i;

    if ((ex->kind == EK_NAME || ex->kind == EK_BICALL) &&
	!strcmp(ex->val.s, mp->name)) {
	ex->kind = EK_VAR;
	ex->val.i = (long)mp;
	ex->val.type = mp->type;
	return 1;
    }
    if (ex->kind == EK_VAR && ex->val.i == (long)mp)
	return 1;
    i = ex->nargs;
    while (--i >= 0)
	if (checkvarmacdef(ex->args[i], mp))
	    return 1;
    return 0;
}


int checkvarmac(mp)
Meaning *mp;
{
    if (mp->kind != MK_VARMAC && mp->kind != MK_FUNCTION)
	return 0;
    if (!mp->constdefn)
	return 0;
    return checkvarmacdef(mp->constdefn, mp);
}



#define varkind(k) ((k)==MK_VAR||(k)==MK_VARREF||(k)==MK_PARAM||(k)==MK_VARPARAM)

int declarevars(ctx, invarstruct)
Meaning *ctx;
int invarstruct;
{
    Meaning *mp, *mp0, *mp2;
    Strlist *fnames, *fn;
    int flag, first;

    if (ctx->kind == MK_FUNCTION && ctx->varstructflag && !invarstruct) {
        output("struct ");
        output(format_s(name_LOC, ctx->name));
        output(" ");
        output(format_s(name_VARS, ctx->name));
        output(";\n");
        flag = 1;
    } else
        flag = 0;
    if (debug>2) {
        fprintf(outf,"declarevars:\n");
        for (mp = ctx->cbase; mp; mp = mp->xnext) {
            fprintf(outf, "  %-22s%-15s%3d", mp->name,
                                             meaningkindname(mp->kind),
                                             mp->refcount);
            if (mp->wasdeclared)
                fprintf(outf, " [decl]");
            if (mp->varstructflag)
                fprintf(outf, " [struct]");
            fprintf(outf, "\n");
        }
    }
    fnames = NULL;
    for (;;) {
        mp = ctx->cbase;
        while (mp && (!(varkind(mp->kind) || checkvarmac(mp)) ||
		      mp->wasdeclared || mp->varstructflag != invarstruct ||
		      mp->refcount <= 0))
            mp = mp->cnext;
        if (!mp)
            break;
        flag = 1;
        first = 1;
        mp0 = mp2 = mp;
	var_mixable = 1;
        while (mp) {
            if ((varkind(mp->kind) || checkvarmac(mp)) &&
		!mp->wasdeclared && var_mixable &&
		mp->dtype == mp0->dtype &&
                varstorageclass(mp) == varstorageclass(mp0) &&
                mp->varstructflag == invarstruct && mp->refcount > 0) {
                if (mixable(mp2, mp, 0, 0) || first) {
                    if (!first)
			if (spacecommas)
			    output(",\001 ");
			else
			    output(",\001");
                    declarevar(mp, (first ? 0x3 : 0x2) |
			           (invarstruct ? 0x8 : 0));
		    mp2 = mp;
                    mp->wasdeclared = 1;
                    if (isfiletype(mp->type, 0)) {
                        fn = strlist_append(&fnames, mp->name);
                        fn->value = (long)mp;
                    }
                    first = 0;
                } else
                    if (mixvars != 1)
                        break;
            }
	    if (first) {
		intwarning("declarevars",
			   format_s("Unable to declare %s [126]", mp->name));
		mp->wasdeclared = 1;
		first = 0;
	    }
            if (mixvars == 0)
                break;
            mp = mp->cnext;
        }
        declarevar(mp2, 0x4);
    }
    declarefiles(fnames);
    return flag;
}



void redeclarevars(ctx)
Meaning *ctx;
{
    Meaning *mp;

    for (mp = ctx->cbase; mp; mp = mp->cnext) {
        if ((mp->kind == MK_VAR || mp->kind == MK_VARREF) &&
            mp->constdefn) {
            mp->wasdeclared = 0;    /* mark for redeclaration, this time */
        }                           /*  with its initializer */
    }
}





void out_argdecls(ftype)
Type *ftype;
{
    Meaning *mp, *mp0;
    Type *tp;
    int done;
    int flag = 1;
    char *name;

    done = 0;
    do {
        mp = ftype->fbase;
        while (mp && mp->wasdeclared)
            mp = mp->xnext;
        if (mp) {
            if (flag)
                output("\n");
            flag = 0;
            mp0 = mp;
            outbasetype(mp->othername ? mp->rectype : mp->type,
			ODECL_CHARSTAR|ODECL_FREEARRAY);
            output(" \005");
            while (mp) {
                if (!mp->wasdeclared) {
                    if (mp == mp0 ||
			mixable(mp0, mp, 1, ODECL_CHARSTAR|ODECL_FREEARRAY)) {
                        if (mp != mp0)
			    if (spacecommas)
				output(",\001 ");
			    else
				output(",\001");
                        name = (mp->othername) ? mp->othername : mp->name;
                        tp = (mp->othername) ? mp->rectype : mp->type;
                        outdeclarator(tp, name,
				      ODECL_CHARSTAR|ODECL_FREEARRAY);
                        mp->wasdeclared = 1;
                    } else
                        if (mixvars != 1)
                            break;
                }
                mp = mp->xnext;
            }
            output(";\n");
        } else
            done = 1;
    } while (!done);
    for (mp0 = ftype->fbase; mp0 && (mp0->type != tp_strptr ||
                                     !mp0->anyvarflag); mp0 = mp0->xnext) ;
    if (mp0) {
        output("int ");
        for (mp = mp0; mp; mp = mp->xnext) {
            if (mp->type == tp_strptr && mp->anyvarflag) {
                if (mp != mp0) {
                    if (mixvars == 0)
                        output(";\nint ");
                    else if (spacecommas)
                        output(",\001 ");
		    else
                        output(",\001");
                }
                output(format_s(name_STRMAX, mp->name));
            }
        }
        output(";\n");
    }
    if (ftype->meaning && ftype->meaning->ctx->kind == MK_FUNCTION &&
                          ftype->meaning->ctx->varstructflag) {
        if (flag)
            output("\n");
        output("struct ");
        output(format_s(name_LOC, ftype->meaning->ctx->name));
        output(" *");
        output(format_s(name_LINK, ftype->meaning->ctx->name));
        output(";\n");
    }
}




void makevarstruct(func)
Meaning *func;
{
    int flag = 0;
    int saveindent;

    outsection(minfuncspace);
    output(format_s("\n/* Local variables for %s: */\n", func->name));
    output("struct ");
    output(format_s(name_LOC, func->name));
    output(" {\n");
    saveindent = outindent;
    moreindent(tabsize);
    moreindent(structindent);
    if (func->ctx->kind == MK_FUNCTION && func->ctx->varstructflag) {
        output("struct ");
        output(format_s(name_LOC, func->ctx->name));
        output(" *");
        output(format_s(name_LINK, func->ctx->name));
        output(";\n");
        flag++;
    }
    flag += declarevars(func, 1);
    if (!flag)                       /* Avoid generating an empty struct */
        output("int _meef_;\n");     /* (I don't think this will ever happen) */
    outindent = saveindent;
    output("} ;\n");
    outsection(minfuncspace);
    strlist_insert(&varstructdecllist, func->name);
}






Type *maketype(kind)
enum typekind kind;
{
    Type *tp;
    tp = ALLOC(1, Type, types);
    tp->kind = kind;
    tp->basetype = NULL;
    tp->indextype = NULL;
    tp->pointertype = NULL;
    tp->meaning = NULL;
    tp->fbase = NULL;
    tp->smin = NULL;
    tp->smax = NULL;
    tp->issigned = 0;
    tp->dumped = 0;
    tp->structdefd = 0;
    tp->preserved = 0;
    return tp;
}




Type *makesubrangetype(type, smin, smax)
Type *type;
Expr *smin, *smax;
{
    Type *tp;

    if (type->kind == TK_SUBR)
        type = type->basetype;
    tp = maketype(TK_SUBR);
    tp->basetype = type;
    tp->smin = smin;
    tp->smax = smax;
    return tp;
}



Type *makesettype(setof)
Type *setof;
{
    Type *tp;
    long smax;

    if (ord_range(setof, NULL, &smax) && smax < setbits && smallsetconst >= 0)
        tp = maketype(TK_SMALLSET);
    else
        tp = maketype(TK_SET);
    tp->basetype = tp_integer;
    tp->indextype = setof;
    return tp;
}



Type *makestringtype(len)
int len;
{
    Type *type;
    int index;

    len |= 1;
    if (len >= stringceiling)
        type = tp_str255;
    else {
        index = (len-1) / 2;
        if (stringtypecache[index])
            return stringtypecache[index];
        type = maketype(TK_STRING);
        type->basetype = tp_char;
        type->indextype = makesubrangetype(tp_integer, 
                                           makeexpr_long(0), 
                                           makeexpr_long(len));
        stringtypecache[index] = type;
    }
    return type;
}



Type *makepointertype(type)
Type *type;
{
    Type *tp;

    if (type->pointertype)
        return type->pointertype;
    tp = maketype(TK_POINTER);
    tp->basetype = type;
    type->pointertype = tp;
    return tp;
}





Value p_constant(type)
Type *type;
{
    Value val;
    Expr *ex;

    ex = p_expr(type);
    if (type)
        ex = gentle_cast(ex, type);
    val = eval_expr(ex);
    freeexpr(ex);
    if (!val.type) {
        warning("Expected a constant [127]");
        val.type = (type) ? type : tp_integer;
    }
    return val;
}




int typebits(smin, smax)
long smin, smax;
{
    unsigned long size;
    int bits;

    if (smin >= 0 || (smin == -1 && smax == 0)) {
        bits = 1;
        size = smax;
    } else {
        bits = 2;
        smin = -1L - smin;
        if (smin >= smax)
            size = smin;
        else
            size = smax;
    }
    while (size > 1) {
        bits++;
        size >>= 1;
    }
    return bits;
}


int packedsize(fname, typep, sizep, mode)
char *fname;
Type **typep;
long *sizep;
int mode;
{
    Type *tp = *typep;
    long smin, smax;
    int res, issigned;
    short savefold;
    long size;

    if (packing == 0)   /* suppress packing */
        return 0;
    if (tp->kind != TK_SUBR && tp->kind != TK_INTEGER && tp->kind != TK_ENUM &&
        tp->kind != TK_CHAR && tp->kind != TK_BOOLEAN)
        return 0;
    if (tp == tp_unsigned)
	return 0;
    if (!ord_range(tp, &smin, &smax)) {
        savefold = foldconsts;
        foldconsts = 1;
        res = ord_range(tp, &smin, &smax);
        foldconsts = savefold;
        if (res) {
            note(format_s("Field width for %s is based on expansion of #defines [103]",
                          fname));
        } else {
            note(format_ss("Cannot compute size of field %s; assuming %s [104]",
                           fname, integername));
            return 0;
        }
    } else {
        if (tp->kind == TK_ENUM)
            note(format_ssd("Field width for %s assumes enum%s has %d elements [105]",
                            fname,
                            (tp->meaning) ? format_s(" %s", tp->meaning->name) : "",
                            smax + 1));
    }
    issigned = (smin < 0);
    size = typebits(smin, smax);
    if (size >= ((sizeof_long > 0) ? sizeof_long : 32))
        return 0;
    if (packing != 1) {
        if (size <= 8)
            size = 8;
        else if (size <= 16)
            size = 16;
        else
            return 0;
    }
    if (!issigned) {
        *typep = (mode == 0) ? tp_int : tp_uint;
    } else {
        if (mode == 2 && !hassignedchar && !*signextname)
            return 0;
        *typep = (mode == 1) ? tp_int : tp_sint;
    }
    *sizep = size;
    return issigned;
}



Static void fielddecl(mp, type, tp2, val, ispacked, aligned)
Meaning *mp;
Type **type, **tp2;
long *val;
int ispacked, *aligned;
{
    long smin, smax, smin2, smax2;

    *tp2 = *type;
    *val = 0;
    if (ispacked && !mp->constdefn && *type != tp_unsigned) {
        (void)packedsize(mp->sym->name, tp2, val, signedfield);
        if (*aligned && *val &&
            (ord_type(*type)->kind == TK_CHAR ||
             ord_type(*type)->kind == TK_INTEGER) &&
            ord_range(findbasetype(*type, 0), &smin, &smax)) {
	    if (ord_range(*type, &smin2, &smax2)) {
		if (typebits(smin, smax) == 16 &&
		    typebits(smin2, smax2) == 8 && *val == 8) {
		    *tp2 = tp_abyte;
		}
	    }
	    if (typebits(smin, smax) == *val &&
		*val != 7) {    /* don't be fooled by tp_abyte */
		/* don't need to use a bit-field for this field */
		/* so not specifying one may make it more efficient */
		/* (and also helps to simulate HP's $allow_packed$ mode) */
		*val = 0;
		*tp2 = *type;
	    } 
        }
        if (*aligned && *val == 8 &&
            (ord_type(*type)->kind == TK_BOOLEAN ||
             ord_type(*type)->kind == TK_ENUM)) {
            *val = 0;
            *tp2 = tp_ubyte;
        }
    }
    if (*val != 8 && *val != 16)
	*aligned = (*val == 0);
}



/* This function locates byte-sized fields which were unaligned, but which
   are followed by aligned quantities so that they can be made aligned
   with no loss in storage efficiency. */

Static void realignfields(firstmp, stopmp)
Meaning *firstmp, *stopmp;
{
    Meaning *mp;

    for (mp = firstmp; mp && mp != stopmp; mp = mp->cnext) {
	if (mp->kind == MK_FIELD) {
	    if (mp->val.i == 16) {
		if (mp->type == tp_uint)
		    mp->type = tp_ushort;
		else
		    mp->type = tp_sshort;
		mp->val.i = 0;
	    } else if (mp->val.i == 8) {
		if (mp->type == tp_uint) {
		    mp->type = tp_ubyte;
		    mp->val.i = 0;
		} else if (hassignedchar || signedchars == 1) {
		    mp->type = tp_sbyte;
		    mp->val.i = 0;
		} else
		    mp->type = tp_abyte;
	    }
	}
    }
}

static void tryrealignfields(firstmp)
Meaning *firstmp;
{
    Meaning *mp, *head;

    head = NULL;
    for (mp = firstmp; mp; mp = mp->cnext) {
	if (mp->kind == MK_FIELD) {
	    if ((mp->val.i == 8 &&
		 (mp->type == tp_uint ||
		  hassignedchar || signedchars == 1)) ||
		mp->val.i == 16) {
		if (!head)
		    head = mp;
	    } else {
		if (mp->val.i == 0)
		    realignfields(head, mp);
		head = NULL;
	    }
	}
    }
    realignfields(head, NULL);
}



void decl_comments(mp)
Meaning *mp;
{
    Strlist *cmt;

    if (spitcomments != 1) {
	changecomments(curcomments, -1, -1, CMT_PRE, 0);
	strlist_mix(&mp->comments, curcomments);
	curcomments = NULL;
	cmt = grabcomment(CMT_TRAIL);
	if (cmt) {
	    changecomments(mp->comments, CMT_TRAIL, -1, CMT_PRE, -1);
	    strlist_mix(&mp->comments, cmt);
	}
	if (mp->comments)
	    mp->refcount++;   /* force it to be included if it has comments */
    }
}





Static void p_fieldlist(tp, flast, ispacked, tname)
Type *tp;
Meaning **flast;
int ispacked;
Meaning *tname;
{
    Meaning *firstm, *lastm, *veryfirstm, *dtype;
    Symbol *sym;
    Type *type, *tp2;
    long li1, li2;
    int aligned, constflag, volatileflag;
    short saveskipind;
    Strlist *l1;

    saveskipind = skipindices;
    skipindices = 0;
    aligned = 1;
    lastm = NULL;
    veryfirstm = NULL;
    while (curtok == TOK_IDENT) {
        firstm = addfield(curtoksym, &flast, tp, tname);
	if (!veryfirstm)
	    veryfirstm = firstm;
        lastm = firstm;
        gettok();
	decl_comments(lastm);
        while (curtok == TOK_COMMA) {
            gettok();
            if (wexpecttok(TOK_IDENT))
		lastm = addfield(curtoksym, &flast, tp, tname);
            gettok();
	    decl_comments(lastm);
        }
        if (wneedtok(TOK_COLON)) {
	    constflag = volatileflag = 0;
	    p_attributes();
	    if ((l1 = strlist_find(attrlist, "READONLY")) != NULL) {
		constflag = 1;
		strlist_delete(&attrlist, l1);
	    }
	    if ((l1 = strlist_find(attrlist, "VOLATILE")) != NULL) {
		volatileflag = 1;
		strlist_delete(&attrlist, l1);
	    }
	    dtype = (curtok == TOK_IDENT) ? curtokmeaning : NULL;
	    type = p_type(firstm);
	    decl_comments(lastm);
	    fielddecl(firstm, &type, &tp2, &li1, ispacked, &aligned);
	    dtype = validatedtype(dtype, type);
	    for (;;) {
		firstm->type = tp2;
		firstm->dtype = dtype;
		firstm->val.type = type;
		firstm->val.i = li1;
		firstm->constqual = constflag;
		firstm->volatilequal = volatileflag;
		tp->meaning = tname;
		setupfilevar(firstm);
		tp->meaning = NULL;
		if (firstm == lastm)
		    break;
		firstm = firstm->cnext;
	    }
	} else
	    skiptotoken2(TOK_SEMI, TOK_CASE);
        if (curtok == TOK_SEMI)
            gettok();
    }
    if (curtok == TOK_CASE) {
        gettok();
	if (curtok == TOK_COLON)
	    gettok();
	wexpecttok(TOK_IDENT);
	sym = curtoksym;
	if (curtokmeaning)
	    type = curtokmeaning->type;
	gettok();
        if (curtok == TOK_COLON) {
            firstm = addfield(sym, &flast, tp, tname);
	    if (!veryfirstm)
		veryfirstm = firstm;
            gettok();
	    firstm->isforward = 1;
            firstm->val.type = type = p_type(firstm);
            fielddecl(firstm, &firstm->val.type, &firstm->type, &firstm->val.i, 
                      ispacked, &aligned);
        } else {
	    firstm = NULL;
	}
        if (!wneedtok(TOK_OF)) {
	    skiptotoken2(TOK_END, TOK_RPAR);
	    goto bounce;
	}
	if (firstm)
	    decl_comments(firstm);
	while (curtok == TOK_VBAR)
	    gettok();
        while (curtok != TOK_END && curtok != TOK_RPAR) {
            firstm = NULL;
            for (;;) {
		lastm = addfield(NULL, &flast, tp, tname);
		if (!firstm)
		    firstm = lastm;
		checkkeyword(TOK_OTHERWISE);
		if (curtok == TOK_ELSE || curtok == TOK_OTHERWISE) {
		    lastm->val = make_ord(type, 999);
		    break;
		} else {
		    lastm->val = p_constant(type);
		    if (curtok == TOK_DOTS) {
			gettok();
			li1 = ord_value(lastm->val);
			li2 = ord_value(p_constant(type));
			while (++li1 <= li2) {
			    lastm = addfield(NULL, &flast, tp, tname);
			    lastm->val = make_ord(type, li1);
			}
		    }
		}
                if (curtok == TOK_COMMA)
                    gettok();
                else
                    break;
            }
	    if (curtok == TOK_ELSE || curtok == TOK_OTHERWISE) {
		gettok();
            } else if (!wneedtok(TOK_COLON) ||
		     (!modula2 && !wneedtok(TOK_LPAR))) {
		skiptotoken2(TOK_END, TOK_RPAR);
		goto bounce;
	    }
            p_fieldlist(tp, &lastm->ctx, ispacked, tname);
            while (firstm != lastm) {
                firstm->ctx = lastm->ctx;
                firstm = firstm->cnext;
            }
	    if (modula2) {
		while (curtok == TOK_VBAR)
		    gettok();
	    } else {
		if (!wneedtok(TOK_RPAR))
		    skiptotoken(TOK_RPAR);
	    }
            if (curtok == TOK_SEMI)
                gettok();
        }
	if (modula2) {
	    wneedtok(TOK_END);
	    if (curtok == TOK_IDENT) {
		note("Record variants supported only at end of record [106]");
		p_fieldlist(tp, &lastm->ctx, ispacked, tname);
	    }
	}
    }
    tryrealignfields(veryfirstm);
    if (lastm && curtok == TOK_END) {
	strlist_mix(&lastm->comments, curcomments);
	curcomments = NULL;
    }

  bounce:
    skipindices = saveskipind;
}



Static Type *p_arraydecl(tname, ispacked, confp)
char *tname;
int ispacked;
Meaning ***confp;
{
    Type *tp, *tp2;
    Meaning *mp;
    Expr *ex;
    long size, smin, smax, bitsize, fullbitsize;
    int issigned, bpower, hasrange;

    tp = maketype(TK_ARRAY);
    if (confp == NULL) {
	tp->indextype = p_type(NULL);
	if (tp->indextype->kind == TK_SUBR) {
	    if (ord_range(tp->indextype, &smin, NULL) &&
		smin > 0 && smin <= skipindices && !ispacked) {
		tp->smin = makeexpr_val(make_ord(tp->indextype->basetype, smin));
		ex = makeexpr_val(make_ord(tp->indextype->basetype, 0));
		tp->indextype = makesubrangetype(tp->indextype->basetype,
						 ex,
						 copyexpr(tp->indextype->smax));
	    }
	}
    } else {
	if (modula2) {
	    **confp = mp = addmeaning(findsymbol(format_s(name_ALOW, tname)), MK_PARAM);
	    mp->fakeparam = 1;
	    mp->constqual = 1;
	    mp->xnext = addmeaning(findsymbol(format_s(name_AHIGH, tname)), MK_PARAM);
	    mp->xnext->fakeparam = 1;
	    mp->xnext->constqual = 1;
	    *confp = &mp->xnext->xnext;
	    tp2 = maketype(TK_SUBR);
	    tp2->basetype = tp_integer;
	    mp->type = tp_integer;
	    mp->xnext->type = mp->type;
	    tp2->smin = makeexpr_long(0);
	    tp2->smax = makeexpr_minus(makeexpr_var(mp->xnext),
				       makeexpr_var(mp));
	    tp->indextype = tp2;
	    tp->structdefd = 1;
	} else {
	    wexpecttok(TOK_IDENT);
	    tp2 = maketype(TK_SUBR);
	    if (peeknextchar() != ',' &&
		(!curtokmeaning || curtokmeaning->kind != MK_TYPE)) {
		mp = addmeaning(curtoksym, MK_PARAM);
		gettok();
		wneedtok(TOK_DOTS);
		wexpecttok(TOK_IDENT);
		mp->xnext = addmeaning(curtoksym, MK_PARAM);
		gettok();
		if (wneedtok(TOK_COLON)) {
		    tp2->basetype = p_type(NULL);
		} else {
		    tp2->basetype = tp_integer;
		}
	    } else {
		mp = addmeaning(findsymbol(format_s(name_ALOW, tname)), MK_PARAM);
		mp->xnext = addmeaning(findsymbol(format_s(name_AHIGH, tname)), MK_PARAM);
		tp2->basetype = p_type(NULL);
	    }
	    mp->fakeparam = 1;
	    mp->constqual = 1;
	    mp->xnext->fakeparam = 1;
	    mp->xnext->constqual = 1;
	    **confp = mp;
	    *confp = &mp->xnext->xnext;
	    mp->type = tp2->basetype;
	    mp->xnext->type = tp2->basetype;
	    tp2->smin = makeexpr_var(mp);
	    tp2->smax = makeexpr_var(mp->xnext);
	    tp->indextype = tp2;
	    tp->structdefd = 1;     /* conformant array flag */
	}
    }
    if (curtok == TOK_COMMA || curtok == TOK_SEMI) {
        gettok();
        tp->basetype = p_arraydecl(tname, ispacked, confp);
        return tp;
    } else {
	if (!modula2) {
	    if (!wneedtok(TOK_RBR))
		skiptotoken(TOK_OF);
	}
        if (!wneedtok(TOK_OF))
	    skippasttotoken(TOK_OF, TOK_COMMA);
	checkkeyword(TOK_VARYING);
	if (confp != NULL &&
	    (curtok == TOK_ARRAY || curtok == TOK_PACKED ||
	     curtok == TOK_VARYING)) {
	    tp->basetype = p_conformant_array(tname, confp);
	} else {
	    tp->fbase = (curtok == TOK_IDENT) ? curtokmeaning : NULL;
	    tp->basetype = p_type(NULL);
	    tp->fbase = validatedtype(tp->fbase, tp->basetype);
	}
        if (!ispacked)
            return tp;
        size = 0;
        tp2 = tp->basetype;
        if (!tname)
            tname = "array";
        issigned = packedsize(tname, &tp2, &size, 1);
        if (!size || size > 8 ||
            (issigned && !packsigned) ||
            (size > 4 &&
             (!issigned || (signedchars == 1 || hassignedchar))))
            return tp;
        bpower = 0;
        while ((1<<bpower) < size)
            bpower++;        /* round size up to power of two */
        size = 1<<bpower;    /* size = # bits in an array element */
        tp->escale = bpower;
        tp->issigned = issigned;
        hasrange = ord_range(tp->indextype, &smin, &smax) &&
                   (smax < 100000);    /* don't be confused by giant arrays */
        if (hasrange &&
	    (bitsize = (smax - smin + 1) * size)
	        <= ((sizeof_integer > 0) ? sizeof_integer : 32)) {
            if (bitsize > ((sizeof_short > 0) ? sizeof_short : 16)) {
                tp2 = (issigned) ? tp_integer : tp_unsigned;
                fullbitsize = ((sizeof_integer > 0) ? sizeof_integer : 32);
            } else if (bitsize > ((sizeof_char > 0) ? sizeof_char : 8) ||
                       (issigned && !(signedchars == 1 || hassignedchar))) {
                tp2 = (issigned) ? tp_sshort : tp_ushort;
                fullbitsize = ((sizeof_short > 0) ? sizeof_short : 16);
            } else {
                tp2 = (issigned) ? tp_sbyte : tp_ubyte;
                fullbitsize = ((sizeof_char > 0) ? sizeof_char : 8);
            }
            tp->kind = TK_SMALLARRAY;
            if (ord_range(tp->indextype, &smin, NULL) &&
                smin > 0 && smin <= fullbitsize - bitsize) {
                tp->smin = makeexpr_val(make_ord(tp->indextype->basetype, smin));
		ex = makeexpr_val(make_ord(tp->indextype->basetype, 0));
                tp->indextype = makesubrangetype(tp->indextype->basetype, ex,
                                                 copyexpr(tp->indextype->smax));
            }
        } else {
            if (!issigned)
                tp2 = tp_ubyte;
            else if (signedchars == 1 || hassignedchar)
                tp2 = tp_sbyte;
            else
                tp2 = tp_sshort;
        }
        tp->smax = makeexpr_type(tp->basetype);
        tp->basetype = tp2;
        return tp;
    }
}



Static Type *p_conformant_array(tname, confp)
char *tname;
Meaning ***confp;
{
    int ispacked;
    Meaning *mp;
    Type *tp, *tp2;

    p_attributes();
    ignore_attributes();
    if (curtok == TOK_PACKED) {
	ispacked = 1;
	gettok();
    } else
	ispacked = 0;
    checkkeyword(TOK_VARYING);
    if (curtok == TOK_VARYING) {
	gettok();
	wneedtok(TOK_LBR);
	wexpecttok(TOK_IDENT);
	mp = addmeaning(curtoksym, MK_PARAM);
	mp->fakeparam = 1;
	mp->constqual = 1;
	**confp = mp;
	*confp = &mp->xnext;
	mp->type = tp_integer;
	tp2 = maketype(TK_SUBR);
	tp2->basetype = tp_integer;
	tp2->smin = makeexpr_long(1);
	tp2->smax = makeexpr_var(mp);
	tp = maketype(TK_STRING);
	tp->indextype = tp2;
	tp->basetype = tp_char;
	tp->structdefd = 1;     /* conformant array flag */
	gettok();
	wneedtok(TOK_RBR);
	skippasttoken(TOK_OF);
	tp->basetype = p_type(NULL);
	return tp;
    }
    if (wneedtok(TOK_ARRAY) &&
	(modula2 || wneedtok(TOK_LBR))) {
	return p_arraydecl(tname, ispacked, confp);
    } else {
	return tp_integer;
    }
}




/* VAX Pascal: */
void p_attributes()
{
    Strlist *l1;

    if (modula2)
	return;
    while (curtok == TOK_LBR) {
	implementationmodules = 1;    /* auto-detect VAX Pascal */
	do {
	    gettok();
	    if (!wexpecttok(TOK_IDENT)) {
		skippasttoken(TOK_RBR);
		return;
	    }
	    l1 = strlist_append(&attrlist, strupper(curtokbuf));
	    l1->value = -1;
	    gettok();
	    if (curtok == TOK_LPAR) {
		gettok();
		if (!strcmp(l1->s, "CHECK") ||
		    !strcmp(l1->s, "OPTIMIZE") ||
		    !strcmp(l1->s, "KEY") ||
		    !strcmp(l1->s, "COMMON") ||
		    !strcmp(l1->s, "PSECT") ||
		    !strcmp(l1->s, "EXTERNAL") ||
		    !strcmp(l1->s, "GLOBAL") ||
		    !strcmp(l1->s, "WEAK_EXTERNAL") ||
		    !strcmp(l1->s, "WEAK_GLOBAL")) {
		    l1->value = (long)stralloc(curtokbuf);
		    gettok();
		    while (curtok == TOK_COMMA) {
			gettok();
			gettok();
		    }
		} else if (!strcmp(l1->s, "INHERIT") ||
			   !strcmp(l1->s, "IDENT") ||
			   !strcmp(l1->s, "ENVIRONMENT")) {
		    p_expr(NULL);
		    while (curtok == TOK_COMMA) {
			gettok();
			p_expr(NULL);
		    }
		} else {
		    l1->value = ord_value(p_constant(tp_integer));
		    while (curtok == TOK_COMMA) {
			gettok();
			p_expr(NULL);
		    }
		}
		if (!wneedtok(TOK_RPAR)) {
		    skippasttotoken(TOK_RPAR, TOK_LBR);
		}
	    }
	} while (curtok == TOK_COMMA);
	if (!wneedtok(TOK_RBR)) {
	    skippasttoken(TOK_RBR);
	}
    }
}


void ignore_attributes()
{
    while (attrlist) {
	if (strcmp(attrlist->s, "HIDDEN") &&
	    strcmp(attrlist->s, "INHERIT") &&
	    strcmp(attrlist->s, "ENVIRONMENT"))
	    warning(format_s("Type attribute %s ignored [128]", attrlist->s));
	strlist_eat(&attrlist);
    }
}


int size_attributes()
{
    int size = -1;
    Strlist *l1;

    if ((l1 = strlist_find(attrlist, "BIT")) != NULL)
	size = 1;
    else if ((l1 = strlist_find(attrlist, "BYTE")) != NULL)
	size = 8;
    else if ((l1 = strlist_find(attrlist, "WORD")) != NULL)
	size = 16;
    else if ((l1 = strlist_find(attrlist, "LONG")) != NULL)
	size = 32;
    else if ((l1 = strlist_find(attrlist, "QUAD")) != NULL)
	size = 64;
    else if ((l1 = strlist_find(attrlist, "OCTA")) != NULL)
	size = 128;
    else
	return -1;
    if (l1->value >= 0)
	size *= l1->value;
    strlist_delete(&attrlist, l1);
    return size;
}


void p_mech_spec(doref)
int doref;
{
    if (curtok == TOK_IDENT && doref &&
	!strcicmp(curtokbuf, "%REF")) {
	note("Mechanism specified %REF treated like VAR [107]");
	curtok = TOK_VAR;
	return;
    }
    if (curtok == TOK_IDENT &&
	(!strcicmp(curtokbuf, "%REF") ||
	 !strcicmp(curtokbuf, "%IMMED") ||
	 !strcicmp(curtokbuf, "%DESCR") ||
	 !strcicmp(curtokbuf, "%STDESCR"))) {
	note(format_s("Mechanism specifier %s ignored [108]", curtokbuf));
	gettok();
    }
}


Type *p_modula_subrange(basetype)
Type *basetype;
{
    Type *tp;
    Value val;

    wneedtok(TOK_LBR);
    tp = maketype(TK_SUBR);
    tp->smin = p_ord_expr();
    if (basetype)
	tp->smin = gentle_cast(tp->smin, basetype);
    if (wexpecttok(TOK_DOTS)) {
	gettok();
	tp->smax = p_ord_expr();
	if (tp->smax->val.type->kind == TK_REAL &&
	    tp->smax->kind == EK_CONST &&
	    strlen(tp->smax->val.s) == 12 &&
	    strcmp(tp->smax->val.s, "2147483648.0") >= 0 &&
	    strcmp(tp->smax->val.s, "4294967295.0") <= 0) {
	    tp = tp_unsigned;
	} else if (basetype) {
	    tp->smin = gentle_cast(tp->smin, basetype);
	    tp->basetype = basetype;
	} else {
	    basetype = ord_type(tp->smin->val.type);
	    if (basetype->kind == TK_INTEGER) {
		val = eval_expr(tp->smin);
		if (val.type && val.i >= 0)
		    basetype = tp_unsigned;
		else
		    basetype = tp_integer;
	    }
	    tp->basetype = basetype;
	}
    } else {
	tp = tp_integer;
    }
    if (!wneedtok(TOK_RBR))
	skippasttotoken(TOK_RBR, TOK_SEMI);
    return tp;
}


void makefakestruct(tp, tname)
Type *tp;
Meaning *tname;
{
    Symbol *sym;

    if (!tname || blockkind == TOK_IMPORT)
	return;
    while (tp && (tp->kind == TK_ARRAY || tp->kind == TK_FILE))
	tp = tp->basetype;
    if (tp && tp->kind == TK_RECORD && !tp->meaning) {
	sym = findsymbol(format_s(name_FAKESTRUCT, tname->name));
	silentalreadydef++;
	tp->meaning = addmeaning(sym, MK_TYPE);
	silentalreadydef--;
	tp->meaning->type = tp;
	tp->meaning->refcount++;
	declaretype(tp->meaning);
    }
}


Type *p_type(tname)
Meaning *tname;
{
    Type *tp;
    int ispacked = 0;
    Meaning **flast;
    Meaning *mp;
    Strlist *sl;
    int num, isfunc, saveind, savenotephase, sizespec;
    Expr *ex;
    Value val;
    static int proctypecount = 0;

    p_attributes();
    sizespec = size_attributes();
    ignore_attributes();
    tp = tp_integer;
    if (curtok == TOK_PACKED) {
        ispacked = 1;
        gettok();
    }
    checkkeyword(TOK_VARYING);
    if (modula2)
	checkkeyword(TOK_POINTER);
    switch (curtok) {

        case TOK_RECORD:
            gettok();
	    savenotephase = notephase;
	    notephase = 1;
            tp = maketype(TK_RECORD);
            p_fieldlist(tp, &(tp->fbase), ispacked, tname);
	    notephase = savenotephase;
            if (!wneedtok(TOK_END)) {
		skippasttoken(TOK_END);
	    }
            break;

        case TOK_ARRAY:
            gettok();
	    if (!modula2) {
		if (!wneedtok(TOK_LBR))
		    break;
	    }
	    tp = p_arraydecl(tname ? tname->name : NULL, ispacked, NULL);
	    makefakestruct(tp, tname);
            break;

	case TOK_VARYING:
	    gettok();
	    tp = maketype(TK_STRING);
	    if (wneedtok(TOK_LBR)) {
		ex = p_ord_expr();
		if (!wneedtok(TOK_RBR))
		    skippasttoken(TOK_RBR);
	    } else
		ex = makeexpr_long(stringdefault);
	    if (wneedtok(TOK_OF))
		tp->basetype = p_type(NULL);
	    else
		tp->basetype = tp_char;
	    val = eval_expr(ex);
	    if (val.type) {
		if (val.i > 255 && val.i > stringceiling) {
		    note(format_d("Strings longer than %d may have problems [109]",
				  stringceiling));
		}
		if (stringceiling != 255 &&
		    (val.i >= 255 || val.i > stringceiling)) {
		    freeexpr(ex);
		    ex = makeexpr_long(stringceiling);
		}
	    }
	    tp->indextype = makesubrangetype(tp_integer, makeexpr_long(0), ex);
	    break;

        case TOK_SET:
            gettok();
            if (!wneedtok(TOK_OF))
		break;
	    tp = p_type(NULL);
	    if (tp == tp_integer || tp == tp_unsigned)
		tp = makesubrangetype(tp, makeexpr_long(0),
				      makeexpr_long(defaultsetsize-1));
	    if (tp->kind == TK_ENUM && !tp->meaning && useenum) {
		outbasetype(tp, 0);
		output(";");
	    }
            tp = makesettype(tp);
            break;

        case TOK_FILE:
            gettok();
	    if (structfilesflag ||
		(tname && strlist_cifind(structfiles, tname->name)))
		tp = maketype(TK_BIGFILE);
	    else
		tp = maketype(TK_FILE);
            if (curtok == TOK_OF) {
                gettok();
                tp->basetype = p_type(NULL);
            } else {
                tp->basetype = tp_abyte;
            }
	    if (tp->basetype->kind == TK_CHAR && charfiletext) {
		if (tp->kind == TK_FILE)
		    tp = tp_text;
		else
		    tp = tp_bigtext;
	    } else {
		if (tp->kind == TK_FILE) {
		    makefakestruct(tp, tname);
		    tp = makepointertype(tp);
		}
	    }
            break;

        case TOK_PROCEDURE:
	case TOK_FUNCTION:
	    isfunc = (curtok == TOK_FUNCTION);
            gettok();
	    if (curtok != TOK_LPAR && !isfunc && hasstaticlinks == 1) {
		tp = tp_proc;
		break;
	    }
	    proctypecount++;
	    mp = addmeaning(findsymbol(format_d("__PROCPTR%d",
						proctypecount)),
			    MK_FUNCTION);
	    pushctx(mp);
	    tp = maketype((hasstaticlinks != 0) ? TK_PROCPTR : TK_CPROCPTR);
	    tp->basetype = p_funcdecl(&isfunc, 1);
	    tp->fbase = mp;   /* (saved, but not currently used) */
	    tp->escale = hasstaticlinks;
	    popctx();
            break;

        case TOK_HAT:
	case TOK_ADDR:
	case TOK_POINTER:
	    if (curtok == TOK_POINTER) {
		gettok();
		wneedtok(TOK_TO);
		if (curtok == TOK_IDENT && !strcmp(curtokbuf, "WORD")) {
		    tp = tp_anyptr;
		    gettok();
		    break;
		}
	    } else
		gettok();
	    p_attributes();
	    ignore_attributes();
            tp = maketype(TK_POINTER);
            if (curtok == TOK_IDENT &&
		(!curtokmeaning || curtokmeaning->kind != MK_TYPE ||
		 (deferallptrs && curtokmeaning->ctx != curctx &&
		  curtokmeaning->ctx != nullctx))) {
                struct ptrdesc *pd;
                pd = ALLOC(1, struct ptrdesc, ptrdescs);
                pd->sym = curtoksym;
                pd->tp = tp;
                pd->next = ptrbase;
                ptrbase = pd;
                tp->basetype = tp_abyte;
		tp->smin = makeexpr_name(curtokcase, tp_integer);
		anydeferredptrs = 1;
                gettok();
            } else {
		tp->fbase = (curtok == TOK_IDENT) ? curtokmeaning : NULL;
                tp->basetype = p_type(NULL);
		tp->fbase = validatedtype(tp->fbase, tp->basetype);
                if (!tp->basetype->pointertype)
                    tp->basetype->pointertype = tp;
            }
            break;

        case TOK_LPAR:
            if (!useenum)
                outsection(minorspace);
	    enum_tname = tname;
            tp = maketype(TK_ENUM);
            flast = &(tp->fbase);
            num = 0;
            do {
                gettok();
                if (!wexpecttok(TOK_IDENT)) {
		    skiptotoken(TOK_RPAR);
		    break;
		}
                sl = strlist_find(constmacros, curtoksym->name);
                mp = addmeaningas(curtoksym, MK_CONST, MK_VARIANT);
                mp->val.type = tp;
                mp->val.i = num++;
                mp->type = tp;
                if (sl) {
                    mp->constdefn = (Expr *)sl->value;
                    mp->anyvarflag = 1;    /* Make sure constant is folded */
                    strlist_delete(&constmacros, sl);
                    if (mp->constdefn->kind == EK_NAME)
                        strchange(&mp->name, mp->constdefn->val.s);
                } else {
                    if (!useenum) {
			output(format_s("#define %s", mp->name));
			mp->isreturn = 1;
			out_spaces(constindent, 0, 0, 0);
			saveind = outindent;
			outindent = cur_column();
			output(format_d("%d\n", mp->val.i));
			outindent = saveind;
		    }
		}
                *flast = mp;
                flast = &(mp->xnext);
                gettok();
            } while (curtok == TOK_COMMA);
	    if (!wneedtok(TOK_RPAR))
		skippasttoken(TOK_RPAR);
            tp->smin = makeexpr_long(0);
            tp->smax = makeexpr_long(num-1);
            if (!useenum)
                outsection(minorspace);
            break;

	case TOK_LBR:
	    tp = p_modula_subrange(NULL);
	    break;

        case TOK_IDENT:
            if (!curtokmeaning) {
                undefsym(curtoksym);
                tp = tp_integer;
                mp = addmeaning(curtoksym, MK_TYPE);
                mp->type = tp;
                gettok();
                break;
            } else if (curtokmeaning == mp_string) {
                gettok();
                tp = maketype(TK_STRING);
                tp->basetype = tp_char;
                if (curtok == TOK_LBR) {
                    gettok();
                    ex = p_ord_expr();
                    if (!wneedtok(TOK_RBR))
			skippasttoken(TOK_RBR);
                } else {
		    ex = makeexpr_long(stringdefault);
                }
                val = eval_expr(ex);
                if (val.type && stringceiling != 255 &&
                    (val.i >= 255 || val.i > stringceiling)) {
                    freeexpr(ex);
                    ex = makeexpr_long(stringceiling);
                }
                tp->indextype = makesubrangetype(tp_integer, makeexpr_long(0), ex);
                break;
            } else if (curtokmeaning->kind == MK_TYPE) {
                tp = curtokmeaning->type;
		if (sizespec > 0) {
		    if (ord_type(tp)->kind == TK_INTEGER && sizespec <= 32) {
			if (checkconst(tp->smin, 0)) {
			    if (sizespec == 32)
				tp = tp_unsigned;
			    else
				tp = makesubrangetype(tp_unsigned,
					 makeexpr_long(0),
				         makeexpr_long((1L << sizespec) - 1));
			} else {
			    tp = makesubrangetype(tp_integer,
				     makeexpr_long(- ((1L << (sizespec-1)))),
				     makeexpr_long((1L << (sizespec-1)) - 1));
			}
			sizespec = -1;
		    }
		}
                gettok();
		if (curtok == TOK_LBR) {
		    if (modula2) {
			tp = p_modula_subrange(tp);
		    } else {
			gettok();
			ex = p_expr(tp_integer);
			note("UCSD size spec ignored; using 'long int' [110]");
			if (ord_type(tp)->kind == TK_INTEGER)
			    tp = tp_integer;
			if (!wneedtok(TOK_RBR))
			    skippasttotoken(TOK_RBR, TOK_SEMI);
		    }
		}
		if (tp == tp_text &&
		    (structfilesflag ||
		     (tname && strlist_cifind(structfiles, tname->name))))
		    tp = tp_bigtext;
                break;
            }

        /* fall through */
        default:
            tp = maketype(TK_SUBR);
            tp->smin = p_ord_expr();
	    if (curtok == TOK_COLON)
		curtok = TOK_DOTS;    /* UCSD Pascal */
	    if (wexpecttok(TOK_DOTS)) {
		gettok();
		tp->smax = p_ord_expr();
		if (tp->smax->val.type->kind == TK_REAL &&
		    tp->smax->kind == EK_CONST &&
		    strlen(tp->smax->val.s) == 12 &&
		    strcmp(tp->smax->val.s, "2147483648.0") >= 0 &&
		    strcmp(tp->smax->val.s, "4294967295.0") <= 0) {
		    tp = tp_unsigned;
		    break;
		}
		tp->basetype = ord_type(tp->smin->val.type);
		if (sizespec >= 0) {
		    long smin, smax;
		    if (ord_range(tp, &smin, &smax) &&
			typebits(smin, smax) == sizespec)
			sizespec = -1;
		}
	    } else {
		tp = tp_integer;
	    }
            break;
    }
    if (sizespec >= 0)
	note(format_d("Don't know how to interpret size = %d bits [111]", sizespec));
    return tp;
}





Type *p_funcdecl(isfunc, istype)
int *isfunc, istype;
{
    Meaning *retmp = NULL, *mp, *firstmp, *lastmp, **prevm, **oldprevm;
    Type *type, *tp;
    enum meaningkind parkind;
    int anyvarflag, constflag, volatileflag, num = 0;
    Symbol *sym;
    Expr *defval;
    Token savetok;
    Strlist *l1;

    if (*isfunc || modula2) {
        sym = findsymbol(format_s(name_RETV, curctx->name));
        retmp = addmeaning(sym, MK_VAR);
	retmp->isreturn = 1;
    }
    type = maketype(TK_FUNCTION);
    if (curtok == TOK_LPAR) {
        prevm = &type->fbase;
        do {
            gettok();
	    if (curtok == TOK_RPAR)
		break;
	    p_mech_spec(1);
	    p_attributes();
	    checkkeyword(TOK_ANYVAR);
            if (curtok == TOK_VAR || curtok == TOK_ANYVAR) {
                parkind = MK_VARPARAM;
                anyvarflag = (curtok == TOK_ANYVAR);
                gettok();
            } else if (curtok == TOK_PROCEDURE || curtok == TOK_FUNCTION) {
		savetok = curtok;
		gettok();
		wexpecttok(TOK_IDENT);
		*prevm = firstmp = addmeaning(curtoksym, MK_PARAM);
		prevm = &firstmp->xnext;
		firstmp->anyvarflag = 0;
		curtok = savetok;   /* rearrange tokens to a proc ptr type! */
		firstmp->type = p_type(firstmp);
		continue;
            } else {
                parkind = MK_PARAM;
                anyvarflag = 0;
            }
	    oldprevm = prevm;
	    if (modula2 && istype) {
		firstmp = addmeaning(findsymbol(format_d("_A%d", ++num)), parkind);
	    } else {
		wexpecttok(TOK_IDENT);
		firstmp = addmeaning(curtoksym, parkind);
		gettok();
	    }
            *prevm = firstmp;
            prevm = &firstmp->xnext;
            firstmp->isactive = 0;   /* nit-picking Turbo compatibility */
	    lastmp = firstmp;
            while (curtok == TOK_COMMA) {
                gettok();
                if (wexpecttok(TOK_IDENT)) {
		    *prevm = lastmp = addmeaning(curtoksym, parkind);
		    prevm = &lastmp->xnext;
		    lastmp->isactive = 0;
		}
                gettok();
            }
	    constflag = volatileflag = 0;
	    defval = NULL;
            if (curtok != TOK_COLON && !modula2) {
		if (parkind != MK_VARPARAM)
		    wexpecttok(TOK_COLON);
		parkind = MK_VARPARAM;
                tp = tp_anyptr;
                anyvarflag = 1;
            } else {
		if (curtok == TOK_COLON)
		    gettok();
		if (curtok == TOK_IDENT && !curtokmeaning &&
		    !strcicmp(curtokbuf, "UNIV")) {
		    if (parkind == MK_PARAM)
			note("UNIV may not work for non-VAR parameters [112]");
		    anyvarflag = 1;
		    gettok();
		}
		p_attributes();
		if ((l1 = strlist_find(attrlist, "READONLY")) != NULL) {
		    constflag = 1;
		    strlist_delete(&attrlist, l1);
		}
		if ((l1 = strlist_find(attrlist, "VOLATILE")) != NULL) {
		    volatileflag = 1;
		    strlist_delete(&attrlist, l1);
		}
		if ((l1 = strlist_find(attrlist, "UNSAFE")) != NULL &&
		    parkind == MK_VARPARAM) {
		    anyvarflag = 1;
		    strlist_delete(&attrlist, l1);
		}
		if ((l1 = strlist_find(attrlist, "REFERENCE")) != NULL) {
		    note("REFERENCE attribute treated like VAR [107]");
		    parkind = MK_VARPARAM;
		    strlist_delete(&attrlist, l1);
		}
		checkkeyword(TOK_VARYING);
                if (curtok == TOK_IDENT && curtokmeaning == mp_string &&
                    !anyvarflag && parkind == MK_VARPARAM) {
                    anyvarflag = (varstrings > 0);
                    tp = tp_str255;
                    gettok();
		    if (curtok == TOK_LBR) {
			wexpecttok(TOK_SEMI);
			skipparens();
		    }
		} else if (curtok == TOK_ARRAY || curtok == TOK_PACKED ||
			   curtok == TOK_VARYING) {
		    prevm = oldprevm;
		    tp = p_conformant_array(firstmp->name, &prevm);
		    *prevm = firstmp;
		    while (*prevm)
			prevm = &(*prevm)->xnext;
                } else {
                    tp = p_type(firstmp);
                }
                if (!varfiles && isfiletype(tp, 0))
                    parkind = MK_PARAM;
                if (parkind == MK_VARPARAM)
                    tp = makepointertype(tp);
            }
	    if (curtok == TOK_ASSIGN) {    /* check for parameter default */
		gettok();
		p_mech_spec(0);
		defval = gentle_cast(p_expr(tp), tp);
		if ((tp->kind == TK_STRING || tp->kind == TK_ARRAY) &&
		    tp->basetype->kind == TK_CHAR &&
		    tp->structdefd &&     /* conformant string */
		    defval->val.type->kind == TK_STRING) {
		    mp = *oldprevm;
		    if (tp->kind == TK_ARRAY) {
			mp->constdefn = makeexpr_long(1);
			mp = mp->xnext;
		    }
		    mp->constdefn = strmax_func(defval);
		}
	    }
            while (firstmp) {
                firstmp->type = tp;
                firstmp->kind = parkind;    /* in case it changed */
                firstmp->isactive = 1;
                firstmp->anyvarflag = anyvarflag;
		firstmp->constqual = constflag;
		firstmp->volatilequal = volatileflag;
		if (defval) {
		    if (firstmp == lastmp)
			firstmp->constdefn = defval;
		    else
			firstmp->constdefn = copyexpr(defval);
		}
                if (parkind == MK_PARAM &&
                    (tp->kind == TK_STRING ||
                     tp->kind == TK_ARRAY ||
                     tp->kind == TK_SET ||
                     ((tp->kind == TK_RECORD ||
		       tp->kind == TK_BIGFILE ||
		       tp->kind == TK_PROCPTR) && copystructs < 2))) {
                    firstmp->othername = stralloc(format_s(name_COPYPAR,
							   firstmp->name));
                    firstmp->rectype = makepointertype(tp);
                }
		if (firstmp == lastmp)
		    break;
                firstmp = firstmp->xnext;
            }
        } while (curtok == TOK_SEMI || curtok == TOK_COMMA);
        if (!wneedtok(TOK_RPAR))
	    skippasttotoken(TOK_RPAR, TOK_SEMI);
    }
    if (modula2) {
	if (curtok == TOK_COLON) {
	    *isfunc = 1;
	} else {
	    unaddmeaning(retmp);
	}
    }
    if (*isfunc) {
        if (wneedtok(TOK_COLON)) {
	    retmp->type = type->basetype = p_type(NULL);
	    switch (retmp->type->kind) {
		
	      case TK_RECORD:
	      case TK_BIGFILE:
	      case TK_PROCPTR:
                if (copystructs >= 3)
                    break;
		
		/* fall through */
	      case TK_ARRAY:
	      case TK_STRING:
	      case TK_SET:
                type->basetype = retmp->type = makepointertype(retmp->type);
                retmp->kind = MK_VARPARAM;
                retmp->anyvarflag = 0;
                retmp->xnext = type->fbase;
                type->fbase = retmp;
                retmp->refcount++;
                break;

	      default:
		break;
	    }
	} else
	    retmp->type = type->basetype = tp_integer;
    } else
        type->basetype = tp_void;
    return type;
}





Symbol *findlabelsym()
{
    if (curtok == TOK_IDENT && 
        curtokmeaning && curtokmeaning->kind == MK_LABEL) {
#if 0
	if (curtokmeaning->ctx != curctx && curtokmeaning->val.i != 0)
	    curtokmeaning->val.i = --nonloclabelcount;
#endif
    } else if (curtok == TOK_INTLIT) {
        strcpy(curtokcase, curtokbuf);
        curtoksym = findsymbol(curtokbuf);
        curtokmeaning = curtoksym->mbase;
        while (curtokmeaning && !curtokmeaning->isactive)
            curtokmeaning = curtokmeaning->snext;
        if (!curtokmeaning || curtokmeaning->kind != MK_LABEL)
            return NULL;
#if 0
	if (curtokmeaning->ctx != curctx && curtokmeaning->val.i != 0)
	    if (curtokint == 0)
		curtokmeaning->val.i = -1;
	    else
		curtokmeaning->val.i = curtokint;
#endif
    } else
	return NULL;
    return curtoksym;
}


void p_labeldecl()
{
    Symbol *sp;
    Meaning *mp;

    do {
        gettok();
        if (curtok != TOK_IDENT)
            wexpecttok(TOK_INTLIT);
        sp = findlabelsym();
        mp = addmeaning(curtoksym, MK_LABEL);
	mp->val.i = 0;
	mp->xnext = addmeaning(findsymbol(format_s(name_LABVAR,
						   mp->name)),
			       MK_VAR);
	mp->xnext->type = tp_jmp_buf;
	mp->xnext->refcount = 0;
        gettok();
    } while (curtok == TOK_COMMA);
    if (!wneedtok(TOK_SEMI))
	skippasttoken(TOK_SEMI);
}





Meaning *findfieldname(sym, variants, nvars)
Symbol *sym;
Meaning **variants;
int *nvars;
{
    Meaning *mp, *mp0;

    mp = variants[*nvars-1];
    while (mp && mp->kind == MK_FIELD) {
        if (mp->sym == sym) {
            return mp;
        }
        mp = mp->cnext;
    }
    while (mp) {
        variants[(*nvars)++] = mp->ctx;
        mp0 = findfieldname(sym, variants, nvars);
        if (mp0)
            return mp0;
        (*nvars)--;
        while (mp->cnext && mp->cnext->ctx == mp->ctx)
            mp = mp->cnext;
        mp = mp->cnext;
    }
    return NULL;
}




Expr *p_constrecord(type, style)
Type *type;
int style;   /* 0=HP, 1=Turbo, 2=Oregon+VAX */
{
    Meaning *mp, *mp0, *variants[20], *newvariants[20], *curfield;
    Symbol *sym;
    Value val;
    Expr *ex, *cex;
    int i, j, nvars, newnvars, varcounts[20];

    if (!wneedtok(style ? TOK_LPAR : TOK_LBR))
	return makeexpr_long(0);
    cex = makeexpr(EK_STRUCTCONST, 0);
    nvars = 0;
    varcounts[0] = 0;
    curfield = type->fbase;
    for (;;) {
	if (style == 2) {
	    if (curfield) {
		mp = curfield;
		if (mp->kind == MK_VARIANT || mp->isforward) {
		    val = p_constant(mp->type);
		    if (mp->kind == MK_FIELD) {
			insertarg(&cex, cex->nargs, makeexpr_val(val));
			mp = mp->cnext;
		    }
		    val.type = mp->val.type;
		    if (!valuesame(val, mp->val)) {
			while (mp && !valuesame(val, mp->val))
			    mp = mp->cnext;
			if (mp) {
			    note("Attempting to initialize union member other than first [113]");
			    curfield = mp->ctx;
			} else {
			    warning("Tag value does not exist in record [129]");
			    curfield = NULL;
			}
		    } else
			curfield = mp->ctx;
		    goto ignorefield;
		} else {
		    i = cex->nargs;
		    insertarg(&cex, i, NULL);
		    if (mp->isforward && curfield->cnext)
			curfield = curfield->cnext->ctx;
		    else
			curfield = curfield->cnext;
		}
	    } else {
		warning("Too many fields in record constructor [130]");
		ex = p_expr(NULL);
		freeexpr(ex);
		goto ignorefield;
	    }
	} else {
	    if (!wexpecttok(TOK_IDENT)) {
		skiptotoken2(TOK_RPAR, TOK_RBR);
		break;
	    }
	    sym = curtoksym;
	    gettok();
	    if (!wneedtok(TOK_COLON)) {
		skiptotoken2(TOK_RPAR, TOK_RBR);
		break;
	    }
	    newnvars = 1;
	    newvariants[0] = type->fbase;
	    mp = findfieldname(sym, newvariants, &newnvars);
	    if (!mp) {
		warning(format_s("Field %s not in record [131]", sym->name));
		ex = p_expr(NULL);   /* good enough */
		freeexpr(ex);
		goto ignorefield;
	    }
	    for (i = 0; i < nvars && i < newnvars; i++) {
		if (variants[i] != newvariants[i]) {
		    warning("Fields are members of incompatible variants [132]");
		    ex = p_subconst(mp->type, style);
		    freeexpr(ex);
		    goto ignorefield;
		}
	    }
	    while (nvars < newnvars) {
		variants[nvars] = newvariants[nvars];
		if (nvars > 0) {
		    for (mp0 = variants[nvars-1]; mp0->kind != MK_VARIANT; mp0 = mp0->cnext) ;
		    if (mp0->ctx != variants[nvars])
			note("Attempting to initialize union member other than first [113]");
		}
		i = varcounts[nvars];
		for (mp0 = variants[nvars]; mp0 && mp0->kind == MK_FIELD; mp0 = mp0->cnext)
		    i++;
		nvars++;
		varcounts[nvars] = i;
		while (cex->nargs < i)
		    insertarg(&cex, cex->nargs, NULL);
	    }
	    i = varcounts[newnvars-1];
	    for (mp0 = variants[newnvars-1]; mp0->sym != sym; mp0 = mp0->cnext)
		i++;
	    if (cex->args[i])
		warning(format_s("Two constructors for %s [133]", mp->name));
	}
	ex = p_subconst(mp->type, style);
	if (ex->kind == EK_CONST &&
	    (ex->val.type->kind == TK_RECORD ||
	     ex->val.type->kind == TK_ARRAY))
	    ex = (Expr *)ex->val.i;
	cex->args[i] = ex;
ignorefield:
        if (curtok == TOK_COMMA || curtok == TOK_SEMI)
            gettok();
        else
            break;
    }
    if (!wneedtok(style ? TOK_RPAR : TOK_RBR))
	skippasttoken2(TOK_RPAR, TOK_RBR);
    if (style != 2) {
	j = 0;
	mp = variants[0];
	for (i = 0; i < cex->nargs; i++) {
	    while (!mp || mp->kind != MK_FIELD)
		mp = variants[++j];
	    if (!cex->args[i]) {
		warning(format_s("No constructor for %s [134]", mp->name));
		cex->args[i] = makeexpr_name("<oops>", mp->type);
	    }
	    mp = mp->cnext;
	}
    }
    val.type = type;
    val.i = (long)cex;
    val.s = NULL;
    return makeexpr_val(val);
}




Expr *p_constarray(type, style)
Type *type;
int style;
{
    Value val;
    Expr *ex, *cex;
    int nvals, skipped;
    long smin, smax;

    if (type->kind == TK_SMALLARRAY)
        warning("Small-array constructors not yet implemented [135]");
    if (!wneedtok(style ? TOK_LPAR : TOK_LBR))
	return makeexpr_long(0);
    if (type->smin && type->smin->kind == EK_CONST)
        skipped = type->smin->val.i;
    else
        skipped = 0;
    cex = NULL;
    for (;;) {
        if (style && (curtok == TOK_LPAR || curtok == TOK_LBR)) {
            ex = p_subconst(type->basetype, style);
            nvals = 1;
	} else if (curtok == TOK_REPEAT) {
	    gettok();
	    ex = p_expr(type->basetype);
	    if (ord_range(type->indextype, &smin, &smax)) {
		nvals = smax - smin + 1;
		if (cex)
		    nvals -= cex->nargs;
	    } else {
		nvals = 1;
		note("REPEAT not translatable for non-constant array bounds [114]");
	    }
            ex = gentle_cast(ex, type->basetype);
        } else {
            ex = p_expr(type->basetype);
            if (ex->kind == EK_CONST && ex->val.type->kind == TK_STRING &&
                ex->val.i > 1 && !skipped && style == 0 && !cex &&
                type->basetype->kind == TK_CHAR &&
                checkconst(type->indextype->smin, 1)) {
                if (!wneedtok(TOK_RBR))
		    skippasttoken2(TOK_RBR, TOK_RPAR);
                return ex;   /* not quite right, but close enough */
            }
            if (curtok == TOK_OF) {
                ex = gentle_cast(ex, tp_integer);
                val = eval_expr(ex);
                freeexpr(ex);
                if (!val.type)
                    warning("Expected a constant [127]");
                nvals = val.i;
                gettok();
                ex = p_expr(type->basetype);
            } else
                nvals = 1;
            ex = gentle_cast(ex, type->basetype);
        }
        nvals += skipped;
        skipped = 0;
        if (ex->kind == EK_CONST &&
            (ex->val.type->kind == TK_RECORD ||
             ex->val.type->kind == TK_ARRAY))
            ex = (Expr *)ex->val.i;
        if (nvals != 1) {
            ex = makeexpr_un(EK_STRUCTOF, type->basetype, ex);
            ex->val.i = nvals;
        }
        if (cex)
            insertarg(&cex, cex->nargs, ex);
        else
            cex = makeexpr_un(EK_STRUCTCONST, type, ex);
        if (curtok == TOK_COMMA)
            gettok();
        else
            break;
    }
    if (!wneedtok(style ? TOK_RPAR : TOK_RBR))
	skippasttoken2(TOK_RPAR, TOK_RBR);
    val.type = type;
    val.i = (long)cex;
    val.s = NULL;
    return makeexpr_val(val);
}




Expr *p_conststring(type, style)
Type *type;
int style;
{
    Expr *ex;
    Token close = (style ? TOK_RPAR : TOK_RBR);

    if (curtok != (style ? TOK_LPAR : TOK_LBR))
	return p_expr(type);
    gettok();
    ex = p_expr(tp_integer);  /* should handle "OF" and "," for constructors */
    if (curtok == TOK_OF || curtok == TOK_COMMA) {
        warning("Multi-element string constructors not yet supported [136]");
	skiptotoken(close);
    }
    if (!wneedtok(close))
	skippasttoken(close);
    return ex;
}




Expr *p_subconst(type, style)
Type *type;
int style;
{
    Value val;

    if (curtok == TOK_IDENT && curtokmeaning &&
	curtokmeaning->kind == MK_TYPE) {
	if (curtokmeaning->type != type)
	    warning("Type conflict in constant [137]");
	gettok();
    }
    if (curtok == TOK_IDENT && !strcicmp(curtokbuf, "ZERO") &&
	!curtokmeaning) {   /* VAX Pascal foolishness */
	gettok();
	if (type->kind == TK_STRING)
	    return makeexpr_string("");
	if (type->kind == TK_REAL)
	    return makeexpr_real("0.0");
	val.type = type;
	if (type->kind == TK_RECORD || type->kind == TK_ARRAY ||
	    type->kind == TK_SET)
	    val.i = (long)makeexpr_un(EK_STRUCTCONST, type, makeexpr_long(0));
	else
	    val.i = 0;
	val.s = NULL;
	return makeexpr_val(val);
    }
    switch (type->kind) {
	
      case TK_RECORD:
	if (curtok == (style ? TOK_LPAR : TOK_LBR))
	    return p_constrecord(type, style);
	break;
	
      case TK_SMALLARRAY:
      case TK_ARRAY:
	if (curtok == (style ? TOK_LPAR : TOK_LBR))
	    return p_constarray(type, style);
	break;
	
      case TK_SMALLSET:
      case TK_SET:
	if (curtok == TOK_LBR)
	    return p_setfactor(type, 1);
	break;
	
      default:
	break;
	
    }
    return gentle_cast(p_expr(type), type);
}



void p_constdecl()
{
    Meaning *mp;
    Expr *ex, *ex2;
    Type *oldtype;
    char savetokcase[sizeof(curtokcase)];
    Symbol *savetoksym;
    Strlist *sl;
    int i, saveindent, outflag = (blockkind != TOK_IMPORT);

    if (outflag)
        outsection(majorspace);
    flushcomments(NULL, -1, -1);
    gettok();
    oldtype = NULL;
    while (curtok == TOK_IDENT) {
        strcpy(savetokcase, curtokcase);
        savetoksym = curtoksym;
        gettok();
        strcpy(curtokcase, savetokcase);   /* what a kludge! */
        curtoksym = savetoksym;
        if (curtok == TOK_COLON) {     /* Turbo Pascal typed constant */
            mp = addmeaning(curtoksym, MK_VAR);
	    decl_comments(mp);
            gettok();
            mp->type = p_type(mp);
            if (wneedtok(TOK_EQ)) {
		if (mp->kind == MK_VARMAC) {
		    freeexpr(p_subconst(mp->type, 1));
		    note("Initializer ignored for variable with VarMacro [115]");
		} else {
		    mp->constdefn = p_subconst(mp->type, 1);
		    if (blockkind == TOK_EXPORT) {
			/*  nothing  */
		    } else {
			mp->isforward = 1;   /* static variable */
		    }
		}
	    }
	    decl_comments(mp);
        } else {
            sl = strlist_find(constmacros, curtoksym->name);
            if (sl) {
                mp = addmeaning(curtoksym, MK_VARMAC);
                mp->constdefn = (Expr *)sl->value;
                strlist_delete(&constmacros, sl);
            } else {
                mp = addmeaning(curtoksym, MK_CONST);
            }
	    decl_comments(mp);
            if (!wexpecttok(TOK_EQ)) {
		skippasttoken(TOK_SEMI);
		continue;
	    }
	    mp->isactive = 0;   /* A fine point indeed (see below) */
	    gettok();
	    if (curtok == TOK_IDENT &&
		curtokmeaning && curtokmeaning->kind == MK_TYPE &&
		(curtokmeaning->type->kind == TK_RECORD ||
		 curtokmeaning->type->kind == TK_SMALLARRAY ||
		 curtokmeaning->type->kind == TK_ARRAY)) {
		oldtype = curtokmeaning->type;
		gettok();
		ex = p_subconst(oldtype, (curtok == TOK_LBR) ? 0 : 2);
	    } else {
		ex = p_expr(NULL);
		if (charconsts)
		    ex = makeexpr_charcast(ex);
	    }
	    mp->isactive = 1;   /* Re-enable visibility of the new constant */
            if (mp->kind == MK_CONST)
                mp->constdefn = ex;
            if (ord_type(ex->val.type)->kind == TK_INTEGER) {
                i = exprlongness(ex);
                if (i > 0)
                    ex->val.type = tp_integer;
		else if (i < 0)
                    ex->val.type = tp_int;
            }
	    decl_comments(mp);
            mp->type = ex->val.type;
            mp->val = eval_expr(ex);
            if (mp->kind == MK_CONST) {
                switch (ex->val.type->kind) {

                    case TK_INTEGER:
                    case TK_BOOLEAN:
                    case TK_CHAR:
                    case TK_ENUM:
                    case TK_SUBR:
                    case TK_REAL:
                        if (foldconsts > 0)
                            mp->anyvarflag = 1;
                        break;

                    case TK_STRING:
                        if (foldstrconsts > 0)
                            mp->anyvarflag = 1;
                        break;

		    default:
			break;
                }
            }
	    flushcomments(&mp->comments, CMT_PRE, -1);
            if (ex->val.type->kind == TK_SET) {
                mp->val.type = NULL;
		if (mp->kind == MK_CONST) {
		    ex2 = makeexpr(EK_MACARG, 0);
		    ex2->val.type = ex->val.type;
		    mp->constdefn = makeexpr_assign(ex2, ex);
		}
            } else if (mp->kind == MK_CONST && outflag) {
                if (ex->val.type != oldtype) {
                    outsection(minorspace);
                    oldtype = ex->val.type;
                }
                switch (ex->val.type->kind) {

                    case TK_ARRAY:
                    case TK_RECORD:
                        select_outfile(codef);
                        outsection(minorspace);
                        if (blockkind == TOK_IMPLEMENT || blockkind == TOK_PROGRAM)
                            output("static ");
                        if (useAnyptrMacros == 1 || useconsts == 2)
                            output("Const ");
                        else if (useconsts > 0)
                            output("const ");
                        outbasetype(mp->type, ODECL_CHARSTAR|ODECL_FREEARRAY);
                        output(" ");
                        outdeclarator(mp->type, mp->name,
				      ODECL_CHARSTAR|ODECL_FREEARRAY);
                        output(" = {");
			outtrailcomment(mp->comments, -1, declcommentindent);
			saveindent = outindent;
			moreindent(tabsize);
			moreindent(structinitindent);
                     /*   if (mp->val.s)
                            output(mp->val.s);
                        else  */
                            out_expr((Expr *)mp->val.i);
                        outindent = saveindent;
                        output("\n};\n");
                        outsection(minorspace);
                        if (blockkind == TOK_EXPORT) {
                            select_outfile(hdrf);
                            if (usevextern)
                                output("vextern ");
                            if (useAnyptrMacros == 1 || useconsts == 2)
                                output("Const ");
                            else if (useconsts > 0)
                                output("const ");
                            outbasetype(mp->type, ODECL_CHARSTAR);
                            output(" ");
                            outdeclarator(mp->type, mp->name, ODECL_CHARSTAR);
                            output(";\n");
                        }
                        break;

                    default:
                        if (foldconsts > 0) break;
                        output(format_s("#define %s", mp->name));
			mp->isreturn = 1;
                        out_spaces(constindent, 0, 0, 0);
			saveindent = outindent;
			outindent = cur_column();
                        out_expr_factor(ex);
			outindent = saveindent;
			outtrailcomment(mp->comments, -1, declcommentindent);
                        break;

                }
            }
	    flushcomments(&mp->comments, -1, -1);
            if (mp->kind == MK_VARMAC)
                freeexpr(ex);
            mp->wasdeclared = 1;
        }
        if (!wneedtok(TOK_SEMI))
	    skippasttoken(TOK_SEMI);
    }
    if (outflag)
        outsection(majorspace);
}




void declaresubtypes(mp)
Meaning *mp;
{
    Meaning *mp2;
    Type *tp;
    struct ptrdesc *pd;

    while (mp) {
	if (mp->kind == MK_VARIANT) {
	    declaresubtypes(mp->ctx);
	} else {
	    tp = mp->type;
	    while (tp->basetype && !tp->meaning && tp->kind != TK_POINTER)
		tp = tp->basetype;
	    if (tp->meaning && !tp->meaning->wasdeclared &&
		(tp->kind == TK_RECORD || tp->kind == TK_ENUM) &&
		tp->meaning->ctx && tp->meaning->ctx != nullctx) {
		pd = ptrbase;   /* Do this now, just in case */
		while (pd) {
		    if (pd->tp->smin && pd->tp->basetype == tp_abyte) {
			pd->tp->smin = NULL;
			mp2 = pd->sym->mbase;
			while (mp2 && !mp2->isactive)
			    mp2 = mp2->snext;
			if (mp2 && mp2->kind == MK_TYPE) {
			    pd->tp->basetype = mp2->type;
			    pd->tp->fbase = mp2;
			    if (!mp2->type->pointertype)
				mp2->type->pointertype = pd->tp;
			}
		    }
		    pd = pd->next;
		}
		declaretype(tp->meaning);
	    }
	}
	mp = mp->cnext;
    }
}


void declaretype(mp)
Meaning *mp;
{
    int saveindent, pres;

    switch (mp->type->kind) {
	
      case TK_RECORD:
      case TK_BIGFILE:
	if (mp->type->meaning != mp) {
	    output(format_ss("typedef %s %s;",
			     mp->type->meaning->name,
			     mp->name));
	} else {
	    declaresubtypes(mp->type->fbase);
	    outsection(minorspace);
	    if (record_is_union(mp->type))
		output("typedef union ");
	    else
		output("typedef struct ");
	    output(format_s("%s {\n", format_s(name_STRUCT, mp->name)));
	    saveindent = outindent;
	    moreindent(tabsize);
	    moreindent(structindent);
	    if (mp->type->kind == TK_BIGFILE)
		declarebigfile(mp->type);
	    else
		outfieldlist(mp->type->fbase);
	    outindent = saveindent;
	    output(format_s("} %s;", mp->name));
	}
	outtrailcomment(mp->comments, -1, declcommentindent);
	mp->type->structdefd = 1;
	if (mp->type->meaning == mp)
	    outsection(minorspace);
	break;
	
      case TK_ARRAY:
      case TK_SMALLARRAY:
	output("typedef ");
	if (mp->type->meaning != mp) {
	    output(format_ss("%s %s",
			     mp->type->meaning->name,
			     mp->name));
	} else {
	    outbasetype(mp->type, 0);
	    output(" ");
	    outdeclarator(mp->type, mp->name, 0);
	}
	output(";");
	outtrailcomment(mp->comments, -1, declcommentindent);
	break;
	
      case TK_ENUM:
	if (useenum) {
	    output("typedef ");
	    if (mp->type->meaning != mp)
		output(mp->type->meaning->name);
	    else
		outbasetype(mp->type, 0);
	    output(" ");
	    output(mp->name);
	    output(";");
	    outtrailcomment(mp->comments, -1,
			    declcommentindent);
	}
	break;
	
      default:
	pres = preservetypes;
	if (mp->type->kind == TK_POINTER && preservepointers >= 0)
	    pres = preservepointers;
	if (mp->type->kind == TK_STRING && preservestrings >= 0)
	    if (preservestrings == 2)
		pres = mp->type->indextype->smax->kind != EK_CONST;
	    else
		pres = preservestrings;
	if (pres) {
	    output("typedef ");
	    mp->type->preserved = 0;
	    outbasetype(mp->type, 0);
	    output(" ");
	    outdeclarator(mp->type, mp->name, 0);
	    output(";\n");
	    mp->type->preserved = 1;
	    outtrailcomment(mp->comments, -1, declcommentindent);
	}
	break;
    }
    mp->wasdeclared = 1;
}



void declaretypes(outflag)
int outflag;
{
    Meaning *mp;

    for (mp = curctx->cbase; mp; mp = mp->cnext) {
        if (mp->kind == MK_TYPE && !mp->wasdeclared) {
            if (outflag) {
		flushcomments(&mp->comments, CMT_PRE, -1);
		declaretype(mp);
		flushcomments(&mp->comments, -1, -1);
            }
            mp->wasdeclared = 1;
        }
    }
}



void p_typedecl()
{
    Meaning *mp;
    int outflag = (blockkind != TOK_IMPORT);
    struct ptrdesc *pd;

    if (outflag)
        outsection(majorspace);
    flushcomments(NULL, -1, -1);
    gettok();
    outsection(minorspace);
    deferallptrs = 1;
    anydeferredptrs = 0;
    notephase = 1;
    while (curtok == TOK_IDENT) {
        mp = addmeaning(curtoksym, MK_TYPE);
	mp->type = tp_integer;    /* in case of syntax errors */
        gettok();
	decl_comments(mp);
	if (curtok == TOK_SEMI) {
	    mp->type = tp_anyptr;    /* Modula-2 opaque type */
	} else {
	    if (!wneedtok(TOK_EQ)) {
		skippasttoken(TOK_SEMI);
		continue;
	    }
	    mp->type = p_type(mp);
	    decl_comments(mp);
	    if (!mp->type->meaning)
		mp->type->meaning = mp;
	    if (mp->type->kind == TK_RECORD ||
		mp->type->kind == TK_BIGFILE)
		mp->type->structdefd = 1;
	    if (!anydeferredptrs)
		declaretypes(outflag);
	}
	if (!wneedtok(TOK_SEMI))
	    skippasttoken(TOK_SEMI);
    }
    notephase = 0;
    deferallptrs = 0;
    while (ptrbase) {
        pd = ptrbase;
	if (pd->tp->smin && pd->tp->basetype == tp_abyte) {
	    pd->tp->smin = NULL;
	    mp = pd->sym->mbase;
	    while (mp && !mp->isactive)
		mp = mp->snext;
	    if (!mp || mp->kind != MK_TYPE) {
		warning(format_s("Unsatisfied forward reference to type %s [138]", pd->sym->name));
	    } else {
		pd->tp->basetype = mp->type;
		pd->tp->fbase = mp;
		if (!mp->type->pointertype)
		    mp->type->pointertype = pd->tp;
	    }
        }
        ptrbase = ptrbase->next;
        FREE(pd);
    }
    declaretypes(outflag);
    outsection(minorspace);
    flushcomments(NULL, -1, -1);
    if (outflag)
        outsection(majorspace);
}





Static void nameexternalvar(mp, name)
Meaning *mp;
char *name;
{
    if (!wasaliased) {
	if (*externalias && my_strchr(externalias, '%'))
	    strchange(&mp->name, format_s(externalias, name));
	else
	    strchange(&mp->name, name);
    }
}


Static void handlebrackets(mp, skip, wasaliased)
Meaning *mp;
int skip, wasaliased;
{
    Expr *ex;

    checkkeyword(TOK_ORIGIN);
    if (curtok == TOK_ORIGIN) {
	gettok();
	ex = p_expr(tp_integer);
	mp->kind = MK_VARREF;
	mp->constdefn = gentle_cast(ex, tp_integer);
    } else if (curtok == TOK_LBR) {
        gettok();
        ex = p_expr(tp_integer);
        if (!wneedtok(TOK_RBR))
	    skippasttotoken(TOK_RBR, TOK_SEMI);
        if (skip) {
            freeexpr(ex);
            return;
        }
        if (ex->kind == EK_CONST && ex->val.type->kind == TK_STRING) {
	    nameexternalvar(mp, ex->val.s);
	    mp->isfunction = 1;   /* make it extern */
        } else {
            note(format_s("Absolute-addressed variable %s was generated [116]", mp->name));
            mp->kind = MK_VARREF;
            mp->constdefn = gentle_cast(ex, tp_integer);
        }
    }
}



Static void handleabsolute(mp, skip)
Meaning *mp;
int skip;
{
    Expr *ex;
    Value val;
    long i;

    checkkeyword(TOK_ABSOLUTE);
    if (curtok == TOK_ABSOLUTE) {
        gettok();
        if (skip) {
            freeexpr(p_expr(tp_integer));
            if (curtok == TOK_COLON) {
                gettok();
                freeexpr(p_expr(tp_integer));
            }
            return;
        }
        note(format_s("Absolute-addressed variable %s was generated [116]", mp->name));
        mp->kind = MK_VARREF;
        if (curtok == TOK_IDENT && 
            curtokmeaning && (curtokmeaning->kind != MK_CONST ||
                              ord_type(curtokmeaning->type)->kind != TK_INTEGER)) {
            mp->constdefn = makeexpr_addr(p_expr(NULL));
	    mp->isfunction = 1;   /* make it extern */
        } else {
            ex = gentle_cast(p_expr(tp_integer), tp_integer);
            if (curtok == TOK_COLON) {
                val = eval_expr(ex);
                if (!val.type)
                    warning("Expected a constant [127]");
                i = val.i & 0xffff;
                gettok();
                val = p_constant(tp_integer);
                i = (i<<16) | (val.i & 0xffff);   /* as good a notation as any! */
                ex = makeexpr_long(i);
                insertarg(&ex, 0, makeexpr_name("%#lx", tp_integer));
            }
            mp->constdefn = ex;
        }
    }
}



void setupfilevar(mp)
Meaning *mp;
{
    if (mp->kind != MK_VARMAC) {
	if (isfiletype(mp->type, 0)) {
	    if (storefilenames && *name_FNVAR)
		mp->namedfile = 1;
	    if (checkvarinlists(bufferedfiles, unbufferedfiles, 0, mp))
		mp->bufferedfile = 1;
	} else if (isfiletype(mp->type, 1)) {
	    mp->namedfile = 1;
	    mp->bufferedfile = 1;
	}
    }
}



Meaning *validatedtype(dtype, type)
Meaning *dtype;
Type *type;
{
    if (dtype &&
	(!type->preserved || !type->meaning ||
	 dtype->kind != MK_TYPE || dtype->type != type ||
	 type->meaning == dtype))
	return NULL;
    return dtype;
}


void p_vardecl()
{
    Meaning *firstmp, *lastmp, *dtype;
    Type *tp;
    int aliasflag, volatileflag, constflag, staticflag, globalflag, externflag;
    Strlist *l1;
    Expr *initexpr;

    gettok();
    notephase = 1;
    while (curtok == TOK_IDENT) {
        firstmp = lastmp = addmeaning(curtoksym, MK_VAR);
	lastmp->type = tp_integer;    /* in case of syntax errors */
        aliasflag = wasaliased;
        gettok();
        handlebrackets(lastmp, (lastmp->kind != MK_VAR), aliasflag);
	decl_comments(lastmp);
        while (curtok == TOK_COMMA) {
            gettok();
            if (wexpecttok(TOK_IDENT)) {
		lastmp = addmeaning(curtoksym, MK_VAR);
		lastmp->type = tp_integer;
		aliasflag = wasaliased;
		gettok();
		handlebrackets(lastmp, (lastmp->kind != MK_VAR), aliasflag);
		decl_comments(lastmp);
	    }
        }
        if (!wneedtok(TOK_COLON)) {
	    skippasttoken(TOK_SEMI);
	    continue;
	}
	p_attributes();
	volatileflag = constflag = staticflag = globalflag = externflag = 0;
	if ((l1 = strlist_find(attrlist, "READONLY")) != NULL) {
	    constflag = 1;
	    strlist_delete(&attrlist, l1);
	}
	if ((l1 = strlist_find(attrlist, "VOLATILE")) != NULL) {
	    volatileflag = 1;
	    strlist_delete(&attrlist, l1);
	}
	if ((l1 = strlist_find(attrlist, "STATIC")) != NULL) {
	    staticflag = 1;
	    strlist_delete(&attrlist, l1);
	}
	if ((l1 = strlist_find(attrlist, "AUTOMATIC")) != NULL) {
	    /* This is the default! */
	    strlist_delete(&attrlist, l1);
	}
	if ((l1 = strlist_find(attrlist, "AT")) != NULL) {
            note(format_s("Absolute-addressed variable %s was generated [116]", lastmp->name));
            lastmp->kind = MK_VARREF;
            lastmp->constdefn = makeexpr_long(l1->value);
	    strlist_delete(&attrlist, l1);
	}
	if ((l1 = strlist_find(attrlist, "GLOBAL")) != NULL ||
	    (l1 = strlist_find(attrlist, "WEAK_GLOBAL")) != NULL) {
	    globalflag = 1;
	    if (l1->value != -1)
		nameexternalvar(lastmp, (char *)l1->value);
	    if (l1->s[0] != 'W')
		strlist_delete(&attrlist, l1);
	}
	if ((l1 = strlist_find(attrlist, "EXTERNAL")) != NULL ||
	    (l1 = strlist_find(attrlist, "WEAK_EXTERNAL")) != NULL) {
	    externflag = 1;
	    if (l1->value != -1)
		nameexternalvar(lastmp, (char *)l1->value);
	    if (l1->s[0] != 'W')
		strlist_delete(&attrlist, l1);
	}
	dtype = (curtok == TOK_IDENT) ? curtokmeaning : NULL;
        tp = p_type(firstmp);
	decl_comments(lastmp);
        handleabsolute(lastmp, (lastmp->kind != MK_VAR));
	initexpr = NULL;
	if (curtok == TOK_ASSIGN) {    /* VAX Pascal initializer */
	    gettok();
	    initexpr = p_subconst(tp, 2);
	    if (lastmp->kind == MK_VARMAC) {
		freeexpr(initexpr);
		initexpr = NULL;
		note("Initializer ignored for variable with VarMacro [115]");
	    }
	}
	dtype = validatedtype(dtype, tp);
        for (;;) {
            if (firstmp->kind == MK_VARREF) {
                firstmp->type = makepointertype(tp);
                firstmp->constdefn = makeexpr_cast(firstmp->constdefn, firstmp->type);
            } else {
                firstmp->type = tp;
		setupfilevar(firstmp);
		if (initexpr) {
		    if (firstmp == lastmp)
			firstmp->constdefn = initexpr;
		    else
			firstmp->constdefn = copyexpr(initexpr);
		}
            }
	    firstmp->dtype = dtype;
	    firstmp->volatilequal = volatileflag;
	    firstmp->constqual = constflag;
	    firstmp->isforward |= staticflag;
	    firstmp->isfunction |= externflag;
	    firstmp->exported |= globalflag;
	    if (globalflag && (curctx->kind != MK_MODULE || mainlocals))
		declarevar(firstmp, -1);
            if (firstmp == lastmp)
                break;
            firstmp = firstmp->cnext;
        }
        if (!wneedtok(TOK_SEMI))
	    skippasttoken(TOK_SEMI);
    }
    notephase = 0;
}




void p_valuedecl()
{
    Meaning *mp;

    gettok();
    while (curtok == TOK_IDENT) {
	if (!curtokmeaning ||
	    curtokmeaning->kind != MK_VAR) {
	    warning(format_s("Initializer ignored for variable %s [139]",
			     curtokbuf));
	    skippasttoken(TOK_SEMI);
	} else {
	    mp = curtokmeaning;
	    gettok();
	    if (curtok == TOK_DOT || curtok == TOK_LBR) {
		note("Partial structure initialization not supported [117]");
		skippasttoken(TOK_SEMI);
	    } else if (wneedtok(TOK_ASSIGN)) {
		mp->constdefn = p_subconst(mp->type, 2);
		if (!wneedtok(TOK_SEMI))
		    skippasttoken(TOK_SEMI);
	    } else
		skippasttoken(TOK_SEMI);
	}
    }
}







/* Make a temporary variable that must be freed manually (or at the end of
   the current function by default) */

Meaning *maketempvar(type, name)
Type *type;
char *name;
{
    struct tempvarlist *tv, **tvp;
    Symbol *sym;
    Meaning *mp;
    char *fullname;

    tvp = &tempvars;   /* find a freed but allocated temporary */
    while ((tv = *tvp) && (!similartypes(tv->tvar->type, type) ||
                           tv->tvar->refcount == 0 ||
                           strcmp(tv->tvar->val.s, name)))
        tvp = &(tv->next);
    if (!tv) {
        tvp = &tempvars;    /* take over a now-cancelled temporary */
        while ((tv = *tvp) && (tv->tvar->refcount > 0 || 
                               strcmp(tv->tvar->val.s, name)))
            tvp = &(tv->next);
    }
    if (tv) {
        tv->tvar->type = type;
        *tvp = tv->next;
        mp = tv->tvar;
        FREE(tv);
        mp->refcount++;
        if (debug>1) { fprintf(outf,"maketempvar revives %s\n", mp->name); }
    } else {
        tempvarcount = 0;    /***/  /* experimental... */
        for (;;) {
            if (tempvarcount)
                fullname = format_s(name, format_d("%d", tempvarcount));
            else
                fullname = format_s(name, "");
            ++tempvarcount;
            sym = findsymbol(fullname);
            mp = sym->mbase;
            while (mp && !mp->isactive)
                mp = mp->snext;
            if (!mp)
                break;
            if (debug>1) { fprintf(outf,"maketempvar rejects %s\n", fullname); }
        }
	mp = addmeaning(sym, MK_VAR);
        mp->istemporary = 1;
        mp->type = type;
        mp->refcount = 1;
        mp->val.s = stralloc(name);
        if (debug>1) { fprintf(outf,"maketempvar creates %s\n", mp->name); }
    }
    return mp;
}



/* Make a temporary variable that will be freed at the end of this statement
   (rather than at the end of the function) by default */

Meaning *makestmttempvar(type, name)
Type *type;
char *name;
{
    struct tempvarlist *tv;
    Meaning *tvar;

    tvar = maketempvar(type, name);
    tv = ALLOC(1, struct tempvarlist, tempvars);
    tv->tvar = tvar;
    tv->active = 1;
    tv->next = stmttempvars;
    stmttempvars = tv;
    return tvar;
}



Meaning *markstmttemps()
{
    return (stmttempvars) ? stmttempvars->tvar : NULL;
}


void freestmttemps(mark)
Meaning *mark;
{
    struct tempvarlist *tv;

    while ((tv = stmttempvars) && tv->tvar != mark) {
        if (tv->active)
            freetempvar(tv->tvar);
        stmttempvars = tv->next;
        FREE(tv);
    }
}



/* This temporary variable is no longer used */

void freetempvar(tvar)
Meaning *tvar;
{
    struct tempvarlist *tv;

    if (debug>1) { fprintf(outf,"freetempvar frees %s\n", tvar->name); }
    tv = stmttempvars;
    while (tv && tv->tvar != tvar)
        tv = tv->next;
    if (tv)
        tv->active = 0;
    tv = ALLOC(1, struct tempvarlist, tempvars);
    tv->tvar = tvar;
    tv->next = tempvars;
    tempvars = tv;
}



/* The code that used this temporary variable has been deleted */

void canceltempvar(tvar)
Meaning *tvar;
{
    if (debug>1) { fprintf(outf,"canceltempvar cancels %s\n", tvar->name); }
    tvar->refcount--;
    freetempvar(tvar);
}








/* End. */


