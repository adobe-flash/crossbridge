/*
 * Copyright (c) 1989, 1990 by the University of Washington
 *
 * For copying and distribution information, please see the file
 * <copyright.h>.
 */

#include <copyright.h>

/* this file and p_err_text.c should be updated simultaneously */

/*
 * perrno.h - definitions for perrno
 *
 * This file contains the declarations and defintions of of the external
 * error values in which errors are returned by the pfs and psrv
 * libraries.
 */

#define		P_ERR_STRING_SZ 100	 /* Size of error string	    */

extern int	perrno;
extern char	p_err_string[];
extern char	*p_err_text[];

extern int	pwarn;
extern char	p_warn_string[];
extern char	*p_warn_text[];

/* Error codes returned or found in verrno */

#ifndef PSUCCESS
#define	PSUCCESS		0
#endif

/* dirsend (perrno) */
#define DIRSEND_PORT_UNKN	1	/* DIRSRV UDP port unknown      */
#define DIRSEND_UDP_CANT	2	/* Can't open local UDP port    */
#define DIRSEND_BAD_HOSTNAME	3	/* Can't resolve hostname       */
#define DIRSEND_NOT_ALL_SENT	4	/* Didn't send entire message   */
#define DIRSEND_SELECT_FAILED	5	/* Select failed	        */
#define DIRSEND_BAD_RECV	6	/* Recvfrom failed 	        */

/* reply */
#define REPLY_NOTSENT		11	/* Reply: sendto failed	        */

/* vl_insert */
#define VL_INSERT_ALREADY_THERE	21	/* Link already exists	        */
#define VL_INSERT_CONFLICT	22	/* Link exists with same name   */

/* ul_insert */
#define UL_INSERT_ALREADY_THERE 25	/* Link already exists		*/
#define UL_INSERT_SUPERSEDING   26	/* Replacing existing link	*/
#define UL_INSERT_POS_NOTFOUND  27	/* Prv entry not in dir->ulinks */

/* rd_vdir */
#define RVD_DIR_NOT_THERE	41	/* Temporary NOT_FOUND		    */
#define RVD_NO_CLOSED_NS	42	/* Namespace not closed w/ object:: */
#define RVD_NO_NS_ALIAS		43	/* No alias for namespace NS#:      */
#define RVD_NS_NOT_FOUND	44	/* Specified namespace not found    */

/* pfs_access */
#define PFSA_AM_NOT_SUPPORTED   51      /* Access method not supported  */

/* pmap_cache */
#define PMC_DELETE_ON_CLOSE     55	/* Delete cached copy on close   */
#define PMC_RETRIEVE_FAILED     56      /* Unable to retrieve file       */

/* mk_vdir */
/* #define MKVD_ALREADY_EXISTS     61	   Directory already exists      */
/* #define MKVD_NAME_CONFLICT	62	   Link with name already exists */

/* vfsetenv */
#define VFSN_NOT_A_VS		65	/* Not a virtual system          */
#define VFSN_CANT_FIND_DIR	66	/* Not a virtual system          */

/* add_vlink */
/* #define ADDVL_ALREADY_EXISTS    71	   Directory already exists      */
/* #define ADDVL_NAME_CONFLICT	72	   Link with name already exists */

/* Local error codes on server */

/* dsrdir */
#define DSRDIR_NOT_A_DIRECTORY 111	/* Not a directory name		*/
/* dsrfinfo */
#define DSRFINFO_NOT_A_FILE    121      /* Object not found             */
#define DSRFINFO_FORWARDED     122      /* Object has moved             */

/* Error codes that may be returned by various procedures               */
#define PFS_FILE_NOT_FOUND     230      /* File not found               */
#define PFS_DIR_NOT_FOUND      231      /* Directory in path not found  */
#define PFS_SYMLINK_DEPTH      232	/* Max sym-link depth exceeded  */
#define PFS_ENV_NOT_INITIALIZED	233	/* Can't read environment	*/
#define PFS_EXT_USED_AS_DIR    234	/* Can't use externals as dirs  */
#define PFS_MAX_FWD_DEPTH      235	/* Exceeded max forward depth   */

/* Error codes returned by directory server                    */
/* some of these duplicate errors from individual routines     */
/* some of those error codes should be eliminated              */
#define DIRSRV_AUTHENT_REQ     242      /* Authentication required       */
#define DIRSRV_NOT_AUTHORIZED  243      /* Not authorized                */
#define DIRSRV_NOT_FOUND       244      /* Not found                     */
#define DIRSRV_BAD_VERS        245
#define DIRSRV_NOT_DIRECTORY   246
#define DIRSRV_ALREADY_EXISTS  247	/* Identical link already exists */
#define DIRSRV_NAME_CONFLICT   248	/* Link with name already exists */

#define DIRSRV_UNIMPLEMENTED   251      /* Unimplemented command         */
#define DIRSRV_BAD_FORMAT      252
#define DIRSRV_ERROR           253
#define DIRSRV_SERVER_FAILED   254      /* Unspecified server failure    */

#ifndef PFAILURE
#define	PFAILURE 	       255
#endif

/* Warning codes */

#define PNOWARN			 0	/* No warning indicated		 */
#define PWARN_OUT_OF_DATE	 1	/* Software is out of date       */
#define PWARN_MSG_FROM_SERVER	 2      /* Warning in p_warn_string      */
#define PWARN_UNRECOGNIZED_RESP  3	/* Unrecognized line in response */
#define PWARNING	       255	/* Warning in p_warn_string      */
