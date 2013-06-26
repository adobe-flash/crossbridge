/*@z33.c:Database Service:OldCrossDb(), NewCrossDb(), SymToNum()@*************/
/*                                                                           */
/*  THE LOUT DOCUMENT FORMATTING SYSTEM (VERSION 3.23)                       */
/*  COPYRIGHT (C) 1991, 2000 Jeffrey H. Kingston                             */
/*                                                                           */
/*  Jeffrey H. Kingston (jeff@cs.usyd.edu.au)                                */
/*  Basser Department of Computer Science                                    */
/*  The University of Sydney 2006                                            */
/*  AUSTRALIA                                                                */
/*                                                                           */
/*  This program is free software; you can redistribute it and/or modify     */
/*  it under the terms of the GNU General Public License as published by     */
/*  the Free Software Foundation; either Version 2, or (at your option)      */
/*  any later version.                                                       */
/*                                                                           */
/*  This program is distributed in the hope that it will be useful,          */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of           */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            */
/*  GNU General Public License for more details.                             */
/*                                                                           */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program; if not, write to the Free Software              */
/*  Foundation, Inc., 59 Temple Place, Suite 330, Boston MA 02111-1307 USA   */
/*                                                                           */
/*  FILE:         z33.c                                                      */
/*  MODULE:       Database Service                                           */
/*  EXTERNS:      OldCrossDb, NewCrossDb, DbCreate(), DbInsert(),            */
/*                DbConvert(), DbClose(), DbLoad(), DbRetrieve(),            */
/*                DbRetrieveNext()                                           */
/*                                                                           */
/*****************************************************************************/
#define INIT_DBCHECK_NUM	107
#include "externs.h"


/*****************************************************************************/
/*                                                                           */
/*  DBCHECK_TABLE                                                            */
/*                                                                           */
/*  A symbol table holding all non-galley cross references, basically        */
/*  implementing a function (sym, tag) -> fpos (if any).                     */
/*                                                                           */
/*     dtab_new(newsize)                New empty table, newsize capacity    */
/*     dtab_insert(x, S)                Insert new (sym, tag) pair x into S  */
/*     dtab_retrieve(sym, tag, S)       Retrieve (sym, tag) pair from S      */
/*     dtab_debug(S, fp)                Debug print of table S to file fp    */
/*                                                                           */
/*****************************************************************************/

typedef struct
{ int dbchecktab_size;				/* size of table             */
  int dbchecktab_count;				/* number of objects held    */
  OBJECT dbchecktab_item[1];
} *DBCHECK_TABLE;

#define	dtab_size(S)	(S)->dbchecktab_size
#define	dtab_count(S)	(S)->dbchecktab_count
#define	dtab_item(S, i)	(S)->dbchecktab_item[i]

#define hash(pos, sym, tag, S)						\
{ FULL_CHAR *p = tag;							\
  pos = (unsigned long) sym;						\
  while( *p ) pos += *p++;						\
  pos = pos % dtab_size(S);						\
}

static DBCHECK_TABLE dtab_new(int newsize)
{ DBCHECK_TABLE S;  int i;
  ifdebug(DMA, D, DebugRegisterUsage(MEM_DBCHECK, 1,
    2*sizeof(int) + newsize * sizeof(OBJECT)));
  S = (DBCHECK_TABLE)
	  malloc(2*sizeof(int) + newsize * sizeof(OBJECT));
  if( S == (DBCHECK_TABLE) NULL )
    Error(33, 1, "run out of memory enlarging dbcheck table", FATAL, no_fpos);
  dtab_size(S) = newsize;
  dtab_count(S) = 0;
  for( i = 0;  i < newsize;  i++ )  dtab_item(S, i) = nilobj;
  return S;
} /* end dtab_new */

static void dtab_insert(OBJECT x, DBCHECK_TABLE *S);

static DBCHECK_TABLE dtab_rehash(DBCHECK_TABLE S, int newsize)
{ DBCHECK_TABLE NewS;  int i;  OBJECT link, z;
  NewS = dtab_new(newsize);
  for( i = 0;  i < dtab_size(S);  i++ )
  { if( dtab_item(S, i) != nilobj )
    { OBJECT ent = dtab_item(S, i);
      assert( type(ent) == ACAT, "dtab_rehash: ACAT!" );
      for( link = Down(ent);  link != ent;  link = NextDown(link) )
      { Child(z, link);
	dtab_insert(z, &NewS);
      }
      DisposeObject(ent);
    }
  }
  ifdebug(DMA, D, DebugRegisterUsage(MEM_DBCHECK, -1,
    -(2*sizeof(int) + dtab_size(S) * sizeof(OBJECT))));
  free(S);
  return NewS;
} /* end dtab_rehash */

static void dtab_insert(OBJECT x, DBCHECK_TABLE *S)
{ unsigned long pos;  OBJECT z, link, y;
  if( dtab_count(*S) == dtab_size(*S) - 1 )	/* one less since 0 unused */
    *S = dtab_rehash(*S, 2*dtab_size(*S));
  dtab_count(*S)++;
  hash(pos, db_checksym(x), string(x), *S);
  if( dtab_item(*S, pos) == nilobj )  New(dtab_item(*S, pos), ACAT);
  z = dtab_item(*S, pos);
  for( link = Down(z);  link != z;  link = NextDown(link) )
  { Child(y, link);
    if( db_checksym(x) == db_checksym(y) && StringEqual(string(x), string(y)) )
    { assert(FALSE, "Dbcheck: entry inserted twice");
    }
  }
  Link(dtab_item(*S, pos), x);
} /* end dtab_insert */

static OBJECT dtab_retrieve(OBJECT sym, FULL_CHAR *tag, DBCHECK_TABLE S)
{ OBJECT x, link, y;  unsigned long pos;
  hash(pos, sym, tag, S);
  x = dtab_item(S, pos);
  if( x == nilobj )  return nilobj;
  for( link = Down(x);  link != x;  link = NextDown(link) )
  { Child(y, link);
    if( sym == db_checksym(y) && StringEqual(tag, string(y)) )
      return y;
  }
  return nilobj;
} /* end dtab_retrieve */

#if DEBUG_ON
static void dtab_debug(DBCHECK_TABLE S, FILE *fp)
{ int i;  OBJECT x, link, y;
  fprintf(fp, "  table size: %d;  current number of items: %d\n",
    dtab_size(S), dtab_count(S));
  for( i = 0;  i < dtab_size(S);  i++ )
  { x = dtab_item(S, i);
    fprintf(fp, "dtab_item(S, %d) =", i);
    if( x == nilobj )
      fprintf(fp, " <nilobj>");
    else if( type(x) != ACAT )
      fprintf(fp, " not ACAT!");
    else for( link = Down(x);  link != x;  link = NextDown(link) )
    { Child(y, link);
      fprintf(fp, " %s&&%s",
	is_word(type(y)) ? SymName(db_checksym(y)) : AsciiToFull("?"),
	is_word(type(y)) ? string(y) : AsciiToFull("not-WORD!"));
    }
    fprintf(fp, "\n");
  }
} /* end dtab_debug */
#endif

static DBCHECK_TABLE DbCheckTable;		/* the dbcheck table         */
static BOOLEAN	     DbCheckTableInit = FALSE;	/* TRUE if table inited	     */


/*****************************************************************************/
/*                                                                           */
/*  OldCrossDb     Database containing cross references from previous run.   */
/*  NewCrossDb     Writable database of cross references from this run.      */
/*                                                                           */
/*****************************************************************************/

OBJECT OldCrossDb, NewCrossDb;


/*****************************************************************************/
/*                                                                           */
/*  #define SymToNum(db, sym, num, gall)                                     */
/*                                                                           */
/*  Set num to the number used to refer to sym in database db.  If sym is    */
/*  not currently referred to in db, create a new number and record sym.     */
/*  If gall is true, sym is the target of galleys stored in this database.   */
/*  Store in boolean fields db_targ(link) and is_extern_target(sym).         */
/*                                                                           */
/*****************************************************************************/

#define SymToNum(db, sym, num, gall)					\
{ OBJECT link, yy;  int count;						\
  count = 0;								\
  for( link = Down(db);  link != db;  link = NextDown(link) )		\
  { Child(yy, link);							\
    assert(type(yy)==CROSS_SYM || type(yy)==ACAT, "SymToNum: yy!");	\
    if( type(yy) != CROSS_SYM )  continue;				\
    if( symb(yy) == sym )  break;					\
    if( number(link) > count )  count = number(link);			\
  }									\
  if( link == db )							\
  { if( cross_sym(sym) == nilobj )  CrossInit(sym);			\
    Link(db, cross_sym(sym));						\
    link = LastDown(db);						\
    number(link) = count + 1;						\
    db_targ(link) = FALSE;						\
  }									\
  num = number(link);							\
  if( gall )  db_targ(link) = is_extern_target(sym) =			\
				uses_extern_target(sym) = TRUE;		\
} /* end SymToNum */


/*@::NumToSym(), DbCreate()@**************************************************/
/*                                                                           */
/*  #define NumToSym(db, num, sym)                                           */
/*                                                                           */
/*  Set sym to the symbol which is referred to in database db by num.        */
/*                                                                           */
/*****************************************************************************/

#define NumToSym(db, num, sym)						\
{ OBJECT link, y;							\
  for( link = Down(db);  link != db;  link = NextDown(link) )		\
  { Child(y, link);							\
    if( type(y) == CROSS_SYM && number(link) == num )  break;		\
  }									\
  assert( link != db, "NumToSym: no sym");				\
  assert( type(y) == CROSS_SYM, "NumToSym: y!" );			\
  sym = symb(y);							\
} /* end NumToSym */


/*****************************************************************************/
/*                                                                           */
/*  OBJECT DbCreate(x)                                                       */
/*                                                                           */
/*  Create a new writable database with name (i.e. file stem) x and file     */
/*  position fpos for error messages.                                        */
/*                                                                           */
/*****************************************************************************/

OBJECT DbCreate(OBJECT x)
{ OBJECT db = x;
  debug1(DBS, DD, "DbCreate(%s)", string(db));
  assert( is_word(type(x)), "DbCreate: !is_word(type(x))" );
  reading(db) = FALSE;  db_filep(db) = null;
  debug1(DBS, DD, "DbCreate returning %s", EchoObject(db));
  return db;
} /* end DbCreate */


/*@::DbInsert()@**************************************************************/
/*                                                                           */
/*  DbInsert(db, gall, sym, tag, tagfpos, seq, dfnum, dlnum, dfpos)          */
/*                                                                           */
/*  Insert a new entry into writable database db.  The primary key of the    */
/*  entry has these three parts:                                             */
/*                                                                           */
/*      gall        TRUE if inserting a galley                               */
/*      sym         The symbol which is the target of this entry             */
/*      tag         The tag of this target (must be a non-null string)       */
/*                                                                           */
/*  tagfpos is the file position that the tag originated from.               */
/*  There is also an auxiliary key, seq, which enforces an ordering on       */
/*  entries with equal primary keys but is not itself ever retrieved.  This  */
/*  ordering is used for sorted galleys.  The value of the entry has the     */
/*  following parts:                                                         */
/*                                                                           */
/*      dfnum       The file containing the object                           */
/*      dfpos       The position of the object in that file                  */
/*      dlnum       The line number of the object in the file                */
/*                                                                           */
/*  If check is TRUE, we need to check whether an entry with this key has    */
/*  been inserted before.  This will never be the case with galley entries.  */
/*                                                                           */
/*****************************************************************************/

void DbInsert(OBJECT db, BOOLEAN gall, OBJECT sym, FULL_CHAR *tag,
FILE_POS *tagfpos, FULL_CHAR *seq, FILE_NUM dfnum, long dfpos, int dlnum,
BOOLEAN check)
{ int symnum;  OBJECT chk;
  static int extra_seq = 0;
  FULL_CHAR buff[MAX_BUFF];
  assert( is_word(type(db)), "DbInsert: db!" );
  assert( tag[0] != '\0', "DbInsert: null tag!" );
  assert( seq[0] != '\0', "DbInsert: null seq!" );
  ifdebug(DPP, D, ProfileOn("DbInsert"));
  debug6(DBS, D, "DbInsert(%s, %s, %s, %s, %s, %s, dlnum, dfpos)",
	string(db), bool(gall), SymName(sym), tag, seq,
	dfnum == NO_FILE ? AsciiToFull(".") : FileName(dfnum));
  assert(!reading(db), "DbInsert: insert into reading database");

  /* if required, check that (sym, tag) not already inserted */
  if( check )
  { 
    debug2(DBS, DD, "  checking %s&&%s, DbCheckTable =", SymName(sym), tag);
    if( !DbCheckTableInit )
    { DbCheckTable = dtab_new(INIT_DBCHECK_NUM);
      DbCheckTableInit = TRUE;
    }
    ifdebug(DBS, DD, dtab_debug(DbCheckTable, stderr));
    chk = dtab_retrieve(sym, tag, DbCheckTable);
    if( chk == nilobj )
    { chk = MakeWord(WORD, tag, tagfpos);
      db_checksym(chk) = sym;
      dtab_insert(chk, &DbCheckTable);
    }
    else
    { if( file_num(fpos(chk)) > 0 )
        Error(33, 4, "cross reference %s&&%s used previously, at%s",
          WARN, tagfpos, SymName(sym), tag, EchoFilePos(&fpos(chk)));
      else Error(33, 5, "cross reference %s&&%s used previously",
	  WARN, tagfpos, SymName(sym), tag);
    }
  }

  /* open database index file if not already done */
  if( db_filep(db) == null )
  { if( StringLength(string(db)) + StringLength(NEW_INDEX_SUFFIX) >= MAX_BUFF )
      Error(33, 2, "database file name %s%s is too long",
	FATAL, no_fpos, string(db), NEW_INDEX_SUFFIX);
    StringCopy(buff, string(db));
    StringCat(buff, NEW_INDEX_SUFFIX);
    db_filep(db) = StringFOpen(buff, WRITE_BINARY);
    if( db_filep(db) == null )
      Error(33, 3, "cannot write to database file %s", FATAL, &fpos(db), buff);
  }

  /* work out database index file entry and append it to file */
  if( dfnum != NO_FILE )
  { StringCopy(buff, FileName(dfnum));
    StringCopy(&buff[StringLength(buff)-StringLength(DATA_SUFFIX)], STR_EMPTY);
  }
  else StringCopy(buff, AsciiToFull("."));
  SymToNum(db, sym, symnum, gall);
  ifdebug(DBS, DD,
  fprintf(stderr, "  -> %s%d&%s\t%s\t%ld\t%d\t%s\n", gall ? "0" : "", symnum,
    tag, seq, dfpos, dlnum, buff);
  );
  fprintf(db_filep(db), "%s%d&%s\t%s\t%s\t%ld\t%d\t%s\n", gall ? "0" : "",
    symnum, tag, seq, StringFiveInt(++extra_seq), dfpos, dlnum, buff);

  /* return */
  debug0(DBS, DD, "DbInsert returning.");
  ifdebug(DPP, D, ProfileOff("DbInsert"));
} /* end DbInsert */


/*@::DbConvert(), DbClose()@**************************************************/
/*                                                                           */
/*  DbConvert(db, full_name)                                                 */
/*                                                                           */
/*  Convert database db from writable to readable, then dispose it.          */
/*  full_name is TRUE if symbols are to be known by their full path name.    */
/*                                                                           */
/*****************************************************************************/

void DbConvert(OBJECT db, BOOLEAN full_name)
{ FULL_CHAR oldname[MAX_BUFF+10], newname[MAX_BUFF];
  OBJECT link, y;
  ifdebug(DPP, D, ProfileOn("DbConvert"));
  debug2(DBS, DD, "DbConvert( %ld %s )", (long) db, string(db));
  assert( !reading(db), "DbConvert: reading database");
  StringCopy(newname, string(db));
  StringCat(newname, INDEX_SUFFIX);
  StringCopy(oldname, string(db));
  StringCat(oldname, NEW_INDEX_SUFFIX);
  if( db_filep(db) != null )
  {
    fprintf(db_filep(db), "00 %s %s\n", LOUT_VERSION, "database index file");
    for( link = Down(db);  link != db;  link = NextDown(link) )
    { Child(y, link);
      assert( type(y) == CROSS_SYM || type(y) == ACAT, "DbConvert: y!" );
      if( type(y) != CROSS_SYM )  continue;
      fprintf(db_filep(db), "%s %d %s\n",
	db_targ(link) ? "00target" : "00symbol",
	number(link),
	full_name ? FullSymName(symb(y), AsciiToFull(" ")) : SymName(symb(y)));
    }
    fclose(db_filep(db));
    debug2(DBS, DD, "  calling SortFile(%s, %s)", oldname, newname);
    SortFile(oldname, newname);
  }
  else StringRemove(newname);
  StringRemove(oldname);
  DeleteNode(db);
  debug0(DBS, DD, "DbConvert returning.");
  ifdebug(DPP, D, ProfileOff("DbConvert"));
} /* end DbConvert */


/*****************************************************************************/
/*                                                                           */
/*  DbClose(db)                                                              */
/*                                                                           */
/*  Close readable database db.                                              */
/*                                                                           */
/*****************************************************************************/

void DbClose(OBJECT db)
{ if( db != nilobj && !in_memory(db) && db_filep(db) != NULL )
  {  fclose(db_filep(db));
     db_filep(db) = NULL;
  }
} /* end DbClose */


/*@::DbLoad()@****************************************************************/
/*                                                                           */
/*  OBJECT DbLoad(stem, fpath, create, symbs, in_mem)                        */
/*                                                                           */
/*  Open for reading the database whose index file name is string(stem).li.  */
/*  This file has not yet been defined; its search path is fpath.  If it     */
/*  will not open and create is true, try creating it from string(stem).ld.  */
/*                                                                           */
/*  symbs is an ACAT of CLOSUREs showing the symbols that the database may   */
/*  contain; or nilobj if the database may contain any symbol.               */
/*                                                                           */
/*  If in_mem is true, this database index is to be kept in internal memory, */
/*  rather than an external file, as a speed optimization.                   */
/*                                                                           */
/*****************************************************************************/

OBJECT DbLoad(OBJECT stem, int fpath, BOOLEAN create, OBJECT symbs,
  BOOLEAN in_mem)
{ FILE *fp;  OBJECT db, t, res, tag, par, sym, link, y;
  int i, lnum, dlnum, num, count, leftp;
  FILE_NUM index_fnum, dfnum;  long dfpos;
  BOOLEAN gall;  FULL_CHAR line[MAX_BUFF], sym_name[MAX_BUFF]; char *gotline;
  ifdebug(DPP, D, ProfileOn("DbLoad"));
  debug3(DBS, DD, "[ DbLoad(%s, %d, %s, -)", string(stem), fpath, bool(create));

  /* open or else create index file fp */
  debug0(DFS, D, "  calling DefineFile from DbLoad (1)");
  index_fnum = DefineFile(string(stem), INDEX_SUFFIX, &fpos(stem), INDEX_FILE,
		 fpath);
  fp = OpenFile(index_fnum, create, FALSE);

  /* read first line of database index file, which should have the version */
  if( fp != null )
  { if( StringFGets(line, MAX_BUFF, fp) == NULL ||
        !StringBeginsWith(&line[3], LOUT_VERSION) )
    {
      /* out of date, pretend it isn't there at all */
      StringRemove(FileName(index_fnum));
      fp = null;
    }
  }

  if( fp == null && create )
  { db = nilobj;
    debug0(DFS, D, "  calling DefineFile from DbLoad (2)");
    /* *** bug fix JeffK 12/9/00; need same path as index file
    dfnum = DefineFile(string(stem), DATA_SUFFIX, &fpos(stem),
      DATABASE_FILE, DATABASE_PATH);
    *** */
    dfnum = DefineFile(string(stem), DATA_SUFFIX, &fpos(stem),
      DATABASE_FILE, fpath);
    dfpos = 0L;  LexPush(dfnum, 0, DATABASE_FILE, 1, FALSE);
    t = LexGetToken();
    dlnum = line_num(fpos(t));
    while( type(t) == LBR )
    { res = Parse(&t, StartSym, FALSE, FALSE);
      if( t != nilobj || type(res) != CLOSURE )
	Error(33, 6, "syntax error in database file %s",
	  FATAL, &fpos(res), FileName(dfnum));
      assert( symbs != nilobj, "DbLoad: create && symbs == nilobj!" );
      if( symbs != nilobj )
      {	for( link = Down(symbs);  link != symbs;  link = NextDown(link) )
	{ Child(y, link);
	  if( type(y) == CLOSURE && actual(y) == actual(res) )  break;
	}
	if( link == symbs )
	  Error(33, 7, "%s found in database but not declared in %s line",
	    FATAL, &fpos(res), SymName(actual(res)), KW_DATABASE);
      }
      for( tag = nilobj, link = Down(res); link != res; link = NextDown(link) )
      {	Child(par, link);
	if( type(par) == PAR && is_tag(actual(par)) && Down(par) != par )
	{ Child(tag, Down(par));
	  break;
	}
      }
      if( tag == nilobj )
	Error(33, 8, "database symbol %s has no tag",
	  FATAL, &fpos(res), SymName(actual(res)));
      tag = ReplaceWithTidy(tag, TRUE);  /* && */
      if( !is_word(type(tag)) )
	Error(33, 9, "database symbol tag is not a simple word",
	  FATAL, &fpos(res));
      if( StringEqual(string(tag), STR_EMPTY) )
	Error(33, 10, "database symbol tag is an empty word", FATAL,&fpos(res));
      if( db == nilobj )
      {	StringCopy(line, FileName(dfnum));
	i = StringLength(line) - StringLength(INDEX_SUFFIX);
	assert( i > 0, "DbLoad: FileName(dfnum) (1)!" );
	StringCopy(&line[i], STR_EMPTY);
	db = DbCreate(MakeWord(WORD, line, &fpos(stem)));
      }
      DbInsert(db, FALSE, actual(res), string(tag), &fpos(tag), STR_ZERO,
	NO_FILE, dfpos, dlnum, TRUE);
      DisposeObject(res);  dfpos = LexNextTokenPos();  t = LexGetToken();
      dlnum = line_num(fpos(t));
    }
    if( type(t) != END )
      Error(33, 11, "%s or end of file expected here", FATAL, &fpos(t), KW_LBR);
    LexPop();
    if( db == nilobj )
    { StringCopy(line, FileName(dfnum));
      i = StringLength(line) - StringLength(INDEX_SUFFIX);
      assert( i > 0, "DbLoad: FileName(dfnum) (2)!" );
      StringCopy(&line[i], STR_EMPTY);
      db = DbCreate(MakeWord(WORD, line, &fpos(stem)));
    }
    DbConvert(db, FALSE);
    if( (fp = OpenFile(index_fnum, FALSE, FALSE)) == null ||
        StringFGets(line, MAX_BUFF, fp) == NULL ||
        !StringBeginsWith(&line[3], LOUT_VERSION) )
      Error(33, 12, "cannot open database file %s",
        FATAL, &fpos(db), FileName(index_fnum));
  }

  /* set up database record */
  StringCopy(line, FileName(index_fnum));
  i = StringLength(line) - StringLength(INDEX_SUFFIX);
  assert( i > 0, "DbLoad: FileName(index_fnum)!" );
  StringCopy(&line[i], STR_EMPTY);
  db = MakeWord(WORD, line, &fpos(stem));
  reading(db) = TRUE;
  in_memory(db) = in_mem;
  if( symbs != nilobj )
  { assert( type(symbs) == ACAT, "DbLoad: type(symbs)!" );
    Link(db, symbs);
  }
  if( fp == null )
  { debug1(DBS, DD, "] DbLoad returning (empty) %s", string(db));
    db_filep(db) = null;
    db_lines(db) = (LINE *) NULL;
    ifdebug(DPP, D, ProfileOff("DbLoad"));
    return db;
  }

  /* read header lines of index file, find its symbols */
  leftp = 0;  lnum = 1;
  gotline = StringFGets(line, MAX_BUFF, fp);
  while( gotline != NULL )
  {
    if( line[0] != '0' || line[1] != '0' )  break;
    lnum++;
    leftp = (int) ftell(fp);
    gall = StringBeginsWith(line, AsciiToFull("00target "));
    sscanf( (char *) line, gall ? "00target %d" : "00symbol %d", &num);
    for( i = 9;  line[i] != CH_SPACE && line[i] != '\0';  i++ );
    if( symbs == nilobj )
    {
      /* any symbols are possible, full path names in index file required */
      count = 0;  sym = StartSym;
      while( line[i] != CH_NEWLINE && line[i] != '\0' )
      {	PushScope(sym, FALSE, FALSE);  count++;
	sscanf( (char *) &line[i+1], "%s", sym_name);
	sym = SearchSym(sym_name, StringLength(sym_name));
	i += StringLength(sym_name) + 1;
      }
      for( i = 1;  i <= count;  i++ )  PopScope();
    }
    else
    {
      /* only symbs symbols possible, full path names not required */
      sym = nilobj;
      sscanf( (char *) &line[i+1], "%s", sym_name);
      for( link = Down(symbs);  link != symbs;  link = NextDown(link) )
      {	Child(y, link);
	assert( type(y) == CLOSURE, "DbLoad: type(y) != CLOSURE!" );
	if( StringEqual(sym_name, SymName(actual(y))) )
	{ sym = actual(y);
	  break;
	}
      }
    }
    if( sym != nilobj && sym != StartSym )
    { if( cross_sym(sym) == nilobj )  CrossInit(sym);
      Link(db, cross_sym(sym));
      link = LastDown(db);
      number(link) = num;  db_targ(link) = gall;
      if( gall )  is_extern_target(sym) = uses_extern_target(sym) = TRUE;
    }
    else
    { Error(33, 13, "undefined symbol in database file %s (line %d)",
	WARN, &fpos(db), FileName(index_fnum), lnum);
      debug1(DBS, DD, "] DbLoad returning %s (error)", string(db));
      fclose(fp);
      in_memory(db) = FALSE;
      db_filep(db) = null;  /* subsequently treated like an empty database */
      ifdebug(DPP, D, ProfileOff("DbLoad"));
      return db;
    }
    gotline = StringFGets(line, MAX_BUFF, fp);
  }

  /* if in_memory, go on to read the entire database index into memory */
  if( in_memory(db) )
  { int len;
    if( gotline == NULL )
      db_lines(db) = 0;
    else
    {
      db_lines(db) = ReadLines(fp, FileName(index_fnum), line, &len);
      db_lineslen(db) = len;
      SortLines(db_lines(db), db_lineslen(db));
    }
  }
  else /* external, save leftpos and file pointer */
  { db_filep(db) = fp;
    left_pos(db) = leftp;
  }

  /* return */
  debug1(DBS, DD, "] DbLoad returning %s", string(db));
  ifdebug(DPP, D, ProfileOff("DbLoad"));
  return db;
} /* end DbLoad */


/*@::SearchFile()@************************************************************/
/*                                                                           */
/*  static BOOLEAN SearchFile(fp, left, right, str, line)                    */
/*                                                                           */
/*  File fp is a text file.  left is the beginning of a line, right is the   */
/*  end of a line.   Search the file by binary search for a line beginning   */
/*  with str.  If found, return it in line, else return FALSE.               */
/*                                                                           */
/*****************************************************************************/

static BOOLEAN SearchFile(FILE *fp, int left, int right,
FULL_CHAR *str, FULL_CHAR *line)
{ int l, r, mid, mid_end;  FULL_CHAR buff[MAX_BUFF];  BOOLEAN res;
  ifdebug(DPP, D, ProfileOn("SearchFile"));
  debug3(DBS, DD, "SearchFile(fp, %d, %d, %s, line)", left, right, str);

  l = left;  r = right;
  while( l <= r )
  {
    /* loop invt: (l==0 or fp[l-1]==CH_NEWLINE) and (fp[r] == CH_NEWLINE)    */
    /* and first key >= str lies in the range fp[l..r+1]                     */

    /* find line near middle of the range; mid..mid_end brackets it */
    debug2(DBS, DD, "  start loop: l = %d, r = %d", l, r);
    mid = (l + r)/2;
    fseek(fp, (long) mid, SEEK_SET);
    do { mid++; } while( getc(fp) != CH_NEWLINE );
    if( mid == r + 1 )
    { mid = l;
      fseek(fp, (long) mid, SEEK_SET);
    }
    StringFGets(line, MAX_BUFF, fp);
    mid_end = (int) ftell(fp) - 1;
    debug3(DBS, DD, "  mid: %d, mid_end: %d, line: %s", mid, mid_end, line);
    assert( l <= mid,      "SearchFile: l > mid!"        );
    assert( mid < mid_end, "SearchFile: mid >= mid_end!" );
    assert( mid_end <= r,  "SearchFile: mid_end > r!"    );

    /* compare str with this line and prepare next step */
    debug2(DBS, DD, "  comparing key %s with line %s", str, line);
    if( TabbedStringLessEqual(str, line) )  r = mid - 1;
    else l = mid_end + 1;
  } /* end while */

  /* now first key >= str lies in fp[l]; compare it with str */
  if( l < right )
  { fseek(fp, (long) l, SEEK_SET);
    StringFGets(line, MAX_BUFF, fp);
    sscanf( (char *) line, "%[^\t]", buff);
    res = StringEqual(str, buff);
  }
  else res = FALSE;
  debug1(DBS, DD, "SearchFile returning %s", bool(res));
  ifdebug(DPP, D, ProfileOff("SearchFile"));
  return res;
} /* end SearchFile */


/*@::SearchLines()@***********************************************************/
/*                                                                           */
/*  static BOOLEAN SearchLines(LINE *lines, int left, int right, str, lnum)  */
/*                                                                           */
/*  Search the sorted array of LINE arrays lines[left..right] for a line     */
/*  beginning with str, and return TRUE if found else FALSE.                 */
/*                                                                           */
/*  If TRUE is returned then the number of the line is in *lnum.             */
/*                                                                           */
/*****************************************************************************/

static BOOLEAN SearchLines(LINE *lines, int left, int right, FULL_CHAR *str,
  int *lnum)
{ int l, r, mid;  FULL_CHAR buff[MAX_BUFF];
  BOOLEAN res;
  debug3(DBS, D, "SearchLines(lines, %d, %d, %s, lnum)", left, right, str);
  if( right < left )
  {
    debug0(DBS, D, "SearchLines returning FALSE (empty lines)");
    return FALSE;
  }
  l = left;
  r = right - 1;
  while( l <= r )
  {
    /* loop invt: first key >= str (if any) lies in the range lines[l..r+1] */
    /* and left <= l <= right and r < right                                 */
    mid = (l + r) / 2;
    debug4(DBS, D, "  [l %d, r %d] examining lines[%d] = %s", l, r, mid,
      lines[mid]);
    if( TabbedStringLessEqual(str, (FULL_CHAR *) lines[mid]) )  r = mid - 1;
    else l = mid + 1;
  }
  sscanf( (char *) lines[l], "%[^\t]", buff);
  if( StringEqual(str, buff) )
  {
    res = TRUE;
    *lnum = l;
    debug1(DBS, D, "SearchLines returning TRUE (lnum %d)", *lnum);
  }
  else
  { res = FALSE;
    debug0(DBS, D, "SearchLines returning FALSE");
  }
  return res;
} /* end SearchLines */


/*@::DbRetrieve()@************************************************************/
/*                                                                           */
/*  BOOLEAN DbRetrieve(db, gall, sym, tag, seq, dfnum, dfpos, dlnum, cont)   */
/*                                                                           */
/*  Retrieve the first entry of database db with the given gall, sym and     */
/*  tag.  Set *seq, *dfnum, *dlnum, *dfpos to the associated value.          */
/*  Set *cont to a private value for passing to DbRetrieveNext.              */
/*                                                                           */
/*****************************************************************************/

BOOLEAN DbRetrieve(OBJECT db, BOOLEAN gall, OBJECT sym, FULL_CHAR *tag,
  FULL_CHAR *seq, FILE_NUM *dfnum, long *dfpos, int *dlnum, long *cont)
{ int symnum, lnum;  FULL_CHAR line[MAX_BUFF], buff[MAX_BUFF];
  ifdebug(DPP, D, ProfileOn("DbRetrieve"));
  debug4(DBS, DD, "DbRetrieve(%s, %s%s&%s)", string(db), gall ? "0" : "",
	SymName(sym), tag);

  /* check OK to proceed */
  if( !reading(db) || db_filep(db) == null )
  { debug0(DBS, DD, "DbRetrieve returning FALSE (empty or not reading)");
    ifdebug(DPP, D, ProfileOff("DbRetrieve"));
    return FALSE;
  }

  /* convert parameters into search key */
  SymToNum(db, sym, symnum, FALSE);
  sprintf( (char *) buff, "%s%d&%s", gall ? "0" : "", symnum, tag);

  if( in_memory(db) )
  {
    /* search internal table, return if not found; set *cont to continuation */
    if( !SearchLines(db_lines(db), 0, db_lineslen(db) - 1, buff, &lnum) )
    { debug0(DBS, DD, "DbRetrieve returning FALSE (key not present)");
      ifdebug(DPP, D, ProfileOff("DbRetrieve"));
      return FALSE;
    }
    sscanf( (char *) db_lines(db)[lnum],
      "%*[^\t]\t%[^\t]\t%*[^\t]\t%ld\t%d\t%[^\n]", seq, dfpos, dlnum, buff);
    *cont = lnum+1;
  }
  else
  {
    /* search for key in file, return if not found; set *cont to continuatn */
    fseek(db_filep(db), 0L, SEEK_END);
    if( !SearchFile(db_filep(db), (int) left_pos(db),
	   (int) ftell(db_filep(db)) - 1, buff, line) )
    { debug0(DBS, DD, "DbRetrieve returning FALSE (key not present)");
      ifdebug(DPP, D, ProfileOff("DbRetrieve"));
      return FALSE;
    }
    sscanf( (char *) line,
      "%*[^\t]\t%[^\t]\t%*[^\t]\t%ld\t%d\t%[^\n]", seq, dfpos, dlnum, buff);
    *cont = ftell(db_filep(db));
  }

  /* work out file name if . abbreviation used, and possibly define file */
  if( StringEqual(buff, AsciiToFull(".")) )
  { StringCopy(buff, string(db));
  }
  *dfnum = FileNum(buff, DATA_SUFFIX);
  if( *dfnum == NO_FILE )  /* can only occur in cross reference database */
  { debug0(DFS, D, "  calling DefineFile from DbRetrieve");
    *dfnum = DefineFile(buff, DATA_SUFFIX, &fpos(db),
      DATABASE_FILE, SOURCE_PATH);
  }

  /* return */
  debug3(DBS, DD, "DbRetrieve returning TRUE (in %s at %ld, line %d)",
    FileName(*dfnum), *dfpos, *dlnum);
  ifdebug(DPP, D, ProfileOff("DbRetrieve"));
  return TRUE;
} /* end DbRetrieve */


/*@::DbRetrieveNext()@********************************************************/
/*                                                                           */
/*  BOOLEAN DbRetrieveNext(db, gall, sym, tag, seq, dfnum, dfpos,dlnum,cont) */
/*                                                                           */
/*  Retrieve the entry of database db pointed to by *cont.                   */
/*  Set *gall, *sym, *tag, *seq, *dfnum, *dlnum, *dfpos to the value.        */
/*  Reset *cont to the next entry for passing to the next DbRetrieveNext.    */
/*                                                                           */
/*****************************************************************************/

BOOLEAN DbRetrieveNext(OBJECT db, BOOLEAN *gall, OBJECT *sym, FULL_CHAR *tag,
  FULL_CHAR *seq, FILE_NUM *dfnum, long *dfpos, int *dlnum, long *cont)
{ FULL_CHAR line[MAX_BUFF], *cline, fname[MAX_BUFF]; int symnum;
  ifdebug(DPP, D, ProfileOn("DbRetrieveNext"));
  debug2(DBS, DD, "DbRetrieveNext( %s, %ld )", string(db), *cont);
  assert(reading(db), "DbRetrieveNext: not reading");

  /* check OK to proceed */
  if( db_filep(db) == null )
  { debug0(DBS, DD, "DbRetrieveNext returning FALSE (empty database)");
    ifdebug(DPP, D, ProfileOff("DbRetrieveNext"));
    return FALSE;
  }

  if( in_memory(db) )
  {
    /* get next entry from internal database */
    if( *cont >= db_lineslen(db) )
    { debug0(DBS, DD, "DbRetrieveNext returning FALSE (no successor)");
      ifdebug(DPP, D, ProfileOff("DbRetrieveNext"));
      return FALSE;
    }
    cline = (FULL_CHAR *) db_lines(db)[*cont];
    *gall = (cline[0] == '0' ? 1 : 0);
    sscanf((char *)&cline[*gall], "%d&%[^\t]\t%[^\t]\t%*[^\t]\t%ld\t%d\t%[^\n]",
      &symnum, tag, seq, dfpos, dlnum, fname);
    *cont = *cont + 1;
  }
  else
  {
    /* use *cont to find position of next entry; advance *cont */
    fseek(db_filep(db), *cont == 0L ? (long) left_pos(db) : *cont, SEEK_SET);
    if( StringFGets(line, MAX_BUFF, db_filep(db)) == NULL )
    { debug0(DBS, DD, "DbRetrieveNext returning FALSE (no successor)");
      ifdebug(DPP, D, ProfileOff("DbRetrieveNext"));
      return FALSE;
    }
    *gall = (line[0] == '0' ? 1 : 0);
    sscanf((char *)&line[*gall], "%d&%[^\t]\t%[^\t]\t%*[^\t]\t%ld\t%d\t%[^\n]",
      &symnum, tag, seq, dfpos, dlnum, fname);
    *cont = ftell(db_filep(db));
  }

  /* work out file name if . abbreviation used, and possibly define file */
  if( StringEqual(fname, AsciiToFull(".")) )
  { StringCopy(fname, string(db));
  }
  *dfnum = FileNum(fname, DATA_SUFFIX);
  if( *dfnum == NO_FILE )  /* can only occur in cross reference database */
  { debug0(DFS, D, "  calling DefineFile from DbRetrieveNext");
    *dfnum = DefineFile(fname, DATA_SUFFIX, &fpos(db),
      DATABASE_FILE, SOURCE_PATH);
  }
  NumToSym(db, symnum, *sym);

  /* return */
  debug3(DBS, DD, "DbRetrieveNext returning TRUE (in %s at %ld, line %d)",
    FileName(*dfnum), *dfpos, *dlnum);
  ifdebug(DPP, D, ProfileOff("DbRetrieveNext"));
  return TRUE;
} /* end DbRetrieveNext */
