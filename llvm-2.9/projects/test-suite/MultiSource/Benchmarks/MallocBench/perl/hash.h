/* $Header$
 *
 *    Copyright (c) 1989, Larry Wall
 *
 *    You may distribute under the terms of the GNU General Public License
 *    as specified in the README file that comes with the perl 3.0 kit.
 *
 * $Log$
 * Revision 1.2  2004/09/01 14:33:24  criswell
 * Migrating test suite out of the source tree.
 *
 * Revision 1.1  2004/02/17 22:21:16  criswell
 * Initial commit of the perl Malloc Benchmark.  I've cheated a little by
 * generating the yacc output files and committing them directly, but it was
 * easier than disabling the Bison Voodoo that gets executed by default.
 *
 * Revision 4.0  91/03/20  01:22:38  lwall
 * 4.0 baseline.
 * 
 */

#define FILLPCT 80		/* don't make greater than 99 */
#define DBM_CACHE_MAX 63	/* cache 64 entries for dbm file */
				/* (resident array acts as a write-thru cache)*/

#define COEFFSIZE (16 * 8)	/* size of coeff array */

typedef struct hentry HENT;

struct hentry {
    HENT	*hent_next;
    char	*hent_key;
    STR		*hent_val;
    int		hent_hash;
    int		hent_klen;
};

struct htbl {
    HENT	**tbl_array;
    int		tbl_max;	/* subscript of last element of tbl_array */
    int		tbl_dosplit;	/* how full to get before splitting */
    int		tbl_fill;	/* how full tbl_array currently is */
    int		tbl_riter;	/* current root of iterator */
    HENT	*tbl_eiter;	/* current entry of iterator */
    SPAT 	*tbl_spatroot;	/* list of spats for this package */
    char	*tbl_name;	/* name, if a symbol table */
#ifdef SOME_DBM
#ifdef HAS_GDBM
    GDBM_FILE	tbl_dbm;
#else
#ifdef HAS_NDBM
    DBM		*tbl_dbm;
#else
    int		tbl_dbm;
#endif
#endif
#endif
    unsigned char tbl_coeffsize;	/* is 0 for symbol tables */
};

STR *hfetch();
bool hstore();
STR *hdelete();
HASH *hnew();
void hclear();
void hentfree();
int hiterinit();
HENT *hiternext();
char *hiterkey();
STR *hiterval();
bool hdbmopen();
void hdbmclose();
bool hdbmstore();
