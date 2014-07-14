/*
 * Copyright (c) 1991 by the University of Washington
 *
 * For copying and distribution information, please see the file
 * <copyright.h>.
 */

#include <copyright.h>

/* Queuing priorities for datagrams */
#define	       RDGRAM_MAX_PRI   32765  /* Maximum user proiority          */
#define	       RDGRAM_MAX_SPRI  32767  /* Maximum priority for system use */
#define	       RDGRAM_MIN_PRI  -32765  /* Maximum user proiority          */
#define	       RDGRAM_MIN_SPRI -32768  /* Maximum priority for system use */

int	rdgram_priority = 0;
