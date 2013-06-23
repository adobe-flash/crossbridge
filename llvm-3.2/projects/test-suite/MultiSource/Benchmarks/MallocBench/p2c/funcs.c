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



#define PROTO_FUNCS_C
#include "trans.h"




Static Strlist *enumnames;
Static int enumnamecount;



void setup_funcs()
{
    enumnames = NULL;
    enumnamecount = 0;
}





int isvar(ex, mp)
Expr *ex;
Meaning *mp;
{
    return (ex->kind == EK_VAR && (Meaning *)ex->val.i == mp);
}




char *getstring(ex)
Expr *ex;
{
    ex = makeexpr_stringify(ex);
    if (ex->kind != EK_CONST || ex->val.type->kind != TK_STRING) {
        intwarning("getstring", "Not a string literal [206]");
	return "";
    }
    return ex->val.s;
}




Expr *p_parexpr(target)
Type *target;
{
    Expr *ex;

    if (wneedtok(TOK_LPAR)) {
	ex = p_expr(target);
	if (!wneedtok(TOK_RPAR))
	    skippasttotoken(TOK_RPAR, TOK_SEMI);
    } else
	ex = p_expr(target);
    return ex;
}



Type *argbasetype(ex)
Expr *ex;
{
    if (ex->kind == EK_CAST)
        ex = ex->args[0];
    if (ex->val.type->kind == TK_POINTER)
        return ex->val.type->basetype;
    else
        return ex->val.type;
}



Type *choosetype(t1, t2)
Type *t1, *t2;
{
    if (t1 == tp_void ||
        (type_sizeof(t2, 1) && !type_sizeof(t1, 1)))
        return t2;
    else
        return t1;
}



Expr *convert_offset(type, ex2)
Type *type;
Expr *ex2;
{
    long size;
    int i;
    Value val;
    Expr *ex3;

    if (type->kind == TK_POINTER ||
        type->kind == TK_ARRAY ||
        type->kind == TK_SET ||
        type->kind == TK_STRING)
        type = type->basetype;
    size = type_sizeof(type, 1);
    if (size == 1)
        return ex2;
    val = eval_expr_pasc(ex2);
    if (val.type) {
        if (val.i == 0)
            return ex2;
        if (size && val.i % size == 0) {
            freeexpr(ex2);
            return makeexpr_long(val.i / size);
        }
    } else {     /* look for terms like "n*sizeof(foo)" */
	while (ex2->kind == EK_CAST || ex2->kind == EK_ACTCAST)
	    ex2 = ex2->args[0];
        if (ex2->kind == EK_TIMES) {
	    for (i = 0; i < ex2->nargs; i++) {
		ex3 = convert_offset(type, ex2->args[i]);
		if (ex3) {
		    ex2->args[i] = ex3;
		    return resimplify(ex2);
		}
	    }
            for (i = 0;
                 i < ex2->nargs && ex2->args[i]->kind != EK_SIZEOF;
                 i++) ;
            if (i < ex2->nargs) {
                if (ex2->args[i]->args[0]->val.type == type) {
                    delfreearg(&ex2, i);
                    if (ex2->nargs == 1)
                        return ex2->args[0];
                    else
                        return ex2;
                }
            }
        } else if (ex2->kind == EK_PLUS) {
	    ex3 = copyexpr(ex2);
	    for (i = 0; i < ex2->nargs; i++) {
		ex3->args[i] = convert_offset(type, ex3->args[i]);
		if (!ex3->args[i]) {
		    freeexpr(ex3);
		    return NULL;
		}
	    }
	    freeexpr(ex2);
	    return resimplify(ex3);
        } else if (ex2->kind == EK_SIZEOF) {
            if (ex2->args[0]->val.type == type) {
                freeexpr(ex2);
                return makeexpr_long(1);
            }
        } else if (ex2->kind == EK_NEG) {
	    ex3 = convert_offset(type, ex2->args[0]);
	    if (ex3)
                return makeexpr_neg(ex3);
        }
    }
    return NULL;
}



Expr *convert_size(type, ex, name)
Type *type;
Expr *ex;
char *name;
{
    long size;
    Expr *ex2;
    int i, okay;
    Value val;

    if (debug>2) { fprintf(outf,"convert_size("); dumpexpr(ex); fprintf(outf,")\n"); }
    while (type->kind == TK_ARRAY || type->kind == TK_STRING)
        type = type->basetype;
    if (type == tp_void)
        return ex;
    size = type_sizeof(type, 1);
    if (size == 1)
        return ex;
    while (ex->kind == EK_CAST || ex->kind == EK_ACTCAST)
	ex = ex->args[0];
    switch (ex->kind) {

        case EK_TIMES:
            for (i = 0; i < ex->nargs; i++) {
                ex2 = convert_size(type, ex->args[i], NULL);
                if (ex2) {
                    ex->args[i] = ex2;
                    return resimplify(ex);
                }
            }
            break;

        case EK_PLUS:
            okay = 1;
            for (i = 0; i < ex->nargs; i++) {
                ex2 = convert_size(type, ex->args[i], NULL);
                if (ex2)
                    ex->args[i] = ex2;
                else
                    okay = 0;
            }
            ex = distribute_plus(ex);
            if ((ex->kind != EK_TIMES || !okay) && name)
                note(format_s("Suspicious mixture of sizes in %s [173]", name));
            return ex;

        case EK_SIZEOF:
            return ex;

	default:
	    break;
    }
    val = eval_expr_pasc(ex);
    if (val.type) {
        if (val.i == 0)
            return ex;
        if (size && val.i % size == 0) {
            freeexpr(ex);
            return makeexpr_times(makeexpr_long(val.i / size),
                                  makeexpr_sizeof(makeexpr_type(type), 0));
        }
    }
    if (name) {
        note(format_s("Can't interpret size in %s [174]", name));
        return ex;
    } else
        return NULL;
}












Static Expr *func_abs()
{
    Expr *ex;
    Meaning *tvar;
    int lness;

    ex = p_parexpr(tp_integer);
    if (ex->val.type->kind == TK_REAL)
        return makeexpr_bicall_1("fabs", tp_longreal, ex);
    else {
        lness = exprlongness(ex);
        if (lness < 0)
            return makeexpr_bicall_1("abs", tp_int, ex);
        else if (lness > 0 && *absname) {
            if (ansiC > 0) {
                return makeexpr_bicall_1("labs", tp_integer, ex);
            } else if (*absname == '*' && (exprspeed(ex) >= 5 || !nosideeffects(ex, 0))) {
                tvar = makestmttempvar(tp_integer, name_TEMP);
                return makeexpr_comma(makeexpr_assign(makeexpr_var(tvar),
                                                      ex),
                                      makeexpr_bicall_1(absname, tp_integer,
                                                        makeexpr_var(tvar)));
            } else {
                return makeexpr_bicall_1(absname, tp_integer, ex);
            }
        } else if (exprspeed(ex) < 5 && nosideeffects(ex, 0)) {
            return makeexpr_cond(makeexpr_rel(EK_LT, copyexpr(ex),
                                                     makeexpr_long(0)),
                                 makeexpr_neg(copyexpr(ex)),
                                 ex);
        } else {
            tvar = makestmttempvar(tp_integer, name_TEMP);
            return makeexpr_cond(makeexpr_rel(EK_LT, makeexpr_assign(makeexpr_var(tvar),
                                                                     ex),
                                                     makeexpr_long(0)),
                                 makeexpr_neg(makeexpr_var(tvar)),
                                 makeexpr_var(tvar));
        }
    }
}



Static Expr *func_addr()
{
    Expr *ex, *ex2, *ex3;
    Type *type, *tp2;
    int haspar;

    haspar = wneedtok(TOK_LPAR);
    ex = p_expr(tp_proc);
    if (curtok == TOK_COMMA) {
        gettok();
        ex2 = p_expr(tp_integer);
        ex3 = convert_offset(ex->val.type, ex2);
        if (checkconst(ex3, 0)) {
            ex = makeexpr_addrf(ex);
        } else {
            ex = makeexpr_addrf(ex);
            if (ex3) {
                ex = makeexpr_plus(ex, ex3);
            } else {
                note("Don't know how to reduce offset for ADDR [175]");
                type = makepointertype(tp_abyte);
		tp2 = ex->val.type;
                ex = makeexpr_cast(makeexpr_plus(makeexpr_cast(ex, type), ex2), tp2);
            }
        }
    } else {
	if ((ex->val.type->kind != TK_PROCPTR &&
	     ex->val.type->kind != TK_CPROCPTR) ||
	    (ex->kind == EK_VAR &&
	     ex->val.type == ((Meaning *)ex->val.i)->type))
	    ex = makeexpr_addrf(ex);
    }
    if (haspar) {
	if (!wneedtok(TOK_RPAR))
	    skippasttotoken(TOK_RPAR, TOK_SEMI);
    }
    return ex;
}


Static Expr *func_iaddress()
{
    return makeexpr_cast(func_addr(), tp_integer);
}



Static Expr *func_addtopointer()
{
    Expr *ex, *ex2, *ex3;
    Type *type, *tp2;

    if (!skipopenparen())
	return NULL;
    ex = p_expr(tp_anyptr);
    if (skipcomma()) {
	ex2 = p_expr(tp_integer);
    } else
	ex2 = makeexpr_long(0);
    skipcloseparen();
    ex3 = convert_offset(ex->val.type, ex2);
    if (!checkconst(ex3, 0)) {
	if (ex3) {
	    ex = makeexpr_plus(ex, ex3);
	} else {
	    note("Don't know how to reduce offset for ADDTOPOINTER [175]");
	    type = makepointertype(tp_abyte);
	    tp2 = ex->val.type;
	    ex = makeexpr_cast(makeexpr_plus(makeexpr_cast(ex, type), ex2), tp2);
	}
    }
    return ex;
}



Stmt *proc_assert()
{
    Expr *ex;

    ex = p_parexpr(tp_boolean);
    return makestmt_call(makeexpr_bicall_1("assert", tp_void, ex));
}



Stmt *wrapopencheck(sp, fex)
Stmt *sp;
Expr *fex;
{
    Stmt *sp2;

    if (FCheck(checkfileisopen) && !is_std_file(fex)) {
        sp2 = makestmt(SK_IF);
        sp2->exp1 = makeexpr_rel(EK_NE, filebasename(fex), makeexpr_nil());
        sp2->stm1 = sp;
        if (iocheck_flag) {
            sp2->stm2 = makestmt_call(makeexpr_bicall_1(name_ESCIO, tp_integer,
							makeexpr_name(filenotopenname, tp_int)));
        } else {
            sp2->stm2 = makestmt_assign(makeexpr_var(mp_ioresult),
					makeexpr_name(filenotopenname, tp_int));
        }
        return sp2;
    } else {
        freeexpr(fex);
        return sp;
    }
}



Static Expr *checkfilename(nex)
Expr *nex;
{
    Expr *ex;

    nex = makeexpr_stringcast(nex);
    if (nex->kind == EK_CONST && nex->val.type->kind == TK_STRING) {
        switch (which_lang) {

            case LANG_HP:
                if (!strncmp(nex->val.s, "#1:", 3) ||
                    !strncmp(nex->val.s, "console:", 8) ||
                    !strncmp(nex->val.s, "CONSOLE:", 8)) {
                    freeexpr(nex);
                    nex = makeexpr_string("/dev/tty");
                } else if (!strncmp(nex->val.s, "#2:", 3) ||
                           !strncmp(nex->val.s, "systerm:", 8) ||
                           !strncmp(nex->val.s, "SYSTERM:", 8)) {
                    freeexpr(nex);
                    nex = makeexpr_string("/dev/tty");     /* should do more? */
                } else if (!strncmp(nex->val.s, "#6:", 3) ||
                           !strncmp(nex->val.s, "printer:", 8) ||
                           !strncmp(nex->val.s, "PRINTER:", 8)) {
                    note("Opening a file named PRINTER: [176]");
                } else if (my_strchr(nex->val.s, ':')) {
                    note("Opening a file whose name contains a ':' [177]");
                }
                break;

            case LANG_TURBO:
                if (checkstring(nex, "con") ||
                    checkstring(nex, "CON") ||
                    checkstring(nex, "")) {
                    freeexpr(nex);
                    nex = makeexpr_string("/dev/tty");
                } else if (checkstring(nex, "nul") ||
                           checkstring(nex, "NUL")) {
                    freeexpr(nex);
                    nex = makeexpr_string("/dev/null");
                } else if (checkstring(nex, "lpt1") ||
                           checkstring(nex, "LPT1") ||
                           checkstring(nex, "lpt2") ||
                           checkstring(nex, "LPT2") ||
                           checkstring(nex, "lpt3") ||
                           checkstring(nex, "LPT3") ||
                           checkstring(nex, "com1") ||
                           checkstring(nex, "COM1") ||
                           checkstring(nex, "com2") ||
                           checkstring(nex, "COM2")) {
                    note("Opening a DOS device file name [178]");
                }
                break;

	    default:
		break;
        }
    } else {
	if (*filenamefilter && strcmp(filenamefilter, "0")) {
	    ex = makeexpr_sizeof(copyexpr(nex), 0);
	    nex = makeexpr_bicall_2(filenamefilter, tp_str255, nex, ex);
	} else
	    nex = makeexpr_stringify(nex);
    }
    return nex;
}



Static Stmt *assignfilename(fex, nex)
Expr *fex, *nex;
{
    Meaning *mp;
    Expr *nvex;

    nvex = filenamepart(fex);
    if (nvex) {
        freeexpr(fex);
        return makestmt_call(makeexpr_assign(nvex, nex));
    } else {
	mp = isfilevar(fex);
        if (mp)
            warning("Don't know how to ASSIGN to a non-explicit file variable [207]");
        else
            note("Encountered an ASSIGN statement [179]");
        return makestmt_call(makeexpr_bicall_2("assign", tp_void, fex, nex));
    }
}



Static Stmt *proc_assign()
{
    Expr *fex, *nex;

    if (!skipopenparen())
	return NULL;
    fex = p_expr(tp_text);
    if (!skipcomma())
	return NULL;
    nex = checkfilename(p_expr(tp_str255));
    skipcloseparen();
    return assignfilename(fex, nex);
}



Static Stmt *handleopen(code)
int code;
{
    Stmt *sp, *sp1, *sp2, *spassign;
    Expr *fex, *nex, *ex, *truenex, *nvex;
    Meaning *fmp;
    int needcheckopen = 1;
    char modebuf[5], *cp;

    if (!skipopenparen())
	return NULL;
    fex = p_expr(tp_text);
    fmp = isfilevar(fex);
    nvex = filenamepart(fex);
    truenex = NULL;
    spassign = NULL;
    if (curtok == TOK_COMMA) {
        gettok();
        ex = p_expr(tp_str255);
    } else
        ex = NULL;
    if (ex && (ex->val.type->kind == TK_STRING ||
	       ex->val.type->kind == TK_ARRAY)) {
        nex = checkfilename(ex);
        if (nvex) {
            spassign = assignfilename(copyexpr(fex), nex);
            nex = nvex;
        }
	truenex = nex;
        if (curtok == TOK_COMMA) {
            gettok();
            ex = p_expr(tp_str255);
        } else
            ex = NULL;
    } else if (nvex) {
        nex = nvex;
    } else {
	switch (code) {
	    case 0:
	        if (ex)
		    note("Can't interpret name argument in RESET [180]");
		break;
  	    case 1:
	        note("REWRITE does not specify a name [181]");
		break;
	    case 2:
		note("OPEN does not specify a name [181]");
		break;
	    case 3:
		note("APPEND does not specify a name [181]");
		break;
	}
	nex = NULL;
    }
    if (ex) {
        if (ord_type(ex->val.type)->kind == TK_INTEGER) {
	    if (!checkconst(ex, 1))
		note("Ignoring block size in binary file [182]");
            freeexpr(ex);
        } else {
	    if (ex->kind == EK_CONST && ex->val.type->kind == TK_STRING) {
		cp = getstring(ex);
		if (strcicmp(cp, "SHARED"))
		    note(format_s("Ignoring option string \"%s\" in open [183]", cp));
	    } else
		note("Ignoring option string in open [183]");
        }
    }
    switch (code) {

        case 0:  /* reset */
            strcpy(modebuf, "r");
            break;

        case 1:  /* rewrite */
            strcpy(modebuf, "w");
            break;

        case 2:  /* open */
            strcpy(modebuf, openmode);
            break;

        case 3:  /* append */
            strcpy(modebuf, "a");
            break;

    }
    if (!*modebuf) {
        strcpy(modebuf, "r+");
    }
    if (readwriteopen == 2 ||
	(readwriteopen &&
	 fex->val.type != tp_text &&
	 fex->val.type != tp_bigtext)) {
	if (!my_strchr(modebuf, '+'))
	    strcat(modebuf, "+");
    }
    if (fex->val.type != tp_text &&
	fex->val.type != tp_bigtext &&
	binarymode != 0) {
        if (binarymode == 1)
            strcat(modebuf, "b");
        else
            note("Opening a binary file [184]");
    }
    if (!nex && fmp &&
	!is_std_file(fex) &&
	literalfilesflag > 0 &&
	(literalfilesflag == 1 ||
	 strlist_cifind(literalfiles, fmp->name))) {
	nex = makeexpr_string(fmp->name);
    }
    sp1 = NULL;
    sp2 = NULL;
    if (!nex || (isfiletype(fex->val.type, 1) && !truenex)) {
	if (isvar(fex, mp_output)) {
	    note("RESET/REWRITE ignored for file OUTPUT [319]");
	} else {
	    sp1 = makestmt_call(makeexpr_bicall_1("rewind", tp_void,
						  filebasename(copyexpr(fex))));
	    if (code == 0 || is_std_file(fex)) {
		sp1 = wrapopencheck(sp1, copyexpr(fex));
		needcheckopen = 0;
	    } else
		sp1 = makestmt_if(makeexpr_rel(EK_NE,
					       filebasename(copyexpr(fex)),
					       makeexpr_nil()),
				 sp1,
				 makestmt_assign(filebasename(copyexpr(fex)),
						 makeexpr_bicall_0("tmpfile",
								   tp_text)));
	}
    }
    if (nex || isfiletype(fex->val.type, 1)) {
	needcheckopen = 1;
	if (!strcmp(freopenname, "fclose") ||
	    !strcmp(freopenname, "fopen")) {
	    sp2 = makestmt_assign(filebasename(copyexpr(fex)),
				  makeexpr_bicall_2("fopen", tp_text,
						    copyexpr(nex),
						    makeexpr_string(modebuf)));
	    if (!strcmp(freopenname, "fclose")) {
		sp2 = makestmt_seq(makestmt_if(makeexpr_rel(EK_NE,
							    filebasename(copyexpr(fex)),
							    makeexpr_nil()),
					       makestmt_call(makeexpr_bicall_1("fclose", tp_void,
									       filebasename(copyexpr(fex)))),
					       NULL),
				   sp2);
	    }
	} else {
	    sp2 = makestmt_assign(filebasename(copyexpr(fex)),
				 makeexpr_bicall_3((*freopenname) ? freopenname : "freopen",
						   tp_text,
						   copyexpr(nex),
						   makeexpr_string(modebuf),
						   filebasename(copyexpr(fex))));
	    if (!*freopenname) {
		sp2 = makestmt_if(makeexpr_rel(EK_NE, filebasename(copyexpr(fex)),
					       makeexpr_nil()),
				  sp2,
				  makestmt_assign(filebasename(copyexpr(fex)),
						  makeexpr_bicall_2("fopen", tp_text,
								    copyexpr(nex),
								    makeexpr_string(modebuf))));
	    }
	}
    }
    if (!sp1)
	sp = sp2;
    else if (!sp2)
	sp = sp1;
    else {
	sp = makestmt_if(makeexpr_rel(EK_NE, copyexpr(nex),
				      makeexpr_string("")),
			 sp2, sp1);
    }
    if (code == 2 && !*openmode && nex) {
        sp = makestmt_seq(sp, makestmt_if(makeexpr_rel(EK_EQ,
						       filebasename(copyexpr(fex)),
						       makeexpr_nil()),
                                          makestmt_assign(filebasename(copyexpr(fex)),
                                                          makeexpr_bicall_2("fopen", tp_text,
                                                                            copyexpr(nex),
                                                                            makeexpr_string("w+"))),
                                          NULL));
    }
    if (nex)
	freeexpr(nex);
    if (FCheck(checkfileopen) && needcheckopen) {
        sp = makestmt_seq(sp, makestmt_call(makeexpr_bicall_2("~SETIO", tp_void,
                                                              makeexpr_rel(EK_NE, filebasename(copyexpr(fex)), makeexpr_nil()),
							      makeexpr_name(filenotfoundname, tp_int))));
    }
    sp = makestmt_seq(spassign, sp);
    cp = (code == 0) ? resetbufname : setupbufname;
    if (*cp &&   /* (may be eaten later, if buffering isn't needed) */
	fileisbuffered(fex, 1))
	sp = makestmt_seq(sp,
	         makestmt_call(
                     makeexpr_bicall_2(cp, tp_void, filebasename(fex),
			 makeexpr_type(filebasetype(fex->val.type)))));
    else
	freeexpr(fex);
    skipcloseparen();
    return sp;
}



Static Stmt *proc_append()
{
    return handleopen(3);
}



Static Expr *func_arccos(ex)
Expr *ex;
{
    return makeexpr_bicall_1("acos", tp_longreal, grabarg(ex, 0));
}


Static Expr *func_arcsin(ex)
Expr *ex;
{
    return makeexpr_bicall_1("asin", tp_longreal, grabarg(ex, 0));
}


Static Expr *func_arctan(ex)
Expr *ex;
{
    ex = grabarg(ex, 0);
    if (atan2flag && ex->kind == EK_DIVIDE)
        return makeexpr_bicall_2("atan2", tp_longreal, 
                                 ex->args[0], ex->args[1]);
    return makeexpr_bicall_1("atan", tp_longreal, ex);
}


Static Expr *func_arctanh(ex)
Expr *ex;
{
    return makeexpr_bicall_1("atanh", tp_longreal, grabarg(ex, 0));
}



Static Stmt *proc_argv()
{
    Expr *ex, *aex, *lex;

    if (!skipopenparen())
	return NULL;
    ex = p_expr(tp_integer);
    if (skipcomma()) {
	aex = p_expr(tp_str255);
    } else
	return NULL;
    skipcloseparen();
    lex = makeexpr_sizeof(copyexpr(aex), 0);
    aex = makeexpr_addrstr(aex);
    return makestmt_call(makeexpr_bicall_3("P_sun_argv", tp_void,
					   aex, lex, makeexpr_arglong(ex, 0)));
}


Static Expr *func_asr()
{
    Expr *ex;

    if (!skipopenparen())
	return NULL;
    ex = p_expr(tp_integer);
    if (skipcomma()) {
        if (signedshift == 0 || signedshift == 2) {
            ex = makeexpr_bicall_2("P_asr", ex->val.type, ex,
				   p_expr(tp_unsigned));
	} else {
	    ex = force_signed(ex);
	    ex = makeexpr_bin(EK_RSH, ex->val.type, ex, p_expr(tp_unsigned));
	    if (signedshift != 1)
		note("Assuming >> is an arithmetic shift [320]");
	}
	skipcloseparen();
    }
    return ex;
}


Static Expr *func_lsl()
{
    Expr *ex;

    if (!skipopenparen())
	return NULL;
    ex = p_expr(tp_integer);
    if (skipcomma()) {
	ex = makeexpr_bin(EK_LSH, ex->val.type, ex, p_expr(tp_unsigned));
	skipcloseparen();
    }
    return ex;
}


Static Expr *func_lsr()
{
    Expr *ex;

    if (!skipopenparen())
	return NULL;
    ex = p_expr(tp_integer);
    if (skipcomma()) {
	ex = force_unsigned(ex);
	ex = makeexpr_bin(EK_RSH, ex->val.type, ex, p_expr(tp_unsigned));
	skipcloseparen();
    }
    return ex;
}



Static Expr *func_bin()
{
    note("Using %b for binary printf format [185]");
    return handle_vax_hex(NULL, "b", 1);
}



Static Expr *func_binary(ex)
Expr *ex;
{
    char *cp;

    ex = grabarg(ex, 0);
    if (ex->kind == EK_CONST) {
        cp = getstring(ex);
        ex = makeexpr_long(my_strtol(cp, NULL, 2));
        insertarg(&ex, 0, makeexpr_name("%#lx", tp_integer));
        return ex;
    } else {
        return makeexpr_bicall_3("strtol", tp_integer, 
                                 ex, makeexpr_nil(), makeexpr_long(2));
    }
}



Static Expr *handle_bitsize(next)
int next;
{
    Expr *ex;
    Type *type;
    int lpar;
    long psize;

    lpar = (curtok == TOK_LPAR);
    if (lpar)
	gettok();
    if (curtok == TOK_IDENT && curtokmeaning &&
	curtokmeaning->kind == MK_TYPE) {
        ex = makeexpr_type(curtokmeaning->type);
        gettok();
    } else
        ex = p_expr(NULL);
    type = ex->val.type;
    if (lpar)
	skipcloseparen();
    psize = 0;
    packedsize(NULL, &type, &psize, 0);
    if (psize > 0 && psize < 32 && next) {
	if (psize > 16)
	    psize = 32;
	else if (psize > 8)
	    psize = 16;
	else if (psize > 4)
	    psize = 8;
	else if (psize > 2)
	    psize = 4;
	else if (psize > 1)
	    psize = 2;
	else
	    psize = 1;
    }
    if (psize)
	return makeexpr_long(psize);
    else
	return makeexpr_times(makeexpr_sizeof(ex, 0),
			      makeexpr_long(sizeof_char ? sizeof_char : 8));
}


Static Expr *func_bitsize()
{
    return handle_bitsize(0);
}


Static Expr *func_bitnext()
{
    return handle_bitsize(1);
}



Static Expr *func_blockread()
{
    Expr *ex, *ex2, *vex, *sex, *fex;
    Type *type;

    if (!skipopenparen())
	return NULL;
    fex = p_expr(tp_text);
    if (!skipcomma())
	return NULL;
    vex = p_expr(NULL);
    if (!skipcomma())
	return NULL;
    ex2 = p_expr(tp_integer);
    if (curtok == TOK_COMMA) {
        gettok();
        sex = p_expr(tp_integer);
	sex = doseek(copyexpr(fex),
		     makeexpr_times(sex, makeexpr_long(512)))->exp1;
    } else
        sex = NULL;
    skipcloseparen();
    type = vex->val.type;
    ex = makeexpr_bicall_4("fread", tp_integer,
			   makeexpr_addr(vex),
			   makeexpr_long(512),
			   convert_size(type, ex2, "BLOCKREAD"),
			   filebasename(copyexpr(fex)));
    return makeexpr_comma(sex, ex);
}



Static Expr *func_blockwrite()
{
    Expr *ex, *ex2, *vex, *sex, *fex;
    Type *type;

    if (!skipopenparen())
	return NULL;
    fex = p_expr(tp_text);
    if (!skipcomma())
	return NULL;
    vex = p_expr(NULL);
    if (!skipcomma())
	return NULL;
    ex2 = p_expr(tp_integer);
    if (curtok == TOK_COMMA) {
        gettok();
        sex = p_expr(tp_integer);
	sex = doseek(copyexpr(fex),
		     makeexpr_times(sex, makeexpr_long(512)))->exp1;
    } else
        sex = NULL;
    skipcloseparen();
    type = vex->val.type;
    ex = makeexpr_bicall_4("fwrite", tp_integer,
			   makeexpr_addr(vex),
			   makeexpr_long(512),
			   convert_size(type, ex2, "BLOCKWRITE"),
			   filebasename(copyexpr(fex)));
    return makeexpr_comma(sex, ex);
}




Static Stmt *proc_blockread()
{
    Expr *ex, *ex2, *vex, *rex, *fex;
    Type *type;

    if (!skipopenparen())
	return NULL;
    fex = p_expr(tp_text);
    if (!skipcomma())
	return NULL;
    vex = p_expr(NULL);
    if (!skipcomma())
	return NULL;
    ex2 = p_expr(tp_integer);
    if (curtok == TOK_COMMA) {
        gettok();
        rex = p_expr(tp_integer);
    } else
        rex = NULL;
    skipcloseparen();
    type = vex->val.type;
    if (rex) {
        ex = makeexpr_bicall_4("fread", tp_integer,
                               makeexpr_addr(vex),
                               makeexpr_long(1),
                               convert_size(type, ex2, "BLOCKREAD"),
                               filebasename(copyexpr(fex)));
        ex = makeexpr_assign(rex, ex);
        if (!iocheck_flag)
            ex = makeexpr_comma(ex,
                                makeexpr_assign(makeexpr_var(mp_ioresult),
                                                makeexpr_long(0)));
    } else {
        ex = makeexpr_bicall_4("fread", tp_integer,
                               makeexpr_addr(vex),
                               convert_size(type, ex2, "BLOCKREAD"),
                               makeexpr_long(1),
                               filebasename(copyexpr(fex)));
        if (checkeof(fex)) {
            ex = makeexpr_bicall_2(name_SETIO, tp_void,
                                   makeexpr_rel(EK_EQ, ex, makeexpr_long(1)),
				   makeexpr_name(endoffilename, tp_int));
        }
    }
    return wrapopencheck(makestmt_call(ex), fex);
}




Static Stmt *proc_blockwrite()
{
    Expr *ex, *ex2, *vex, *rex, *fex;
    Type *type;

    if (!skipopenparen())
	return NULL;
    fex = p_expr(tp_text);
    if (!skipcomma())
	return NULL;
    vex = p_expr(NULL);
    if (!skipcomma())
	return NULL;
    ex2 = p_expr(tp_integer);
    if (curtok == TOK_COMMA) {
        gettok();
        rex = p_expr(tp_integer);
    } else
        rex = NULL;
    skipcloseparen();
    type = vex->val.type;
    if (rex) {
        ex = makeexpr_bicall_4("fwrite", tp_integer,
                               makeexpr_addr(vex),
                               makeexpr_long(1),
                               convert_size(type, ex2, "BLOCKWRITE"),
                               filebasename(copyexpr(fex)));
        ex = makeexpr_assign(rex, ex);
        if (!iocheck_flag)
            ex = makeexpr_comma(ex,
                                makeexpr_assign(makeexpr_var(mp_ioresult),
                                                makeexpr_long(0)));
    } else {
        ex = makeexpr_bicall_4("fwrite", tp_integer,
                               makeexpr_addr(vex),
                               convert_size(type, ex2, "BLOCKWRITE"),
                               makeexpr_long(1),
                               filebasename(copyexpr(fex)));
        if (FCheck(checkfilewrite)) {
            ex = makeexpr_bicall_2(name_SETIO, tp_void,
                                   makeexpr_rel(EK_EQ, ex, makeexpr_long(1)),
				   makeexpr_name(filewriteerrorname, tp_int));
        }
    }
    return wrapopencheck(makestmt_call(ex), fex);
}



Static Stmt *proc_bclr()
{
    Expr *ex, *ex2;

    if (!skipopenparen())
	return NULL;
    ex = p_expr(tp_integer);
    if (!skipcomma())
	return NULL;
    ex2 = p_expr(tp_integer);
    skipcloseparen();
    return makestmt_assign(ex,
			   makeexpr_bin(EK_BAND, ex->val.type,
					copyexpr(ex),
					makeexpr_un(EK_BNOT, ex->val.type,
					makeexpr_bin(EK_LSH, tp_integer,
						     makeexpr_arglong(
						         makeexpr_long(1), 1),
						     ex2))));
}



Static Stmt *proc_bset()
{
    Expr *ex, *ex2;

    if (!skipopenparen())
	return NULL;
    ex = p_expr(tp_integer);
    if (!skipcomma())
	return NULL;
    ex2 = p_expr(tp_integer);
    skipcloseparen();
    return makestmt_assign(ex,
			   makeexpr_bin(EK_BOR, ex->val.type,
					copyexpr(ex),
					makeexpr_bin(EK_LSH, tp_integer,
						     makeexpr_arglong(
						         makeexpr_long(1), 1),
						     ex2)));
}



Static Expr *func_bsl()
{
    Expr *ex, *ex2;

    if (!skipopenparen())
	return NULL;
    ex = p_expr(tp_integer);
    if (!skipcomma())
	return NULL;
    ex2 = p_expr(tp_integer);
    skipcloseparen();
    return makeexpr_bin(EK_LSH, tp_integer, ex, ex2);
}



Static Expr *func_bsr()
{
    Expr *ex, *ex2;

    if (!skipopenparen())
	return NULL;
    ex = p_expr(tp_integer);
    if (!skipcomma())
	return NULL;
    ex2 = p_expr(tp_integer);
    skipcloseparen();
    return makeexpr_bin(EK_RSH, tp_integer, force_unsigned(ex), ex2);
}



Static Expr *func_btst()
{
    Expr *ex, *ex2;

    if (!skipopenparen())
	return NULL;
    ex = p_expr(tp_integer);
    if (!skipcomma())
	return NULL;
    ex2 = p_expr(tp_integer);
    skipcloseparen();
    return makeexpr_rel(EK_NE,
			makeexpr_bin(EK_BAND, tp_integer,
				     ex,
				     makeexpr_bin(EK_LSH, tp_integer,
						  makeexpr_arglong(
						      makeexpr_long(1), 1),
						  ex2)),
			makeexpr_long(0));
}



Static Expr *func_byteread()
{
    Expr *ex, *ex2, *vex, *sex, *fex;
    Type *type;

    if (!skipopenparen())
	return NULL;
    fex = p_expr(tp_text);
    if (!skipcomma())
	return NULL;
    vex = p_expr(NULL);
    if (!skipcomma())
	return NULL;
    ex2 = p_expr(tp_integer);
    if (curtok == TOK_COMMA) {
        gettok();
        sex = p_expr(tp_integer);
	sex = doseek(copyexpr(fex), sex)->exp1;
    } else
        sex = NULL;
    skipcloseparen();
    type = vex->val.type;
    ex = makeexpr_bicall_4("fread", tp_integer,
			   makeexpr_addr(vex),
			   makeexpr_long(1),
			   convert_size(type, ex2, "BYTEREAD"),
			   filebasename(copyexpr(fex)));
    return makeexpr_comma(sex, ex);
}



Static Expr *func_bytewrite()
{
    Expr *ex, *ex2, *vex, *sex, *fex;
    Type *type;

    if (!skipopenparen())
	return NULL;
    fex = p_expr(tp_text);
    if (!skipcomma())
	return NULL;
    vex = p_expr(NULL);
    if (!skipcomma())
	return NULL;
    ex2 = p_expr(tp_integer);
    if (curtok == TOK_COMMA) {
        gettok();
        sex = p_expr(tp_integer);
	sex = doseek(copyexpr(fex), sex)->exp1;
    } else
        sex = NULL;
    skipcloseparen();
    type = vex->val.type;
    ex = makeexpr_bicall_4("fwrite", tp_integer,
			   makeexpr_addr(vex),
			   makeexpr_long(1),
			   convert_size(type, ex2, "BYTEWRITE"),
			   filebasename(copyexpr(fex)));
    return makeexpr_comma(sex, ex);
}



Static Expr *func_byte_offset()
{
    Type *tp;
    Meaning *mp;
    Expr *ex;

    if (!skipopenparen())
	return NULL;
    tp = p_type(NULL);
    if (!skipcomma())
	return NULL;
    if (!wexpecttok(TOK_IDENT))
	return NULL;
    mp = curtoksym->fbase;
    while (mp && mp->rectype != tp)
	mp = mp->snext;
    if (!mp)
	ex = makeexpr_name(curtokcase, tp_integer);
    else
	ex = makeexpr_name(mp->name, tp_integer);
    gettok();
    skipcloseparen();
    return makeexpr_bicall_2("OFFSETOF", (size_t_long) ? tp_integer : tp_int,
			     makeexpr_type(tp), ex);
}



Static Stmt *proc_call()
{
    Expr *ex, *ex2, *ex3;
    Type *type, *tp;
    Meaning *mp;

    if (!skipopenparen())
	return NULL;
    ex2 = p_expr(tp_proc);
    type = ex2->val.type;
    if (type->kind != TK_PROCPTR && type->kind != TK_CPROCPTR) {
        warning("CALL requires a procedure variable [208]");
	type = tp_proc;
    }
    ex = makeexpr(EK_SPCALL, 1);
    ex->val.type = tp_void;
    ex->args[0] = copyexpr(ex2);
    if (type->escale != 0)
	ex->args[0] = makeexpr_cast(makeexpr_dotq(ex2, "proc", tp_anyptr),
				    makepointertype(type->basetype));
    mp = type->basetype->fbase;
    if (mp) {
        if (wneedtok(TOK_COMMA))
	    ex = p_funcarglist(ex, mp, 0, 0);
    }
    skipcloseparen();
    if (type->escale != 1 || hasstaticlinks == 2) {
	freeexpr(ex2);
	return makestmt_call(ex);
    }
    ex2 = makeexpr_dotq(ex2, "link", tp_anyptr),
    ex3 = copyexpr(ex);
    insertarg(&ex3, ex3->nargs, copyexpr(ex2));
    tp = maketype(TK_FUNCTION);
    tp->basetype = type->basetype->basetype;
    tp->fbase = type->basetype->fbase;
    tp->issigned = 1;
    ex3->args[0]->val.type = makepointertype(tp);
    return makestmt_if(makeexpr_rel(EK_NE, ex2, makeexpr_nil()),
                       makestmt_call(ex3),
                       makestmt_call(ex));
}



Static Expr *func_chr()
{
    Expr *ex;

    ex = p_expr(tp_integer);
    if ((exprlongness(ex) < 0 || ex->kind == EK_CAST) && ex->kind != EK_ACTCAST)
        ex->val.type = tp_char;
    else
        ex = makeexpr_cast(ex, tp_char);
    return ex;
}



Static Stmt *proc_close()
{
    Stmt *sp;
    Expr *fex, *ex;
    char *opt;

    if (!skipopenparen())
	return NULL;
    fex = p_expr(tp_text);
    sp = makestmt_if(makeexpr_rel(EK_NE, filebasename(copyexpr(fex)),
				  makeexpr_nil()),
                     makestmt_call(makeexpr_bicall_1("fclose", tp_void,
                                                     filebasename(copyexpr(fex)))),
                     (FCheck(checkfileisopen))
		         ? makestmt_call(
			     makeexpr_bicall_1(name_ESCIO,
					       tp_integer,
					       makeexpr_name(filenotopenname,
							     tp_int)))
                         : NULL);
    if (curtok == TOK_COMMA) {
        gettok();
	opt = "";
	if (curtok == TOK_IDENT &&
	    (!strcicmp(curtokbuf, "LOCK") ||
	     !strcicmp(curtokbuf, "PURGE") ||
	     !strcicmp(curtokbuf, "NORMAL") ||
	     !strcicmp(curtokbuf, "CRUNCH"))) {
	    opt = stralloc(curtokbuf);
	    gettok();
	} else {
	    ex = p_expr(tp_str255);
	    if (ex->kind == EK_CONST && ex->val.type->kind == TK_STRING)
		opt = ex->val.s;
	}
	if (!strcicmp(opt, "PURGE")) {
	    note("File is being closed with PURGE option [186]");
        }
    }
    sp = makestmt_seq(sp, makestmt_assign(filebasename(fex), makeexpr_nil()));
    skipcloseparen();
    return sp;
}



Static Expr *func_concat()
{
    Expr *ex;

    if (!skipopenparen())
	return makeexpr_string("oops");
    ex = p_expr(tp_str255);
    while (curtok == TOK_COMMA) {
        gettok();
        ex = makeexpr_concat(ex, p_expr(tp_str255), 0);
    }
    skipcloseparen();
    return ex;
}



Static Expr *func_copy(ex)
Expr *ex;
{
    if (isliteralconst(ex->args[3], NULL) == 2 &&
        ex->args[3]->val.i >= stringceiling) {
        return makeexpr_bicall_3("sprintf", ex->val.type,
                                 ex->args[0],
                                 makeexpr_string("%s"),
                                 bumpstring(ex->args[1], 
                                            makeexpr_unlongcast(ex->args[2]), 1));
    }
    if (checkconst(ex->args[2], 1)) {
        return makeexpr_addr(makeexpr_substring(ex->args[0], ex->args[1], 
                                                ex->args[2], ex->args[3]));
    }
    return makeexpr_bicall_4(strsubname, ex->val.type,
                             ex->args[0],
                             ex->args[1],
                             makeexpr_arglong(ex->args[2], 0),
                             makeexpr_arglong(ex->args[3], 0));
}



Static Expr *func_cos(ex)
Expr *ex;
{
    return makeexpr_bicall_1("cos", tp_longreal, grabarg(ex, 0));
}


Static Expr *func_cosh(ex)
Expr *ex;
{
    return makeexpr_bicall_1("cosh", tp_longreal, grabarg(ex, 0));
}



Static Stmt *proc_cycle()
{
    return makestmt(SK_CONTINUE);
}



Static Stmt *proc_date()
{
    Expr *ex;

    if (!skipopenparen())
	return NULL;
    ex = p_expr(tp_str255);
    skipcloseparen();
    return makestmt_call(makeexpr_bicall_1("VAXdate", tp_integer, ex));
}


Static Stmt *proc_dec()
{
    Expr *vex, *ex;

    if (!skipopenparen())
	return NULL;
    vex = p_expr(NULL);
    if (curtok == TOK_COMMA) {
        gettok();
        ex = p_expr(tp_integer);
    } else
        ex = makeexpr_long(1);
    skipcloseparen();
    return makestmt_assign(vex, makeexpr_minus(copyexpr(vex), ex));
}



Static Expr *func_dec()
{
    return handle_vax_hex(NULL, "d", 0);
}



Static Stmt *proc_delete(ex)
Expr *ex;
{
    if (ex->nargs == 1)   /* Kludge for Oregon Software Pascal's delete(f) */
	return makestmt_call(makeexpr_bicall_1(strdeletename, tp_void, ex->args[0]));
    return makestmt_call(makeexpr_bicall_3(strdeletename, tp_void,
                                           ex->args[0], 
                                           makeexpr_arglong(ex->args[1], 0),
                                           makeexpr_arglong(ex->args[2], 0)));
}



void parse_special_variant(tp, buf)
Type *tp;
char *buf;
{
    char *cp;
    Expr *ex;

    if (!tp)
	intwarning("parse_special_variant", "tp == NULL");
    if (!tp || tp->meaning == NULL) {
	*buf = 0;
	if (curtok == TOK_COMMA) {
	    skiptotoken(TOK_RPAR);
	}
	return;
    }
    strcpy(buf, tp->meaning->name);
    while (curtok == TOK_COMMA) {
	gettok();
	cp = buf + strlen(buf);
	*cp++ = '.';
	if (curtok == TOK_MINUS) {
	    *cp++ = '-';
	    gettok();
	}
	if (curtok == TOK_INTLIT ||
	    curtok == TOK_HEXLIT ||
	    curtok == TOK_OCTLIT) {
	    sprintf(cp, "%ld", curtokint);
	    gettok();
	} else if (curtok == TOK_HAT || curtok == TOK_STRLIT) {
	    ex = makeexpr_charcast(accumulate_strlit());
	    if (ex->kind == EK_CONST) {
		if (ex->val.i <= 32 || ex->val.i > 126 ||
		    ex->val.i == '\'' || ex->val.i == '\\' ||
		    ex->val.i == '=' || ex->val.i == '}')
		    sprintf(cp, "%ld", ex->val.i);
		else
		    strcpy(cp, makeCchar(ex->val.i));
	    } else {
		*buf = 0;
		*cp = 0;
	    }
	    freeexpr(ex);
	} else {
	    if (!wexpecttok(TOK_IDENT)) {
		skiptotoken(TOK_RPAR);
		return;
	    }
	    if (curtokmeaning)
		strcpy(cp, curtokmeaning->name);
	    else
		strcpy(cp, curtokbuf);
	    gettok();
	}
    }
}


char *find_special_variant(buf, spname, splist, need)
char *buf, *spname;
Strlist *splist;
int need;
{
    Strlist *best = NULL;
    int len, bestlen = -1;
    char *cp, *cp2;

    if (!*buf)
	return NULL;
    while (splist) {
	cp = splist->s;
	cp2 = buf;
	while (*cp && toupper(*cp) == toupper(*cp2))
	    cp++, cp2++;
	len = cp2 - buf;
	if (!*cp && (!*cp2 || *cp2 == '.') && len > bestlen) {
	    best = splist;
	    bestlen = len;
	}
	splist = splist->next;
    }
    if (bestlen != strlen(buf) && my_strchr(buf, '.')) {
	if ((need & 1) || bestlen >= 0) {
	    if (need & 2)
		return NULL;
	    if (spname)
		note(format_ss("No %s form known for %s [187]",
			       spname, strupper(buf)));
	}
    }
    if (bestlen >= 0)
	return (char *)best->value;
    else
	return NULL;
}



Static char *choose_free_func(ex)
Expr *ex;
{
    if (!*freename) {
	if (!*freervaluename)
	    return "free";
	else
	    return freervaluename;
    }
    if (!*freervaluename)
	return freervaluename;
    if (expr_is_lvalue(ex))
	return freename;
    else
	return freervaluename;
}


Static Stmt *proc_dispose()
{
    Expr *ex;
    Type *type;
    char *name, vbuf[1000];

    if (!skipopenparen())
	return NULL;
    ex = p_expr(tp_anyptr);
    type = ex->val.type->basetype;
    parse_special_variant(type, vbuf);
    skipcloseparen();
    name = find_special_variant(vbuf, "SpecialFree", specialfrees, 0);
    if (!name)
	name = choose_free_func(ex);
    return makestmt_call(makeexpr_bicall_1(name, tp_void, ex));
}



Static Expr *func_exp(ex)
Expr *ex;
{
    return makeexpr_bicall_1("exp", tp_longreal, grabarg(ex, 0));
}



Static Expr *func_expo(ex)
Expr *ex;
{
    Meaning *tvar;

    tvar = makestmttempvar(tp_int, name_TEMP);
    return makeexpr_comma(makeexpr_bicall_2("frexp", tp_longreal,
					    grabarg(ex, 0),
					    makeexpr_addr(makeexpr_var(tvar))),
			  makeexpr_var(tvar));
}



int is_std_file(ex)
Expr *ex;
{
    return isvar(ex, mp_input) || isvar(ex, mp_output) ||
           isvar(ex, mp_stderr);
}



Static Expr *iofunc(ex, code)
Expr *ex;
int code;
{
    Expr *ex2 = NULL, *ex3 = NULL;
    Meaning *tvar = NULL;

    if (FCheck(checkfileisopen) && !is_std_file(ex)) {
        if (isfiletype(ex->val.type, 1) ||
	    (exprspeed(ex) < 5 && nosideeffects(ex, 0))) {
            ex2 = filebasename(copyexpr(ex));
	} else {
            ex3 = ex;
            tvar = makestmttempvar(ex->val.type, name_TEMP);
            ex2 = makeexpr_var(tvar);
            ex = makeexpr_var(tvar);
        }
    }
    ex = filebasename(ex);
    switch (code) {

        case 0:  /* eof */
            if (fileisbuffered(ex, 0) && *eofbufname)
		ex = makeexpr_bicall_1(eofbufname, tp_boolean, ex);
	    else if (*eofname)
		ex = makeexpr_bicall_1(eofname, tp_boolean, ex);
	    else
		ex = makeexpr_rel(EK_NE, makeexpr_bicall_1("feof", tp_int, ex),
				         makeexpr_long(0));
            break;

        case 1:  /* eoln */
            ex = makeexpr_bicall_1(eolnname, tp_boolean, ex);
            break;

        case 2:  /* position or filepos */
            if (fileisbuffered(ex, 0) && *fileposbufname)
		ex = makeexpr_bicall_1(fileposbufname, tp_integer, ex);
	    else
		ex = makeexpr_bicall_1(fileposname, tp_integer, ex);
            break;

        case 3:  /* maxpos or filesize */
            ex = makeexpr_bicall_1(maxposname, tp_integer, ex);
            break;

    }
    if (ex2) {
        ex = makeexpr_bicall_4("~CHKIO",
                               (code == 0 || code == 1) ? tp_boolean : tp_integer,
                               makeexpr_rel(EK_NE, ex2, makeexpr_nil()),
			       makeexpr_name("FileNotOpen", tp_int),
                               ex, makeexpr_long(0));
    }
    if (ex3)
        ex = makeexpr_comma(makeexpr_assign(makeexpr_var(tvar), ex3), ex);
    return ex;
}



Static Expr *func_eof()
{
    Expr *ex;

    if (curtok == TOK_LPAR)
        ex = p_parexpr(tp_text);
    else
        ex = makeexpr_var(mp_input);
    return iofunc(ex, 0);
}



Static Expr *func_eoln()
{
    Expr *ex;

    if (curtok == TOK_LPAR)
        ex = p_parexpr(tp_text);
    else
        ex = makeexpr_var(mp_input);
    return iofunc(ex, 1);
}



Static Stmt *proc_escape()
{
    Expr *ex;

    if (curtok == TOK_LPAR)
        ex = p_parexpr(tp_integer);
    else
        ex = makeexpr_long(0);
    return makestmt_call(makeexpr_bicall_1(name_ESCAPE, tp_int, 
                                           makeexpr_arglong(ex, 0)));
}



Static Stmt *proc_excl()
{
    Expr *vex, *ex;

    if (!skipopenparen())
	return NULL;
    vex = p_expr(NULL);
    if (!skipcomma())
	return NULL;
    ex = p_expr(vex->val.type->indextype);
    skipcloseparen();
    if (vex->val.type->kind == TK_SMALLSET)
	return makestmt_assign(vex, makeexpr_bin(EK_BAND, vex->val.type,
						 copyexpr(vex),
						 makeexpr_un(EK_BNOT, vex->val.type,
							     makeexpr_bin(EK_LSH, vex->val.type,
									  makeexpr_longcast(makeexpr_long(1), 1),
									  ex))));
    else
	return makestmt_call(makeexpr_bicall_2(setremname, tp_void, vex,
					       makeexpr_arglong(enum_to_int(ex), 0)));
}



Stmt *proc_exit()
{
    Stmt *sp;

    if (modula2) {
	return makestmt(SK_BREAK);
    }
    if (curtok == TOK_LPAR) {
        gettok();
	if (curtok == TOK_PROGRAM ||
	    (curtok == TOK_IDENT && curtokmeaning->kind == MK_MODULE)) {
	    gettok();
	    skipcloseparen();
	    return makestmt_call(makeexpr_bicall_1("exit", tp_void,
						   makeexpr_name("EXIT_SUCCESS",
								 tp_integer)));
	}
        if (curtok != TOK_IDENT || !curtokmeaning || curtokmeaning != curctx)
            note("Attempting to EXIT beyond this function [188]");
        gettok();
	skipcloseparen();
    }
    sp = makestmt(SK_RETURN);
    if (curctx->kind == MK_FUNCTION && curctx->isfunction) {
        sp->exp1 = makeexpr_var(curctx->cbase);
        curctx->cbase->refcount++;
    }
    return sp;
}



Static Expr *file_iofunc(code, base)
int code;
long base;
{
    Expr *ex;
    Type *basetype;

    if (curtok == TOK_LPAR)
	ex = p_parexpr(tp_text);
    else
	ex = makeexpr_var(mp_input);
    if (!ex->val.type || !ex->val.type->basetype ||
	!filebasetype(ex->val.type))
	basetype = tp_char;
    else
	basetype = filebasetype(ex->val.type);
    return makeexpr_plus(makeexpr_div(iofunc(ex, code),
                                      makeexpr_sizeof(makeexpr_type(basetype), 0)),
                         makeexpr_long(base));
}



Static Expr *func_fcall()
{
    Expr *ex, *ex2, *ex3;
    Type *type, *tp;
    Meaning *mp, *tvar = NULL;
    int firstarg = 0;

    if (!skipopenparen())
	return NULL;
    ex2 = p_expr(tp_proc);
    type = ex2->val.type;
    if (type->kind != TK_PROCPTR && type->kind != TK_CPROCPTR) {
        warning("FCALL requires a function variable [209]");
	type = tp_proc;
    }
    ex = makeexpr(EK_SPCALL, 1);
    ex->val.type = type->basetype->basetype;
    ex->args[0] = copyexpr(ex2);
    if (type->escale != 0)
	ex->args[0] = makeexpr_cast(makeexpr_dotq(ex2, "proc", tp_anyptr),
				    makepointertype(type->basetype));
    mp = type->basetype->fbase;
    if (mp && mp->isreturn) {    /* pointer to buffer for return value */
        tvar = makestmttempvar(ex->val.type->basetype,
            (ex->val.type->basetype->kind == TK_STRING) ? name_STRING : name_TEMP);
        insertarg(&ex, 1, makeexpr_addr(makeexpr_var(tvar)));
        mp = mp->xnext;
	firstarg++;
    }
    if (mp) {
        if (wneedtok(TOK_COMMA))
	    ex = p_funcarglist(ex, mp, 0, 0);
    }
    if (tvar)
	ex = makeexpr_hat(ex, 0);    /* returns pointer to structured result */
    skipcloseparen();
    if (type->escale != 1 || hasstaticlinks == 2) {
	freeexpr(ex2);
	return ex;
    }
    ex2 = makeexpr_dotq(ex2, "link", tp_anyptr),
    ex3 = copyexpr(ex);
    insertarg(&ex3, ex3->nargs, copyexpr(ex2));
    tp = maketype(TK_FUNCTION);
    tp->basetype = type->basetype->basetype;
    tp->fbase = type->basetype->fbase;
    tp->issigned = 1;
    ex3->args[0]->val.type = makepointertype(tp);
    return makeexpr_cond(makeexpr_rel(EK_NE, ex2, makeexpr_nil()),
			 ex3, ex);
}



Static Expr *func_filepos()
{
    return file_iofunc(2, seek_base);
}



Static Expr *func_filesize()
{
    return file_iofunc(3, 1L);
}



Static Stmt *proc_fillchar()
{
    Expr *vex, *ex, *cex;

    if (!skipopenparen())
	return NULL;
    vex = gentle_cast(makeexpr_addr(p_expr(NULL)), tp_anyptr);
    if (!skipcomma())
	return NULL;
    ex = convert_size(argbasetype(vex), p_expr(tp_integer), "FILLCHAR");
    if (!skipcomma())
	return NULL;
    cex = makeexpr_charcast(p_expr(tp_integer));
    skipcloseparen();
    return makestmt_call(makeexpr_bicall_3("memset", tp_void,
                                           vex,
                                           makeexpr_arglong(cex, 0),
                                           makeexpr_arglong(ex, (size_t_long != 0))));
}



Static Expr *func_sngl()
{
    Expr *ex;

    ex = p_parexpr(tp_real);
    return makeexpr_cast(ex, tp_real);
}



Static Expr *func_float()
{
    Expr *ex;

    ex = p_parexpr(tp_longreal);
    return makeexpr_cast(ex, tp_longreal);
}



Static Stmt *proc_flush()
{
    Expr *ex;
    Stmt *sp;

    ex = p_parexpr(tp_text);
    sp = makestmt_call(makeexpr_bicall_1("fflush", tp_void, filebasename(ex)));
    if (iocheck_flag)
        sp = makestmt_seq(sp, makestmt_assign(makeexpr_var(mp_ioresult), 
                                              makeexpr_long(0)));
    return sp;
}



Static Expr *func_frac(ex)
Expr *ex;
{
    Meaning *tvar;

    tvar = makestmttempvar(tp_longreal, name_DUMMY);
    return makeexpr_bicall_2("modf", tp_longreal, 
                             grabarg(ex, 0),
                             makeexpr_addr(makeexpr_var(tvar)));
}



Static Stmt *proc_freemem(ex)
Expr *ex;
{
    Stmt *sp;
    Expr *vex;

    vex = makeexpr_hat(eatcasts(ex->args[0]), 0);
    sp = makestmt_call(makeexpr_bicall_1(choose_free_func(vex),
					 tp_void, copyexpr(vex)));
    if (alloczeronil) {
        sp = makestmt_if(makeexpr_rel(EK_NE, vex, makeexpr_nil()),
                         sp, NULL);
    } else
        freeexpr(vex);
    return sp;
}



Static Stmt *proc_get()
{
    Expr *ex;
    Type *type;

    if (curtok == TOK_LPAR)
	ex = p_parexpr(tp_text);
    else
	ex = makeexpr_var(mp_input);
    requirefilebuffer(ex);
    type = ex->val.type;
    if (isfiletype(type, -1) && *chargetname &&
	filebasetype(type)->kind == TK_CHAR)
	return makestmt_call(makeexpr_bicall_1(chargetname, tp_void,
					       filebasename(ex)));
    else if (isfiletype(type, -1) && *arraygetname &&
	     filebasetype(type)->kind == TK_ARRAY)
	return makestmt_call(makeexpr_bicall_2(arraygetname, tp_void,
					       filebasename(ex),
					       makeexpr_type(filebasetype(type))));
    else
	return makestmt_call(makeexpr_bicall_2(getname, tp_void,
					       filebasename(ex),
					       makeexpr_type(filebasetype(type))));
}



Static Stmt *proc_getmem(ex)
Expr *ex;
{
    Expr *vex, *ex2, *sz = NULL;
    Stmt *sp;

    vex = makeexpr_hat(eatcasts(ex->args[0]), 0);
    ex2 = ex->args[1];
    if (vex->val.type->kind == TK_POINTER)
        ex2 = convert_size(vex->val.type->basetype, ex2, "GETMEM");
    if (alloczeronil)
        sz = copyexpr(ex2);
    ex2 = makeexpr_bicall_1(mallocname, tp_anyptr, ex2);
    sp = makestmt_assign(copyexpr(vex), ex2);
    if (malloccheck) {
        sp = makestmt_seq(sp, makestmt_if(makeexpr_rel(EK_EQ, copyexpr(vex), makeexpr_nil()),
                                          makestmt_call(makeexpr_bicall_0(name_OUTMEM, tp_int)),
                                          NULL));
    }
    if (sz && !isconstantexpr(sz)) {
        if (alloczeronil == 2)
            note("Called GETMEM with variable argument [189]");
        sp = makestmt_if(makeexpr_rel(EK_NE, sz, makeexpr_long(0)),
                         sp,
                         makestmt_assign(vex, makeexpr_nil()));
    } else
        freeexpr(vex);
    return sp;
}



Static Stmt *proc_gotoxy(ex)
Expr *ex;
{
    return makestmt_call(makeexpr_bicall_2("gotoxy", tp_void,
                                           makeexpr_arglong(ex->args[0], 0),
                                           makeexpr_arglong(ex->args[1], 0)));
}



Static Expr *handle_vax_hex(ex, fmt, scale)
Expr *ex;
char *fmt;
int scale;
{
    Expr *lex, *dex, *vex;
    Meaning *tvar;
    Type *tp;
    long smin, smax;
    int bits;

    if (!ex) {
	if (!skipopenparen())
	    return NULL;
	ex = p_expr(tp_integer);
    }
    tp = true_type(ex);
    if (ord_range(tp, &smin, &smax))
	bits = typebits(smin, smax);
    else
	bits = 32;
    if (curtok == TOK_COMMA) {
	gettok();
	if (curtok != TOK_COMMA)
	    lex = makeexpr_arglong(p_expr(tp_integer), 0);
	else
	    lex = NULL;
    } else
	lex = NULL;
    if (!lex) {
	if (!scale)
	    lex = makeexpr_long(11);
	else
	    lex = makeexpr_long((bits+scale-1) / scale + 1);
    }
    if (curtok == TOK_COMMA) {
	gettok();
	dex = makeexpr_arglong(p_expr(tp_integer), 0);
    } else {
	if (!scale)
	    dex = makeexpr_long(10);
	else
	    dex = makeexpr_long((bits+scale-1) / scale);
    }
    if (lex->kind == EK_CONST && dex->kind == EK_CONST &&
	lex->val.i < dex->val.i)
	lex = NULL;
    skipcloseparen();
    tvar = makestmttempvar(tp_str255, name_STRING);
    vex = makeexpr_var(tvar);
    ex = makeexpr_forcelongness(ex);
    if (exprlongness(ex) > 0)
	fmt = format_s("l%s", fmt);
    if (checkconst(lex, 0) || checkconst(lex, 1))
	lex = NULL;
    if (checkconst(dex, 0) || checkconst(dex, 1))
	dex = NULL;
    if (lex) {
	if (dex)
	    ex = makeexpr_bicall_5("sprintf", tp_str255, vex,
				   makeexpr_string(format_s("%%*.*%s", fmt)),
				   lex, dex, ex);
	else
	    ex = makeexpr_bicall_4("sprintf", tp_str255, vex,
				   makeexpr_string(format_s("%%*%s", fmt)),
				   lex, ex);
    } else {
	if (dex)
	    ex = makeexpr_bicall_4("sprintf", tp_str255, vex,
				   makeexpr_string(format_s("%%.*%s", fmt)),
				   dex, ex);
	else
	    ex = makeexpr_bicall_3("sprintf", tp_str255, vex,
				   makeexpr_string(format_s("%%%s", fmt)),
				   ex);
    }
    return ex;
}




Static Expr *func_hex()
{
    Expr *ex;
    char *cp;

    if (!skipopenparen())
	return NULL;
    ex = makeexpr_stringcast(p_expr(tp_integer));
    if ((ex->val.type->kind == TK_STRING ||
	 ex->val.type == tp_strptr) &&
	curtok != TOK_COMMA) {
	skipcloseparen();
	if (ex->kind == EK_CONST) {    /* HP Pascal */
	    cp = getstring(ex);
	    ex = makeexpr_long(my_strtol(cp, NULL, 16));
	    insertarg(&ex, 0, makeexpr_name("%#lx", tp_integer));
	    return ex;
	} else {
	    return makeexpr_bicall_3("strtol", tp_integer, 
				     ex, makeexpr_nil(), makeexpr_long(16));
	}
    } else {    /* VAX Pascal */
	return handle_vax_hex(ex, "x", 4);
    }
}



Static Expr *func_hi()
{
    Expr *ex;

    ex = force_unsigned(p_parexpr(tp_integer));
    return makeexpr_bin(EK_RSH, tp_ubyte,
                        ex, makeexpr_long(8));
}



Static Expr *func_high()
{
    Expr *ex;
    Type *type;

    ex = p_parexpr(tp_integer);
    type = ex->val.type;
    if (type->kind == TK_POINTER)
	type = type->basetype;
    if (type->kind == TK_ARRAY ||
	type->kind == TK_SMALLARRAY) {
	ex = makeexpr_minus(copyexpr(type->indextype->smax),
			    copyexpr(type->indextype->smin));
    } else {
	warning("HIGH requires an array name parameter [210]");
	ex = makeexpr_bicall_1("HIGH", tp_int, ex);
    }
    return ex;
}



Static Expr *func_hiword()
{
    Expr *ex;

    ex = force_unsigned(p_parexpr(tp_unsigned));
    return makeexpr_bin(EK_RSH, tp_unsigned,
                        ex, makeexpr_long(16));
}



Static Stmt *proc_inc()
{
    Expr *vex, *ex;

    if (!skipopenparen())
	return NULL;
    vex = p_expr(NULL);
    if (curtok == TOK_COMMA) {
        gettok();
        ex = p_expr(tp_integer);
    } else
        ex = makeexpr_long(1);
    skipcloseparen();
    return makestmt_assign(vex, makeexpr_plus(copyexpr(vex), ex));
}



Static Stmt *proc_incl()
{
    Expr *vex, *ex;

    if (!skipopenparen())
	return NULL;
    vex = p_expr(NULL);
    if (!skipcomma())
	return NULL;
    ex = p_expr(vex->val.type->indextype);
    skipcloseparen();
    if (vex->val.type->kind == TK_SMALLSET)
	return makestmt_assign(vex, makeexpr_bin(EK_BOR, vex->val.type,
						 copyexpr(vex),
						 makeexpr_bin(EK_LSH, vex->val.type,
							      makeexpr_longcast(makeexpr_long(1), 1),
							      ex)));
    else
	return makestmt_call(makeexpr_bicall_2(setaddname, tp_void, vex,
					       makeexpr_arglong(enum_to_int(ex), 0)));
}



Static Stmt *proc_insert(ex)
Expr *ex;
{
    return makestmt_call(makeexpr_bicall_3(strinsertname, tp_void,
                                           ex->args[0], 
                                           ex->args[1],
                                           makeexpr_arglong(ex->args[2], 0)));
}



Static Expr *func_int()
{
    Expr *ex;
    Meaning *tvar;

    ex = p_parexpr(tp_integer);
    if (ex->val.type->kind == TK_REAL) {    /* Turbo Pascal INT */
	tvar = makestmttempvar(tp_longreal, name_TEMP);
	return makeexpr_comma(makeexpr_bicall_2("modf", tp_longreal,
						grabarg(ex, 0),
						makeexpr_addr(makeexpr_var(tvar))),
			      makeexpr_var(tvar));
    } else {     /* VAX Pascal INT */
	return makeexpr_ord(ex);
    }
}


Static Expr *func_uint()
{
    Expr *ex;

    ex = p_parexpr(tp_integer);
    return makeexpr_cast(ex, tp_unsigned);
}



Static Stmt *proc_leave()
{
    return makestmt(SK_BREAK);
}



Static Expr *func_lo()
{
    Expr *ex;

    ex = gentle_cast(p_parexpr(tp_integer), tp_ushort);
    return makeexpr_bin(EK_BAND, tp_ubyte,
                        ex, makeexpr_long(255));
}


Static Expr *func_loophole()
{
    Type *type;
    Expr *ex;

    if (!skipopenparen())
	return NULL;
    type = p_type(NULL);
    if (!skipcomma())
	return NULL;
    ex = p_expr(tp_integer);
    skipcloseparen();
    return pascaltypecast(type, ex);
}



Static Expr *func_lower()
{
    Expr *ex;
    Value val;

    if (!skipopenparen())
	return NULL;
    ex = p_expr(tp_integer);
    if (curtok == TOK_COMMA) {
	gettok();
	val = p_constant(tp_integer);
	if (!val.type || val.i != 1)
	    note("LOWER(v,n) not supported for n>1 [190]");
    }
    skipcloseparen();
    return copyexpr(ex->val.type->indextype->smin);
}



Static Expr *func_loword()
{
    Expr *ex;

    ex = p_parexpr(tp_integer);
    return makeexpr_bin(EK_BAND, tp_ushort,
                        ex, makeexpr_long(65535));
}



Static Expr *func_ln(ex)
Expr *ex;
{
    return makeexpr_bicall_1("log", tp_longreal, grabarg(ex, 0));
}



Static Expr *func_log(ex)
Expr *ex;
{
    return makeexpr_bicall_1("log10", tp_longreal, grabarg(ex, 0));
}



Static Expr *func_max()
{
    Type *tp;
    Expr *ex, *ex2;

    if (!skipopenparen())
	return NULL;
    if (curtok == TOK_IDENT && curtokmeaning &&
	curtokmeaning->kind == MK_TYPE) {
	tp = curtokmeaning->type;
	gettok();
	skipcloseparen();
	return copyexpr(tp->smax);
    }
    ex = p_expr(tp_integer);
    while (curtok == TOK_COMMA) {
	gettok();
	ex2 = p_expr(ex->val.type);
	if (ex->val.type->kind == TK_REAL) {
	    tp = ex->val.type;
	    if (ex2->val.type->kind != TK_REAL)
		ex2 = makeexpr_cast(ex2, tp);
	} else {
	    tp = ex2->val.type;
	    if (ex->val.type->kind != TK_REAL)
		ex = makeexpr_cast(ex, tp);
	}
	ex = makeexpr_bicall_2((tp->kind == TK_REAL) ? "P_rmax" : "P_imax",
			       tp, ex, ex2);
    }				
    skipcloseparen();
    return ex;
}



Static Expr *func_maxavail(ex)
Expr *ex;
{
    freeexpr(ex);
    return makeexpr_bicall_0("maxavail", tp_integer);
}



Static Expr *func_maxpos()
{
    return file_iofunc(3, seek_base);
}



Static Expr *func_memavail(ex)
Expr *ex;
{
    freeexpr(ex);
    return makeexpr_bicall_0("memavail", tp_integer);
}



Static Expr *var_mem()
{
    Expr *ex, *ex2;

    if (!wneedtok(TOK_LBR))
	return makeexpr_name("MEM", tp_integer);
    ex = p_expr(tp_integer);
    if (curtok == TOK_COLON) {
	gettok();
	ex2 = p_expr(tp_integer);
	ex = makeexpr_bicall_2("MEM", tp_ubyte, ex, ex2);
    } else {
	ex = makeexpr_bicall_1("MEM", tp_ubyte, ex);
    }
    if (!wneedtok(TOK_RBR))
	skippasttotoken(TOK_RBR, TOK_SEMI);
    note("Reference to MEM [191]");
    return ex;
}



Static Expr *var_memw()
{
    Expr *ex, *ex2;

    if (!wneedtok(TOK_LBR))
	return makeexpr_name("MEMW", tp_integer);
    ex = p_expr(tp_integer);
    if (curtok == TOK_COLON) {
	gettok();
	ex2 = p_expr(tp_integer);
	ex = makeexpr_bicall_2("MEMW", tp_ushort, ex, ex2);
    } else {
	ex = makeexpr_bicall_1("MEMW", tp_ushort, ex);
    }
    if (!wneedtok(TOK_RBR))
	skippasttotoken(TOK_RBR, TOK_SEMI);
    note("Reference to MEMW [191]");
    return ex;
}



Static Expr *var_meml()
{
    Expr *ex, *ex2;

    if (!wneedtok(TOK_LBR))
	return makeexpr_name("MEML", tp_integer);
    ex = p_expr(tp_integer);
    if (curtok == TOK_COLON) {
	gettok();
	ex2 = p_expr(tp_integer);
	ex = makeexpr_bicall_2("MEML", tp_integer, ex, ex2);
    } else {
	ex = makeexpr_bicall_1("MEML", tp_integer, ex);
    }
    if (!wneedtok(TOK_RBR))
	skippasttotoken(TOK_RBR, TOK_SEMI);
    note("Reference to MEML [191]");
    return ex;
}



Static Expr *func_min()
{
    Type *tp;
    Expr *ex, *ex2;

    if (!skipopenparen())
	return NULL;
    if (curtok == TOK_IDENT && curtokmeaning &&
	curtokmeaning->kind == MK_TYPE) {
	tp = curtokmeaning->type;
	gettok();
	skipcloseparen();
	return copyexpr(tp->smin);
    }
    ex = p_expr(tp_integer);
    while (curtok == TOK_COMMA) {
	gettok();
	ex2 = p_expr(ex->val.type);
	if (ex->val.type->kind == TK_REAL) {
	    tp = ex->val.type;
	    if (ex2->val.type->kind != TK_REAL)
		ex2 = makeexpr_cast(ex2, tp);
	} else {
	    tp = ex2->val.type;
	    if (ex->val.type->kind != TK_REAL)
		ex = makeexpr_cast(ex, tp);
	}
	ex = makeexpr_bicall_2((tp->kind == TK_REAL) ? "P_rmin" : "P_imin",
			       tp, ex, ex2);
    }				
    skipcloseparen();
    return ex;
}



Static Stmt *proc_move(ex)
Expr *ex;
{
    ex->args[0] = gentle_cast(ex->args[0], tp_anyptr);    /* source */
    ex->args[1] = gentle_cast(ex->args[1], tp_anyptr);    /* dest */
    ex->args[2] = convert_size(choosetype(argbasetype(ex->args[0]),
                                          argbasetype(ex->args[1])), ex->args[2], "MOVE");
    return makestmt_call(makeexpr_bicall_3("memmove", tp_void,
                                           ex->args[1],
                                           ex->args[0],
                                           makeexpr_arglong(ex->args[2], (size_t_long != 0))));
}



Static Stmt *proc_move_fast()
{
    Expr *ex, *ex2, *ex3, *ex4;

    if (!skipopenparen())
	return NULL;
    ex = p_expr(tp_integer);
    if (!skipcomma())
	return NULL;
    ex2 = p_expr(tp_integer);
    if (!skipcomma())
	return NULL;
    ord_range_expr(ex2->val.type->indextype, &ex4, NULL);
    ex2 = makeexpr_index(ex2, p_expr(tp_integer), copyexpr(ex4));
    if (!skipcomma())
	return NULL;
    ex3 = p_expr(tp_integer);
    if (!skipcomma())
	return NULL;
    ord_range_expr(ex3->val.type->indextype, &ex4, NULL);
    ex3 = makeexpr_index(ex3, p_expr(tp_integer), copyexpr(ex4));
    skipcloseparen();
    ex = convert_size(choosetype(argbasetype(ex2),
				 argbasetype(ex3)), ex, "MOVE_FAST");
    return makestmt_call(makeexpr_bicall_3("memmove", tp_void,
					   makeexpr_addr(ex3),
					   makeexpr_addr(ex2),
					   makeexpr_arglong(ex, (size_t_long != 0))));
}



Static Stmt *proc_new()
{
    Expr *ex, *ex2;
    Stmt *sp, **spp;
    Type *type;
    char *name, *name2 = NULL, vbuf[1000];

    if (!skipopenparen())
	return NULL;
    ex = p_expr(tp_anyptr);
    type = ex->val.type;
    if (type->kind == TK_POINTER)
	type = type->basetype;
    parse_special_variant(type, vbuf);
    skipcloseparen();
    name = find_special_variant(vbuf, NULL, specialmallocs, 3);
    if (!name) {
        name2 = find_special_variant(vbuf, NULL, specialsizeofs, 3);
	if (!name2) {
	    name = find_special_variant(vbuf, NULL, specialmallocs, 1);
	    name2 = find_special_variant(vbuf, NULL, specialsizeofs, 1);
	    if (name || !name2)
		name = find_special_variant(vbuf, "SpecialMalloc", specialmallocs, 1);
	    else
		name2 = find_special_variant(vbuf, "SpecialSizeOf", specialsizeofs, 1);
	}
    }
    if (name) {
	ex2 = makeexpr_bicall_0(name, ex->val.type);
    } else if (name2) {
	ex2 = makeexpr_bicall_1(mallocname, tp_anyptr, pc_expr_str(name2));
    } else {
	ex2 = makeexpr_bicall_1(mallocname, tp_anyptr,
				makeexpr_sizeof(makeexpr_type(type), 1));
    }
    sp = makestmt_assign(copyexpr(ex), ex2);
    if (malloccheck) {
        sp = makestmt_seq(sp, makestmt_if(makeexpr_rel(EK_EQ,
						       copyexpr(ex),
						       makeexpr_nil()),
                                          makestmt_call(makeexpr_bicall_0(name_OUTMEM, tp_int)),
                                          NULL));
    }
    spp = &sp->next;
    while (*spp)
	spp = &(*spp)->next;
    if (type->kind == TK_RECORD)
	initfilevars(type->fbase, &spp, makeexpr_hat(ex, 0));
    else if (isfiletype(type, -1))
	sp = makestmt_seq(sp, makestmt_call(initfilevar(makeexpr_hat(ex, 0))));
    else
	freeexpr(ex);
    return sp;
}



Static Expr *func_oct()
{
    return handle_vax_hex(NULL, "o", 3);
}



Static Expr *func_octal(ex)
Expr *ex;
{
    char *cp;

    ex = grabarg(ex, 0);
    if (ex->kind == EK_CONST) {
        cp = getstring(ex);
        ex = makeexpr_long(my_strtol(cp, NULL, 8));
        insertarg(&ex, 0, makeexpr_name("0%lo", tp_integer));
        return ex;
    } else {
        return makeexpr_bicall_3("strtol", tp_integer, 
                                 ex, makeexpr_nil(), makeexpr_long(8));
    }
}



Static Expr *func_odd(ex)
Expr *ex;
{
    ex = makeexpr_unlongcast(grabarg(ex, 0));
    if (*oddname)
        return makeexpr_bicall_1(oddname, tp_boolean, ex);
    else
        return makeexpr_bin(EK_BAND, tp_boolean, ex, makeexpr_long(1));
}



Static Stmt *proc_open()
{
    return handleopen(2);
}



Static Expr *func_ord()
{
    Expr *ex;

    if (wneedtok(TOK_LPAR)) {
	ex = p_ord_expr();
	skipcloseparen();
    } else
	ex = p_ord_expr();
    return makeexpr_ord(ex);
}



Static Expr *func_ord4()
{
    Expr *ex;

    if (wneedtok(TOK_LPAR)) {
	ex = p_ord_expr();
	skipcloseparen();
    } else
	ex = p_ord_expr();
    return makeexpr_longcast(makeexpr_ord(ex), 1);
}



Static Stmt *proc_pack()
{
    Expr *exs, *exd, *exi, *mind;
    Meaning *tvar;
    Stmt *sp;

    if (!skipopenparen())
	return NULL;
    exs = p_expr(NULL);
    if (!skipcomma())
	return NULL;
    exi = p_ord_expr();
    if (!skipcomma())
	return NULL;
    exd = p_expr(NULL);
    skipcloseparen();
    if (exs->val.type->kind != TK_ARRAY ||
	(exd->val.type->kind != TK_ARRAY &&
	 exd->val.type->kind != TK_SMALLARRAY)) {
	warning("Bad argument types for PACK/UNPACK [325]");
	return makestmt_call(makeexpr_bicall_3("pack", tp_void,
					       exs, exi, exd));
    }
    if (exs->val.type->smax || exd->val.type->smax) {
	tvar = makestmttempvar(exd->val.type->indextype, name_TEMP);
	sp = makestmt(SK_FOR);
	if (exd->val.type->smin)
	    mind = exd->val.type->smin;
	else
	    mind = exd->val.type->indextype->smin;
	sp->exp1 = makeexpr_assign(makeexpr_var(tvar),
				   copyexpr(mind));
	sp->exp2 = makeexpr_rel(EK_LE, makeexpr_var(tvar),
				copyexpr(exd->val.type->indextype->smax));
	sp->exp3 = makeexpr_assign(makeexpr_var(tvar),
				   makeexpr_plus(makeexpr_var(tvar),
						 makeexpr_long(1)));
	exi = makeexpr_minus(exi, copyexpr(mind));
	sp->stm1 = makestmt_assign(p_index(exd, makeexpr_var(tvar)),
				   p_index(exs,
					   makeexpr_plus(makeexpr_var(tvar),
							 exi)));
	return sp;
    } else {
	exi = gentle_cast(exi, exs->val.type->indextype);
	return makestmt_call(makeexpr_bicall_3("memcpy", exd->val.type,
					       exd,
					       makeexpr_addr(p_index(exs, exi)),
					       makeexpr_sizeof(copyexpr(exd), 0)));
    }
}



Static Expr *func_pad(ex)
Expr *ex;
{
    if (checkconst(ex->args[1], 0) ||    /* "s" is null string */
	checkconst(ex->args[2], ' ')) {
        return makeexpr_bicall_4("sprintf", tp_strptr, ex->args[0],
                                 makeexpr_string("%*s"),
                                 makeexpr_longcast(ex->args[3], 0),
                                 makeexpr_string(""));
    }
    return makeexpr_bicall_4(strpadname, tp_strptr,
			     ex->args[0], ex->args[1], ex->args[2],
			     makeexpr_arglong(ex->args[3], 0));
}



Static Stmt *proc_page()
{
    Expr *fex, *ex;

    if (curtok == TOK_LPAR) {
        fex = p_parexpr(tp_text);
        ex = makeexpr_bicall_2("fprintf", tp_int,
                               filebasename(copyexpr(fex)),
                               makeexpr_string("\f"));
    } else {
        fex = makeexpr_var(mp_output);
        ex = makeexpr_bicall_1("printf", tp_int,
                               makeexpr_string("\f"));
    }
    if (FCheck(checkfilewrite)) {
        ex = makeexpr_bicall_2("~SETIO", tp_void,
                               makeexpr_rel(EK_GE, ex, makeexpr_long(0)),
			       makeexpr_name(filewriteerrorname, tp_int));
    }
    return wrapopencheck(makestmt_call(ex), fex);
}



Static Expr *func_paramcount(ex)
Expr *ex;
{
    freeexpr(ex);
    return makeexpr_minus(makeexpr_name(name_ARGC, tp_int),
                          makeexpr_long(1));
}



Static Expr *func_paramstr(ex)
Expr *ex;
{
    Expr *ex2;

    ex2 = makeexpr_index(makeexpr_name(name_ARGV,
				       makepointertype(tp_strptr)),
			 makeexpr_unlongcast(ex->args[1]),
			 makeexpr_long(0));
    ex2->val.type = tp_str255;
    return makeexpr_bicall_3("sprintf", tp_strptr,
			     ex->args[0],
			     makeexpr_string("%s"),
			     ex2);
}



Static Expr *func_pi()
{
    return makeexpr_name("M_PI", tp_longreal);
}



Static Expr *var_port()
{
    Expr *ex;

    if (!wneedtok(TOK_LBR))
	return makeexpr_name("PORT", tp_integer);
    ex = p_expr(tp_integer);
    if (!wneedtok(TOK_RBR))
	skippasttotoken(TOK_RBR, TOK_SEMI);
    note("Reference to PORT [191]");
    return makeexpr_bicall_1("PORT", tp_ubyte, ex);
}



Static Expr *var_portw()
{
    Expr *ex;

    if (!wneedtok(TOK_LBR))
	return makeexpr_name("PORTW", tp_integer);
    ex = p_expr(tp_integer);
    if (!wneedtok(TOK_RBR))
	skippasttotoken(TOK_RBR, TOK_SEMI);
    note("Reference to PORTW [191]");
    return makeexpr_bicall_1("PORTW", tp_ushort, ex);
}



Static Expr *func_pos(ex)
Expr *ex;
{
    char *cp;

    cp = strposname;
    if (!*cp) {
        note("POS function used [192]");
        cp = "POS";
    } 
    return makeexpr_bicall_3(cp, tp_int,
                             ex->args[1], 
                             ex->args[0],
                             makeexpr_long(1));
}



Static Expr *func_ptr(ex)
Expr *ex;
{
    note("PTR function was used [193]");
    return ex;
}



Static Expr *func_position()
{
    return file_iofunc(2, seek_base);
}



Static Expr *func_pred()
{
    Expr *ex;

    if (wneedtok(TOK_LPAR)) {
	ex = p_ord_expr();
	skipcloseparen();
    } else
	ex = p_ord_expr();
#if 1
    ex = makeexpr_inc(ex, makeexpr_long(-1));
#else
    ex = makeexpr_cast(makeexpr_plus(ex, makeexpr_long(-1)), ex->val.type);
#endif
    return ex;
}



Static Stmt *proc_put()
{
    Expr *ex;
    Type *type;

    if (curtok == TOK_LPAR)
	ex = p_parexpr(tp_text);
    else
	ex = makeexpr_var(mp_output);
    requirefilebuffer(ex);
    type = ex->val.type;
    if (isfiletype(type, -1) && *charputname &&
	filebasetype(type)->kind == TK_CHAR)
	return makestmt_call(makeexpr_bicall_1(charputname, tp_void,
					       filebasename(ex)));
    else if (isfiletype(type, -1) && *arrayputname &&
	     filebasetype(type)->kind == TK_ARRAY)
	return makestmt_call(makeexpr_bicall_2(arrayputname, tp_void,
					       filebasename(ex),
					       makeexpr_type(filebasetype(type))));
    else
	return makestmt_call(makeexpr_bicall_2(putname, tp_void,
					       filebasename(ex),
					       makeexpr_type(filebasetype(type))));
}



Static Expr *func_pwroften(ex)
Expr *ex;
{
    return makeexpr_bicall_2("pow", tp_longreal,
			     makeexpr_real("10.0"), grabarg(ex, 0));
}



Static Stmt *proc_reset()
{
    return handleopen(0);
}



Static Stmt *proc_rewrite()
{
    return handleopen(1);
}




Stmt *doseek(fex, ex)
Expr *fex, *ex;
{
    Expr *ex2;
    Type *basetype = filebasetype(fex->val.type);

    if (ansiC == 1)
        ex2 = makeexpr_name("SEEK_SET", tp_int);
    else
        ex2 = makeexpr_long(0);
    ex = makeexpr_bicall_3("fseek", tp_int,
                           filebasename(copyexpr(fex)),
                           makeexpr_arglong(
                               makeexpr_times(makeexpr_minus(ex,
                                                             makeexpr_long(seek_base)),
                                              makeexpr_sizeof(makeexpr_type(basetype), 0)),
                               1),
                           ex2);
    if (FCheck(checkfileseek)) {
        ex = makeexpr_bicall_2("~SETIO", tp_void,
                               makeexpr_rel(EK_EQ, ex, makeexpr_long(0)),
			       makeexpr_name(endoffilename, tp_int));
    }
    return makestmt_call(ex);
}




Static Expr *makegetchar(fex)
Expr *fex;
{
    if (isvar(fex, mp_input))
        return makeexpr_bicall_0("getchar", tp_char);
    else
        return makeexpr_bicall_1("getc", tp_char, filebasename(copyexpr(fex)));
}



Static Stmt *fixscanf(sp, fex)
Stmt *sp;
Expr *fex;
{
    int nargs, i, isstrread;
    char *cp;
    Expr *ex;
    Stmt *sp2;

    isstrread = (fex->val.type->kind == TK_STRING);
    if (sp->kind == SK_ASSIGN && sp->exp1->kind == EK_BICALL &&
        !strcmp(sp->exp1->val.s, "scanf")) {
        if (sp->exp1->args[0]->kind == EK_CONST &&
            !(sp->exp1->args[0]->val.i&1) && !isstrread) {
            cp = sp->exp1->args[0]->val.s;    /* scanf("%c%c") -> getchar;getchar */
            for (i = 0; cp[i] == '%' && cp[i+1] == 'c'; ) {
                i += 2;
                if (i == sp->exp1->args[0]->val.i) {
                    sp2 = NULL;
                    for (i = 1; i < sp->exp1->nargs; i++) {
                        ex = makeexpr_hat(sp->exp1->args[i], 0);
                        sp2 = makestmt_seq(sp2,
                                           makestmt_assign(copyexpr(ex),
                                                           makegetchar(fex)));
                        if (checkeof(fex)) {
                            sp2 = makestmt_seq(sp2,
                                makestmt_call(makeexpr_bicall_2("~SETIO", tp_void,
                                                                makeexpr_rel(EK_NE,
                                                                             ex,
                                                                             makeexpr_name("EOF", tp_char)),
								makeexpr_name(endoffilename, tp_int))));
                        } else
                            freeexpr(ex);
                    }
                    return sp2;
                }
            }
        }
        nargs = sp->exp1->nargs - 1;
        if (isstrread) {
            strchange(&sp->exp1->val.s, "sscanf");
            insertarg(&sp->exp1, 0, copyexpr(fex));
        } else if (!isvar(fex, mp_input)) {
            strchange(&sp->exp1->val.s, "fscanf");
            insertarg(&sp->exp1, 0, filebasename(copyexpr(fex)));
        }
        if (FCheck(checkreadformat)) {
            if (checkeof(fex) && !isstrread)
                ex = makeexpr_cond(makeexpr_rel(EK_NE,
                                                makeexpr_bicall_1("feof",
								  tp_int,
								  filebasename(copyexpr(fex))),
                                                makeexpr_long(0)),
				   makeexpr_name(endoffilename, tp_int),
				   makeexpr_name(badinputformatname, tp_int));
            else
		ex = makeexpr_name(badinputformatname, tp_int);
            sp->exp1 = makeexpr_bicall_2("~SETIO", tp_void,
                                         makeexpr_rel(EK_EQ,
                                                      sp->exp1,
                                                      makeexpr_long(nargs)),
                                         ex);
        } else if (checkeof(fex) && !isstrread) {
            sp->exp1 = makeexpr_bicall_2("~SETIO", tp_void,
                                         makeexpr_rel(EK_NE,
                                                      sp->exp1,
                                                      makeexpr_name("EOF", tp_int)),
					 makeexpr_name(endoffilename, tp_int));
        }
    }
    return sp;
}



Static Expr *makefgets(vex, lex, fex)
Expr *vex, *lex, *fex;
{
    Expr *ex;

    ex = makeexpr_bicall_3("fgets", tp_strptr,
                           vex,
                           lex,
                           filebasename(copyexpr(fex)));
    if (checkeof(fex)) {
        ex = makeexpr_bicall_2("~SETIO", tp_void,
                               makeexpr_rel(EK_NE, ex, makeexpr_nil()),
			       makeexpr_name(endoffilename, tp_int));
    }
    return ex;
}



Static Stmt *skipeoln(fex)
Expr *fex;
{
    Meaning *tvar;
    Expr *ex;

    if (!strcmp(readlnname, "fgets")) {
        tvar = makestmttempvar(tp_str255, name_STRING);
        return makestmt_call(makefgets(makeexpr_var(tvar),
                                       makeexpr_long(stringceiling+1),
                                       filebasename(fex)));
    } else if (!strcmp(readlnname, "scanf") || !*readlnname) {
        if (checkeof(fex))
            ex = makeexpr_bicall_2("~SETIO", tp_void,
                                   makeexpr_rel(EK_NE,
                                                makegetchar(fex),
                                                makeexpr_name("EOF", tp_char)),
				   makeexpr_name(endoffilename, tp_int));
        else
            ex = makegetchar(fex);
        return makestmt_seq(fixscanf(
                    makestmt_call(makeexpr_bicall_1("scanf", tp_int,
                                                    makeexpr_string("%*[^\n]"))), fex),
                    makestmt_call(ex));
    } else {
        return makestmt_call(makeexpr_bicall_1(readlnname, tp_void,
                                               filebasename(copyexpr(fex))));
    }
}



Static Stmt *handleread_text(fex, var, isreadln)
Expr *fex, *var;
int isreadln;
{
    Stmt *spbase, *spafter, *sp;
    Expr *ex = NULL, *exj = NULL;
    Type *type;
    Meaning *tvar, *tempcp, *mp;
    int i, isstrread, scanfmode, readlnflag, varstring, maxstring;
    int longstrsize = (longstringsize > 0) ? longstringsize : stringceiling;
    long rmin, rmax;
    char *fmt;

    spbase = NULL;
    spafter = NULL;
    sp = NULL;
    tempcp = NULL;
    if (fex->val.type->kind == TK_ARRAY)
	fex = makeexpr_sprintfify(fex);
    isstrread = (fex->val.type->kind == TK_STRING);
    if (isstrread) {
        exj = var;
        var = p_expr(NULL);
    }
    scanfmode = !strcmp(readlnname, "scanf") || !*readlnname || isstrread;
    for (;;) {
        readlnflag = isreadln && curtok == TOK_RPAR;
        if (var->val.type->kind == TK_STRING && !isstrread) {
            if (sp)
                spbase = makestmt_seq(spbase, fixscanf(sp, fex));
            spbase = makestmt_seq(spbase, spafter);
            varstring = (varstrings && var->kind == EK_VAR &&
                         (mp = (Meaning *)var->val.i)->kind == MK_VARPARAM &&
                         mp->type == tp_strptr);
            maxstring = (strmax(var) >= longstrsize && !varstring);
            if (isvar(fex, mp_input) && maxstring && usegets && readlnflag) {
                spbase = makestmt_seq(spbase,
                                      makestmt_call(makeexpr_bicall_1("gets", tp_str255,
                                                                      makeexpr_addr(var))));
                isreadln = 0;
            } else if (scanfmode && !varstring &&
                       (*readlnname || !isreadln)) {
                spbase = makestmt_seq(spbase, makestmt_assign(makeexpr_hat(copyexpr(var), 0),
                                                              makeexpr_char(0)));
                if (maxstring && usegets)
                    ex = makeexpr_string("%[^\n]");
                else
                    ex = makeexpr_string(format_d("%%%d[^\n]", strmax(var)));
                ex = makeexpr_bicall_2("scanf", tp_int, ex, makeexpr_addr(var));
                spbase = makestmt_seq(spbase, fixscanf(makestmt_call(ex), fex));
                if (readlnflag && maxstring && usegets) {
                    spbase = makestmt_seq(spbase, makestmt_call(makegetchar(fex)));
                    isreadln = 0;
                }
            } else {
                ex = makeexpr_plus(strmax_func(var), makeexpr_long(1));
                spbase = makestmt_seq(spbase,
                                      makestmt_call(makefgets(makeexpr_addr(copyexpr(var)),
                                                              ex,
                                                              fex)));
                if (!tempcp)
                    tempcp = makestmttempvar(tp_charptr, name_TEMP);
                spbase = makestmt_seq(spbase,
                                      makestmt_assign(makeexpr_var(tempcp),
                                                      makeexpr_bicall_2("strchr", tp_charptr,
                                                                        makeexpr_addr(copyexpr(var)),
                                                                        makeexpr_char('\n'))));
                sp = makestmt_assign(makeexpr_hat(makeexpr_var(tempcp), 0),
                                     makeexpr_long(0));
                if (readlnflag)
                    isreadln = 0;
                else
                    sp = makestmt_seq(sp,
                                      makestmt_call(makeexpr_bicall_2("ungetc", tp_void,
                                                                      makeexpr_char('\n'),
                                                                      filebasename(copyexpr(fex)))));
                spbase = makestmt_seq(spbase, makestmt_if(makeexpr_rel(EK_NE,
                                                                       makeexpr_var(tempcp),
                                                                       makeexpr_nil()),
                                                          sp,
                                                          NULL));
            }
            sp = NULL;
            spafter = NULL;
        } else if (var->val.type->kind == TK_ARRAY && !isstrread) {
            if (sp)
                spbase = makestmt_seq(spbase, fixscanf(sp, fex));
            spbase = makestmt_seq(spbase, spafter);
	    ex = makeexpr_sizeof(copyexpr(var), 0);
	    if (readlnflag) {
		spbase = makestmt_seq(spbase,
		     makestmt_call(
			 makeexpr_bicall_3("P_readlnpaoc", tp_void,
					   filebasename(copyexpr(fex)),
					   makeexpr_addr(var),
					   makeexpr_arglong(ex, 0))));
		isreadln = 0;
	    } else {
		spbase = makestmt_seq(spbase,
		     makestmt_call(
			 makeexpr_bicall_3("P_readpaoc", tp_void,
					   filebasename(copyexpr(fex)),
					   makeexpr_addr(var),
					   makeexpr_arglong(ex, 0))));
	    }
            sp = NULL;
            spafter = NULL;
        } else {
            switch (ord_type(var->val.type)->kind) {

                case TK_INTEGER:
		    fmt = "d";
		    if (curtok == TOK_COLON) {
			gettok();
			if (curtok == TOK_IDENT &&
			    !strcicmp(curtokbuf, "HEX")) {
			    fmt = "x";
			} else if (curtok == TOK_IDENT &&
			    !strcicmp(curtokbuf, "OCT")) {
			    fmt = "o";
			} else if (curtok == TOK_IDENT &&
			    !strcicmp(curtokbuf, "BIN")) {
			    fmt = "b";
			    note("Using %b for binary format in scanf [194]");
			} else
			    warning("Unrecognized format specified in READ [212]");
			gettok();
		    }
                    type = findbasetype(var->val.type, ODECL_NOPRES);
                    if (exprlongness(var) > 0)
                        ex = makeexpr_string(format_s("%%l%s", fmt));
                    else if (type == tp_integer || type == tp_int ||
                             type == tp_uint || type == tp_sint)
                        ex = makeexpr_string(format_s("%%%s", fmt));
                    else if (type == tp_sshort || type == tp_ushort)
                        ex = makeexpr_string(format_s("%%h%s", fmt));
                    else {
                        tvar = makestmttempvar(tp_int, name_TEMP);
                        spafter = makestmt_seq(spafter,
                                               makestmt_assign(var,
                                                               makeexpr_var(tvar)));
                        var = makeexpr_var(tvar);
                        ex = makeexpr_string(format_s("%%%s", fmt));
                    }
                    break;

                case TK_CHAR:
                    ex = makeexpr_string("%c");
                    if (newlinespace && !isstrread) {
                        spafter = makestmt_seq(spafter,
                                               makestmt_if(makeexpr_rel(EK_EQ,
                                                                        copyexpr(var),
                                                                        makeexpr_char('\n')),
                                                           makestmt_assign(copyexpr(var),
                                                                           makeexpr_char(' ')),
                                                           NULL));
                    }
                    break;

                case TK_BOOLEAN:
                    tvar = makestmttempvar(tp_str255, name_STRING);
                    spafter = makestmt_seq(spafter,
                        makestmt_assign(var,
                                        makeexpr_or(makeexpr_rel(EK_EQ,
                                                                 makeexpr_hat(makeexpr_var(tvar), 0),
                                                                 makeexpr_char('T')),
                                                    makeexpr_rel(EK_EQ,
                                                                 makeexpr_hat(makeexpr_var(tvar), 0),
                                                                 makeexpr_char('t')))));
                    var = makeexpr_var(tvar);
                    ex = makeexpr_string(" %[a-zA-Z]");
                    break;

                case TK_ENUM:
                    warning("READ on enumerated types not yet supported [213]");
                    if (useenum)
                        ex = makeexpr_string("%d");
                    else
                        ex = makeexpr_string("%hd");
                    break;

                case TK_REAL:
		    if (var->val.type == tp_longreal)
			ex = makeexpr_string("%lg");
		    else
			ex = makeexpr_string("%g");
                    break;

                case TK_STRING:     /* strread only */
                    ex = makeexpr_string(format_d("%%%lds", strmax(fex)));
                    break;

                case TK_ARRAY:      /* strread only */
                    if (!ord_range(ex->val.type->indextype, &rmin, &rmax)) {
                        rmin = 1;
                        rmax = 1;
                        note("Can't determine length of packed array of chars [195]");
                    }
                    ex = makeexpr_string(format_d("%%%ldc", rmax-rmin+1));
                    break;

                default:
                    note("Element has wrong type for WRITE statement [196]");
                    ex = NULL;
                    break;

            }
            if (ex) {
                var = makeexpr_addr(var);
                if (sp) {
                    sp->exp1->args[0] = makeexpr_concat(sp->exp1->args[0], ex, 0);
                    insertarg(&sp->exp1, sp->exp1->nargs, var);
                } else {
                    sp = makestmt_call(makeexpr_bicall_2("scanf", tp_int, ex, var));
                }
            }
        }
        if (curtok == TOK_COMMA) {
            gettok();
            var = p_expr(NULL);
        } else
            break;
    }
    if (sp) {
        if (isstrread && !FCheck(checkreadformat) &&
            ((i=0, checkstring(sp->exp1->args[0], "%d")) ||
             (i++, checkstring(sp->exp1->args[0], "%ld")) ||
             (i++, checkstring(sp->exp1->args[0], "%hd")) ||
             (i++, checkstring(sp->exp1->args[0], "%lg")))) {
            if (fullstrread != 0 && exj) {
                tvar = makestmttempvar(tp_strptr, name_STRING);
                sp->exp1 = makeexpr_assign(makeexpr_hat(sp->exp1->args[1], 0),
                                           (i == 3) ? makeexpr_bicall_2("strtod", tp_longreal,
                                                                        copyexpr(fex),
                                                                        makeexpr_addr(makeexpr_var(tvar)))
                                                    : makeexpr_bicall_3("strtol", tp_integer,
                                                                        copyexpr(fex),
                                                                        makeexpr_addr(makeexpr_var(tvar)),
                                                                        makeexpr_long(10)));
		spafter = makestmt_seq(spafter,
				       makestmt_assign(copyexpr(exj),
						       makeexpr_minus(makeexpr_var(tvar),
								      makeexpr_addr(copyexpr(fex)))));
            } else {
                sp->exp1 = makeexpr_assign(makeexpr_hat(sp->exp1->args[1], 0),
                                           makeexpr_bicall_1((i == 1) ? "atol" : (i == 3) ? "atof" : "atoi",
                                                             (i == 1) ? tp_integer : (i == 3) ? tp_longreal : tp_int,
                                                             copyexpr(fex)));
            }
        } else if (isstrread && fullstrread != 0 && exj) {
            sp->exp1->args[0] = makeexpr_concat(sp->exp1->args[0],
                                                makeexpr_string(sizeof_int >= 32 ? "%n" : "%ln"), 0);
            insertarg(&sp->exp1, sp->exp1->nargs, makeexpr_addr(copyexpr(exj)));
        } else if (isreadln && scanfmode && !FCheck(checkreadformat)) {
            isreadln = 0;
            sp->exp1->args[0] = makeexpr_concat(sp->exp1->args[0],
                                                makeexpr_string("%*[^\n]"), 0);
            spafter = makestmt_seq(makestmt_call(makegetchar(fex)), spafter);
        }
        spbase = makestmt_seq(spbase, fixscanf(sp, fex));
    }
    spbase = makestmt_seq(spbase, spafter);
    if (isreadln)
        spbase = makestmt_seq(spbase, skipeoln(fex));
    return spbase;
}



Static Stmt *handleread_bin(fex, var)
Expr *fex, *var;
{
    Type *basetype;
    Stmt *sp;
    Expr *ex, *tvardef = NULL;

    sp = NULL;
    basetype = filebasetype(fex->val.type);
    for (;;) {
        ex = makeexpr_bicall_4("fread", tp_integer, makeexpr_addr(var),
                                                    makeexpr_sizeof(makeexpr_type(basetype), 0),
                                                    makeexpr_long(1),
                                                    filebasename(copyexpr(fex)));
        if (checkeof(fex)) {
            ex = makeexpr_bicall_2("~SETIO", tp_void,
                                   makeexpr_rel(EK_EQ, ex, makeexpr_long(1)),
				   makeexpr_name(endoffilename, tp_int));
        }
        sp = makestmt_seq(sp, makestmt_call(ex));
        if (curtok == TOK_COMMA) {
            gettok();
            var = p_expr(NULL);
        } else
            break;
    }
    freeexpr(tvardef);
    return sp;
}



Static Stmt *proc_read()
{
    Expr *fex, *ex;
    Stmt *sp;

    if (!skipopenparen())
	return NULL;
    ex = p_expr(NULL);
    if (isfiletype(ex->val.type, -1) && wneedtok(TOK_COMMA)) {
        fex = ex;
        ex = p_expr(NULL);
    } else {
        fex = makeexpr_var(mp_input);
    }
    if (fex->val.type == tp_text || fex->val.type == tp_bigtext)
        sp = handleread_text(fex, ex, 0);
    else
        sp = handleread_bin(fex, ex);
    skipcloseparen();
    return wrapopencheck(sp, fex);
}



Static Stmt *proc_readdir()
{
    Expr *fex, *ex;
    Stmt *sp;

    if (!skipopenparen())
	return NULL;
    fex = p_expr(tp_text);
    if (!skipcomma())
	return NULL;
    ex = p_expr(tp_integer);
    sp = doseek(fex, ex);
    if (!skipopenparen())
	return sp;
    sp = makestmt_seq(sp, handleread_bin(fex, p_expr(NULL)));
    skipcloseparen();
    return wrapopencheck(sp, fex);
}



Static Stmt *proc_readln()
{
    Expr *fex, *ex;
    Stmt *sp;

    if (curtok != TOK_LPAR) {
        fex = makeexpr_var(mp_input);
        return wrapopencheck(skipeoln(copyexpr(fex)), fex);
    } else {
        gettok();
        ex = p_expr(NULL);
        if (isfiletype(ex->val.type, -1)) {
            fex = ex;
            if (curtok == TOK_RPAR || !wneedtok(TOK_COMMA)) {
                skippasttotoken(TOK_RPAR, TOK_SEMI);
                return wrapopencheck(skipeoln(copyexpr(fex)), fex);
            } else {
                ex = p_expr(NULL);
            }
        } else {
            fex = makeexpr_var(mp_input);
        }
        sp = handleread_text(fex, ex, 1);
        skipcloseparen();
    }
    return wrapopencheck(sp, fex);
}



Static Stmt *proc_readv()
{
    Expr *vex;
    Stmt *sp;

    if (!skipopenparen())
	return NULL;
    vex = p_expr(tp_str255);
    if (!skipcomma())
	return NULL;
    sp = handleread_text(vex, NULL, 0);
    skipcloseparen();
    return sp;
}



Static Stmt *proc_strread()
{
    Expr *vex, *exi, *exj, *exjj, *ex;
    Stmt *sp, *sp2;
    Meaning *tvar, *jvar;

    if (!skipopenparen())
	return NULL;
    vex = p_expr(tp_str255);
    if (vex->kind != EK_VAR) {
        tvar = makestmttempvar(tp_str255, name_STRING);
        sp = makestmt_assign(makeexpr_var(tvar), vex);
        vex = makeexpr_var(tvar);
    } else
        sp = NULL;
    if (!skipcomma())
	return NULL;
    exi = p_expr(tp_integer);
    if (!skipcomma())
	return NULL;
    exj = p_expr(tp_integer);
    if (!skipcomma())
	return NULL;
    if (exprspeed(exi) >= 5 || !nosideeffects(exi, 0)) {
        sp = makestmt_seq(sp, makestmt_assign(copyexpr(exj), exi));
        exi = copyexpr(exj);
    }
    if (fullstrread != 0 &&
        ((ex = singlevar(exj)) == NULL || exproccurs(exi, ex))) {
        jvar = makestmttempvar(exj->val.type, name_TEMP);
        exjj = makeexpr_var(jvar);
    } else {
        exjj = copyexpr(exj);
        jvar = (exj->kind == EK_VAR) ? (Meaning *)exj->val.i : NULL;
    }
    sp2 = handleread_text(bumpstring(copyexpr(vex),
                                     copyexpr(exi), 1),
                          exjj, 0);
    sp = makestmt_seq(sp, sp2);
    skipcloseparen();
    if (fullstrread == 0) {
        sp = makestmt_seq(sp, makestmt_assign(exj,
                                              makeexpr_plus(makeexpr_bicall_1("strlen", tp_int,
                                                                              vex),
                                                            makeexpr_long(1))));
        freeexpr(exjj);
        freeexpr(exi);
    } else {
        sp = makestmt_seq(sp, makestmt_assign(exj,
                                              makeexpr_plus(exjj, exi)));
        if (fullstrread == 2)
            note("STRREAD was used [197]");
        freeexpr(vex);
    }
    return mixassignments(sp, jvar);
}




Static Expr *func_random()
{
    Expr *ex;

    if (curtok == TOK_LPAR) {
        gettok();
        ex = p_expr(tp_integer);
        skipcloseparen();
        return makeexpr_bicall_1(randintname, tp_integer, makeexpr_arglong(ex, 1));
    } else {
        return makeexpr_bicall_0(randrealname, tp_longreal);
    }
}



Static Stmt *proc_randomize()
{
    if (*randomizename)
        return makestmt_call(makeexpr_bicall_0(randomizename, tp_void));
    else
        return NULL;
}



Static Expr *func_round(ex)
Expr *ex;
{
    Meaning *tvar;

    ex = grabarg(ex, 0);
    if (ex->val.type->kind != TK_REAL)
	return ex;
    if (*roundname) {
        if (*roundname != '*' || (exprspeed(ex) < 5 && nosideeffects(ex, 0))) {
            return makeexpr_bicall_1(roundname, tp_integer, ex);
        } else {
            tvar = makestmttempvar(tp_longreal, name_TEMP);
            return makeexpr_comma(makeexpr_assign(makeexpr_var(tvar), ex),
                                  makeexpr_bicall_1(roundname, tp_integer, makeexpr_var(tvar)));
        }
    } else {
        return makeexpr_actcast(makeexpr_bicall_1("floor", tp_longreal,
						  makeexpr_plus(ex, makeexpr_real("0.5"))),
                                tp_integer);
    }
}



Static Stmt *proc_unpack()
{
    Expr *exs, *exd, *exi, *mins;
    Meaning *tvar;
    Stmt *sp;

    if (!skipopenparen())
	return NULL;
    exs = p_expr(NULL);
    if (!skipcomma())
	return NULL;
    exd = p_expr(NULL);
    if (!skipcomma())
	return NULL;
    exi = p_ord_expr();
    skipcloseparen();
    if (exd->val.type->kind != TK_ARRAY ||
	(exs->val.type->kind != TK_ARRAY &&
	 exs->val.type->kind != TK_SMALLARRAY)) {
	warning("Bad argument types for PACK/UNPACK [325]");
	return makestmt_call(makeexpr_bicall_3("unpack", tp_void,
					       exs, exd, exi));
    }
    if (exs->val.type->smax || exd->val.type->smax) {
	tvar = makestmttempvar(exs->val.type->indextype, name_TEMP);
	sp = makestmt(SK_FOR);
	if (exs->val.type->smin)
	    mins = exs->val.type->smin;
	else
	    mins = exs->val.type->indextype->smin;
	sp->exp1 = makeexpr_assign(makeexpr_var(tvar),
				   copyexpr(mins));
	sp->exp2 = makeexpr_rel(EK_LE, makeexpr_var(tvar),
				copyexpr(exs->val.type->indextype->smax));
	sp->exp3 = makeexpr_assign(makeexpr_var(tvar),
				   makeexpr_plus(makeexpr_var(tvar),
						 makeexpr_long(1)));
	exi = makeexpr_minus(exi, copyexpr(mins));
	sp->stm1 = makestmt_assign(p_index(exd,
					   makeexpr_plus(makeexpr_var(tvar),
							 exi)),
				   p_index(exs, makeexpr_var(tvar)));
	return sp;
    } else {
	exi = gentle_cast(exi, exs->val.type->indextype);
	return makestmt_call(makeexpr_bicall_3("memcpy", exd->val.type,
					       exd,
					       makeexpr_addr(p_index(exs, exi)),
					       makeexpr_sizeof(copyexpr(exd), 0)));
    }
}



Static Expr *func_uround(ex)
Expr *ex;
{
    ex = grabarg(ex, 0);
    if (ex->val.type->kind != TK_REAL)
	return ex;
    return makeexpr_actcast(makeexpr_bicall_1("floor", tp_longreal,
					      makeexpr_plus(ex, makeexpr_real("0.5"))),
			    tp_unsigned);
}



Static Expr *func_scan()
{
    Expr *ex, *ex2, *ex3;
    char *name;

    if (!skipopenparen())
	return NULL;
    ex = p_expr(tp_integer);
    if (!skipcomma())
	return NULL;
    if (curtok == TOK_EQ)
	name = "P_scaneq";
    else 
	name = "P_scanne";
    gettok();
    ex2 = p_expr(tp_char);
    if (!skipcomma())
	return NULL;
    ex3 = p_expr(tp_str255);
    skipcloseparen();
    return makeexpr_bicall_3(name, tp_int,
			     makeexpr_arglong(ex, 0),
			     makeexpr_charcast(ex2), ex3);
}



Static Expr *func_scaneq(ex)
Expr *ex;
{
    return makeexpr_bicall_3("P_scaneq", tp_int,
			     makeexpr_arglong(ex->args[0], 0),
			     makeexpr_charcast(ex->args[1]),
			     ex->args[2]);
}


Static Expr *func_scanne(ex)
Expr *ex;
{
    return makeexpr_bicall_3("P_scanne", tp_int,
			     makeexpr_arglong(ex->args[0], 0),
			     makeexpr_charcast(ex->args[1]),
			     ex->args[2]);
}



Static Stmt *proc_seek()
{
    Expr *fex, *ex;
    Stmt *sp;

    if (!skipopenparen())
	return NULL;
    fex = p_expr(tp_text);
    if (!skipcomma())
	return NULL;
    ex = p_expr(tp_integer);
    skipcloseparen();
    sp = wrapopencheck(doseek(fex, ex), copyexpr(fex));
    if (*setupbufname && fileisbuffered(fex, 1))
	sp = makestmt_seq(sp,
		 makestmt_call(
		     makeexpr_bicall_2(setupbufname, tp_void,
				       filebasename(fex),
				       makeexpr_type(filebasetype(fex->val.type)))));
    else
	freeexpr(fex);
    return sp;
}



Static Expr *func_seekeof()
{
    Expr *ex;

    if (curtok == TOK_LPAR)
        ex = p_parexpr(tp_text);
    else
        ex = makeexpr_var(mp_input);
    if (*skipspacename)
        ex = makeexpr_bicall_1(skipspacename, tp_text, filebasename(ex));
    else
        note("SEEKEOF was used [198]");
    return iofunc(ex, 0);
}



Static Expr *func_seekeoln()
{
    Expr *ex;

    if (curtok == TOK_LPAR)
        ex = p_parexpr(tp_text);
    else
        ex = makeexpr_var(mp_input);
    if (*skipspacename)
        ex = makeexpr_bicall_1(skipspacename, tp_text, filebasename(ex));
    else
        note("SEEKEOLN was used [199]");
    return iofunc(ex, 1);
}



Static Stmt *proc_setstrlen()
{
    Expr *ex, *ex2;

    if (!skipopenparen())
	return NULL;
    ex = p_expr(tp_str255);
    if (!skipcomma())
	return NULL;
    ex2 = p_expr(tp_integer);
    skipcloseparen();
    return makestmt_assign(makeexpr_bicall_1("strlen", tp_int, ex),
                           ex2);
}



Static Stmt *proc_settextbuf()
{
    Expr *fex, *bex, *sex;

    if (!skipopenparen())
	return NULL;
    fex = p_expr(tp_text);
    if (!skipcomma())
	return NULL;
    bex = p_expr(NULL);
    if (curtok == TOK_COMMA) {
        gettok();
        sex = p_expr(tp_integer);
    } else
        sex = makeexpr_sizeof(copyexpr(bex), 0);
    skipcloseparen();
    note("Make sure setvbuf() call occurs when file is open [200]");
    return makestmt_call(makeexpr_bicall_4("setvbuf", tp_void,
                                           filebasename(fex),
                                           makeexpr_addr(bex),
                                           makeexpr_name("_IOFBF", tp_integer),
                                           sex));
}



Static Expr *func_sin(ex)
Expr *ex;
{
    return makeexpr_bicall_1("sin", tp_longreal, grabarg(ex, 0));
}


Static Expr *func_sinh(ex)
Expr *ex;
{
    return makeexpr_bicall_1("sinh", tp_longreal, grabarg(ex, 0));
}



Static Expr *func_sizeof()
{
    Expr *ex;
    Type *type;
    char *name, vbuf[1000];
    int lpar;

    lpar = (curtok == TOK_LPAR);
    if (lpar)
	gettok();
    if (curtok == TOK_IDENT && curtokmeaning && curtokmeaning->kind == MK_TYPE) {
        ex = makeexpr_type(curtokmeaning->type);
        gettok();
    } else
        ex = p_expr(NULL);
    type = ex->val.type;
    parse_special_variant(type, vbuf);
    if (lpar)
	skipcloseparen();
    name = find_special_variant(vbuf, "SpecialSizeOf", specialsizeofs, 1);
    if (name) {
	freeexpr(ex);
	return pc_expr_str(name);
    } else
	return makeexpr_sizeof(ex, 0);
}



Static Expr *func_statusv()
{
    return makeexpr_name(name_IORESULT, tp_integer);
}



Static Expr *func_str_hp(ex)
Expr *ex;
{
    return makeexpr_addr(makeexpr_substring(ex->args[0], ex->args[1], 
                                            ex->args[2], ex->args[3]));
}



Static Stmt *proc_strappend()
{
    Expr *ex, *ex2;

    if (!skipopenparen())
	return NULL;
    ex = p_expr(tp_str255);
    if (!skipcomma())
	return NULL;
    ex2 = p_expr(tp_str255);
    skipcloseparen();
    return makestmt_assign(ex, makeexpr_concat(copyexpr(ex), ex2, 0));
}



Static Stmt *proc_strdelete()
{
    Meaning *tvar = NULL, *tvari;
    Expr *ex, *ex2, *ex3, *ex4, *exi, *exn;
    Stmt *sp;

    if (!skipopenparen())
	return NULL;
    ex = p_expr(tp_str255);
    if (!skipcomma())
	return NULL;
    exi = p_expr(tp_integer);
    if (curtok == TOK_COMMA) {
	gettok();
	exn = p_expr(tp_integer);
    } else
	exn = makeexpr_long(1);
    skipcloseparen();
    if (exprspeed(exi) < 5 && nosideeffects(exi, 0))
        sp = NULL;
    else {
        tvari = makestmttempvar(tp_int, name_TEMP);
        sp = makestmt_assign(makeexpr_var(tvari), exi);
        exi = makeexpr_var(tvari);
    }
    ex3 = bumpstring(copyexpr(ex), copyexpr(exi), 1);
    ex4 = bumpstring(copyexpr(ex), makeexpr_plus(exi, exn), 1);
    if (strcpyleft) {
        ex2 = ex3;
    } else {
        tvar = makestmttempvar(tp_str255, name_STRING);
        ex2 = makeexpr_var(tvar);
    }
    sp = makestmt_seq(sp, makestmt_assign(ex2, ex4));
    if (!strcpyleft)
        sp = makestmt_seq(sp, makestmt_assign(ex3, makeexpr_var(tvar)));
    return sp;
}



Static Stmt *proc_strinsert()
{
    Meaning *tvari;
    Expr *exs, *exd, *exi;
    Stmt *sp;

    if (!skipopenparen())
	return NULL;
    exs = p_expr(tp_str255);
    if (!skipcomma())
	return NULL;
    exd = p_expr(tp_str255);
    if (!skipcomma())
	return NULL;
    exi = p_expr(tp_integer);
    skipcloseparen();
#if 0
    if (checkconst(exi, 1)) {
        freeexpr(exi);
        return makestmt_assign(exd,
                               makeexpr_concat(exs, copyexpr(exd)));
    }
#endif
    if (exprspeed(exi) < 5 && nosideeffects(exi, 0))
        sp = NULL;
    else {
        tvari = makestmttempvar(tp_int, name_TEMP);
        sp = makestmt_assign(makeexpr_var(tvari), exi);
        exi = makeexpr_var(tvari);
    }
    exd = bumpstring(exd, exi, 1);
    sp = makestmt_seq(sp, makestmt_assign(exd,
                                          makeexpr_concat(exs, copyexpr(exd), 0)));
    return sp;
}



Static Stmt *proc_strmove()
{
    Expr *exlen, *exs, *exsi, *exd, *exdi;

    if (!skipopenparen())
	return NULL;
    exlen = p_expr(tp_integer);
    if (!skipcomma())
	return NULL;
    exs = p_expr(tp_str255);
    if (!skipcomma())
	return NULL;
    exsi = p_expr(tp_integer);
    if (!skipcomma())
	return NULL;
    exd = p_expr(tp_str255);
    if (!skipcomma())
	return NULL;
    exdi = p_expr(tp_integer);
    skipcloseparen();
    exsi = makeexpr_arglong(exsi, 0);
    exdi = makeexpr_arglong(exdi, 0);
    return makestmt_call(makeexpr_bicall_5(strmovename, tp_str255,
					   exlen, exs, exsi, exd, exdi));
}



Static Expr *func_strlen(ex)
Expr *ex;
{
    return makeexpr_bicall_1("strlen", tp_int, grabarg(ex, 0));
}



Static Expr *func_strltrim(ex)
Expr *ex;
{
    return makeexpr_assign(makeexpr_hat(ex->args[0], 0),
                           makeexpr_bicall_1(strltrimname, tp_str255, ex->args[1]));
}



Static Expr *func_strmax(ex)
Expr *ex;
{
    return strmax_func(grabarg(ex, 0));
}



Static Expr *func_strpos(ex)
Expr *ex;
{
    char *cp;

    if (!switch_strpos)
        swapexprs(ex->args[0], ex->args[1]);
    cp = strposname;
    if (!*cp) {
        note("STRPOS function used [201]");
        cp = "STRPOS";
    } 
    return makeexpr_bicall_3(cp, tp_int,
                             ex->args[0], 
                             ex->args[1],
                             makeexpr_long(1));
}



Static Expr *func_strrpt(ex)
Expr *ex;
{
    if (ex->args[1]->kind == EK_CONST &&
        ex->args[1]->val.i == 1 && ex->args[1]->val.s[0] == ' ') {
        return makeexpr_bicall_4("sprintf", tp_strptr, ex->args[0],
                                 makeexpr_string("%*s"),
                                 makeexpr_longcast(ex->args[2], 0),
                                 makeexpr_string(""));
    } else
        return makeexpr_bicall_3(strrptname, tp_strptr, ex->args[0], ex->args[1],
                                 makeexpr_arglong(ex->args[2], 0));
}



Static Expr *func_strrtrim(ex)
Expr *ex;
{
    return makeexpr_bicall_1(strrtrimname, tp_strptr,
                             makeexpr_assign(makeexpr_hat(ex->args[0], 0),
                                             ex->args[1]));
}



Static Expr *func_succ()
{
    Expr *ex;

    if (wneedtok(TOK_LPAR)) {
	ex = p_ord_expr();
	skipcloseparen();
    } else
	ex = p_ord_expr();
#if 1
    ex = makeexpr_inc(ex, makeexpr_long(1));
#else
    ex = makeexpr_cast(makeexpr_plus(ex, makeexpr_long(1)), ex->val.type);
#endif
    return ex;
}



Static Expr *func_sqr()
{
    return makeexpr_sqr(p_parexpr(tp_integer), 0);
}



Static Expr *func_sqrt(ex)
Expr *ex;
{
    return makeexpr_bicall_1("sqrt", tp_longreal, grabarg(ex, 0));
}



Static Expr *func_swap(ex)
Expr *ex;
{
    char *cp;

    ex = grabarg(ex, 0);
    cp = swapname;
    if (!*cp) {
        note("SWAP function was used [202]");
        cp = "SWAP";
    }
    return makeexpr_bicall_1(swapname, tp_int, ex);
}



Static Expr *func_tan(ex)
Expr *ex;
{
    return makeexpr_bicall_1("tan", tp_longreal, grabarg(ex, 0));
}


Static Expr *func_tanh(ex)
Expr *ex;
{
    return makeexpr_bicall_1("tanh", tp_longreal, grabarg(ex, 0));
}



Static Expr *func_trunc(ex)
Expr *ex;
{
    return makeexpr_actcast(grabarg(ex, 0), tp_integer);
}



Static Expr *func_utrunc(ex)
Expr *ex;
{
    return makeexpr_actcast(grabarg(ex, 0), tp_unsigned);
}



Static Expr *func_uand()
{
    Expr *ex;

    if (!skipopenparen())
	return NULL;
    ex = p_expr(tp_unsigned);
    if (skipcomma()) {
	ex = makeexpr_bin(EK_BAND, ex->val.type, ex, p_expr(tp_unsigned));
	skipcloseparen();
    }
    return ex;
}



Static Expr *func_udec()
{
    return handle_vax_hex(NULL, "u", 0);
}



Static Expr *func_unot()
{
    Expr *ex;

    if (!skipopenparen())
	return NULL;
    ex = p_expr(tp_unsigned);
    ex = makeexpr_un(EK_BNOT, ex->val.type, ex);
    skipcloseparen();
    return ex;
}



Static Expr *func_uor()
{
    Expr *ex;

    if (!skipopenparen())
	return NULL;
    ex = p_expr(tp_unsigned);
    if (skipcomma()) {
	ex = makeexpr_bin(EK_BOR, ex->val.type, ex, p_expr(tp_unsigned));
	skipcloseparen();
    }
    return ex;
}



Static Expr *func_upcase(ex)
Expr *ex;
{
    return makeexpr_bicall_1("toupper", tp_char, grabarg(ex, 0));
}



Static Expr *func_upper()
{
    Expr *ex;
    Value val;

    if (!skipopenparen())
	return NULL;
    ex = p_expr(tp_integer);
    if (curtok == TOK_COMMA) {
	gettok();
	val = p_constant(tp_integer);
	if (!val.type || val.i != 1)
	    note("UPPER(v,n) not supported for n>1 [190]");
    }
    skipcloseparen();
    return copyexpr(ex->val.type->indextype->smax);
}



Static Expr *func_uxor()
{
    Expr *ex;

    if (!skipopenparen())
	return NULL;
    ex = p_expr(tp_unsigned);
    if (skipcomma()) {
	ex = makeexpr_bin(EK_BXOR, ex->val.type, ex, p_expr(tp_unsigned));
	skipcloseparen();
    }
    return ex;
}



Static Expr *func_val_modula()
{
    Expr *ex;
    Type *tp;

    if (!skipopenparen())
	return NULL;
    tp = p_type(NULL);
    if (!skipcomma())
	return NULL;
    ex = p_expr(tp);
    skipcloseparen();
    return pascaltypecast(tp, ex);
}



Static Stmt *proc_val_turbo()
{
    Expr *ex, *vex, *code, *fmt;

    if (!skipopenparen())
	return NULL;
    ex = gentle_cast(p_expr(tp_str255), tp_str255);
    if (!skipcomma())
	return NULL;
    vex = p_expr(NULL);
    if (curtok == TOK_COMMA) {
	gettok();
	code = gentle_cast(p_expr(tp_integer), tp_integer);
    } else
	code = NULL;
    skipcloseparen();
    if (vex->val.type->kind == TK_REAL)
        fmt = makeexpr_string("%lg");
    else if (exprlongness(vex) > 0)
        fmt = makeexpr_string("%ld");
    else
        fmt = makeexpr_string("%d");
    ex = makeexpr_bicall_3("sscanf", tp_int,
                           ex, fmt, makeexpr_addr(vex));
    if (code) {
	ex = makeexpr_rel(EK_EQ, ex, makeexpr_long(0));
	return makestmt_assign(code, makeexpr_ord(ex));
    } else
	return makestmt_call(ex);
}







Static Expr *writestrelement(ex, wid, vex, code, needboth)
Expr *ex, *wid, *vex;
int code, needboth;
{
    if (formatstrings && needboth) {
        return makeexpr_bicall_5("sprintf", tp_str255, vex,
                                 makeexpr_string(format_d("%%*.*%c", code)),
                                 copyexpr(wid),
                                 wid,
                                 ex);
    } else {
        return makeexpr_bicall_4("sprintf", tp_str255, vex,
                                 makeexpr_string(format_d("%%*%c", code)),
                                 wid,
                                 ex);
    }
}



Static char *makeenumnames(tp)
Type *tp;
{
    Strlist *sp;
    char *name;
    Meaning *mp;
    int saveindent;

    for (sp = enumnames; sp && sp->value != (long)tp; sp = sp->next) ;
    if (!sp) {
        if (tp->meaning)
            name = format_s(name_ENUM, tp->meaning->name);
        else
            name = format_s(name_ENUM, format_d("_%d", ++enumnamecount));
        sp = strlist_insert(&enumnames, name);
        sp->value = (long)tp;
        outsection(2);
        output(format_s("static %s *", charname));
        output(sp->s);
        output("[] = {\n");
	saveindent = outindent;
	moreindent(tabsize);
	moreindent(structinitindent);
        for (mp = tp->fbase; mp; mp = mp->xnext) {
            output(makeCstring(mp->sym->name, strlen(mp->sym->name)));
            if (mp->xnext)
                output(",\002 ");
        }
        outindent = saveindent;
        output("\n} ;\n");
        outsection(2);
    }
    return sp->s;
}





/* This function must return a "tempsprintf" */

Expr *writeelement(ex, wid, prec, base)
Expr *ex, *wid, *prec;
int base;
{
    Expr *vex, *ex1, *ex2;
    Meaning *tvar;
    char *fmtcode;
    Type *type;

    ex = makeexpr_charcast(ex);
    if (ex->val.type->kind == TK_POINTER) {
        ex = makeexpr_hat(ex, 0);   /* convert char *'s to strings */
        intwarning("writeelement", "got a char * instead of a string [214]");
    }
    if ((ex->val.type->kind == TK_STRING && !wid) ||
        (ord_type(ex->val.type)->kind == TK_CHAR && (!wid || checkconst(wid, 1)))) {
        return makeexpr_sprintfify(ex);
    }
    tvar = makestmttempvar(tp_str255, name_STRING);
    vex = makeexpr_var(tvar);
    if (wid)
        wid = makeexpr_longcast(wid, 0);
    if (prec)
        prec = makeexpr_longcast(prec, 0);
#if 0
    if (wid && (wid->kind == EK_CONST && wid->val.i < 0 ||
                checkconst(wid, -1))) {
        freeexpr(wid);     /* P-system uses write(x:-1) to mean write(x) */
        wid = NULL;
    }
    if (prec && (prec->kind == EK_CONST && prec->val.i < 0 ||
                 checkconst(prec, -1))) {
        freeexpr(prec);
        prec = NULL;
    }
#endif
    switch (ord_type(ex->val.type)->kind) {

        case TK_INTEGER:
            if (!wid) {
		if (integerwidth < 0)
		    integerwidth = (which_lang == LANG_TURBO) ? 1 : 12;
		wid = makeexpr_long(integerwidth);
	    }
	    type = findbasetype(ex->val.type, ODECL_NOPRES);
	    if (base == 16)
		fmtcode = "x";
	    else if (base == 8)
		fmtcode = "o";
	    else if ((possiblesigns(wid) & (1|4)) == 1) {
		wid = makeexpr_neg(wid);
		fmtcode = "x";
	    } else if (type == tp_unsigned ||
		       type == tp_uint ||
		       (type == tp_ushort && sizeof_int < 32))
		fmtcode = "u";
	    else
		fmtcode = "d";
            ex = makeexpr_forcelongness(ex);
            if (checkconst(wid, 0) || checkconst(wid, 1)) {
                ex = makeexpr_bicall_3("sprintf", tp_str255, vex,
                                       makeexpr_string(format_ss("%%%s%s",
								 (exprlongness(ex) > 0) ? "l" : "",
								 fmtcode)),
                                       ex);
            } else {
                ex = makeexpr_bicall_4("sprintf", tp_str255, vex,
                                       makeexpr_string(format_ss("%%*%s%s",
								 (exprlongness(ex) > 0) ? "l" : "",
								 fmtcode)),
                                       wid,
                                       ex);
            }
            break;

        case TK_CHAR:
            ex = writestrelement(ex, wid, vex, 'c',
                                     (wid->kind != EK_CONST || wid->val.i < 1));
            break;

        case TK_BOOLEAN:
            if (!wid) {
                ex = makeexpr_bicall_3("sprintf", tp_str255, vex,
                                       makeexpr_string("%s"),
                                       makeexpr_cond(ex,
                                                     makeexpr_string(" TRUE"),
                                                     makeexpr_string("FALSE")));
            } else if (checkconst(wid, 1)) {
                ex = makeexpr_bicall_3("sprintf", tp_str255, vex,
                                       makeexpr_string("%c"),
                                       makeexpr_cond(ex,
                                                     makeexpr_char('T'),
                                                     makeexpr_char('F')));
            } else {
                ex = writestrelement(makeexpr_cond(ex,
                                                   makeexpr_string("TRUE"),
                                                   makeexpr_string("FALSE")),
                                     wid, vex, 's',
                                     (wid->kind != EK_CONST || wid->val.i < 5));
            }
            break;

        case TK_ENUM:
            ex = makeexpr_bicall_3("sprintf", tp_str255, vex,
                                   makeexpr_string("%s"),
                                   makeexpr_index(makeexpr_name(makeenumnames(ex->val.type),
                                                                tp_strptr),
                                                  ex, NULL));
            break;

        case TK_REAL:
            if (!wid)
                wid = makeexpr_long(realwidth);
            if (prec && (possiblesigns(prec) & (1|4)) != 1) {
                ex = makeexpr_bicall_5("sprintf", tp_str255, vex,
                                       makeexpr_string("%*.*f"),
                                       wid,
                                       prec,
                                       ex);
            } else {
		if (prec)
		    prec = makeexpr_neg(prec);
		else
		    prec = makeexpr_minus(copyexpr(wid),
					  makeexpr_long(7));
		if (prec->kind == EK_CONST) {
		    if (prec->val.i <= 0)
			prec = makeexpr_long(1);
		} else {
		    prec = makeexpr_bicall_2("P_max", tp_integer, prec,
					     makeexpr_long(1));
		}
                if (wid->kind == EK_CONST && wid->val.i > 21) {
                    ex = makeexpr_bicall_5("sprintf", tp_str255, vex,
                                           makeexpr_string("%*.*E"),
                                           wid,
					   prec,
                                           ex);
#if 0
                } else if (checkconst(wid, 7)) {
                    ex = makeexpr_bicall_3("sprintf", tp_str255, vex,
                                           makeexpr_string("%E"),
                                           ex);
#endif
                } else {
                    ex = makeexpr_bicall_4("sprintf", tp_str255, vex,
                                           makeexpr_string("% .*E"),
					   prec,
                                           ex);
                }
            }
            break;

        case TK_STRING:
            ex = writestrelement(ex, wid, vex, 's', 1);
            break;

        case TK_ARRAY:     /* assume packed array of char */
	    ord_range_expr(ex->val.type->indextype, &ex1, &ex2);
	    ex1 = makeexpr_plus(makeexpr_minus(copyexpr(ex2),
					       copyexpr(ex1)),
				makeexpr_long(1));
	    ex1 = makeexpr_longcast(ex1, 0);
	    fmtcode = "%.*s";
            if (!wid) {
		wid = ex1;
            } else {
		if (isliteralconst(wid, NULL) == 2 &&
		    isliteralconst(ex1, NULL) == 2) {
		    if (wid->val.i > ex1->val.i) {
			fmtcode = format_ds("%*s%%.*s",
					    wid->val.i - ex1->val.i, "");
			wid = ex1;
		    }
		} else
		    note("Format for packed-array-of-char will work only if width < length [321]");
	    }
            ex = makeexpr_bicall_4("sprintf", tp_str255, vex,
                                   makeexpr_string(fmtcode),
                                   wid,
                                   makeexpr_addr(ex));
            break;

        default:
            note("Element has wrong type for WRITE statement [196]");
            ex = makeexpr_bicall_2("sprintf", tp_str255, vex, makeexpr_string("<meef>"));
            break;

    }
    return ex;
}



Static Stmt *handlewrite_text(fex, ex, iswriteln)
Expr *fex, *ex;
int iswriteln;
{
    Expr *print, *wid, *prec;
    unsigned char *ucp;
    int i, done, base;

    print = NULL;
    for (;;) {
        wid = NULL;
        prec = NULL;
	base = 10;
	if (curtok == TOK_COLON && iswriteln >= 0) {
	    gettok();
	    wid = p_expr(tp_integer);
	    if (curtok == TOK_COLON) {
		gettok();
		prec = p_expr(tp_integer);
	    }
	}
	if (curtok == TOK_IDENT &&
	    !strcicmp(curtokbuf, "OCT")) {
	    base = 8;
	    gettok();
	} else if (curtok == TOK_IDENT &&
		   !strcicmp(curtokbuf, "HEX")) {
	    base = 16;
	    gettok();
	}
        ex = writeelement(ex, wid, prec, base);
        print = makeexpr_concat(print, cleansprintf(ex), 1);
        if (curtok == TOK_COMMA && iswriteln >= 0) {
            gettok();
            ex = p_expr(NULL);
        } else
            break;
    }
    if (fex->val.type->kind != TK_STRING) {      /* not strwrite */
        switch (iswriteln) {
            case 1:
            case -1:
                print = makeexpr_concat(print, makeexpr_string("\n"), 1);
                break;
            case 2:
            case -2:
                print = makeexpr_concat(print, makeexpr_string("\r"), 1);
                break;
        }
        if (isvar(fex, mp_output)) {
            ucp = (unsigned char *)print->args[1]->val.s;
            for (i = 0; i < print->args[1]->val.i; i++) {
                if (ucp[i] >= 128 && ucp[i] < 144) {
                    note("WRITE statement contains color/attribute characters [203]");
		    break;
		}
            }
        }
        if ((i = sprintflength(print, 0)) > 0 && print->nargs == 2 && printfonly != 1) {
            print = makeexpr_unsprintfify(print);
            done = 1;
            if (isvar(fex, mp_output)) {
                if (i == 1) {
                    print = makeexpr_bicall_1("putchar", tp_int,
                                              makeexpr_charcast(print));
                } else {
                    if (printfonly == 0) {
                        if (print->val.s[print->val.i-1] == '\n') {
			    print->val.s[--(print->val.i)] = 0;
                            print = makeexpr_bicall_1("puts", tp_int, print);
                        } else {
                            print = makeexpr_bicall_2("fputs", tp_int,
                                                      print,
                                                      copyexpr(fex));
                        }
                    } else {
                        print = makeexpr_sprintfify(print);
                        done = 0;
                    }
                }
            } else {
                if (i == 1) {
                    print = makeexpr_bicall_2("putc", tp_int,
                                              makeexpr_charcast(print),
                                              filebasename(copyexpr(fex)));
                } else if (printfonly == 0) {
                    print = makeexpr_bicall_2("fputs", tp_int,
                                              print,
                                              filebasename(copyexpr(fex)));
                } else {
                    print = makeexpr_sprintfify(print);
                    done = 0;
                }
            }
        } else
            done = 0;
        if (!done) {
            canceltempvar(istempvar(print->args[0]));
            if (checkstring(print->args[1], "%s") && printfonly != 1) {
                print = makeexpr_bicall_2("fputs", tp_int,
                                          grabarg(print, 2),
                                          filebasename(copyexpr(fex)));
            } else if (checkstring(print->args[1], "%c") && printfonly != 1 &&
                       !nosideeffects(print->args[2], 0)) {
                print = makeexpr_bicall_2("fputc", tp_int,
                                          grabarg(print, 2),
                                          filebasename(copyexpr(fex)));
            } else if (isvar(fex, mp_output)) {
                if (checkstring(print->args[1], "%s\n") && printfonly != 1) {
                    print = makeexpr_bicall_1("puts", tp_int, grabarg(print, 2));
                } else if (checkstring(print->args[1], "%c") && printfonly != 1) {
                    print = makeexpr_bicall_1("putchar", tp_int, grabarg(print, 2));
                } else {
                    strchange(&print->val.s, "printf");
                    delfreearg(&print, 0);
                    print->val.type = tp_int;
                }
            } else {
                if (checkstring(print->args[1], "%c") && printfonly != 1) {
                    print = makeexpr_bicall_2("putc", tp_int,
                                              grabarg(print, 2),
                                              filebasename(copyexpr(fex)));
                } else {
                    strchange(&print->val.s, "fprintf");
                    freeexpr(print->args[0]);
                    print->args[0] = filebasename(copyexpr(fex));
                    print->val.type = tp_int;
                }
            }
        }
        if (FCheck(checkfilewrite)) {
            print = makeexpr_bicall_2("~SETIO", tp_void,
                                      makeexpr_rel(EK_GE, print, makeexpr_long(0)),
				      makeexpr_name(filewriteerrorname, tp_int));
        }
    }
    return makestmt_call(print);
}



Static Stmt *handlewrite_bin(fex, ex)
Expr *fex, *ex;
{
    Type *basetype;
    Stmt *sp;
    Expr *tvardef = NULL;
    Meaning *tvar = NULL;

    sp = NULL;
    basetype = filebasetype(fex->val.type);
    for (;;) {
        if (!expr_has_address(ex) || ex->val.type != basetype) {
            if (!tvar)
                tvar = makestmttempvar(basetype, name_TEMP);
            if (!tvardef || !exprsame(tvardef, ex, 1)) {
                freeexpr(tvardef);
                tvardef = copyexpr(ex);
                sp = makestmt_seq(sp, makestmt_assign(makeexpr_var(tvar),
                                                      ex));
            } else
                freeexpr(ex);
            ex = makeexpr_var(tvar);
        }
        ex = makeexpr_bicall_4("fwrite", tp_integer, makeexpr_addr(ex),
                                                     makeexpr_sizeof(makeexpr_type(basetype), 0),
                                                     makeexpr_long(1),
			                             filebasename(copyexpr(fex)));
        if (FCheck(checkfilewrite)) {
            ex = makeexpr_bicall_2("~SETIO", tp_void,
                                   makeexpr_rel(EK_EQ, ex, makeexpr_long(1)),
				   makeexpr_name(filewriteerrorname, tp_int));
        }
        sp = makestmt_seq(sp, makestmt_call(ex));
        if (curtok == TOK_COMMA) {
            gettok();
            ex = p_expr(NULL);
        } else
            break;
    }
    freeexpr(tvardef);
    return sp;
}



Static Stmt *proc_write()
{
    Expr *fex, *ex;
    Stmt *sp;

    if (!skipopenparen())
	return NULL;
    ex = p_expr(NULL);
    if (isfiletype(ex->val.type, -1) && wneedtok(TOK_COMMA)) {
        fex = ex;
        ex = p_expr(NULL);
    } else {
        fex = makeexpr_var(mp_output);
    }
    if (fex->val.type == tp_text || fex->val.type == tp_bigtext)
        sp = handlewrite_text(fex, ex, 0);
    else
        sp = handlewrite_bin(fex, ex);
    skipcloseparen();
    return wrapopencheck(sp, fex);
}



Static Stmt *handle_modula_write(fmt)
char *fmt;
{
    Expr *ex, *wid;

    if (!skipopenparen())
	return NULL;
    ex = makeexpr_forcelongness(p_expr(NULL));
    if (skipcomma())
	wid = p_expr(tp_integer);
    else
	wid = makeexpr_long(1);
    if (checkconst(wid, 0) || checkconst(wid, 1))
	ex = makeexpr_bicall_2("printf", tp_str255,
			       makeexpr_string(format_ss("%%%s%s",
							 (exprlongness(ex) > 0) ? "l" : "",
							 fmt)),
			       ex);
    else
	ex = makeexpr_bicall_3("printf", tp_str255,
			       makeexpr_string(format_ss("%%*%s%s",
							 (exprlongness(ex) > 0) ? "l" : "",
							 fmt)),
			       makeexpr_arglong(wid, 0),
			       ex);
    skipcloseparen();
    return makestmt_call(ex);
}


Static Stmt *proc_writecard()
{
    return handle_modula_write("u");
}


Static Stmt *proc_writeint()
{
    return handle_modula_write("d");
}


Static Stmt *proc_writehex()
{
    return handle_modula_write("x");
}


Static Stmt *proc_writeoct()
{
    return handle_modula_write("o");
}


Static Stmt *proc_writereal()
{
    return handle_modula_write("f");
}



Static Stmt *proc_writedir()
{
    Expr *fex, *ex;
    Stmt *sp;

    if (!skipopenparen())
	return NULL;
    fex = p_expr(tp_text);
    if (!skipcomma())
	return NULL;
    ex = p_expr(tp_integer);
    sp = doseek(fex, ex);
    if (!skipcomma())
	return sp;
    sp = makestmt_seq(sp, handlewrite_bin(fex, p_expr(NULL)));
    skipcloseparen();
    return wrapopencheck(sp, fex);
}



Static Stmt *handlewriteln(iswriteln)
int iswriteln;
{
    Expr *fex, *ex;
    Stmt *sp;
    Meaning *deffile = mp_output;

    sp = NULL;
    if (iswriteln == 3) {
	iswriteln = 1;
	if (messagestderr)
	    deffile = mp_stderr;
    }
    if (curtok != TOK_LPAR) {
        fex = makeexpr_var(deffile);
        if (iswriteln)
            sp = handlewrite_text(fex, makeexpr_string(""), -iswriteln);
    } else {
        gettok();
        ex = p_expr(NULL);
        if (isfiletype(ex->val.type, -1)) {
            fex = ex;
            if (curtok == TOK_RPAR || !wneedtok(TOK_COMMA)) {
                if (iswriteln)
                    ex = makeexpr_string("");
                else
                    ex = NULL;
            } else {
                ex = p_expr(NULL);
            }
        } else {
            fex = makeexpr_var(deffile);
        }
        if (ex)
            sp = handlewrite_text(fex, ex, iswriteln);
        skipcloseparen();
    }
    if (iswriteln == 0) {
        sp = makestmt_seq(sp, makestmt_call(makeexpr_bicall_1("fflush", tp_void,
                                                              filebasename(copyexpr(fex)))));
    }
    return wrapopencheck(sp, fex);
}



Static Stmt *proc_overprint()
{
    return handlewriteln(2);
}



Static Stmt *proc_prompt()
{
    return handlewriteln(0);
}



Static Stmt *proc_writeln()
{
    return handlewriteln(1);
}


Static Stmt *proc_message()
{
    return handlewriteln(3);
}



Static Stmt *proc_writev()
{
    Expr *vex, *ex;
    Stmt *sp;
    Meaning *mp;

    if (!skipopenparen())
	return NULL;
    vex = p_expr(tp_str255);
    if (curtok == TOK_RPAR) {
	gettok();
	return makestmt_assign(vex, makeexpr_string(""));
    }
    if (!skipcomma())
	return NULL;
    sp = handlewrite_text(vex, p_expr(NULL), 0);
    skipcloseparen();
    ex = sp->exp1;
    if (ex->kind == EK_BICALL && !strcmp(ex->val.s, "sprintf") &&
        (mp = istempvar(ex->args[0])) != NULL) {
        canceltempvar(mp);
        ex->args[0] = vex;
    } else
        sp->exp1 = makeexpr_assign(vex, ex);
    return sp;
}


Static Stmt *proc_strwrite(mp_x, spbase)
Meaning *mp_x;
Stmt *spbase;
{
    Expr *vex, *exi, *exj, *ex;
    Stmt *sp;
    Meaning *mp;

    if (!skipopenparen())
	return NULL;
    vex = p_expr(tp_str255);
    if (!skipcomma())
	return NULL;
    exi = p_expr(tp_integer);
    if (!skipcomma())
	return NULL;
    exj = p_expr(tp_integer);
    if (!skipcomma())
	return NULL;
    sp = handlewrite_text(vex, p_expr(NULL), 0);
    skipcloseparen();
    ex = sp->exp1;
    FREE(sp);
    if (checkconst(exi, 1)) {
        sp = spbase;
        while (sp && sp->next)
            sp = sp->next;
        if (sp && sp->kind == SK_ASSIGN && sp->exp1->kind == EK_ASSIGN &&
             (sp->exp1->args[0]->kind == EK_HAT ||
              sp->exp1->args[0]->kind == EK_INDEX) &&
             exprsame(sp->exp1->args[0]->args[0], vex, 1) &&
             checkconst(sp->exp1->args[1], 0)) {
            nukestmt(sp);     /* remove preceding bogus setstrlen */
        }
    }
    if (ex->kind == EK_BICALL && !strcmp(ex->val.s, "sprintf") &&
        (mp = istempvar(ex->args[0])) != NULL) {
        canceltempvar(mp);
        ex->args[0] = bumpstring(copyexpr(vex), exi, 1);
        sp = makestmt_call(ex);
    } else
        sp = makestmt_assign(bumpstring(copyexpr(vex), exi, 1), ex);
    if (fullstrwrite != 0) {
        sp = makestmt_seq(sp, makestmt_assign(exj,
                                              makeexpr_plus(makeexpr_bicall_1("strlen", tp_int, vex),
                                                            makeexpr_long(1))));
        if (fullstrwrite == 1)
            note("FullStrWrite=1 not yet supported [204]");
        if (fullstrwrite == 2)
            note("STRWRITE was used [205]");
    } else {
        freeexpr(vex);
    }
    return mixassignments(sp, NULL);
}



Static Stmt *proc_str_turbo()
{
    Expr *ex, *wid, *prec;

    if (!skipopenparen())
	return NULL;
    ex = p_expr(NULL);
    wid = NULL;
    prec = NULL;
    if (curtok == TOK_COLON) {
        gettok();
        wid = p_expr(tp_integer);
        if (curtok == TOK_COLON) {
            gettok();
            prec = p_expr(tp_integer);
        }
    }
    ex = writeelement(ex, wid, prec, 10);
    if (!skipcomma())
	return NULL;
    wid = p_expr(tp_str255);
    skipcloseparen();
    return makestmt_assign(wid, ex);
}



Static Stmt *proc_time()
{
    Expr *ex;

    if (!skipopenparen())
	return NULL;
    ex = p_expr(tp_str255);
    skipcloseparen();
    return makestmt_call(makeexpr_bicall_1("VAXtime", tp_integer, ex));
}


Static Expr *func_xor()
{
    Expr *ex, *ex2;
    Type *type;
    Meaning *tvar;

    if (!skipopenparen())
	return NULL;
    ex = p_expr(NULL);
    if (!skipcomma())
	return ex;
    ex2 = p_expr(ex->val.type);
    skipcloseparen();
    if (ex->val.type->kind != TK_SET &&
	ex->val.type->kind != TK_SMALLSET) {
	ex = makeexpr_bin(EK_BXOR, ex->val.type, ex, ex2);
    } else {
	type = mixsets(&ex, &ex2);
	tvar = makestmttempvar(type, name_SET);
	ex = makeexpr_bicall_3(setxorname, type,
			       makeexpr_var(tvar),
			       ex, ex2);
    }
    return ex;
}







void decl_builtins()
{
    makespecialfunc( "ABS",           func_abs);
    makespecialfunc( "ADDR",          func_addr);
    if (!modula2)
	makespecialfunc( "ADDRESS",   func_addr);
    makespecialfunc( "ADDTOPOINTER",  func_addtopointer);
    makespecialfunc( "ADR",           func_addr);
    makespecialfunc( "ASL",	      func_lsl);
    makespecialfunc( "ASR",	      func_asr);
    makespecialfunc( "BADDRESS",      func_iaddress);
    makespecialfunc( "BAND",	      func_uand);
    makespecialfunc( "BIN",           func_bin);
    makespecialfunc( "BITNEXT",	      func_bitnext);
    makespecialfunc( "BITSIZE",	      func_bitsize);
    makespecialfunc( "BITSIZEOF",     func_bitsize);
mp_blockread_ucsd =
    makespecialfunc( "BLOCKREAD",     func_blockread);
mp_blockwrite_ucsd =
    makespecialfunc( "BLOCKWRITE",    func_blockwrite);
    makespecialfunc( "BNOT",	      func_unot);
    makespecialfunc( "BOR",	      func_uor);
    makespecialfunc( "BSL",	      func_bsl);
    makespecialfunc( "BSR",	      func_bsr);
    makespecialfunc( "BTST",	      func_btst);
    makespecialfunc( "BXOR",	      func_uxor);
    makespecialfunc( "BYTEREAD",      func_byteread);
    makespecialfunc( "BYTEWRITE",     func_bytewrite);
    makespecialfunc( "BYTE_OFFSET",   func_byte_offset);
    makespecialfunc( "CHR",           func_chr);         
    makespecialfunc( "CONCAT",        func_concat);
    makespecialfunc( "DBLE",          func_float);
mp_dec_dec =
    makespecialfunc( "DEC",           func_dec);
    makespecialfunc( "EOF",           func_eof);
    makespecialfunc( "EOLN",          func_eoln);
    makespecialfunc( "FCALL",         func_fcall);
    makespecialfunc( "FILEPOS",       func_filepos);
    makespecialfunc( "FILESIZE",      func_filesize);
    makespecialfunc( "FLOAT",	      func_float);
    makespecialfunc( "HEX",           func_hex);         
    makespecialfunc( "HI",            func_hi);
    makespecialfunc( "HIWORD",        func_hiword);
    makespecialfunc( "HIWRD",         func_hiword);
    makespecialfunc( "HIGH",          func_high);
    makespecialfunc( "IADDRESS",      func_iaddress);
    makespecialfunc( "INT",           func_int);         
    makespecialfunc( "LAND",	      func_uand);
    makespecialfunc( "LNOT",	      func_unot);
    makespecialfunc( "LO",            func_lo);
    makespecialfunc( "LOOPHOLE",      func_loophole);
    makespecialfunc( "LOR",	      func_uor);
    makespecialfunc( "LOWER",	      func_lower);
    makespecialfunc( "LOWORD",        func_loword);
    makespecialfunc( "LOWRD",         func_loword);
    makespecialfunc( "LSL",	      func_lsl);
    makespecialfunc( "LSR",	      func_lsr);
    makespecialfunc( "MAX",	      func_max);
    makespecialfunc( "MAXPOS",        func_maxpos);
    makespecialfunc( "MIN",	      func_min);
    makespecialfunc( "NEXT",          func_sizeof);
    makespecialfunc( "OCT",           func_oct);
    makespecialfunc( "ORD",           func_ord);
    makespecialfunc( "ORD4",          func_ord4);
    makespecialfunc( "PI",	      func_pi);
    makespecialfunc( "POSITION",      func_position);
    makespecialfunc( "PRED",          func_pred);
    makespecialfunc( "QUAD",          func_float);
    makespecialfunc( "RANDOM",        func_random);
    makespecialfunc( "REF",	      func_addr);
    makespecialfunc( "SCAN",	      func_scan);
    makespecialfunc( "SEEKEOF",       func_seekeof);
    makespecialfunc( "SEEKEOLN",      func_seekeoln);
    makespecialfunc( "SIZE",          func_sizeof);
    makespecialfunc( "SIZEOF",        func_sizeof);
    makespecialfunc( "SNGL",          func_sngl);
    makespecialfunc( "SQR",           func_sqr);
    makespecialfunc( "STATUSV",	      func_statusv);
    makespecialfunc( "SUCC",          func_succ);
    makespecialfunc( "TSIZE",         func_sizeof);
    makespecialfunc( "UAND",	      func_uand);
    makespecialfunc( "UDEC",          func_udec);
    makespecialfunc( "UINT",          func_uint);         
    makespecialfunc( "UNOT",	      func_unot);
    makespecialfunc( "UOR",	      func_uor);
    makespecialfunc( "UPPER",	      func_upper);
    makespecialfunc( "UXOR",	      func_uxor);
mp_val_modula =
    makespecialfunc( "VAL",	      func_val_modula);
    makespecialfunc( "WADDRESS",      func_iaddress);
    makespecialfunc( "XOR",	      func_xor);

    makestandardfunc("ARCTAN",        func_arctan);
    makestandardfunc("ARCTANH",       func_arctanh);
    makestandardfunc("BINARY",        func_binary);      
    makestandardfunc("CAP",           func_upcase);
    makestandardfunc("COPY",          func_copy);        
    makestandardfunc("COS",           func_cos);         
    makestandardfunc("COSH",          func_cosh);         
    makestandardfunc("EXP",           func_exp);         
    makestandardfunc("EXP10",         func_pwroften);
    makestandardfunc("EXPO",          func_expo);         
    makestandardfunc("FRAC",          func_frac);        
    makestandardfunc("INDEX",         func_strpos);      
    makestandardfunc("LASTPOS",       NULL);             
    makestandardfunc("LINEPOS",       NULL);             
    makestandardfunc("LENGTH",        func_strlen);      
    makestandardfunc("LN",            func_ln);          
    makestandardfunc("LOG",           func_log);
    makestandardfunc("LOG10",         func_log);
    makestandardfunc("MAXAVAIL",      func_maxavail);
    makestandardfunc("MEMAVAIL",      func_memavail);
    makestandardfunc("OCTAL",         func_octal);       
    makestandardfunc("ODD",           func_odd);         
    makestandardfunc("PAD",           func_pad);
    makestandardfunc("PARAMCOUNT",    func_paramcount);
    makestandardfunc("PARAMSTR",      func_paramstr);    
    makestandardfunc("POS",           func_pos);         
    makestandardfunc("PTR",           func_ptr);
    makestandardfunc("PWROFTEN",      func_pwroften);
    makestandardfunc("ROUND",         func_round);       
    makestandardfunc("SCANEQ",        func_scaneq);
    makestandardfunc("SCANNE",        func_scanne);
    makestandardfunc("SIN",           func_sin);         
    makestandardfunc("SINH",          func_sinh);         
    makestandardfunc("SQRT",          func_sqrt);        
mp_str_hp =
    makestandardfunc("STR",           func_str_hp);
    makestandardfunc("STRLEN",        func_strlen);      
    makestandardfunc("STRLTRIM",      func_strltrim);    
    makestandardfunc("STRMAX",        func_strmax);      
    makestandardfunc("STRPOS",        func_strpos);      
    makestandardfunc("STRRPT",        func_strrpt);      
    makestandardfunc("STRRTRIM",      func_strrtrim);    
    makestandardfunc("SUBSTR",        func_str_hp);
    makestandardfunc("SWAP",          func_swap);        
    makestandardfunc("TAN",           func_tan);       
    makestandardfunc("TANH",          func_tanh);       
    makestandardfunc("TRUNC",         func_trunc);       
    makestandardfunc("UPCASE",        func_upcase);      
    makestandardfunc("UROUND",        func_uround);
    makestandardfunc("UTRUNC",        func_utrunc);

    makespecialproc( "APPEND",        proc_append);
    makespecialproc( "ARGV",	      proc_argv);
    makespecialproc( "ASSERT",        proc_assert);
    makespecialproc( "ASSIGN",        proc_assign);
    makespecialproc( "BCLR",	      proc_bclr);
mp_blockread_turbo =
    makespecialproc( "BLOCKREAD_TURBO", proc_blockread);
mp_blockwrite_turbo =
    makespecialproc( "BLOCKWRITE_TURBO", proc_blockwrite);
    makespecialproc( "BREAK",         proc_flush);
    makespecialproc( "BSET",	      proc_bset);
    makespecialproc( "CALL",          proc_call);
    makespecialproc( "CLOSE",         proc_close);
    makespecialproc( "CONNECT",       proc_assign);
    makespecialproc( "CYCLE",	      proc_cycle);
    makespecialproc( "DATE",	      proc_date);
mp_dec_turbo =
    makespecialproc( "DEC_TURBO",     proc_dec);
    makespecialproc( "DISPOSE",       proc_dispose);
    makespecialproc( "ESCAPE",        proc_escape);
    makespecialproc( "EXCL",          proc_excl);
    makespecialproc( "EXIT",          proc_exit);
    makespecialproc( "FILLCHAR",      proc_fillchar);
    makespecialproc( "FLUSH",         proc_flush);
    makespecialproc( "GET",           proc_get);
    makespecialproc( "HALT",          proc_escape);
    makespecialproc( "INC",           proc_inc);
    makespecialproc( "INCL",          proc_incl);
    makespecialproc( "LEAVE",	      proc_leave);
    makespecialproc( "LOCATE",        proc_seek);
    makespecialproc( "MESSAGE",       proc_message);
    makespecialproc( "MOVE_FAST",     proc_move_fast);        
    makespecialproc( "MOVE_L_TO_R",   proc_move_fast);        
    makespecialproc( "MOVE_R_TO_L",   proc_move_fast);        
    makespecialproc( "NEW",           proc_new);
    if (which_lang != LANG_VAX)
	makespecialproc( "OPEN",      proc_open);
    makespecialproc( "OVERPRINT",     proc_overprint);
    makespecialproc( "PACK",          proc_pack);
    makespecialproc( "PAGE",          proc_page);
    makespecialproc( "PUT",           proc_put);
    makespecialproc( "PROMPT",        proc_prompt);
    makespecialproc( "RANDOMIZE",     proc_randomize);
    makespecialproc( "READ",          proc_read);
    makespecialproc( "READDIR",       proc_readdir);
    makespecialproc( "READLN",        proc_readln);
    makespecialproc( "READV",         proc_readv);
    makespecialproc( "RESET",         proc_reset);
    makespecialproc( "REWRITE",       proc_rewrite);
    makespecialproc( "SEEK",          proc_seek);
    makespecialproc( "SETSTRLEN",     proc_setstrlen);
    makespecialproc( "SETTEXTBUF",    proc_settextbuf);
mp_str_turbo =
    makespecialproc( "STR_TURBO",     proc_str_turbo);
    makespecialproc( "STRAPPEND",     proc_strappend);
    makespecialproc( "STRDELETE",     proc_strdelete);
    makespecialproc( "STRINSERT",     proc_strinsert);
    makespecialproc( "STRMOVE",       proc_strmove);
    makespecialproc( "STRREAD",       proc_strread);
    makespecialproc( "STRWRITE",      proc_strwrite);
    makespecialproc( "TIME",	      proc_time);
    makespecialproc( "UNPACK",        proc_unpack);
    makespecialproc( "WRITE",         proc_write);
    makespecialproc( "WRITEDIR",      proc_writedir);
    makespecialproc( "WRITELN",       proc_writeln);
    makespecialproc( "WRITEV",        proc_writev);
mp_val_turbo =
    makespecialproc( "VAL_TURBO",     proc_val_turbo);

    makestandardproc("DELETE",        proc_delete);      
    makestandardproc("FREEMEM",       proc_freemem);     
    makestandardproc("GETMEM",        proc_getmem);
    makestandardproc("GOTOXY",        proc_gotoxy);      
    makestandardproc("INSERT",        proc_insert);      
    makestandardproc("MARK",          NULL);             
    makestandardproc("MOVE",          proc_move);        
    makestandardproc("MOVELEFT",      proc_move);        
    makestandardproc("MOVERIGHT",     proc_move);        
    makestandardproc("RELEASE",       NULL);             

    makespecialvar(  "MEM",           var_mem);
    makespecialvar(  "MEMW",          var_memw);
    makespecialvar(  "MEML",          var_meml);
    makespecialvar(  "PORT",          var_port);
    makespecialvar(  "PORTW",         var_portw);

    /* Modula-2 standard I/O procedures (case-sensitive!) */
    makespecialproc( "Read",          proc_read);
    makespecialproc( "ReadCard",      proc_read);
    makespecialproc( "ReadInt",       proc_read);
    makespecialproc( "ReadReal",      proc_read);
    makespecialproc( "ReadString",    proc_read);
    makespecialproc( "Write",         proc_write);
    makespecialproc( "WriteCard",     proc_writecard);
    makespecialproc( "WriteHex",      proc_writehex);
    makespecialproc( "WriteInt",      proc_writeint);
    makespecialproc( "WriteOct",      proc_writeoct);
    makespecialproc( "WriteLn",       proc_writeln);
    makespecialproc( "WriteReal",     proc_writereal);
    makespecialproc( "WriteString",   proc_write);
}




/* End. */



