/*
 * Copyright (c) 1989, 1990, 1991 by the University of Washington
 *
 * For copying and distribution information, please see the file
 * <copyright.h>.
 *
 * v1.2.8 - 12/17/91 (jwb) - fixed warning message
 * v1.2.7 - 12/13/91 (bpk) - took out XARCHIE code since we've diverged
 * v1.2.6 - 12/13/91 (jwb) - UCX stuff
 * v1.2.5 - 11/21/91 (bkc) - new version for CUTCP, ugly as it is.
 * v1.2.4 - 11/19/91 (mmt) - added MSDOS & OS2 stuff
 * v1.2.3 - 11/04/91 (bcn) - removed host comparison and replaced with check
 *			     for connection id (undoes effect of v1.2.2.).
 * v1.2.2 - 11/02/91 (gf)  - removed extra inet_ntoa() calls and stuff for
 *			     multi-interface nets (lmjm@doc.imperial.ac.uk)
 * v1.2.1 - 10/20/91 (gf)  - asynch implementation
 * v1.2.0 - 09/17/91 (bpk) - added BULL & USG stuff, thanks to Jim Sillas
 * v1.1.2 - 08/30/91 (bpk) - added VMS support
 * v1.1.1 - 08/29/91 (bcn) - changed backoff handling
 * v1.1.0 - 08/13/91 (gf)  - added XArchie status calls
 *
 * gf: 20 Oct 1991:
 *  Broken into pieces so that under X dirsend() doesn't block in select()
 *  but rather uses Xt calls to allow continued event processing. If
 *  XARCHIE is not defined, can still be used since processEvent() will
 *  use select() in this case.
 */

/* If you're going to hack on this, I'd suggest using unifdef with -UCUTCP
   and possibly -UVMS, for your working copy.  When you've got your changes
   done, come back and add them into this main file.  It's getting pretty
   nasty down there.  */

#include <copyright.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#ifdef VMS
# ifdef WOLLONGONG
#  include "twg$tcp:[netdist.include]netdb.h"
# else /* not Wollongong */
#  ifdef UCX
#   include netdb
#  else /* Multinet */
#   include "multinet_root:[multinet.include]netdb.h"
#  endif
# endif
# include <vms.h>
#else /* not VMS */
# include <sys/types.h> /* this may/will define FD_SET etc */
# ifdef u3b2
#  include <sys/inet.h> /* THIS does FD_SET etc on AT&T 3b2s.  */
# endif /* u3b2 */
# include <pmachine.h>
# if defined(NEED_TIME_H) && !defined(AUX)
#  include <time.h>
# else
#  include <sys/time.h>
# endif
# ifdef NEED_STRING_H
#  include <string.h>
# else
#  include <strings.h>
# endif
# ifdef CUTCP
#  include <msdos/cutcp.h>
#  include <msdos/netevent.h>
#  include <msdos/hostform.h>
# else /* not CUTCP */
#  include <netdb.h>
#  include <sys/socket.h>
# endif
# ifdef NEED_SELECT_H
#  include <sys/select.h>
# endif /* NEED_SELECT_H */
# ifndef IN_H
#  include <netinet/in.h>
#  define IN_H
# endif
# ifndef hpux
#  include <arpa/inet.h>
# endif
#endif /* !VMS */

/* Interactive UNIX keeps some of the socket definitions in funny places.  */
#ifdef ISC
# include <net/errno.h>
#endif /* ISC */

#include <pfs.h>
#include <pprot.h>
#include <pcompat.h>
#include <perrno.h>

/* Gnu C currently fails to pass structures on Sparcs properly.  This directly
   effects the calling of inet_ntoa().  To get around it, we use this hack;
   take the address of what's being called to inet_ntoa, so it gets it
   properly.  This won't be necessary with gcc 2.0.  */
#if defined(sun) && defined(__GNUC__) && !defined(__GNU_LIBRARY__)
# define SUN_GNU_FIX &
#else
# define SUN_GNU_FIX
#endif

static int notprived = 0;
#ifndef MSDOS
extern int errno;
#endif
extern int perrno;
extern int rdgram_priority;
#ifdef DEBUG
extern int pfs_debug;
#endif
extern int pfs_disable_flag;

char	*nlsindex();

#define max(X, Y)  ((X) > (Y) ? (X) : (Y))

static int		dir_udp_port = 0;	/* Remote UDP port number */

#ifdef CUTCP
# define	NS_TIMEOUT	15
#endif

static unsigned short	next_conn_id = 0;

static int client_dirsrv_timeout = CLIENT_DIRSRV_TIMEOUT;
static int client_dirsrv_retry = CLIENT_DIRSRV_RETRY; 

/* These were parameters to dirsend() */
static PTEXT pkt;
static char *hostname;
static struct sockaddr_in *hostaddr;

/* These were locals in dirsend(). Note that the initializations here
 * are really meaningless since we have to redo them for each call to
 * dirsend() since they were formerly automatically initialized.
 */
static PTEXT		first = NULL;	/* First returned packet	 */
static PTEXT		next;		/* The one we are waiting for 	 */
static PTEXT		vtmp;           /* For reorganizing linked list  */
static PTEXT		comp_thru;	/* We have all packets though    */
static int		lp = -1;	/* Opened UDP port	         */
static int		hdr_len;	/* Header Length                 */
static int		nd_pkts;	/* Number of packets we want     */
static int		no_pkts;	/* Number of packets we have     */
static int		pkt_cid;        /* Packet connection identifier  */
static unsigned short	this_conn_id;	/* Connection ID we are using    */
static unsigned short	recvd_thru;	/* Received through              */
static short		priority;	/* Priority for request          */
static short		one = 0;	/* Pointer to value 1            */
static short		zero = 0;	/* Pointer to value 0		 */
static char		*seqtxt;	/* Pointer to text w/ sequence # */
static struct sockaddr_in  us;		/* Our address                   */
static struct sockaddr_in  to;		/* Address to send query	 */
static struct sockaddr_in  from;	/* Reply received from		 */
static int		from_sz;	/* Size of from structure	 */
static struct hostent	*host;		/* Host info from gethostbyname  */
static long		newhostaddr;    /* New host address from *host   */
static int		req_udp_port=0; /* Requested port (optional)     */
static char		*openparen;	/* Delimits port in name         */
static char		hostnoport[500];/* Host name without port        */
static int		ns;		/* Number of bytes actually sent */
static int		nr;		/* Number of bytes received      */
static SELECTARG	readfds;	/* Used for select		 */
static int		tmp;
static char		*ctlptr;	/* Pointer to control field      */
static short		stmp;		/* Temp short for conversions    */
static int		backoff;	/* Server requested backoff      */
static unsigned char	rdflag11;	/* First byte of flags (bit vect)*/
static unsigned char	rdflag12;	/* Second byte of flags (int)    */
static int		scpflag = 0;	/* Set if any sequencd cont pkts */
static int		ackpend = 0;    /* Acknowledgement pending      */
static int		gaps = 0;	/* Gaps present in recvd pkts   */
static struct timeval	timeout;	/* Time to wait for response    */
static struct timeval	ackwait;	/* Time to wait before acking   */
static struct timeval	gapwait;	/* Time to wait b4 filling gaps */
static struct timeval	*selwait;	/* Time to wait for select      */
static int		retries;	/* was = client_dirsrv_retry    */
char   to_hostname[512];		/* lmjm: saves inet_ntoa() str  */

/* These are added so dirsend() "blocks" properly */
static PTEXT dirsendReturn;
static int dirsendDone;

/* And here are the values for dirsendDone */
#define DSRET_DONE		1
#define DSRET_SEND_ERROR	-1
#define DSRET_RECV_ERROR	-2
#define DSRET_SELECT_ERROR	-3
#define DSRET_TIMEOUT		-4
#define DSRET_ABORTED		-5

/* New procedures to break up dirsend() */
static int initDirsend();
static void retryDirsend(), keepWaitingDirsend();
static void timeoutProc();
static void readProc();

/* Wrappers around X calls to allow non-X usage */
static void processEvent();

/* Extra stuff for the asynchronous X version of dirsend() */
typedef char *XtPointer;
typedef char *XtInputId;
typedef char *XtIntervalId;

static XtInputId inputId;
static XtIntervalId timerId = (XtIntervalId)0;

/*
 * dirsend - send packet and receive response
 *
 *   DIRSEND takes a pointer to a structure of type PTEXT, a hostname,
 *   and a pointer to a host address.  It then sends the supplied
 *   packet off to the directory server on the specified host.  If
 *   hostaddr points to a valid address, that address is used.  Otherwise,
 *   the hostname is looked up to obtain the address.  If hostaddr is a
 *   non-null pointer to a 0 address, then the address will be replaced
 *   with that found in the hostname lookup.
 *
 *   DIRSEND will wait for a response and retry an appropriate
 *   number of times as defined by timeout and retries (both static
 *   variables).  It will collect however many packets form the reply, and
 *   return them in a structure (or structures) of type PTEXT.
 *
 *   DIRSEND will free the packet that it is presented as an argument.
 *   The packet is freed even if dirsend fails.
 */

PTEXT dirsend(PTEXT pkt_p,char *hostname_p,struct sockaddr_in *hostaddr_p)
{
    /* copy parameters to globals since other routines use them */
    pkt = pkt_p;
    hostname = hostname_p;
    hostaddr = hostaddr_p;
    /* Do the initializations of formerly auto variables */
    first = NULL;
    lp = -1;
    one = 0;
    zero = 0;
    req_udp_port=0;
    scpflag = 0;
    ackpend = 0;
    gaps = 0;
    retries = client_dirsrv_retry;

    if (initDirsend() < 0)
	return(NULL);

    /* set the first timeout */
    retryDirsend();

    dirsendReturn = NULL;
    dirsendDone = 0;
    /* Until one of the callbacks says to return, keep processing events */
    while (!dirsendDone)
	processEvent();

    /* Return whatever we're supposed to */
    return(dirsendReturn);
}


/*	-	-	-	-	-	-	-	-	*/
/* This function does all the initialization that used to be done at the
 * start of dirsend(), including opening the socket descriptor "lp". It
 * returns the descriptor if successful, otherwise -1 to indicate that
 * dirsend() should return NULL immediately.
 */

static int initDirsend(void)
{
    if(one == 0) one = htons((short) 1);

    priority = htons(rdgram_priority);

    timeout.tv_sec = client_dirsrv_timeout;
    timeout.tv_usec = 0;

    ackwait.tv_sec = 0;
    ackwait.tv_usec = 500000;

    gapwait.tv_sec = (client_dirsrv_timeout < 5 ? client_dirsrv_timeout : 5);
    gapwait.tv_usec = 0;

    comp_thru = NULL;
    perrno = 0;
    nd_pkts = 0;
    no_pkts = 0;
    pkt_cid = 0;

    /* Find first connection ID */
    if(next_conn_id == 0) {
	srand(getpid()+time(0)); /* XXX: arg ok, but not right type. */
	next_conn_id = rand();
    }


    /* If necessary, find out what udp port to send to */
    if (dir_udp_port == 0) {
        register struct servent *sp;
	tmp = pfs_enable; pfs_enable = PMAP_DISABLE;
#ifdef USE_ASSIGNED_PORT
	/* UCX needs 0 & -1 */
        if ((sp = getservbyname("prospero","udp")) <= 0) {
#ifdef DEBUG
	    if (pfs_debug)
		fprintf(stderr, "dirsrv: udp/prospero unknown service - using %d\n", 
			PROSPERO_PORT);
#endif
	    dir_udp_port = htons((u_short) PROSPERO_PORT);
        }
#else
	/* UCX needs 0 & -1 */
        sp = getservbyname("dirsrv","udp");
	if (sp == (struct servent *)0 || sp == (struct servent *)-1) {
#ifdef DEBUG
	    if (pfs_debug)
		fprintf(stderr, "dirsrv: udp/dirsrv unknown service - using %d\n", 
			DIRSRV_PORT);
#endif
	    dir_udp_port = htons((unsigned short) DIRSRV_PORT);
        }
#endif
	else dir_udp_port = sp->s_port;
	pfs_enable = tmp;
#ifdef DEBUG
        if (pfs_debug > 3)
            fprintf(stderr,"dir_udp_port is %d\n", ntohs(dir_udp_port));
#endif
    }

    /* If we were given the host address, then use it.  Otherwise  */
    /* lookup the hostname.  If we were passed a host address of   */
    /* 0, we must lookup the host name, then replace the old value */
    if(!hostaddr || (hostaddr->sin_addr.s_addr == 0)) {
	/* I we have a null host name, return an error */
	if((hostname == NULL) || (*hostname == '\0')) {
#ifdef DEBUG
            if (pfs_debug)
                fprintf(stderr, "dirsrv: Null hostname specified\n");
#endif
	    perrno = DIRSEND_BAD_HOSTNAME;
	    ptlfree(pkt);
            /* return(NULL); */
	    return(-1);
	}
	/* If a port is included, save it away */
	if((openparen = strchr(hostname,(int) '('))) {
	    sscanf(openparen+1,"%d",&req_udp_port);
	    strncpy(hostnoport,hostname,400);
	    if((openparen - hostname) < 400) {
		*(hostnoport + (openparen - hostname)) = '\0';
		hostname = hostnoport;
	    }
	}
	tmp = pfs_enable; pfs_enable = PMAP_DISABLE;
	if((host = gethostbyname(hostname)) == NULL) {
	    pfs_enable = tmp;
	    /* Check if a numeric address */
	    newhostaddr = inet_addr(hostname);
	    if(newhostaddr == -1) {
#ifdef DEBUG
		if (pfs_debug)
		  fprintf(stderr, "dirsrv: Can't resolve host %s\n",hostname);
#endif
		perrno = DIRSEND_BAD_HOSTNAME;
		ptlfree(pkt);
		/* return(NULL); */
		return(-1);
	    }
	    bzero((char *)&to, S_AD_SZ);
	    to.sin_family = AF_INET;
	    bcopy(&newhostaddr,&to.sin_addr, 4);
	    if(hostaddr) bcopy(&to,hostaddr, S_AD_SZ);
	}
	else {
	    pfs_enable = tmp;
	    bzero((char *)&to, S_AD_SZ);
	    to.sin_family = host->h_addrtype;
#ifdef CUTCP
	    bcopy(&host->h_addr, &to.sin_addr, host->h_length);
#else
	    bcopy(host->h_addr, &to.sin_addr, host->h_length);
#endif
	    if(hostaddr) bcopy(&to, hostaddr, S_AD_SZ);
	}
    }
    else bcopy(hostaddr, &to, S_AD_SZ);
    /* lmjm: Save away the hostname */
    strncpy(to_hostname,
	    inet_ntoa(/* SUN_GNU_FIX */ to.sin_addr),
	    sizeof(to_hostname)-1);

    if(req_udp_port) to.sin_port = htons(req_udp_port);
    else to.sin_port = dir_udp_port;

    /* If a port was specified in hostaddr, use it, otherwise fill it in */
    if(hostaddr) {
	if(hostaddr->sin_port) to.sin_port = hostaddr->sin_port;
	else hostaddr->sin_port = to.sin_port;
    }

#ifndef CUTCP
    /* Must open a new port each time. we do not want to see old */
    /* responses to messages we are done with                    */
    if ((lp = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
#ifdef DEBUG
        if (pfs_debug)
            fprintf(stderr,"dirsrv: Can't open socket\n");
#endif
	perrno = DIRSEND_UDP_CANT;
	ptlfree(pkt);
        /* return(NULL); */
	return(-1);
    }
#endif /* not CUTCP */

    /* Try to bind it to a privileged port - loop through candidate */
    /* ports trying to bind.  If failed, that's OK, we will let the */
    /* system assign a non-privileged port later                    */
#ifndef CUTCP
    if(!notprived) {
	for(tmp = PROS_FIRST_PRIVP; tmp < PROS_FIRST_PRIVP+PROS_NUM_PRIVP; 
	    tmp++) {
#endif
	    bzero((char *)&us, sizeof(us));
	    us.sin_family = AF_INET;
#ifndef CUTCP
	    us.sin_port = htons((unsigned short) tmp);
	    if (bind(lp, (struct sockaddr *)&us, sizeof(us))) {
		if(errno != EADDRINUSE) {
		    notprived++;
		    break;
		}
	    }
	    else break;
	}
    }
#else
    us.sin_port = htons(PROS_FIRST_PRIVP);
    netulisten(PROS_FIRST_PRIVP);
#endif

#ifndef USE_V3_PROT
    /* Add header */
    if(rdgram_priority) {
	pkt->start -= 15;
	pkt->length += 15;
	*(pkt->start) = (char) 15;
	bzero(pkt->start+9,4);
	*(pkt->start+11) = 0x02;
	bcopy(&priority,pkt->start+13,2);
    }
    else {
	pkt->start -= 9;
	pkt->length += 9;
	*(pkt->start) = (char) 9;
    }
    this_conn_id = htons(next_conn_id++);
    if(next_conn_id == 0) next_conn_id++;
    bcopy(&this_conn_id,pkt->start+1,2);
    bcopy(&one,pkt->start+3,2);
    bcopy(&one,pkt->start+5,2);
    bzero(pkt->start+7,2);
#endif

#ifdef DEBUG
    if (pfs_debug > 2) {
#ifndef USE_V3_PROT
        if (to.sin_family == AF_INET) {
	    if(req_udp_port) 
		fprintf(stderr,"Sending message to %s+%d(%d)...",
			to_hostname, req_udp_port, ntohs(this_conn_id));
	    else fprintf(stderr,"Sending message to %s(%d)...",
			 to_hostname, ntohs(this_conn_id));
	}
#else
        if (to.sin_family == AF_INET) 
	    fprintf(stderr,"Sending message to %s...", to_hostname);
#endif /* USE_V3_PROT */
        else
            fprintf(stderr,"Sending message...");
        (void) fflush(stderr);
    }
#endif /* DEBUG */

    first = ptalloc();
    next = first;

#ifndef CUTCP
    return(lp);
#else
    return(1);
#endif /* CUTCP */
}

/*	-	-	-	-	-	-	-	-	*/
/*
 * This used to be a label to goto to retry the last packet. Now we resend
 * the packet and call keepWaitingDirsend() to wait for a reply. (We
 * call keepWaitingDirsend() because formerly the code dropped through
 * the keep_waiting label.)
 */

static void retryDirsend(void)
{
#ifdef CUTCP
    int lretry = 3;
#endif
    gaps = ackpend = 0;

#ifndef CUTCP
    ns = sendto(lp,(char *)(pkt->start), pkt->length, 0, (struct sockaddr *)&to, S_AD_SZ);
#else
    while(--lretry) {
	    ns = netusend(&to.sin_addr,ntohs(to.sin_port),ntohs(us.sin_port),
			  (char *) pkt->start, pkt->length);
	    if(!ns)
		break;
	    Stask();
	    Stask();
	    Stask();
    }
#endif /* CUTCP */

#ifndef CUTCP
    if(ns != pkt->length) {
#else
    if(ns != 0) {
#endif
#ifdef DEBUG
	if (pfs_debug) {
    fprintf(stderr,"\nsent only %d/%d: ",ns, pkt->length);
	    perror("");
	}
#endif
	close(lp);
	perrno = DIRSEND_NOT_ALL_SENT;
	ptlfree(first);
	ptlfree(pkt);
	/* return(NULL); */
	dirsendReturn = NULL;
	dirsendDone = DSRET_SEND_ERROR;
    }
#ifdef DEBUG
    if (pfs_debug > 2) fprintf(stderr,"Sent.\n");
#endif
    keepWaitingDirsend();
}

/*	-	-	-	-	-	-	-	-	*/
/*
 * This used to be a label to goto to set the appropriate timeout value
 * and blocked in select(). Now we set selwait and the SELECTARGs to the
 * appropriate values, and in X register a new timeout, then return to
 * allow event processing.
 */

static void keepWaitingDirsend(void)
{
    /* We come back to this point (by a goto) if the packet */
    /* received is only part of the response, or if the     */
    /* response came from the wrong host		    */

#ifdef DEBUG
    if (pfs_debug > 2) fprintf(stderr,"Waiting for reply...");
#endif

#ifndef CUTCP
    FD_ZERO(&readfds);
    FD_SET(lp, &readfds);
#endif

    if(ackpend) selwait = &ackwait;
    else if(gaps) selwait = &gapwait;
    else selwait = &timeout;
}

/*	-	-	-	-	-	-	-	-	*/
/*
 * This routine is called when a timeout occurs. It includes the code that
 * was formerly used when select() returned 0 (indicating a timeout).
 */
/*ARGSUSED*/

static void timeoutProc(XtPointer client_data,XtIntervalId *id)
{
    if (gaps || ackpend) { /* Send acknowledgment */
	/* Acks are piggybacked on retries - If we have received */
	/* an ack from the server, then the packet sent is only  */
	/* an ack and the rest of the message will be empty      */
#ifdef DEBUG
	if (pfs_debug > 2) {
            fprintf(stderr,"Acknowledging (%s).\n",
		    (ackpend ? "requested" : "gaps"));
	}	
#endif
	retryDirsend();
	return;
    }

    if (retries-- > 0) {
	timeout.tv_sec = CLIENT_DIRSRV_BACKOFF(timeout.tv_sec);
#ifdef DEBUG
	if (pfs_debug > 2) {
            fprintf(stderr,"Timed out.  Setting timeout to %d seconds.\n",
		    (int) timeout.tv_sec);
	}
#endif
	retryDirsend();
	return;
    }

#ifdef DEBUG
    if (pfs_debug) {
	fprintf(stderr, "select failed(timeoutProc): readfds=%x ",
	        readfds);
	perror("");
    }
#endif
#ifndef CUTCP
    close(lp);
#endif
    perrno = DIRSEND_SELECT_FAILED;
    ptlfree(first);
    ptlfree(pkt);
    /* return(NULL); */
    dirsendReturn = NULL;
    dirsendDone = DSRET_TIMEOUT;
}

/*	-	-	-	-	-	-	-	-	*/
/*
 * This function is called whenever there's something to read on the
 * connection. It includes the code that was run when select() returned
 * greater than 0 (indicating read ready).
 */
/*ARGSUSED*/

static void readProc(XtPointer client_data,int *source,XtInputId *id)
{
#ifdef CUTCP
    int lretry = 3;
#endif

    from_sz = sizeof(from);
    next->start = next->dat;

#ifndef CUTCP
    if ((nr = recvfrom(lp, next->start, sizeof(next->dat), 0, (struct sockaddr *)&from, &from_sz)) < 0) {
#else
    nr = neturead(next->start);
    if (nr < 1) {
#endif
#ifdef DEBUG
        if (pfs_debug) perror("recvfrom");
#endif
#ifndef CUTCP
	close(lp);
#endif
	perrno = DIRSEND_BAD_RECV;
	ptlfree(first);
	ptlfree(pkt);
	/* return(NULL) */
	dirsendReturn = NULL;
	dirsendDone = DSRET_RECV_ERROR;
        return;
    }

    next->length = nr;
    next->start[next->length] = 0;

#ifdef DEBUG
    if (pfs_debug > 2)
        fprintf(stderr,"Received packet from %s\n",
		/*
		inet_ntoa(SUN_GNU_FIX *(struct in_addr *)&from.sin_addr));
		*/
		inet_ntoa(from.sin_addr));
#endif


    /* For the current format, if the first byte is less than             */
    /* 20, then the first two bits are a version number and the next six  */
    /* are the header length (including the first byte).                  */
    if((hdr_len = (unsigned char) *(next->start)) < 20) {
	ctlptr = next->start + 1;
	next->seq = 0;
	if(hdr_len >= 3) { 	/* Connection ID */
	    bcopy(ctlptr,(char *) &stmp,2);
	    if(stmp) pkt_cid = ntohs(stmp);
	    ctlptr += 2;
	}
	if(pkt_cid && this_conn_id && (pkt_cid != this_conn_id)) {
	    /* The packet is not for us */
	    /* goto keep_waiting; */
	    keepWaitingDirsend();
	    return;
	}
	if(hdr_len >= 5) {	/* Packet number */
	    bcopy(ctlptr,&stmp,2);
	    next->seq = ntohs(stmp);
	    ctlptr += 2;
	}
	else { /* No packet number specified, so this is the only one */
	    next->seq = 1;
	    nd_pkts = 1;
	}
	if(hdr_len >= 7) {	    /* Total number of packets */
	    bcopy(ctlptr,&stmp,2);  /* 0 means don't know      */
	    if(stmp) nd_pkts = ntohs(stmp);
	    ctlptr += 2;
	}
	if(hdr_len >= 9) {	/* Receievd through */
	    bcopy(ctlptr,&stmp,2);  /* 1 means received request */
#ifndef USE_V3_PROT
	    if((stmp) && (ntohs(stmp) == 1)) {
		/* Future retries will be acks only */
		pkt->length = 9;
		bcopy(&zero,pkt->start+3,2);
#ifdef DEBUG
		if(pfs_debug > 2) 
		    fprintf(stderr,"Server acked request - retries will be acks only\n");
#endif
	    }
#endif
	    ctlptr += 2;
	}
	if(hdr_len >= 11) {	/* Backoff */
	    bcopy(ctlptr,&stmp,2);
	    if(stmp) {
		backoff = ntohs(stmp);
#ifdef DEBUG
		if(pfs_debug > 2) 
		    fprintf(stderr,"Backing off to %d seconds\n", backoff);
#endif
		timeout.tv_sec = backoff;
		if ((backoff > 60) && (first == next) && (no_pkts == 0)) {
		    /* Probably a long queue on the server - don't give up */
		    retries = client_dirsrv_retry;
		}
	    }
	    ctlptr += 2;
	}
	if(hdr_len >= 12) {	/* Flags (1st byte) */
	    bcopy(ctlptr,&rdflag11,1);
	    if(rdflag11 & 0x80) {
#ifdef DEBUG
		if(pfs_debug > 2) 
		    fprintf(stderr,"Ack requested\n");
#endif
		ackpend++;
	    }
	    if(rdflag11 & 0x40) {
#ifdef DEBUG
		if(pfs_debug > 2) 
		    fprintf(stderr,"Sequenced control packet\n");
#endif
		next->length = -1;
		scpflag++;
	    }
	    ctlptr += 1;
	}
	if(hdr_len >= 13) {	/* Flags (2nd byte) */
	    /* Reserved for future use */
	    bcopy(ctlptr,&rdflag12,1);
	    ctlptr += 1;
	}
	if(next->seq == 0) {
	    /* goto keep_waiting; */
	    keepWaitingDirsend();
	    return;
	}
	if(next->length >= 0) next->length -= hdr_len;
	next->start += hdr_len;
	goto done_old;
    }

    pkt_cid = 0;

    /* if intermediate format (between old and new), then process */
    /* and go to done_old                                         */
    ctlptr = next->start + max(0,next->length-20);
    while(*ctlptr) ctlptr++;
    /* Control fields start after the terminating null */
    ctlptr++;
    /* Until old version are gone, must be 4 extra bytes minimum */
    /* When no version 3 servers, can remove the -4              */
    if(ctlptr < (next->start + next->length - 4)) {
	/* Connection ID */
	bcopy(ctlptr,&stmp,2);
	if(stmp) pkt_cid = ntohs(stmp);
	ctlptr += 2;
	if(pkt_cid && this_conn_id && (pkt_cid != this_conn_id)) {
	    /* The packet is not for us */
	    /* goto keep_waiting; */
	    keepWaitingDirsend();
	    return;
	}
	/* Packet number */
	if(ctlptr < (next->start + next->length)) {
	    bcopy(ctlptr,&stmp,2);
	    next->seq = ntohs(stmp);
	    ctlptr += 2;
	}
	/* Total number of packets */
	if(ctlptr < (next->start + next->length)) {
	    bcopy(ctlptr,&stmp,2);
	    if(stmp) nd_pkts = ntohs(stmp);
	    ctlptr += 2;
	}
	/* Receievd through */
	if(ctlptr < (next->start + next->length)) {
	    /* Not supported by clients */
	    ctlptr += 2;
	}
	/* Backoff */
	if(ctlptr < (next->start + next->length)) {
	    bcopy(ctlptr,&stmp,2);
	    backoff = ntohs(stmp);
#ifdef DEBUG
	    if(pfs_debug > 2) 
		fprintf(stderr,"Backing off to %d seconds\n", backoff);
#endif
	    if(backoff) timeout.tv_sec = backoff;
	    ctlptr += 2;
	}
	if(next->seq == 0) {
	    /* goto keep_waiting; */
	    keepWaitingDirsend();
	    return;
	}
	goto done_old;

    }

    /* Notes that we have to start searching 11 bytes before the    */
    /* expected start of the MULTI-PACKET line because the message  */
    /* might include up to 10 bytes of data after the trailing null */
    /* The order of those bytes is two bytes each for Connection ID */
    /* Packet-no, of, Received-through, Backoff                     */
    seqtxt = nlsindex(next->start + max(0,next->length - 40),"MULTI-PACKET"); 
    if(seqtxt) seqtxt+= 13;

    if((nd_pkts == 0) && (no_pkts == 0) && (seqtxt == NULL)) goto all_done;

    tmp = sscanf(seqtxt,"%d OF %d", &(next->seq), &nd_pkts);
#ifdef DEBUG    
    if (pfs_debug && (tmp == 0)) 
	fprintf(stderr,"Cant read packet sequence number: %s", seqtxt);    
#endif
 done_old:
#ifdef DEBUG
    if(pfs_debug > 2) fprintf(stderr,"Packet %d of %d\n",next->seq,nd_pkts);
#endif
    if ((first == next) && (no_pkts == 0)) {
	if(first->seq == 1) {
	    comp_thru = first;
	    /* If only one packet, then return it */
	    if(nd_pkts == 1) goto all_done;
	}
	else gaps++;
	no_pkts = 1;
	next = ptalloc();
	/* goto keep_waiting; */
	keepWaitingDirsend();
	return;
    }
	
    if(comp_thru && (next->seq <= comp_thru->seq))
	ptfree(next);
    else if (next->seq < first->seq) {
	vtmp = first;
	first = next;
	first->next = vtmp;
	first->previous = NULL;
	vtmp->previous = first;
	if(first->seq == 1) comp_thru = first;
	no_pkts++;
    }
    else {
	vtmp = (comp_thru ? comp_thru : first);
	while (vtmp->seq < next->seq) {
	    if(vtmp->next == NULL) {
		vtmp->next = next;
		next->previous = vtmp;
		next->next = NULL;
		no_pkts++;
		goto ins_done;
	    }
	    vtmp = vtmp->next;
	}
	if(vtmp->seq == next->seq)
	    ptfree(next);
	else {
	    vtmp->previous->next = next;
	    next->previous = vtmp->previous;
	    next->next = vtmp;
	    vtmp->previous = next;
	    no_pkts++;
	}
    }   

ins_done:
	
    while(comp_thru && comp_thru->next && 
	  (comp_thru->next->seq == (comp_thru->seq + 1))) {
	comp_thru = comp_thru->next;
#ifndef USE_V3_PROT
	recvd_thru = htons(comp_thru->seq);
	bcopy(&recvd_thru,pkt->start+7,2); /* Let server know we got it */
#endif
	/* We've made progress, so reset retry count */
	retries = client_dirsrv_retry;
	/* Also, next retry will be only an acknowledgement */
	/* but for now, we can't fill in the ack field      */
#ifdef DEBUG
	if(pfs_debug > 2) 
	    fprintf(stderr,"Packets now received through %d\n",comp_thru->seq);
#endif
    }

    /* See if there are any gaps */
    if(!comp_thru || comp_thru->next) gaps++;
    else gaps = 0;

    if ((nd_pkts == 0) || (no_pkts < nd_pkts)) {
	next = ptalloc();
	/* goto keep_waiting; */
	keepWaitingDirsend();
	return;
    }

 all_done:
    if(ackpend) { /* Send acknowledgement if requested */
#ifdef DEBUG
	if (pfs_debug > 2) {
	    if (to.sin_family == AF_INET)
		fprintf(stderr,"Acknowledging final packet to %s(%d)\n",
			to_hostname, ntohs(this_conn_id));
            else
                fprintf(stderr,"Acknowledging final packet\n");
	    (void) fflush(stderr);
	}
#endif
#ifndef CUTCP
	ns = sendto(lp,(char *)(pkt->start), pkt->length, 0, (struct sockaddr *)&to, S_AD_SZ);
#else
	while(--lretry) {
	    ns = netusend(&to.sin_addr, ntohs(to.sin_port), ntohs(us.sin_port),(char *) pkt->start, pkt->length);
		if(!ns)
			break;
		Stask();
		Stask();
	}
#endif

#ifndef CUTCP
	if(ns != pkt->length) {
#else
	if(ns != 0) {
#endif

#ifdef DEBUG
	    if (pfs_debug) {
		fprintf(stderr,"\nsent only %d/%d: ",ns, pkt->length);
		perror("");
	    }
#endif
	}

    }
#ifndef CUTCP
    close(lp);
#endif
    ptlfree(pkt);

    /* Get rid of any sequenced control packets */
    if(scpflag) {
	while(first && (first->length < 0)) {
	    vtmp = first;
	    first = first->next;
	    if(first) first->previous = NULL;
	    ptfree(vtmp);
	}
	vtmp = first;
	while(vtmp && vtmp->next) {
	    if(vtmp->next->length < 0) {
		if(vtmp->next->next) {
		    vtmp->next = vtmp->next->next;
		    ptfree(vtmp->next->previous);
		    vtmp->next->previous = vtmp;
		}
		else {
		    ptfree(vtmp->next);
		    vtmp->next = NULL;
		}
	    }
	    vtmp = vtmp->next;
	}
    }

    /* return(first); */
    dirsendReturn = first;
    dirsendDone = DSRET_DONE;

}

static void processEvent(void)
{
#ifdef CUTCP
    unsigned long now;
#endif
    /* select - either recv is ready, or timeout */
    /* see if timeout or error or wrong descriptor */
#ifndef CUTCP
    tmp = select(lp + 1, &readfds, (SELECTARG *)0, (SELECTARG *)0, selwait);
    if (tmp == 0) {
	timeoutProc(NULL,&timerId);
    } else if ((tmp < 0) || !FD_ISSET(lp,&readfds)) {
#ifdef DEBUG
	if (pfs_debug) {
	    fprintf(stderr, "select failed(processEvent): readfds=%x ",
		    readfds);
	    perror("");
	}
#endif
	close(lp);
#else /* CUTCP's flood. */
	/* while not timeout in selwait loop, stask looking for uevents */
	now = time(NULL) + selwait->tv_sec;
#ifdef	DEBUG
 	if(pfs_debug) {
		fprintf(stderr,"Waiting %d seconds\n",selwait->tv_sec);
	}

#endif
	while(now > time(NULL)) {
		int	i, cl, dat;

		Stask();
		if (0 < (i = Sgetevent(USERCLASS, &cl, &dat))) {
			/* got a user class event */
			if(cl == USERCLASS &&
				i == UDPDATA) {
					readProc(NULL,&lp,&inputId);
					return;
			}
		}
		if(kbhit()) {
			int c = getch();
			if(c == 27 || c == 3)
				break;
			fprintf(stderr,"Press <ESCAPE> to abort\n");
		}
	}
	if(now <= time(NULL)) {	/* timeout */
		timeoutProc(NULL,&timerId);
		 return;
	}

#endif /* CUTCP */
	perrno = DIRSEND_SELECT_FAILED;
	ptlfree(first);
	ptlfree(pkt);
	/* return(NULL); */
	dirsendReturn = NULL;
	dirsendDone = DSRET_SELECT_ERROR;
#ifndef CUTCP
    } else {
	readProc(NULL,&lp,&inputId);
    }
#endif /* CUTCP */
}

void abortDirsend(void)
{
    if (!dirsendDone) {
#ifndef CUTCP
	close(lp);
#endif
	ptlfree(first);
	ptlfree(pkt);
	dirsendReturn = NULL;
	dirsendDone = DSRET_ABORTED;
    }
    return;
}
