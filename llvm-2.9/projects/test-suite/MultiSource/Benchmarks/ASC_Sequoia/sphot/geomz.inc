c This is 'geomz.inc'
c
c      scatter, shared
      dimension
     .    ng_incr (5),
     .    g_rr   (nzmax,5),
     .    g_zz   (nzmax,5),
     .    ng_itype(nzmax,5),
     .    g_bom  (nzmax,5),
     .    g_sqm  (nzmax,5),
     .    g_ximp (nzmax),
     .    ng_mid  (nzmax),
     .    g_volcl(nzmax),
     .    g_ximpl(nrzmax)

      dimension   
     .    incr (5),
     .    rr   (nzmax,5),
     .    zz   (nzmax,5),
     .    itype(nzmax,5),
     .    bom(nzmax,5),
     .    sqm(nzmax,5),
     .    ximp (nzmax),
     .    mid  (nzmax),
     .    volcl(nzmax),
     .    ximpl(nrzmax)

c      common, shared /geomz/ naxl,axl,nradl,radl,nzones
      common /geomz/ axl,naxl,nradl,radl,nzones
      

c      common, private/pgeomz/
      common /pgeomz/
     .    rr   ,
     .    g_rr ,
     .    zz   ,
     .    g_zz ,
     .    bom  ,
     .    g_bom,
     .    sqm  ,
     .    g_sqm,
     .    ximp ,
     .    g_ximp,
     .    mid  ,
     .    volcl,
     .    g_volcl,
     .    ximpl,
     .    g_ximpl,
     .    itype,
     .    ng_itype,
     .    ng_mid,
     .    incr,
     .    ng_incr

 
