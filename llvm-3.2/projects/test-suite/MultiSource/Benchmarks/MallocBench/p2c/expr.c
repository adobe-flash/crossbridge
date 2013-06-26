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



#define PROTO_EXPR_C
#include "trans.h"





void free_value(val)
Value *val;
{
    if (!val || !val->type)
	return;
    switch (val->type->kind) {

        case TK_STRING:
        case TK_REAL:
        case TK_ARRAY:
        case TK_RECORD:
        case TK_SET:
            if (val->s)
                FREE(val->s);
            break;

	default:
	    break;
    }
}


Value copyvalue(val)
Value val;
{
    char *cp;

    switch (val.type->kind) {

        case TK_STRING:
        case TK_SET:
            if (val.s) {
                cp = ALLOC(val.i+1, char, literals);
                memcpy(cp, val.s, val.i);
		cp[val.i] = 0;
                val.s = cp;
            }
            break;

        case TK_REAL:
        case TK_ARRAY:
        case TK_RECORD:
            if (val.s)
                val.s = stralloc(val.s);
            break;

	default:
	    break;
    }
    return val;
}


int valuesame(a, b)
Value a, b;
{
    if (a.type != b.type)
        return 0;
    switch (a.type->kind) {

        case TK_INTEGER:
        case TK_CHAR:
        case TK_BOOLEAN:
        case TK_ENUM:
        case TK_SMALLSET:
        case TK_SMALLARRAY:
            return (a.i == b.i);

        case TK_STRING:
        case TK_SET:
            return (a.i == b.i && !memcmp(a.s, b.s, a.i));

        case TK_REAL:
        case TK_ARRAY:
        case TK_RECORD:
            return (!strcmp(a.s, b.s));

        default:
            return 1;
    }
}



char *value_name(val, intfmt, islong)
Value val;
char *intfmt;
int islong;
{
    Meaning *mp;
    Type *type = val.type;

    if (type->kind == TK_SUBR)
	type = type->basetype;
    switch (type->kind) {

        case TK_INTEGER:
        case TK_SMALLSET:
        case TK_SMALLARRAY:
            if (!intfmt)
		intfmt = "%ld";
	    if (*intfmt == '\'') {
		if (val.i >= -'~' && val.i <= -' ') {
		    intfmt = format_s("-%s", intfmt);
		    val.i = -val.i;
		}
		if (val.i < ' ' || val.i > '~' || islong)
		    intfmt = "%ld";
	    }
            if (islong)
                intfmt = format_s("%sL", intfmt);
            return format_d(intfmt, val.i);

        case TK_REAL:
            return val.s;

        case TK_ARRAY:    /* obsolete */
        case TK_RECORD:   /* obsolete */
            return val.s;

        case TK_STRING:
            return makeCstring(val.s, val.i);

        case TK_BOOLEAN:
            if (!intfmt)
                if (val.i == 1 && *name_TRUE &&
		    strcmp(name_TRUE, "1") && !islong)
                    intfmt = name_TRUE;
                else if (val.i == 0 && *name_FALSE &&
			 strcmp(name_FALSE, "0") && !islong)
                    intfmt = name_FALSE;
                else
                    intfmt = "%ld";
            if (islong)
                intfmt = format_s("%sL", intfmt);
            return format_d(intfmt, val.i);

        case TK_CHAR:
            if (islong)
                return format_d("%ldL", val.i);
	    else if ((val.i < 0 || val.i > 127) && highcharints)
		return format_d("%ld", val.i);
            else
                return makeCchar(val.i);

        case TK_POINTER:
            return (*name_NULL) ? name_NULL : "NULL";

        case TK_ENUM:
            mp = val.type->fbase;
            while (mp && mp->val.i != val.i)
                mp = mp->xnext;
            if (!mp) {
                intwarning("value_name", "bad enum value [152]");
                return format_d("%ld", val.i);
            }
            return mp->name;

        default:
            intwarning("value_name", format_s("bad type for constant: %s [153]",
                                              typekindname(type->kind)));
            return "<spam>";
    }
}




Value value_cast(val, type)
Value val;
Type *type;
{
    char buf[20];

    if (type->kind == TK_SUBR)
        type = type->basetype;
    if (val.type == type)
        return val;
    if (type && val.type) {
        switch (type->kind) {

            case TK_REAL:
                if (ord_type(val.type)->kind == TK_INTEGER) {
                    sprintf(buf, "%d.0", val.i);
                    val.s = stralloc(buf);
                    val.type = tp_real;
                    return val;
                }
                break;

            case TK_CHAR:
                if (val.type->kind == TK_STRING) {
                    if (val.i != 1)
                        if (val.i > 0)
                            warning("Char constant with more than one character [154]");
                        else
                            warning("Empty char constant [155]");
                    val.i = val.s[0] & 0xff;
                    val.s = NULL;
                    val.type = tp_char;
                    return val;
                }

            case TK_POINTER:
                if (val.type == tp_anyptr && castnull != 1) {
                    val.type = type;
                    return val;
                }

	    default:
		break;
        }
    }
    val.type = NULL;
    return val;
}



Type *ord_type(tp)
Type *tp;
{
    if (!tp) {
        warning("Expected a constant [127]");
        return tp_integer;
    }
    switch (tp->kind) {

        case TK_SUBR:
            tp = tp->basetype;
            break;

        case TK_STRING:
            if (!CHECKORDEXPR(tp->indextype->smax, 1))
                tp = tp_char;
            break;

	default:
	    break;

    }
    return tp;
}



int long_type(tp)
Type *tp;
{
    switch (tp->kind) {

        case TK_INTEGER:
            return (tp != tp_int && tp != tp_uint && tp != tp_sint);

        case TK_SUBR:
            return (findbasetype(tp, ODECL_NOPRES) == tp_integer);

        default:
            return 0;
    }
}



Value make_ord(type, i)
Type *type;
long i;
{
    Value val;

    if (type->kind == TK_ENUM)
        type = findbasetype(type, ODECL_NOPRES);
    if (type->kind == TK_SUBR)
        type = type->basetype;
    val.type = type;
    val.i = i;
    val.s = NULL;
    return val;
}



long ord_value(val)
Value val;
{
    switch (val.type->kind) {

        case TK_INTEGER:
        case TK_ENUM:
        case TK_CHAR:
        case TK_BOOLEAN:
            return val.i;

        case TK_STRING:
            if (val.i == 1)
                return val.s[0] & 0xff;

        /* fall through */
        default:
            warning("Expected an ordinal type [156]");
            return 0;
    }
}



void ord_range_expr(type, smin, smax)
Type *type;
Expr **smin, **smax;
{
    if (!type) {
        warning("Expected a constant [127]");
        type = tp_integer;
    }
    if (type->kind == TK_STRING)
        type = tp_char;
    switch (type->kind) {

        case TK_SUBR:
        case TK_INTEGER:
        case TK_ENUM:
        case TK_CHAR:
        case TK_BOOLEAN:
            if (smin) *smin = type->smin;
            if (smax) *smax = type->smax;
            break;

        default:
            warning("Expected an ordinal type [156]");
            if (smin) *smin = makeexpr_long(0);
            if (smax) *smax = makeexpr_long(1);
            break;
    }
}


int ord_range(type, smin, smax)
Type *type;
long *smin, *smax;
{
    Expr *emin, *emax;
    Value vmin, vmax;

    ord_range_expr(type, &emin, &emax);
    if (smin) {
        vmin = eval_expr(emin);
        if (!vmin.type)
            return 0;
    }
    if (smax) {
        vmax = eval_expr(emax);
        if (!vmax.type)
            return 0;
    }
    if (smin) *smin = ord_value(vmin);
    if (smax) *smax = ord_value(vmax);
    return 1;
}







void freeexpr(ex)
register Expr *ex;
{
    register int i;

    if (ex) {
        for (i = 0; i < ex->nargs; i++)
            freeexpr(ex->args[i]);
        switch (ex->kind) {

            case EK_CONST:
            case EK_LONGCONST:
                free_value(&ex->val);
                break;

            case EK_DOT:
            case EK_NAME:
            case EK_BICALL:
                if (ex->val.s)
                    FREE(ex->val.s);
                break;

	    default:
		break;
        }
        FREE(ex);
    }
}




Expr *makeexpr(kind, n)
enum exprkind kind;
int n;
{
    Expr *ex;

    ex = ALLOCV(sizeof(Expr) + (n-1)*sizeof(Expr *), Expr, exprs);
    ex->val.i = 0;
    ex->val.s = NULL;
    ex->kind = kind;
    ex->nargs = n;
    return ex;
}


Expr *makeexpr_un(kind, type, arg1)
enum exprkind kind;
Type *type;
Expr *arg1;
{
    Expr *ex;

    ex = makeexpr(kind, 1);
    ex->val.type = type;
    ex->args[0] = arg1;
    if (debug>2) { fprintf(outf,"makeexpr_un returns "); dumpexpr(ex); fprintf(outf,"\n"); }
    return ex;
}


Expr *makeexpr_bin(kind, type, arg1, arg2)
enum exprkind kind;
Type *type;
Expr *arg1, *arg2;
{
    Expr *ex;

    ex = makeexpr(kind, 2);
    ex->val.type = type;
    ex->args[0] = arg1;
    ex->args[1] = arg2;
    if (debug>2) { fprintf(outf,"makeexpr_bin returns "); dumpexpr(ex); fprintf(outf,"\n"); }
    return ex;
}



Expr *makeexpr_val(val)
Value val;
{
    Expr *ex;

    if (val.type->kind == TK_INTEGER &&
        (val.i < -32767 || val.i > 32767) &&
        sizeof_int < 32)
        ex = makeexpr(EK_LONGCONST, 0);
    else
        ex = makeexpr(EK_CONST, 0);
    ex->val = val;
    if (debug>2) { fprintf(outf,"makeexpr_val returns "); dumpexpr(ex); fprintf(outf,"\n"); }
    return ex;
}



Expr *makeexpr_char(c)
int c;
{
    return makeexpr_val(make_ord(tp_char, c));
}


Expr *makeexpr_long(i)
long i;
{
    return makeexpr_val(make_ord(tp_integer, i));
}


Expr *makeexpr_real(r)
char *r;
{
    Value val;

    val.type = tp_real;
    val.i = 0;
    val.s = stralloc(r);
    return makeexpr_val(val);
}


Expr *makeexpr_lstring(msg, len)
char *msg;
int len;
{
    Value val;

    val.type = tp_str255;
    val.i = len;
    val.s = ALLOC(len+1, char, literals);
    memcpy(val.s, msg, len);
    val.s[len] = 0;
    return makeexpr_val(val);
}


Expr *makeexpr_string(msg)
char *msg;
{
    Value val;

    val.type = tp_str255;
    val.i = strlen(msg);
    val.s = stralloc(msg);
    return makeexpr_val(val);
}


int checkstring(ex, msg)
Expr *ex;
char *msg;
{
    if (!ex || ex->val.type->kind != TK_STRING || ex->kind != EK_CONST)
        return 0;
    if (ex->val.i != strlen(msg))
        return 0;
    return memcmp(ex->val.s, msg, ex->val.i) == 0;
}



Expr *makeexpr_var(mp)
Meaning *mp;
{
    Expr *ex;

    ex = makeexpr(EK_VAR, 0);
    ex->val.i = (long) mp;
    ex->val.type = mp->type;
    if (debug>2) { fprintf(outf,"makeexpr_var returns "); dumpexpr(ex); fprintf(outf,"\n"); }
    return ex;
}



Expr *makeexpr_name(name, type)
char *name;
Type *type;
{
    Expr *ex;

    ex = makeexpr(EK_NAME, 0);
    ex->val.s = stralloc(name);
    ex->val.type = type;
    if (debug>2) { fprintf(outf,"makeexpr_name returns "); dumpexpr(ex); fprintf(outf,"\n"); }
    return ex;
}



Expr *makeexpr_setbits()
{
    if (*name_SETBITS)
        return makeexpr_name(name_SETBITS, tp_integer);
    else
        return makeexpr_long(setbits);
}



/* Note: BICALL's to the following functions should obey the ANSI standard. */
/*       Non-ANSI transformations occur while writing the expression. */
/*              char *sprintf(buf, fmt, ...)   [returns buf]  */
/*              void *memcpy(dest, src, size)  [returns dest] */

Expr *makeexpr_bicall_0(name, type)
char *name;
Type *type;
{
    Expr *ex;

    if (!name || !*name) {
        intwarning("makeexpr_bicall_0", "Required name of built-in procedure is missing [157]");
        name = "MissingProc";
    }
    ex = makeexpr(EK_BICALL, 0);
    ex->val.s = stralloc(name);
    ex->val.type = type;
    if (debug>2) { fprintf(outf,"makeexpr_bicall returns "); dumpexpr(ex); fprintf(outf,"\n"); }
    return ex;
}


Expr *makeexpr_bicall_1(name, type, arg1)
char *name;
Type *type;
Expr *arg1;
{
    Expr *ex;

    if (!name || !*name) {
        intwarning("makeexpr_bicall_1", "Required name of built-in procedure is missing [157]");
        name = "MissingProc";
    }
    ex = makeexpr(EK_BICALL, 1);
    ex->val.s = stralloc(name);
    ex->val.type = type;
    ex->args[0] = arg1;
    if (debug>2) { fprintf(outf,"makeexpr_bicall returns "); dumpexpr(ex); fprintf(outf,"\n"); }
    return ex;
}


Expr *makeexpr_bicall_2(name, type, arg1, arg2)
char *name;
Type *type;
Expr *arg1, *arg2;
{
    Expr *ex;

    if (!name || !*name) {
        intwarning("makeexpr_bicall_2", "Required name of built-in procedure is missing [157]");
        name = "MissingProc";
    }
    ex = makeexpr(EK_BICALL, 2);
    if (!strcmp(name, "~SETIO"))
        name = (iocheck_flag) ? "~~SETIO" : name_SETIO;
    ex->val.s = stralloc(name);
    ex->val.type = type;
    ex->args[0] = arg1;
    ex->args[1] = arg2;
    if (debug>2) { fprintf(outf,"makeexpr_bicall returns "); dumpexpr(ex); fprintf(outf,"\n"); }
    return ex;
}


Expr *makeexpr_bicall_3(name, type, arg1, arg2, arg3)
char *name;
Type *type;
Expr *arg1, *arg2, *arg3;
{
    Expr *ex;

    if (!name || !*name) {
        intwarning("makeexpr_bicall_3", "Required name of built-in procedure is missing [157]");
        name = "MissingProc";
    }
    ex = makeexpr(EK_BICALL, 3);
    ex->val.s = stralloc(name);
    ex->val.type = type;
    ex->args[0] = arg1;
    ex->args[1] = arg2;
    ex->args[2] = arg3;
    if (debug>2) { fprintf(outf,"makeexpr_bicall returns "); dumpexpr(ex); fprintf(outf,"\n"); }
    return ex;
}


Expr *makeexpr_bicall_4(name, type, arg1, arg2, arg3, arg4)
char *name;
Type *type;
Expr *arg1, *arg2, *arg3, *arg4;
{
    Expr *ex;

    if (!name || !*name) {
        intwarning("makeexpr_bicall_4", "Required name of built-in procedure is missing [157]");
        name = "MissingProc";
    }
    ex = makeexpr(EK_BICALL, 4);
    if (!strcmp(name, "~CHKIO"))
        name = (iocheck_flag) ? "~~CHKIO" : name_CHKIO;
    ex->val.s = stralloc(name);
    ex->val.type = type;
    ex->args[0] = arg1;
    ex->args[1] = arg2;
    ex->args[2] = arg3;
    ex->args[3] = arg4;
    if (debug>2) { fprintf(outf,"makeexpr_bicall returns "); dumpexpr(ex); fprintf(outf,"\n"); }
    return ex;
}


Expr *makeexpr_bicall_5(name, type, arg1, arg2, arg3, arg4, arg5)
char *name;
Type *type;
Expr *arg1, *arg2, *arg3, *arg4, *arg5;
{
    Expr *ex;

    if (!name || !*name) {
        intwarning("makeexpr_bicall_5", "Required name of built-in procedure is missing [157]");
        name = "MissingProc";
    }
    ex = makeexpr(EK_BICALL, 5);
    ex->val.s = stralloc(name);
    ex->val.type = type;
    ex->args[0] = arg1;
    ex->args[1] = arg2;
    ex->args[2] = arg3;
    ex->args[3] = arg4;
    ex->args[4] = arg5;
    if (debug>2) { fprintf(outf,"makeexpr_bicall returns "); dumpexpr(ex); fprintf(outf,"\n"); }
    return ex;
}




Expr *copyexpr(ex)
register Expr *ex;
{
    register int i;
    register Expr *ex2;

    if (ex) {
        ex2 = makeexpr(ex->kind, ex->nargs);
        for (i = 0; i < ex->nargs; i++)
            ex2->args[i] = copyexpr(ex->args[i]);
        switch (ex->kind) {

            case EK_CONST:
            case EK_LONGCONST:
                ex2->val = copyvalue(ex->val);
                break;

            case EK_DOT:
            case EK_NAME:
            case EK_BICALL:
                ex2->val.type = ex->val.type;
                ex2->val.i = ex->val.i;
                if (ex->val.s)
                    ex2->val.s = stralloc(ex->val.s);
                break;

            default:
                ex2->val = ex->val;
                break;
        }
        return ex2;
    } else
        return NULL;
}



int exprsame(a, b, strict)
register Expr *a, *b;
int strict;
{
    register int i;

    if (!a)
        return (!b);
    if (!b)
        return 0;
    if (a->val.type != b->val.type && strict != 2) {
        if (strict ||
	    !((a->val.type->kind == TK_POINTER &&
	       a->val.type->basetype == b->val.type) ||
	      (b->val.type->kind == TK_POINTER &&
	       b->val.type->basetype == a->val.type)))
        return 0;
    }
    if (a->kind != b->kind || a->nargs != b->nargs)
        return 0;
    switch (a->kind) {

        case EK_CONST:
        case EK_LONGCONST:
            if (!valuesame(a->val, b->val))
                return 0;
            break;

        case EK_BICALL:
        case EK_NAME:
            if (strcmp(a->val.s, b->val.s))
                return 0;
            break;

        case EK_VAR:
        case EK_FUNCTION:
        case EK_CTX:
        case EK_MACARG:
            if (a->val.i != b->val.i)
                return 0;
            break;

        case EK_DOT:
            if (a->val.i != b->val.i ||
                (!a->val.i && strcmp(a->val.s, b->val.s)))
                return 0;
            break;

	default:
	    break;
    }
    i = a->nargs;
    while (--i >= 0)
        if (!exprsame(a->args[i], b->args[i], (strict == 2) ? 1 : strict))
            return 0;
    return 1;
}



int exprequiv(a, b)
register Expr *a, *b;
{
    register int i, j, k;
    enum exprkind kind2;

    if (!a)
        return (!b);
    if (!b)
        return 0;
    switch (a->kind) {

        case EK_PLUS:
        case EK_TIMES:
        case EK_BAND:
        case EK_BOR:
        case EK_BXOR:
        case EK_EQ:
        case EK_NE:
            if (b->kind != a->kind || b->nargs != a->nargs ||
                b->val.type != a->val.type)
                return 0;
            if (a->nargs > 3)
                break;
            for (i = 0; i < b->nargs; i++) {
                if (exprequiv(a->args[0], b->args[i])) {
                    for (j = 0; j < b->nargs; j++) {
                        if (j != i &&
                            exprequiv(a->args[1], b->args[i])) {
                            if (a->nargs == 2)
                                return 1;
                            for (k = 0; k < b->nargs; k++) {
                                if (k != i && k != j &&
                                    exprequiv(a->args[2], b->args[k]))
                                    return 1;
                            }
                        }
                    }
                }
            }
            break;

        case EK_LT:
        case EK_GT:
        case EK_LE:
        case EK_GE:
            switch (a->kind) {
                case EK_LT:  kind2 = EK_GT; break;
                case EK_GT:  kind2 = EK_LT; break;
                case EK_LE:  kind2 = EK_GE; break;
                default:     kind2 = EK_LE; break;
            }
            if (b->kind != kind2 || b->val.type != a->val.type)
                break;
            if (exprequiv(a->args[0], b->args[1]) &&
                exprequiv(a->args[1], b->args[0])) {
                return 1;
            }
            break;

        case EK_CONST:
        case EK_LONGCONST:
        case EK_BICALL:
        case EK_NAME:
        case EK_VAR:
        case EK_FUNCTION:
        case EK_CTX:
        case EK_DOT:
            return exprsame(a, b, 0);

	default:
	    break;
    }
    if (b->kind != a->kind || b->nargs != a->nargs ||
        b->val.type != a->val.type)
        return 0;
    i = a->nargs;
    while (--i >= 0)
        if (!exprequiv(a->args[i], b->args[i]))
            return 0;
    return 1;
}



void deletearg(ex, n)
Expr **ex;
register int n;
{
    register Expr *ex1 = *ex, *ex2;
    register int i;

    if (debug>2) { fprintf(outf,"deletearg("); dumpexpr(*ex); fprintf(outf,", %d)\n", n); }
    if (n < 0 || n >= (*ex)->nargs) {
        intwarning("deletearg", "argument number out of range [158]");
        return;
    }
    ex2 = makeexpr(ex1->kind, ex1->nargs-1);
    ex2->val = ex1->val;
    for (i = 0; i < n; i++)
        ex2->args[i] = ex1->args[i];
    for (; i < ex2->nargs; i++)
        ex2->args[i] = ex1->args[i+1];
    *ex = ex2;
    FREE(ex1);
    if (debug>2) { fprintf(outf,"deletearg returns "); dumpexpr(*ex); fprintf(outf,"\n"); }
}



void insertarg(ex, n, arg)
Expr **ex;
Expr *arg;
register int n;
{
    register Expr *ex1 = *ex, *ex2;
    register int i;

    if (debug>2) { fprintf(outf,"insertarg("); dumpexpr(*ex); fprintf(outf,", %d)\n", n); }
    if (n < 0 || n > (*ex)->nargs) {
        intwarning("insertarg", "argument number out of range [159]");
        return;
    }
    ex2 = makeexpr(ex1->kind, ex1->nargs+1);
    ex2->val = ex1->val;
    for (i = 0; i < n; i++)
        ex2->args[i] = ex1->args[i];
    ex2->args[n] = arg;
    for (; i < ex1->nargs; i++)
        ex2->args[i+1] = ex1->args[i];
    *ex = ex2;
    FREE(ex1);
    if (debug>2) { fprintf(outf,"insertarg returns "); dumpexpr(*ex); fprintf(outf,"\n"); }
}



Expr *grabarg(ex, n)
Expr *ex;
int n;
{
    Expr *ex2;

    if (n < 0 || n >= ex->nargs) {
        intwarning("grabarg", "argument number out of range [160]");
        return ex;
    }
    ex2 = ex->args[n];
    ex->args[n] = makeexpr_long(0);   /* placeholder */
    freeexpr(ex);
    return ex2;
}



void delsimparg(ep, n)
Expr **ep;
int n;
{
    if (n < 0 || n >= (*ep)->nargs) {
        intwarning("delsimparg", "argument number out of range [161]");
        return;
    }
    deletearg(ep, n);
    switch ((*ep)->kind) {

        case EK_PLUS:
        case EK_TIMES:
        case EK_COMMA:
            if ((*ep)->nargs == 1)
                *ep = grabarg(*ep, 0);
            break;

	default:
	    break;

    }
}




Expr *resimplify(ex)
Expr *ex;
{
    Expr *ex2;
    Type *type;
    int i;

    if (debug>2) { fprintf(outf,"resimplify("); dumpexpr(ex); fprintf(outf,")\n"); }
    if (!ex)
        return NULL;
    type = ex->val.type;
    switch (ex->kind) {

        case EK_PLUS:
            ex2 = ex->args[0];
            for (i = 1; i < ex->nargs; i++)
                ex2 = makeexpr_plus(ex2, ex->args[i]);
            FREE(ex);
            return ex2;

        case EK_TIMES:
            ex2 = ex->args[0];
            for (i = 1; i < ex->nargs; i++)
                ex2 = makeexpr_times(ex2, ex->args[i]);
            FREE(ex);
            return ex2;

        case EK_NEG:
            ex = makeexpr_neg(grabarg(ex, 0));
            ex->val.type = type;
            return ex;

        case EK_NOT:
            ex = makeexpr_not(grabarg(ex, 0));
            ex->val.type = type;
            return ex;

        case EK_HAT:
            ex = makeexpr_hat(grabarg(ex, 0), 0);
	    if (ex->kind == EK_HAT)
		ex->val.type = type;
            return ex;

        case EK_ADDR:
            ex = makeexpr_addr(grabarg(ex, 0));
            ex->val.type = type;
            return ex;

	case EK_ASSIGN:
	    ex2 = makeexpr_assign(ex->args[0], ex->args[1]);
	    FREE(ex);
	    return ex2;

	default:
	    break;
    }
    return ex;
}






int realzero(s)
register char *s;
{
    if (*s == '-') s++;
    while (*s == '0' || *s == '.') s++;
    return (!isdigit(*s));
}

int realint(s, i)
register char *s;
int i;
{
    if (i == 0)
	return realzero(s);
    if (*s == '-') {
	s++;
	i = -i;
    }
    if (i < 0 || i > 9) return 0;   /* we don't care about large values here */
    while (*s == '0') s++;
    if (*s++ != i + '0') return 0;
    if (*s == '.')
	while (*++s == '0') ;
    return (!isdigit(*s));
}


int checkconst(ex, val)
Expr *ex;
long val;
{
    Meaning *mp;
    Value exval;

    if (!ex)
        return 0;
    if (ex->kind == EK_CAST || ex->kind == EK_ACTCAST)
        ex = ex->args[0];
    if (ex->kind == EK_CONST || ex->kind == EK_LONGCONST)
        exval = ex->val;
    else if (ex->kind == EK_VAR &&
             (mp = (Meaning *)ex->val.i)->kind == MK_CONST &&
	     mp->val.type &&
             foldconsts != 0)
        exval = mp->val;
    else
        return 0;
    switch (exval.type->kind) {

        case TK_BOOLEAN:
        case TK_INTEGER:
        case TK_CHAR:
        case TK_ENUM:
        case TK_SUBR:
        case TK_SMALLSET:
        case TK_SMALLARRAY:
            return exval.i == val;

        case TK_POINTER:
        case TK_STRING:
            return (val == 0 && exval.i == 0);

        case TK_REAL:
            return realint(exval.s, val);

	default:
	    return 0;
    }
}



int isliteralconst(ex, valp)
Expr *ex;
Value *valp;
{
    Meaning *mp;

    if (ex) {
        switch (ex->kind) {

            case EK_CONST:
            case EK_LONGCONST:
                if (valp)
                    *valp = ex->val;
                return 2;

            case EK_VAR:
                mp = (Meaning *)ex->val.i;
                if (mp->kind == MK_CONST) {
                    if (valp) {
                        if (foldconsts == 0)
                            valp->type = NULL;
                        else
                            *valp = mp->val;
                    }
                    return 1;
                }
                break;

	    default:
		break;
        }
    }
    if (valp)
        valp->type = NULL;
    return 0;
}



int isconstexpr(ex, valp)
Expr *ex;
long *valp;
{
    Value exval;

    if (debug>2) { fprintf(outf,"isconstexpr("); dumpexpr(ex); fprintf(outf,")\n"); }
    exval = eval_expr(ex);
    if (exval.type) {
        if (valp)
            *valp = exval.i;
        return 1;
    } else
        return 0;
}



int isconstantexpr(ex)
Expr *ex;
{
    Meaning *mp;
    int i;

    switch (ex->kind) {

        case EK_CONST:
        case EK_LONGCONST:
        case EK_SIZEOF:
            return 1;

        case EK_ADDR:
            if (ex->args[0]->kind == EK_VAR) {
                mp = (Meaning *)ex->val.i;
                return (!mp->ctx || mp->ctx->kind == MK_MODULE);
            }
            return 0;

        case EK_VAR:
            mp = (Meaning *)ex->val.i;
            return (mp->kind == MK_CONST);

        case EK_BICALL:
        case EK_FUNCTION:
            if (!deterministic_func(ex))
                return 0;

        /* fall through */
        case EK_EQ:
        case EK_NE:
        case EK_LT:
        case EK_GT:
        case EK_LE:
        case EK_GE:
        case EK_PLUS:
        case EK_NEG:
        case EK_TIMES:
        case EK_DIVIDE:
        case EK_DIV:
        case EK_MOD:
        case EK_AND:
        case EK_OR:
        case EK_NOT:
        case EK_BAND:
        case EK_BOR:
        case EK_BXOR:
        case EK_BNOT:
        case EK_LSH:
        case EK_RSH:
        case EK_CAST:
        case EK_ACTCAST:
        case EK_COND:
            for (i = 0; i < ex->nargs; i++) {
                if (!isconstantexpr(ex->args[i]))
                    return 0;
            }
            return 1;

        case EK_COMMA:
            return isconstantexpr(ex->args[ex->nargs-1]);

	default:
	    return 0;
    }
}





Static Expr *docast(a, type)
Expr *a;
Type *type;
{
    Value val;
    Meaning *mp;
    int i;
    Expr *ex;

    if (a->val.type->kind == TK_SMALLSET && type->kind == TK_SET) {
        mp = makestmttempvar(type, name_SET);
        return makeexpr_bicall_2(setexpandname, type,
                                 makeexpr_var(mp),
                                 makeexpr_arglong(a, 1));
    } else if (a->val.type->kind == TK_SET && type->kind == TK_SMALLSET) {
        return packset(a, type);
    }
    switch (a->kind) {

        case EK_VAR:
            mp = (Meaning *) a->val.i;
            if (mp->kind == MK_CONST) {
                if (mp->val.type && mp->val.type->kind == TK_STRING &&
		    type->kind == TK_CHAR) {
                    val = value_cast(mp->val, type);
                    a->kind = EK_CONST;
                    a->val = val;
                    return a;
                }
            }
            break;

        case EK_CONST:
        case EK_LONGCONST:
            val = value_cast(a->val, type);
            if (val.type) {
                a->val = val;
                return a;
            }
            break;

        case EK_PLUS:
        case EK_NEG:
        case EK_TIMES:
            if (type->kind == TK_REAL) {
                for (i = 0; i < a->nargs; i++) {
                    ex = docast(a->args[i], type);
                    if (ex) {
                        a->args[i] = ex;
                        a->val.type = type;
                        return a;
                    }
                }
            }
            break;

	default:
	    break;
    }
    return NULL;
}



/* Make an "active" cast, i.e., one that performs an explicit operation */
Expr *makeexpr_actcast(a, type)
Expr *a;
Type *type;
{
    if (debug>2) { fprintf(outf,"makeexpr_actcast("); dumpexpr(a); fprintf(outf,", "); dumptypename(type, 1); fprintf(outf,")\n"); }

    if (similartypes(a->val.type, type)) {
        a->val.type = type;
        return a;
    }
    return makeexpr_un(EK_ACTCAST, type, a);
}



Expr *makeexpr_cast(a, type)
Expr *a;
Type *type;
{
    Expr *ex;

    if (debug>2) { fprintf(outf,"makeexpr_cast("); dumpexpr(a); fprintf(outf,", "); dumptypename(type, 1); fprintf(outf,")\n"); }
    if (a->val.type == type)
        return a;
    ex = docast(a, type);
    if (ex)
        return ex;
    if (a->kind == EK_CAST &&
        a->args[0]->val.type->kind == TK_POINTER &&
        similartypes(type, a->args[0]->val.type)) {
        a = grabarg(a, 0);
        a->val.type = type;
        return a;
    }
    if ((a->kind == EK_CAST &&
         ((a->val.type->kind == TK_POINTER && type->kind == TK_POINTER) ||
          (ord_type(a->val.type)->kind == TK_INTEGER && ord_type(type)->kind == TK_INTEGER))) ||
        similartypes(type, a->val.type)) {
        a->val.type = type;
        return a;
    }
    return makeexpr_un(EK_CAST, type, a);
}



Expr *gentle_cast(a, type)
Expr *a;
Type *type;
{
    Expr *ex;
    Type *btype;
    long smin, smax;
    Value val;
    char c;

    if (debug>2) { fprintf(outf,"gentle_cast("); dumpexpr(a); fprintf(outf,", "); dumptypename(type, 1); fprintf(outf,")\n"); }
    if (!type) {
	intwarning("gentle_cast", "type == NULL");
	return a;
    }
    if (a->val.type->kind == TK_POINTER && type->kind == TK_POINTER) {
        if (voidstar && (type == tp_anyptr || a->val.type == tp_anyptr)) {
            if (type == tp_anyptr && a->kind == EK_CAST &&
                a->args[0]->val.type->kind == TK_POINTER)
                return a->args[0];    /* remove explicit cast since casting implicitly */
            return a;                 /* casting to/from "void *" */
        }
        return makeexpr_cast(a, type);
    }
    if (type->kind == TK_STRING)
        return makeexpr_stringify(a);
    if (type->kind == TK_ARRAY &&
	(a->val.type->kind == TK_STRING ||
	 a->val.type->kind == TK_CHAR) &&
        isliteralconst(a, &val) && val.type &&
	ord_range(type->indextype, &smin, &smax)) {
	smax = smax - smin + 1;
	if (a->val.type->kind == TK_CHAR) {
	    val.s = &c;
	    c = val.i;
	    val.i = 1;
	}
	if (val.i > smax) {
	    warning("Too many characters for packed array of char [162]");
	} else if (val.i < smax || a->val.type->kind == TK_CHAR) {
	    ex = makeexpr_lstring(val.s, smax);
	    while (smax > val.i)
		ex->val.s[--smax] = ' ';
	    freeexpr(a);
	    return ex;
	}
    }
    btype = (type->kind == TK_SUBR) ? type->basetype : type;
    if ((a->kind == EK_CAST || a->kind == EK_ACTCAST) &&
        btype->kind == TK_INTEGER &&
        ord_type(a->val.type)->kind == TK_INTEGER)
        return makeexpr_longcast(a, long_type(type));
    if (a->val.type == btype)
        return a;
    ex = docast(a, btype);
    if (ex)
        return ex;
    if (btype->kind == TK_CHAR && a->val.type->kind == TK_STRING)
        return makeexpr_hat(a, 0);
    return a;
}



Expr *makeexpr_charcast(ex)
Expr *ex;
{
    Meaning *mp;

    if (ex->kind == EK_CONST && ex->val.type->kind == TK_STRING &&
        ex->val.i == 1) {
        ex->val.type = tp_char;
        ex->val.i = ex->val.s[0] & 0xff;
        ex->val.s = NULL;
    }
    if (ex->kind == EK_VAR &&
	(mp = (Meaning *)ex->val.i)->kind == MK_CONST &&
	mp->val.type &&
	mp->val.type->kind == TK_STRING &&
	mp->val.i == 1) {
      ex->kind = EK_CONST;
      ex->val.type = tp_char;
      ex->val.i = mp->val.s[0] & 0xff;
      ex->val.s = NULL;
    }
    return ex;
}



Expr *makeexpr_stringcast(ex)
Expr *ex;
{
    char ch;

    if (ex->kind == EK_CONST && ord_type(ex->val.type)->kind == TK_CHAR) {
        ch = ex->val.i;
        freeexpr(ex);
        ex = makeexpr_lstring(&ch, 1);
    }
    return ex;
}





/* 0/1 = force to int/long, 2/3 = check if int/long */

Static Expr *dolongcast(a, tolong)
Expr *a;
int tolong;
{
    Meaning *mp;
    Expr *ex;
    Type *type;
    int i;

    switch (a->kind) {

        case EK_DOT:
            if (!a->val.i) {
                if (long_type(a->val.type) == (tolong&1))
                    return a;
                break;
            }

        /* fall through */
        case EK_VAR:
            mp = (Meaning *)a->val.i;
            if (mp->kind == MK_FIELD && mp->val.i) {
                if (mp->val.i <= ((sizeof_int > 0) ? sizeof_int : 16) &&
                    !(tolong&1))
                    return a;
            } else if (mp->kind == MK_VAR ||
                       mp->kind == MK_VARREF ||
                       mp->kind == MK_PARAM ||
                       mp->kind == MK_VARPARAM ||
                       mp->kind == MK_FIELD) {
                if (long_type(mp->type) == (tolong&1))
                    return a;
            }
            break;

        case EK_FUNCTION:
            mp = (Meaning *)a->val.i;
            if (long_type(mp->type->basetype) == (tolong&1))
                return a;
            break;

        case EK_BICALL:
            if (!strcmp(a->val.s, signextname) && *signextname) {
                i = 0;
                goto unary;
            }
	    if (!strcmp(a->val.s, "strlen"))
		goto size_t_case;
            /* fall through */

        case EK_HAT:      /* get true type from a->val.type */
        case EK_INDEX:
        case EK_SPCALL:
        case EK_NAME:
            if (long_type(a->val.type) == (tolong&1))
                return a;
            break;

        case EK_ASSIGN:   /* destination determines type, */
        case EK_POSTINC:  /*  but must not be changed */
        case EK_POSTDEC:
            return dolongcast(a->args[0], tolong|2);

        case EK_CAST:
            if (ord_type(a->val.type)->kind == TK_INTEGER &&
                 long_type(a->val.type) == (tolong&1))
                return a;
            if (tolong == 0) {
                a->val.type = tp_int;
                return a;
            } else if (tolong == 1) {
                a->val.type = tp_integer;
                return a;
            }
            break;

        case EK_ACTCAST:
            if (ord_type(a->val.type)->kind == TK_INTEGER &&
                 long_type(a->val.type) == (tolong&1))
                return a;
            break;

        case EK_CONST:
            type = ord_type(a->val.type);
            if (type->kind == TK_INTEGER || type->kind == TK_SMALLSET) {
                if (tolong == 1)
                    a->kind = EK_LONGCONST;
                if (tolong != 3)
                    return a;
            }
            break;

        case EK_LONGCONST:
            if (tolong == 0) {
                if (a->val.i >= -32767 && a->val.i <= 32767)
                    a->kind = EK_CONST;
                else
                    return NULL;
            }
            if (tolong != 2)
                return a;
            break;

        case EK_SIZEOF:
	size_t_case:
            if (size_t_long > 0 && tolong&1)
                return a;
            if (size_t_long == 0 && !(tolong&1))
                return a;
            break;

        case EK_PLUS:     /* usual arithmetic conversions apply */
        case EK_TIMES:
        case EK_DIV:
        case EK_MOD:
        case EK_BAND:
        case EK_BOR:
        case EK_BXOR:
        case EK_COND:
            i = (a->kind == EK_COND) ? 1 : 0;
            if (tolong&1) {
                for (; i < a->nargs; i++) {
                    ex = dolongcast(a->args[i], tolong);
                    if (ex) {
                        a->args[i] = ex;
                        return a;
                    }
                }
            } else {
                for (; i < a->nargs; i++) {
                    if (!dolongcast(a->args[i], tolong))
                        return NULL;
                }
                return a;
            }
            break;

        case EK_BNOT:     /* single argument defines result type */
        case EK_NEG:
        case EK_LSH:
        case EK_RSH:
        case EK_COMMA:
            i = (a->kind == EK_COMMA) ? a->nargs-1 : 0;
unary:
            if (tolong&1) {
                ex = dolongcast(a->args[i], tolong);
                if (ex) {
                    a->args[i] = ex;
                    return a;
                }
            } else {
                if (dolongcast(a->args[i], tolong))
                    return a;
            }
            break;

        case EK_AND:  /* operators which always return int */
        case EK_OR:
        case EK_EQ:
        case EK_NE:
        case EK_LT:
        case EK_GT:
        case EK_LE:
        case EK_GE:
            if (tolong&1)
                break;
            return a;

	default:
	    break;
    }
    return NULL;
}


/* Return -1 if short int or plain int, 1 if long, 0 if can't tell */
int exprlongness(ex)
Expr *ex;
{
    if (sizeof_int >= 32)
        return -1;
    return (dolongcast(ex, 3) != NULL) -
           (dolongcast(ex, 2) != NULL);
}


Expr *makeexpr_longcast(a, tolong)
Expr *a;
int tolong;
{
    Expr *ex;
    Type *type;

    if (sizeof_int >= 32)
        return a;
    type = ord_type(a->val.type);
    if (type->kind != TK_INTEGER && type->kind != TK_SMALLSET)
        return a;
    a = makeexpr_unlongcast(a);
    if (tolong) {
        ex = dolongcast(a, 1);
    } else {
        ex = dolongcast(copyexpr(a), 0);
        if (ex) {
            if (!dolongcast(ex, 2)) {
                freeexpr(ex);
                ex = NULL;
            }
        }
    }
    if (ex)
        return ex;
    return makeexpr_un(EK_CAST, (tolong) ? tp_integer : tp_int, a);
}


Expr *makeexpr_arglong(a, tolong)
Expr *a;
int tolong;
{
    int cast = castlongargs;
    if (cast < 0)
	cast = castargs;
    if (cast > 0 || (cast < 0 && prototypes == 0)) {
	return makeexpr_longcast(a, tolong);
    }
    return a;
}



Expr *makeexpr_unlongcast(a)
Expr *a;
{
    switch (a->kind) {

        case EK_LONGCONST:
            if (a->val.i >= -32767 && a->val.i <= 32767)
                a->kind = EK_CONST;
            break;

        case EK_CAST:
            if ((a->val.type == tp_integer ||
                 a->val.type == tp_int) &&
                ord_type(a->args[0]->val.type)->kind == TK_INTEGER) {
                a = grabarg(a, 0);
            }
            break;

        default:
	    break;

    }
    return a;
}



Expr *makeexpr_forcelongness(a)    /* force a to have a definite longness */
Expr *a;
{
    Expr *ex;

    ex = makeexpr_unlongcast(copyexpr(a));
    if (exprlongness(ex)) {
        freeexpr(a);
        return ex;
    }
    freeexpr(ex);
    if (exprlongness(a) == 0)
        return makeexpr_longcast(a, 1);
    else
        return a;
}



Expr *makeexpr_ord(ex)
Expr *ex;
{
    ex = makeexpr_charcast(ex);
    switch (ord_type(ex->val.type)->kind) {

        case TK_ENUM:
            return makeexpr_cast(ex, tp_int);

        case TK_CHAR:
            if (ex->kind == EK_CONST &&
                (ex->val.i >= 32 && ex->val.i < 127)) {
                insertarg(&ex, 0, makeexpr_name("'%lc'", tp_integer));
            }
            ex->val.type = tp_int;
            return ex;

        case TK_BOOLEAN:
            ex->val.type = tp_int;
            return ex;

        case TK_POINTER:
            return makeexpr_cast(ex, tp_integer);

        default:
            return ex;
    }
}




/* Tell whether an expression "looks" negative */
int expr_looks_neg(ex)
Expr *ex;
{
    int i;

    switch (ex->kind) {

        case EK_NEG:
            return 1;

        case EK_CONST:
        case EK_LONGCONST:
            switch (ord_type(ex->val.type)->kind) {
                case TK_INTEGER:
                case TK_CHAR:
                    return (ex->val.i < 0);
                case TK_REAL:
                    return (ex->val.s && ex->val.s[0] == '-');
                default:
                    return 0;
            }

        case EK_TIMES:
        case EK_DIVIDE:
            for (i = 0; i < ex->nargs; i++) {
                if (expr_looks_neg(ex->args[i]))
                    return 1;
            }
            return 0;

        case EK_CAST:
            return expr_looks_neg(ex->args[0]);

        default:
            return 0;
    }
}



/* Tell whether an expression is probably negative */
int expr_is_neg(ex)
Expr *ex;
{
    int i;

    i = possiblesigns(ex) & (1|4);
    if (i == 1)
	return 1;    /* if expression really is negative! */
    if (i == 4)
	return 0;    /* if expression is definitely positive. */
    return expr_looks_neg(ex);
}



int expr_neg_cost(a)
Expr *a;
{
    int i, c;

    switch (a->kind) {

        case EK_CONST:
        case EK_LONGCONST:
            switch (ord_type(a->val.type)->kind) {
                case TK_INTEGER:
                case TK_CHAR:
                case TK_REAL:
                    return 0;
		default:
		    return 1;
            }

        case EK_NEG:
            return -1;

        case EK_TIMES:
        case EK_DIVIDE:
            for (i = 0; i < a->nargs; i++) {
                c = expr_neg_cost(a->args[i]);
                if (c <= 0)
                    return c;
            }
            return 1;

        case EK_PLUS:
            for (i = 0; i < a->nargs; i++) {
                if (expr_looks_neg(a->args[i]))
                    return 0;
            }
            return 1;

        default:
            return 1;
    }
}



Expr *enum_to_int(a)
Expr *a;
{
    if (ord_type(a->val.type)->kind == TK_ENUM) {
        if (a->kind == EK_CAST &&
             ord_type(a->args[0]->val.type)->kind == TK_INTEGER)
            return grabarg(a, 0);
        else
            return makeexpr_cast(a, tp_integer);
    } else
        return a;
}



Expr *neg_inside_sum(a)
Expr *a;
{
    int i;

    for (i = 0; i < a->nargs; i++)
        a->args[i] = makeexpr_neg(a->args[i]);
    return a;
}



Expr *makeexpr_neg(a)
Expr *a;
{
    int i;

    if (debug>2) { fprintf(outf,"makeexpr_neg("); dumpexpr(a); fprintf(outf,")\n"); }
    a = enum_to_int(a);
    switch (a->kind) {

        case EK_CONST:
        case EK_LONGCONST:
            switch (ord_type(a->val.type)->kind) {

                case TK_INTEGER:
                case TK_CHAR:
                    if (a->val.i == MININT)
                        valrange();
                    else
                        a->val.i = - a->val.i;
                    return a;

                case TK_REAL:
                    if (!realzero(a->val.s)) {
                        if (a->val.s[0] == '-')
                            strchange(&a->val.s, a->val.s+1);
                        else
                            strchange(&a->val.s, format_s("-%s", a->val.s));
                    }
                    return a;

		default:
		    break;
            }
            break;

        case EK_PLUS:
            if (expr_neg_cost(a) <= 0)
                return neg_inside_sum(a);
            break;

        case EK_TIMES:
        case EK_DIVIDE:
            for (i = 0; i < a->nargs; i++) {
                if (expr_neg_cost(a->args[i]) <= 0) {
                    a->args[i] = makeexpr_neg(a->args[i]);
                    return a;
                }
            }
            break;

        case EK_CAST:
            if (a->val.type != tp_unsigned &&
                 a->val.type != tp_uint &&
                 a->val.type != tp_ushort &&
                 a->val.type != tp_ubyte &&
                 a->args[0]->val.type != tp_unsigned &&
                 a->args[0]->val.type != tp_uint &&
                 a->args[0]->val.type != tp_ushort &&
                 a->args[0]->val.type != tp_ubyte &&
                 expr_looks_neg(a->args[0])) {
                a->args[0] = makeexpr_neg(a->args[0]);
                return a;
            }
            break;

        case EK_NEG:
            return grabarg(a, 0);

	default:
	    break;
    }
    return makeexpr_un(EK_NEG, promote_type(a->val.type), a);
}




#define ISCONST(kind) ((kind)==EK_CONST || (kind)==EK_LONGCONST)
#define MOVCONST(ex) (ISCONST((ex)->kind) && (ex)->val.type->kind != TK_STRING)
#define COMMUTATIVE (kind != EK_COMMA && type->kind != TK_REAL)

Type *true_type(ex)
Expr *ex;
{
    Meaning *mp;
    Type *type, *tp;

    while (ex->kind == EK_CAST)
	ex = ex->args[0];
    type = ex->val.type;
    if (ex->kind == EK_VAR || ex->kind == EK_FUNCTION || ex->kind == EK_DOT) {
	mp = (Meaning *)ex->val.i;
	if (mp && mp->type && mp->type->kind != TK_VOID)
	    type = mp->type;
    }
    if (ex->kind == EK_INDEX) {
	tp = true_type(ex->args[0]);
	if ((tp->kind == TK_ARRAY || tp->kind == TK_SMALLARRAY ||
	     tp->kind == TK_STRING) &&
	    tp->basetype && tp->basetype->kind != TK_VOID)
	    type = tp->basetype;
    }
    if (type->kind == TK_SUBR)
	type = findbasetype(type, ODECL_NOPRES);
    return type;
}

int ischartype(ex)
Expr *ex;
{
    if (ord_type(ex->val.type)->kind == TK_CHAR)
	return 1;
    if (true_type(ex)->kind == TK_CHAR)
	return 1;
    if (ISCONST(ex->kind) && ex->nargs > 0 &&
	ex->args[0]->kind == EK_NAME &&
	ex->args[0]->val.s[0] == '\'')
	return 1;
    return 0;
}

Static Expr *commute(a, b, kind)
Expr *a, *b;
enum exprkind kind;
{
    int i, di;
    Type *type;

    if (debug>2) { fprintf(outf,"commute("); dumpexpr(a); fprintf(outf,", "); dumpexpr(b); fprintf(outf,")\n"); }
#if 1
    type = promote_type_bin(a->val.type, b->val.type);
#else
    type = a->val.type;
    if (b->val.type->kind == TK_REAL)
        type = b->val.type;
#endif
    if (MOVCONST(a) && !MOVCONST(b) && COMMUTATIVE)
        swapexprs(a, b);                /* put constant last */
    if (a->kind == kind) {
        di = (MOVCONST(a->args[a->nargs-1]) && COMMUTATIVE) ? -1 : 0;
        if (b->kind == kind) {
            for (i = 0; i < b->nargs; i++)
                insertarg(&a, a->nargs + di, b->args[i]);
            FREE(b);
        } else
            insertarg(&a, a->nargs + di, b);
        a->val.type = type;
    } else if (b->kind == kind) {
        if (MOVCONST(a) && COMMUTATIVE)
            insertarg(&b, b->nargs, a);
        else
            insertarg(&b, 0, a);
        a = b;
        a->val.type = type;
    } else {
        a = makeexpr_bin(kind, type, a, b);
    }
    if (debug>2) { fprintf(outf,"commute returns "); dumpexpr(a); fprintf(outf,"\n"); }
    return a;
}


Expr *makeexpr_plus(a, b)
Expr *a, *b;
{
    int i, j, k, castdouble = 0;
    Type *type;

    if (debug>2) { fprintf(outf,"makeexpr_plus("); dumpexpr(a); fprintf(outf,", "); dumpexpr(b); fprintf(outf,")\n"); }
    if (!a)
        return b;
    if (!b)
        return a;
    if (a->kind == EK_NEG && a->args[0]->kind == EK_PLUS)
        a = neg_inside_sum(grabarg(a, 0));
    if (b->kind == EK_NEG && b->args[0]->kind == EK_PLUS)
        b = neg_inside_sum(grabarg(b, 0));
    a = commute(enum_to_int(a), enum_to_int(b), EK_PLUS);
    type = NULL;
    for (i = 0; i < a->nargs; i++) {
        if (ord_type(a->args[i]->val.type)->kind == TK_CHAR ||
            a->args[i]->val.type->kind == TK_POINTER ||
            a->args[i]->val.type->kind == TK_STRING) {   /* for string literals */
            if (type == ord_type(a->args[i]->val.type))
                type = tp_integer;   /* 'z'-'a' and p1-p2 are integers */
            else
                type = ord_type(a->args[i]->val.type);
        }
    }
    if (type)
        a->val.type = type;
    for (i = 0; i < a->nargs && !ISCONST(a->args[i]->kind); i++) ;
    if (i < a->nargs-1) {
        for (j = i+1; j < a->nargs; j++) {
            if (ISCONST(a->args[j]->kind)) {
                if ((ord_type(a->args[i]->val.type) == ord_type(a->args[j]->val.type) ||
		     ord_type(a->args[i]->val.type)->kind == TK_INTEGER ||
		     ord_type(a->args[j]->val.type)->kind == TK_INTEGER) &&
		    (!(ischartype(a->args[i]) || ischartype(a->args[j])) ||
		     a->args[i]->val.i == - a->args[j]->val.i ||
		     a->args[i]->val.i == 0 || a->args[j]->val.i == 0) &&
                    (a->args[i]->val.type->kind != TK_REAL &&
                     a->args[i]->val.type->kind != TK_STRING &&
                     a->args[j]->val.type->kind != TK_REAL &&
                     a->args[j]->val.type->kind != TK_STRING)) {
                    a->args[i]->val.i += a->args[j]->val.i;
                    delfreearg(&a, j);
                    j--;
                } else if (a->args[i]->val.type->kind == TK_STRING &&
                           ord_type(a->args[j]->val.type)->kind == TK_INTEGER &&
                           a->args[j]->val.i < 0 &&
                           a->args[j]->val.i >= -stringleaders) {
                    /* strictly speaking, the following is illegal pointer arithmetic */
                    a->args[i] = makeexpr_lstring(a->args[i]->val.s + a->args[j]->val.i,
                                                  a->args[i]->val.i - a->args[j]->val.i);
                    for (k = 0; k < - a->args[j]->val.i; k++)
                        a->args[i]->val.s[k] = '>';
                    delfreearg(&a, j);
                    j--;
                }
            }
        }
    }
    if (checkconst(a->args[a->nargs-1], 0)) {
	if (a->args[a->nargs-1]->val.type->kind == TK_REAL &&
	    a->args[0]->val.type->kind != TK_REAL)
	    castdouble = 1;
        delfreearg(&a, a->nargs-1);
    }
    for (i = 0; i < a->nargs; i++) {
        if (a->args[i]->kind == EK_NEG && nosideeffects(a->args[i], 1)) {
            for (j = 0; j < a->nargs; j++) {
                if (exprsame(a->args[j], a->args[i]->args[0], 1)) {
                    delfreearg(&a, i);
                    if (i < j) j--; else i--;
                    delfreearg(&a, j);
                    i--;
                    break;
                }
            }
        }
    }
    if (a->nargs == 0) {
	type = a->val.type;
	FREE(a);
	a = gentle_cast(makeexpr_long(0), type);
	a->val.type = type;
	return a;
    } else if (a->nargs == 1) {
	b = a->args[0];
	FREE(a);
	a = b;
    } else {
	if (a->nargs == 2 && ISCONST(a->args[1]->kind) &&
	    a->args[1]->val.i <= -127 &&
	    true_type(a->args[0]) == tp_char && signedchars != 0) {
	    a->args[0] = force_unsigned(a->args[0]);
	}
	if (a->nargs > 2 &&
	    ISCONST(a->args[a->nargs-1]->kind) &&
	    ISCONST(a->args[a->nargs-2]->kind) &&
	    ischartype(a->args[a->nargs-1]) &&
	    ischartype(a->args[a->nargs-2])) {
	    i = a->args[a->nargs-1]->val.i;
	    j = a->args[a->nargs-2]->val.i;
	    if ((i == 'a' || i == 'A' || i == -'a' || i == -'A') &&
		(j == 'a' || j == 'A' || j == -'a' || j == -'A')) {
		if (abs(i+j) == 32) {
		    delfreearg(&a, a->nargs-1);
		    delsimpfreearg(&a, a->nargs-1);
		    a = makeexpr_bicall_1((i+j > 0) ? "_tolower" : "_toupper",
					  tp_char, a);
		}
	    }
	}
    }
    if (castdouble)
	a = makeexpr_cast(a, tp_real);
    return a;
}


Expr *makeexpr_minus(a, b)
Expr *a, *b;
{
    int okneg;

    if (debug>2) { fprintf(outf,"makeexpr_minus("); dumpexpr(a); fprintf(outf,", "); dumpexpr(b); fprintf(outf,")\n"); }
    if (ISCONST(b->kind) && b->val.i == 0 &&       /* kludge for array indexing */
        ord_type(b->val.type)->kind == TK_ENUM) {
        b->val.type = tp_integer;
    }
    okneg = (a->kind != EK_PLUS && b->kind != EK_PLUS);
    a = makeexpr_plus(a, makeexpr_neg(b));
    if (okneg && a->kind == EK_PLUS)
        a->val.i = 1;   /* this flag says to write as "a-b" if possible */
    return a;
}


Expr *makeexpr_inc(a, b)
Expr *a, *b;
{
    Type *type;

    type = a->val.type;
    a = makeexpr_plus(makeexpr_charcast(a), b);
    if (ord_type(type)->kind != TK_INTEGER &&
	ord_type(type)->kind != TK_CHAR)
	a = makeexpr_cast(a, type);
    return a;
}



/* Apply the distributive law for a sum of products */
Expr *distribute_plus(ex)
Expr *ex;
{
    int i, j, icom;
    Expr *common, *outer, *ex2, **exp;

    if (debug>2) { fprintf(outf,"distribute_plus("); dumpexpr(ex); fprintf(outf,")\n"); }
    if (ex->kind != EK_PLUS)
        return ex;
    for (i = 0; i < ex->nargs; i++)
        if (ex->args[i]->kind == EK_TIMES)
            break;
    if (i == ex->nargs)
        return ex;
    outer = NULL;
    icom = 0;
    for (;;) {
	ex2 = ex->args[0];
	if (ex2->kind == EK_NEG)
	    ex2 = ex2->args[0];
        if (ex2->kind == EK_TIMES) {
	    if (icom >= ex2->nargs)
		break;
            common = ex2->args[icom];
	    if (common->kind == EK_NEG)
		common = common->args[0];
        } else {
	    if (icom > 0)
		break;
            common = ex2;
	    icom++;
	}
        for (i = 1; i < ex->nargs; i++) {
	    ex2 = ex->args[i];
	    if (ex2->kind == EK_NEG)
		ex2 = ex2->args[i];
            if (ex2->kind == EK_TIMES) {
                for (j = ex2->nargs; --j >= 0; ) {
                    if (exprsame(ex2->args[j], common, 1) ||
			(ex2->args[j]->kind == EK_NEG &&
			 exprsame(ex2->args[j]->args[0], common, 1)))
                        break;
                }
                if (j < 0)
                    break;
            } else {
                if (!exprsame(ex2, common, 1))
                    break;
            }
        }
        if (i == ex->nargs) {
            if (debug>2) { fprintf(outf,"distribute_plus does "); dumpexpr(common); fprintf(outf,"\n"); }
	    common = copyexpr(common);
            for (i = 0; i < ex->nargs; i++) {
		if (ex->args[i]->kind == EK_NEG)
		    ex2 = *(exp = &ex->args[i]->args[0]);
		else
		    ex2 = *(exp = &ex->args[i]);
		if (ex2->kind == EK_TIMES) {
                    for (j = ex2->nargs; --j >= 0; ) {
                        if (exprsame(ex2->args[j], common, 1)) {
                            delsimpfreearg(exp, j);
                            break;
                        } else if (ex2->args[j]->kind == EK_NEG &&
				   exprsame(ex2->args[j]->args[0], common,1)) {
			    freeexpr(ex2->args[j]);
			    ex2->args[j] = makeexpr_long(-1);
			    break;
			}
                    }
		} else {
		    freeexpr(ex2);
		    *exp = makeexpr_long(1);
                }
		ex->args[i] = resimplify(ex->args[i]);
            }
            outer = makeexpr_times(common, outer);
        } else
	    icom++;
    }
    return makeexpr_times(resimplify(ex), outer);
}





Expr *makeexpr_times(a, b)
Expr *a, *b;
{
    int i, n, castdouble = 0;
    Type *type;

    if (debug>2) { fprintf(outf,"makeexpr_times("); dumpexpr(a); fprintf(outf,", "); dumpexpr(b); fprintf(outf,")\n"); }
    if (!a)
        return b;
    if (!b)
        return a;
    a = commute(a, b, EK_TIMES);
    if (a->val.type->kind == TK_INTEGER) {
        i = a->nargs-1;
        if (i > 0 && ISCONST(a->args[i-1]->kind)) {
            a->args[i-1]->val.i *= a->args[i]->val.i;
            delfreearg(&a, i);
        }
    }
    for (i = n = 0; i < a->nargs; i++) {
        if (expr_neg_cost(a->args[i]) < 0)
            n++;
    }
    if (n & 1) {
        for (i = 0; i < a->nargs; i++) {
            if (ISCONST(a->args[i]->kind) &&
                expr_neg_cost(a->args[i]) >= 0) {
                a->args[i] = makeexpr_neg(a->args[i]);
                n++;
                break;
            }
        }
    } else
        n++;
    for (i = 0; i < a->nargs && n >= 2; i++) {
        if (expr_neg_cost(a->args[i]) < 0) {
            a->args[i] = makeexpr_neg(a->args[i]);
            n--;
        }
    }
    if (checkconst(a->args[a->nargs-1], 1)) {
	if (a->args[a->nargs-1]->val.type->kind == TK_REAL &&
	    a->args[0]->val.type->kind != TK_REAL)
	    castdouble = 1;
        delfreearg(&a, a->nargs-1);
    } else if (checkconst(a->args[a->nargs-1], -1)) {
	if (a->args[a->nargs-1]->val.type->kind == TK_REAL &&
	    a->args[0]->val.type->kind != TK_REAL)
	    castdouble = 1;
        delfreearg(&a, a->nargs-1);
	a->args[0] = makeexpr_neg(a->args[0]);
    } else if (checkconst(a->args[a->nargs-1], 0) && nosideeffects(a, 1)) {
	if (a->args[a->nargs-1]->val.type->kind == TK_REAL)
	    type = a->args[a->nargs-1]->val.type;
	else
	    type = a->val.type;
        return makeexpr_cast(grabarg(a, a->nargs-1), type);
    }
    if (a->nargs < 2) {
        if (a->nargs < 1) {
            FREE(a);
            a = makeexpr_long(1);
        } else {
            b = a->args[0];
            FREE(a);
            a = b;
        }
    }
    if (castdouble)
	a = makeexpr_cast(a, tp_real);
    return a;
}



Expr *makeexpr_sqr(ex, cube)
Expr *ex;
int cube;
{
    Expr *ex2;
    Meaning *tvar;
    Type *type;

    if (exprspeed(ex) <= 2 && nosideeffects(ex, 0)) {
	ex2 = NULL;
    } else {
	type = (ex->val.type->kind == TK_REAL) ? tp_longreal : tp_integer;
	tvar = makestmttempvar(type, name_TEMP);
	ex2 = makeexpr_assign(makeexpr_var(tvar), ex);
	ex = makeexpr_var(tvar);
    }
    if (cube)
	ex = makeexpr_times(ex, makeexpr_times(copyexpr(ex), copyexpr(ex)));
    else
	ex = makeexpr_times(ex, copyexpr(ex));
    return makeexpr_comma(ex2, ex);
}



Expr *makeexpr_divide(a, b)
Expr *a, *b;
{
    Expr *ex;
    int p;

    if (debug>2) { fprintf(outf,"makeexpr_divide("); dumpexpr(a); fprintf(outf,", "); dumpexpr(b); fprintf(outf,")\n"); }
    if (a->val.type->kind != TK_REAL &&
	b->val.type->kind != TK_REAL) {     /* must do a real division */
        ex = docast(a, tp_longreal);
        if (ex)
            a = ex;
        else {
            ex = docast(b, tp_longreal);
            if (ex)
                b = ex;
            else
                a = makeexpr_cast(a, tp_longreal);
        }
    }
    if (a->kind == EK_TIMES) {
	for (p = 0; p < a->nargs; p++)
	    if (exprsame(a->args[p], b, 1))
		break;
	if (p < a->nargs) {
	    delfreearg(&a, p);
	    freeexpr(b);
	    if (a->nargs == 1)
		return grabarg(a, 0);
	    else
		return a;
	}
    }
    if (expr_neg_cost(a) < 0 && expr_neg_cost(b) < 0) {
        a = makeexpr_neg(a);
        b = makeexpr_neg(b);
    }
    if (checkconst(b, 0))
        warning("Division by zero [163]");
    return makeexpr_bin(EK_DIVIDE, tp_longreal, a, b);
}




int gcd(a, b)
int a, b;
{
    if (a < 0) a = -a;
    if (b < 0) b = -b;
    while (a != 0) {
	b %= a;
	if (b != 0)
	    a %= b;
	else
	    return a;
    }
    return b;
}



/* possible signs of ex: 1=may be neg, 2=may be zero, 4=may be pos */

int negsigns(mask)
int mask;
{
    return (mask & 2) |
	   ((mask & 1) << 2) |
	   ((mask & 4) >> 2);
}


int possiblesigns(ex)
Expr *ex;
{
    Value val;
    Type *tp;
    char *cp;
    int i, mask, mask2;

    if (isliteralconst(ex, &val) && val.type) {
	if (val.type == tp_real || val.type == tp_longreal) {
	    if (realzero(val.s))
		return 2;
	    if (*val.s == '-')
		return 1;
	    return 4;
	} else
	    return (val.i < 0) ? 1 : (val.i == 0) ? 2 : 4;
    }
    if (ex->kind == EK_CAST &&
	similartypes(ex->val.type, ex->args[0]->val.type))
	return possiblesigns(ex->args[0]);
    if (ex->kind == EK_NEG)
	return negsigns(possiblesigns(ex->args[0]));
    if (ex->kind == EK_TIMES || ex->kind == EK_DIVIDE) {
	mask = possiblesigns(ex->args[0]);
	for (i = 1; i < ex->nargs; i++) {
	    mask2 = possiblesigns(ex->args[i]);
	    if (mask2 & 2)
		mask |= 2;
	    if ((mask2 & (1|4)) == 1)
		mask = negsigns(mask);
	    else if ((mask2 & (1|4)) != 4)
		mask = 1|2|4;
	}
	return mask;
    }
    if (ex->kind == EK_DIV || ex->kind == EK_MOD) {
	mask = possiblesigns(ex->args[0]);
	mask2 = possiblesigns(ex->args[1]);
	if (!((mask | mask2) & 1))
	    return 2|4;
    }
    if (ex->kind == EK_PLUS) {
	mask = 0;
	for (i = 0; i < ex->nargs; i++) {
	    mask2 = possiblesigns(ex->args[i]);
	    if ((mask & negsigns(mask2)) & (1|4))
		mask |= (1|2|4);
	    else
		mask |= mask2;
	}
	return mask;
    }
    if (ex->kind == EK_COND) {
	return possiblesigns(ex->args[1]) | possiblesigns(ex->args[2]);
    }
    if (ex->kind == EK_EQ || ex->kind == EK_LT || ex->kind == EK_GT ||
	ex->kind == EK_NE || ex->kind == EK_LE || ex->kind == EK_GE ||
	ex->kind == EK_AND || ex->kind == EK_OR || ex->kind == EK_NOT)
	return 2|4;
    if (ex->kind == EK_BICALL) {
	cp = ex->val.s;
	if (!strcmp(cp, "strlen") ||
	    !strcmp(cp, "abs") ||
	    !strcmp(cp, "labs") ||
	    !strcmp(cp, "fabs"))
	    return 2|4;
    }
    tp = (ex->kind == EK_VAR) ? ((Meaning *)ex->val.i)->type : ex->val.type;
    if (ord_range(ex->val.type, &val.i, NULL)) {
	if (val.i > 0)
	    return 4;
	if (val.i >= 0)
	    return 2|4;
    }
    if (ord_range(ex->val.type, NULL, &val.i)) {
	if (val.i < 0)
	    return 1;
	if (val.i <= 0)
	    return 1|2;
    }
    return 1|2|4;
}





Expr *dodivmod(funcname, ekind, a, b)
char *funcname;
enum exprkind ekind;
Expr *a, *b;
{
    Meaning *tvar;
    Type *type;
    Expr *asn;
    int sa, sb;

    type = promote_type_bin(a->val.type, b->val.type);
    tvar = NULL;
    sa = possiblesigns(a);
    sb = possiblesigns(b);
    if ((sa & 1) || (sb & 1)) {
	if (*funcname) {
	    asn = NULL;
	    if (*funcname == '*') {
		if (exprspeed(a) >= 5 || !nosideeffects(a, 0)) {
		    tvar = makestmttempvar(a->val.type, name_TEMP);
		    asn = makeexpr_assign(makeexpr_var(tvar), a);
		    a = makeexpr_var(tvar);
		}
		if (exprspeed(b) >= 5 || !nosideeffects(b, 0)) {
		    tvar = makestmttempvar(b->val.type, name_TEMP);
		    asn = makeexpr_comma(asn,
					 makeexpr_assign(makeexpr_var(tvar),
							 b));
		    b = makeexpr_var(tvar);
		}
	    }
	    return makeexpr_comma(asn,
				  makeexpr_bicall_2(funcname, type, a, b));
	} else {
	    if ((sa & 1) && (ekind == EK_MOD))
		note("Using % for possibly-negative arguments [317]");
	    return makeexpr_bin(ekind, type, a, b);
	}
    } else
	return makeexpr_bin(ekind, type, a, b);
}



Expr *makeexpr_div(a, b)
Expr *a, *b;
{
    Meaning *mp;
    Type *type;
    long i;
    int p;

    if (ISCONST(a->kind) && ISCONST(b->kind)) {
        if (a->val.i >= 0 && b->val.i > 0) {
	    a->val.i /= b->val.i;
	    freeexpr(b);
	    return a;
	}
	i = gcd(a->val.i, b->val.i);
	if (i >= 0) {
	    a->val.i /= i;
	    b->val.i /= i;
	}
    }
    if (((b->kind == EK_CONST && (i = b->val.i)) ||
         (b->kind == EK_VAR && (mp = (Meaning *)b->val.i)->kind == MK_CONST &&
	  mp->val.type && (i = mp->val.i) && foldconsts != 0)) && i > 0) {
        if (i == 1)
            return a;
        if (div_po2 > 0) {
            p = 0;
            while (!(i&1))
                p++, i >>= 1;
            if (i == 1) {
		type = promote_type_bin(a->val.type, b->val.type);
                return makeexpr_bin(EK_RSH, type, a, makeexpr_long(p));
            }
        }
    }
    if (a->kind == EK_TIMES) {
	for (p = 0; p < a->nargs; p++) {
	    if (exprsame(a->args[p], b, 1)) {
		delfreearg(&a, p);
		freeexpr(b);
		if (a->nargs == 1)
		    return grabarg(a, 0);
		else
		    return a;
	    } else if (ISCONST(a->args[p]->kind) && ISCONST(b->kind)) {
		i = gcd(a->args[p]->val.i, b->val.i);
		if (i > 1) {
		    a->args[p]->val.i /= i;
		    b->val.i /= i;
		    i = a->args[p]->val.i;
		    delfreearg(&a, p);
		    a = makeexpr_times(a, makeexpr_long(i));   /* resimplify */
		    p = -1;   /* start the loop over */
		}
	    }
	}
    }
    if (checkconst(b, 1)) {
        freeexpr(b);
        return a;
    } else if (checkconst(b, -1)) {
        freeexpr(b);
        return makeexpr_neg(a);
    } else {
        if (checkconst(b, 0))
            warning("Division by zero [163]");
        return dodivmod(divname, EK_DIV, a, b);
    }
}



Expr *makeexpr_mod(a, b)
Expr *a, *b;
{
    Meaning *mp;
    Type *type;
    long i;

    if (a->kind == EK_CONST && b->kind == EK_CONST &&
        a->val.i >= 0 && b->val.i > 0) {
        a->val.i %= b->val.i;
        freeexpr(b);
        return a;
    }
    if (((b->kind == EK_CONST && (i = b->val.i)) ||
         (b->kind == EK_VAR && (mp = (Meaning *)b->val.i)->kind == MK_CONST &&
	  mp->val.type && (i = mp->val.i) && foldconsts != 0)) && i > 0) {
        if (i == 1)
            return makeexpr_long(0);
        if (mod_po2 != 0) {
            while (!(i&1))
                i >>= 1;
            if (i == 1) {
		type = promote_type_bin(a->val.type, b->val.type);
                return makeexpr_bin(EK_BAND, type, a,
                                    makeexpr_minus(b, makeexpr_long(1)));
            }
        }
    }
    if (checkconst(b, 0))
        warning("Division by zero [163]");
    return dodivmod(modname, EK_MOD, a, b);
}



Expr *makeexpr_rem(a, b)
Expr *a, *b;
{
    if (!(possiblesigns(a) & 1) && !(possiblesigns(b) & 1))
	return makeexpr_mod(a, b);
    if (checkconst(b, 0))
        warning("Division by zero [163]");
    if (!*remname)
	note("Translating REM same as MOD [141]");
    return dodivmod(*remname ? remname : modname, EK_MOD, a, b);
}





int expr_not_cost(a)
Expr *a;
{
    int i, c;

    switch (a->kind) {

        case EK_CONST:
            return 0;

        case EK_NOT:
            return -1;

        case EK_EQ:
        case EK_NE:
        case EK_LT:
        case EK_GT:
        case EK_LE:
        case EK_GE:
            return 0;

        case EK_AND:
        case EK_OR:
            c = 0;
            for (i = 0; i < a->nargs; i++)
                c += expr_not_cost(a->args[i]);
            return (c > 1) ? 1 : c;

        case EK_BICALL:
            if (!strcmp(a->val.s, oddname) ||
                !strcmp(a->val.s, evenname))
                return 0;
            return 1;

        default:
            return 1;
    }
}



Expr *makeexpr_not(a)
Expr *a;
{
    Expr *ex;
    int i;

    if (debug>2) { fprintf(outf,"makeexpr_not("); dumpexpr(a); fprintf(outf,")\n"); }
    switch (a->kind) {

        case EK_CONST:
            if (a->val.type == tp_boolean) {
                a->val.i = !a->val.i;
                return a;
            }
            break;

        case EK_EQ:
            a->kind = EK_NE;
            return a;

        case EK_NE:
            a->kind = EK_EQ;
            return a;

        case EK_LT:
            a->kind = EK_GE;
            return a;

        case EK_GT:
            a->kind = EK_LE;
            return a;

        case EK_LE:
            a->kind = EK_GT;
            return a;

        case EK_GE:
            a->kind = EK_LT;
            return a;

        case EK_AND:
        case EK_OR:
            if (expr_not_cost(a) > 0)
                break;
            a->kind = (a->kind == EK_OR) ? EK_AND : EK_OR;
            for (i = 0; i < a->nargs; i++)
                a->args[i] = makeexpr_not(a->args[i]);
            return a;

        case EK_NOT:
            ex = a->args[0];
            FREE(a);
            ex->val.type = tp_boolean;
            return ex;

        case EK_BICALL:
            if (!strcmp(a->val.s, oddname) && *evenname) {
                strchange(&a->val.s, evenname);
                return a;
            } else if (!strcmp(a->val.s, evenname)) {
                strchange(&a->val.s, oddname);
                return a;
            }
            break;

	default:
	    break;
    }
    return makeexpr_un(EK_NOT, tp_boolean, a);
}




Type *mixsets(ep1, ep2)
Expr **ep1, **ep2;
{
    Expr *ex1 = *ep1, *ex2 = *ep2;
    Meaning *tvar;
    long min1, max1, min2, max2;
    Type *type;

    if (ex1->val.type->kind == TK_SMALLSET &&
        ex2->val.type->kind == TK_SMALLSET)
        return ex1->val.type;
    if (ex1->val.type->kind == TK_SMALLSET) {
        tvar = makestmttempvar(ex2->val.type, name_SET);
        ex1 = makeexpr_bicall_2(setexpandname, ex2->val.type,
                                makeexpr_var(tvar),
                                makeexpr_arglong(ex1, 1));
    }
    if (ex2->val.type->kind == TK_SMALLSET) {
        tvar = makestmttempvar(ex1->val.type, name_SET);
        ex2 = makeexpr_bicall_2(setexpandname, ex1->val.type,
                                makeexpr_var(tvar),
                                makeexpr_arglong(ex2, 1));
    }
    if (ord_range(ex1->val.type->indextype, &min1, &max1) &&
        ord_range(ex2->val.type->indextype, &min2, &max2)) {
        if (min1 <= min2 && max1 >= max2)
            type = ex1->val.type;
        else if (min2 <= min1 && max2 >= max1)
            type = ex2->val.type;
        else {
            if (min2 < min1) min1 = min2;
            if (max2 > max1) max1 = max2;
            type = maketype(TK_SET);
            type->basetype = tp_integer;
            type->indextype = maketype(TK_SUBR);
            type->indextype->basetype = ord_type(ex1->val.type->indextype);
            type->indextype->smin = makeexpr_long(min1);
            type->indextype->smax = makeexpr_long(max1);
        }
    } else
	type = ex1->val.type;
    *ep1 = ex1, *ep2 = ex2;
    return type;
}



Meaning *istempprocptr(ex)
Expr *ex;
{
    Meaning *mp;

    if (debug>2) { fprintf(outf,"istempprocptr("); dumpexpr(ex); fprintf(outf,")\n"); }
    if (ex->kind == EK_COMMA && ex->nargs == 3) {
        if ((mp = istempvar(ex->args[2])) != NULL &&
	    mp->type->kind == TK_PROCPTR &&
	    ex->args[0]->kind == EK_ASSIGN &&
	    ex->args[0]->args[0]->kind == EK_DOT &&
	    exprsame(ex->args[0]->args[0]->args[0], ex->args[2], 1) &&
	    ex->args[1]->kind == EK_ASSIGN &&
	    ex->args[1]->args[0]->kind == EK_DOT &&
	    exprsame(ex->args[1]->args[0]->args[0], ex->args[2], 1))
	    return mp;
    }
    if (ex->kind == EK_COMMA && ex->nargs == 2) {
        if ((mp = istempvar(ex->args[1])) != NULL &&
	    mp->type->kind == TK_CPROCPTR &&
	    ex->args[0]->kind == EK_ASSIGN &&
	    exprsame(ex->args[0]->args[0], ex->args[1], 1))
	    return mp;
    }
    return NULL;
}




Expr *makeexpr_stringify(ex)
Expr *ex;
{
    ex = makeexpr_stringcast(ex);
    if (ex->val.type->kind == TK_STRING)
        return ex;
    return makeexpr_sprintfify(ex);
}



Expr *makeexpr_rel(rel, a, b)
enum exprkind rel;
Expr *a, *b;
{
    int i, sign;
    Expr *ex, *ex2;
    Meaning *mp;
    char *name;

    if (debug>2) { fprintf(outf,"makeexpr_rel(%s,", exprkindname(rel)); dumpexpr(a); fprintf(outf,", "); dumpexpr(b); fprintf(outf,")\n"); }

    a = makeexpr_unlongcast(a);
    b = makeexpr_unlongcast(b);
    if ((compenums == 0 || (compenums < 0 && ansiC <= 0)) &&
	(rel != EK_EQ && rel != EK_NE)){
	a = enum_to_int(a);
	b = enum_to_int(b);
    }
    if (a->val.type != b->val.type) {
        if (a->val.type->kind == TK_STRING &&
            a->kind != EK_CONST) {
            b = makeexpr_stringify(b);
        } else if (b->val.type->kind == TK_STRING &&
                   b->kind != EK_CONST) {
	    a = makeexpr_stringify(a);
        } else if (ord_type(a->val.type)->kind == TK_CHAR ||
                   a->val.type->kind == TK_ARRAY) {
            b = gentle_cast(b, ord_type(a->val.type));
        } else if (ord_type(b->val.type)->kind == TK_CHAR ||
                   b->val.type->kind == TK_ARRAY) {
            a = gentle_cast(a, ord_type(b->val.type));
        } else if (a->val.type == tp_anyptr && !voidstar) {
            a = gentle_cast(a, b->val.type);
        } else if (b->val.type == tp_anyptr && !voidstar) {
            b = gentle_cast(b, a->val.type);
        }
    }
    if (useisspace && b->val.type->kind == TK_CHAR && checkconst(b, ' ')) {
        if (rel == EK_EQ) {
            freeexpr(b);
            return makeexpr_bicall_1("isspace", tp_boolean, a);
        } else if (rel == EK_NE) {
            freeexpr(b);
            return makeexpr_not(makeexpr_bicall_1("isspace", tp_boolean, a));
        }
    }
    if (rel == EK_LT || rel == EK_GE)
        sign = 1;
    else if (rel == EK_GT || rel == EK_LE)
        sign = -1;
    else
        sign = 0;
    if (ord_type(b->val.type)->kind == TK_INTEGER ||
	ord_type(b->val.type)->kind == TK_CHAR) {
        for (;;) {
            if (a->kind == EK_PLUS && ISCONST(a->args[a->nargs-1]->kind) &&
                 a->args[a->nargs-1]->val.i &&
                 (ISCONST(b->kind) ||
                  (b->kind == EK_PLUS && ISCONST(b->args[b->nargs-1]->kind)))) {
                b = makeexpr_minus(b, copyexpr(a->args[a->nargs-1]));
                a = makeexpr_minus(a, copyexpr(a->args[a->nargs-1]));
                continue;
            }
            if (b->kind == EK_PLUS && ISCONST(b->args[b->nargs-1]->kind) &&
                 b->args[b->nargs-1]->val.i &&
                 ISCONST(a->kind)) {
                a = makeexpr_minus(a, copyexpr(b->args[b->nargs-1]));
                b = makeexpr_minus(b, copyexpr(b->args[b->nargs-1]));
                continue;
            }
            if (b->kind == EK_PLUS && sign &&
	         ISCONST(b->args[b->nargs-1]->kind) &&
                 checkconst(b->args[b->nargs-1], sign)) {
                b = makeexpr_plus(b, makeexpr_long(-sign));
                switch (rel) {
                    case EK_LT:
                        rel = EK_LE;
                        break;
                    case EK_GT:
                        rel = EK_GE;
                        break;
                    case EK_LE:
                        rel = EK_LT;
                        break;
                    case EK_GE:
                        rel = EK_GT;
                        break;
		    default:
			break;
                }
                sign = -sign;
                continue;
            }
            if (a->kind == EK_TIMES && checkconst(b, 0) && !sign) {
                for (i = 0; i < a->nargs; i++) {
                    if (ISCONST(a->args[i]->kind) && a->args[i]->val.i)
                        break;
                    if (a->args[i]->kind == EK_SIZEOF)
                        break;
                }
                if (i < a->nargs) {
                    delfreearg(&a, i);
                    continue;
                }
            }
            break;
        }
        if (a->kind == EK_BICALL && !strcmp(a->val.s, "strlen") &&
            checkconst(b, 0)) {
            if (rel == EK_LT || rel == EK_GE) {
                note("Unusual use of STRLEN encountered [142]");
            } else {
                freeexpr(b);
                a = makeexpr_hat(grabarg(a, 0), 0);
                b = makeexpr_char(0);      /* "strlen(a) = 0" => "*a == 0" */
                if (rel == EK_EQ || rel == EK_LE)
                    return makeexpr_rel(EK_EQ, a, b);
                else
                    return makeexpr_rel(EK_NE, a, b);
            }
        }
        if (ISCONST(a->kind) && ISCONST(b->kind)) {
            if ((a->val.i == b->val.i && (rel == EK_EQ || rel == EK_GE || rel == EK_LE)) ||
                (a->val.i <  b->val.i && (rel == EK_NE || rel == EK_LE || rel == EK_LT)) ||
                (a->val.i >  b->val.i && (rel == EK_NE || rel == EK_GE || rel == EK_GT)))
                return makeexpr_val(make_ord(tp_boolean, 1));
            else
                return makeexpr_val(make_ord(tp_boolean, 0));
        }
	if ((a->val.type == tp_char || true_type(a) == tp_char) &&
	    ISCONST(b->kind) && signedchars != 0) {
	    i = (b->val.i == 128 && sign == 1) ||
		(b->val.i == 127 && sign == -1);
	    if (highcharbits && (highcharbits > 0 || signedchars < 0) && i) {
		if (highcharbits == 2)
		    b = makeexpr_long(128);
		else
		    b = makeexpr_un(EK_BNOT, tp_integer, makeexpr_long(127));
		return makeexpr_rel((rel == EK_GE || rel == EK_GT)
				    ? EK_NE : EK_EQ,
				    makeexpr_bin(EK_BAND, tp_integer,
						 eatcasts(a), b),
				    makeexpr_long(0));
	    } else if (signedchars == 1 && i) {
		return makeexpr_rel((rel == EK_GE || rel == EK_GT)
				    ? EK_LT : EK_GE,
				    eatcasts(a), makeexpr_long(0));
	    } else if (signedchars == 1 && b->val.i >= 128 && sign == 0) {
		b->val.i -= 256;
	    } else if (b->val.i >= 128 ||
		       (b->val.i == 127 && sign != 0)) {
		if (highcharbits && (highcharbits > 0 || signedchars < 0))
		    a = makeexpr_bin(EK_BAND, a->val.type, eatcasts(a),
				     makeexpr_long(255));
		else
		    a = force_unsigned(a);
	    }
	}
    } else if (a->val.type->kind == TK_STRING &&
               b->val.type->kind == TK_STRING) {
        if (b->kind == EK_CONST && b->val.i == 0 && !sign) {
            a = makeexpr_hat(a, 0);
            b = makeexpr_char(0);      /* "a = ''" => "*a == 0" */
        } else {
            a = makeexpr_bicall_2("strcmp", tp_int, a, b);
            b = makeexpr_long(0);
        }
    } else if ((a->val.type->kind == TK_ARRAY ||
		a->val.type->kind == TK_STRING ||
		a->val.type->kind == TK_RECORD) &&
	       (b->val.type->kind == TK_ARRAY ||
		b->val.type->kind == TK_STRING ||
		b->val.type->kind == TK_RECORD)) {
        if (a->val.type->kind == TK_ARRAY) {
            if (b->val.type->kind == TK_ARRAY) {
                ex = makeexpr_sizeof(copyexpr(a), 0);
                ex2 = makeexpr_sizeof(copyexpr(b), 0);
                if (!exprsame(ex, ex2, 1))
                    warning("Incompatible array sizes [164]");
                freeexpr(ex2);
            } else {
                ex = makeexpr_sizeof(copyexpr(a), 0);
            }
        } else
            ex = makeexpr_sizeof(copyexpr(b), 0);
	name = (usestrncmp &&
		a->val.type->kind == TK_ARRAY &&
		a->val.type->basetype->kind == TK_CHAR) ? "strncmp" : "memcmp";
        a = makeexpr_bicall_3(name, tp_int,
			      makeexpr_addr(a),
			      makeexpr_addr(b), ex);
        b = makeexpr_long(0);
    } else if (a->val.type->kind == TK_SET ||
               a->val.type->kind == TK_SMALLSET) {
        if (rel == EK_GE) {
            swapexprs(a, b);
            rel = EK_LE;
        }
        if (mixsets(&a, &b)->kind == TK_SMALLSET) {
            if (rel == EK_LE) {
                a = makeexpr_bin(EK_BAND, tp_integer,
                                 a, makeexpr_un(EK_BNOT, tp_integer, b));
                b = makeexpr_long(0);
                rel = EK_EQ;
            }
        } else if (b->kind == EK_BICALL &&
                   !strcmp(b->val.s, setexpandname) &&
                   (mp = istempvar(b->args[0])) != NULL &&
                   checkconst(b->args[1], 0)) {
            canceltempvar(mp);
            a = makeexpr_hat(a, 0);
            b = grabarg(b, 1);
            if (rel == EK_LE)
                rel = EK_EQ;
        } else {
            ex = makeexpr_bicall_2((rel == EK_LE) ? subsetname : setequalname,
                                   tp_boolean, a, b);
            return (rel == EK_NE) ? makeexpr_not(ex) : ex;
        }
    } else if (a->val.type->kind == TK_PROCPTR ||
	       a->val.type->kind == TK_CPROCPTR) {
        /* we compare proc only (not link) -- same as Pascal compiler! */
	if (a->val.type->kind == TK_PROCPTR)
	    a = makeexpr_dotq(a, "proc", tp_anyptr);
        if ((mp = istempprocptr(b)) != NULL) {
            canceltempvar(mp);
	    b = grabarg(grabarg(b, 0), 1);
            if (!voidstar)
                b = makeexpr_cast(b, tp_anyptr);
        } else if (b->val.type->kind == TK_PROCPTR)
            b = makeexpr_dotq(b, "proc", tp_anyptr);
    }
    return makeexpr_bin(rel, tp_boolean, a, b);
}




Expr *makeexpr_and(a, b)
Expr *a, *b;
{
    Expr *ex, **exp, *low;

    if (!a)
        return b;
    if (!b)
        return a;
    for (exp = &a; (ex = *exp)->kind == EK_AND; exp = &ex->args[1]) ;
    if ((b->kind == EK_LT || b->kind == EK_LE) &&
        ((ex->kind == EK_LE && exprsame(ex->args[1], b->args[0], 1)) ||
         (ex->kind == EK_GE && exprsame(ex->args[0], b->args[0], 1)))) {
        low = (ex->kind == EK_LE) ? ex->args[0] : ex->args[1];
        if (unsignedtrick && checkconst(low, 0)) {
            freeexpr(ex);
            b->args[0] = force_unsigned(b->args[0]);
            *exp = b;
            return a;
        }
        if (b->args[1]->val.type->kind == TK_CHAR && useisalpha) {
            if (checkconst(low, 'A') && checkconst(b->args[1], 'Z')) {
                freeexpr(ex);
                *exp = makeexpr_bicall_1("isupper", tp_boolean, grabarg(b, 0));
                return a;
            }
            if (checkconst(low, 'a') && checkconst(b->args[1], 'z')) {
                freeexpr(ex);
                *exp = makeexpr_bicall_1("islower", tp_boolean, grabarg(b, 0));
                return a;
            }
            if (checkconst(low, '0') && checkconst(b->args[1], '9')) {
                freeexpr(ex);
                *exp = makeexpr_bicall_1("isdigit", tp_boolean, grabarg(b, 0));
                return a;
            }
        }
    }
    return makeexpr_bin(EK_AND, tp_boolean, a, b);
}



Expr *makeexpr_or(a, b)
Expr *a, *b;
{
    Expr *ex, **exp, *low;

    if (!a)
        return b;
    if (!b)
        return a;
    for (exp = &a; (ex = *exp)->kind == EK_OR; exp = &ex->args[1]) ;
    if (((b->kind == EK_BICALL && !strcmp(b->val.s, "isdigit") &&
          ex->kind == EK_BICALL && !strcmp(ex->val.s, "isalpha")) ||
         (b->kind == EK_BICALL && !strcmp(b->val.s, "isalpha") &&
          ex->kind == EK_BICALL && !strcmp(ex->val.s, "isdigit"))) &&
        exprsame(ex->args[0], b->args[0], 1)) {
        strchange(&ex->val.s, "isalnum");
        freeexpr(b);
        return a;
    }
    if (((b->kind == EK_BICALL && !strcmp(b->val.s, "islower") &&
          ex->kind == EK_BICALL && !strcmp(ex->val.s, "isupper")) ||
         (b->kind == EK_BICALL && !strcmp(b->val.s, "isupper") &&
          ex->kind == EK_BICALL && !strcmp(ex->val.s, "islower"))) &&
        exprsame(ex->args[0], b->args[0], 1)) {
        strchange(&ex->val.s, "isalpha");
        freeexpr(b);
        return a;
    }
    if ((b->kind == EK_GT || b->kind == EK_GE) &&
        ((ex->kind == EK_GT && exprsame(ex->args[1], b->args[0], 1)) ||
         (ex->kind == EK_LT && exprsame(ex->args[0], b->args[0], 1)))) {
        low = (ex->kind == EK_GT) ? ex->args[0] : ex->args[1];
        if (unsignedtrick && checkconst(low, 0)) {
            freeexpr(ex);
            b->args[0] = force_unsigned(b->args[0]);
            *exp = b;
            return a;
        }
    }
    return makeexpr_bin(EK_OR, tp_boolean, a, b);
}



Expr *makeexpr_range(ex, exlow, exhigh, higheq)
Expr *ex, *exlow, *exhigh;
int higheq;
{
    Expr *ex2;
    enum exprkind rel = (higheq) ? EK_LE : EK_LT;

    if (exprsame(exlow, exhigh, 1) && higheq)
        return makeexpr_rel(EK_EQ, ex, exlow);
    ex2 = makeexpr_rel(rel, copyexpr(ex), exhigh);
    if (lelerange)
        return makeexpr_and(makeexpr_rel(EK_LE, exlow, ex), ex2);
    else
        return makeexpr_and(makeexpr_rel(EK_GE, ex, exlow), ex2);
}




Expr *makeexpr_cond(c, a, b)
Expr *c, *a, *b;
{
    Expr *ex;

    ex = makeexpr(EK_COND, 3);
    ex->val.type = a->val.type;
    ex->args[0] = c;
    ex->args[1] = a;
    ex->args[2] = b;
    if (debug>2) { fprintf(outf,"makeexpr_cond returns "); dumpexpr(ex); fprintf(outf,"\n"); }
    return ex;
}




int expr_is_lvalue(ex)
Expr *ex;
{
    Meaning *mp;

    switch (ex->kind) {

        case EK_VAR:
            mp = (Meaning *)ex->val.i;
            return (mp->kind == MK_VAR || mp->kind == MK_PARAM ||
		    mp->kind == MK_VARPARAM ||
                    (mp->kind == MK_CONST &&
                     (mp->type->kind == TK_ARRAY ||
                      mp->type->kind == TK_RECORD ||
                      mp->type->kind == TK_SET)));

        case EK_HAT:
        case EK_NAME:
            return 1;

        case EK_INDEX:
	case EK_DOT:
	    return expr_is_lvalue(ex->args[0]);

	case EK_COMMA:
	    return expr_is_lvalue(ex->args[ex->nargs-1]);

        default:
            return 0;
    }
}


int expr_has_address(ex)
Expr *ex;
{
    if (ex->kind == EK_DOT &&
	((Meaning *)ex->val.i)->val.i)
	return 0;    /* bit fields do not have an address */
    return expr_is_lvalue(ex);
}



Expr *checknil(ex)
Expr *ex;
{
    if (nilcheck == 1) {
        if (singlevar(ex)) {
            ex = makeexpr_un(EK_CHECKNIL, ex->val.type, ex);
        } else {
            ex = makeexpr_bin(EK_CHECKNIL, ex->val.type, ex,
                              makeexpr_var(makestmttempvar(ex->val.type,
                                                           name_PTR)));
        }
    }
    return ex;
}


int checkvarinlists(yes, no, def, mp)
Strlist *yes, *no;
int def;
Meaning *mp;
{
    char *cp;
    Meaning *ctx;

    if (mp->kind == MK_FIELD)
	ctx = mp->rectype->meaning;
    else
	ctx = mp->ctx;
    if (ctx && ctx->name)
	cp = format_ss("%s.%s", ctx->name, mp->name);
    else
	cp = NULL;
    if (strlist_cifind(yes, cp))
	return 1;
    if (strlist_cifind(no, cp))
	return 0;
    if (strlist_cifind(yes, mp->name))
	return 1;
    if (strlist_cifind(no, mp->name))
	return 0;
    if (strlist_cifind(yes, "1"))
	return 1;
    if (strlist_cifind(no, "1"))
	return 0;
    return def;
}


void requirefilebuffer(ex)
Expr *ex;
{
    Meaning *mp;

    if (!isfiletype(ex->val.type, 0))
	return;
    mp = isfilevar(ex);
    if (!mp) {
	if (ex->kind == EK_HAT)
	    ex = ex->args[0];
	if (ex->kind == EK_VAR) {
	    mp = (Meaning *)ex->val.i;
	    if (mp->kind == MK_PARAM || mp->kind == MK_VARPARAM)
		note(format_s("File parameter %s can't access buffers (try StructFiles = 1) [318]",
			      mp->name));
	}
    } else if (!mp->bufferedfile &&
	       checkvarinlists(bufferedfiles, unbufferedfiles, 1, mp)) {
	if (mp->wasdeclared)
	    note(format_s("Discovered too late that %s should be buffered [143]",
			  mp->name));
	mp->bufferedfile = 1;
    }
}


Expr *makeexpr_hat(a, check)
Expr *a;
int check;
{
    Expr *ex;

    if (debug>2) { fprintf(outf,"makeexpr_hat("); dumpexpr(a); fprintf(outf,")\n"); }
    if (isfiletype(a->val.type, -1)) {
	requirefilebuffer(a);
	if (*chargetfbufname &&
	    filebasetype(a->val.type)->kind == TK_CHAR)
	    return makeexpr_bicall_1(chargetfbufname,
				     filebasetype(a->val.type),
				     filebasename(a));
	else if (*arraygetfbufname &&
		 filebasetype(a->val.type)->kind == TK_ARRAY)
	    return makeexpr_bicall_2(arraygetfbufname,
				     filebasetype(a->val.type),
				     filebasename(a),
				     makeexpr_type(filebasetype(a->val.type)));
	else
	    return makeexpr_bicall_2(getfbufname,
				     filebasetype(a->val.type),
				     filebasename(a),
				     makeexpr_type(filebasetype(a->val.type)));
    }
    if (a->kind == EK_PLUS &&
               (ex = a->args[0])->val.type->kind == TK_POINTER &&
               (ex->val.type->basetype->kind == TK_ARRAY ||
                ex->val.type->basetype->kind == TK_STRING ||
                ex->val.type->basetype->kind == TK_SET)) {
        ex->val.type = ex->val.type->basetype;   /* convert *(a+n) to a[n] */
        deletearg(&a, 0);
        if (a->nargs == 1)
            a = grabarg(a, 0);
        return makeexpr_bin(EK_INDEX, ex->val.type->basetype, ex, a);
    }
    if (a->val.type->kind == TK_STRING ||
        a->val.type->kind == TK_ARRAY ||
        a->val.type->kind == TK_SET) {
        if (starindex == 0)
            return makeexpr_bin(EK_INDEX, a->val.type->basetype, a, makeexpr_long(0));
        else
            return makeexpr_un(EK_HAT, a->val.type->basetype, a);
    }
    if (a->val.type->kind != TK_POINTER || !a->val.type->basetype) {
        warning("bad pointer dereference [165]");
        return a;
    }
    if (a->kind == EK_CAST &&
	a->val.type->basetype->kind == TK_POINTER &&
	a->args[0]->val.type->kind == TK_POINTER &&
	a->args[0]->val.type->basetype->kind == TK_POINTER) {
	return makeexpr_cast(makeexpr_hat(a->args[0], 0),
			     a->val.type->basetype);
    }
    switch (a->val.type->basetype->kind) {

      case TK_ARRAY:
      case TK_STRING:
      case TK_SET:
	if (a->kind != EK_HAT || 1 ||
	    a->val.type == a->args[0]->val.type->basetype) {
	    a->val.type = a->val.type->basetype;
	    return a;
	}

      default:
	if (a->kind == EK_ADDR) {
	    ex = a->args[0];
	    FREE(a);
	    return ex;
	} else {
	    if (check)
		ex = checknil(a);
	    else
		ex = a;
	    return makeexpr_un(EK_HAT, a->val.type->basetype, ex);
        }
    }
}



Expr *un_sign_extend(a)
Expr *a;
{
    if (a->kind == EK_BICALL &&
        !strcmp(a->val.s, signextname) && *signextname) {
        return grabarg(a, 0);
    }
    return a;
}



Expr *makeexpr_addr(a)
Expr *a;
{
    Expr *ex;
    Type *type;
    Meaning *mp;

    a = un_sign_extend(a);
    type = makepointertype(a->val.type);
    if (debug>2) { fprintf(outf,"makeexpr_addr("); dumpexpr(a); fprintf(outf,", "); dumptypename(type, 1); fprintf(outf,")\n"); }
    if (a->kind == EK_CONST && a->val.type->kind == TK_STRING) {
        return a;     /* kludge to help assignments */
    } else if (a->kind == EK_INDEX &&
	       (a->val.type->kind != TK_ARRAY &&
		a->val.type->kind != TK_SET &&
		a->val.type->kind != TK_STRING) &&
	       (addindex == 1 ||
		(addindex != 0 && checkconst(a->args[1], 0)))) {
        ex = makeexpr_plus(makeexpr_addr(a->args[0]), a->args[1]);
        FREE(a);
        ex->val.type = type;
        return ex;
    } else if (a->kind == EK_CAST) {
	return makeexpr_cast(makeexpr_addr(a->args[0]), type);
    } else if (a->kind == EK_ACTCAST) {
	return makeexpr_actcast(makeexpr_addr(a->args[0]), type);
    } else if (a->kind == EK_LITCAST) {
	if (a->args[0]->kind == EK_NAME) {
	    if (my_strchr(a->args[0]->val.s, '(') ||
		my_strchr(a->args[0]->val.s, '['))
		note("Constructing pointer type by adding '*' may be incorrect [322]");
	    return makeexpr_bin(EK_LITCAST, tp_integer,
				makeexpr_name(format_s("%s*",
						       a->args[0]->val.s),
					      tp_integer),
				makeexpr_addr(a->args[1]));
	} else
	    return makeexpr_bin(EK_LITCAST, tp_integer, makeexpr_type(type),
				makeexpr_addr(a->args[1]));
    } else {
        switch (a->val.type->kind) {

	  case TK_ARRAY:
	  case TK_STRING:
	  case TK_SET:
	    if (a->val.type->smin) {
		return makeexpr_un(EK_ADDR, type,
				   makeexpr_index(a,
						  copyexpr(a->val.type->smin),
						  NULL));
	    }
	    a->val.type = type;
	    return a;

	  default:
	    if (a->kind == EK_HAT) {
		ex = a->args[0];
		FREE(a);
		return ex;
	    } else if (a->kind == EK_ACTCAST)
		return makeexpr_actcast(makeexpr_addr(grabarg(a, 0)), type);
	    else if (a->kind == EK_CAST)
		return makeexpr_cast(makeexpr_addr(grabarg(a, 0)), type);
	    else {
		if (a->kind == EK_VAR &&
		    (mp = (Meaning *)a->val.i)->kind == MK_PARAM &&
		    mp->type != promote_type(mp->type) &&
		    fixpromotedargs) {
		    note(format_s("Taking & of possibly promoted param %s [324]",
				  mp->name));
		    if (fixpromotedargs == 1) {
			mp->varstructflag = 1;
			mp->ctx->varstructflag = 1;
		    }
		}
		return makeexpr_un(EK_ADDR, type, a);
	    }
	}
    }
}



Expr *makeexpr_addrstr(a)
Expr *a;
{
    if (debug>2) { fprintf(outf,"makeexpr_addrstr("); dumpexpr(a); fprintf(outf,")\n"); }
    if (a->val.type->kind == TK_POINTER)
	return a;
    return makeexpr_addr(a);
}



Expr *makeexpr_addrf(a)
Expr *a;
{
    Meaning *mp, *tvar;

    mp = (Meaning *)a->val.i;
    if (is_std_file(a)) {
        if (addrstdfiles == 0) {
            note(format_s("Taking address of %s; consider setting VarFiles = 0 [144]",
                          (a->kind == EK_VAR) ? ((Meaning *)a->val.i)->name
                                              : a->val.s));
            tvar = makestmttempvar(tp_text, name_TEMP);
            return makeexpr_comma(makeexpr_assign(makeexpr_var(tvar), a),
                                  makeexpr_addr(makeexpr_var(tvar)));
        }
    }
    if ((a->kind == EK_VAR &&
         mp->kind == MK_FIELD && mp->val.i) ||
        (a->kind == EK_BICALL &&
         !strcmp(a->val.s, getbitsname))) {
        warning("Can't take the address of a bit-field [166]");
    }
    return makeexpr_addr(a);
}



Expr *makeexpr_index(a, b, offset)
Expr *a, *b, *offset;
{
    Type *indextype, *btype;

    if (debug>2) { fprintf(outf,"makeexpr_index("); dumpexpr(a); fprintf(outf,", "); dumpexpr(b);
                                                                 fprintf(outf,", "); dumpexpr(offset); fprintf(outf,")\n"); }
    indextype = (a->val.type->kind == TK_ARRAY) ? a->val.type->indextype
                                                : tp_integer;
    b = gentle_cast(b, indextype);
    if (!offset)
        offset = makeexpr_long(0);
    b = makeexpr_minus(b, gentle_cast(offset, indextype));
    btype = a->val.type;
    if (btype->basetype)
	btype = btype->basetype;
    if (checkconst(b, 0) && starindex == 1)
        return makeexpr_un(EK_HAT, btype, a);
    else
        return makeexpr_bin(EK_INDEX, btype, a,
                            gentle_cast(b, indextype));
}



Expr *makeexpr_type(type)
Type *type;
{
    Expr *ex;

    ex = makeexpr(EK_TYPENAME, 0);
    ex->val.type = type;
    return ex;
}


Expr *makeexpr_sizeof(ex, incskipped)
Expr *ex;
int incskipped;
{
    Expr *ex2, *ex3;
    Type *btype;
    char *name;

    if (ex->val.type->meaning) {
	name = find_special_variant(ex->val.type->meaning->name,
				    "SpecialSizeOf", specialsizeofs, 1);
	if (name) {
	    freeexpr(ex);
	    return pc_expr_str(name);
	}
    }
    switch (ex->val.type->kind) {

        case TK_CHAR:
        case TK_BOOLEAN:
            freeexpr(ex);
            return makeexpr_long(1);

        case TK_SUBR:
	    btype = findbasetype(ex->val.type, ODECL_NOPRES);
	    if (btype->kind == TK_CHAR || btype == tp_abyte) {
		freeexpr(ex);
		return makeexpr_long(1);
	    }
	    break;

        case TK_STRING:
        case TK_ARRAY:
            if (!ex->val.type->meaning || ex->val.type->kind == TK_STRING ||
		(!incskipped && ex->val.type->smin)) {
                ex3 = arraysize(ex->val.type, incskipped);
                return makeexpr_times(ex3,
                                      makeexpr_sizeof(makeexpr_type(
                                           ex->val.type->basetype), 1));
            }
            break;

        case TK_SET:
            ord_range_expr(ex->val.type->indextype, NULL, &ex2);
            freeexpr(ex);
            return makeexpr_times(makeexpr_plus(makeexpr_div(copyexpr(ex2),
                                                             makeexpr_setbits()),
                                                makeexpr_long(2)),
                                  makeexpr_sizeof(makeexpr_type(tp_integer), 0));

	default:
	    break;
    }
    if (ex->kind != EK_CONST &&
        (findbasetype(ex->val.type,0)->meaning || /* if type has a name... */
         ex->val.type->kind == TK_STRING ||       /* if C sizeof(expr) will give wrong answer */
         ex->val.type->kind == TK_ARRAY ||
         ex->val.type->kind == TK_SET)) {
        ex2 = makeexpr_type(ex->val.type);
        freeexpr(ex);
        ex = ex2;
    }
    return makeexpr_un(EK_SIZEOF, tp_integer, ex);
}




/* Compute a measure of how fast or slow the expression is likely to be.
   0 is a constant, 1 is a variable, extra points added per "operation". */

int exprspeed(ex)
Expr *ex;
{
    Meaning *mp, *mp2;
    int i, cost, speed;

    switch (ex->kind) {

        case EK_VAR:
            mp = (Meaning *)ex->val.i;
            if (mp->kind == MK_CONST)
                return 0;
            if (!mp->ctx || mp->ctx->kind == MK_FUNCTION)
                return 1;
            i = 1;
            for (mp2 = curctx; mp2 && mp2 != mp->ctx; mp2 = mp2->ctx)
                i++;    /* cost of following static links */
            return (i);

        case EK_CONST:
        case EK_LONGCONST:
        case EK_SIZEOF:
            return 0;

        case EK_ADDR:
            speed = exprspeed(ex->args[0]);
            return (speed > 1) ? speed : 0;

        case EK_DOT:
            return exprspeed(ex->args[0]);

        case EK_NEG:
            return exprspeed(ex->args[0]) + 1;

        case EK_CAST:
        case EK_ACTCAST:
            i = (ord_type(ex->val.type)->kind == TK_REAL) !=
                (ord_type(ex->args[0]->val.type)->kind == TK_REAL);
            return (i + exprspeed(ex->args[0]));

        case EK_COND:
            return 2 + exprspeed(ex->args[0]) +
                   MAX(exprspeed(ex->args[1]), exprspeed(ex->args[2]));

        case EK_AND:
        case EK_OR:
        case EK_COMMA:
            speed = 2;
            for (i = 0; i < ex->nargs; i++)
                speed += exprspeed(ex->args[i]);
            return speed;

        case EK_FUNCTION:
        case EK_BICALL:
        case EK_SPCALL:
            return 1000;

        case EK_ASSIGN:
        case EK_POSTINC:
        case EK_POSTDEC:
            return 100 + exprspeed(ex->args[0]) + exprspeed(ex->args[1]);

        default:
            cost = (ex->kind == EK_PLUS) ? 1 : 2;
            if (ex->val.type->kind == TK_REAL)
                cost *= 2;
            speed = -cost;
            for (i = 0; i < ex->nargs; i++) {
                if (!isliteralconst(ex->args[i], NULL) ||
                    ex->val.type->kind == TK_REAL)
                    speed += exprspeed(ex->args[i]) + cost;
            }
            return MAX(speed, 0);
    }
}




int noargdependencies(ex, vars)
Expr *ex;
int vars;
{
    int i;

    for (i = 0; i < ex->nargs; i++) {
        if (!nodependencies(ex->args[i], vars))
            return 0;
    }
    return 1;
}


int nodependencies(ex, vars)
Expr *ex;
int vars;   /* 1 if explicit dependencies on vars count as dependencies */
{           /* 2 if global but not local vars count as dependencies */
    Meaning *mp;

    if (debug>2) { fprintf(outf,"nodependencies("); dumpexpr(ex); fprintf(outf,")\n"); }
    if (!noargdependencies(ex, vars))
        return 0;
    switch (ex->kind) {

        case EK_VAR:
            mp = (Meaning *)ex->val.i;
	    if (mp->kind == MK_CONST)
		return 1;
	    if (vars == 2 &&
		mp->ctx == curctx &&
		mp->ctx->kind == MK_FUNCTION &&
		!mp->varstructflag)
		return 1;
            return (mp->kind == MK_CONST ||
		    (!vars &&
		     (mp->kind == MK_VAR || mp->kind == MK_VARREF ||
		      mp->kind == MK_PARAM || mp->kind == MK_VARPARAM)));

        case EK_BICALL:
            return nosideeffects_func(ex);

        case EK_FUNCTION:
        case EK_SPCALL:
        case EK_ASSIGN:
        case EK_POSTINC:
        case EK_POSTDEC:
        case EK_HAT:
        case EK_INDEX:
            return 0;

        default:
            return 1;
    }
}



int exprdependsvar(ex, mp)
Expr *ex;
Meaning *mp;
{
    int i;

    i = ex->nargs;
    while (--i >= 0)
	if (exprdependsvar(ex->args[i], mp))
	    return 1;
    switch (ex->kind) {

        case EK_VAR:
	    return ((Meaning *)ex->val.i == mp);

	case EK_BICALL:
	    if (nodependencies(ex, 1))
		return 0;

	/* fall through */
	case EK_FUNCTION:
	case EK_SPCALL:
	    return (mp->ctx != curctx ||
		    mp->ctx->kind != MK_FUNCTION ||
		    mp->varstructflag);

	case EK_HAT:
	    return 1;

	default:
	    return 0;
    }
}


int exprdepends(ex, ex2)
Expr *ex, *ex2;     /* Expression ex somehow depends on value of ex2 */
{
    switch (ex2->kind) {

        case EK_VAR:
	    return exprdependsvar(ex, (Meaning *)ex2->val.i);

	case EK_CONST:
	case EK_LONGCONST:
	    return 0;

	case EK_INDEX:
	case EK_DOT:
	    return exprdepends(ex, ex2->args[0]);

	default:
	    return !nodependencies(ex, 1);
    }
}


int nosideeffects_func(ex)
Expr *ex;
{
    Meaning *mp;
    Symbol *sp;

    switch (ex->kind) {

        case EK_FUNCTION:
            mp = (Meaning *)ex->val.i;
            sp = findsymbol_opt(mp->name);
            return sp && (sp->flags & (NOSIDEEFF|DETERMF));

        case EK_BICALL:
            sp = findsymbol_opt(ex->val.s);
            return sp && (sp->flags & (NOSIDEEFF|DETERMF));

        default:
            return 0;
    }
}



int deterministic_func(ex)
Expr *ex;
{
    Meaning *mp;
    Symbol *sp;

    switch (ex->kind) {

        case EK_FUNCTION:
            mp = (Meaning *)ex->val.i;
            sp = findsymbol_opt(mp->name);
            return sp && (sp->flags & DETERMF);

        case EK_BICALL:
            sp = findsymbol_opt(ex->val.s);
            return sp && (sp->flags & DETERMF);

        default:
            return 0;
    }
}




int noargsideeffects(ex, mode)
Expr *ex;
int mode;
{
    int i;

    for (i = 0; i < ex->nargs; i++) {
        if (!nosideeffects(ex->args[i], mode))
            return 0;
    }
    return 1;
}


/* mode=0: liberal about bicall's: safe unless sideeffects_bicall() */
/* mode=1: conservative about bicall's: must be explicitly NOSIDEEFF */

int nosideeffects(ex, mode)
Expr *ex;
int mode;
{
    if (debug>2) { fprintf(outf,"nosideeffects("); dumpexpr(ex); fprintf(outf,")\n"); }
    if (!noargsideeffects(ex, mode))
        return 0;
    switch (ex->kind) {

        case EK_BICALL:
            if (mode == 0)
                return !sideeffects_bicall(ex->val.s);

        /* fall through */
        case EK_FUNCTION:
            return nosideeffects_func(ex);

        case EK_SPCALL:
        case EK_ASSIGN:
        case EK_POSTINC:
        case EK_POSTDEC:
            return 0;

        default:
            return 1;
    }
}



int exproccurs(ex, ex2)
Expr *ex, *ex2;
{
    int i, count = 0;

    if (debug>2) { fprintf(outf,"exproccurs("); dumpexpr(ex); fprintf(outf,", "); dumpexpr(ex2); fprintf(outf,")\n"); }
    for (i = 0; i < ex->nargs; i++)
        count += exproccurs(ex->args[i], ex2);
    if (exprsame(ex, ex2, 0))
        count++;
    return count;
}



Expr *singlevar(ex)
Expr *ex;
{
    if (debug>2) { fprintf(outf,"singlevar("); dumpexpr(ex); fprintf(outf,")\n"); }
    switch (ex->kind) {

        case EK_VAR:
        case EK_MACARG:
            return ex;

        case EK_HAT:
        case EK_ADDR:
        case EK_DOT:
            return singlevar(ex->args[0]);

        case EK_INDEX:
#if 0
            if (!nodependencies(ex->args[1], 1))
                return NULL;
#endif
            return singlevar(ex->args[0]);

	default:
	    return NULL;
    }
}



/* Is "ex" a function which takes a return buffer pointer as its
   first argument, and returns a copy of that pointer? */

int structuredfunc(ex)
Expr *ex;
{
    Meaning *mp;
    Symbol *sp;

    if (debug>2) { fprintf(outf,"structuredfunc("); dumpexpr(ex); fprintf(outf,")\n"); }
    switch (ex->kind) {

        case EK_FUNCTION:
            mp = (Meaning *)ex->val.i;
            if (mp->isfunction && mp->cbase && mp->cbase->kind == MK_VARPARAM)
                return 1;
            sp = findsymbol_opt(mp->name);
            return sp && (sp->flags & (STRUCTF|STRLAPF));

        case EK_BICALL:
            sp = findsymbol_opt(ex->val.s);
            return sp && (sp->flags & (STRUCTF|STRLAPF));

	default:
	    return 0;
    }
}



int strlapfunc(ex)
Expr *ex;
{
    Meaning *mp;
    Symbol *sp;

    switch (ex->kind) {

        case EK_FUNCTION:
            mp = (Meaning *)ex->val.i;
            sp = findsymbol_opt(mp->name);
            return sp && (sp->flags & STRLAPF);

        case EK_BICALL:
            sp = findsymbol_opt(ex->val.s);
            return sp && (sp->flags & STRLAPF);

        default:
            return 0;
    }
}



Meaning *istempvar(ex)
Expr *ex;
{
    Meaning *mp;

    if (debug>2) { fprintf(outf,"istempvar("); dumpexpr(ex); fprintf(outf,")\n"); }
    if (ex->kind == EK_VAR) {
        mp = (Meaning *)ex->val.i;
        if (mp->istemporary)
            return mp;
        else
            return NULL;
    }
    return NULL;
}


Meaning *totempvar(ex)
Expr *ex;
{
    while (structuredfunc(ex))
	ex = ex->args[0];
    return istempvar(ex);
}



Meaning *isretvar(ex)
Expr *ex;
{
    Meaning *mp;

    if (debug>2) { fprintf(outf,"isretvar("); dumpexpr(ex); fprintf(outf,")\n"); }
    if (ex->kind == EK_HAT)
        ex = ex->args[0];
    if (ex->kind == EK_VAR) {
        mp = (Meaning *)ex->val.i;
        if (mp->ctx && mp->ctx->kind == MK_FUNCTION &&
            mp->ctx->isfunction && mp == mp->ctx->cbase)
            return mp;
        else
            return NULL;
    }
    return NULL;
}



Expr *bumpstring(ex, index, offset)
Expr *ex, *index;
int offset;
{
    if (checkconst(index, offset)) {
        freeexpr(index);
        return ex;
    }
    if (addindex != 0)
        ex = makeexpr_plus(makeexpr_addrstr(ex),
			   makeexpr_minus(index, makeexpr_long(offset)));
    else
        ex = makeexpr_addr(makeexpr_index(ex, index, makeexpr_long(offset)));
    ex->val.type = tp_str255;
    return ex;
}



long po2m1(n)
int n;
{
    if (n == 32)
        return -1;
    else if (n == 31)
        return 0x7fffffff;
    else
        return (1<<n) - 1;
}



int isarithkind(kind)
enum exprkind kind;
{
    return (kind == EK_EQ || kind == EK_LT || kind == EK_GT ||
	    kind == EK_NE || kind == EK_LE || kind == EK_GE ||
	    kind == EK_PLUS || kind == EK_TIMES || kind == EK_DIVIDE ||
	    kind == EK_DIV || kind == EK_MOD || kind == EK_NEG ||
	    kind == EK_AND || kind == EK_OR || kind == EK_NOT ||
	    kind == EK_BAND || kind == EK_BOR || kind == EK_BXOR ||
	    kind == EK_LSH || kind == EK_RSH || kind == EK_BNOT ||
	    kind == EK_FUNCTION || kind == EK_BICALL);
}


Expr *makeexpr_assign(a, b)
Expr *a, *b;
{
    int i, j;
    Expr *ex, *ex2, *ex3, **ep;
    Meaning *mp;
    Type *tp;

    if (debug>2) { fprintf(outf,"makeexpr_assign("); dumpexpr(a); fprintf(outf,", "); dumpexpr(b); fprintf(outf,")\n"); }
    if (stringtrunclimit > 0 &&
	a->val.type->kind == TK_STRING &&
	(i = strmax(a)) <= stringtrunclimit &&
	strmax(b) > i) {
	note("Possible string truncation in assignment [145]");
    }
    a = un_sign_extend(a);
    b = gentle_cast(b, a->val.type);
    if (b->kind == EK_BICALL && !strcmp(b->val.s, "sprintf") &&
         (mp = istempvar(b->args[0])) != NULL &&
         b->nargs >= 2 &&
         b->args[1]->kind == EK_CONST &&              /* all this handles string appending */
         b->args[1]->val.i > 2 &&                     /*   of the form, "s := s + ..." */
         !strncmp(b->args[1]->val.s, "%s", 2) &&
         exprsame(a, b->args[2], 1) &&
         nosideeffects(a, 0) &&
         (ex = singlevar(a)) != NULL) {
        ex2 = copyexpr(b);
        delfreearg(&ex2, 2);
        freeexpr(ex2->args[1]);
        ex2->args[1] = makeexpr_lstring(b->args[1]->val.s+2,
                                        b->args[1]->val.i-2);
        if (/*(ex = singlevar(a)) != NULL && */
           /* noargdependencies(ex2) && */ !exproccurs(ex2, ex)) {
            freeexpr(b);
            if (ex2->args[1]->val.i == 2 &&     /* s := s + s2 */
                !strncmp(ex2->args[1]->val.s, "%s", 2)) {
                canceltempvar(mp);
		tp = ex2->val.type;
                return makeexpr_bicall_2("strcat", tp,
                                         makeexpr_addrstr(a), grabarg(ex2, 2));
            } else if (sprintflength(ex2, 0) >= 0) {    /* s := s + 's2' */
		tp = ex2->val.type;
                return makeexpr_bicall_2("strcat", tp,
                                         makeexpr_addrstr(a),
                                         makeexpr_unsprintfify(ex2));
            } else {                            /* general case */
                canceltempvar(mp);
                freeexpr(ex2->args[0]);
                ex = makeexpr_bicall_1("strlen", tp_int, copyexpr(a));
                ex2->args[0] = bumpstring(a, ex, 0);
                return ex2;
            }
        } else
            freeexpr(ex2);
    }
    if (b->kind == EK_BICALL && !strcmp(b->val.s, "sprintf") &&
         istempvar(b->args[0]) &&
         (ex = singlevar(a)) != NULL) {
        j = -1;     /* does lhs var appear exactly once on rhs? */
        for (i = 2; i < b->nargs; i++) {
            if (exprsame(b->args[i], ex, 1) && j < 0)
                j = i;
            else if (exproccurs(b->args[i], ex))
                break;
        }
        if (i == b->nargs && j > 0) {
            b->args[j] = makeexpr_bicall_2("strcpy", tp_str255,
                                           makeexpr_addrstr(b->args[0]),
                                           makeexpr_addrstr(b->args[j]));
            b->args[0] = makeexpr_addrstr(a);
            return b;
        }
    }
    if (structuredfunc(b) && (ex2 = singlevar(a)) != NULL) {
	ep = &b->args[0];
	i = strlapfunc(b);
	while (structuredfunc((ex = *ep))) {
	    i = i && strlapfunc(ex);
	    ep = &ex->args[0];
	}
	if ((mp = istempvar(ex)) != NULL &&
	    (i || !exproccurs(b, ex2))) {
	    canceltempvar(mp);
	    freeexpr(*ep);
	    *ep = makeexpr_addrstr(a);
	    return b;
	}
    }
    if (a->val.type->kind == TK_PROCPTR &&
        (mp = istempprocptr(b)) != NULL &&
        nosideeffects(a, 0)) {
        freeexpr(b->args[0]->args[0]->args[0]);
        b->args[0]->args[0]->args[0] = copyexpr(a);
	if (b->nargs == 3) {
	    freeexpr(b->args[1]->args[0]->args[0]);
	    b->args[1]->args[0]->args[0] = a;
	    delfreearg(&b, 2);
	} else {
	    freeexpr(b->args[1]);
	    b->args[1] = makeexpr_assign(makeexpr_dotq(a, "link", tp_anyptr),
					 makeexpr_nil());
	}
        canceltempvar(mp);
        return b;
    }
    if (a->val.type->kind == TK_PROCPTR &&
	(b->val.type->kind == TK_CPROCPTR ||
	 checkconst(b, 0))) {
	ex = makeexpr_dotq(copyexpr(a), "proc", tp_anyptr);
	b = makeexpr_comma(makeexpr_assign(ex, b),
			   makeexpr_assign(makeexpr_dotq(a, "link", tp_anyptr),
					   makeexpr_nil()));
	return b;
    }
    if (a->val.type->kind == TK_CPROCPTR &&
	(mp = istempprocptr(b)) != NULL &&
	nosideeffects(a, 0)) {
	freeexpr(b->args[0]->args[0]);
	b->args[0]->args[0] = a;
	if (b->nargs == 3)
	    delfreearg(&b, 1);
	delfreearg(&b, 1);
	canceltempvar(mp);
	return b;
    }
    if (a->val.type->kind == TK_CPROCPTR &&
	b->val.type->kind == TK_PROCPTR) {
	b = makeexpr_dotq(b, "proc", tp_anyptr);
    }
    if (a->val.type->kind == TK_STRING) {
        if (b->kind == EK_CONST && b->val.i == 0 && !isretvar(a)) {
                /* optimizing retvar would mess up "return" optimization */
            return makeexpr_assign(makeexpr_hat(a, 0),
                                   makeexpr_char(0));
        }
        a = makeexpr_addrstr(a);
        b = makeexpr_addrstr(b);
        return makeexpr_bicall_2("strcpy", a->val.type, a, b);
    }
    if (a->kind == EK_BICALL && !strcmp(a->val.s, "strlen")) {
        if (b->kind == EK_CAST &&
             ord_type(b->args[0]->val.type)->kind == TK_INTEGER) {
            b = grabarg(b, 0);
        }
        j = (b->kind == EK_PLUS &&      /* handle "s[0] := xxx" */
             b->args[0]->kind == EK_BICALL &&
             !strcmp(b->args[0]->val.s, "strlen") &&
             exprsame(a->args[0], b->args[0]->args[0], 0) &&
             isliteralconst(b->args[1], NULL) == 2);
        if (j && b->args[1]->val.i > 0 &&
                 b->args[1]->val.i <= 5) {     /* lengthening the string */
            a = grabarg(a, 0);
            i = b->args[1]->val.i;
            freeexpr(b);
            if (i == 1)
                b = makeexpr_string(" ");
            else
                b = makeexpr_lstring("12345", i);
            return makeexpr_bicall_2("strcat", a->val.type, a, b);
        } else {      /* maybe shortening the string */
            if (!j && !isconstexpr(b, NULL))
                note("Modification of string length may translate incorrectly [146]");
            a = grabarg(a, 0);
            b = makeexpr_ord(b);
            return makeexpr_assign(makeexpr_index(a, b, NULL),
                                   makeexpr_char(0));
        }
    }
    if (a->val.type->kind == TK_ARRAY ||
	(a->val.type->kind == TK_PROCPTR && copystructs < 1) ||
	(a->val.type->kind == TK_RECORD &&
	 (copystructs < 1 || a->val.type != b->val.type))) {
        ex = makeexpr_sizeof(copyexpr(a), 0);
        ex2 = makeexpr_sizeof(copyexpr(b), 0);
        if (!exprsame(ex, ex2, 1)) {
	    if (a->val.type->kind == TK_ARRAY &&
		b->val.type->kind == TK_ARRAY &&
		a->val.type->basetype->kind == TK_CHAR &&
		(!ISCONST(ex->kind) || !ISCONST(ex2->kind) ||
		 ex->val.i > ex2->val.i)) {
		ex = makeexpr_arglong(ex, (size_t_long != 0));
		ex2 = makeexpr_arglong(ex2, (size_t_long != 0));
		a = makeexpr_addrstr(a);
		b = makeexpr_addrstr(b);
		b = makeexpr_bicall_3("memcpy", a->val.type,
				      copyexpr(a), b, copyexpr(ex2));
		ex3 = copyexpr(ex2);
		return makeexpr_comma(b,
				      makeexpr_bicall_3("memset", a->val.type,
							makeexpr_plus(a, ex3),
							makeexpr_char(' '),
							makeexpr_minus(ex,
								       ex2)));
	    } else if (!(a->val.type->kind == TK_ARRAY &&
			 b->val.type->kind != TK_ARRAY))
		warning("Incompatible types or sizes [167]");
	}
        freeexpr(ex2);
        ex = makeexpr_arglong(ex, (size_t_long != 0));
        a = makeexpr_addrstr(a);
        b = makeexpr_addrstr(b);
        return makeexpr_bicall_3("memcpy", a->val.type, a, b, ex);
    }
    if (a->val.type->kind == TK_SET) {
        a = makeexpr_addrstr(a);
        b = makeexpr_addrstr(b);
        return makeexpr_bicall_2(setcopyname, a->val.type, a, b);
    }
    for (ep = &a; (ex3 = *ep); ) {
        if (ex3->kind == EK_COMMA)
            ep = &ex3->args[ex3->nargs-1];
        else if (ex3->kind == EK_CAST || ex3->kind == EK_ACTCAST)
            ep = &ex3->args[0];
        else
            break;
    }
    if (ex3->kind == EK_BICALL) {
        if (!strcmp(ex3->val.s, getbitsname)) {
	    tp = ex3->args[0]->val.type;
	    if (tp->kind == TK_ARRAY)
		ex3->args[0] = makeexpr_addr(ex3->args[0]);
            ex3->val.type = tp_void;
            if (checkconst(b, 0) && *clrbitsname) {
                strchange(&ex3->val.s, clrbitsname);
            } else if (*putbitsname &&
                       ((ISCONST(b->kind) &&
                         (b->val.i | ~((1 << (1 << tp->escale)) - 1)) == -1) ||
                        checkconst(b, (1 << (1 << tp->escale)) - 1))) {
                strchange(&ex3->val.s, putbitsname);
                insertarg(ep, 2, makeexpr_arglong(makeexpr_ord(b), 0));
            } else {
                b = makeexpr_arglong(makeexpr_ord(b), 0);
                if (*storebitsname) {
                    strchange(&ex3->val.s, storebitsname);
                    insertarg(ep, 2, b);
                } else {
                    if (exproccurs(b, ex3->args[0])) {
                        mp = makestmttempvar(b->val.type, name_TEMP);
                        ex2 = makeexpr_assign(makeexpr_var(mp), b);
                        b = makeexpr_var(mp);
                    } else
                        ex2 = NULL;
                    ex = copyexpr(ex3);
                    strchange(&ex3->val.s, putbitsname);
                    insertarg(&ex3, 2, b);
                    strchange(&ex->val.s, clrbitsname);
                    *ep = makeexpr_comma(ex2, makeexpr_comma(ex, ex3));
                }
            }
            return a;
        } else if (!strcmp(ex3->val.s, getfbufname)) {
	    ex3->val.type = tp_void;
	    strchange(&ex3->val.s, putfbufname);
	    insertarg(ep, 2, b);
	    return a;
        } else if (!strcmp(ex3->val.s, chargetfbufname)) {
	    ex3->val.type = tp_void;
	    if (*charputfbufname) {
		strchange(&ex3->val.s, charputfbufname);
		insertarg(ep, 1, b);
	    } else {
		strchange(&ex3->val.s, putfbufname);
		insertarg(ep, 1, makeexpr_type(ex3->val.type->basetype->basetype));
		insertarg(ep, 2, b);
	    }
	    return a;
        } else if (!strcmp(ex3->val.s, arraygetfbufname)) {
	    ex3->val.type = tp_void;
	    if (*arrayputfbufname) {
		strchange(&ex3->val.s, arrayputfbufname);
		insertarg(ep, 1, b);
	    } else {
		strchange(&ex3->val.s, putfbufname);
		insertarg(ep, 1, makeexpr_type(ex3->val.type->basetype->basetype));
		insertarg(ep, 2, b);
	    }
	    return a;
	}
    }
    while (a->kind == EK_CAST || a->kind == EK_ACTCAST ||
	   a->kind == EK_LITCAST) {
	if (a->kind == EK_LITCAST) {
	    b = makeexpr_cast(b, a->args[1]->val.type);
	    a = grabarg(a, 1);
	} else if (ansiC < 2 ||    /* in GNU C, a cast is an lvalue */
		   isarithkind(a->args[0]->kind) ||
		   (a->val.type->kind == TK_POINTER &&
		    a->args[0]->val.type->kind == TK_POINTER)) {
	    if (a->kind == EK_CAST)
		b = makeexpr_cast(b, a->args[0]->val.type);
	    else
		b = makeexpr_actcast(b, a->args[0]->val.type);
	    a = grabarg(a, 0);
	} else
	    break;
    }
    if (a->kind == EK_NEG)
	return makeexpr_assign(grabarg(a, 0), makeexpr_neg(b));
    if (a->kind == EK_NOT)
	return makeexpr_assign(grabarg(a, 0), makeexpr_not(b));
    if (a->kind == EK_BNOT)
	return makeexpr_assign(grabarg(a, 0),
			       makeexpr_un(EK_BNOT, b->val.type, b));
    if (a->kind == EK_PLUS) {
	for (i = 0; i < a->nargs && a->nargs > 1; ) {
	    if (isconstantexpr(a->args[i])) {
		b = makeexpr_minus(b, a->args[i]);
		deletearg(&a, i);
	    } else
		i++;
	}
	if (a->nargs == 1)
	    return makeexpr_assign(grabarg(a, 0), b);
    }
    if (a->kind == EK_TIMES) {
	for (i = 0; i < a->nargs && a->nargs > 1; ) {
	    if (isconstantexpr(a->args[i])) {
		if (a->val.type->kind == TK_REAL)
		    b = makeexpr_divide(b, a->args[i]);
		else {
		    if (ISCONST(b->kind) && ISCONST(a->args[i]->kind) &&
			(b->val.i % a->args[i]->val.i) != 0) {
			break;
		    }
		    b = makeexpr_div(b, a->args[i]);
		}
		deletearg(&a, i);
	    } else
		i++;
	}
	if (a->nargs == 1)
	    return makeexpr_assign(grabarg(a, 0), b);
    }
    if ((a->kind == EK_DIVIDE || a->kind == EK_DIV) &&
	 isconstantexpr(a->args[1])) {
	b = makeexpr_times(b, a->args[1]);
	return makeexpr_assign(a->args[0], b);
    }
    if (a->kind == EK_LSH && isconstantexpr(a->args[1])) {
	if (ISCONST(b->kind) && ISCONST(a->args[1]->kind)) {
	    if ((b->val.i & ((1L << a->args[1]->val.i)-1)) == 0) {
		b->val.i >>= a->args[1]->val.i;
		return makeexpr_assign(grabarg(a, 0), b);
	    }
	} else {
	    b = makeexpr_bin(EK_RSH, b->val.type, b, a->args[1]);
	    return makeexpr_assign(a->args[0], b);
	}
    }
    if (a->kind == EK_RSH && isconstantexpr(a->args[1])) {
	if (ISCONST(b->kind) && ISCONST(a->args[1]->kind))
	    b->val.i <<= a->args[1]->val.i;
	else
	    b = makeexpr_bin(EK_LSH, b->val.type, b, a->args[1]);
	return makeexpr_assign(a->args[0], b);
    }
    if (isarithkind(a->kind))
	warning("Invalid assignment [168]");
    return makeexpr_bin(EK_ASSIGN, a->val.type, a, makeexpr_unlongcast(b));
}




Expr *makeexpr_comma(a, b)
Expr *a, *b;
{
    Type *type;

    if (!a || nosideeffects(a, 1))
        return b;
    if (!b)
        return a;
    type = b->val.type;
    a = commute(a, b, EK_COMMA);
    a->val.type = type;
    return a;
}




int strmax(ex)
Expr *ex;
{
    Meaning *mp;
    long smin, smax;
    Value val;
    Type *type;

    type = ex->val.type;
    if (type->kind == TK_POINTER)
        type = type->basetype;
    if (type->kind == TK_CHAR)
        return 1;
    if (type->kind == TK_ARRAY && type->basetype->kind == TK_CHAR) {
        if (ord_range(type->indextype, &smin, &smax))
            return smax - smin + 1;
        else
            return stringceiling;
    }
    if (type->kind != TK_STRING) {
        intwarning("strmax", "strmax encountered a non-string value [169]");
        return stringceiling;
    }
    if (ex->kind == EK_CONST)
        return ex->val.i;
    if (ex->kind == EK_VAR && foldstrconsts != 0 &&
        (mp = (Meaning *)(ex->val.i))->kind == MK_CONST && mp->val.type)
        return mp->val.i;
    if (ex->kind == EK_BICALL) {
	if (!strcmp(ex->val.s, strsubname)) {
	    if (isliteralconst(ex->args[3], &val) && val.type)
		return val.i;
	}
    }
    if (ord_range(type->indextype, NULL, &smax))
        return smax;
    else
        return stringceiling;
}




int strhasnull(val)
Value val;
{
    int i;

    for (i = 0; i < val.i; i++) {
        if (!val.s[i])
            return (i == val.i-1) ? 1 : 2;
    }
    return 0;
}



int istempsprintf(ex)
Expr *ex;
{
    return (ex->kind == EK_BICALL && !strcmp(ex->val.s, "sprintf") &&
            ex->nargs >= 2 &&
            istempvar(ex->args[0]) &&
            ex->args[1]->kind == EK_CONST &&
            ex->args[1]->val.type->kind == TK_STRING);
}



Expr *makeexpr_sprintfify(ex)
Expr *ex;
{
    Meaning *tvar;
    char stringbuf[500];
    char *cp, ch;
    int j, nnulls;
    Expr *ex2;

    if (debug>2) { fprintf(outf,"makeexpr_sprintfify("); dumpexpr(ex); fprintf(outf,")\n"); }
    if (istempsprintf(ex))
        return ex;
    ex = makeexpr_stringcast(ex);
    tvar = makestmttempvar(tp_str255, name_STRING);
    if (ex->kind == EK_CONST && ex->val.type->kind == TK_STRING) {
        cp = stringbuf;
        nnulls = 0;
        for (j = 0; j < ex->val.i; j++) {
            ch = ex->val.s[j];
            if (!ch) {
                if (j < ex->val.i-1)
                    note("Null character in sprintf control string [147]");
                else
                    note("Null character at end of sprintf control string [148]");
                if (keepnulls) {
                    *cp++ = '%';
                    *cp++ = 'c';
                    nnulls++;
                }
            } else {
                *cp++ = ch;
                if (ch == '%')
                    *cp++ = ch;
            }
        }
        *cp = 0;
        ex = makeexpr_bicall_2("sprintf", tp_str255,
                               makeexpr_var(tvar),
                               makeexpr_string(stringbuf));
        while (--nnulls >= 0)
            insertarg(&ex, 2, makeexpr_char(0));
        return ex;
    } else if (ex->val.type->kind == TK_ARRAY &&
               ex->val.type->basetype->kind == TK_CHAR) {
        ex2 = arraysize(ex->val.type, 0);
        return cleansprintf(
                makeexpr_bicall_4("sprintf", tp_str255,
                                  makeexpr_var(tvar),
                                  makeexpr_string("%.*s"),
                                  ex2,
                                  makeexpr_addrstr(ex)));
    } else {
        if (ord_type(ex->val.type)->kind == TK_CHAR)
            cp = "%c";
        else if (ex->val.type->kind == TK_STRING)
            cp = "%s";
        else {
            warning("Mixing non-strings with strings [170]");
            return ex;
        }
        return makeexpr_bicall_3("sprintf", tp_str255,
                                 makeexpr_var(tvar),
                                 makeexpr_string(cp),
                                 ex);
    }
}



Expr *makeexpr_unsprintfify(ex)
Expr *ex;
{
    char stringbuf[500];
    char *cp, ch;
    int i;

    if (debug>2) { fprintf(outf,"makeexpr_unsprintfify("); dumpexpr(ex); fprintf(outf,")\n"); }
    if (!istempsprintf(ex))
        return ex;
    canceltempvar(istempvar(ex->args[0]));
    for (i = 2; i < ex->nargs; i++) {
        if (ex->args[i]->val.type->kind != TK_CHAR ||
            !checkconst(ex, 0))
            return ex;
    }
    cp = stringbuf;
    for (i = 0; i < ex->args[1]->val.i; i++) {
        ch = ex->args[1]->val.s[i];
        *cp++ = ch;
        if (ch == '%') {
            if (++i == ex->args[1]->val.i)
                return ex;
            ch = ex->args[1]->val.s[i];
            if (ch == 'c')
                cp[-1] = 0;
            else if (ch != '%')
                return ex;
        }
    }
    freeexpr(ex);
    return makeexpr_lstring(stringbuf, cp - stringbuf);
}



/* Returns >= 0 iff unsprintfify would return a string constant */

int sprintflength(ex, allownulls)
Expr *ex;
int allownulls;
{
    int i, len;

    if (!istempsprintf(ex))
        return -1;
    for (i = 2; i < ex->nargs; i++) {
        if (!allownulls ||
            ex->args[i]->val.type->kind != TK_CHAR ||
            !checkconst(ex, 0))
            return -1;
    }
    len = 0;
    for (i = 0; i < ex->args[1]->val.i; i++) {
        len++;
        if (ex->args[1]->val.s[i] == '%') {
            if (++i == ex->args[1]->val.i)
                return -1;
            if (ex->args[1]->val.s[i] != 'c' &&
                ex->args[1]->val.s[i] != '%')
                return -1;
        }
    }
    return len;
}



Expr *makeexpr_concat(a, b, usesprintf)
Expr *a, *b;
int usesprintf;
{
    int i, ii, j, len, nargs;
    Type *type;
    Meaning *mp, *tvar;
    Expr *ex, *args[2];
    int akind[2];
    Value val, val1, val2;
    char formatstr[300];

    if (debug>2) { fprintf(outf,"makeexpr_concat("); dumpexpr(a); fprintf(outf,", "); dumpexpr(b); fprintf(outf,")\n"); }
    if (!a)
        return b;
    if (!b)
        return a;
    a = makeexpr_stringcast(a);
    b = makeexpr_stringcast(b);
    if (checkconst(a, 0)) {
        freeexpr(a);
        return b;
    }
    if (checkconst(b, 0)) {
        freeexpr(b);
        return a;
    }
    len = strmax(a) + strmax(b);
    type = makestringtype(len);
    if (a->kind == EK_CONST && b->kind == EK_CONST) {
        val1 = a->val;
        val2 = b->val;
        val.i = val1.i + val2.i;
        val.s = ALLOC(val.i+1, char, literals);
	val.s[val.i] = 0;
        val.type = type;
        memcpy(val.s, val1.s, val1.i);
        memcpy(val.s + val1.i, val2.s, val2.i);
        freeexpr(a);
        freeexpr(b);
        return makeexpr_val(val);
    }
    tvar = makestmttempvar(type, name_STRING);
    if (sprintf_value != 2 || usesprintf) {
        nargs = 2;                 /* Generate a call to sprintf(), unfolding */
        args[0] = a;               /*  nested sprintf()'s. */
        args[1] = b;
        *formatstr = 0;
        for (i = 0; i < 2; i++) {
#if 1
            ex = args[i] = makeexpr_sprintfify(args[i]);
	    if (!ex->args[1] || !ex->args[1]->val.s)
		intwarning("makeexpr_concat", "NULL in ex->args[1]");
	    else
		strncat(formatstr, ex->args[1]->val.s, ex->args[1]->val.i);
            canceltempvar(istempvar(ex->args[0]));
            nargs += (ex->nargs - 2);
            akind[i] = 0;      /* now obsolete */
#else
            ex = args[i];
            if (ex->kind == EK_CONST)
                ex = makeexpr_sprintfify(ex);
            if (istempsprintf(ex)) {
                strncat(formatstr, ex->args[1]->val.s, ex->args[1]->val.i);
                canceltempvar(istempvar(ex->args[0]));
                nargs += (ex->nargs - 2);
                akind[i] = 0;
            } else {
                strcat(formatstr, "%s");
                nargs++;
                akind[i] = 1;
            }
#endif
        }
        ex = makeexpr(EK_BICALL, nargs);
        ex->val.type = type;
        ex->val.s = stralloc("sprintf");
        ex->args[0] = makeexpr_var(tvar);
        ex->args[1] = makeexpr_string(formatstr);
        j = 2;
        for (i = 0; i < 2; i++) {
            switch (akind[i]) {
                case 0:   /* flattened sub-sprintf */
                    for (ii = 2; ii < args[i]->nargs; ii++)
                        ex->args[j++] = copyexpr(args[i]->args[ii]);
                    freeexpr(args[i]);
                    break;
                case 1:   /* included string expr */
                    ex->args[j++] = args[i];
                    break;
            }
        }
    } else {
        ex = a;
        while (ex->kind == EK_BICALL && !strcmp(ex->val.s, "strcat"))
            ex = ex->args[0];
        if (ex->kind == EK_BICALL && !strcmp(ex->val.s, "strcpy") &&
            (mp = istempvar(ex->args[0])) != NULL) {
            canceltempvar(mp);
            freeexpr(ex->args[0]);
            ex->args[0] = makeexpr_var(tvar);
        } else {
            a = makeexpr_bicall_2("strcpy", type, makeexpr_var(tvar), a);
        }
        ex = makeexpr_bicall_2("strcat", type, a, b);
    }
    if (debug>2) { fprintf(outf,"makeexpr_concat returns "); dumpexpr(ex); fprintf(outf,"\n"); }
    return ex;
}



Expr *cleansprintf(ex)
Expr *ex;
{
    int fidx, i, j, k, len, changed = 0;
    char *cp, *bp;
    char fmtbuf[300];

    if (ex->kind != EK_BICALL)
	return ex;
    if (!strcmp(ex->val.s, "printf"))
	fidx = 0;
    else if (!strcmp(ex->val.s, "sprintf") ||
	     !strcmp(ex->val.s, "fprintf"))
	fidx = 1;
    else
	return ex;
    len = ex->args[fidx]->val.i;
    cp = ex->args[fidx]->val.s;      /* printf("%*d",17,x)  =>  printf("%17d",x) */
    bp = fmtbuf;
    j = fidx + 1;
    for (i = 0; i < len; i++) {
        *bp++ = cp[i];
        if (cp[i] == '%') {
	    if (cp[i+1] == 's' && ex->args[j]->kind == EK_CONST) {
		bp--;
		for (k = 0; k < ex->args[j]->val.i; k++)
		    *bp++ = ex->args[j]->val.s[k];
		delfreearg(&ex, j);
		changed = 1;
		i++;
		continue;
	    }
            for (i++; i < len &&
                      !(isalpha(cp[i]) && cp[i] != 'l'); i++) {
                if (cp[i] == '*') {
                    if (isliteralconst(ex->args[j], NULL) == 2) {
                        sprintf(bp, "%ld", ex->args[j]->val.i);
                        bp += strlen(bp);
                        delfreearg(&ex, j);
                        changed = 1;
                    } else {
                        *bp++ = cp[i];
                        j++;
                    }
                } else
                    *bp++ = cp[i];
            }
            if (i < len)
                *bp++ = cp[i];
            j++;
        }
    }
    *bp = 0;
    if (changed) {
        freeexpr(ex->args[fidx]);
        ex->args[fidx] = makeexpr_string(fmtbuf);
    }
    return ex;
}



Expr *makeexpr_substring(vex, ex, exi, exj)
Expr *vex, *ex, *exi, *exj;
{
    exi = makeexpr_unlongcast(exi);
    exj = makeexpr_longcast(exj, 0);
    ex = bumpstring(ex, exi, 1);
    return cleansprintf(makeexpr_bicall_4("sprintf", tp_str255,
                                          vex,
                                          makeexpr_string("%.*s"),
                                          exj,
                                          ex));
}




Expr *makeexpr_dot(ex, mp)
Expr *ex;
Meaning *mp;
{
    Type *ot1, *ot2;
    Expr *ex2, *ex3, *nex;
    Meaning *tvar;

    if (ex->kind == EK_FUNCTION && copystructfuncs > 0) {
        tvar = makestmttempvar(ex->val.type, name_TEMP);
        ex2 = makeexpr_assign(makeexpr_var(tvar), ex);
        ex = makeexpr_var(tvar);
    } else
        ex2 = NULL;
    if (mp->constdefn) {
        nex = makeexpr(EK_MACARG, 0);
        nex->val.type = tp_integer;
        ex3 = replaceexprexpr(copyexpr(mp->constdefn), nex, ex, 0);
        freeexpr(ex);
        freeexpr(nex);
        ex = gentle_cast(ex3, mp->val.type);
    } else {
        ex = makeexpr_un(EK_DOT, mp->type, ex);
        ex->val.i = (long)mp;
        ot1 = ord_type(mp->type);
        ot2 = ord_type(mp->val.type);
        if (ot1->kind != ot2->kind && ot2->kind == TK_ENUM && ot2->meaning && useenum)
            ex = makeexpr_cast(ex, mp->val.type);
        else if (mp->val.i && !hassignedchar &&
		 (mp->type == tp_sint || mp->type == tp_abyte)) {
            if (*signextname) {
                ex = makeexpr_bicall_2(signextname, tp_integer,
                                       ex, makeexpr_long(mp->val.i));
            } else
                note(format_s("Unable to sign-extend field %s [149]", mp->name));
        }
    }
    ex->val.type = mp->val.type;
    return makeexpr_comma(ex2, ex);
}



Expr *makeexpr_dotq(ex, name, type)
Expr *ex;
char *name;
Type *type;
{
    ex = makeexpr_un(EK_DOT, type, ex);
    ex->val.s = stralloc(name);
    return ex;
}



Expr *strmax_func(ex)
Expr *ex;
{
    Meaning *mp;
    Expr *ex2;
    Type *type;

    type = ex->val.type;
    if (type->kind == TK_POINTER) {
        intwarning("strmax_func", "got a pointer instead of a string [171]");
        type = type->basetype;
    }
    if (type->kind == TK_CHAR)
        return makeexpr_long(1);
    if (type->kind != TK_STRING) {
        warning("STRMAX of non-string value [172]");
        return makeexpr_long(stringceiling);
    }
    if (ex->kind == EK_CONST)
	return makeexpr_long(ex->val.i);
    if (ex->kind == EK_VAR &&
	(mp = (Meaning *)ex->val.i)->kind == MK_CONST &&
	mp->type == tp_str255 && mp->val.type)
	return makeexpr_long(mp->val.i);
    if (ex->kind == EK_VAR &&
        (mp = (Meaning *)ex->val.i)->kind == MK_VARPARAM &&
        mp->type == tp_strptr) {
	if (mp->anyvarflag) {
	    if (mp->ctx != curctx && mp->ctx->kind == MK_FUNCTION)
		note(format_s("Reference to STRMAX of parent proc's \"%s\" must be fixed [150]",
			      mp->name));
	    return makeexpr_name(format_s(name_STRMAX, mp->name), tp_int);
	} else
	    note(format_s("STRMAX of \"%s\" wants VarStrings=1 [151]", mp->name));
    }
    ord_range_expr(type->indextype, NULL, &ex2);
    return copyexpr(ex2);
}




Expr *makeexpr_nil()
{
    Expr *ex;

    ex = makeexpr(EK_CONST, 0);
    ex->val.type = tp_anyptr;
    ex->val.i = 0;
    ex->val.s = NULL;
    return ex;
}



Expr *makeexpr_ctx(ctx)
Meaning *ctx;
{
    Expr *ex;

    ex = makeexpr(EK_CTX, 0);
    ex->val.type = tp_text;     /* handy pointer type */
    ex->val.i = (long)ctx;
    return ex;
}




Expr *force_signed(ex)
Expr *ex;
{
    Type *tp;

    if (isliteralconst(ex, NULL) == 2 && ex->nargs == 0)
        return ex;
    tp = true_type(ex);
    if (tp == tp_ushort || tp == tp_ubyte || tp == tp_uchar)
	return makeexpr_cast(ex, tp_sshort);
    else if (tp == tp_unsigned || tp == tp_uint) {
	if (exprlongness(ex) < 0)
	    return makeexpr_cast(ex, tp_sint);
	else
	    return makeexpr_cast(ex, tp_integer);
    }
    return ex;
}



Expr *force_unsigned(ex)
Expr *ex;
{
    Type *tp;

    if (isliteralconst(ex, NULL) == 2 && !expr_is_neg(ex))
        return ex;
    tp = true_type(ex);
    if (tp == tp_unsigned || tp == tp_uint || tp == tp_ushort ||
	tp == tp_ubyte || tp == tp_uchar)
        return ex;
    if (tp->kind == TK_CHAR)
	return makeexpr_actcast(ex, tp_uchar);
    else if (exprlongness(ex) < 0)
        return makeexpr_cast(ex, tp_uint);
    else
        return makeexpr_cast(ex, tp_unsigned);
}




#define CHECKSIZE(size) (((size) > 0 && (size)%charsize == 0) ? (size)/charsize : 0)

long type_sizeof(type, pasc)
Type *type;
int pasc;
{
    long s1, smin, smax;
    int charsize = (sizeof_char) ? sizeof_char : CHAR_BIT;      /* from <limits.h> */

    switch (type->kind) {

        case TK_INTEGER:
            if (type == tp_integer ||
                type == tp_unsigned)
                return pasc ? 4 : CHECKSIZE(sizeof_integer);
            else
                return pasc ? 2 : CHECKSIZE(sizeof_short);

        case TK_CHAR:
        case TK_BOOLEAN:
            return 1;

        case TK_SUBR:
            type = findbasetype(type, ODECL_NOPRES);
            if (pasc) {
                if (type == tp_integer || type == tp_unsigned)
                    return 4;
                else
                    return 2;
            } else {
                if (type == tp_abyte || type == tp_ubyte || type == tp_sbyte)
                    return 1;
                else if (type == tp_ushort || type == tp_sshort)
                    return CHECKSIZE(sizeof_short);
                else
                    return CHECKSIZE(sizeof_integer);
            }

        case TK_POINTER:
            return pasc ? 4 : CHECKSIZE(sizeof_pointer);

        case TK_REAL:
	    if (type == tp_longreal)
		return pasc ? (which_lang == LANG_TURBO ? 6 : 8) : CHECKSIZE(sizeof_double);
	    else
		return pasc ? 4 : CHECKSIZE(sizeof_float);

        case TK_ENUM:
	    if (!pasc)
		return CHECKSIZE(sizeof_enum);
	    type = findbasetype(type, ODECL_NOPRES);
            return type->kind != TK_ENUM ? type_sizeof(type, pasc)
		   : CHECKSIZE(pascalenumsize);

        case TK_SMALLSET:
        case TK_SMALLARRAY:
            return pasc ? 0 : type_sizeof(type->basetype, pasc);

        case TK_ARRAY:
            s1 = type_sizeof(type->basetype, pasc);
            if (s1 && ord_range(type->indextype, &smin, &smax))
                return s1 * (smax - smin + 1);
            else
                return 0;

        case TK_RECORD:
            if (pasc && type->meaning) {
                if (!strcmp(type->meaning->sym->name, "NA_WORD"))
                    return 2;
                else if (!strcmp(type->meaning->sym->name, "NA_LONGWORD"))
                    return 4;
                else if (!strcmp(type->meaning->sym->name, "NA_QUADWORD"))
                    return 8;
                else
                    return 0;
            } else
                return 0;

        default:
            return 0;
    }
}



Static Value eval_expr_either(ex, pasc)
Expr *ex;
int pasc;
{
    Value val, val2;
    Meaning *mp;
    int i;

    if (debug>2) { fprintf(outf,"eval_expr("); dumpexpr(ex); fprintf(outf,")\n"); }
    switch (ex->kind) {

        case EK_CONST:
        case EK_LONGCONST:
            return ex->val;

        case EK_VAR:
            mp = (Meaning *) ex->val.i;
            if (mp->kind == MK_CONST &&
                (foldconsts != 0 ||
                 mp == mp_maxint || mp == mp_minint))
                return mp->val;
            break;

        case EK_SIZEOF:
            i = type_sizeof(ex->args[0]->val.type, pasc);
            if (i)
                return make_ord(tp_integer, i);
            break;

        case EK_PLUS:
            val = eval_expr_either(ex->args[0], pasc);
            if (!val.type || ord_type(val.type)->kind != TK_INTEGER)
                val.type = NULL;
            for (i = 1; val.type && i < ex->nargs; i++) {
                val2 = eval_expr_either(ex->args[i], pasc);
                if (!val2.type || ord_type(val2.type)->kind != TK_INTEGER)
                    val.type = NULL;
                else {
                    val.i += val2.i;
		    val.type = tp_integer;
		}
            }
            return val;

        case EK_TIMES:
            val = eval_expr_either(ex->args[0], pasc);
            if (!val.type || ord_type(val.type)->kind != TK_INTEGER)
                val.type = NULL;
            for (i = 1; val.type && i < ex->nargs; i++) {
                val2 = eval_expr_either(ex->args[i], pasc);
                if (!val2.type || ord_type(val2.type)->kind != TK_INTEGER)
                    val.type = NULL;
                else {
                    val.i *= val2.i;
		    val.type = tp_integer;
		}
            }
            return val;

        case EK_DIV:
            val = eval_expr_either(ex->args[0], pasc);
            val2 = eval_expr_either(ex->args[1], pasc);
            if (val.type && ord_type(val.type)->kind == TK_INTEGER &&
                val2.type && ord_type(val2.type)->kind == TK_INTEGER &&
		val2.i) {
                val.i /= val2.i;
		val.type = tp_integer;
                return val;
            }
            break;

        case EK_MOD:
            val = eval_expr_either(ex->args[0], pasc);
            val2 = eval_expr_either(ex->args[1], pasc);
            if (val.type && ord_type(val.type)->kind == TK_INTEGER &&
                val2.type && ord_type(val2.type)->kind == TK_INTEGER &&
		val2.i) {
                val.i %= val2.i;
		val.type = tp_integer;
                return val;
            }
            break;

        case EK_NEG:
            val = eval_expr_either(ex->args[0], pasc);
            if (val.type) {
                val.i = -val.i;
                return val;
            }
            break;

        case EK_LSH:
            val = eval_expr_either(ex->args[0], pasc);
            val2 = eval_expr_either(ex->args[1], pasc);
            if (val.type && val2.type) {
                val.i <<= val2.i;
                return val;
            }
            break;

        case EK_RSH:
            val = eval_expr_either(ex->args[0], pasc);
            val2 = eval_expr_either(ex->args[1], pasc);
            if (val.type && val2.type) {
                val.i >>= val2.i;
                return val;
            }
            break;

        case EK_BAND:
            val = eval_expr_either(ex->args[0], pasc);
            val2 = eval_expr_either(ex->args[1], pasc);
            if (val.type && val2.type) {
                val.i &= val2.i;
                return val;
            }
            break;

        case EK_BOR:
            val = eval_expr_either(ex->args[0], pasc);
            val2 = eval_expr_either(ex->args[1], pasc);
            if (val.type && val2.type) {
                val.i |= val2.i;
                return val;
            }
            break;

        case EK_BXOR:
            val = eval_expr_either(ex->args[0], pasc);
            val2 = eval_expr_either(ex->args[1], pasc);
            if (val.type && val2.type) {
                val.i ^= val2.i;
                return val;
            }
            break;

        case EK_BNOT:
            val = eval_expr_either(ex->args[0], pasc);
            if (val.type) {
                val.i = ~val.i;
                return val;
            }
            break;

        case EK_EQ:
        case EK_NE:
        case EK_GT:
        case EK_LT:
        case EK_GE:
        case EK_LE:
            val = eval_expr_either(ex->args[0], pasc);
            val2 = eval_expr_either(ex->args[1], pasc);
            if (val.type) {
                if (val.i == val2.i)
                    val.i = (ex->kind == EK_EQ || ex->kind == EK_GE || ex->kind == EK_LE);
                else if (val.i < val2.i)
                    val.i = (ex->kind == EK_LT || ex->kind == EK_LE || ex->kind == EK_NE);
                else
                    val.i = (ex->kind == EK_GT || ex->kind == EK_GE || ex->kind == EK_NE);
                val.type = tp_boolean;
                return val;
            }
            break;

        case EK_NOT:
            val = eval_expr_either(ex->args[0], pasc);
            if (val.type)
                val.i = !val.i;
            return val;

        case EK_AND:
            for (i = 0; i < ex->nargs; i++) {
                val = eval_expr_either(ex->args[i], pasc);
                if (!val.type || !val.i)
                    return val;
            }
            return val;

        case EK_OR:
            for (i = 0; i < ex->nargs; i++) {
                val = eval_expr_either(ex->args[i], pasc);
                if (!val.type || val.i)
                    return val;
            }
            return val;

        case EK_COMMA:
            return eval_expr_either(ex->args[ex->nargs-1], pasc);

	default:
	    break;
    }
    val.type = NULL;
    return val;
}


Value eval_expr(ex)
Expr *ex;
{
    return eval_expr_either(ex, 0);
}


Value eval_expr_consts(ex)
Expr *ex;
{
    Value val;
    short save_fold = foldconsts;

    foldconsts = 1;
    val = eval_expr_either(ex, 0);
    foldconsts = save_fold;
    return val;
}


Value eval_expr_pasc(ex)
Expr *ex;
{
    return eval_expr_either(ex, 1);
}



int expr_is_const(ex)
Expr *ex;
{
    int i;

    switch (ex->kind) {

        case EK_CONST:
        case EK_LONGCONST:
        case EK_SIZEOF:
            return 1;

        case EK_VAR:
            return (((Meaning *)ex->val.i)->kind == MK_CONST);

        case EK_HAT:
        case EK_ASSIGN:
        case EK_POSTINC:
        case EK_POSTDEC:
            return 0;

        case EK_ADDR:
            if (ex->args[0]->kind == EK_VAR)
                return 1;
            return 0;   /* conservative */

        case EK_FUNCTION:
            if (!nosideeffects_func(ex))
                return 0;
            break;

        case EK_BICALL:
            if (!nosideeffects_func(ex))
                return 0;
            break;

	default:
	    break;
    }
    for (i = 0; i < ex->nargs; i++) {
        if (!expr_is_const(ex->args[i]))
            return 0;
    }
    return 1;
}





Expr *eatcasts(ex)
Expr *ex;
{
    while (ex->kind == EK_CAST)
        ex = grabarg(ex, 0);
    return ex;
}





/* End. */



