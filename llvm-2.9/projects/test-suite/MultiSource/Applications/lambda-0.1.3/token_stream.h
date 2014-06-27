/* token_stream.h */
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
#ifndef Boolean 
#define Boolean short
#define False 0
#define True 1
#endif
class token_stream
{
public:
	token_stream(const char* filename=0);
	~token_stream();
	void reset_token();
public:
	Boolean get_read_error() {return read_error; };
	Boolean get_read_EOF() { return read_EOF; };
	enum token_type {
		NOTOKEN=0,LPAREN,RPAREN,LBRACE,
		RBRACE,COMMA,SEMICOLON,STRING,NAME,
		LAMBDA, PERIOD, EOE
	};
	enum header_type {
		NOTHDR=0, DEF, LOAD, SAVE, QUIT, LIST, SET, EXT
	};
public:
	Boolean open(const char* filename=0);
	void close();
	header_type is_header(token_type tok, char* ptk);
	void push_token( token_type tok,char *ptok );
	token_type get_token( char **atok);
	void dderrmsg(char* format,...);
	int get_linenum() { return linenum_v; };
	void set_linenum(int ln ) { linenum_v = ln; };
	int get_paren_level() { return paren_level_v; };
	void set_paren_level(int ln ) { paren_level_v = ln; };
public:
	enum { MAX_TOKEN_SIZE=8192 };
	enum { READ_BUF_SIZE=512 };
private:
	Boolean read_error;
	Boolean read_EOF;
	char* read_line();
//private:
//	char *read_line(FILE *in);
private:
	FILE* in;
	char *ptok; // = NULL;
	token_type pushed_token; // = NOTOKEN;
	char token[MAX_TOKEN_SIZE];
	char linebuf[READ_BUF_SIZE];
	char* file_name_v;
	int linenum_v;
	int paren_level_v;
	int need_new_line_v;
};
