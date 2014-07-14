/*
** 2001 September 15
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** This header file defines the interface that the SQLite library
** presents to client programs.  If a C-function, structure, datatype,
** or constant definition does not appear in this file, then it is
** not a published API of SQLite, is subject to change without
** notice, and should not be referenced by programs that use SQLite.
**
** Some of the definitions that are in this file are marked as
** "experimental".  Experimental interfaces are normally new
** features recently added to SQLite.  We do not anticipate changes 
** to experimental interfaces but reserve to make minor changes if
** experience from use "in the wild" suggest such changes are prudent.
**
** The official C-language API documentation for SQLite is derived
** from comments in this file.  This file is the authoritative source
** on how SQLite interfaces are suppose to operate.
**
** The name of this file under configuration management is "sqlite.h.in".
** The makefile makes some minor changes to this file (such as inserting
** the version number) and changes its name to "sqlite3.h" as
** part of the build process.
**
** @(#) $Id: sqlite.h.in,v 1.212 2007/06/14 20:57:19 drh Exp $
*/
#ifndef _SQLITE3_H_
#define _SQLITE3_H_
#include <stdarg.h>     /* Needed for the definition of va_list */

/*
** Make sure we can call this stuff from C++.
*/
#ifdef __cplusplus
extern "C" {
#endif

/*
** Make sure these symbols where not defined by some previous header
** file.
*/
#ifdef SQLITE_VERSION
# undef SQLITE_VERSION
#endif
#ifdef SQLITE_VERSION_NUMBER
# undef SQLITE_VERSION_NUMBER
#endif

/*
** CAPI3REF: Compile-Time Library Version Numbers
**
** The version of the SQLite library is contained in the sqlite3.h
** header file in a #define named SQLITE_VERSION.  The SQLITE_VERSION
** macro resolves to a string constant.
**
** The format of the version string is "X.Y.Z", where
** X is the major version number, Y is the minor version number and Z
** is the release number.  The X.Y.Z might be followed by "alpha" or "beta".
** For example "3.1.1beta".
**
** The X value is always 3 in SQLite.  The X value only changes when
** backwards compatibility is broken and we intend to never break
** backwards compatibility.  The Y value only changes when
** there are major feature enhancements that are forwards compatible
** but not backwards compatible.  The Z value is incremented with
** each release but resets back to 0 when Y is incremented.
**
** The SQLITE_VERSION_NUMBER is an integer with the value 
** (X*1000000 + Y*1000 + Z). For example, for version "3.1.1beta", 
** SQLITE_VERSION_NUMBER is set to 3001001. To detect if they are using 
** version 3.1.1 or greater at compile time, programs may use the test 
** (SQLITE_VERSION_NUMBER>=3001001).
**
** See also: [sqlite3_libversion()] and [sqlite3_libversion_number()].
*/
#define SQLITE_VERSION         "3.4.0"
#define SQLITE_VERSION_NUMBER 3004000

/*
** CAPI3REF: Run-Time Library Version Numbers
**
** These routines return values equivalent to the header constants
** [SQLITE_VERSION] and [SQLITE_VERSION_NUMBER].  The values returned
** by this routines should only be different from the header values
** if you compile your program using an sqlite3.h header from a
** different version of SQLite that the version of the library you
** link against.
**
** The sqlite3_version[] string constant contains the text of the
** [SQLITE_VERSION] string.  The sqlite3_libversion() function returns
** a poiner to the sqlite3_version[] string constant.  The function
** is provided for DLL users who can only access functions and not
** constants within the DLL.
*/
extern const char sqlite3_version[];
const char *sqlite3_libversion(void);
int sqlite3_libversion_number(void);

/*
** CAPI3REF: Database Connection Handle
**
** Each open SQLite database is represented by pointer to an instance of the
** opaque structure named "sqlite3".  It is useful to think of an sqlite3
** pointer as an object.  The [sqlite3_open] interface is its constructor
** and [sqlite3_close] is its destructor.  There are many other interfaces
** (such as [sqlite3_prepare_v2], [sqlite3_create_function], and
** [sqlite3_busy_timeout] to name but three) that are methods on this
** object.
*/
typedef struct sqlite3 sqlite3;


/*
** CAPI3REF: 64-Bit Integer Types
**
** Some compilers do not support the "long long" datatype.  So we have
** to do compiler-specific typedefs for 64-bit signed and unsigned integers.
**
** Many SQLite interface functions require a 64-bit integer arguments.
** Those interfaces are declared using this typedef.
*/
#ifdef SQLITE_INT64_TYPE
  typedef SQLITE_INT64_TYPE sqlite_int64;
  typedef unsigned SQLITE_INT64_TYPE sqlite_uint64;
#elif defined(_MSC_VER) || defined(__BORLANDC__)
  typedef __int64 sqlite_int64;
  typedef unsigned __int64 sqlite_uint64;
#else
  typedef long long int sqlite_int64;
  typedef unsigned long long int sqlite_uint64;
#endif

/*
** If compiling for a processor that lacks floating point support,
** substitute integer for floating-point
*/
#ifdef SQLITE_OMIT_FLOATING_POINT
# define double sqlite_int64
#endif

/*
** CAPI3REF: Closing A Database Connection
**
** Call this function with a pointer to a structure that was previously
** returned from [sqlite3_open()] and the corresponding database will by
** closed.
**
** All SQL statements prepared using [sqlite3_prepare_v2()] or
** [sqlite3_prepare16_v2()] must be destroyed using [sqlite3_finalize()]
** before this routine is called. Otherwise, SQLITE_BUSY is returned and the
** database connection remains open.
*/
int sqlite3_close(sqlite3 *);

/*
** The type for a callback function.
** This is legacy and deprecated.  It is included for historical
** compatibility and is not documented.
*/
typedef int (*sqlite3_callback)(void*,int,char**, char**);

/*
** CAPI3REF: One-Step Query Execution Interface
**
** This interface is used to do a one-time evaluatation of zero
** or more SQL statements.  UTF-8 text of the SQL statements to
** be evaluted is passed in as the second parameter.  The statements
** are prepared one by one using [sqlite3_prepare()], evaluated
** using [sqlite3_step()], then destroyed using [sqlite3_finalize()].
**
** If one or more of the SQL statements are queries, then
** the callback function specified by the 3rd parameter is
** invoked once for each row of the query result.  This callback
** should normally return 0.  If the callback returns a non-zero
** value then the query is aborted, all subsequent SQL statements
** are skipped and the sqlite3_exec() function returns the SQLITE_ABORT.
**
** The 4th parameter to this interface is an arbitrary pointer that is
** passed through to the callback function as its first parameter.
**
** The 2nd parameter to the callback function is the number of
** columns in the query result.  The 3rd parameter to the callback
** is an array of strings holding the values for each column
** as extracted using [sqlite3_column_text()].
** The 4th parameter to the callback is an array of strings
** obtained using [sqlite3_column_name()] and holding
** the names of each column.
**
** The callback function may be NULL, even for queries.  A NULL
** callback is not an error.  It just means that no callback
** will be invoked.
**
** If an error occurs while parsing or evaluating the SQL (but
** not while executing the callback) then an appropriate error
** message is written into memory obtained from [sqlite3_malloc()] and
** *errmsg is made to point to that message.  The calling function
** is responsible for freeing the memory that holds the error
** message.   Use [sqlite3_free()] for this.  If errmsg==NULL,
** then no error message is ever written.
**
** The return value is is SQLITE_OK if there are no errors and
** some other [SQLITE_OK | return code] if there is an error.  
** The particular return value depends on the type of error. 
**
*/
int sqlite3_exec(
  sqlite3*,                                  /* An open database */
  const char *sql,                           /* SQL to be evaluted */
  int (*callback)(void*,int,char**,char**),  /* Callback function */
  void *,                                    /* 1st argument to callback */
  char **errmsg                              /* Error msg written here */
);

/*
** CAPI3REF: Result Codes
** KEYWORDS: SQLITE_OK
**
** Many SQLite functions return an integer result code from the set shown
** above in order to indicates success or failure.
**
** The result codes above are the only ones returned by SQLite in its
** default configuration.  However, the [sqlite3_extended_result_codes()]
** API can be used to set a database connectoin to return more detailed
** result codes.
**
** See also: [SQLITE_IOERR_READ | extended result codes]
**
*/
#define SQLITE_OK           0   /* Successful result */
/* beginning-of-error-codes */
#define SQLITE_ERROR        1   /* SQL error or missing database */
#define SQLITE_INTERNAL     2   /* NOT USED. Internal logic error in SQLite */
#define SQLITE_PERM         3   /* Access permission denied */
#define SQLITE_ABORT        4   /* Callback routine requested an abort */
#define SQLITE_BUSY         5   /* The database file is locked */
#define SQLITE_LOCKED       6   /* A table in the database is locked */
#define SQLITE_NOMEM        7   /* A malloc() failed */
#define SQLITE_READONLY     8   /* Attempt to write a readonly database */
#define SQLITE_INTERRUPT    9   /* Operation terminated by sqlite3_interrupt()*/
#define SQLITE_IOERR       10   /* Some kind of disk I/O error occurred */
#define SQLITE_CORRUPT     11   /* The database disk image is malformed */
#define SQLITE_NOTFOUND    12   /* NOT USED. Table or record not found */
#define SQLITE_FULL        13   /* Insertion failed because database is full */
#define SQLITE_CANTOPEN    14   /* Unable to open the database file */
#define SQLITE_PROTOCOL    15   /* NOT USED. Database lock protocol error */
#define SQLITE_EMPTY       16   /* Database is empty */
#define SQLITE_SCHEMA      17   /* The database schema changed */
#define SQLITE_TOOBIG      18   /* String or BLOB exceeds size limit */
#define SQLITE_CONSTRAINT  19   /* Abort due to contraint violation */
#define SQLITE_MISMATCH    20   /* Data type mismatch */
#define SQLITE_MISUSE      21   /* Library used incorrectly */
#define SQLITE_NOLFS       22   /* Uses OS features not supported on host */
#define SQLITE_AUTH        23   /* Authorization denied */
#define SQLITE_FORMAT      24   /* Auxiliary database format error */
#define SQLITE_RANGE       25   /* 2nd parameter to sqlite3_bind out of range */
#define SQLITE_NOTADB      26   /* File opened that is not a database file */
#define SQLITE_ROW         100  /* sqlite3_step() has another row ready */
#define SQLITE_DONE        101  /* sqlite3_step() has finished executing */
/* end-of-error-codes */

/*
** CAPI3REF: Extended Result Codes
**
** In its default configuration, SQLite API routines return one of 26 integer
** result codes described at result-codes.  However, experience has shown that
** many of these result codes are too course-grained.  They do not provide as
** much information about problems as users might like.  In an effort to
** address this, newer versions of SQLite (version 3.3.8 and later) include
** support for additional result codes that provide more detailed information
** about errors.  The extended result codes are enabled (or disabled) for 
** each database
** connection using the [sqlite3_extended_result_codes()] API.
** 
** Some of the available extended result codes are listed above.
** We expect the number of extended result codes will be expand
** over time.  Software that uses extended result codes should expect
** to see new result codes in future releases of SQLite.
** 
** The symbolic name for an extended result code always contains a related
** primary result code as a prefix.  Primary result codes contain a single
** "_" character.  Extended result codes contain two or more "_" characters.
** The numeric value of an extended result code can be converted to its
** corresponding primary result code by masking off the lower 8 bytes.
**
** The SQLITE_OK result code will never be extended.  It will always
** be exactly zero.
*/
#define SQLITE_IOERR_READ          (SQLITE_IOERR | (1<<8))
#define SQLITE_IOERR_SHORT_READ    (SQLITE_IOERR | (2<<8))
#define SQLITE_IOERR_WRITE         (SQLITE_IOERR | (3<<8))
#define SQLITE_IOERR_FSYNC         (SQLITE_IOERR | (4<<8))
#define SQLITE_IOERR_DIR_FSYNC     (SQLITE_IOERR | (5<<8))
#define SQLITE_IOERR_TRUNCATE      (SQLITE_IOERR | (6<<8))
#define SQLITE_IOERR_FSTAT         (SQLITE_IOERR | (7<<8))
#define SQLITE_IOERR_UNLOCK        (SQLITE_IOERR | (8<<8))
#define SQLITE_IOERR_RDLOCK        (SQLITE_IOERR | (9<<8))
#define SQLITE_IOERR_DELETE        (SQLITE_IOERR | (10<<8))
#define SQLITE_IOERR_BLOCKED       (SQLITE_IOERR | (11<<8))

/*
** CAPI3REF: Enable Or Disable Extended Result Codes
**
** This routine enables or disables the
** [SQLITE_IOERR_READ | extended result codes] feature.
** By default, SQLite API routines return one of only 26 integer
** [SQLITE_OK | result codes].  When extended result codes
** are enabled by this routine, the repetoire of result codes can be
** much larger and can (hopefully) provide more detailed information
** about the cause of an error.
**
** The second argument is a boolean value that turns extended result
** codes on and off.  Extended result codes are off by default for
** backwards compatibility with older versions of SQLite.
*/
int sqlite3_extended_result_codes(sqlite3*, int onoff);

/*
** CAPI3REF: Last Insert Rowid
**
** Each entry in an SQLite table has a unique 64-bit signed integer key
** called the "rowid". The rowid is always available as an undeclared
** column named ROWID, OID, or _ROWID_.  If the table has a column of
** type INTEGER PRIMARY KEY then that column is another an alias for the
** rowid.
**
** This routine returns the rowid of the most recent INSERT into
** the database from the database connection given in the first 
** argument.  If no inserts have ever occurred on this database
** connection, zero is returned.
**
** If an INSERT occurs within a trigger, then the rowid of the
** inserted row is returned by this routine as long as the trigger
** is running.  But once the trigger terminates, the value returned
** by this routine reverts to the last value inserted before the
** trigger fired.
*/
sqlite_int64 sqlite3_last_insert_rowid(sqlite3*);

/*
** CAPI3REF: Count The Number Of Rows Modified
**
** This function returns the number of database rows that were changed
** (or inserted or deleted) by the most recent SQL statement.  Only
** changes that are directly specified by the INSERT, UPDATE, or
** DELETE statement are counted.  Auxiliary changes caused by
** triggers are not counted.  Use the [sqlite3_total_changes()] function
** to find the total number of changes including changes caused by triggers.
**
** Within the body of a trigger, the sqlite3_changes() interface can be
** called to find the number of
** changes in the most recently completed INSERT, UPDATE, or DELETE
** statement within the body of the trigger.
**
** All changes are counted, even if they were later undone by a
** ROLLBACK or ABORT.  Except, changes associated with creating and
** dropping tables are not counted.
**
** If a callback invokes [sqlite3_exec()] or [sqlite3_step()] recursively,
** then the changes in the inner, recursive call are counted together
** with the changes in the outer call.
**
** SQLite implements the command "DELETE FROM table" without a WHERE clause
** by dropping and recreating the table.  (This is much faster than going
** through and deleting individual elements form the table.)  Because of
** this optimization, the change count for "DELETE FROM table" will be
** zero regardless of the number of elements that were originally in the
** table. To get an accurate count of the number of rows deleted, use
** "DELETE FROM table WHERE 1" instead.
*/
int sqlite3_changes(sqlite3*);

/*
** CAPI3REF: Total Number Of Rows Modified
***
** This function returns the number of database rows that have been
** modified by INSERT, UPDATE or DELETE statements since the database handle
** was opened. This includes UPDATE, INSERT and DELETE statements executed
** as part of trigger programs. All changes are counted as soon as the
** statement that makes them is completed (when the statement handle is
** passed to [sqlite3_reset()] or [sqlite_finalise()]).
**
** See also the [sqlite3_change()] interface.
**
** SQLite implements the command "DELETE FROM table" without a WHERE clause
** by dropping and recreating the table.  (This is much faster than going
** through and deleting individual elements form the table.)  Because of
** this optimization, the change count for "DELETE FROM table" will be
** zero regardless of the number of elements that were originally in the
** table. To get an accurate count of the number of rows deleted, use
** "DELETE FROM table WHERE 1" instead.
*/
int sqlite3_total_changes(sqlite3*);

/*
** CAPI3REF: Interrupt A Long-Running Query
**
** This function causes any pending database operation to abort and
** return at its earliest opportunity.  This routine is typically
** called in response to a user action such as pressing "Cancel"
** or Ctrl-C where the user wants a long query operation to halt
** immediately.
**
** It is safe to call this routine from a thread different from the
** thread that is currently running the database operation.
**
** The SQL operation that is interrupted will return [SQLITE_INTERRUPT].
** If an interrupted operation was an update that is inside an
** explicit transaction, then the entire transaction will be rolled
** back automatically.
*/
void sqlite3_interrupt(sqlite3*);

/*
** CAPI3REF: Determine If An SQL Statement Is Complete
**
** These functions return true if the given input string comprises
** one or more complete SQL statements. For the sqlite3_complete() call,
** the parameter must be a nul-terminated UTF-8 string. For
** sqlite3_complete16(), a nul-terminated machine byte order UTF-16 string
** is required.
**
** These routines are useful for command-line input to determine if the
** currently entered text forms one or more complete SQL statements or
** if additional input is needed before sending the statements into
** SQLite for parsing. The algorithm is simple.  If the 
** last token other than spaces and comments is a semicolon, then return 
** true.  Actually, the algorithm is a little more complicated than that
** in order to deal with triggers, but the basic idea is the same:  the
** statement is not complete unless it ends in a semicolon.
*/
int sqlite3_complete(const char *sql);
int sqlite3_complete16(const void *sql);

/*
** CAPI3REF: Register A Callback To Handle SQLITE_BUSY Errors
**
** This routine identifies a callback function that might be invoked
** whenever an attempt is made to open a database table 
** that another thread or process has locked.
** If the busy callback is NULL, then [SQLITE_BUSY]
** (or sometimes [SQLITE_IOERR_BLOCKED])
** is returned immediately upon encountering the lock.
** If the busy callback is not NULL, then the
** callback will be invoked with two arguments.  The
** first argument to the handler is a copy of the void* pointer which
** is the third argument to this routine.  The second argument to
** the handler is the number of times that the busy handler has
** been invoked for this locking event. If the
** busy callback returns 0, then no additional attempts are made to
** access the database and [SQLITE_BUSY] or [SQLITE_IOERR_BLOCKED] is returned.
** If the callback returns non-zero, then another attempt is made to open the
** database for reading and the cycle repeats.
**
** The presence of a busy handler does not guarantee that
** it will be invoked when there is lock contention.
** If SQLite determines that invoking the busy handler could result in
** a deadlock, it will return [SQLITE_BUSY] instead.
** Consider a scenario where one process is holding a read lock that
** it is trying to promote to a reserved lock and
** a second process is holding a reserved lock that it is trying
** to promote to an exclusive lock.  The first process cannot proceed
** because it is blocked by the second and the second process cannot
** proceed because it is blocked by the first.  If both processes
** invoke the busy handlers, neither will make any progress.  Therefore,
** SQLite returns [SQLITE_BUSY] for the first process, hoping that this
** will induce the first process to release its read lock and allow
** the second process to proceed.
**
** The default busy callback is NULL.
**
** The [SQLITE_BUSY] error is converted to [SQLITE_IOERR_BLOCKED] when
** SQLite is in the middle of a large transaction where all the
** changes will not fit into the in-memory cache.  SQLite will
** already hold a RESERVED lock on the database file, but it needs
** to promote this lock to EXCLUSIVE so that it can spill cache
** pages into the database file without harm to concurrent
** readers.  If it is unable to promote the lock, then the in-memory
** cache will be left in an inconsistent state and so the error
** code is promoted from the relatively benign [SQLITE_BUSY] to
** the more severe [SQLITE_IOERR_BLOCKED].  This error code promotion
** forces an automatic rollback of the changes. See the
** <a href="http://www.sqlite.org/cvstrac/wiki?p=CorruptionFollowingBusyError">
** CorruptionFollowingBusyError</a> wiki page for a discussion of why
** this is important.
**	
** Sqlite is re-entrant, so the busy handler may start a new query. 
** (It is not clear why anyone would every want to do this, but it
** is allowed, in theory.)  But the busy handler may not close the
** database.  Closing the database from a busy handler will delete 
** data structures out from under the executing query and will 
** probably result in a segmentation fault or other runtime error.
**
** There can only be a single busy handler defined for each database
** connection.  Setting a new busy handler clears any previous one.
** Note that calling [sqlite3_busy_timeout()] will also set or clear
** the busy handler.
*/
int sqlite3_busy_handler(sqlite3*, int(*)(void*,int), void*);

/*
** CAPI3REF: Set A Busy Timeout
**
** This routine sets a busy handler that sleeps for a while when a
** table is locked.  The handler will sleep multiple times until 
** at least "ms" milliseconds of sleeping have been done.  After
** "ms" milliseconds of sleeping, the handler returns 0 which
** causes [sqlite3_step()] to return [SQLITE_BUSY] or [SQLITE_IOERR_BLOCKED].
**
** Calling this routine with an argument less than or equal to zero
** turns off all busy handlers.
**
** There can only be a single busy handler for a particular database
** connection.  If another busy handler was defined  
** (using [sqlite3_busy_handler()]) prior to calling
** this routine, that other busy handler is cleared.
*/
int sqlite3_busy_timeout(sqlite3*, int ms);

/*
** CAPI3REF: Convenience Routines For Running Queries
**
** This next routine is a convenience wrapper around [sqlite3_exec()].
** Instead of invoking a user-supplied callback for each row of the
** result, this routine remembers each row of the result in memory
** obtained from [sqlite3_malloc()], then returns all of the result after the
** query has finished. 
**
** As an example, suppose the query result where this table:
**
** <pre>
**        Name        | Age
**        -----------------------
**        Alice       | 43
**        Bob         | 28
**        Cindy       | 21
** </pre>
**
** If the 3rd argument were &azResult then after the function returns
** azResult will contain the following data:
**
** <pre>
**        azResult[0] = "Name";
**        azResult[1] = "Age";
**        azResult[2] = "Alice";
**        azResult[3] = "43";
**        azResult[4] = "Bob";
**        azResult[5] = "28";
**        azResult[6] = "Cindy";
**        azResult[7] = "21";
** </pre>
**
** Notice that there is an extra row of data containing the column
** headers.  But the *nrow return value is still 3.  *ncolumn is
** set to 2.  In general, the number of values inserted into azResult
** will be ((*nrow) + 1)*(*ncolumn).
**
** After the calling function has finished using the result, it should 
** pass the result data pointer to sqlite3_free_table() in order to 
** release the memory that was malloc-ed.  Because of the way the 
** [sqlite3_malloc()] happens, the calling function must not try to call 
** [sqlite3_free()] directly.  Only [sqlite3_free_table()] is able to release 
** the memory properly and safely.
**
** The return value of this routine is the same as from [sqlite3_exec()].
*/
int sqlite3_get_table(
  sqlite3*,              /* An open database */
  const char *sql,       /* SQL to be executed */
  char ***resultp,       /* Result written to a char *[]  that this points to */
  int *nrow,             /* Number of result rows written here */
  int *ncolumn,          /* Number of result columns written here */
  char **errmsg          /* Error msg written here */
);
void sqlite3_free_table(char **result);

/*
** CAPI3REF: Formatted String Printing Functions
**
** These routines are workalikes of the "printf()" family of functions
** from the standard C library.
**
** The sqlite3_mprintf() and sqlite3_vmprintf() routines write their
** results into memory obtained from [sqlite_malloc()].
** The strings returned by these two routines should be
** released by [sqlite3_free()].  Both routines return a
** NULL pointer if [sqlite3_malloc()] is unable to allocate enough
** memory to hold the resulting string.
**
** In sqlite3_snprintf() routine is similar to "snprintf()" from
** the standard C library.  The result is written into the
** buffer supplied as the second parameter whose size is given by
** the first parameter.  Note that the order of the
** first two parameters is reversed from snprintf().  This is an
** historical accident that cannot be fixed without breaking
** backwards compatibility.  Note also that sqlite3_snprintf()
** returns a pointer to its buffer instead of the number of
** characters actually written into the buffer.  We admit that
** the number of characters written would be a more useful return
** value but we cannot change the implementation of sqlite3_snprintf()
** now without breaking compatibility.
**
** As long as the buffer size is greater than zero, sqlite3_snprintf()
** guarantees that the buffer is always zero-terminated.  The first
** parameter "n" is the total size of the buffer, including space for
** the zero terminator.  So the longest string that can be completely
** written will be n-1 characters.
**
** These routines all implement some additional formatting
** options that are useful for constructing SQL statements.
** All of the usual printf formatting options apply.  In addition, there
** is are "%q" and "%Q" options.
**
** The %q option works like %s in that it substitutes a null-terminated
** string from the argument list.  But %q also doubles every '\'' character.
** %q is designed for use inside a string literal.  By doubling each '\''
** character it escapes that character and allows it to be inserted into
** the string.
**
** For example, so some string variable contains text as follows:
**
** <blockquote><pre>
**  char *zText = "It's a happy day!";
** </pre></blockquote>
**
** One can use this text in an SQL statement as follows:
**
** <blockquote><pre>
**  char *zSQL = sqlite3_mprintf("INSERT INTO table VALUES('%q')", zText);
**  sqlite3_exec(db, zSQL, 0, 0, 0);
**  sqlite3_free(zSQL);
** </pre></blockquote>
**
** Because the %q format string is used, the '\'' character in zText
** is escaped and the SQL generated is as follows:
**
** <blockquote><pre>
**  INSERT INTO table1 VALUES('It''s a happy day!')
** </pre></blockquote>
**
** This is correct.  Had we used %s instead of %q, the generated SQL
** would have looked like this:
**
** <blockquote><pre>
**  INSERT INTO table1 VALUES('It's a happy day!');
** </pre></blockquote>
**
** This second example is an SQL syntax error.  As a general rule you
** should always use %q instead of %s when inserting text into a string 
** literal.
**
** The %Q option works like %q except it also adds single quotes around
** the outside of the total string.  Or if the parameter in the argument
** list is a NULL pointer, %Q substitutes the text "NULL" (without single
** quotes) in place of the %Q option.  So, for example, one could say:
**
** <blockquote><pre>
**  char *zSQL = sqlite3_mprintf("INSERT INTO table VALUES(%Q)", zText);
**  sqlite3_exec(db, zSQL, 0, 0, 0);
**  sqlite3_free(zSQL);
** </pre></blockquote>
**
** The code above will render a correct SQL statement in the zSQL
** variable even if the zText variable is a NULL pointer.
*/
char *sqlite3_mprintf(const char*,...);
char *sqlite3_vmprintf(const char*, va_list);
char *sqlite3_snprintf(int,char*,const char*, ...);

/*
** CAPI3REF: Memory Allocation Functions
**
** SQLite uses its own memory allocator.  On some installations, this
** memory allocator is identical to the standard malloc()/realloc()/free()
** and can be used interchangable.  On others, the implementations are
** different.  For maximum portability, it is best not to mix calls
** to the standard malloc/realloc/free with the sqlite versions.
*/
void *sqlite3_malloc(int);
void *sqlite3_realloc(void*, int);
void sqlite3_free(void*);

/*
** CAPI3REF: Compile-Time Authorization Callbacks
***
** This routine registers a authorizer callback with the SQLite library.  
** The authorizer callback is invoked as SQL statements are being compiled
** by [sqlite3_prepare()] or its variants [sqlite3_prepare_v2()],
** [sqlite3_prepare16()] and [sqlite3_prepare16_v2()].  At various
** points during the compilation process, as logic is being created
** to perform various actions, the authorizer callback is invoked to
** see if those actions are allowed.  The authorizer callback should
** return SQLITE_OK to allow the action, [SQLITE_IGNORE] to disallow the
** specific action but allow the SQL statement to continue to be
** compiled, or [SQLITE_DENY] to cause the entire SQL statement to be
** rejected with an error.  
**
** Depending on the action, the [SQLITE_IGNORE] and [SQLITE_DENY] return
** codes might mean something different or they might mean the same
** thing.  If the action is, for example, to perform a delete opertion,
** then [SQLITE_IGNORE] and [SQLITE_DENY] both cause the statement compilation
** to fail with an error.  But if the action is to read a specific column
** from a specific table, then [SQLITE_DENY] will cause the entire
** statement to fail but [SQLITE_IGNORE] will cause a NULL value to be
** read instead of the actual column value.
**
** The first parameter to the authorizer callback is a copy of
** the third parameter to the sqlite3_set_authorizer() interface.
** The second parameter to the callback is an integer 
** [SQLITE_COPY | action code] that specifies the particular action
** to be authorized.  The available action codes are
** [SQLITE_COPY | documented separately].  The third through sixth
** parameters to the callback are strings that contain additional
** details about the action to be authorized.
**
** An authorizer is used when preparing SQL statements from an untrusted
** source, to ensure that the SQL statements do not try to access data
** that they are not allowed to see, or that they do not try to
** execute malicious statements that damage the database.  For
** example, an application may allow a user to enter arbitrary
** SQL queries for evaluation by a database.  But the application does
** not want the user to be able to make arbitrary changes to the
** database.  An authorizer could then be put in place while the
** user-entered SQL is being prepared that disallows everything
** except SELECT statements.  
**
** Only a single authorizer can be in place on a database connection
** at a time.  Each call to sqlite3_set_authorizer overrides the
** previous call.  A NULL authorizer means that no authorization
** callback is invoked.  The default authorizer is NULL.
**
** Note that the authorizer callback is invoked only during 
** [sqlite3_prepare()] or its variants.  Authorization is not
** performed during statement evaluation in [sqlite3_step()].
*/
int sqlite3_set_authorizer(
  sqlite3*,
  int (*xAuth)(void*,int,const char*,const char*,const char*,const char*),
  void *pUserData
);

/*
** CAPI3REF: Authorizer Return Codes
**
** The [sqlite3_set_authorizer | authorizer callback function] must
** return either [SQLITE_OK] or one of these two constants in order
** to signal SQLite whether or not the action is permitted.  See the
** [sqlite3_set_authorizer | authorizer documentation] for additional
** information.
*/
#define SQLITE_DENY   1   /* Abort the SQL statement with an error */
#define SQLITE_IGNORE 2   /* Don't allow access, but don't generate an error */

/*
** CAPI3REF: Authorizer Action Codes
**
** The [sqlite3_set_authorizer()] interface registers a callback function
** that is invoked to authorizer certain SQL statement actions.  The
** second parameter to the callback is an integer code that specifies
** what action is being authorized.  These are the integer action codes that
** the authorizer callback may be passed.
**
** These action code values signify what kind of operation is to be 
** authorized.  The 3rd and 4th parameters to the authorization callback
** function will be parameters or NULL depending on which of these
** codes is used as the second parameter.  The 5th parameter to the
** authorizer callback is the name of the database ("main", "temp", 
** etc.) if applicable.  The 6th parameter to the authorizer callback
** is the name of the inner-most trigger or view that is responsible for
** the access attempt or NULL if this access attempt is directly from 
** top-level SQL code.
*/
/******************************************* 3rd ************ 4th ***********/
#define SQLITE_CREATE_INDEX          1   /* Index Name      Table Name      */
#define SQLITE_CREATE_TABLE          2   /* Table Name      NULL            */
#define SQLITE_CREATE_TEMP_INDEX     3   /* Index Name      Table Name      */
#define SQLITE_CREATE_TEMP_TABLE     4   /* Table Name      NULL            */
#define SQLITE_CREATE_TEMP_TRIGGER   5   /* Trigger Name    Table Name      */
#define SQLITE_CREATE_TEMP_VIEW      6   /* View Name       NULL            */
#define SQLITE_CREATE_TRIGGER        7   /* Trigger Name    Table Name      */
#define SQLITE_CREATE_VIEW           8   /* View Name       NULL            */
#define SQLITE_DELETE                9   /* Table Name      NULL            */
#define SQLITE_DROP_INDEX           10   /* Index Name      Table Name      */
#define SQLITE_DROP_TABLE           11   /* Table Name      NULL            */
#define SQLITE_DROP_TEMP_INDEX      12   /* Index Name      Table Name      */
#define SQLITE_DROP_TEMP_TABLE      13   /* Table Name      NULL            */
#define SQLITE_DROP_TEMP_TRIGGER    14   /* Trigger Name    Table Name      */
#define SQLITE_DROP_TEMP_VIEW       15   /* View Name       NULL            */
#define SQLITE_DROP_TRIGGER         16   /* Trigger Name    Table Name      */
#define SQLITE_DROP_VIEW            17   /* View Name       NULL            */
#define SQLITE_INSERT               18   /* Table Name      NULL            */
#define SQLITE_PRAGMA               19   /* Pragma Name     1st arg or NULL */
#define SQLITE_READ                 20   /* Table Name      Column Name     */
#define SQLITE_SELECT               21   /* NULL            NULL            */
#define SQLITE_TRANSACTION          22   /* NULL            NULL            */
#define SQLITE_UPDATE               23   /* Table Name      Column Name     */
#define SQLITE_ATTACH               24   /* Filename        NULL            */
#define SQLITE_DETACH               25   /* Database Name   NULL            */
#define SQLITE_ALTER_TABLE          26   /* Database Name   Table Name      */
#define SQLITE_REINDEX              27   /* Index Name      NULL            */
#define SQLITE_ANALYZE              28   /* Table Name      NULL            */
#define SQLITE_CREATE_VTABLE        29   /* Table Name      Module Name     */
#define SQLITE_DROP_VTABLE          30   /* Table Name      Module Name     */
#define SQLITE_FUNCTION             31   /* Function Name   NULL            */
#define SQLITE_COPY                  0   /* No longer used */

/*
** CAPI3REF: Tracing And Profiling Functions
**
** These routines register callback functions that can be used for
** tracing and profiling the execution of SQL statements.
** The callback function registered by sqlite3_trace() is invoked
** at the first [sqlite3_step()] for the evaluation of an SQL statement.
** The callback function registered by sqlite3_profile() is invoked
** as each SQL statement finishes and includes
** information on how long that statement ran.
**
** The sqlite3_profile() API is currently considered experimental and
** is subject to change.
*/
void *sqlite3_trace(sqlite3*, void(*xTrace)(void*,const char*), void*);
void *sqlite3_profile(sqlite3*,
   void(*xProfile)(void*,const char*,sqlite_uint64), void*);

/*
** CAPI3REF: Query Progress Callbacks
**
** This routine configures a callback function - the progress callback - that
** is invoked periodically during long running calls to [sqlite3_exec()],
** [sqlite3_step()] and [sqlite3_get_table()].  An example use for this 
** interface is to keep a GUI updated during a large query.
**
** The progress callback is invoked once for every N virtual machine opcodes,
** where N is the second argument to this function. The progress callback
** itself is identified by the third argument to this function. The fourth
** argument to this function is a void pointer passed to the progress callback
** function each time it is invoked.
**
** If a call to [sqlite3_exec()], [sqlite3_step()], or [sqlite3_get_table()]
** results in fewer than N opcodes being executed, then the progress 
** callback is never invoked.
** 
** Only a single progress callback function may be registered for each
** open database connection.  Every call to sqlite3_progress_handler()
** overwrites the results of the previous call.
** To remove the progress callback altogether, pass NULL as the third
** argument to this function.
**
** If the progress callback returns a result other than 0, then the current 
** query is immediately terminated and any database changes rolled back.
** The containing [sqlite3_exec()], [sqlite3_step()], or
** [sqlite3_get_table()] call returns SQLITE_INTERRUPT.   This feature
** can be used, for example, to implement the "Cancel" button on a
** progress dialog box in a GUI.
*/
void sqlite3_progress_handler(sqlite3*, int, int(*)(void*), void*);

/*
** CAPI3REF: Opening A New Database Connection
**
** Open the sqlite database file "filename".  The "filename" is UTF-8
** encoded for sqlite3_open() and UTF-16 encoded in the native byte order
** for sqlite3_open16().  An [sqlite3*] handle is returned in *ppDb, even
** if an error occurs. If the database is opened (or created) successfully,
** then SQLITE_OK is returned. Otherwise an error code is returned. The
** sqlite3_errmsg() or sqlite3_errmsg16()  routines can be used to obtain
** an English language description of the error.
**
** If the database file does not exist, then a new database will be created
** as needed.  The default encoding for the database will be UTF-8 if
** sqlite3_open() is called and UTF-16 if sqlite3_open16 is used.
**
** Whether or not an error occurs when it is opened, resources associated
** with the [sqlite3*] handle should be released by passing it to
** sqlite3_close() when it is no longer required.
**
** Note to windows users:  The encoding used for the filename argument
** of sqlite3_open() must be UTF-8, not whatever codepage is currently
** defined.  Filenames containing international characters must be converted
** to UTF-8 prior to passing them into sqlite3_open().
*/
int sqlite3_open(
  const char *filename,   /* Database filename (UTF-8) */
  sqlite3 **ppDb          /* OUT: SQLite db handle */
);
int sqlite3_open16(
  const void *filename,   /* Database filename (UTF-16) */
  sqlite3 **ppDb          /* OUT: SQLite db handle */
);

/*
** CAPI3REF: Error Codes And Messages
**
** The sqlite3_errcode() interface returns the numeric
** [SQLITE_OK | result code] or [SQLITE_IOERR_READ | extended result code]
** for the most recent failed sqlite3_* API call associated
** with [sqlite3] handle 'db'.  If a prior API call failed but the
** most recent API call succeeded, the return value from sqlite3_errcode()
** is undefined. 
**
** The sqlite3_errmsg() and sqlite3_errmsg16() return English-langauge
** text that describes the error, as either UTF8 or UTF16 respectively.
** Memory to hold the error message string is managed internally.  The 
** string may be overwritten or deallocated by subsequent calls to SQLite
** interface functions.
**
** Calls to many sqlite3_* functions set the error code and string returned
** by [sqlite3_errcode()], [sqlite3_errmsg()], and [sqlite3_errmsg16()]
** (overwriting the previous values). Note that calls to [sqlite3_errcode()],
** [sqlite3_errmsg()], and [sqlite3_errmsg16()] themselves do not affect the
** results of future invocations.  Calls to API routines that do not return
** an error code (examples: [sqlite3_data_count()] or [sqlite3_mprintf()]) do
** not change the error code returned by this routine.
**
** Assuming no other intervening sqlite3_* API calls are made, the error
** code returned by this function is associated with the same error as
** the strings returned by [sqlite3_errmsg()] and [sqlite3_errmsg16()].
*/
int sqlite3_errcode(sqlite3 *db);
const char *sqlite3_errmsg(sqlite3*);
const void *sqlite3_errmsg16(sqlite3*);

/*
** CAPI3REF: SQL Statement Object
**
** Instance of this object represent single SQL statements.  This
** is variously known as a "prepared statement" or a 
** "compiled SQL statement" or simply as a "statement".
** 
** The life of a statement object goes something like this:
**
** <ol>
** <li> Create the object using [sqlite3_prepare_v2()] or a related
**      function.
** <li> Bind values to host parameters using
**      [sqlite3_bind_blob | sqlite3_bind_* interfaces].
** <li> Run the SQL by calling [sqlite3_step()] one or more times.
** <li> Reset the statement using [sqlite3_reset()] then go back
**      to step 2.  Do this zero or more times.
** <li> Destroy the object using [sqlite3_finalize()].
** </ol>
**
** Refer to documentation on individual methods above for additional
** information.
*/
typedef struct sqlite3_stmt sqlite3_stmt;

/*
** CAPI3REF: Compiling An SQL Statement
**
** To execute an SQL query, it must first be compiled into a byte-code
** program using one of these routines. 
**
** The first argument "db" is an [sqlite3 | SQLite database handle] 
** obtained from a prior call to [sqlite3_open()] or [sqlite3_open16()].
** The second argument "zSql" is the statement to be compiled, encoded
** as either UTF-8 or UTF-16.  The sqlite3_prepare() and sqlite3_prepare_v2()
** interfaces uses UTF-8 and sqlite3_prepare16() and sqlite3_prepare16_v2()
** use UTF-16. If the next argument, "nBytes", is less
** than zero, then zSql is read up to the first zero terminator.  If
** "nBytes" is not less than zero, then it is the length of the string zSql
** in bytes (not characters).
**
** *pzTail is made to point to the first byte past the end of the first
** SQL statement in zSql.  This routine only compiles the first statement
** in zSql, so *pzTail is left pointing to what remains uncompiled.
**
** *ppStmt is left pointing to a compiled 
** [sqlite3_stmt | SQL statement structure] that can be
** executed using [sqlite3_step()].  Or if there is an error, *ppStmt may be
** set to NULL.  If the input text contained no SQL (if the input is and
** empty string or a comment) then *ppStmt is set to NULL.  The calling
** procedure is responsible for deleting the compiled SQL statement
** using [sqlite3_finalize()] after it has finished with it.
**
** On success, [SQLITE_OK] is returned.  Otherwise an 
** [SQLITE_ERROR | error code] is returned.
**
** The sqlite3_prepare_v2() and sqlite3_prepare16_v2() interfaces are
** recommended for all new programs. The two older interfaces are retained
** for backwards compatibility, but their use is discouraged.
** In the "v2" interfaces, the prepared statement
** that is returned (the [sqlite3_stmt] object) contains a copy of the 
** original SQL text. This causes the [sqlite3_step()] interface to
** behave a differently in two ways:
**
** <ol>
** <li>
** If the database schema changes, instead of returning [SQLITE_SCHEMA] as it
** always used to do, [sqlite3_step()] will automatically recompile the SQL
** statement and try to run it again.  If the schema has changed in a way
** that makes the statement no longer valid, [sqlite3_step()] will still
** return [SQLITE_SCHEMA].  But unlike the legacy behavior, [SQLITE_SCHEMA] is
** now a fatal error.  Calling [sqlite3_prepare_v2()] again will not make the
** error go away.  Note: use [sqlite3_errmsg()] to find the text of the parsing
** error that results in an [SQLITE_SCHEMA] return.
** </li>
**
** <li>
** When an error occurs, 
** [sqlite3_step()] will return one of the detailed 
** [SQLITE_ERROR | result codes] or
** [SQLITE_IOERR_READ | extended result codes] such as directly.
** The legacy behavior was that [sqlite3_step()] would only return a generic
** [SQLITE_ERROR] result code and you would have to make a second call to
** [sqlite3_reset()] in order to find the underlying cause of the problem.
** With the "v2" prepare interfaces, the underlying reason for the error is
** returned immediately.
** </li>
** </ol>
*/
int sqlite3_prepare(
  sqlite3 *db,            /* Database handle */
  const char *zSql,       /* SQL statement, UTF-8 encoded */
  int nBytes,             /* Length of zSql in bytes. */
  sqlite3_stmt **ppStmt,  /* OUT: Statement handle */
  const char **pzTail     /* OUT: Pointer to unused portion of zSql */
);
int sqlite3_prepare_v2(
  sqlite3 *db,            /* Database handle */
  const char *zSql,       /* SQL statement, UTF-8 encoded */
  int nBytes,             /* Length of zSql in bytes. */
  sqlite3_stmt **ppStmt,  /* OUT: Statement handle */
  const char **pzTail     /* OUT: Pointer to unused portion of zSql */
);
int sqlite3_prepare16(
  sqlite3 *db,            /* Database handle */
  const void *zSql,       /* SQL statement, UTF-16 encoded */
  int nBytes,             /* Length of zSql in bytes. */
  sqlite3_stmt **ppStmt,  /* OUT: Statement handle */
  const void **pzTail     /* OUT: Pointer to unused portion of zSql */
);
int sqlite3_prepare16_v2(
  sqlite3 *db,            /* Database handle */
  const void *zSql,       /* SQL statement, UTF-16 encoded */
  int nBytes,             /* Length of zSql in bytes. */
  sqlite3_stmt **ppStmt,  /* OUT: Statement handle */
  const void **pzTail     /* OUT: Pointer to unused portion of zSql */
);

/*
** CAPI3REF:  Dynamically Typed Value Object
**
** SQLite uses dynamic typing for the values it stores.  Values can 
** be integers, floating point values, strings, BLOBs, or NULL.  When
** passing around values internally, each value is represented as
** an instance of the sqlite3_value object.
*/
typedef struct Mem sqlite3_value;

/*
** CAPI3REF:  SQL Function Context Object
**
** The context in which an SQL function executes is stored in an
** sqlite3_context object.  A pointer to such an object is the
** first parameter to user-defined SQL functions.
*/
typedef struct sqlite3_context sqlite3_context;

/*
** CAPI3REF:  Binding Values To Prepared Statements
**
** In the SQL strings input to [sqlite3_prepare_v2()] and its variants,
** one or more literals can be replace by a parameter in one of these
** forms:
**
** <ul>
** <li>  ?
** <li>  ?NNN
** <li>  :AAA
** <li>  @AAA
** <li>  $VVV
** </ul>
**
** In the parameter forms shown above NNN is an integer literal,
** AAA is an alphanumeric identifier and VVV is a variable name according
** to the syntax rules of the TCL programming language.
** The values of these parameters (also called "host parameter names")
** can be set using the sqlite3_bind_*() routines defined here.
**
** The first argument to the sqlite3_bind_*() routines always is a pointer
** to the [sqlite3_stmt] object returned from [sqlite3_prepare_v2()] or
** its variants.  The second
** argument is the index of the parameter to be set.  The first parameter has
** an index of 1. When the same named parameter is used more than once, second
** and subsequent
** occurrences have the same index as the first occurrence.  The index for
** named parameters can be looked up using the
** [sqlite3_bind_parameter_name()] API if desired.  The index for "?NNN"
** parametes is the value of NNN.
** The NNN value must be between 1 and the compile-time
** parameter SQLITE_MAX_VARIABLE_NUMBER (default value: 999).
** See <a href="limits.html">limits.html</a> for additional information.
**
** The third argument is the value to bind to the parameter.
**
** In those
** routines that have a fourth argument, its value is the number of bytes
** in the parameter.  To be clear: the value is the number of bytes in the
** string, not the number of characters.  The number
** of bytes does not include the zero-terminator at the end of strings.
** If the fourth parameter is negative, the length of the string is
** number of bytes up to the first zero terminator.
**
** The fifth argument to sqlite3_bind_blob(), sqlite3_bind_text(), and
** sqlite3_bind_text16() is a destructor used to dispose of the BLOB or
** text after SQLite has finished with it.  If the fifth argument is the
** special value [SQLITE_STATIC], then the library assumes that the information
** is in static, unmanaged space and does not need to be freed.  If the
** fifth argument has the value [SQLITE_TRANSIENT], then SQLite makes its
** own private copy of the data immediately, before the sqlite3_bind_*()
** routine returns.
**
** The sqlite3_bind_zeroblob() routine binds a BLOB of length n that
** is filled with zeros.  A zeroblob uses a fixed amount of memory
** (just an integer to hold it size) while it is being processed.
** Zeroblobs are intended to serve as place-holders for BLOBs whose
** content is later written using 
** [sqlite3_blob_open | increment BLOB I/O] routines.
**
** The sqlite3_bind_*() routines must be called after
** [sqlite3_prepare_v2()] (and its variants) or [sqlite3_reset()] and
** before [sqlite3_step()].
** Bindings are not cleared by the [sqlite3_reset()] routine.
** Unbound parameters are interpreted as NULL.
**
** These routines return [SQLITE_OK] on success or an error code if
** anything goes wrong.  [SQLITE_RANGE] is returned if the parameter
** index is out of range.  [SQLITE_NOMEM] is returned if malloc fails.
** [SQLITE_MISUSE] is returned if these routines are called on a virtual
** machine that is the wrong state or which has already been finalized.
*/
int sqlite3_bind_blob(sqlite3_stmt*, int, const void*, int n, void(*)(void*));
int sqlite3_bind_double(sqlite3_stmt*, int, double);
int sqlite3_bind_int(sqlite3_stmt*, int, int);
int sqlite3_bind_int64(sqlite3_stmt*, int, sqlite_int64);
int sqlite3_bind_null(sqlite3_stmt*, int);
int sqlite3_bind_text(sqlite3_stmt*, int, const char*, int n, void(*)(void*));
int sqlite3_bind_text16(sqlite3_stmt*, int, const void*, int, void(*)(void*));
int sqlite3_bind_value(sqlite3_stmt*, int, const sqlite3_value*);
int sqlite3_bind_zeroblob(sqlite3_stmt*, int, int n);

/*
** CAPI3REF: Number Of Host Parameters
**
** Return the largest host parameter index in the precompiled statement given
** as the argument.  When the host parameters are of the forms like ":AAA"
** or "?", then they are assigned sequential increasing numbers beginning
** with one, so the value returned is the number of parameters.  However
** if the same host parameter name is used multiple times, each occurrance
** is given the same number, so the value returned in that case is the number
** of unique host parameter names.  If host parameters of the form "?NNN"
** are used (where NNN is an integer) then there might be gaps in the
** numbering and the value returned by this interface is the index of the
** host parameter with the largest index value.
*/
int sqlite3_bind_parameter_count(sqlite3_stmt*);

/*
** CAPI3REF: Name Of A Host Parameter
**
** This routine returns a pointer to the name of the n-th parameter in a 
** [sqlite3_stmt | prepared statement].
** Host parameters of the form ":AAA" or "@AAA" or "$VVV" have a name
** which is the string ":AAA" or "@AAA" or "$VVV".  
** In other words, the initial ":" or "$" or "@"
** is included as part of the name.
** Parameters of the form "?" or "?NNN" have no name.
**
** The first bound parameter has an index of 1, not 0.
**
** If the value n is out of range or if the n-th parameter is nameless,
** then NULL is returned.  The returned string is always in the
** UTF-8 encoding even if the named parameter was originally specified
** as UTF-16 in [sqlite3_prepare16()] or [sqlite3_prepare16_v2()].
*/
const char *sqlite3_bind_parameter_name(sqlite3_stmt*, int);

/*
** CAPI3REF: Index Of A Parameter With A Given Name
**
** This routine returns the index of a host parameter with the given name.
** The name must match exactly.  If no parameter with the given name is 
** found, return 0.  Parameter names must be UTF8.
*/
int sqlite3_bind_parameter_index(sqlite3_stmt*, const char *zName);

/*
** CAPI3REF: Reset All Bindings On A Prepared Statement
**
** Contrary to the intuition of many, [sqlite3_reset()] does not
** reset the [sqlite3_bind_blob | bindings] on a 
** [sqlite3_stmt | prepared statement].  Use this routine to
** reset all host parameters to NULL.
*/
int sqlite3_clear_bindings(sqlite3_stmt*);

/*
** CAPI3REF: Number Of Columns In A Result Set
**
** Return the number of columns in the result set returned by the 
** [sqlite3_stmt | compiled SQL statement]. This routine returns 0
** if pStmt is an SQL statement that does not return data (for 
** example an UPDATE).
*/
int sqlite3_column_count(sqlite3_stmt *pStmt);

/*
** CAPI3REF: Column Names In A Result Set
**
** These routines return the name assigned to a particular column
** in the result set of a SELECT statement.  The sqlite3_column_name()
** interface returns a pointer to a UTF8 string and sqlite3_column_name16()
** returns a pointer to a UTF16 string.  The first parameter is the
** [sqlite_stmt | prepared statement] that implements the SELECT statement.
** The second parameter is the column number.  The left-most column is
** number 0.
**
** The returned string pointer is valid until either the 
** [sqlite_stmt | prepared statement] is destroyed by [sqlite3_finalize()]
** or until the next call sqlite3_column_name() or sqlite3_column_name16()
** on the same column.
*/
const char *sqlite3_column_name(sqlite3_stmt*, int N);
const void *sqlite3_column_name16(sqlite3_stmt*, int N);

/*
** CAPI3REF: Source Of Data In A Query Result
**
** These routines provide a means to determine what column of what
** table in which database a result of a SELECT statement comes from.
** The name of the database or table or column can be returned as
** either a UTF8 or UTF16 string.  The returned string is valid until
** the [sqlite3_stmt | prepared statement] is destroyed using
** [sqlite3_finalize()] or until the same information is requested
** again about the same column.
**
** The first argument to the following calls is a 
** [sqlite3_stmt | compiled SQL statement].
** These functions return information about the Nth column returned by 
** the statement, where N is the second function argument.
**
** If the Nth column returned by the statement is an expression
** or subquery and is not a column value, then all of these functions
** return NULL. Otherwise, they return the 
** name of the attached database, table and column that query result
** column was extracted from.
**
** As with all other SQLite APIs, those postfixed with "16" return UTF-16
** encoded strings, the other functions return UTF-8.
**
** These APIs are only available if the library was compiled with the 
** SQLITE_ENABLE_COLUMN_METADATA preprocessor symbol defined.
*/
const char *sqlite3_column_database_name(sqlite3_stmt*,int);
const void *sqlite3_column_database_name16(sqlite3_stmt*,int);
const char *sqlite3_column_table_name(sqlite3_stmt*,int);
const void *sqlite3_column_table_name16(sqlite3_stmt*,int);
const char *sqlite3_column_origin_name(sqlite3_stmt*,int);
const void *sqlite3_column_origin_name16(sqlite3_stmt*,int);

/*
** CAPI3REF: Declared Datatype Of A Query Result
**
** The first parameter is a [sqlite3_stmt | compiled SQL statement]. 
** If this statement is a SELECT statement and the Nth column of the 
** returned result set  of that SELECT is a table column (not an
** expression or subquery) then the declared type of the table
** column is returned. If the Nth column of the result set is an
** expression or subquery, then a NULL pointer is returned.
** The returned string is always UTF-8 encoded. For example, in
** the database schema:
**
** CREATE TABLE t1(c1 VARIANT);
**
** And the following statement compiled:
**
** SELECT c1 + 1, c1 FROM t1;
**
** Then this routine would return the string "VARIANT" for the second
** result column (i==1), and a NULL pointer for the first result column
** (i==0).
**
** SQLite uses dynamic run-time typing.  So just because a column
** is declared to contain a particular type does not mean that the
** data stored in that column is of the declared type.  SQLite is
** strongly typed, but the typing is dynamic not static.  Type
** is associated with individual values, not with the containers
** used to hold those values.
*/
const char *sqlite3_column_decltype(sqlite3_stmt *, int i);
const void *sqlite3_column_decltype16(sqlite3_stmt*,int);

/* 
** CAPI3REF:  Evaluate An SQL Statement
**
** After an [sqlite3_stmt | SQL statement] has been prepared with a call
** to either [sqlite3_prepare_v2()] or [sqlite3_prepare16_v2()] or to one of
** the legacy interfaces [sqlite3_prepare()] or [sqlite3_prepare16()],
** then this function must be called one or more times to evaluate the 
** statement.
**
** The details of the behavior of this sqlite3_step() interface depend
** on whether the statement was prepared using the newer "v2" interface
** [sqlite3_prepare_v2()] and [sqlite3_prepare16_v2()] or the older legacy
** interface [sqlite3_prepare()] and [sqlite3_prepare16()].  The use of the
** new "v2" interface is recommended for new applications but the legacy
** interface will continue to be supported.
**
** In the lagacy interface, the return value will be either [SQLITE_BUSY], 
** [SQLITE_DONE], [SQLITE_ROW], [SQLITE_ERROR], or [SQLITE_MISUSE].
** With the "v2" interface, any of the other [SQLITE_OK | result code]
** or [SQLITE_IOERR_READ | extended result code] might be returned as
** well.
**
** [SQLITE_BUSY] means that the database engine was unable to acquire the
** database locks it needs to do its job.  If the statement is a COMMIT
** or occurs outside of an explicit transaction, then you can retry the
** statement.  If the statement is not a COMMIT and occurs within a
** explicit transaction then you should rollback the transaction before
** continuing.
**
** [SQLITE_DONE] means that the statement has finished executing
** successfully.  sqlite3_step() should not be called again on this virtual
** machine without first calling [sqlite3_reset()] to reset the virtual
** machine back to its initial state.
**
** If the SQL statement being executed returns any data, then 
** [SQLITE_ROW] is returned each time a new row of data is ready
** for processing by the caller. The values may be accessed using
** the [sqlite3_column_int | column access functions].
** sqlite3_step() is called again to retrieve the next row of data.
** 
** [SQLITE_ERROR] means that a run-time error (such as a constraint
** violation) has occurred.  sqlite3_step() should not be called again on
** the VM. More information may be found by calling [sqlite3_errmsg()].
** With the legacy interface, a more specific error code (example:
** [SQLITE_INTERRUPT], [SQLITE_SCHEMA], [SQLITE_CORRUPT], and so forth)
** can be obtained by calling [sqlite3_reset()] on the
** [sqlite_stmt | prepared statement].  In the "v2" interface,
** the more specific error code is returned directly by sqlite3_step().
**
** [SQLITE_MISUSE] means that the this routine was called inappropriately.
** Perhaps it was called on a [sqlite_stmt | prepared statement] that has
** already been [sqlite3_finalize | finalized] or on one that had 
** previously returned [SQLITE_ERROR] or [SQLITE_DONE].  Or it could
** be the case that the same database connection is being used by two or
** more threads at the same moment in time.
**
** <b>Goofy Interface Alert:</b>
** In the legacy interface, 
** the sqlite3_step() API always returns a generic error code,
** [SQLITE_ERROR], following any error other than [SQLITE_BUSY]
** and [SQLITE_MISUSE].  You must call [sqlite3_reset()] or
** [sqlite3_finalize()] in order to find one of the specific
** [SQLITE_ERROR | result codes] that better describes the error.
** We admit that this is a goofy design.  The problem has been fixed
** with the "v2" interface.  If you prepare all of your SQL statements
** using either [sqlite3_prepare_v2()] or [sqlite3_prepare16_v2()] instead
** of the legacy [sqlite3_prepare()] and [sqlite3_prepare16()], then the 
** more specific [SQLITE_ERROR | result codes] are returned directly
** by sqlite3_step().  The use of the "v2" interface is recommended.
*/
int sqlite3_step(sqlite3_stmt*);

/*
** CAPI3REF:
**
** Return the number of values in the current row of the result set.
**
** After a call to [sqlite3_step()] that returns [SQLITE_ROW], this routine
** will return the same value as the [sqlite3_column_count()] function.
** After [sqlite3_step()] has returned an [SQLITE_DONE], [SQLITE_BUSY], or
** a [SQLITE_ERROR | error code], or before [sqlite3_step()] has been 
** called on the [sqlite_stmt | prepared statement] for the first time,
** this routine returns zero.
*/
int sqlite3_data_count(sqlite3_stmt *pStmt);

/*
** CAPI3REF: Fundamental Datatypes
**
** Every value in SQLite has one of five fundamental datatypes:
**
** <ul>
** <li> 64-bit signed integer
** <li> 64-bit IEEE floating point number
** <li> string
** <li> BLOB
** <li> NULL
** </ul>
**
** These constants are codes for each of those types.
**
** Note that the SQLITE_TEXT constant was also used in SQLite version 2
** for a completely different meaning.  Software that links against both
** SQLite version 2 and SQLite version 3 should use SQLITE3_TEXT not
** SQLITE_TEXT.
*/
#define SQLITE_INTEGER  1
#define SQLITE_FLOAT    2
#define SQLITE_BLOB     4
#define SQLITE_NULL     5
#ifdef SQLITE_TEXT
# undef SQLITE_TEXT
#else
# define SQLITE_TEXT     3
#endif
#define SQLITE3_TEXT     3

/*
** CAPI3REF: Results Values From A Query
**
** These routines return information about the information
** in a single column of the current result row of a query.  In every
** case the first argument is a pointer to the 
** [sqlite3_stmt | SQL statement] that is being
** evaluate (the [sqlite_stmt*] that was returned from 
** [sqlite3_prepare_v2()] or one of its variants) and
** the second argument is the index of the column for which information 
** should be returned.  The left-most column has an index of 0.
**
** If the SQL statement is not currently point to a valid row, or if the
** the column index is out of range, the result is undefined.
**
** The sqlite3_column_type() routine returns 
** [SQLITE_INTEGER | datatype code] for the initial data type
** of the result column.  The returned value is one of [SQLITE_INTEGER],
** [SQLITE_FLOAT], [SQLITE_TEXT], [SQLITE_BLOB], or [SQLITE_NULL].  The value
** returned by sqlite3_column_type() is only meaningful if no type
** conversions have occurred as described below.  After a type conversion,
** the value returned by sqlite3_column_type() is undefined.  Future
** versions of SQLite may change the behavior of sqlite3_column_type()
** following a type conversion.
**
*** The sqlite3_column_nm
**
** If the result is a BLOB or UTF-8 string then the sqlite3_column_bytes() 
** routine returns the number of bytes in that BLOB or string.
** If the result is a UTF-16 string, then sqlite3_column_bytes() converts
** the string to UTF-8 and then returns the number of bytes.
** If the result is a numeric value then sqlite3_column_bytes() uses
** [sqlite3_snprintf()] to convert that value to a UTF-8 string and returns
** the number of bytes in that string.
** The value returned does not include the zero terminator at the end
** of the string.  For clarity: the value returned is the number of
** bytes in the string, not the number of characters.
**
** The sqlite3_column_bytes16() routine is similar to sqlite3_column_bytes()
** but leaves the result in UTF-16 instead of UTF-8.  
** The zero terminator is not included in this count.
**
** These routines attempt to convert the value where appropriate.  For
** example, if the internal representation is FLOAT and a text result
** is requested, [sqlite3_snprintf()] is used internally to do the conversion
** automatically.  The following table details the conversions that
** are applied:
**
** <blockquote>
** <table border="1">
** <tr><th> Internal <th> Requested <th> 
** <tr><th>  Type    <th>    Type   <th> Conversion
**
** <tr><td>  NULL    <td> INTEGER   <td> Result is 0
** <tr><td>  NULL    <td>  FLOAT    <td> Result is 0.0
** <tr><td>  NULL    <td>   TEXT    <td> Result is NULL pointer
** <tr><td>  NULL    <td>   BLOB    <td> Result is NULL pointer
** <tr><td> INTEGER  <td>  FLOAT    <td> Convert from integer to float
** <tr><td> INTEGER  <td>   TEXT    <td> ASCII rendering of the integer
** <tr><td> INTEGER  <td>   BLOB    <td> Same as for INTEGER->TEXT
** <tr><td>  FLOAT   <td> INTEGER   <td> Convert from float to integer
** <tr><td>  FLOAT   <td>   TEXT    <td> ASCII rendering of the float
** <tr><td>  FLOAT   <td>   BLOB    <td> Same as FLOAT->TEXT
** <tr><td>  TEXT    <td> INTEGER   <td> Use atoi()
** <tr><td>  TEXT    <td>  FLOAT    <td> Use atof()
** <tr><td>  TEXT    <td>   BLOB    <td> No change
** <tr><td>  BLOB    <td> INTEGER   <td> Convert to TEXT then use atoi()
** <tr><td>  BLOB    <td>  FLOAT    <td> Convert to TEXT then use atof()
** <tr><td>  BLOB    <td>   TEXT    <td> Add a zero terminator if needed
** </table>
** </blockquote>
**
** The table above makes reference to standard C library functions atoi()
** and atof().  SQLite does not really use these functions.  It has its
** on equavalent internal routines.  The atoi() and atof() names are
** used in the table for brevity and because they are familiar to most
** C programmers.
**
** Note that when type conversions occur, pointers returned by prior
** calls to sqlite3_column_blob(), sqlite3_column_text(), and/or
** sqlite3_column_text16() may be invalidated. 
** Type conversions and pointer invalidations might occur
** in the following cases:
**
** <ul>
** <li><p>  The initial content is a BLOB and sqlite3_column_text() 
**          or sqlite3_column_text16() is called.  A zero-terminator might
**          need to be added to the string.</p></li>
**
** <li><p>  The initial content is UTF-8 text and sqlite3_column_bytes16() or
**          sqlite3_column_text16() is called.  The content must be converted
**          to UTF-16.</p></li>
**
** <li><p>  The initial content is UTF-16 text and sqlite3_column_bytes() or
**          sqlite3_column_text() is called.  The content must be converted
**          to UTF-8.</p></li>
** </ul>
**
** Conversions between UTF-16be and UTF-16le are always done in place and do
** not invalidate a prior pointer, though of course the content of the buffer
** that the prior pointer points to will have been modified.  Other kinds
** of conversion are done in place when it is possible, but sometime it is
** not possible and in those cases prior pointers are invalidated.  
**
** The safest and easiest to remember policy is to invoke these routines
** in one of the following ways:
**
**  <ul>
**  <li>sqlite3_column_text() followed by sqlite3_column_bytes()</li>
**  <li>sqlite3_column_blob() followed by sqlite3_column_bytes()</li>
**  <li>sqlite3_column_text16() followed by sqlite3_column_bytes16()</li>
**  </ul>
**
** In other words, you should call sqlite3_column_text(), sqlite3_column_blob(),
** or sqlite3_column_text16() first to force the result into the desired
** format, then invoke sqlite3_column_bytes() or sqlite3_column_bytes16() to
** find the size of the result.  Do not mix call to sqlite3_column_text() or
** sqlite3_column_blob() with calls to sqlite3_column_bytes16().  And do not
** mix calls to sqlite3_column_text16() with calls to sqlite3_column_bytes().
*/
const void *sqlite3_column_blob(sqlite3_stmt*, int iCol);
int sqlite3_column_bytes(sqlite3_stmt*, int iCol);
int sqlite3_column_bytes16(sqlite3_stmt*, int iCol);
double sqlite3_column_double(sqlite3_stmt*, int iCol);
int sqlite3_column_int(sqlite3_stmt*, int iCol);
sqlite_int64 sqlite3_column_int64(sqlite3_stmt*, int iCol);
const unsigned char *sqlite3_column_text(sqlite3_stmt*, int iCol);
const void *sqlite3_column_text16(sqlite3_stmt*, int iCol);
int sqlite3_column_type(sqlite3_stmt*, int iCol);
sqlite3_value *sqlite3_column_value(sqlite3_stmt*, int iCol);

/*
** CAPI3REF: Destroy A Prepared Statement Object
**
** The sqlite3_finalize() function is called to delete a 
** [sqlite3_stmt | compiled SQL statement]. If the statement was
** executed successfully, or not executed at all, then SQLITE_OK is returned.
** If execution of the statement failed then an 
** [SQLITE_ERROR | error code] or [SQLITE_IOERR_READ | extended error code]
** is returned. 
**
** This routine can be called at any point during the execution of the
** [sqlite3_stmt | virtual machine].  If the virtual machine has not 
** completed execution when this routine is called, that is like
** encountering an error or an interrupt.  (See [sqlite3_interrupt()].) 
** Incomplete updates may be rolled back and transactions cancelled,  
** depending on the circumstances, and the 
** [SQLITE_ERROR | result code] returned will be [SQLITE_ABORT].
*/
int sqlite3_finalize(sqlite3_stmt *pStmt);

/*
** CAPI3REF: Reset A Prepared Statement Object
**
** The sqlite3_reset() function is called to reset a 
** [sqlite_stmt | compiled SQL statement] object.
** back to it's initial state, ready to be re-executed.
** Any SQL statement variables that had values bound to them using
** the [sqlite3_bind_blob | sqlite3_bind_*() API] retain their values.
** Use [sqlite3_clear_bindings()] to reset the bindings.
*/
int sqlite3_reset(sqlite3_stmt *pStmt);

/*
** CAPI3REF: Create Or Redefine SQL Functions
**
** The following two functions are used to add SQL functions or aggregates
** or to redefine the behavior of existing SQL functions or aggregates.  The
** difference only between the two is that the second parameter, the
** name of the (scalar) function or aggregate, is encoded in UTF-8 for
** sqlite3_create_function() and UTF-16 for sqlite3_create_function16().
**
** The first argument is the [sqlite3 | database handle] that holds the
** SQL function or aggregate is to be added or redefined. If a single
** program uses more than one database handle internally, then SQL
** functions or aggregates must be added individually to each database
** handle with which they will be used.
**
** The second parameter is the name of the SQL function to be created
** or redefined.
** The length of the name is limited to 255 bytes, exclusive of the 
** zero-terminator.  Note that the name length limit is in bytes, not
** characters.  Any attempt to create a function with a longer name
** will result in an SQLITE_ERROR error.
**
** The third parameter is the number of arguments that the SQL function or
** aggregate takes. If this parameter is negative, then the SQL function or
** aggregate may take any number of arguments.
**
** The fourth parameter, eTextRep, specifies what 
** [SQLITE_UTF8 | text encoding] this SQL function prefers for
** its parameters.  Any SQL function implementation should be able to work
** work with UTF-8, UTF-16le, or UTF-16be.  But some implementations may be
** more efficient with one encoding than another.  It is allowed to
** invoke sqlite_create_function() or sqlite3_create_function16() multiple
** times with the same function but with different values of eTextRep.
** When multiple implementations of the same function are available, SQLite
** will pick the one that involves the least amount of data conversion.
** If there is only a single implementation which does not care what
** text encoding is used, then the fourth argument should be
** [SQLITE_ANY].
**
** The fifth parameter is an arbitrary pointer.  The implementation
** of the function can gain access to this pointer using
** [sqlite_user_data()].
**
** The seventh, eighth and ninth parameters, xFunc, xStep and xFinal, are
** pointers to C-language functions that implement the SQL
** function or aggregate. A scalar SQL function requires an implementation of
** the xFunc callback only, NULL pointers should be passed as the xStep
** and xFinal parameters. An aggregate SQL function requires an implementation
** of xStep and xFinal and NULL should be passed for xFunc. To delete an
** existing SQL function or aggregate, pass NULL for all three function
** callback.
**
** It is permitted to register multiple implementations of the same
** functions with the same name but with either differing numbers of
** arguments or differing perferred text encodings.  SQLite will use
** the implementation most closely matches the way in which the
** SQL function is used.
*/
int sqlite3_create_function(
  sqlite3 *,
  const char *zFunctionName,
  int nArg,
  int eTextRep,
  void*,
  void (*xFunc)(sqlite3_context*,int,sqlite3_value**),
  void (*xStep)(sqlite3_context*,int,sqlite3_value**),
  void (*xFinal)(sqlite3_context*)
);
int sqlite3_create_function16(
  sqlite3*,
  const void *zFunctionName,
  int nArg,
  int eTextRep,
  void*,
  void (*xFunc)(sqlite3_context*,int,sqlite3_value**),
  void (*xStep)(sqlite3_context*,int,sqlite3_value**),
  void (*xFinal)(sqlite3_context*)
);

/*
** CAPI3REF: Text Encodings
**
** These constant define integer codes that represent the various
** text encodings supported by SQLite.
*/
#define SQLITE_UTF8           1
#define SQLITE_UTF16LE        2
#define SQLITE_UTF16BE        3
#define SQLITE_UTF16          4    /* Use native byte order */
#define SQLITE_ANY            5    /* sqlite3_create_function only */
#define SQLITE_UTF16_ALIGNED  8    /* sqlite3_create_collation only */

/*
** CAPI3REF: Obsolete Functions
**
** These functions are all now obsolete.  In order to maintain
** backwards compatibility with older code, we continue to support
** these functions.  However, new development projects should avoid
** the use of these functions.  To help encourage people to avoid
** using these functions, we are not going to tell you want they do.
*/
int sqlite3_aggregate_count(sqlite3_context*);
int sqlite3_expired(sqlite3_stmt*);
int sqlite3_transfer_bindings(sqlite3_stmt*, sqlite3_stmt*);
int sqlite3_global_recover(void);


/*
** CAPI3REF: Obtaining SQL Function Parameter Values
**
** The C-language implementation of SQL functions and aggregates uses
** this set of interface routines to access the parameter values on
** the function or aggregate.
**
** The xFunc (for scalar functions) or xStep (for aggregates) parameters
** to [sqlite3_create_function()] and [sqlite3_create_function16()]
** define callbacks that implement the SQL functions and aggregates.
** The 4th parameter to these callbacks is an array of pointers to
** [sqlite3_value] objects.  There is one [sqlite3_value] object for
** each parameter to the SQL function.  These routines are used to
** extract values from the [sqlite3_value] objects.
**
** These routines work just like the corresponding 
** [sqlite3_column_blob | sqlite3_column_* routines] except that 
** these routines take a single [sqlite3_value*] pointer instead
** of an [sqlite3_stmt*] pointer and an integer column number.
**
** The sqlite3_value_text16() interface extracts a UTF16 string
** in the native byte-order of the host machine.  The
** sqlite3_value_text16be() and sqlite3_value_text16le() interfaces
** extract UTF16 strings as big-endian and little-endian respectively.
**
** The sqlite3_value_numeric_type() interface attempts to apply
** numeric affinity to the value.  This means that an attempt is
** made to convert the value to an integer or floating point.  If
** such a conversion is possible without loss of information (in order
** words if the value is original a string that looks like a number)
** then it is done.  Otherwise no conversion occurs.  The 
** [SQLITE_INTEGER | datatype] after conversion is returned.
**
** Please pay particular attention to the fact that the pointer that
** is returned from [sqlite3_value_blob()], [sqlite3_value_text()], or
** [sqlite3_value_text16()] can be invalidated by a subsequent call to
** [sqlite3_value_bytes()], [sqlite3_value_bytes16()], [sqlite_value_text()],
** or [sqlite3_value_text16()].  
*/
const void *sqlite3_value_blob(sqlite3_value*);
int sqlite3_value_bytes(sqlite3_value*);
int sqlite3_value_bytes16(sqlite3_value*);
double sqlite3_value_double(sqlite3_value*);
int sqlite3_value_int(sqlite3_value*);
sqlite_int64 sqlite3_value_int64(sqlite3_value*);
const unsigned char *sqlite3_value_text(sqlite3_value*);
const void *sqlite3_value_text16(sqlite3_value*);
const void *sqlite3_value_text16le(sqlite3_value*);
const void *sqlite3_value_text16be(sqlite3_value*);
int sqlite3_value_type(sqlite3_value*);
int sqlite3_value_numeric_type(sqlite3_value*);

/*
** CAPI3REF: Obtain Aggregate Function Context
**
** The implementation of aggregate SQL functions use this routine to allocate
** a structure for storing their state.  The first time this routine
** is called for a particular aggregate, a new structure of size nBytes
** is allocated, zeroed, and returned.  On subsequent calls (for the
** same aggregate instance) the same buffer is returned.  The implementation
** of the aggregate can use the returned buffer to accumulate data.
**
** The buffer allocated is freed automatically by SQLite whan the aggregate
** query concludes.
**
** The first parameter should be a copy of the 
** [sqlite3_context | SQL function context] that is the first
** parameter to the callback routine that implements the aggregate
** function.
*/
void *sqlite3_aggregate_context(sqlite3_context*, int nBytes);

/*
** CAPI3REF: User Data For Functions
**
** The pUserData parameter to the [sqlite3_create_function()]
** and [sqlite3_create_function16()] routines
** used to register user functions is available to
** the implementation of the function using this call.
*/
void *sqlite3_user_data(sqlite3_context*);

/*
** CAPI3REF: Function Auxiliary Data
**
** The following two functions may be used by scalar SQL functions to
** associate meta-data with argument values. If the same value is passed to
** multiple invocations of the same SQL function during query execution, under
** some circumstances the associated meta-data may be preserved. This may
** be used, for example, to add a regular-expression matching scalar
** function. The compiled version of the regular expression is stored as
** meta-data associated with the SQL value passed as the regular expression
** pattern.  The compiled regular expression can be reused on multiple
** invocations of the same function so that the original pattern string
** does not need to be recompiled on each invocation.
**
** The sqlite3_get_auxdata() interface returns a pointer to the meta-data
** associated with the Nth argument value to the current SQL function
** call, where N is the second parameter. If no meta-data has been set for
** that value, then a NULL pointer is returned.
**
** The sqlite3_set_auxdata() is used to associate meta-data with an SQL
** function argument. The third parameter is a pointer to the meta-data
** to be associated with the Nth user function argument value. The fourth
** parameter specifies a destructor that will be called on the meta-
** data pointer to release it when it is no longer required. If the 
** destructor is NULL, it is not invoked.
**
** In practice, meta-data is preserved between function calls for
** expressions that are constant at compile time. This includes literal
** values and SQL variables.
*/
void *sqlite3_get_auxdata(sqlite3_context*, int);
void sqlite3_set_auxdata(sqlite3_context*, int, void*, void (*)(void*));


/*
** CAPI3REF: Constants Defining Special Destructor Behavior
**
** These are special value for the destructor that is passed in as the
** final argument to routines like [sqlite3_result_blob()].  If the destructor
** argument is SQLITE_STATIC, it means that the content pointer is constant
** and will never change.  It does not need to be destroyed.  The 
** SQLITE_TRANSIENT value means that the content will likely change in
** the near future and that SQLite should make its own private copy of
** the content before returning.
**
** The typedef is necessary to work around problems in certain
** C++ compilers.  See ticket #2191.
*/
typedef void (*sqlite3_destructor_type)(void*);
#define SQLITE_STATIC      ((sqlite3_destructor_type)0)
#define SQLITE_TRANSIENT   ((sqlite3_destructor_type)-1)

/*
** CAPI3REF: Setting The Result Of An SQL Function
**
** These routines are used by the xFunc or xFinal callbacks that
** implement SQL functions and aggregates.  See
** [sqlite3_create_function()] and [sqlite3_create_function16()]
** for additional information.
**
** These functions work very much like the 
** [sqlite3_bind_blob | sqlite3_bind_*] family of functions used
** to bind values to host parameters in prepared statements.
** Refer to the
** [sqlite3_bind_blob | sqlite3_bind_* documentation] for
** additional information.
**
** The sqlite3_result_error() and sqlite3_result_error16() functions
** cause the implemented SQL function to throw an exception.  The
** parameter to sqlite3_result_error() or sqlite3_result_error16()
** is the text of an error message.
**
** The sqlite3_result_toobig() cause the function implementation
** to throw and error indicating that a string or BLOB is to long
** to represent.
*/
void sqlite3_result_blob(sqlite3_context*, const void*, int, void(*)(void*));
void sqlite3_result_double(sqlite3_context*, double);
void sqlite3_result_error(sqlite3_context*, const char*, int);
void sqlite3_result_error16(sqlite3_context*, const void*, int);
void sqlite3_result_error_toobig(sqlite3_context*);
void sqlite3_result_int(sqlite3_context*, int);
void sqlite3_result_int64(sqlite3_context*, sqlite_int64);
void sqlite3_result_null(sqlite3_context*);
void sqlite3_result_text(sqlite3_context*, const char*, int, void(*)(void*));
void sqlite3_result_text16(sqlite3_context*, const void*, int, void(*)(void*));
void sqlite3_result_text16le(sqlite3_context*, const void*, int,void(*)(void*));
void sqlite3_result_text16be(sqlite3_context*, const void*, int,void(*)(void*));
void sqlite3_result_value(sqlite3_context*, sqlite3_value*);
void sqlite3_result_zeroblob(sqlite3_context*, int n);

/*
** CAPI3REF: Define New Collating Sequences
**
** These functions are used to add new collation sequences to the
** [sqlite3*] handle specified as the first argument. 
**
** The name of the new collation sequence is specified as a UTF-8 string
** for sqlite3_create_collation() and sqlite3_create_collation_v2()
** and a UTF-16 string for sqlite3_create_collation16().  In all cases
** the name is passed as the second function argument.
**
** The third argument must be one of the constants [SQLITE_UTF8],
** [SQLITE_UTF16LE] or [SQLITE_UTF16BE], indicating that the user-supplied
** routine expects to be passed pointers to strings encoded using UTF-8,
** UTF-16 little-endian or UTF-16 big-endian respectively.
**
** A pointer to the user supplied routine must be passed as the fifth
** argument. If it is NULL, this is the same as deleting the collation
** sequence (so that SQLite cannot call it anymore). Each time the user
** supplied function is invoked, it is passed a copy of the void* passed as
** the fourth argument to sqlite3_create_collation() or
** sqlite3_create_collation16() as its first parameter.
**
** The remaining arguments to the user-supplied routine are two strings,
** each represented by a [length, data] pair and encoded in the encoding
** that was passed as the third argument when the collation sequence was
** registered. The user routine should return negative, zero or positive if
** the first string is less than, equal to, or greater than the second
** string. i.e. (STRING1 - STRING2).
**
** The sqlite3_create_collation_v2() works like sqlite3_create_collation()
** excapt that it takes an extra argument which is a destructor for
** the collation.  The destructor is called when the collation is
** destroyed and is passed a copy of the fourth parameter void* pointer
** of the sqlite3_create_collation_v2().  Collations are destroyed when
** they are overridden by later calls to the collation creation functions
** or when the [sqlite3*] database handle is closed using [sqlite3_close()].
**
** The sqlite3_create_collation_v2() interface is experimental and
** subject to change in future releases.  The other collation creation
** functions are stable.
*/
int sqlite3_create_collation(
  sqlite3*, 
  const char *zName, 
  int eTextRep, 
  void*,
  int(*xCompare)(void*,int,const void*,int,const void*)
);
int sqlite3_create_collation_v2(
  sqlite3*, 
  const char *zName, 
  int eTextRep, 
  void*,
  int(*xCompare)(void*,int,const void*,int,const void*),
  void(*xDestroy)(void*)
);
int sqlite3_create_collation16(
  sqlite3*, 
  const char *zName, 
  int eTextRep, 
  void*,
  int(*xCompare)(void*,int,const void*,int,const void*)
);

/*
** CAPI3REF: Collation Needed Callbacks
**
** To avoid having to register all collation sequences before a database
** can be used, a single callback function may be registered with the
** database handle to be called whenever an undefined collation sequence is
** required.
**
** If the function is registered using the sqlite3_collation_needed() API,
** then it is passed the names of undefined collation sequences as strings
** encoded in UTF-8. If sqlite3_collation_needed16() is used, the names
** are passed as UTF-16 in machine native byte order. A call to either
** function replaces any existing callback.
**
** When the callback is invoked, the first argument passed is a copy
** of the second argument to sqlite3_collation_needed() or
** sqlite3_collation_needed16(). The second argument is the database
** handle. The third argument is one of [SQLITE_UTF8], [SQLITE_UTF16BE], or
** [SQLITE_UTF16LE], indicating the most desirable form of the collation
** sequence function required. The fourth parameter is the name of the
** required collation sequence.
**
** The callback function should register the desired collation using
** [sqlite3_create_collation()], [sqlite3_create_collation16()], or
** [sqlite3_create_collation_v2()].
*/
int sqlite3_collation_needed(
  sqlite3*, 
  void*, 
  void(*)(void*,sqlite3*,int eTextRep,const char*)
);
int sqlite3_collation_needed16(
  sqlite3*, 
  void*,
  void(*)(void*,sqlite3*,int eTextRep,const void*)
);

/*
** Specify the key for an encrypted database.  This routine should be
** called right after sqlite3_open().
**
** The code to implement this API is not available in the public release
** of SQLite.
*/
int sqlite3_key(
  sqlite3 *db,                   /* Database to be rekeyed */
  const void *pKey, int nKey     /* The key */
);

/*
** Change the key on an open database.  If the current database is not
** encrypted, this routine will encrypt it.  If pNew==0 or nNew==0, the
** database is decrypted.
**
** The code to implement this API is not available in the public release
** of SQLite.
*/
int sqlite3_rekey(
  sqlite3 *db,                   /* Database to be rekeyed */
  const void *pKey, int nKey     /* The new key */
);

/*
** CAPI3REF:  Suspend Execution For A Short Time
**
** This function causes the current thread to suspect execution
** a number of milliseconds specified in its parameter.
**
** If the operating system does not support sleep requests with 
** millisecond time resolution, then the time will be rounded up to 
** the nearest second. The number of milliseconds of sleep actually 
** requested from the operating system is returned.
*/
int sqlite3_sleep(int);

/*
** CAPI3REF:  Name Of The Folder Holding Temporary Files
**
** If this global variable is made to point to a string which is
** the name of a folder (a.ka. directory), then all temporary files
** created by SQLite will be placed in that directory.  If this variable
** is NULL pointer, then SQLite does a search for an appropriate temporary
** file directory.
**
** Once [sqlite3_open()] has been called, changing this variable will
** invalidate the current temporary database, if any.  Generally speaking,
** it is not safe to invoke this routine after [sqlite3_open()] has
** been called.
*/
extern char *sqlite3_temp_directory;

/*
** CAPI3REF:  Test To See If The Databse Is In Auto-Commit Mode
**
** Test to see whether or not the database connection is in autocommit
** mode.  Return TRUE if it is and FALSE if not.  Autocommit mode is on
** by default.  Autocommit is disabled by a BEGIN statement and reenabled
** by the next COMMIT or ROLLBACK.
*/
int sqlite3_get_autocommit(sqlite3*);

/*
** CAPI3REF:  Find The Database Handle Associated With A Prepared Statement
**
** Return the [sqlite3*] database handle to which a
** [sqlite3_stmt | prepared statement] belongs.
** This is the same database handle that was
** the first argument to the [sqlite3_prepare_v2()] or its variants
** that was used to create the statement in the first place.
*/
sqlite3 *sqlite3_db_handle(sqlite3_stmt*);


/*
** CAPI3REF: Commit And Rollback Notification Callbacks
**
** These routines
** register callback functions to be invoked whenever a transaction
** is committed or rolled back.  The pArg argument is passed through
** to the callback.  If the callback on a commit hook function 
** returns non-zero, then the commit is converted into a rollback.
**
** If another function was previously registered, its pArg value is returned.
** Otherwise NULL is returned.
**
** Registering a NULL function disables the callback.
**
** For the purposes of this API, a transaction is said to have been 
** rolled back if an explicit "ROLLBACK" statement is executed, or
** an error or constraint causes an implicit rollback to occur. The 
** callback is not invoked if a transaction is automatically rolled
** back because the database connection is closed.
**
** These are experimental interfaces and are subject to change.
*/
void *sqlite3_commit_hook(sqlite3*, int(*)(void*), void*);
void *sqlite3_rollback_hook(sqlite3*, void(*)(void *), void*);

/*
** CAPI3REF: Data Change Notification Callbacks
**
** Register a callback function with the database connection identified by the 
** first argument to be invoked whenever a row is updated, inserted or deleted.
** Any callback set by a previous call to this function for the same 
** database connection is overridden.
**
** The second argument is a pointer to the function to invoke when a 
** row is updated, inserted or deleted. The first argument to the callback is
** a copy of the third argument to sqlite3_update_hook(). The second callback 
** argument is one of SQLITE_INSERT, SQLITE_DELETE or SQLITE_UPDATE, depending
** on the operation that caused the callback to be invoked. The third and 
** fourth arguments to the callback contain pointers to the database and 
** table name containing the affected row. The final callback parameter is 
** the rowid of the row. In the case of an update, this is the rowid after 
** the update takes place.
**
** The update hook is not invoked when internal system tables are
** modified (i.e. sqlite_master and sqlite_sequence).
**
** If another function was previously registered, its pArg value is returned.
** Otherwise NULL is returned.
*/
void *sqlite3_update_hook(
  sqlite3*, 
  void(*)(void *,int ,char const *,char const *,sqlite_int64),
  void*
);

/*
** CAPI3REF:  Enable Or Disable Shared Pager Cache
**
** This routine enables or disables the sharing of the database cache
** and schema data structures between connections to the same database.
** Sharing is enabled if the argument is true and disabled if the argument
** is false.
**
** Cache sharing is enabled and disabled on a thread-by-thread basis.
** Each call to this routine enables or disables cache sharing only for
** connections created in the same thread in which this routine is called.
** There is no mechanism for sharing cache between database connections
** running in different threads.
**
** Sharing must be disabled prior to shutting down a thread or else
** the thread will leak memory.  Call this routine with an argument of
** 0 to turn off sharing.  Or use the sqlite3_thread_cleanup() API.
**
** This routine must not be called when any database connections
** are active in the current thread.  Enabling or disabling shared
** cache while there are active database connections will result
** in memory corruption.
**
** When the shared cache is enabled, the
** following routines must always be called from the same thread:
** [sqlite3_open()], [sqlite3_prepare_v2()], [sqlite3_step()],
** [sqlite3_reset()], [sqlite3_finalize()], and [sqlite3_close()].
** This is due to the fact that the shared cache makes use of
** thread-specific storage so that it will be available for sharing
** with other connections.
**
** Virtual tables cannot be used with a shared cache.  When shared
** cache is enabled, the sqlite3_create_module() API used to register
** virtual tables will always return an error.
**
** This routine returns [SQLITE_OK] if shared cache was
** enabled or disabled successfully.  An [SQLITE_ERROR | error code]
** is returned otherwise.
**
** Shared cache is disabled by default for backward compatibility.
*/
int sqlite3_enable_shared_cache(int);

/*
** CAPI3REF:  Attempt To Free Heap Memory
**
** Attempt to free N bytes of heap memory by deallocating non-essential
** memory allocations held by the database library (example: memory 
** used to cache database pages to improve performance).
**
** This function is not a part of standard builds.  It is only created
** if SQLite is compiled with the SQLITE_ENABLE_MEMORY_MANAGEMENT macro.
*/
int sqlite3_release_memory(int);

/*
** CAPI3REF:  Impose A Limit On Heap Size
**
** Place a "soft" limit on the amount of heap memory that may be allocated by
** SQLite within the current thread. If an internal allocation is requested 
** that would exceed the specified limit, [sqlite3_release_memory()] is invoked
** one or more times to free up some space before the allocation is made.
**
** The limit is called "soft", because if [sqlite3_release_memory()] cannot free
** sufficient memory to prevent the limit from being exceeded, the memory is
** allocated anyway and the current operation proceeds.
**
** Prior to shutting down a thread sqlite3_soft_heap_limit() must be set to 
** zero (the default) or else the thread will leak memory. Alternatively, use
** the [sqlite3_thread_cleanup()] API.
**
** A negative or zero value for N means that there is no soft heap limit and
** [sqlite3_release_memory()] will only be called when memory is exhaused.
** The default value for the soft heap limit is zero.
**
** SQLite makes a best effort to honor the soft heap limit.  But if it
** is unable to reduce memory usage below the soft limit, execution will
** continue without error or notification.  This is why the limit is 
** called a "soft" limit.  It is advisory only.
**
** This function is only available if the library was compiled with the 
** SQLITE_ENABLE_MEMORY_MANAGEMENT option set.
** memory-management has been enabled.
*/
void sqlite3_soft_heap_limit(int);

/*
** CAPI3REF:  Clean Up Thread Local Storage
**
** This routine makes sure that all thread-local storage has been
** deallocated for the current thread.
**
** This routine is not technically necessary.  All thread-local storage
** will be automatically deallocated once memory-management and
** shared-cache are disabled and the soft heap limit has been set
** to zero.  This routine is provided as a convenience for users who
** want to make absolutely sure they have not forgotten something
** prior to killing off a thread.
*/
void sqlite3_thread_cleanup(void);

/*
** CAPI3REF:  Extract Metadata About A Column Of A Table
**
** This routine
** returns meta-data about a specific column of a specific database
** table accessible using the connection handle passed as the first function 
** argument.
**
** The column is identified by the second, third and fourth parameters to 
** this function. The second parameter is either the name of the database
** (i.e. "main", "temp" or an attached database) containing the specified
** table or NULL. If it is NULL, then all attached databases are searched
** for the table using the same algorithm as the database engine uses to 
** resolve unqualified table references.
**
** The third and fourth parameters to this function are the table and column 
** name of the desired column, respectively. Neither of these parameters 
** may be NULL.
**
** Meta information is returned by writing to the memory locations passed as
** the 5th and subsequent parameters to this function. Any of these 
** arguments may be NULL, in which case the corresponding element of meta 
** information is ommitted.
**
** <pre>
** Parameter     Output Type      Description
** -----------------------------------
**
**   5th         const char*      Data type
**   6th         const char*      Name of the default collation sequence 
**   7th         int              True if the column has a NOT NULL constraint
**   8th         int              True if the column is part of the PRIMARY KEY
**   9th         int              True if the column is AUTOINCREMENT
** </pre>
**
**
** The memory pointed to by the character pointers returned for the 
** declaration type and collation sequence is valid only until the next 
** call to any sqlite API function.
**
** If the specified table is actually a view, then an error is returned.
**
** If the specified column is "rowid", "oid" or "_rowid_" and an 
** INTEGER PRIMARY KEY column has been explicitly declared, then the output 
** parameters are set for the explicitly declared column. If there is no
** explicitly declared IPK column, then the output parameters are set as 
** follows:
**
** <pre>
**     data type: "INTEGER"
**     collation sequence: "BINARY"
**     not null: 0
**     primary key: 1
**     auto increment: 0
** </pre>
**
** This function may load one or more schemas from database files. If an
** error occurs during this process, or if the requested table or column
** cannot be found, an SQLITE error code is returned and an error message
** left in the database handle (to be retrieved using sqlite3_errmsg()).
**
** This API is only available if the library was compiled with the
** SQLITE_ENABLE_COLUMN_METADATA preprocessor symbol defined.
*/
int sqlite3_table_column_metadata(
  sqlite3 *db,                /* Connection handle */
  const char *zDbName,        /* Database name or NULL */
  const char *zTableName,     /* Table name */
  const char *zColumnName,    /* Column name */
  char const **pzDataType,    /* OUTPUT: Declared data type */
  char const **pzCollSeq,     /* OUTPUT: Collation sequence name */
  int *pNotNull,              /* OUTPUT: True if NOT NULL constraint exists */
  int *pPrimaryKey,           /* OUTPUT: True if column part of PK */
  int *pAutoinc               /* OUTPUT: True if colums is auto-increment */
);

/*
** CAPI3REF: Load An Extension
**
** Attempt to load an SQLite extension library contained in the file
** zFile.  The entry point is zProc.  zProc may be 0 in which case the
** name of the entry point defaults to "sqlite3_extension_init".
**
** Return [SQLITE_OK] on success and [SQLITE_ERROR] if something goes wrong.
**
** If an error occurs and pzErrMsg is not 0, then fill *pzErrMsg with 
** error message text.  The calling function should free this memory
** by calling [sqlite3_free()].
**
** Extension loading must be enabled using [sqlite3_enable_load_extension()]
** prior to calling this API or an error will be returned.
*/
int sqlite3_load_extension(
  sqlite3 *db,          /* Load the extension into this database connection */
  const char *zFile,    /* Name of the shared library containing extension */
  const char *zProc,    /* Entry point.  Derived from zFile if 0 */
  char **pzErrMsg       /* Put error message here if not 0 */
);

/*
** CAPI3REF:  Enable Or Disable Extension Loading
**
** So as not to open security holes in older applications that are
** unprepared to deal with extension loading, and as a means of disabling
** extension loading while evaluating user-entered SQL, the following
** API is provided to turn the [sqlite3_load_extension()] mechanism on and
** off.  It is off by default.  See ticket #1863.
**
** Call this routine with onoff==1 to turn extension loading on
** and call it with onoff==0 to turn it back off again.
*/
int sqlite3_enable_load_extension(sqlite3 *db, int onoff);

/*
** CAPI3REF: Make Arrangements To Automatically Load An Extension
**
** Register an extension entry point that is automatically invoked
** whenever a new database connection is opened using
** [sqlite3_open()] or [sqlite3_open16()].
**
** This API can be invoked at program startup in order to register
** one or more statically linked extensions that will be available
** to all new database connections.
**
** Duplicate extensions are detected so calling this routine multiple
** times with the same extension is harmless.
**
** This routine stores a pointer to the extension in an array
** that is obtained from malloc().  If you run a memory leak
** checker on your program and it reports a leak because of this
** array, then invoke [sqlite3_automatic_extension_reset()] prior
** to shutdown to free the memory.
**
** Automatic extensions apply across all threads.
**
** This interface is experimental and is subject to change or
** removal in future releases of SQLite.
*/
int sqlite3_auto_extension(void *xEntryPoint);


/*
** CAPI3REF: Reset Automatic Extension Loading
**
** Disable all previously registered automatic extensions.  This
** routine undoes the effect of all prior [sqlite3_automatic_extension()]
** calls.
**
** This call disabled automatic extensions in all threads.
**
** This interface is experimental and is subject to change or
** removal in future releases of SQLite.
*/
void sqlite3_reset_auto_extension(void);


/*
****** EXPERIMENTAL - subject to change without notice **************
**
** The interface to the virtual-table mechanism is currently considered
** to be experimental.  The interface might change in incompatible ways.
** If this is a problem for you, do not use the interface at this time.
**
** When the virtual-table mechanism stablizes, we will declare the
** interface fixed, support it indefinitely, and remove this comment.
*/

/*
** Structures used by the virtual table interface
*/
typedef struct sqlite3_vtab sqlite3_vtab;
typedef struct sqlite3_index_info sqlite3_index_info;
typedef struct sqlite3_vtab_cursor sqlite3_vtab_cursor;
typedef struct sqlite3_module sqlite3_module;

/*
** A module is a class of virtual tables.  Each module is defined
** by an instance of the following structure.  This structure consists
** mostly of methods for the module.
*/
struct sqlite3_module {
  int iVersion;
  int (*xCreate)(sqlite3*, void *pAux,
               int argc, const char *const*argv,
               sqlite3_vtab **ppVTab, char**);
  int (*xConnect)(sqlite3*, void *pAux,
               int argc, const char *const*argv,
               sqlite3_vtab **ppVTab, char**);
  int (*xBestIndex)(sqlite3_vtab *pVTab, sqlite3_index_info*);
  int (*xDisconnect)(sqlite3_vtab *pVTab);
  int (*xDestroy)(sqlite3_vtab *pVTab);
  int (*xOpen)(sqlite3_vtab *pVTab, sqlite3_vtab_cursor **ppCursor);
  int (*xClose)(sqlite3_vtab_cursor*);
  int (*xFilter)(sqlite3_vtab_cursor*, int idxNum, const char *idxStr,
                int argc, sqlite3_value **argv);
  int (*xNext)(sqlite3_vtab_cursor*);
  int (*xEof)(sqlite3_vtab_cursor*);
  int (*xColumn)(sqlite3_vtab_cursor*, sqlite3_context*, int);
  int (*xRowid)(sqlite3_vtab_cursor*, sqlite_int64 *pRowid);
  int (*xUpdate)(sqlite3_vtab *, int, sqlite3_value **, sqlite_int64 *);
  int (*xBegin)(sqlite3_vtab *pVTab);
  int (*xSync)(sqlite3_vtab *pVTab);
  int (*xCommit)(sqlite3_vtab *pVTab);
  int (*xRollback)(sqlite3_vtab *pVTab);
  int (*xFindFunction)(sqlite3_vtab *pVtab, int nArg, const char *zName,
                       void (**pxFunc)(sqlite3_context*,int,sqlite3_value**),
                       void **ppArg);
};

/*
** The sqlite3_index_info structure and its substructures is used to
** pass information into and receive the reply from the xBestIndex
** method of an sqlite3_module.  The fields under **Inputs** are the
** inputs to xBestIndex and are read-only.  xBestIndex inserts its
** results into the **Outputs** fields.
**
** The aConstraint[] array records WHERE clause constraints of the
** form:
**
**         column OP expr
**
** Where OP is =, <, <=, >, or >=.  The particular operator is stored
** in aConstraint[].op.  The index of the column is stored in 
** aConstraint[].iColumn.  aConstraint[].usable is TRUE if the
** expr on the right-hand side can be evaluated (and thus the constraint
** is usable) and false if it cannot.
**
** The optimizer automatically inverts terms of the form "expr OP column"
** and makes other simplificatinos to the WHERE clause in an attempt to
** get as many WHERE clause terms into the form shown above as possible.
** The aConstraint[] array only reports WHERE clause terms in the correct
** form that refer to the particular virtual table being queried.
**
** Information about the ORDER BY clause is stored in aOrderBy[].
** Each term of aOrderBy records a column of the ORDER BY clause.
**
** The xBestIndex method must fill aConstraintUsage[] with information
** about what parameters to pass to xFilter.  If argvIndex>0 then
** the right-hand side of the corresponding aConstraint[] is evaluated
** and becomes the argvIndex-th entry in argv.  If aConstraintUsage[].omit
** is true, then the constraint is assumed to be fully handled by the
** virtual table and is not checked again by SQLite.
**
** The idxNum and idxPtr values are recorded and passed into xFilter.
** sqlite3_free() is used to free idxPtr if needToFreeIdxPtr is true.
**
** The orderByConsumed means that output from xFilter will occur in
** the correct order to satisfy the ORDER BY clause so that no separate
** sorting step is required.
**
** The estimatedCost value is an estimate of the cost of doing the
** particular lookup.  A full scan of a table with N entries should have
** a cost of N.  A binary search of a table of N entries should have a
** cost of approximately log(N).
*/
struct sqlite3_index_info {
  /* Inputs */
  const int nConstraint;     /* Number of entries in aConstraint */
  const struct sqlite3_index_constraint {
     int iColumn;              /* Column on left-hand side of constraint */
     unsigned char op;         /* Constraint operator */
     unsigned char usable;     /* True if this constraint is usable */
     int iTermOffset;          /* Used internally - xBestIndex should ignore */
  } *const aConstraint;      /* Table of WHERE clause constraints */
  const int nOrderBy;        /* Number of terms in the ORDER BY clause */
  const struct sqlite3_index_orderby {
     int iColumn;              /* Column number */
     unsigned char desc;       /* True for DESC.  False for ASC. */
  } *const aOrderBy;         /* The ORDER BY clause */

  /* Outputs */
  struct sqlite3_index_constraint_usage {
    int argvIndex;           /* if >0, constraint is part of argv to xFilter */
    unsigned char omit;      /* Do not code a test for this constraint */
  } *const aConstraintUsage;
  int idxNum;                /* Number used to identify the index */
  char *idxStr;              /* String, possibly obtained from sqlite3_malloc */
  int needToFreeIdxStr;      /* Free idxStr using sqlite3_free() if true */
  int orderByConsumed;       /* True if output is already ordered */
  double estimatedCost;      /* Estimated cost of using this index */
};
#define SQLITE_INDEX_CONSTRAINT_EQ    2
#define SQLITE_INDEX_CONSTRAINT_GT    4
#define SQLITE_INDEX_CONSTRAINT_LE    8
#define SQLITE_INDEX_CONSTRAINT_LT    16
#define SQLITE_INDEX_CONSTRAINT_GE    32
#define SQLITE_INDEX_CONSTRAINT_MATCH 64

/*
** This routine is used to register a new module name with an SQLite
** connection.  Module names must be registered before creating new
** virtual tables on the module, or before using preexisting virtual
** tables of the module.
*/
int sqlite3_create_module(
  sqlite3 *db,               /* SQLite connection to register module with */
  const char *zName,         /* Name of the module */
  const sqlite3_module *,    /* Methods for the module */
  void *                     /* Client data for xCreate/xConnect */
);

/*
** Every module implementation uses a subclass of the following structure
** to describe a particular instance of the module.  Each subclass will
** be taylored to the specific needs of the module implementation.   The
** purpose of this superclass is to define certain fields that are common
** to all module implementations.
**
** Virtual tables methods can set an error message by assigning a
** string obtained from sqlite3_mprintf() to zErrMsg.  The method should
** take care that any prior string is freed by a call to sqlite3_free()
** prior to assigning a new string to zErrMsg.  After the error message
** is delivered up to the client application, the string will be automatically
** freed by sqlite3_free() and the zErrMsg field will be zeroed.  Note
** that sqlite3_mprintf() and sqlite3_free() are used on the zErrMsg field
** since virtual tables are commonly implemented in loadable extensions which
** do not have access to sqlite3MPrintf() or sqlite3Free().
*/
struct sqlite3_vtab {
  const sqlite3_module *pModule;  /* The module for this virtual table */
  int nRef;                       /* Used internally */
  char *zErrMsg;                  /* Error message from sqlite3_mprintf() */
  /* Virtual table implementations will typically add additional fields */
};

/* Every module implementation uses a subclass of the following structure
** to describe cursors that point into the virtual table and are used
** to loop through the virtual table.  Cursors are created using the
** xOpen method of the module.  Each module implementation will define
** the content of a cursor structure to suit its own needs.
**
** This superclass exists in order to define fields of the cursor that
** are common to all implementations.
*/
struct sqlite3_vtab_cursor {
  sqlite3_vtab *pVtab;      /* Virtual table of this cursor */
  /* Virtual table implementations will typically add additional fields */
};

/*
** The xCreate and xConnect methods of a module use the following API
** to declare the format (the names and datatypes of the columns) of
** the virtual tables they implement.
*/
int sqlite3_declare_vtab(sqlite3*, const char *zCreateTable);

/*
** Virtual tables can provide alternative implementations of functions
** using the xFindFunction method.  But global versions of those functions
** must exist in order to be overloaded.
**
** This API makes sure a global version of a function with a particular
** name and number of parameters exists.  If no such function exists
** before this API is called, a new function is created.  The implementation
** of the new function always causes an exception to be thrown.  So
** the new function is not good for anything by itself.  Its only
** purpose is to be a place-holder function that can be overloaded
** by virtual tables.
**
** This API should be considered part of the virtual table interface,
** which is experimental and subject to change.
*/
int sqlite3_overload_function(sqlite3*, const char *zFuncName, int nArg);

/*
** The interface to the virtual-table mechanism defined above (back up
** to a comment remarkably similar to this one) is currently considered
** to be experimental.  The interface might change in incompatible ways.
** If this is a problem for you, do not use the interface at this time.
**
** When the virtual-table mechanism stablizes, we will declare the
** interface fixed, support it indefinitely, and remove this comment.
**
****** EXPERIMENTAL - subject to change without notice **************
*/

/*
** CAPI3REF: A Handle To An Open BLOB
**
** An instance of the following opaque structure is used to 
** represent an blob-handle.  A blob-handle is created by
** [sqlite3_blob_open()] and destroyed by [sqlite3_blob_close()].
** The [sqlite3_blob_read()] and [sqlite3_blob_write()] interfaces
** can be used to read or write small subsections of the blob.
** The [sqltie3_blob_size()] interface returns the size of the
** blob in bytes.
*/
typedef struct sqlite3_blob sqlite3_blob;

/*
** CAPI3REF: Open A BLOB For Incremental I/O
**
** Open a handle to the blob located in row iRow,, column zColumn, 
** table zTable in database zDb. i.e. the same blob that would
** be selected by:
**
** <pre>
**     SELECT zColumn FROM zDb.zTable WHERE rowid = iRow;
** </pre>
**
** If the flags parameter is non-zero, the blob is opened for 
** read and write access. If it is zero, the blob is opened for read 
** access.
**
** On success, [SQLITE_OK] is returned and the new 
** [sqlite3_blob | blob handle] is written to *ppBlob.
** Otherwise an error code is returned and 
** any value written to *ppBlob should not be used by the caller.
** This function sets the database-handle error code and message
** accessible via [sqlite3_errcode()] and [sqlite3_errmsg()].
*/
int sqlite3_blob_open(
  sqlite3*,
  const char *zDb,
  const char *zTable,
  const char *zColumn,
  sqlite_int64 iRow,
  int flags,
  sqlite3_blob **ppBlob
);

/*
** CAPI3REF:  Close A BLOB Handle
**
** Close an open [sqlite3_blob | blob handle].
*/
int sqlite3_blob_close(sqlite3_blob *);

/*
** CAPI3REF:  Return The Size Of An Open BLOB
**
** Return the size in bytes of the blob accessible via the open 
** [sqlite3_blob | blob-handle] passed as an argument.
*/
int sqlite3_blob_bytes(sqlite3_blob *);

/*
** CAPI3REF:  Read Data From A BLOB Incrementally
**
** This function is used to read data from an open 
** [sqlite3_blob | blob-handle] into a caller supplied buffer.
** n bytes of data are copied into buffer
** z from the open blob, starting at offset iOffset.
**
** On success, SQLITE_OK is returned. Otherwise, an 
** [SQLITE_ERROR | SQLite error code] or an
** [SQLITE_IOERR_READ | extended error code] is returned.
*/
int sqlite3_blob_read(sqlite3_blob *, void *z, int n, int iOffset);

/*
** CAPI3REF:  Write Data Into A BLOB Incrementally
**
** This function is used to write data into an open 
** [sqlite3_blob | blob-handle] from a user supplied buffer.
** n bytes of data are copied from the buffer
** pointed to by z into the open blob, starting at offset iOffset.
**
** If the [sqlite3_blob | blob-handle] passed as the first argument
** was not opened for writing (the flags parameter to [sqlite3_blob_open()]
*** was zero), this function returns [SQLITE_READONLY].
**
** This function may only modify the contents of the blob, it is
** not possible to increase the size of a blob using this API. If
** offset iOffset is less than n bytes from the end of the blob, 
** [SQLITE_ERROR] is returned and no data is written.
**
** On success, SQLITE_OK is returned. Otherwise, an 
** [SQLITE_ERROR | SQLite error code] or an
** [SQLITE_IOERR_READ | extended error code] is returned.
*/
int sqlite3_blob_write(sqlite3_blob *, const void *z, int n, int iOffset);

/*
** Undo the hack that converts floating point types to integer for
** builds on processors without floating point support.
*/
#ifdef SQLITE_OMIT_FLOATING_POINT
# undef double
#endif

#ifdef __cplusplus
}  /* End of the 'extern "C"' block */
#endif
#endif
