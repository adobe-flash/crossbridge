/* token_stream.cc */
/*
Copyright (C) 2003 Unique Software Designs

This file is part of the program "lambda".

The program "lambda" is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

The program "lambda" is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with "lambda"; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

A copy of the GNU General Public License may also be found at:
http://www.gnu.org/copyleft/gpl.html
*/
#if HAVE_CONFIG_H
#  include "config.h"
#endif
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <config.h>
#include "token_stream.h"

static struct header_def{
	token_stream::header_type type;
	char* value;
} headers[] = {
	{token_stream::DEF, "def"},
	{token_stream::LOAD, "load"},
	{token_stream::SAVE, "save"},
	{token_stream::QUIT, "quit"},
	{token_stream::LIST, "list"},
	{token_stream::SET,  "set"},
	{token_stream::EXT,  "ext"},
	{token_stream::NOTHDR, NULL} };

token_stream::
token_stream(const char* filename)
{
	in = 0;
	file_name_v = 0;
	paren_level_v = 0;
	linenum_v = 0;
	need_new_line_v = 0;
	open(filename);
}

Boolean token_stream::
open(const char* filename)
{
	read_error = False;
	if( filename ){
		if( in ) fclose(in);
		in = fopen(filename,"r" );
		if( !in ){
			read_error = True;
			dderrmsg("Cannot Open \"%s\":%s\n",
				filename, strerror(errno));
		}
		file_name_v = new char[strlen(filename)+1];
		if( file_name_v ) strcpy(file_name_v,filename);
	}else{
		in = stdin;
		file_name_v = 0;
	}
	read_EOF = False;
	push_token(NOTOKEN,"");
	ptok = 0;
	linenum_v = 0;
	paren_level_v = 0;
	need_new_line_v = 1;
	return (in!=0);
}

token_stream::
~token_stream()
{
	if( in ) fclose(in);
	if( file_name_v ) delete [] file_name_v;
}

void token_stream::
reset_token()
{
	paren_level_v = 0;
#if 1
	pushed_token = NOTOKEN;
	need_new_line_v = 1;
	ptok = linebuf;
	*ptok = '\0';
#endif
}

void token_stream::
close()
{
	if( in && file_name_v) fclose(in);
	in = 0;
}

void token_stream::
dderrmsg(char* format,...)
{
	va_list args;

	va_start(args, format);
	//format = va_arg(args,char*);
#if defined _WINDOWS
	char msg[512];
	sprintf(msg, "***line %d:\n", linenum_v);
	vsprintf(msg+strlen(msg), format, args );
	AfxMessageBox(msg);
#else
	if(linenum_v>=0)
		fprintf(stderr,"\n*** line %d: ",linenum_v);
	vfprintf(stderr,format,args);
#endif
	va_end(args);
	fflush(stderr);
}

char *token_stream::
read_line()
{
	//#define READ_BUF_SIZE 512
	//static char linebuf[READ_BUF_SIZE];
	int n;

	char *res = fgets(linebuf,READ_BUF_SIZE,in);
	if( res != NULL ){
		linenum_v++;
		n = strlen(linebuf);
		if( n>0 ){
			n -= 1;
			if( linebuf[n]=='\n' ) linebuf[n] = '\0';
		}
	} else {
		if( ferror(in) ){
			n = errno;
			printf(
			"*error reading line %d of DataDesc input file: %s\n",
				linenum_v,strerror(errno)  );
			read_error = True;
		}else if( feof(in) ){
			read_EOF = True;
		}
	}
	need_new_line_v = 0;
	return res;
}

token_stream::header_type token_stream::
is_header(token_type tok, char *ptk )
{
	header_def* phdr;
	header_type res = NOTHDR;

	if( NAME==tok ){
		for(phdr = headers; phdr->type!=NOTHDR; phdr++ ){
			if( strcmp(ptk,phdr->value)==0 ){
				res = phdr->type;
				break;
			}
		}
	}
	return res;
}

void token_stream::
push_token(token_type tok, char *ptk )
{
	pushed_token = tok;
	if( ptk!=NULL && token!=ptk ) strncpy(token,ptk,MAX_TOKEN_SIZE-1);
	token[MAX_TOKEN_SIZE-1] = '\0';
}

token_stream::token_type token_stream::
get_token(char **atok)
{
	char *ttok;
	token_stream::token_type res;
	register char chr;
	Boolean found;
	
	if( !in ) return NOTOKEN;
	if( pushed_token!=NOTOKEN ){
		*atok = token;
		res = pushed_token;
		pushed_token = NOTOKEN;
		return res;
	}
	res = NOTOKEN;
	*atok = NULL;
	found = False;
	while( !found ){
		if( NULL==ptok || '\0'==ptok[0] ){
			if( 0==paren_level_v && !need_new_line_v ) {
				need_new_line_v = 1;
				token[0] = '\0';
				res = EOE;
				break;
			}
			if( paren_level_v > 0 )
				printf("*need %d )%s*<< ", paren_level_v,
					paren_level_v > 1 ? "'s" : "" );
			ptok = read_line();
		}
		if( NULL==ptok ) break;

		found = True;
		while( (chr=ptok[0])!='\0' && chr<=' ' ) ptok++;

		token[0] = ptok[0];
		token[1] = '\0';
		switch( ptok[0] ){
		case '\0':
		case '#':
			found = False;
			ptok = NULL;
			break;
		case '^':
			res = LAMBDA;
			ptok++;
			break;
		case '.':
			res = PERIOD;
			ptok++;
			break;
		case '(':
			res = LPAREN;
			paren_level_v++;
			ptok++;
			break;
		case ')':
			res = RPAREN;
			paren_level_v--;
			ptok++;
			break;
		case '{':
			res = LBRACE;
			ptok++;
			break;
		case '}':
			res = RBRACE;
			ptok++;
			break;
		case ',':
			res = COMMA;
			ptok++;
			break;
		case ';':
			res = SEMICOLON;
			ptok++;
			break;
		case '"':
			res = STRING;
			for( ttok=token,ptok++;
			     ttok<token+MAX_TOKEN_SIZE-1;
			     ttok++,ptok++
			){
				while( '\\'==ptok[0] && '\0'==ptok[1] ){
					ptok = read_line();
					if( NULL==ptok ){
						res = NOTOKEN;
						goto endit;
					}
				}
				chr = ptok[0];
				if( chr=='\\' ){
					ptok++;
					chr = ptok[0];
					if( chr=='n' ) chr = '\n';
					else if( chr=='t' ) chr = '\t';
				}else{
					if( '\0'==chr ) break;
					if( '"' ==chr ){
						ptok++;
						break;
					}
				}
				ttok[0] = chr;
			}
			ttok[0] = '\0';
			break;
		default:
			res = NAME;
			for( ttok=token;
			     ttok<token+MAX_TOKEN_SIZE-1
			       && '\0'!=ptok[0];
			     ttok++,ptok++
			){
				chr = ptok[0];
				if( NULL!=strchr(" .(){},;\"",chr) ) break;
				ttok[0] = chr;
			}
			ttok[0] = '\0';
		}
	}
endit:
	if( res!=NOTOKEN ) *atok = token;
	#ifdef PRINTTOKENS
	if( res!=NOTOKEN ){
		/*fprintf(stderr,"%s ",tokname[tok] );*/
		fprintf(stderr," %s%s%s",
			res==STRING? "\"":
			 res==LBRACE? "\n":
			"",
			token,
			res==STRING? "\"" :
			 res==SEMICOLON? "\n":
			is_header(res,token)? "\n":
			""
		);
	}else{
		fprintf(stderr,"\nEnd of Token Stream!\n");
	}
	fflush(stderr);
	#endif
	if( res!=NOTOKEN ) *atok = token;
	return res;
}
