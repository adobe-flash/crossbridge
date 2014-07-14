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



#define define_parameters
#define PROTO_DIR_C
#include "trans.h"


/* This file is user-modifiable.  It is the "directory" of C functions
   for compiling in-line various Pascal library routines. */



extern void setup_module_hp();
extern void setup_module_cit();
extern void setup_module_tanner();




/* This function is called once when p2c is starting up, before
   the p2crc file has been read.
*/

void init_dir()
{


}





/* This function is called once when p2c is starting up, after
   the p2crc file has been read.
*/

void setup_dir()
{


}





/* This procedure is called after reading the import text for a module,
   where "name" is the module name, in upper-case letters.  Calls to
   "addmeaning", "makestandardfunc", etc. will annotate the context of
   the module.  Note that this will be called if the module is searched,
   even if it is never actually imported.
*/

#if 0
Static void _setup(name, defn)
char *name;
int defn;
{
    /* this is a dummy procedure which may be called by setup_module */
}
#endif

#define _setup(a,b)

void setup_module(name, defn)
char *name;
int defn;
{
    if (!strcicmp(name, "SYSTEM"))
	decl_builtins();
#ifdef CUST1
    CUST1(name, defn);
#endif
#ifdef CUST2
    CUST2(name, defn);
#endif
#ifdef CUST3
    CUST3(name, defn);
#endif
#ifdef CUST4
    CUST4(name, defn);
#endif
#ifdef CUST5
    CUST5(name, defn);
#endif
}





/* This procedure is called once after the p2crc file has been
   read and the built-in parameters have been "fixed".  It should
   check ranges and add defaults for any newly introduced parameters
   in the "rctable" (see "trans.h").
*/

void fix_parameters()
{


}





/* This function is called during a traversal of the tree of statements for
   a procedure.  Ordinarily it returns its argument; it may instead return
   an arbitrary other statement or sequence of statements, which will then
   be spliced in to replace the original one.  It may return NULL to delete
   the statement altogether.
*/

Stmt *fix_statement(sp)
Stmt *sp;
{
    return sp;
}





/* This is the analogous function for expression traversals.  It is
   called after the arguments have been (recursively) fixed and all
   built-in fixes have been performed.
*/

Expr *fix_expression(ex, env)
Expr *ex;
int env;
{
    return ex;
}





/* This procedure is called when fixing an expression of type
   EK_BICALL.  It is called before the arguments are fixed.  If
   it recognizes the BICALL, it should fix the arguments, then
   return a (possibly modified) fixed expression, which may or
   may not be a BICALL.  That expression will then be sent to
   fix_expression() as usual, but other standard fixes will not
   automatically be performed on it.  If the BICALL is not
   recognized, the function should return NULL.
*/

Expr *fix_bicall(ex, env)
Expr *ex;
int env;
{
    return NULL;
}





/* This function returns nonzero if the built-in function "name"
   should be written "if (f(x))" rather than "if (f(x) != 0)"
   when used as a boolean.  The call does *not* necessarily have
   to return a 1-or-0 value.
*/

int boolean_bicall(name)
char *name;
{
    return (!strcmp(name, "strcmp") ||
            !strcmp(name, "strncmp") ||
            !strcmp(name, "memcmp") ||
            !strcmp(name, "feof") ||
            !strcmp(name, "feoln"));
}





/* The function "name" promises not to change certain of its
   VAR-style parameters.  For each of arguments i = 0 through 15,
   if bit 1<<i of the return value of this function is set, and
   the i'th parameter is a pointer to an object, then the function
   guarantees not to change that object.
*/

unsigned int safemask_bicall(name)
char *name;
{
    Symbol *sp;

    sp = findsymbol_opt(name);
    if (sp) {
	if (sp->flags & (STRUCTF|STRLAPF))
	    return ~1;
	if (sp->flags & (NOSIDEEFF|DETERMF))
	    return ~0;
    }
    if (!strcmp(name, "fwrite") ||
        !strcmp(name, "memchr"))
        return 1;
    if (!strcmp(name, "memcpy") ||
        !strcmp(name, "memmove"))
        return 2;
    if (!strcmp(name, "memcmp"))
        return 3;
    if (!strcmp(name, "sprintf") ||
        !strcmp(name, "fprintf"))
        return ~1;
    if (!strcmp(name, "printf"))
        return ~0;
    return 0;
}





/* The function "name" has side effects that could affect other variables
   in the program besides those that are explicitly mentioned.
*/

int sideeffects_bicall(name)
char *name;
{
    return 0;
}






/* End. */



