/* node.h */
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
class arg_node;
class exp_node;
class var_node;
class lam_node;
class app_node;

class alst_node;
class arglst_node;
class argrefs_node;

#ifndef Boolean 
#define Boolean short
#define False 0
#define True 1
#endif

/////////////////////////////////////////////////////////////////////
class node {
public:
	enum type { NTYPE, NODE, ARG, VAR, LAM, APP, ARGLST, STACKFRAME, ARGREFS };
	enum { RECURSE_LVL=1, BETA_ETA_DONE=2, ETA_DONE=4 };
	enum { DO_STEP=1, DO_APP_ORDER=2, DO_REDUCE_BODY=4,
		DO_PRINT_SYM=8,DO_PRINT_BRIEF=16,
		DO_TRACE=32, DO_EXTRACT_ETA=64,
		DO_EXTRACT_APP=128,
		DO_REDUCE_FULLY=256};
public:
	node() { parent_v = 0;};
	node(const node& from) { parent_v=from.parent_v; };
	virtual node* clone() const= 0;
	virtual ~node() {};
public:
	virtual type op() const{ return NODE;};
	virtual const char* name() const{ return 0;};
	virtual exp_node* value() const{ return 0;};
	virtual arg_node* arg() const{ return 0;};
	virtual exp_node* body() const{ return 0;};
	virtual exp_node* left() const{ return 0;};
	virtual exp_node* right() const{ return 0;};
	virtual alst_node* next() const{ return 0;};
	virtual node* parent() const{ return parent_v; };
public:
	virtual void set_parent(node* par){ parent_v = par;};
public:
	static void reset();
public:
	//virtual void rename() {};
	virtual exp_node* reduce(
		const alst_node* env,int inp,int* outp){
		return 0;};
public:
	virtual void print(const alst_node*env=0,int inp=0) const;
protected:
	node* parent_v;
};

/////////////////////////////////////////////////////////////////////
class exp_node: public node
{
public:
	virtual Boolean has_free(const arg_node*, const alst_node* env)const{ return False; };
	virtual arglst_node* bind(const alst_node* env=0)const{return 0;};
	virtual exp_node* reduce_vars(const alst_node* env,int inp,int* outp){
		return this; };
	virtual void rename(
		arg_node* arg,const char* newname,alst_node* env=0){};
	virtual void resolve_name_clash(
		arg_node* outer_arg,alst_node* env=0){};
public:
	virtual int operator == (const exp_node& cmp) const {return False;};
public:
	virtual exp_node* export_body(){return 0;};
	virtual exp_node* export_left(){return 0;};
	virtual exp_node* export_right(){return 0;};
public:
	virtual const alst_node* match(const alst_node* env=0) const;
	virtual exp_node* extract_defs(const alst_node* env=0){return 0; };
	void symbolic_print(const alst_node* env,int inp = 0) const;
public:
	virtual exp_node* extract(const char* nm,int inp=0) const {return 0; };
};

/////////////////////////////////////////////////////////////////////
class alst_node: public node
{
public:
	virtual ~alst_node(){};
	virtual alst_node* add(
		arg_node* anode,Boolean import=False){
		return 0; };
	virtual arg_node* find(arg_node* anode) {
		return 0;};
	virtual void list(){};
};

/////////////////////////////////////////////////////////////////////
class arg_node: public node
{
public:
	arg_node(){ name_v = 0; value_v=0;};
	arg_node(const char*nm,const exp_node* ex,
		Boolean import=False);

	arg_node(const arg_node& from);
	virtual node* clone() const;
	virtual ~arg_node();
public:
	virtual arg_node& operator =(const arg_node& from);
public:
	//Boolean free_in(exp_node* exp);
	virtual void print(const alst_node*env=0, int inp=0) const;
public:
	virtual type op() const{ return ARG;};
	virtual const char* name() const{ return name_v;};
	virtual exp_node* value() const{ return value_v;}; // VAR, LAM, or APP
	//virtual arg_node* arg() const{ return 0;};
	//virtual exp_node* body() const{ return 0;};
	//virtual exp_node* left() const{ return 0;};
	//virtual exp_node* right() const{ return 0;};
	//virtual alst_node* next() const{ return 0;};
public:
	//virtual exp_node* reduce(const alst_node* env,int inp,int* outp) { return 0;};
	void import_value(exp_node** val);
	int operator == (const arg_node& cmp) const;
	int operator == (const char* cmp) const;
public:
	void set_name(const char* nm)
	{
		if( name_v == nm ) return;
		if( name_v ) delete [] name_v;
		name_v = nm?new char[strlen(nm)+1]:0;
		if( name_v && nm ) strcpy(name_v,nm);
	};
	void import_value(exp_node* nd)
	{
		if( value_v == nd ) return;
		if( value_v ) delete value_v;
		value_v = nd;
		if( value_v ) value_v->set_parent(this);
	};
private:
	char* name_v;
	exp_node* value_v;
};

/////////////////////////////////////////////////////////////////////
class var_node: public exp_node
{
public:
	var_node(const char* nm=0);
	var_node(const var_node& from);
	virtual node* clone() const { return (node*)new var_node(*this); };
	virtual ~var_node(){ if( name_v ) delete [] name_v;};
public:
	virtual Boolean has_free(const arg_node* argr, const alst_node* env) const;
	arglst_node* bind(const alst_node* env=0)const;
	virtual void print(const alst_node*env=0,int inp=0) const;
public:
	var_node& operator =(const var_node& from);
public:
	virtual type op() const { return VAR;};
	virtual const char* name() const{ 
		return name_v;};
	//virtual exp_node* value() const{return arg() ? arg()->value() : 0; }; // VAR, LAM, or APP
	//virtual exp_node* value() const;
	////virtual arg_node* arg() const{return ref_arg_v ? ref_arg_v : def_arg_v;};
	//virtual exp_node* body() const{ return 0;};
	//virtual exp_node* left() const{ return 0;};
	//virtual exp_node* right() const{ return 0;};
	//virtual alst_node* next() const{ return 0;};
public:
	void set_name(const char* nm);
public:
	virtual void rename(
		arg_node* arg,const char* newname,alst_node*env=0);
	virtual void resolve_name_clash(
		arg_node* outer_arg,alst_node* env=0){};
	virtual exp_node* reduce_value(
		const alst_node* env, int inp, int* outp,
			const alst_node** pnenv);
	exp_node* reduce_number();
	virtual exp_node* reduce(
		const alst_node* env,int inp,int* outp);
	virtual exp_node* reduce_vars(const alst_node* env,int inp,int* outp)
	{
		return reduce(env,inp,outp);
	};
public:
	////void reduce_arg();
	////arg_node* ref_arg() { return ref_arg_v;};
public:
	virtual int operator == (const exp_node& cmp) const;
public:
	virtual exp_node* extract(const char* nm,int inp=0) const;
private:
	////arg_node* def_arg_v;
	////arg_node* ref_arg_v;
	char* name_v;
};

/////////////////////////////////////////////////////////////////////
class lam_node: public exp_node
{
public:
	lam_node(){ arg_v=0; body_v=0;};
	lam_node(arg_node* arg, exp_node* bdy,
		Boolean import=False);
	lam_node(const lam_node& from);
	virtual node* clone() const { return (node*)new lam_node(*this); };
	virtual ~lam_node();
public:
	virtual Boolean has_free(const arg_node* argr, const alst_node* env) const;
	virtual arglst_node* bind(const alst_node* env=0)const{
		//if( body_v ){body_v->set_parent(this);body_v->bind(env); };
		return 0;
	};
	virtual void print(const alst_node*env=0,int inp=0) const;
public:
	lam_node& operator =(const lam_node& from);
public:
	virtual type op() const { return LAM;};
	//virtual const char* name() const{ return 0;};
	//virtual exp_node* value() const{ return value;}; // VAR, LAM, or APP
	virtual arg_node* arg() const{ return arg_v;};
	virtual exp_node* body() const{ return body_v;};
	//virtual exp_node* left() const{ return 0;};
	//virtual exp_node* right() const{ return 0;};
	//virtual alst_node* next() const{ return 0;};
public:
	virtual void rename(
		arg_node* arg,const char* newname,alst_node* env=0);
	virtual void resolve_name_clash(
		arg_node* outer_arg,alst_node* env=0);
	virtual exp_node* reduce(
		const alst_node* env,int inp,int* outp);
	virtual exp_node* eta_reduce(
		const alst_node* env,int inp,int* outp);
	virtual exp_node* reduce_vars(const alst_node* env,int inp,int* outp);
public:
	void set_arg(arg_node* nd, Boolean import);
	void set_body(exp_node* nd, Boolean import);
public:
	virtual int operator == (const exp_node& cmp) const;
public:
	virtual exp_node* export_body(){
		exp_node* exp = body_v;
		body_v = 0;
		return exp;};
public:
	virtual exp_node* extract_defs(const alst_node* env=0);
public:
	virtual exp_node* extract(const char* nm,int inp=0) const;
private:
	arg_node* arg_v;
	exp_node* body_v;
};

/////////////////////////////////////////////////////////////////////
class app_node: public exp_node
{
public:
	app_node(){ left_v=0; right_v=0;};
	app_node( exp_node* lft, exp_node* rgt,
		Boolean import = False);
	app_node(const app_node& from);
	virtual node* clone() const { return (node*)new app_node(*this); };
	virtual ~app_node();
public:
	virtual Boolean has_free(const arg_node* argr, const alst_node* env) const;
	virtual arglst_node* bind(const alst_node* env=0)const{
		////if( left_v ){left_v->set_parent(this);left_v->bind(env); }
		////if( right_v ){right_v->set_parent(this);right_v->bind(env); };
		return 0;
	};
	virtual void print(const alst_node*env=0,int inp=0) const;
public:
	app_node& operator =(const app_node& from);
public:
	virtual type op() const{ return APP;};
	//virtual const char* name() const{ return 0;};
	//virtual exp_node* value() const{ return value;}; // VAR, LAM, or APP
	//virtual arg_node* arg() const{ return 0;};
	//virtual exp_node* body() const{ return 0;};
	virtual exp_node* left() const{ return left_v;};
	virtual exp_node* right() const{ return right_v;};
	//virtual alst_node* next() const{ return 0;};
public:
	virtual void rename(
		arg_node* arg,const char* newname,alst_node* env=0);
	virtual void resolve_name_clash(
		arg_node* outer_arg,alst_node* env=0);
	virtual exp_node* reduce(
		const alst_node* env,int inp,int* outp);
	virtual exp_node* reduce_vars(const alst_node* env,int inp,int* outp);
public:
	void set_left(exp_node* nd, Boolean import);
	void set_right(exp_node* nd, Boolean import);
public:
	virtual int operator == (const exp_node& cmp) const;
public:
	virtual exp_node* export_left(){
		exp_node* exp = left_v;
		left_v = 0;
		return exp;};
	virtual exp_node* export_right(){
		exp_node* exp = right_v;
		right_v = 0;
		return exp;};
public:
	virtual exp_node* extract_defs(const alst_node* env=0);
public:
	virtual exp_node* extract(const char* nm,int inp=0) const;
private:
	exp_node* left_v;
	exp_node* right_v;
};

/////////////////////////////////////////////////////////////////////
class arglst_node: public alst_node
{
public:
	arglst_node(
		arg_node* arg=0,arglst_node* next=0,
		Boolean import=False);
	arglst_node(const arglst_node& from);
	virtual node* clone() const { return (node*)new arglst_node(*this); };
	virtual ~arglst_node();
public:
	virtual type op() const { return ARGLST;};
	//virtual const char* name() const{ return 0;};
	//virtual exp_node* value() const{ return value;}; // VAR, LAM, or APP
	virtual arg_node* arg() const{ return arg_v;};
	//virtual exp_node* body() const{ return 0;};
	//virtual exp_node* left() const{ return 0;};
	//virtual exp_node* right() const{ return 0;};
	virtual alst_node* next() const{ return (alst_node*)next_v;};
public:
	//virtual void rename() {};
	//virtual exp_node* reduce(const alst_node* env,int inp,int* outp) { return 0;};
	virtual alst_node* add(arg_node* anode,Boolean import=False);
	virtual arg_node* find(arg_node* anode);
public:
	void set_arg(arg_node* nd, Boolean import);
	virtual void list();
protected:
	arg_node* arg_v;
	arglst_node* next_v;
};

/////////////////////////////////////////////////////////////////////
class stack_frame: public arglst_node
{
public:
	virtual type op() const{ return STACKFRAME;};
	stack_frame(arg_node* arg=0, const alst_node* next=0)
		{ arg_v = arg; next_v = (arglst_node*)next; }
	virtual ~stack_frame(){arg_v = 0;next_v = 0;};
};

extern const arglst_node* definition_env;
