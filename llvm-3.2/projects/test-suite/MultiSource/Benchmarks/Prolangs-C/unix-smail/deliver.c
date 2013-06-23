/*
**  Deliver.c
**
**  Routines to effect delivery of mail for rmail/smail. 
**
*/

#ifndef lint
static char 	*sccsid="@(#)deliver.c	2.5 (smail) 9/15/87";
#endif

# include	<stdio.h>
# include	<sys/types.h>
# include	<sys/stat.h>
# include	<ctype.h>
# include	<signal.h>
# include	"defs.h"

extern int  exitstat;		/* set if a forked mailer fails */
extern enum edebug debug;	/* how verbose we are 		*/ 
extern char hostname[];		/* our uucp hostname 		*/
extern char hostdomain[];	/* our host's domain 		*/
extern enum ehandle handle;	/* what we handle		*/
extern enum erouting routing;	/* how we're routing addresses  */
extern char *uuxargs;		/* arguments given to uux       */
extern int  queuecost;		/* threshold for queueing mail  */
extern int  maxnoqueue;		/* max number of uucico's       */
extern char *spoolfile;		/* file name of spooled message */
extern FILE *spoolfp;		/* file ptr  to spooled message */
extern int spoolmaster;		/* set if creator of spoolfile  */
extern char nows[];		/* local time in ctime(3) format*/
extern char arpanows[];		/* local time in arpadate format*/
char stderrfile[20];		/* error file for stderr traping*/

extern char *postmaster(char *user);

void return_mail(char *from,char *fcommand);

/*
**
**  deliver():  hand the letter to the proper mail programs.
**
**  Issues one command for each different host of <hostv>,
**  constructing the proper command for LOCAL or UUCP mail.
**  Note that LOCAL mail has blank host names.
**
**  The <userv> names for each host are arguments to the command.
** 
**  Prepends a "From" line to the letter just before going 
**  out, with a "remote from <hostname>" if it is a UUCP letter.
**
*/

void deliver(int argc,char *hostv[],char *userv[],enum eform formv[],
             int costv[])
{
	FILE *out;			/* pipe to mailer		*/
	FILE *popen();			/* to fork a mailer 		*/
#ifdef RECORD
	void record();			/* record all transactions	*/
#endif
#ifdef LOG
	void log();
#endif
	char *mktemp();
	char from[SMLBUF];		/* accumulated from argument 	*/
	char lcommand[SMLBUF];		/* local command issued 	*/
	char rcommand[SMLBUF];		/* remote command issued	*/
	char scommand[SMLBUF];		/* retry  command issued	*/
	char *command;			/* actual command		*/
	char buf[SMLBUF];		/* copying rest of the letter   */
	enum eform form;		/* holds form[i] for speed 	*/
	long size;			/* number of bytes of message 	*/
	char *flags;			/* flags for uux		*/
	char *sflag;			/* flag  for smail		*/
	int i, j, status, retrying;
	char *c, *postmaster();
	int failcount = 0;
	int noqcnt = 0;			/* number of uucico's started   */
	char *uux_noqueue = UUX_NOQUEUE;/* uucico starts immediately    */
	char *uux_queue   = UUX_QUEUE;	/* uucico job gets queued       */
	off_t message;
	struct stat st;

/*
** rewind the spool file and read the collapsed From_ line
*/
	(void) fseek(spoolfp, 0L, 0);
	(void) fgets(from, sizeof(from), spoolfp);
	if((c = index(from, '\n')) != 0) *c = '\0';
	message = ftell(spoolfp);

/*
**  We pass through the list of addresses.
*/
	stderrfile[0] = '\0';
	for(i = 0; i < argc; i++) {
		char *lend = lcommand;
		char *rend = rcommand;
		char *send = scommand;

/*
**  If we don't have sendmail, arrange to trap standard error
**  for inclusion in the message that is returned with failed mail.
*/
		(void) unlink(stderrfile);
		(void) strcpy(stderrfile, "/tmp/stderrXXXXXX");
		(void) mktemp(stderrfile);
		(void) freopen(stderrfile, "w", stderr);
		if(debug != YES) {
			(void) freopen(stderrfile, "w", stdout);
		}

		*lend = *rend = *send = '\0';

/*
**  If form == ERROR, the address was bad 
**  If form == SENT, it has been sent on a  previous pass.
*/
		form = formv[i];
		if (form == SENT) {
			continue;
		}
/*
**  Build the command based on whether this is local mail or uucp mail.
**  By default, don't allow more than 'maxnoqueue' uucico commands to
**  be started by a single invocation of 'smail'.
*/
		if(uuxargs == NULL) {	/* flags not set on command line */
			if(noqcnt < maxnoqueue && costv[i] <= queuecost) {
				flags = uux_noqueue;
			} else {
				flags = uux_queue;
			}
		} else {
			flags = uuxargs;
		}

		retrying = 0;
		if(routing == JUSTDOMAIN) {
			sflag = "-r";
		} else if(routing == ALWAYS) {
			sflag = "-R";
		} else {
			sflag = "";
		}

		(void) sprintf(lcommand, LMAIL(from, hostv[i]));
		(void) sprintf(rcommand, RMAIL(flags, from, hostv[i]));

/*
**  For each address with the same host name and form, append the user
**  name to the command line, and set form = ERROR so we skip this address
**  on later passes. 
*/
		/* we initialized lend (rend) to point at the
		 * beginning of its buffer, so that at
		 * least one address will be used regardless
		 * of the length of lcommand (rcommand).
		 */
		for (j = i; j < argc; j++) {
		        int tmp1,tmp2;		  
			if ((formv[j] != form)
			 || (strcmpic(hostv[i], hostv[j]) != 0)
			 || ((tmp1 = (lend - lcommand)) > MAXCLEN)
			 || ((tmp2 = (rend - rcommand)) > MAXCLEN)) {
				continue;
			}

			/*
			** seek to the end of scommand
			** and add on a 'smail' command
			** multiple commands are separated by ';'
			*/

			send += strlen(send);
			if(send != scommand) {
				*send++ = ';' ;
			}

			(void) sprintf(send, RETRY(sflag));
			send += strlen(send);

			lend += strlen(lend);
			rend += strlen(rend);

			if (form == LOCAL) {
				(void) sprintf(lend, LARG(userv[j]));
				(void) sprintf(send, LARG(userv[j]));
			} else {
				(void) sprintf(lend, RLARG(hostv[i], userv[j]));
				(void) sprintf(send, RLARG(hostv[i], userv[j]));
			}

			(void) sprintf(rend, RARG(userv[j]));
			formv[j] = SENT;
		}
retry:
/*
** rewind the spool file and read the collapsed From_ line
*/
		(void) fseek(spoolfp, message, 0);

		/* if the address was in a bogus form (usually DOMAIN),
		** then don't bother trying the uux.
		**
		** Rather, go straight to the next smail routing level.
		*/
		if(form == ERROR) {
			static char errbuf[SMLBUF];
			(void) sprintf(errbuf,
				"address resolution ('%s' @ '%s') failed",
					userv[i], hostv[i]);
			command = errbuf;
			size    = 0;
			goto form_error;
		}

		if (retrying) {
			command = scommand;
		} else if (form == LOCAL) {
			command = lcommand;
		} else {
			command = rcommand;
			if(flags == uux_noqueue) {
				noqcnt++;
			}
		}
		ADVISE("COMMAND: %s\n", command);

/*
** Fork the mailer and set it up for writing so we can send the mail to it,
** or for debugging divert the output to stdout.
*/

/*
** We may try to write on a broken pipe, if the uux'd host
** is unknown to us.  Ignore this signal, since we can use the
** return value of the pclose() as our indication of failure.
*/
		(void) signal(SIGPIPE, SIG_IGN);

		if (debug == YES) {
			out = stdout;
		} else {
			failcount = 0;
			do {
				out = popen(command, "w");
				if (out) break;
				/*
				 * Fork failed.  System probably overloaded.
				 * Wait awhile and try again 10 times.
				 * If it keeps failing, probably some
				 * other problem, like no uux or smail.
				 */
				(void) sleep(60);
			} while (++failcount < 10);
		}
		if(out == NULL) {
			exitstat = EX_UNAVAILABLE;
			(void) printf("couldn't execute %s.\n", command);
			continue;
		}

		size = 0;
		if(fstat(fileno(spoolfp), &st) >= 0) {
			size = st.st_size - message;
		}
/*
**  Output our From_ line.
*/
		if (form == LOCAL) {
#ifdef SENDMAIL
			(void) sprintf(buf, LFROM(from, nows, hostname));
			size += strlen(buf);
			(void) fputs(buf, out);
#else
			char *p;
			if((p=index(from, '!')) == NULL) {
				(void) sprintf(buf,
					LFROM(from, nows, hostname));
				size += strlen(buf);
				(void) fputs(buf, out);
			} else {
				*p = NULL;
				(void) sprintf(buf, RFROM(p+1, nows, from));
				size += strlen(buf);
				(void) fputs(buf, out);
				*p = '!';
			}
#endif
		} else {
			(void) sprintf(buf, RFROM(from, nows, hostname));
			size += strlen(buf);
			(void) fputs(buf, out);
		}

#ifdef SENDMAIL
/*
**  If using sendmail, insert a Received: line only for mail
**  that is being passed to uux.  If not using sendmail, always
**  insert the received line, since sendmail isn't there to do it.
*/
		if(command == rcommand && handle != ALL)
#endif
		{
			(void) sprintf(buf,
				"Received: by %s (%s)\n\tid AA%05d; %s\n",
					hostdomain, VERSION,
					getpid(), arpanows);
			size += strlen(buf);
			(void) fputs(buf, out);
		}

/*
**  Copy input.
*/
		while(fgets(buf, sizeof(buf), spoolfp) != NULL) {
			(void) fputs(buf, out);
		}
/*
**  Get exit status and if non-zero, set global exitstat so when we exit
**  we can indicate an error.
*/
form_error:
		if (debug != YES) {
			if(form == ERROR) {
				exitstat = EX_NOHOST;
			} else if (status = pclose(out)) {
				exitstat = status >> 8;
			}
			/*
			 * The 'retrying' check prevents a smail loop.
			 */
			if(exitstat != 0) {
				/*
				** the mail failed, probably because the host
				** being uux'ed isn't in L.sys or local user
				** is unknown.
				*/

				if((retrying == 0)	/* first pass */
				&& (routing != REROUTE)	/* have higher level */
				&& (form != LOCAL)) {	/* can't route local */
					/*
					** Try again using a higher
					** level of routing.
					*/
					ADVISE("%s failed (%d)\ntrying %s\n",
						command, exitstat, scommand);
					exitstat = 0;
					retrying = 1;
					form = SENT;
					goto retry;
				}

				/*
				** if we have no other routing possibilities
				** see that the mail is returned to sender.
				*/

				if((routing == REROUTE)
			        || (form == LOCAL)) {

					/*
					** if this was our last chance,
					** return the mail to the sender.
					*/

					ADVISE("%s failed (%d)\n",
						command, exitstat);
					
					(void) fseek(spoolfp, message, 0);
#ifdef SENDMAIL
					/* if we have sendmail, then it
					** was handed the mail, which failed.
					** sendmail returns the failed mail
					** for us, so we need not do it again.
					*/
					if(form != LOCAL)
#endif
					{
						return_mail(from, command);
					}
					exitstat = 0;
				}
			}
# ifdef LOG
			else {
				if(retrying == 0) log(command, from, size); /* */
			}
# endif
		}
	}
/*
**  Update logs and records.
*/
# ifdef RECORD
	(void) fseek(spoolfp, message, 0);
	record(command, from, size);
# endif

/*
**  close spool file pointer.
**  if we created it, then unlink file.
*/
	(void) fclose(spoolfp);
	if(spoolmaster) {
		(void) unlink(spoolfile);
	}
	(void) unlink(stderrfile);
}

/*
** return mail to sender, as determined by From_ line.
*/

void return_mail(char *from,char *fcommand)
{
	char buf[SMLBUF];
	char domain[SMLBUF], user[SMLBUF];
	char *r;
	FILE *fp, *out, *popen();
	int i = 0;

	r = buf;

	(void) sprintf(r, "%s %s", SMAIL, VFLAG);
	r += strlen(r);

	if(islocal(from, domain, user)) {
		(void) sprintf(r, LARG(user));
	} else {
		(void) sprintf(r, RLARG(domain, user));
	}

	i = 0;
	do {
		out = popen(buf, "w");
		if (out) break;
		/*
		 * Fork failed.  System probably overloaded.
		 * Wait awhile and try again 10 times.
		 * If it keeps failing, probably some
		 * other problem, like no uux or smail.
		 */
		(void) sleep(60);
	} while (++i < 10);

	if(out == NULL) {
		(void) printf("couldn't execute %s.\n", buf);
		return;
	}

	(void) fprintf(out, "Date: %s\n", arpanows);
	(void) fprintf(out, "From: MAILER-DAEMON@%s\n", hostdomain);
	(void) fprintf(out, "Subject: failed mail\n");
	(void) fprintf(out, "To: %s\n", from);
	(void) fprintf(out, "\n");
	(void) fprintf(out, "=======     command failed      =======\n\n");
	(void) fprintf(out, " COMMAND: %s\n\n", fcommand);

	(void) fprintf(out, "======= standard error follows  =======\n");
	(void) fflush(stderr);
	if((fp = fopen(stderrfile, "r")) != NULL) {
		while(fgets(buf, sizeof(buf), fp) != NULL) {
			(void) fputs(buf, out);
		}
	}
	(void) fclose(fp);
	(void) fprintf(out, "======= text of message follows =======\n");
/*
**  Copy input.
*/
	(void) fprintf(out, "From %s\n", from);
	while(fgets(buf, sizeof(buf), spoolfp) != NULL) {
		(void) fputs(buf, out);
	}
	(void) pclose(out);
}
