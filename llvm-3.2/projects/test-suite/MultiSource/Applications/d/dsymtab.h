/*
  Copyright 2002-2003 John Plevyak, All Rights Reserved
*/
#ifndef _dsymtab_H_
#define _dsymtab_H_

#ifndef D_UserSym
#define D_UserSym unsigned int
#endif

struct D_SymHash;

typedef struct D_Sym {
  char		*name;
  int		len;
  unsigned int	hash;
  struct D_Sym	*update_of;
  struct D_Sym	*next;
  D_UserSym	user;
} D_Sym;

#define SCOPE_INHERIT			0
#define SCOPE_RECURSIVE			1
#define SCOPE_PARALLEL			2
#define SCOPE_SEQUENTIAL		3

typedef struct D_Scope {
  unsigned int		owned_by_user:1; /* don't automatically delete */
  unsigned int		kind:2;
  D_Sym		 	*ll;
  struct D_SymHash	*hash;
  D_Sym		 	*updates;
  struct D_Scope *search;       /* scope to start search */
  struct D_Scope *up;		/* enclosing scope */
  struct D_Scope *up_updates;	/* prior scope in speculative parse */
  struct D_Scope *down;		/* enclosed scopes (for FREE) */
  struct D_Scope *down_next;	/* next enclosed scope */
} D_Scope;

D_Scope *new_D_Scope(D_Scope *parent);
D_Scope *enter_D_Scope(D_Scope *current, D_Scope *scope);
D_Scope *commit_D_Scope(D_Scope *scope);
void free_D_Scope(D_Scope *st, int force);
D_Sym *new_D_Sym(D_Scope *st, char *name, char *end, int sizeof_D_Sym);
#define NEW_D_SYM(_st, _name, _end) new_D_Sym(_st, _name, _end, sizeof(D_Sym))
void free_D_Sym(D_Sym *sym);
D_Sym *find_D_Sym(D_Scope *st, char *name, char *end);
D_Sym *update_D_Sym(D_Scope *st, D_Sym *sym, int sizeof_D_Sym);
#define UPDATE_D_SYM(_st, _sym) update_D_Sym(_st, _sym, sizeof(D_Sym))
D_Sym *current_D_Sym(D_Scope *st, D_Sym *sym);
D_Sym *find_D_Sym_in_Scope(D_Scope *st, char *name, char *end);
void print_scope(D_Scope *st);

#endif
