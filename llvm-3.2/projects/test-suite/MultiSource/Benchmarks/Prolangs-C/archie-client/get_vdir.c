/*
 * Copyright (c) 1989, 1990, 1991 by the University of Washington
 *
 * For copying and distribution information, please see the file
 * <copyright.h>.
 *
 * v1.2.2 - 11/19/91 (mmt) - added MSDOS & OS2 stuff
 * v1.2.1 - 10/23/91 (bpk) - added missing code
 * v1.2.0 - 09/17/91 (bpk) - added new get_vdir code
 * v1.1.1 - 08/30/91 (bpk) - cast rindex()
 */

#include <copyright.h>
#include <stdio.h>

#include <pfs.h>
#include <pprot.h>
#include <perrno.h>
#include <pcompat.h>
#include <pauthent.h>
#include <pmachine.h>

#ifdef NEED_STRING_H
# include <string.h>
#else
# include <strings.h>
#endif

#ifdef DEBUG
extern int	pfs_debug;
#endif

extern int	pwarn;
extern char	p_warn_string[];
extern int	perrno;
extern char	p_err_string[];

extern int wcmatch(char *s,char *template);
extern int vl_insert(VLINK vl,VDIR1 vd,int allow_conflict);
extern int ul_insert(VLINK ul,VDIR1 vd,VLINK p);
extern int scan_error(char *erst);

/*
 * get_vdir - Get contents of a directory given its location
 *
 *	      GET_VDIR takes a directory location, a list of desired
 *	      components, a pointer to a directory structure to be 
 *	      filled in, and flags.  It then queries the appropriate 
 *	      directory server and retrieves the desired information.
 *
 *      ARGS:   dhost       - Host on which directory resides
 *              dfile       - Directory on that host
 *              components  - The names from the directory we want
 *		dir	    - Structure to be filled in
 *	        flags       - Options.  See FLAGS
 *		filters     - filters to be applied to result 
 *              acomp       - Pointer to remaining components
 *
 *     FLAGS:	GVD_UNION   - Do not expand union links
 *		GVD_EXPAND  - Expand union links locally
 *		GVD_REMEXP  - Request remote expansion (& local if refused)
 *		GVD_LREMEXP - Request remote expansion of local union links
 *		GVD_VERIFY  - Only verify that args are for a directory
 *              GVD_ATTRIB  - Request attributes from directory server
 *              GVD_NOSORT  - Do not sort links when adding to directory
 *
 *   RETURNS:   PSUCCESS (0) or error code
 *		On some codes addition information in p_err_string
 *
 *     NOTES:   If acomp is non-null the string it points to might be modified
 *
 *              If the directory passed as an argument already has
 *		links or union links, then those lists will be freed
 *              before the new contents are filled in.
 *
 *              If a filter is passed to the procedure, and application of
 *              the filter results in additional union link, then those links
 *              will (or will not) be expanded as specified in the FLAGS field.
 *
 *              If the list of components in NULL, or the null string, then
 *              get_vdir will return all links in the requested directory.
 *
 *      BUGS:   Doesn't process union links yet
 *              Doesn't process errors returned from server
 *		Doesn't expand union links if requested to
 */
int get_vdir(char *dhost,char *dfile,char *components,VDIR1 dir,long flags,
             VLINK filters,char *acomp)
    {
        PTEXT	request;	/* Text of request to dir server             */
	PTEXT	resp;	    	/* Response from dir server	             */

	char	ulcomp[MAX_VPATH];/* Work space for new current component    */
	char	*comp = components;

	VLINK	cur_link = NULL;/* Current link being filled in              */
	VLINK 	exp = NULL; 	/* The current ulink being expanded	     */
	VLINK	pul = NULL;     /* Prev union link (insert new one after it) */
	VLINK	l;		/* Temp link pointer 			     */
	int	mcomp;		/* Flag - check multiple components          */
	int	unresp;		/* Flag - received unresolved response       */
	int	getattrib = 0;  /* Get attributes from server                */
	int	vl_insert_flag; /* Flags to vl_insert                        */

	int	fwdcnt = MAX_FWD_DEPTH;

	int	no_links = 0;   /* Count of number of links found	     */

	char	options[40];    /* LIST option                               */
	char	*opt;           /* After leading +                           */

	PAUTH	authinfo;

	/* Treat null string like NULL (return entire directory) */
	if(!components || !*components) comp = NULL;

	if(acomp && !filters) mcomp = 1;
	else mcomp = 0;

	if(flags&GVD_ATTRIB) {
	    getattrib++;
	    flags &= (~GVD_ATTRIB);
	}

	if(flags&GVD_NOSORT) vl_insert_flag = VLI_NOSORT;
	else vl_insert_flag = VLI_ALLOW_CONF;
	flags &= (~GVD_NOSORT);

	if(filters) comp = NULL;

	perrno = 0;

	authinfo = get_pauth(PFSA_UNAUTHENTICATED);

	*options = '\0';

	if(getattrib) {
	    strcat(options,"+ATTRIBUTES");
	    flags &= (~GVD_ATTRIB);
	}

	if(!filters) { /* Can't do remote expansion if filters to be applied */
	    if(flags == GVD_REMEXP) strcat(options,"+EXPAND");
	    if(flags == GVD_LREMEXP) strcat(options,"+LEXPAND");
	}

	/* If all we are doing is verifying that dfile is a directory */
	/* then we do not want a big response from the directory      */
	/* server.  A NOT-FOUND is sufficient.			      */
	if(flags == GVD_VERIFY)
#ifdef NEWVERIFYOPT
	    strcat(options,"+VERIFY");
#else
	comp = "%#$PRobably_nOn_existaNT$#%";
#endif

	if(*options) opt = options+1;
	else opt = "''";

    startover:
	request = ptalloc();

	sprintf(request->start,
		"VERSION %d\nAUTHENTICATOR %s %s\nDIRECTORY ASCII %s\nLIST %s COMPONENTS %s%s%s\n",
		VFPROT_VNO, authinfo->auth_type, authinfo->authenticator,
		dfile,opt, (comp ? comp : ""), (mcomp ? "/" : ""),
		(mcomp ? acomp : ""));

	request->length = strlen(request->start);

#ifdef DEBUG
	if(pfs_debug > 2)
	    fprintf(stderr,"Sending message to dirsrv:\n%s",request->start);
#endif

#if defined(MSDOS)
	resp = dirsend(request,dhost,0L);
#else
	resp = dirsend(request,dhost,0);
#endif

#ifdef DEBUG
	if(pfs_debug && (resp == NULL)) {
	    fprintf(stderr,"Dirsend failed: %d\n",perrno);
	}
#endif

	/* If we don't get a response, then if the requested       */
	/* directory, return error, if a ulink, mark it unexpanded */
	if(resp == NULL) {
	    if(exp) exp->expanded = FAILED;
	    else return(perrno);
	}

	unresp = 0;

	/* Here we must parse reponse and put in directory */
	/* While looking at each packet 		   */
	while(resp) {
	    PTEXT		vtmp;
	    char		*line;

	    vtmp = resp;
#ifdef DEBUG
	    if(pfs_debug > 3) fprintf(stderr,"%s\n",resp->start);
#endif
	    /* Look at each line in packet */
	    for(line = resp->start;line != NULL;line = nxtline(line)) {
		switch (*line) {
		    
		    /* Temporary variables to hold link info */
		    char	l_linktype;
		    char 	l_name[MAX_DIR_LINESIZE];
		    char	l_type[MAX_DIR_LINESIZE];
		    char 	l_htype[MAX_DIR_LINESIZE];
		    char 	l_host[MAX_DIR_LINESIZE];
		    char 	l_ntype[MAX_DIR_LINESIZE];
		    char 	l_fname[MAX_DIR_LINESIZE];
		    int		l_version;
		    char 	t_unresolved[MAX_DIR_LINESIZE];
		    int		l_magic;
		    int		tmp;

		case 'L': /* LINK or LINK-INFO */
		    if(strncmp(line,"LINK-INFO",9) == 0) {
			PATTRIB		at;
			PATTRIB		last_at;
			at = parse_attribute(line);
			if(!at) break;

			/* Cant have link info without a link */
			if(!cur_link) {
			    perrno = DIRSRV_BAD_FORMAT;
			    atfree(at);
			    break;
			}
			
			if(cur_link->lattrib) {
			    last_at = cur_link->lattrib;
			    while(last_at->next) last_at = last_at->next;
			    at->previous = last_at;
			    last_at->next = at;
			}
			else {
			    cur_link->lattrib = at;
			    at->previous = NULL;
			}
			break;
		    }

		    /* Not LINK-INFO, must be LINK - if not check for error */
		    if(strncmp(line,"LINK",4) != 0) goto scanerr;

		    /* If only verifying, don't want to change dir */
		    if(flags == GVD_VERIFY) {
			break;
		    }
		    /* If first link and some links in dir, free them */
		    if(!no_links++) {
			if(dir->links) vllfree(dir->links); dir->links=NULL;
			if(dir->ulinks) vllfree(dir->ulinks); dir->ulinks=NULL;
			}
			
		    cur_link = vlalloc();

		    /* parse and insert file info */
		    tmp = sscanf(line,"LINK %c %s %s %s %s %s %s %d %d", &l_linktype,
				 l_type, l_name, l_htype, l_host, 
				 l_ntype, l_fname, &(cur_link->version),
				 &(cur_link->f_magic_no));

		    if(tmp != 9) {
			perrno = DIRSRV_BAD_FORMAT;
			vlfree(cur_link);
			break;
		    }

		    cur_link->linktype = l_linktype;
		    cur_link->type = stcopyr(l_type,cur_link->type);
		    cur_link->name = stcopyr(unquote(l_name),cur_link->name);
		    cur_link->hosttype = stcopyr(l_htype,cur_link->hosttype);
		    cur_link->host = stcopyr(l_host,cur_link->host);
		    cur_link->nametype = stcopyr(l_ntype,cur_link->nametype);
		    cur_link->filename = stcopyr(l_fname,cur_link->filename);

		    /* Double check to make sure we don't get */
		    /* back unwanted components		      */
		    /* OK to keep if special (URP) links      */
		    /* or if mcomp specified                  */
		    if(!mcomp && (cur_link->linktype == 'L') && 
		       (!wcmatch(cur_link->name,comp))) {
			vlfree(cur_link);
			break;
		    }

		    /* If other optional info was sent back, it must */
		    /* also be parsed before inserting link     ***  */
		    
		    
		    if(cur_link->linktype == 'L') 
			vl_insert(cur_link,dir,vl_insert_flag);
		    else {
			tmp = ul_insert(cur_link,dir,pul);

			/* If inserted after pul, next one after cur_link */
			if(pul && (!tmp || (tmp == UL_INSERT_SUPERSEDING)))
			    pul = cur_link;
		    }
		    
		    break;

		case 'F': /* FILTER, FAILURE or FORWARDED*/
		    /* FORWARDED */
		    if(strncmp(line,"FORWARDED",9) == 0) {
			if(fwdcnt-- <= 0) {
			    ptlfree(resp);
			    perrno = PFS_MAX_FWD_DEPTH;
			    return(perrno);
			}
			/* parse and start over */

			tmp = sscanf(line,"FORWARDED %s %s %s %s %d %d", 
				     l_htype,l_host,l_ntype,l_fname,
				     &l_version, &l_magic);

			dhost = stcopy(l_host);
			dfile = stcopy(l_fname);

			if(tmp < 4) {
			    perrno = DIRSRV_BAD_FORMAT;
			    break;
			}

			ptlfree(resp);
			goto startover;
		    }
		    if(strncmp(line,"FILTER",6) != 0) goto scanerr;
		    break;


		case 'M': /* MULTI-PACKET (processed by dirsend) */
		case 'P': /* PACKET (processed by dirsend) */
		    break;

		case 'N': /* NOT-A-DIRECTORY or NONE-FOUND */
		    /* NONE-FOUND, we just have no links to insert */
		    /* It is not an error, but we must clear any   */
		    /* old links in the directory arg              */
		    if(strncmp(line,"NONE-FOUND",10) == 0) {
			/* If only verifying, don't want to change dir */
			if(flags == GVD_VERIFY) {
			    break;
			}

			/* If first link and some links in dir, free them */
			if(!no_links++) {
			    if(dir->links) vllfree(dir->links);
			    if(dir->ulinks) vllfree(dir->ulinks);
			    dir->links = NULL;
			    dir->ulinks = NULL;
			}
			break;
		    }
		    /* If NOT-A-DIRECTORY or anything else, scan error */
		    goto scanerr;

		case 'U': /* UNRESOLVED */
		    if(strncmp(line,"UNRESOLVED",10) != 0) {
			goto scanerr;
		    }
		    tmp = sscanf(line,"UNRESOLVED %s", t_unresolved);
		    if(tmp < 1) {
			perrno = DIRSRV_BAD_FORMAT;
			break;
		    }
		    /* If multiple components were resolved */
		    if(strlen(t_unresolved) < strlen(acomp)) {
			strcpy(ulcomp,acomp);
			/* ulcomp is the components that were resolved */
			*(ulcomp+strlen(acomp)-strlen(t_unresolved)-1) = '\0';
			/* Comp gets the last component resolved */
			comp = strrchr(ulcomp,(int) '/');
			if(comp) comp++;
			else comp = ulcomp;
			/* Let rd_vdir know what remains */
			strcpy(acomp,t_unresolved);
		    }
		    unresp = 1;
		    break;

		case 'V': /* VERSION-NOT-SUPPORTED */
		    if(strncmp(line,"VERSION-NOT-SUPPORTED",21) == 0) {
			perrno = DIRSRV_BAD_VERS;
			return(perrno);
		    }
		    goto scanerr;

		scanerr:
		default:
		    if(*line && (tmp = scan_error(line))) {
			ptlfree(resp);
			return(tmp);
		    }
		    break;
		}
	    }

	    resp = resp->next;

	    ptfree(vtmp);
	}

	/* We sent multiple components and weren't told any */
	/* were unresolved                                  */
	if(mcomp && !unresp) {
	    /* ulcomp is the components that were resolved */
	    strcpy(ulcomp,acomp);
	    /* Comp gets the last component resolved */
	    comp = strrchr(ulcomp,(int) '/');
	    if(comp) comp++;
	    else comp = ulcomp;
	    /* If we have union links to resolve, only one component remains */
	    mcomp = 0;
	    /* Let rd_vdir know what remains */
	    *acomp = '\0';
	}

	/* If only verifying, we already know it is a directory */
	if(flags == GVD_VERIFY) return(PSUCCESS);

	/* Don't return if matching was delayed by the need to filter    */
	/* if FIND specified, and dir->links is non null, then we have   */
	/* found a match, and should return.                             */
	if((flags & GVD_FIND) && dir->links && (!filters))
	    return(PSUCCESS);

	/* If expand specified, and ulinks must be expanded, making sure */
        /* that the order of the links is maintained properly            */

expand_ulinks:

	if((flags != GVD_UNION) && (flags != GVD_VERIFY)) {

	    l = dir->ulinks;

	    /* Find first unexpanded ulink */
	    while(l && l->expanded && (l->linktype == 'U')) l = l->next;
	    
	    /* Only expand if a FILE or DIRECTORY -  Mark as  */
            /* failed otherwise                               */
	    /* We must still add support for symbolic ulinks */
	    if(l) {
		if ((strcmp(l->type,"DIRECTORY") == 0) || 
		    (strcmp(l->type,"FILE") == 0)) {
		    l->expanded = TRUE;
		    exp = l;
		    pul = l;
		    dhost = l->host;
		    dfile = l->filename;
		    goto startover; /* was get_contents; */
		}
		else l->expanded = FAILED;
	    }
	}

	/* Double check to make sure we don't get */
	/* back unwanted components		  */
	/* OK to keep if special (URP) links      */
	if(components && *components) {
	    l = dir->links;
	    while(l) {
		VLINK	ol;
		if((l->linktype == 'L') && (!wcmatch(l->name,components))) {
		    if(l == dir->links)
			dir->links = l->next;
		    else l->previous->next = l->next;
		    if(l->next) l->next->previous = l->previous;
		    ol = l;
		    l = l->next;
		    vlfree(ol);
		}
		else l = l->next;
	    }
	}

	return(PSUCCESS);
    }
