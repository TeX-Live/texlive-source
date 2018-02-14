*** pmx284.for.orig	2018-02-12 13:36:14.825209815 -0500
--- pmx284.for	2018-02-12 13:36:14.848209816 -0500
***************
*** 11875,11881 ****
        end if
        go to 1
        end
!       subroutine getmidi(noinst,lineq,iccount,ibarcnt,ibaroff,nbars,
       *                   lenbar,
  c      subroutine getmidi(nv,lineq,iccount,ibarcnt,ibaroff,nbars,lenbar,
       *                    mtrdenl,first)
--- 11875,11881 ----
        end if
        go to 1
        end
!       subroutine getmidi(noinstarg,lineq,iccount,ibarcnt,ibaroff,nbars,
       *                   lenbar,
  c      subroutine getmidi(nv,lineq,iccount,ibarcnt,ibaroff,nbars,lenbar,
       *                    mtrdenl,first)
***************
*** 11981,11987 ****
  c  Instrument numbers or letters.  Expect noinst of them.
  c
  c        do 2 ivx = 1 , nv
!         do 2 ivx = 1 , noinst
            call getchar(lineq,iccount,durq)
            if (ichar(durq) .gt. 96) then
  c
--- 11981,11987 ----
  c  Instrument numbers or letters.  Expect noinst of them.
  c
  c        do 2 ivx = 1 , nv
!         do 2 ivx = 1 , noinstarg
            call getchar(lineq,iccount,durq)
            if (ichar(durq) .gt. 96) then
  c
***************
*** 12026,12032 ****
  c    Follow same pattern as for insttrument numbers above.
  c 	
  c        do 7 ivx = 1 , nv
!         do 7 ivx = 1 , noinst
            call getchar(lineq,iccount,durq)
            if (index('123456789',durq) .eq. 0) then
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
--- 12026,12032 ----
  c    Follow same pattern as for insttrument numbers above.
  c 	
  c        do 7 ivx = 1 , nv
!         do 7 ivx = 1 , noinstarg
            call getchar(lineq,iccount,durq)
            if (index('123456789',durq) .eq. 0) then
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
***************
*** 12049,12055 ****
  c    Follow same pattern as for instrument numbers above.
  c 	
  c        do 8 ivx = 1 , nv
!         do 8 ivx = 1 , noinst
            call getchar(lineq,iccount,durq)
            if (index('123456789',durq) .eq. 0) then
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
--- 12049,12055 ----
  c    Follow same pattern as for instrument numbers above.
  c 	
  c        do 8 ivx = 1 , nv
!         do 8 ivx = 1 , noinstarg
            call getchar(lineq,iccount,durq)
            if (index('123456789',durq) .eq. 0) then
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
***************
*** 12072,12078 ****
  c    Follow similar pattern as above, but separator is +|-.
  c 	
  c        do 9 ivx = 1 , nv
!         do 9 ivx = 1 , noinst
            call getchar(lineq,iccount,durq)
            ipm = index('-+',durq)
            if (ipm .eq. 0) then
--- 12072,12078 ----
  c    Follow similar pattern as above, but separator is +|-.
  c 	
  c        do 9 ivx = 1 , nv
!         do 9 ivx = 1 , noinstarg
            call getchar(lineq,iccount,durq)
            ipm = index('-+',durq)
            if (ipm .eq. 0) then
***************
*** 20724,20736 ****
        common /comclefrests/ centrests
        logical newclef, centrests
        common /comlyr/ inputmlyr
!       logical inputmlyr /.false./
        common /combottop/ botamt,topamt,bottopgap
        logical bottopgap
        common /comis4bignv/ is4bignv,AIset
        logical is4bignv,AIset
        common /comhair/ idhairuse,idhair(nm)
        bottopgap = .false.
        idhairuse = 0
        if (.not.optimize) then
          print*
--- 20724,20737 ----
        common /comclefrests/ centrests
        logical newclef, centrests
        common /comlyr/ inputmlyr
!       logical inputmlyr 
        common /combottop/ botamt,topamt,bottopgap
        logical bottopgap
        common /comis4bignv/ is4bignv,AIset
        logical is4bignv,AIset
        common /comhair/ idhairuse,idhair(nm)
        bottopgap = .false.
+       inputmlyr = .false.
        idhairuse = 0
        if (.not.optimize) then
          print*
***************
*** 25399,25405 ****
  c   (unless preceded with '\'), check length
  c
        character*128 lineq,lineqt
!       character*1 sq /'\'/
        iend = lenstr(lineq,128)
  c
  c      i2nd = iccount+index(lineq(iccount+1:128),'"')
--- 25400,25407 ----
  c   (unless preceded with '\'), check length
  c
        character*128 lineq,lineqt
!       character*1 sq 
!       sq = '\'
        iend = lenstr(lineq,128)
  c
  c      i2nd = iccount+index(lineq(iccount+1:128),'"')
