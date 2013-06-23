/* parse.h */
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

extern int trace_lambda;
extern int step_lambda;
extern int print_symbols;
extern int applicative_order;
extern int reduce_body;
extern int brief_print;
extern int step_thru;
extern int reduce_fully;

class lambda_expression_parser
{
public:
	lambda_expression_parser(token_stream* tokstr=0);
	~lambda_expression_parser();
	exp_node* expression(arglst_node** penv=0);
	exp_node* application();
	exp_node* lambda();
	exp_node* alist();
	exp_node* atom();
public:
	arglst_node* definition(arglst_node** penv);
	arglst_node* load(arglst_node** penv);
public:
	exp_node* extraction(arglst_node** penv,int inp=0);
public:
	void set_tok_str(token_stream* tokstr);
	void reset();
	void dderrmsg(char* format,...);
private:
	token_stream* tokstr_v;
	int paren_nest_level_v;
};
