/*
 * Copyright (c) 1989, 1990, 1991 by the University of Washington
 *
 * For copying and distribution information, please see the file
 * <copyright.h>.
 *
 * v1.2.0 - 11/19/91 (mmt) - added MSDOS & OS2 stuff
 */

#include <copyright.h>

#ifndef MAXPATHLEN
# ifdef VMS
#  define MAXPATHLEN 32
# else /* not VMS */
#  if defined(MSDOS) && !defined(OS2)
#   define MAXPATHLEN 255
#  else /* not MSDOS */
#   ifdef ISC
#    define MAXPATHLEN 512
#   else /* not Interactive..normal! (gasp) */
#    include <sys/param.h>
#   endif /* ISC */
#  endif /* MSDOS && !OS2 */
# endif /* VMS */
#endif

/* Protocol Definitions */

#define	       VFPROT_VNO	1      /* Protocol Version Number           */

#define	       DIRSRV_PORT      1525   /* Server port used if not in srvtab */
#define        PROSPERO_PORT	191    /* Officially assigned prived port   */
#define	       PROS_FIRST_PRIVP 901    /* First privileged port to try      */
#define        PROS_NUM_PRIVP   20     /* Number of privileged ports to try */

#define	       MAXPKT	        1024   /* Max size of response from server  */
#define	       SEQ_SIZE		32     /* Max size of sequence text in resp */ 
#define	       MAX_DIR_LINESIZE 160+MAXPATHLEN /* Max linesize in directory */

#define	       MAX_FWD_DEPTH    20     /* Max fwd pointers to follow        */

#define S_AD_SZ		sizeof(struct sockaddr_in)

/* Replacement for strtok that doesn't keep state.  Both the variable  */
/* S and the variable S_next must be defined.  To initialize, assign   */
/* the string to be stepped through to S_next, then call get_token on  */
/* S.  The first token will be in S, and S_next will point to the next */
/* token.  Like strtok, this macro does modify the string passed to it */
#define get_token(S,C) \
    S = S/**/_next; \
    if(S) { \
     while(*S == C) S++; \
     S/**/_next = index(S,C); \
     if(S/**/_next) *(S/**/_next++) = '\0'; \
     if(!*S) S = NULL; \
    }


