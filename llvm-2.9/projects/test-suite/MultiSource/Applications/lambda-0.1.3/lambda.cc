/* lambda.cc */
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
#include "node.h"
#include "token_stream.h"
#include "parse.h"

#ifdef WIN32
#include <direct.h>

#else
#include <unistd.h>
#include <sys/param.h>
#endif

static const char *mybasename(const char *str) {
  const char *base = strrchr(str, '/');
  return base ? base+1 : str;
}

int
main(int argc, char** argv)
{
	printf(
		"Copyright (c) 2000 John A. Maiorana. "
		"All rights reserved.\n" );
	#ifdef WIN32
	{
		char buf[512];
		char* cwd = _getcwd(buf,sizeof(buf)-1);
		int dr = _getdrive();
		if( cwd ){
			printf( "%s\n", cwd);
		}
	}
	#else
	{
		char buf[MAXPATHLEN+1];
		char* cwd = getcwd(buf,sizeof(buf)-1);
		if( cwd ){
                        printf( "%s\n", mybasename(cwd));
		}
	}
	#endif
#if 0
	arg_node arg1("x",0);
	var_node varx("x");
	var_node vary("y");
	var_node varf("f");
	app_node appxx(&varx, &varx);
	app_node appfxx(&varf, &appxx);
	lam_node lamD(&arg1,&appfxx);
	app_node appDD(&lamD,&lamD);
	printf("(^x.f(x x)) (^x.f(x x)) = ");
	appDD.print();
	printf("\n");
	appDD.bind();

	printf("reduced = ");
	exp_node* rn = appDD.reduce();
	rn->print();
	printf("\n");
#endif
	token_stream tokstr;
	tokstr.open();
	lambda_expression_parser parse(&tokstr);

	arglst_node* env = 0;
	while( 1 )
	{
		printf("<< ");
		exp_node* exp = parse.expression(&env);
		if( exp )
		{
			int inp = 0;
			int outp = 0;
			if( node::ARG==exp->op() )
			{
				delete exp;
				break;
			}
			printf("==> ");
			exp->print();
			printf("\n");
			//exp->bind(env);
			node::reset();
			if( trace_lambda ) inp |= node::DO_TRACE;
			if( print_symbols ) inp |= node::DO_PRINT_SYM;
			if( applicative_order ) inp |= node::DO_APP_ORDER;
			if( reduce_body ) inp |= node::DO_REDUCE_BODY;
			if( step_lambda ) inp |= node::DO_STEP;
			if( brief_print ) inp |= node::DO_PRINT_BRIEF;
			if( reduce_fully ) inp |= node::DO_REDUCE_FULLY;

			int exp_was_var = node::VAR==exp->op(); 

			if( step_lambda )
			{
				int printed = 0;
				exp_node* rexp = exp->reduce(env,inp,&outp);
				do {
					if( rexp ){
						if(1){
							char* step = "=";
							if( outp & node::BETA_ETA_DONE ){
								if( outp & node::ETA_DONE ){
									step = "H";
								}else{
									step = "B";
								}
							}
							printf("=%1.1s==> ", step );
							//rexp->symbolic_print(env,inp);
							if( exp_was_var )
								rexp->print(env,inp&~node::DO_PRINT_SYM);
							else
								rexp->symbolic_print(env,inp);
							printf("\n");
							printed = 1;
						}
						exp_node* nexp;
						if( outp & node::BETA_ETA_DONE) {
							outp &= ~(node::BETA_ETA_DONE|node::ETA_DONE);
							nexp = rexp->reduce(env,inp,&outp);
							if( nexp && rexp!=nexp ){
								//if( rexp != exp ) delete rexp;
								delete rexp;
								if( rexp == exp ) exp = 0;
								rexp = nexp;
							}
							if( !(outp & node::BETA_ETA_DONE))
								break;
						}else{
							break;
						}
						printf("*** Continue?[y/n]:");
						int ans = getchar();
						if( '\n'!=ans )
							while ( '\n'!=getchar() );
						if( 'n'==ans || 'N'==ans ) break;
					}
				} while (rexp);
				if( !printed && rexp ){
					printf("=====> ");
					if( exp_was_var )
						rexp->print(env,inp&~node::DO_PRINT_SYM);
					else
						rexp->symbolic_print(env,inp);
					printf("\n");
					printed = 1;
					//if( rexp && rexp != exp ) delete rexp;
					delete rexp;
					if( rexp == exp ) exp = 0;
					rexp = 0;
				}
			}else if( step_thru ) {
				inp |= node::DO_STEP;
				exp_node* rexp = exp->reduce(env,inp,&outp);			
				do {
					if( rexp ){
						exp_node* nexp;
						if( outp & node::BETA_ETA_DONE) {
							outp &= ~node::BETA_ETA_DONE;
							nexp = rexp->reduce(env,inp,&outp);
							if( nexp && rexp!=nexp ){
								//if( rexp != exp ) delete rexp;
								delete rexp;
								if( rexp == exp ) exp = 0;
								rexp = nexp;
							}
							if( !(outp & node::BETA_ETA_DONE))
								break;
						}else{
							break;
						}
					}
				} while (rexp);
				if( rexp )
				{
					printf("====>");
					if( exp_was_var )
						rexp->print(env,inp&~node::DO_PRINT_SYM);
					else
						rexp->symbolic_print(env,inp);
					printf("\n");
					//if( rexp && rexp != exp ) delete rexp;
					delete rexp;
					if( rexp == exp ) exp = 0;
					rexp = 0;
				}
			}else{
				exp_node* rexp = exp->reduce(env,inp,&outp);			
				if( rexp )
				{
					printf("====>");
					if( exp_was_var )
						rexp->print(env,inp&~node::DO_PRINT_SYM);
					else
						rexp->symbolic_print(env,inp);
					printf("\n");
					//if( rexp && rexp != exp ) delete rexp;
					delete rexp;
					if( rexp == exp ) exp = 0;
					rexp = 0;
				}
			}
			if( exp ) delete exp;
			exp = 0;
		}
	}
	tokstr.close();
	return 0;
}

