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



#define PROTO_PEXPR_C
#include "trans.h"




Expr *dots_n_hats(ex, target)
Expr *ex;
Type *target;
{
    Expr *ex2, *ex3;
    Type *tp, *tp2;
    Meaning *mp, *tvar;
    int hassl;

    for (;;) {
	if ((ex->val.type->kind == TK_PROCPTR ||
	     ex->val.type->kind == TK_CPROCPTR) &&
	    curtok != TOK_ASSIGN &&
	    ((mp = (tp2 = ex->val.type)->basetype->fbase) == NULL ||
	     (mp->isreturn && mp->xnext == NULL) ||
	     curtok == TOK_LPAR) &&
	    (tp2->basetype->basetype != tp_void || target == tp_void) &&
	    (!target || (target->kind != TK_PROCPTR &&
			 target->kind != TK_CPROCPTR))) {
	    hassl = tp2->escale;
	    ex2 = ex;
	    ex3 = copyexpr(ex2);
	    if (hassl != 0)
		ex3 = makeexpr_cast(makeexpr_dotq(ex3, "proc", tp_anyptr),
				    makepointertype(tp2->basetype));
	    ex = makeexpr_un(EK_SPCALL, tp2->basetype->basetype, ex3);
	    if (mp && mp->isreturn) {  /* pointer to buffer for return value */
		tvar = makestmttempvar(ex->val.type->basetype,
				       (ex->val.type->basetype->kind == TK_STRING) ? name_STRING : name_TEMP);
		insertarg(&ex, 1, makeexpr_addr(makeexpr_var(tvar)));
		mp = mp->xnext;
	    }
	    if (mp) {
		if (wneedtok(TOK_LPAR)) {
		    ex = p_funcarglist(ex, mp, 0, 0);
		    skipcloseparen();
		}
	    } else if (curtok == TOK_LPAR) {
		gettok();
		if (!wneedtok(TOK_RPAR))
		    skippasttoken(TOK_RPAR);
	    }
	    if (hassl != 1 || hasstaticlinks == 2) {
		freeexpr(ex2);
	    } else {
		ex2 = makeexpr_dotq(ex2, "link", tp_anyptr),
		ex3 = copyexpr(ex);
		insertarg(&ex3, ex3->nargs, copyexpr(ex2));
		tp = maketype(TK_FUNCTION);
		tp->basetype = tp2->basetype->basetype;
		tp->fbase = tp2->basetype->fbase;
		tp->issigned = 1;
		ex3->args[0]->val.type = makepointertype(tp);
		ex = makeexpr_cond(makeexpr_rel(EK_NE, ex2, makeexpr_nil()),
				   ex3, ex);
	    }
	    if (tp2->basetype->fbase &&
		tp2->basetype->fbase->isreturn &&
		tp2->basetype->fbase->kind == MK_VARPARAM)
		ex = makeexpr_hat(ex, 0);    /* returns pointer to structured result */
	    continue;
	}
        switch (curtok) {

            case TOK_HAT:
	    case TOK_ADDR:
                gettok();
                ex = makeexpr_hat(ex, 1);
                break;

            case TOK_LBR:
                do {
                    gettok();
		    ex2 = p_ord_expr();
		    ex = p_index(ex, ex2);
                } while (curtok == TOK_COMMA);
                if (!wneedtok(TOK_RBR))
		    skippasttotoken(TOK_RBR, TOK_SEMI);
                break;

            case TOK_DOT:
                gettok();
                if (!wexpecttok(TOK_IDENT))
		    break;
		if (ex->val.type->kind == TK_STRING) {
		    if (!strcicmp(curtokbuf, "LENGTH")) {
			ex = makeexpr_bicall_1("strlen", tp_int, ex);
		    } else if (!strcicmp(curtokbuf, "BODY")) {
			/* nothing to do */
		    }
		    gettok();
		    break;
		}
                mp = curtoksym->fbase;
                while (mp && mp->rectype != ex->val.type)
                    mp = mp->snext;
                if (mp)
                    ex = makeexpr_dot(ex, mp);
                else {
                    warning(format_s("No field called %s in that record [288]", curtokbuf));
		    ex = makeexpr_dotq(ex, curtokcase, tp_integer);
		}
                gettok();
                break;

	    case TOK_COLONCOLON:
		gettok();
		if (wexpecttok(TOK_IDENT)) {
		    ex = pascaltypecast(curtokmeaning->type, ex);
		    gettok();
		}
		break;

            default:
                return ex;
        }
    }
}


Expr *p_index(ex, ex2)
Expr *ex, *ex2;
{
    Expr *ex3;
    Type *tp, *ot;
    Meaning *mp;
    int bits;

    tp = ex->val.type;
    if (tp->kind == TK_STRING) {
	if (checkconst(ex2, 0))   /* is it "s[0]"? */
	    return makeexpr_bicall_1("strlen", tp_char, ex);
	else
	    return makeexpr_index(ex, ex2, makeexpr_long(1));
    } else if (tp->kind == TK_ARRAY ||
	       tp->kind == TK_SMALLARRAY) {
	if (tp->smax) {
	    ord_range_expr(tp->indextype, &ex3, NULL);
	    ex2 = makeexpr_minus(ex2, copyexpr(ex3));
	    if (!nodependencies(ex2, 0) &&
		*getbitsname == '*') {
		mp = makestmttempvar(tp_integer, name_TEMP);
		ex3 = makeexpr_assign(makeexpr_var(mp), ex2);
		ex2 = makeexpr_var(mp);
	    } else
		ex3 = NULL;
	    ex = makeexpr_bicall_3(getbitsname, tp_int,
				   ex, ex2,
				   makeexpr_long(tp->escale));
	    if (tp->kind == TK_ARRAY) {
		if (tp->basetype == tp_sshort)
		    bits = 4;
		else
		    bits = 3;
		insertarg(&ex, 3, makeexpr_long(bits));
	    }
	    ex = makeexpr_comma(ex3, ex);
	    ot = ord_type(tp->smax->val.type);
	    if (ot->kind == TK_ENUM && ot->meaning && useenum)
		ex = makeexpr_cast(ex, tp->smax->val.type);
	    ex->val.type = tp->smax->val.type;
	    return ex;
	} else {
	    ord_range_expr(ex->val.type->indextype, &ex3, NULL);
	    if (debug>2) { fprintf(outf, "ord_range_expr returns "); dumpexpr(ex3); fprintf(outf, "\n"); }
	    return makeexpr_index(ex, ex2, copyexpr(ex3));
	}
    } else {
	warning("Index on a non-array variable [287]");
	return makeexpr_bin(EK_INDEX, tp_integer, ex, ex2);
    }
}


Expr *fake_dots_n_hats(ex)
Expr *ex;
{
    for (;;) {
        switch (curtok) {

            case TOK_HAT:
	    case TOK_ADDR:
	        if (ex->val.type->kind == TK_POINTER)
		    ex = makeexpr_hat(ex, 0);
		else {
		    ex->val.type = makepointertype(ex->val.type);
		    ex = makeexpr_un(EK_HAT, ex->val.type->basetype, ex);
		}
                gettok();
                break;

            case TOK_LBR:
                do {
                    gettok();
                    ex = makeexpr_bin(EK_INDEX, tp_integer, ex, p_expr(tp_integer));
                } while (curtok == TOK_COMMA);
                if (!wneedtok(TOK_RBR))
		    skippasttotoken(TOK_RBR, TOK_SEMI);
                break;

            case TOK_DOT:
                gettok();
                if (!wexpecttok(TOK_IDENT))
		    break;
                ex = makeexpr_dotq(ex, curtokcase, tp_integer);
                gettok();
                break;

	    case TOK_COLONCOLON:
		gettok();
		if (wexpecttok(TOK_IDENT)) {
		    ex = pascaltypecast(curtokmeaning->type, ex);
		    gettok();
		}
		break;

            default:
                return ex;
        }
    }
}



Static void bindnames(ex)
Expr *ex;
{
    int i;
    Symbol *sp;
    Meaning *mp;

    if (ex->kind == EK_NAME) {
	sp = findsymbol_opt(fixpascalname(ex->val.s));
	if (sp) {
	    mp = sp->mbase;
	    while (mp && !mp->isactive)
		mp = mp->snext;
	    if (mp && !strcmp(mp->name, ex->val.s)) {
		ex->kind = EK_VAR;
		ex->val.i = (long)mp;
		ex->val.type = mp->type;
	    }
	}
    }
    i = ex->nargs;
    while (--i >= 0)
	bindnames(ex->args[i]);
}



void var_reference(mp)
Meaning *mp;
{
    Meaning *mp2;

    mp->refcount++;
    if (mp->ctx && mp->ctx->kind == MK_FUNCTION &&
	mp->ctx->needvarstruct &&
	(mp->kind == MK_VAR ||
	 mp->kind == MK_VARREF ||
	 mp->kind == MK_VARMAC ||
	 mp->kind == MK_PARAM ||
	 mp->kind == MK_VARPARAM ||
	 (mp->kind == MK_CONST &&
	  (mp->type->kind == TK_ARRAY ||
	   mp->type->kind == TK_RECORD)))) {
        if (debug>1) { fprintf(outf, "varstruct'ing %s\n", mp->name); }
        if (!mp->varstructflag) {
            mp->varstructflag = 1;
            if (mp->constdefn &&      /* move init code into function body */
		mp->kind != MK_VARMAC) {
                mp2 = addmeaningafter(mp, curtoksym, MK_VAR);
                curtoksym->mbase = mp2->snext;  /* hide this fake variable */
                mp2->snext = mp;      /* remember true variable */
                mp2->type = mp->type;
                mp2->constdefn = mp->constdefn;
                mp2->isforward = 1;   /* declare it "static" */
                mp2->refcount++;      /* so it won't be purged! */
                mp->constdefn = NULL;
                mp->isforward = 0;
            }
        }
        for (mp2 = curctx->ctx; mp2 != mp->ctx; mp2 = mp2->ctx)
            mp2->varstructflag = 1;
        mp2->varstructflag = 1;
    }
}



Static Expr *p_variable(target)
Type *target;
{
    Expr *ex, *ex2;
    Meaning *mp;
    Symbol *sym;

    if (curtok != TOK_IDENT) {
        warning("Expected a variable [289]");
	return makeexpr_long(0);
    }
    if (!curtokmeaning) {
	sym = curtoksym;
        ex = makeexpr_name(curtokcase, tp_integer);
        gettok();
        if (curtok == TOK_LPAR) {
            ex = makeexpr_bicall_0(ex->val.s, tp_integer);
            do {
                gettok();
                insertarg(&ex, ex->nargs, p_expr(NULL));
            } while (curtok == TOK_COMMA || curtok == TOK_ASSIGN);
            if (!wneedtok(TOK_RPAR))
		skippasttotoken(TOK_RPAR, TOK_SEMI);
        }
	if (!tryfuncmacro(&ex, NULL))
	    undefsym(sym);
        return fake_dots_n_hats(ex);
    }
    var_reference(curtokmeaning);
    mp = curtokmeaning;
    if (mp->kind == MK_FIELD) {
        ex = makeexpr_dot(copyexpr(withexprs[curtokint]), mp);
    } else if (mp->kind == MK_CONST &&
	       mp->type->kind == TK_SET &&
	       mp->constdefn) {
	ex = copyexpr(mp->constdefn);
	mp = makestmttempvar(ex->val.type, name_SET);
        ex2 = makeexpr(EK_MACARG, 0);
        ex2->val.type = ex->val.type;
	ex = replaceexprexpr(ex, ex2, makeexpr_var(mp), 0);
        freeexpr(ex2);
    } else if (mp->kind == MK_CONST &&
               (mp == mp_false ||
                mp == mp_true ||
                mp->anyvarflag ||
                (foldconsts > 0 &&
                 (mp->type->kind == TK_INTEGER ||
                  mp->type->kind == TK_BOOLEAN ||
                  mp->type->kind == TK_CHAR ||
                  mp->type->kind == TK_ENUM ||
                  mp->type->kind == TK_SUBR ||
                  mp->type->kind == TK_REAL)) ||
                (foldstrconsts > 0 &&
                 (mp->type->kind == TK_STRING)))) {
        if (mp->constdefn) {
            ex = copyexpr(mp->constdefn);
            if (ex->val.type == tp_int)   /* kludge! */
                ex->val.type = tp_integer;
        } else
            ex = makeexpr_val(copyvalue(mp->val));
    } else if (mp->kind == MK_VARPARAM ||
               mp->kind == MK_VARREF) {
        ex = makeexpr_hat(makeexpr_var(mp), 0);
    } else if (mp->kind == MK_VARMAC) {
        ex = copyexpr(mp->constdefn);
	bindnames(ex);
        ex = gentle_cast(ex, mp->type);
        ex->val.type = mp->type;
    } else if (mp->kind == MK_SPVAR && mp->handler) {
        gettok();
        ex = (*mp->handler)(mp);
        return dots_n_hats(ex, target);
    } else if (mp->kind == MK_VAR ||
               mp->kind == MK_CONST ||
               mp->kind == MK_PARAM) {
        ex = makeexpr_var(mp);
    } else {
        symclass(mp->sym);
        ex = makeexpr_name(mp->name, tp_integer);
    }
    gettok();
    return dots_n_hats(ex, target);
}




Expr *p_ord_expr()
{
    return makeexpr_charcast(p_expr(tp_integer));
}



Static Expr *makesmallsetconst(bits, type)
long bits;
Type *type;
{
    Expr *ex;

    ex = makeexpr_long(bits);
    ex->val.type = type;
    if (smallsetconst != 2)
        insertarg(&ex, 0, makeexpr_name("%#lx", tp_integer));
    return ex;
}



Expr *packset(ex, type)
Expr *ex;
Type *type;
{
    Meaning *mp;
    Expr *ex2;
    long max2;

    if (ex->kind == EK_BICALL) {
        if (!strcmp(ex->val.s, setexpandname) &&
            (mp = istempvar(ex->args[0])) != NULL) {
            canceltempvar(mp);
            return grabarg(ex, 1);
        }
        if (!strcmp(ex->val.s, setunionname) &&
            (mp = istempvar(ex->args[0])) != NULL &&
            !exproccurs(ex->args[1], ex->args[0]) &&
            !exproccurs(ex->args[2], ex->args[0])) {
            canceltempvar(mp);
            return makeexpr_bin(EK_BOR, type, packset(ex->args[1], type),
                                              packset(ex->args[2], type));
        }
        if (!strcmp(ex->val.s, setaddname)) {
            ex2 = makeexpr_bin(EK_LSH, type,
                               makeexpr_longcast(makeexpr_long(1), 1),
                               ex->args[1]);
            ex = packset(ex->args[0], type);
            if (checkconst(ex, 0))
                return ex2;
            else
                return makeexpr_bin(EK_BOR, type, ex, ex2);
        }
        if (!strcmp(ex->val.s, setaddrangename)) {
            if (ord_range(type->indextype, NULL, &max2) && max2 == setbits-1)
                note("Range construction was implemented by a subtraction which may overflow [278]");
            ex2 = makeexpr_minus(makeexpr_bin(EK_LSH, type,
                                              makeexpr_longcast(makeexpr_long(1), 1),
                                              makeexpr_plus(ex->args[2],
                                                            makeexpr_long(1))),
                                 makeexpr_bin(EK_LSH, type,
                                              makeexpr_longcast(makeexpr_long(1), 1),
                                              ex->args[1]));
            ex = packset(ex->args[0], type);
            if (checkconst(ex, 0))
                return ex2;
            else
                return makeexpr_bin(EK_BOR, type, ex, ex2);
        }
    }
    return makeexpr_bicall_1(setpackname, type, ex);
}



#define MAXSETLIT 400

Expr *p_setfactor(target, sure)
Type *target;
int sure;
{
    Expr *ex, *exmax = NULL, *ex2;
    Expr *first[MAXSETLIT], *last[MAXSETLIT];
    char doneflag[MAXSETLIT];
    int i, j, num, donecount;
    int isconst, guesstype;
    long maxv, max2;
    Value val;
    Type *tp, *type;
    Meaning *tvar;

    if (curtok == TOK_LBRACE)
	gettok();
    else if (!wneedtok(TOK_LBR))
	return makeexpr_long(0);
    if (curtok == TOK_RBR || curtok == TOK_RBRACE) {        /* empty set */
        gettok();
        val.type = tp_smallset;
        val.i = 0;
        val.s = NULL;
        return makeexpr_val(val);
    }
    type = target;
    guesstype = !sure;
    maxv = -1;
    isconst = 1;
    num = 0;
    for (;;) {
        if (num >= MAXSETLIT) {
            warning(format_d("Too many elements in set literal; max=%d [290]", MAXSETLIT));
            ex = p_expr(type);
            while (curtok != TOK_RBR && curtok != TOK_RBRACE) {
                gettok();
                ex = p_expr(type);
            }
            break;
        }
        if (guesstype && num == 0) {
            ex = p_ord_expr();
	    type = ex->val.type;
        } else {
            ex = p_expr(type);
        }
        first[num] = ex = gentle_cast(ex, type);
        doneflag[num] = 0;
        if (curtok == TOK_DOTS || curtok == TOK_COLON) {   /* UCSD? */
            val = eval_expr(ex);
            if (val.type) {
		if (val.i > maxv) {     /* In case of [127..0] */
		    maxv = val.i;
		    exmax = ex;
		}
	    } else
                isconst = 0;
            gettok();
            last[num] = ex = gentle_cast(p_expr(type), type);
        } else {
            last[num] = NULL;
        }
        val = eval_expr(ex);
        if (val.type) {
            if (val.i > maxv) {
                maxv = val.i;
                exmax = ex;
            }
        } else {
            isconst = 0;
            maxv = LONG_MAX;
        }
        num++;
        if (curtok == TOK_COMMA)
            gettok();
        else
            break;
    }
    if (curtok == TOK_RBRACE)
	gettok();
    else if (!wneedtok(TOK_RBR))
	skippasttotoken(TOK_RBR, TOK_SEMI);
    tp = first[0]->val.type;
    if (guesstype) {      /* must determine type */
        if (maxv == LONG_MAX) {
	    if (target && ord_range(target, NULL, &max2))
		maxv = max2;
            else if (ord_range(tp, NULL, &max2) && max2 < 1000000 &&
		     (max2 >= defaultsetsize || num == 1))
                maxv = max2;
	    else
		maxv = defaultsetsize-1;
            exmax = makeexpr_long(maxv);
        } else
            exmax = copyexpr(exmax);
        if (!ord_range(tp, NULL, &max2) || maxv != max2)
            tp = makesubrangetype(tp, makeexpr_long(0), exmax);
        type = makesettype(tp);
    } else
	type = makesettype(type);
    donecount = 0;
    if (smallsetconst > 0) {
        val.i = 0;
        for (i = 0; i < num; i++) {
            if (first[i]->kind == EK_CONST && first[i]->val.i < setbits &&
                (!last[i] || (last[i]->kind == EK_CONST &&
                              last[i]->val.i >= 0 &&
                              last[i]->val.i < setbits))) {
                if (last[i]) {
                    for (j = first[i]->val.i; j <= last[i]->val.i; j++)
                        val.i |= 1<<j;
                } else
		    val.i |= 1 << first[i]->val.i;
                doneflag[i] = 1;
                donecount++;
            }
        }
    }
    if (donecount) {
        ex = makesmallsetconst(val.i, tp_smallset);
    } else
        ex = NULL;
    if (type->kind == TK_SMALLSET) {
        for (i = 0; i < num; i++) {
            if (!doneflag[i]) {
                ex2 = makeexpr_bin(EK_LSH, type,
				   makeexpr_longcast(makeexpr_long(1), 1),
				   enum_to_int(first[i]));
                if (last[i]) {
                    if (ord_range(type->indextype, NULL, &max2) && max2 == setbits-1)
                        note("Range construction was implemented by a subtraction which may overflow [278]");
                    ex2 = makeexpr_minus(makeexpr_bin(EK_LSH, type,
                                                      makeexpr_longcast(makeexpr_long(1), 1),
                                                      makeexpr_plus(enum_to_int(last[i]),
                                                                    makeexpr_long(1))),
                                         ex2);
                }
                if (ex)
                    ex = makeexpr_bin(EK_BOR, type, makeexpr_longcast(ex, 1), ex2);
                else
                    ex = ex2;
            }
        }
    } else {
        tvar = makestmttempvar(type, name_SET);
        if (!ex) {
            val.type = tp_smallset;
	    val.i = 0;
	    val.s = NULL;
	    ex = makeexpr_val(val);
	}
        ex = makeexpr_bicall_2(setexpandname, type,
                               makeexpr_var(tvar), makeexpr_arglong(ex, 1));
        for (i = 0; i < num; i++) {
            if (!doneflag[i]) {
                if (last[i])
                    ex = makeexpr_bicall_3(setaddrangename, type,
                                           ex, makeexpr_arglong(enum_to_int(first[i]), 0),
                                               makeexpr_arglong(enum_to_int(last[i]), 0));
                else
                    ex = makeexpr_bicall_2(setaddname, type,
                                           ex, makeexpr_arglong(enum_to_int(first[i]), 0));
            }
        }
    }
    return ex;
}




Expr *p_funcarglist(ex, args, firstarg, ismacro)
Expr *ex;
Meaning *args;
int firstarg, ismacro;
{
    Meaning *mp, *mp2, *arglist = args, *prevarg = NULL;
    Expr *ex2;
    int i, fi, fakenum = -1, castit, isconf, isnonpos = 0;
    Type *tp, *tp2;
    char *name;

    castit = castargs;
    if (castit < 0)
	castit = (prototypes == 0);
    while (args) {
	if (isnonpos) {
	    while (curtok == TOK_COMMA)
		gettok();
	    if (curtok == TOK_RPAR) {
		args = arglist;
		i = firstarg;
		while (args) {
		    if (ex->nargs <= i)
			insertarg(&ex, ex->nargs, NULL);
		    if (!ex->args[i]) {
			if (args->constdefn)
			    ex->args[i] = copyexpr(args->constdefn);
			else {
			    warning(format_s("Missing value for parameter %s [291]",
					     args->name));
			    ex->args[i] = makeexpr_long(0);
			}
		    }
		    args = args->xnext;
		    i++;
		}
		break;
	    }
	}
	if (args->isreturn || args->fakeparam) {
	    if (args->fakeparam) {
		if (fakenum < 0)
		    fakenum = ex->nargs;
		if (args->constdefn)
		    insertarg(&ex, ex->nargs, copyexpr(args->constdefn));
		else
		    insertarg(&ex, ex->nargs, makeexpr_long(0));
	    }
	    args = args->xnext;     /* return value parameter */
	    continue;
	}
	if (curtok == TOK_RPAR) {
	    if (args->constdefn) {
		insertarg(&ex, ex->nargs, copyexpr(args->constdefn));
		args = args->xnext;
		continue;
	    } else {
		if (ex->kind == EK_FUNCTION) {
		    name = ((Meaning *)ex->val.i)->name;
		    ex->kind = EK_BICALL;
		    ex->val.s = stralloc(name);
		} else
		    name = "function";
		warning(format_s("Too few arguments for %s [292]", name));
		return ex;
	    }
	}
	if (curtok == TOK_COMMA) {
	    if (args->constdefn)
		insertarg(&ex, ex->nargs, copyexpr(args->constdefn));
	    else {
		warning(format_s("Missing parameter %s [293]", args->name));
		insertarg(&ex, ex->nargs, makeexpr_long(0));
	    }
	    gettok();
	    args = args->xnext;
	    continue;
	}
	p_mech_spec(0);
	if (curtok == TOK_IDENT) {
	    mp = arglist;
	    mp2 = NULL;
	    i = firstarg;
	    fi = -1;
	    while (mp && strcmp(curtokbuf, mp->sym->name)) {
		if (mp->fakeparam) {
		    if (fi < 0)
			fi = i;
		} else
		    fi = -1;
		i++;
		mp2 = mp;
		mp = mp->xnext;
	    }
	    if (mp &&
		(peeknextchar() == ':' || !curtokmeaning || isnonpos)) {
		gettok();
		wneedtok(TOK_ASSIGN);
		prevarg = mp2;
		args = mp;
		fakenum = fi;
		isnonpos = 1;
	    } else
		i = ex->nargs;
	} else
	    i = ex->nargs;
	while (ex->nargs <= i)
	    insertarg(&ex, ex->nargs, NULL);
	if (ex->args[i])
	    warning(format_s("Multiple values for parameter %s [294]",
			     args->name));
	tp = args->type;
	ex2 = p_expr(tp);
	if (args->kind == MK_VARPARAM)
	    tp = tp->basetype;
	if (isfiletype(tp, 1) && is_std_file(ex2)) {
	    mp2 = makestmttempvar(tp_bigtext, name_TEMP);
	    ex2 = makeexpr_comma(
		   makeexpr_comma(makeexpr_assign(filebasename(makeexpr_var(mp2)),
						  ex2),
				  makeexpr_assign(filenamepart(makeexpr_var(mp2)),
						  makeexpr_string(""))),
				 makeexpr_var(mp2));
	}
	tp2 = ex2->val.type;
	isconf = ((tp->kind == TK_ARRAY ||
		   tp->kind == TK_STRING) && tp->structdefd);
        switch (args->kind) {

            case MK_PARAM:
	        if (castit && tp->kind == TK_REAL &&
		    ex2->val.type->kind != TK_REAL)
                    ex2 = makeexpr_cast(ex2, tp);
                else if (ord_type(tp)->kind == TK_INTEGER && !ismacro)
                    ex2 = makeexpr_arglong(ex2, long_type(tp));
                else if (args->othername && args->rectype != tp &&
                         tp->kind != TK_STRING && args->type == tp2)
                    ex2 = makeexpr_addr(ex2);
                else
                    ex2 = gentle_cast(ex2, tp);
		ex->args[i] = ex2;
                break;

            case MK_VARPARAM:
                if (args->type == tp_strptr && args->anyvarflag) {
		    ex->args[i] = strmax_func(ex2);
                    insertarg(&ex, ex->nargs-1, makeexpr_addr(ex2));
		    if (isnonpos)
			note("Non-positional conformant parameters may not work [279]");
                } else {                        /* regular VAR parameter */
		    if (!expr_is_lvalue(ex2) ||
			(tp->kind == TK_REAL &&
			 ord_type(tp2)->kind == TK_INTEGER)) {
			mp2 = makestmttempvar(tp, name_TEMP);
			ex2 = makeexpr_comma(makeexpr_assign(makeexpr_var(mp2),
							     ex2),
					     makeexpr_addrf(makeexpr_var(mp2)));
		    } else
			ex2 = makeexpr_addrf(ex2);
                    if (args->anyvarflag ||
                        (tp->kind == TK_POINTER && tp2->kind == TK_POINTER &&
                         (tp == tp_anyptr || tp2 == tp_anyptr))) {
			if (!ismacro)
			    ex2 = makeexpr_cast(ex2, args->type);
                    } else {
                        if (tp2 != tp && !isconf &&
			    (tp2->kind != TK_STRING ||
			     tp->kind != TK_STRING))
                            warning(format_s("Type mismatch in VAR parameter %s [295]",
                                             args->name));
                    }
		    ex->args[i] = ex2;
                }
                break;

	    default:
		intwarning("p_funcarglist",
			   format_s("Parameter type is %s [296]",
				    meaningkindname(args->kind)));
		break;
        }
	if (isconf &&   /* conformant array or string */
	    (!prevarg || prevarg->type != args->type)) {
	    while (tp->kind == TK_ARRAY && tp->structdefd) {
		if (tp2->kind == TK_SMALLARRAY) {
		    warning("Trying to pass a small-array for a conformant array [297]");
		    /* this has a chance of working... */
		    ex->args[ex->nargs-1] =
			makeexpr_addr(ex->args[ex->nargs-1]);
		} else if (tp2->kind == TK_STRING) {
		    ex->args[fakenum++] =
			makeexpr_arglong(makeexpr_long(1), integer16 == 0);
		    ex->args[fakenum++] =
			makeexpr_arglong(strmax_func(ex->args[ex->nargs-1]),
					 integer16 == 0);
		    break;
	        } else if (tp2->kind != TK_ARRAY) {
		    warning("Type mismatch for conformant array [298]");
		    break;
		}
		ex->args[fakenum++] =
		    makeexpr_arglong(copyexpr(tp2->indextype->smin),
				     integer16 == 0);
		ex->args[fakenum++] =
		    makeexpr_arglong(copyexpr(tp2->indextype->smax),
				     integer16 == 0);
		tp = tp->basetype;
		tp2 = tp2->basetype;
	    }
	    if (tp->kind == TK_STRING && tp->structdefd) {
		ex->args[fakenum] =
		    makeexpr_arglong(strmax_func(ex->args[ex->nargs-1]),
				     integer16 == 0);
	    }
	}
	fakenum = -1;
	if (!isnonpos) {
	    prevarg = args;
	    args = args->xnext;
	    if (args) {
		if (curtok != TOK_RPAR && !wneedtok(TOK_COMMA))
		    skiptotoken2(TOK_RPAR, TOK_SEMI);
	    }
	}
    }
    if (curtok == TOK_COMMA) {
	if (ex->kind == EK_FUNCTION) {
	    name = ((Meaning *)ex->val.i)->name;
	    ex->kind = EK_BICALL;
	    ex->val.s = stralloc(name);
	} else
	    name = "function";
	warning(format_s("Too many arguments for %s [299]", name));
	while (curtok == TOK_COMMA) {
	    gettok();
	    insertarg(&ex, ex->nargs, p_expr(tp_integer));
	}
    }
    return ex;
}



Expr *replacemacargs(ex, fex)
Expr *ex, *fex;
{
    int i;
    Expr *ex2;

    for (i = 0; i < ex->nargs; i++)
        ex->args[i] = replacemacargs(ex->args[i], fex);
    if (ex->kind == EK_MACARG) {
	if (ex->val.i <= fex->nargs) {
	    ex2 = copyexpr(fex->args[ex->val.i - 1]);
	} else {
	    ex2 = makeexpr_name("<meef>", tp_integer);
	    note("FuncMacro specified more arguments than call [280]");
	}
	freeexpr(ex);
	return ex2;
    }
    return resimplify(ex);
}


Expr *p_noarglist(ex, mp, args)
Expr *ex;
Meaning *mp, *args;
{
    while (args && args->constdefn) {
	insertarg(&ex, ex->nargs, copyexpr(args->constdefn));
	args = args->xnext;
    }
    if (args) {
	warning(format_s("Expected an argument list for %s [300]", mp->name));
	ex->kind = EK_BICALL;
	ex->val.s = stralloc(mp->name);
    }
    return ex;
}


void func_reference(func)
Meaning *func;
{
    Meaning *mp;

    if (func->ctx && func->ctx != curctx &&func->ctx->kind == MK_FUNCTION &&
	func->ctx->varstructflag && !curctx->ctx->varstructflag) {
	for (mp = curctx->ctx; mp != func->ctx; mp = mp->ctx)
	    mp->varstructflag = 1;
    }
}


Expr *p_funccall(mp)
Meaning *mp;
{
    Meaning *mp2, *tvar;
    Expr *ex, *ex2;
    int firstarg = 0;

    func_reference(mp);
    ex = makeexpr(EK_FUNCTION, 0);
    ex->val.i = (long)mp;
    ex->val.type = mp->type->basetype;
    mp2 = mp->type->fbase;
    if (mp2 && mp2->isreturn) {    /* pointer to buffer for return value */
        tvar = makestmttempvar(ex->val.type->basetype,
            (ex->val.type->basetype->kind == TK_STRING) ? name_STRING : name_TEMP);
        insertarg(&ex, 0, makeexpr_addr(makeexpr_var(tvar)));
        mp2 = mp2->xnext;
	firstarg++;
    }
    if (mp2 && curtok != TOK_LPAR) {
	ex = p_noarglist(ex, mp, mp2);
    } else if (curtok == TOK_LPAR) {
	gettok();
        ex = p_funcarglist(ex, mp2, firstarg, (mp->constdefn != NULL));
        skipcloseparen();
    }
    if (mp->constdefn) {
        ex2 = replacemacargs(copyexpr(mp->constdefn), ex);
	ex2 = gentle_cast(ex2, ex->val.type);
	ex2->val.type = ex->val.type;
        freeexpr(ex);
        return ex2;
    }
    return ex;
}






Expr *accumulate_strlit()
{
    char buf[256], ch, *cp, *cp2;
    int len, i, danger = 0;

    len = 0;
    cp = buf;
    for (;;) {
        if (curtok == TOK_STRLIT) {
            cp2 = curtokbuf;
            i = curtokint;
            while (--i >= 0) {
                if (++len <= 255) {
                    ch = *cp++ = *cp2++;
                    if (ch & 128)
                        danger++;
                }
            }
        } else if (curtok == TOK_HAT) {    /* Turbo */
            i = getchartok() & 0x1f;
            if (++len <= 255)
                *cp++ = i;
	} else if (curtok == TOK_LPAR) {   /* VAX */
	    Value val;
	    do {
		gettok();
		val = p_constant(tp_integer);
		if (++len <= 255)
		    *cp++ = val.i;
	    } while (curtok == TOK_COMMA);
	    skipcloseparen();
	    continue;
        } else
            break;
        gettok();
    }
    if (len > 255) {
        warning("String literal too long [301]");
        len = 255;
    }
    if (danger &&
        !(unsignedchar == 1 ||
          (unsignedchar != 0 && signedchars == 0)))
        note(format_s("Character%s >= 128 encountered [281]", (danger > 1) ? "s" : ""));
    return makeexpr_lstring(buf, len);
}



Expr *pascaltypecast(type, ex2)
Type *type;
Expr *ex2;
{
    if (type->kind == TK_POINTER || type->kind == TK_STRING ||
	type->kind == TK_ARRAY)
	ex2 = makeexpr_stringcast(ex2);
    else
	ex2 = makeexpr_charcast(ex2);
    if ((ex2->val.type->kind == TK_INTEGER ||
	 ex2->val.type->kind == TK_CHAR ||
	 ex2->val.type->kind == TK_BOOLEAN ||
	 ex2->val.type->kind == TK_ENUM ||
	 ex2->val.type->kind == TK_SUBR ||
	 ex2->val.type->kind == TK_REAL ||
	 ex2->val.type->kind == TK_POINTER ||
	 ex2->val.type->kind == TK_STRING) &&
	(type->kind == TK_INTEGER ||
	 type->kind == TK_CHAR ||
	 type->kind == TK_BOOLEAN ||
	 type->kind == TK_ENUM ||
	 type->kind == TK_SUBR ||
	 type->kind == TK_REAL ||
	 type->kind == TK_POINTER)) {
	if (type->kind == TK_POINTER || ex2->val.type->kind == TK_POINTER)
	    return makeexpr_un(EK_CAST, type, ex2);
	else
	    return makeexpr_un(EK_ACTCAST, type, ex2);
    } else {
	return makeexpr_hat(makeexpr_cast(makeexpr_addr(ex2),
					  makepointertype(type)), 0);
    }
}




Static Expr *p_factor(target)
Type *target;
{
    Expr *ex, *ex2;
    Type *type;
    Meaning *mp, *mp2;

    switch (curtok) {

        case TOK_INTLIT:
            ex = makeexpr_long(curtokint);
            gettok();
            return ex;

        case TOK_HEXLIT:
            ex = makeexpr_long(curtokint);
            insertarg(&ex, 0, makeexpr_name("%#lx", tp_integer));
            gettok();
            return ex;

        case TOK_OCTLIT:
            ex = makeexpr_long(curtokint);
            insertarg(&ex, 0, makeexpr_name("%#lo", tp_integer));
            gettok();
            return ex;

        case TOK_MININT:
	    strcat(curtokbuf, ".0");

	/* fall through */
        case TOK_REALLIT:
            ex = makeexpr_real(curtokbuf);
            gettok();
            return ex;

        case TOK_HAT:
        case TOK_STRLIT:
            ex = accumulate_strlit();
            return ex;

        case TOK_LPAR:
            gettok();
            ex = p_expr(target);
            skipcloseparen();
            return dots_n_hats(ex, target);

        case TOK_NOT:
	case TOK_TWIDDLE:
            gettok();
            ex = p_factor(tp_integer);
            if (ord_type(ex->val.type)->kind == TK_INTEGER)
                return makeexpr_un(EK_BNOT, tp_integer, ex);
            else
                return makeexpr_not(ex);

	case TOK_MINUS:
	    gettok();
            if (curtok == TOK_MININT) {
                gettok();
                return makeexpr_long(MININT);
            } else
		return makeexpr_neg(p_factor(target));
	    
        case TOK_PLUS:
	    gettok();
	    return p_factor(target);

        case TOK_ADDR:
            gettok();
	    if (curtok == TOK_ADDR) {
		gettok();
		ex = p_factor(tp_proc);
		if (ex->val.type->kind == TK_PROCPTR && ex->kind == EK_COMMA)
		    return grabarg(grabarg(grabarg(ex, 0), 1), 0);
		if (ex->val.type->kind != TK_CPROCPTR)
		    warning("@@ allowed only for procedure pointers [302]");
		return makeexpr_addrf(ex);
	    }
            if (curtok == TOK_IDENT && 0 &&  /***/
                curtokmeaning && (curtokmeaning->kind == MK_FUNCTION ||
                                  curtokmeaning->kind == MK_SPECIAL)) {
                if (curtokmeaning->ctx == nullctx)
                    warning(format_s("Can't take address of predefined object %s [303]",
                                     curtokmeaning->name));
                ex = makeexpr_name(curtokmeaning->name, tp_anyptr);
                gettok();
            } else {
		ex = p_factor(tp_proc);
		if (ex->val.type->kind == TK_PROCPTR) {
		  /*  ex = makeexpr_dotq(ex, "proc", tp_anyptr);  */
		} else if (ex->val.type->kind == TK_CPROCPTR) {
		    ex = makeexpr_cast(ex, tp_anyptr);
		} else
		    ex = makeexpr_addrf(ex);
            }
            return ex;

        case TOK_LBR:
	case TOK_LBRACE:
            return p_setfactor(target && target->kind == TK_SET
			       ? target->indextype : NULL, 0);

        case TOK_NIL:
            gettok();
            return makeexpr_nil();

	case TOK_IF:    /* nifty Pascal extension */
	    gettok();
	    ex = p_expr(tp_boolean);
	    wneedtok(TOK_THEN);
	    ex2 = p_expr(tp_integer);
	    if (wneedtok(TOK_ELSE))
		return makeexpr_cond(ex, ex2, p_factor(ex2->val.type));
	    else
		return makeexpr_cond(ex, ex2, makeexpr_long(0));

        case TOK_IDENT:
            mp = curtokmeaning;
            switch ((mp) ? mp->kind : MK_VAR) {

                case MK_TYPE:
                    gettok();
                    type = mp->type;
                    switch (curtok) {

                        case TOK_LPAR:    /* Turbo type cast */
                            gettok();
                            ex2 = p_expr(type);
			    ex = pascaltypecast(type, ex2);
                            skipcloseparen();
                            return dots_n_hats(ex, target);

                        case TOK_LBR:
			case TOK_LBRACE:
                            switch (type->kind) {

                                case TK_SET:
                                case TK_SMALLSET:
                                    return p_setfactor(type->indextype, 1);

                                case TK_RECORD:
                                    return p_constrecord(type, 0);

                                case TK_ARRAY:
                                case TK_SMALLARRAY:
                                    return p_constarray(type, 0);

                                case TK_STRING:
                                    return p_conststring(type, 0);

                                default:
                                    warning("Bad type for constructor [304]");
				    skipparens();
				    return makeexpr_name(mp->name, mp->type);
                            }

			default:
			    wexpected("an expression");
			    return makeexpr_name(mp->name, mp->type);
                    }

                case MK_SPECIAL:
                    if (mp->handler && mp->isfunction &&
			(curtok == TOK_LPAR || !target ||
			 (target->kind != TK_PROCPTR &&
			  target->kind != TK_CPROCPTR))) {
                        gettok();
                        if ((mp->sym->flags & LEAVEALONE) || mp->constdefn) {
                            ex = makeexpr_bicall_0(mp->name, tp_integer);
                            if (curtok == TOK_LPAR) {
                                do {
                                    gettok();
                                    insertarg(&ex, ex->nargs, p_expr(NULL));
                                } while (curtok == TOK_COMMA);
                                skipcloseparen();
                            }
                            tryfuncmacro(&ex, mp);
			    return ex;
                        }
                        ex = (*mp->handler)(mp);
			if (!ex)
			    ex = makeexpr_long(0);
			return ex;
                    } else {
			if (target &&
			    (target->kind == TK_PROCPTR ||
			     target->kind == TK_CPROCPTR))
			    note("Using a built-in procedure as a procedure pointer [316]");
                        else
			    symclass(curtoksym);
                        gettok();
                        return makeexpr_name(mp->name, tp_integer);
                    }

                case MK_FUNCTION:
                    mp->refcount++;
                    need_forward_decl(mp);
		    gettok();
                    if (mp->isfunction &&
			(curtok == TOK_LPAR || !target ||
			 (target->kind != TK_PROCPTR &&
			  target->kind != TK_CPROCPTR))) {
                        ex = p_funccall(mp);
                        if (!mp->constdefn) {
                            if (mp->handler && !(mp->sym->flags & LEAVEALONE))
                                ex = (*mp->handler)(ex);
			}
			if (mp->cbase->kind == MK_VARPARAM) {
			    ex = makeexpr_hat(ex, 0);    /* returns pointer to structured result */
                        }
                        return dots_n_hats(ex, target);
                    } else {
			if (mp->handler && !(mp->sym->flags & LEAVEALONE))
			    note("Using a built-in procedure as a procedure pointer [316]");
			if (target && target->kind == TK_CPROCPTR) {
			    type = maketype(TK_CPROCPTR);
			    type->basetype = mp->type;
			    type->escale = 0;
			    mp2 = makestmttempvar(type, name_TEMP);
			    ex = makeexpr_comma(
                                    makeexpr_assign(
                                       makeexpr_var(mp2),
				       makeexpr_name(mp->name, tp_text)),
				    makeexpr_var(mp2));
			    if (mp->ctx->kind == MK_FUNCTION)
				warning("Procedure pointer to nested procedure [305]");
			} else {
			    type = maketype(TK_PROCPTR);
			    type->basetype = mp->type;
			    type->escale = 1;
			    mp2 = makestmttempvar(type, name_TEMP);
			    ex = makeexpr_comma(
                                    makeexpr_comma(
                                       makeexpr_assign(
                                          makeexpr_dotq(makeexpr_var(mp2),
							"proc",
							tp_anyptr),
					  makeexpr_name(mp->name, tp_text)),
                                          /* handy pointer type */
				       makeexpr_assign(
                                          makeexpr_dotq(makeexpr_var(mp2),
							"link",
							tp_anyptr),
				          makeexpr_ctx(mp->ctx))),
				    makeexpr_var(mp2));
			}
                        return ex;
                    }

                default:
                    return p_variable(target);
            }

	default:
	    wexpected("an expression");
	    return makeexpr_long(0);
	    
    }
}




Static Expr *p_powterm(target)
Type *target;
{
    Expr *ex = p_factor(target);
    Expr *ex2;
    int i, castit;
    long v;

    if (curtok == TOK_STARSTAR) {
	gettok();
	ex2 = p_powterm(target);
	if (ex->val.type->kind == TK_REAL ||
	    ex2->val.type->kind == TK_REAL) {
	    if (checkconst(ex2, 2)) {
		ex = makeexpr_sqr(ex, 0);
	    } else if (checkconst(ex2, 3)) {
		ex = makeexpr_sqr(ex, 1);
	    } else {
		castit = castargs >= 0 ? castargs : (prototypes == 0);
		if (ex->val.type->kind != TK_REAL && castit)
		    ex = makeexpr_cast(ex, tp_longreal);
		if (ex2->val.type->kind != TK_REAL && castit)
		    ex2 = makeexpr_cast(ex2, tp_longreal);
		ex = makeexpr_bicall_2("pow", tp_longreal, ex, ex2);
	    }
	} else if (checkconst(ex, 2)) {
	    freeexpr(ex);
	    ex = makeexpr_bin(EK_LSH, tp_integer,
			      makeexpr_longcast(makeexpr_long(1), 1), ex2);
	} else if (checkconst(ex, 0) ||
		   checkconst(ex, 1) ||
		   checkconst(ex2, 1)) {
	    freeexpr(ex2);
	} else if (checkconst(ex2, 0)) {
	    freeexpr(ex);
	    freeexpr(ex2);
	    ex = makeexpr_long(1);
	} else if (isliteralconst(ex, NULL) == 2 &&
		   isliteralconst(ex2, NULL) == 2 &&
		   ex2->val.i > 0) {
	    v = ex->val.i;
	    i = ex2->val.i;
	    while (--i > 0)
		v *= ex->val.i;
	    freeexpr(ex);
	    freeexpr(ex2);
	    ex = makeexpr_long(v);
	} else if (checkconst(ex2, 2)) {
	    ex = makeexpr_sqr(ex, 0);
	} else if (checkconst(ex2, 3)) {
	    ex = makeexpr_sqr(ex, 1);
	} else {
	    ex = makeexpr_bicall_2("ipow", tp_integer,
				   makeexpr_arglong(ex, 1),
				   makeexpr_arglong(ex2, 1));
	}
    }
    return ex;
}


Static Expr *p_term(target)
Type *target;
{
    Expr *ex = p_powterm(target);
    Expr *ex2;
    Type *type;
    Meaning *tvar;
    int useshort;

    for (;;) {
	checkkeyword(TOK_SHL);
	checkkeyword(TOK_SHR);
	checkkeyword(TOK_REM);
        switch (curtok) {

            case TOK_STAR:
                gettok();
                if (ex->val.type->kind == TK_SET ||
                    ex->val.type->kind == TK_SMALLSET) {
                    ex2 = p_powterm(ex->val.type);
                    type = mixsets(&ex, &ex2);
                    if (type->kind == TK_SMALLSET) {
                        ex = makeexpr_bin(EK_BAND, type, ex, ex2);
                    } else {
                        tvar = makestmttempvar(type, name_SET);
                        ex = makeexpr_bicall_3(setintname, type,
                                               makeexpr_var(tvar),
                                               ex, ex2);
                    }
                } else
                    ex = makeexpr_times(ex, p_powterm(tp_integer));
                break;

            case TOK_SLASH:
                gettok();
                if (ex->val.type->kind == TK_SET ||
                    ex->val.type->kind == TK_SMALLSET) {
                    ex2 = p_powterm(ex->val.type);
                    type = mixsets(&ex, &ex2);
                    if (type->kind == TK_SMALLSET) {
                        ex = makeexpr_bin(EK_BXOR, type, ex, ex2);
                    } else {
                        tvar = makestmttempvar(type, name_SET);
                        ex = makeexpr_bicall_3(setxorname, type,
                                               makeexpr_var(tvar),
                                               ex, ex2);
                    }
		} else
		    ex = makeexpr_divide(ex, p_powterm(tp_integer));
                break;

            case TOK_DIV:
                gettok();
                ex = makeexpr_div(ex, p_powterm(tp_integer));
                break;

            case TOK_REM:
                gettok();
                ex = makeexpr_rem(ex, p_powterm(tp_integer));
                break;

            case TOK_MOD:
                gettok();
                ex = makeexpr_mod(ex, p_powterm(tp_integer));
                break;

            case TOK_AND:
	    case TOK_AMP:
		useshort = (curtok == TOK_AMP);
                gettok();
                ex2 = p_powterm(tp_integer);
                if (ord_type(ex->val.type)->kind == TK_INTEGER)
                    ex = makeexpr_bin(EK_BAND, ex->val.type, ex, ex2);
                else if (partial_eval_flag || useshort ||
                         (shortopt && nosideeffects(ex2, 1)))
                    ex = makeexpr_and(ex, ex2);
                else
                    ex = makeexpr_bin(EK_BAND, tp_boolean, ex, ex2);
                break;

            case TOK_SHL:
                gettok();
                ex = makeexpr_bin(EK_LSH, ex->val.type, ex, p_powterm(tp_integer));
                break;

            case TOK_SHR:
                gettok();
                ex = force_unsigned(ex);
                ex = makeexpr_bin(EK_RSH, ex->val.type, ex, p_powterm(tp_integer));
                break;

            default:
                return ex;
        }
    }
}



Static Expr *p_sexpr(target)
Type *target;
{
    Expr *ex, *ex2;
    Type *type;
    Meaning *tvar;
    int useshort;

    switch (curtok) {
        case TOK_MINUS:
            gettok();
            if (curtok == TOK_MININT) {
                gettok();
                ex = makeexpr_long(MININT);
		break;
            }
            ex = makeexpr_neg(p_term(target));
            break;
        case TOK_PLUS:
            gettok();
        /* fall through */
        default:
            ex = p_term(target);
            break;
    }
    if (curtok == TOK_PLUS &&
        (ex->val.type->kind == TK_STRING ||
         ord_type(ex->val.type)->kind == TK_CHAR ||
         ex->val.type->kind == TK_ARRAY)) {
        while (curtok == TOK_PLUS) {
            gettok();
            ex = makeexpr_concat(ex, p_term(NULL), 0);
        }
        return ex;
    } else {
        for (;;) {
	    checkkeyword(TOK_XOR);
            switch (curtok) {

                case TOK_PLUS:
                    gettok();
                    if (ex->val.type->kind == TK_SET ||
                        ex->val.type->kind == TK_SMALLSET) {
                        ex2 = p_term(ex->val.type);
                        type = mixsets(&ex, &ex2);
                        if (type->kind == TK_SMALLSET) {
                            ex = makeexpr_bin(EK_BOR, type, ex, ex2);
                        } else {
                            tvar = makestmttempvar(type, name_SET);
                            ex = makeexpr_bicall_3(setunionname, type,
                                                   makeexpr_var(tvar),
                                                   ex, ex2);
                        }
                    } else
                        ex = makeexpr_plus(ex, p_term(tp_integer));
                    break;

                case TOK_MINUS:
                    gettok();
                    if (ex->val.type->kind == TK_SET ||
                        ex->val.type->kind == TK_SMALLSET) {
                        ex2 = p_term(tp_integer);
                        type = mixsets(&ex, &ex2);
                        if (type->kind == TK_SMALLSET) {
                            ex = makeexpr_bin(EK_BAND, type, ex,
                                              makeexpr_un(EK_BNOT, type, ex2));
                        } else {
                            tvar = makestmttempvar(type, name_SET);
                            ex = makeexpr_bicall_3(setdiffname, type,
                                                   makeexpr_var(tvar), ex, ex2);
                        }
                    } else
                        ex = makeexpr_minus(ex, p_term(tp_integer));
                    break;

		case TOK_VBAR:
		    if (modula2)
			return ex;
		    /* fall through */

                case TOK_OR:
		    useshort = (curtok == TOK_VBAR);
                    gettok();
                    ex2 = p_term(tp_integer);
                    if (ord_type(ex->val.type)->kind == TK_INTEGER)
                        ex = makeexpr_bin(EK_BOR, ex->val.type, ex, ex2);
                    else if (partial_eval_flag || useshort ||
                             (shortopt && nosideeffects(ex2, 1)))
                        ex = makeexpr_or(ex, ex2);
                    else
                        ex = makeexpr_bin(EK_BOR, tp_boolean, ex, ex2);
                    break;

                case TOK_XOR:
                    gettok();
                    ex2 = p_term(tp_integer);
                    ex = makeexpr_bin(EK_BXOR, ex->val.type, ex, ex2);
                    break;

                default:
                    return ex;
            }
        }
    }
}



Expr *p_expr(target)
Type *target;
{
    Expr *ex = p_sexpr(target);
    Expr *ex2, *ex3, *ex4;
    Type *type;
    Meaning *tvar;
    long mask, smin, smax;
    int i, j;

    switch (curtok) {

        case TOK_EQ:
            gettok();
            return makeexpr_rel(EK_EQ, ex, p_sexpr(ex->val.type));

        case TOK_NE:
            gettok();
            return makeexpr_rel(EK_NE, ex, p_sexpr(ex->val.type));

        case TOK_LT:
            gettok();
            return makeexpr_rel(EK_LT, ex, p_sexpr(ex->val.type));

        case TOK_GT:
            gettok();
            return makeexpr_rel(EK_GT, ex, p_sexpr(ex->val.type));

        case TOK_LE:
            gettok();
            return makeexpr_rel(EK_LE, ex, p_sexpr(ex->val.type));

        case TOK_GE:
            gettok();
            return makeexpr_rel(EK_GE, ex, p_sexpr(ex->val.type));

        case TOK_IN:
            gettok();
            ex2 = p_sexpr(tp_smallset);
            ex = gentle_cast(ex, ex2->val.type->indextype);
            if (ex2->val.type->kind == TK_SMALLSET) {
                if (!ord_range(ex->val.type, &smin, &smax)) {
                    smin = -1;
                    smax = setbits;
                }
                if (!nosideeffects(ex, 0)) {
                    tvar = makestmttempvar(ex->val.type, name_TEMP);
                    ex3 = makeexpr_assign(makeexpr_var(tvar), ex);
                    ex = makeexpr_var(tvar);
                } else
                    ex3 = NULL;
                ex4 = copyexpr(ex);
                if (ex->kind == EK_CONST && smallsetconst)
                    ex = makesmallsetconst(1<<ex->val.i, ex2->val.type);
                else
                    ex = makeexpr_bin(EK_LSH, ex2->val.type,
                                      makeexpr_longcast(makeexpr_long(1), 1),
                                      enum_to_int(ex));
                ex = makeexpr_rel(EK_NE, makeexpr_bin(EK_BAND, tp_integer, ex, ex2),
                                         makeexpr_long(0));
                if (*name_SETBITS ||
                    ((ex4->kind == EK_CONST) ? ((unsigned long)ex4->val.i >= setbits)
                                             : !(0 <= smin && smax < setbits))) {
                    ex = makeexpr_and(makeexpr_range(enum_to_int(ex4),
                                                     makeexpr_long(0),
                                                     makeexpr_setbits(), 0),
                                      ex);
                } else
                    freeexpr(ex4);
                ex = makeexpr_comma(ex3, ex);
                return ex;
            } else {
                ex3 = ex2;
                while (ex3->kind == EK_BICALL &&
                       (!strcmp(ex3->val.s, setaddname) ||
                        !strcmp(ex3->val.s, setaddrangename)))
                    ex3 = ex3->args[0];
                if (ex3->kind == EK_BICALL && !strcmp(ex3->val.s, setexpandname) &&
                    (tvar = istempvar(ex3->args[0])) != NULL && 
                    isconstexpr(ex3->args[1], &mask)) {
                    canceltempvar(tvar);
                    if (!nosideeffects(ex, 0)) {
                        tvar = makestmttempvar(ex->val.type, name_TEMP);
                        ex3 = makeexpr_assign(makeexpr_var(tvar), ex);
                        ex = makeexpr_var(tvar);
                    } else
                        ex3 = NULL;
                    type = ord_type(ex2->val.type->indextype);
                    ex4 = NULL;
                    i = 0;
                    while (i < setbits) {
                        if (mask & (1<<i++)) {
                            if (i+1 < setbits && (mask & (2<<i))) {
                                for (j = i; j < setbits && (mask & (1<<j)); j++) ;
                                ex4 = makeexpr_or(ex4,
                                        makeexpr_range(copyexpr(ex),
                                                       makeexpr_val(make_ord(type, i-1)),
                                                       makeexpr_val(make_ord(type, j-1)), 1));
                                i = j;
                            } else {
                                ex4 = makeexpr_or(ex4,
                                        makeexpr_rel(EK_EQ, copyexpr(ex),
                                                            makeexpr_val(make_ord(type, i-1))));
                            }
                        }
                    }
                    mask = 0;
                    for (;;) {
                        if (!strcmp(ex2->val.s, setaddrangename)) {
                            if (checkconst(ex2->args[1], 'a') &&
                                checkconst(ex2->args[2], 'z')) {
                                mask |= 0x1;
                            } else if (checkconst(ex2->args[1], 'A') &&
                                       checkconst(ex2->args[2], 'Z')) {
                                mask |= 0x2;
                            } else if (checkconst(ex2->args[1], '0') &&
                                       checkconst(ex2->args[2], '9')) {
                                mask |= 0x4;
                            } else {
                                ex4 = makeexpr_or(ex4,
                                        makeexpr_range(copyexpr(ex), ex2->args[1], ex2->args[2], 1));
                            }
                        } else if (!strcmp(ex2->val.s, setaddname)) {
                            ex4 = makeexpr_or(ex4,
                                    makeexpr_rel(EK_EQ, copyexpr(ex), ex2->args[1]));
                        } else
                            break;
                        ex2 = ex2->args[0];
                    }
                    /* do these now so that EK_OR optimizations will work: */
                    if (mask & 0x1)
                        ex4 = makeexpr_or(ex4, makeexpr_range(copyexpr(ex),
                                                              makeexpr_char('a'),
                                                              makeexpr_char('z'), 1));
                    if (mask & 0x2)
                        ex4 = makeexpr_or(ex4, makeexpr_range(copyexpr(ex),
                                                              makeexpr_char('A'),
                                                              makeexpr_char('Z'), 1));
                    if (mask & 0x4)
                        ex4 = makeexpr_or(ex4, makeexpr_range(copyexpr(ex),
                                                              makeexpr_char('0'),
                                                              makeexpr_char('9'), 1));
                    freeexpr(ex);
                    return makeexpr_comma(ex3, ex4);
                }
                return makeexpr_bicall_2(setinname, tp_boolean,
                                         makeexpr_arglong(ex, 0), ex2);
            }

	default:
	    return ex;
    }
}







/* Parse a C expression; used by VarMacro, etc. */

Type *nametotype(name)
char *name;
{
    if (!strcicmp(name, "malloc") ||
	!strcicmp(name, mallocname)) {
	return tp_anyptr;
    }
    return tp_integer;
}


int istypespec()
{
    switch (curtok) {

        case TOK_CONST:
            return 1;

        case TOK_IDENT:
            return !strcmp(curtokcase, "volatile") ||
                   !strcmp(curtokcase, "void") ||
                   !strcmp(curtokcase, "char") ||
                   !strcmp(curtokcase, "short") ||
                   !strcmp(curtokcase, "int") ||
                   !strcmp(curtokcase, "long") ||
                   !strcmp(curtokcase, "float") ||
                   !strcmp(curtokcase, "double") ||
                   !strcmp(curtokcase, "signed") ||
                   !strcmp(curtokcase, "unsigned") ||
                   !strcmp(curtokcase, "struct") ||
                   !strcmp(curtokcase, "union") ||
                   !strcmp(curtokcase, "class") ||
                   !strcmp(curtokcase, "enum") ||
                   !strcmp(curtokcase, "typedef") ||
                   (curtokmeaning &&
                    curtokmeaning->kind == MK_TYPE);

        default:
            return 0;
    }
}



Expr *pc_parentype(cp)
char *cp;
{
    Expr *ex;

    if (curtok == TOK_IDENT &&
         curtokmeaning &&
         curtokmeaning->kind == MK_TYPE) {
        ex = makeexpr_type(curtokmeaning->type);
        gettok();
        skipcloseparen();
    } else if (curtok == TOK_IDENT && !strcmp(curtokcase, "typedef")) {
        ex = makeexpr_name(getparenstr(inbufptr), tp_integer);
        gettok();
    } else {
        ex = makeexpr_name(getparenstr(cp), tp_integer);
        gettok();
    }
    return ex;
}




Expr *pc_expr2();

Expr *pc_factor()
{
    Expr *ex;
    char *cp;
    Strlist *sl;
    int i;

    switch (curtok) {

        case TOK_BANG:
            gettok();
            return makeexpr_not(pc_expr2(14));

        case TOK_TWIDDLE:
            gettok();
            return makeexpr_un(EK_BNOT, tp_integer, pc_expr2(14));

        case TOK_PLPL:
            gettok();
            ex = pc_expr2(14);
            return makeexpr_assign(ex, makeexpr_plus(copyexpr(ex), makeexpr_long(1)));

        case TOK_MIMI:
            gettok();
            ex = pc_expr2(14);
            return makeexpr_assign(ex, makeexpr_minus(copyexpr(ex), makeexpr_long(1)));

        case TOK_STAR:
            gettok();
            ex = pc_expr2(14);
            if (ex->val.type->kind != TK_POINTER)
                ex->val.type = makepointertype(ex->val.type);
            return makeexpr_hat(ex, 0);

        case TOK_AMP:
            gettok();
            return makeexpr_addr(pc_expr2(14));

        case TOK_PLUS:
            gettok();
            return pc_expr2(14);

        case TOK_MINUS:
            gettok();
            return makeexpr_neg(pc_expr2(14));

        case TOK_LPAR:
            cp = inbufptr;
            gettok();
            if (istypespec()) {
                ex = pc_parentype(cp);
                return makeexpr_bin(EK_LITCAST, tp_integer, ex, pc_expr2(14));
            }
            ex = pc_expr();
            skipcloseparen();
            return ex;

        case TOK_IDENT:
            if (!strcmp(curtokcase, "sizeof")) {
                gettok();
                if (curtok != TOK_LPAR)
                    return makeexpr_sizeof(pc_expr2(14), 1);
                cp = inbufptr;
                gettok();
                if (istypespec()) {
                    ex = makeexpr_sizeof(pc_parentype(cp), 1);
                } else {
                    ex = makeexpr_sizeof(pc_expr(), 1);
                    skipcloseparen();
                }
                return ex;
            }
            if (curtoksym->flags & FMACREC) {
                ex = makeexpr(EK_MACARG, 0);
                ex->val.type = tp_integer;
                ex->val.i = 0;
                for (sl = funcmacroargs, i = 1; sl; sl = sl->next, i++) {
                    if (sl->value == (long)curtoksym) {
                        ex->val.i = i;
                        break;
                    }
                }
            } else
                ex = makeexpr_name(curtokcase, nametotype(curtokcase));
            gettok();
            return ex;

        case TOK_INTLIT:
            ex = makeexpr_long(curtokint);
            if (curtokbuf[strlen(curtokbuf)-1] == 'L')
                ex = makeexpr_longcast(ex, 1);
            gettok();
            return ex;

        case TOK_HEXLIT:
            ex = makeexpr_long(curtokint);
            insertarg(&ex, 0, makeexpr_name("%#lx", tp_integer));
            if (curtokbuf[strlen(curtokbuf)-1] == 'L')
                ex = makeexpr_longcast(ex, 1);
            gettok();
            return ex;

        case TOK_OCTLIT:
            ex = makeexpr_long(curtokint);
            insertarg(&ex, 0, makeexpr_name("%#lo", tp_integer));
            if (curtokbuf[strlen(curtokbuf)-1] == 'L')
                ex = makeexpr_longcast(ex, 1);
            gettok();
            return ex;

        case TOK_REALLIT:
            ex = makeexpr_real(curtokbuf);
            gettok();
            return ex;

        case TOK_STRLIT:
            ex = makeexpr_lstring(curtokbuf, curtokint);
            gettok();
            return ex;

        case TOK_CHARLIT:
            ex = makeexpr_char(curtokint);
            gettok();
            return ex;

        default:
	    wexpected("a C expression");
	    return makeexpr_long(0);
    }
}




#define pc_prec(pr)  if (prec > (pr)) return ex; gettok();

Expr *pc_expr2(prec)
int prec;
{
    Expr *ex, *ex2;
    int i;

    ex = pc_factor();
    for (;;) {
        switch (curtok) {

            case TOK_COMMA:
                pc_prec(1);
                ex = makeexpr_comma(ex, pc_expr2(2));
                break;

            case TOK_EQ:
                pc_prec(2);
                ex = makeexpr_assign(ex, pc_expr2(2));
                break;

            case TOK_QM:
                pc_prec(3);
                ex2 = pc_expr();
                if (wneedtok(TOK_COLON))
		    ex = makeexpr_cond(ex, ex2, pc_expr2(3));
		else
		    ex = makeexpr_cond(ex, ex2, makeexpr_long(0));
                break;

            case TOK_OROR:
                pc_prec(4);
                ex = makeexpr_or(ex, pc_expr2(5));
                break;

            case TOK_ANDAND:
                pc_prec(5);
                ex = makeexpr_and(ex, pc_expr2(6));
                break;

            case TOK_VBAR:
                pc_prec(6);
                ex = makeexpr_bin(EK_BOR, tp_integer, ex, pc_expr2(7));
                break;

            case TOK_HAT:
                pc_prec(7);
                ex = makeexpr_bin(EK_BXOR, tp_integer, ex, pc_expr2(8));
                break;

            case TOK_AMP:
                pc_prec(8);
                ex = makeexpr_bin(EK_BAND, tp_integer, ex, pc_expr2(9));
                break;

            case TOK_EQEQ:
                pc_prec(9);
                ex = makeexpr_rel(EK_EQ, ex, pc_expr2(10));
                break;

            case TOK_BANGEQ:
                pc_prec(9);
                ex = makeexpr_rel(EK_NE, ex, pc_expr2(10));
                break;

            case TOK_LT:
                pc_prec(10);
                ex = makeexpr_rel(EK_LT, ex, pc_expr2(11));
                break;

            case TOK_LE:
                pc_prec(10);
                ex = makeexpr_rel(EK_LE, ex, pc_expr2(11));
                break;

            case TOK_GT:
                pc_prec(10);
                ex = makeexpr_rel(EK_GT, ex, pc_expr2(11));
                break;

            case TOK_GE:
                pc_prec(10);
                ex = makeexpr_rel(EK_GE, ex, pc_expr2(11));
                break;

            case TOK_LTLT:
                pc_prec(11);
                ex = makeexpr_bin(EK_LSH, tp_integer, ex, pc_expr2(12));
                break;

            case TOK_GTGT:
                pc_prec(11);
                ex = makeexpr_bin(EK_RSH, tp_integer, ex, pc_expr2(12));
                break;

            case TOK_PLUS:
                pc_prec(12);
                ex = makeexpr_plus(ex, pc_expr2(13));
                break;

            case TOK_MINUS:
                pc_prec(12);
                ex = makeexpr_minus(ex, pc_expr2(13));
                break;

            case TOK_STAR:
                pc_prec(13);
                ex = makeexpr_times(ex, pc_expr2(14));
                break;

            case TOK_SLASH:
                pc_prec(13);
                ex = makeexpr_div(ex, pc_expr2(14));
                break;

            case TOK_PERC:
                pc_prec(13);
                ex = makeexpr_mod(ex, pc_expr2(14));
                break;

            case TOK_PLPL:
                pc_prec(15);
                ex = makeexpr_un(EK_POSTINC, tp_integer, ex);
                break;

            case TOK_MIMI:
                pc_prec(15);
                ex = makeexpr_un(EK_POSTDEC, tp_integer, ex);
                break;

            case TOK_LPAR:
                pc_prec(16);
                if (ex->kind == EK_NAME) {
                    ex->kind = EK_BICALL;
                } else {
                    ex = makeexpr_un(EK_SPCALL, tp_integer, ex);
                }
                while (curtok != TOK_RPAR) {
                    insertarg(&ex, ex->nargs, pc_expr2(2));
                    if (curtok != TOK_RPAR)
                        if (!wneedtok(TOK_COMMA))
			    skiptotoken2(TOK_RPAR, TOK_SEMI);
                }
                gettok();
                break;

            case TOK_LBR:
                pc_prec(16);
                ex = makeexpr_index(ex, pc_expr(), NULL);
                if (!wneedtok(TOK_RBR))
		    skippasttoken(TOK_RBR);
                break;

            case TOK_ARROW:
                pc_prec(16);
                if (!wexpecttok(TOK_IDENT))
		    break;
                if (ex->val.type->kind != TK_POINTER)
                    ex->val.type = makepointertype(ex->val.type);
                ex = makeexpr_dotq(makeexpr_hat(ex, 0),
                                   curtokcase, tp_integer);
                gettok();
                break;

            case TOK_DOT:
                pc_prec(16);
                if (!wexpecttok(TOK_IDENT))
		    break;
                ex = makeexpr_dotq(ex, curtokcase, tp_integer);
                gettok();
                break;

	    case TOK_COLONCOLON:
		if (prec > 16)
		    return ex;
		i = C_lex;
		C_lex = 0;
		gettok();
		if (curtok == TOK_IDENT &&
		    curtokmeaning && curtokmeaning->kind == MK_TYPE) {
		    ex->val.type = curtokmeaning->type;
		} else if (curtok == TOK_LPAR) {
		    gettok();
		    ex->val.type = p_type(NULL);
		    if (!wexpecttok(TOK_RPAR))
			skiptotoken(TOK_RPAR);
		} else
		    wexpected("a type name");
		C_lex = i;
		gettok();
		break;

            default:
                return ex;
        }
    }
}




Expr *pc_expr()
{
    return pc_expr2(0);
}



Expr *pc_expr_str(buf)
char *buf;
{
    Strlist *defsl, *sl;
    Expr *ex;

    defsl = NULL;
    sl = strlist_append(&defsl, buf);
    C_lex++;
    push_input_strlist(defsl, buf);
    ex = pc_expr();
    if (curtok != TOK_EOF)
        warning(format_s("Junk (%s) at end of C expression [306]",
			 tok_name(curtok)));
    pop_input();
    C_lex--;
    strlist_empty(&defsl);
    return ex;
}






/* Simplify an expression */

Expr *fixexpr(ex, env)
Expr *ex;
int env;
{
    Expr *ex2, *ex3;
    Type *type, *type2;
    char *cp;
    char sbuf[5];
    int i, j;
    Value val;

    if (!ex)
        return NULL;
    if (debug>4) {fprintf(outf, "fixexpr("); dumpexpr(ex); fprintf(outf, ")\n");}
    switch (ex->kind) {

        case EK_BICALL:
            ex2 = fix_bicall(ex, env);
            if (ex2) {
                ex = ex2;
                break;
            }
            cp = ex->val.s;
            if (!strcmp(cp, "strlen")) {
                if (ex->args[0]->kind == EK_BICALL &&
                    !strcmp(ex->args[0]->val.s, "sprintf") &&
                    sprintf_value == 0) {     /* does sprintf return char count? */
                    ex = grabarg(ex, 0);
                    strchange(&ex->val.s, "*sprintf");
                    ex = fixexpr(ex, env);
                } else {
                    ex->args[0] = fixexpr(ex->args[0], ENV_EXPR);
                }
            } else if (!strcmp(cp, name_SETIO)) {
                ex->args[0] = fixexpr(ex->args[0], ENV_BOOL);
            } else if (!strcmp(cp, "~~SETIO")) {
                ex->args[0] = fixexpr(ex->args[0], ENV_BOOL);
                ex = makeexpr_cond(ex->args[0],
                                   makeexpr_long(0),
                                   makeexpr_bicall_1(name_ESCIO, tp_int, ex->args[1]));
            } else if (!strcmp(cp, name_CHKIO)) {
                ex->args[0] = fixexpr(ex->args[0], ENV_BOOL);
                ex->args[2] = fixexpr(ex->args[2], env);
                ex->args[3] = fixexpr(ex->args[3], env);
            } else if (!strcmp(cp, "~~CHKIO")) {
                ex->args[0] = fixexpr(ex->args[0], ENV_BOOL);
                ex->args[2] = fixexpr(ex->args[2], env);
                ex->args[3] = fixexpr(ex->args[3], env);
                ex2 = makeexpr_bicall_1(name_ESCIO, tp_int, ex->args[1]);
                if (ord_type(ex->args[3]->val.type)->kind != TK_INTEGER)
                    ex2 = makeexpr_cast(ex2, ex->args[3]->val.type);
                ex = makeexpr_cond(ex->args[0], ex->args[2], ex2);
            } else if (!strcmp(cp, "assert")) {
                ex->args[0] = fixexpr(ex->args[0], ENV_BOOL);
	    } else if ((!strcmp(cp, setaddname) ||
			!strcmp(cp, setaddrangename)) &&
		       (ex2 = ex->args[0])->kind == EK_BICALL &&
		       (!strcmp(ex2->val.s, setaddname) ||
			!strcmp(ex2->val.s, setaddrangename))) {
		while (ex2->kind == EK_BICALL &&
		       (!strcmp(ex2->val.s, setaddname) ||
			!strcmp(ex2->val.s, setaddrangename) ||
			!strcmp(ex2->val.s, setexpandname)))
		    ex2 = ex2->args[0];
		if (nosideeffects(ex2, 1)) {
		    ex = makeexpr_comma(ex->args[0], ex);
		    ex->args[1]->args[0] = ex2;
		    ex = fixexpr(ex, env);
		} else
		    for (i = 0; i < ex->nargs; i++)
			ex->args[i] = fixexpr(ex->args[i], ENV_EXPR);
	    } else if (!strcmp(cp, setunionname) &&
		       (ex3 = singlevar(ex->args[0])) != NULL &&
		       ((i=1, exprsame(ex->args[0], ex->args[i], 0)) ||
			(i=2, exprsame(ex->args[0], ex->args[i], 0))) &&
		       !exproccurs(ex3, ex->args[3-i]) &&
		       ex->args[3-i]->kind == EK_BICALL &&
		       (!strcmp(ex->args[3-i]->val.s, setaddname) ||
			!strcmp(ex->args[3-i]->val.s, setaddrangename) ||
			(!strcmp(ex->args[3-i]->val.s, setexpandname) &&
			 checkconst(ex->args[3-i]->args[1], 0))) &&
		       totempvar(ex->args[3-i])) {
		if (!strcmp(ex->args[3-i]->val.s, setexpandname)) {
		    ex = grabarg(ex, 0);
		} else {
		    ex = makeexpr_comma(ex, ex->args[3-i]);
		    ex->args[0]->args[3-i] = ex->args[1]->args[0];
		    ex->args[1]->args[0] = copyexpr(ex->args[0]->args[0]);
		}
		ex = fixexpr(ex, env);
	    } else if (!strcmp(cp, setdiffname) && *setremname &&
		       (ex3 = singlevar(ex->args[0])) != NULL &&
		       exprsame(ex->args[0], ex->args[1], 0) &&
		       !exproccurs(ex3, ex->args[2]) &&
		       ex->args[2]->kind == EK_BICALL &&
		       (!strcmp(ex->args[2]->val.s, setaddname) ||
			(!strcmp(ex->args[2]->val.s, setexpandname) &&
			 checkconst(ex->args[2]->args[1], 0))) &&
		       totempvar(ex->args[2])) {
		if (!strcmp(ex->args[2]->val.s, setexpandname)) {
		    ex = grabarg(ex, 0);
		} else {
		    ex = makeexpr_comma(ex, ex->args[2]);
		    ex->args[0]->args[2] = ex->args[1]->args[0];
		    ex->args[1]->args[0] = copyexpr(ex->args[0]->args[0]);
		    strchange(&ex->args[1]->val.s, setremname);
		}
		ex = fixexpr(ex, env);
            } else {
                for (i = 0; i < ex->nargs; i++)
                    ex->args[i] = fixexpr(ex->args[i], ENV_EXPR);
		ex = cleansprintf(ex);
                if (!strcmp(cp, "sprintf")) {
                    if (checkstring(ex->args[1], "%s")) {
                        delfreearg(&ex, 1);
                        strchange(&ex->val.s, "strcpy");
                        ex = fixexpr(ex, env);
                    } else if (sprintf_value != 1 && env != ENV_STMT) {
                        if (*sprintfname) {
                            strchange(&ex->val.s, format_s("*%s", sprintfname));
                        } else {
                            strchange(&ex->val.s, "*sprintf");
                            ex = makeexpr_comma(ex, copyexpr(ex->args[0]));
                        }
                    }
                } else if (!strcmp(cp, "strcpy")) {
                    if (env == ENV_STMT &&
                         ex->args[1]->kind == EK_BICALL &&
                         !strcmp(ex->args[1]->val.s, "strcpy") &&
                         nosideeffects(ex->args[1]->args[0], 1)) {
                        ex2 = ex->args[1];
                        ex->args[1] = copyexpr(ex2->args[0]);
                        ex = makeexpr_comma(ex2, ex);
                    }
                } else if (!strcmp(cp, "memcpy")) {
                    strchange(&ex->val.s, format_s("*%s", memcpyname));
                    if (!strcmp(memcpyname, "*bcopy")) {
                        swapexprs(ex->args[0], ex->args[1]);
                        if (env != ENV_STMT)
                            ex = makeexpr_comma(ex, copyexpr(ex->args[1]));
                    }
#if 0
		} else if (!strcmp(cp, setunionname) &&
			   (ex3 = singlevar(ex->args[0])) != NULL &&
			   ((i=1, exprsame(ex->args[0], ex->args[i], 0)) ||
			    (i=2, exprsame(ex->args[0], ex->args[i], 0))) &&
			   !exproccurs(ex3, ex->args[3-i])) {
		    ep = &ex->args[3-i];
		    while ((ex2 = *ep)->kind == EK_BICALL &&
			   (!strcmp(ex2->val.s, setaddname) ||
			    !strcmp(ex2->val.s, setaddrangename)))
			ep = &ex2->args[0];
		    if (ex2->kind == EK_BICALL &&
			!strcmp(ex2->val.s, setexpandname) &&
			checkconst(ex2->args[1], 0) &&
			(mp = istempvar(ex2->args[0])) != NULL) {
			if (ex2 == ex->args[3-i]) {
			    ex = grabarg(ex, i);
			} else {
			    freeexpr(ex2);
			    *ep = ex->args[i];
			    ex = ex->args[3-i];
			}
		    }
		} else if (!strcmp(cp, setdiffname) && *setremname &&
			   (ex3 = singlevar(ex->args[0])) != NULL &&
			   exprsame(ex->args[0], ex->args[1], 0) &&
			   !exproccurs(ex3, ex->args[2])) {
		    ep = &ex->args[2];
		    while ((ex2 = *ep)->kind == EK_BICALL &&
			   !strcmp(ex2->val.s, setaddname))
			ep = &ex2->args[0];
		    if (ex2->kind == EK_BICALL &&
			!strcmp(ex2->val.s, setexpandname) &&
			checkconst(ex2->args[1], 0) &&
			(mp = istempvar(ex2->args[0])) != NULL) {
			if (ex2 == ex->args[2]) {
			    ex = grabarg(ex, 1);
			} else {
			    ex2 = ex->args[2];
			    while (ex2->kind == EK_BICALL &&
				   !strcmp(ex2->val.s, setaddname)) {
				strchange(&ex2->val.s, setremname);
				ex2 = ex2->args[0];
			    }
			    freeexpr(ex2);
			    *ep = ex->args[1];
			    ex = ex->args[2];
			}
		    }
#endif
                } else if (!strcmp(cp, setexpandname) && env == ENV_STMT &&
                           checkconst(ex->args[1], 0)) {
                    ex = makeexpr_assign(makeexpr_hat(ex->args[0], 0),
                                         ex->args[1]);
                } else if (!strcmp(cp, getbitsname)) {
		    type = ex->args[0]->val.type;
		    if (type->kind == TK_POINTER)
			type = type->basetype;
                    sbuf[0] = (type->issigned) ? 'S' : 'U';
                    sbuf[1] = (type->kind == TK_ARRAY) ? 'B' : 'S';
                    sbuf[2] = 0;
                    if (sbuf[1] == 'S' &&
                        type->smax->val.type == tp_boolean) {
                        ex = makeexpr_rel(EK_NE,
                                          makeexpr_bin(EK_BAND, tp_integer,
                                                       ex->args[0],
                                                       makeexpr_bin(EK_LSH, tp_integer,
                                                                    makeexpr_longcast(makeexpr_long(1),
                                                                                      type->basetype
                                                                                            == tp_unsigned),
                                                                    ex->args[1])),
                                          makeexpr_long(0));
                        ex = fixexpr(ex, env);
                    } else
                        strchange(&ex->val.s, format_s(cp, sbuf));
                } else if (!strcmp(cp, putbitsname)) {
		    type = ex->args[0]->val.type;
		    if (type->kind == TK_POINTER)
			type = type->basetype;
                    sbuf[0] = (type->issigned) ? 'S' : 'U';
                    sbuf[1] = (type->kind == TK_ARRAY) ? 'B' : 'S';
                    sbuf[2] = 0;
                    if (sbuf[1] == 'S' &&
                        type->smax->val.type == tp_boolean) {
                        ex = makeexpr_assign(ex->args[0],
                                             makeexpr_bin(EK_BOR, tp_integer,
                                                          copyexpr(ex->args[0]),
                                                          makeexpr_bin(EK_LSH, tp_integer,
                                                                       makeexpr_longcast(ex->args[2],
                                                                                         type->basetype
                                                                                               == tp_unsigned),
                                                                       ex->args[1])));
                    } else
                        strchange(&ex->val.s, format_s(cp, sbuf));
                } else if (!strcmp(cp, storebitsname)) {
		    type = ex->args[0]->val.type;
		    if (type->kind == TK_POINTER)
			type = type->basetype;
                    sbuf[0] = (type->issigned) ? 'S' : 'U';
                    sbuf[1] = (type->kind == TK_ARRAY) ? 'B' : 'S';
                    sbuf[2] = 0;
                    strchange(&ex->val.s, format_s(cp, sbuf));
                } else if (!strcmp(cp, clrbitsname)) {
		    type = ex->args[0]->val.type;
		    if (type->kind == TK_POINTER)
			type = type->basetype;
                    sbuf[0] = (type->kind == TK_ARRAY) ? 'B' : 'S';
                    sbuf[1] = 0;
                    if (sbuf[0] == 'S' &&
                        type->smax->val.type == tp_boolean) {
                        ex = makeexpr_assign(ex->args[0],
                                             makeexpr_bin(EK_BAND, tp_integer,
                                                   copyexpr(ex->args[0]),
                                                   makeexpr_un(EK_BNOT, tp_integer,
                                                          makeexpr_bin(EK_LSH, tp_integer,
                                                                       makeexpr_longcast(makeexpr_long(1),
                                                                                         type->basetype
                                                                                               == tp_unsigned),
                                                                       ex->args[1]))));
                    } else
                        strchange(&ex->val.s, format_s(cp, sbuf));
                } else if (!strcmp(cp, "fopen")) {
		    if (which_lang == LANG_HP &&
			ex->args[0]->kind == EK_CONST &&
			ex->args[0]->val.type->kind == TK_STRING &&
			ex->args[0]->val.i >= 1 &&
			ex->args[0]->val.i <= 2 &&
			isdigit(ex->args[0]->val.s[0]) &&
			(ex->args[0]->val.i == 1 ||
			 isdigit(ex->args[0]->val.s[1]))) {
			strchange(&ex->val.s, "fdopen");
			ex->args[0] = makeexpr_long(atoi(ex->args[0]->val.s));
		    }
		}
            }
            break;

        case EK_NOT:
            ex = makeexpr_not(fixexpr(grabarg(ex, 0), ENV_BOOL));
            break;

        case EK_AND:
        case EK_OR:
            for (i = 0; i < ex->nargs; ) {
                ex->args[i] = fixexpr(ex->args[i], ENV_BOOL);
		if (checkconst(ex->args[i], (ex->kind == EK_OR) ? 0 : 1) &&
		    ex->nargs > 1)
		    delfreearg(&ex, i);
		else if (checkconst(ex->args[i], (ex->kind == EK_OR) ? 1 : 0))
		    return grabarg(ex, i);
		else
		    i++;
	    }
	    if (ex->nargs == 1)
		ex = grabarg(ex, 0);
            break;

        case EK_EQ:
        case EK_NE:
            ex->args[0] = fixexpr(ex->args[0], ENV_EXPR);
            ex->args[1] = fixexpr(ex->args[1], ENV_EXPR);
            if (checkconst(ex->args[1], 0) && env == ENV_BOOL &&
                ord_type(ex->args[1]->val.type)->kind != TK_ENUM &&
                (implicitzero > 0 ||
                 (implicitzero < 0 && ex->args[0]->kind == EK_BICALL &&
                                      boolean_bicall(ex->args[0]->val.s)))) {
                if (ex->kind == EK_EQ)
                    ex = makeexpr_not(grabarg(ex, 0));
                else {
                    ex = grabarg(ex, 0);
                    ex->val.type = tp_boolean;
                }
            }
            break;

        case EK_COND:
            ex->args[0] = fixexpr(ex->args[0], ENV_BOOL);
#if 0
            val = eval_expr(ex->args[0]);
#else
	    val = ex->args[0]->val;
	    if (ex->args[0]->kind != EK_CONST)
		val.type = NULL;
#endif
            if (val.type == tp_boolean) {
                ex = grabarg(ex, (val.i) ? 1 : 2);
                ex = fixexpr(ex, env);
            } else {
                ex->args[1] = fixexpr(ex->args[1], env);
                ex->args[2] = fixexpr(ex->args[2], env);
            }
            break;

        case EK_COMMA:
            for (i = 0; i < ex->nargs; ) {
		j = (i < ex->nargs-1);
                ex->args[i] = fixexpr(ex->args[i], j ? ENV_STMT : env);
                if (nosideeffects(ex->args[i], 1) && j) {
                    delfreearg(&ex, i);
                } else if (ex->args[i]->kind == EK_COMMA) {
		    ex2 = ex->args[i];
		    ex->args[i++] = ex2->args[0];
		    for (j = 1; j < ex2->nargs; j++)
			insertarg(&ex, i++, ex2->args[j]);
		    FREE(ex2);
		} else
                    i++;
            }
            if (ex->nargs == 1)
                ex = grabarg(ex, 0);
            break;

        case EK_CHECKNIL:
            ex->args[0] = fixexpr(ex->args[0], ENV_EXPR);
            if (ex->nargs == 2) {
                ex->args[1] = fixexpr(ex->args[1], ENV_EXPR);
                ex2 = makeexpr_assign(copyexpr(ex->args[1]), ex->args[0]);
                ex3 = ex->args[1];
            } else {
                ex2 = copyexpr(ex->args[0]);
                ex3 = ex->args[0];
            }
            type = ex->args[0]->val.type;
            type2 = ex->val.type;
            ex = makeexpr_cond(makeexpr_rel(EK_NE, ex2, makeexpr_nil()),
                               ex3,
                               makeexpr_cast(makeexpr_bicall_0(name_NILCHECK,
                                                               tp_int),
                                             type));
            ex->val.type = type2;
            ex = fixexpr(ex, env);
            break;

        case EK_CAST:
        case EK_ACTCAST:
            if (env == ENV_STMT) {
                ex = fixexpr(grabarg(ex, 0), ENV_STMT);
            } else {
                ex->args[0] = fixexpr(ex->args[0], ENV_EXPR);
            }
            break;

        default:
            for (i = 0; i < ex->nargs; i++)
                ex->args[i] = fixexpr(ex->args[i], ENV_EXPR);
            break;
    }
    if (debug>4) {fprintf(outf, "fixexpr returns "); dumpexpr(ex); fprintf(outf, "\n");}
    return fix_expression(ex, env);
}








/* Output an expression */


#define bitOp(k)  ((k)==EK_BAND || (k)==EK_BOR || (k)==EK_BXOR)

#define shfOp(k)  ((k)==EK_LSH || (k)==EK_RSH)

#define logOp(k)  ((k)==EK_AND || (k)==EK_OR)

#define relOp(k)  ((k)==EK_EQ || (k)==EK_LT || (k)==EK_GT ||    \
		   (k)==EK_NE || (k)==EK_GE || (k)==EK_LE)

#define mathOp(k) ((k)==EK_PLUS || (k)==EK_TIMES || (k)==EK_NEG ||   \
		   (k)==EK_DIV || (k)==EK_DIVIDE || (k)==EK_MOD)

#define divOp(k)  ((k)==EK_DIV || (k)==EK_DIVIDE)


Static int incompat(ex, num, prec)
Expr *ex;
int num, prec;
{
    Expr *subex = ex->args[num];

    if (extraparens == 0)
	return prec;
    if (ex->kind == subex->kind) {
	if (logOp(ex->kind) || bitOp(ex->kind) ||
	    (divOp(ex->kind) && num == 0))
	    return -99;   /* not even invisible parens */
	else if (extraparens != 2)
	    return prec;
    }
    if (extraparens == 2)
	return 15;
    if (divOp(ex->kind) && num == 0 &&
	(subex->kind == EK_TIMES || divOp(subex->kind)))
	return -99;
    if (bitOp(ex->kind) || shfOp(ex->kind))
	return 15;
    if (relOp(ex->kind) && relOp(subex->kind))
	return 15;
    if ((relOp(ex->kind) || logOp(ex->kind)) && bitOp(subex->kind))
	return 15;
    if (ex->kind == EK_COMMA)
	return 15;
    if (ex->kind == EK_ASSIGN && relOp(subex->kind))
	return 15;
    if (extraparens != 1)
	return prec;
    if (ex->kind == EK_ASSIGN)
	return prec;
    if (relOp(ex->kind) && mathOp(subex->kind))
	return prec;
    return 15;
}




#define EXTRASPACE() if (spaceexprs == 1) output(" ")
#define NICESPACE()  if (spaceexprs != 0) output(" ")

#define setprec(p) \
    if ((subprec=(p)) <= prec) { \
        parens = 1; output("("); \
    }

#define setprec2(p) \
    if ((subprec=(p)) <= prec) { \
        parens = 1; output("("); \
    } else if (prec != -99) { \
        parens = 2; output((breakparens == 1) ? "\010" : "\003"); \
    }

#define setprec3(p) \
    if ((subprec=(p)) <= prec) { \
         parens = 1; output("("); \
    } else if (prec != -99) { \
         parens = 2; output((prec > 2 && breakparens != 0) ? "\010" \
			                                   : "\003"); \
    }


Static void outop3(breakbefore, name)
int breakbefore;
char *name;
{
    if (breakbefore & BRK_LEFT) {
	output("\002");
	if (breakbefore & BRK_RPREF)
	    output("\013");
    }
    output(name);
    if (breakbefore & BRK_HANG)
	output("\015");
    if (breakbefore & BRK_RIGHT) {
	output("\002");
	if (breakbefore & BRK_LPREF)
	    output("\013");
    }
}

#define outop(name) do { \
    NICESPACE(); outop3(breakflag, name); NICESPACE(); \
} while (0)

#define outop2(name) do { \
    EXTRASPACE(); outop3(breakflag, name); EXTRASPACE(); \
} while (0)

#define checkbreak(code) do { \
    breakflag=(code); \
    if ((prec != -99) && (breakflag & BRK_ALLNONE)) output("\007"); \
} while (0)


Static void out_ctx(ctx, address)
Meaning *ctx;
int address;
{
    Meaning *ctx2;
    int breakflag = breakbeforedot;

    if (ctx->kind == MK_FUNCTION && ctx->varstructflag) {
        if (curctx != ctx) {
	    if (address && curctx->ctx && curctx->ctx != ctx) {
		output("\003");
		if (breakflag & BRK_ALLNONE)
		    output("\007");
	    }
            output(format_s(name_LINK, curctx->ctx->name));
            ctx2 = curctx->ctx;
            while (ctx2 && ctx2 != ctx) {
                outop2("->");
                output(format_s(name_LINK, ctx2->ctx->name));
                ctx2 = ctx2->ctx;
            }
            if (ctx2 != ctx)
                intwarning("out_ctx",
                           format_s("variable from %s not present in context path [307]",
                                     ctx->name));
	    if (address && curctx->ctx && curctx->ctx != ctx)
		output("\004");
            if (!address)
                outop2("->");
        } else {
            if (address) {
                output("&");
		EXTRASPACE();
	    }
            output(format_s(name_VARS, curctx->name));
            if (!address) {
                outop2(".");
	    }
        }
    } else {
        if (address)
            output("NULL");
    }
}



void out_var(mp, prec)
Meaning *mp;
int prec;
{
    switch (mp->kind) {

        case MK_CONST:
            output(mp->name);
            return;

        case MK_VAR:
        case MK_VARREF:
        case MK_VARMAC:
        case MK_PARAM:
        case MK_VARPARAM:
            if (mp->varstructflag) {
		output("\003");
                out_ctx(mp->ctx, 0);
		output(mp->name);
		output("\004");
	    } else
		output(mp->name);
            return;

	default:
	    if (mp->name)
		output(mp->name);
	    else
		intwarning("out_var", "mp->sym == NULL [308]");
	    return;
    }
}



Static int scanfield(variants, unions, lev, mp, field)
Meaning **variants, *mp, *field;
short *unions;
int lev;
{
    int i, num, breakflag;
    Value v;

    unions[lev] = (mp && mp->kind == MK_VARIANT);
    while (mp && mp->kind == MK_FIELD) {
        if (mp == field) {
            for (i = 0; i < lev; i++) {
		v = variants[i]->val;    /* sidestep a Sun 386i compiler bug */
                num = ord_value(v);
		breakflag = breakbeforedot;
                if (!unions[i]) {
                    output(format_s(name_UNION, ""));
		    outop2(".");
                }
                if (variants[i]->ctx->cnext ||
                    variants[i]->ctx->kind != MK_FIELD) {
                    output(format_s(name_VARIANT, variantfieldname(num)));
		    outop2(".");
                }
            }
            output(mp->name);
            return 1;
        }
        mp = mp->cnext;
    }
    while (mp && mp->kind == MK_VARIANT) {
        variants[lev] = mp;
        if (scanfield(variants, unions, lev+1, mp->ctx, field))
            return 1;
        mp = mp->cnext;
    }
    return 0;
}


void out_field(mp)
Meaning *mp;
{
    Meaning *variants[50];
    short unions[51];

    if (!scanfield(variants, unions, 0, mp->rectype->fbase, mp))
        intwarning("out_field", "field name not in tree [309]");
    else if (mp->warnifused) {
        if (mp->rectype->meaning)
            note(format_ss("Reference to field %s of record %s [282]", 
                           mp->name, mp->rectype->meaning->name));
        else
            note(format_s("Reference to field %s [282]", mp->name));
    }
}




Static void wrexpr(ex, prec)
Expr *ex;
int prec;
{
    short parens = 0;
    int subprec, i, j, minusflag, breakflag = 0;
    int saveindent;
    Expr *ex2, *ex3;
    char *cp;
    Meaning *mp;
    Symbol *sp;

    if (debug>2) { fprintf(outf,"wrexpr{"); dumpexpr(ex); fprintf(outf,", %d}\n", prec); }
    switch (ex->kind) {

        case EK_VAR:
            mp = (Meaning *)ex->val.i;
            if (mp->warnifused)
                note(format_s("Reference to %s [283]", mp->name));
            out_var(mp, prec);
            break;

        case EK_NAME:
            output(ex->val.s);
            break;

        case EK_MACARG:
            output("<meef>");
            intwarning("wrexpr", "Stray EK_MACARG encountered [310]");
            break;

        case EK_CTX:
            out_ctx((Meaning *)ex->val.i, 1);
            break;

        case EK_CONST:
            if (ex->nargs > 0)
                cp = value_name(ex->val, ex->args[0]->val.s, 0);
            else
                cp = value_name(ex->val, NULL, 0);
            if (*cp == '-')
                setprec(14);
            output(cp);
            break;

        case EK_LONGCONST:
            if (ex->nargs > 0)
                cp = value_name(ex->val, ex->args[0]->val.s, 1);
            else
                cp = value_name(ex->val, NULL, 1);
            if (*cp == '-')
                setprec(14);
            output(cp);
            break;

        case EK_STRUCTCONST:
            ex3 = NULL;
            for (i = 0; i < ex->nargs; i++) {
                ex2 = ex->args[i];
                if (ex2->kind == EK_STRUCTOF) {
                    j = ex2->val.i;
                    ex2 = ex2->args[0];
                } else
                    j = 1;
                if (ex2->kind == EK_VAR) {
                    mp = (Meaning *)ex2->val.i;
                    if (mp->kind == MK_CONST &&
			mp->val.type &&
                        (mp->val.type->kind == TK_RECORD ||
                         mp->val.type->kind == TK_ARRAY)) {
                        if (foldconsts != 1)
                            note(format_s("Expanding constant %s into another constant [284]",
                                          mp->name));
                        ex2 = (Expr *)mp->val.i;
                    }
                }
                while (--j >= 0) {
                    if (ex3) {
                        if (ex3->kind == EK_STRUCTCONST ||
                            ex2->kind == EK_STRUCTCONST)
                            output(",\n");
                        else if (spacecommas)
                            output(",\001 ");
			else
			    output(",\001");
                    }
                    if (ex2->kind == EK_STRUCTCONST) {
                        output("{ \005");
			saveindent = outindent;
			moreindent(extrainitindent);
                        out_expr(ex2);
                        outindent = saveindent;
                        output(" }");
                    } else
                        out_expr(ex2);
                    ex3 = ex2;
                }
            }
            break;

        case EK_FUNCTION:
            mp = (Meaning *)ex->val.i;
	    sp = findsymbol_opt(mp->name);
	    if ((sp && (sp->flags & WARNLIBR)) || mp->warnifused)
                note(format_s("Called procedure %s [285]", mp->name));
            output(mp->name);
	    if (spacefuncs)
		output(" ");
            output("(\002");
	    j = sp ? (sp->flags & FUNCBREAK) : 0;
	    if (j == FALLBREAK)
		output("\007");
            for (i = 0; i < ex->nargs; i++) {
		if ((j == FSPCARG1 && i == 1) ||
		    (j == FSPCARG2 && i == 2) ||
		    (j == FSPCARG3 && i == 3))
		    if (spacecommas)
			output(",\011 ");
		    else
			output(",\011");
                else if (i > 0)
		    if (spacecommas)
			output(",\002 ");
		    else
			output(",\002");
                out_expr(ex->args[i]);
            }
            if (mp->ctx->kind == MK_FUNCTION && mp->ctx->varstructflag) {
                if (i > 0)
		    if (spacecommas)
			output(",\002 ");
		    else
			output(",\002");
                out_ctx(mp->ctx, 1);
            }
            output(")");
            break;

        case EK_BICALL:
            cp = ex->val.s;
            while (*cp == '*')
                cp++;
	    sp = findsymbol_opt(cp);
	    if (sp && (sp->flags & WARNLIBR))
                note(format_s("Called library procedure %s [286]", cp));
            output(cp);
	    if (spacefuncs)
		output(" ");
            output("(\002");
	    j = sp ? (sp->flags & FUNCBREAK) : 0;
	    if (j == FALLBREAK)
		output("\007");
            for (i = 0; i < ex->nargs; i++) {
		if ((j == FSPCARG1 && i == 1) ||
		    (j == FSPCARG2 && i == 2) ||
		    (j == FSPCARG3 && i == 3))
		    if (spacecommas)
			output(",\011 ");
		    else
			output(",\011");
                else if (i > 0)
		    if (spacecommas)
			output(",\002 ");
		    else
			output(",\002");
                out_expr(ex->args[i]);
            }
            output(")");
            break;

        case EK_SPCALL:
            setprec(16);
            if (starfunctions) {
                output("(\002*");
                wrexpr(ex->args[0], 13);
                output(")");
            } else
                wrexpr(ex->args[0], subprec-1);
	    if (spacefuncs)
		output(" ");
            output("(\002");
            for (i = 1; i < ex->nargs; i++) {
                if (i > 1)
		    if (spacecommas)
			output(",\002 ");
		    else
			output(",\002");
                out_expr(ex->args[i]);
            }
            output(")");
            break;

        case EK_INDEX:
            setprec(16);
            wrexpr(ex->args[0], subprec-1);
	    if (lookback(1) == ']')
		output("\001");
            output("[");
            out_expr(ex->args[1]);
            output("]");
            break;

        case EK_DOT:
            setprec2(16);
	    checkbreak(breakbeforedot);
            if (ex->args[0]->kind == EK_HAT) {
                wrexpr(ex->args[0]->args[0], subprec-1);
                outop2("->");
            } else if (ex->args[0]->kind == EK_CTX) {
                out_ctx((Meaning *)ex->args[0]->val.i, 0);
            } else {
                wrexpr(ex->args[0], subprec-1);
                outop2(".");
            }
            if (ex->val.i)
                out_field((Meaning *)ex->val.i);
            else
                output(ex->val.s);
            break;

        case EK_POSTINC:
	    if (prec == 0 && !postincrement) {
		setprec(14);
		output("++");
		EXTRASPACE();
		wrexpr(ex->args[0], subprec);
	    } else {
		setprec(15);
		wrexpr(ex->args[0], subprec);
		EXTRASPACE();
		output("++");
	    }
            break;

        case EK_POSTDEC:
	    if (prec == 0 && !postincrement) {
		setprec(14);
		output("--");
		EXTRASPACE();
		wrexpr(ex->args[0], subprec);
	    } else {
		setprec(15);
		wrexpr(ex->args[0], subprec);
		EXTRASPACE();
		output("--");
	    }
            break;

        case EK_HAT:
            setprec(14);
	    if (lookback_prn(1) == '/')
		output(" ");
            output("*");
            EXTRASPACE();
            wrexpr(ex->args[0], subprec-1);
            break;

        case EK_ADDR:
            setprec(14);
	    if (lookback_prn(1) == '&')
		output(" ");
            output("&");
            EXTRASPACE();
            wrexpr(ex->args[0], subprec-1);
            break;

        case EK_NEG:
            setprec(14);
            output("-");
            EXTRASPACE();
            if (ex->args[0]->kind == EK_TIMES)
                wrexpr(ex->args[0], 12);
            else
                wrexpr(ex->args[0], subprec-1);
            break;

        case EK_NOT:
            setprec(14);
            output("!");
            EXTRASPACE();
            wrexpr(ex->args[0], subprec-1);
            break;

        case EK_BNOT:
            setprec(14);
            output("~");
            EXTRASPACE();
            wrexpr(ex->args[0], subprec-1);
            break;

        case EK_CAST:
        case EK_ACTCAST:
            if (similartypes(ex->val.type, ex->args[0]->val.type)) {
                wrexpr(ex->args[0], prec);
            } else if (ord_type(ex->args[0]->val.type)->kind == TK_ENUM &&
                       ex->val.type == tp_int && !useenum) {
                wrexpr(ex->args[0], prec);
            } else {
                setprec2(14);
                output("(");
                out_type(ex->val.type, 0);
                output(")\002");
                EXTRASPACE();
                if (extraparens != 0)
                    wrexpr(ex->args[0], 15);
                else
                    wrexpr(ex->args[0], subprec-1);
            }
            break;

        case EK_LITCAST:
            setprec2(14);
            output("(");
            out_expr(ex->args[0]);
            output(")\002");
            EXTRASPACE();
            if (extraparens != 0)
                wrexpr(ex->args[1], 15);
            else
                wrexpr(ex->args[1], subprec-1);
            break;

        case EK_SIZEOF:
            setprec(14);
            output("sizeof");
	    if (spacefuncs)
		output(" ");
	    output("(");
	    out_expr(ex->args[0]);
            output(")");
            break;

	case EK_TYPENAME:
	    out_type(ex->val.type, 1);
	    break;

        case EK_TIMES:
	    setprec2(13);
	    checkbreak(breakbeforearith);
            ex2 = copyexpr(ex);
            if (expr_looks_neg(ex2->args[ex2->nargs-1])) {
                ex2->args[0] = makeexpr_neg(ex2->args[0]);
                ex2->args[ex2->nargs-1] = makeexpr_neg(ex2->args[ex2->nargs-1]);
            }
            wrexpr(ex2->args[0], incompat(ex2, 0, subprec-1));
            for (i = 1; i < ex2->nargs; i++) {
                outop("*");
                wrexpr(ex2->args[i], incompat(ex2, i, subprec));
            }
            freeexpr(ex2);
            break;

        case EK_DIV:
        case EK_DIVIDE:
            setprec2(13);
	    checkbreak(breakbeforearith);
	    wrexpr(ex->args[0], incompat(ex, 0, subprec-1));
            outop("/");
            wrexpr(ex->args[1], incompat(ex, 1, subprec));
            break;

        case EK_MOD:
            setprec2(13);
	    checkbreak(breakbeforearith);
            wrexpr(ex->args[0], incompat(ex, 0, subprec-1));
            outop("%");
            wrexpr(ex->args[1], incompat(ex, 1, subprec));
            break;

        case EK_PLUS:
            setprec2(12);
	    checkbreak(breakbeforearith);
            ex2 = copyexpr(ex);
            minusflag = 0;
            if (expr_looks_neg(ex2->args[0])) {
                j = 1;
                while (j < ex2->nargs && expr_looks_neg(ex2->args[j])) j++;
                if (j < ex2->nargs)
                    swapexprs(ex2->args[0], ex2->args[j]);
            } else if (ex2->val.i && ex2->nargs == 2) {   /* this was originally "a-b" */
                if (isliteralconst(ex2->args[1], NULL) != 2) {
                    if (expr_neg_cost(ex2->args[1]) <= 0) {
                        minusflag = 1;
                    } else if (expr_neg_cost(ex2->args[0]) <= 0) {
                        swapexprs(ex2->args[0], ex2->args[1]);
                        if (isliteralconst(ex2->args[0], NULL) != 2)
                            minusflag = 1;
                    }
                }
            }
            wrexpr(ex2->args[0], incompat(ex, 0, subprec));
            for (i = 1; i < ex2->nargs; i++) {
                if (expr_looks_neg(ex2->args[i]) || minusflag) {
                    outop("-");
                    ex2->args[i] = makeexpr_neg(ex2->args[i]);
                } else
                    outop("+");
                wrexpr(ex2->args[i], incompat(ex, i, subprec));
            }
            freeexpr(ex2);
            break;

        case EK_LSH:
            setprec3(11);
	    checkbreak(breakbeforearith);
            wrexpr(ex->args[0], incompat(ex, 0, subprec));
            outop("<<");
            wrexpr(ex->args[1], incompat(ex, 1, subprec));
            break;

        case EK_RSH:
            setprec3(11);
	    checkbreak(breakbeforearith);
	    wrexpr(ex->args[0], incompat(ex, 0, subprec));
            outop(">>");
            wrexpr(ex->args[1], incompat(ex, 1, subprec));
            break;

        case EK_LT:
            setprec2(10);
	    checkbreak(breakbeforerel);
            wrexpr(ex->args[0], incompat(ex, 0, subprec));
            outop("<");
            wrexpr(ex->args[1], incompat(ex, 0, subprec));
            break;

        case EK_GT:
            setprec2(10);
	    checkbreak(breakbeforerel);
            wrexpr(ex->args[0], incompat(ex, 0, subprec));
            outop(">");
            wrexpr(ex->args[1], incompat(ex, 0, subprec));
            break;

        case EK_LE:
            setprec2(10);
	    checkbreak(breakbeforerel);
            wrexpr(ex->args[0], incompat(ex, 0, subprec));
            outop("<=");
            wrexpr(ex->args[1], incompat(ex, 0, subprec));
            break;

        case EK_GE:
            setprec2(10);
	    checkbreak(breakbeforerel);
            wrexpr(ex->args[0], incompat(ex, 0, subprec));
            outop(">=");
            wrexpr(ex->args[1], incompat(ex, 0, subprec));
            break;

        case EK_EQ:
            setprec2(9);
	    checkbreak(breakbeforerel);
            wrexpr(ex->args[0], incompat(ex, 0, subprec));
            outop("==");
            wrexpr(ex->args[1], incompat(ex, 0, subprec));
            break;

        case EK_NE:
            setprec2(9);
	    checkbreak(breakbeforerel);
            wrexpr(ex->args[0], incompat(ex, 0, subprec));
            outop("!=");
            wrexpr(ex->args[1], incompat(ex, 0, subprec));
            break;

        case EK_BAND:
            setprec3(8);
	    if (ex->val.type == tp_boolean)
		checkbreak(breakbeforelog);
	    else
		checkbreak(breakbeforearith);
            wrexpr(ex->args[0], incompat(ex, 0, subprec-1));
	    outop("&");
            wrexpr(ex->args[1], incompat(ex, 1, subprec-1));
            break;

        case EK_BXOR:
            setprec3(7);
	    checkbreak(breakbeforearith);
            wrexpr(ex->args[0], incompat(ex, 0, subprec-1));
            outop("^");
            wrexpr(ex->args[1], incompat(ex, 1, subprec-1));
            break;

        case EK_BOR:
            setprec3(6);
	    if (ex->val.type == tp_boolean)
		checkbreak(breakbeforelog);
	    else
		checkbreak(breakbeforearith);
            wrexpr(ex->args[0], incompat(ex, 0, subprec-1));
	    outop("|");
            wrexpr(ex->args[1], incompat(ex, 1, subprec-1));
            break;

        case EK_AND:
            setprec3(5);
	    checkbreak(breakbeforelog);
	    wrexpr(ex->args[0], incompat(ex, 0, subprec-1));
            outop("&&");
	    wrexpr(ex->args[1], incompat(ex, 1, subprec-1));
            break;

        case EK_OR:
            setprec3(4);
	    checkbreak(breakbeforelog);
	    wrexpr(ex->args[0], incompat(ex, 0, subprec-1));
            outop("||");
	    wrexpr(ex->args[1], incompat(ex, 1, subprec-1));
            break;

        case EK_COND:
            setprec3(3);
	    i = 0;
	    for (;;) {
		i++;
		if (extraparens != 0)
		    wrexpr(ex->args[0], 15);
		else
		    wrexpr(ex->args[0], subprec);
		NICESPACE();
		output("\002?");
		NICESPACE();
		out_expr(ex->args[1]);
		if (ex->args[2]->kind == EK_COND) {
		    NICESPACE();
		    output("\002:");
		    NICESPACE();
		    ex = ex->args[2];
		} else {
		    NICESPACE();
		    output((i == 1) ? "\017:" : "\002:");
		    NICESPACE();
		    wrexpr(ex->args[2], subprec-1);
		    break;
		}
	    }
            break;

        case EK_ASSIGN:
            if (ex->args[1]->kind == EK_PLUS &&
                exprsame(ex->args[1]->args[0], ex->args[0], 2) &&
                ex->args[1]->args[1]->kind == EK_CONST &&
                ex->args[1]->args[1]->val.type->kind == TK_INTEGER &&
                abs(ex->args[1]->args[1]->val.i) == 1) {
		if (prec == 0 && postincrement) {
		    setprec(15);
		    wrexpr(ex->args[0], subprec);
		    EXTRASPACE();
		    if (ex->args[1]->args[1]->val.i == 1)
			output("++");
		    else
			output("--");
		} else {
		    setprec(14);
		    if (ex->args[1]->args[1]->val.i == 1)
			output("++");
		    else
			output("--");
		    EXTRASPACE();
		    wrexpr(ex->args[0], subprec-1);
		}
            } else {
                setprec2(2);
		checkbreak(breakbeforeassign);
                wrexpr(ex->args[0], subprec);
                ex2 = copyexpr(ex->args[1]);
                j = -1;
                switch (ex2->kind) {

                    case EK_PLUS:
                    case EK_TIMES:
                    case EK_BAND:
                    case EK_BOR:
                    case EK_BXOR:
                        for (i = 0; i < ex2->nargs; i++) {
                            if (exprsame(ex->args[0], ex2->args[i], 2)) {
                                j = i;
                                break;
                            }
                            if (ex2->val.type->kind == TK_REAL)
                                break;   /* non-commutative */
                        }
                        break;

                    case EK_DIVIDE:
                    case EK_DIV:
                    case EK_MOD:
                    case EK_LSH:
                    case EK_RSH:
                        if (exprsame(ex->args[0], ex2->args[0], 2))
                            j = 0;
                        break;

		    default:
			break;
                }
                if (j >= 0) {
                    if (ex2->nargs == 2)
                        ex2 = grabarg(ex2, 1-j);
                    else
                        delfreearg(&ex2, j);
                    switch (ex->args[1]->kind) {

                        case EK_PLUS:
                            if (expr_looks_neg(ex2)) {
                                outop("-=");
                                ex2 = makeexpr_neg(ex2);
                            } else
                                outop("+=");
                            break;

                        case EK_TIMES:
                            outop("*=");
                            break;

                        case EK_DIVIDE:
                        case EK_DIV:
                            outop("/=");
                            break;

                        case EK_MOD:
                            outop("%=");
                            break;

                        case EK_LSH:
                            outop("<<=");
                            break;

                        case EK_RSH:
                            outop(">>=");
                            break;

                        case EK_BAND:
                            outop("&=");
                            break;

                        case EK_BOR:
                            outop("|=");
                            break;

                        case EK_BXOR:
                            outop("^=");
                            break;

			default:
			    break;
                    }
                } else {
		    output(" ");
		    outop3(breakbeforeassign, "=");
		    output(" ");
                }
                if (extraparens != 0 &&
                    (ex2->kind == EK_EQ || ex2->kind == EK_NE ||
                     ex2->kind == EK_GT || ex2->kind == EK_LT ||
                     ex2->kind == EK_GE || ex2->kind == EK_LE ||
                     ex2->kind == EK_AND || ex2->kind == EK_OR))
                    wrexpr(ex2, 16);
                else
                    wrexpr(ex2, subprec-1);
                freeexpr(ex2);
            }
            break;

        case EK_COMMA:
            setprec3(1);
            for (i = 0; i < ex->nargs-1; i++) {
                wrexpr(ex->args[i], subprec);
                output(",\002");
		if (spacecommas)
		    NICESPACE();
            }
            wrexpr(ex->args[ex->nargs-1], subprec);
            break;

        default:
            intwarning("wrexpr", "bad ex->kind [311]");
    }
    switch (parens) {
      case 1:
        output(")");
	break;
      case 2:
	output("\004");
	break;
    }
}



/* will parenthesize assignments and "," operators */

void out_expr(ex)
Expr *ex;
{
    wrexpr(ex, 2);
}



/* will not parenthesize anything at top level */

void out_expr_top(ex)
Expr *ex;
{
    wrexpr(ex, 0);
}



/* will parenthesize unless only writing a factor */

void out_expr_factor(ex)
Expr *ex;
{
    wrexpr(ex, 15);
}



/* will parenthesize always */

void out_expr_parens(ex)
Expr *ex;
{
    output("(");
    wrexpr(ex, 1);
    output(")");
}



/* evaluate expression for side effects only */
/* no top-level parentheses */

void out_expr_stmt(ex)
Expr *ex;
{
    wrexpr(ex, 0);
}



/* evaluate expression for boolean (zero/non-zero) result only */
/* parenthesizes like out_expr() */

void out_expr_bool(ex)
Expr *ex;
{
    wrexpr(ex, 2);
}




/* End. */



