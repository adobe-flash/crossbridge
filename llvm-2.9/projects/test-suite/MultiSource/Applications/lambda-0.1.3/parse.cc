/* parse.cc */
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
#include <stdarg.h>
#include <config.h>

#include "token_stream.h"
#include "node.h"
#include "parse.h"

int trace_lambda = 0;
int step_lambda = 0;
int print_symbols = 1;
int applicative_order = 0;
int reduce_body = 0;
int brief_print = 1;
int step_thru = 0;
int extract_eta = 1;
int extract_app = 0;
int reduce_fully = 1;

lambda_expression_parser::
lambda_expression_parser(token_stream* tokstr)
{
	tokstr_v = tokstr;
	paren_nest_level_v = 0;
}

lambda_expression_parser::
~lambda_expression_parser()
{
}

exp_node* lambda_expression_parser::
expression(arglst_node** penv)
{
	exp_node* exp = 0;
	if( !tokstr_v ) return exp;
	token_stream::token_type tok;
	char* ptok;
	token_stream::header_type htype;

	tok = tokstr_v->get_token(&ptok);
	htype = tokstr_v->is_header(tok,ptok);
	if( htype != token_stream::NOTHDR){
		switch(htype){
		case token_stream::QUIT:
			exp = (exp_node*)new arg_node("QUIT", 0);
			break;
		case token_stream::DEF:
			definition(penv);
			break;
		case token_stream::LOAD:
			load(penv);
			break;
		case token_stream::SAVE:
			break;
		case token_stream::LIST:
			if( penv && *penv) (*penv)->list();
			break;
		case token_stream::SET:
			{
				int do_list = 0;
				int cnt = 0;
				for( tok = tokstr_v->get_token(&ptok);
					tok!=token_stream::EOE;
					tok = tokstr_v->get_token(&ptok) )
				{
					if( tok==token_stream::NAME ){
						if( 0==strcasecmp("trace", ptok) ){
							trace_lambda = (!trace_lambda);
						}else
						if( 0==strcasecmp("step", ptok) ){
							step_lambda = (!step_lambda);
							if( step_lambda ) step_thru = 0;
						}else
						if( 0==strcasecmp("thru", ptok) ){
							step_thru = (!step_thru);
							if( step_thru ) step_lambda = 0;
						}else
						if( 0==strcasecmp("sym", ptok) ){
							print_symbols = (!print_symbols);
						}else
						if( 0==strcasecmp("app", ptok) ){
							applicative_order = (!applicative_order);
						}else
						if( 0==strcasecmp("body", ptok) ){
							reduce_body = (!reduce_body);
						}else
						if( 0==strcasecmp("brief", ptok) ){
							brief_print = (!brief_print);
						}else
						if( 0==strcasecmp("eta", ptok) ){
							extract_eta = (!extract_eta);
						}else
						if( 0==strcasecmp("xapp", ptok) ){
							extract_app = (!extract_app);
						}else
						if( 0==strcasecmp("full", ptok) ){
							reduce_fully = (!reduce_fully);
						}else{
							do_list = 1;
						}
					}else do_list =1;
					cnt = 1;
				}
				if( do_list || !cnt ){
					printf(">trace = %d\n",trace_lambda);
					printf(">step  = %d\n",step_lambda);
					printf(">thru  = %d\n",step_thru);
					printf(">app   = %d\n",applicative_order);
					printf(">body  = %d\n",reduce_body);
					printf(">brief = %d\n",brief_print);
					printf(">sym   = %d\n",print_symbols);
					printf(">eta   = %d\n",extract_eta);
					printf(">xapp  = %d\n",extract_app);
					printf(">full  = %d\n",reduce_fully);
				}
			}
			break;
		case token_stream::EXT:
			{
				int inp = 0;
				if( extract_eta ) inp |= node::DO_EXTRACT_ETA;
				if( extract_app ) inp |= node::DO_EXTRACT_APP;
				exp_node* exp = extraction(penv,inp);
				arglst_node* env = 0;
				if( penv ) env = *penv;
				if( exp )
				{
					int inp = 0;
					if( brief_print )
						inp |= node::DO_PRINT_BRIEF;
					exp->print(env,inp);
					printf("\n");
					delete exp;
				}
			}
			break;
		default:
			break;
		}
	}else{
		tokstr_v->push_token(tok,ptok );
		exp = application();
	}
	if( tokstr_v ) tokstr_v->reset_token();
	node::reset();

	return exp;
}

exp_node* lambda_expression_parser::
application()
{
	exp_node* exp = 0;
	if( !tokstr_v ) return exp;
	token_stream::token_type tok;
	char* ptok;
	exp_node* left = 0;
	exp_node* right = 0;

	tok = tokstr_v->get_token(&ptok);
	switch( tok ){
	case token_stream::LAMBDA:
		tokstr_v->push_token(tok,ptok );
		exp = lambda();
		break;
	case token_stream::NAME:
	case token_stream::LPAREN:
		tokstr_v->push_token(tok,ptok );
		left = alist();
		right = lambda();
		if( !right ){
			exp = left;
		}else{
			exp = (exp_node*)new app_node(left,right,True);
		}
	case token_stream::EOE:
		break;
	default:
		dderrmsg("expresion expected: got %s", ptok?ptok:"(*null*)" );
		//tokstr_v->push_token(tok,ptok );
	}

	return exp;
}

exp_node* lambda_expression_parser::
lambda()
{
	exp_node* exp = 0;
	if( !tokstr_v ) return exp;
	token_stream::token_type tok;
	char* ptok;
	arg_node* arg = 0;
	exp_node* body = 0;

	tok = tokstr_v->get_token(&ptok);
	if( token_stream::LAMBDA!=tok ){
		//dderrmsg("^ expected: got %s", ptok?ptok:"(*null*)" );
		if( token_stream::EOE!=tok )
			tokstr_v->push_token(tok,ptok );
		return exp;
	}

	tok = tokstr_v->get_token(&ptok);
	if( token_stream::NAME!=tok ){
		dderrmsg("NAME expected: got %s", ptok?ptok:"(*null*)" );
		//tokstr_v->push_token(tok,ptok );
		return exp;
	}
	arg = new arg_node(ptok,0);

	tok = tokstr_v->get_token(&ptok);
	if( token_stream::PERIOD!=tok ){
		dderrmsg("PERIOD expected: got %s", ptok?ptok:"(*null*)" );
		//tokstr_v->push_token(tok,ptok );
		return exp;
	}

	body = application(); //expression();
	if( !body ) {
		delete arg;
	}else{
		exp = new lam_node(arg,body,True);
	}

	return exp;
}

exp_node* lambda_expression_parser::
alist()
{
	exp_node* exp = 0;
	if( !tokstr_v ) return exp;
	token_stream::token_type tok;
	char* ptok;

	for( tok = tokstr_v->get_token(&ptok);
		token_stream::NAME==tok ||
			token_stream::LPAREN==tok;
		tok = tokstr_v->get_token(&ptok) )
	{
		tokstr_v->push_token(tok,ptok);
		exp_node* anode = atom();
		if( anode ){
			if( exp )
			{
				exp = (exp_node*)
					new app_node( exp, anode, True);
			}else exp = anode;
		}
	}
	tokstr_v->push_token(tok,ptok );

	return exp;
}

exp_node* lambda_expression_parser::
atom()
{
	exp_node* exp = 0;
	if( !tokstr_v ) return exp;
	token_stream::token_type tok;
	char* ptok;

	tok = tokstr_v->get_token(&ptok);
	switch(tok)
	{
	case token_stream::LPAREN:
		exp = application();//expression();
		tok = tokstr_v->get_token(&ptok);
		if( token_stream::RPAREN!=tok ){
			dderrmsg(") expected: got %s", ptok?ptok:"(*null*)" );
			//if( token_stream::EOE!=tok )
			//	tokstr_v->push_token(tok,ptok );
		}	
		break;
	case token_stream::NAME:
		exp = (exp_node*)new var_node(ptok);
		break;
	default:
		tokstr_v->push_token(tok,ptok );
	}

	return exp;
}

/////////////////////////////////////////////////////////////////////
arglst_node* lambda_expression_parser::
definition(arglst_node** penv)
{
	arglst_node* arglst = 0;
	if( !tokstr_v ) return arglst;
	token_stream::token_type tok;
	char* ptok;
	tok = tokstr_v->get_token(&ptok);
	if( token_stream::NAME==tok ){
		arg_node* arg= new arg_node(ptok, 0);
		exp_node* exp= expression();
		if( penv ){
#define APPEND_DEFS
#undef APPEND_DEFS
#ifdef APPEND_DEFS
			if( arg ) arg->import_value(&exp);
			if( *penv ){
				arglst = (arglst_node*)((*penv)->add(arg,True));
				*penv = arglst;
			}
			else{
				arglst = new arglst_node(arg,0,True);
				*penv = arglst;
			}
#else
			if( *penv ){
				arg_node* old = (*penv)->find(arg);
				if( old ){
					old->import_value(&exp);
					if( arg ) delete arg;// arg not used!
				}else{
					if( arg ) arg->import_value(&exp);
					arglst = new arglst_node(arg,*penv,True);
					*penv = arglst;
					definition_env = arglst;
				}
			}else{
				if( arg ) arg->import_value(&exp);
				arglst = new arglst_node(arg,0,True);
	  			*penv = arglst;
				definition_env = arglst;
			}
#endif
		}
	}
	return arglst;
}

arglst_node* lambda_expression_parser::
load(arglst_node** penv)
{
	arglst_node* arglst = 0;
	if( !tokstr_v ) return arglst;
	token_stream::token_type tok;
	char* ptok;
	tok = tokstr_v->get_token(&ptok);
	if( token_stream::STRING==tok ){
		char* tk = strtok( ptok, "\"" );
		if( tk ) ptok = tk;
		tok = token_stream::NAME;
	}
	if( token_stream::NAME==tok  ){
		token_stream* loadstr = new token_stream(ptok);
		if( loadstr ){
			lambda_expression_parser parse(loadstr);
			while( !loadstr->get_read_EOF()
				&& !loadstr->get_read_error() )
				parse.expression(penv);
			delete loadstr;
		}
	}
	return arglst;
}

exp_node* lambda_expression_parser::
extraction(arglst_node** penv,int inp)
{
	exp_node* exp = 0;
	if( !tokstr_v ) return exp;
	token_stream::token_type tok;
	char* ptok;
	tok = tokstr_v->get_token(&ptok);
	if( token_stream::NAME==tok
	||  token_stream::LAMBDA==tok )
	{
		char* nm = 0;
		if( token_stream::NAME==tok
		&&  0 != strcmp(ptok, "~" ) )
		{
			nm = new char[strlen(ptok)+1];
			strcpy(nm,ptok);
		}
		exp_node* exp1 = expression();
		if( exp1 )
		{
			exp = exp1->extract(nm,inp);
			delete exp1;
		}
		if( nm ) delete nm;
	}
	return exp;
}

/////////////////////////////////////////////////////////////////////
void lambda_expression_parser::
set_tok_str(token_stream* tokstr)
{
	tokstr_v = tokstr;
}

void lambda_expression_parser::
reset()
{
	paren_nest_level_v = 0;
}

void lambda_expression_parser::
dderrmsg(char* format,...)
{
	va_list args;

	va_start(args, format);
	//format = va_arg(args,char*);
#if defined _WINDOWS
	char msg[512];
	sprintf(msg, "***line %d:\n", linenum);
	vsprintf(msg+strlen(msg), format, args );
	AfxMessageBox(msg);
#else
	if(tokstr_v->get_linenum()>=0)
		printf("\n*** line %d: ",
			tokstr_v->get_linenum());
	vfprintf(stdout,format,args);
	printf("\n");
#endif
	va_end(args);
	fflush(stderr);
}
