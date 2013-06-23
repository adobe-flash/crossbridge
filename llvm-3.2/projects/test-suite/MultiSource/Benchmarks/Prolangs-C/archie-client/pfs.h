/*
 * Copyright (c) 1989, 1990, 1991 by the University of Washington
 *
 * For copying and distribution information, please see the file
 * <copyright.h>.
 *
 * v1.2.0 - 11/19/91 (mmt) - added MSDOS & OS2 stuff
 * v1.1.2 - 08/30/91 (bpk) - added VMS support
 * v1.1.1 - 08/20/91 (bpk) - 4.2 -> 4.2B
 */

#include <copyright.h>

#ifdef VMS
# include <vms.h>
#else /* not VMS */
# ifndef _TYPES_
#  include <sys/types.h>
# endif /* _TYPES_ */
# ifndef IN_H
#  include <netinet/in.h> 
#  define IN_H
# endif
#endif /* VMS */

#ifndef NULL
# ifdef MSDOS
#  include <stdio.h>
# else
#  define NULL 0
# endif /* MSDOS */
#endif /* NULL */

#define		PFS_RELEASE	"Beta.4.2D"
/* moved up for vdir_init */
#define ZERO(p)		bzero((char *)(p), sizeof(*(p)))

/* General Definitions */

#define		MAX_PTXT_LEN	1250     /* Max length of PTEXT structure   */
#define		MAX_PTXT_HDR	32       /* Max offset for start            */
#define		P_ERR_STRING_SZ 100	 /* Size of error string	    */
#define		MAX_VPATH	1024	 /* Max length of virtual pathname  */

/* Definition of text structure used to pass text around */

struct ptext {
    int			length;		  /* Length of text (from start)    */
    char		*start;		  /* Start of text		    */
    char		dat[MAX_PTXT_LEN+2*MAX_PTXT_HDR];/* The data itself */
    unsigned long 	mbz;		  /* ZERO to catch runaway strings  */
    struct ptext	*previous;        /* Previous element in list       */
    struct ptext	*next;		  /* Next element in linked list    */
    int			seq;		  /* Sequence Number		    */
};

typedef struct ptext *PTEXT;
typedef struct ptext PTEXT_ST;

/* Request structure: maintains information about server requests */
struct preq {
    int			cid;		  /* Connection ID                  */
    short		priority;	  /* Connection priority            */
    int			pf_priority;	  /* Priority assigned by pri_func  */
    int			recv_tot;	  /* Total # of packets received    */
    int			trns_tot;	  /* Total # of packets to transmit */
    struct ptext	*cpkt;		  /* Current packet being filled in */
    struct ptext	*recv;		  /* Received packets               */
    struct ptext	*trns;		  /* Transmitted packets            */
    int			rcvd_thru;	  /* Received all packets through # */
    struct preq		*previous;        /* Previous element in list       */
    struct preq		*next;		  /* Next element in linked list    */
    struct sockaddr_in	fromto; 	  /* Sender/Destination		    */
};

typedef struct preq *PREQ;
typedef struct preq PREQ_ST;


/* Definition of structure containing information on virtual link */

struct vlink {
    int			dontfree;	  /* Flag: don't free this link     */
    char		*name;		  /* Component of path name	    */
    char		linktype;	  /* L = Link, U = Union, N= Native */
    int			expanded;	  /* Has a union link been expanded */
    char		*type;            /* Type of object pointed to      */
    struct vlink	*filters;	  /* Filters associated with link   */
    struct vlink	*replicas;	  /* Replicas (* see comment below) */
    char		*hosttype;	  /* Type of hostname		    */
    char		*host;		  /* Files physical location	    */
    char		*nametype;	  /* Type of filename		    */
    char		*filename;	  /* System level filename	    */
    long		version;	  /* Version number of destination  */
    long		f_magic_no;	  /* File's magic number	    */
    struct acl		*acl;		  /* ACL for link		    */
    long		dest_exp;	  /* Expiration for dest of link    */
    long		link_exp;	  /* Expiration of link itself      */
    char		*args;		  /* Arguments if this is a filter  */
    struct pattrib	*lattrib;	  /* Attributes associated w/ link  */
    struct pfile	*f_info;	  /* Info to be assoicated w/ file  */
    struct vlink	*previous;        /* Previous elt in linked list    */
    struct vlink	*next;		  /* Next element in linked list    */
};

typedef struct vlink *VLINK;
typedef struct vlink VLINK_ST;

/* * Note that vlink->replicas is not really a list of replicas of the  */
/*   object.  Instead, it is a list of the objects returned during name */
/*   resolution that share the same name as the current object.  Such   */
/*   an object should only be considered a replica if it also shares    */
/*   the same non-zero magic number.                                    */

/* Definition of structure continiaing virtual directory information    */

struct vdir {
    int			version;	  /* Version of local directory fmt  */
    int			inc_native;	  /* Include the native directory    */
    long		magic_no;	  /* Magic number of current file    */
    struct acl		*dacl;            /* Default acl for links in dir    */
    struct pfile	*f_info;	  /* Directory file info             */
    struct vlink	*links;		  /* The directory entries	     */
    struct vlink	*lastlink;	  /* Last directory entry            */
    struct vlink	*ulinks;	  /* The entries for union links     */
    struct vdir		*previous;        /* Previous element in linked list */
    struct vdir		*next;		  /* Next element in linked list     */
};

typedef struct vdir *VDIR1;
typedef struct vdir VDIR_ST;

/* Initialize directory */
#define vdir_init(dir)  ZERO(dir)
/* XXX: was

  dir->version = 0;     dir->inc_native = 0; \
  dir->magic_no = 0;    dir->f_info = NULL; \
  dir->links = NULL;    dir->lastlink = NULL; \
  dir->ulinks = NULL;   dir->dacl = NULL; \
  dir->previous = NULL; dir->next = NULL;
*/

#define vdir_copy(d1,d2) d2->version = d1->version; \
                         d2->inc_native = d1->inc_native; \
                         d2->magic_no = d1->magic_no; \
    			 d2->f_info = d1->f_info; \
                         d2->links = d1->links; \
                         d2->lastlink = d1->lastlink; \
                         d2->ulinks = d1->ulinks; \
                         d2->dacl = d1->dacl; \
                         d2->previous = d1->previous; \
                         d2->next = d1->next; 
                         
/* Values of ->inc_native in vdir structure */
#define VDIN_REALONLY	-1   /* Include native files, but not . and ..       */
#define VDIN_NONATIVE	 0   /* Do not include files from native directory   */
#define VDIN_INCLNATIVE	 1   /* Include files from native directory          */
#define VDIN_NATIVEONLY  2   /* All entries in directory are from native dir */
#define VDIN_PSEUDO      3   /* Directory is not real                        */


/* Definition of structure containing information on a specific file */

union avalue {
    char		*ascii;		/* Character string                */
    struct vlink	*link;		/* A link			   */
};


struct pattrib {
    char		precedence;	/* Precedence for link attribute   */
    char		*aname;		/* Name of the attribute           */
    char		*avtype;	/* Type of the attribute value     */
    union avalue	value;		/* Attribute Value                 */
    struct pattrib	*previous;      /* Previous element in linked list */
    struct pattrib	*next;		/* Next element in linked list     */
};

typedef struct pattrib *PATTRIB;
typedef struct pattrib PATTRIB_ST;

#define 	ATR_PREC_OBJECT  'O'   /* Authoritative answer for object */
#define 	ATR_PREC_LINK    'L'   /* Authoritative answer for link   */
#define 	ATR_PREC_CACHED  'C'   /* Object info cached w/ link      */
#define 	ATR_PREC_REPLACE 'R'   /* From link (replaces O)          */
#define 	ATR_PREC_ADD     'A'   /* From link (additional value)    */

/* **** Incomplete **** */
struct pfile {
    int			version;	  /* Version of local finfo format   */
    long		f_magic_no;	  /* Magic number of current file    */
    long		exp;		  /* Expiration date of timeout      */
    long		ttl;		  /* Time to live after reference    */
    long		last_ref;	  /* Time of last reference          */
    struct vlink	*forward;	  /* List of forwarding pointers     */
    struct vlink	*backlinks;	  /* Partial list of back links      */
    struct pattrib	*attributes;	  /* List of file attributes         */
    struct pfile	*previous;        /* Previous element in linked list */
    struct pfile	*next;		  /* Next element in linked list     */
};

typedef struct pfile *PFILE;
typedef struct pfile PFILE_ST;

/* Definition of structure contining an access control list entry */

struct acl {
    int			acetype;	  /* Access Contol Entry type       */
    char		*atype;           /* Authentication type            */
    char		*rights;          /* Rights                         */
    char		*principals;      /* Authorized principals          */
    struct restrict_    *restrictions;    /* Restrictions on use            */
    struct acl		*previous;        /* Previous elt in linked list    */
    struct acl		*next;		  /* Next element in linked list    */
};
typedef struct acl *ACL;
typedef struct acl ACL_ST;

#define ACL_NONE		0         /* Nobody authorized by ths entry */
#define ACL_DEFAULT		1         /* System default                 */
#define ACL_SYSTEM		2         /* System administrator           */
#define ACL_OWNER               3         /* Directory owner                */
#define ACL_DIRECTORY           4         /* Same as directory              */
#define ACL_ANY                 5         /* Any user                       */
#define ACL_AUTHENT             6         /* Authenticated principal        */
#define ACL_LGROUP              7         /* Local group                    */
#define ACL_GROUP               8         /* External group                 */
#define ACL_ASRTHOST            10        /* Check host and asserted userid */
#define ACL_TRSTHOST            11        /* ASRTHOST from privileged port  */


/* Definition of structure contining access restrictions */
/* for future extensions                                 */
struct restrict_ {
    struct acl		*previous;        /* Previous elt in linked list    */
    struct acl		*next;		  /* Next element in linked list    */
};

/* Definitions for send_to_dirsrv */
#define	CLIENT_DIRSRV_TIMEOUT		4	/* time between retries      */
#define CLIENT_DIRSRV_BACKOFF(x)  (2 * x)	/* Backoff algorithm         */
#define CLIENT_DIRSRV_RETRY		3	/* retry this many times     */

/* Definitions for rd_vlink and rd_vdir */
#define		SYMLINK_NESTING 10       /* Max nesting depth for sym links */

/* Definition fo check_acl */
#define		ACL_NESTING     10       /* Max depth for ACL group nesting */

/* Flags for mk_vdir */
#define	     MKVD_LPRIV     1   /* Minimize privs for creator in new ACL    */

/* Flags for get_vdir */
#define	     GVD_UNION      0	/* Do not expand union links 		     */
#define      GVD_EXPAND     1   /* Expand union links locally		     */
#define	     GVD_LREMEXP    3   /* Request remote expansion of local links   */
#define	     GVD_REMEXP     7   /* Request remote expansion of all links     */
#define	     GVD_VERIFY     8	/* Only verify args are for a directory      */
#define      GVD_FIND	   16   /* Stop expanding when match is found        */
#define	     GVD_ATTRIB    32   /* Request attributes from remote server     */
#define	     GVD_NOSORT	   64   /* Do not sort links when adding to dir      */

/* Flags for rd_vdir */
#define	     RVD_UNION      GVD_UNION
#define	     RVD_EXPAND     GVD_EXPAND 
#define	     RVD_LREMEXP    GVD_LREMEXP
#define	     RVD_REMEXP     GVD_REMEXP
#define	     RVD_DFILE_ONLY GVD_VERIFY /* Only return ptr to dir file        */
#define      RVD_FIND       GVD_FIND   
#define      RVD_ATTRIB     GVD_ATTRIB
#define	     RVD_NOSORT	    GVD_NOSORT
#define	     RVD_NOCACHE    128

/* Flags for add_vlink */
#define	     AVL_UNION      1   /* Link is a union link                      */

/* Flags for vl_insert */
#define	     VLI_NOCONFLICT 0   /* Do not insert links w/ conflicting names  */
#define      VLI_ALLOW_CONF 1   /* Allow links with conflicting names        */
#define	     VLI_NOSORT     2   /* Allow conflicts and don't sort            */

/* Flags for mapname */
#define      MAP_READWRITE  0   /* Named file to be read and written         */
#define	     MAP_READONLY   1   /* Named file to be read only                */

/* Flags for modify_acl */
#define	     MACL_NOSYSTEM   0x01
#define      MACL_NOSELF     0x02
#define      MACL_DEFAULT    0x08
#define      MACL_SET        0x0C
#define      MACL_INSERT     0x14
#define      MACL_DELETE     0x10
#define      MACL_ADD        0x1C
#define      MACL_SUBTRACT   0x18
#define      MACL_LINK       0x00
#define      MACL_DIRECTORY  0x20
#define      MACL_OBJECT     0x60
#define      MACL_INCLUDE    0x40

#define      MACL_OP    (MACL_DEFAULT|MACL_SET|MACL_INSERT|\
			 MACL_DELETE|MACL_ADD|MACL_SUBTRACT)

#define      MACL_OTYPE (MACL_LINK|MACL_DIRECTORY|MACL_OBJECT|MACL_INCLUDE)

/* Access methods returned by Pget_am */
#define P_AM_ERROR			0
#define P_AM_FTP			1
#define P_AM_AFTP			2  /* Anonymous FTP  */
#define P_AM_NFS			4
#define P_AM_KNFS			8  /* Kerberized NFS */
#define P_AM_AFS		       16

/* Return codes */

#define		PSUCCESS	0
#define		PFAILURE	255

/* Hush up warnings.  */
void vllfree();

/* Procedures in libpfs.a */

char *pget_wdhost(), *pget_wdfile(), *pget_wd(), *pget_hdhost();
char *pget_hdfile(), *pget_hd(), *pget_rdhost(), *pget_rdfile();
char *pget_dhost(), *pget_dfile(), *pget_vsname(), *nlsindex();
char *sindex(), *strtok(), *nxtline(), *unquote(), *stcopy();
char *stcopyr(), *readheader(), *month_sname();

long		asntotime();
void		procquery(char *host,char *str,int max_hits,int offset,
                      char query_type,int sortflag,int listflag);

PTEXT		ptalloc();
PTEXT		dirsend();
void		ptfree();
void		ptlfree();

PREQ		pralloc();
PREQ		get_next_request();

VLINK		rd_slink();
VLINK		rd_vlink();
VLINK		vl_delete();
VLINK		vlalloc();
VLINK		vlcopy();
void		vlfree();

PFILE		pfalloc();

PATTRIB         parse_attribute();
PATTRIB         atalloc();
PATTRIB 	pget_at();
void		atfree();
void		atlfree();

ACL             acalloc();
ACL             get_acl();

void		stfree();

/* Miscellaneous useful definitions */
#ifndef TRUE
#define TRUE		1
#define FALSE		0
#endif

#define AUTHORIZED      1
#define NOT_AUTHORIZED  0
#define NEG_AUTHORIZED  -1

#ifndef NULL
#define NULL		0
#endif

#define FAILED		-1
