/* Copyright (C) 1989, 1990 Aladdin Enterprises.  All rights reserved.
   Distributed by Free Software Foundation, Inc.

This file is part of Ghostscript.

Ghostscript is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY.  No author or distributor accepts responsibility
to anyone for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.  Refer
to the Ghostscript General Public License for full details.

Everyone is granted permission to copy, modify and redistribute
Ghostscript, but only under the conditions described in the Ghostscript
General Public License.  A copy of this license is supposed to have been
given to you along with Ghostscript so you can know your rights and
responsibilities.  It should be in a file named COPYING.  Among other
things, the copyright notice and this notice must be preserved on all
copies.  */

/* zfile.c */
/* File operators for GhostScript */
#include "memory_.h"
#include "string_.h"
#include "ghost.h"
#include "errors.h"
#include "oper.h"
#include "alloc.h"
#include "stream.h"
#include "store.h"
#include "gsmatrix.h"			/* for gxdevice.h */
#include "gxdevice.h"
#include "gxdevmem.h"

/* Forward references */
int zreadline_from(P4(byte *, uint, uint *, stream *));
int lib_file_open(P3(byte *, uint, ref *));
private int open_std_file(P3(ref *, char *, ref *));
private ref *get_current_file();
private int write_string(P2(ref *, stream *));

/* Imported from gs.c */
extern char **gs_lib_paths;		/* search path list, */
					/* terminated by a null pointer */

/* Imported from gp_*.c. */
extern char gp_file_name_list_separator;
extern int gp_file_name_is_absolute(P2(char *, uint));
extern char *gp_file_name_concat_string(P4(char *, uint, char *, uint));

/* Imported from iutil.c */
extern char *ref_to_string(P2(ref *, char *));

/* Import the execution stack for currentfile */
extern ref estack[];
extern ref *esp;

/* File objects store a pointer to a file entry in value.pfile. */
/* A file entry is valid if its stream pointer is non-zero. */
#define fptr(pref) (pref)->value.pfile
#define make_file(pref,a,pfe)\
  make_tav(pref,t_file,a,pfile,pfe)
typedef struct file_entry_s file_entry;
struct file_entry_s {
	stream *s;
	int can_close;			/* 0 for stdin/out/err, */
					/* -1 for line/statementedit, */
					/* 1 for other files */
	ref file_name;			/* t_string */
};

/* File buffer sizes.  For real files, this is arbitrary, */
/* since the C library does its own buffering in addition. */
/* stdout and stderr use smaller buffers, */
/* on the assumption that they are usually not real files. */
/* The buffer size for type 1 encrypted files is NOT arbitrary: */
/* it must be at most 512. */
#define buffer_size 512

/* Standard file objects: */
/* 0 is stdin, 1 is stdout, 2 is stderr, 3 is lineedit, 4 is statementedit */
#define num_std_files 5
private file_entry invalid_file_entry = { (stream *)0 };
private byte
#define stdin_buf_size 1
	stdin_buf[stdin_buf_size],
#define stdout_buf_size 128
	stdout_buf[stdout_buf_size],
#define stderr_buf_size 128
	stderr_buf[stderr_buf_size],
#define lineedit_buf_size 160
	lineedit_buf[lineedit_buf_size];
/* statementedit is equivalent to lineedit for now */
private stream std_file_streams[num_std_files];
private file_entry std_files[num_std_files] =
   {	{ &std_file_streams[0], 0 },
	{ &std_file_streams[1], 0 },
	{ &std_file_streams[2], 0 },
	{ &std_file_streams[3], -1 },
	{ &std_file_streams[4], -1 }
   };
private char *std_file_names[num_std_files] =
   {	"%stdin",
	"%stdout",
	"%stderr",
	"%lineedit",
	"%statementedit"
   };
private int std_file_attrs[num_std_files] =
   {	a_read+a_execute,
	a_write+a_execute,
	a_write+a_execute,
	a_read+a_execute,
	a_read+a_execute
   };

/* Macros for checking file validity */
#define check_file_access(svar,op,acc)\
   {	file_entry *fe = fptr(op);\
	svar = fe->s;		/* do first, acc may refer to it */\
	if ( svar == 0 || !(acc) )\
		return e_invalidaccess;\
   }
#define check_file_ref(svar,op,acc)\
   {	if ( r_type(op) != t_file ) return e_typecheck;\
	check_file_access(svar,op,acc);\
   }
#define check_file(svar,op) check_file_ref(svar,op,1)
#define check_read_file(svar,op) check_file_ref(svar,op,!svar->writing)
#define check_write_file(svar,op) check_file_ref(svar,op,svar->writing)

/* Initialize the file table */
void
zfile_init()
{	/* Create files for stdin, stdout, and stderr. */
	int i;
	/****** stdin IS NOT IMPLEMENTED PROPERLY ******/
	sread_file(std_files[0].s, stdin, stdin_buf, stdin_buf_size);
	swrite_file(std_files[1].s, stdout, stdout_buf, stdout_buf_size);
	swrite_file(std_files[2].s, stderr, stderr_buf, stderr_buf_size);
	for ( i = 0; i < num_std_files; i++ )
	   {	ref *pfn = &std_files[i].file_name;
		make_tv(pfn, t_null, intval, 0);	/* pre-clear */
		if ( string_to_ref(std_file_names[i], pfn, "zfile_init") < 0 )
		   {	dprintf("alloc failed in zfile_init!\n");
			gs_exit(1);
		   }
	   }
}

/* file */
int
zfile(register ref *op)
{	char *file_access;
	ref fname;
	int code;
	fname = op[-1];
	check_type(fname, t_string);
	check_type(*op, t_string);
	if ( op->size != 1 ) return e_invalidfileaccess;
	switch ( *op->value.bytes )
	   {
	case 'r': file_access = "r"; break;
	case 'w': file_access = "w"; break;
	default: return e_invalidfileaccess;
	   }
	code = open_std_file(op - 1, file_access, op - 1);
	switch ( code )
	   {
	case 0:				/* successful open */
		pop(1);
	default:			/* unsuccessful open */
		return code;
	case e_undefinedfilename:	/* not a %file */
		;
	   }
	code = file_open(fname.value.bytes, fname.size, file_access, op - 1);
	if ( code >= 0 ) pop(1);
	return code;
}

/* closefile */
int
zclosefile(register ref *op)
{	stream *s;
	int code;
	check_file(s, op);
	if ( (code = file_close(op, s)) >= 0 )
	   {	/* If we just closed the file from which the interpreter */
		/* is reading, zap it on the exec stack. */
		ref *fp = get_current_file();
		if ( fp != 0 && fptr(fp) == fptr(op) )
			/* A null would confuse the estack parser.... */
			make_tasv(fp, t_array, a_executable+a_execute, 0, refs, (ref *)0);
		pop(1);
	   }
	return code;
}

/* read */
int
zread(register ref *op)
{	stream *s;
	int ch;
	check_read_file(s, op);
	check_read(*op);
	ch = sgetc(s);
	if ( ch == EOFC )
		make_bool(op, 0);
	else
	   {	make_int(op, ch);
		push(1);
		make_bool(op, 1);
	   }
	return 0;
}

/* unread */
int
zunread(register ref *op)
{	stream *s;
	ulong ch;
	check_read_file(s, op - 1);
	check_type(*op, t_integer);
	ch = op->value.intval;
	if ( ch > 0xff ) return e_rangecheck;
	if ( sungetc(s, (byte)ch) < 0 ) return e_ioerror;
	pop(2);
	return 0;
}

/* write */
int
zwrite(register ref *op)
{	stream *s;
	ulong ch;
	check_write_file(s, op - 1);
	check_write(op[-1]);
	check_type(*op, t_integer);
	ch = op->value.intval;
	if ( ch > 0xff ) return e_rangecheck;
	sputc(s, (byte)ch);
	pop(2);
	return 0;
}

/* readhexstring */
int
zreadhexstring(register ref *op)
{	stream st;
	stream *s;
	ref *op1 = op - 1;
	int odd = -1;
	int code;
	uint nread;
	switch ( r_type(op1) )
	   {
	default: return e_typecheck;
	case t_file:
		check_read_file(s, op1);
		check_read(*op1);
		break;
	case t_string:
		s = &st;
		sread_string(s, op1->value.bytes, op1->size);
	   }
	check_type(*op, t_string);
	check_write(*op);
	code = sreadhex(s, op->value.bytes, op->size, &nread, &odd);
	switch ( code )
	   {
	case 1:
		/* Reached end-of-file before filling the string. */
		/* Return an appropriate substring. */
		op->size = nread;
		r_set_attrs(op, a_subrange);
		break;
	case 0:
		/* Filled the string. */
		break;
	default:			/* Error */
		return e_ioerror;
	   }
	if ( s == &st )
	   {	/* Reading from a string, return remainder */
		uint pos = stell(&st);
		op1->size -= pos, op1->value.bytes += pos;
		r_set_attrs(op1, a_subrange);
		push(1);
	   }
	else
	   {	/* Reading from a file */
		*op1 = *op;
	   }
	make_bool(op, 1 - code);
	return 0;
}

/* writehexstring */
int
zwritehexstring(register ref *op)
{	stream *s;
	byte *p;
	uint len;
	static char *hex_digits = "0123456789abcdef";
	check_write_file(s, op - 1);
	check_write(op[-1]);
	check_read_type(*op, t_string);
	p = op->value.bytes;
	len = op->size;
	while ( len-- )
	   {	byte ch = *p++;
		sputc(s, hex_digits[ch >> 4]);
		sputc(s, hex_digits[ch & 0xf]);
	   }
	pop(2);
	return 0;
}

/* readstring */
int
zreadstring(register ref *op)
{	stream *s;
	uint len, rlen;
	check_read_file(s, op - 1);
	check_read(op[-1]);
	check_write_type(*op, t_string);
	len = op->size;
	rlen = sgets(s, op->value.bytes, len);
	op->size = rlen;
	r_set_attrs(op, a_subrange);
	op[-1] = *op;
	make_bool(op, (rlen == len ? 1 : 0));
	return 0;
}

/* writestring */
int
zwritestring(register ref *op)
{	stream *s;
	int code;
	check_write_file(s, op - 1);
	check_write(op[-1]);
	code = write_string(op, s);
	if ( code >= 0 ) pop(2);
	return code;
}

/* readline */
int
zreadline(register ref *op)
{	stream *s;
	uint count;
	int code;
	check_read_file(s, op - 1);
	check_read(op[-1]);
	check_write_type(*op, t_string);
	code = zreadline_from(op->value.bytes, op->size, &count, s);
	if ( code < 0 ) return code;
	op->size = count;
	r_set_attrs(op, a_subrange);
	op[-1] = *op;
	make_bool(op, code);
	return 0;
}

/* Read a line from stdin.  This is called from gs.c. */
int
zreadline_stdin(byte *ptr, uint size, uint *pcount)
{	return zreadline_from(ptr, size, pcount, std_files[0].s);
}

/* Internal readline routine. */
/* Returns 1 if OK, 0 if end of file, or an error code. */
int
zreadline_from(byte *ptr, uint size, uint *pcount, stream *s)
{	uint count = 0;
	int ch;
	while ( count < size )
	   {	switch ( ch = sgetc(s) )
		   {
		case '\r':
			ch = sgetc(s);
			if ( ch != '\n' && ch != EOFC ) sputback(s);
			/* falls through */
		case '\n':
			*pcount = count;
			return 1;
		case EOFC:
			*pcount = count;
			return 0;
		   }
		*ptr++ = ch;
		count++;
	   }
	return e_rangecheck;		/* filled the string */
}

/* token - this is called from zstring.c */
int
ztoken_file(register ref *op)
{	stream *s;
	ref token;
	int code;
	check_read_file(s, op);
	check_read(*op);
	switch ( code = scan_token(s, 0, &token) )
	   {
	case 0:				/* read a token */
		*op = token;
		push(1);
		make_bool(op, 1);
		return 0;
	case 1:				/* no tokens */
		make_bool(op, 0);
		return 0;
	default:			/* error */
		return code;
	   }
}

/* bytesavailable */
int
zbytesavailable(register ref *op)
{	stream *s;
	long avail;
	check_read_file(s, op);
	if ( savailable(s, &avail) < 0 ) return e_ioerror;
	make_int(op, avail);
	return 0;
}

/* flush */
int
zflush(register ref *op)
{	sflush(std_files[1].s);
	return 0;
}

/* flushfile */
int
zflushfile(register ref *op)
{	stream *s;
	check_file(s, op);
	sflush(s);
	if ( !s->writing )
		fseek(s->file, 0L, 2);	/* set to end */
	pop(1);
	return 0;
}

/* resetfile */
int
zresetfile(register ref *op)
{	NYI("resetfile");
	pop(1);
	return 0;
}

/* status */
int
zstatus(register ref *op)
{	check_type(*op, t_file);
	make_bool(op, (fptr(op)->s != 0 ? 1 : 0));
	return 0;
}

/* run */
int
zrun(register ref *op)
{	NYI("run");
	pop(1);
	return 0;
}

/* currentfile */
int
zcurrentfile(register ref *op)
{	ref *fp;
	push(1);
	if ( (fp = get_current_file()) == 0 )
	   {	/* Return an invalid file object. */
		/* This doesn't make a lot of sense to me, */
		/* but it's what the PostScript manual specifies. */
		make_file(op, 0, &invalid_file_entry);
	   }
	else
		*op = *fp;
	/* Make sure the returned value is literal. */
	r_clear_attrs(op, a_executable);
	return 0;
}

/* print */
int
zprint(register ref *op)
{	int code = write_string(op, std_files[1].s);
	if ( code >= 0 ) pop(1);
	return code;
}

/* echo */
int
zecho(register ref *op)
{	check_type(*op, t_boolean);
	/****** NOT IMPLEMENTED YET ******/
	pop(1);
	return 0;
}

/* ------ Level 2 extensions ------ */

/* setfileposition */
int
zsetfileposition(register ref *op)
{	stream *s;
	check_file(s, op - 1);
	check_type(*op, t_integer);
	if ( sseek(s, op->value.intval) < 0 ) return e_ioerror;
	pop(2);
	return 0;
}

/* fileposition */
int
zfileposition(register ref *op)
{	stream *s;
	check_file(s, op);
	if ( !sseekable(s) ) return e_ioerror;
	make_int(op, stell(s));
	return 0;
}

/* deletefile */
int
zdeletefile(register ref *op)
{	char *str;
	int stat;
	check_read_type(*op, t_string);
	str = ref_to_string(op, "deletefile");
	if ( str == 0 ) return e_VMerror;
	stat = unlink(str);
	alloc_free(str, op->size + 1, 1, "deletefile");
	if ( stat != 0 ) return e_ioerror;
	pop(1);
	return 0;
}

/* renamefile */
int
zrenamefile(register ref *op)
{	char *str1 = 0, *str2 = 0;
	check_read_type(*op, t_string);
	check_read_type(op[-1], t_string);
	str1 = ref_to_string(op - 1, "renamefile(from)");
	str2 = ref_to_string(op, "renamefile(to)");
	if ( str1 != 0 && str2 != 0 && rename(str1, str2) == 0 )
	   {	pop(2);
	   }
	if ( str1 != 0 )
		alloc_free(str1, op[-1].size + 1, 1, "renamefile(from)");
	if ( str2 != 0 )
		alloc_free(str2, op->size + 1, 1, "renamefile(to)");
	return 0;
}	

/* ------ Ghostscript extensions ------ */

/* filename */
int
zfilename(register ref *op)
{	stream *s;
	check_file(s, op);
	*op = op->value.pfile->file_name;
	return 0;
}

/* findlibfile */
int
zfindlibfile(register ref *op)
{	int code;
	check_type(*op, t_string);
	code = open_std_file(op, "r", op);
	switch ( code )
	   {
	case 0:				/* successful open */
		push(1);
		make_bool(op, 1);
	default:			/* unsuccessful open */
		return code;
	case e_undefinedfilename:	/* not a %file */
		;
	   }
	code = lib_file_open(op->value.bytes, op->size, op);
	push(1);
	make_bool(op, code >= 0);
	return 0;
}

/* writeppmfile */
int
zwriteppmfile(register ref *op)
{	stream *s;
	int code;
	check_write_file(s, op - 1);
	check_write(op[-1]);
	check_type(*op, t_device);
	if ( !gs_device_is_memory(op->value.pdevice) ) return e_typecheck;
	sflush(s);
	code = gs_writeppmfile((gx_device_memory *)(op->value.pdevice), s->file);
	if ( code >= 0 ) pop(2);
	return code;
}

/* type1decryptfile */
int
ztype1decryptfile(register ref *op)
{	stream *s;
	ushort state;
	ref dec_file;
	int code;
	stream *es;
	check_type(op[-1], t_integer);
	state = op[-1].value.intval;
	if ( op[-1].value.intval != state )
		return e_rangecheck;	/* state value was truncated */
	check_read_file(s, op);
	code = file_open((byte *)0, 0, "r", &dec_file);
	if ( code < 0 ) return code;
	es = fptr(&dec_file)->s;
	sread_decrypt(es, fptr(op)->s, es->cbuf, es->bsize, state);
	op[-1] = dec_file;
	pop(1);
	return 0;
}

/* ------ Initialization procedure ------ */

void
zfile_op_init()
{	static op_def my_defs[] = {
		{"1bytesavailable", zbytesavailable},
		{"1closefile", zclosefile},
		{"0currentfile", zcurrentfile},
		{"1deletefile", zdeletefile},
		{"1echo", zecho},
		{"2file", zfile},
		{"1filename", zfilename},
		{"1fileposition", zfileposition},
		{"1findlibfile", zfindlibfile},
		{"0flush", zflush},
		{"1flushfile", zflushfile},
		{"1print", zprint},
		{"1read", zread},
		{"2readhexstring", zreadhexstring},
		{"2readline", zreadline},
		{"2readstring", zreadstring},
		{"2renamefile", zrenamefile},
		{"1resetfile", zresetfile},
		{"1run", zrun},
		{"2setfileposition", zsetfileposition},
		{"2type1decryptfile", ztype1decryptfile},
		{"2unread", zunread},
		{"1status", zstatus},
		{"2write", zwrite},
		{"2writehexstring", zwritehexstring},
		{"2writeppmfile", zwriteppmfile},
		{"2writestring", zwritestring},
		op_def_end
	};
	z_op_init(my_defs);
}

/* ------ Non-operator routines ------ */

/* Open a file, using the search paths if necessary. */
/* The startup code calls this to open the initialization file ghost.ps, */
/* and any other files specified on the command line. */
int
lib_file_open(byte *fname, uint len, ref *pfile)
{	int code;
	char **ppath;
#define max_len 200
	char cname[max_len];
	code = file_open(fname, len, "r", pfile);
	if ( code >= 0 ) return code;
	if ( gp_file_name_is_absolute((char *)fname, len) )
		return e_undefinedfilename;
	/* Go through the list of search paths */
	for ( ppath = gs_lib_paths; *ppath != 0; ppath++ )
	   {	char *path = *ppath;
		for ( ; ; )
		   {	/* Find the end of the next path */
			char *npath = path;
			uint plen;
			char *cstr;
			int clen;
			while ( *npath != 0 && *npath != gp_file_name_list_separator )
				npath++;
			plen = npath - path;
			cstr = gp_file_name_concat_string(path, plen,
							  (char *)fname, len);
			/* Concatenate the prefix, combiner, and file name. */
			clen = plen + strlen(cstr) + len;
			if ( clen <= max_len )	/* otherwise punt */
			   {	memcpy(cname, path, plen);
				strcpy(cname + plen, cstr);
				memcpy(cname + clen - len, fname, len);
				code = file_open(cname, clen, "r", pfile);
				if ( code >= 0 ) return code;
			   }
			/****** NYI ******/
			if ( !*npath ) break;
			path = npath + 1;
		   }
	   }
	return code;
}

/* Open a file and create a file object. */
/* Return 0 if successful, error code if not. */
/* If fname==0, set up the file entry, stream, and buffer, */
/* but don't open an OS file or initialize the stream. */
int
file_open(byte *fname, uint len, char *file_access, ref *pfile)
{	byte *buffer;
	stream *s;
	file_entry *fe;
	int code;
	if ( len >= buffer_size )
		return e_limitcheck;	/* we copy the file name into the buffer */
	/* Allocate the file entry first, since it persists */
	/* even after the file has been closed. */
	fe = (file_entry *)alloc(1, sizeof(file_entry), "file_open(file_entry)");
	if ( fe == 0 ) return e_VMerror;
	/* Allocate the buffer and stream. */
	buffer = (byte *)alloc(buffer_size, 1, "file_open(buffer)");
	if ( buffer == 0 )
	   {	alloc_free((char *)fe, 1, sizeof(file_entry), "file_open(file_entry)");
		return e_VMerror;
	   }
	s = (stream *)alloc(1, sizeof(stream), "file_open(stream)");
	if ( s == 0 )
	   {	alloc_free((char *)buffer, buffer_size, 1, "file_open(buffer)");
		alloc_free((char *)fe, 1, sizeof(file_entry), "file_open(file_entry)");
		return e_VMerror;
	   }
	if ( fname != 0 )
	   {	/* Copy the name (so we can terminate it with a zero byte.) */
		char *file_name = (char *)buffer;
		FILE *file;
		memcpy(file_name, fname, len);
		file_name[len] = 0;		/* terminate string */
		/* Open the file. */
		file = fopen(file_name, file_access);
		code = e_undefinedfilename;
		if ( file == 0 || (code = string_to_ref(file_name, &fe->file_name, "file_open(file_name)")) < 0 )
		   {	alloc_free((char *)s, 1, sizeof(stream), "file_open(stream)");
			alloc_free((char *)buffer, buffer_size, 1, "file_open(buffer)");
			alloc_free((char *)fe, 1, sizeof(file_entry), "file_open(file_entry)");
			return code;
		   }
		/* Set up the stream. */
		if ( *file_access == 'r' )	/* reading */
			sread_file(s, file, buffer, buffer_size);
		else
			swrite_file(s, file, buffer, buffer_size);
	   }
	else				/* save the buffer and size */
	   {	s->cbuf = buffer;
		s->bsize = buffer_size;
	   }
	fe->s = s;
	fe->can_close = 1;
	make_file(pfile,
		(*file_access == 'r' ? a_read+a_execute : a_write+a_execute),
		fe);
	return 0;
}

/* Check a file for reading. */
/* The interpreter calls this to check an executable file. */
int
file_check_read(ref *op, stream **ps)
{	stream *s;
	check_file_access(s, op, !s->writing);
	*ps = s;
	return 0;
}

/* Get the current file from which the interpreter is reading. */
private ref *
get_current_file()
{	ref *ep = esp;
	while ( ep >= estack )
	{	if ( r_type(ep) == t_file && r_has_attrs(ep, a_executable) )
			return ep;
		ep--;
	}
	return (ref *)0;
}

/* Close a file.  The interpreter calls this when */
/* it reaches the end of an executable file. */
int
file_close(ref *fp /* t_file */, stream *s)
{	file_entry *fe = fptr(fp);
	byte *buffer = s->cbuf;
	switch ( fe->can_close )
	   {
	case 0:				/* can't close std files */
		return e_invalidaccess;
	case -1:			/* ignore on statement/lineedit */
		sclose(s);
		break;
	default:			/* ordinary file */
		if ( sclose(s) ) return e_ioerror;
		/* Free the stream and buffer in the reverse of the order */
		/* in which they were created, and hope for LIFO storage behavior. */
		alloc_free((char *)s, 1, sizeof(stream), "file_close(stream)");
		alloc_free((char *)buffer, buffer_size, 1, "file_close(buffer)");
	   }
	fe->s = 0;
	return 0;
}

/* ------ Internal routines ------ */

/* If a file name refers to one of the standard %files, */
/* 'open' the file and return 0 or an error code, otherwise */
/* return e_undefinedfilename. */
private int
open_std_file(ref *pfname, char *file_access, ref *pfile)
{	int i;
	for ( i = 0; i < num_std_files; i++ )
	  if ( !bytes_compare(pfname->value.bytes, pfname->size,
			    std_file_names[i], strlen(std_file_names[i]))
	    )
	   {	/* This is a standard file */
		int attrs = (*file_access == 'r' ? a_read+a_execute : a_write+a_execute);
		file_entry *fe = &std_files[i];
		if ( attrs != std_file_attrs[i] )
			return e_invalidaccess;
		make_file(pfile, attrs, fe);
		/* If this is %lineedit or %statementedit, */
		/* read a line now. */
		switch ( i )
		   {
		case 3: case 4:
		   {	uint count;
			int code = zreadline_stdin(lineedit_buf,
				lineedit_buf_size, &count);
			if ( code < 0 ) return code;
			fe->s = &std_file_streams[i];
			sread_string(fe->s, lineedit_buf, count);
			return 0;
		   }
		   }
		return 0;
	   }
	return e_undefinedfilename;
}

/* Write a string on a file.  The file has been checked for validity, */
/* but not the string. */
private int
write_string(ref *op, stream *s)
{	uint len;
	check_read_type(*op, t_string);
	len = op->size;
	if ( sputs(s, op->value.bytes, len) != len ) return e_ioerror;
	return 0;
}
