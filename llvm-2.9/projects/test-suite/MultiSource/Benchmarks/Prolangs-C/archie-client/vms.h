#ifndef _ARCHIE_VMS
#define _ARCHIE_VMS
#include <pmachine.h>

#if !defined(MULTINET_30) && !defined(WOLLONGONG) && !defined(UCX)
#include "[.vms]pseudos.h"
#include "[.vms]types.h"
#include "[.vms]in.h"
#include "[.vms]signal.h"
#include "[.vms]socket.h"
#include "[.vms]time.h"

#else

/* time_t gets multiply defined <ekup> */
#ifndef __TYPES
#define __TYPES
#endif
#ifdef MULTINET_30
# include "multinet_root:[multinet.include.sys]types.h"
# include "multinet_root:[multinet.include.netinet]in.h"
# include "multinet_root:[multinet.include.sys]socket.h"
# include "multinet_root:[multinet.include.sys]time.h"
#endif /* MULTINET_30 */

#ifdef WOLLONGONG
/* We don't want size_t defined.  */
# ifndef __STDDEF
#  define __STDDEF
# endif
# include "twg$tcp:[netdist.include.sys]types.h"
# include "twg$tcp:[netdist.include.netinet]in.h"
# include "twg$tcp:[netdist.include.sys]socket.h"
# include "twg$tcp:[netdist.include.sys]time.h"
#endif /* WOLLONGONG */

#ifdef UCX
# include <types.h>
# include <in.h>
# include <socket.h>
# include <time.h>
# include "[.vms]fd.h"
#endif /* UCX */

#endif /* Multinet or Wallongong or UCX */

#endif /* _ARCHIE_VMS */
