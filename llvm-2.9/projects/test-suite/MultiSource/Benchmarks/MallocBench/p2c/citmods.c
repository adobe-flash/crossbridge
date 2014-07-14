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



#define PROTO_CITMODS_C
#include "trans.h"



/* The following functions define special translations for several
 * HP Pascal modules developed locally at Caltech.  For non-Caltech
 * readers this file will serve mainly as a body of examples.
 *
 * The FuncMacro mechanism (introduced after this file was written)
 * provides a simpler method for cases where the function translates
 * into some fixed C equivalent.
 */




/* NEWASM functions */


/* na_fillbyte: equivalent to memset, though convert_size is used to
 * generalize the size a bit:  na_fillbyte(a, 0, 80) where a is an array
 * of integers (4 bytes in HP Pascal) will be translated to
 * memset(a, 0, 20 * sizeof(int)).
 */

Static Stmt *proc_na_fillbyte(ex)
Expr *ex;
{
    ex->args[0] = gentle_cast(ex->args[0], tp_anyptr);
    ex->args[2] = convert_size(argbasetype(ex->args[0]), ex->args[2], "NA_FILLBYTE");
    return makestmt_call(makeexpr_bicall_3("memset", tp_void,
                                           ex->args[0],
                                           makeexpr_arglong(ex->args[1], 0),
                                           makeexpr_arglong(ex->args[2], (size_t_long != 0))));
}



/* This function fills with a 32-bit pattern.  If all four bytes of the
 * pattern are equal, memset is used, otherwise the na_fill call is
 * left unchanged.
 */

Static Stmt *proc_na_fill(ex)
Expr *ex;
{
    unsigned long ul;
    Symbol *sym;

    ex->args[0] = gentle_cast(ex->args[0], tp_anyptr);
    ex->args[2] = convert_size(argbasetype(ex->args[0]), ex->args[2], "NA_FILL");
    if (!strcmp(((Meaning *)ex->val.i)->sym->name, "NA_FILLP")) {
        sym = findsymbol("NA_FILL");
        if (sym->mbase)
            ex->val.i = (long)sym->mbase;
    }
    if (isliteralconst(ex->args[1], NULL) != 2)
        return makestmt_call(ex);
    ul = ex->args[1]->val.i;
    if ((((ul >> 16) ^ ul) & 0xffff) ||    /* all four bytes must be the same */
        (((ul >> 8) ^ ul) & 0xff))
        return makestmt_call(ex);
    ex->args[1]->val.i &= 0xff;
    return makestmt_call(makeexpr_bicall_3("memset", tp_void,
                                           ex->args[0],
                                           makeexpr_arglong(ex->args[1], 0),
                                           makeexpr_arglong(ex->args[2], (size_t_long != 0))));
}



Static Stmt *proc_na_move(ex)
Expr *ex;
{
    ex->args[0] = gentle_cast(ex->args[0], tp_anyptr);   /* source */
    ex->args[1] = gentle_cast(ex->args[1], tp_anyptr);   /* dest */
    ex->args[2] = convert_size(choosetype(argbasetype(ex->args[0]),
                                          argbasetype(ex->args[1])), ex->args[2], "NA_MOVE");
    return makestmt_call(makeexpr_bicall_3("memmove", tp_void,
                                           ex->args[1],
                                           ex->args[0],
                                           makeexpr_arglong(ex->args[2], (size_t_long != 0))));
}



/* This just generalizes the size and leaves the function call alone,
 * except that na_exchp (a version using pointer args) is transformed
 * to na_exch (a version using VAR args, equivalent in C).
 */

Static Stmt *proc_na_exch(ex)
Expr *ex;
{
    Symbol *sym;

    ex->args[0] = gentle_cast(ex->args[0], tp_anyptr);
    ex->args[1] = gentle_cast(ex->args[1], tp_anyptr);
    ex->args[2] = convert_size(choosetype(argbasetype(ex->args[0]),
                                          argbasetype(ex->args[1])), ex->args[2], "NA_EXCH");
    if (!strcmp(((Meaning *)ex->val.i)->sym->name, "NA_EXCHP")) {
        sym = findsymbol("NA_EXCH");
        if (sym->mbase)
            ex->val.i = (long)sym->mbase;
    }
    return makestmt_call(ex);
}



Static Expr *func_na_comp(ex)
Expr *ex;
{
    ex->args[0] = gentle_cast(ex->args[0], tp_anyptr);
    ex->args[1] = gentle_cast(ex->args[1], tp_anyptr);
    ex->args[2] = convert_size(choosetype(argbasetype(ex->args[0]),
                                          argbasetype(ex->args[1])), ex->args[2], "NA_COMP");
    return makeexpr_bicall_3("memcmp", tp_int,
                             ex->args[0],
                             ex->args[1],
                             makeexpr_arglong(ex->args[2], (size_t_long != 0)));
}



Static Expr *func_na_scaneq(ex)
Expr *ex;
{
    Symbol *sym;

    ex->args[0] = gentle_cast(ex->args[0], tp_anyptr);
    ex->args[2] = convert_size(argbasetype(ex->args[0]), ex->args[2], "NA_SCANEQ");
    if (!strcmp(((Meaning *)ex->val.i)->sym->name, "NA_SCANEQP")) {
        sym = findsymbol("NA_SCANEQ");
        if (sym->mbase)
            ex->val.i = (long)sym->mbase;
    }
    return ex;
}



Static Expr *func_na_scanne(ex)
Expr *ex;
{
    Symbol *sym;

    ex->args[0] = gentle_cast(ex->args[0], tp_anyptr);
    ex->args[2] = convert_size(argbasetype(ex->args[0]), ex->args[2], "NA_SCANNE");
    if (!strcmp(((Meaning *)ex->val.i)->sym->name, "NA_SCANNEP")) {
        sym = findsymbol("NA_SCANNE");
        if (sym->mbase)
            ex->val.i = (long)sym->mbase;
    }
    return ex;
}



Static Stmt *proc_na_new(ex)
Expr *ex;
{
    Expr *vex, *ex2, *sz = NULL;
    Stmt *sp;

    vex = makeexpr_hat(eatcasts(ex->args[0]), 0);
    ex2 = ex->args[1];
    if (vex->val.type->kind == TK_POINTER)
        ex2 = convert_size(vex->val.type->basetype, ex2, "NA_NEW");
    if (alloczeronil)
        sz = copyexpr(ex2);
    ex2 = makeexpr_bicall_1(mallocname, tp_anyptr, ex2);
    sp = makestmt_assign(copyexpr(vex), ex2);
    if (malloccheck) {
        sp = makestmt_seq(sp, makestmt_if(makeexpr_rel(EK_EQ, copyexpr(vex), makeexpr_nil()),
                                          makestmt_call(makeexpr_bicall_1(name_ESCAPE, tp_int, 
                                                                          makeexpr_long(-2))),
                                          NULL));
    }
    if (sz && !isconstantexpr(sz)) {
        if (alloczeronil == 2)
            note("Called NA_NEW with variable argument [500]");
        sp = makestmt_if(makeexpr_rel(EK_NE, sz, makeexpr_long(0)),
                         sp,
                         makestmt_assign(vex, makeexpr_nil()));
    } else
        freeexpr(vex);
    return sp;
}



Static Stmt *proc_na_dispose(ex)
Expr *ex;
{
    Stmt *sp;
    Expr *vex;

    vex = makeexpr_hat(eatcasts(ex->args[0]), 0);
    sp = makestmt_call(makeexpr_bicall_1(freename, tp_void, copyexpr(vex)));
    if (alloczeronil) {
        sp = makestmt_if(makeexpr_rel(EK_NE, vex, makeexpr_nil()),
                         sp, NULL);
    } else
        freeexpr(vex);
    return sp;
}



/* These functions provide functionality similar to alloca; we just warn
 * about them here since alloca would not have been portable enough for
 * our purposes anyway.
 */

Static Stmt *proc_na_alloc(ex)
Expr *ex;
{
    Expr *ex2;

    note("Call to NA_ALLOC [501]");
    ex->args[0] = eatcasts(ex->args[0]);
    ex2 = ex->args[0];
    if (ex2->val.type->kind == TK_POINTER &&
	ex2->val.type->basetype->kind == TK_POINTER)
        ex->args[1] = convert_size(ex2->val.type->basetype->basetype,
				   ex->args[1], "NA_ALLOC");
    return makestmt_call(ex);
}



Static Stmt *proc_na_outeralloc(ex)
Expr *ex;
{
    note("Call to NA_OUTERALLOC [502]");
    return makestmt_call(ex);
}



Static Stmt *proc_na_free(ex)
Expr *ex;
{
    note("Call to NA_FREE [503]");
    return makestmt_call(ex);
}




Static Expr *func_na_memavail(ex)
Expr *ex;
{
    freeexpr(ex);
    return makeexpr_bicall_0("memavail", tp_integer);
}




/* A simple collection of bitwise operations. */

Static Expr *func_na_and(ex)
Expr *ex;
{
    Expr *ex0, *ex1;

    ex0 = makeexpr_unlongcast(ex->args[0]);
    ex1 = makeexpr_unlongcast(ex->args[1]);
    return makeexpr_bin(EK_BAND, tp_integer, ex0, ex1);
}



Static Expr *func_na_bic(ex)
Expr *ex;
{
    Expr *ex0, *ex1;

    ex0 = makeexpr_unlongcast(ex->args[0]);
    ex1 = makeexpr_unlongcast(ex->args[1]);
    return makeexpr_bin(EK_BAND, tp_integer, 
                        ex0,
                        makeexpr_un(EK_BNOT, ex1->val.type, ex1));
}



Static Expr *func_na_or(ex)
Expr *ex;
{
    Expr *ex0, *ex1;

    ex0 = makeexpr_unlongcast(ex->args[0]);
    ex1 = makeexpr_unlongcast(ex->args[1]);
    return makeexpr_bin(EK_BOR, tp_integer, ex0, ex1);
}



Static Expr *func_na_xor(ex)
Expr *ex;
{
    Expr *ex0, *ex1;

    ex0 = makeexpr_unlongcast(ex->args[0]);
    ex1 = makeexpr_unlongcast(ex->args[1]);
    return makeexpr_bin(EK_BXOR, tp_integer, ex0, ex1);
}



Static Expr *func_na_not(ex)
Expr *ex;
{
    ex = makeexpr_unlongcast(grabarg(ex, 0));
    return makeexpr_un(EK_BNOT, ex->val.type, ex);
}



Static Expr *func_na_mask(ex)
Expr *ex;
{
    Expr *ex0, *ex1;

    ex0 = makeexpr_unlongcast(ex->args[0]);
    ex1 = makeexpr_unlongcast(ex->args[1]);
    ex = makeexpr_bin(EK_BAND, tp_integer, ex0, ex1);
    return makeexpr_rel(EK_NE, ex, makeexpr_long(0));
}



Static int check0_31(ex)
Expr *ex;
{
    if (isliteralconst(ex, NULL) == 2)
        return (ex->val.i >= 0 && ex->val.i <= 31);
    else
        return (assumebits != 0);
}



/* This function is defined to test a bit of an integer, returning false
 * if the bit number is out of range.  It is only safe to use C bitwise
 * ops if we can prove the bit number is always in range, or if the
 * user has asked us to assume that it is.  Lacking flow analysis,
 * we settle for checking constants only.
 */

Static Expr *func_na_test(ex)
Expr *ex;
{
    Expr *ex1;
    int longness;

    if (!check0_31(ex->args[0]))
        return ex;
    ex1 = makeexpr_unlongcast(ex->args[1]);
    longness = (exprlongness(ex1) != 0);
    return makeexpr_rel(EK_NE,
                        makeexpr_bin(EK_BAND, tp_integer,
                                     ex1,
                                     makeexpr_bin(EK_LSH, tp_integer,
                                                  makeexpr_longcast(makeexpr_long(1), longness),
                                                  makeexpr_unlongcast(ex->args[0]))),
                        makeexpr_long(0));
}



Static Stmt *proc_na_set(ex)
Expr *ex;
{
    Stmt *sp;
    Expr *vex;
    Meaning *tvar;

    if (!check0_31(ex->args[0]))
        return makestmt_call(ex);
    if (!nosideeffects(ex->args[1], 1)) {
        tvar = makestmttempvar(ex->args[1]->val.type, name_TEMP);
        sp = makestmt_assign(makeexpr_var(tvar), ex->args[1]);
        vex = makeexpr_hat(makeexpr_var(tvar), 0);
    } else {
        sp = NULL;
        vex = makeexpr_hat(ex->args[1], 0);
    }
    sp = makestmt_seq(sp,
                      makestmt_assign(vex,
                                      makeexpr_bin(EK_BOR, tp_integer,
                                                   copyexpr(vex),
                                                   makeexpr_bin(EK_LSH, tp_integer,
                                                                makeexpr_longcast(makeexpr_long(1), 1),
                                                                makeexpr_unlongcast(ex->args[0])))));
    return sp;
}



Static Stmt *proc_na_clear(ex)
Expr *ex;
{
    Stmt *sp;
    Expr *vex;
    Meaning *tvar;

    if (!check0_31(ex->args[0]))
        return makestmt_call(ex);
    if (!nosideeffects(ex->args[1], 1)) {
        tvar = makestmttempvar(ex->args[1]->val.type, name_TEMP);
        sp = makestmt_assign(makeexpr_var(tvar), ex->args[1]);
        vex = makeexpr_hat(makeexpr_var(tvar), 0);
    } else {
        sp = NULL;
        vex = makeexpr_hat(ex->args[1], 0);
    }
    sp = makestmt_seq(sp,
                      makestmt_assign(vex,
                                      makeexpr_bin(EK_BAND, tp_integer,
                                                   copyexpr(vex),
                                                   makeexpr_un(EK_BNOT, tp_integer,
                                                       makeexpr_bin(EK_LSH, tp_integer,
                                                                    makeexpr_longcast(makeexpr_long(1), 1),
                                                                    makeexpr_unlongcast(ex->args[0]))))));
    return sp;
}



Static Expr *func_na_po2(ex)
Expr *ex;
{
    if (!check0_31(ex->args[0]))
        return ex;
    return makeexpr_bin(EK_LSH, tp_integer,
                        makeexpr_longcast(makeexpr_long(1), 1),
                        makeexpr_unlongcast(grabarg(ex, 0)));
}



Static Expr *func_na_lobits(ex)
Expr *ex;
{
    if (!check0_31(ex->args[0]))
        return ex;
    return makeexpr_un(EK_BNOT, tp_integer,
                       makeexpr_bin(EK_LSH, tp_integer,
                                    makeexpr_longcast(makeexpr_long(-1), 1),
                                    makeexpr_unlongcast(grabarg(ex, 0))));
}



Static Expr *func_na_hibits(ex)
Expr *ex;
{
    if (!check0_31(ex->args[0]))
        return ex;
    return makeexpr_bin(EK_LSH, tp_integer,
                        makeexpr_longcast(makeexpr_long(-1), 1),
                        makeexpr_minus(makeexpr_long(32),
                                       makeexpr_unlongcast(grabarg(ex, 0))));
}



/* This function does an arithmetic shift left, or right for negative shift
 * count.  We translate into a C shift only if we are confident of the
 * sign of the shift count.
 */

Static Expr *func_na_asl(ex)
Expr *ex;
{
    Expr *ex2;

    ex2 = makeexpr_unlongcast(copyexpr(ex->args[0]));
    if (expr_is_neg(ex2)) {
        if (signedshift == 0 || signedshift == 2)
            return ex;
	if (possiblesigns(ex2) & 4) {
            if (assumesigns)
                note("Assuming count for NA_ASL is negative [504]");
            else
                return ex;
        }
        if (signedshift != 1)
            note("Assuming >> is an arithmetic shift [505]");
        return makeexpr_bin(EK_RSH, tp_integer,
			    grabarg(ex, 1), makeexpr_neg(ex2));
    } else {
	if (possiblesigns(ex2) & 1) {
            if (assumesigns)
                note("Assuming count for NA_ASL is positive [504]");
            else
                return ex;
        }
        return makeexpr_bin(EK_LSH, tp_integer, grabarg(ex, 1), ex2);
    }
}



Static Expr *func_na_lsl(ex)
Expr *ex;
{
    Expr *ex2;

    ex2 = makeexpr_unlongcast(copyexpr(ex->args[0]));
    if (expr_is_neg(ex2)) {
	if (possiblesigns(ex2) & 4) {
            if (assumesigns)
                note("Assuming count for NA_LSL is negative [506]");
            else
                return ex;
        }
        return makeexpr_bin(EK_RSH, tp_integer, 
                            force_unsigned(grabarg(ex, 1)),
			    makeexpr_neg(ex2));
    } else {
	if (possiblesigns(ex2) & 1) {
            if (assumesigns)
                note("Assuming count for NA_LSL is positive [506]");
            else
                return ex;
        }
        return makeexpr_bin(EK_LSH, tp_integer, grabarg(ex, 1), ex2);
    }
}



/* These bit-field operations were generalized slightly on the way to C;
 * they used to perform D &= S and now perform D = S1 & S2.
 */

Static Stmt *proc_na_bfand(ex)
Expr *ex;
{
    Stmt *sp;
    Meaning *tvar;

    if (!nosideeffects(ex->args[2], 1)) {
        tvar = makestmttempvar(ex->args[2]->val.type, name_TEMP);
        sp = makestmt_assign(makeexpr_var(tvar), ex->args[2]);
        ex->args[2] = makeexpr_var(tvar);
    } else
        sp = NULL;
    insertarg(&ex, 1, copyexpr(ex->args[2]));
    return makestmt_seq(sp, makestmt_call(ex));
}



Static Stmt *proc_na_bfbic(ex)
Expr *ex;
{
    return proc_na_bfand(ex);
}



Static Stmt *proc_na_bfor(ex)
Expr *ex;
{
    return proc_na_bfand(ex);
}



Static Stmt *proc_na_bfxor(ex)
Expr *ex;
{
    return proc_na_bfand(ex);
}



Static Expr *func_imin(ex)
Expr *ex;
{
    return makeexpr_bicall_2("P_imin2", tp_integer,
                             ex->args[0], ex->args[1]);
}



Static Expr *func_imax(ex)
Expr *ex;
{
    return makeexpr_bicall_2("P_imax2", tp_integer,
                             ex->args[0], ex->args[1]);
}



/* Unsigned non-overflowing arithmetic functions in Pascal; we translate
 * into plain arithmetic in C and assume C doesn't check for overflow.
 * (A valid assumption in the case when this was used.)
 */

Static Expr *func_na_add(ex)
Expr *ex;
{
    return makeexpr_plus(makeexpr_unlongcast(ex->args[0]),
                         makeexpr_unlongcast(ex->args[1]));
}



Static Expr *func_na_sub(ex)
Expr *ex;
{
    return makeexpr_minus(makeexpr_unlongcast(ex->args[0]),
                          makeexpr_unlongcast(ex->args[1]));
}



extern Stmt *proc_exit();    /* from funcs.c */

Static Stmt *proc_return()
{
    return proc_exit();
}



Static Expr *func_charupper(ex)
Expr *ex;
{
    return makeexpr_bicall_1("toupper", tp_char,
                             grabarg(ex, 0));
}



Static Expr *func_charlower(ex)
Expr *ex;
{
    return makeexpr_bicall_1("tolower", tp_char,
                             grabarg(ex, 0));
}



/* Convert an integer to its string representation.  We produce a sprintf
 * into a temporary variable; the temporary will probably be eliminated
 * as the surrounding code is translated.
 */

Static Expr *func_strint(ex)
Expr *ex;
{
    Expr *ex2;

    ex2 = makeexpr_forcelongness(ex->args[1]);
    return makeexpr_bicall_3("sprintf", ex->val.type,
                             ex->args[0],
                             makeexpr_string((exprlongness(ex2) > 0) ? "%ld" : "%d"),
                             ex2);
}



Static Expr *func_strint2(ex)
Expr *ex;
{
    Expr *ex2, *len, *fmt;

    if (checkconst(ex->args[2], 0) || checkconst(ex->args[2], 1))
        return func_strint(ex);
    if (expr_is_neg(ex->args[2])) {
	if (possiblesigns(ex->args[2]) & 4) {
            if (assumesigns)
                note("Assuming width for STRINT2 is negative [507]");
            else
                return ex;
        }
        ex2 = makeexpr_forcelongness(ex->args[1]);
        fmt = makeexpr_string((exprlongness(ex2) > 0) ? "%0*ld" : "%0*d");
        len = makeexpr_neg(makeexpr_longcast(ex->args[2], 0));
    } else {
	if (possiblesigns(ex->args[2]) & 1) {
            if (assumesigns)
                note("Assuming width for STRINT2 is positive [507]");
            else
                return ex;
        }
        ex2 = makeexpr_forcelongness(ex->args[1]);
        fmt = makeexpr_string((exprlongness(ex2) > 0) ? "%*ld" : "%*d");
        len = makeexpr_longcast(ex->args[2], 0);
    }
    ex = makeexpr_bicall_4("sprintf", ex->val.type,
                           ex->args[0], fmt, len, ex2);
    return cleansprintf(ex);
}



Static Expr *func_strhex(ex)
Expr *ex;
{
    Expr *ex2, *ex3;
    Value val;

    if (isliteralconst(ex->args[2], &val) == 2) {
        ex2 = makeexpr_forcelongness(ex->args[1]);
        if (val.i < 1 || val.i > 8) {
            ex = makeexpr_bicall_3("sprintf", ex->val.type,
                                   ex->args[0],
                                   makeexpr_string((exprlongness(ex2) > 0) ? "%lX" : "%X"),
                                   ex2);
        } else {
            if (val.i < 8) {
                ex3 = makeexpr_long((1 << (val.i*4)) - 1);
                insertarg(&ex3, 0, makeexpr_name("%#lx", tp_integer));
                ex2 = makeexpr_bin(EK_BAND, ex2->val.type, ex2, ex3);
            }
            ex = makeexpr_bicall_3("sprintf", ex->val.type,
                                   ex->args[0],
                                   makeexpr_string(format_d((exprlongness(ex2) > 0) ? "%%.%ldlX" :
                                                                                      "%%.%ldX",
                                                            val.i)),
                                   ex2);
        }
    }
    return ex;
}



Static Expr *func_strreal(ex)
Expr *ex;
{
    return makeexpr_bicall_3("sprintf", ex->val.type,
                             ex->args[0],
                             makeexpr_string("%g"),
                             ex->args[1]);
}



Static Expr *func_strchar(ex)
Expr *ex;
{
    return makeexpr_bicall_3("sprintf", ex->val.type,
                             ex->args[0],
                             makeexpr_string("%c"),
                             ex->args[1]);
}



Static Expr *func_strreadint(ex)
Expr *ex;
{
    return makeexpr_bicall_3("strtol", tp_integer,
                             grabarg(ex, 0), 
                             makeexpr_nil(),
                             makeexpr_long(0));
}



Static Expr *func_strreadreal(ex)
Expr *ex;
{
    return makeexpr_bicall_1("atof", tp_longreal,
                             grabarg(ex, 0));
}



Static Stmt *proc_strappendc(ex)
Expr *ex;
{
    Expr *ex2;

    ex2 = makeexpr_hat(ex->args[0], 0);
    return makestmt_assign(ex2, makeexpr_concat(copyexpr(ex2), ex->args[1], 0));
}



/* Check if a string begins with a given prefix; this is easy if the
 * prefix is known at compile-time.
 */

Static Expr *func_strbegins(ex)
Expr *ex;
{
    Expr *ex1, *ex2;

    ex1 = ex->args[0];
    ex2 = ex->args[1];
    if (ex2->kind == EK_CONST) {
        if (ex2->val.i == 1) {
            return makeexpr_rel(EK_EQ,
                                makeexpr_hat(ex1, 0),
                                makeexpr_char(ex2->val.s[0]));
        } else {
            return makeexpr_rel(EK_EQ,
                                makeexpr_bicall_3("strncmp", tp_int,
                                                  ex1,
                                                  ex2,
                                                  makeexpr_arglong(makeexpr_long(ex2->val.i), (size_t_long != 0))),
                                makeexpr_long(0));
        }
    }
    return ex;
}



Static Expr *func_strcontains(ex)
Expr *ex;
{
    return makeexpr_rel(EK_NE,
                        makeexpr_bicall_2("strpbrk", tp_strptr,
                                          ex->args[0],
                                          ex->args[1]),
                        makeexpr_nil());
}



/* Extract a substring of a string.  If arguments are out-of-range, extract
 * an empty or shorter substring.  Here, the length=infinity and constant
 * starting index cases are handled specially.
 */

Static Expr *func_strsub(ex)
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
    ex->args[2] = makeexpr_arglong(ex->args[2], 0);
    ex->args[3] = makeexpr_arglong(ex->args[3], 0);
    return ex;
}



Static Expr *func_strpart(ex)
Expr *ex;
{
    return func_strsub(ex);     /* all the special cases match */
}



Static Expr *func_strequal(ex)
Expr *ex;
{
    if (!*strcicmpname)
        return ex;
    return makeexpr_rel(EK_EQ, 
                        makeexpr_bicall_2(strcicmpname, tp_int,
                                          ex->args[0], ex->args[1]),
                        makeexpr_long(0));
}



Static Expr *func_strcmp(ex)
Expr *ex;
{
    return makeexpr_bicall_2("strcmp", tp_int, ex->args[0], ex->args[1]);
}



Static Expr *func_strljust(ex)
Expr *ex;
{
    return makeexpr_bicall_4("sprintf", ex->val.type,
                             ex->args[0],
                             makeexpr_string("%-*s"),
                             makeexpr_longcast(ex->args[2], 0),
                             ex->args[1]);
}



Static Expr *func_strrjust(ex)
Expr *ex;
{
    return makeexpr_bicall_4("sprintf", ex->val.type,
                             ex->args[0],
                             makeexpr_string("%*s"),
                             makeexpr_longcast(ex->args[2], 0),
                             ex->args[1]);
}




/* The procedure strnew(p,s) is converted into an assignment p = strdup(s). */

Static Stmt *proc_strnew(ex)
Expr *ex;
{
    return makestmt_assign(makeexpr_hat(ex->args[0], 0),
                           makeexpr_bicall_1("strdup", ex->args[1]->val.type,
                                             ex->args[1]));
}



/* These procedures are also changed to functions returning a result. */

Static Stmt *proc_strlist_add(ex)
Expr *ex;
{
    return makestmt_assign(makeexpr_hat(ex->args[1], 0),
                           makeexpr_bicall_2("strlist_add", ex->args[0]->val.type->basetype,
                                             ex->args[0],
                                             ex->args[2]));
}



Static Stmt *proc_strlist_append(ex)
Expr *ex;
{
    return makestmt_assign(makeexpr_hat(ex->args[1], 0),
                           makeexpr_bicall_2("strlist_append", ex->args[0]->val.type->basetype,
                                             ex->args[0],
                                             ex->args[2]));
}



Static Stmt *proc_strlist_insert(ex)
Expr *ex;
{
    return makestmt_assign(makeexpr_hat(ex->args[1], 0),
                           makeexpr_bicall_2("strlist_insert", ex->args[0]->val.type->basetype,
                                             ex->args[0],
                                             ex->args[2]));
}









/* NEWCI functions */


Static Stmt *proc_fixfname(ex)
Expr *ex;
{
    if (ex->args[1]->kind == EK_CONST)
	lwc(ex->args[1]->val.s);    /* Unix uses lower-case suffixes */
    return makestmt_call(ex);
}


Static Stmt *proc_forcefname(ex)
Expr *ex;
{
    return proc_fixfname(ex);
}


/* In Pascal these were variables of type pointer-to-text; we translate
 * them as, e.g., &stdin.  Note that even though &stdin is not legal in
 * many systems, in the common usage of writeln(stdin^) the & will
 * cancel out in a later stage of the translation.
 */

Static Expr *func_stdin()
{
    return makeexpr_addr(makeexpr_var(mp_input));
}


Static Expr *func_stdout()
{
    return makeexpr_addr(makeexpr_var(mp_output));
}


Static Expr *func_stderr()
{
    return makeexpr_addr(makeexpr_var(mp_stderr));
}








/* MYLIB functions */


Static Stmt *proc_m_color(ex)
Expr *ex;
{
    int i;
    long val;

    if (ex->kind == EK_PLUS) {
        for (i = 0; i < ex->nargs; i++) {
            if (isconstexpr(ex->args[i], &val)) {
                if (val > 0 && (val & 15) == 0) {
                    note("M_COLOR called with suspicious argument [508]");
                }
            }
        }
    } else if (ex->kind == EK_CONST) {
        if (ex->val.i >= 16 && ex->val.i < 255) {    /* accept true colors and m_trans */
            note("M_COLOR called with suspicious argument [508]");
        }
    }
    return makestmt_call(ex);
}







void citmods(name, defn)
char *name;
int defn;
{
    if (!strcmp(name, "NEWASM")) {
        makestandardproc("na_fillbyte", proc_na_fillbyte);
        makestandardproc("na_fill", proc_na_fill);
        makestandardproc("na_fillp", proc_na_fill);
        makestandardproc("na_move", proc_na_move);
        makestandardproc("na_movep", proc_na_move);
        makestandardproc("na_exch", proc_na_exch);
        makestandardproc("na_exchp", proc_na_exch);
        makestandardfunc("na_comp", func_na_comp);
        makestandardfunc("na_compp", func_na_comp);
        makestandardfunc("na_scaneq", func_na_scaneq);
        makestandardfunc("na_scaneqp", func_na_scaneq);
        makestandardfunc("na_scanne", func_na_scanne);
        makestandardfunc("na_scannep", func_na_scanne);
        makestandardproc("na_new", proc_na_new);
        makestandardproc("na_dispose", proc_na_dispose);
        makestandardproc("na_alloc", proc_na_alloc);
        makestandardproc("na_outeralloc", proc_na_outeralloc);
        makestandardproc("na_free", proc_na_free);
        makestandardfunc("na_memavail", func_na_memavail);
        makestandardfunc("na_and", func_na_and);
        makestandardfunc("na_bic", func_na_bic);
        makestandardfunc("na_or", func_na_or);
        makestandardfunc("na_xor", func_na_xor);
        makestandardfunc("na_not", func_na_not);
        makestandardfunc("na_mask", func_na_mask);
        makestandardfunc("na_test", func_na_test);
        makestandardproc("na_set", proc_na_set);
        makestandardproc("na_clear", proc_na_clear);
        makestandardfunc("na_po2", func_na_po2);
        makestandardfunc("na_hibits", func_na_hibits);
        makestandardfunc("na_lobits", func_na_lobits);
        makestandardfunc("na_asl", func_na_asl);
        makestandardfunc("na_lsl", func_na_lsl);
        makestandardproc("na_bfand", proc_na_bfand);
        makestandardproc("na_bfbic", proc_na_bfbic);
        makestandardproc("na_bfor", proc_na_bfor);
        makestandardproc("na_bfxor", proc_na_bfxor);
        makestandardfunc("imin", func_imin);
        makestandardfunc("imax", func_imax);
        makestandardfunc("na_add", func_na_add);
        makestandardfunc("na_sub", func_na_sub);
        makestandardproc("return", proc_return);
        makestandardfunc("charupper", func_charupper);
        makestandardfunc("charlower", func_charlower);
        makestandardfunc("strint", func_strint);
        makestandardfunc("strint2", func_strint2);
        makestandardfunc("strhex", func_strhex);
        makestandardfunc("strreal", func_strreal);
        makestandardfunc("strchar", func_strchar);
        makestandardfunc("strreadint", func_strreadint);
        makestandardfunc("strreadreal", func_strreadreal);
        makestandardproc("strappendc", proc_strappendc);
        makestandardfunc("strbegins", func_strbegins);
        makestandardfunc("strcontains", func_strcontains);
        makestandardfunc("strsub", func_strsub);
        makestandardfunc("strpart", func_strpart);
        makestandardfunc("strequal", func_strequal);
        makestandardfunc("strcmp", func_strcmp);
        makestandardfunc("strljust", func_strljust);
        makestandardfunc("strrjust", func_strrjust);
        makestandardproc("strnew", proc_strnew);
        makestandardproc("strlist_add", proc_strlist_add);
        makestandardproc("strlist_append", proc_strlist_append);
        makestandardproc("strlist_insert", proc_strlist_insert);
    } else if (!strcmp(name, "NEWCI")) {
	makestandardproc("fixfname", proc_fixfname);
	makestandardproc("forcefname", proc_forcefname);
        makestandardfunc("stdin", func_stdin);
        makestandardfunc("stdout", func_stdout);
        makestandardfunc("stderr", func_stderr);
    } else if (!strcmp(name, "MYLIB")) {
        makestandardproc("m_color", proc_m_color);
    }
}




/* End. */



