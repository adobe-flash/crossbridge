/*
 * Copyright (c) 1989, 1990 by the University of Washington
 *
 * For copying and distribution information, please see the file
 * <copyright.h>.
 */

#include <copyright.h>

/* 
 * pcompat.h - Definitions for compatability library
 *
 * This file contains the defintions used by the compatability
 * library.  Among the definitions are the possible values for
 * pfs_disable_flag.  This file also contains the external 
 * declaration of that variable.  Note, however that the 
 * the module pfs_disable_flag.o is included in libpfs.a
 * because some of the routines in that library set it.
 * The only place it is checked, however, is in pfs_access, 
 * found in libpcompat.a
 *
 */

extern	int		pfs_default;
extern	int		pfs_enable;

/* Definitions for values of pfs_enable */
#define PMAP_DISABLE      0
#define PMAP_ENABLE       1
#define PMAP_COLON	  2
#define PMAP_ATSIGN_NF	  3
#define PMAP_ATSIGN	  4

#define DISABLE_PFS(stmt) {int DpfStmp; DpfStmp = pfs_enable;\
			   pfs_enable = PMAP_DISABLE; \
			   stmt; \
			   pfs_enable = DpfStmp;}

/* Definitions for PFS_ACCESS */
#define PFA_MAP           0  /* Map the file name only                       */
#define PFA_CREATE        1  /* Create file if not found                     */
#define PFA_CRMAP         2  /* Map file name.  Map to new name if not found */
#define PFA_RO            4  /* Access to file is read only                  */

#define check_pfs_default() if(pfs_default == -1) get_pfs_default()
