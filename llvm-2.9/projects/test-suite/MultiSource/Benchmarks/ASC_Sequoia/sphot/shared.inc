c This is 'shared.inc'
c
c      common, shared /g_samples/ 
      common /g_samples/ 
c     .   g_bwgt(maxruns), 
c     .   g_wcut(maxruns), 
c     .   g_wmin(maxruns),  
c     .   g_wmax(maxruns),
     .   g_etot(maxruns), 
     .   g_epgain(maxruns),
     .   g_wlost(maxruns), 
     .   g_wesc(maxruns), 
     .   g_wrr(maxruns), 
     .   g_wabs(maxruns), 
     .   g_wcen(maxruns),
     .   g_enesc (maxruns,negrps),
     .   ng_npart(maxruns), 
     .   ng_nphtot(maxruns), 
     .   ng_nploss(maxruns),
     .   ng_nlost(maxruns), 
     .   ng_nesc(maxruns), 
     .   ng_nrr(maxruns), 
     .   ng_nabs(maxruns), 
     .   ng_ncen(maxruns),
     .   ng_nscat(maxruns), 
     .   ng_nsplt(maxruns), 
     .   ng_ntrac(maxruns),
     .   ng_nescgp(maxruns,negrps)

c      common, shared /g_escprb/  g_ffesc(maxruns)
      common /g_escprb/  g_ffesc(maxruns)

c      common, shared /g_times/
      common /g_times/
     .   g_tstart(maxruns),
     .   g_tstop(maxruns),
     .   g_texec(maxruns),
     .   g_overheadTime(maxruns),
     .   g_firstRanf(maxruns),
     .   g_lastRanf(maxruns)

      integer*4 NRuns
c      common, shared /NRunsCom/ NRuns
      common  /NRunsCom/ NRuns


 
