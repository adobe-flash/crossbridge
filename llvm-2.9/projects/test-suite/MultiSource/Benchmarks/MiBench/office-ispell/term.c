#ifndef lint
static char Rcs_Id[] =
    "$Id: term.c 83944 2009-10-13 05:37:17Z lattner $";
#endif

/*
 * term.c - deal with termcap, and unix terminal mode settings
 *
 * Pace Willisson, 1983
 *
 * Copyright 1987, 1988, 1989, 1992, 1993, Geoff Kuenning, Granada Hills, CA
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All modifications to the source code must be clearly marked as
 *    such.  Binary redistributions based on modified source code
 *    must be clearly marked as modified versions in the documentation
 *    and/or other materials provided with the distribution.
 * 4. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgment:
 *      This product includes software developed by Geoff Kuenning and
 *      other unpaid contributors.
 * 5. The name of Geoff Kuenning may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY GEOFF KUENNING AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL GEOFF KUENNING OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * $Log$
 * Revision 1.1  2007/01/09 23:57:18  lattner
 * initial recheckin of mibench
 *
 * Revision 1.1.1.1  2007/01/09 02:59:05  evancheng
 * Add selected tests from MiBench 1.0 to LLVM test suite.
 *
 * Revision 1.48  1994/10/25  05:46:11  geoff
 * Fix a couple of places where ifdefs were omitted, though apparently
 * harmlessly.
 *
 * Revision 1.47  1994/09/01  06:06:32  geoff
 * Change erasechar/killchar to uerasechar/ukillchar to avoid
 * shared-library problems on HP systems.
 *
 * Revision 1.46  1994/01/25  07:12:11  geoff
 * Get rid of all old RCS log lines in preparation for the 3.1 release.
 *
 */

#include "config.h"
#include "ispell.h"
#include "proto.h"
#include "msgs.h"
#include <signal.h>

#if defined(__GLIBC__) || defined(__FreeBSD__)
/* Use termios under at least glibc */
#include <termios.h>
#define USE_TERMIOS
#ifndef USG
#define USG
#endif
#else
#ifdef USG
#include <termio.h>
#else
#include <sgtty.h>
#endif
#endif

void		erase P ((void));
void		move P ((int row, int col));
void		inverse P ((void));
void		normal P ((void));
void		backup P ((void));
static int	putch P ((int c));
void		terminit P ((void));
SIGNAL_TYPE	done P ((int signo));
#ifdef SIGTSTP
static SIGNAL_TYPE onstop P ((int signo));
#endif /* SIGTSTP */
void		stop P ((void));
int		shellescape P ((char * buf));
#ifdef USESH
void		shescape P ((char * buf));
#endif /* USESH */

void erase ()
    {
#ifdef FIXME
    if (cl)
	tputs (cl, li, putch);
    else
	{
	if (ho)
	    tputs (ho, 100, putch);
	else if (cm)
	    tputs (tgoto (cm, 0, 0), 100, putch);
	tputs (cd, li, putch);
	}
#endif
    }

void move (row, col)
    int		row;
    int		col;
    {
#ifdef FIXME
    tputs (tgoto (cm, col, row), 100, putch);
#endif
    }

void inverse ()
    {
#ifdef FIXME
    tputs (so, 10, putch);
#endif
    }

void normal ()
    {
#ifdef FIXME
    tputs (se, 10, putch);
#endif
    }

void backup ()
    {
#ifdef FIXME
    if (BC)
	tputs (BC, 1, putch);
    else
	(void) putchar ('\b');
#endif
    }

static int putch (c)
    int			c;
    {

    return putchar (c);
    }

#ifdef USE_TERMIOS
static struct termios	sbuf;
static struct termios	osbuf;
#else
#ifdef USG
static struct termio	sbuf;
static struct termio	osbuf;
#else
static struct sgttyb	sbuf;
static struct sgttyb	osbuf;
#ifdef TIOCSLTC
static struct ltchars	ltc;
static struct ltchars	oltc;
#endif
#endif
#endif
static int		termchanged = 0;
static SIGNAL_TYPE	(*oldint) ();
static SIGNAL_TYPE	(*oldterm) ();
#ifdef SIGTSTP
static SIGNAL_TYPE	(*oldttin) ();
static SIGNAL_TYPE	(*oldttou) ();
static SIGNAL_TYPE	(*oldtstp) ();
#endif

void terminit ()
    {
#ifdef TIOCPGRP
    int			tpgrp;
#else
#ifdef TIOCGPGRP
    int			tpgrp;
#endif
#endif
#ifdef TIOCGWINSZ
    struct winsize	wsize;
#endif /* TIOCGWINSZ */

#ifdef FIXME
    tgetent (termcap, getenv ("TERM"));
    termptr = termstr;
    BC = tgetstr ("bc", &termptr);
    cd = tgetstr ("cd", &termptr);
    cl = tgetstr ("cl", &termptr);
    cm = tgetstr ("cm", &termptr);
    ho = tgetstr ("ho", &termptr);
    nd = tgetstr ("nd", &termptr);
    so = tgetstr ("so", &termptr);	/* inverse video on */
    se = tgetstr ("se", &termptr);	/* inverse video off */
    if ((sg = tgetnum ("sg")) < 0)	/* space taken by so/se */
	sg = 0;
    ti = tgetstr ("ti", &termptr);	/* terminal initialization */
    te = tgetstr ("te", &termptr);	/* terminal termination */
    co = tgetnum ("co");
    li = tgetnum ("li");
#endif
#ifdef TIOCGWINSZ
    if (ioctl (0, TIOCGWINSZ, (char *) &wsize) >= 0)
	{
	if (wsize.ws_col != 0)
	    co = wsize.ws_col;
	if (wsize.ws_row != 0)
	    li = wsize.ws_row;
	}
#endif /* TIOCGWINSZ */
    /*
     * Let the variables "LINES" and "COLUMNS" override the termcap
     * entry.  Technically, this is a terminfo-ism, but I think the
     * vast majority of users will find it pretty handy.
     */
    if (getenv ("COLUMNS") != NULL)
	co = atoi (getenv ("COLUMNS"));
    if (getenv ("LINES") != NULL)
	li = atoi (getenv ("LINES"));
#if MAX_SCREEN_SIZE > 0
    if (li > MAX_SCREEN_SIZE)
	li = MAX_SCREEN_SIZE;
#endif /* MAX_SCREEN_SIZE > 0 */
#if MAXCONTEXT == MINCONTEXT
    contextsize = MINCONTEXT;
#else /* MAXCONTEXT == MINCONTEXT */
    if (contextsize == 0)
#ifdef CONTEXTROUNDUP
	contextsize = (li * CONTEXTPCT + 99) / 100;
#else /* CONTEXTROUNDUP */
	contextsize = (li * CONTEXTPCT) / 100;
#endif /* CONTEXTROUNDUP */
    if (contextsize > MAXCONTEXT)
	contextsize = MAXCONTEXT;
    else if (contextsize < MINCONTEXT)
	contextsize = MINCONTEXT;
#endif /* MAX_CONTEXT == MIN_CONTEXT */
    /*
     * Insist on 2 lines for the screen header, 2 for blank lines
     * separating areas of the screen, 2 for word choices, and 2 for
     * the minimenu, plus however many are needed for context.  If
     * possible, make the context smaller to fit on the screen.
     */
    if (li < contextsize + 8  &&  contextsize > MINCONTEXT)
	{
	contextsize = li - 8;
	if (contextsize < MINCONTEXT)
	    contextsize = MINCONTEXT;
	}
    if (li < MINCONTEXT + 8)
	(void) fprintf (stderr, TERM_C_SMALL_SCREEN, MINCONTEXT + 8);

#ifdef SIGTSTP
#ifdef TIOCPGRP
retry:
#endif /* SIGTSTP */
#endif /* TIOCPGRP */

#ifdef USG
    if (!isatty (0))
	{
	(void) fprintf (stderr, TERM_C_NO_BATCH);
	exit (1);
	}
#ifdef USE_TERMIOS
    (void) tcgetattr (0, &osbuf);
#else
    (void) ioctl (0, TCGETA, (char *) &osbuf);
#endif
    termchanged = 1;

    sbuf = osbuf;
    sbuf.c_lflag &= ~(ECHO | ECHOK | ECHONL | ICANON);
    sbuf.c_oflag &= ~(OPOST);
    sbuf.c_iflag &= ~(INLCR | IGNCR | ICRNL);
    sbuf.c_cc[VMIN] = 1;
    sbuf.c_cc[VTIME] = 1;
#ifdef USE_TERMIOS
    (void) tcsetattr (0, TCSANOW, &sbuf);
#else
    (void) ioctl (0, TCSETAW, (char *) &sbuf);
#endif

    uerasechar = osbuf.c_cc[VERASE];
    ukillchar = osbuf.c_cc[VKILL];

#endif

#ifdef SIGTSTP
#ifndef USG
    (void) sigsetmask (1<<(SIGTSTP-1) | 1<<(SIGTTIN-1) | 1<<(SIGTTOU-1));
#endif
#endif
#ifdef TIOCGPGRP
    if (ioctl (0, TIOCGPGRP, (char *) &tpgrp) != 0)
	{
	(void) fprintf (stderr, TERM_C_NO_BATCH);
	exit (1);
	}
#endif
#ifdef SIGTSTP
#ifdef TIOCPGRP
    if (tpgrp != getpgrp(0)) /* not in foreground */
	{
#ifndef USG
	(void) sigsetmask (1 << (SIGTSTP - 1) | 1 << (SIGTTIN - 1));
#endif
	(void) signal (SIGTTOU, SIG_DFL);
	(void) kill (0, SIGTTOU);
	/* job stops here waiting for SIGCONT */
	goto retry;
	}
#endif
#endif

#ifndef USG
    (void) ioctl (0, TIOCGETP, (char *) &osbuf);
#ifdef TIOCGLTC
    (void) ioctl (0, TIOCGLTC, (char *) &oltc);
#endif
    termchanged = 1;

    sbuf = osbuf;
    sbuf.sg_flags &= ~ECHO;
    sbuf.sg_flags |= TERM_MODE;
    (void) ioctl (0, TIOCSETP, (char *) &sbuf);

    uerasechar = sbuf.sg_erase;
    ukillchar = sbuf.sg_kill;

#ifdef TIOCSLTC
    ltc = oltc;
    ltc.t_suspc = -1;
    (void) ioctl (0, TIOCSLTC, (char *) &ltc);
#endif

#endif /* USG */

    if ((oldint = signal (SIGINT, SIG_IGN)) != SIG_IGN)
	(void) signal (SIGINT, done);
    if ((oldterm = signal (SIGTERM, SIG_IGN)) != SIG_IGN)
	(void) signal (SIGTERM, done);

#ifdef SIGTSTP
#ifndef USG
    (void) sigsetmask (0);
#endif
    if ((oldttin = signal (SIGTTIN, SIG_IGN)) != SIG_IGN)
	(void) signal (SIGTTIN, onstop);
    if ((oldttou = signal (SIGTTOU, SIG_IGN)) != SIG_IGN)
	(void) signal (SIGTTOU, onstop);
    if ((oldtstp = signal (SIGTSTP, SIG_IGN)) != SIG_IGN)
	(void) signal (SIGTSTP, onstop);
#endif
#ifdef FIXME
    if (ti)
	tputs (ti, 1, putch);
#endif
    }

/* ARGSUSED */
SIGNAL_TYPE done (signo)
    int		signo;
    {
    if (tempfile[0] != '\0')
	(void) unlink (tempfile);
    if (termchanged)
	{
#ifdef FIXME
	if (te)
	    tputs (te, 1, putch);
#ifdef USE_TERMIOS
	(void) tcsetattr (0, TCSADRAIN, &osbuf);
#else
#ifdef USG
	(void) ioctl (0, TCSETAW, (char *) &osbuf);
#else
	(void) ioctl (0, TIOCSETP, (char *) &osbuf);
#ifdef TIOCSLTC
	(void) ioctl (0, TIOCSLTC, (char *) &oltc);
#endif
#endif
#endif
#endif
	}
    exit (0);
    }

#ifdef SIGTSTP
static SIGNAL_TYPE onstop (signo)
    int		signo;
    {
#ifdef USE_TERMIOS
    (void) tcsetattr (0, TCSANOW, &osbuf);
#else
#ifdef USG
    (void) ioctl (0, TCSETAW, (char *) &osbuf);
#else
    (void) ioctl (0, TIOCSETP, (char *) &osbuf);
#ifdef TIOCSLTC
    (void) ioctl (0, TIOCSLTC, (char *) &oltc);
#endif
#endif
#endif
    (void) signal (signo, SIG_DFL);
#ifndef USG
    (void) sigsetmask (sigblock (0) & ~(1 << (signo - 1)));
#endif
    (void) kill (0, signo);
    /* stop here until continued */
    (void) signal (signo, onstop);
#ifdef USE_TERMIOS
    (void) tcsetattr (0, TCSANOW, &sbuf);
#else
#ifdef USG
    (void) ioctl (0, TCSETAW, (char *) &sbuf);
#else
    (void) ioctl (0, TIOCSETP, (char *) &sbuf);
#ifdef TIOCSLTC
    (void) ioctl (0, TIOCSLTC, (char *) &ltc);
#endif
#endif
#endif
    }
#endif

void stop ()
    {
#ifdef SIGTSTP
    onstop (SIGTSTP);
#else
    /* for System V */
    move (li - 1, 0);
    (void) fflush (stdout);
    if (getenv ("SHELL"))
	(void) shellescape (getenv ("SHELL"));
    else
	(void) shellescape ("sh");
#endif
    }

/* Fork and exec a process.  Returns NZ if command found, regardless of
** command's return status.  Returns zero if command was not found.
** Doesn't use a shell.
*/
#ifndef USESH
#define NEED_SHELLESCAPE
#endif /* USESH */
#ifndef REGEX_LOOKUP
#define NEED_SHELLESCAPE
#endif /* REGEX_LOOKUP */
#ifdef NEED_SHELLESCAPE
int shellescape	(buf)
    char *	buf;
    {
    char *	argv[100];
    char *	cp = buf;
    int		i = 0;
    int		termstat;

    /* parse buf to args (destroying it in the process) */
    while (*cp != '\0')
	{
	while (*cp == ' '  ||  *cp == '\t')
	    ++cp;
	if (*cp == '\0')
	    break;
	argv[i++] = cp;
	while (*cp != ' '  &&  *cp != '\t'  &&  *cp != '\0')
	    ++cp;
	if (*cp != '\0')
	    *cp++ = '\0';
	}
    argv[i] = NULL;

#ifdef USE_TERMIOS
    (void) tcsetattr (0, TCSANOW, &osbuf);
#else
#ifdef USG
    (void) ioctl (0, TCSETAW, (char *) &osbuf);
#else
    (void) ioctl (0, TIOCSETP, (char *) &osbuf);
#ifdef TIOCSLTC
    (void) ioctl (0, TIOCSLTC, (char *) &oltc);
#endif /* TIOCSLTC */
#endif
#endif
    (void) signal (SIGINT, oldint);
    (void) signal (SIGTERM, oldterm);
#ifdef SIGTSTP
    (void) signal (SIGTTIN, oldttin);
    (void) signal (SIGTTOU, oldttou);
    (void) signal (SIGTSTP, oldtstp);
#endif
    if ((i = fork ()) == 0)
	{
	(void) execvp (argv[0], (char **) argv);
	_exit (123);		/* Command not found */
	}
    else if (i > 0)
	{
	while (wait (&termstat) != i)
	    ;
	termstat = (termstat == (123 << 8)) ? 0 : -1;
	}
    else
	{
	(void) printf (TERM_C_CANT_FORK);
	termstat = -1;		/* Couldn't fork */
	}

    if (oldint != SIG_IGN)
	(void) signal (SIGINT, done);
    if (oldterm != SIG_IGN)
	(void) signal (SIGTERM, done);

#ifdef SIGTSTP
    if (oldttin != SIG_IGN)
	(void) signal (SIGTTIN, onstop);
    if (oldttou != SIG_IGN)
	(void) signal (SIGTTOU, onstop);
    if (oldtstp != SIG_IGN)
	(void) signal (SIGTSTP, onstop);
#endif

#ifdef USE_TERMIOS
    (void) tcsetattr (0, TCSANOW, &sbuf);
#else
#ifdef USG
    (void) ioctl (0, TCSETAW, (char *) &sbuf);
#else
    (void) ioctl (0, TIOCSETP, (char *) &sbuf);
#ifdef TIOCSLTC
    (void) ioctl (0, TIOCSLTC, (char *) &ltc);
#endif /* TIOCSLTC */
#endif
#endif
    if (termstat)
	{
	(void) printf (TERM_C_TYPE_SPACE);
	(void) fflush (stdout);
#ifdef COMMANDFORSPACE
	i = GETKEYSTROKE ();
	if (i != ' ' && i != '\n' && i != '\r')
	    (void) ungetc (i, stdin);
#else
	while (GETKEYSTROKE () != ' ')
	    ;
#endif
	}
    return (termstat);
    }
#endif /* NEED_SHELLESCAPE */

#ifdef	USESH
void shescape (buf)
    char *	buf;
    {
#ifdef COMMANDFORSPACE
    int		ch;
#endif

#ifdef USE_TERMIOS
    (void) tcsetattr (0, TCSANOW, &osbuf);
#else
#ifdef USG
    (void) ioctl (0, TCSETAW, (char *) &osbuf);
#else
    (void) ioctl (0, TIOCSETP, (char *) &osbuf);
#ifdef TIOCSLTC
    (void) ioctl (0, TIOCSLTC, (char *) &oltc);
#endif
#endif
#endif
    (void) signal (SIGINT, oldint);
    (void) signal (SIGTERM, oldterm);
#ifdef SIGTSTP
    (void) signal (SIGTTIN, oldttin);
    (void) signal (SIGTTOU, oldttou);
    (void) signal (SIGTSTP, oldtstp);
#endif

    (void) system (buf);

    if (oldint != SIG_IGN)
	(void) signal (SIGINT, done);
    if (oldterm != SIG_IGN)
	(void) signal (SIGTERM, done);

#ifdef SIGTSTP
    if (oldttin != SIG_IGN)
	(void) signal (SIGTTIN, onstop);
    if (oldttou != SIG_IGN)
	(void) signal (SIGTTOU, onstop);
    if (oldtstp != SIG_IGN)
	(void) signal (SIGTSTP, onstop);
#endif

#ifdef USE_TERMIOS
    (void) tcsetattr (0, TCSANOW, &sbuf);
#else
#ifdef USG
    (void) ioctl (0, TCSETAW, (char *) &sbuf);
#else
    (void) ioctl (0, TIOCSETP, (char *) &sbuf);
#ifdef TIOCSLTC
    (void) ioctl (0, TIOCSLTC, (char *) &ltc);
#endif
#endif
#endif
    (void) printf (TERM_C_TYPE_SPACE);
    (void) fflush (stdout);
#ifdef COMMANDFORSPACE
    ch = GETKEYSTROKE ();
    if (ch != ' '  &&  ch != '\n'  &&  ch != '\r')
	(void) ungetc (ch, stdin);
#else
    while (GETKEYSTROKE () != ' ')
	;
#endif
    }
#endif
