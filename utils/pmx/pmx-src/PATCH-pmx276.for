*** pmx276.for.orig	2016-12-07 08:56:24.231733002 -0500
--- pmx276.for	2016-12-07 14:55:34.769485275 -0500
***************
*** 11104,11110 ****
        end if
        go to 1
        end
!       subroutine getmidi(noinst,lineq,iccount,ibarcnt,ibaroff,nbars,
       *                   lenbar,
  c      subroutine getmidi(nv,lineq,iccount,ibarcnt,ibaroff,nbars,lenbar,
       *                    mtrdenl,first)
--- 11104,11110 ----
        end if
        go to 1
        end
!       subroutine getmidi(noinstarg,lineq,iccount,ibarcnt,ibaroff,nbars,
       *                   lenbar,
  c      subroutine getmidi(nv,lineq,iccount,ibarcnt,ibaroff,nbars,lenbar,
       *                    mtrdenl,first)
***************
*** 11210,11216 ****
  c  Instrument numbers or letters.  Expect noinst of them.
  c
  c        do 2 ivx = 1 , nv
!         do 2 ivx = 1 , noinst
            call getchar(lineq,iccount,durq)
            if (ichar(durq) .gt. 96) then
  c
--- 11210,11216 ----
  c  Instrument numbers or letters.  Expect noinst of them.
  c
  c        do 2 ivx = 1 , nv
!         do 2 ivx = 1 , noinstarg
            call getchar(lineq,iccount,durq)
            if (ichar(durq) .gt. 96) then
  c
***************
*** 11255,11261 ****
  c    Follow same pattern as for insttrument numbers above.
  c 	
  c        do 7 ivx = 1 , nv
!         do 7 ivx = 1 , noinst
            call getchar(lineq,iccount,durq)
            if (index('123456789',durq) .eq. 0) then
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
--- 11255,11261 ----
  c    Follow same pattern as for insttrument numbers above.
  c 	
  c        do 7 ivx = 1 , nv
!         do 7 ivx = 1 , noinstarg
            call getchar(lineq,iccount,durq)
            if (index('123456789',durq) .eq. 0) then
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
***************
*** 11278,11284 ****
  c    Follow same pattern as for instrument numbers above.
  c 	
  c        do 8 ivx = 1 , nv
!         do 8 ivx = 1 , noinst
            call getchar(lineq,iccount,durq)
            if (index('123456789',durq) .eq. 0) then
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
--- 11278,11284 ----
  c    Follow same pattern as for instrument numbers above.
  c 	
  c        do 8 ivx = 1 , nv
!         do 8 ivx = 1 , noinstarg
            call getchar(lineq,iccount,durq)
            if (index('123456789',durq) .eq. 0) then
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
***************
*** 11301,11307 ****
  c    Follow similar pattern as above, but separator is +|-.
  c 	
  c        do 9 ivx = 1 , nv
!         do 9 ivx = 1 , noinst
            call getchar(lineq,iccount,durq)
            ipm = index('-+',durq)
            if (ipm .eq. 0) then
--- 11301,11307 ----
  c    Follow similar pattern as above, but separator is +|-.
  c 	
  c        do 9 ivx = 1 , nv
!         do 9 ivx = 1 , noinstarg
            call getchar(lineq,iccount,durq)
            ipm = index('-+',durq)
            if (ipm .eq. 0) then
***************
*** 11634,11639 ****
--- 11634,11640 ----
        common /commidisig/ midisig
        common /comlyr/ inputmlyr
        logical inputmlyr
+       data inputmlyr /.false./
        cdot = .false.
  1     call getchar(lineq,iccount,charq)
        if (lastchar) return
***************
*** 19899,19905 ****
        common /comclefrests/ centrests
        logical newclef, centrests
        common /comlyr/ inputmlyr
!       logical inputmlyr /.false./
        if (.not.optimize) then
          print*
          print*,'Starting second PMX pass'
--- 19900,19906 ----
        common /comclefrests/ centrests
        logical newclef, centrests
        common /comlyr/ inputmlyr
!       logical inputmlyr 
        if (.not.optimize) then
          print*
          print*,'Starting second PMX pass'
***************
*** 24541,24547 ****
  c   (unless preceded with '\'), check length
  c
        character*128 lineq,lineqt
!       character*1 sq /'\'/
        iend = lenstr(lineq,128)
  c
  c      i2nd = iccount+index(lineq(iccount+1:128),'"')
--- 24542,24549 ----
  c   (unless preceded with '\'), check length
  c
        character*128 lineq,lineqt
!       character*1 sq 
!       data sq /'\'/
        iend = lenstr(lineq,128)
  c
  c      i2nd = iccount+index(lineq(iccount+1:128),'"')
