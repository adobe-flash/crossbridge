/*
 * io.c - routines for dealing with input and output and records
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
#ifndef O_RDONLY
#include <fcntl.h>
#endif
#include <signal.h>

extern FILE *popen();

static void do_file();
static IOBUF *nextfile();
static int get_a_record();
static int iop_close();
static IOBUF *iop_alloc();
static void close_one();
static int close_redir();
static IOBUF *gawk_popen();
static int gawk_pclose();

static struct redirect *red_head = NULL;
static int getline_redirect = 0;	/* "getline <file" being executed */

extern char *line_buf;
extern int output_is_tty;
extern NODE *ARGC_node;
extern NODE *ARGV_node;
extern NODE **fields_arr;

int field_num;

static IOBUF *
nextfile()
{
	static int i = 1;
	static int files = 0;
	static IOBUF *curfile = NULL;
	char *arg;
	char *cp;
	int fd = -1;

	if (curfile != NULL && curfile->cnt != EOF)
		return curfile;
	for (; i < (int) (ARGC_node->lnode->numbr); i++) {
		arg = (*assoc_lookup(ARGV_node, tmp_number((AWKNUM) i)))->stptr;
		if (*arg == '\0')
			continue;
		cp = strchr(arg, '=');
		if (cp != NULL) {
			*cp++ = '\0';
			variable(arg)->var_value = make_string(cp, strlen(cp));
			*--cp = '=';	/* restore original text of ARGV */
		} else {
			files++;
			if (STREQ(arg, "-"))
				fd = 0;
			else
				fd = devopen(arg, "r");
			if (fd == -1)
				fatal("cannot open file `%s' for reading (%s)",
					arg, strerror(errno));
				/* NOTREACHED */
			/* This is a kludge.  */
			deref = FILENAME_node->var_value;
			do_deref();
			FILENAME_node->var_value =
				make_string(arg, strlen(arg));
			FNR_node->var_value->numbr = 0.0;
			i++;
			break;
		}
	}
	if (files == 0) {
		files++;
		/* no args. -- use stdin */
		/* FILENAME is init'ed to "-" */
		/* FNR is init'ed to 0 */
		fd = 0;
	}
	if (fd == -1)
		return NULL;
	return curfile = iop_alloc(fd);
}

static IOBUF *
iop_alloc(fd)
int fd;
{
	IOBUF *iop;
	struct stat stb;

	/*
	 * System V doesn't have the file system block size in the
	 * stat structure. So we have to make some sort of reasonable
	 * guess. We use stdio's BUFSIZ, since that is what it was
	 * meant for in the first place.
	 */
#ifdef BLKSIZE_MISSING
#define	DEFBLKSIZE	BUFSIZ
#else
#define DEFBLKSIZE	(stb.st_blksize ? stb.st_blksize : BUFSIZ)
#endif

	if (fd == -1)
		return NULL;
	emalloc(iop, IOBUF *, sizeof(IOBUF), "nextfile");
	iop->flag = 0;
	if (isatty(fd)) {
		iop->flag |= IOP_IS_TTY;
		iop->size = BUFSIZ;
	} else if (fstat(fd, &stb) == -1)
		fatal("can't stat fd %d (%s)", fd, strerror(errno));
	else if (lseek(fd, 0L, 0) == -1)
		iop->size = DEFBLKSIZE;
	else
		iop->size = (stb.st_size < DEFBLKSIZE ?
				stb.st_size+1 : DEFBLKSIZE);
	errno = 0;
	iop->fd = fd;
	emalloc(iop->buf, char *, iop->size, "nextfile");
	iop->off = iop->buf;
	iop->cnt = 0;
	iop->secsiz = iop->size < BUFSIZ ? iop->size : BUFSIZ;
	emalloc(iop->secbuf, char *, iop->secsiz, "nextfile");
	return iop;
}

void
do_input()
{
	IOBUF *iop;
	extern int exiting;

	while ((iop = nextfile()) != NULL) {
		do_file(iop);
		if (exiting)
			break;
	}
}

static int
iop_close(iop)
IOBUF *iop;
{
	int ret;

	ret = close(iop->fd);
	if (ret == -1)
		warning("close of fd %d failed (%s)", iop->fd, strerror(errno));
	free(iop->buf);
	free(iop->secbuf);
	free((char *)iop);
	return ret == -1 ? 1 : 0;
}

/*
 * This reads in a record from the input file
 */
static int
inrec(iop)
IOBUF *iop;
{
	int cnt;
	int retval = 0;

	cnt = get_a_record(&line_buf, iop);
	if (cnt == EOF) {
		cnt = 0;
		retval = 1;
	} else {
		if (!getline_redirect) {
			assign_number(&NR_node->var_value,
			    NR_node->var_value->numbr + 1.0);
			assign_number(&FNR_node->var_value,
			    FNR_node->var_value->numbr + 1.0);
		}
	}
	set_record(line_buf, cnt);

	return retval;
}

static void
do_file(iop)
IOBUF *iop;
{
	/* This is where it spends all its time.  The infamous MAIN LOOP */
	if (inrec(iop) == 0)
		while (interpret(expression_value) && inrec(iop) == 0)
			;
	(void) iop_close(iop);
}

int
get_rs()
{
	register NODE *tmp;

	tmp = force_string(RS_node->var_value);
	if (tmp->stlen == 0)
		return 0;
	return *(tmp->stptr);
}

/* Redirection for printf and print commands */
struct redirect *
redirect(tree, errflg)
NODE *tree;
int *errflg;
{
	register NODE *tmp;
	register struct redirect *rp;
	register char *str;
	int tflag = 0;
	int outflag = 0;
	char *direction = "to";
	char *mode;
	int fd;

	switch (tree->type) {
	case Node_redirect_append:
		tflag = RED_APPEND;
	case Node_redirect_output:
		outflag = (RED_FILE|RED_WRITE);
		tflag |= outflag;
		break;
	case Node_redirect_pipe:
		tflag = (RED_PIPE|RED_WRITE);
		break;
	case Node_redirect_pipein:
		tflag = (RED_PIPE|RED_READ);
		break;
	case Node_redirect_input:
		tflag = (RED_FILE|RED_READ);
		break;
	default:
		fatal ("invalid tree type %d in redirect()", tree->type);
		break;
	}
	tmp = force_string(tree_eval(tree->subnode));
	str = tmp->stptr;
	for (rp = red_head; rp != NULL; rp = rp->next)
		if (STREQ(rp->value, str)
		    && ((rp->flag & ~RED_NOBUF) == tflag
			|| (outflag
			    && (rp->flag & (RED_FILE|RED_WRITE)) == outflag)))
			break;
	if (rp == NULL) {
		emalloc(rp, struct redirect *, sizeof(struct redirect),
			"redirect");
		emalloc(str, char *, tmp->stlen+1, "redirect");
		memcpy(str, tmp->stptr, tmp->stlen+1);
		rp->value = str;
		rp->flag = tflag;
		rp->offset = 0;
		rp->fp = NULL;
		rp->iop = NULL;
		/* maintain list in most-recently-used first order */
		if (red_head)
			red_head->prev = rp;
		rp->prev = NULL;
		rp->next = red_head;
		red_head = rp;
	}
	while (rp->fp == NULL && rp->iop == NULL) {
		mode = NULL;
		errno = 0;
		switch (tree->type) {
		case Node_redirect_output:
			mode = "w";
			break;
		case Node_redirect_append:
			mode = "a";
			break;
		case Node_redirect_pipe:
			if ((rp->fp = popen(str, "w")) == NULL)
				fatal("can't open pipe (\"%s\") for output (%s)",
					str, strerror(errno));
			rp->flag |= RED_NOBUF;
			break;
		case Node_redirect_pipein:
			direction = "from";
			if (gawk_popen(str, rp) == NULL)
				fatal("can't open pipe (\"%s\") for input (%s)",
					str, strerror(errno));
			break;
		case Node_redirect_input:
			direction = "from";
			rp->iop = iop_alloc(devopen(str, "r"));
			break;
		default:
			cant_happen();
		}
		if (mode != NULL) {
			fd = devopen(str, mode);
			if (fd != -1) {
				rp->fp = fdopen(fd, mode);
				if (isatty(fd))
					rp->flag |= RED_NOBUF;
			}
		}
		if (rp->fp == NULL && rp->iop == NULL) {
			/* too many files open -- close one and try again */
			if (errno == ENFILE || errno == EMFILE)
				close_one();
			else {
				/*
				 * Some other reason for failure.
				 *
				 * On redirection of input from a file,
				 * just return an error, so e.g. getline
				 * can return -1.  For output to file,
				 * complain. The shell will complain on
				 * a bad command to a pipe.
				 */
				*errflg = 1;
				if (tree->type == Node_redirect_output
				    || tree->type == Node_redirect_append)
					fatal("can't redirect %s `%s' (%s)",
					    direction, str, strerror(errno));
				else
					return NULL;
			}
		}
	}
	if (rp->offset != 0)	/* this file was previously open */
		if (fseek(rp->fp, rp->offset, 0) == -1)
			fatal("can't seek to %ld on `%s' (%s)",
				rp->offset, str, strerror(errno));
	free_temp(tmp);
	return rp;
}

static void
close_one()
{
	register struct redirect *rp;
	register struct redirect *rplast = NULL;

	/* go to end of list first, to pick up least recently used entry */
	for (rp = red_head; rp != NULL; rp = rp->next)
		rplast = rp;
	/* now work back up through the list */
	for (rp = rplast; rp != NULL; rp = rp->prev)
		if (rp->fp && (rp->flag & RED_FILE)) {
			rp->offset = ftell(rp->fp);
			if (fclose(rp->fp))
				warning("close of \"%s\" failed (%s).",
					rp->value, strerror(errno));
			rp->fp = NULL;
			break;
		}
	if (rp == NULL)
		/* surely this is the only reason ??? */
		fatal("too many pipes or input files open"); 
}

NODE *
do_close(tree)
NODE *tree;
{
	NODE *tmp;
	register struct redirect *rp;

	tmp = force_string(tree_eval(tree->subnode));
	for (rp = red_head; rp != NULL; rp = rp->next) {
		if (STREQ(rp->value, tmp->stptr))
			break;
	}
	free_temp(tmp);
	if (rp == NULL) /* no match */
		return tmp_number((AWKNUM) 0.0);
	fflush(stdout);	/* synchronize regular output */
	return tmp_number((AWKNUM)close_redir(rp));
}

static int
close_redir(rp)
register struct redirect *rp;
{
	int status = 0;

	if ((rp->flag & (RED_PIPE|RED_WRITE)) == (RED_PIPE|RED_WRITE))
		status = pclose(rp->fp);
	else if (rp->fp)
		status = fclose(rp->fp);
	else if (rp->iop) {
		if (rp->flag & RED_PIPE)
			status = gawk_pclose(rp);
		else
			status = iop_close(rp->iop);

	}
	/* SVR4 awk checks and warns about status of close */
	if (status)
		warning("failure status (%d) on %s close of \"%s\" (%s).",
			status,
			(rp->flag & RED_PIPE) ? "pipe" :
			"file", rp->value, strerror(errno));
	if (rp->next)
		rp->next->prev = rp->prev;
	if (rp->prev)
		rp->prev->next = rp->next;
	else
		red_head = rp->next;
	free(rp->value);
	free((char *)rp);
	return status;
}

int
flush_io ()
{
	register struct redirect *rp;
	int status = 0;

	errno = 0;
	if (fflush(stdout)) {
		warning("error writing standard output (%s).", strerror(errno));
		status++;
	}
	errno = 0;
	if (fflush(stderr)) {
		warning("error writing standard error (%s).", strerror(errno));
		status++;
	}
	for (rp = red_head; rp != NULL; rp = rp->next)
		/* flush both files and pipes, what the heck */
		if ((rp->flag & RED_WRITE) && rp->fp != NULL)
			if (fflush(rp->fp)) {
				warning("%s flush of \"%s\" failed (%s).",
				    (rp->flag  & RED_PIPE) ? "pipe" :
				    "file", rp->value, strerror(errno));
				status++;
			}
	return status;
}

int
close_io ()
{
	register struct redirect *rp;
	int status = 0;

	for (rp = red_head; rp != NULL; rp = rp->next)
		if (close_redir(rp))
			status++;
	return status;
}

/* devopen --- handle /dev/std{in,out,err}, /dev/fd/N, regular files */
int
devopen (name, mode)
char *name, *mode;
{
	int openfd = -1;
	FILE *fdopen ();
	char *cp;
	int flag = 0;

	switch(mode[0]) {
	case 'r':
		flag = O_RDONLY;
		break;

	case 'w':
		flag = O_WRONLY|O_CREAT|O_TRUNC;
		break;

	case 'a':
		flag = O_WRONLY|O_APPEND|O_CREAT;
		break;
	default:
		cant_happen();
	}

#if defined(STRICT) || defined(NO_DEV_FD)
	return (open (name, flag, 0666));
#else
	if (strict)
		return (open (name, flag, 0666));

	if (!STREQN (name, "/dev/", 5))
		return (open (name, flag, 0666));
	else
		cp = name + 5;
		
	/* XXX - first three tests ignore mode */
	if (STREQ(cp, "stdin"))
		return (0);
	else if (STREQ(cp, "stdout"))
		return (1);
	else if (STREQ(cp, "stderr"))
		return (2);
	else if (STREQN(cp, "fd/", 3)) {
		cp += 3;
		if (sscanf (cp, "%d", & openfd) == 1 && openfd >= 0)
			/* got something */
			return openfd;
		else
			return -1;
	} else
		return (open (name, flag, 0666));
#endif
}

#ifndef MSDOS
static IOBUF *
gawk_popen(cmd, rp)
char *cmd;
struct redirect *rp;
{
	int p[2];
	register int pid;

	rp->pid = -1;
	rp->iop = NULL;
	if (pipe(p) < 0)
		return NULL;
	if ((pid = fork()) == 0) {
		close(p[0]);
		dup2(p[1], 1);
		close(p[1]);
		execl("/bin/sh", "sh", "-c", cmd, 0);
		_exit(127);
	}
	if (pid == -1)
		return NULL;
	rp->pid = pid;
	close(p[1]);
	return (rp->iop = iop_alloc(p[0]));
}

static int
gawk_pclose(rp)
struct redirect *rp;
{
	SIGTYPE (*hstat)(), (*istat)(), (*qstat)();
	int pid;
	int status;
	struct redirect *redp;

	iop_close(rp->iop);
	if (rp->pid == -1)
		return rp->status;
	hstat = signal(SIGHUP, SIG_IGN);
	istat = signal(SIGINT, SIG_IGN);
	qstat = signal(SIGQUIT, SIG_IGN);
	for (;;) {
		pid = wait(&status);
		if (pid == -1 && errno == ECHILD)
			break;
		else if (pid == rp->pid) {
			rp->pid = -1;
			rp->status = status;
			break;
		} else {
			for (redp = red_head; redp != NULL; redp = redp->next)
				if (pid == redp->pid) {
					redp->pid = -1;
					redp->status = status;
					break;
				}
		}
	}
	signal(SIGHUP, hstat);
	signal(SIGINT, istat);
	signal(SIGQUIT, qstat);
	return(rp->status);
}
#else
static
struct {
	char *command;
	char *name;
} pipes[_NFILE];

static IOBUF *
gawk_popen(cmd, rp)
char *cmd;
struct redirect *rp;
{
	extern char *strdup(const char *);
	int current;
	char *name;
	static char cmdbuf[256];

	/* get a name to use.  */
	if ((name = tempnam(".", "pip")) == NULL)
		return NULL;
	sprintf(cmdbuf,"%s > %s", cmd, name);
	system(cmdbuf);
	if ((current = open(name,O_RDONLY)) == -1)
		return NULL;
	pipes[current].name = name;
	pipes[current].command = strdup(cmd);
	return (rp->iop = iop_alloc(current));
}

static int
gawk_pclose(rp)
struct redirect *rp;
{
	int cur = rp->iop->fd;
	int rval;

	rval = iop_close(rp->iop);

	/* check for an open file  */
	if (pipes[cur].name == NULL)
		return -1;
	unlink(pipes[cur].name);
	free(pipes[cur].name);
	pipes[cur].name = NULL;
	free(pipes[cur].command);
	return rval;
}
#endif

#define	DO_END_OF_BUF	len = bp - iop->off;\
			used = last - start;\
			while (len + used > iop->secsiz) {\
				iop->secsiz *= 2;\
				erealloc(iop->secbuf,char *,iop->secsiz,"get");\
			}\
			last = iop->secbuf + used;\
			start = iop->secbuf;\
			memcpy(last, iop->off, len);\
			last += len;\
			iop->cnt = read(iop->fd, iop->buf, iop->size);\
			if (iop->cnt < 0)\
				return iop->cnt;\
			end_data = iop->buf + iop->cnt;\
			iop->off = bp = iop->buf;

#define	DO_END_OF_DATA	iop->cnt = read(iop->fd, end_data, end_buf - end_data);\
			if (iop->cnt < 0)\
				return iop->cnt;\
			end_data += iop->cnt;\
			if (iop->cnt == 0)\
				break;\
			iop->cnt = end_data - iop->buf;

static int
get_a_record(res, iop)
char **res;
IOBUF *iop;
{
	register char *end_data;
	register char *end_buf;
	char *start;
	register char *bp;
	register char *last;
	int len, used;
	register char rs = get_rs();

	if (iop->cnt < 0)
		return iop->cnt;
	if ((iop->flag & IOP_IS_TTY) && output_is_tty)
		fflush(stdout);
	end_data = iop->buf + iop->cnt;
	if (iop->off >= end_data) {
		iop->cnt = read(iop->fd, iop->buf, iop->size);
		if (iop->cnt <= 0)
			return iop->cnt = EOF;
		end_data = iop->buf + iop->cnt;
		iop->off = iop->buf;
	}
	last = start = bp = iop->off;
	end_buf = iop->buf + iop->size;
	if (rs == 0) {
		while (!(*bp == '\n' && bp != iop->buf && bp[-1] == '\n')) {
			if (++bp == end_buf) {
				DO_END_OF_BUF
			}
			if (bp == end_data) {
				DO_END_OF_DATA
			}
		}
		if (*bp == '\n' && bp != iop->off && bp[-1] == '\n') {
			int tmp = 0;

			/* allow for more than two newlines */
			while (*bp == '\n') {
				tmp++;
				if (++bp == end_buf) {
					DO_END_OF_BUF
				}
				if (bp == end_data) {
					DO_END_OF_DATA
				}
			}
			iop->off = bp;
			bp -= 1 + tmp;
		} else if (bp != iop->buf && bp[-1] != '\n') {
			warning("record not terminated");
			iop->off = bp + 2;
		} else {
			bp--;
			iop->off = bp + 2;
		}
	} else {
		while (*bp++ != rs) {
			if (bp == end_buf) {
				DO_END_OF_BUF
			}
			if (bp == end_data) {
				DO_END_OF_DATA
			}
		}
		if (*--bp != rs) {
			warning("record not terminated");
			bp++;
		}
		iop->off = bp + 1;
	}
	if (start == iop->secbuf) {
		len = bp - iop->buf;
		if (len > 0) {
			used = last - start;
			while (len + used > iop->secsiz) {
				iop->secsiz *= 2;
				erealloc(iop->secbuf,char *,iop->secsiz,"get2");
			}
			last = iop->secbuf + used;
			start = iop->secbuf;
			memcpy(last, iop->buf, len);
			last += len;
		}
	} else
		last = bp;
	*last = '\0';
	*res = start;
	return last - start;
}

NODE *
do_getline(tree)
NODE *tree;
{
	struct redirect *rp;
	IOBUF *iop;
	int cnt;
	NODE **lhs;
	int redir_error = 0;

	if (tree->rnode == NULL) {	 /* no redirection */
		iop = nextfile();
		if (iop == NULL)		/* end of input */
			return tmp_number((AWKNUM) 0.0);
	} else {
		rp = redirect(tree->rnode, &redir_error);
		if (rp == NULL && redir_error)	/* failed redirect */
			return tmp_number((AWKNUM) -1.0);
		iop = rp->iop;
		getline_redirect++;
	}
	if (tree->lnode == NULL) {	/* no optional var. -- read in $0 */
		if (inrec(iop) != 0) {
			getline_redirect = 0;
			return tmp_number((AWKNUM) 0.0);
		}
	} else {			/* read in a named variable */
		char *s = NULL;

		lhs = get_lhs(tree->lnode, 1);
		cnt = get_a_record(&s, iop);
		if (!getline_redirect) {
			assign_number(&NR_node->var_value,
			    NR_node->var_value->numbr + 1.0);
			assign_number(&FNR_node->var_value,
			    FNR_node->var_value->numbr + 1.0);
		}
		if (cnt == EOF) {
			getline_redirect = 0;
			free(s);
			return tmp_number((AWKNUM) 0.0);
		}
		*lhs = make_string(s, strlen(s));
		do_deref();
		/* we may have to regenerate $0 here! */
		if (field_num == 0)
			set_record(fields_arr[0]->stptr, fields_arr[0]->stlen);
		field_num = -1;
	}
	getline_redirect = 0;
	return tmp_number((AWKNUM) 1.0);
}
