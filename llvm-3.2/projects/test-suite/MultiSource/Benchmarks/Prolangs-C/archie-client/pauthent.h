/*
 * Copyright (c) 1989, 1990, 1991 by the University of Washington
 *
 * For copying and distribution information, please see the file
 * <copyright.h>.
 */

#include <copyright.h>

#define PFSA_UNAUTHENTICATED		1

struct pfs_auth_info {
    char			auth_type[100];
    char			authenticator[250];
};

typedef struct pfs_auth_info *PAUTH;
typedef struct pfs_auth_info PAUTH_ST;

PAUTH get_pauth();

#ifndef VMS
# ifndef IN_H
#  include <netinet/in.h>
#  define IN_H
# endif
#else
# ifndef _ARCHIE_VMS
#  include <vms.h>
# endif
#endif

struct client_info {
    int				ainfo_type;
    char			*auth_type;
    char			*authenticator;
    char			*userid;
    short			port;
    struct in_addr		haddr;
    struct pfs_auth_info	*previous;
    struct pfs_auth_info	*next;
};

typedef struct client_info *CINFO;
typedef struct client_info CINFO_ST;
