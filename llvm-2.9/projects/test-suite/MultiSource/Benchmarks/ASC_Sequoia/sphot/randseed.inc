c This is 'randseed.inc'
c
      include 'pranf.inc'

      integer*4 seedarray 
      integer*4 NStreams
c      integer*4 mySeed(IrandNumSize)

c      scatter, shared
      dimension
     .   seedarray(IrandNumSize,MaxStreams)

c      common, shared    /seedcom/ 
      common  /seedcom/ 
     .   NStreams
 
c      common, private   /pseedcom/
      common  /pseedcom/
     .   seedarray

