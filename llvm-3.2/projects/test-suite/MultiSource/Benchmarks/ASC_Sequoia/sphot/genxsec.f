      
      subroutine genxsec
c----------------------------------------------------------------------
c     Setup the opacity data by group (13) and region (nreg).
c     ixopec .eq. 0 :  use library opacities from subroutine interp
c     ixopec .eq. 1 :  all opacities = opec (input opacity)
c     ixopec .eq. 2 :  use opacities from data statement (sigdat),
c     which means that formatted library on unit 4
c     or binary library on unit 15 are not needed for
c     this particular case (see data sigdat)
      
c     igroup = 0      :  normal 12 group opacity tables used
c     igroup = 1 - 12 :  opacity from that group is used
c     igroup = 13     :  opacity from group 13 is used (rosseland mean)
      
c     CALL subroutine interp for each material (nreg)
c----------------------------------------------------------------------
      include 'params.inc'
      include 'geomz.inc'
      include 'globals.inc'
      include 'mpif.h'

      INTEGER MPIid, ierr

c      common /xsecdat/ sigdat(2,negp1)
      REAL *8 sigdat(2,negp1)

      data sigdat/     .1395449d+06,.8093366d+06,.2022023d+05,
     |     .1344934d+06,.4188673d+04,.4467951d+05,.1033437d+04,
     |     .3250243d+05,.4297604d+03,.1237669d+05,.2235549d+03,
     |     .5984154d+04,.1270899d+03,.3111531d+04,.7992034d+02,
     |     .1722049d+04,.4425178d+02,.9100233d+03,.1456625d+02,
     |     .3776509d+03,.7138723d+00,.7534896d+02,.4231184d-01,
     |     .1032693d+01,.7666376d+01,.6094672d+04/
      

      CALL MPI_COMM_RANK( MPI_COMM_WORLD, MPIid, ierr )

      xthom = 0.0d0
c***  ithom non-zero implies that thomson scattering is used
      if (ithom .ne. 0) xthom = 1.0d0
      
      do 75 i = 1,nreg
         if (ixopec .eq. 0) then
c--------------------ixopec = 0 means opacity library is used
            CALL interp (mtl(i),dns(i),tmp(i),opcv)
         endif
         
c-----store the thomson scattering cross section.
c     - thomson cross section depends only on the material,
c     - and not on the energy group
         
         sigth(i) = .4006*atrat(i)*dns(i)*xthom

         do 69 ig = 1,13
            if( igroup.ne.0 .and. ixopec.eq.0) opcv(ig) = opcv(igroup)
            
c-----store total cross section without thomson.
c     - first index in cross section is the material id,
c     - the second is the energy group
            
            if (ixopec .eq. 1) then
               sigtot(i,ig) = opec
            else if (ixopec .eq. 2) then
               sigtot(i,ig) = sigdat(i,ig)
            else
               sigtot(i,ig) = dns(i) * opcv(ig)
            endif
            
            sig  (i,ig) = sigtot(i,ig) + sigth(i)
            scrat(i,ig) = sigth(i) / sig(i,ig)
 69      continue
 75   continue


      if (icross .ne. 0 .and. MPIid .eq. 0) then
c--------------------print edit of total and thomson cross sections
         do 76 i=1,nreg
            write (6,207) i, sigth(i)
 207      format(//' material',i3,' with thomson cross section (1/cm) '
     .           ,e15.7/)
            write (6,208)
 208        format(10x,'group',15x,'sigtot (1/cm)',15x,'scat ratio'/)
            
             write (6,209) (j, sigtot(i,j), scrat(i,j),
     |           j=1,13)
 209        format(12x,i2,14x,e15.7,10x,e15.7)
 76      continue
      endif

      
      return
      end
