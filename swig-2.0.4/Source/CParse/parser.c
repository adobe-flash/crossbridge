
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 16 "parser.y"


#define yylex yylex

char cvsroot_parser_y[] = "$Id: parser.y 12666 2011-05-14 00:13:43Z wsfulton $";

#include "swig.h"
#include "cparse.h"
#include "preprocessor.h"
#include <ctype.h>

/* We do this for portability */
#undef alloca
#define alloca malloc

/* -----------------------------------------------------------------------------
 *                               Externals
 * ----------------------------------------------------------------------------- */

int  yyparse();

/* NEW Variables */

static Node    *top = 0;      /* Top of the generated parse tree */
static int      unnamed = 0;  /* Unnamed datatype counter */
static Hash    *extendhash = 0;     /* Hash table of added methods */
static Hash    *classes = 0;        /* Hash table of classes */
static Symtab  *prev_symtab = 0;
static Node    *current_class = 0;
String  *ModuleName = 0;
static Node    *module_node = 0;
static String  *Classprefix = 0;  
static String  *Namespaceprefix = 0;
static int      inclass = 0;
static int      nested_template = 0; /* template class/function definition within a class */
static char    *last_cpptype = 0;
static int      inherit_list = 0;
static Parm    *template_parameters = 0;
static int      extendmode   = 0;
static int      compact_default_args = 0;
static int      template_reduce = 0;
static int      cparse_externc = 0;

static int      max_class_levels = 0;
static int      class_level = 0;
static Node   **class_decl = NULL;

/* -----------------------------------------------------------------------------
 *                            Assist Functions
 * ----------------------------------------------------------------------------- */


 
/* Called by the parser (yyparse) when an error is found.*/
static void yyerror (const char *e) {
  (void)e;
}

static Node *new_node(const_String_or_char_ptr tag) {
  Node *n = NewHash();
  set_nodeType(n,tag);
  Setfile(n,cparse_file);
  Setline(n,cparse_line);
  return n;
}

/* Copies a node.  Does not copy tree links or symbol table data (except for
   sym:name) */

static Node *copy_node(Node *n) {
  Node *nn;
  Iterator k;
  nn = NewHash();
  Setfile(nn,Getfile(n));
  Setline(nn,Getline(n));
  for (k = First(n); k.key; k = Next(k)) {
    String *ci;
    String *key = k.key;
    char *ckey = Char(key);
    if ((strcmp(ckey,"nextSibling") == 0) ||
	(strcmp(ckey,"previousSibling") == 0) ||
	(strcmp(ckey,"parentNode") == 0) ||
	(strcmp(ckey,"lastChild") == 0)) {
      continue;
    }
    if (Strncmp(key,"csym:",5) == 0) continue;
    /* We do copy sym:name.  For templates */
    if ((strcmp(ckey,"sym:name") == 0) || 
	(strcmp(ckey,"sym:weak") == 0) ||
	(strcmp(ckey,"sym:typename") == 0)) {
      String *ci = Copy(k.item);
      Setattr(nn,key, ci);
      Delete(ci);
      continue;
    }
    if (strcmp(ckey,"sym:symtab") == 0) {
      Setattr(nn,"sym:needs_symtab", "1");
    }
    /* We don't copy any other symbol table attributes */
    if (strncmp(ckey,"sym:",4) == 0) {
      continue;
    }
    /* If children.  We copy them recursively using this function */
    if (strcmp(ckey,"firstChild") == 0) {
      /* Copy children */
      Node *cn = k.item;
      while (cn) {
	Node *copy = copy_node(cn);
	appendChild(nn,copy);
	Delete(copy);
	cn = nextSibling(cn);
      }
      continue;
    }
    /* We don't copy the symbol table.  But we drop an attribute 
       requires_symtab so that functions know it needs to be built */

    if (strcmp(ckey,"symtab") == 0) {
      /* Node defined a symbol table. */
      Setattr(nn,"requires_symtab","1");
      continue;
    }
    /* Can't copy nodes */
    if (strcmp(ckey,"node") == 0) {
      continue;
    }
    if ((strcmp(ckey,"parms") == 0) || (strcmp(ckey,"pattern") == 0) || (strcmp(ckey,"throws") == 0)
	|| (strcmp(ckey,"kwargs") == 0)) {
      ParmList *pl = CopyParmList(k.item);
      Setattr(nn,key,pl);
      Delete(pl);
      continue;
    }
    /* Looks okay.  Just copy the data using Copy */
    ci = Copy(k.item);
    Setattr(nn, key, ci);
    Delete(ci);
  }
  return nn;
}

/* -----------------------------------------------------------------------------
 *                              Variables
 * ----------------------------------------------------------------------------- */

static char  *typemap_lang = 0;    /* Current language setting */

static int cplus_mode  = 0;
static String  *class_rename = 0;

/* C++ modes */

#define  CPLUS_PUBLIC    1
#define  CPLUS_PRIVATE   2
#define  CPLUS_PROTECTED 3

/* include types */
static int   import_mode = 0;

void SWIG_typemap_lang(const char *tm_lang) {
  typemap_lang = Swig_copy_string(tm_lang);
}

void SWIG_cparse_set_compact_default_args(int defargs) {
  compact_default_args = defargs;
}

int SWIG_cparse_template_reduce(int treduce) {
  template_reduce = treduce;
  return treduce;  
}

/* -----------------------------------------------------------------------------
 *                           Assist functions
 * ----------------------------------------------------------------------------- */

static int promote_type(int t) {
  if (t <= T_UCHAR || t == T_CHAR) return T_INT;
  return t;
}

/* Perform type-promotion for binary operators */
static int promote(int t1, int t2) {
  t1 = promote_type(t1);
  t2 = promote_type(t2);
  return t1 > t2 ? t1 : t2;
}

static String *yyrename = 0;

/* Forward renaming operator */

static String *resolve_node_scope(String *cname);


Hash *Swig_cparse_features(void) {
  static Hash   *features_hash = 0;
  if (!features_hash) features_hash = NewHash();
  return features_hash;
}

static String *feature_identifier_fix(String *s) {
  String *tp = SwigType_istemplate_templateprefix(s);
  if (tp) {
    String *ts, *ta, *tq;
    ts = SwigType_templatesuffix(s);
    ta = SwigType_templateargs(s);
    tq = Swig_symbol_type_qualify(ta,0);
    Append(tp,tq);
    Append(tp,ts);
    Delete(ts);
    Delete(ta);
    Delete(tq);
    return tp;
  } else {
    return NewString(s);
  }
}

/* Generate the symbol table name for an object */
/* This is a bit of a mess. Need to clean up */
static String *add_oldname = 0;



static String *make_name(Node *n, String *name,SwigType *decl) {
  int destructor = name && (*(Char(name)) == '~');

  if (yyrename) {
    String *s = NewString(yyrename);
    Delete(yyrename);
    yyrename = 0;
    if (destructor  && (*(Char(s)) != '~')) {
      Insert(s,0,"~");
    }
    return s;
  }

  if (!name) return 0;
  return Swig_name_make(n,Namespaceprefix,name,decl,add_oldname);
}

/* Generate an unnamed identifier */
static String *make_unnamed() {
  unnamed++;
  return NewStringf("$unnamed%d$",unnamed);
}

/* Return if the node is a friend declaration */
static int is_friend(Node *n) {
  return Cmp(Getattr(n,"storage"),"friend") == 0;
}

static int is_operator(String *name) {
  return Strncmp(name,"operator ", 9) == 0;
}


/* Add declaration list to symbol table */
static int  add_only_one = 0;

static void add_symbols(Node *n) {
  String *decl;
  String *wrn = 0;

  if (nested_template) {
    if (!(n && Equal(nodeType(n), "template"))) {
      return;
    }
    /* continue if template function, but not template class, declared within a class */
  }

  if (inclass && n) {
    cparse_normalize_void(n);
  }
  while (n) {
    String *symname = 0;
    /* for friends, we need to pop the scope once */
    String *old_prefix = 0;
    Symtab *old_scope = 0;
    int isfriend = inclass && is_friend(n);
    int iscdecl = Cmp(nodeType(n),"cdecl") == 0;
    int only_csymbol = 0;
    if (extendmode) {
      Setattr(n,"isextension","1");
    }
    
    if (inclass) {
      String *name = Getattr(n, "name");
      if (isfriend) {
	/* for friends, we need to add the scopename if needed */
	String *prefix = name ? Swig_scopename_prefix(name) : 0;
	old_prefix = Namespaceprefix;
	old_scope = Swig_symbol_popscope();
	Namespaceprefix = Swig_symbol_qualifiedscopename(0);
	if (!prefix) {
	  if (name && !is_operator(name) && Namespaceprefix) {
	    String *nname = NewStringf("%s::%s", Namespaceprefix, name);
	    Setattr(n,"name",nname);
	    Delete(nname);
	  }
	} else {
	  Symtab *st = Swig_symbol_getscope(prefix);
	  String *ns = st ? Getattr(st,"name") : prefix;
	  String *base  = Swig_scopename_last(name);
	  String *nname = NewStringf("%s::%s", ns, base);
	  Setattr(n,"name",nname);
	  Delete(nname);
	  Delete(base);
	  Delete(prefix);
	}
	Namespaceprefix = 0;
      } else {
	/* for member functions, we need to remove the redundant
	   class scope if provided, as in
	   
	   struct Foo {
	   int Foo::method(int a);
	   };
	   
	*/
	String *prefix = name ? Swig_scopename_prefix(name) : 0;
	if (prefix) {
	  if (Classprefix && (Equal(prefix,Classprefix))) {
	    String *base = Swig_scopename_last(name);
	    Setattr(n,"name",base);
	    Delete(base);
	  }
	  Delete(prefix);
	}

        /*
	if (!Getattr(n,"parentNode") && class_level) set_parentNode(n,class_decl[class_level - 1]);
        */
	Setattr(n,"ismember","1");
      }
    }
    if (!isfriend && inclass) {
      if ((cplus_mode != CPLUS_PUBLIC)) {
	only_csymbol = 1;
	if (cplus_mode == CPLUS_PROTECTED) {
	  Setattr(n,"access", "protected");
	  only_csymbol = !Swig_need_protected(n);
	} else {
	  Setattr(n,"access", "private");
	  /* private are needed only when they are pure virtuals - why? */
	  if ((Cmp(Getattr(n,"storage"),"virtual") == 0) && (Cmp(Getattr(n,"value"),"0") == 0)) {
	    only_csymbol = 0;
	  }
	}
      } else {
	  Setattr(n,"access", "public");
      }
    }
    if (Getattr(n,"sym:name")) {
      n = nextSibling(n);
      continue;
    }
    decl = Getattr(n,"decl");
    if (!SwigType_isfunction(decl)) {
      String *name = Getattr(n,"name");
      String *makename = Getattr(n,"parser:makename");
      if (iscdecl) {	
	String *storage = Getattr(n, "storage");
	if (Cmp(storage,"typedef") == 0) {
	  Setattr(n,"kind","typedef");
	} else {
	  SwigType *type = Getattr(n,"type");
	  String *value = Getattr(n,"value");
	  Setattr(n,"kind","variable");
	  if (value && Len(value)) {
	    Setattr(n,"hasvalue","1");
	  }
	  if (type) {
	    SwigType *ty;
	    SwigType *tmp = 0;
	    if (decl) {
	      ty = tmp = Copy(type);
	      SwigType_push(ty,decl);
	    } else {
	      ty = type;
	    }
	    if (!SwigType_ismutable(ty)) {
	      SetFlag(n,"hasconsttype");
	      SetFlag(n,"feature:immutable");
	    }
	    if (tmp) Delete(tmp);
	  }
	  if (!type) {
	    Printf(stderr,"notype name %s\n", name);
	  }
	}
      }
      Swig_features_get(Swig_cparse_features(), Namespaceprefix, name, 0, n);
      if (makename) {
	symname = make_name(n, makename,0);
        Delattr(n,"parser:makename"); /* temporary information, don't leave it hanging around */
      } else {
        makename = name;
	symname = make_name(n, makename,0);
      }
      
      if (!symname) {
	symname = Copy(Getattr(n,"unnamed"));
      }
      if (symname) {
	wrn = Swig_name_warning(n, Namespaceprefix, symname,0);
      }
    } else {
      String *name = Getattr(n,"name");
      SwigType *fdecl = Copy(decl);
      SwigType *fun = SwigType_pop_function(fdecl);
      if (iscdecl) {	
	Setattr(n,"kind","function");
      }
      
      Swig_features_get(Swig_cparse_features(),Namespaceprefix,name,fun,n);

      symname = make_name(n, name,fun);
      wrn = Swig_name_warning(n, Namespaceprefix,symname,fun);
      
      Delete(fdecl);
      Delete(fun);
      
    }
    if (!symname) {
      n = nextSibling(n);
      continue;
    }
    if (only_csymbol || GetFlag(n,"feature:ignore")) {
      /* Only add to C symbol table and continue */
      Swig_symbol_add(0, n);
    } else if (strncmp(Char(symname),"$ignore",7) == 0) {
      char *c = Char(symname)+7;
      SetFlag(n,"feature:ignore");
      if (strlen(c)) {
	SWIG_WARN_NODE_BEGIN(n);
	Swig_warning(0,Getfile(n), Getline(n), "%s\n",c+1);
	SWIG_WARN_NODE_END(n);
      }
      Swig_symbol_add(0, n);
    } else {
      Node *c;
      if ((wrn) && (Len(wrn))) {
	String *metaname = symname;
	if (!Getmeta(metaname,"already_warned")) {
	  SWIG_WARN_NODE_BEGIN(n);
	  Swig_warning(0,Getfile(n),Getline(n), "%s\n", wrn);
	  SWIG_WARN_NODE_END(n);
	  Setmeta(metaname,"already_warned","1");
	}
      }
      c = Swig_symbol_add(symname,n);

      if (c != n) {
        /* symbol conflict attempting to add in the new symbol */
        if (Getattr(n,"sym:weak")) {
          Setattr(n,"sym:name",symname);
        } else {
          String *e = NewStringEmpty();
          String *en = NewStringEmpty();
          String *ec = NewStringEmpty();
          int redefined = Swig_need_redefined_warn(n,c,inclass);
          if (redefined) {
            Printf(en,"Identifier '%s' redefined (ignored)",symname);
            Printf(ec,"previous definition of '%s'",symname);
          } else {
            Printf(en,"Redundant redeclaration of '%s'",symname);
            Printf(ec,"previous declaration of '%s'",symname);
          }
          if (Cmp(symname,Getattr(n,"name"))) {
            Printf(en," (Renamed from '%s')", SwigType_namestr(Getattr(n,"name")));
          }
          Printf(en,",");
          if (Cmp(symname,Getattr(c,"name"))) {
            Printf(ec," (Renamed from '%s')", SwigType_namestr(Getattr(c,"name")));
          }
          Printf(ec,".");
	  SWIG_WARN_NODE_BEGIN(n);
          if (redefined) {
            Swig_warning(WARN_PARSE_REDEFINED,Getfile(n),Getline(n),"%s\n",en);
            Swig_warning(WARN_PARSE_REDEFINED,Getfile(c),Getline(c),"%s\n",ec);
          } else if (!is_friend(n) && !is_friend(c)) {
            Swig_warning(WARN_PARSE_REDUNDANT,Getfile(n),Getline(n),"%s\n",en);
            Swig_warning(WARN_PARSE_REDUNDANT,Getfile(c),Getline(c),"%s\n",ec);
          }
	  SWIG_WARN_NODE_END(n);
          Printf(e,"%s:%d:%s\n%s:%d:%s\n",Getfile(n),Getline(n),en,
                 Getfile(c),Getline(c),ec);
          Setattr(n,"error",e);
	  Delete(e);
          Delete(en);
          Delete(ec);
        }
      }
    }
    /* restore the class scope if needed */
    if (isfriend) {
      Swig_symbol_setscope(old_scope);
      if (old_prefix) {
	Delete(Namespaceprefix);
	Namespaceprefix = old_prefix;
      }
    }
    Delete(symname);

    if (add_only_one) return;
    n = nextSibling(n);
  }
}


/* add symbols a parse tree node copy */

static void add_symbols_copy(Node *n) {
  String *name;
  int    emode = 0;
  while (n) {
    char *cnodeType = Char(nodeType(n));

    if (strcmp(cnodeType,"access") == 0) {
      String *kind = Getattr(n,"kind");
      if (Strcmp(kind,"public") == 0) {
	cplus_mode = CPLUS_PUBLIC;
      } else if (Strcmp(kind,"private") == 0) {
	cplus_mode = CPLUS_PRIVATE;
      } else if (Strcmp(kind,"protected") == 0) {
	cplus_mode = CPLUS_PROTECTED;
      }
      n = nextSibling(n);
      continue;
    }

    add_oldname = Getattr(n,"sym:name");
    if ((add_oldname) || (Getattr(n,"sym:needs_symtab"))) {
      int old_inclass = -1;
      Node *old_current_class = 0;
      if (add_oldname) {
	DohIncref(add_oldname);
	/*  Disable this, it prevents %rename to work with templates */
	/* If already renamed, we used that name  */
	/*
	if (Strcmp(add_oldname, Getattr(n,"name")) != 0) {
	  Delete(yyrename);
	  yyrename = Copy(add_oldname);
	}
	*/
      }
      Delattr(n,"sym:needs_symtab");
      Delattr(n,"sym:name");

      add_only_one = 1;
      add_symbols(n);

      if (Getattr(n,"partialargs")) {
	Swig_symbol_cadd(Getattr(n,"partialargs"),n);
      }
      add_only_one = 0;
      name = Getattr(n,"name");
      if (Getattr(n,"requires_symtab")) {
	Swig_symbol_newscope();
	Swig_symbol_setscopename(name);
	Delete(Namespaceprefix);
	Namespaceprefix = Swig_symbol_qualifiedscopename(0);
      }
      if (strcmp(cnodeType,"class") == 0) {
	old_inclass = inclass;
	inclass = 1;
	old_current_class = current_class;
	current_class = n;
	if (Strcmp(Getattr(n,"kind"),"class") == 0) {
	  cplus_mode = CPLUS_PRIVATE;
	} else {
	  cplus_mode = CPLUS_PUBLIC;
	}
      }
      if (strcmp(cnodeType,"extend") == 0) {
	emode = cplus_mode;
	cplus_mode = CPLUS_PUBLIC;
      }
      add_symbols_copy(firstChild(n));
      if (strcmp(cnodeType,"extend") == 0) {
	cplus_mode = emode;
      }
      if (Getattr(n,"requires_symtab")) {
	Setattr(n,"symtab", Swig_symbol_popscope());
	Delattr(n,"requires_symtab");
	Delete(Namespaceprefix);
	Namespaceprefix = Swig_symbol_qualifiedscopename(0);
      }
      if (add_oldname) {
	Delete(add_oldname);
	add_oldname = 0;
      }
      if (strcmp(cnodeType,"class") == 0) {
	inclass = old_inclass;
	current_class = old_current_class;
      }
    } else {
      if (strcmp(cnodeType,"extend") == 0) {
	emode = cplus_mode;
	cplus_mode = CPLUS_PUBLIC;
      }
      add_symbols_copy(firstChild(n));
      if (strcmp(cnodeType,"extend") == 0) {
	cplus_mode = emode;
      }
    }
    n = nextSibling(n);
  }
}

/* Extension merge.  This function is used to handle the %extend directive
   when it appears before a class definition.   To handle this, the %extend
   actually needs to take precedence.  Therefore, we will selectively nuke symbols
   from the current symbol table, replacing them with the added methods */

static void merge_extensions(Node *cls, Node *am) {
  Node *n;
  Node *csym;

  n = firstChild(am);
  while (n) {
    String *symname;
    if (Strcmp(nodeType(n),"constructor") == 0) {
      symname = Getattr(n,"sym:name");
      if (symname) {
	if (Strcmp(symname,Getattr(n,"name")) == 0) {
	  /* If the name and the sym:name of a constructor are the same,
             then it hasn't been renamed.  However---the name of the class
             itself might have been renamed so we need to do a consistency
             check here */
	  if (Getattr(cls,"sym:name")) {
	    Setattr(n,"sym:name", Getattr(cls,"sym:name"));
	  }
	}
      } 
    }

    symname = Getattr(n,"sym:name");
    DohIncref(symname);
    if ((symname) && (!Getattr(n,"error"))) {
      /* Remove node from its symbol table */
      Swig_symbol_remove(n);
      csym = Swig_symbol_add(symname,n);
      if (csym != n) {
	/* Conflict with previous definition.  Nuke previous definition */
	String *e = NewStringEmpty();
	String *en = NewStringEmpty();
	String *ec = NewStringEmpty();
	Printf(ec,"Identifier '%s' redefined by %%extend (ignored),",symname);
	Printf(en,"%%extend definition of '%s'.",symname);
	SWIG_WARN_NODE_BEGIN(n);
	Swig_warning(WARN_PARSE_REDEFINED,Getfile(csym),Getline(csym),"%s\n",ec);
	Swig_warning(WARN_PARSE_REDEFINED,Getfile(n),Getline(n),"%s\n",en);
	SWIG_WARN_NODE_END(n);
	Printf(e,"%s:%d:%s\n%s:%d:%s\n",Getfile(csym),Getline(csym),ec, 
	       Getfile(n),Getline(n),en);
	Setattr(csym,"error",e);
	Delete(e);
	Delete(en);
	Delete(ec);
	Swig_symbol_remove(csym);              /* Remove class definition */
	Swig_symbol_add(symname,n);            /* Insert extend definition */
      }
    }
    n = nextSibling(n);
  }
}

static void append_previous_extension(Node *cls, Node *am) {
  Node *n, *ne;
  Node *pe = 0;
  Node *ae = 0;

  if (!am) return;
  
  n = firstChild(am);
  while (n) {
    ne = nextSibling(n);
    set_nextSibling(n,0);
    /* typemaps and fragments need to be prepended */
    if (((Cmp(nodeType(n),"typemap") == 0) || (Cmp(nodeType(n),"fragment") == 0)))  {
      if (!pe) pe = new_node("extend");
      appendChild(pe, n);
    } else {
      if (!ae) ae = new_node("extend");
      appendChild(ae, n);
    }    
    n = ne;
  }
  if (pe) prependChild(cls,pe);
  if (ae) appendChild(cls,ae);
}
 

/* Check for unused %extend.  Special case, don't report unused
   extensions for templates */
 
static void check_extensions() {
  Iterator ki;

  if (!extendhash) return;
  for (ki = First(extendhash); ki.key; ki = Next(ki)) {
    if (!Strchr(ki.key,'<')) {
      SWIG_WARN_NODE_BEGIN(ki.item);
      Swig_warning(WARN_PARSE_EXTEND_UNDEF,Getfile(ki.item), Getline(ki.item), "%%extend defined for an undeclared class %s.\n", ki.key);
      SWIG_WARN_NODE_END(ki.item);
    }
  }
}

/* Check a set of declarations to see if any are pure-abstract */

static List *pure_abstract(Node *n) {
  List *abs = 0;
  while (n) {
    if (Cmp(nodeType(n),"cdecl") == 0) {
      String *decl = Getattr(n,"decl");
      if (SwigType_isfunction(decl)) {
	String *init = Getattr(n,"value");
	if (Cmp(init,"0") == 0) {
	  if (!abs) {
	    abs = NewList();
	  }
	  Append(abs,n);
	  Setattr(n,"abstract","1");
	}
      }
    } else if (Cmp(nodeType(n),"destructor") == 0) {
      if (Cmp(Getattr(n,"value"),"0") == 0) {
	if (!abs) {
	  abs = NewList();
	}
	Append(abs,n);
	Setattr(n,"abstract","1");
      }
    }
    n = nextSibling(n);
  }
  return abs;
}

/* Make a classname */

static String *make_class_name(String *name) {
  String *nname = 0;
  String *prefix;
  if (Namespaceprefix) {
    nname= NewStringf("%s::%s", Namespaceprefix, name);
  } else {
    nname = NewString(name);
  }
  prefix = SwigType_istemplate_templateprefix(nname);
  if (prefix) {
    String *args, *qargs;
    args   = SwigType_templateargs(nname);
    qargs  = Swig_symbol_type_qualify(args,0);
    Append(prefix,qargs);
    Delete(nname);
    Delete(args);
    Delete(qargs);
    nname = prefix;
  }
  return nname;
}

static List *make_inherit_list(String *clsname, List *names) {
  int i, ilen;
  String *derived;
  List *bases = NewList();

  if (Namespaceprefix) derived = NewStringf("%s::%s", Namespaceprefix,clsname);
  else derived = NewString(clsname);

  ilen = Len(names);
  for (i = 0; i < ilen; i++) {
    Node *s;
    String *base;
    String *n = Getitem(names,i);
    /* Try to figure out where this symbol is */
    s = Swig_symbol_clookup(n,0);
    if (s) {
      while (s && (Strcmp(nodeType(s),"class") != 0)) {
	/* Not a class.  Could be a typedef though. */
	String *storage = Getattr(s,"storage");
	if (storage && (Strcmp(storage,"typedef") == 0)) {
	  String *nn = Getattr(s,"type");
	  s = Swig_symbol_clookup(nn,Getattr(s,"sym:symtab"));
	} else {
	  break;
	}
      }
      if (s && ((Strcmp(nodeType(s),"class") == 0) || (Strcmp(nodeType(s),"template") == 0))) {
	String *q = Swig_symbol_qualified(s);
	Append(bases,s);
	if (q) {
	  base = NewStringf("%s::%s", q, Getattr(s,"name"));
	  Delete(q);
	} else {
	  base = NewString(Getattr(s,"name"));
	}
      } else {
	base = NewString(n);
      }
    } else {
      base = NewString(n);
    }
    if (base) {
      Swig_name_inherit(base,derived);
      Delete(base);
    }
  }
  return bases;
}

/* If the class name is qualified.  We need to create or lookup namespace entries */

static Symtab *set_scope_to_global() {
  Symtab *symtab = Swig_symbol_global_scope();
  Swig_symbol_setscope(symtab);
  return symtab;
}
 
/* Remove the block braces, { and }, if the 'noblock' attribute is set.
 * Node *kw can be either a Hash or Parmlist. */
static String *remove_block(Node *kw, const String *inputcode) {
  String *modified_code = 0;
  while (kw) {
   String *name = Getattr(kw,"name");
   if (name && (Cmp(name,"noblock") == 0)) {
     char *cstr = Char(inputcode);
     size_t len = Len(inputcode);
     if (len && cstr[0] == '{') {
       --len; ++cstr; 
       if (len && cstr[len - 1] == '}') { --len; }
       /* we now remove the extra spaces */
       while (len && isspace((int)cstr[0])) { --len; ++cstr; }
       while (len && isspace((int)cstr[len - 1])) { --len; }
       modified_code = NewStringWithSize(cstr, len);
       break;
     }
   }
   kw = nextSibling(kw);
  }
  return modified_code;
}


static Node *nscope = 0;
static Node *nscope_inner = 0;

/* Remove the scope prefix from cname and return the base name without the prefix.
 * The scopes specified in the prefix are found, or created in the current namespace.
 * So ultimately the scope is changed to that required for the base name.
 * For example AA::BB::CC as input returns CC and creates the namespace AA then inner 
 * namespace BB in the current scope. If no scope separator (::) in the input, then nothing happens! */
static String *resolve_node_scope(String *cname) {
  Symtab *gscope = 0;
  nscope = 0;
  nscope_inner = 0;  
  if (Swig_scopename_check(cname)) {
    Node   *ns;
    String *prefix = Swig_scopename_prefix(cname);
    String *base = Swig_scopename_last(cname);
    if (prefix && (Strncmp(prefix,"::",2) == 0)) {
      /* Use the global scope */
      String *nprefix = NewString(Char(prefix)+2);
      Delete(prefix);
      prefix= nprefix;
      gscope = set_scope_to_global();
    }    
    if (!prefix || (Len(prefix) == 0)) {
      /* Use the global scope, but we need to add a 'global' namespace.  */
      if (!gscope) gscope = set_scope_to_global();
      /* note that this namespace is not the "unnamed" one,
	 and we don't use Setattr(nscope,"name", ""),
	 because the unnamed namespace is private */
      nscope = new_node("namespace");
      Setattr(nscope,"symtab", gscope);;
      nscope_inner = nscope;
      return base;
    }
    /* Try to locate the scope */
    ns = Swig_symbol_clookup(prefix,0);
    if (!ns) {
      Swig_error(cparse_file,cparse_line,"Undefined scope '%s'\n", prefix);
    } else {
      Symtab *nstab = Getattr(ns,"symtab");
      if (!nstab) {
	Swig_error(cparse_file,cparse_line,
		   "'%s' is not defined as a valid scope.\n", prefix);
	ns = 0;
      } else {
	/* Check if the node scope is the current scope */
	String *tname = Swig_symbol_qualifiedscopename(0);
	String *nname = Swig_symbol_qualifiedscopename(nstab);
	if (tname && (Strcmp(tname,nname) == 0)) {
	  ns = 0;
	  cname = base;
	}
	Delete(tname);
	Delete(nname);
      }
      if (ns) {
	/* we will try to create a new node using the namespaces we
	   can find in the scope name */
	List *scopes;
	String *sname;
	Iterator si;
	String *name = NewString(prefix);
	scopes = NewList();
	while (name) {
	  String *base = Swig_scopename_last(name);
	  String *tprefix = Swig_scopename_prefix(name);
	  Insert(scopes,0,base);
	  Delete(base);
	  Delete(name);
	  name = tprefix;
	}
	for (si = First(scopes); si.item; si = Next(si)) {
	  Node *ns1,*ns2;
	  sname = si.item;
	  ns1 = Swig_symbol_clookup(sname,0);
	  assert(ns1);
	  if (Strcmp(nodeType(ns1),"namespace") == 0) {
	    if (Getattr(ns1,"alias")) {
	      ns1 = Getattr(ns1,"namespace");
	    }
	  } else {
	    /* now this last part is a class */
	    si = Next(si);
	    ns1 = Swig_symbol_clookup(sname,0);
	    /*  or a nested class tree, which is unrolled here */
	    for (; si.item; si = Next(si)) {
	      if (si.item) {
		Printf(sname,"::%s",si.item);
	      }
	    }
	    /* we get the 'inner' class */
	    nscope_inner = Swig_symbol_clookup(sname,0);
	    /* set the scope to the inner class */
	    Swig_symbol_setscope(Getattr(nscope_inner,"symtab"));
	    /* save the last namespace prefix */
	    Delete(Namespaceprefix);
	    Namespaceprefix = Swig_symbol_qualifiedscopename(0);
	    /* and return the node name, including the inner class prefix */
	    break;
	  }
	  /* here we just populate the namespace tree as usual */
	  ns2 = new_node("namespace");
	  Setattr(ns2,"name",sname);
	  Setattr(ns2,"symtab", Getattr(ns1,"symtab"));
	  add_symbols(ns2);
	  Swig_symbol_setscope(Getattr(ns1,"symtab"));
	  Delete(Namespaceprefix);
	  Namespaceprefix = Swig_symbol_qualifiedscopename(0);
	  if (nscope_inner) {
	    if (Getattr(nscope_inner,"symtab") != Getattr(ns2,"symtab")) {
	      appendChild(nscope_inner,ns2);
	      Delete(ns2);
	    }
	  }
	  nscope_inner = ns2;
	  if (!nscope) nscope = ns2;
	}
	cname = base;
	Delete(scopes);
      }
    }
    Delete(prefix);
  }
  return cname;
}
 


/* Structures for handling code fragments built for nested classes */

typedef struct Nested {
  String   *code;        /* Associated code fragment */
  int      line;         /* line number where it starts */
  const char *name;      /* Name associated with this nested class */
  const char *kind;      /* Kind of class */
  int      unnamed;      /* unnamed class */
  SwigType *type;        /* Datatype associated with the name */
  struct Nested   *next; /* Next code fragment in list */
} Nested;

/* Some internal variables for saving nested class information */

static Nested      *nested_list = 0;

/* Add a function to the nested list */

static void add_nested(Nested *n) {
  if (!nested_list) {
    nested_list = n;
  } else {
    Nested *n1 = nested_list;
    while (n1->next)
      n1 = n1->next;
    n1->next = n;
  }
}

/* -----------------------------------------------------------------------------
 * nested_new_struct()
 *
 * Nested struct handling for C code only creates a global struct from the nested struct.
 *
 * Nested structure. This is a sick "hack". If we encounter
 * a nested structure, we're going to grab the text of its definition and
 * feed it back into the scanner.  In the meantime, we need to grab
 * variable declaration information and generate the associated wrapper
 * code later.  Yikes!
 *
 * This really only works in a limited sense.   Since we use the
 * code attached to the nested class to generate both C code
 * it can't have any SWIG directives in it.  It also needs to be parsable
 * by SWIG or this whole thing is going to puke.
 * ----------------------------------------------------------------------------- */

static void nested_new_struct(const char *kind, String *struct_code, Node *cpp_opt_declarators) {
  String *name;
  String *decl;

  /* Create a new global struct declaration which is just a copy of the nested struct */
  Nested *nested = (Nested *) malloc(sizeof(Nested));
  Nested *n = nested;

  name = Getattr(cpp_opt_declarators, "name");
  decl = Getattr(cpp_opt_declarators, "decl");

  n->code = NewStringEmpty();
  Printv(n->code, "typedef ", kind, " ", struct_code, " $classname_", name, ";\n", NIL);
  n->name = Swig_copy_string(Char(name));
  n->line = cparse_start_line;
  n->type = NewStringEmpty();
  n->kind = kind;
  n->unnamed = 0;
  SwigType_push(n->type, decl);
  n->next = 0;

  /* Repeat for any multiple instances of the nested struct */
  {
    Node *p = cpp_opt_declarators;
    p = nextSibling(p);
    while (p) {
      Nested *nn = (Nested *) malloc(sizeof(Nested));

      name = Getattr(p, "name");
      decl = Getattr(p, "decl");

      nn->code = NewStringEmpty();
      Printv(nn->code, "typedef ", kind, " ", struct_code, " $classname_", name, ";\n", NIL);
      nn->name = Swig_copy_string(Char(name));
      nn->line = cparse_start_line;
      nn->type = NewStringEmpty();
      nn->kind = kind;
      nn->unnamed = 0;
      SwigType_push(nn->type, decl);
      nn->next = 0;
      n->next = nn;
      n = nn;
      p = nextSibling(p);
    }
  }

  add_nested(nested);
}

/* -----------------------------------------------------------------------------
 * nested_forward_declaration()
 * 
 * Nested struct handling for C++ code only.
 *
 * Treat the nested class/struct/union as a forward declaration until a proper 
 * nested class solution is implemented.
 * ----------------------------------------------------------------------------- */

static Node *nested_forward_declaration(const char *storage, const char *kind, String *sname, const char *name, Node *cpp_opt_declarators) {
  Node *nn = 0;
  int warned = 0;

  if (sname) {
    /* Add forward declaration of the nested type */
    Node *n = new_node("classforward");
    Setfile(n, cparse_file);
    Setline(n, cparse_line);
    Setattr(n, "kind", kind);
    Setattr(n, "name", sname);
    Setattr(n, "storage", storage);
    Setattr(n, "sym:weak", "1");
    add_symbols(n);
    nn = n;
  }

  /* Add any variable instances. Also add in any further typedefs of the nested type.
     Note that anonymous typedefs (eg typedef struct {...} a, b;) are treated as class forward declarations */
  if (cpp_opt_declarators) {
    int storage_typedef = (storage && (strcmp(storage, "typedef") == 0));
    int variable_of_anonymous_type = !sname && !storage_typedef;
    if (!variable_of_anonymous_type) {
      int anonymous_typedef = !sname && (storage && (strcmp(storage, "typedef") == 0));
      Node *n = cpp_opt_declarators;
      SwigType *type = NewString(name);
      while (n) {
	Setattr(n, "type", type);
	Setattr(n, "storage", storage);
	if (anonymous_typedef) {
	  Setattr(n, "nodeType", "classforward");
	  Setattr(n, "sym:weak", "1");
	}
	n = nextSibling(n);
      }
      Delete(type);
      add_symbols(cpp_opt_declarators);

      if (nn) {
	set_nextSibling(nn, cpp_opt_declarators);
      } else {
	nn = cpp_opt_declarators;
      }
    }
  }

  if (nn && Equal(nodeType(nn), "classforward")) {
    Node *n = nn;
    if (GetFlag(n, "feature:nestedworkaround")) {
      Swig_symbol_remove(n);
      nn = 0;
      warned = 1;
    } else {
      SWIG_WARN_NODE_BEGIN(n);
      Swig_warning(WARN_PARSE_NAMED_NESTED_CLASS, cparse_file, cparse_line,"Nested %s not currently supported (%s ignored)\n", kind, sname ? sname : name);
      SWIG_WARN_NODE_END(n);
      warned = 1;
    }
  }

  if (!warned)
    Swig_warning(WARN_PARSE_UNNAMED_NESTED_CLASS, cparse_file, cparse_line, "Nested %s not currently supported (ignored).\n", kind);

  return nn;
}

/* Strips C-style and C++-style comments from string in-place. */
static void strip_comments(char *string) {
  int state = 0; /* 
                  * 0 - not in comment
                  * 1 - in c-style comment
                  * 2 - in c++-style comment
                  * 3 - in string
                  * 4 - after reading / not in comments
                  * 5 - after reading * in c-style comments
                  * 6 - after reading \ in strings
                  */
  char * c = string;
  while (*c) {
    switch (state) {
    case 0:
      if (*c == '\"')
        state = 3;
      else if (*c == '/')
        state = 4;
      break;
    case 1:
      if (*c == '*')
        state = 5;
      *c = ' ';
      break;
    case 2:
      if (*c == '\n')
        state = 0;
      else
        *c = ' ';
      break;
    case 3:
      if (*c == '\"')
        state = 0;
      else if (*c == '\\')
        state = 6;
      break;
    case 4:
      if (*c == '/') {
        *(c-1) = ' ';
        *c = ' ';
        state = 2;
      } else if (*c == '*') {
        *(c-1) = ' ';
        *c = ' ';
        state = 1;
      } else
        state = 0;
      break;
    case 5:
      if (*c == '/')
        state = 0;
      else 
        state = 1;
      *c = ' ';
      break;
    case 6:
      state = 3;
      break;
    }
    ++c;
  }
}

/* Dump all of the nested class declarations to the inline processor
 * However.  We need to do a few name replacements and other munging
 * first.  This function must be called before closing a class! */

static Node *dump_nested(const char *parent) {
  Nested *n,*n1;
  Node *ret = 0;
  Node *last = 0;
  n = nested_list;
  if (!parent) {
    nested_list = 0;
    return 0;
  }
  while (n) {
    Node *retx;
    SwigType *nt;
    /* Token replace the name of the parent class */
    Replace(n->code, "$classname", parent, DOH_REPLACE_ANY);

    /* Fix up the name of the datatype (for building typedefs and other stuff) */
    Append(n->type,parent);
    Append(n->type,"_");
    Append(n->type,n->name);

    /* Add the appropriate declaration to the C++ processor */
    retx = new_node("cdecl");
    Setattr(retx,"name",n->name);
    nt = Copy(n->type);
    Setattr(retx,"type",nt);
    Delete(nt);
    Setattr(retx,"nested",parent);
    if (n->unnamed) {
      Setattr(retx,"unnamed","1");
    }
    
    add_symbols(retx);
    if (ret) {
      set_nextSibling(last, retx);
      Delete(retx);
    } else {
      ret = retx;
    }
    last = retx;

    /* Strip comments - further code may break in presence of comments. */
    strip_comments(Char(n->code));

    /* Make all SWIG created typedef structs/unions/classes unnamed else 
       redefinition errors occur - nasty hack alert.*/

    {
      const char* types_array[3] = {"struct", "union", "class"};
      int i;
      for (i=0; i<3; i++) {
	char* code_ptr = Char(n->code);
	while (code_ptr) {
	  /* Replace struct name (as in 'struct name {...}' ) with whitespace
	     name will be between struct and opening brace */
	
	  code_ptr = strstr(code_ptr, types_array[i]);
	  if (code_ptr) {
	    char *open_bracket_pos;
	    code_ptr += strlen(types_array[i]);
	    open_bracket_pos = strchr(code_ptr, '{');
	    if (open_bracket_pos) { 
	      /* Make sure we don't have something like struct A a; */
	      char* semi_colon_pos = strchr(code_ptr, ';');
	      if (!(semi_colon_pos && (semi_colon_pos < open_bracket_pos)))
		while (code_ptr < open_bracket_pos)
		  *code_ptr++ = ' ';
	    }
	  }
	}
      }
    }
    
    {
      /* Remove SWIG directive %constant which may be left in the SWIG created typedefs */
      char* code_ptr = Char(n->code);
      while (code_ptr) {
	code_ptr = strstr(code_ptr, "%constant");
	if (code_ptr) {
	  char* directive_end_pos = strchr(code_ptr, ';');
	  if (directive_end_pos) { 
            while (code_ptr <= directive_end_pos)
              *code_ptr++ = ' ';
	  }
	}
      }
    }
    {
      Node *newnode = new_node("insert");
      String *code = NewStringEmpty();
      Wrapper_pretty_print(n->code, code);
      Setattr(newnode,"code", code);
      Delete(code);
      set_nextSibling(last, newnode);
      Delete(newnode);      
      last = newnode;
    }
      
    /* Dump the code to the scanner */
    start_inline(Char(Getattr(last, "code")),n->line);

    n1 = n->next;
    Delete(n->code);
    free(n);
    n = n1;
  }
  nested_list = 0;
  return ret;
}

Node *Swig_cparse(File *f) {
  scanner_file(f);
  top = 0;
  yyparse();
  return top;
}

static void single_new_feature(const char *featurename, String *val, Hash *featureattribs, char *declaratorid, SwigType *type, ParmList *declaratorparms, String *qualifier) {
  String *fname;
  String *name;
  String *fixname;
  SwigType *t = Copy(type);

  /* Printf(stdout, "single_new_feature: [%s] [%s] [%s] [%s] [%s] [%s]\n", featurename, val, declaratorid, t, ParmList_str_defaultargs(declaratorparms), qualifier); */

  fname = NewStringf("feature:%s",featurename);
  if (declaratorid) {
    fixname = feature_identifier_fix(declaratorid);
  } else {
    fixname = NewStringEmpty();
  }
  if (Namespaceprefix) {
    name = NewStringf("%s::%s",Namespaceprefix, fixname);
  } else {
    name = fixname;
  }

  if (declaratorparms) Setmeta(val,"parms",declaratorparms);
  if (!Len(t)) t = 0;
  if (t) {
    if (qualifier) SwigType_push(t,qualifier);
    if (SwigType_isfunction(t)) {
      SwigType *decl = SwigType_pop_function(t);
      if (SwigType_ispointer(t)) {
	String *nname = NewStringf("*%s",name);
	Swig_feature_set(Swig_cparse_features(), nname, decl, fname, val, featureattribs);
	Delete(nname);
      } else {
	Swig_feature_set(Swig_cparse_features(), name, decl, fname, val, featureattribs);
      }
      Delete(decl);
    } else if (SwigType_ispointer(t)) {
      String *nname = NewStringf("*%s",name);
      Swig_feature_set(Swig_cparse_features(),nname,0,fname,val, featureattribs);
      Delete(nname);
    }
  } else {
    /* Global feature, that is, feature not associated with any particular symbol */
    Swig_feature_set(Swig_cparse_features(),name,0,fname,val, featureattribs);
  }
  Delete(fname);
  Delete(name);
}

/* Add a new feature to the Hash. Additional features are added if the feature has a parameter list (declaratorparms)
 * and one or more of the parameters have a default argument. An extra feature is added for each defaulted parameter,
 * simulating the equivalent overloaded method. */
static void new_feature(const char *featurename, String *val, Hash *featureattribs, char *declaratorid, SwigType *type, ParmList *declaratorparms, String *qualifier) {

  ParmList *declparms = declaratorparms;

  /* remove the { and } braces if the noblock attribute is set */
  String *newval = remove_block(featureattribs, val);
  val = newval ? newval : val;

  /* Add the feature */
  single_new_feature(featurename, val, featureattribs, declaratorid, type, declaratorparms, qualifier);

  /* Add extra features if there are default parameters in the parameter list */
  if (type) {
    while (declparms) {
      if (ParmList_has_defaultargs(declparms)) {

        /* Create a parameter list for the new feature by copying all
           but the last (defaulted) parameter */
        ParmList* newparms = CopyParmListMax(declparms, ParmList_len(declparms)-1);

        /* Create new declaration - with the last parameter removed */
        SwigType *newtype = Copy(type);
        Delete(SwigType_pop_function(newtype)); /* remove the old parameter list from newtype */
        SwigType_add_function(newtype,newparms);

        single_new_feature(featurename, Copy(val), featureattribs, declaratorid, newtype, newparms, qualifier);
        declparms = newparms;
      } else {
        declparms = 0;
      }
    }
  }
}

/* check if a function declaration is a plain C object */
static int is_cfunction(Node *n) {
  if (!cparse_cplusplus || cparse_externc) return 1;
  if (Cmp(Getattr(n,"storage"),"externc") == 0) {
    return 1;
  }
  return 0;
}

/* If the Node is a function with parameters, check to see if any of the parameters
 * have default arguments. If so create a new function for each defaulted argument. 
 * The additional functions form a linked list of nodes with the head being the original Node n. */
static void default_arguments(Node *n) {
  Node *function = n;

  if (function) {
    ParmList *varargs = Getattr(function,"feature:varargs");
    if (varargs) {
      /* Handles the %varargs directive by looking for "feature:varargs" and 
       * substituting ... with an alternative set of arguments.  */
      Parm     *p = Getattr(function,"parms");
      Parm     *pp = 0;
      while (p) {
	SwigType *t = Getattr(p,"type");
	if (Strcmp(t,"v(...)") == 0) {
	  if (pp) {
	    ParmList *cv = Copy(varargs);
	    set_nextSibling(pp,cv);
	    Delete(cv);
	  } else {
	    ParmList *cv =  Copy(varargs);
	    Setattr(function,"parms", cv);
	    Delete(cv);
	  }
	  break;
	}
	pp = p;
	p = nextSibling(p);
      }
    }

    /* Do not add in functions if kwargs is being used or if user wants old default argument wrapping
       (one wrapped method per function irrespective of number of default arguments) */
    if (compact_default_args 
	|| is_cfunction(function) 
	|| GetFlag(function,"feature:compactdefaultargs") 
	|| GetFlag(function,"feature:kwargs")) {
      ParmList *p = Getattr(function,"parms");
      if (p) 
        Setattr(p,"compactdefargs", "1"); /* mark parameters for special handling */
      function = 0; /* don't add in extra methods */
    }
  }

  while (function) {
    ParmList *parms = Getattr(function,"parms");
    if (ParmList_has_defaultargs(parms)) {

      /* Create a parameter list for the new function by copying all
         but the last (defaulted) parameter */
      ParmList* newparms = CopyParmListMax(parms,ParmList_len(parms)-1);

      /* Create new function and add to symbol table */
      {
	SwigType *ntype = Copy(nodeType(function));
	char *cntype = Char(ntype);
        Node *new_function = new_node(ntype);
        SwigType *decl = Copy(Getattr(function,"decl"));
        int constqualifier = SwigType_isconst(decl);
	String *ccode = Copy(Getattr(function,"code"));
	String *cstorage = Copy(Getattr(function,"storage"));
	String *cvalue = Copy(Getattr(function,"value"));
	SwigType *ctype = Copy(Getattr(function,"type"));
	String *cthrow = Copy(Getattr(function,"throw"));

        Delete(SwigType_pop_function(decl)); /* remove the old parameter list from decl */
        SwigType_add_function(decl,newparms);
        if (constqualifier)
          SwigType_add_qualifier(decl,"const");

        Setattr(new_function,"name", Getattr(function,"name"));
        Setattr(new_function,"code", ccode);
        Setattr(new_function,"decl", decl);
        Setattr(new_function,"parms", newparms);
        Setattr(new_function,"storage", cstorage);
        Setattr(new_function,"value", cvalue);
        Setattr(new_function,"type", ctype);
        Setattr(new_function,"throw", cthrow);

	Delete(ccode);
	Delete(cstorage);
	Delete(cvalue);
	Delete(ctype);
	Delete(cthrow);
	Delete(decl);

        {
          Node *throws = Getattr(function,"throws");
	  ParmList *pl = CopyParmList(throws);
          if (throws) Setattr(new_function,"throws",pl);
	  Delete(pl);
        }

        /* copy specific attributes for global (or in a namespace) template functions - these are not templated class methods */
        if (strcmp(cntype,"template") == 0) {
          Node *templatetype = Getattr(function,"templatetype");
          Node *symtypename = Getattr(function,"sym:typename");
          Parm *templateparms = Getattr(function,"templateparms");
          if (templatetype) {
	    Node *tmp = Copy(templatetype);
	    Setattr(new_function,"templatetype",tmp);
	    Delete(tmp);
	  }
          if (symtypename) {
	    Node *tmp = Copy(symtypename);
	    Setattr(new_function,"sym:typename",tmp);
	    Delete(tmp);
	  }
          if (templateparms) {
	    Parm *tmp = CopyParmList(templateparms);
	    Setattr(new_function,"templateparms",tmp);
	    Delete(tmp);
	  }
        } else if (strcmp(cntype,"constructor") == 0) {
          /* only copied for constructors as this is not a user defined feature - it is hard coded in the parser */
          if (GetFlag(function,"feature:new")) SetFlag(new_function,"feature:new");
        }

        add_symbols(new_function);
        /* mark added functions as ones with overloaded parameters and point to the parsed method */
        Setattr(new_function,"defaultargs", n);

        /* Point to the new function, extending the linked list */
        set_nextSibling(function, new_function);
	Delete(new_function);
        function = new_function;
	
	Delete(ntype);
      }
    } else {
      function = 0;
    }
  }
}

/* -----------------------------------------------------------------------------
 * tag_nodes()
 *
 * Used by the parser to mark subtypes with extra information.
 * ----------------------------------------------------------------------------- */

static void tag_nodes(Node *n, const_String_or_char_ptr attrname, DOH *value) {
  while (n) {
    Setattr(n, attrname, value);
    tag_nodes(firstChild(n), attrname, value);
    n = nextSibling(n);
  }
}



/* Line 189 of yacc.c  */
#line 1650 "y.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     ID = 258,
     HBLOCK = 259,
     POUND = 260,
     STRING = 261,
     INCLUDE = 262,
     IMPORT = 263,
     INSERT = 264,
     CHARCONST = 265,
     NUM_INT = 266,
     NUM_FLOAT = 267,
     NUM_UNSIGNED = 268,
     NUM_LONG = 269,
     NUM_ULONG = 270,
     NUM_LONGLONG = 271,
     NUM_ULONGLONG = 272,
     NUM_BOOL = 273,
     TYPEDEF = 274,
     TYPE_INT = 275,
     TYPE_UNSIGNED = 276,
     TYPE_SHORT = 277,
     TYPE_LONG = 278,
     TYPE_FLOAT = 279,
     TYPE_DOUBLE = 280,
     TYPE_CHAR = 281,
     TYPE_WCHAR = 282,
     TYPE_VOID = 283,
     TYPE_SIGNED = 284,
     TYPE_BOOL = 285,
     TYPE_COMPLEX = 286,
     TYPE_TYPEDEF = 287,
     TYPE_RAW = 288,
     TYPE_NON_ISO_INT8 = 289,
     TYPE_NON_ISO_INT16 = 290,
     TYPE_NON_ISO_INT32 = 291,
     TYPE_NON_ISO_INT64 = 292,
     LPAREN = 293,
     RPAREN = 294,
     COMMA = 295,
     SEMI = 296,
     EXTERN = 297,
     INIT = 298,
     LBRACE = 299,
     RBRACE = 300,
     PERIOD = 301,
     CONST_QUAL = 302,
     VOLATILE = 303,
     REGISTER = 304,
     STRUCT = 305,
     UNION = 306,
     EQUAL = 307,
     SIZEOF = 308,
     MODULE = 309,
     LBRACKET = 310,
     RBRACKET = 311,
     ILLEGAL = 312,
     CONSTANT = 313,
     NAME = 314,
     RENAME = 315,
     NAMEWARN = 316,
     EXTEND = 317,
     PRAGMA = 318,
     FEATURE = 319,
     VARARGS = 320,
     ENUM = 321,
     CLASS = 322,
     TYPENAME = 323,
     PRIVATE = 324,
     PUBLIC = 325,
     PROTECTED = 326,
     COLON = 327,
     STATIC = 328,
     VIRTUAL = 329,
     FRIEND = 330,
     THROW = 331,
     CATCH = 332,
     EXPLICIT = 333,
     USING = 334,
     NAMESPACE = 335,
     NATIVE = 336,
     INLINE = 337,
     TYPEMAP = 338,
     EXCEPT = 339,
     ECHO = 340,
     APPLY = 341,
     CLEAR = 342,
     SWIGTEMPLATE = 343,
     FRAGMENT = 344,
     WARN = 345,
     LESSTHAN = 346,
     GREATERTHAN = 347,
     DELETE_KW = 348,
     LESSTHANOREQUALTO = 349,
     GREATERTHANOREQUALTO = 350,
     EQUALTO = 351,
     NOTEQUALTO = 352,
     QUESTIONMARK = 353,
     TYPES = 354,
     PARMS = 355,
     NONID = 356,
     DSTAR = 357,
     DCNOT = 358,
     TEMPLATE = 359,
     OPERATOR = 360,
     COPERATOR = 361,
     PARSETYPE = 362,
     PARSEPARM = 363,
     PARSEPARMS = 364,
     CAST = 365,
     LOR = 366,
     LAND = 367,
     OR = 368,
     XOR = 369,
     AND = 370,
     RSHIFT = 371,
     LSHIFT = 372,
     MINUS = 373,
     PLUS = 374,
     MODULO = 375,
     SLASH = 376,
     STAR = 377,
     LNOT = 378,
     NOT = 379,
     UMINUS = 380,
     DCOLON = 381
   };
#endif
/* Tokens.  */
#define ID 258
#define HBLOCK 259
#define POUND 260
#define STRING 261
#define INCLUDE 262
#define IMPORT 263
#define INSERT 264
#define CHARCONST 265
#define NUM_INT 266
#define NUM_FLOAT 267
#define NUM_UNSIGNED 268
#define NUM_LONG 269
#define NUM_ULONG 270
#define NUM_LONGLONG 271
#define NUM_ULONGLONG 272
#define NUM_BOOL 273
#define TYPEDEF 274
#define TYPE_INT 275
#define TYPE_UNSIGNED 276
#define TYPE_SHORT 277
#define TYPE_LONG 278
#define TYPE_FLOAT 279
#define TYPE_DOUBLE 280
#define TYPE_CHAR 281
#define TYPE_WCHAR 282
#define TYPE_VOID 283
#define TYPE_SIGNED 284
#define TYPE_BOOL 285
#define TYPE_COMPLEX 286
#define TYPE_TYPEDEF 287
#define TYPE_RAW 288
#define TYPE_NON_ISO_INT8 289
#define TYPE_NON_ISO_INT16 290
#define TYPE_NON_ISO_INT32 291
#define TYPE_NON_ISO_INT64 292
#define LPAREN 293
#define RPAREN 294
#define COMMA 295
#define SEMI 296
#define EXTERN 297
#define INIT 298
#define LBRACE 299
#define RBRACE 300
#define PERIOD 301
#define CONST_QUAL 302
#define VOLATILE 303
#define REGISTER 304
#define STRUCT 305
#define UNION 306
#define EQUAL 307
#define SIZEOF 308
#define MODULE 309
#define LBRACKET 310
#define RBRACKET 311
#define ILLEGAL 312
#define CONSTANT 313
#define NAME 314
#define RENAME 315
#define NAMEWARN 316
#define EXTEND 317
#define PRAGMA 318
#define FEATURE 319
#define VARARGS 320
#define ENUM 321
#define CLASS 322
#define TYPENAME 323
#define PRIVATE 324
#define PUBLIC 325
#define PROTECTED 326
#define COLON 327
#define STATIC 328
#define VIRTUAL 329
#define FRIEND 330
#define THROW 331
#define CATCH 332
#define EXPLICIT 333
#define USING 334
#define NAMESPACE 335
#define NATIVE 336
#define INLINE 337
#define TYPEMAP 338
#define EXCEPT 339
#define ECHO 340
#define APPLY 341
#define CLEAR 342
#define SWIGTEMPLATE 343
#define FRAGMENT 344
#define WARN 345
#define LESSTHAN 346
#define GREATERTHAN 347
#define DELETE_KW 348
#define LESSTHANOREQUALTO 349
#define GREATERTHANOREQUALTO 350
#define EQUALTO 351
#define NOTEQUALTO 352
#define QUESTIONMARK 353
#define TYPES 354
#define PARMS 355
#define NONID 356
#define DSTAR 357
#define DCNOT 358
#define TEMPLATE 359
#define OPERATOR 360
#define COPERATOR 361
#define PARSETYPE 362
#define PARSEPARM 363
#define PARSEPARMS 364
#define CAST 365
#define LOR 366
#define LAND 367
#define OR 368
#define XOR 369
#define AND 370
#define RSHIFT 371
#define LSHIFT 372
#define MINUS 373
#define PLUS 374
#define MODULO 375
#define SLASH 376
#define STAR 377
#define LNOT 378
#define NOT 379
#define UMINUS 380
#define DCOLON 381




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 1592 "parser.y"

  char  *id;
  List  *bases;
  struct Define {
    String *val;
    String *rawval;
    int     type;
    String *qualifier;
    String *bitfield;
    Parm   *throws;
    String *throwf;
  } dtype;
  struct {
    char *type;
    String *filename;
    int   line;
  } loc;
  struct {
    char      *id;
    SwigType  *type;
    String    *defarg;
    ParmList  *parms;
    short      have_parms;
    ParmList  *throws;
    String    *throwf;
  } decl;
  Parm         *tparms;
  struct {
    String     *method;
    Hash       *kwargs;
  } tmap;
  struct {
    String     *type;
    String     *us;
  } ptype;
  SwigType     *type;
  String       *str;
  Parm         *p;
  ParmList     *pl;
  int           intvalue;
  Node         *node;



/* Line 214 of yacc.c  */
#line 1983 "y.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 1995 "y.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  55
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   3950

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  127
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  148
/* YYNRULES -- Number of rules.  */
#define YYNRULES  467
/* YYNRULES -- Number of states.  */
#define YYNSTATES  907

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   381

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     9,    12,    16,    19,    25,    29,
      32,    34,    36,    38,    40,    42,    44,    46,    49,    51,
      53,    55,    57,    59,    61,    63,    65,    67,    69,    71,
      73,    75,    77,    79,    81,    83,    85,    87,    89,    91,
      92,   100,   106,   110,   116,   122,   126,   129,   132,   138,
     141,   147,   150,   155,   157,   159,   167,   175,   181,   182,
     190,   192,   194,   197,   200,   202,   208,   214,   220,   224,
     229,   233,   241,   250,   256,   260,   262,   264,   268,   270,
     275,   283,   290,   292,   294,   302,   312,   321,   332,   338,
     346,   353,   362,   364,   366,   372,   377,   383,   391,   393,
     397,   404,   411,   420,   422,   425,   429,   431,   434,   438,
     445,   451,   461,   464,   466,   468,   470,   471,   478,   484,
     486,   491,   493,   495,   498,   504,   511,   516,   524,   534,
     541,   543,   545,   547,   549,   551,   553,   554,   564,   565,
     575,   577,   581,   586,   587,   594,   598,   600,   602,   604,
     606,   608,   610,   612,   615,   617,   619,   621,   625,   627,
     631,   636,   637,   644,   645,   651,   657,   660,   661,   668,
     670,   672,   673,   677,   679,   681,   683,   685,   687,   689,
     691,   693,   697,   699,   701,   703,   705,   707,   709,   711,
     713,   715,   722,   729,   737,   746,   755,   765,   773,   779,
     782,   785,   788,   789,   797,   798,   805,   807,   809,   811,
     813,   815,   817,   819,   821,   823,   825,   827,   830,   833,
     836,   841,   844,   850,   852,   855,   857,   859,   861,   863,
     865,   867,   869,   872,   874,   878,   880,   883,   891,   895,
     897,   900,   902,   906,   908,   910,   912,   915,   921,   924,
     927,   929,   932,   935,   937,   939,   941,   943,   946,   950,
     952,   955,   959,   964,   970,   975,   977,   980,   984,   989,
     995,   999,  1004,  1009,  1011,  1014,  1019,  1024,  1030,  1034,
    1039,  1044,  1046,  1049,  1052,  1056,  1058,  1061,  1063,  1066,
    1070,  1075,  1079,  1084,  1087,  1091,  1095,  1100,  1104,  1108,
    1111,  1114,  1116,  1118,  1121,  1123,  1125,  1127,  1129,  1132,
    1134,  1137,  1141,  1143,  1145,  1147,  1150,  1153,  1155,  1157,
    1160,  1162,  1164,  1167,  1169,  1171,  1173,  1175,  1177,  1179,
    1181,  1183,  1185,  1187,  1189,  1191,  1193,  1195,  1196,  1199,
    1201,  1203,  1207,  1209,  1211,  1215,  1217,  1219,  1221,  1223,
    1225,  1227,  1233,  1235,  1237,  1241,  1246,  1252,  1258,  1265,
    1268,  1271,  1273,  1275,  1277,  1279,  1281,  1283,  1285,  1287,
    1291,  1295,  1299,  1303,  1307,  1311,  1315,  1319,  1323,  1327,
    1331,  1335,  1339,  1343,  1347,  1351,  1357,  1360,  1363,  1366,
    1369,  1372,  1374,  1375,  1379,  1381,  1383,  1387,  1388,  1392,
    1393,  1399,  1401,  1403,  1405,  1407,  1409,  1411,  1413,  1415,
    1417,  1419,  1421,  1426,  1432,  1434,  1438,  1442,  1447,  1452,
    1456,  1459,  1461,  1463,  1467,  1470,  1474,  1476,  1478,  1480,
    1482,  1484,  1487,  1492,  1494,  1498,  1500,  1504,  1508,  1511,
    1514,  1517,  1520,  1523,  1528,  1530,  1534,  1536,  1540,  1544,
    1547,  1550,  1553,  1556,  1558,  1560,  1562,  1564,  1568,  1570,
    1574,  1580,  1582,  1586,  1590,  1596,  1598,  1600
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     128,     0,    -1,   129,    -1,   107,   213,    41,    -1,   107,
       1,    -1,   108,   213,    41,    -1,   108,     1,    -1,   109,
      38,   210,    39,    41,    -1,   109,     1,    41,    -1,   129,
     130,    -1,   274,    -1,   131,    -1,   168,    -1,   176,    -1,
      41,    -1,     1,    -1,   175,    -1,     1,   106,    -1,   132,
      -1,   134,    -1,   135,    -1,   136,    -1,   137,    -1,   138,
      -1,   141,    -1,   142,    -1,   145,    -1,   146,    -1,   147,
      -1,   148,    -1,   149,    -1,   150,    -1,   153,    -1,   155,
      -1,   158,    -1,   160,    -1,   165,    -1,   166,    -1,   167,
      -1,    -1,    62,   271,   264,    44,   133,   193,    45,    -1,
      86,   164,    44,   162,    45,    -1,    87,   162,    41,    -1,
      58,     3,    52,   235,    41,    -1,    58,   229,   221,   218,
      41,    -1,    58,     1,    41,    -1,    85,     4,    -1,    85,
     269,    -1,    84,    38,     3,    39,    44,    -1,    84,    44,
      -1,    84,    38,     3,    39,    41,    -1,    84,    41,    -1,
     269,    44,   213,    45,    -1,   269,    -1,   139,    -1,    89,
      38,   140,    40,   272,    39,     4,    -1,    89,    38,   140,
      40,   272,    39,    44,    -1,    89,    38,   140,    39,    41,
      -1,    -1,   144,   271,   269,    55,   143,   129,    56,    -1,
       7,    -1,     8,    -1,    82,     4,    -1,    82,    44,    -1,
       4,    -1,     9,    38,   262,    39,   269,    -1,     9,    38,
     262,    39,     4,    -1,     9,    38,   262,    39,    44,    -1,
      54,   271,   262,    -1,    59,    38,   262,    39,    -1,    59,
      38,    39,    -1,    81,    38,     3,    39,   209,     3,    41,
      -1,    81,    38,     3,    39,   209,   229,   221,    41,    -1,
      63,   152,     3,    52,   151,    -1,    63,   152,     3,    -1,
     269,    -1,     4,    -1,    38,     3,    39,    -1,   274,    -1,
     154,   221,   262,    41,    -1,   154,    38,   272,    39,   221,
     256,    41,    -1,   154,    38,   272,    39,   269,    41,    -1,
      60,    -1,    61,    -1,    64,    38,   262,    39,   221,   256,
     156,    -1,    64,    38,   262,    40,   273,    39,   221,   256,
      41,    -1,    64,    38,   262,   157,    39,   221,   256,   156,
      -1,    64,    38,   262,    40,   273,   157,    39,   221,   256,
      41,    -1,    64,    38,   262,    39,   156,    -1,    64,    38,
     262,    40,   273,    39,    41,    -1,    64,    38,   262,   157,
      39,   156,    -1,    64,    38,   262,    40,   273,   157,    39,
      41,    -1,   270,    -1,    41,    -1,   100,    38,   210,    39,
      41,    -1,    40,   262,    52,   273,    -1,    40,   262,    52,
     273,   157,    -1,    65,    38,   159,    39,   221,   256,    41,
      -1,   210,    -1,    11,    40,   213,    -1,    83,    38,   161,
      39,   162,   270,    -1,    83,    38,   161,    39,   162,    41,
      -1,    83,    38,   161,    39,   162,    52,   164,    41,    -1,
     272,    -1,   164,   163,    -1,    40,   164,   163,    -1,   274,
      -1,   229,   220,    -1,    38,   210,    39,    -1,    38,   210,
      39,    38,   210,    39,    -1,    99,    38,   210,    39,   156,
      -1,    88,    38,   263,    39,   267,    91,   214,    92,    41,
      -1,    90,   269,    -1,   170,    -1,   174,    -1,   173,    -1,
      -1,    42,   269,    44,   169,   129,    45,    -1,   209,   229,
     221,   172,   171,    -1,    41,    -1,    40,   221,   172,   171,
      -1,    44,    -1,   218,    -1,   227,   218,    -1,    76,    38,
     210,    39,   218,    -1,   227,    76,    38,   210,    39,   218,
      -1,   209,    66,     3,    41,    -1,   209,    66,   237,    44,
     238,    45,    41,    -1,   209,    66,   237,    44,   238,    45,
     221,   172,   171,    -1,   209,   229,    38,   210,    39,   257,
      -1,   177,    -1,   181,    -1,   182,    -1,   189,    -1,   190,
      -1,   200,    -1,    -1,   209,   254,   264,   245,    44,   178,
     193,    45,   180,    -1,    -1,   209,   254,    44,   179,   193,
      45,   221,   172,   171,    -1,    41,    -1,   221,   172,   171,
      -1,   209,   254,   264,    41,    -1,    -1,   104,    91,   185,
      92,   183,   184,    -1,   104,   254,   264,    -1,   170,    -1,
     177,    -1,   197,    -1,   182,    -1,   181,    -1,   199,    -1,
     186,    -1,   187,   188,    -1,   274,    -1,   253,    -1,   213,
      -1,    40,   187,   188,    -1,   274,    -1,    79,   264,    41,
      -1,    79,    80,   264,    41,    -1,    -1,    80,   264,    44,
     191,   129,    45,    -1,    -1,    80,    44,   192,   129,    45,
      -1,    80,     3,    52,   264,    41,    -1,   196,   193,    -1,
      -1,    62,    44,   194,   193,    45,   193,    -1,   142,    -1,
     274,    -1,    -1,     1,   195,   193,    -1,   168,    -1,   197,
      -1,   198,    -1,   201,    -1,   205,    -1,   199,    -1,   181,
      -1,   202,    -1,   209,   264,    41,    -1,   189,    -1,   182,
      -1,   200,    -1,   166,    -1,   167,    -1,   208,    -1,   141,
      -1,   165,    -1,    41,    -1,   209,   229,    38,   210,    39,
     257,    -1,   124,   266,    38,   210,    39,   206,    -1,    74,
     124,   266,    38,   210,    39,   207,    -1,   209,   106,   229,
     226,    38,   210,    39,   207,    -1,   209,   106,   229,   115,
      38,   210,    39,   207,    -1,   209,   106,   229,   226,   115,
      38,   210,    39,   207,    -1,   209,   106,   229,    38,   210,
      39,   207,    -1,    77,    38,   210,    39,    44,    -1,    70,
      72,    -1,    69,    72,    -1,    71,    72,    -1,    -1,   209,
     254,   264,   245,    44,   203,   180,    -1,    -1,   209,   254,
     245,    44,   204,   180,    -1,   150,    -1,   136,    -1,   148,
      -1,   153,    -1,   155,    -1,   158,    -1,   146,    -1,   160,
      -1,   134,    -1,   135,    -1,   137,    -1,   256,    41,    -1,
     256,    44,    -1,   256,    41,    -1,   256,    52,   235,    41,
      -1,   256,    44,    -1,   209,   229,    72,   241,    41,    -1,
      42,    -1,    42,   269,    -1,    73,    -1,    19,    -1,    74,
      -1,    75,    -1,    78,    -1,   274,    -1,   211,    -1,   213,
     212,    -1,   274,    -1,    40,   213,   212,    -1,   274,    -1,
     230,   219,    -1,   104,    91,   254,    92,   254,   264,   218,
      -1,    46,    46,    46,    -1,   215,    -1,   217,   216,    -1,
     274,    -1,    40,   217,   216,    -1,   274,    -1,   213,    -1,
     242,    -1,    52,   235,    -1,    52,   235,    55,   241,    56,
      -1,    52,    44,    -1,    72,   241,    -1,   274,    -1,   221,
     218,    -1,   224,   218,    -1,   218,    -1,   221,    -1,   224,
      -1,   274,    -1,   226,   222,    -1,   226,   115,   222,    -1,
     223,    -1,   115,   222,    -1,   264,   102,   222,    -1,   226,
     264,   102,   222,    -1,   226,   264,   102,   115,   222,    -1,
     264,   102,   115,   222,    -1,   264,    -1,   124,   264,    -1,
      38,   264,    39,    -1,    38,   226,   222,    39,    -1,    38,
     264,   102,   222,    39,    -1,   222,    55,    56,    -1,   222,
      55,   241,    56,    -1,   222,    38,   210,    39,    -1,   264,
      -1,   124,   264,    -1,    38,   226,   223,    39,    -1,    38,
     115,   223,    39,    -1,    38,   264,   102,   223,    39,    -1,
     223,    55,    56,    -1,   223,    55,   241,    56,    -1,   223,
      38,   210,    39,    -1,   226,    -1,   226,   225,    -1,   226,
     115,    -1,   226,   115,   225,    -1,   225,    -1,   115,   225,
      -1,   115,    -1,   264,   102,    -1,   226,   264,   102,    -1,
     226,   264,   102,   225,    -1,   225,    55,    56,    -1,   225,
      55,   241,    56,    -1,    55,    56,    -1,    55,   241,    56,
      -1,    38,   224,    39,    -1,   225,    38,   210,    39,    -1,
      38,   210,    39,    -1,   122,   227,   226,    -1,   122,   226,
      -1,   122,   227,    -1,   122,    -1,   228,    -1,   228,   227,
      -1,    47,    -1,    48,    -1,    49,    -1,   230,    -1,   227,
     231,    -1,   231,    -1,   231,   227,    -1,   227,   231,   227,
      -1,   232,    -1,    30,    -1,    28,    -1,    32,   261,    -1,
      66,   264,    -1,    33,    -1,   264,    -1,   254,   264,    -1,
     233,    -1,   234,    -1,   234,   233,    -1,    20,    -1,    22,
      -1,    23,    -1,    26,    -1,    27,    -1,    24,    -1,    25,
      -1,    29,    -1,    21,    -1,    31,    -1,    34,    -1,    35,
      -1,    36,    -1,    37,    -1,    -1,   236,   241,    -1,     3,
      -1,   274,    -1,   238,    40,   239,    -1,   239,    -1,     3,
      -1,     3,    52,   240,    -1,   274,    -1,   241,    -1,   242,
      -1,   229,    -1,   243,    -1,   269,    -1,    53,    38,   229,
     219,    39,    -1,   244,    -1,    10,    -1,    38,   241,    39,
      -1,    38,   241,    39,   241,    -1,    38,   241,   226,    39,
     241,    -1,    38,   241,   115,    39,   241,    -1,    38,   241,
     226,   115,    39,   241,    -1,   115,   241,    -1,   122,   241,
      -1,    11,    -1,    12,    -1,    13,    -1,    14,    -1,    15,
      -1,    16,    -1,    17,    -1,    18,    -1,   241,   119,   241,
      -1,   241,   118,   241,    -1,   241,   122,   241,    -1,   241,
     121,   241,    -1,   241,   120,   241,    -1,   241,   115,   241,
      -1,   241,   113,   241,    -1,   241,   114,   241,    -1,   241,
     117,   241,    -1,   241,   116,   241,    -1,   241,   112,   241,
      -1,   241,   111,   241,    -1,   241,    96,   241,    -1,   241,
      97,   241,    -1,   241,    95,   241,    -1,   241,    94,   241,
      -1,   241,    98,   241,    72,   241,    -1,   118,   241,    -1,
     119,   241,    -1,   124,   241,    -1,   123,   241,    -1,   229,
      38,    -1,   246,    -1,    -1,    72,   247,   248,    -1,   274,
      -1,   249,    -1,   248,    40,   249,    -1,    -1,   255,   250,
     264,    -1,    -1,   255,   252,   251,   255,   264,    -1,    70,
      -1,    69,    -1,    71,    -1,    67,    -1,    68,    -1,   253,
      -1,    50,    -1,    51,    -1,    74,    -1,   274,    -1,   227,
      -1,    76,    38,   210,    39,    -1,   227,    76,    38,   210,
      39,    -1,   274,    -1,   256,   258,    41,    -1,   256,   258,
      44,    -1,    38,   210,    39,    41,    -1,    38,   210,    39,
      44,    -1,    52,   235,    41,    -1,    72,   259,    -1,   274,
      -1,   260,    -1,   259,    40,   260,    -1,   264,    38,    -1,
      91,   214,    92,    -1,   274,    -1,     3,    -1,   269,    -1,
     262,    -1,   274,    -1,   266,   265,    -1,   101,   126,   266,
     265,    -1,   266,    -1,   101,   126,   266,    -1,   105,    -1,
     101,   126,   105,    -1,   126,   266,   265,    -1,   126,   266,
      -1,   126,   105,    -1,   103,   266,    -1,     3,   261,    -1,
       3,   268,    -1,   101,   126,     3,   268,    -1,     3,    -1,
     101,   126,     3,    -1,   105,    -1,   101,   126,   105,    -1,
     126,     3,   268,    -1,   126,     3,    -1,   126,   105,    -1,
     103,     3,    -1,   269,     6,    -1,     6,    -1,   269,    -1,
      44,    -1,     4,    -1,    38,   272,    39,    -1,   274,    -1,
     262,    52,   273,    -1,   262,    52,   273,    40,   272,    -1,
     262,    -1,   262,    40,   272,    -1,   262,    52,   139,    -1,
     262,    52,   139,    40,   272,    -1,   269,    -1,   243,    -1,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,  1745,  1745,  1758,  1762,  1765,  1768,  1771,  1774,  1779,
    1784,  1789,  1790,  1791,  1792,  1793,  1799,  1815,  1825,  1826,
    1827,  1828,  1829,  1830,  1831,  1832,  1833,  1834,  1835,  1836,
    1837,  1838,  1839,  1840,  1841,  1842,  1843,  1844,  1845,  1852,
    1852,  1924,  1934,  1945,  1966,  1988,  1999,  2008,  2027,  2033,
    2039,  2044,  2051,  2058,  2062,  2075,  2084,  2099,  2112,  2112,
    2167,  2168,  2175,  2194,  2225,  2229,  2239,  2244,  2262,  2302,
    2308,  2321,  2327,  2353,  2359,  2366,  2367,  2370,  2371,  2379,
    2425,  2471,  2482,  2485,  2512,  2518,  2524,  2530,  2538,  2544,
    2550,  2556,  2564,  2565,  2566,  2569,  2574,  2584,  2620,  2621,
    2656,  2673,  2681,  2694,  2719,  2725,  2729,  2732,  2743,  2748,
    2761,  2773,  3047,  3057,  3064,  3065,  3069,  3069,  3100,  3161,
    3165,  3187,  3193,  3199,  3205,  3211,  3224,  3239,  3249,  3327,
    3378,  3379,  3380,  3381,  3382,  3383,  3388,  3388,  3631,  3631,
    3754,  3755,  3767,  3787,  3787,  4076,  4082,  4085,  4088,  4091,
    4094,  4097,  4102,  4132,  4136,  4139,  4142,  4147,  4151,  4156,
    4166,  4197,  4197,  4226,  4226,  4248,  4275,  4290,  4290,  4300,
    4301,  4302,  4302,  4318,  4319,  4336,  4337,  4338,  4339,  4340,
    4341,  4342,  4343,  4344,  4345,  4346,  4347,  4348,  4349,  4350,
    4351,  4360,  4385,  4409,  4450,  4465,  4483,  4502,  4521,  4528,
    4535,  4543,  4564,  4564,  4590,  4590,  4626,  4629,  4633,  4636,
    4637,  4638,  4639,  4640,  4641,  4642,  4643,  4646,  4651,  4658,
    4666,  4674,  4685,  4691,  4692,  4700,  4701,  4702,  4703,  4704,
    4705,  4712,  4723,  4727,  4730,  4734,  4738,  4748,  4756,  4764,
    4777,  4781,  4784,  4788,  4792,  4820,  4828,  4839,  4853,  4862,
    4870,  4880,  4884,  4888,  4895,  4912,  4929,  4937,  4945,  4954,
    4958,  4967,  4978,  4990,  5000,  5013,  5020,  5028,  5044,  5052,
    5063,  5074,  5085,  5104,  5112,  5129,  5137,  5144,  5155,  5166,
    5177,  5196,  5202,  5208,  5215,  5224,  5227,  5236,  5243,  5250,
    5260,  5271,  5282,  5293,  5300,  5307,  5310,  5327,  5337,  5344,
    5350,  5355,  5361,  5365,  5371,  5372,  5373,  5379,  5385,  5389,
    5390,  5394,  5401,  5404,  5405,  5406,  5407,  5408,  5410,  5413,
    5418,  5443,  5446,  5500,  5504,  5508,  5512,  5516,  5520,  5524,
    5528,  5532,  5536,  5540,  5544,  5548,  5552,  5558,  5558,  5584,
    5585,  5588,  5601,  5609,  5617,  5627,  5630,  5644,  5645,  5664,
    5665,  5669,  5674,  5675,  5689,  5696,  5713,  5720,  5727,  5735,
    5739,  5745,  5746,  5747,  5748,  5749,  5750,  5751,  5752,  5755,
    5759,  5763,  5767,  5771,  5775,  5779,  5783,  5787,  5791,  5795,
    5799,  5803,  5807,  5821,  5825,  5829,  5835,  5839,  5843,  5847,
    5851,  5867,  5872,  5872,  5873,  5876,  5893,  5902,  5902,  5918,
    5918,  5934,  5935,  5936,  5940,  5944,  5950,  5953,  5957,  5963,
    5964,  5967,  5972,  5977,  5982,  5989,  5996,  6003,  6011,  6019,
    6027,  6028,  6031,  6032,  6035,  6041,  6047,  6050,  6051,  6054,
    6055,  6058,  6063,  6067,  6070,  6073,  6076,  6081,  6085,  6088,
    6095,  6101,  6110,  6115,  6119,  6122,  6125,  6128,  6133,  6137,
    6140,  6143,  6149,  6154,  6157,  6160,  6164,  6169,  6182,  6186,
    6191,  6197,  6201,  6206,  6210,  6217,  6220,  6225
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ID", "HBLOCK", "POUND", "STRING",
  "INCLUDE", "IMPORT", "INSERT", "CHARCONST", "NUM_INT", "NUM_FLOAT",
  "NUM_UNSIGNED", "NUM_LONG", "NUM_ULONG", "NUM_LONGLONG", "NUM_ULONGLONG",
  "NUM_BOOL", "TYPEDEF", "TYPE_INT", "TYPE_UNSIGNED", "TYPE_SHORT",
  "TYPE_LONG", "TYPE_FLOAT", "TYPE_DOUBLE", "TYPE_CHAR", "TYPE_WCHAR",
  "TYPE_VOID", "TYPE_SIGNED", "TYPE_BOOL", "TYPE_COMPLEX", "TYPE_TYPEDEF",
  "TYPE_RAW", "TYPE_NON_ISO_INT8", "TYPE_NON_ISO_INT16",
  "TYPE_NON_ISO_INT32", "TYPE_NON_ISO_INT64", "LPAREN", "RPAREN", "COMMA",
  "SEMI", "EXTERN", "INIT", "LBRACE", "RBRACE", "PERIOD", "CONST_QUAL",
  "VOLATILE", "REGISTER", "STRUCT", "UNION", "EQUAL", "SIZEOF", "MODULE",
  "LBRACKET", "RBRACKET", "ILLEGAL", "CONSTANT", "NAME", "RENAME",
  "NAMEWARN", "EXTEND", "PRAGMA", "FEATURE", "VARARGS", "ENUM", "CLASS",
  "TYPENAME", "PRIVATE", "PUBLIC", "PROTECTED", "COLON", "STATIC",
  "VIRTUAL", "FRIEND", "THROW", "CATCH", "EXPLICIT", "USING", "NAMESPACE",
  "NATIVE", "INLINE", "TYPEMAP", "EXCEPT", "ECHO", "APPLY", "CLEAR",
  "SWIGTEMPLATE", "FRAGMENT", "WARN", "LESSTHAN", "GREATERTHAN",
  "DELETE_KW", "LESSTHANOREQUALTO", "GREATERTHANOREQUALTO", "EQUALTO",
  "NOTEQUALTO", "QUESTIONMARK", "TYPES", "PARMS", "NONID", "DSTAR",
  "DCNOT", "TEMPLATE", "OPERATOR", "COPERATOR", "PARSETYPE", "PARSEPARM",
  "PARSEPARMS", "CAST", "LOR", "LAND", "OR", "XOR", "AND", "RSHIFT",
  "LSHIFT", "MINUS", "PLUS", "MODULO", "SLASH", "STAR", "LNOT", "NOT",
  "UMINUS", "DCOLON", "$accept", "program", "interface", "declaration",
  "swig_directive", "extend_directive", "$@1", "apply_directive",
  "clear_directive", "constant_directive", "echo_directive",
  "except_directive", "stringtype", "fname", "fragment_directive",
  "include_directive", "$@2", "includetype", "inline_directive",
  "insert_directive", "module_directive", "name_directive",
  "native_directive", "pragma_directive", "pragma_arg", "pragma_lang",
  "rename_directive", "rename_namewarn", "feature_directive",
  "stringbracesemi", "featattr", "varargs_directive", "varargs_parms",
  "typemap_directive", "typemap_type", "tm_list", "tm_tail",
  "typemap_parm", "types_directive", "template_directive",
  "warn_directive", "c_declaration", "$@3", "c_decl", "c_decl_tail",
  "initializer", "c_enum_forward_decl", "c_enum_decl",
  "c_constructor_decl", "cpp_declaration", "cpp_class_decl", "@4", "@5",
  "cpp_opt_declarators", "cpp_forward_class_decl", "cpp_template_decl",
  "$@6", "cpp_temp_possible", "template_parms", "templateparameters",
  "templateparameter", "templateparameterstail", "cpp_using_decl",
  "cpp_namespace_decl", "$@7", "$@8", "cpp_members", "$@9", "$@10",
  "cpp_member", "cpp_constructor_decl", "cpp_destructor_decl",
  "cpp_conversion_operator", "cpp_catch_decl", "cpp_protection_decl",
  "cpp_nested", "@11", "@12", "cpp_swig_directive", "cpp_end", "cpp_vend",
  "anonymous_bitfield", "storage_class", "parms", "rawparms", "ptail",
  "parm", "valparms", "rawvalparms", "valptail", "valparm", "def_args",
  "parameter_declarator", "typemap_parameter_declarator", "declarator",
  "notso_direct_declarator", "direct_declarator", "abstract_declarator",
  "direct_abstract_declarator", "pointer", "type_qualifier",
  "type_qualifier_raw", "type", "rawtype", "type_right", "primitive_type",
  "primitive_type_list", "type_specifier", "definetype", "$@13", "ename",
  "enumlist", "edecl", "etype", "expr", "valexpr", "exprnum",
  "exprcompound", "inherit", "raw_inherit", "$@14", "base_list",
  "base_specifier", "@15", "@16", "access_specifier", "templcpptype",
  "cpptype", "opt_virtual", "cpp_const", "ctor_end", "ctor_initializer",
  "mem_initializer_list", "mem_initializer", "template_decl", "idstring",
  "idstringopt", "idcolon", "idcolontail", "idtemplate", "idcolonnt",
  "idcolontailnt", "string", "stringbrace", "options", "kwargs",
  "stringnum", "empty", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   127,   128,   128,   128,   128,   128,   128,   128,   129,
     129,   130,   130,   130,   130,   130,   130,   130,   131,   131,
     131,   131,   131,   131,   131,   131,   131,   131,   131,   131,
     131,   131,   131,   131,   131,   131,   131,   131,   131,   133,
     132,   134,   135,   136,   136,   136,   137,   137,   138,   138,
     138,   138,   139,   140,   140,   141,   141,   141,   143,   142,
     144,   144,   145,   145,   146,   146,   146,   146,   147,   148,
     148,   149,   149,   150,   150,   151,   151,   152,   152,   153,
     153,   153,   154,   154,   155,   155,   155,   155,   155,   155,
     155,   155,   156,   156,   156,   157,   157,   158,   159,   159,
     160,   160,   160,   161,   162,   163,   163,   164,   164,   164,
     165,   166,   167,   168,   168,   168,   169,   168,   170,   171,
     171,   171,   172,   172,   172,   172,   173,   174,   174,   175,
     176,   176,   176,   176,   176,   176,   178,   177,   179,   177,
     180,   180,   181,   183,   182,   182,   184,   184,   184,   184,
     184,   184,   185,   186,   186,   187,   187,   188,   188,   189,
     189,   191,   190,   192,   190,   190,   193,   194,   193,   193,
     193,   195,   193,   196,   196,   196,   196,   196,   196,   196,
     196,   196,   196,   196,   196,   196,   196,   196,   196,   196,
     196,   197,   198,   198,   199,   199,   199,   199,   200,   201,
     201,   201,   203,   202,   204,   202,   205,   205,   205,   205,
     205,   205,   205,   205,   205,   205,   205,   206,   206,   207,
     207,   207,   208,   209,   209,   209,   209,   209,   209,   209,
     209,   210,   211,   211,   212,   212,   213,   213,   213,   214,
     215,   215,   216,   216,   217,   217,   218,   218,   218,   218,
     218,   219,   219,   219,   220,   220,   220,   221,   221,   221,
     221,   221,   221,   221,   221,   222,   222,   222,   222,   222,
     222,   222,   222,   223,   223,   223,   223,   223,   223,   223,
     223,   224,   224,   224,   224,   224,   224,   224,   224,   224,
     224,   225,   225,   225,   225,   225,   225,   225,   226,   226,
     226,   226,   227,   227,   228,   228,   228,   229,   230,   230,
     230,   230,   231,   231,   231,   231,   231,   231,   231,   231,
     232,   233,   233,   234,   234,   234,   234,   234,   234,   234,
     234,   234,   234,   234,   234,   234,   234,   236,   235,   237,
     237,   238,   238,   239,   239,   239,   240,   241,   241,   242,
     242,   242,   242,   242,   242,   242,   242,   242,   242,   242,
     242,   243,   243,   243,   243,   243,   243,   243,   243,   244,
     244,   244,   244,   244,   244,   244,   244,   244,   244,   244,
     244,   244,   244,   244,   244,   244,   244,   244,   244,   244,
     244,   245,   247,   246,   246,   248,   248,   250,   249,   251,
     249,   252,   252,   252,   253,   253,   254,   254,   254,   255,
     255,   256,   256,   256,   256,   257,   257,   257,   257,   257,
     258,   258,   259,   259,   260,   261,   261,   262,   262,   263,
     263,   264,   264,   264,   264,   264,   264,   265,   265,   265,
     265,   266,   267,   267,   267,   267,   267,   267,   268,   268,
     268,   268,   269,   269,   270,   270,   270,   271,   271,   272,
     272,   272,   272,   272,   272,   273,   273,   274
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     3,     2,     3,     2,     5,     3,     2,
       1,     1,     1,     1,     1,     1,     1,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     0,
       7,     5,     3,     5,     5,     3,     2,     2,     5,     2,
       5,     2,     4,     1,     1,     7,     7,     5,     0,     7,
       1,     1,     2,     2,     1,     5,     5,     5,     3,     4,
       3,     7,     8,     5,     3,     1,     1,     3,     1,     4,
       7,     6,     1,     1,     7,     9,     8,    10,     5,     7,
       6,     8,     1,     1,     5,     4,     5,     7,     1,     3,
       6,     6,     8,     1,     2,     3,     1,     2,     3,     6,
       5,     9,     2,     1,     1,     1,     0,     6,     5,     1,
       4,     1,     1,     2,     5,     6,     4,     7,     9,     6,
       1,     1,     1,     1,     1,     1,     0,     9,     0,     9,
       1,     3,     4,     0,     6,     3,     1,     1,     1,     1,
       1,     1,     1,     2,     1,     1,     1,     3,     1,     3,
       4,     0,     6,     0,     5,     5,     2,     0,     6,     1,
       1,     0,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     6,     6,     7,     8,     8,     9,     7,     5,     2,
       2,     2,     0,     7,     0,     6,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     2,     2,
       4,     2,     5,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     2,     1,     3,     1,     2,     7,     3,     1,
       2,     1,     3,     1,     1,     1,     2,     5,     2,     2,
       1,     2,     2,     1,     1,     1,     1,     2,     3,     1,
       2,     3,     4,     5,     4,     1,     2,     3,     4,     5,
       3,     4,     4,     1,     2,     4,     4,     5,     3,     4,
       4,     1,     2,     2,     3,     1,     2,     1,     2,     3,
       4,     3,     4,     2,     3,     3,     4,     3,     3,     2,
       2,     1,     1,     2,     1,     1,     1,     1,     2,     1,
       2,     3,     1,     1,     1,     2,     2,     1,     1,     2,
       1,     1,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     0,     2,     1,
       1,     3,     1,     1,     3,     1,     1,     1,     1,     1,
       1,     5,     1,     1,     3,     4,     5,     5,     6,     2,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     5,     2,     2,     2,     2,
       2,     1,     0,     3,     1,     1,     3,     0,     3,     0,
       5,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     4,     5,     1,     3,     3,     4,     4,     3,
       2,     1,     1,     3,     2,     3,     1,     1,     1,     1,
       1,     2,     4,     1,     3,     1,     3,     3,     2,     2,
       2,     2,     2,     4,     1,     3,     1,     3,     3,     2,
       2,     2,     2,     1,     1,     1,     1,     3,     1,     3,
       5,     1,     3,     3,     5,     1,     1,     0
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
     467,     0,     0,     0,     0,     0,    10,     4,   467,   323,
     331,   324,   325,   328,   329,   326,   327,   314,   330,   313,
     332,   467,   317,   333,   334,   335,   336,     0,   304,   305,
     306,   407,   408,     0,   404,   405,     0,     0,   435,     0,
       0,   302,   467,   309,   312,   320,   321,   406,     0,   318,
     433,     6,     0,     0,   467,     1,    15,    64,    60,    61,
       0,   226,    14,   223,   467,     0,     0,    82,    83,   467,
     467,     0,     0,   225,   227,   228,     0,   229,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     9,    11,    18,    19,    20,    21,    22,    23,
      24,    25,   467,    26,    27,    28,    29,    30,    31,    32,
       0,    33,    34,    35,    36,    37,    38,    12,   113,   115,
     114,    16,    13,   130,   131,   132,   133,   134,   135,     0,
     230,   467,   441,   426,   315,     0,   316,     0,     0,     3,
     308,   303,   467,   337,     0,     0,   287,   301,     0,   253,
     236,   467,   259,   467,   285,   281,   273,   250,   310,   322,
     319,     0,     0,   431,     5,     8,     0,   231,   467,   233,
      17,     0,   453,   224,     0,     0,   458,     0,   467,     0,
     307,     0,     0,     0,     0,    78,     0,   467,   467,     0,
       0,   467,   163,     0,     0,    62,    63,     0,     0,    51,
      49,    46,    47,   467,     0,   467,     0,   467,   467,     0,
     112,   467,   467,     0,     0,     0,     0,     0,     0,   273,
     467,     0,     0,   353,   361,   362,   363,   364,   365,   366,
     367,   368,     0,     0,     0,     0,     0,     0,     0,     0,
     244,     0,   239,   467,   348,   307,     0,   347,   349,   352,
     350,   241,   238,   436,   434,     0,   311,   467,   287,     0,
       0,   281,   318,   248,   246,     0,   293,     0,   347,   249,
     467,     0,   260,   286,   265,   299,   300,   274,   251,   467,
       0,   252,   467,     0,   283,   257,   282,   265,   288,   440,
     439,   438,     0,     0,   232,   235,   427,     0,   428,   452,
     116,   461,     0,    68,    45,   337,     0,   467,    70,     0,
       0,     0,    74,     0,     0,     0,    98,     0,     0,   159,
       0,   467,   161,     0,     0,   103,     0,     0,     0,   107,
     254,   255,   256,    42,     0,   104,   106,   429,     0,   430,
      54,     0,    53,     0,     0,   152,   467,   156,   406,   154,
     145,     0,   427,     0,     0,     0,     0,     0,     0,     0,
     265,     0,   467,     0,   340,   467,   467,   138,   319,     0,
       0,   359,   386,   387,   360,   389,   388,   425,     0,   240,
     243,   390,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   432,
       0,   287,   281,   318,     0,   273,   297,   295,   283,     0,
     273,   288,     0,   338,   294,   281,   318,   266,   467,     0,
     298,     0,   278,     0,     0,   291,     0,   258,   284,   289,
       0,   261,   437,     7,   467,     0,   467,     0,     0,   457,
       0,     0,    69,    39,    77,     0,     0,     0,     0,     0,
       0,     0,   160,     0,     0,   467,   467,     0,     0,   108,
       0,   467,     0,     0,     0,     0,     0,   143,     0,   153,
     158,    58,     0,     0,     0,     0,    79,     0,   126,   467,
       0,   318,     0,     0,   122,   467,     0,   142,   392,     0,
     391,   394,   354,     0,   301,     0,   467,   467,   384,   383,
     381,   382,     0,   380,   379,   375,   376,   374,   378,   377,
     370,   369,   373,   372,   371,     0,     0,   288,   276,   275,
     289,     0,     0,     0,   265,   267,   288,     0,   270,     0,
     280,   279,   296,   292,     0,   262,   290,   264,   234,    66,
      67,    65,     0,   462,   463,   466,   465,   459,    43,    44,
       0,    76,    73,    75,   456,    93,   455,     0,    88,   467,
     454,    92,     0,   465,     0,     0,    99,   467,   198,   165,
     164,     0,   223,     0,     0,    50,    48,   467,    41,   105,
     444,     0,   446,     0,    57,     0,     0,   110,   467,   467,
     467,   467,     0,     0,   343,     0,   342,   345,   467,   467,
       0,   119,   121,   118,     0,   123,   171,   190,     0,     0,
       0,     0,   227,     0,   214,   215,   207,   216,   188,   169,
     212,   208,   206,   209,   210,   211,   213,   189,   185,   186,
     173,   179,   183,   182,     0,     0,   174,   175,   178,   184,
     176,   180,   177,   187,     0,   230,   467,   136,   355,     0,
     301,   300,     0,     0,     0,   242,     0,   467,   277,   247,
     268,     0,   272,   271,   263,   117,     0,     0,     0,   467,
       0,   411,     0,   414,     0,     0,     0,     0,    90,   467,
       0,   162,   224,   467,     0,   101,     0,   100,     0,     0,
       0,   442,     0,   467,     0,    52,   146,   147,   150,   149,
     144,   148,   151,     0,   157,     0,     0,    81,     0,   467,
       0,   467,   337,   467,   129,     0,   467,   467,     0,   167,
     200,   199,   201,     0,     0,     0,   166,     0,     0,   467,
     318,   409,   393,   395,   397,   410,     0,   357,   356,     0,
     351,   385,   237,   269,   464,   460,    40,     0,   467,     0,
      84,   465,    95,    89,   467,     0,     0,    97,    71,     0,
       0,   109,   451,   449,   450,   445,   447,     0,    55,    56,
       0,    59,    80,   344,   346,   341,   127,   467,     0,     0,
       0,     0,   421,   467,     0,     0,   172,     0,     0,   467,
     467,     0,   467,     0,     0,   319,   181,   467,   402,   401,
     403,     0,   399,     0,   358,     0,     0,   467,    96,     0,
      91,   467,    86,    72,   102,   448,   443,     0,     0,     0,
     419,   420,   422,     0,   415,   416,   124,   120,   467,     0,
     467,     0,     0,   467,     0,     0,     0,     0,   204,     0,
     396,   398,   467,     0,    94,   412,     0,    85,     0,   111,
     128,   417,   418,     0,   424,   125,     0,     0,   467,   139,
       0,   467,   467,     0,   467,   222,     0,   202,     0,   140,
     137,   467,   413,    87,   423,   168,   467,   192,     0,   467,
       0,     0,   467,   191,   205,     0,   400,     0,   193,     0,
     217,   218,   197,   467,   467,     0,   203,   141,   219,   221,
     337,   195,   194,   467,     0,   196,   220
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     4,     5,    92,    93,    94,   550,   614,   615,   616,
     617,    99,   340,   341,   618,   619,   590,   102,   103,   620,
     105,   621,   107,   622,   552,   184,   623,   110,   624,   558,
     448,   625,   315,   626,   324,   206,   335,   207,   627,   628,
     629,   630,   436,   118,   603,   483,   119,   120,   121,   122,
     123,   736,   486,   870,   631,   632,   588,   700,   344,   345,
     346,   469,   633,   127,   455,   321,   634,   787,   718,   635,
     636,   637,   638,   639,   640,   641,   885,   866,   642,   877,
     888,   643,   644,   259,   167,   294,   168,   241,   242,   379,
     243,   484,   150,   329,   151,   272,   152,   153,   154,   218,
      40,    41,   244,   180,    43,    44,    45,    46,   264,   265,
     363,   595,   596,   773,   246,   268,   248,   249,   489,   490,
     646,   732,   733,   801,   842,   802,    47,    48,   734,   889,
     714,   781,   821,   822,   132,   301,   338,    49,   163,    50,
     583,   691,   250,   561,   175,   302,   547,   169
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -807
static const yytype_int16 yypact[] =
{
     466,  3243,  3303,    75,    78,  2757,  -807,  -807,    41,  -807,
    -807,  -807,  -807,  -807,  -807,  -807,  -807,  -807,  -807,  -807,
    -807,    41,  -807,  -807,  -807,  -807,  -807,    72,  -807,  -807,
    -807,  -807,  -807,   171,  -807,  -807,    14,   143,  -807,   254,
    3845,    98,   275,    98,  -807,  -807,  1194,  -807,   171,  -807,
     211,  -807,   263,   270,  3580,  -807,   219,  -807,  -807,  -807,
     321,  -807,  -807,   359,   348,  3362,   356,  -807,  -807,   348,
     370,   371,   399,  -807,  -807,  -807,   413,  -807,    62,   185,
     426,   133,   431,   653,   520,  3629,  3629,   440,   452,   359,
     478,   174,  -807,  -807,  -807,  -807,  -807,  -807,  -807,  -807,
    -807,  -807,   348,  -807,  -807,  -807,  -807,  -807,  -807,  -807,
     546,  -807,  -807,  -807,  -807,  -807,  -807,  -807,  -807,  -807,
    -807,  -807,  -807,  -807,  -807,  -807,  -807,  -807,  -807,  3688,
    -807,  1757,  -807,  -807,  -807,   352,  -807,    71,   657,  -807,
      98,  -807,  1446,   418,  1879,  2426,   719,   335,   171,  -807,
    -807,   240,   293,   240,   378,   288,   441,  -807,  -807,  -807,
    -807,   555,    74,  -807,  -807,  -807,   521,  -807,   526,  -807,
    -807,   194,  -807,   231,   194,   194,  -807,   538,   130,   638,
    -807,   367,   171,   582,   591,  -807,   194,  3520,  3580,   171,
     540,   141,  -807,   551,   604,  -807,  -807,   194,   606,  -807,
    -807,  -807,   609,  3580,   576,    89,   581,   588,   194,   359,
     609,  3580,  3580,   171,   359,    33,   580,   194,   178,   534,
     223,   940,   237,  -807,  -807,  -807,  -807,  -807,  -807,  -807,
    -807,  -807,  2426,   602,  2426,  2426,  2426,  2426,  2426,  2426,
    -807,   558,  -807,   624,   627,   867,  1854,    44,  -807,  -807,
     609,  -807,  -807,  -807,   211,   575,  -807,  2534,   910,   630,
     636,   369,   599,  -807,   622,  2426,  -807,  1479,  -807,  1854,
    2534,   171,   428,   378,  -807,  -807,   560,  -807,  -807,  3580,
    2001,  -807,  3580,  2123,   719,   428,   378,   610,   420,  -807,
    -807,   211,   670,  3580,  -807,  -807,  -807,   645,   609,  -807,
    -807,   210,   680,  -807,  -807,  -807,    84,   240,  -807,   682,
     679,   688,   678,   216,   692,   695,  -807,   698,   701,  -807,
     171,  -807,  -807,   713,   716,  -807,   728,   731,  3629,  -807,
    -807,  -807,  -807,  -807,  3629,  -807,  -807,  -807,   733,  -807,
    -807,   339,   264,   742,   697,  -807,   751,  -807,    69,  -807,
    -807,   164,   436,   981,   981,   707,   774,    38,   792,   580,
     734,   420,    59,   793,  -807,  2594,  1059,  -807,   360,  1160,
    3737,  1084,  -807,  -807,  -807,  -807,  -807,  -807,  1757,  -807,
    -807,  -807,  2426,  2426,  2426,  2426,  2426,  2426,  2426,  2426,
    2426,  2426,  2426,  2426,  2426,  2426,  2426,  2426,  2426,  -807,
     657,   450,   898,   738,   490,  -807,  -807,  -807,   450,   710,
     739,   981,  2426,  1854,  -807,   797,    18,  -807,  3580,  2245,
    -807,   805,  -807,  1522,   808,  -807,  1842,   428,   378,   976,
     580,   428,  -807,  -807,   526,   409,  -807,   194,  1232,  -807,
     813,   814,  -807,  -807,  -807,   530,   295,  1146,   810,  3580,
     638,   815,  -807,   822,  2856,  -807,   614,  3629,   233,   828,
     824,   588,   244,   831,   194,  3580,   347,  -807,  3580,  -807,
    -807,  -807,   981,   195,   580,    23,  -807,   970,  -807,   870,
     836,   707,   842,   777,  -807,   350,  1652,  -807,  -807,   837,
    -807,  -807,  2426,  2304,  2485,    13,   275,   624,  1054,  1054,
    1024,  1024,  1724,  1735,  1280,  1976,  1437,  1084,   994,   994,
     578,   578,  -807,  -807,  -807,   171,   739,  -807,  -807,  -807,
     450,   741,  1964,   755,   739,  -807,   580,   846,  -807,  2086,
    -807,  -807,  -807,  -807,   580,   428,   378,   428,  -807,  -807,
    -807,   609,  2955,  -807,   847,  -807,   264,   850,  -807,  -807,
    1652,  -807,  -807,   609,  -807,  -807,  -807,   854,  -807,   424,
     609,  -807,   843,    47,   513,   295,  -807,   424,  -807,  -807,
    -807,  3054,   359,  3796,   922,  -807,  -807,  3580,  -807,  -807,
     229,   770,  -807,   809,  -807,   860,   859,  -807,  1050,   751,
    -807,   424,   222,   580,   856,   458,  -807,  -807,   391,  3580,
     638,  -807,  -807,  -807,   868,  -807,  -807,  -807,   866,   839,
     848,   852,   799,   555,  -807,  -807,  -807,  -807,  -807,  -807,
    -807,  -807,  -807,  -807,  -807,  -807,  -807,  -807,  -807,  -807,
    -807,  -807,  -807,  -807,   872,  1652,  -807,  -807,  -807,  -807,
    -807,  -807,  -807,  -807,  3411,   884,   858,  -807,  1854,  2426,
    2485,  2651,  2426,   891,   895,  -807,  2426,   240,  -807,  -807,
    -807,   784,  -807,  -807,   428,  -807,   194,   194,   892,  3580,
     900,   869,   347,  -807,  1232,   320,   194,   905,  -807,   424,
     906,  -807,   609,   -11,   638,  -807,  3629,  -807,   923,   958,
      90,  -807,   113,  1757,   179,  -807,  -807,  -807,  -807,  -807,
    -807,  -807,  -807,  3471,  -807,  3153,   929,  -807,  2426,   870,
     326,  3580,  -807,   899,  -807,   925,  1059,  3580,  1652,  -807,
    -807,  -807,  -807,   555,   938,   638,  -807,  3737,   408,   163,
     942,  -807,   945,  -807,   756,  -807,  1652,  1854,  1854,  2426,
    -807,  1318,  -807,  -807,  -807,  -807,  -807,   947,  3580,   949,
    -807,   609,   948,  -807,   424,   845,   347,  -807,  -807,   954,
     955,  -807,  -807,   229,  -807,   229,  -807,   908,  -807,  -807,
     952,  -807,  -807,  -807,  1854,  -807,  -807,  1059,   962,   957,
     171,   300,  -807,   240,   777,   966,  -807,  1652,   971,  3580,
    1059,    81,  2594,  2426,   968,   360,  -807,   858,  -807,  -807,
    -807,   171,  -807,   965,  1854,   977,   978,  3580,  -807,   980,
    -807,   424,  -807,  -807,  -807,  -807,  -807,   982,   777,   586,
    -807,   987,  -807,   990,  -807,  -807,  -807,  -807,   240,   984,
    3580,   991,   777,  3580,   995,    20,   996,  1270,  -807,   992,
    -807,  -807,   858,   939,  -807,  -807,   999,  -807,   998,  -807,
    -807,  -807,  -807,   171,  -807,  -807,  1652,  1004,   424,  -807,
    1007,  3580,  3580,  1009,   391,  -807,   939,  -807,   171,  -807,
    -807,  1059,  -807,  -807,  -807,  -807,   424,  -807,   611,   424,
    1010,  1011,  3580,  -807,  -807,   939,  -807,   777,  -807,   319,
    -807,  -807,  -807,   424,   424,  1012,  -807,  -807,  -807,  -807,
    -807,  -807,  -807,   424,  1029,  -807,  -807
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -807,  -807,  -303,  -807,  -807,  -807,  -807,    21,    28,    35,
      40,  -807,   640,  -807,    45,    58,  -807,  -807,  -807,    61,
    -807,    63,  -807,    66,  -807,  -807,    68,  -807,    77,  -441,
    -530,    83,  -807,   100,  -807,  -293,   618,   -73,   101,   102,
     103,   107,  -807,   464,  -757,  -679,  -807,  -807,  -807,  -807,
     492,  -807,  -807,  -806,    -2,     5,  -807,  -807,  -807,  -807,
     615,   495,   124,  -807,  -807,  -807,  -534,  -807,  -807,  -807,
     499,  -807,   500,   160,  -807,  -807,  -807,  -807,  -807,  -807,
    -392,  -807,     9,   -31,  -807,   662,    42,   405,  -807,   605,
     721,   -36,   613,  -807,    31,   643,  -167,   -95,  -135,    12,
     -26,  -807,   289,    27,   -39,  -807,  1064,  -807,  -298,  -807,
    -807,  -807,   412,  -807,   959,  -129,  -416,  -807,  -691,  -807,
    -807,  -807,   325,  -807,  -807,  -807,  -208,   -43,   284,  -508,
     265,  -807,  -807,   274,  1109,  -162,  -807,   736,  -169,   -65,
    -807,  -335,   851,   559,   -13,  -148,  -415,     0
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -468
static const yytype_int16 yytable[] =
{
       6,   140,   247,   124,   348,   130,   149,   440,   133,   297,
     125,   273,   204,   303,   129,   141,   668,   158,   454,   309,
     286,   133,   545,   166,   313,   587,    95,   827,    42,    42,
     758,   545,   564,    96,   677,   460,   352,   784,   794,   172,
      97,     8,   157,    39,    52,    98,   337,   260,   213,   325,
     100,   672,   652,   299,   155,   358,   182,   525,   862,   680,
     884,   850,   525,   101,   176,     8,   104,   356,   106,   176,
     185,   108,   254,   109,     8,   859,    53,     8,    55,   896,
     131,    42,   111,   706,  -245,   399,   222,     8,   112,   214,
     713,   404,     8,   763,   409,   255,   289,   291,   818,  -428,
     478,   726,   176,  -339,   839,   113,   114,   115,   116,  -155,
     331,   832,   117,    54,   256,   278,   765,   281,   135,   833,
     526,   276,   432,   273,   678,   593,   286,   142,   653,   126,
     897,   251,   131,   542,    36,   863,  -245,   195,    38,    36,
     137,   217,   189,    38,   144,    28,    29,    30,   353,   428,
     131,   157,   571,   157,   261,   147,   316,   317,   245,   275,
     147,  -155,   260,    36,   574,   128,     8,    38,   295,    42,
     299,   756,   327,   240,     8,   260,   253,   196,   133,   290,
     343,     8,   305,   768,   786,    36,   404,   409,   191,    38,
      36,   133,   887,   320,    38,   764,   834,   296,     8,   353,
     172,   172,   803,   147,   146,   332,   147,   336,   339,   149,
     307,   147,   349,   148,    42,    42,   357,   155,   766,   471,
     364,   131,   808,   769,    31,    32,   362,   354,   299,   192,
      42,   750,   131,   306,   138,   488,   330,   299,    42,    42,
       8,    34,    35,   380,   521,   157,   809,   580,   421,   247,
     437,   424,   366,   829,   347,   446,   447,   155,   545,   752,
     348,   461,   438,   707,    36,   212,   273,   286,    38,   402,
     299,   441,    36,   428,   575,   300,    38,   576,     8,    36,
     286,   367,   415,    38,    42,   562,    36,   705,   420,   543,
      38,     8,   143,   359,   536,   139,    36,    42,     8,   554,
      38,   172,   271,   848,   164,   521,    42,   157,   465,    42,
     216,   165,   145,   142,   161,   812,   585,   147,   354,   148,
      42,     6,   875,     8,    36,   170,   270,   143,    38,     8,
     144,   279,   689,   306,   480,   434,   555,   162,    36,   556,
     485,   824,    38,   144,   825,   581,   470,   145,   280,   582,
     878,   554,   133,   172,   179,   690,   713,   515,   306,   171,
     898,   753,   133,   899,   306,   172,   157,   776,   491,   474,
     296,   900,     8,   172,   205,   205,    36,   354,   463,   464,
      38,   495,    28,    29,    30,   536,   174,   527,   555,    36,
     146,   556,    42,    38,   181,   557,    36,   147,   252,   148,
      38,   487,   143,   284,  -467,   245,   308,   142,   183,   186,
     216,     8,   271,   539,   779,   172,   282,   147,   221,   148,
     240,    36,   145,     8,   144,    38,   604,    36,   815,   711,
     816,    38,   488,   283,   295,   216,     6,   187,    28,    29,
      30,   216,   147,   712,   148,    42,   792,   557,   147,   605,
     148,   188,   124,   540,   130,     6,   130,   147,   357,   125,
     149,   336,   263,   129,   194,   573,   418,   670,   651,   197,
      36,    28,    29,    30,    38,    95,    42,   559,   208,   597,
     793,   567,    96,   419,   408,   157,   645,   892,   257,    97,
     209,   566,    42,   148,    98,    42,   157,   380,   709,   100,
     670,   901,   902,   710,   591,   144,   275,   586,   155,    36,
     347,   905,   101,    38,   562,   104,   211,   106,   744,   745,
     108,    36,   109,   216,   201,    38,   172,   131,   279,   518,
     147,   111,   148,   671,   551,   430,   172,   112,  -467,  -467,
     124,   671,   130,   288,   271,   280,   688,   125,   724,     8,
     645,   129,   675,   676,   113,   114,   115,   116,     8,   673,
     292,   117,  -467,    95,   247,   671,   293,   673,   715,   124,
      96,   130,   671,     1,     2,     3,   125,    97,   126,   304,
     129,   319,    98,     8,   215,   311,   698,   100,   130,   470,
       6,   673,    95,   699,   312,   322,   679,   703,   673,    96,
     101,   729,   904,   104,    42,   106,    97,   323,   108,   326,
     109,    98,   140,   760,   128,   299,   100,   205,   357,   111,
     328,   742,   333,   205,   651,   112,    42,   851,   334,   101,
     852,   716,   104,    61,   106,   645,   361,   108,   747,   109,
     370,     8,   113,   114,   115,   116,   735,    36,   111,   117,
     377,    38,   890,   671,   112,   891,   572,   157,   788,   496,
     222,   216,   275,   420,   378,   381,   126,   400,   147,   406,
     148,   113,   114,   115,   116,   407,   306,   412,   117,   673,
     778,    36,   147,   133,   435,    38,   785,    73,    74,    75,
     485,   198,    77,   251,   199,   126,    42,   200,   396,   397,
     398,   411,   128,   124,   271,   130,   754,    31,    32,   597,
     125,   433,   429,   782,   129,   759,   157,   806,   645,   439,
     245,   442,     8,   443,    34,    35,    95,   444,   671,   491,
     445,   128,   449,    96,   450,   240,   645,   451,    42,    36,
      97,   777,   452,    38,    42,    98,   205,   826,   279,   519,
     100,   485,   456,   216,   673,   457,   790,   270,   831,   366,
     147,   836,   148,   101,   485,   280,   104,   458,   106,   136,
     459,   108,   462,   109,   144,    42,   846,   157,   156,   279,
     658,   466,   111,   157,   160,   671,   811,   645,   112,   467,
     157,   468,   855,   418,   660,   491,   280,   735,   285,   857,
       8,   366,   860,   835,   354,   113,   114,   115,   116,   472,
     419,   673,   117,   473,   190,   193,    42,   600,   601,    42,
      36,   602,   418,   743,    38,   798,   799,   800,   157,   126,
     880,   881,   671,   476,    42,   270,   477,   479,   671,   419,
     517,   520,   735,   271,   530,   485,   219,   532,     8,   565,
     671,   895,   144,   671,   548,   549,   645,    42,   673,   568,
      42,   285,   684,   569,   673,   128,   577,   671,   671,   578,
       8,   157,   584,   594,   871,   598,   673,   671,   262,   673,
     599,   647,   274,   306,   277,   662,   810,   666,    42,    42,
     667,   287,   669,   673,   673,   674,   692,   871,    36,   694,
     693,     8,    38,   673,   695,   142,   717,  -467,   708,    42,
     719,   720,   408,     8,   173,   219,   871,   725,   310,   143,
     721,   271,   144,   723,   722,   318,   554,   427,   172,  -170,
     739,   431,   731,   728,   740,   202,   257,   746,   748,   145,
     210,   156,     8,     8,   755,   749,    36,   757,   142,   350,
      38,   355,   274,   144,   360,     8,   136,   219,   368,  -467,
     216,   762,   761,   685,   783,   144,   556,   147,    36,   148,
     772,   780,    38,     8,   686,   205,   789,   306,   365,     8,
     869,   156,   146,   796,     8,   797,   805,   807,   676,   147,
     792,   148,   770,   403,   405,   813,   814,   410,   820,    36,
     817,   819,   427,    38,   431,   828,   416,   417,   357,   830,
     843,    36,   838,   408,   270,    38,   791,   845,   844,   306,
     274,   847,   298,   849,   274,   298,   298,   853,   854,   856,
     858,   144,   298,   861,   148,   864,   867,   298,   872,   873,
      36,    36,   355,   876,    38,    38,   879,   882,   298,   893,
     894,   903,   696,    36,   216,   216,   453,    38,   523,   298,
     342,   147,   147,   148,   148,   351,   298,   216,   298,    61,
     906,    36,   535,   537,   147,    38,   148,    36,   544,   579,
     697,    38,    36,   589,   704,   534,    38,   701,   702,   405,
     405,   534,   572,   475,   271,   274,   538,   274,   767,   497,
     271,   481,   655,   267,   269,   148,    28,    29,    30,   654,
     159,   143,   394,   395,   396,   397,   398,   523,   382,   383,
     535,   775,   840,    73,    74,    75,   868,   874,    77,   883,
     134,   145,     0,   687,     0,   482,     0,     0,   516,     0,
     392,   393,   394,   395,   396,   397,   398,   405,     0,   296,
       0,   524,   172,     0,    91,     0,     0,   224,   225,   226,
     227,   228,   229,   230,   231,   274,   274,     0,     0,   661,
     392,   393,   394,   395,   396,   397,   398,   664,   382,   383,
     384,   385,   219,     0,     0,     0,   219,     0,     0,     0,
       0,   369,     0,   371,   372,   373,   374,   375,   376,   492,
     392,   393,   394,   395,   396,   397,   398,     0,   405,   219,
     274,     0,     0,   274,     9,    10,    11,    12,    13,    14,
      15,    16,     0,    18,   413,    20,     0,     0,    23,    24,
      25,    26,   156,     0,     0,     0,   661,     0,   172,   423,
       0,     0,   426,   224,   225,   226,   227,   228,   229,   230,
     231,   657,     0,     0,   382,   383,   384,   385,   386,     0,
       0,     0,   274,     0,     0,     0,     0,     0,     0,     0,
     274,   387,   388,   389,   390,   493,   392,   393,   394,   395,
     396,   397,   494,     0,     0,     0,   541,     0,   298,   546,
       0,     0,     0,     0,     0,     0,   553,   560,   563,     0,
       0,   219,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   865,     0,     0,     0,   298,     0,   560,     0,     0,
       0,     0,     0,     0,   592,     0,     0,     0,     0,   274,
       0,     0,     0,     0,     0,     0,   219,     0,     0,     0,
       0,   498,   499,   500,   501,   502,   503,   504,   505,   506,
     507,   508,   509,   510,   511,   512,   513,   514,     0,     0,
       0,     0,     0,     0,   382,   383,   384,   385,   386,     0,
       0,   522,     0,     0,   382,   383,   384,   385,   529,     0,
     730,   387,   388,   389,   390,   391,   392,   393,   394,   395,
     396,   397,   398,   389,   390,   391,   392,   393,   394,   395,
     396,   397,   398,     0,     0,     0,     0,     0,     0,     0,
       0,   219,   382,   383,   384,   385,   560,     0,     0,     0,
     219,     0,     0,   682,     0,   560,     0,     0,     0,   387,
     388,   389,   390,   391,   392,   393,   394,   395,   396,   397,
     398,     0,     0,     0,     0,     0,   219,     0,     0,     8,
       0,   648,   507,   514,     0,     0,     0,     0,     0,     0,
       0,   219,     0,     0,   219,   795,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,   257,     0,     0,     0,     0,     0,
       0,   219,    27,    28,    29,    30,    31,    32,     0,     0,
       0,   144,     0,     0,     0,     0,   219,     0,     0,     0,
       0,     0,    33,    34,    35,     0,   823,   298,   298,     0,
       0,     0,     0,   560,     0,   751,     0,   298,   481,     0,
       0,   382,   383,   384,   385,   414,     0,   841,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    36,     0,     0,
      37,    38,   391,   392,   393,   394,   395,   396,   397,   398,
       0,   258,     0,     0,     0,     0,     0,     0,   147,     0,
       0,     0,     0,   382,   383,   384,   385,   386,   531,   219,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   823,
     387,   388,   389,   390,   391,   392,   393,   394,   395,   396,
     397,   398,   219,     0,   886,     0,     0,   560,   737,   374,
       0,   738,     0,     0,     0,   741,   382,   383,   384,   385,
     386,   219,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   387,   388,   389,   390,   391,   392,   393,
     394,   395,   396,   397,   398,     0,     0,     0,     0,     0,
       0,     0,     0,   606,     0,  -467,    57,     0,     0,    58,
      59,    60,     0,     0,     0,     0,     0,   774,     0,     0,
       0,    61,  -467,  -467,  -467,  -467,  -467,  -467,  -467,  -467,
    -467,  -467,  -467,  -467,  -467,  -467,  -467,  -467,  -467,  -467,
       0,     0,     0,   607,    63,     0,     0,  -467,   804,  -467,
    -467,  -467,  -467,  -467,     0,     0,     0,     0,     0,     0,
      65,    66,    67,    68,   608,    70,    71,    72,  -467,  -467,
    -467,   609,   610,   611,     0,    73,   612,    75,     0,    76,
      77,    78,     0,     0,     0,    82,     0,    84,    85,    86,
      87,    88,    89,     0,     0,     0,     0,     0,     0,     0,
       0,    90,   837,  -467,     0,     0,    91,  -467,  -467,     0,
       8,     0,     0,   172,     0,     0,     0,   223,   224,   225,
     226,   227,   228,   229,   230,   231,   613,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,   232,   656,     0,     0,     0,
       0,     0,     0,    27,    28,    29,    30,    31,    32,     0,
     233,     0,     0,     0,     0,     0,     0,     0,   382,   383,
     384,   385,   386,    33,    34,    35,     0,     0,     0,   382,
     383,   384,   385,     0,     0,   387,   388,   389,   390,   391,
     392,   393,   394,   395,   396,   397,   398,   388,   389,   390,
     391,   392,   393,   394,   395,   396,   397,   398,    36,     0,
       0,    37,    38,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   234,     0,     0,   235,   236,     0,     0,   237,
     238,   239,     8,     0,     0,   172,     0,     0,     0,   223,
     224,   225,   226,   227,   228,   229,   230,   231,   533,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,   232,     0,     0,
       0,     0,     0,     0,     0,     0,    28,    29,    30,    31,
      32,     0,   233,     0,     0,   266,   382,   383,   384,   385,
     386,     0,     0,     0,     0,    33,    34,    35,   382,   383,
     384,   385,   386,   387,   388,   389,   390,   391,   392,   393,
     394,   395,   396,   397,   398,   387,   388,   389,   390,   391,
     392,   393,   394,   395,   396,   397,   398,     0,     0,     0,
      36,     0,     0,     0,    38,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   234,     0,     0,   235,   236,     0,
       0,   237,   238,   239,     8,     0,     0,   172,     0,     0,
       0,   223,   224,   225,   226,   227,   228,   229,   230,   231,
     659,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,   232,
       0,     0,     0,     0,     0,     0,     0,     0,    28,    29,
      30,    31,    32,     0,   233,     0,     0,   422,   382,   383,
     384,   385,   386,     0,     0,     0,     0,    33,    34,    35,
     382,   383,   384,   385,     0,   387,   388,   389,   390,   391,
     392,   393,   394,   395,   396,   397,   398,     0,     0,     0,
     390,   391,   392,   393,   394,   395,   396,   397,   398,     0,
       0,     0,    36,     0,     0,     0,    38,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   234,     0,     0,   235,
     236,     0,     0,   237,   238,   239,     8,     0,     0,   172,
       0,     0,     0,   223,   224,   225,   226,   227,   228,   229,
     230,   231,   663,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,   232,     0,     0,     0,     0,     0,     0,     0,     0,
      28,    29,    30,    31,    32,     0,   233,     0,     0,   425,
     382,   383,   384,   385,   386,     0,     0,     0,     0,    33,
      34,    35,     0,     0,     0,     0,     0,   387,   388,   389,
     390,   391,   392,   393,   394,   395,   396,   397,   398,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    36,     0,     0,     0,    38,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   234,     0,
       0,   235,   236,     0,     0,   237,   238,   239,     8,     0,
       0,   172,     0,     0,     0,   223,   224,   225,   226,   227,
     228,   229,   230,   231,     0,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,   232,     0,     0,     0,     0,     0,     0,
       0,     0,    28,    29,    30,    31,    32,     0,   233,     0,
       0,   528,     0,     0,     0,     0,     0,     8,     0,     0,
     172,    33,    34,    35,   223,   224,   225,   226,   227,   228,
     229,   230,   231,     0,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,   232,   649,     0,     0,    36,     0,     0,     0,
      38,    28,    29,    30,    31,    32,     0,   233,     0,     0,
     234,     0,     0,   235,   236,     0,     0,   237,   238,   239,
      33,    34,    35,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    36,     0,     0,     0,    38,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   234,
       0,     0,   235,   236,     0,     0,   237,   238,   239,     8,
       0,     0,   172,     0,     0,     0,   223,   224,   225,   226,
     227,   228,   229,   230,   231,     0,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,   232,     0,     0,     0,     0,     0,
       0,     0,     0,    28,    29,    30,    31,    32,     0,   233,
       0,     0,     0,     0,     0,     0,     0,     0,     8,     0,
       0,   172,    33,    34,    35,   223,   224,   225,   226,   227,
     228,   229,   230,   231,     0,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,   232,     0,     0,     0,    36,     0,     0,
       0,    38,    28,    29,    30,    31,    32,     8,   233,     0,
       0,   234,     0,     0,   235,   236,     0,     0,   237,   238,
     239,    33,    34,    35,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,   257,     0,     0,     0,     0,     0,     0,     0,
      27,    28,    29,    30,    31,    32,    36,     0,     0,   144,
      38,     0,     0,     0,     0,     0,     0,     8,     0,     0,
      33,    34,    35,   235,   236,     0,     0,   650,   238,   239,
       0,     0,     0,     0,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,     0,     0,     0,    36,     0,     0,    37,    38,
      27,    28,    29,    30,    31,    32,     0,     0,     0,   401,
       0,     0,     0,     0,     8,     0,   147,     0,     0,     0,
      33,    34,    35,     0,     0,     0,     0,     0,     0,     0,
       0,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,     0,
       0,     0,     0,     0,     0,    36,     0,     0,    37,    38,
       0,    31,    32,     0,     0,     0,     0,     0,     0,   353,
       0,     0,     0,     0,     0,     0,   147,    33,    34,    35,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    36,     0,     0,     0,    38,    -2,    56,     0,
    -467,    57,     0,     0,    58,    59,    60,     0,     0,     0,
       0,     0,     0,   147,     0,     0,    61,  -467,  -467,  -467,
    -467,  -467,  -467,  -467,  -467,  -467,  -467,  -467,  -467,  -467,
    -467,  -467,  -467,  -467,  -467,     0,     0,     0,    62,    63,
       0,     0,     0,     0,  -467,  -467,  -467,  -467,  -467,     0,
       0,    64,     0,     0,     0,    65,    66,    67,    68,    69,
      70,    71,    72,  -467,  -467,  -467,     0,     0,     0,     0,
      73,    74,    75,     0,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,     0,     0,
       0,     0,     0,     0,     0,     0,    90,    56,  -467,  -467,
      57,    91,  -467,    58,    59,    60,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    61,  -467,  -467,  -467,  -467,
    -467,  -467,  -467,  -467,  -467,  -467,  -467,  -467,  -467,  -467,
    -467,  -467,  -467,  -467,     0,     0,     0,    62,    63,     0,
       0,   570,     0,  -467,  -467,  -467,  -467,  -467,     0,     0,
      64,     0,     0,     0,    65,    66,    67,    68,    69,    70,
      71,    72,  -467,  -467,  -467,     0,     0,     0,     0,    73,
      74,    75,     0,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,     0,     0,     0,
       0,     0,     0,     0,     0,    90,    56,  -467,  -467,    57,
      91,  -467,    58,    59,    60,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    61,  -467,  -467,  -467,  -467,  -467,
    -467,  -467,  -467,  -467,  -467,  -467,  -467,  -467,  -467,  -467,
    -467,  -467,  -467,     0,     0,     0,    62,    63,     0,     0,
     665,     0,  -467,  -467,  -467,  -467,  -467,     0,     0,    64,
       0,     0,     0,    65,    66,    67,    68,    69,    70,    71,
      72,  -467,  -467,  -467,     0,     0,     0,     0,    73,    74,
      75,     0,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,     0,     0,     0,     0,
       0,     0,     0,     0,    90,    56,  -467,  -467,    57,    91,
    -467,    58,    59,    60,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    61,  -467,  -467,  -467,  -467,  -467,  -467,
    -467,  -467,  -467,  -467,  -467,  -467,  -467,  -467,  -467,  -467,
    -467,  -467,     0,     0,     0,    62,    63,     0,     0,   681,
       0,  -467,  -467,  -467,  -467,  -467,     0,     0,    64,     0,
       0,     0,    65,    66,    67,    68,    69,    70,    71,    72,
    -467,  -467,  -467,     0,     0,     0,     0,    73,    74,    75,
       0,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,     0,     0,     0,     0,     0,
       0,     0,     0,    90,    56,  -467,  -467,    57,    91,  -467,
      58,    59,    60,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    61,  -467,  -467,  -467,  -467,  -467,  -467,  -467,
    -467,  -467,  -467,  -467,  -467,  -467,  -467,  -467,  -467,  -467,
    -467,     0,     0,     0,    62,    63,     0,     0,     0,     0,
    -467,  -467,  -467,  -467,  -467,     0,     0,    64,     0,   771,
       0,    65,    66,    67,    68,    69,    70,    71,    72,  -467,
    -467,  -467,     0,     0,     0,     0,    73,    74,    75,     0,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,     7,     0,     8,     0,     0,     0,
       0,     0,    90,     0,  -467,     0,     0,    91,  -467,     0,
       0,     0,     0,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,     0,     0,     0,     0,     0,     0,     0,     0,    27,
      28,    29,    30,    31,    32,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    51,     0,     8,     0,     0,    33,
      34,    35,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,     0,     0,     0,    36,     0,     0,    37,    38,    27,
      28,    29,    30,    31,    32,     0,     0,     0,     0,     0,
       0,     0,     0,   177,     0,   178,     0,     0,     0,    33,
      34,    35,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
       0,     0,     0,     0,    36,     0,     0,    37,    38,    28,
      29,    30,    31,    32,     8,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    33,    34,
      35,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    28,    29,
      30,    31,    32,    36,     0,     0,     0,    38,     0,     0,
       0,     0,     0,     0,     8,     0,     0,   220,    34,    35,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,     0,
       0,     0,    36,     0,     0,     0,    38,   727,    28,    29,
      30,    31,    32,     8,     0,     0,     0,     0,     0,     0,
       0,   314,     0,     0,     0,     0,     0,    33,    34,    35,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,     0,     0,
       0,     0,     0,     0,     0,     0,    27,    28,    29,    30,
      31,    32,    36,     0,     0,     0,    38,   727,     0,     0,
       0,     0,     0,     8,     0,     0,    33,    34,    35,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,     0,     0,
       0,    36,     0,     0,    37,    38,    27,    28,    29,    30,
      31,    32,     8,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    33,    34,    35,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,   203,     0,     0,
       0,     0,     0,     0,     0,     0,    28,    29,    30,    31,
      32,    36,     0,     0,    37,    38,     0,     0,     0,     0,
       0,     8,     0,     0,     0,    33,    34,    35,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,     0,     0,     0,     0,
      36,     0,     0,     0,    38,    28,    29,    30,    31,    32,
       8,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   220,    34,    35,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    28,    29,    30,    31,    32,    36,
       0,     0,     0,    38,     0,     0,     0,     0,     0,   683,
       0,     0,     0,    33,    34,    35,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,     0,     0,     0,     0,    36,     0,
       0,     0,    38,    28,    29,    30,    31,    32,     8,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    33,    34,    35,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    31,    32,    36,     0,     0,
       0,    38,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    33,    34,    35,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    36,     0,     0,     0,
      38
};

static const yytype_int16 yycheck[] =
{
       0,    40,   131,     5,   212,     5,    42,   305,     8,   171,
       5,   146,    85,   175,     5,    41,   550,    43,   321,   181,
     155,    21,   438,    54,   186,   466,     5,   784,     1,     2,
      41,   447,   447,     5,   564,   328,     3,   716,   729,     6,
       5,     3,    42,     1,     2,     5,   208,   142,    91,   197,
       5,   559,    39,     6,    42,   217,    69,    39,    38,   567,
     866,   818,    39,     5,    64,     3,     5,   215,     5,    69,
      70,     5,   137,     5,     3,   832,     1,     3,     0,   885,
      91,    54,     5,   591,    40,   254,   129,     3,     5,   102,
     598,   258,     3,     3,   261,   138,   161,   162,   777,    52,
      41,   635,   102,    44,   795,     5,     5,     5,     5,    40,
     205,   790,     5,    38,   140,   151,     3,   153,    46,    38,
     102,   147,   291,   258,   565,   102,   261,    38,   115,     5,
     887,   131,    91,   436,   101,   115,    92,     4,   105,   101,
     126,   110,    80,   105,    55,    47,    48,    49,   115,   284,
      91,   151,   455,   153,   142,   122,   187,   188,   131,   147,
     122,    92,   257,   101,   457,     5,     3,   105,   168,   142,
       6,   679,   203,   131,     3,   270,   105,    44,   178,   105,
     211,     3,    52,     4,   718,   101,   353,   354,     3,   105,
     101,   191,   871,    52,   105,   105,   115,     3,     3,   115,
       6,     6,   736,   122,   115,   205,   122,   207,   208,   245,
     179,   122,   212,   124,   187,   188,    38,   205,   105,    55,
     220,    91,   752,    44,    50,    51,     3,   215,     6,    44,
     203,   672,    91,    38,    91,    72,   205,     6,   211,   212,
       3,    67,    68,   243,   411,   245,   754,     3,   279,   378,
      40,   282,   221,   787,   212,    39,    40,   245,   674,   674,
     468,   334,    52,    41,   101,    91,   401,   402,   105,   257,
       6,   307,   101,   408,    41,    44,   105,    44,     3,   101,
     415,    44,   270,   105,   257,   447,   101,   590,   276,   437,
     105,     3,    52,   115,   429,    41,   101,   270,     3,     4,
     105,     6,   124,   811,    41,   472,   279,   307,    44,   282,
     115,    41,    72,    38,   103,   756,   464,   122,   306,   124,
     293,   321,   856,     3,   101,   106,    38,    52,   105,     3,
      55,    38,   103,    38,   365,   293,    41,   126,   101,    44,
     366,    41,   105,    55,    44,   101,   346,    72,    55,   105,
     858,     4,   352,     6,    65,   126,   864,   400,    38,    38,
      41,    41,   362,    44,    38,     6,   366,    41,   368,   357,
       3,    52,     3,     6,    85,    86,   101,   365,    39,    40,
     105,   369,    47,    48,    49,   520,    38,   418,    41,   101,
     115,    44,   365,   105,    38,   100,   101,   122,    46,   124,
     105,    41,    52,   115,    44,   378,    39,    38,    38,    38,
     115,     3,   124,     4,   712,     6,    38,   122,   129,   124,
     378,   101,    72,     3,    55,   105,    76,   101,   763,    38,
     765,   105,    72,    55,   434,   115,   436,    38,    47,    48,
      49,   115,   122,    52,   124,   418,    38,   100,   122,   485,
     124,    38,   454,    44,   454,   455,   456,   122,    38,   454,
     496,   461,    44,   454,    38,   456,    38,    76,   494,    38,
     101,    47,    48,    49,   105,   454,   449,   446,    38,   479,
      72,   450,   454,    55,   115,   485,   486,   879,    38,   454,
      38,   449,   465,   124,   454,   468,   496,   497,    40,   454,
      76,   893,   894,    45,   473,    55,   494,   465,   496,   101,
     468,   903,   454,   105,   676,   454,    38,   454,   666,   667,
     454,   101,   454,   115,     4,   105,     6,    91,    38,    39,
     122,   454,   124,   559,     4,   115,     6,   454,   102,   103,
     542,   567,   542,   102,   124,    55,   577,   542,   613,     3,
     550,   542,    39,    40,   454,   454,   454,   454,     3,   559,
      39,   454,   126,   542,   693,   591,    40,   567,   599,   571,
     542,   571,   598,   107,   108,   109,   571,   542,   454,    41,
     571,    41,   542,     3,    38,     3,   588,   542,   588,   589,
     590,   591,   571,   588,     3,    44,   565,   588,   598,   571,
     542,   644,   900,   542,   577,   542,   571,     3,   542,     3,
     542,   571,   651,   686,   454,     6,   571,   328,    38,   542,
      44,   657,    41,   334,   650,   542,   599,    41,    40,   571,
      44,   600,   571,    19,   571,   635,   102,   571,   669,   571,
      38,     3,   542,   542,   542,   542,   646,   101,   571,   542,
      92,   105,    41,   679,   571,    44,    42,   657,   723,   370,
     703,   115,   650,   651,    40,    38,   542,    92,   122,    39,
     124,   571,   571,   571,   571,    39,    38,    55,   571,   679,
     711,   101,   122,   683,    39,   105,   717,    73,    74,    75,
     716,    38,    78,   693,    41,   571,   669,    44,   120,   121,
     122,   102,   542,   705,   124,   705,   675,    50,    51,   709,
     705,    41,   102,   713,   705,   684,   716,   748,   718,    39,
     693,    39,     3,    44,    67,    68,   705,    39,   754,   729,
      52,   571,    40,   705,    39,   693,   736,    39,   711,   101,
     705,   710,    41,   105,   717,   705,   457,   783,    38,    39,
     705,   777,    39,   115,   754,    39,   725,    38,   789,   728,
     122,   792,   124,   705,   790,    55,   705,    39,   705,    33,
      39,   705,    39,   705,    55,   748,   807,   777,    42,    38,
      39,    39,   705,   783,    48,   811,   755,   787,   705,    92,
     790,    40,   828,    38,    39,   795,    55,   797,   155,   830,
       3,   770,   833,   791,   792,   705,   705,   705,   705,   102,
      55,   811,   705,    39,    78,    79,   789,    40,    41,   792,
     101,    44,    38,    39,   105,    69,    70,    71,   828,   705,
     861,   862,   858,    41,   807,    38,   102,    44,   864,    55,
     102,   102,   842,   124,    39,   871,   110,    39,     3,    39,
     876,   882,    55,   879,    41,    41,   856,   830,   858,    44,
     833,   218,   573,    41,   864,   705,    38,   893,   894,    45,
       3,   871,    41,     3,   843,    39,   876,   903,   142,   879,
      38,    44,   146,    38,   148,    39,    41,    40,   861,   862,
      40,   155,    38,   893,   894,    52,   126,   866,   101,    39,
      91,     3,   105,   903,    45,    38,    38,    40,    52,   882,
      44,    72,   115,     3,    63,   179,   885,    45,   182,    52,
      72,   124,    55,   124,    72,   189,     4,   284,     6,    45,
      39,   288,    74,   644,    39,    84,    38,    45,    38,    72,
      89,   205,     3,     3,    39,    76,   101,    41,    38,   213,
     105,   215,   216,    55,   218,     3,   220,   221,   222,    92,
     115,     3,    39,    41,    39,    55,    44,   122,   101,   124,
      41,    72,   105,     3,    52,   686,    38,    38,    38,     3,
      41,   245,   115,    41,     3,    40,    39,    38,    40,   122,
      38,   124,   703,   257,   258,    41,    41,   261,    41,   101,
      92,    39,   359,   105,   361,    39,   270,   271,    38,    38,
      45,   101,    44,   115,    38,   105,   727,    39,    41,    38,
     284,    41,   171,    41,   288,   174,   175,    40,    38,    45,
      39,    55,   181,    38,   124,    39,    44,   186,    39,    41,
     101,   101,   306,    39,   105,   105,    39,    38,   197,    39,
      39,    39,   588,   101,   115,   115,   320,   105,   415,   208,
     209,   122,   122,   124,   124,   214,   215,   115,   217,    19,
      41,   101,   429,   430,   122,   105,   124,   101,   438,   461,
     588,   105,   101,   468,   589,   115,   105,   588,   588,   353,
     354,   115,    42,   357,   124,   359,   434,   361,   693,   378,
     124,   365,   497,   144,   145,   124,    47,    48,    49,   496,
      46,    52,   118,   119,   120,   121,   122,   474,    94,    95,
     477,   709,   797,    73,    74,    75,   842,   853,    78,   864,
      21,    72,    -1,   574,    -1,    76,    -1,    -1,   402,    -1,
     116,   117,   118,   119,   120,   121,   122,   411,    -1,     3,
      -1,   415,     6,    -1,   104,    -1,    -1,    11,    12,    13,
      14,    15,    16,    17,    18,   429,   430,    -1,    -1,   526,
     116,   117,   118,   119,   120,   121,   122,   534,    94,    95,
      96,    97,   446,    -1,    -1,    -1,   450,    -1,    -1,    -1,
      -1,   232,    -1,   234,   235,   236,   237,   238,   239,    39,
     116,   117,   118,   119,   120,   121,   122,    -1,   472,   473,
     474,    -1,    -1,   477,    20,    21,    22,    23,    24,    25,
      26,    27,    -1,    29,   265,    31,    -1,    -1,    34,    35,
      36,    37,   496,    -1,    -1,    -1,   593,    -1,     6,   280,
      -1,    -1,   283,    11,    12,    13,    14,    15,    16,    17,
      18,   515,    -1,    -1,    94,    95,    96,    97,    98,    -1,
      -1,    -1,   526,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     534,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,    -1,    -1,    -1,   435,    -1,   437,   438,
      -1,    -1,    -1,    -1,    -1,    -1,   445,   446,   447,    -1,
      -1,   565,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    41,    -1,    -1,    -1,   464,    -1,   466,    -1,    -1,
      -1,    -1,    -1,    -1,   473,    -1,    -1,    -1,    -1,   593,
      -1,    -1,    -1,    -1,    -1,    -1,   600,    -1,    -1,    -1,
      -1,   382,   383,   384,   385,   386,   387,   388,   389,   390,
     391,   392,   393,   394,   395,   396,   397,   398,    -1,    -1,
      -1,    -1,    -1,    -1,    94,    95,    96,    97,    98,    -1,
      -1,   412,    -1,    -1,    94,    95,    96,    97,   419,    -1,
     644,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   675,    94,    95,    96,    97,   565,    -1,    -1,    -1,
     684,    -1,    -1,   572,    -1,   574,    -1,    -1,    -1,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,    -1,    -1,    -1,    -1,    -1,   710,    -1,    -1,     3,
      -1,   492,   493,   494,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   725,    -1,    -1,   728,   729,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    -1,    -1,    -1,    -1,    -1,
      -1,   755,    46,    47,    48,    49,    50,    51,    -1,    -1,
      -1,    55,    -1,    -1,    -1,    -1,   770,    -1,    -1,    -1,
      -1,    -1,    66,    67,    68,    -1,   780,   666,   667,    -1,
      -1,    -1,    -1,   672,    -1,   674,    -1,   676,   792,    -1,
      -1,    94,    95,    96,    97,    56,    -1,   801,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   101,    -1,    -1,
     104,   105,   115,   116,   117,   118,   119,   120,   121,   122,
      -1,   115,    -1,    -1,    -1,    -1,    -1,    -1,   122,    -1,
      -1,    -1,    -1,    94,    95,    96,    97,    98,    56,   843,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   853,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   866,    -1,   868,    -1,    -1,   756,   649,   650,
      -1,   652,    -1,    -1,    -1,   656,    94,    95,    96,    97,
      98,   885,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     1,    -1,     3,     4,    -1,    -1,     7,
       8,     9,    -1,    -1,    -1,    -1,    -1,   708,    -1,    -1,
      -1,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      -1,    -1,    -1,    41,    42,    -1,    -1,    45,   739,    47,
      48,    49,    50,    51,    -1,    -1,    -1,    -1,    -1,    -1,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    -1,    73,    74,    75,    -1,    77,
      78,    79,    -1,    -1,    -1,    83,    -1,    85,    86,    87,
      88,    89,    90,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    99,   793,   101,    -1,    -1,   104,   105,   106,    -1,
       3,    -1,    -1,     6,    -1,    -1,    -1,    10,    11,    12,
      13,    14,    15,    16,    17,    18,   124,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    72,    -1,    -1,    -1,
      -1,    -1,    -1,    46,    47,    48,    49,    50,    51,    -1,
      53,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    94,    95,
      96,    97,    98,    66,    67,    68,    -1,    -1,    -1,    94,
      95,    96,    97,    -1,    -1,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   101,    -1,
      -1,   104,   105,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   115,    -1,    -1,   118,   119,    -1,    -1,   122,
     123,   124,     3,    -1,    -1,     6,    -1,    -1,    -1,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    56,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    47,    48,    49,    50,
      51,    -1,    53,    -1,    -1,    56,    94,    95,    96,    97,
      98,    -1,    -1,    -1,    -1,    66,    67,    68,    94,    95,
      96,    97,    98,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,    -1,    -1,    -1,
     101,    -1,    -1,    -1,   105,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   115,    -1,    -1,   118,   119,    -1,
      -1,   122,   123,   124,     3,    -1,    -1,     6,    -1,    -1,
      -1,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      56,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    47,    48,
      49,    50,    51,    -1,    53,    -1,    -1,    56,    94,    95,
      96,    97,    98,    -1,    -1,    -1,    -1,    66,    67,    68,
      94,    95,    96,    97,    -1,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,    -1,    -1,    -1,
     114,   115,   116,   117,   118,   119,   120,   121,   122,    -1,
      -1,    -1,   101,    -1,    -1,    -1,   105,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   115,    -1,    -1,   118,
     119,    -1,    -1,   122,   123,   124,     3,    -1,    -1,     6,
      -1,    -1,    -1,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    56,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      47,    48,    49,    50,    51,    -1,    53,    -1,    -1,    56,
      94,    95,    96,    97,    98,    -1,    -1,    -1,    -1,    66,
      67,    68,    -1,    -1,    -1,    -1,    -1,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   101,    -1,    -1,    -1,   105,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   115,    -1,
      -1,   118,   119,    -1,    -1,   122,   123,   124,     3,    -1,
      -1,     6,    -1,    -1,    -1,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    -1,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    47,    48,    49,    50,    51,    -1,    53,    -1,
      -1,    56,    -1,    -1,    -1,    -1,    -1,     3,    -1,    -1,
       6,    66,    67,    68,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    -1,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    -1,    -1,   101,    -1,    -1,    -1,
     105,    47,    48,    49,    50,    51,    -1,    53,    -1,    -1,
     115,    -1,    -1,   118,   119,    -1,    -1,   122,   123,   124,
      66,    67,    68,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   101,    -1,    -1,    -1,   105,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   115,
      -1,    -1,   118,   119,    -1,    -1,   122,   123,   124,     3,
      -1,    -1,     6,    -1,    -1,    -1,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    -1,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    47,    48,    49,    50,    51,    -1,    53,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,    -1,
      -1,     6,    66,    67,    68,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    -1,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    -1,    -1,    -1,   101,    -1,    -1,
      -1,   105,    47,    48,    49,    50,    51,     3,    53,    -1,
      -1,   115,    -1,    -1,   118,   119,    -1,    -1,   122,   123,
     124,    66,    67,    68,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      46,    47,    48,    49,    50,    51,   101,    -1,    -1,    55,
     105,    -1,    -1,    -1,    -1,    -1,    -1,     3,    -1,    -1,
      66,    67,    68,   118,   119,    -1,    -1,   122,   123,   124,
      -1,    -1,    -1,    -1,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    -1,    -1,    -1,   101,    -1,    -1,   104,   105,
      46,    47,    48,    49,    50,    51,    -1,    -1,    -1,   115,
      -1,    -1,    -1,    -1,     3,    -1,   122,    -1,    -1,    -1,
      66,    67,    68,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    -1,
      -1,    -1,    -1,    -1,    -1,   101,    -1,    -1,   104,   105,
      -1,    50,    51,    -1,    -1,    -1,    -1,    -1,    -1,   115,
      -1,    -1,    -1,    -1,    -1,    -1,   122,    66,    67,    68,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   101,    -1,    -1,    -1,   105,     0,     1,    -1,
       3,     4,    -1,    -1,     7,     8,     9,    -1,    -1,    -1,
      -1,    -1,    -1,   122,    -1,    -1,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    -1,    -1,    -1,    41,    42,
      -1,    -1,    -1,    -1,    47,    48,    49,    50,    51,    -1,
      -1,    54,    -1,    -1,    -1,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    -1,    -1,    -1,    -1,
      73,    74,    75,    -1,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    99,     1,   101,     3,
       4,   104,   105,     7,     8,     9,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    -1,    -1,    -1,    41,    42,    -1,
      -1,    45,    -1,    47,    48,    49,    50,    51,    -1,    -1,
      54,    -1,    -1,    -1,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    -1,    -1,    -1,    -1,    73,
      74,    75,    -1,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    99,     1,   101,     3,     4,
     104,   105,     7,     8,     9,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    -1,    -1,    -1,    41,    42,    -1,    -1,
      45,    -1,    47,    48,    49,    50,    51,    -1,    -1,    54,
      -1,    -1,    -1,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    -1,    -1,    -1,    -1,    73,    74,
      75,    -1,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    99,     1,   101,     3,     4,   104,
     105,     7,     8,     9,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    -1,    -1,    -1,    41,    42,    -1,    -1,    45,
      -1,    47,    48,    49,    50,    51,    -1,    -1,    54,    -1,
      -1,    -1,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    -1,    -1,    -1,    -1,    73,    74,    75,
      -1,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    99,     1,   101,     3,     4,   104,   105,
       7,     8,     9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    -1,    -1,    -1,    41,    42,    -1,    -1,    -1,    -1,
      47,    48,    49,    50,    51,    -1,    -1,    54,    -1,    56,
      -1,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    -1,    -1,    -1,    -1,    73,    74,    75,    -1,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,     1,    -1,     3,    -1,    -1,    -1,
      -1,    -1,    99,    -1,   101,    -1,    -1,   104,   105,    -1,
      -1,    -1,    -1,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    46,
      47,    48,    49,    50,    51,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     1,    -1,     3,    -1,    -1,    66,
      67,    68,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    -1,    -1,    -1,   101,    -1,    -1,   104,   105,    46,
      47,    48,    49,    50,    51,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     1,    -1,     3,    -1,    -1,    -1,    66,
      67,    68,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      -1,    -1,    -1,    -1,   101,    -1,    -1,   104,   105,    47,
      48,    49,    50,    51,     3,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    66,    67,
      68,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    47,    48,
      49,    50,    51,   101,    -1,    -1,    -1,   105,    -1,    -1,
      -1,    -1,    -1,    -1,     3,    -1,    -1,    66,    67,    68,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    -1,
      -1,    -1,   101,    -1,    -1,    -1,   105,   106,    47,    48,
      49,    50,    51,     3,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    11,    -1,    -1,    -1,    -1,    -1,    66,    67,    68,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    46,    47,    48,    49,
      50,    51,   101,    -1,    -1,    -1,   105,   106,    -1,    -1,
      -1,    -1,    -1,     3,    -1,    -1,    66,    67,    68,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    -1,    -1,
      -1,   101,    -1,    -1,   104,   105,    46,    47,    48,    49,
      50,    51,     3,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    66,    67,    68,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    47,    48,    49,    50,
      51,   101,    -1,    -1,   104,   105,    -1,    -1,    -1,    -1,
      -1,     3,    -1,    -1,    -1,    66,    67,    68,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    -1,    -1,    -1,    -1,
     101,    -1,    -1,    -1,   105,    47,    48,    49,    50,    51,
       3,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    66,    67,    68,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    47,    48,    49,    50,    51,   101,
      -1,    -1,    -1,   105,    -1,    -1,    -1,    -1,    -1,     3,
      -1,    -1,    -1,    66,    67,    68,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    -1,    -1,    -1,    -1,   101,    -1,
      -1,    -1,   105,    47,    48,    49,    50,    51,     3,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    66,    67,    68,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    50,    51,   101,    -1,    -1,
      -1,   105,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    66,    67,    68,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   101,    -1,    -1,    -1,
     105
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,   107,   108,   109,   128,   129,   274,     1,     3,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    46,    47,    48,
      49,    50,    51,    66,    67,    68,   101,   104,   105,   213,
     227,   228,   230,   231,   232,   233,   234,   253,   254,   264,
     266,     1,   213,     1,    38,     0,     1,     4,     7,     8,
       9,    19,    41,    42,    54,    58,    59,    60,    61,    62,
      63,    64,    65,    73,    74,    75,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      99,   104,   130,   131,   132,   134,   135,   136,   137,   138,
     141,   142,   144,   145,   146,   147,   148,   149,   150,   153,
     154,   155,   158,   160,   165,   166,   167,   168,   170,   173,
     174,   175,   176,   177,   181,   182,   189,   190,   200,   209,
     274,    91,   261,   274,   261,    46,   264,   126,    91,    41,
     231,   227,    38,    52,    55,    72,   115,   122,   124,   218,
     219,   221,   223,   224,   225,   226,   264,   274,   227,   233,
     264,   103,   126,   265,    41,    41,   210,   211,   213,   274,
     106,    38,     6,   269,    38,   271,   274,     1,     3,   229,
     230,    38,   271,    38,   152,   274,    38,    38,    38,    80,
     264,     3,    44,   264,    38,     4,    44,    38,    38,    41,
      44,     4,   269,    38,   164,   229,   162,   164,    38,    38,
     269,    38,    91,   254,   271,    38,   115,   221,   226,   264,
      66,   229,   254,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    38,    53,   115,   118,   119,   122,   123,   124,
     213,   214,   215,   217,   229,   230,   241,   242,   243,   244,
     269,   274,    46,   105,   266,   254,   227,    38,   115,   210,
     224,   226,   264,    44,   235,   236,    56,   241,   242,   241,
      38,   124,   222,   225,   264,   226,   227,   264,   218,    38,
      55,   218,    38,    55,   115,   222,   225,   264,   102,   266,
     105,   266,    39,    40,   212,   274,     3,   262,   269,     6,
      44,   262,   272,   262,    41,    52,    38,   221,    39,   262,
     264,     3,     3,   262,    11,   159,   210,   210,   264,    41,
      52,   192,    44,     3,   161,   272,     3,   210,    44,   220,
     221,   224,   274,    41,    40,   163,   274,   262,   263,   274,
     139,   140,   269,   210,   185,   186,   187,   213,   253,   274,
     264,   269,     3,   115,   226,   264,   272,    38,   262,   115,
     264,   102,     3,   237,   274,    38,   221,    44,   264,   241,
      38,   241,   241,   241,   241,   241,   241,    92,    40,   216,
     274,    38,    94,    95,    96,    97,    98,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   265,
      92,   115,   226,   264,   223,   264,    39,    39,   115,   223,
     264,   102,    55,   241,    56,   226,   264,   264,    38,    55,
     226,   210,    56,   241,   210,    56,   241,   222,   225,   102,
     115,   222,   265,    41,   213,    39,   169,    40,    52,    39,
     235,   218,    39,    44,    39,    52,    39,    40,   157,    40,
      39,    39,    41,   264,   129,   191,    39,    39,    39,    39,
     162,   164,    39,    39,    40,    44,    39,    92,    40,   188,
     274,    55,   102,    39,   226,   264,    41,   102,    41,    44,
     210,   264,    76,   172,   218,   227,   179,    41,    72,   245,
     246,   274,    39,   115,   122,   226,   229,   217,   241,   241,
     241,   241,   241,   241,   241,   241,   241,   241,   241,   241,
     241,   241,   241,   241,   241,   254,   264,   102,    39,    39,
     102,   223,   241,   222,   264,    39,   102,   210,    56,   241,
      39,    56,    39,    56,   115,   222,   225,   222,   212,     4,
      44,   269,   129,   272,   139,   243,   269,   273,    41,    41,
     133,     4,   151,   269,     4,    41,    44,   100,   156,   221,
     269,   270,   262,   269,   273,    39,   213,   221,    44,    41,
      45,   129,    42,   209,   162,    41,    44,    38,    45,   163,
       3,   101,   105,   267,    41,   272,   213,   156,   183,   187,
     143,   221,   269,   102,     3,   238,   239,   274,    39,    38,
      40,    41,    44,   171,    76,   218,     1,    41,    62,    69,
      70,    71,    74,   124,   134,   135,   136,   137,   141,   142,
     146,   148,   150,   153,   155,   158,   160,   165,   166,   167,
     168,   181,   182,   189,   193,   196,   197,   198,   199,   200,
     201,   202,   205,   208,   209,   274,   247,    44,   241,    39,
     122,   227,    39,   115,   219,   216,    72,   264,    39,    56,
      39,   222,    39,    56,   222,    45,    40,    40,   193,    38,
      76,   227,   256,   274,    52,    39,    40,   157,   156,   221,
     256,    45,   269,     3,   229,    41,    52,   270,   210,   103,
     126,   268,   126,    91,    39,    45,   170,   177,   181,   182,
     184,   197,   199,   209,   188,   129,   256,    41,    52,    40,
      45,    38,    52,   256,   257,   210,   221,    38,   195,    44,
      72,    72,    72,   124,   266,    45,   193,   106,   229,   254,
     264,    74,   248,   249,   255,   274,   178,   241,   241,    39,
      39,   241,   218,    39,   272,   272,    45,   210,    38,    76,
     156,   269,   273,    41,   221,    39,   256,    41,    41,   221,
     164,    39,     3,     3,   105,     3,   105,   214,     4,    44,
     229,    56,    41,   240,   241,   239,    41,   221,   210,   235,
      72,   258,   274,    39,   172,   210,   193,   194,   266,    38,
     221,   229,    38,    72,   245,   264,    41,    40,    69,    70,
      71,   250,   252,   193,   241,    39,   210,    38,   157,   256,
      41,   221,   156,    41,    41,   268,   268,    92,   172,    39,
      41,   259,   260,   264,    41,    44,   218,   171,    39,   193,
      38,   210,   172,    38,   115,   226,   210,   241,    44,   245,
     249,   264,   251,    45,    41,    39,   210,    41,   256,    41,
     171,    41,    44,    40,    38,   218,    45,   210,    39,   171,
     210,    38,    38,   115,    39,    41,   204,    44,   255,    41,
     180,   221,    39,    41,   260,   193,    39,   206,   256,    39,
     210,   210,    38,   257,   180,   203,   264,   172,   207,   256,
      41,    44,   207,    39,    39,   210,   180,   171,    41,    44,
      52,   207,   207,    39,   235,   207,    41
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1455 of yacc.c  */
#line 1745 "parser.y"
    {
                   if (!classes) classes = NewHash();
		   Setattr((yyvsp[(1) - (1)].node),"classes",classes); 
		   Setattr((yyvsp[(1) - (1)].node),"name",ModuleName);
		   
		   if ((!module_node) && ModuleName) {
		     module_node = new_node("module");
		     Setattr(module_node,"name",ModuleName);
		   }
		   Setattr((yyvsp[(1) - (1)].node),"module",module_node);
		   check_extensions();
	           top = (yyvsp[(1) - (1)].node);
               }
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 1758 "parser.y"
    {
                 top = Copy(Getattr((yyvsp[(2) - (3)].p),"type"));
		 Delete((yyvsp[(2) - (3)].p));
               }
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 1762 "parser.y"
    {
                 top = 0;
               }
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 1765 "parser.y"
    {
                 top = (yyvsp[(2) - (3)].p);
               }
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 1768 "parser.y"
    {
                 top = 0;
               }
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 1771 "parser.y"
    {
                 top = (yyvsp[(3) - (5)].pl);
               }
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 1774 "parser.y"
    {
                 top = 0;
               }
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 1779 "parser.y"
    {  
                   /* add declaration to end of linked list (the declaration isn't always a single declaration, sometimes it is a linked list itself) */
                   appendChild((yyvsp[(1) - (2)].node),(yyvsp[(2) - (2)].node));
                   (yyval.node) = (yyvsp[(1) - (2)].node);
               }
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 1784 "parser.y"
    {
                   (yyval.node) = new_node("top");
               }
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 1789 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 1790 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 1791 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 1792 "parser.y"
    { (yyval.node) = 0; }
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 1793 "parser.y"
    {
                  (yyval.node) = 0;
		  Swig_error(cparse_file, cparse_line,"Syntax error in input(1).\n");
		  exit(1);
               }
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 1799 "parser.y"
    { 
                  if ((yyval.node)) {
   		      add_symbols((yyval.node));
                  }
                  (yyval.node) = (yyvsp[(1) - (1)].node); 
	       }
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 1815 "parser.y"
    {
                  (yyval.node) = 0;
                  skip_decl();
               }
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 1825 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 1826 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 1827 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 1828 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 1829 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 1830 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 1831 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 1832 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 1833 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 1834 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 1835 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 1836 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 1837 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 1838 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 1839 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 1840 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 1841 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 1842 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 1843 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 1844 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 1845 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 1852 "parser.y"
    {
               Node *cls;
	       String *clsname;
	       cplus_mode = CPLUS_PUBLIC;
	       if (!classes) classes = NewHash();
	       if (!extendhash) extendhash = NewHash();
	       clsname = make_class_name((yyvsp[(3) - (4)].str));
	       cls = Getattr(classes,clsname);
	       if (!cls) {
		 /* No previous definition. Create a new scope */
		 Node *am = Getattr(extendhash,clsname);
		 if (!am) {
		   Swig_symbol_newscope();
		   Swig_symbol_setscopename((yyvsp[(3) - (4)].str));
		   prev_symtab = 0;
		 } else {
		   prev_symtab = Swig_symbol_setscope(Getattr(am,"symtab"));
		 }
		 current_class = 0;
	       } else {
		 /* Previous class definition.  Use its symbol table */
		 prev_symtab = Swig_symbol_setscope(Getattr(cls,"symtab"));
		 current_class = cls;
		 extendmode = 1;
	       }
	       Classprefix = NewString((yyvsp[(3) - (4)].str));
	       Namespaceprefix= Swig_symbol_qualifiedscopename(0);
	       Delete(clsname);
	     }
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 1880 "parser.y"
    {
               String *clsname;
	       extendmode = 0;
               (yyval.node) = new_node("extend");
	       Setattr((yyval.node),"symtab",Swig_symbol_popscope());
	       if (prev_symtab) {
		 Swig_symbol_setscope(prev_symtab);
	       }
	       Namespaceprefix = Swig_symbol_qualifiedscopename(0);
               clsname = make_class_name((yyvsp[(3) - (7)].str));
	       Setattr((yyval.node),"name",clsname);

	       /* Mark members as extend */

	       tag_nodes((yyvsp[(6) - (7)].node),"feature:extend",(char*) "1");
	       if (current_class) {
		 /* We add the extension to the previously defined class */
		 appendChild((yyval.node),(yyvsp[(6) - (7)].node));
		 appendChild(current_class,(yyval.node));
	       } else {
		 /* We store the extensions in the extensions hash */
		 Node *am = Getattr(extendhash,clsname);
		 if (am) {
		   /* Append the members to the previous extend methods */
		   appendChild(am,(yyvsp[(6) - (7)].node));
		 } else {
		   appendChild((yyval.node),(yyvsp[(6) - (7)].node));
		   Setattr(extendhash,clsname,(yyval.node));
		 }
	       }
	       current_class = 0;
	       Delete(Classprefix);
	       Delete(clsname);
	       Classprefix = 0;
	       prev_symtab = 0;
	       (yyval.node) = 0;

	     }
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 1924 "parser.y"
    {
                    (yyval.node) = new_node("apply");
                    Setattr((yyval.node),"pattern",Getattr((yyvsp[(2) - (5)].p),"pattern"));
		    appendChild((yyval.node),(yyvsp[(4) - (5)].p));
               }
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 1934 "parser.y"
    {
		 (yyval.node) = new_node("clear");
		 appendChild((yyval.node),(yyvsp[(2) - (3)].p));
               }
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 1945 "parser.y"
    {
		   if (((yyvsp[(4) - (5)].dtype).type != T_ERROR) && ((yyvsp[(4) - (5)].dtype).type != T_SYMBOL)) {
		     SwigType *type = NewSwigType((yyvsp[(4) - (5)].dtype).type);
		     (yyval.node) = new_node("constant");
		     Setattr((yyval.node),"name",(yyvsp[(2) - (5)].id));
		     Setattr((yyval.node),"type",type);
		     Setattr((yyval.node),"value",(yyvsp[(4) - (5)].dtype).val);
		     if ((yyvsp[(4) - (5)].dtype).rawval) Setattr((yyval.node),"rawval", (yyvsp[(4) - (5)].dtype).rawval);
		     Setattr((yyval.node),"storage","%constant");
		     SetFlag((yyval.node),"feature:immutable");
		     add_symbols((yyval.node));
		     Delete(type);
		   } else {
		     if ((yyvsp[(4) - (5)].dtype).type == T_ERROR) {
		       Swig_warning(WARN_PARSE_UNSUPPORTED_VALUE,cparse_file,cparse_line,"Unsupported constant value (ignored)\n");
		     }
		     (yyval.node) = 0;
		   }

	       }
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 1966 "parser.y"
    {
		 if (((yyvsp[(4) - (5)].dtype).type != T_ERROR) && ((yyvsp[(4) - (5)].dtype).type != T_SYMBOL)) {
		   SwigType_push((yyvsp[(2) - (5)].type),(yyvsp[(3) - (5)].decl).type);
		   /* Sneaky callback function trick */
		   if (SwigType_isfunction((yyvsp[(2) - (5)].type))) {
		     SwigType_add_pointer((yyvsp[(2) - (5)].type));
		   }
		   (yyval.node) = new_node("constant");
		   Setattr((yyval.node),"name",(yyvsp[(3) - (5)].decl).id);
		   Setattr((yyval.node),"type",(yyvsp[(2) - (5)].type));
		   Setattr((yyval.node),"value",(yyvsp[(4) - (5)].dtype).val);
		   if ((yyvsp[(4) - (5)].dtype).rawval) Setattr((yyval.node),"rawval", (yyvsp[(4) - (5)].dtype).rawval);
		   Setattr((yyval.node),"storage","%constant");
		   SetFlag((yyval.node),"feature:immutable");
		   add_symbols((yyval.node));
		 } else {
		     if ((yyvsp[(4) - (5)].dtype).type == T_ERROR) {
		       Swig_warning(WARN_PARSE_UNSUPPORTED_VALUE,cparse_file,cparse_line,"Unsupported constant value\n");
		     }
		   (yyval.node) = 0;
		 }
               }
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 1988 "parser.y"
    {
		 Swig_warning(WARN_PARSE_BAD_VALUE,cparse_file,cparse_line,"Bad constant value (ignored).\n");
		 (yyval.node) = 0;
	       }
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 1999 "parser.y"
    {
		 char temp[64];
		 Replace((yyvsp[(2) - (2)].str),"$file",cparse_file, DOH_REPLACE_ANY);
		 sprintf(temp,"%d", cparse_line);
		 Replace((yyvsp[(2) - (2)].str),"$line",temp,DOH_REPLACE_ANY);
		 Printf(stderr,"%s\n", (yyvsp[(2) - (2)].str));
		 Delete((yyvsp[(2) - (2)].str));
                 (yyval.node) = 0;
	       }
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 2008 "parser.y"
    {
		 char temp[64];
		 String *s = NewString((yyvsp[(2) - (2)].id));
		 Replace(s,"$file",cparse_file, DOH_REPLACE_ANY);
		 sprintf(temp,"%d", cparse_line);
		 Replace(s,"$line",temp,DOH_REPLACE_ANY);
		 Printf(stderr,"%s\n", s);
		 Delete(s);
                 (yyval.node) = 0;
               }
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 2027 "parser.y"
    {
                    skip_balanced('{','}');
		    (yyval.node) = 0;
		    Swig_warning(WARN_DEPRECATED_EXCEPT,cparse_file, cparse_line, "%%except is deprecated.  Use %%exception instead.\n");
	       }
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 2033 "parser.y"
    {
                    skip_balanced('{','}');
		    (yyval.node) = 0;
		    Swig_warning(WARN_DEPRECATED_EXCEPT,cparse_file, cparse_line, "%%except is deprecated.  Use %%exception instead.\n");
               }
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 2039 "parser.y"
    {
		 (yyval.node) = 0;
		 Swig_warning(WARN_DEPRECATED_EXCEPT,cparse_file, cparse_line, "%%except is deprecated.  Use %%exception instead.\n");
               }
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 2044 "parser.y"
    {
		 (yyval.node) = 0;
		 Swig_warning(WARN_DEPRECATED_EXCEPT,cparse_file, cparse_line, "%%except is deprecated.  Use %%exception instead.\n");
	       }
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 2051 "parser.y"
    {		 
                 (yyval.node) = NewHash();
                 Setattr((yyval.node),"value",(yyvsp[(1) - (4)].id));
		 Setattr((yyval.node),"type",Getattr((yyvsp[(3) - (4)].p),"type"));
               }
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 2058 "parser.y"
    {
                 (yyval.node) = NewHash();
                 Setattr((yyval.node),"value",(yyvsp[(1) - (1)].id));
              }
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 2062 "parser.y"
    {
                (yyval.node) = (yyvsp[(1) - (1)].node);
              }
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 2075 "parser.y"
    {
                   Hash *p = (yyvsp[(5) - (7)].node);
		   (yyval.node) = new_node("fragment");
		   Setattr((yyval.node),"value",Getattr((yyvsp[(3) - (7)].node),"value"));
		   Setattr((yyval.node),"type",Getattr((yyvsp[(3) - (7)].node),"type"));
		   Setattr((yyval.node),"section",Getattr(p,"name"));
		   Setattr((yyval.node),"kwargs",nextSibling(p));
		   Setattr((yyval.node),"code",(yyvsp[(7) - (7)].str));
                 }
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 2084 "parser.y"
    {
		   Hash *p = (yyvsp[(5) - (7)].node);
		   String *code;
                   skip_balanced('{','}');
		   (yyval.node) = new_node("fragment");
		   Setattr((yyval.node),"value",Getattr((yyvsp[(3) - (7)].node),"value"));
		   Setattr((yyval.node),"type",Getattr((yyvsp[(3) - (7)].node),"type"));
		   Setattr((yyval.node),"section",Getattr(p,"name"));
		   Setattr((yyval.node),"kwargs",nextSibling(p));
		   Delitem(scanner_ccode,0);
		   Delitem(scanner_ccode,DOH_END);
		   code = Copy(scanner_ccode);
		   Setattr((yyval.node),"code",code);
		   Delete(code);
                 }
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 2099 "parser.y"
    {
		   (yyval.node) = new_node("fragment");
		   Setattr((yyval.node),"value",Getattr((yyvsp[(3) - (5)].node),"value"));
		   Setattr((yyval.node),"type",Getattr((yyvsp[(3) - (5)].node),"type"));
		   Setattr((yyval.node),"emitonly","1");
		 }
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 2112 "parser.y"
    {
                     (yyvsp[(1) - (4)].loc).filename = Copy(cparse_file);
		     (yyvsp[(1) - (4)].loc).line = cparse_line;
		     scanner_set_location(NewString((yyvsp[(3) - (4)].id)),1);
                     if ((yyvsp[(2) - (4)].node)) { 
		       String *maininput = Getattr((yyvsp[(2) - (4)].node), "maininput");
		       if (maininput)
		         scanner_set_main_input_file(NewString(maininput));
		     }
               }
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 2121 "parser.y"
    {
                     String *mname = 0;
                     (yyval.node) = (yyvsp[(6) - (7)].node);
		     scanner_set_location((yyvsp[(1) - (7)].loc).filename,(yyvsp[(1) - (7)].loc).line+1);
		     if (strcmp((yyvsp[(1) - (7)].loc).type,"include") == 0) set_nodeType((yyval.node),"include");
		     if (strcmp((yyvsp[(1) - (7)].loc).type,"import") == 0) {
		       mname = (yyvsp[(2) - (7)].node) ? Getattr((yyvsp[(2) - (7)].node),"module") : 0;
		       set_nodeType((yyval.node),"import");
		       if (import_mode) --import_mode;
		     }
		     
		     Setattr((yyval.node),"name",(yyvsp[(3) - (7)].id));
		     /* Search for the module (if any) */
		     {
			 Node *n = firstChild((yyval.node));
			 while (n) {
			     if (Strcmp(nodeType(n),"module") == 0) {
			         if (mname) {
				   Setattr(n,"name", mname);
				   mname = 0;
				 }
				 Setattr((yyval.node),"module",Getattr(n,"name"));
				 break;
			     }
			     n = nextSibling(n);
			 }
			 if (mname) {
			   /* There is no module node in the import
			      node, ie, you imported a .h file
			      directly.  We are forced then to create
			      a new import node with a module node.
			   */			      
			   Node *nint = new_node("import");
			   Node *mnode = new_node("module");
			   Setattr(mnode,"name", mname);
			   appendChild(nint,mnode);
			   Delete(mnode);
			   appendChild(nint,firstChild((yyval.node)));
			   (yyval.node) = nint;
			   Setattr((yyval.node),"module",mname);
			 }
		     }
		     Setattr((yyval.node),"options",(yyvsp[(2) - (7)].node));
               }
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 2167 "parser.y"
    { (yyval.loc).type = (char *) "include"; }
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 2168 "parser.y"
    { (yyval.loc).type = (char *) "import"; ++import_mode;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 2175 "parser.y"
    {
                 String *cpps;
		 if (Namespaceprefix) {
		   Swig_error(cparse_file, cparse_start_line, "%%inline directive inside a namespace is disallowed.\n");
		   (yyval.node) = 0;
		 } else {
		   (yyval.node) = new_node("insert");
		   Setattr((yyval.node),"code",(yyvsp[(2) - (2)].str));
		   /* Need to run through the preprocessor */
		   Seek((yyvsp[(2) - (2)].str),0,SEEK_SET);
		   Setline((yyvsp[(2) - (2)].str),cparse_start_line);
		   Setfile((yyvsp[(2) - (2)].str),cparse_file);
		   cpps = Preprocessor_parse((yyvsp[(2) - (2)].str));
		   start_inline(Char(cpps), cparse_start_line);
		   Delete((yyvsp[(2) - (2)].str));
		   Delete(cpps);
		 }
		 
	       }
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 2194 "parser.y"
    {
                 String *cpps;
		 int start_line = cparse_line;
		 skip_balanced('{','}');
		 if (Namespaceprefix) {
		   Swig_error(cparse_file, cparse_start_line, "%%inline directive inside a namespace is disallowed.\n");
		   
		   (yyval.node) = 0;
		 } else {
		   String *code;
                   (yyval.node) = new_node("insert");
		   Delitem(scanner_ccode,0);
		   Delitem(scanner_ccode,DOH_END);
		   code = Copy(scanner_ccode);
		   Setattr((yyval.node),"code", code);
		   Delete(code);		   
		   cpps=Copy(scanner_ccode);
		   start_inline(Char(cpps), start_line);
		   Delete(cpps);
		 }
               }
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 2225 "parser.y"
    {
                 (yyval.node) = new_node("insert");
		 Setattr((yyval.node),"code",(yyvsp[(1) - (1)].str));
	       }
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 2229 "parser.y"
    {
		 String *code = NewStringEmpty();
		 (yyval.node) = new_node("insert");
		 Setattr((yyval.node),"section",(yyvsp[(3) - (5)].id));
		 Setattr((yyval.node),"code",code);
		 if (Swig_insert_file((yyvsp[(5) - (5)].id),code) < 0) {
		   Swig_error(cparse_file, cparse_line, "Couldn't find '%s'.\n", (yyvsp[(5) - (5)].id));
		   (yyval.node) = 0;
		 } 
               }
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 2239 "parser.y"
    {
		 (yyval.node) = new_node("insert");
		 Setattr((yyval.node),"section",(yyvsp[(3) - (5)].id));
		 Setattr((yyval.node),"code",(yyvsp[(5) - (5)].str));
               }
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 2244 "parser.y"
    {
		 String *code;
                 skip_balanced('{','}');
		 (yyval.node) = new_node("insert");
		 Setattr((yyval.node),"section",(yyvsp[(3) - (5)].id));
		 Delitem(scanner_ccode,0);
		 Delitem(scanner_ccode,DOH_END);
		 code = Copy(scanner_ccode);
		 Setattr((yyval.node),"code", code);
		 Delete(code);
	       }
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 2262 "parser.y"
    {
                 (yyval.node) = new_node("module");
		 if ((yyvsp[(2) - (3)].node)) {
		   Setattr((yyval.node),"options",(yyvsp[(2) - (3)].node));
		   if (Getattr((yyvsp[(2) - (3)].node),"directors")) {
		     Wrapper_director_mode_set(1);
		   } 
		   if (Getattr((yyvsp[(2) - (3)].node),"dirprot")) {
		     Wrapper_director_protected_mode_set(1);
		   } 
		   if (Getattr((yyvsp[(2) - (3)].node),"allprotected")) {
		     Wrapper_all_protected_mode_set(1);
		   } 
		   if (Getattr((yyvsp[(2) - (3)].node),"templatereduce")) {
		     template_reduce = 1;
		   }
		   if (Getattr((yyvsp[(2) - (3)].node),"notemplatereduce")) {
		     template_reduce = 0;
		   }
		 }
		 if (!ModuleName) ModuleName = NewString((yyvsp[(3) - (3)].id));
		 if (!import_mode) {
		   /* first module included, we apply global
		      ModuleName, which can be modify by -module */
		   String *mname = Copy(ModuleName);
		   Setattr((yyval.node),"name",mname);
		   Delete(mname);
		 } else { 
		   /* import mode, we just pass the idstring */
		   Setattr((yyval.node),"name",(yyvsp[(3) - (3)].id));   
		 }		 
		 if (!module_node) module_node = (yyval.node);
	       }
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 2302 "parser.y"
    {
                 Swig_warning(WARN_DEPRECATED_NAME,cparse_file,cparse_line, "%%name is deprecated.  Use %%rename instead.\n");
		 Delete(yyrename);
                 yyrename = NewString((yyvsp[(3) - (4)].id));
		 (yyval.node) = 0;
               }
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 2308 "parser.y"
    {
		 Swig_warning(WARN_DEPRECATED_NAME,cparse_file,cparse_line, "%%name is deprecated.  Use %%rename instead.\n");
		 (yyval.node) = 0;
		 Swig_error(cparse_file,cparse_line,"Missing argument to %%name directive.\n");
	       }
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 2321 "parser.y"
    {
                 (yyval.node) = new_node("native");
		 Setattr((yyval.node),"name",(yyvsp[(3) - (7)].id));
		 Setattr((yyval.node),"wrap:name",(yyvsp[(6) - (7)].id));
	         add_symbols((yyval.node));
	       }
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 2327 "parser.y"
    {
		 if (!SwigType_isfunction((yyvsp[(7) - (8)].decl).type)) {
		   Swig_error(cparse_file,cparse_line,"%%native declaration '%s' is not a function.\n", (yyvsp[(7) - (8)].decl).id);
		   (yyval.node) = 0;
		 } else {
		     Delete(SwigType_pop_function((yyvsp[(7) - (8)].decl).type));
		     /* Need check for function here */
		     SwigType_push((yyvsp[(6) - (8)].type),(yyvsp[(7) - (8)].decl).type);
		     (yyval.node) = new_node("native");
	             Setattr((yyval.node),"name",(yyvsp[(3) - (8)].id));
		     Setattr((yyval.node),"wrap:name",(yyvsp[(7) - (8)].decl).id);
		     Setattr((yyval.node),"type",(yyvsp[(6) - (8)].type));
		     Setattr((yyval.node),"parms",(yyvsp[(7) - (8)].decl).parms);
		     Setattr((yyval.node),"decl",(yyvsp[(7) - (8)].decl).type);
		 }
	         add_symbols((yyval.node));
	       }
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 2353 "parser.y"
    {
                 (yyval.node) = new_node("pragma");
		 Setattr((yyval.node),"lang",(yyvsp[(2) - (5)].id));
		 Setattr((yyval.node),"name",(yyvsp[(3) - (5)].id));
		 Setattr((yyval.node),"value",(yyvsp[(5) - (5)].str));
	       }
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 2359 "parser.y"
    {
		(yyval.node) = new_node("pragma");
		Setattr((yyval.node),"lang",(yyvsp[(2) - (3)].id));
		Setattr((yyval.node),"name",(yyvsp[(3) - (3)].id));
	      }
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 2366 "parser.y"
    { (yyval.str) = NewString((yyvsp[(1) - (1)].id)); }
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 2367 "parser.y"
    { (yyval.str) = (yyvsp[(1) - (1)].str); }
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 2370 "parser.y"
    { (yyval.id) = (yyvsp[(2) - (3)].id); }
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 2371 "parser.y"
    { (yyval.id) = (char *) "swig"; }
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 2379 "parser.y"
    {
                SwigType *t = (yyvsp[(2) - (4)].decl).type;
		Hash *kws = NewHash();
		String *fixname;
		fixname = feature_identifier_fix((yyvsp[(2) - (4)].decl).id);
		Setattr(kws,"name",(yyvsp[(3) - (4)].id));
		if (!Len(t)) t = 0;
		/* Special declarator check */
		if (t) {
		  if (SwigType_isfunction(t)) {
		    SwigType *decl = SwigType_pop_function(t);
		    if (SwigType_ispointer(t)) {
		      String *nname = NewStringf("*%s",fixname);
		      if ((yyvsp[(1) - (4)].intvalue)) {
			Swig_name_rename_add(Namespaceprefix, nname,decl,kws,(yyvsp[(2) - (4)].decl).parms);
		      } else {
			Swig_name_namewarn_add(Namespaceprefix,nname,decl,kws);
		      }
		      Delete(nname);
		    } else {
		      if ((yyvsp[(1) - (4)].intvalue)) {
			Swig_name_rename_add(Namespaceprefix,(fixname),decl,kws,(yyvsp[(2) - (4)].decl).parms);
		      } else {
			Swig_name_namewarn_add(Namespaceprefix,(fixname),decl,kws);
		      }
		    }
		    Delete(decl);
		  } else if (SwigType_ispointer(t)) {
		    String *nname = NewStringf("*%s",fixname);
		    if ((yyvsp[(1) - (4)].intvalue)) {
		      Swig_name_rename_add(Namespaceprefix,(nname),0,kws,(yyvsp[(2) - (4)].decl).parms);
		    } else {
		      Swig_name_namewarn_add(Namespaceprefix,(nname),0,kws);
		    }
		    Delete(nname);
		  }
		} else {
		  if ((yyvsp[(1) - (4)].intvalue)) {
		    Swig_name_rename_add(Namespaceprefix,(fixname),0,kws,(yyvsp[(2) - (4)].decl).parms);
		  } else {
		    Swig_name_namewarn_add(Namespaceprefix,(fixname),0,kws);
		  }
		}
                (yyval.node) = 0;
		scanner_clear_rename();
              }
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 2425 "parser.y"
    {
		String *fixname;
		Hash *kws = (yyvsp[(3) - (7)].node);
		SwigType *t = (yyvsp[(5) - (7)].decl).type;
		fixname = feature_identifier_fix((yyvsp[(5) - (7)].decl).id);
		if (!Len(t)) t = 0;
		/* Special declarator check */
		if (t) {
		  if ((yyvsp[(6) - (7)].dtype).qualifier) SwigType_push(t,(yyvsp[(6) - (7)].dtype).qualifier);
		  if (SwigType_isfunction(t)) {
		    SwigType *decl = SwigType_pop_function(t);
		    if (SwigType_ispointer(t)) {
		      String *nname = NewStringf("*%s",fixname);
		      if ((yyvsp[(1) - (7)].intvalue)) {
			Swig_name_rename_add(Namespaceprefix, nname,decl,kws,(yyvsp[(5) - (7)].decl).parms);
		      } else {
			Swig_name_namewarn_add(Namespaceprefix,nname,decl,kws);
		      }
		      Delete(nname);
		    } else {
		      if ((yyvsp[(1) - (7)].intvalue)) {
			Swig_name_rename_add(Namespaceprefix,(fixname),decl,kws,(yyvsp[(5) - (7)].decl).parms);
		      } else {
			Swig_name_namewarn_add(Namespaceprefix,(fixname),decl,kws);
		      }
		    }
		    Delete(decl);
		  } else if (SwigType_ispointer(t)) {
		    String *nname = NewStringf("*%s",fixname);
		    if ((yyvsp[(1) - (7)].intvalue)) {
		      Swig_name_rename_add(Namespaceprefix,(nname),0,kws,(yyvsp[(5) - (7)].decl).parms);
		    } else {
		      Swig_name_namewarn_add(Namespaceprefix,(nname),0,kws);
		    }
		    Delete(nname);
		  }
		} else {
		  if ((yyvsp[(1) - (7)].intvalue)) {
		    Swig_name_rename_add(Namespaceprefix,(fixname),0,kws,(yyvsp[(5) - (7)].decl).parms);
		  } else {
		    Swig_name_namewarn_add(Namespaceprefix,(fixname),0,kws);
		  }
		}
                (yyval.node) = 0;
		scanner_clear_rename();
              }
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 2471 "parser.y"
    {
		if ((yyvsp[(1) - (6)].intvalue)) {
		  Swig_name_rename_add(Namespaceprefix,(yyvsp[(5) - (6)].id),0,(yyvsp[(3) - (6)].node),0);
		} else {
		  Swig_name_namewarn_add(Namespaceprefix,(yyvsp[(5) - (6)].id),0,(yyvsp[(3) - (6)].node));
		}
		(yyval.node) = 0;
		scanner_clear_rename();
              }
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 2482 "parser.y"
    {
		    (yyval.intvalue) = 1;
                }
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 2485 "parser.y"
    {
                    (yyval.intvalue) = 0;
                }
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 2512 "parser.y"
    {
                    String *val = (yyvsp[(7) - (7)].str) ? NewString((yyvsp[(7) - (7)].str)) : NewString("1");
                    new_feature((yyvsp[(3) - (7)].id), val, 0, (yyvsp[(5) - (7)].decl).id, (yyvsp[(5) - (7)].decl).type, (yyvsp[(5) - (7)].decl).parms, (yyvsp[(6) - (7)].dtype).qualifier);
                    (yyval.node) = 0;
                    scanner_clear_rename();
                  }
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 2518 "parser.y"
    {
                    String *val = Len((yyvsp[(5) - (9)].id)) ? NewString((yyvsp[(5) - (9)].id)) : 0;
                    new_feature((yyvsp[(3) - (9)].id), val, 0, (yyvsp[(7) - (9)].decl).id, (yyvsp[(7) - (9)].decl).type, (yyvsp[(7) - (9)].decl).parms, (yyvsp[(8) - (9)].dtype).qualifier);
                    (yyval.node) = 0;
                    scanner_clear_rename();
                  }
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 2524 "parser.y"
    {
                    String *val = (yyvsp[(8) - (8)].str) ? NewString((yyvsp[(8) - (8)].str)) : NewString("1");
                    new_feature((yyvsp[(3) - (8)].id), val, (yyvsp[(4) - (8)].node), (yyvsp[(6) - (8)].decl).id, (yyvsp[(6) - (8)].decl).type, (yyvsp[(6) - (8)].decl).parms, (yyvsp[(7) - (8)].dtype).qualifier);
                    (yyval.node) = 0;
                    scanner_clear_rename();
                  }
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 2530 "parser.y"
    {
                    String *val = Len((yyvsp[(5) - (10)].id)) ? NewString((yyvsp[(5) - (10)].id)) : 0;
                    new_feature((yyvsp[(3) - (10)].id), val, (yyvsp[(6) - (10)].node), (yyvsp[(8) - (10)].decl).id, (yyvsp[(8) - (10)].decl).type, (yyvsp[(8) - (10)].decl).parms, (yyvsp[(9) - (10)].dtype).qualifier);
                    (yyval.node) = 0;
                    scanner_clear_rename();
                  }
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 2538 "parser.y"
    {
                    String *val = (yyvsp[(5) - (5)].str) ? NewString((yyvsp[(5) - (5)].str)) : NewString("1");
                    new_feature((yyvsp[(3) - (5)].id), val, 0, 0, 0, 0, 0);
                    (yyval.node) = 0;
                    scanner_clear_rename();
                  }
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 2544 "parser.y"
    {
                    String *val = Len((yyvsp[(5) - (7)].id)) ? NewString((yyvsp[(5) - (7)].id)) : 0;
                    new_feature((yyvsp[(3) - (7)].id), val, 0, 0, 0, 0, 0);
                    (yyval.node) = 0;
                    scanner_clear_rename();
                  }
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 2550 "parser.y"
    {
                    String *val = (yyvsp[(6) - (6)].str) ? NewString((yyvsp[(6) - (6)].str)) : NewString("1");
                    new_feature((yyvsp[(3) - (6)].id), val, (yyvsp[(4) - (6)].node), 0, 0, 0, 0);
                    (yyval.node) = 0;
                    scanner_clear_rename();
                  }
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 2556 "parser.y"
    {
                    String *val = Len((yyvsp[(5) - (8)].id)) ? NewString((yyvsp[(5) - (8)].id)) : 0;
                    new_feature((yyvsp[(3) - (8)].id), val, (yyvsp[(6) - (8)].node), 0, 0, 0, 0);
                    (yyval.node) = 0;
                    scanner_clear_rename();
                  }
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 2564 "parser.y"
    { (yyval.str) = (yyvsp[(1) - (1)].str); }
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 2565 "parser.y"
    { (yyval.str) = 0; }
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 2566 "parser.y"
    { (yyval.str) = (yyvsp[(3) - (5)].pl); }
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 2569 "parser.y"
    {
		  (yyval.node) = NewHash();
		  Setattr((yyval.node),"name",(yyvsp[(2) - (4)].id));
		  Setattr((yyval.node),"value",(yyvsp[(4) - (4)].id));
                }
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 2574 "parser.y"
    {
		  (yyval.node) = NewHash();
		  Setattr((yyval.node),"name",(yyvsp[(2) - (5)].id));
		  Setattr((yyval.node),"value",(yyvsp[(4) - (5)].id));
                  set_nextSibling((yyval.node),(yyvsp[(5) - (5)].node));
                }
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 2584 "parser.y"
    {
                 Parm *val;
		 String *name;
		 SwigType *t;
		 if (Namespaceprefix) name = NewStringf("%s::%s", Namespaceprefix, (yyvsp[(5) - (7)].decl).id);
		 else name = NewString((yyvsp[(5) - (7)].decl).id);
		 val = (yyvsp[(3) - (7)].pl);
		 if ((yyvsp[(5) - (7)].decl).parms) {
		   Setmeta(val,"parms",(yyvsp[(5) - (7)].decl).parms);
		 }
		 t = (yyvsp[(5) - (7)].decl).type;
		 if (!Len(t)) t = 0;
		 if (t) {
		   if ((yyvsp[(6) - (7)].dtype).qualifier) SwigType_push(t,(yyvsp[(6) - (7)].dtype).qualifier);
		   if (SwigType_isfunction(t)) {
		     SwigType *decl = SwigType_pop_function(t);
		     if (SwigType_ispointer(t)) {
		       String *nname = NewStringf("*%s",name);
		       Swig_feature_set(Swig_cparse_features(), nname, decl, "feature:varargs", val, 0);
		       Delete(nname);
		     } else {
		       Swig_feature_set(Swig_cparse_features(), name, decl, "feature:varargs", val, 0);
		     }
		     Delete(decl);
		   } else if (SwigType_ispointer(t)) {
		     String *nname = NewStringf("*%s",name);
		     Swig_feature_set(Swig_cparse_features(),nname,0,"feature:varargs",val, 0);
		     Delete(nname);
		   }
		 } else {
		   Swig_feature_set(Swig_cparse_features(),name,0,"feature:varargs",val, 0);
		 }
		 Delete(name);
		 (yyval.node) = 0;
              }
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 2620 "parser.y"
    { (yyval.pl) = (yyvsp[(1) - (1)].pl); }
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 2621 "parser.y"
    { 
		  int i;
		  int n;
		  Parm *p;
		  n = atoi(Char((yyvsp[(1) - (3)].dtype).val));
		  if (n <= 0) {
		    Swig_error(cparse_file, cparse_line,"Argument count in %%varargs must be positive.\n");
		    (yyval.pl) = 0;
		  } else {
		    String *name = Getattr((yyvsp[(3) - (3)].p), "name");
		    (yyval.pl) = Copy((yyvsp[(3) - (3)].p));
		    if (name)
		      Setattr((yyval.pl), "name", NewStringf("%s%d", name, n));
		    for (i = 1; i < n; i++) {
		      p = Copy((yyvsp[(3) - (3)].p));
		      name = Getattr(p, "name");
		      if (name)
		        Setattr(p, "name", NewStringf("%s%d", name, n-i));
		      set_nextSibling(p,(yyval.pl));
		      Delete((yyval.pl));
		      (yyval.pl) = p;
		    }
		  }
                }
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 2656 "parser.y"
    {
		   (yyval.node) = 0;
		   if ((yyvsp[(3) - (6)].tmap).method) {
		     String *code = 0;
		     (yyval.node) = new_node("typemap");
		     Setattr((yyval.node),"method",(yyvsp[(3) - (6)].tmap).method);
		     if ((yyvsp[(3) - (6)].tmap).kwargs) {
		       ParmList *kw = (yyvsp[(3) - (6)].tmap).kwargs;
                       code = remove_block(kw, (yyvsp[(6) - (6)].str));
		       Setattr((yyval.node),"kwargs", (yyvsp[(3) - (6)].tmap).kwargs);
		     }
		     code = code ? code : NewString((yyvsp[(6) - (6)].str));
		     Setattr((yyval.node),"code", code);
		     Delete(code);
		     appendChild((yyval.node),(yyvsp[(5) - (6)].p));
		   }
	       }
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 2673 "parser.y"
    {
		 (yyval.node) = 0;
		 if ((yyvsp[(3) - (6)].tmap).method) {
		   (yyval.node) = new_node("typemap");
		   Setattr((yyval.node),"method",(yyvsp[(3) - (6)].tmap).method);
		   appendChild((yyval.node),(yyvsp[(5) - (6)].p));
		 }
	       }
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 2681 "parser.y"
    {
		   (yyval.node) = 0;
		   if ((yyvsp[(3) - (8)].tmap).method) {
		     (yyval.node) = new_node("typemapcopy");
		     Setattr((yyval.node),"method",(yyvsp[(3) - (8)].tmap).method);
		     Setattr((yyval.node),"pattern", Getattr((yyvsp[(7) - (8)].p),"pattern"));
		     appendChild((yyval.node),(yyvsp[(5) - (8)].p));
		   }
	       }
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 2694 "parser.y"
    {
		 Hash *p;
		 String *name;
		 p = nextSibling((yyvsp[(1) - (1)].node));
		 if (p && (!Getattr(p,"value"))) {
 		   /* this is the deprecated two argument typemap form */
 		   Swig_warning(WARN_DEPRECATED_TYPEMAP_LANG,cparse_file, cparse_line,
				"Specifying the language name in %%typemap is deprecated - use #ifdef SWIG<LANG> instead.\n");
		   /* two argument typemap form */
		   name = Getattr((yyvsp[(1) - (1)].node),"name");
		   if (!name || (Strcmp(name,typemap_lang))) {
		     (yyval.tmap).method = 0;
		     (yyval.tmap).kwargs = 0;
		   } else {
		     (yyval.tmap).method = Getattr(p,"name");
		     (yyval.tmap).kwargs = nextSibling(p);
		   }
		 } else {
		   /* one-argument typemap-form */
		   (yyval.tmap).method = Getattr((yyvsp[(1) - (1)].node),"name");
		   (yyval.tmap).kwargs = p;
		 }
                }
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 2719 "parser.y"
    {
                 (yyval.p) = (yyvsp[(1) - (2)].p);
		 set_nextSibling((yyval.p),(yyvsp[(2) - (2)].p));
		}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 2725 "parser.y"
    {
                 (yyval.p) = (yyvsp[(2) - (3)].p);
		 set_nextSibling((yyval.p),(yyvsp[(3) - (3)].p));
                }
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 2729 "parser.y"
    { (yyval.p) = 0;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 2732 "parser.y"
    {
                  Parm *parm;
		  SwigType_push((yyvsp[(1) - (2)].type),(yyvsp[(2) - (2)].decl).type);
		  (yyval.p) = new_node("typemapitem");
		  parm = NewParmWithoutFileLineInfo((yyvsp[(1) - (2)].type),(yyvsp[(2) - (2)].decl).id);
		  Setattr((yyval.p),"pattern",parm);
		  Setattr((yyval.p),"parms", (yyvsp[(2) - (2)].decl).parms);
		  Delete(parm);
		  /*		  $$ = NewParmWithoutFileLineInfo($1,$2.id);
				  Setattr($$,"parms",$2.parms); */
                }
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 2743 "parser.y"
    {
                  (yyval.p) = new_node("typemapitem");
		  Setattr((yyval.p),"pattern",(yyvsp[(2) - (3)].pl));
		  /*		  Setattr($$,"multitype",$2); */
               }
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 2748 "parser.y"
    {
		 (yyval.p) = new_node("typemapitem");
		 Setattr((yyval.p),"pattern", (yyvsp[(2) - (6)].pl));
		 /*                 Setattr($$,"multitype",$2); */
		 Setattr((yyval.p),"parms",(yyvsp[(5) - (6)].pl));
               }
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 2761 "parser.y"
    {
                   (yyval.node) = new_node("types");
		   Setattr((yyval.node),"parms",(yyvsp[(3) - (5)].pl));
                   if ((yyvsp[(5) - (5)].str))
		     Setattr((yyval.node),"convcode",NewString((yyvsp[(5) - (5)].str)));
               }
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 2773 "parser.y"
    {
                  Parm *p, *tp;
		  Node *n;
		  Symtab *tscope = 0;
		  int     specialized = 0;

		  (yyval.node) = 0;

		  tscope = Swig_symbol_current();          /* Get the current scope */

		  /* If the class name is qualified, we need to create or lookup namespace entries */
		  if (!inclass) {
		    (yyvsp[(5) - (9)].str) = resolve_node_scope((yyvsp[(5) - (9)].str));
		  }

		  /*
		    We use the new namespace entry 'nscope' only to
		    emit the template node. The template parameters are
		    resolved in the current 'tscope'.

		    This is closer to the C++ (typedef) behavior.
		  */
		  n = Swig_cparse_template_locate((yyvsp[(5) - (9)].str),(yyvsp[(7) - (9)].p),tscope);

		  /* Patch the argument types to respect namespaces */
		  p = (yyvsp[(7) - (9)].p);
		  while (p) {
		    SwigType *value = Getattr(p,"value");
		    if (!value) {
		      SwigType *ty = Getattr(p,"type");
		      if (ty) {
			SwigType *rty = 0;
			int reduce = template_reduce;
			if (reduce || !SwigType_ispointer(ty)) {
			  rty = Swig_symbol_typedef_reduce(ty,tscope);
			  if (!reduce) reduce = SwigType_ispointer(rty);
			}
			ty = reduce ? Swig_symbol_type_qualify(rty,tscope) : Swig_symbol_type_qualify(ty,tscope);
			Setattr(p,"type",ty);
			Delete(ty);
			Delete(rty);
		      }
		    } else {
		      value = Swig_symbol_type_qualify(value,tscope);
		      Setattr(p,"value",value);
		      Delete(value);
		    }

		    p = nextSibling(p);
		  }

		  /* Look for the template */
		  {
                    Node *nn = n;
                    Node *linklistend = 0;
                    while (nn) {
                      Node *templnode = 0;
                      if (Strcmp(nodeType(nn),"template") == 0) {
                        int nnisclass = (Strcmp(Getattr(nn,"templatetype"),"class") == 0); /* if not a templated class it is a templated function */
                        Parm *tparms = Getattr(nn,"templateparms");
                        if (!tparms) {
                          specialized = 1;
                        }
                        if (nnisclass && !specialized && ((ParmList_len((yyvsp[(7) - (9)].p)) > ParmList_len(tparms)))) {
                          Swig_error(cparse_file, cparse_line, "Too many template parameters. Maximum of %d.\n", ParmList_len(tparms));
                        } else if (nnisclass && !specialized && ((ParmList_len((yyvsp[(7) - (9)].p)) < ParmList_numrequired(tparms)))) {
                          Swig_error(cparse_file, cparse_line, "Not enough template parameters specified. %d required.\n", ParmList_numrequired(tparms));
                        } else if (!nnisclass && ((ParmList_len((yyvsp[(7) - (9)].p)) != ParmList_len(tparms)))) {
                          /* must be an overloaded templated method - ignore it as it is overloaded with a different number of template parameters */
                          nn = Getattr(nn,"sym:nextSibling"); /* repeat for overloaded templated functions */
                          continue;
                        } else {
			  String *tname = Copy((yyvsp[(5) - (9)].str));
                          int def_supplied = 0;
                          /* Expand the template */
			  Node *templ = Swig_symbol_clookup((yyvsp[(5) - (9)].str),0);
			  Parm *targs = templ ? Getattr(templ,"templateparms") : 0;

                          ParmList *temparms;
                          if (specialized) temparms = CopyParmList((yyvsp[(7) - (9)].p));
                          else temparms = CopyParmList(tparms);

                          /* Create typedef's and arguments */
                          p = (yyvsp[(7) - (9)].p);
                          tp = temparms;
                          if (!p && ParmList_len(p) != ParmList_len(temparms)) {
                            /* we have no template parameters supplied in %template for a template that has default args*/
                            p = tp;
                            def_supplied = 1;
                          }

                          while (p) {
                            String *value = Getattr(p,"value");
                            if (def_supplied) {
                              Setattr(p,"default","1");
                            }
                            if (value) {
                              Setattr(tp,"value",value);
                            } else {
                              SwigType *ty = Getattr(p,"type");
                              if (ty) {
                                Setattr(tp,"type",ty);
                              }
                              Delattr(tp,"value");
                            }
			    /* fix default arg values */
			    if (targs) {
			      Parm *pi = temparms;
			      Parm *ti = targs;
			      String *tv = Getattr(tp,"value");
			      if (!tv) tv = Getattr(tp,"type");
			      while(pi != tp && ti && pi) {
				String *name = Getattr(ti,"name");
				String *value = Getattr(pi,"value");
				if (!value) value = Getattr(pi,"type");
				Replaceid(tv, name, value);
				pi = nextSibling(pi);
				ti = nextSibling(ti);
			      }
			    }
                            p = nextSibling(p);
                            tp = nextSibling(tp);
                            if (!p && tp) {
                              p = tp;
                              def_supplied = 1;
                            }
                          }

                          templnode = copy_node(nn);
                          /* We need to set the node name based on name used to instantiate */
                          Setattr(templnode,"name",tname);
			  Delete(tname);
                          if (!specialized) {
                            Delattr(templnode,"sym:typename");
                          } else {
                            Setattr(templnode,"sym:typename","1");
                          }
                          if ((yyvsp[(3) - (9)].id) && !inclass) {
			    /*
			       Comment this out for 1.3.28. We need to
			       re-enable it later but first we need to
			       move %ignore from using %rename to use
			       %feature(ignore).

			       String *symname = Swig_name_make(templnode,0,$3,0,0);
			    */
			    String *symname = (yyvsp[(3) - (9)].id);
                            Swig_cparse_template_expand(templnode,symname,temparms,tscope);
                            Setattr(templnode,"sym:name",symname);
                          } else {
                            static int cnt = 0;
                            String *nname = NewStringf("__dummy_%d__", cnt++);
                            Swig_cparse_template_expand(templnode,nname,temparms,tscope);
                            Setattr(templnode,"sym:name",nname);
			    Delete(nname);
                            Setattr(templnode,"feature:onlychildren", "typemap,typemapitem,typemapcopy,typedef,types,fragment");

			    if ((yyvsp[(3) - (9)].id)) {
			      Swig_warning(WARN_PARSE_NESTED_TEMPLATE, cparse_file, cparse_line, "Named nested template instantiations not supported. Processing as if no name was given to %%template().\n");
			    }
                          }
                          Delattr(templnode,"templatetype");
                          Setattr(templnode,"template",nn);
                          Setfile(templnode,cparse_file);
                          Setline(templnode,cparse_line);
                          Delete(temparms);

                          add_symbols_copy(templnode);

                          if (Strcmp(nodeType(templnode),"class") == 0) {

                            /* Identify pure abstract methods */
                            Setattr(templnode,"abstract", pure_abstract(firstChild(templnode)));

                            /* Set up inheritance in symbol table */
                            {
                              Symtab  *csyms;
                              List *baselist = Getattr(templnode,"baselist");
                              csyms = Swig_symbol_current();
                              Swig_symbol_setscope(Getattr(templnode,"symtab"));
                              if (baselist) {
                                List *bases = make_inherit_list(Getattr(templnode,"name"),baselist);
                                if (bases) {
                                  Iterator s;
                                  for (s = First(bases); s.item; s = Next(s)) {
                                    Symtab *st = Getattr(s.item,"symtab");
                                    if (st) {
				      Setfile(st,Getfile(s.item));
				      Setline(st,Getline(s.item));
                                      Swig_symbol_inherit(st);
                                    }
                                  }
				  Delete(bases);
                                }
                              }
                              Swig_symbol_setscope(csyms);
                            }

                            /* Merge in %extend methods for this class */

			    /* !!! This may be broken.  We may have to add the
			       %extend methods at the beginning of the class */

                            if (extendhash) {
                              String *stmp = 0;
                              String *clsname;
                              Node *am;
                              if (Namespaceprefix) {
                                clsname = stmp = NewStringf("%s::%s", Namespaceprefix, Getattr(templnode,"name"));
                              } else {
                                clsname = Getattr(templnode,"name");
                              }
                              am = Getattr(extendhash,clsname);
                              if (am) {
                                Symtab *st = Swig_symbol_current();
                                Swig_symbol_setscope(Getattr(templnode,"symtab"));
                                /*			    Printf(stdout,"%s: %s %x %x\n", Getattr(templnode,"name"), clsname, Swig_symbol_current(), Getattr(templnode,"symtab")); */
                                merge_extensions(templnode,am);
                                Swig_symbol_setscope(st);
				append_previous_extension(templnode,am);
                                Delattr(extendhash,clsname);
                              }
			      if (stmp) Delete(stmp);
                            }
                            /* Add to classes hash */
                            if (!classes) classes = NewHash();

                            {
                              if (Namespaceprefix) {
                                String *temp = NewStringf("%s::%s", Namespaceprefix, Getattr(templnode,"name"));
                                Setattr(classes,temp,templnode);
				Delete(temp);
                              } else {
				String *qs = Swig_symbol_qualifiedscopename(templnode);
                                Setattr(classes, qs,templnode);
				Delete(qs);
                              }
                            }
                          }
                        }

                        /* all the overloaded templated functions are added into a linked list */
                        if (nscope_inner) {
                          /* non-global namespace */
                          if (templnode) {
                            appendChild(nscope_inner,templnode);
			    Delete(templnode);
                            if (nscope) (yyval.node) = nscope;
                          }
                        } else {
                          /* global namespace */
                          if (!linklistend) {
                            (yyval.node) = templnode;
                          } else {
                            set_nextSibling(linklistend,templnode);
			    Delete(templnode);
                          }
                          linklistend = templnode;
                        }
                      }
                      nn = Getattr(nn,"sym:nextSibling"); /* repeat for overloaded templated functions. If a templated class there will never be a sibling. */
                    }
		  }
	          Swig_symbol_setscope(tscope);
		  Delete(Namespaceprefix);
		  Namespaceprefix = Swig_symbol_qualifiedscopename(0);
                }
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 3047 "parser.y"
    {
		  Swig_warning(0,cparse_file, cparse_line,"%s\n", (yyvsp[(2) - (2)].id));
		  (yyval.node) = 0;
               }
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 3057 "parser.y"
    {
                    (yyval.node) = (yyvsp[(1) - (1)].node); 
                    if ((yyval.node)) {
   		      add_symbols((yyval.node));
                      default_arguments((yyval.node));
   	            }
                }
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 3064 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 3065 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 3069 "parser.y"
    {
		  if (Strcmp((yyvsp[(2) - (3)].id),"C") == 0) {
		    cparse_externc = 1;
		  }
		}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 3073 "parser.y"
    {
		  cparse_externc = 0;
		  if (Strcmp((yyvsp[(2) - (6)].id),"C") == 0) {
		    Node *n = firstChild((yyvsp[(5) - (6)].node));
		    (yyval.node) = new_node("extern");
		    Setattr((yyval.node),"name",(yyvsp[(2) - (6)].id));
		    appendChild((yyval.node),n);
		    while (n) {
		      SwigType *decl = Getattr(n,"decl");
		      if (SwigType_isfunction(decl) && Strcmp(Getattr(n, "storage"), "typedef") != 0) {
			Setattr(n,"storage","externc");
		      }
		      n = nextSibling(n);
		    }
		  } else {
		     Swig_warning(WARN_PARSE_UNDEFINED_EXTERN,cparse_file, cparse_line,"Unrecognized extern type \"%s\".\n", (yyvsp[(2) - (6)].id));
		    (yyval.node) = new_node("extern");
		    Setattr((yyval.node),"name",(yyvsp[(2) - (6)].id));
		    appendChild((yyval.node),firstChild((yyvsp[(5) - (6)].node)));
		  }
                }
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 3100 "parser.y"
    {
              (yyval.node) = new_node("cdecl");
	      if ((yyvsp[(4) - (5)].dtype).qualifier) SwigType_push((yyvsp[(3) - (5)].decl).type,(yyvsp[(4) - (5)].dtype).qualifier);
	      Setattr((yyval.node),"type",(yyvsp[(2) - (5)].type));
	      Setattr((yyval.node),"storage",(yyvsp[(1) - (5)].id));
	      Setattr((yyval.node),"name",(yyvsp[(3) - (5)].decl).id);
	      Setattr((yyval.node),"decl",(yyvsp[(3) - (5)].decl).type);
	      Setattr((yyval.node),"parms",(yyvsp[(3) - (5)].decl).parms);
	      Setattr((yyval.node),"value",(yyvsp[(4) - (5)].dtype).val);
	      Setattr((yyval.node),"throws",(yyvsp[(4) - (5)].dtype).throws);
	      Setattr((yyval.node),"throw",(yyvsp[(4) - (5)].dtype).throwf);
	      if (!(yyvsp[(5) - (5)].node)) {
		if (Len(scanner_ccode)) {
		  String *code = Copy(scanner_ccode);
		  Setattr((yyval.node),"code",code);
		  Delete(code);
		}
	      } else {
		Node *n = (yyvsp[(5) - (5)].node);
		/* Inherit attributes */
		while (n) {
		  String *type = Copy((yyvsp[(2) - (5)].type));
		  Setattr(n,"type",type);
		  Setattr(n,"storage",(yyvsp[(1) - (5)].id));
		  n = nextSibling(n);
		  Delete(type);
		}
	      }
	      if ((yyvsp[(4) - (5)].dtype).bitfield) {
		Setattr((yyval.node),"bitfield", (yyvsp[(4) - (5)].dtype).bitfield);
	      }

	      /* Look for "::" declarations (ignored) */
	      if (Strstr((yyvsp[(3) - (5)].decl).id,"::")) {
                /* This is a special case. If the scope name of the declaration exactly
                   matches that of the declaration, then we will allow it. Otherwise, delete. */
                String *p = Swig_scopename_prefix((yyvsp[(3) - (5)].decl).id);
		if (p) {
		  if ((Namespaceprefix && Strcmp(p,Namespaceprefix) == 0) ||
		      (inclass && Strcmp(p,Classprefix) == 0)) {
		    String *lstr = Swig_scopename_last((yyvsp[(3) - (5)].decl).id);
		    Setattr((yyval.node),"name",lstr);
		    Delete(lstr);
		    set_nextSibling((yyval.node),(yyvsp[(5) - (5)].node));
		  } else {
		    Delete((yyval.node));
		    (yyval.node) = (yyvsp[(5) - (5)].node);
		  }
		  Delete(p);
		} else {
		  Delete((yyval.node));
		  (yyval.node) = (yyvsp[(5) - (5)].node);
		}
	      } else {
		set_nextSibling((yyval.node),(yyvsp[(5) - (5)].node));
	      }
           }
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 3161 "parser.y"
    { 
                   (yyval.node) = 0;
                   Clear(scanner_ccode); 
               }
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 3165 "parser.y"
    {
		 (yyval.node) = new_node("cdecl");
		 if ((yyvsp[(3) - (4)].dtype).qualifier) SwigType_push((yyvsp[(2) - (4)].decl).type,(yyvsp[(3) - (4)].dtype).qualifier);
		 Setattr((yyval.node),"name",(yyvsp[(2) - (4)].decl).id);
		 Setattr((yyval.node),"decl",(yyvsp[(2) - (4)].decl).type);
		 Setattr((yyval.node),"parms",(yyvsp[(2) - (4)].decl).parms);
		 Setattr((yyval.node),"value",(yyvsp[(3) - (4)].dtype).val);
		 Setattr((yyval.node),"throws",(yyvsp[(3) - (4)].dtype).throws);
		 Setattr((yyval.node),"throw",(yyvsp[(3) - (4)].dtype).throwf);
		 if ((yyvsp[(3) - (4)].dtype).bitfield) {
		   Setattr((yyval.node),"bitfield", (yyvsp[(3) - (4)].dtype).bitfield);
		 }
		 if (!(yyvsp[(4) - (4)].node)) {
		   if (Len(scanner_ccode)) {
		     String *code = Copy(scanner_ccode);
		     Setattr((yyval.node),"code",code);
		     Delete(code);
		   }
		 } else {
		   set_nextSibling((yyval.node),(yyvsp[(4) - (4)].node));
		 }
	       }
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 3187 "parser.y"
    { 
                   skip_balanced('{','}');
                   (yyval.node) = 0;
               }
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 3193 "parser.y"
    { 
                   (yyval.dtype) = (yyvsp[(1) - (1)].dtype); 
                   (yyval.dtype).qualifier = 0;
		   (yyval.dtype).throws = 0;
		   (yyval.dtype).throwf = 0;
              }
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 3199 "parser.y"
    { 
                   (yyval.dtype) = (yyvsp[(2) - (2)].dtype); 
		   (yyval.dtype).qualifier = (yyvsp[(1) - (2)].str);
		   (yyval.dtype).throws = 0;
		   (yyval.dtype).throwf = 0;
	      }
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 3205 "parser.y"
    { 
		   (yyval.dtype) = (yyvsp[(5) - (5)].dtype); 
                   (yyval.dtype).qualifier = 0;
		   (yyval.dtype).throws = (yyvsp[(3) - (5)].pl);
		   (yyval.dtype).throwf = NewString("1");
              }
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 3211 "parser.y"
    { 
                   (yyval.dtype) = (yyvsp[(6) - (6)].dtype); 
                   (yyval.dtype).qualifier = (yyvsp[(1) - (6)].str);
		   (yyval.dtype).throws = (yyvsp[(4) - (6)].pl);
		   (yyval.dtype).throwf = NewString("1");
              }
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 3224 "parser.y"
    {
		   SwigType *ty = 0;
		   (yyval.node) = new_node("enumforward");
		   ty = NewStringf("enum %s", (yyvsp[(3) - (4)].id));
		   Setattr((yyval.node),"name",(yyvsp[(3) - (4)].id));
		   Setattr((yyval.node),"type",ty);
		   Setattr((yyval.node),"sym:weak", "1");
		   add_symbols((yyval.node));
	      }
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 3239 "parser.y"
    {
		  SwigType *ty = 0;
                  (yyval.node) = new_node("enum");
		  ty = NewStringf("enum %s", (yyvsp[(3) - (7)].id));
		  Setattr((yyval.node),"name",(yyvsp[(3) - (7)].id));
		  Setattr((yyval.node),"type",ty);
		  appendChild((yyval.node),(yyvsp[(5) - (7)].node));
		  add_symbols((yyval.node));       /* Add to tag space */
		  add_symbols((yyvsp[(5) - (7)].node));       /* Add enum values to id space */
               }
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 3249 "parser.y"
    {
		 Node *n;
		 SwigType *ty = 0;
		 String   *unnamed = 0;
		 int       unnamedinstance = 0;

		 (yyval.node) = new_node("enum");
		 if ((yyvsp[(3) - (9)].id)) {
		   Setattr((yyval.node),"name",(yyvsp[(3) - (9)].id));
		   ty = NewStringf("enum %s", (yyvsp[(3) - (9)].id));
		 } else if ((yyvsp[(7) - (9)].decl).id) {
		   unnamed = make_unnamed();
		   ty = NewStringf("enum %s", unnamed);
		   Setattr((yyval.node),"unnamed",unnamed);
                   /* name is not set for unnamed enum instances, e.g. enum { foo } Instance; */
		   if ((yyvsp[(1) - (9)].id) && Cmp((yyvsp[(1) - (9)].id),"typedef") == 0) {
		     Setattr((yyval.node),"name",(yyvsp[(7) - (9)].decl).id);
                   } else {
                     unnamedinstance = 1;
                   }
		   Setattr((yyval.node),"storage",(yyvsp[(1) - (9)].id));
		 }
		 if ((yyvsp[(7) - (9)].decl).id && Cmp((yyvsp[(1) - (9)].id),"typedef") == 0) {
		   Setattr((yyval.node),"tdname",(yyvsp[(7) - (9)].decl).id);
                   Setattr((yyval.node),"allows_typedef","1");
                 }
		 appendChild((yyval.node),(yyvsp[(5) - (9)].node));
		 n = new_node("cdecl");
		 Setattr(n,"type",ty);
		 Setattr(n,"name",(yyvsp[(7) - (9)].decl).id);
		 Setattr(n,"storage",(yyvsp[(1) - (9)].id));
		 Setattr(n,"decl",(yyvsp[(7) - (9)].decl).type);
		 Setattr(n,"parms",(yyvsp[(7) - (9)].decl).parms);
		 Setattr(n,"unnamed",unnamed);

                 if (unnamedinstance) {
		   SwigType *cty = NewString("enum ");
		   Setattr((yyval.node),"type",cty);
		   SetFlag((yyval.node),"unnamedinstance");
		   SetFlag(n,"unnamedinstance");
		   Delete(cty);
                 }
		 if ((yyvsp[(9) - (9)].node)) {
		   Node *p = (yyvsp[(9) - (9)].node);
		   set_nextSibling(n,p);
		   while (p) {
		     SwigType *cty = Copy(ty);
		     Setattr(p,"type",cty);
		     Setattr(p,"unnamed",unnamed);
		     Setattr(p,"storage",(yyvsp[(1) - (9)].id));
		     Delete(cty);
		     p = nextSibling(p);
		   }
		 } else {
		   if (Len(scanner_ccode)) {
		     String *code = Copy(scanner_ccode);
		     Setattr(n,"code",code);
		     Delete(code);
		   }
		 }

                 /* Ensure that typedef enum ABC {foo} XYZ; uses XYZ for sym:name, like structs.
                  * Note that class_rename/yyrename are bit of a mess so used this simple approach to change the name. */
                 if ((yyvsp[(7) - (9)].decl).id && (yyvsp[(3) - (9)].id) && Cmp((yyvsp[(1) - (9)].id),"typedef") == 0) {
		   String *name = NewString((yyvsp[(7) - (9)].decl).id);
                   Setattr((yyval.node), "parser:makename", name);
		   Delete(name);
                 }

		 add_symbols((yyval.node));       /* Add enum to tag space */
		 set_nextSibling((yyval.node),n);
		 Delete(n);
		 add_symbols((yyvsp[(5) - (9)].node));       /* Add enum values to id space */
	         add_symbols(n);
		 Delete(unnamed);
	       }
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 3327 "parser.y"
    {
                   /* This is a sick hack.  If the ctor_end has parameters,
                      and the parms parameter only has 1 parameter, this
                      could be a declaration of the form:

                         type (id)(parms)

			 Otherwise it's an error. */
                    int err = 0;
                    (yyval.node) = 0;

		    if ((ParmList_len((yyvsp[(4) - (6)].pl)) == 1) && (!Swig_scopename_check((yyvsp[(2) - (6)].type)))) {
		      SwigType *ty = Getattr((yyvsp[(4) - (6)].pl),"type");
		      String *name = Getattr((yyvsp[(4) - (6)].pl),"name");
		      err = 1;
		      if (!name) {
			(yyval.node) = new_node("cdecl");
			Setattr((yyval.node),"type",(yyvsp[(2) - (6)].type));
			Setattr((yyval.node),"storage",(yyvsp[(1) - (6)].id));
			Setattr((yyval.node),"name",ty);

			if ((yyvsp[(6) - (6)].decl).have_parms) {
			  SwigType *decl = NewStringEmpty();
			  SwigType_add_function(decl,(yyvsp[(6) - (6)].decl).parms);
			  Setattr((yyval.node),"decl",decl);
			  Setattr((yyval.node),"parms",(yyvsp[(6) - (6)].decl).parms);
			  if (Len(scanner_ccode)) {
			    String *code = Copy(scanner_ccode);
			    Setattr((yyval.node),"code",code);
			    Delete(code);
			  }
			}
			if ((yyvsp[(6) - (6)].decl).defarg) {
			  Setattr((yyval.node),"value",(yyvsp[(6) - (6)].decl).defarg);
			}
			Setattr((yyval.node),"throws",(yyvsp[(6) - (6)].decl).throws);
			Setattr((yyval.node),"throw",(yyvsp[(6) - (6)].decl).throwf);
			err = 0;
		      }
		    }
		    if (err) {
		      Swig_error(cparse_file,cparse_line,"Syntax error in input(2).\n");
		      exit(1);
		    }
                }
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 3378 "parser.y"
    {  (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 3379 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 3380 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 3381 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 3382 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 3383 "parser.y"
    { (yyval.node) = 0; }
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 3388 "parser.y"
    {
                 if (nested_template == 0) {
                   String *prefix;
                   List *bases = 0;
		   Node *scope = 0;
		   (yyval.node) = new_node("class");
		   Setline((yyval.node),cparse_start_line);
		   Setattr((yyval.node),"kind",(yyvsp[(2) - (5)].id));
		   if ((yyvsp[(4) - (5)].bases)) {
		     Setattr((yyval.node),"baselist", Getattr((yyvsp[(4) - (5)].bases),"public"));
		     Setattr((yyval.node),"protectedbaselist", Getattr((yyvsp[(4) - (5)].bases),"protected"));
		     Setattr((yyval.node),"privatebaselist", Getattr((yyvsp[(4) - (5)].bases),"private"));
		   }
		   Setattr((yyval.node),"allows_typedef","1");

		   /* preserve the current scope */
		   prev_symtab = Swig_symbol_current();
		  
		   /* If the class name is qualified.  We need to create or lookup namespace/scope entries */
		   scope = resolve_node_scope((yyvsp[(3) - (5)].str));
		   Setfile(scope,cparse_file);
		   Setline(scope,cparse_line);
		   (yyvsp[(3) - (5)].str) = scope;
		   
		   /* support for old nested classes "pseudo" support, such as:

		         %rename(Ala__Ola) Ala::Ola;
			class Ala::Ola {
			public:
			    Ola() {}
		         };

		      this should disappear when a proper implementation is added.
		   */
		   if (nscope_inner && Strcmp(nodeType(nscope_inner),"namespace") != 0) {
		     if (Namespaceprefix) {
		       String *name = NewStringf("%s::%s", Namespaceprefix, (yyvsp[(3) - (5)].str));		       
		       (yyvsp[(3) - (5)].str) = name;
		       Namespaceprefix = 0;
		       nscope_inner = 0;
		     }
		   }
		   Setattr((yyval.node),"name",(yyvsp[(3) - (5)].str));

		   Delete(class_rename);
                   class_rename = make_name((yyval.node),(yyvsp[(3) - (5)].str),0);
		   Classprefix = NewString((yyvsp[(3) - (5)].str));
		   /* Deal with inheritance  */
		   if ((yyvsp[(4) - (5)].bases)) {
		     bases = make_inherit_list((yyvsp[(3) - (5)].str),Getattr((yyvsp[(4) - (5)].bases),"public"));
		   }
		   prefix = SwigType_istemplate_templateprefix((yyvsp[(3) - (5)].str));
		   if (prefix) {
		     String *fbase, *tbase;
		     if (Namespaceprefix) {
		       fbase = NewStringf("%s::%s", Namespaceprefix,(yyvsp[(3) - (5)].str));
		       tbase = NewStringf("%s::%s", Namespaceprefix, prefix);
		     } else {
		       fbase = Copy((yyvsp[(3) - (5)].str));
		       tbase = Copy(prefix);
		     }
		     Swig_name_inherit(tbase,fbase);
		     Delete(fbase);
		     Delete(tbase);
		   }
                   if (strcmp((yyvsp[(2) - (5)].id),"class") == 0) {
		     cplus_mode = CPLUS_PRIVATE;
		   } else {
		     cplus_mode = CPLUS_PUBLIC;
		   }
		   Swig_symbol_newscope();
		   Swig_symbol_setscopename((yyvsp[(3) - (5)].str));
		   if (bases) {
		     Iterator s;
		     for (s = First(bases); s.item; s = Next(s)) {
		       Symtab *st = Getattr(s.item,"symtab");
		       if (st) {
			 Setfile(st,Getfile(s.item));
			 Setline(st,Getline(s.item));
			 Swig_symbol_inherit(st); 
		       }
		     }
		     Delete(bases);
		   }
		   Delete(Namespaceprefix);
		   Namespaceprefix = Swig_symbol_qualifiedscopename(0);
		   cparse_start_line = cparse_line;

		   /* If there are active template parameters, we need to make sure they are
                      placed in the class symbol table so we can catch shadows */

		   if (template_parameters) {
		     Parm *tp = template_parameters;
		     while(tp) {
		       String *tpname = Copy(Getattr(tp,"name"));
		       Node *tn = new_node("templateparm");
		       Setattr(tn,"name",tpname);
		       Swig_symbol_cadd(tpname,tn);
		       tp = nextSibling(tp);
		       Delete(tpname);
		     }
		   }
		   if (class_level >= max_class_levels) {
		       if (!max_class_levels) {
			   max_class_levels = 16;
		       } else {
			   max_class_levels *= 2;
		       }
		       class_decl = (Node**) realloc(class_decl, sizeof(Node*) * max_class_levels);
		       if (!class_decl) {
			   Swig_error(cparse_file, cparse_line, "realloc() failed\n");
		       }
		   }
		   class_decl[class_level++] = (yyval.node);
		   Delete(prefix);
		   inclass = 1;
		 }
               }
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 3505 "parser.y"
    {
	         (void) (yyvsp[(6) - (9)].node);
		 if (nested_template == 0) {
		   Node *p;
		   SwigType *ty;
		   Symtab *cscope = prev_symtab;
		   Node *am = 0;
		   String *scpname = 0;
		   (yyval.node) = class_decl[--class_level];
		   inclass = 0;
		   
		   /* Check for pure-abstract class */
		   Setattr((yyval.node),"abstract", pure_abstract((yyvsp[(7) - (9)].node)));
		   
		   /* This bit of code merges in a previously defined %extend directive (if any) */
		   
		   if (extendhash) {
		     String *clsname = Swig_symbol_qualifiedscopename(0);
		     am = Getattr(extendhash,clsname);
		     if (am) {
		       merge_extensions((yyval.node),am);
		       Delattr(extendhash,clsname);
		     }
		     Delete(clsname);
		   }
		   if (!classes) classes = NewHash();
		   scpname = Swig_symbol_qualifiedscopename(0);
		   Setattr(classes,scpname,(yyval.node));
		   Delete(scpname);

		   appendChild((yyval.node),(yyvsp[(7) - (9)].node));
		   
		   if (am) append_previous_extension((yyval.node),am);

		   p = (yyvsp[(9) - (9)].node);
		   if (p) {
		     set_nextSibling((yyval.node),p);
		   }
		   
		   if (cparse_cplusplus && !cparse_externc) {
		     ty = NewString((yyvsp[(3) - (9)].str));
		   } else {
		     ty = NewStringf("%s %s", (yyvsp[(2) - (9)].id),(yyvsp[(3) - (9)].str));
		   }
		   while (p) {
		     Setattr(p,"storage",(yyvsp[(1) - (9)].id));
		     Setattr(p,"type",ty);
		     p = nextSibling(p);
		   }
		   /* Dump nested classes */
		   {
		     String *name = (yyvsp[(3) - (9)].str);
		     if ((yyvsp[(9) - (9)].node)) {
		       SwigType *decltype = Getattr((yyvsp[(9) - (9)].node),"decl");
		       if (Cmp((yyvsp[(1) - (9)].id),"typedef") == 0) {
			 if (!decltype || !Len(decltype)) {
			   String *cname;
			   name = Getattr((yyvsp[(9) - (9)].node),"name");
			   cname = Copy(name);
			   Setattr((yyval.node),"tdname",cname);
			   Delete(cname);

			   /* Use typedef name as class name */
			   if (class_rename && (Strcmp(class_rename,(yyvsp[(3) - (9)].str)) == 0)) {
			     Delete(class_rename);
			     class_rename = NewString(name);
			   }
			   if (!Getattr(classes,name)) {
			     Setattr(classes,name,(yyval.node));
			   }
			   Setattr((yyval.node),"decl",decltype);
			 }
		       }
		     }
		     appendChild((yyval.node),dump_nested(Char(name)));
		   }

		   if (cplus_mode != CPLUS_PUBLIC) {
		   /* we 'open' the class at the end, to allow %template
		      to add new members */
		     Node *pa = new_node("access");
		     Setattr(pa,"kind","public");
		     cplus_mode = CPLUS_PUBLIC;
		     appendChild((yyval.node),pa);
		     Delete(pa);
		   }

		   Setattr((yyval.node),"symtab",Swig_symbol_popscope());

		   Classprefix = 0;
		   if (nscope_inner) {
		     /* this is tricky */
		     /* we add the declaration in the original namespace */
		     appendChild(nscope_inner,(yyval.node));
		     Swig_symbol_setscope(Getattr(nscope_inner,"symtab"));
		     Delete(Namespaceprefix);
		     Namespaceprefix = Swig_symbol_qualifiedscopename(0);
		     add_symbols((yyval.node));
		     if (nscope) (yyval.node) = nscope;
		     /* but the variable definition in the current scope */
		     Swig_symbol_setscope(cscope);
		     Delete(Namespaceprefix);
		     Namespaceprefix = Swig_symbol_qualifiedscopename(0);
		     add_symbols((yyvsp[(9) - (9)].node));
		   } else {
		     Delete(yyrename);
		     yyrename = Copy(class_rename);
		     Delete(Namespaceprefix);
		     Namespaceprefix = Swig_symbol_qualifiedscopename(0);

		     add_symbols((yyval.node));
		     add_symbols((yyvsp[(9) - (9)].node));
		   }
		   Swig_symbol_setscope(cscope);
		   Delete(Namespaceprefix);
		   Namespaceprefix = Swig_symbol_qualifiedscopename(0);
		 } else {
		    (yyval.node) = new_node("class");
		    Setattr((yyval.node),"kind",(yyvsp[(2) - (9)].id));
		    Setattr((yyval.node),"name",NewString((yyvsp[(3) - (9)].str)));
		    SetFlag((yyval.node),"nestedtemplateclass");
		 }
	       }
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 3631 "parser.y"
    {
	       String *unnamed;
	       unnamed = make_unnamed();
	       (yyval.node) = new_node("class");
	       Setline((yyval.node),cparse_start_line);
	       Setattr((yyval.node),"kind",(yyvsp[(2) - (3)].id));
	       Setattr((yyval.node),"storage",(yyvsp[(1) - (3)].id));
	       Setattr((yyval.node),"unnamed",unnamed);
	       Setattr((yyval.node),"allows_typedef","1");
	       Delete(class_rename);
	       class_rename = make_name((yyval.node),0,0);
	       if (strcmp((yyvsp[(2) - (3)].id),"class") == 0) {
		 cplus_mode = CPLUS_PRIVATE;
	       } else {
		 cplus_mode = CPLUS_PUBLIC;
	       }
	       Swig_symbol_newscope();
	       cparse_start_line = cparse_line;
	       if (class_level >= max_class_levels) {
		   if (!max_class_levels) {
		       max_class_levels = 16;
		   } else {
		       max_class_levels *= 2;
		   }
		   class_decl = (Node**) realloc(class_decl, sizeof(Node*) * max_class_levels);
		   if (!class_decl) {
		       Swig_error(cparse_file, cparse_line, "realloc() failed\n");
		   }
	       }
	       class_decl[class_level++] = (yyval.node);
	       inclass = 1;
	       Classprefix = NewStringEmpty();
	       Delete(Namespaceprefix);
	       Namespaceprefix = Swig_symbol_qualifiedscopename(0);
             }
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 3665 "parser.y"
    {
	       String *unnamed;
	       Node *n;
	       (void) (yyvsp[(4) - (9)].node);
	       Classprefix = 0;
	       (yyval.node) = class_decl[--class_level];
	       inclass = 0;
	       unnamed = Getattr((yyval.node),"unnamed");

	       /* Check for pure-abstract class */
	       Setattr((yyval.node),"abstract", pure_abstract((yyvsp[(5) - (9)].node)));

	       n = new_node("cdecl");
	       Setattr(n,"name",(yyvsp[(7) - (9)].decl).id);
	       Setattr(n,"unnamed",unnamed);
	       Setattr(n,"type",unnamed);
	       Setattr(n,"decl",(yyvsp[(7) - (9)].decl).type);
	       Setattr(n,"parms",(yyvsp[(7) - (9)].decl).parms);
	       Setattr(n,"storage",(yyvsp[(1) - (9)].id));
	       if ((yyvsp[(9) - (9)].node)) {
		 Node *p = (yyvsp[(9) - (9)].node);
		 set_nextSibling(n,p);
		 while (p) {
		   String *type = Copy(unnamed);
		   Setattr(p,"name",(yyvsp[(7) - (9)].decl).id);
		   Setattr(p,"unnamed",unnamed);
		   Setattr(p,"type",type);
		   Delete(type);
		   Setattr(p,"storage",(yyvsp[(1) - (9)].id));
		   p = nextSibling(p);
		 }
	       }
	       set_nextSibling((yyval.node),n);
	       Delete(n);
	       {
		 /* If a proper typedef name was given, we'll use it to set the scope name */
		 String *name = 0;
		 if ((yyvsp[(1) - (9)].id) && (strcmp((yyvsp[(1) - (9)].id),"typedef") == 0)) {
		   if (!Len((yyvsp[(7) - (9)].decl).type)) {	
		     String *scpname = 0;
		     name = (yyvsp[(7) - (9)].decl).id;
		     Setattr((yyval.node),"tdname",name);
		     Setattr((yyval.node),"name",name);
		     Swig_symbol_setscopename(name);

		     /* If a proper name was given, we use that as the typedef, not unnamed */
		     Clear(unnamed);
		     Append(unnamed, name);
		     
		     n = nextSibling(n);
		     set_nextSibling((yyval.node),n);

		     /* Check for previous extensions */
		     if (extendhash) {
		       String *clsname = Swig_symbol_qualifiedscopename(0);
		       Node *am = Getattr(extendhash,clsname);
		       if (am) {
			 /* Merge the extension into the symbol table */
			 merge_extensions((yyval.node),am);
			 append_previous_extension((yyval.node),am);
			 Delattr(extendhash,clsname);
		       }
		       Delete(clsname);
		     }
		     if (!classes) classes = NewHash();
		     scpname = Swig_symbol_qualifiedscopename(0);
		     Setattr(classes,scpname,(yyval.node));
		     Delete(scpname);
		   } else {
		     Swig_symbol_setscopename("<unnamed>");
		   }
		 }
		 appendChild((yyval.node),(yyvsp[(5) - (9)].node));
		 appendChild((yyval.node),dump_nested(Char(name)));
	       }
	       /* Pop the scope */
	       Setattr((yyval.node),"symtab",Swig_symbol_popscope());
	       if (class_rename) {
		 Delete(yyrename);
		 yyrename = NewString(class_rename);
	       }
	       Delete(Namespaceprefix);
	       Namespaceprefix = Swig_symbol_qualifiedscopename(0);
	       add_symbols((yyval.node));
	       add_symbols(n);
	       Delete(unnamed);
              }
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 3754 "parser.y"
    { (yyval.node) = 0; }
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 3755 "parser.y"
    {
                        (yyval.node) = new_node("cdecl");
                        Setattr((yyval.node),"name",(yyvsp[(1) - (3)].decl).id);
                        Setattr((yyval.node),"decl",(yyvsp[(1) - (3)].decl).type);
                        Setattr((yyval.node),"parms",(yyvsp[(1) - (3)].decl).parms);
			set_nextSibling((yyval.node),(yyvsp[(3) - (3)].node));
                    }
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 3767 "parser.y"
    {
              if ((yyvsp[(1) - (4)].id) && (Strcmp((yyvsp[(1) - (4)].id),"friend") == 0)) {
		/* Ignore */
                (yyval.node) = 0; 
	      } else {
		(yyval.node) = new_node("classforward");
		Setfile((yyval.node),cparse_file);
		Setline((yyval.node),cparse_line);
		Setattr((yyval.node),"kind",(yyvsp[(2) - (4)].id));
		Setattr((yyval.node),"name",(yyvsp[(3) - (4)].str));
		Setattr((yyval.node),"sym:weak", "1");
		add_symbols((yyval.node));
	      }
             }
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 3787 "parser.y"
    { 
		    template_parameters = (yyvsp[(3) - (4)].tparms); 
		    if (inclass)
		      nested_template++;

		  }
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 3792 "parser.y"
    {

		    /* Don't ignore templated functions declared within a class, unless the templated function is within a nested class */
		    if (nested_template <= 1) {
		      int is_nested_template_class = (yyvsp[(6) - (6)].node) && GetFlag((yyvsp[(6) - (6)].node), "nestedtemplateclass");
		      if (is_nested_template_class) {
			(yyval.node) = 0;
			/* Nested template classes would probably better be ignored like ordinary nested classes using cpp_nested, but that introduces shift/reduce conflicts */
			if (cplus_mode == CPLUS_PUBLIC) {
			  /* Treat the nested class/struct/union as a forward declaration until a proper nested class solution is implemented */
			  String *kind = Getattr((yyvsp[(6) - (6)].node), "kind");
			  String *name = Getattr((yyvsp[(6) - (6)].node), "name");
			  (yyval.node) = new_node("template");
			  Setattr((yyval.node),"kind",kind);
			  Setattr((yyval.node),"name",name);
			  Setattr((yyval.node),"sym:weak", "1");
			  Setattr((yyval.node),"templatetype","classforward");
			  Setattr((yyval.node),"templateparms", (yyvsp[(3) - (6)].tparms));
			  add_symbols((yyval.node));

			  if (GetFlag((yyval.node), "feature:nestedworkaround")) {
			    Swig_symbol_remove((yyval.node));
			    (yyval.node) = 0;
			  } else {
			    SWIG_WARN_NODE_BEGIN((yyval.node));
			    Swig_warning(WARN_PARSE_NAMED_NESTED_CLASS, cparse_file, cparse_line, "Nested template %s not currently supported (%s ignored).\n", kind, name);
			    SWIG_WARN_NODE_END((yyval.node));
			  }
			}
			Delete((yyvsp[(6) - (6)].node));
		      } else {
			String *tname = 0;
			int     error = 0;

			/* check if we get a namespace node with a class declaration, and retrieve the class */
			Symtab *cscope = Swig_symbol_current();
			Symtab *sti = 0;
			Node *ntop = (yyvsp[(6) - (6)].node);
			Node *ni = ntop;
			SwigType *ntype = ni ? nodeType(ni) : 0;
			while (ni && Strcmp(ntype,"namespace") == 0) {
			  sti = Getattr(ni,"symtab");
			  ni = firstChild(ni);
			  ntype = nodeType(ni);
			}
			if (sti) {
			  Swig_symbol_setscope(sti);
			  Delete(Namespaceprefix);
			  Namespaceprefix = Swig_symbol_qualifiedscopename(0);
			  (yyvsp[(6) - (6)].node) = ni;
			}

			(yyval.node) = (yyvsp[(6) - (6)].node);
			if ((yyval.node)) tname = Getattr((yyval.node),"name");
			
			/* Check if the class is a template specialization */
			if (((yyval.node)) && (Strchr(tname,'<')) && (!is_operator(tname))) {
			  /* If a specialization.  Check if defined. */
			  Node *tempn = 0;
			  {
			    String *tbase = SwigType_templateprefix(tname);
			    tempn = Swig_symbol_clookup_local(tbase,0);
			    if (!tempn || (Strcmp(nodeType(tempn),"template") != 0)) {
			      SWIG_WARN_NODE_BEGIN(tempn);
			      Swig_warning(WARN_PARSE_TEMPLATE_SP_UNDEF, Getfile((yyval.node)),Getline((yyval.node)),"Specialization of non-template '%s'.\n", tbase);
			      SWIG_WARN_NODE_END(tempn);
			      tempn = 0;
			      error = 1;
			    }
			    Delete(tbase);
			  }
			  Setattr((yyval.node),"specialization","1");
			  Setattr((yyval.node),"templatetype",nodeType((yyval.node)));
			  set_nodeType((yyval.node),"template");
			  /* Template partial specialization */
			  if (tempn && ((yyvsp[(3) - (6)].tparms)) && ((yyvsp[(6) - (6)].node))) {
			    List   *tlist;
			    String *targs = SwigType_templateargs(tname);
			    tlist = SwigType_parmlist(targs);
			    /*			  Printf(stdout,"targs = '%s' %s\n", targs, tlist); */
			    if (!Getattr((yyval.node),"sym:weak")) {
			      Setattr((yyval.node),"sym:typename","1");
			    }
			    
			    if (Len(tlist) != ParmList_len(Getattr(tempn,"templateparms"))) {
			      Swig_error(Getfile((yyval.node)),Getline((yyval.node)),"Inconsistent argument count in template partial specialization. %d %d\n", Len(tlist), ParmList_len(Getattr(tempn,"templateparms")));
			      
			    } else {

			    /* This code builds the argument list for the partial template
			       specialization.  This is a little hairy, but the idea is as
			       follows:

			       $3 contains a list of arguments supplied for the template.
			       For example template<class T>.

			       tlist is a list of the specialization arguments--which may be
			       different.  For example class<int,T>.

			       tp is a copy of the arguments in the original template definition.
       
			       The patching algorithm walks through the list of supplied
			       arguments ($3), finds the position in the specialization arguments
			       (tlist), and then patches the name in the argument list of the
			       original template.
			    */

			    {
			      String *pn;
			      Parm *p, *p1;
			      int i, nargs;
			      Parm *tp = CopyParmList(Getattr(tempn,"templateparms"));
			      nargs = Len(tlist);
			      p = (yyvsp[(3) - (6)].tparms);
			      while (p) {
				for (i = 0; i < nargs; i++){
				  pn = Getattr(p,"name");
				  if (Strcmp(pn,SwigType_base(Getitem(tlist,i))) == 0) {
				    int j;
				    Parm *p1 = tp;
				    for (j = 0; j < i; j++) {
				      p1 = nextSibling(p1);
				    }
				    Setattr(p1,"name",pn);
				    Setattr(p1,"partialarg","1");
				  }
				}
				p = nextSibling(p);
			      }
			      p1 = tp;
			      i = 0;
			      while (p1) {
				if (!Getattr(p1,"partialarg")) {
				  Delattr(p1,"name");
				  Setattr(p1,"type", Getitem(tlist,i));
				} 
				i++;
				p1 = nextSibling(p1);
			      }
			      Setattr((yyval.node),"templateparms",tp);
			      Delete(tp);
			    }
  #if 0
			    /* Patch the parameter list */
			    if (tempn) {
			      Parm *p,*p1;
			      ParmList *tp = CopyParmList(Getattr(tempn,"templateparms"));
			      p = (yyvsp[(3) - (6)].tparms);
			      p1 = tp;
			      while (p && p1) {
				String *pn = Getattr(p,"name");
				Printf(stdout,"pn = '%s'\n", pn);
				if (pn) Setattr(p1,"name",pn);
				else Delattr(p1,"name");
				pn = Getattr(p,"type");
				if (pn) Setattr(p1,"type",pn);
				p = nextSibling(p);
				p1 = nextSibling(p1);
			      }
			      Setattr((yyval.node),"templateparms",tp);
			      Delete(tp);
			    } else {
			      Setattr((yyval.node),"templateparms",(yyvsp[(3) - (6)].tparms));
			    }
  #endif
			    Delattr((yyval.node),"specialization");
			    Setattr((yyval.node),"partialspecialization","1");
			    /* Create a specialized name for matching */
			    {
			      Parm *p = (yyvsp[(3) - (6)].tparms);
			      String *fname = NewString(Getattr((yyval.node),"name"));
			      String *ffname = 0;
			      ParmList *partialparms = 0;

			      char   tmp[32];
			      int    i, ilen;
			      while (p) {
				String *n = Getattr(p,"name");
				if (!n) {
				  p = nextSibling(p);
				  continue;
				}
				ilen = Len(tlist);
				for (i = 0; i < ilen; i++) {
				  if (Strstr(Getitem(tlist,i),n)) {
				    sprintf(tmp,"$%d",i+1);
				    Replaceid(fname,n,tmp);
				  }
				}
				p = nextSibling(p);
			      }
			      /* Patch argument names with typedef */
			      {
				Iterator tt;
				Parm *parm_current = 0;
				List *tparms = SwigType_parmlist(fname);
				ffname = SwigType_templateprefix(fname);
				Append(ffname,"<(");
				for (tt = First(tparms); tt.item; ) {
				  SwigType *rtt = Swig_symbol_typedef_reduce(tt.item,0);
				  SwigType *ttr = Swig_symbol_type_qualify(rtt,0);

				  Parm *newp = NewParmWithoutFileLineInfo(ttr, 0);
				  if (partialparms)
				    set_nextSibling(parm_current, newp);
				  else
				    partialparms = newp;
				  parm_current = newp;

				  Append(ffname,ttr);
				  tt = Next(tt);
				  if (tt.item) Putc(',',ffname);
				  Delete(rtt);
				  Delete(ttr);
				}
				Delete(tparms);
				Append(ffname,")>");
			      }
			      {
				Node *new_partial = NewHash();
				String *partials = Getattr(tempn,"partials");
				if (!partials) {
				  partials = NewList();
				  Setattr(tempn,"partials",partials);
				  Delete(partials);
				}
				/*			      Printf(stdout,"partial: fname = '%s', '%s'\n", fname, Swig_symbol_typedef_reduce(fname,0)); */
				Setattr(new_partial, "partialparms", partialparms);
				Setattr(new_partial, "templcsymname", ffname);
				Append(partials, new_partial);
			      }
			      Setattr((yyval.node),"partialargs",ffname);
			      Swig_symbol_cadd(ffname,(yyval.node));
			    }
			    }
			    Delete(tlist);
			    Delete(targs);
			  } else {
			    /* An explicit template specialization */
			    /* add default args from primary (unspecialized) template */
			    String *ty = Swig_symbol_template_deftype(tname,0);
			    String *fname = Swig_symbol_type_qualify(ty,0);
			    Swig_symbol_cadd(fname,(yyval.node));
			    Delete(ty);
			    Delete(fname);
			  }
			}  else if ((yyval.node)) {
			  Setattr((yyval.node),"templatetype",nodeType((yyvsp[(6) - (6)].node)));
			  set_nodeType((yyval.node),"template");
			  Setattr((yyval.node),"templateparms", (yyvsp[(3) - (6)].tparms));
			  if (!Getattr((yyval.node),"sym:weak")) {
			    Setattr((yyval.node),"sym:typename","1");
			  }
			  add_symbols((yyval.node));
			  default_arguments((yyval.node));
			  /* We also place a fully parameterized version in the symbol table */
			  {
			    Parm *p;
			    String *fname = NewStringf("%s<(", Getattr((yyval.node),"name"));
			    p = (yyvsp[(3) - (6)].tparms);
			    while (p) {
			      String *n = Getattr(p,"name");
			      if (!n) n = Getattr(p,"type");
			      Append(fname,n);
			      p = nextSibling(p);
			      if (p) Putc(',',fname);
			    }
			    Append(fname,")>");
			    Swig_symbol_cadd(fname,(yyval.node));
			  }
			}
			(yyval.node) = ntop;
			Swig_symbol_setscope(cscope);
			Delete(Namespaceprefix);
			Namespaceprefix = Swig_symbol_qualifiedscopename(0);
			if (error) (yyval.node) = 0;
		      }
		    } else {
		      (yyval.node) = 0;
		    }
		    template_parameters = 0;
		    if (inclass)
		      nested_template--;
                  }
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 4076 "parser.y"
    {
		  Swig_warning(WARN_PARSE_EXPLICIT_TEMPLATE, cparse_file, cparse_line, "Explicit template instantiation ignored.\n");
                   (yyval.node) = 0; 
                }
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 4082 "parser.y"
    {
		  (yyval.node) = (yyvsp[(1) - (1)].node);
                }
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 4085 "parser.y"
    {
                   (yyval.node) = (yyvsp[(1) - (1)].node);
                }
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 4088 "parser.y"
    {
                   (yyval.node) = (yyvsp[(1) - (1)].node);
                }
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 4091 "parser.y"
    {
		  (yyval.node) = 0;
                }
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 4094 "parser.y"
    {
                  (yyval.node) = (yyvsp[(1) - (1)].node);
                }
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 4097 "parser.y"
    {
                  (yyval.node) = (yyvsp[(1) - (1)].node);
                }
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 4102 "parser.y"
    {
		   /* Rip out the parameter names */
		  Parm *p = (yyvsp[(1) - (1)].pl);
		  (yyval.tparms) = (yyvsp[(1) - (1)].pl);

		  while (p) {
		    String *name = Getattr(p,"name");
		    if (!name) {
		      /* Hmmm. Maybe it's a 'class T' parameter */
		      char *type = Char(Getattr(p,"type"));
		      /* Template template parameter */
		      if (strncmp(type,"template<class> ",16) == 0) {
			type += 16;
		      }
		      if ((strncmp(type,"class ",6) == 0) || (strncmp(type,"typename ", 9) == 0)) {
			char *t = strchr(type,' ');
			Setattr(p,"name", t+1);
		      } else {
			/*
			 Swig_error(cparse_file, cparse_line, "Missing template parameter name\n");
			 $$.rparms = 0;
			 $$.parms = 0;
			 break; */
		      }
		    }
		    p = nextSibling(p);
		  }
                 }
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 4132 "parser.y"
    {
                      set_nextSibling((yyvsp[(1) - (2)].p),(yyvsp[(2) - (2)].pl));
                      (yyval.pl) = (yyvsp[(1) - (2)].p);
                   }
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 4136 "parser.y"
    { (yyval.pl) = 0; }
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 4139 "parser.y"
    {
		    (yyval.p) = NewParmWithoutFileLineInfo(NewString((yyvsp[(1) - (1)].id)), 0);
                  }
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 4142 "parser.y"
    {
                    (yyval.p) = (yyvsp[(1) - (1)].p);
                  }
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 4147 "parser.y"
    {
                         set_nextSibling((yyvsp[(2) - (3)].p),(yyvsp[(3) - (3)].pl));
                         (yyval.pl) = (yyvsp[(2) - (3)].p);
                       }
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 4151 "parser.y"
    { (yyval.pl) = 0; }
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 4156 "parser.y"
    {
                  String *uname = Swig_symbol_type_qualify((yyvsp[(2) - (3)].str),0);
		  String *name = Swig_scopename_last((yyvsp[(2) - (3)].str));
                  (yyval.node) = new_node("using");
		  Setattr((yyval.node),"uname",uname);
		  Setattr((yyval.node),"name", name);
		  Delete(uname);
		  Delete(name);
		  add_symbols((yyval.node));
             }
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 4166 "parser.y"
    {
	       Node *n = Swig_symbol_clookup((yyvsp[(3) - (4)].str),0);
	       if (!n) {
		 Swig_error(cparse_file, cparse_line, "Nothing known about namespace '%s'\n", (yyvsp[(3) - (4)].str));
		 (yyval.node) = 0;
	       } else {

		 while (Strcmp(nodeType(n),"using") == 0) {
		   n = Getattr(n,"node");
		 }
		 if (n) {
		   if (Strcmp(nodeType(n),"namespace") == 0) {
		     Symtab *current = Swig_symbol_current();
		     Symtab *symtab = Getattr(n,"symtab");
		     (yyval.node) = new_node("using");
		     Setattr((yyval.node),"node",n);
		     Setattr((yyval.node),"namespace", (yyvsp[(3) - (4)].str));
		     if (current != symtab) {
		       Swig_symbol_inherit(symtab);
		     }
		   } else {
		     Swig_error(cparse_file, cparse_line, "'%s' is not a namespace.\n", (yyvsp[(3) - (4)].str));
		     (yyval.node) = 0;
		   }
		 } else {
		   (yyval.node) = 0;
		 }
	       }
             }
    break;

  case 161:

/* Line 1455 of yacc.c  */
#line 4197 "parser.y"
    { 
                Hash *h;
                (yyvsp[(1) - (3)].node) = Swig_symbol_current();
		h = Swig_symbol_clookup((yyvsp[(2) - (3)].str),0);
		if (h && ((yyvsp[(1) - (3)].node) == Getattr(h,"sym:symtab")) && (Strcmp(nodeType(h),"namespace") == 0)) {
		  if (Getattr(h,"alias")) {
		    h = Getattr(h,"namespace");
		    Swig_warning(WARN_PARSE_NAMESPACE_ALIAS, cparse_file, cparse_line, "Namespace alias '%s' not allowed here. Assuming '%s'\n",
				 (yyvsp[(2) - (3)].str), Getattr(h,"name"));
		    (yyvsp[(2) - (3)].str) = Getattr(h,"name");
		  }
		  Swig_symbol_setscope(Getattr(h,"symtab"));
		} else {
		  Swig_symbol_newscope();
		  Swig_symbol_setscopename((yyvsp[(2) - (3)].str));
		}
		Delete(Namespaceprefix);
		Namespaceprefix = Swig_symbol_qualifiedscopename(0);
             }
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 4215 "parser.y"
    {
                Node *n = (yyvsp[(5) - (6)].node);
		set_nodeType(n,"namespace");
		Setattr(n,"name",(yyvsp[(2) - (6)].str));
                Setattr(n,"symtab", Swig_symbol_popscope());
		Swig_symbol_setscope((yyvsp[(1) - (6)].node));
		(yyval.node) = n;
		Delete(Namespaceprefix);
		Namespaceprefix = Swig_symbol_qualifiedscopename(0);
		add_symbols((yyval.node));
             }
    break;

  case 163:

/* Line 1455 of yacc.c  */
#line 4226 "parser.y"
    {
	       Hash *h;
	       (yyvsp[(1) - (2)].node) = Swig_symbol_current();
	       h = Swig_symbol_clookup((char *)"    ",0);
	       if (h && (Strcmp(nodeType(h),"namespace") == 0)) {
		 Swig_symbol_setscope(Getattr(h,"symtab"));
	       } else {
		 Swig_symbol_newscope();
		 /* we don't use "__unnamed__", but a long 'empty' name */
		 Swig_symbol_setscopename("    ");
	       }
	       Namespaceprefix = 0;
             }
    break;

  case 164:

/* Line 1455 of yacc.c  */
#line 4238 "parser.y"
    {
	       (yyval.node) = (yyvsp[(4) - (5)].node);
	       set_nodeType((yyval.node),"namespace");
	       Setattr((yyval.node),"unnamed","1");
	       Setattr((yyval.node),"symtab", Swig_symbol_popscope());
	       Swig_symbol_setscope((yyvsp[(1) - (5)].node));
	       Delete(Namespaceprefix);
	       Namespaceprefix = Swig_symbol_qualifiedscopename(0);
	       add_symbols((yyval.node));
             }
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 4248 "parser.y"
    {
	       /* Namespace alias */
	       Node *n;
	       (yyval.node) = new_node("namespace");
	       Setattr((yyval.node),"name",(yyvsp[(2) - (5)].id));
	       Setattr((yyval.node),"alias",(yyvsp[(4) - (5)].str));
	       n = Swig_symbol_clookup((yyvsp[(4) - (5)].str),0);
	       if (!n) {
		 Swig_error(cparse_file, cparse_line, "Unknown namespace '%s'\n", (yyvsp[(4) - (5)].str));
		 (yyval.node) = 0;
	       } else {
		 if (Strcmp(nodeType(n),"namespace") != 0) {
		   Swig_error(cparse_file, cparse_line, "'%s' is not a namespace\n",(yyvsp[(4) - (5)].str));
		   (yyval.node) = 0;
		 } else {
		   while (Getattr(n,"alias")) {
		     n = Getattr(n,"namespace");
		   }
		   Setattr((yyval.node),"namespace",n);
		   add_symbols((yyval.node));
		   /* Set up a scope alias */
		   Swig_symbol_alias((yyvsp[(2) - (5)].id),Getattr(n,"symtab"));
		 }
	       }
             }
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 4275 "parser.y"
    {
                   (yyval.node) = (yyvsp[(1) - (2)].node);
                   /* Insert cpp_member (including any siblings) to the front of the cpp_members linked list */
		   if ((yyval.node)) {
		     Node *p = (yyval.node);
		     Node *pp =0;
		     while (p) {
		       pp = p;
		       p = nextSibling(p);
		     }
		     set_nextSibling(pp,(yyvsp[(2) - (2)].node));
		   } else {
		     (yyval.node) = (yyvsp[(2) - (2)].node);
		   }
             }
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 4290 "parser.y"
    { 
                  if (cplus_mode != CPLUS_PUBLIC) {
		     Swig_error(cparse_file,cparse_line,"%%extend can only be used in a public section\n");
		  }
             }
    break;

  case 168:

/* Line 1455 of yacc.c  */
#line 4294 "parser.y"
    {
	       (yyval.node) = new_node("extend");
	       tag_nodes((yyvsp[(4) - (6)].node),"feature:extend",(char*) "1");
	       appendChild((yyval.node),(yyvsp[(4) - (6)].node));
	       set_nextSibling((yyval.node),(yyvsp[(6) - (6)].node));
	     }
    break;

  case 169:

/* Line 1455 of yacc.c  */
#line 4300 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 170:

/* Line 1455 of yacc.c  */
#line 4301 "parser.y"
    { (yyval.node) = 0;}
    break;

  case 171:

/* Line 1455 of yacc.c  */
#line 4302 "parser.y"
    {
	       int start_line = cparse_line;
	       skip_decl();
	       Swig_error(cparse_file,start_line,"Syntax error in input(3).\n");
	       exit(1);
	       }
    break;

  case 172:

/* Line 1455 of yacc.c  */
#line 4307 "parser.y"
    { 
		 (yyval.node) = (yyvsp[(3) - (3)].node);
   	     }
    break;

  case 173:

/* Line 1455 of yacc.c  */
#line 4318 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 174:

/* Line 1455 of yacc.c  */
#line 4319 "parser.y"
    { 
                 (yyval.node) = (yyvsp[(1) - (1)].node); 
		 if (extendmode) {
		   String *symname;
		   symname= make_name((yyval.node),Getattr((yyval.node),"name"), Getattr((yyval.node),"decl"));
		   if (Strcmp(symname,Getattr((yyval.node),"name")) == 0) {
		     /* No renaming operation.  Set name to class name */
		     Delete(yyrename);
		     yyrename = NewString(Getattr(current_class,"sym:name"));
		   } else {
		     Delete(yyrename);
		     yyrename = symname;
		   }
		 }
		 add_symbols((yyval.node));
                 default_arguments((yyval.node));
             }
    break;

  case 175:

/* Line 1455 of yacc.c  */
#line 4336 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 176:

/* Line 1455 of yacc.c  */
#line 4337 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 177:

/* Line 1455 of yacc.c  */
#line 4338 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 178:

/* Line 1455 of yacc.c  */
#line 4339 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 179:

/* Line 1455 of yacc.c  */
#line 4340 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 180:

/* Line 1455 of yacc.c  */
#line 4341 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 181:

/* Line 1455 of yacc.c  */
#line 4342 "parser.y"
    { (yyval.node) = 0; }
    break;

  case 182:

/* Line 1455 of yacc.c  */
#line 4343 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 183:

/* Line 1455 of yacc.c  */
#line 4344 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 184:

/* Line 1455 of yacc.c  */
#line 4345 "parser.y"
    { (yyval.node) = 0; }
    break;

  case 185:

/* Line 1455 of yacc.c  */
#line 4346 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 186:

/* Line 1455 of yacc.c  */
#line 4347 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 187:

/* Line 1455 of yacc.c  */
#line 4348 "parser.y"
    { (yyval.node) = 0; }
    break;

  case 188:

/* Line 1455 of yacc.c  */
#line 4349 "parser.y"
    {(yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 189:

/* Line 1455 of yacc.c  */
#line 4350 "parser.y"
    {(yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 190:

/* Line 1455 of yacc.c  */
#line 4351 "parser.y"
    { (yyval.node) = 0; }
    break;

  case 191:

/* Line 1455 of yacc.c  */
#line 4360 "parser.y"
    {
              if (Classprefix) {
		 SwigType *decl = NewStringEmpty();
		 (yyval.node) = new_node("constructor");
		 Setattr((yyval.node),"storage",(yyvsp[(1) - (6)].id));
		 Setattr((yyval.node),"name",(yyvsp[(2) - (6)].type));
		 Setattr((yyval.node),"parms",(yyvsp[(4) - (6)].pl));
		 SwigType_add_function(decl,(yyvsp[(4) - (6)].pl));
		 Setattr((yyval.node),"decl",decl);
		 Setattr((yyval.node),"throws",(yyvsp[(6) - (6)].decl).throws);
		 Setattr((yyval.node),"throw",(yyvsp[(6) - (6)].decl).throwf);
		 if (Len(scanner_ccode)) {
		   String *code = Copy(scanner_ccode);
		   Setattr((yyval.node),"code",code);
		   Delete(code);
		 }
		 SetFlag((yyval.node),"feature:new");
	      } else {
		(yyval.node) = 0;
              }
              }
    break;

  case 192:

/* Line 1455 of yacc.c  */
#line 4385 "parser.y"
    {
               String *name = NewStringf("%s",(yyvsp[(2) - (6)].str));
	       if (*(Char(name)) != '~') Insert(name,0,"~");
               (yyval.node) = new_node("destructor");
	       Setattr((yyval.node),"name",name);
	       Delete(name);
	       if (Len(scanner_ccode)) {
		 String *code = Copy(scanner_ccode);
		 Setattr((yyval.node),"code",code);
		 Delete(code);
	       }
	       {
		 String *decl = NewStringEmpty();
		 SwigType_add_function(decl,(yyvsp[(4) - (6)].pl));
		 Setattr((yyval.node),"decl",decl);
		 Delete(decl);
	       }
	       Setattr((yyval.node),"throws",(yyvsp[(6) - (6)].dtype).throws);
	       Setattr((yyval.node),"throw",(yyvsp[(6) - (6)].dtype).throwf);
	       add_symbols((yyval.node));
	      }
    break;

  case 193:

/* Line 1455 of yacc.c  */
#line 4409 "parser.y"
    {
		String *name;
		char *c = 0;
		(yyval.node) = new_node("destructor");
	       /* Check for template names.  If the class is a template
		  and the constructor is missing the template part, we
		  add it */
	        if (Classprefix) {
                  c = strchr(Char(Classprefix),'<');
                  if (c && !Strchr((yyvsp[(3) - (7)].str),'<')) {
                    (yyvsp[(3) - (7)].str) = NewStringf("%s%s",(yyvsp[(3) - (7)].str),c);
                  }
		}
		Setattr((yyval.node),"storage","virtual");
	        name = NewStringf("%s",(yyvsp[(3) - (7)].str));
		if (*(Char(name)) != '~') Insert(name,0,"~");
		Setattr((yyval.node),"name",name);
		Delete(name);
		Setattr((yyval.node),"throws",(yyvsp[(7) - (7)].dtype).throws);
		Setattr((yyval.node),"throw",(yyvsp[(7) - (7)].dtype).throwf);
		if ((yyvsp[(7) - (7)].dtype).val) {
		  Setattr((yyval.node),"value","0");
		}
		if (Len(scanner_ccode)) {
		  String *code = Copy(scanner_ccode);
		  Setattr((yyval.node),"code",code);
		  Delete(code);
		}
		{
		  String *decl = NewStringEmpty();
		  SwigType_add_function(decl,(yyvsp[(5) - (7)].pl));
		  Setattr((yyval.node),"decl",decl);
		  Delete(decl);
		}

		add_symbols((yyval.node));
	      }
    break;

  case 194:

/* Line 1455 of yacc.c  */
#line 4450 "parser.y"
    {
                 (yyval.node) = new_node("cdecl");
                 Setattr((yyval.node),"type",(yyvsp[(3) - (8)].type));
		 Setattr((yyval.node),"name",(yyvsp[(2) - (8)].str));
		 Setattr((yyval.node),"storage",(yyvsp[(1) - (8)].id));

		 SwigType_add_function((yyvsp[(4) - (8)].type),(yyvsp[(6) - (8)].pl));
		 if ((yyvsp[(8) - (8)].dtype).qualifier) {
		   SwigType_push((yyvsp[(4) - (8)].type),(yyvsp[(8) - (8)].dtype).qualifier);
		 }
		 Setattr((yyval.node),"decl",(yyvsp[(4) - (8)].type));
		 Setattr((yyval.node),"parms",(yyvsp[(6) - (8)].pl));
		 Setattr((yyval.node),"conversion_operator","1");
		 add_symbols((yyval.node));
              }
    break;

  case 195:

/* Line 1455 of yacc.c  */
#line 4465 "parser.y"
    {
		 SwigType *decl;
                 (yyval.node) = new_node("cdecl");
                 Setattr((yyval.node),"type",(yyvsp[(3) - (8)].type));
		 Setattr((yyval.node),"name",(yyvsp[(2) - (8)].str));
		 Setattr((yyval.node),"storage",(yyvsp[(1) - (8)].id));
		 decl = NewStringEmpty();
		 SwigType_add_reference(decl);
		 SwigType_add_function(decl,(yyvsp[(6) - (8)].pl));
		 if ((yyvsp[(8) - (8)].dtype).qualifier) {
		   SwigType_push(decl,(yyvsp[(8) - (8)].dtype).qualifier);
		 }
		 Setattr((yyval.node),"decl",decl);
		 Setattr((yyval.node),"parms",(yyvsp[(6) - (8)].pl));
		 Setattr((yyval.node),"conversion_operator","1");
		 add_symbols((yyval.node));
	       }
    break;

  case 196:

/* Line 1455 of yacc.c  */
#line 4483 "parser.y"
    {
		 SwigType *decl;
                 (yyval.node) = new_node("cdecl");
                 Setattr((yyval.node),"type",(yyvsp[(3) - (9)].type));
		 Setattr((yyval.node),"name",(yyvsp[(2) - (9)].str));
		 Setattr((yyval.node),"storage",(yyvsp[(1) - (9)].id));
		 decl = NewStringEmpty();
		 SwigType_add_pointer(decl);
		 SwigType_add_reference(decl);
		 SwigType_add_function(decl,(yyvsp[(7) - (9)].pl));
		 if ((yyvsp[(9) - (9)].dtype).qualifier) {
		   SwigType_push(decl,(yyvsp[(9) - (9)].dtype).qualifier);
		 }
		 Setattr((yyval.node),"decl",decl);
		 Setattr((yyval.node),"parms",(yyvsp[(7) - (9)].pl));
		 Setattr((yyval.node),"conversion_operator","1");
		 add_symbols((yyval.node));
	       }
    break;

  case 197:

/* Line 1455 of yacc.c  */
#line 4502 "parser.y"
    {
		String *t = NewStringEmpty();
		(yyval.node) = new_node("cdecl");
		Setattr((yyval.node),"type",(yyvsp[(3) - (7)].type));
		Setattr((yyval.node),"name",(yyvsp[(2) - (7)].str));
		 Setattr((yyval.node),"storage",(yyvsp[(1) - (7)].id));
		SwigType_add_function(t,(yyvsp[(5) - (7)].pl));
		if ((yyvsp[(7) - (7)].dtype).qualifier) {
		  SwigType_push(t,(yyvsp[(7) - (7)].dtype).qualifier);
		}
		Setattr((yyval.node),"decl",t);
		Setattr((yyval.node),"parms",(yyvsp[(5) - (7)].pl));
		Setattr((yyval.node),"conversion_operator","1");
		add_symbols((yyval.node));
              }
    break;

  case 198:

/* Line 1455 of yacc.c  */
#line 4521 "parser.y"
    {
                 skip_balanced('{','}');
                 (yyval.node) = 0;
               }
    break;

  case 199:

/* Line 1455 of yacc.c  */
#line 4528 "parser.y"
    { 
                (yyval.node) = new_node("access");
		Setattr((yyval.node),"kind","public");
                cplus_mode = CPLUS_PUBLIC;
              }
    break;

  case 200:

/* Line 1455 of yacc.c  */
#line 4535 "parser.y"
    { 
                (yyval.node) = new_node("access");
                Setattr((yyval.node),"kind","private");
		cplus_mode = CPLUS_PRIVATE;
	      }
    break;

  case 201:

/* Line 1455 of yacc.c  */
#line 4543 "parser.y"
    { 
		(yyval.node) = new_node("access");
		Setattr((yyval.node),"kind","protected");
		cplus_mode = CPLUS_PROTECTED;
	      }
    break;

  case 202:

/* Line 1455 of yacc.c  */
#line 4564 "parser.y"
    {
		cparse_start_line = cparse_line;
		skip_balanced('{','}');
		(yyval.str) = NewString(scanner_ccode); /* copied as initializers overwrite scanner_ccode */
	      }
    break;

  case 203:

/* Line 1455 of yacc.c  */
#line 4568 "parser.y"
    {
	        (yyval.node) = 0;
		if (cplus_mode == CPLUS_PUBLIC) {
		  if (cparse_cplusplus) {
		    (yyval.node) = nested_forward_declaration((yyvsp[(1) - (7)].id), (yyvsp[(2) - (7)].id), (yyvsp[(3) - (7)].str), (yyvsp[(3) - (7)].str), (yyvsp[(7) - (7)].node));
		  } else if ((yyvsp[(7) - (7)].node)) {
		    nested_new_struct((yyvsp[(2) - (7)].id), (yyvsp[(6) - (7)].str), (yyvsp[(7) - (7)].node));
		  }
		}
		Delete((yyvsp[(6) - (7)].str));
	      }
    break;

  case 204:

/* Line 1455 of yacc.c  */
#line 4590 "parser.y"
    {
		cparse_start_line = cparse_line;
		skip_balanced('{','}');
		(yyval.str) = NewString(scanner_ccode); /* copied as initializers overwrite scanner_ccode */
	      }
    break;

  case 205:

/* Line 1455 of yacc.c  */
#line 4594 "parser.y"
    {
	        (yyval.node) = 0;
		if (cplus_mode == CPLUS_PUBLIC) {
		  if (cparse_cplusplus) {
		    const char *name = (yyvsp[(6) - (6)].node) ? Getattr((yyvsp[(6) - (6)].node), "name") : 0;
		    (yyval.node) = nested_forward_declaration((yyvsp[(1) - (6)].id), (yyvsp[(2) - (6)].id), 0, name, (yyvsp[(6) - (6)].node));
		  } else {
		    if ((yyvsp[(6) - (6)].node)) {
		      nested_new_struct((yyvsp[(2) - (6)].id), (yyvsp[(5) - (6)].str), (yyvsp[(6) - (6)].node));
		    } else {
		      Swig_warning(WARN_PARSE_UNNAMED_NESTED_CLASS, cparse_file, cparse_line, "Nested %s not currently supported (ignored).\n", (yyvsp[(2) - (6)].id));
		    }
		  }
		}
		Delete((yyvsp[(5) - (6)].str));
	      }
    break;

  case 206:

/* Line 1455 of yacc.c  */
#line 4626 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 207:

/* Line 1455 of yacc.c  */
#line 4629 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 208:

/* Line 1455 of yacc.c  */
#line 4633 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 209:

/* Line 1455 of yacc.c  */
#line 4636 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 210:

/* Line 1455 of yacc.c  */
#line 4637 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 211:

/* Line 1455 of yacc.c  */
#line 4638 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 212:

/* Line 1455 of yacc.c  */
#line 4639 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 213:

/* Line 1455 of yacc.c  */
#line 4640 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 214:

/* Line 1455 of yacc.c  */
#line 4641 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 215:

/* Line 1455 of yacc.c  */
#line 4642 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 216:

/* Line 1455 of yacc.c  */
#line 4643 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 217:

/* Line 1455 of yacc.c  */
#line 4646 "parser.y"
    {
	            Clear(scanner_ccode);
		    (yyval.dtype).throws = (yyvsp[(1) - (2)].dtype).throws;
		    (yyval.dtype).throwf = (yyvsp[(1) - (2)].dtype).throwf;
               }
    break;

  case 218:

/* Line 1455 of yacc.c  */
#line 4651 "parser.y"
    { 
		    skip_balanced('{','}'); 
		    (yyval.dtype).throws = (yyvsp[(1) - (2)].dtype).throws;
		    (yyval.dtype).throwf = (yyvsp[(1) - (2)].dtype).throwf;
	       }
    break;

  case 219:

/* Line 1455 of yacc.c  */
#line 4658 "parser.y"
    { 
                     Clear(scanner_ccode);
                     (yyval.dtype).val = 0;
                     (yyval.dtype).qualifier = (yyvsp[(1) - (2)].dtype).qualifier;
                     (yyval.dtype).bitfield = 0;
                     (yyval.dtype).throws = (yyvsp[(1) - (2)].dtype).throws;
                     (yyval.dtype).throwf = (yyvsp[(1) - (2)].dtype).throwf;
                }
    break;

  case 220:

/* Line 1455 of yacc.c  */
#line 4666 "parser.y"
    { 
                     Clear(scanner_ccode);
                     (yyval.dtype).val = (yyvsp[(3) - (4)].dtype).val;
                     (yyval.dtype).qualifier = (yyvsp[(1) - (4)].dtype).qualifier;
                     (yyval.dtype).bitfield = 0;
                     (yyval.dtype).throws = (yyvsp[(1) - (4)].dtype).throws; 
                     (yyval.dtype).throwf = (yyvsp[(1) - (4)].dtype).throwf; 
               }
    break;

  case 221:

/* Line 1455 of yacc.c  */
#line 4674 "parser.y"
    { 
                     skip_balanced('{','}');
                     (yyval.dtype).val = 0;
                     (yyval.dtype).qualifier = (yyvsp[(1) - (2)].dtype).qualifier;
                     (yyval.dtype).bitfield = 0;
                     (yyval.dtype).throws = (yyvsp[(1) - (2)].dtype).throws; 
                     (yyval.dtype).throwf = (yyvsp[(1) - (2)].dtype).throwf; 
               }
    break;

  case 222:

/* Line 1455 of yacc.c  */
#line 4685 "parser.y"
    { }
    break;

  case 223:

/* Line 1455 of yacc.c  */
#line 4691 "parser.y"
    { (yyval.id) = "extern"; }
    break;

  case 224:

/* Line 1455 of yacc.c  */
#line 4692 "parser.y"
    { 
                   if (strcmp((yyvsp[(2) - (2)].id),"C") == 0) {
		     (yyval.id) = "externc";
		   } else {
		     Swig_warning(WARN_PARSE_UNDEFINED_EXTERN,cparse_file, cparse_line,"Unrecognized extern type \"%s\".\n", (yyvsp[(2) - (2)].id));
		     (yyval.id) = 0;
		   }
               }
    break;

  case 225:

/* Line 1455 of yacc.c  */
#line 4700 "parser.y"
    { (yyval.id) = "static"; }
    break;

  case 226:

/* Line 1455 of yacc.c  */
#line 4701 "parser.y"
    { (yyval.id) = "typedef"; }
    break;

  case 227:

/* Line 1455 of yacc.c  */
#line 4702 "parser.y"
    { (yyval.id) = "virtual"; }
    break;

  case 228:

/* Line 1455 of yacc.c  */
#line 4703 "parser.y"
    { (yyval.id) = "friend"; }
    break;

  case 229:

/* Line 1455 of yacc.c  */
#line 4704 "parser.y"
    { (yyval.id) = "explicit"; }
    break;

  case 230:

/* Line 1455 of yacc.c  */
#line 4705 "parser.y"
    { (yyval.id) = 0; }
    break;

  case 231:

/* Line 1455 of yacc.c  */
#line 4712 "parser.y"
    {
                 Parm *p;
		 (yyval.pl) = (yyvsp[(1) - (1)].pl);
		 p = (yyvsp[(1) - (1)].pl);
                 while (p) {
		   Replace(Getattr(p,"type"),"typename ", "", DOH_REPLACE_ANY);
		   p = nextSibling(p);
                 }
               }
    break;

  case 232:

/* Line 1455 of yacc.c  */
#line 4723 "parser.y"
    {
                  set_nextSibling((yyvsp[(1) - (2)].p),(yyvsp[(2) - (2)].pl));
                  (yyval.pl) = (yyvsp[(1) - (2)].p);
		}
    break;

  case 233:

/* Line 1455 of yacc.c  */
#line 4727 "parser.y"
    { (yyval.pl) = 0; }
    break;

  case 234:

/* Line 1455 of yacc.c  */
#line 4730 "parser.y"
    {
                 set_nextSibling((yyvsp[(2) - (3)].p),(yyvsp[(3) - (3)].pl));
		 (yyval.pl) = (yyvsp[(2) - (3)].p);
                }
    break;

  case 235:

/* Line 1455 of yacc.c  */
#line 4734 "parser.y"
    { (yyval.pl) = 0; }
    break;

  case 236:

/* Line 1455 of yacc.c  */
#line 4738 "parser.y"
    {
                   SwigType_push((yyvsp[(1) - (2)].type),(yyvsp[(2) - (2)].decl).type);
		   (yyval.p) = NewParmWithoutFileLineInfo((yyvsp[(1) - (2)].type),(yyvsp[(2) - (2)].decl).id);
		   Setfile((yyval.p),cparse_file);
		   Setline((yyval.p),cparse_line);
		   if ((yyvsp[(2) - (2)].decl).defarg) {
		     Setattr((yyval.p),"value",(yyvsp[(2) - (2)].decl).defarg);
		   }
		}
    break;

  case 237:

/* Line 1455 of yacc.c  */
#line 4748 "parser.y"
    {
                  (yyval.p) = NewParmWithoutFileLineInfo(NewStringf("template<class> %s %s", (yyvsp[(5) - (7)].id),(yyvsp[(6) - (7)].str)), 0);
		  Setfile((yyval.p),cparse_file);
		  Setline((yyval.p),cparse_line);
                  if ((yyvsp[(7) - (7)].dtype).val) {
                    Setattr((yyval.p),"value",(yyvsp[(7) - (7)].dtype).val);
                  }
                }
    break;

  case 238:

/* Line 1455 of yacc.c  */
#line 4756 "parser.y"
    {
		  SwigType *t = NewString("v(...)");
		  (yyval.p) = NewParmWithoutFileLineInfo(t, 0);
		  Setfile((yyval.p),cparse_file);
		  Setline((yyval.p),cparse_line);
		}
    break;

  case 239:

/* Line 1455 of yacc.c  */
#line 4764 "parser.y"
    {
                 Parm *p;
		 (yyval.p) = (yyvsp[(1) - (1)].p);
		 p = (yyvsp[(1) - (1)].p);
                 while (p) {
		   if (Getattr(p,"type")) {
		     Replace(Getattr(p,"type"),"typename ", "", DOH_REPLACE_ANY);
		   }
		   p = nextSibling(p);
                 }
               }
    break;

  case 240:

/* Line 1455 of yacc.c  */
#line 4777 "parser.y"
    {
                  set_nextSibling((yyvsp[(1) - (2)].p),(yyvsp[(2) - (2)].p));
                  (yyval.p) = (yyvsp[(1) - (2)].p);
		}
    break;

  case 241:

/* Line 1455 of yacc.c  */
#line 4781 "parser.y"
    { (yyval.p) = 0; }
    break;

  case 242:

/* Line 1455 of yacc.c  */
#line 4784 "parser.y"
    {
                 set_nextSibling((yyvsp[(2) - (3)].p),(yyvsp[(3) - (3)].p));
		 (yyval.p) = (yyvsp[(2) - (3)].p);
                }
    break;

  case 243:

/* Line 1455 of yacc.c  */
#line 4788 "parser.y"
    { (yyval.p) = 0; }
    break;

  case 244:

/* Line 1455 of yacc.c  */
#line 4792 "parser.y"
    {
		  (yyval.p) = (yyvsp[(1) - (1)].p);
		  {
		    /* We need to make a possible adjustment for integer parameters. */
		    SwigType *type;
		    Node     *n = 0;

		    while (!n) {
		      type = Getattr((yyvsp[(1) - (1)].p),"type");
		      n = Swig_symbol_clookup(type,0);     /* See if we can find a node that matches the typename */
		      if ((n) && (Strcmp(nodeType(n),"cdecl") == 0)) {
			SwigType *decl = Getattr(n,"decl");
			if (!SwigType_isfunction(decl)) {
			  String *value = Getattr(n,"value");
			  if (value) {
			    String *v = Copy(value);
			    Setattr((yyvsp[(1) - (1)].p),"type",v);
			    Delete(v);
			    n = 0;
			  }
			}
		      } else {
			break;
		      }
		    }
		  }

               }
    break;

  case 245:

/* Line 1455 of yacc.c  */
#line 4820 "parser.y"
    {
                  (yyval.p) = NewParmWithoutFileLineInfo(0,0);
                  Setfile((yyval.p),cparse_file);
		  Setline((yyval.p),cparse_line);
		  Setattr((yyval.p),"value",(yyvsp[(1) - (1)].dtype).val);
               }
    break;

  case 246:

/* Line 1455 of yacc.c  */
#line 4828 "parser.y"
    { 
                  (yyval.dtype) = (yyvsp[(2) - (2)].dtype); 
		  if ((yyvsp[(2) - (2)].dtype).type == T_ERROR) {
		    Swig_warning(WARN_PARSE_BAD_DEFAULT,cparse_file, cparse_line, "Can't set default argument (ignored)\n");
		    (yyval.dtype).val = 0;
		    (yyval.dtype).rawval = 0;
		    (yyval.dtype).bitfield = 0;
		    (yyval.dtype).throws = 0;
		    (yyval.dtype).throwf = 0;
		  }
               }
    break;

  case 247:

/* Line 1455 of yacc.c  */
#line 4839 "parser.y"
    { 
		  (yyval.dtype) = (yyvsp[(2) - (5)].dtype);
		  if ((yyvsp[(2) - (5)].dtype).type == T_ERROR) {
		    Swig_warning(WARN_PARSE_BAD_DEFAULT,cparse_file, cparse_line, "Can't set default argument (ignored)\n");
		    (yyval.dtype) = (yyvsp[(2) - (5)].dtype);
		    (yyval.dtype).val = 0;
		    (yyval.dtype).rawval = 0;
		    (yyval.dtype).bitfield = 0;
		    (yyval.dtype).throws = 0;
		    (yyval.dtype).throwf = 0;
		  } else {
		    (yyval.dtype).val = NewStringf("%s[%s]",(yyvsp[(2) - (5)].dtype).val,(yyvsp[(4) - (5)].dtype).val); 
		  }		  
               }
    break;

  case 248:

/* Line 1455 of yacc.c  */
#line 4853 "parser.y"
    {
		 skip_balanced('{','}');
		 (yyval.dtype).val = 0;
		 (yyval.dtype).rawval = 0;
                 (yyval.dtype).type = T_INT;
		 (yyval.dtype).bitfield = 0;
		 (yyval.dtype).throws = 0;
		 (yyval.dtype).throwf = 0;
	       }
    break;

  case 249:

/* Line 1455 of yacc.c  */
#line 4862 "parser.y"
    { 
		 (yyval.dtype).val = 0;
		 (yyval.dtype).rawval = 0;
		 (yyval.dtype).type = 0;
		 (yyval.dtype).bitfield = (yyvsp[(2) - (2)].dtype).val;
		 (yyval.dtype).throws = 0;
		 (yyval.dtype).throwf = 0;
	       }
    break;

  case 250:

/* Line 1455 of yacc.c  */
#line 4870 "parser.y"
    {
                 (yyval.dtype).val = 0;
                 (yyval.dtype).rawval = 0;
                 (yyval.dtype).type = T_INT;
		 (yyval.dtype).bitfield = 0;
		 (yyval.dtype).throws = 0;
		 (yyval.dtype).throwf = 0;
               }
    break;

  case 251:

/* Line 1455 of yacc.c  */
#line 4880 "parser.y"
    {
                 (yyval.decl) = (yyvsp[(1) - (2)].decl);
		 (yyval.decl).defarg = (yyvsp[(2) - (2)].dtype).rawval ? (yyvsp[(2) - (2)].dtype).rawval : (yyvsp[(2) - (2)].dtype).val;
            }
    break;

  case 252:

/* Line 1455 of yacc.c  */
#line 4884 "parser.y"
    {
              (yyval.decl) = (yyvsp[(1) - (2)].decl);
	      (yyval.decl).defarg = (yyvsp[(2) - (2)].dtype).rawval ? (yyvsp[(2) - (2)].dtype).rawval : (yyvsp[(2) - (2)].dtype).val;
            }
    break;

  case 253:

/* Line 1455 of yacc.c  */
#line 4888 "parser.y"
    {
   	      (yyval.decl).type = 0;
              (yyval.decl).id = 0;
	      (yyval.decl).defarg = (yyvsp[(1) - (1)].dtype).rawval ? (yyvsp[(1) - (1)].dtype).rawval : (yyvsp[(1) - (1)].dtype).val;
            }
    break;

  case 254:

/* Line 1455 of yacc.c  */
#line 4895 "parser.y"
    {
                 (yyval.decl) = (yyvsp[(1) - (1)].decl);
		 if (SwigType_isfunction((yyvsp[(1) - (1)].decl).type)) {
		   Delete(SwigType_pop_function((yyvsp[(1) - (1)].decl).type));
		 } else if (SwigType_isarray((yyvsp[(1) - (1)].decl).type)) {
		   SwigType *ta = SwigType_pop_arrays((yyvsp[(1) - (1)].decl).type);
		   if (SwigType_isfunction((yyvsp[(1) - (1)].decl).type)) {
		     Delete(SwigType_pop_function((yyvsp[(1) - (1)].decl).type));
		   } else {
		     (yyval.decl).parms = 0;
		   }
		   SwigType_push((yyvsp[(1) - (1)].decl).type,ta);
		   Delete(ta);
		 } else {
		   (yyval.decl).parms = 0;
		 }
            }
    break;

  case 255:

/* Line 1455 of yacc.c  */
#line 4912 "parser.y"
    {
              (yyval.decl) = (yyvsp[(1) - (1)].decl);
	      if (SwigType_isfunction((yyvsp[(1) - (1)].decl).type)) {
		Delete(SwigType_pop_function((yyvsp[(1) - (1)].decl).type));
	      } else if (SwigType_isarray((yyvsp[(1) - (1)].decl).type)) {
		SwigType *ta = SwigType_pop_arrays((yyvsp[(1) - (1)].decl).type);
		if (SwigType_isfunction((yyvsp[(1) - (1)].decl).type)) {
		  Delete(SwigType_pop_function((yyvsp[(1) - (1)].decl).type));
		} else {
		  (yyval.decl).parms = 0;
		}
		SwigType_push((yyvsp[(1) - (1)].decl).type,ta);
		Delete(ta);
	      } else {
		(yyval.decl).parms = 0;
	      }
            }
    break;

  case 256:

/* Line 1455 of yacc.c  */
#line 4929 "parser.y"
    {
   	      (yyval.decl).type = 0;
              (yyval.decl).id = 0;
	      (yyval.decl).parms = 0;
	      }
    break;

  case 257:

/* Line 1455 of yacc.c  */
#line 4937 "parser.y"
    {
              (yyval.decl) = (yyvsp[(2) - (2)].decl);
	      if ((yyval.decl).type) {
		SwigType_push((yyvsp[(1) - (2)].type),(yyval.decl).type);
		Delete((yyval.decl).type);
	      }
	      (yyval.decl).type = (yyvsp[(1) - (2)].type);
           }
    break;

  case 258:

/* Line 1455 of yacc.c  */
#line 4945 "parser.y"
    {
              (yyval.decl) = (yyvsp[(3) - (3)].decl);
	      SwigType_add_reference((yyvsp[(1) - (3)].type));
              if ((yyval.decl).type) {
		SwigType_push((yyvsp[(1) - (3)].type),(yyval.decl).type);
		Delete((yyval.decl).type);
	      }
	      (yyval.decl).type = (yyvsp[(1) - (3)].type);
           }
    break;

  case 259:

/* Line 1455 of yacc.c  */
#line 4954 "parser.y"
    {
              (yyval.decl) = (yyvsp[(1) - (1)].decl);
	      if (!(yyval.decl).type) (yyval.decl).type = NewStringEmpty();
           }
    break;

  case 260:

/* Line 1455 of yacc.c  */
#line 4958 "parser.y"
    { 
	     (yyval.decl) = (yyvsp[(2) - (2)].decl);
	     (yyval.decl).type = NewStringEmpty();
	     SwigType_add_reference((yyval.decl).type);
	     if ((yyvsp[(2) - (2)].decl).type) {
	       SwigType_push((yyval.decl).type,(yyvsp[(2) - (2)].decl).type);
	       Delete((yyvsp[(2) - (2)].decl).type);
	     }
           }
    break;

  case 261:

/* Line 1455 of yacc.c  */
#line 4967 "parser.y"
    { 
	     SwigType *t = NewStringEmpty();

	     (yyval.decl) = (yyvsp[(3) - (3)].decl);
	     SwigType_add_memberpointer(t,(yyvsp[(1) - (3)].str));
	     if ((yyval.decl).type) {
	       SwigType_push(t,(yyval.decl).type);
	       Delete((yyval.decl).type);
	     }
	     (yyval.decl).type = t;
	     }
    break;

  case 262:

/* Line 1455 of yacc.c  */
#line 4978 "parser.y"
    { 
	     SwigType *t = NewStringEmpty();
	     (yyval.decl) = (yyvsp[(4) - (4)].decl);
	     SwigType_add_memberpointer(t,(yyvsp[(2) - (4)].str));
	     SwigType_push((yyvsp[(1) - (4)].type),t);
	     if ((yyval.decl).type) {
	       SwigType_push((yyvsp[(1) - (4)].type),(yyval.decl).type);
	       Delete((yyval.decl).type);
	     }
	     (yyval.decl).type = (yyvsp[(1) - (4)].type);
	     Delete(t);
	   }
    break;

  case 263:

/* Line 1455 of yacc.c  */
#line 4990 "parser.y"
    { 
	     (yyval.decl) = (yyvsp[(5) - (5)].decl);
	     SwigType_add_memberpointer((yyvsp[(1) - (5)].type),(yyvsp[(2) - (5)].str));
	     SwigType_add_reference((yyvsp[(1) - (5)].type));
	     if ((yyval.decl).type) {
	       SwigType_push((yyvsp[(1) - (5)].type),(yyval.decl).type);
	       Delete((yyval.decl).type);
	     }
	     (yyval.decl).type = (yyvsp[(1) - (5)].type);
	   }
    break;

  case 264:

/* Line 1455 of yacc.c  */
#line 5000 "parser.y"
    { 
	     SwigType *t = NewStringEmpty();
	     (yyval.decl) = (yyvsp[(4) - (4)].decl);
	     SwigType_add_memberpointer(t,(yyvsp[(1) - (4)].str));
	     SwigType_add_reference(t);
	     if ((yyval.decl).type) {
	       SwigType_push(t,(yyval.decl).type);
	       Delete((yyval.decl).type);
	     } 
	     (yyval.decl).type = t;
	   }
    break;

  case 265:

/* Line 1455 of yacc.c  */
#line 5013 "parser.y"
    {
                /* Note: This is non-standard C.  Template declarator is allowed to follow an identifier */
                 (yyval.decl).id = Char((yyvsp[(1) - (1)].str));
		 (yyval.decl).type = 0;
		 (yyval.decl).parms = 0;
		 (yyval.decl).have_parms = 0;
                  }
    break;

  case 266:

/* Line 1455 of yacc.c  */
#line 5020 "parser.y"
    {
                  (yyval.decl).id = Char(NewStringf("~%s",(yyvsp[(2) - (2)].str)));
                  (yyval.decl).type = 0;
                  (yyval.decl).parms = 0;
                  (yyval.decl).have_parms = 0;
                  }
    break;

  case 267:

/* Line 1455 of yacc.c  */
#line 5028 "parser.y"
    {
                  (yyval.decl).id = Char((yyvsp[(2) - (3)].str));
                  (yyval.decl).type = 0;
                  (yyval.decl).parms = 0;
                  (yyval.decl).have_parms = 0;
                  }
    break;

  case 268:

/* Line 1455 of yacc.c  */
#line 5044 "parser.y"
    {
		    (yyval.decl) = (yyvsp[(3) - (4)].decl);
		    if ((yyval.decl).type) {
		      SwigType_push((yyvsp[(2) - (4)].type),(yyval.decl).type);
		      Delete((yyval.decl).type);
		    }
		    (yyval.decl).type = (yyvsp[(2) - (4)].type);
                  }
    break;

  case 269:

/* Line 1455 of yacc.c  */
#line 5052 "parser.y"
    {
		    SwigType *t;
		    (yyval.decl) = (yyvsp[(4) - (5)].decl);
		    t = NewStringEmpty();
		    SwigType_add_memberpointer(t,(yyvsp[(2) - (5)].str));
		    if ((yyval.decl).type) {
		      SwigType_push(t,(yyval.decl).type);
		      Delete((yyval.decl).type);
		    }
		    (yyval.decl).type = t;
		    }
    break;

  case 270:

/* Line 1455 of yacc.c  */
#line 5063 "parser.y"
    { 
		    SwigType *t;
		    (yyval.decl) = (yyvsp[(1) - (3)].decl);
		    t = NewStringEmpty();
		    SwigType_add_array(t,(char*)"");
		    if ((yyval.decl).type) {
		      SwigType_push(t,(yyval.decl).type);
		      Delete((yyval.decl).type);
		    }
		    (yyval.decl).type = t;
                  }
    break;

  case 271:

/* Line 1455 of yacc.c  */
#line 5074 "parser.y"
    { 
		    SwigType *t;
		    (yyval.decl) = (yyvsp[(1) - (4)].decl);
		    t = NewStringEmpty();
		    SwigType_add_array(t,(yyvsp[(3) - (4)].dtype).val);
		    if ((yyval.decl).type) {
		      SwigType_push(t,(yyval.decl).type);
		      Delete((yyval.decl).type);
		    }
		    (yyval.decl).type = t;
                  }
    break;

  case 272:

/* Line 1455 of yacc.c  */
#line 5085 "parser.y"
    {
		    SwigType *t;
                    (yyval.decl) = (yyvsp[(1) - (4)].decl);
		    t = NewStringEmpty();
		    SwigType_add_function(t,(yyvsp[(3) - (4)].pl));
		    if (!(yyval.decl).have_parms) {
		      (yyval.decl).parms = (yyvsp[(3) - (4)].pl);
		      (yyval.decl).have_parms = 1;
		    }
		    if (!(yyval.decl).type) {
		      (yyval.decl).type = t;
		    } else {
		      SwigType_push(t, (yyval.decl).type);
		      Delete((yyval.decl).type);
		      (yyval.decl).type = t;
		    }
		  }
    break;

  case 273:

/* Line 1455 of yacc.c  */
#line 5104 "parser.y"
    {
                /* Note: This is non-standard C.  Template declarator is allowed to follow an identifier */
                 (yyval.decl).id = Char((yyvsp[(1) - (1)].str));
		 (yyval.decl).type = 0;
		 (yyval.decl).parms = 0;
		 (yyval.decl).have_parms = 0;
                  }
    break;

  case 274:

/* Line 1455 of yacc.c  */
#line 5112 "parser.y"
    {
                  (yyval.decl).id = Char(NewStringf("~%s",(yyvsp[(2) - (2)].str)));
                  (yyval.decl).type = 0;
                  (yyval.decl).parms = 0;
                  (yyval.decl).have_parms = 0;
                  }
    break;

  case 275:

/* Line 1455 of yacc.c  */
#line 5129 "parser.y"
    {
		    (yyval.decl) = (yyvsp[(3) - (4)].decl);
		    if ((yyval.decl).type) {
		      SwigType_push((yyvsp[(2) - (4)].type),(yyval.decl).type);
		      Delete((yyval.decl).type);
		    }
		    (yyval.decl).type = (yyvsp[(2) - (4)].type);
                  }
    break;

  case 276:

/* Line 1455 of yacc.c  */
#line 5137 "parser.y"
    {
                    (yyval.decl) = (yyvsp[(3) - (4)].decl);
		    if (!(yyval.decl).type) {
		      (yyval.decl).type = NewStringEmpty();
		    }
		    SwigType_add_reference((yyval.decl).type);
                  }
    break;

  case 277:

/* Line 1455 of yacc.c  */
#line 5144 "parser.y"
    {
		    SwigType *t;
		    (yyval.decl) = (yyvsp[(4) - (5)].decl);
		    t = NewStringEmpty();
		    SwigType_add_memberpointer(t,(yyvsp[(2) - (5)].str));
		    if ((yyval.decl).type) {
		      SwigType_push(t,(yyval.decl).type);
		      Delete((yyval.decl).type);
		    }
		    (yyval.decl).type = t;
		    }
    break;

  case 278:

/* Line 1455 of yacc.c  */
#line 5155 "parser.y"
    { 
		    SwigType *t;
		    (yyval.decl) = (yyvsp[(1) - (3)].decl);
		    t = NewStringEmpty();
		    SwigType_add_array(t,(char*)"");
		    if ((yyval.decl).type) {
		      SwigType_push(t,(yyval.decl).type);
		      Delete((yyval.decl).type);
		    }
		    (yyval.decl).type = t;
                  }
    break;

  case 279:

/* Line 1455 of yacc.c  */
#line 5166 "parser.y"
    { 
		    SwigType *t;
		    (yyval.decl) = (yyvsp[(1) - (4)].decl);
		    t = NewStringEmpty();
		    SwigType_add_array(t,(yyvsp[(3) - (4)].dtype).val);
		    if ((yyval.decl).type) {
		      SwigType_push(t,(yyval.decl).type);
		      Delete((yyval.decl).type);
		    }
		    (yyval.decl).type = t;
                  }
    break;

  case 280:

/* Line 1455 of yacc.c  */
#line 5177 "parser.y"
    {
		    SwigType *t;
                    (yyval.decl) = (yyvsp[(1) - (4)].decl);
		    t = NewStringEmpty();
		    SwigType_add_function(t,(yyvsp[(3) - (4)].pl));
		    if (!(yyval.decl).have_parms) {
		      (yyval.decl).parms = (yyvsp[(3) - (4)].pl);
		      (yyval.decl).have_parms = 1;
		    }
		    if (!(yyval.decl).type) {
		      (yyval.decl).type = t;
		    } else {
		      SwigType_push(t, (yyval.decl).type);
		      Delete((yyval.decl).type);
		      (yyval.decl).type = t;
		    }
		  }
    break;

  case 281:

/* Line 1455 of yacc.c  */
#line 5196 "parser.y"
    {
		    (yyval.decl).type = (yyvsp[(1) - (1)].type);
                    (yyval.decl).id = 0;
		    (yyval.decl).parms = 0;
		    (yyval.decl).have_parms = 0;
                  }
    break;

  case 282:

/* Line 1455 of yacc.c  */
#line 5202 "parser.y"
    { 
                     (yyval.decl) = (yyvsp[(2) - (2)].decl);
                     SwigType_push((yyvsp[(1) - (2)].type),(yyvsp[(2) - (2)].decl).type);
		     (yyval.decl).type = (yyvsp[(1) - (2)].type);
		     Delete((yyvsp[(2) - (2)].decl).type);
                  }
    break;

  case 283:

/* Line 1455 of yacc.c  */
#line 5208 "parser.y"
    {
		    (yyval.decl).type = (yyvsp[(1) - (2)].type);
		    SwigType_add_reference((yyval.decl).type);
		    (yyval.decl).id = 0;
		    (yyval.decl).parms = 0;
		    (yyval.decl).have_parms = 0;
		  }
    break;

  case 284:

/* Line 1455 of yacc.c  */
#line 5215 "parser.y"
    {
		    (yyval.decl) = (yyvsp[(3) - (3)].decl);
		    SwigType_add_reference((yyvsp[(1) - (3)].type));
		    if ((yyval.decl).type) {
		      SwigType_push((yyvsp[(1) - (3)].type),(yyval.decl).type);
		      Delete((yyval.decl).type);
		    }
		    (yyval.decl).type = (yyvsp[(1) - (3)].type);
                  }
    break;

  case 285:

/* Line 1455 of yacc.c  */
#line 5224 "parser.y"
    {
		    (yyval.decl) = (yyvsp[(1) - (1)].decl);
                  }
    break;

  case 286:

/* Line 1455 of yacc.c  */
#line 5227 "parser.y"
    {
		    (yyval.decl) = (yyvsp[(2) - (2)].decl);
		    (yyval.decl).type = NewStringEmpty();
		    SwigType_add_reference((yyval.decl).type);
		    if ((yyvsp[(2) - (2)].decl).type) {
		      SwigType_push((yyval.decl).type,(yyvsp[(2) - (2)].decl).type);
		      Delete((yyvsp[(2) - (2)].decl).type);
		    }
                  }
    break;

  case 287:

/* Line 1455 of yacc.c  */
#line 5236 "parser.y"
    { 
                    (yyval.decl).id = 0;
                    (yyval.decl).parms = 0;
		    (yyval.decl).have_parms = 0;
                    (yyval.decl).type = NewStringEmpty();
		    SwigType_add_reference((yyval.decl).type);
                  }
    break;

  case 288:

/* Line 1455 of yacc.c  */
#line 5243 "parser.y"
    { 
		    (yyval.decl).type = NewStringEmpty();
                    SwigType_add_memberpointer((yyval.decl).type,(yyvsp[(1) - (2)].str));
                    (yyval.decl).id = 0;
                    (yyval.decl).parms = 0;
		    (yyval.decl).have_parms = 0;
      	          }
    break;

  case 289:

/* Line 1455 of yacc.c  */
#line 5250 "parser.y"
    { 
		    SwigType *t = NewStringEmpty();
                    (yyval.decl).type = (yyvsp[(1) - (3)].type);
		    (yyval.decl).id = 0;
		    (yyval.decl).parms = 0;
		    (yyval.decl).have_parms = 0;
		    SwigType_add_memberpointer(t,(yyvsp[(2) - (3)].str));
		    SwigType_push((yyval.decl).type,t);
		    Delete(t);
                  }
    break;

  case 290:

/* Line 1455 of yacc.c  */
#line 5260 "parser.y"
    { 
		    (yyval.decl) = (yyvsp[(4) - (4)].decl);
		    SwigType_add_memberpointer((yyvsp[(1) - (4)].type),(yyvsp[(2) - (4)].str));
		    if ((yyval.decl).type) {
		      SwigType_push((yyvsp[(1) - (4)].type),(yyval.decl).type);
		      Delete((yyval.decl).type);
		    }
		    (yyval.decl).type = (yyvsp[(1) - (4)].type);
                  }
    break;

  case 291:

/* Line 1455 of yacc.c  */
#line 5271 "parser.y"
    { 
		    SwigType *t;
		    (yyval.decl) = (yyvsp[(1) - (3)].decl);
		    t = NewStringEmpty();
		    SwigType_add_array(t,(char*)"");
		    if ((yyval.decl).type) {
		      SwigType_push(t,(yyval.decl).type);
		      Delete((yyval.decl).type);
		    }
		    (yyval.decl).type = t;
                  }
    break;

  case 292:

/* Line 1455 of yacc.c  */
#line 5282 "parser.y"
    { 
		    SwigType *t;
		    (yyval.decl) = (yyvsp[(1) - (4)].decl);
		    t = NewStringEmpty();
		    SwigType_add_array(t,(yyvsp[(3) - (4)].dtype).val);
		    if ((yyval.decl).type) {
		      SwigType_push(t,(yyval.decl).type);
		      Delete((yyval.decl).type);
		    }
		    (yyval.decl).type = t;
                  }
    break;

  case 293:

/* Line 1455 of yacc.c  */
#line 5293 "parser.y"
    { 
		    (yyval.decl).type = NewStringEmpty();
		    (yyval.decl).id = 0;
		    (yyval.decl).parms = 0;
		    (yyval.decl).have_parms = 0;
		    SwigType_add_array((yyval.decl).type,(char*)"");
                  }
    break;

  case 294:

/* Line 1455 of yacc.c  */
#line 5300 "parser.y"
    { 
		    (yyval.decl).type = NewStringEmpty();
		    (yyval.decl).id = 0;
		    (yyval.decl).parms = 0;
		    (yyval.decl).have_parms = 0;
		    SwigType_add_array((yyval.decl).type,(yyvsp[(2) - (3)].dtype).val);
		  }
    break;

  case 295:

/* Line 1455 of yacc.c  */
#line 5307 "parser.y"
    {
                    (yyval.decl) = (yyvsp[(2) - (3)].decl);
		  }
    break;

  case 296:

/* Line 1455 of yacc.c  */
#line 5310 "parser.y"
    {
		    SwigType *t;
                    (yyval.decl) = (yyvsp[(1) - (4)].decl);
		    t = NewStringEmpty();
                    SwigType_add_function(t,(yyvsp[(3) - (4)].pl));
		    if (!(yyval.decl).type) {
		      (yyval.decl).type = t;
		    } else {
		      SwigType_push(t,(yyval.decl).type);
		      Delete((yyval.decl).type);
		      (yyval.decl).type = t;
		    }
		    if (!(yyval.decl).have_parms) {
		      (yyval.decl).parms = (yyvsp[(3) - (4)].pl);
		      (yyval.decl).have_parms = 1;
		    }
		  }
    break;

  case 297:

/* Line 1455 of yacc.c  */
#line 5327 "parser.y"
    {
                    (yyval.decl).type = NewStringEmpty();
                    SwigType_add_function((yyval.decl).type,(yyvsp[(2) - (3)].pl));
		    (yyval.decl).parms = (yyvsp[(2) - (3)].pl);
		    (yyval.decl).have_parms = 1;
		    (yyval.decl).id = 0;
                  }
    break;

  case 298:

/* Line 1455 of yacc.c  */
#line 5337 "parser.y"
    { 
               (yyval.type) = NewStringEmpty();
               SwigType_add_pointer((yyval.type));
	       SwigType_push((yyval.type),(yyvsp[(2) - (3)].str));
	       SwigType_push((yyval.type),(yyvsp[(3) - (3)].type));
	       Delete((yyvsp[(3) - (3)].type));
           }
    break;

  case 299:

/* Line 1455 of yacc.c  */
#line 5344 "parser.y"
    {
	     (yyval.type) = NewStringEmpty();
	     SwigType_add_pointer((yyval.type));
	     SwigType_push((yyval.type),(yyvsp[(2) - (2)].type));
	     Delete((yyvsp[(2) - (2)].type));
	     }
    break;

  case 300:

/* Line 1455 of yacc.c  */
#line 5350 "parser.y"
    { 
	     	(yyval.type) = NewStringEmpty();	
		SwigType_add_pointer((yyval.type));
	        SwigType_push((yyval.type),(yyvsp[(2) - (2)].str));
           }
    break;

  case 301:

/* Line 1455 of yacc.c  */
#line 5355 "parser.y"
    {
	      (yyval.type) = NewStringEmpty();
	      SwigType_add_pointer((yyval.type));
           }
    break;

  case 302:

/* Line 1455 of yacc.c  */
#line 5361 "parser.y"
    {
	          (yyval.str) = NewStringEmpty();
	          if ((yyvsp[(1) - (1)].id)) SwigType_add_qualifier((yyval.str),(yyvsp[(1) - (1)].id));
               }
    break;

  case 303:

/* Line 1455 of yacc.c  */
#line 5365 "parser.y"
    {
		  (yyval.str) = (yyvsp[(2) - (2)].str);
	          if ((yyvsp[(1) - (2)].id)) SwigType_add_qualifier((yyval.str),(yyvsp[(1) - (2)].id));
               }
    break;

  case 304:

/* Line 1455 of yacc.c  */
#line 5371 "parser.y"
    { (yyval.id) = "const"; }
    break;

  case 305:

/* Line 1455 of yacc.c  */
#line 5372 "parser.y"
    { (yyval.id) = "volatile"; }
    break;

  case 306:

/* Line 1455 of yacc.c  */
#line 5373 "parser.y"
    { (yyval.id) = 0; }
    break;

  case 307:

/* Line 1455 of yacc.c  */
#line 5379 "parser.y"
    {
                   (yyval.type) = (yyvsp[(1) - (1)].type);
                   Replace((yyval.type),"typename ","", DOH_REPLACE_ANY);
                }
    break;

  case 308:

/* Line 1455 of yacc.c  */
#line 5385 "parser.y"
    {
                   (yyval.type) = (yyvsp[(2) - (2)].type);
	           SwigType_push((yyval.type),(yyvsp[(1) - (2)].str));
               }
    break;

  case 309:

/* Line 1455 of yacc.c  */
#line 5389 "parser.y"
    { (yyval.type) = (yyvsp[(1) - (1)].type); }
    break;

  case 310:

/* Line 1455 of yacc.c  */
#line 5390 "parser.y"
    {
		  (yyval.type) = (yyvsp[(1) - (2)].type);
	          SwigType_push((yyval.type),(yyvsp[(2) - (2)].str));
	       }
    break;

  case 311:

/* Line 1455 of yacc.c  */
#line 5394 "parser.y"
    {
		  (yyval.type) = (yyvsp[(2) - (3)].type);
	          SwigType_push((yyval.type),(yyvsp[(3) - (3)].str));
	          SwigType_push((yyval.type),(yyvsp[(1) - (3)].str));
	       }
    break;

  case 312:

/* Line 1455 of yacc.c  */
#line 5401 "parser.y"
    { (yyval.type) = (yyvsp[(1) - (1)].type);
                  /* Printf(stdout,"primitive = '%s'\n", $$);*/
                }
    break;

  case 313:

/* Line 1455 of yacc.c  */
#line 5404 "parser.y"
    { (yyval.type) = (yyvsp[(1) - (1)].type); }
    break;

  case 314:

/* Line 1455 of yacc.c  */
#line 5405 "parser.y"
    { (yyval.type) = (yyvsp[(1) - (1)].type); }
    break;

  case 315:

/* Line 1455 of yacc.c  */
#line 5406 "parser.y"
    { (yyval.type) = NewStringf("%s%s",(yyvsp[(1) - (2)].type),(yyvsp[(2) - (2)].id)); }
    break;

  case 316:

/* Line 1455 of yacc.c  */
#line 5407 "parser.y"
    { (yyval.type) = NewStringf("enum %s", (yyvsp[(2) - (2)].str)); }
    break;

  case 317:

/* Line 1455 of yacc.c  */
#line 5408 "parser.y"
    { (yyval.type) = (yyvsp[(1) - (1)].type); }
    break;

  case 318:

/* Line 1455 of yacc.c  */
#line 5410 "parser.y"
    {
		  (yyval.type) = (yyvsp[(1) - (1)].str);
               }
    break;

  case 319:

/* Line 1455 of yacc.c  */
#line 5413 "parser.y"
    { 
		 (yyval.type) = NewStringf("%s %s", (yyvsp[(1) - (2)].id), (yyvsp[(2) - (2)].str));
               }
    break;

  case 320:

/* Line 1455 of yacc.c  */
#line 5418 "parser.y"
    {
		 if (!(yyvsp[(1) - (1)].ptype).type) (yyvsp[(1) - (1)].ptype).type = NewString("int");
		 if ((yyvsp[(1) - (1)].ptype).us) {
		   (yyval.type) = NewStringf("%s %s", (yyvsp[(1) - (1)].ptype).us, (yyvsp[(1) - (1)].ptype).type);
		   Delete((yyvsp[(1) - (1)].ptype).us);
                   Delete((yyvsp[(1) - (1)].ptype).type);
		 } else {
                   (yyval.type) = (yyvsp[(1) - (1)].ptype).type;
		 }
		 if (Cmp((yyval.type),"signed int") == 0) {
		   Delete((yyval.type));
		   (yyval.type) = NewString("int");
                 } else if (Cmp((yyval.type),"signed long") == 0) {
		   Delete((yyval.type));
                   (yyval.type) = NewString("long");
                 } else if (Cmp((yyval.type),"signed short") == 0) {
		   Delete((yyval.type));
		   (yyval.type) = NewString("short");
		 } else if (Cmp((yyval.type),"signed long long") == 0) {
		   Delete((yyval.type));
		   (yyval.type) = NewString("long long");
		 }
               }
    break;

  case 321:

/* Line 1455 of yacc.c  */
#line 5443 "parser.y"
    { 
                 (yyval.ptype) = (yyvsp[(1) - (1)].ptype);
               }
    break;

  case 322:

/* Line 1455 of yacc.c  */
#line 5446 "parser.y"
    {
                    if ((yyvsp[(1) - (2)].ptype).us && (yyvsp[(2) - (2)].ptype).us) {
		      Swig_error(cparse_file, cparse_line, "Extra %s specifier.\n", (yyvsp[(2) - (2)].ptype).us);
		    }
                    (yyval.ptype) = (yyvsp[(2) - (2)].ptype);
                    if ((yyvsp[(1) - (2)].ptype).us) (yyval.ptype).us = (yyvsp[(1) - (2)].ptype).us;
		    if ((yyvsp[(1) - (2)].ptype).type) {
		      if (!(yyvsp[(2) - (2)].ptype).type) (yyval.ptype).type = (yyvsp[(1) - (2)].ptype).type;
		      else {
			int err = 0;
			if ((Cmp((yyvsp[(1) - (2)].ptype).type,"long") == 0)) {
			  if ((Cmp((yyvsp[(2) - (2)].ptype).type,"long") == 0) || (Strncmp((yyvsp[(2) - (2)].ptype).type,"double",6) == 0)) {
			    (yyval.ptype).type = NewStringf("long %s", (yyvsp[(2) - (2)].ptype).type);
			  } else if (Cmp((yyvsp[(2) - (2)].ptype).type,"int") == 0) {
			    (yyval.ptype).type = (yyvsp[(1) - (2)].ptype).type;
			  } else {
			    err = 1;
			  }
			} else if ((Cmp((yyvsp[(1) - (2)].ptype).type,"short")) == 0) {
			  if (Cmp((yyvsp[(2) - (2)].ptype).type,"int") == 0) {
			    (yyval.ptype).type = (yyvsp[(1) - (2)].ptype).type;
			  } else {
			    err = 1;
			  }
			} else if (Cmp((yyvsp[(1) - (2)].ptype).type,"int") == 0) {
			  (yyval.ptype).type = (yyvsp[(2) - (2)].ptype).type;
			} else if (Cmp((yyvsp[(1) - (2)].ptype).type,"double") == 0) {
			  if (Cmp((yyvsp[(2) - (2)].ptype).type,"long") == 0) {
			    (yyval.ptype).type = NewString("long double");
			  } else if (Cmp((yyvsp[(2) - (2)].ptype).type,"complex") == 0) {
			    (yyval.ptype).type = NewString("double complex");
			  } else {
			    err = 1;
			  }
			} else if (Cmp((yyvsp[(1) - (2)].ptype).type,"float") == 0) {
			  if (Cmp((yyvsp[(2) - (2)].ptype).type,"complex") == 0) {
			    (yyval.ptype).type = NewString("float complex");
			  } else {
			    err = 1;
			  }
			} else if (Cmp((yyvsp[(1) - (2)].ptype).type,"complex") == 0) {
			  (yyval.ptype).type = NewStringf("%s complex", (yyvsp[(2) - (2)].ptype).type);
			} else {
			  err = 1;
			}
			if (err) {
			  Swig_error(cparse_file, cparse_line, "Extra %s specifier.\n", (yyvsp[(1) - (2)].ptype).type);
			}
		      }
		    }
               }
    break;

  case 323:

/* Line 1455 of yacc.c  */
#line 5500 "parser.y"
    { 
		    (yyval.ptype).type = NewString("int");
                    (yyval.ptype).us = 0;
               }
    break;

  case 324:

/* Line 1455 of yacc.c  */
#line 5504 "parser.y"
    { 
                    (yyval.ptype).type = NewString("short");
                    (yyval.ptype).us = 0;
                }
    break;

  case 325:

/* Line 1455 of yacc.c  */
#line 5508 "parser.y"
    { 
                    (yyval.ptype).type = NewString("long");
                    (yyval.ptype).us = 0;
                }
    break;

  case 326:

/* Line 1455 of yacc.c  */
#line 5512 "parser.y"
    { 
                    (yyval.ptype).type = NewString("char");
                    (yyval.ptype).us = 0;
                }
    break;

  case 327:

/* Line 1455 of yacc.c  */
#line 5516 "parser.y"
    { 
                    (yyval.ptype).type = NewString("wchar_t");
                    (yyval.ptype).us = 0;
                }
    break;

  case 328:

/* Line 1455 of yacc.c  */
#line 5520 "parser.y"
    { 
                    (yyval.ptype).type = NewString("float");
                    (yyval.ptype).us = 0;
                }
    break;

  case 329:

/* Line 1455 of yacc.c  */
#line 5524 "parser.y"
    { 
                    (yyval.ptype).type = NewString("double");
                    (yyval.ptype).us = 0;
                }
    break;

  case 330:

/* Line 1455 of yacc.c  */
#line 5528 "parser.y"
    { 
                    (yyval.ptype).us = NewString("signed");
                    (yyval.ptype).type = 0;
                }
    break;

  case 331:

/* Line 1455 of yacc.c  */
#line 5532 "parser.y"
    { 
                    (yyval.ptype).us = NewString("unsigned");
                    (yyval.ptype).type = 0;
                }
    break;

  case 332:

/* Line 1455 of yacc.c  */
#line 5536 "parser.y"
    { 
                    (yyval.ptype).type = NewString("complex");
                    (yyval.ptype).us = 0;
                }
    break;

  case 333:

/* Line 1455 of yacc.c  */
#line 5540 "parser.y"
    { 
                    (yyval.ptype).type = NewString("__int8");
                    (yyval.ptype).us = 0;
                }
    break;

  case 334:

/* Line 1455 of yacc.c  */
#line 5544 "parser.y"
    { 
                    (yyval.ptype).type = NewString("__int16");
                    (yyval.ptype).us = 0;
                }
    break;

  case 335:

/* Line 1455 of yacc.c  */
#line 5548 "parser.y"
    { 
                    (yyval.ptype).type = NewString("__int32");
                    (yyval.ptype).us = 0;
                }
    break;

  case 336:

/* Line 1455 of yacc.c  */
#line 5552 "parser.y"
    { 
                    (yyval.ptype).type = NewString("__int64");
                    (yyval.ptype).us = 0;
                }
    break;

  case 337:

/* Line 1455 of yacc.c  */
#line 5558 "parser.y"
    { /* scanner_check_typedef(); */ }
    break;

  case 338:

/* Line 1455 of yacc.c  */
#line 5558 "parser.y"
    {
                   (yyval.dtype) = (yyvsp[(2) - (2)].dtype);
		   if ((yyval.dtype).type == T_STRING) {
		     (yyval.dtype).rawval = NewStringf("\"%(escape)s\"",(yyval.dtype).val);
		   } else if ((yyval.dtype).type != T_CHAR) {
		     (yyval.dtype).rawval = 0;
		   }
		   (yyval.dtype).bitfield = 0;
		   (yyval.dtype).throws = 0;
		   (yyval.dtype).throwf = 0;
		   scanner_ignore_typedef();
                }
    break;

  case 339:

/* Line 1455 of yacc.c  */
#line 5584 "parser.y"
    { (yyval.id) = (yyvsp[(1) - (1)].id); }
    break;

  case 340:

/* Line 1455 of yacc.c  */
#line 5585 "parser.y"
    { (yyval.id) = (char *) 0;}
    break;

  case 341:

/* Line 1455 of yacc.c  */
#line 5588 "parser.y"
    { 

                  /* Ignore if there is a trailing comma in the enum list */
                  if ((yyvsp[(3) - (3)].node)) {
                    Node *leftSibling = Getattr((yyvsp[(1) - (3)].node),"_last");
                    if (!leftSibling) {
                      leftSibling=(yyvsp[(1) - (3)].node);
                    }
                    set_nextSibling(leftSibling,(yyvsp[(3) - (3)].node));
                    Setattr((yyvsp[(1) - (3)].node),"_last",(yyvsp[(3) - (3)].node));
                  }
		  (yyval.node) = (yyvsp[(1) - (3)].node);
               }
    break;

  case 342:

/* Line 1455 of yacc.c  */
#line 5601 "parser.y"
    { 
                   (yyval.node) = (yyvsp[(1) - (1)].node); 
                   if ((yyvsp[(1) - (1)].node)) {
                     Setattr((yyvsp[(1) - (1)].node),"_last",(yyvsp[(1) - (1)].node));
                   }
               }
    break;

  case 343:

/* Line 1455 of yacc.c  */
#line 5609 "parser.y"
    {
		   SwigType *type = NewSwigType(T_INT);
		   (yyval.node) = new_node("enumitem");
		   Setattr((yyval.node),"name",(yyvsp[(1) - (1)].id));
		   Setattr((yyval.node),"type",type);
		   SetFlag((yyval.node),"feature:immutable");
		   Delete(type);
		 }
    break;

  case 344:

/* Line 1455 of yacc.c  */
#line 5617 "parser.y"
    {
		   SwigType *type = NewSwigType((yyvsp[(3) - (3)].dtype).type == T_BOOL ? T_BOOL : ((yyvsp[(3) - (3)].dtype).type == T_CHAR ? T_CHAR : T_INT));
		   (yyval.node) = new_node("enumitem");
		   Setattr((yyval.node),"name",(yyvsp[(1) - (3)].id));
		   Setattr((yyval.node),"type",type);
		   SetFlag((yyval.node),"feature:immutable");
		   Setattr((yyval.node),"enumvalue", (yyvsp[(3) - (3)].dtype).val);
		   Setattr((yyval.node),"value",(yyvsp[(1) - (3)].id));
		   Delete(type);
                 }
    break;

  case 345:

/* Line 1455 of yacc.c  */
#line 5627 "parser.y"
    { (yyval.node) = 0; }
    break;

  case 346:

/* Line 1455 of yacc.c  */
#line 5630 "parser.y"
    {
                   (yyval.dtype) = (yyvsp[(1) - (1)].dtype);
		   if (((yyval.dtype).type != T_INT) && ((yyval.dtype).type != T_UINT) &&
		       ((yyval.dtype).type != T_LONG) && ((yyval.dtype).type != T_ULONG) &&
		       ((yyval.dtype).type != T_SHORT) && ((yyval.dtype).type != T_USHORT) &&
		       ((yyval.dtype).type != T_SCHAR) && ((yyval.dtype).type != T_UCHAR) &&
		       ((yyval.dtype).type != T_CHAR) && ((yyval.dtype).type != T_BOOL)) {
		     Swig_error(cparse_file,cparse_line,"Type error. Expecting an integral type\n");
		   }
                }
    break;

  case 347:

/* Line 1455 of yacc.c  */
#line 5644 "parser.y"
    { (yyval.dtype) = (yyvsp[(1) - (1)].dtype); }
    break;

  case 348:

/* Line 1455 of yacc.c  */
#line 5645 "parser.y"
    {
		 Node *n;
		 (yyval.dtype).val = (yyvsp[(1) - (1)].type);
		 (yyval.dtype).type = T_INT;
		 /* Check if value is in scope */
		 n = Swig_symbol_clookup((yyvsp[(1) - (1)].type),0);
		 if (n) {
                   /* A band-aid for enum values used in expressions. */
                   if (Strcmp(nodeType(n),"enumitem") == 0) {
                     String *q = Swig_symbol_qualified(n);
                     if (q) {
                       (yyval.dtype).val = NewStringf("%s::%s", q, Getattr(n,"name"));
                       Delete(q);
                     }
                   }
		 }
               }
    break;

  case 349:

/* Line 1455 of yacc.c  */
#line 5664 "parser.y"
    { (yyval.dtype) = (yyvsp[(1) - (1)].dtype); }
    break;

  case 350:

/* Line 1455 of yacc.c  */
#line 5665 "parser.y"
    {
		    (yyval.dtype).val = NewString((yyvsp[(1) - (1)].id));
                    (yyval.dtype).type = T_STRING;
               }
    break;

  case 351:

/* Line 1455 of yacc.c  */
#line 5669 "parser.y"
    {
		  SwigType_push((yyvsp[(3) - (5)].type),(yyvsp[(4) - (5)].decl).type);
		  (yyval.dtype).val = NewStringf("sizeof(%s)",SwigType_str((yyvsp[(3) - (5)].type),0));
		  (yyval.dtype).type = T_ULONG;
               }
    break;

  case 352:

/* Line 1455 of yacc.c  */
#line 5674 "parser.y"
    { (yyval.dtype) = (yyvsp[(1) - (1)].dtype); }
    break;

  case 353:

/* Line 1455 of yacc.c  */
#line 5675 "parser.y"
    {
		  (yyval.dtype).val = NewString((yyvsp[(1) - (1)].str));
		  if (Len((yyval.dtype).val)) {
		    (yyval.dtype).rawval = NewStringf("'%(escape)s'", (yyval.dtype).val);
		  } else {
		    (yyval.dtype).rawval = NewString("'\\0'");
		  }
		  (yyval.dtype).type = T_CHAR;
		  (yyval.dtype).bitfield = 0;
		  (yyval.dtype).throws = 0;
		  (yyval.dtype).throwf = 0;
	       }
    break;

  case 354:

/* Line 1455 of yacc.c  */
#line 5689 "parser.y"
    {
   	            (yyval.dtype).val = NewStringf("(%s)",(yyvsp[(2) - (3)].dtype).val);
		    (yyval.dtype).type = (yyvsp[(2) - (3)].dtype).type;
   	       }
    break;

  case 355:

/* Line 1455 of yacc.c  */
#line 5696 "parser.y"
    {
                 (yyval.dtype) = (yyvsp[(4) - (4)].dtype);
		 if ((yyvsp[(4) - (4)].dtype).type != T_STRING) {
		   switch ((yyvsp[(2) - (4)].dtype).type) {
		     case T_FLOAT:
		     case T_DOUBLE:
		     case T_LONGDOUBLE:
		     case T_FLTCPLX:
		     case T_DBLCPLX:
		       (yyval.dtype).val = NewStringf("(%s)%s", (yyvsp[(2) - (4)].dtype).val, (yyvsp[(4) - (4)].dtype).val); /* SwigType_str and decimal points don't mix! */
		       break;
		     default:
		       (yyval.dtype).val = NewStringf("(%s) %s", SwigType_str((yyvsp[(2) - (4)].dtype).val,0), (yyvsp[(4) - (4)].dtype).val);
		       break;
		   }
		 }
 	       }
    break;

  case 356:

/* Line 1455 of yacc.c  */
#line 5713 "parser.y"
    {
                 (yyval.dtype) = (yyvsp[(5) - (5)].dtype);
		 if ((yyvsp[(5) - (5)].dtype).type != T_STRING) {
		   SwigType_push((yyvsp[(2) - (5)].dtype).val,(yyvsp[(3) - (5)].type));
		   (yyval.dtype).val = NewStringf("(%s) %s", SwigType_str((yyvsp[(2) - (5)].dtype).val,0), (yyvsp[(5) - (5)].dtype).val);
		 }
 	       }
    break;

  case 357:

/* Line 1455 of yacc.c  */
#line 5720 "parser.y"
    {
                 (yyval.dtype) = (yyvsp[(5) - (5)].dtype);
		 if ((yyvsp[(5) - (5)].dtype).type != T_STRING) {
		   SwigType_add_reference((yyvsp[(2) - (5)].dtype).val);
		   (yyval.dtype).val = NewStringf("(%s) %s", SwigType_str((yyvsp[(2) - (5)].dtype).val,0), (yyvsp[(5) - (5)].dtype).val);
		 }
 	       }
    break;

  case 358:

/* Line 1455 of yacc.c  */
#line 5727 "parser.y"
    {
                 (yyval.dtype) = (yyvsp[(6) - (6)].dtype);
		 if ((yyvsp[(6) - (6)].dtype).type != T_STRING) {
		   SwigType_push((yyvsp[(2) - (6)].dtype).val,(yyvsp[(3) - (6)].type));
		   SwigType_add_reference((yyvsp[(2) - (6)].dtype).val);
		   (yyval.dtype).val = NewStringf("(%s) %s", SwigType_str((yyvsp[(2) - (6)].dtype).val,0), (yyvsp[(6) - (6)].dtype).val);
		 }
 	       }
    break;

  case 359:

/* Line 1455 of yacc.c  */
#line 5735 "parser.y"
    {
		 (yyval.dtype) = (yyvsp[(2) - (2)].dtype);
                 (yyval.dtype).val = NewStringf("&%s",(yyvsp[(2) - (2)].dtype).val);
	       }
    break;

  case 360:

/* Line 1455 of yacc.c  */
#line 5739 "parser.y"
    {
		 (yyval.dtype) = (yyvsp[(2) - (2)].dtype);
                 (yyval.dtype).val = NewStringf("*%s",(yyvsp[(2) - (2)].dtype).val);
	       }
    break;

  case 361:

/* Line 1455 of yacc.c  */
#line 5745 "parser.y"
    { (yyval.dtype) = (yyvsp[(1) - (1)].dtype); }
    break;

  case 362:

/* Line 1455 of yacc.c  */
#line 5746 "parser.y"
    { (yyval.dtype) = (yyvsp[(1) - (1)].dtype); }
    break;

  case 363:

/* Line 1455 of yacc.c  */
#line 5747 "parser.y"
    { (yyval.dtype) = (yyvsp[(1) - (1)].dtype); }
    break;

  case 364:

/* Line 1455 of yacc.c  */
#line 5748 "parser.y"
    { (yyval.dtype) = (yyvsp[(1) - (1)].dtype); }
    break;

  case 365:

/* Line 1455 of yacc.c  */
#line 5749 "parser.y"
    { (yyval.dtype) = (yyvsp[(1) - (1)].dtype); }
    break;

  case 366:

/* Line 1455 of yacc.c  */
#line 5750 "parser.y"
    { (yyval.dtype) = (yyvsp[(1) - (1)].dtype); }
    break;

  case 367:

/* Line 1455 of yacc.c  */
#line 5751 "parser.y"
    { (yyval.dtype) = (yyvsp[(1) - (1)].dtype); }
    break;

  case 368:

/* Line 1455 of yacc.c  */
#line 5752 "parser.y"
    { (yyval.dtype) = (yyvsp[(1) - (1)].dtype); }
    break;

  case 369:

/* Line 1455 of yacc.c  */
#line 5755 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s+%s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = promote((yyvsp[(1) - (3)].dtype).type,(yyvsp[(3) - (3)].dtype).type);
	       }
    break;

  case 370:

/* Line 1455 of yacc.c  */
#line 5759 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s-%s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = promote((yyvsp[(1) - (3)].dtype).type,(yyvsp[(3) - (3)].dtype).type);
	       }
    break;

  case 371:

/* Line 1455 of yacc.c  */
#line 5763 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s*%s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = promote((yyvsp[(1) - (3)].dtype).type,(yyvsp[(3) - (3)].dtype).type);
	       }
    break;

  case 372:

/* Line 1455 of yacc.c  */
#line 5767 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s/%s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = promote((yyvsp[(1) - (3)].dtype).type,(yyvsp[(3) - (3)].dtype).type);
	       }
    break;

  case 373:

/* Line 1455 of yacc.c  */
#line 5771 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s%%%s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = promote((yyvsp[(1) - (3)].dtype).type,(yyvsp[(3) - (3)].dtype).type);
	       }
    break;

  case 374:

/* Line 1455 of yacc.c  */
#line 5775 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s&%s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = promote((yyvsp[(1) - (3)].dtype).type,(yyvsp[(3) - (3)].dtype).type);
	       }
    break;

  case 375:

/* Line 1455 of yacc.c  */
#line 5779 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s|%s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = promote((yyvsp[(1) - (3)].dtype).type,(yyvsp[(3) - (3)].dtype).type);
	       }
    break;

  case 376:

/* Line 1455 of yacc.c  */
#line 5783 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s^%s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = promote((yyvsp[(1) - (3)].dtype).type,(yyvsp[(3) - (3)].dtype).type);
	       }
    break;

  case 377:

/* Line 1455 of yacc.c  */
#line 5787 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s << %s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = promote_type((yyvsp[(1) - (3)].dtype).type);
	       }
    break;

  case 378:

/* Line 1455 of yacc.c  */
#line 5791 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s >> %s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = promote_type((yyvsp[(1) - (3)].dtype).type);
	       }
    break;

  case 379:

/* Line 1455 of yacc.c  */
#line 5795 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s&&%s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = cparse_cplusplus ? T_BOOL : T_INT;
	       }
    break;

  case 380:

/* Line 1455 of yacc.c  */
#line 5799 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s||%s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = cparse_cplusplus ? T_BOOL : T_INT;
	       }
    break;

  case 381:

/* Line 1455 of yacc.c  */
#line 5803 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s==%s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = cparse_cplusplus ? T_BOOL : T_INT;
	       }
    break;

  case 382:

/* Line 1455 of yacc.c  */
#line 5807 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s!=%s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = cparse_cplusplus ? T_BOOL : T_INT;
	       }
    break;

  case 383:

/* Line 1455 of yacc.c  */
#line 5821 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s >= %s", (yyvsp[(1) - (3)].dtype).val, (yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = cparse_cplusplus ? T_BOOL : T_INT;
	       }
    break;

  case 384:

/* Line 1455 of yacc.c  */
#line 5825 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s <= %s", (yyvsp[(1) - (3)].dtype).val, (yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = cparse_cplusplus ? T_BOOL : T_INT;
	       }
    break;

  case 385:

/* Line 1455 of yacc.c  */
#line 5829 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s?%s:%s", (yyvsp[(1) - (5)].dtype).val, (yyvsp[(3) - (5)].dtype).val, (yyvsp[(5) - (5)].dtype).val);
		 /* This may not be exactly right, but is probably good enough
		  * for the purposes of parsing constant expressions. */
		 (yyval.dtype).type = promote((yyvsp[(3) - (5)].dtype).type, (yyvsp[(5) - (5)].dtype).type);
	       }
    break;

  case 386:

/* Line 1455 of yacc.c  */
#line 5835 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("-%s",(yyvsp[(2) - (2)].dtype).val);
		 (yyval.dtype).type = (yyvsp[(2) - (2)].dtype).type;
	       }
    break;

  case 387:

/* Line 1455 of yacc.c  */
#line 5839 "parser.y"
    {
                 (yyval.dtype).val = NewStringf("+%s",(yyvsp[(2) - (2)].dtype).val);
		 (yyval.dtype).type = (yyvsp[(2) - (2)].dtype).type;
	       }
    break;

  case 388:

/* Line 1455 of yacc.c  */
#line 5843 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("~%s",(yyvsp[(2) - (2)].dtype).val);
		 (yyval.dtype).type = (yyvsp[(2) - (2)].dtype).type;
	       }
    break;

  case 389:

/* Line 1455 of yacc.c  */
#line 5847 "parser.y"
    {
                 (yyval.dtype).val = NewStringf("!%s",(yyvsp[(2) - (2)].dtype).val);
		 (yyval.dtype).type = T_INT;
	       }
    break;

  case 390:

/* Line 1455 of yacc.c  */
#line 5851 "parser.y"
    {
		 String *qty;
                 skip_balanced('(',')');
		 qty = Swig_symbol_type_qualify((yyvsp[(1) - (2)].type),0);
		 if (SwigType_istemplate(qty)) {
		   String *nstr = SwigType_namestr(qty);
		   Delete(qty);
		   qty = nstr;
		 }
		 (yyval.dtype).val = NewStringf("%s%s",qty,scanner_ccode);
		 Clear(scanner_ccode);
		 (yyval.dtype).type = T_INT;
		 Delete(qty);
               }
    break;

  case 391:

/* Line 1455 of yacc.c  */
#line 5867 "parser.y"
    {
		 (yyval.bases) = (yyvsp[(1) - (1)].bases);
               }
    break;

  case 392:

/* Line 1455 of yacc.c  */
#line 5872 "parser.y"
    { inherit_list = 1; }
    break;

  case 393:

/* Line 1455 of yacc.c  */
#line 5872 "parser.y"
    { (yyval.bases) = (yyvsp[(3) - (3)].bases); inherit_list = 0; }
    break;

  case 394:

/* Line 1455 of yacc.c  */
#line 5873 "parser.y"
    { (yyval.bases) = 0; }
    break;

  case 395:

/* Line 1455 of yacc.c  */
#line 5876 "parser.y"
    {
		   Hash *list = NewHash();
		   Node *base = (yyvsp[(1) - (1)].node);
		   Node *name = Getattr(base,"name");
		   List *lpublic = NewList();
		   List *lprotected = NewList();
		   List *lprivate = NewList();
		   Setattr(list,"public",lpublic);
		   Setattr(list,"protected",lprotected);
		   Setattr(list,"private",lprivate);
		   Delete(lpublic);
		   Delete(lprotected);
		   Delete(lprivate);
		   Append(Getattr(list,Getattr(base,"access")),name);
	           (yyval.bases) = list;
               }
    break;

  case 396:

/* Line 1455 of yacc.c  */
#line 5893 "parser.y"
    {
		   Hash *list = (yyvsp[(1) - (3)].bases);
		   Node *base = (yyvsp[(3) - (3)].node);
		   Node *name = Getattr(base,"name");
		   Append(Getattr(list,Getattr(base,"access")),name);
                   (yyval.bases) = list;
               }
    break;

  case 397:

/* Line 1455 of yacc.c  */
#line 5902 "parser.y"
    {
		 (yyval.intvalue) = cparse_line;
	       }
    break;

  case 398:

/* Line 1455 of yacc.c  */
#line 5904 "parser.y"
    {
		 (yyval.node) = NewHash();
		 Setfile((yyval.node),cparse_file);
		 Setline((yyval.node),(yyvsp[(2) - (3)].intvalue));
		 Setattr((yyval.node),"name",(yyvsp[(3) - (3)].str));
		 Setfile((yyvsp[(3) - (3)].str),cparse_file);
		 Setline((yyvsp[(3) - (3)].str),(yyvsp[(2) - (3)].intvalue));
                 if (last_cpptype && (Strcmp(last_cpptype,"struct") != 0)) {
		   Setattr((yyval.node),"access","private");
		   Swig_warning(WARN_PARSE_NO_ACCESS, Getfile((yyval.node)), Getline((yyval.node)), "No access specifier given for base class '%s' (ignored).\n", SwigType_namestr((yyvsp[(3) - (3)].str)));
                 } else {
		   Setattr((yyval.node),"access","public");
		 }
               }
    break;

  case 399:

/* Line 1455 of yacc.c  */
#line 5918 "parser.y"
    {
		 (yyval.intvalue) = cparse_line;
	       }
    break;

  case 400:

/* Line 1455 of yacc.c  */
#line 5920 "parser.y"
    {
		 (yyval.node) = NewHash();
		 Setfile((yyval.node),cparse_file);
		 Setline((yyval.node),(yyvsp[(3) - (5)].intvalue));
		 Setattr((yyval.node),"name",(yyvsp[(5) - (5)].str));
		 Setfile((yyvsp[(5) - (5)].str),cparse_file);
		 Setline((yyvsp[(5) - (5)].str),(yyvsp[(3) - (5)].intvalue));
		 Setattr((yyval.node),"access",(yyvsp[(2) - (5)].id));
	         if (Strcmp((yyvsp[(2) - (5)].id),"public") != 0) {
		   Swig_warning(WARN_PARSE_PRIVATE_INHERIT, Getfile((yyval.node)), Getline((yyval.node)), "%s inheritance from base '%s' (ignored).\n", (yyvsp[(2) - (5)].id), SwigType_namestr((yyvsp[(5) - (5)].str)));
		 }
               }
    break;

  case 401:

/* Line 1455 of yacc.c  */
#line 5934 "parser.y"
    { (yyval.id) = (char*)"public"; }
    break;

  case 402:

/* Line 1455 of yacc.c  */
#line 5935 "parser.y"
    { (yyval.id) = (char*)"private"; }
    break;

  case 403:

/* Line 1455 of yacc.c  */
#line 5936 "parser.y"
    { (yyval.id) = (char*)"protected"; }
    break;

  case 404:

/* Line 1455 of yacc.c  */
#line 5940 "parser.y"
    { 
                   (yyval.id) = (char*)"class"; 
		   if (!inherit_list) last_cpptype = (yyval.id);
               }
    break;

  case 405:

/* Line 1455 of yacc.c  */
#line 5944 "parser.y"
    { 
                   (yyval.id) = (char *)"typename"; 
		   if (!inherit_list) last_cpptype = (yyval.id);
               }
    break;

  case 406:

/* Line 1455 of yacc.c  */
#line 5950 "parser.y"
    {
                 (yyval.id) = (yyvsp[(1) - (1)].id);
               }
    break;

  case 407:

/* Line 1455 of yacc.c  */
#line 5953 "parser.y"
    { 
                   (yyval.id) = (char*)"struct"; 
		   if (!inherit_list) last_cpptype = (yyval.id);
               }
    break;

  case 408:

/* Line 1455 of yacc.c  */
#line 5957 "parser.y"
    {
                   (yyval.id) = (char*)"union"; 
		   if (!inherit_list) last_cpptype = (yyval.id);
               }
    break;

  case 411:

/* Line 1455 of yacc.c  */
#line 5967 "parser.y"
    {
                    (yyval.dtype).qualifier = (yyvsp[(1) - (1)].str);
                    (yyval.dtype).throws = 0;
                    (yyval.dtype).throwf = 0;
               }
    break;

  case 412:

/* Line 1455 of yacc.c  */
#line 5972 "parser.y"
    {
                    (yyval.dtype).qualifier = 0;
                    (yyval.dtype).throws = (yyvsp[(3) - (4)].pl);
                    (yyval.dtype).throwf = NewString("1");
               }
    break;

  case 413:

/* Line 1455 of yacc.c  */
#line 5977 "parser.y"
    {
                    (yyval.dtype).qualifier = (yyvsp[(1) - (5)].str);
                    (yyval.dtype).throws = (yyvsp[(4) - (5)].pl);
                    (yyval.dtype).throwf = NewString("1");
               }
    break;

  case 414:

/* Line 1455 of yacc.c  */
#line 5982 "parser.y"
    { 
                    (yyval.dtype).qualifier = 0; 
                    (yyval.dtype).throws = 0;
                    (yyval.dtype).throwf = 0;
               }
    break;

  case 415:

/* Line 1455 of yacc.c  */
#line 5989 "parser.y"
    { 
                    Clear(scanner_ccode); 
                    (yyval.decl).have_parms = 0; 
                    (yyval.decl).defarg = 0; 
		    (yyval.decl).throws = (yyvsp[(1) - (3)].dtype).throws;
		    (yyval.decl).throwf = (yyvsp[(1) - (3)].dtype).throwf;
               }
    break;

  case 416:

/* Line 1455 of yacc.c  */
#line 5996 "parser.y"
    { 
                    skip_balanced('{','}'); 
                    (yyval.decl).have_parms = 0; 
                    (yyval.decl).defarg = 0; 
                    (yyval.decl).throws = (yyvsp[(1) - (3)].dtype).throws;
                    (yyval.decl).throwf = (yyvsp[(1) - (3)].dtype).throwf;
               }
    break;

  case 417:

/* Line 1455 of yacc.c  */
#line 6003 "parser.y"
    { 
                    Clear(scanner_ccode); 
                    (yyval.decl).parms = (yyvsp[(2) - (4)].pl); 
                    (yyval.decl).have_parms = 1; 
                    (yyval.decl).defarg = 0; 
		    (yyval.decl).throws = 0;
		    (yyval.decl).throwf = 0;
               }
    break;

  case 418:

/* Line 1455 of yacc.c  */
#line 6011 "parser.y"
    {
                    skip_balanced('{','}'); 
                    (yyval.decl).parms = (yyvsp[(2) - (4)].pl); 
                    (yyval.decl).have_parms = 1; 
                    (yyval.decl).defarg = 0; 
                    (yyval.decl).throws = 0;
                    (yyval.decl).throwf = 0;
               }
    break;

  case 419:

/* Line 1455 of yacc.c  */
#line 6019 "parser.y"
    { 
                    (yyval.decl).have_parms = 0; 
                    (yyval.decl).defarg = (yyvsp[(2) - (3)].dtype).val; 
                    (yyval.decl).throws = 0;
                    (yyval.decl).throwf = 0;
               }
    break;

  case 424:

/* Line 1455 of yacc.c  */
#line 6035 "parser.y"
    {
	            skip_balanced('(',')');
                    Clear(scanner_ccode);
            	}
    break;

  case 425:

/* Line 1455 of yacc.c  */
#line 6041 "parser.y"
    { 
                     String *s = NewStringEmpty();
                     SwigType_add_template(s,(yyvsp[(2) - (3)].p));
                     (yyval.id) = Char(s);
		     scanner_last_id(1);
                 }
    break;

  case 426:

/* Line 1455 of yacc.c  */
#line 6047 "parser.y"
    { (yyval.id) = (char*)"";  }
    break;

  case 427:

/* Line 1455 of yacc.c  */
#line 6050 "parser.y"
    { (yyval.id) = (yyvsp[(1) - (1)].id); }
    break;

  case 428:

/* Line 1455 of yacc.c  */
#line 6051 "parser.y"
    { (yyval.id) = (yyvsp[(1) - (1)].id); }
    break;

  case 429:

/* Line 1455 of yacc.c  */
#line 6054 "parser.y"
    { (yyval.id) = (yyvsp[(1) - (1)].id); }
    break;

  case 430:

/* Line 1455 of yacc.c  */
#line 6055 "parser.y"
    { (yyval.id) = 0; }
    break;

  case 431:

/* Line 1455 of yacc.c  */
#line 6058 "parser.y"
    { 
                  (yyval.str) = 0;
		  if (!(yyval.str)) (yyval.str) = NewStringf("%s%s", (yyvsp[(1) - (2)].str),(yyvsp[(2) - (2)].str));
      	          Delete((yyvsp[(2) - (2)].str));
               }
    break;

  case 432:

/* Line 1455 of yacc.c  */
#line 6063 "parser.y"
    { 
		 (yyval.str) = NewStringf("::%s%s",(yyvsp[(3) - (4)].str),(yyvsp[(4) - (4)].str));
                 Delete((yyvsp[(4) - (4)].str));
               }
    break;

  case 433:

/* Line 1455 of yacc.c  */
#line 6067 "parser.y"
    {
		 (yyval.str) = NewString((yyvsp[(1) - (1)].str));
   	       }
    break;

  case 434:

/* Line 1455 of yacc.c  */
#line 6070 "parser.y"
    {
		 (yyval.str) = NewStringf("::%s",(yyvsp[(3) - (3)].str));
               }
    break;

  case 435:

/* Line 1455 of yacc.c  */
#line 6073 "parser.y"
    {
                 (yyval.str) = NewString((yyvsp[(1) - (1)].str));
	       }
    break;

  case 436:

/* Line 1455 of yacc.c  */
#line 6076 "parser.y"
    {
                 (yyval.str) = NewStringf("::%s",(yyvsp[(3) - (3)].str));
               }
    break;

  case 437:

/* Line 1455 of yacc.c  */
#line 6081 "parser.y"
    {
                   (yyval.str) = NewStringf("::%s%s",(yyvsp[(2) - (3)].str),(yyvsp[(3) - (3)].str));
		   Delete((yyvsp[(3) - (3)].str));
               }
    break;

  case 438:

/* Line 1455 of yacc.c  */
#line 6085 "parser.y"
    {
                   (yyval.str) = NewStringf("::%s",(yyvsp[(2) - (2)].str));
               }
    break;

  case 439:

/* Line 1455 of yacc.c  */
#line 6088 "parser.y"
    {
                   (yyval.str) = NewStringf("::%s",(yyvsp[(2) - (2)].str));
               }
    break;

  case 440:

/* Line 1455 of yacc.c  */
#line 6095 "parser.y"
    {
		 (yyval.str) = NewStringf("::~%s",(yyvsp[(2) - (2)].str));
               }
    break;

  case 441:

/* Line 1455 of yacc.c  */
#line 6101 "parser.y"
    {
                  (yyval.str) = NewStringf("%s%s",(yyvsp[(1) - (2)].id),(yyvsp[(2) - (2)].id));
		  /*		  if (Len($2)) {
		    scanner_last_id(1);
		    } */
              }
    break;

  case 442:

/* Line 1455 of yacc.c  */
#line 6110 "parser.y"
    { 
                  (yyval.str) = 0;
		  if (!(yyval.str)) (yyval.str) = NewStringf("%s%s", (yyvsp[(1) - (2)].id),(yyvsp[(2) - (2)].str));
      	          Delete((yyvsp[(2) - (2)].str));
               }
    break;

  case 443:

/* Line 1455 of yacc.c  */
#line 6115 "parser.y"
    { 
		 (yyval.str) = NewStringf("::%s%s",(yyvsp[(3) - (4)].id),(yyvsp[(4) - (4)].str));
                 Delete((yyvsp[(4) - (4)].str));
               }
    break;

  case 444:

/* Line 1455 of yacc.c  */
#line 6119 "parser.y"
    {
		 (yyval.str) = NewString((yyvsp[(1) - (1)].id));
   	       }
    break;

  case 445:

/* Line 1455 of yacc.c  */
#line 6122 "parser.y"
    {
		 (yyval.str) = NewStringf("::%s",(yyvsp[(3) - (3)].id));
               }
    break;

  case 446:

/* Line 1455 of yacc.c  */
#line 6125 "parser.y"
    {
                 (yyval.str) = NewString((yyvsp[(1) - (1)].str));
	       }
    break;

  case 447:

/* Line 1455 of yacc.c  */
#line 6128 "parser.y"
    {
                 (yyval.str) = NewStringf("::%s",(yyvsp[(3) - (3)].str));
               }
    break;

  case 448:

/* Line 1455 of yacc.c  */
#line 6133 "parser.y"
    {
                   (yyval.str) = NewStringf("::%s%s",(yyvsp[(2) - (3)].id),(yyvsp[(3) - (3)].str));
		   Delete((yyvsp[(3) - (3)].str));
               }
    break;

  case 449:

/* Line 1455 of yacc.c  */
#line 6137 "parser.y"
    {
                   (yyval.str) = NewStringf("::%s",(yyvsp[(2) - (2)].id));
               }
    break;

  case 450:

/* Line 1455 of yacc.c  */
#line 6140 "parser.y"
    {
                   (yyval.str) = NewStringf("::%s",(yyvsp[(2) - (2)].str));
               }
    break;

  case 451:

/* Line 1455 of yacc.c  */
#line 6143 "parser.y"
    {
		 (yyval.str) = NewStringf("::~%s",(yyvsp[(2) - (2)].id));
               }
    break;

  case 452:

/* Line 1455 of yacc.c  */
#line 6149 "parser.y"
    { 
                   (yyval.id) = (char *) malloc(strlen((yyvsp[(1) - (2)].id))+strlen((yyvsp[(2) - (2)].id))+1);
                   strcpy((yyval.id),(yyvsp[(1) - (2)].id));
                   strcat((yyval.id),(yyvsp[(2) - (2)].id));
               }
    break;

  case 453:

/* Line 1455 of yacc.c  */
#line 6154 "parser.y"
    { (yyval.id) = (yyvsp[(1) - (1)].id);}
    break;

  case 454:

/* Line 1455 of yacc.c  */
#line 6157 "parser.y"
    {
		 (yyval.str) = NewString((yyvsp[(1) - (1)].id));
               }
    break;

  case 455:

/* Line 1455 of yacc.c  */
#line 6160 "parser.y"
    {
                  skip_balanced('{','}');
		  (yyval.str) = NewString(scanner_ccode);
               }
    break;

  case 456:

/* Line 1455 of yacc.c  */
#line 6164 "parser.y"
    {
		 (yyval.str) = (yyvsp[(1) - (1)].str);
              }
    break;

  case 457:

/* Line 1455 of yacc.c  */
#line 6169 "parser.y"
    {
                  Hash *n;
                  (yyval.node) = NewHash();
                  n = (yyvsp[(2) - (3)].node);
                  while(n) {
                     String *name, *value;
                     name = Getattr(n,"name");
                     value = Getattr(n,"value");
		     if (!value) value = (String *) "1";
                     Setattr((yyval.node),name, value);
		     n = nextSibling(n);
		  }
               }
    break;

  case 458:

/* Line 1455 of yacc.c  */
#line 6182 "parser.y"
    { (yyval.node) = 0; }
    break;

  case 459:

/* Line 1455 of yacc.c  */
#line 6186 "parser.y"
    {
		 (yyval.node) = NewHash();
		 Setattr((yyval.node),"name",(yyvsp[(1) - (3)].id));
		 Setattr((yyval.node),"value",(yyvsp[(3) - (3)].id));
               }
    break;

  case 460:

/* Line 1455 of yacc.c  */
#line 6191 "parser.y"
    {
		 (yyval.node) = NewHash();
		 Setattr((yyval.node),"name",(yyvsp[(1) - (5)].id));
		 Setattr((yyval.node),"value",(yyvsp[(3) - (5)].id));
		 set_nextSibling((yyval.node),(yyvsp[(5) - (5)].node));
               }
    break;

  case 461:

/* Line 1455 of yacc.c  */
#line 6197 "parser.y"
    {
                 (yyval.node) = NewHash();
                 Setattr((yyval.node),"name",(yyvsp[(1) - (1)].id));
	       }
    break;

  case 462:

/* Line 1455 of yacc.c  */
#line 6201 "parser.y"
    {
                 (yyval.node) = NewHash();
                 Setattr((yyval.node),"name",(yyvsp[(1) - (3)].id));
                 set_nextSibling((yyval.node),(yyvsp[(3) - (3)].node));
               }
    break;

  case 463:

/* Line 1455 of yacc.c  */
#line 6206 "parser.y"
    {
                 (yyval.node) = (yyvsp[(3) - (3)].node);
		 Setattr((yyval.node),"name",(yyvsp[(1) - (3)].id));
               }
    break;

  case 464:

/* Line 1455 of yacc.c  */
#line 6210 "parser.y"
    {
                 (yyval.node) = (yyvsp[(3) - (5)].node);
		 Setattr((yyval.node),"name",(yyvsp[(1) - (5)].id));
		 set_nextSibling((yyval.node),(yyvsp[(5) - (5)].node));
               }
    break;

  case 465:

/* Line 1455 of yacc.c  */
#line 6217 "parser.y"
    {
		 (yyval.id) = (yyvsp[(1) - (1)].id);
               }
    break;

  case 466:

/* Line 1455 of yacc.c  */
#line 6220 "parser.y"
    {
                 (yyval.id) = Char((yyvsp[(1) - (1)].dtype).val);
               }
    break;



/* Line 1455 of yacc.c  */
#line 11321 "y.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 6227 "parser.y"


SwigType *Swig_cparse_type(String *s) {
   String *ns;
   ns = NewStringf("%s;",s);
   Seek(ns,0,SEEK_SET);
   scanner_file(ns);
   top = 0;
   scanner_next_token(PARSETYPE);
   yyparse();
   /*   Printf(stdout,"typeparse: '%s' ---> '%s'\n", s, top); */
   return top;
}


Parm *Swig_cparse_parm(String *s) {
   String *ns;
   ns = NewStringf("%s;",s);
   Seek(ns,0,SEEK_SET);
   scanner_file(ns);
   top = 0;
   scanner_next_token(PARSEPARM);
   yyparse();
   /*   Printf(stdout,"typeparse: '%s' ---> '%s'\n", s, top); */
   Delete(ns);
   return top;
}


ParmList *Swig_cparse_parms(String *s, Node *file_line_node) {
   String *ns;
   char *cs = Char(s);
   if (cs && cs[0] != '(') {
     ns = NewStringf("(%s);",s);
   } else {
     ns = NewStringf("%s;",s);
   }
   Setfile(ns, Getfile(file_line_node));
   Setline(ns, Getline(file_line_node));
   Seek(ns,0,SEEK_SET);
   scanner_file(ns);
   top = 0;
   scanner_next_token(PARSEPARMS);
   yyparse();
   /*   Printf(stdout,"typeparse: '%s' ---> '%s'\n", s, top); */
   return top;
}


