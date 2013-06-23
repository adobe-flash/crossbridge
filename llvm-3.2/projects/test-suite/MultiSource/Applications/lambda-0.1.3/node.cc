/* node.cc */
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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <config.h>
#include "node.h"

#define REDUCE_VARS
//#undef REDUCE_VARS
#define USE_UNDER_SCORE_PREFIX_FOR_GENERATED_NAMES
#undef USE_UNDER_SCORE_PREFIX_FOR_GENERATED_NAMES

//const int MAX_RECURSE=200;
const int MAX_RECURSE=4000;

const arglst_node* definition_env = 0;

static int name_sequence = 1;
static int lambda_reduce_recurse_level = 0;
static int app_reduce_recurse_level = 0;
static int var_reduce_recurse_level = 0;

static char* newname(const char* oldname)
{
	char* nm = 0;
	char buf[128];
	if( !oldname ) oldname = "~";

#ifdef USE_UNDER_SCORE_PREFIX_FOR_GENERATED_NAMES
	sprintf( buf, "_%s_%d", oldname, name_sequence );
#else
	sprintf( buf, "%s#%d", oldname, name_sequence );
#endif
	name_sequence += 1;

	nm = new char[strlen(buf)+1];
	strcpy(nm,buf);
	return nm;
}

char* newstring(const char*os)
{
	char* ns;
	if(os) {
		ns = new char[strlen(os)+1];
		strcpy(ns,os);
	}else ns = 0;
	return ns;
};

//////////////////////////////////////////////////////////////////////
void node::
reset()
{
	name_sequence = 1;
	lambda_reduce_recurse_level = 0;
	app_reduce_recurse_level = 0;
	var_reduce_recurse_level = 0;
}

void node::
print(const alst_node*env,int inp) const
{
	printf("(%p:node)",this);
}

//////////////////////////////////////////////////////////////////////
// arg_node
arg_node::
arg_node(const char* nm, const exp_node* ex, Boolean import)
{
	this->name_v = newstring(nm);
	if( import ){
		if( ex ) value_v = (exp_node*)ex;
		else value_v = 0;
	}else{
		if( ex ) value_v = (exp_node*)ex->clone();
		else value_v = 0;
		if( value_v ) value_v->set_parent(this);
	}
}

arg_node::
arg_node(const arg_node& from)
{
	//*this = from;
	if( from.name_v ) {
		name_v = new char [strlen(from.name_v)+1];
		strcpy(name_v, from.name_v);
	}else name_v = 0;
	if( from.value_v )
		value_v = 
			(exp_node*)from.value_v->clone();
	else value_v = 0;
	if( value_v ) value_v->set_parent(this);
	parent_v = from.parent_v;
}

node* arg_node::
clone() const
{
	return (node*)new arg_node(*this);
}

arg_node::
~arg_node()
{
	if( name_v ) delete [] name_v;
	if( value_v ) delete value_v;
}

arg_node& arg_node::
operator =(const arg_node& from)
{
	if( this != &from ){
		this->~arg_node();
		if( from.name_v ) {
			name_v = new char [strlen(from.name_v)+1];
			strcpy(name_v, from.name_v);
		}else name_v = 0;
		if( from.value_v )
			value_v = 
				(exp_node*)from.value_v->clone();
		else value_v = 0;
		if( value_v ) value_v->set_parent(this);
		parent_v = from.parent_v;
	}
	return *this;
}

//Boolean arg_node::
//free_in(exp_node* exp)
//{
//	Boolean in = False;
//	in = exp->has_free(this);
//	return in;
//}

void arg_node::
print(const alst_node*env, int inp)const
{
	printf("%s", name_v ? name_v : "(null)" );
	if( value_v ) {
		printf(" = ");
		value_v->print(env,inp);
	}
}

void arg_node::
import_value(exp_node** val)
{
	if( value_v ) delete value_v;
	if( val ){
		value_v = *val;
		*val = 0;
	}else value_v = 0;
	if( value_v ) value_v->set_parent(this);
}

int arg_node::
operator == (const arg_node& cmp) const
{
	Boolean b1 = 
		!name_v && !cmp.name_v ||
		name_v && cmp.name_v && 0==strcmp(name_v, cmp.name_v);
	return b1;
}

int arg_node::
operator == (const char* cmp) const
{
	Boolean b1 = 
		!name_v && !cmp ||
		name_v && cmp && 0==strcmp(name_v, cmp);
	return b1;
}

/////////////////////////////////////////////////////////////////////
// exp_node's
const alst_node* exp_node::
match(const alst_node* env ) const
{
	const alst_node* match = 0;
	for(const alst_node* en=env; en; en = en->next() )
	{
		if( en->arg() && en->arg()->value() ){
			if( *this==*(en->arg()->value()) )
			{
				match = en;
				break;
			}
		}
	}
	return match;
}

void exp_node::
symbolic_print(const alst_node*env,int inp)const
{
	if( inp&node::DO_PRINT_SYM ){
		exp_node* exp = (exp_node*)this->clone();
		if( exp ){
			exp_node* pexp = exp->extract_defs(env);
			if( pexp ){
				pexp->print(env,inp);
				if( pexp != exp ) delete pexp;
			}else{
				exp->print(env,inp);
			}
			delete exp;
		}else{
			this->print(env,inp);
		}
	}else{
		this->print(env,inp);
	}
}

/////////////////////////////////////////////////////////////////////
// var_node
var_node::
var_node(const char* nm)
{
	if( nm ){
		name_v = new char[strlen(nm)+1];
		strcpy(name_v,nm);
	}else nm = 0;
}

var_node::
var_node(const var_node& from)
{
	//*this = from;
	if( from.name_v ) {
		name_v = new char [strlen(from.name_v)+1];
		strcpy(name_v, from.name_v);
	}else name_v = 0;
	parent_v = 0;//from.parent_v;
}

Boolean var_node::
has_free(const arg_node* argr, const alst_node* env) const
{
//	const exp_node* exp=this;
	#ifndef REDUCE_VARS
		exp = reduce_value(env);
		//if( exp != this )
		if( VAR!=exp->op() )
			return exp->has_free(argr, env);
	#endif
#undef MATCH_NAME
#ifdef MATCH_NAME
	//if( argr ) return argr->operator==(name_v);
	if( argr ) return *argr==(exp->name());
#else
	if( argr ){
		const alst_node* bd = this->bind(env);
		if( bd && (bd->arg()==argr))
		{
			return True;
		}
	}
#endif
	return False;
}

arglst_node* var_node::
bind(const alst_node* env) const
{
	//arg_node* arg = 0;
	//if( !this->name() ) return arg;
	//if( ref_arg_v ) return;
	if( !this->name() ) return 0;
#if 0
	for( node* par = parent_v; par; par = par->parent() )
	{
		if( LAM==par->op() && par->arg() && par->arg()->name() ){
			if( 0==strcmp(par->arg()->name(),
				          this->name()))
			{
				arg = par->arg();
				break;
			}
		}
//		if( arg ) break;
	}
#endif
	//if( !arg ) {
		alst_node* lst;
		for( lst=(arglst_node*)env;
			lst; lst=(arglst_node*)lst->next() )
		{
			if( lst->arg() && lst->arg()->name() ){
				if( 0==strcmp(lst->arg()->name(),
					          this->name()))
				{
					//arg = lst->arg();
					break;
				}
			}
		}
	//}
	////if( arg ){ref_arg_v = arg;if( def_arg_v ) delete def_arg_v;def_arg_v = 0;}
	return (arglst_node*)lst;
}

void var_node::
print(const alst_node*env,int inp) const
{
	printf("%s", name() ? name() : "(null-var)" );
}

var_node& var_node::
operator =(const var_node& from)
{
	if( name_v ) delete [] name_v;
	if( from.name_v ) {
		name_v = new char [strlen(from.name_v)+1];
		strcpy(name_v, from.name_v);
	}else name_v = 0;
	if( this != &from ){
		parent_v = from.parent_v;
	}
	return *this;
}

void var_node::
set_name(const char* nm)
{
	if( name_v == nm ) return;
	if( name_v ) delete [] name_v;
	name_v = nm?new char[strlen(nm)+1]:0;
	if( name_v && nm ) strcpy(name_v,nm);
}

exp_node* var_node::
reduce_value(const alst_node* env, int inp, int* outp,
	const alst_node** pnenv)
{
	exp_node* exp = this;
	exp_node* val;

	val = exp;
	int reduced = 0;
	int cnt = 0;
	int max_loops = (inp & DO_REDUCE_FULLY) ? 2 : 0;
	do {
		do {
			env = val->bind(env);
			if( !val ) goto end;
			arg_node* barg = env ? env->arg() : 0;
			exp_node* nval = barg ? barg->value() : 0;
			if( !nval ) break;
			if( val == nval ) goto end;
			if( val == nval ) break;
			val = nval;
			reduced = 1;
		} while (env && val && VAR==val->op() && val!=this );
		env = definition_env;
		cnt++;
	} while (reduced && cnt < max_loops);
end:
	if( val ) exp = val;
	if( pnenv ){
		//if( env ) env = env->next();
		//*pnenv = (alst_node*)env;
		*pnenv = definition_env;
	}
	return exp;
}

exp_node* var_node::
reduce_number()
{
	exp_node* exp = 0;
	if( name() )
	{
		int is_number = 1;
		for(const char*pc = name();*pc!='\0';pc++)
		{
			int ch = *pc;
			if( ch < '0' || ch > '9' )
			{
				is_number = 0;
				break;
			}
		}
		if( is_number )
		{
			arg_node* arg1 = new arg_node("m",0);
			arg_node* arg2 = new arg_node("n",0);
			exp_node *bdy = new var_node("n");
			int number = atoi((const char*)name());
			for( ; number > 0; number -= 1 )
			{
				var_node* lft = new var_node("m");
				bdy = new app_node(lft,bdy,True);
			}
			lam_node* lam2 = new lam_node(arg2,bdy,True);
			lam_node* lam1 = new lam_node(arg1,lam2,True);
			exp = lam1;
		}
	}
	return exp;
}

exp_node* var_node::
reduce(const alst_node* env, int inp,int* outp)
{
	var_reduce_recurse_level++;
	if( var_reduce_recurse_level > MAX_RECURSE ){
		printf("\nvar_reduce_recurse_level %d [",
			var_reduce_recurse_level);
		this->print();
		printf ("]\n");
		var_reduce_recurse_level--;
		if( outp ) *outp |= node::RECURSE_LVL;
		return this;
	}
	const alst_node* nenv = 0;
	exp_node* exp = this->reduce_value(env, inp, outp, &nenv);
	if( exp != this )
	{
		exp = (exp_node*)exp->clone();
		if( nenv && (APP==exp->op() || LAM==exp->op())
		&& (inp&DO_REDUCE_FULLY)!=0 )
		{
			exp_node* nexp = exp->reduce_vars(nenv,inp,outp);
			if( nexp && nexp != exp )
			{
				delete exp;
				exp = nexp;
			}
		}
	}
	exp_node* num = reduce_number();
	if( num )
	{
		if( exp != this ){
			delete exp;
			exp = num;
		}else{
			exp = num;
		}
	}
	var_reduce_recurse_level--;
	return exp;
}

void var_node::
rename(arg_node* arg, const char* newname, alst_node* env)
{
	arglst_node* lst = this->bind(env);
	arg_node* barg = lst ? lst->arg() : 0;
	if( barg && barg==arg ) this->set_name(newname);
}

int var_node::
operator == (const exp_node& cmp) const
{
	Boolean b1 = False;
	if( VAR!=cmp.op() ) return b1;
	b1 = !name() && !cmp.name() ||
		name() && cmp.name() && 0==strcmp(name(), cmp.name());
	return b1;
}

exp_node* var_node::
extract(const char* nm, int inp) const
{
	if( name() && nm && 0==strcmp(nm,name()) )
	{
		return (exp_node*)new var_node("I");
	}else
	if( nm )
	{
		exp_node* lft = new var_node("K");
		exp_node* rgt = new var_node(name());
		return (exp_node*)new app_node(lft,rgt,True);
	}else{
		return (exp_node*)new var_node(name());
	}
}


//////////////////////////////////////////////////////////////////////
// lam_node
lam_node::
lam_node(arg_node* arg, exp_node* bdy, Boolean import)
{
	arg_v = 0;
	body_v = 0;
	set_arg(arg,import);
	set_body(bdy,import);
}

lam_node::
lam_node(const lam_node& from)
{
	//*this = from;
	if( from.arg_v )
		arg_v = (arg_node*)from.arg_v->clone();
	else arg_v = 0;
	if( arg_v ) arg_v->set_parent(this);
	if( from.body_v )
		body_v = (exp_node*)from.body_v->clone();
	else body_v = 0;
	if( body_v ) body_v->set_parent(this);
	parent_v = 0;//from.parent_v;
}

lam_node::
~lam_node()
{
	if( arg_v ) delete arg_v;
	if( body_v ) delete body_v;
}

Boolean lam_node::
has_free(const arg_node* argr, const alst_node* env) const
{
	stack_frame nenv(arg_v,env);
	if( arg_v && arg_v->operator ==(*argr) ) return False;
	if( body_v ) return body_v->has_free(argr, (alst_node*)&nenv);
	return False;
}

void lam_node::
print(const alst_node*env, int inp) const
{
	int printed = 0;
	if( (inp&DO_PRINT_SYM) )
	{
		if( env )
		{
			const alst_node* alst = this->match(env);
			if( alst ){
				printf("%s", alst->arg()->name());
				printed = 1;
			}
		}
		if( !printed )
		{
			int value = 0;
			if( LAM==body()->op()
			&& arg()
			&& arg()->name()
			&& body()
			&& body()->arg()
			&& body()->arg()->name()
			&& body()->body())
			{
				const char* name1 = arg()->name();
				const char* name2 = body()->arg()->name();
				for( exp_node* numbody = body()->body();
					numbody; numbody = numbody->right() )
				{
					if( APP==numbody->op()
					&& numbody->left()
					&& VAR==numbody->left()->op()
					&& numbody->left()->name()
					&& 0==strcmp(name1,numbody->left()->name())
					){
						value += 1;
					}else
					if( VAR==numbody->op()
					&&  numbody->name()
					&&  0==strcmp(name2,numbody->name())
					){
						break;
					}else{
						value = -1;
						break;
					}
				}
				if( value >= 0 )
				{
					printf( "%d",value);
					printed = 1;
				}
			}else
			if( arg()
			&& arg()->name()
			&& body()
			&& VAR==body()->op()
			&& 0==strcmp(arg()->name(), body()->name()) )
			{
				printf( "I");
				printed = 1;
			}
		}
	}
	if( !printed )
	{
		printf("^");
		//if( arg() ) arg()->print(env,inp);
		if( arg() && arg()->name() ) printf("%s",arg()->name());
		else printf("(null-arg)");
		if( arg() && arg()->value() ){
			printf("[");
			arg()->value()->print(env,inp);
			printf("]");
		}
		printf(".");
		if( body() ) body()->print(env,inp);
		else printf("(null-body)");
	}
}

lam_node& lam_node::
operator =(const lam_node& from)
{
	if( this!=&from ){
		this->~lam_node();
		if( from.arg_v )
			arg_v = (arg_node*)from.arg_v->clone();
		else arg_v = 0;
		if( arg_v ) arg_v->set_parent(this);
		if( from.body_v )
			body_v = (exp_node*)from.body_v->clone();
		else body_v = 0;
		if( body_v ) body_v->set_parent(this);
		parent_v = 0;//from.parent_v;
	}
	return *this;
}

exp_node* lam_node::
reduce(const alst_node* env, int inp, int* outp)
{
	exp_node* exp = this;
	stack_frame nenv(this->arg_v, (arglst_node*)env );
	if(inp&DO_TRACE){
		for(int idx=0;idx<lambda_reduce_recurse_level;idx++)printf(".");
		printf("L:  ");
		symbolic_print(env, inp);
		printf("\n");
		exp_node* par;
		for(par=this;par->parent();par=(exp_node*)par->parent() );
		par->symbolic_print(env,inp);
		printf("\n");
	}
	lambda_reduce_recurse_level++;
	if( lambda_reduce_recurse_level > MAX_RECURSE ){
		printf("\nlambda_reduce_recurse_level %d\n",
			lambda_reduce_recurse_level);
		lambda_reduce_recurse_level--;
		if( outp ) *outp |= node::RECURSE_LVL;
		return this;
	}
	body_v->set_parent(this);
	if( arg_v && arg_v->name() && '&'==arg_v->name()[0] )
	{
		inp &= ~DO_STEP;
		inp &= ~DO_REDUCE_BODY;
	}
	if( arg_v && arg_v->value() && body_v ){
		body_v->resolve_name_clash(arg_v,&nenv);
		//old delet -- body_v->bind(nenv);
		//
		if( arg_v->name() && '$'==arg_v->name()[0] )
		{
			inp &= ~DO_STEP;
			inp |= DO_REDUCE_BODY;
		}
		if( !(inp&DO_STEP) ){
			int toutp = outp ? *outp : 0;
			exp = body_v;
			exp = body_v->reduce_vars((alst_node*)&nenv,inp,outp);
			if( exp!=body_v ) set_body(0,True);
			arg_v->import_value((exp_node*)0);
int cnt = 0;
			do {
				exp_node* nexp = 0;
				nexp = exp->reduce((alst_node*)&nenv,inp,&toutp);
				if( nexp && exp != nexp ){
					if( exp==body_v ) body_v = 0;
					delete exp;
					exp = nexp;
				}
				if( toutp && toutp&node::RECURSE_LVL ) break;
				if( toutp & node::BETA_ETA_DONE){
					toutp &= ~node::BETA_ETA_DONE;
				}else break;
cnt++;
			} while( exp );
			if( outp ) *outp = toutp;
if( cnt > 1 )
{
printf("[%d.%d]",cnt,lambda_reduce_recurse_level);
}
		}else{
			exp = body_v->reduce_vars((alst_node*)&nenv,inp,outp);
		}
		if( body_v && body_v!=exp ) delete body_v;
		this->body_v = 0;
		if( exp ) exp->set_parent(parent_v);
		//if( parent() ) delete this;
	}else if( body_v ){
		exp_node* nexp = eta_reduce( &nenv,inp,outp);
		if( exp!=nexp ){
			if( exp != this ) delete exp;
			exp = nexp;
		}else{
			int toutp = outp ? *outp : 0;
			if( inp&DO_REDUCE_BODY ){
				exp_node* nd =
					body_v->reduce((alst_node*)&nenv,inp,&toutp);
				if( nd ) set_body(nd,True);
			}else{
				exp_node* nd =
					body_v->reduce_vars((alst_node*)&nenv,inp,outp);
				if( nd ) set_body(nd,True);
			}
			if( !(inp&DO_STEP) || !(toutp&node::BETA_ETA_DONE) ){
				nexp = eta_reduce( &nenv,inp, &toutp);
				if( exp!=nexp ){
					if( exp != this ) delete exp;
					exp = nexp;
				}
			}
			if( outp ) *outp = toutp;
		}
	}
	lambda_reduce_recurse_level--;
	if(0 && inp&DO_TRACE ){
		for(int idx=0;idx<lambda_reduce_recurse_level;idx++)printf(".");
		printf("L> ");
		//exp->print(env, inp);
		exp->set_parent(this->parent());
		exp_node* par;
		for(par=exp;par->parent();par=(exp_node*)par->parent() );
		par->symbolic_print(env,inp);
		printf("\n");
	}
	return exp;
}

exp_node* lam_node::
eta_reduce(const alst_node* env,int inp,int* outp)
{
	// assume stack frame already set>
	exp_node* exp = this;
	if( !body_v ) return exp;
	if( !(inp&DO_STEP) || !outp || !(*outp&node::BETA_ETA_DONE))
	{
		if(APP==body_v->op() && !(inp&DO_APP_ORDER) ){
			// check for  eta reduction
			if( body()->left() )
			{
#ifdef REDUCE_VARS
				exp_node* nd = body_v->left()->reduce_vars(env,inp,outp);
				if( nd ) ((app_node*)body())->set_left(nd,True);
#endif
				if( body_v->right() && arg_v
					&& *arg_v == body_v->right()->name()
					&& !body_v->left()->has_free(arg_v, env) )
				{
					// do eta reduction
					exp = this->body_v->export_left();
					//this->body_v = 0; // right_v memory leak?
					set_body(0,true);
					if( outp ) *outp |=
						(node::BETA_ETA_DONE | node::ETA_DONE);
					if( !(inp&DO_STEP ) ){
						exp_node* nexp = exp->reduce(env,inp,outp);
						if( nexp && exp != nexp ){
							if( exp ) delete exp;
							exp = nexp;
						}
					}
				}
			}
		}
	}
	return exp;
}

exp_node* lam_node::
reduce_vars(const alst_node* env,int inp,int* outp)
{
	stack_frame nenv(this->arg_v, (arglst_node*)env );

	exp_node* nd = body_v->reduce_vars((alst_node*)&nenv,inp,outp);
	if( nd ) set_body(nd,True);

	return this;
}

void lam_node::
rename(arg_node* arg, const char* newname, alst_node* env)
{
	stack_frame nenv(this->arg_v, (arglst_node*)env );
	if( body_v ) body_v->rename(arg,newname,&nenv);
}

void lam_node::
resolve_name_clash( arg_node* outer_arg,alst_node* env)
{
	stack_frame nenv(this->arg_v, (arglst_node*)env );
	if( outer_arg->value() ){
		arg_node* inner_arg = this->arg();
		exp_node* inner_body = this->body();
		if( inner_arg && inner_body
			&& !(inner_arg==outer_arg))
		{
			Boolean b1 = outer_arg->value()
				->has_free(inner_arg, (alst_node*)&nenv);
			Boolean b2 =
				inner_body->has_free(outer_arg, (alst_node*)&nenv);
			if( b1 && b2 )
			{
				char* nm = newname(inner_arg->name());
				body_v->rename(inner_arg,nm,(alst_node*)&nenv);
				inner_arg->set_name(nm);
				delete [] nm;
			}
		}
		if( body_v && body_v->has_free(outer_arg, &nenv) )
			body_v->resolve_name_clash(outer_arg, &nenv);
	}
}

void lam_node::
set_arg(arg_node* nd, Boolean import)
{
	if( arg_v == nd ) return;
	if( arg_v ) delete arg_v;
	if( import ) arg_v = nd;
	else if( nd ) arg_v = (arg_node*)nd->clone();
	else arg_v = 0;
	if( arg_v ) arg_v->set_parent(this);
}

void lam_node::
set_body(exp_node* nd, Boolean import)
{
	if( body_v == nd ) return;
	if( body_v ) delete body_v;
	if( import ) body_v = nd;
	else if( nd ) body_v = (exp_node*)nd->clone();
	else body_v = 0;
	if( body_v ) body_v->set_parent(this);
}

int lam_node::
operator == (const exp_node& cmp) const
{
	Boolean b1 = False;
	if( LAM!=cmp.op() ) return b1;
	if( !arg() || !cmp.arg() ) return b1;
	if( !body() || !cmp.body() ) return b1;
	b1 = *arg() == *(cmp.arg()) 
		&& *body() == *(cmp.body());
	return b1;
}

exp_node* lam_node::
extract_defs(const alst_node* env)
{
	exp_node* exp = this;
	const alst_node* alst = this->match(env);
	if( !alst ){
		if( body() ) {
			exp_node* nb = body()->extract_defs(env);
			if( nb )
				set_body(nb,True);
			alst = this->match(env);
			if( alst )
				 exp = new var_node(alst->arg()->name());
		}
	}else{
		exp = new var_node(alst->arg()->name());
	}
	return exp;
}

exp_node* lam_node::
extract(const char* nm, int inp) const
{
	exp_node* exp = 0;
	if( body() && arg() )
	{
		exp_node* bdy = body()->extract(arg()->name(),inp);
		if( bdy && nm )
		{
			exp = bdy->extract(nm,inp);
			delete bdy;
		} else {
			exp = bdy;
		}
	}
	return exp;
}

//////////////////////////////////////////////////////////////////////
// app_node
app_node::
app_node(exp_node* lft,
				   exp_node* rgt,
				   Boolean import)
{
	left_v = 0;
	right_v = 0;
	set_left(lft,import);
	set_right(rgt,import);
}

app_node::
app_node(const app_node& from)
{
	//*this = from;
	if( from.left_v )
		left_v = (exp_node*)from.left_v->clone();
	else left_v = 0;
	if( left_v ) left_v->set_parent(this);
	if( from.right_v )
		right_v =
			(exp_node*)from.right_v->clone();
	else right_v = 0;
	if( right_v) set_parent(this);
	parent_v = from.parent_v;
}
	
app_node::
~app_node()
{
	if( left_v ) delete left_v;
	if( right_v ) delete right_v;
}

Boolean app_node::
has_free(const arg_node* argr, const alst_node* env) const
{
	Boolean has = False;
	if( left_v ) has =  has || left_v->has_free(argr,env);
	if( right_v ) has = has || right_v->has_free(argr,env);
	return has;
}

void app_node::
print(const alst_node*env,int inp) const
{
	if( inp&DO_PRINT_BRIEF ){
		int left_paren = 1;
		int right_paren = 1;
		int left_right_paren = 0;
		int left_match = 0;
		int right_match = 0;
		int left_right_match = 0;
		if( left() ){
			if( inp&DO_PRINT_SYM )
				left_match = left()->match(env) ? 1 : 0;
			if( VAR==left()->op() || APP==left()->op() || left_match )
			{
				left_paren = 0;
			}
			if( APP==left()->op() && left()->right() )
			{
				if( inp&DO_PRINT_SYM )
					left_right_match =
						left()->right()->match(env) ? 1 : 0;
				if( VAR!=left()->right()->op() && !left_right_match )
					left_right_paren = 1;
			}
			if( left_paren ) printf("(");
			if( left() ) left()->print(env,inp);
			else printf("(null-right)");
			if( left_paren ) printf(")");
		}else printf("()");
		if( right() ){
			if( inp&DO_PRINT_SYM )
				right_match = right()->match(env) ? 1 : 0;
			if( VAR==right()->op() || right_match ) right_paren = 0;
			if( right_paren ) printf("(");
			else if( !left_paren && !left_right_paren )
				printf(" ");
			if( right() ) right()->print(env,inp);
			else printf("(null-right)");
			if( right_paren ) printf(")");
		}else printf("()");
	}else{
		if( left() ){
			if( VAR!=left()->op()||left()->value() ) printf("(");
			if( left() ) left()->print(env,inp);
			else printf("(null-right)");
			if( VAR!=left()->op()||left()->value() ) printf(")");
		}else printf("()");
		if( right() ){
			if( VAR!=right()->op()||right()->value() ) printf("(");
			else printf(" ");
			if( right() ) right()->print(env,inp);
			else printf("(null-right)");
			if( VAR!=right()->op()||right()->value() ) printf(")");
		}else printf("()");
	}
}

app_node& app_node::
operator =(const app_node& from)
{
	if( this!=&from ){
		this->~app_node();
		if( from.left_v )
			left_v = (exp_node*)from.left_v->clone();
		else left_v = 0;
		if( left_v ) left_v->set_parent(this);
		if( from.right_v )
			right_v = (exp_node*)from.right_v->clone();
		else right_v = 0;
		if( right_v ) right_v->set_parent(this);
		parent_v = from.parent_v;
	}
	return *this;
}

void app_node::
rename(arg_node* arg, const char* newname, alst_node* env)
{
	if( left_v ) left_v->rename(arg,newname,env);
	if( right_v ) right_v->rename(arg,newname,env);
}

void app_node::
resolve_name_clash( arg_node* outer_arg,alst_node* env)
{
	if( left_v ) left_v->resolve_name_clash(outer_arg,env);
	if( right_v ) right_v->resolve_name_clash(outer_arg,env);
}

exp_node* app_node::
reduce(const alst_node* env, int inp,int* outp)
{
	exp_node* exp = this;
	app_reduce_recurse_level += 1;
	if( app_reduce_recurse_level > MAX_RECURSE ){
		printf("\napp_reduce_recurse_level %d\n",
			app_reduce_recurse_level);
		app_reduce_recurse_level--;
		if( outp ) *outp |= node::RECURSE_LVL;
		return this;
	}
	
	if( left_v ){
		int ninp = inp & ~DO_REDUCE_BODY;
		left_v->set_parent(this);
		exp_node* nd = left_v->reduce(env,ninp,outp);
		set_left(nd,True);
		if( outp && *outp&node::RECURSE_LVL ){
			app_reduce_recurse_level--;
			return exp;
		}
		if( inp&node::DO_STEP && outp && (*outp&node::BETA_ETA_DONE) )
		{
			app_reduce_recurse_level--;
			return exp;
		}
	}
	if( left_v && APP==left_v->op() ){
		exp_node* nd = 0;
		int ninp = inp & ~DO_REDUCE_BODY;
		left_v->set_parent(this);
		if( left_v ) nd = left_v->reduce(env,ninp,outp);
		set_left(nd, True);
		if( outp && *outp&node::RECURSE_LVL )
		{
			app_reduce_recurse_level--;
			return exp;
		}
		if( inp&node::DO_STEP && outp && (*outp&node::BETA_ETA_DONE) )
		{
			app_reduce_recurse_level--;
			return exp;
		}
		if( right_v ){
			nd = 0;
			right_v->set_parent(this);
#ifdef REDUCE_VARS
			nd = right_v->reduce_vars(env,inp,outp);
			set_right(nd,True);
#endif
		}
	}
	if( left_v && LAM==left_v->op() )
	{
		int app_inp = inp;
		if( left_v->arg()
		&&  left_v->arg()
		&&  left_v->arg()->name() )
		{
			if('@'==left_v->arg()->name()[0] )
			{
				app_inp |= DO_APP_ORDER;
			}else
			if( '#'==left_v->arg()->name()[0] )
			{
				app_inp &= ~DO_APP_ORDER;
			}
		}
		if( right_v )
		{
			Boolean has = False;
			right_v->set_parent(this);
			if( app_inp&DO_APP_ORDER ){
				arg_node* an = left_v->arg();
				exp_node* bd = left_v->body();
				stack_frame nenv(an, (arglst_node*)env );
				has = bd && an && bd->has_free(an, &nenv);
			}
			if( (app_inp&DO_APP_ORDER) && has ){
				int ninp = inp & ~DO_REDUCE_BODY;
				set_right(right_v->reduce(env, ninp, outp),True);
				if( outp && (*outp&node::RECURSE_LVL) )
				{
					app_reduce_recurse_level--;
					return exp;
				}
				if( (inp&node::DO_STEP)
					&& outp && (*outp&node::BETA_ETA_DONE) )
				{
					app_reduce_recurse_level--;
					return exp;
				}
			}else{
#ifdef REDUCE_VARS
				set_right(right_v->reduce_vars(env,inp,outp),True);
				if( outp && (*outp&node::RECURSE_LVL) )
				{
					app_reduce_recurse_level--;
					return exp;
				}
#endif
			}
		}
		left_v->arg()->import_value(&right_v);
		right_v = 0;
		left_v->set_parent(this);
		exp = left_v->reduce(env,inp,outp);
		if( left_v && left_v!=exp ) delete left_v;
		left_v = 0;
		if( exp ) exp->set_parent(this);
		if( outp ) *outp |= node::BETA_ETA_DONE;
	}else{
		if( right_v ){
			right_v->set_parent(this);
#ifdef REDUCE_VARS
			set_right(right_v->reduce_vars(env,inp,outp),True);
#endif
			set_right(right_v->reduce(env,inp,outp),True);

			#if 0
				if( right_v && APP==right_v->op()
					&& right_v->left()
					&& LAM==right_v->left()->op()
				){
					set_right(right_v->reduce(env,inp,outp),True);//OLD
				}
			#endif
		}
	}
	app_reduce_recurse_level -= 1;
	return exp;
}

exp_node* app_node::
reduce_vars(const alst_node* env,int inp,int* outp)
{
	if( left_v ){
		left_v->set_parent(this);
		exp_node* exp = left_v->reduce_vars(env,inp,outp);
		set_left(exp,True);
  		if( outp && (*outp&node::RECURSE_LVL) )
  		{
			if( right_v ) right_v->set_parent(this);
  			return this;
  		}
	}
	if( right_v ){
		right_v->set_parent(this);
		exp_node* exp = right_v->reduce_vars(env,inp,outp);
		set_right(exp,True);
	}
	return this;
}

void app_node::
set_left(exp_node* nd, Boolean import)
{
	if( left_v == nd ) return;
	if( left_v ) delete left_v;
	if( import ) left_v = nd;
	else if( nd ) left_v = (exp_node*)nd->clone();
	else left_v = 0;
	if( left_v ) left_v->set_parent(this);
}

void app_node::
set_right(exp_node* nd, Boolean import)
{
	if( right_v==nd ) return;
	if( right_v ) delete right_v;
	if( import ) right_v = nd;
	else if( nd ) right_v = (exp_node*)nd->clone();
	else right_v = 0;
	if( right_v ) right_v->set_parent(this);
}

int app_node::
operator == (const exp_node& cmp) const
{
	Boolean b1 = False;
	if( APP!=cmp.op() ) return b1;
	if( !left() || !cmp.left() ) return b1;
	if( !right() || !cmp.right() ) return b1;
	b1 = *left() == *(cmp.left()) 
		&& *right() == *(cmp.right());
	return b1;
}

exp_node* app_node::
extract_defs(const alst_node* env)
{
	exp_node* exp = this;
	const alst_node* alst = match(env);
	if( !alst ){
		if( left() ){
			exp_node* nl = left()->extract_defs(env);
			if( nl )
				set_left( nl, True);
		}
		if( right() ){
			exp_node* nr = right()->extract_defs(env);
			if( nr )
				set_right( nr, True);
		}
		alst = match(env);
		if( alst )
			exp = new var_node(alst->arg()->name());
	}else{
		exp = new var_node(alst->arg()->name());
	}
	return exp;
}

exp_node* app_node::
extract(const char* nm,int inp) const
{
	exp_node* exp = 0;
	exp_node* lft = 0;
	exp_node* rgt = 0;
	exp_node* exp1 = 0;
	//if( left() && right() && nm && (inp&DO_EXTRACT_ETA) )
	if( left() && right() && nm )
	{
		arg_node* argr = new arg_node(nm,0);
		arglst_node env(argr,0, True);
		if( !this->has_free(argr, &env) )
		{
			if( inp&DO_EXTRACT_APP )
			{
				if( left() ) lft = left()->extract(nm,inp);
				if( right() ) rgt = right()->extract(nm,inp);
				exp1 = (exp_node*)new app_node(
					new var_node("S"),lft,True);
				exp = (exp_node*)new app_node(exp1,rgt,True);
			}else{
				//exp = (exp_node*)new app_node(new var_node("K"),
				//	(exp_node*)this->clone(),True);
				exp1 = this->extract(0,inp);
				exp = (exp_node*)new app_node(
					new var_node("K"), exp1, True);
			}
		}else
		if( !left()->has_free(argr, &env)
			&& VAR==right()->op()
			&& right()->name()
			&& (inp&DO_EXTRACT_ETA)
			&& 0==strcmp( nm, right()->name()) )
		{
			exp = (exp_node*)left()->clone();
		}
	}
	if( !exp )
	{
		if( left() ) lft = left()->extract(nm,inp);
		if( right() ) rgt = right()->extract(nm,inp);
		if( nm )
		{
			exp1 = (exp_node*)new app_node(
				new var_node("S"),lft,True);
			exp = (exp_node*)new app_node(exp1,rgt,True);
		}else{
			exp = (exp_node*)new app_node(lft,rgt,True);
		}
	}
	return exp;
}

//////////////////////////////////////////////////////////////////////
// alst_node's
arglst_node::
arglst_node( arg_node* arg,arglst_node* next, Boolean import)
{
	if( import) arg_v=arg;
	else if( arg ) arg_v = (arg_node*)arg->clone();
	else arg_v = 0;
	if( import ) next_v=next;
	else if( next ) next_v = (arglst_node*)next->clone();
	else next_v = 0;
}

arglst_node::
arglst_node(const arglst_node& from) {
	if( from.arg_v )
		arg_v = (arg_node*)from.arg_v->clone();
	else arg_v = 0;
	if( from.next_v )
		next_v = (arglst_node*)from.next_v->clone();
	else arg_v = 0;
}


arglst_node::
~arglst_node()
{
	if( arg_v ) delete arg_v;
	if( next_v ) delete next_v;
}
alst_node* arglst_node::
add(arg_node* anode,Boolean import)
{
	arglst_node* retval = this;
	arglst_node* where = 0;
	if( !anode ) return where;

	for( arglst_node* lst=this;
		lst; lst=(arglst_node*)lst->next() )
	{
		if( lst->arg() && lst->arg()->name() ){
			if( 0==strcmp(anode->name(),
					      lst->arg_v->name()))
			{
				where = lst;
				where->set_arg(anode, import);
				break;
			}
		}
	}

	if( !where){
		where = new arglst_node(anode,0,import);
		where->next_v = this;
		retval = where;
	}

	return (alst_node*)retval;
}

arg_node* arglst_node::
find(arg_node* anode)
{
	arglst_node* where = 0;
	if( !anode ) return 0;
	arglst_node* last = 0;
	arg_node* arg = 0;

	for( arglst_node* lst=this;
		lst; last = lst, lst=(arglst_node*)lst->next() )
	{
		if( lst->arg() && lst->arg()->name() ){
			if( 0==strcmp(anode->name(),
					      lst->arg_v->name()))
			{
				where = lst;
				break;
			}
		}
	}
	if( where ) arg = where->arg_v;
	return arg;
}

void arglst_node::
set_arg(arg_node* nd, Boolean import)
{
	if( arg_v == nd ) return;
	if( arg_v ) delete arg_v;
	if( import ) arg_v = nd;
	else if( nd ) arg_v = (arg_node*)nd->clone();
	else arg_v = 0;
	if( arg_v ) arg_v->set_parent(this);
}

void arglst_node::
list()
{
	for(alst_node* pnode=(alst_node*)this;pnode; pnode=pnode->next() )
	{
		if( pnode->arg() ) pnode->arg()->print();
		printf("\n");
	}
}
