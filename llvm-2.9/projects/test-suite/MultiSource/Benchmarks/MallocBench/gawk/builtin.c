/*
 * builtin.c - Builtin functions and various utility procedures 
 */

/* 
 * Copyright (C) 1986, 1988, 1989 the Free Software Foundation, Inc.
 * 
 * This file is part of GAWK, the GNU implementation of the
 * AWK Progamming Language.
 * 
 * GAWK is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 1, or (at your option)
 * any later version.
 * 
 * GAWK is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GAWK; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "gawk.h"

extern void srandom();
extern char *initstate();
extern char *setstate();
extern long random();

extern NODE **fields_arr;

static void get_one();
static void get_two();
static int get_three();

/* Builtin functions */
NODE *
do_exp(tree)
NODE *tree;
{
	NODE *tmp;
	double d, res;
	double exp();

	get_one(tree, &tmp);
	d = force_number(tmp);
	free_temp(tmp);
	errno = 0;
	res = exp(d);
	if (errno == ERANGE)
		warning("exp argument %g is out of range", d);
	return tmp_number((AWKNUM) res);
}

NODE *
do_index(tree)
NODE *tree;
{
	NODE *s1, *s2;
	register char *p1, *p2;
	register int l1, l2;
	long ret;


	get_two(tree, &s1, &s2);
	force_string(s1);
	force_string(s2);
	p1 = s1->stptr;
	p2 = s2->stptr;
	l1 = s1->stlen;
	l2 = s2->stlen;
	ret = 0;
	if (! strict && IGNORECASE_node->var_value->numbr != 0.0) {
		while (l1) {
			if (casetable[*p1] == casetable[*p2]
			    && strncasecmp(p1, p2, l2) == 0) {
				ret = 1 + s1->stlen - l1;
				break;
			}
			l1--;
			p1++;
		}
	} else {
		while (l1) {
			if (STREQN(p1, p2, l2)) {
				ret = 1 + s1->stlen - l1;
				break;
			}
			l1--;
			p1++;
		}
	}
	free_temp(s1);
	free_temp(s2);
	return tmp_number((AWKNUM) ret);
}

NODE *
do_int(tree)
NODE *tree;
{
	NODE *tmp;
	double floor();
	double d;

	get_one(tree, &tmp);
	d = floor((double)force_number(tmp));
	free_temp(tmp);
	return tmp_number((AWKNUM) d);
}

NODE *
do_length(tree)
NODE *tree;
{
	NODE *tmp;
	int len;

	get_one(tree, &tmp);
	len = force_string(tmp)->stlen;
	free_temp(tmp);
	return tmp_number((AWKNUM) len);
}

NODE *
do_log(tree)
NODE *tree;
{
	NODE *tmp;
	double log();
	double d, arg;

	get_one(tree, &tmp);
	arg = (double) force_number(tmp);
	if (arg < 0.0)
		warning("log called with negative argument %g", arg);
	d = log(arg);
	free_temp(tmp);
	return tmp_number((AWKNUM) d);
}

/*
 * Note that the output buffer cannot be static because sprintf may get
 * called recursively by force_string.  Hence the wasteful alloca calls 
 */

/* %e and %f formats are not properly implemented.  Someone should fix them */
NODE *
do_sprintf(tree)
NODE *tree;
{
#define bchunk(s,l) if(l) {\
    while((l)>ofre) {\
      char *tmp;\
      tmp=(char *)alloca(osiz*2);\
      memcpy(tmp,obuf,olen);\
      obuf=tmp;\
      ofre+=osiz;\
      osiz*=2;\
    }\
    memcpy(obuf+olen,s,(l));\
    olen+=(l);\
    ofre-=(l);\
  }

	/* Is there space for something L big in the buffer? */
#define chksize(l)  if((l)>ofre) {\
    char *tmp;\
    tmp=(char *)alloca(osiz*2);\
    memcpy(tmp,obuf,olen);\
    obuf=tmp;\
    ofre+=osiz;\
    osiz*=2;\
  }

	/*
	 * Get the next arg to be formatted.  If we've run out of args,
	 * return "" (Null string) 
	 */
#define parse_next_arg() {\
  if(!carg) arg= Nnull_string;\
  else {\
  	get_one(carg,&arg);\
	carg=carg->rnode;\
  }\
 }

	char *obuf;
	int osiz, ofre, olen;
	static char chbuf[] = "0123456789abcdef";
	static char sp[] = " ";
	char *s0, *s1;
	int n0;
	NODE *sfmt, *arg;
	register NODE *carg;
	long fw, prec, lj, alt, big;
	long *cur;
	long val;
#ifdef sun386			/* Can't cast unsigned (int/long) from ptr->value */
	long tmp_uval;		/* on 386i 4.0.1 C compiler -- it just hangs */
#endif
	unsigned long uval;
	int sgn;
	int base;
	char cpbuf[30];		/* if we have numbers bigger than 30 */
	char *cend = &cpbuf[30];/* chars, we lose, but seems unlikely */
	char *cp;
	char *fill;
	double tmpval;
	char *pr_str;
	int ucasehex = 0;
	extern char *gcvt();


	obuf = (char *) alloca(120);
	osiz = 120;
	ofre = osiz;
	olen = 0;
	get_one(tree, &sfmt);
	sfmt = force_string(sfmt);
	carg = tree->rnode;
	for (s0 = s1 = sfmt->stptr, n0 = sfmt->stlen; n0-- > 0;) {
		if (*s1 != '%') {
			s1++;
			continue;
		}
		bchunk(s0, s1 - s0);
		s0 = s1;
		cur = &fw;
		fw = 0;
		prec = 0;
		lj = alt = big = 0;
		fill = sp;
		cp = cend;
		s1++;

retry:
		--n0;
		switch (*s1++) {
		case '%':
			bchunk("%", 1);
			s0 = s1;
			break;

		case '0':
			if (fill != sp || lj)
				goto lose;
			if (cur == &fw)
				fill = "0";	/* FALL through */
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			if (cur == 0)
				goto lose;
			*cur = s1[-1] - '0';
			while (n0 > 0 && *s1 >= '0' && *s1 <= '9') {
				--n0;
				*cur = *cur * 10 + *s1++ - '0';
			}
			goto retry;
#ifdef not_yet
		case ' ':		/* print ' ' or '-' */
		case '+':		/* print '+' or '-' */
#endif
		case '-':
			if (lj || fill != sp)
				goto lose;
			lj++;
			goto retry;
		case '.':
			if (cur != &fw)
				goto lose;
			cur = &prec;
			goto retry;
		case '#':
			if (alt)
				goto lose;
			alt++;
			goto retry;
		case 'l':
			if (big)
				goto lose;
			big++;
			goto retry;
		case 'c':
			parse_next_arg();
			if (arg->flags & NUMERIC) {
#ifdef sun386
				tmp_uval = arg->numbr; 
				uval= (unsigned long) tmp_uval;
#else
				uval = (unsigned long) arg->numbr;
#endif
				cpbuf[0] = uval;
				prec = 1;
				pr_str = cpbuf;
				goto dopr_string;
			}
			if (! prec)
				prec = 1;
			else if (prec > arg->stlen)
				prec = arg->stlen;
			pr_str = arg->stptr;
			goto dopr_string;
		case 's':
			parse_next_arg();
			arg = force_string(arg);
			if (!prec || prec > arg->stlen)
				prec = arg->stlen;
			pr_str = arg->stptr;

	dopr_string:
			if (fw > prec && !lj) {
				while (fw > prec) {
					bchunk(sp, 1);
					fw--;
				}
			}
			bchunk(pr_str, (int) prec);
			if (fw > prec) {
				while (fw > prec) {
					bchunk(sp, 1);
					fw--;
				}
			}
			s0 = s1;
			free_temp(arg);
			break;
		case 'd':
		case 'i':
			parse_next_arg();
			val = (long) force_number(arg);
			free_temp(arg);
			if (val < 0) {
				sgn = 1;
				val = -val;
			} else
				sgn = 0;
			do {
				*--cp = '0' + val % 10;
				val /= 10;
			} while (val);
			if (sgn)
				*--cp = '-';
			if (prec > fw)
				fw = prec;
			prec = cend - cp;
			if (fw > prec && !lj) {
				if (fill != sp && *cp == '-') {
					bchunk(cp, 1);
					cp++;
					prec--;
					fw--;
				}
				while (fw > prec) {
					bchunk(fill, 1);
					fw--;
				}
			}
			bchunk(cp, (int) prec);
			if (fw > prec) {
				while (fw > prec) {
					bchunk(fill, 1);
					fw--;
				}
			}
			s0 = s1;
			break;
		case 'u':
			base = 10;
			goto pr_unsigned;
		case 'o':
			base = 8;
			goto pr_unsigned;
		case 'X':
			ucasehex = 1;
		case 'x':
			base = 16;
			goto pr_unsigned;
	pr_unsigned:
			parse_next_arg();
			uval = (unsigned long) force_number(arg);
			free_temp(arg);
			do {
				*--cp = chbuf[uval % base];
				if (ucasehex && isalpha(*cp))
					*cp = toupper(*cp);
				uval /= base;
			} while (uval);
			if (alt && (base == 8 || base == 16)) {
				if (base == 16) {
					if (ucasehex)
						*--cp = 'X';
					else
						*--cp = 'x';
				}
				*--cp = '0';
			}
			prec = cend - cp;
			if (fw > prec && !lj) {
				while (fw > prec) {
					bchunk(fill, 1);
					fw--;
				}
			}
			bchunk(cp, (int) prec);
			if (fw > prec) {
				while (fw > prec) {
					bchunk(fill, 1);
					fw--;
				}
			}
			s0 = s1;
			break;
		case 'g':
			parse_next_arg();
			tmpval = force_number(arg);
			free_temp(arg);
			if (prec == 0)
				prec = 13;
			(void) gcvt(tmpval, (int) prec, cpbuf);
			prec = strlen(cpbuf);
			cp = cpbuf;
			if (fw > prec && !lj) {
				if (fill != sp && *cp == '-') {
					bchunk(cp, 1);
					cp++;
					prec--;
				}	/* Deal with .5 as 0.5 */
				if (fill == sp && *cp == '.') {
					--fw;
					while (--fw >= prec) {
						bchunk(fill, 1);
					}
					bchunk("0", 1);
				} else
					while (fw-- > prec)
						bchunk(fill, 1);
			} else {/* Turn .5 into 0.5 */
				/* FOO */
				if (*cp == '.' && fill == sp) {
					bchunk("0", 1);
					--fw;
				}
			}
			bchunk(cp, (int) prec);
			if (fw > prec)
				while (fw-- > prec)
					bchunk(fill, 1);
			s0 = s1;
			break;
		case 'f':
			parse_next_arg();
			tmpval = force_number(arg);
			free_temp(arg);
			chksize(fw + prec + 5);	/* 5==slop */

			cp = cpbuf;
			*cp++ = '%';
			if (lj)
				*cp++ = '-';
			if (fill != sp)
				*cp++ = '0';
			if (cur != &fw) {
				(void) strcpy(cp, "*.*f");
				(void) sprintf(obuf + olen, cpbuf, (int) fw, (int) prec, (double) tmpval);
			} else {
				(void) strcpy(cp, "*f");
				(void) sprintf(obuf + olen, cpbuf, (int) fw, (double) tmpval);
			}
			ofre -= strlen(obuf + olen);
			olen += strlen(obuf + olen);	/* There may be nulls */
			s0 = s1;
			break;
		case 'e':
			parse_next_arg();
			tmpval = force_number(arg);
			free_temp(arg);
			chksize(fw + prec + 5);	/* 5==slop */
			cp = cpbuf;
			*cp++ = '%';
			if (lj)
				*cp++ = '-';
			if (fill != sp)
				*cp++ = '0';
			if (cur != &fw) {
				(void) strcpy(cp, "*.*e");
				(void) sprintf(obuf + olen, cpbuf, (int) fw, (int) prec, (double) tmpval);
			} else {
				(void) strcpy(cp, "*e");
				(void) sprintf(obuf + olen, cpbuf, (int) fw, (double) tmpval);
			}
			ofre -= strlen(obuf + olen);
			olen += strlen(obuf + olen);	/* There may be nulls */
			s0 = s1;
			break;

		default:
	lose:
			break;
		}
	}
	bchunk(s0, s1 - s0);
	free_temp(sfmt);
	return tmp_string(obuf, olen);
}

void
do_printf(tree)
NODE *tree;
{
	struct redirect *rp = NULL;
	register FILE *fp = stdout;
	int errflg = 0;		/* not used, sigh */

	if (tree->rnode) {
		rp = redirect(tree->rnode, &errflg);
		if (rp)
			fp = rp->fp;
	}
	if (fp)
		print_simple(do_sprintf(tree->lnode), fp);
	if (rp && (rp->flag & RED_NOBUF))
		fflush(fp);
}

NODE *
do_sqrt(tree)
NODE *tree;
{
	NODE *tmp;
	double sqrt();
	double d, arg;

	get_one(tree, &tmp);
	arg = (double) force_number(tmp);
	if (arg < 0.0)
		warning("sqrt called with negative argument %g", arg);
	d = sqrt(arg);
	free_temp(tmp);
	return tmp_number((AWKNUM) d);
}

NODE *
do_substr(tree)
NODE *tree;
{
	NODE *t1, *t2, *t3;
	NODE *r;
	register int indx, length;

	t1 = t2 = t3 = NULL;
	length = -1;
	if (get_three(tree, &t1, &t2, &t3) == 3)
		length = (int) force_number(t3);
	indx = (int) force_number(t2) - 1;
	t1 = force_string(t1);
	if (length == -1)
		length = t1->stlen;
	if (indx < 0)
		indx = 0;
	if (indx >= t1->stlen || length <= 0) {
		if (t3)
			free_temp(t3);
		free_temp(t2);
		free_temp(t1);
		return Nnull_string;
	}
	if (indx + length > t1->stlen)
		length = t1->stlen - indx;
	if (t3)
		free_temp(t3);
	free_temp(t2);
	r =  tmp_string(t1->stptr + indx, length);
	free_temp(t1);
	return r;
}

NODE *
do_system(tree)
NODE *tree;
{
#if defined(unix) || defined(MSDOS) /* || defined(gnu) */
	NODE *tmp;
	int ret;

	(void) flush_io ();	/* so output is synchronous with gawk's */
	get_one(tree, &tmp);
	ret = system(force_string(tmp)->stptr);
	ret = (ret >> 8) & 0xff;
	free_temp(tmp);
	return tmp_number((AWKNUM) ret);
#else
	fatal("the \"system\" function is not supported.");
	/* NOTREACHED */
#endif
}

void 
do_print(tree)
register NODE *tree;
{
	struct redirect *rp = NULL;
	register FILE *fp = stdout;
	int errflg = 0;		/* not used, sigh */

	if (tree->rnode) {
		rp = redirect(tree->rnode, &errflg);
		if (rp)
			fp = rp->fp;
	}
	if (!fp)
		return;
	tree = tree->lnode;
	if (!tree)
		tree = WHOLELINE;
	if (tree->type != Node_expression_list) {
		if (!(tree->flags & STR))
			cant_happen();
		print_simple(tree, fp);
	} else {
		while (tree) {
			print_simple(force_string(tree_eval(tree->lnode)), fp);
			tree = tree->rnode;
			if (tree)
				print_simple(OFS_node->var_value, fp);
		}
	}
	print_simple(ORS_node->var_value, fp);
	if (rp && (rp->flag & RED_NOBUF))
		fflush(fp);
}

NODE *
do_tolower(tree)
NODE *tree;
{
	NODE *t1, *t2;
	register char *cp, *cp2;

	get_one(tree, &t1);
	t1 = force_string(t1);
	t2 = tmp_string(t1->stptr, t1->stlen);
	for (cp = t2->stptr, cp2 = t2->stptr + t2->stlen; cp < cp2; cp++)
		if (isupper(*cp))
			*cp = tolower(*cp);
	free_temp(t1);
	return t2;
}

NODE *
do_toupper(tree)
NODE *tree;
{
	NODE *t1, *t2;
	register char *cp;

	get_one(tree, &t1);
	t1 = force_string(t1);
	t2 = tmp_string(t1->stptr, t1->stlen);
	for (cp = t2->stptr; cp < t2->stptr + t2->stlen; cp++)
		if (islower(*cp))
			*cp = toupper(*cp);
	free_temp(t1);
	return t2;
}

/*
 * Get the arguments to functions.  No function cares if you give it too many
 * args (they're ignored).  Only a few fuctions complain about being given
 * too few args.  The rest have defaults.
 */

static void
get_one(tree, res)
NODE *tree, **res;
{
	if (!tree) {
		*res = WHOLELINE;
		return;
	}
	*res = tree_eval(tree->lnode);
}

static void
get_two(tree, res1, res2)
NODE *tree, **res1, **res2;
{
	if (!tree) {
		*res1 = WHOLELINE;
		return;
	}
	*res1 = tree_eval(tree->lnode);
	if (!tree->rnode)
		return;
	tree = tree->rnode;
	*res2 = tree_eval(tree->lnode);
}

static int
get_three(tree, res1, res2, res3)
NODE *tree, **res1, **res2, **res3;
{
	if (!tree) {
		*res1 = WHOLELINE;
		return 0;
	}
	*res1 = tree_eval(tree->lnode);
	if (!tree->rnode)
		return 1;
	tree = tree->rnode;
	*res2 = tree_eval(tree->lnode);
	if (!tree->rnode)
		return 2;
	tree = tree->rnode;
	*res3 = tree_eval(tree->lnode);
	return 3;
}

int
a_get_three(tree, res1, res2, res3)
NODE *tree, **res1, **res2, **res3;
{
	if (!tree) {
		*res1 = WHOLELINE;
		return 0;
	}
	*res1 = tree_eval(tree->lnode);
	if (!tree->rnode)
		return 1;
	tree = tree->rnode;
	*res2 = tree->lnode;
	if (!tree->rnode)
		return 2;
	tree = tree->rnode;
	*res3 = tree_eval(tree->lnode);
	return 3;
}

void
print_simple(tree, fp)
NODE *tree;
FILE *fp;
{
	if (fwrite(tree->stptr, sizeof(char), tree->stlen, fp) != tree->stlen)
		warning("fwrite: %s", strerror(errno));
	free_temp(tree);
}

NODE *
do_atan2(tree)
NODE *tree;
{
	NODE *t1, *t2;
	extern double atan2();
	double d1, d2;

	get_two(tree, &t1, &t2);
	d1 = force_number(t1);
	d2 = force_number(t2);
	free_temp(t1);
	free_temp(t2);
	return tmp_number((AWKNUM) atan2(d1, d2));
}

NODE *
do_sin(tree)
NODE *tree;
{
	NODE *tmp;
	extern double sin();
	double d;

	get_one(tree, &tmp);
	d = sin((double)force_number(tmp));
	free_temp(tmp);
	return tmp_number((AWKNUM) d);
}

NODE *
do_cos(tree)
NODE *tree;
{
	NODE *tmp;
	extern double cos();
	double d;

	get_one(tree, &tmp);
	d = cos((double)force_number(tmp));
	free_temp(tmp);
	return tmp_number((AWKNUM) d);
}

static int firstrand = 1;
static char state[256];

#define	MAXLONG	2147483647	/* maximum value for long int */

/* ARGSUSED */
NODE *
do_rand(tree)
NODE *tree;
{
	if (firstrand) {
		(void) initstate((unsigned) 1, state, sizeof state);
		srandom(1);
		firstrand = 0;
	}
	return tmp_number((AWKNUM) random() / MAXLONG);
}

NODE *
do_srand(tree)
NODE *tree;
{
	NODE *tmp;
	static long save_seed = 1;
	long ret = save_seed;	/* SVR4 awk srand returns previous seed */
	extern long time();

	if (firstrand)
		(void) initstate((unsigned) 1, state, sizeof state);
	else
		(void) setstate(state);

	if (!tree)
		srandom((int) (save_seed = time((long *) 0)));
	else {
		get_one(tree, &tmp);
		srandom((int) (save_seed = (long) force_number(tmp)));
		free_temp(tmp);
	}
	firstrand = 0;
	return tmp_number((AWKNUM) ret);
}

NODE *
do_match(tree)
NODE *tree;
{
	NODE *t1;
	int rstart;
	struct re_registers reregs;
	struct re_pattern_buffer *rp;
	int need_to_free = 0;

	t1 = force_string(tree_eval(tree->lnode));
	tree = tree->rnode;
	if (tree == NULL || tree->lnode == NULL)
		fatal("match called with only one argument");
	tree = tree->lnode;
	if (tree->type == Node_regex) {
		rp = tree->rereg;
		if (!strict && ((IGNORECASE_node->var_value->numbr != 0)
		    ^ (tree->re_case != 0))) {
			/* recompile since case sensitivity differs */
			rp = tree->rereg =
				mk_re_parse(tree->re_text,
				(IGNORECASE_node->var_value->numbr != 0));
			tree->re_case =
				(IGNORECASE_node->var_value->numbr != 0);
		}
	} else {
		need_to_free = 1;
		rp = make_regexp(force_string(tree_eval(tree)),
				(IGNORECASE_node->var_value->numbr != 0));
		if (rp == NULL)
			cant_happen();
	}
	rstart = re_search(rp, t1->stptr, t1->stlen, 0, t1->stlen, &reregs);
	free_temp(t1);
	if (rstart >= 0) {
		rstart++;	/* 1-based indexing */
		/* RSTART set to rstart below */
		RLENGTH_node->var_value->numbr =
			(AWKNUM) (reregs.end[0] - reregs.start[0]);
	} else {
		/*
		 * Match failed. Set RSTART to 0, RLENGTH to -1.
		 * Return the value of RSTART.
		 */
		rstart = 0;	/* used as return value */
		RLENGTH_node->var_value->numbr = -1.0;
	}
	RSTART_node->var_value->numbr = (AWKNUM) rstart;
	if (need_to_free) {
		free(rp->buffer);
		free(rp->fastmap);
		free((char *) rp);
	}
	return tmp_number((AWKNUM) rstart);
}

static NODE *
sub_common(tree, global)
NODE *tree;
int global;
{
	register int len;
	register char *scan;
	register char *bp, *cp;
	int search_start = 0;
	int match_length;
	int matches = 0;
	char *buf;
	struct re_pattern_buffer *rp;
	NODE *s;		/* subst. pattern */
	NODE *t;		/* string to make sub. in; $0 if none given */
	struct re_registers reregs;
	unsigned int saveflags;
	NODE *tmp;
	NODE **lhs;
	char *lastbuf;
	int need_to_free = 0;

	if (tree == NULL)
		fatal("sub or gsub called with 0 arguments");
	tmp = tree->lnode;
	if (tmp->type == Node_regex) {
		rp = tmp->rereg;
		if (! strict && ((IGNORECASE_node->var_value->numbr != 0)
		    ^ (tmp->re_case != 0))) {
			/* recompile since case sensitivity differs */
			rp = tmp->rereg =
				mk_re_parse(tmp->re_text,
				(IGNORECASE_node->var_value->numbr != 0));
			tmp->re_case = (IGNORECASE_node->var_value->numbr != 0);
		}
	} else {
		need_to_free = 1;
		rp = make_regexp(force_string(tree_eval(tmp)),
				(IGNORECASE_node->var_value->numbr != 0));
		if (rp == NULL)
			cant_happen();
	}
	tree = tree->rnode;
	if (tree == NULL)
		fatal("sub or gsub called with only 1 argument");
	s = force_string(tree_eval(tree->lnode));
	tree = tree->rnode;
	deref = 0;
	field_num = -1;
	if (tree == NULL) {
		t = node0_valid ? fields_arr[0] : *get_field(0, 0);
		lhs = &fields_arr[0];
		field_num = 0;
		deref = t;
	} else {
		t = tree->lnode;
		lhs = get_lhs(t, 1);
		t = force_string(tree_eval(t));
	}
	/*
	 * create a private copy of the string
	 */
	if (t->stref > 1 || (t->flags & PERM)) {
		saveflags = t->flags;
		t->flags &= ~MALLOC;
		tmp = dupnode(t);
		t->flags = saveflags;
		do_deref();
		t = tmp;
		if (lhs)
			*lhs = tmp;
	}
	lastbuf = t->stptr;
	do {
		if (re_search(rp, t->stptr, t->stlen, search_start,
		    t->stlen-search_start, &reregs) == -1
		    || reregs.start[0] == reregs.end[0])
			break;
		matches++;

		/*
		 * first, make a pass through the sub. pattern, to calculate
		 * the length of the string after substitution 
		 */
		match_length = reregs.end[0] - reregs.start[0];
		len = t->stlen - match_length;
		for (scan = s->stptr; scan < s->stptr + s->stlen; scan++)
			if (*scan == '&')
				len += match_length;
			else if (*scan == '\\' && *(scan+1) == '&') {
				scan++;
				len++;
			} else
				len++;
		emalloc(buf, char *, len + 1, "do_sub");
		bp = buf;

		/*
		 * now, create the result, copying in parts of the original
		 * string 
		 */
		for (scan = t->stptr; scan < t->stptr + reregs.start[0]; scan++)
			*bp++ = *scan;
		for (scan = s->stptr; scan < s->stptr + s->stlen; scan++)
			if (*scan == '&')
				for (cp = t->stptr + reregs.start[0];
				     cp < t->stptr + reregs.end[0]; cp++)
					*bp++ = *cp;
			else if (*scan == '\\' && *(scan+1) == '&') {
				scan++;
				*bp++ = *scan;
			} else
				*bp++ = *scan;
		search_start = bp - buf;
		for (scan = t->stptr + reregs.end[0];
		     scan < t->stptr + t->stlen; scan++)
			*bp++ = *scan;
		*bp = '\0';
		free(lastbuf);
		t->stptr = buf;
		lastbuf = buf;
		t->stlen = len;
	} while (global && search_start < t->stlen);

	free_temp(s);
	if (need_to_free) {
		free(rp->buffer);
		free(rp->fastmap);
		free((char *) rp);
	}
	if (matches > 0) {
		if (field_num == 0)
			set_record(fields_arr[0]->stptr, fields_arr[0]->stlen);
		t->flags &= ~(NUM|NUMERIC);
	}
	field_num = -1;
	return tmp_number((AWKNUM) matches);
}

NODE *
do_gsub(tree)
NODE *tree;
{
	return sub_common(tree, 1);
}

NODE *
do_sub(tree)
NODE *tree;
{
	return sub_common(tree, 0);
}

