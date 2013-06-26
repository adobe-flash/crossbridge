c This is 'globals.inc'
c
c      scatter, shared 
       dimension
     .    mtl  (nmrmax),
     .    dns  (nmrmax),
     .    tmp  (nmrmax),
     .    atrat(nmrmax),
     .    sig   (nmrmax,negp1),
     .    scrat (nmrmax,negp1),
     .    sigtot(nmrmax,negp1),
     .    sigth (nmrmax),
     .    opcv  (       negp1)

c      common, shared /material/ nreg
      common  /material/ nreg

c      common, private /pmaterial/
      common /pmaterial/
     .    mtl  ,
     .    dns  ,
     .    tmp  ,
     .    atrat
c      common, shared /xsec/ opec
      common /xsec/ opec

c      common, private /pxsec/
      common /pxsec/
     .    sig   ,
     .    scrat ,
     .    sigtot,
     .    sigth , 
     .    opcv  
 
c      common, shared /controls/ igroup,ixopec,isorst,irr,ithom,icross,
      common /controls/ igroup,ixopec,isorst,irr,ithom,icross,
     .       dtol,tcen,xmult,ilib,illib,title(20)
 
c      common, shared /newcom/ bwgt    !Formerly 1st item in /samples/.
      common /newcom/ bwgt    !Formerly 1st item in /samples/.


c      common /samples/ nescgp(negrps), enesc(negrps), wcut, wmin,  wmax,
c     .   wlost, wesc, wrr, wabs, wcen, epgain, etot,
c     .   npart, nphtot, nploss, 
c     .   nlost, nesc, nrr, nabs, ncen,
c     .   nscat, nsplt, ntrac


c
c     O    nothing
c     1    summary only
c     2    full output
c
      integer print_flag
      common /output_flags/
     .   print_flag

