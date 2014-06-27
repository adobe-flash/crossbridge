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



#define PROTO_HPMODS_C
#include "trans.h"





/* FS functions */


Static Stmt *proc_freadbytes()
{
    Expr *ex, *ex2, *vex, *fex;
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
    skipcloseparen();
    type = vex->val.type;
    ex = makeexpr_bicall_4("fread", tp_integer,
                           makeexpr_addr(vex),
                           convert_size(type, ex2, "FREADBYTES"),
                           makeexpr_long(1),
                           filebasename(copyexpr(fex)));
    if (checkeof(fex)) {
        ex = makeexpr_bicall_2(name_SETIO, tp_void,
                               makeexpr_rel(EK_EQ, ex, makeexpr_long(1)),
                               makeexpr_long(30));
    }
    return wrapopencheck(makestmt_call(ex), fex);
}




Static Stmt *proc_fwritebytes()
{
    Expr *ex, *ex2, *vex, *fex;
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
    skipcloseparen();
    type = vex->val.type;
    ex = makeexpr_bicall_4("fwrite", tp_integer,
                           makeexpr_addr(vex),
                           convert_size(type, ex2, "FWRITEBYTES"),
                           makeexpr_long(1),
                           filebasename(copyexpr(fex)));
    if (checkfilewrite) {
        ex = makeexpr_bicall_2(name_SETIO, tp_void,
                               makeexpr_rel(EK_EQ, ex, makeexpr_long(1)),
                               makeexpr_long(3));
    }
    return wrapopencheck(makestmt_call(ex), fex);
}










/* SYSGLOBALS */


Static void setup_sysglobals()
{
    Symbol *sym;

    sym = findsymbol("SYSESCAPECODE");
    if (sym->mbase)
        strchange(&sym->mbase->name, name_ESCAPECODE);
    sym = findsymbol("SYSIORESULT");
    if (sym->mbase)
        strchange(&sym->mbase->name, name_IORESULT);
}








void hpmods(name, defn)
char *name;
int defn;
{
    if (!strcmp(name, "FS")) {
        makespecialproc("freadbytes", proc_freadbytes);
        makespecialproc("fwritebytes", proc_fwritebytes);
    } else if (!strcmp(name, "SYSGLOBALS")) {
        setup_sysglobals();
    }
}




/* End. */



