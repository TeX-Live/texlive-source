*** pmx274.for.orig	2016-02-20 22:48:43.793458913 -0500
--- pmx274.for	2016-02-20 23:08:55.803226069 -0500
***************
*** 11062,11068 ****
        end if
        go to 1
        end
!       subroutine getmidi(noinst,lineq,iccount,ibarcnt,ibaroff,nbars,
       *                   lenbar,
  c      subroutine getmidi(nv,lineq,iccount,ibarcnt,ibaroff,nbars,lenbar,
       *                    mtrdenl,first)
--- 11062,11068 ----
        end if
        go to 1
        end
!       subroutine getmidi(noinstarg,lineq,iccount,ibarcnt,ibaroff,nbars,
       *                   lenbar,
  c      subroutine getmidi(nv,lineq,iccount,ibarcnt,ibaroff,nbars,lenbar,
       *                    mtrdenl,first)
***************
*** 11091,11097 ****
       *       debugmidi
        logical debugmidi
        common /commvel/ midivel(nm),midvelc(0:nm),midibal(nm),midbc(0:nm)
!      *                ,miditran(nm),midtc(0:nm),noinstdum,iinsiv(nm)
        integer*2 iinsiv
        character*1 durq
        character*2 instq
--- 11091,11097 ----
       *       debugmidi
        logical debugmidi
        common /commvel/ midivel(nm),midvelc(0:nm),midibal(nm),midbc(0:nm)
!      *                ,miditran(nm),midtc(0:nm),noinst,iinsiv(nm)
        integer*2 iinsiv
        character*1 durq
        character*2 instq
***************
*** 11168,11174 ****
  c  Instrument numbers or letters.  Expect noinst of them.
  c
  c        do 2 ivx = 1 , nv
!         do 2 ivx = 1 , noinst
            call getchar(lineq,iccount,durq)
            if (ichar(durq) .gt. 96) then
  c
--- 11168,11174 ----
  c  Instrument numbers or letters.  Expect noinst of them.
  c
  c        do 2 ivx = 1 , nv
!         do 2 ivx = 1 , noinstarg
            call getchar(lineq,iccount,durq)
            if (ichar(durq) .gt. 96) then
  c
***************
*** 11213,11219 ****
  c    Follow same pattern as for insttrument numbers above.
  c 	
  c        do 7 ivx = 1 , nv
!         do 7 ivx = 1 , noinst
            call getchar(lineq,iccount,durq)
            if (index('123456789',durq) .eq. 0) then
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
--- 11213,11219 ----
  c    Follow same pattern as for insttrument numbers above.
  c 	
  c        do 7 ivx = 1 , nv
!         do 7 ivx = 1 , noinstarg
            call getchar(lineq,iccount,durq)
            if (index('123456789',durq) .eq. 0) then
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
***************
*** 11236,11242 ****
  c    Follow same pattern as for instrument numbers above.
  c 	
  c        do 8 ivx = 1 , nv
!         do 8 ivx = 1 , noinst
            call getchar(lineq,iccount,durq)
            if (index('123456789',durq) .eq. 0) then
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
--- 11236,11242 ----
  c    Follow same pattern as for instrument numbers above.
  c 	
  c        do 8 ivx = 1 , nv
!         do 8 ivx = 1 , noinstarg
            call getchar(lineq,iccount,durq)
            if (index('123456789',durq) .eq. 0) then
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
***************
*** 11259,11265 ****
  c    Follow similar pattern as above, but separator is +|-.
  c 	
  c        do 9 ivx = 1 , nv
!         do 9 ivx = 1 , noinst
            call getchar(lineq,iccount,durq)
            ipm = index('-+',durq)
            if (ipm .eq. 0) then
--- 11259,11265 ----
  c    Follow similar pattern as above, but separator is +|-.
  c 	
  c        do 9 ivx = 1 , nv
!         do 9 ivx = 1 , noinstarg
            call getchar(lineq,iccount,durq)
            ipm = index('-+',durq)
            if (ipm .eq. 0) then
***************
*** 11592,11597 ****
--- 11592,11598 ----
        common /commidisig/ midisig
        common /comlyr/ inputmlyr
        logical inputmlyr
+       data inputmlyr /.false./
        cdot = .false.
  1     call getchar(lineq,iccount,charq)
        if (lastchar) return
***************
*** 14491,14497 ****
  c
  c 160130 Replace '\' by '/'
  c
! 12    ipos = index(pathnameq,'\')
        if (ipos .gt. 0) then
          pathnameq(ipos:ipos)='/'
          print*,'Changed pathname to ',pathnameq(1:lpath)
--- 14492,14498 ----
  c
  c 160130 Replace '\' by '/'
  c
! 12    ipos = index(pathnameq,'\\')
        if (ipos .gt. 0) then
          pathnameq(ipos:ipos)='/'
          print*,'Changed pathname to ',pathnameq(1:lpath)
***************
*** 19855,19861 ****
        common /comclefrests/ centrests
        logical newclef, centrests
        common /comlyr/ inputmlyr
!       logical inputmlyr /.false./
        if (.not.optimize) then
          print*
          print*,'Starting second PMX pass'
--- 19856,19862 ----
        common /comclefrests/ centrests
        logical newclef, centrests
        common /comlyr/ inputmlyr
!       logical inputmlyr 
        if (.not.optimize) then
          print*
          print*,'Starting second PMX pass'
***************
*** 24497,24503 ****
  c   (unless preceded with '\'), check length
  c
        character*128 lineq,lineqt
!       character*1 sq /'\'/
        iend = lenstr(lineq,128)
  c
  c      i2nd = iccount+index(lineq(iccount+1:128),'"')
--- 24498,24505 ----
  c   (unless preceded with '\'), check length
  c
        character*128 lineq,lineqt
!       character*1 sq 
!       data sq /'\\'/
        iend = lenstr(lineq,128)
  c
  c      i2nd = iccount+index(lineq(iccount+1:128),'"')
