cccccccccccccccccccccccccc
cc                      
cc  scor2prt 2/19/16 for PMX 2.74                     
cccccccccccccccccccccccccc
c This program, developed by Don Simons (dsimons@roadrunner.com), is 
c part of the PMX distribution, PMX is a preprocessor for MusiXTeX. In concert 
c with MusiXTeX and TeX, the purpose of PMX is to allow the user to create 
c high-quality typeset musical scores by including a sequence of PMX commands 
c in an ASCII input file. Scor2prt is an auxiliary program that creates PMX 
c input files for individual parts from the PMX input file for the score.
c 
c This program is free software: you can redistribute it and/or modify
c it under the terms of the GNU General Public License as published by
c the Free Software Foundation, either version 3 of the License, or
c (at your option) any later version.
c 
c This program is distributed in the hope that it will be useful,
c but WITHOUT ANY WARRANTY; without even the implied warranty of
c MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
c GNU General Public License for more details.
c 
c You should have received a copy of the GNU General Public License
c along with this program.  If not, see <http://www.gnu.org/licenses/>.
c
c 2/19/16
c   Exit gracefully when last input line is comment, with mods in
c     subroutine chkcom, adding logical argument goto999
c     which is set to .true. in that case, causing input file to
c     be closed after exiting.
c 4/9/12
c   Add logical declarations to subroutine dosetup
c 8/8/11
c   Copy & mod Ki into parts when after beginning.
c 7/18/11
c   Start to fix up for AS..., also Ki was done a little earlier.
c 7/5/10
c   Modify Manual to explain extended hex numbering for part-specific comments
c 5/16/10
c   Extend part-specific comments for up to 24 parts (subroutine chkcom)
c     Allow files 11-34 for parts
c     Change file 30 to 40
c 6/1/08 es2p
c   noimax = 24
c
c To do
c  Override fracindent, musicsize?
c  Deal with midbar R?
c 2/29/04
c  Check for special character being in quoted string. 
c 10/20/02 PMX 2.407
c  Interpret AN[n]"[used-defined-part-file-name]"
c 1/21/02 
c  Deals with rm in multiple parts. 
c s2p15
c  Get right non-tex index even if there's "\" inside D"..."
c s2p14 
c  10/29/00 fix transfer to parts of negative mtrdnp
c  4/29/00 screen for "%%" followed by "T"
c  Restore change from "Version 2.1a, 21 Dec"
c s2p13
c  Allow whole-bar double-whole rests.
c  Bypass MIDI commands "I..." achar(10)="I"
c s2p12
c  Let %ablabla ... %cblabla represent pmx input for parts 10-12.  But %?blabla
c    only taken as such if ? represents a hex digit .le. noinst, otherwise it's
c    a simple comment.  This lessens incompatibility.
c  In subroutine mbrest, properly open parts 10-12 if needed
c s2p11
c  Ignore leading blanks
c  Fix undefined linelength in mbrest at very end of comments. 
c s2p10
c  Fix non-transfer of P..c" "
c  Allow "%%" and "%"n anywhere
c Version 1.43 
c  Fix bug with P in TeX string.
c  Ignore shifted whole-bar rests when consolidating whole-bar rests
c  Copy type 4 TeX into all parts.
c  Deal with XB and XP.
c  Permit transfer of blank line into parts
c  Change staves/inst in 'M' command.
c  Arbitrary staves/inst.
c  Recognize m1/2/3/4 syntax.
c  Enable comment and one-voice syntax in instrument names
c
c Changes since 1.1
c  Deal with saved macros.
c  Revise setup readin, to admit comments.
c  Do not copy 'X' into parts
c
cccccccccccccccccccccccccccccccccccccccccccccccc  
      parameter (noimax=24)
      common /all/ noinow,iorig(noimax),noinst,insetup,replacing,
     *       instnum(noimax),botv(noimax),nvi(noimax),nsyst,nvnow
      integer*4 ludpfn(noimax)
      character*1 sq,achar(10)
      character*2 termsym
      character*27 jobname,infileq
      character*128 instrum(noimax),udpfnq(noimax)
      character*128 line,holdln,templine
      logical termrpt,isachar,frstln,oneof2,botv,clefpend,fexist,
     *        insetup,replacing,gotname,yesodd,goto999
      character*1 dumq
      data achar /'P','m','V','R','A','h','w','K','M','I'/
      clefpend = .false.
      insetup = .true.
      replacing = .false.
      frstln = .true.
      lenhold = 0
      sq = char(92)
      print*,'This is scor2prt for PMX 2.74, 19 February 2016'
      numargs = iargc()
      if (numargs .eq. 0) then
        print*,'You could have entered a jobname on the command line,'
        print*,'      but you may enter one now:'
        read(*,'(a)')jobname
        numargs = 1
      else 
c       call getarg(1,jobname,idum) ! May need to replace this w/ next line 
        call getarg(1,jobname) 
      end if
      ljob = lenstr(jobname,27)
      if (ljob .eq. 0) then
        print*,'No was jobname entered. Restart and try again.'
        stop
      end if
c
c  Strip ".pmx" if necessary
c
      ndxpmx = max(index(jobname,'.pmx'),index(jobname,'.PMX'))
      if (ndxpmx .gt. 0) then
        jobname = jobname(1:ndxpmx-1)
        ljob = ljob-4
      end if
c
c  Check for existence of input file
c
      infileq = jobname(1:ljob)//'.pmx'
      inquire(file=infileq,EXIST=fexist)
      if (.not.fexist) then
        print*,'Cannot find file '//infileq
        stop
      end if
      open(10,file=jobname(1:ljob)//'.pmx')
c
c Open all instrument files now for allparts stuff.  Later disgard those >nv
c
      do 19 iv = 1 , noimax
        iorig(iv) = iv
        open(10+iv,status='SCRATCH')
        ludpfn(iv) = 0
19    continue
      read(10,'(a)')line
      call chkcom(line,goto999)
      if (line(1:3) .eq. '---') then
        call allparts(line,128)
31      read(10,'(a)')line
        if (line(1:3) .ne. '---') then
          call allparts(line,128)
          go to 31
        end if
        call allparts(line,128)
        read(10,'(a)')line
        call chkcom(line,goto999)
      end if
      iccount = 0
      nv = readin(line,iccount,1)+.1
      noinst = readin(line,iccount,2)+.1
      if (noinst .gt. 0) then
        nvi(1) = nv-noinst+1
      else
        noinst = 1-noinst
        do 21 iinst = 1 , noinst
          nvi(iinst) = readin(line,iccount,-1)+.1    
21      continue
      end if
      noinow = noinst
      insnow = 1
c
c  ivlast is last iv in current inst.  instnum(iv) is iinst for current voice. 
c
      ivlast = nvi(1)
      do 22 iv = 1 , nv
        instnum(iv) = insnow
        if (iv .eq. ivlast) then
          if (iv .lt. nv) botv(iv+1) = .true.	
c
c  The previous stmt will set botv true only for bot voice of iinst>1.  It is
c  used when writing termrpts, but the one in voice one is handled differently,
c  so botv(1) is left .false.
c
          if (insnow .lt. noinst) then
            insnow = insnow+1
            ivlast = ivlast+nvi(insnow)
          end if
        end if
22    continue
      mtrnuml = readin(line,iccount,0)+.1
      mtrdenl = readin(line,iccount,0)+.1
      mtrnmp = readin(line,iccount,0)+.1
      mtrdnp = readin(line,iccount,0)+.1
      xmtrnum0 = readin(line,iccount,0)
      isig = readin(line,iccount,0)+.1 
      npages = readin(line,iccount,3)+.1
      nsyst = readin(line,iccount,0)+.1
      musicsize = readin(line,iccount,4)+.1
      fracindent = readin(line,iccount,5)
      if (npages .eq. 0) then
        print*,
     *'You entered npages=0, which means nsyst is not the total number'
        print*,
     *'of systems.  Scor2prt has to know the total number of systems.'
        print*,
     *'Please set npages and nsyst to their real values.'   
        stop
      end if
c
c  Must leave insetup=.true. else could bypass ALL instrument names.
c
      read(10,'(a)')line
      call chkcom(line,goto999)
	backspace(10)
c
c  Normally this puts pointer at start of line with 1st inst name
c  Check if prior line was "%%"
c
      backspace(10)
	read(10,'(a)')line
	if (line(1:2) .eq. '%%') backspace(10)
      do 14 iv = 1 , noinst
	  gotname = .false.
16      read(10,'(a)') instrum(iv)
        if (instrum(iv)(1:2) .eq. '%%') then
          read(10,'(a)')line
          go to 16
        else if (instrum(iv)(1:1) .eq. '%') then
          ivq = ichar(instrum(iv)(2:2))-48
		if (ivq.ne.iv) then
c
c  It's really a comment.  Copy to parts, then get another trial name.
c
            call allparts(instrum(iv),128)
            go to 16
          else
            line = instrum(iv)(3:)  
            instrum(iv) = line
            gotname = .true.
          end if
        else
          gotname = .true.
        end if
cc
cc  The following checks for macro that write original C-clef as part of
cc  instrument name.  See pmx.tex
cc
c        if (index(instrum(iv),'namewpc') .eq. 0) then
c          write(10+iv,'(a)')' '
c        else
c          inm1 = index(instrum(iv),'{')+1
c          inm2 = index(instrum(iv),'}')-1
c          read(instrum(iv)(inm2+2:inm2+8),'(i1,4x,2i1)')ilev,iy1,iy2
c          write(10+iv,'(a)')sq//'namewpc{}'//char(ilev+48)//'{20}'//
c     *      char(iy1+49)//char(iy2+49)
c          instrum(iv) = instrum(iv)(inm1:inm2)
c        end if
        if (.not.gotname) then
          print*,'You must provide a replacement instrument name'
          stop
        end if
        write(10+iv,'(a)')' '
14    continue
      replacing = .false.
      nvnow = nv
c
c  Clef string:  Note insetup is still T, so "%%" will be treated specially
c
      read(10,'(a)')line
      call chkcom(line,goto999)
	if (replacing) then
c
c  If here, we have next line after "%%", containing score's clef string
c  Assume all clefs are handled with instrument comments.
c
        read(10,'(a)')line
        call chkcom(line,goto999)
        backspace(10)
      else
c
c  If here, line has the clef string in it.  Handle the old way
c
        kvstart = 1
        kvend = nvi(1)
        do 2 inst = 1 , noinst
          write(10+inst,'(a'//char(48+nvi(inst))//')')
     *                                line(kvstart:kvend)
          if (inst .lt. noinst) then
            kvstart = kvend+1
            kvend = kvstart+nvi(inst+1)-1
          end if
2       continue
      end if
      replacing = .false.
      insetup = .false.
c
c  *****NOTE*****This comment applies to stuff done earlier!
c  Before starting the big loop, copy initial instnum and staffnum stuff
c  into working values.  Latter may change if noinst changes.  Also make
c  list of current inst nums relative to original ones.  In addition to those
c  below, must redo instnum(iv) and botv(iv) when we change noinst.
c
c  Path string:  ASSUME THIS WILL NEVER BE ALTERED IN PARTS!
c
18    read(10,'(a)') line
      if (line(1:1) .eq. '%') then
        call allparts(line,128)
        go to 18
      end if
      call allparts(line,128)
c
c  Write instrument names.  Will be blank if later part of a score.
c
      if (instrum(1)(1:1) .ne. ' ') then
        do 3 iv = 1 , noinst
          len = lenstr(instrum(iv),79)
          write(10+iv,'(a2/a)')'Ti',instrum(iv)(1:len)
3       continue
      end if
c
c  The big loop.  Except for '%%', put all comment lines in all parts.
c  Unless preceeded by '%%', put all type 2 or 3 TeX Strings in all parts
c  If a line starts with %!, put the rest of it in each part.
c  If a line starts with %[n], put the rest of it in part [n].
c  Check for Tt, Tc, Voltas, Repeats, headers, lower texts, meter changes.
c     Assume they only come at top of block, except terminal repeat needs
c     special handling.
c  Check for "P"; ignore in parts.
c  Check for consecutive full-bar rests; if found, replace with rm[nn]
c
      iv = 1
      iinst = 1
      termrpt = .false.
4     continue
      read(10,'(a)',end=999)line
      lenline = lenstr(line,128)
      if (lenline .eq. 0) go to 4
      call zapbl(line,128)
      call chkcom(line,goto999)
      if (goto999) go to 999
      lenline = lenstr(line,128)
	if (lenline .eq. 0) go to 4
      if (line(1:1) .eq. 'T') then
        call allparts(line,128)
        read(10,'(a)')line
        call allparts(line,128)
        go to 4
      else if (line(1:2) .eq. sq//sq) then
        call allparts(line,128)
        go to 4
      else if (index('hl',line(1:1)).gt.0 .and.
     *    index(' +-',line(2:2)) .gt. 0) then
        call allparts(line,128)
        read(10,'(a)')line
        call allparts(line,128)
        go to 4
      else if (iv .eq. 1) then
        do 5 ia = 1 , 10
24        continue
          idxa = ntindex(line,achar(ia))
          isachar = idxa .gt. 0
          if (idxa.gt.1) isachar = line(idxa-1:idxa-1).eq.' '
c
c                   1   2   3   4   5   6   7   8   9   10
c      data achar /'P','m','V','R','A','h','w','K','M','I'/
c
          if (ia .eq. 9) isachar = 
     *                    isachar .and. line(idxa+1:idxa+1).eq.'S'
          if (isachar) then
c
c  Check whether character is inside a quoted string by counting 
c  how many quotes precede it in the line
c

            call OddQuotesBefore(line,idxa,yesodd)
            if (yesodd) isachar=.false.
          end if            
          if (isachar) then
c
c  Find next blank
c
            do 6 ib = idxa+1 , 128
              if (line(ib:ib) .eq. ' ') go to 7
6           continue
            print*,'Problem with "V,R,m,P,A,h,MS, or w"'
            print*,
     *        'Send files to Dr. Don at dsimons at roadrunner dot com'
            stop 1
7           continue
c
c  Next blank is at position ib.  Later, if ia=1, must check for Pc"  " ; 
c    i.e., look for '"' between P and blank
c
            if (ia .eq. 4) then
c
c  Check for terminal repeat.  Note if there's a term rpt, there can't be any
c  others.  Also, must process repeats LAST, after m's and 'V's
c
              do 8 ic = ib+1 , 128
c
c  If any subsequent character on this line is neither blank nor "/", get out
c
                if (index(' /',line(ic:ic)) .eq. 0) go to 9
                if (line(ic:ic) .eq. '/') then
                  termrpt = .true.
                  termsym = line(ib-2:ib-1)
c
c  Process the line as if there were no "R"
c
                  go to 10
                end if
8             continue
c
c +++ 060812
c  If here, all chars after "R" symbol are blanks, so process the line normally,
c    but only IF next line is not the M-Tx line " /"
c
              read(10,'(a)')templine
              if (templine(1:2) .ne. ' /') then
                backspace(10)  ! and flow out
              else
c
c  We have the M-Tx case where one line ends with R? and next is " /". Add / to the line,
c    and proceed as above
c
                line = line(1:ib)//'/'  
                lenline = lenline+2
                termrpt = .true.
                termsym = line(ib-2:ib-1)
                go to 10
              end if
c +++ 060812
c
            else if (ia .eq. 1) then
              idxq = ntindex(line,'"')
              if (idxq.gt.idxa .and. idxq.lt.ib) then
c
c  Quote is between P and next blank.  Find 2nd quote, starting at the blank.
c
                idxq2 = ib-1+ntindex(line,'"')
                if (idxq.eq.0 .or. line(idxq2+1:idxq2+1).ne.' ') then
                  print*
                  print*,'Error copying P with quotes, idxq2:',idxq2
                  print*,line(1:60)
                  stop 1
                end if 
                ib = idxq2+1
              end if
c
c  Do not transfer P into parts.
c
              go to 12
            else if (ia .eq. 9) then
c
c  Start Saving a macro. After leaving here, a symbol will be sent to all parts,
c  If all on this line, set ib to end and exit normally.  
c
              ndxm = index(line(ib+1:128),'M')
              if (ndxm.gt.0 .and. line(ib+ndxm-1:ib+ndxm-1).eq.' ') then 
c
c  Macro ends on this line
c
                ib = ib+ndxm+1
              else
c
c  Save leading part of current line 
c
                lenhold = idxa-1
                if (lenhold .gt. 0) holdln = line(1:lenhold)
c
c  Transfer rest of line
c
                call allparts(line(idxa:128),129-idxa)
c
c  Read next line
c
20              read(10,'(a)')line
c
c  Check for comment, transfer and loop if so
c
c                if (line(1:1) .eq.'%') then
23              if (line(1:1) .eq.'%') then
c                  call allparts(line,128)
c                  go to 20
                  call chkcom(line,goto999)
                  go to 23
                end if
c
c  Look for terminal ' M'  
c
                if (line(1:1) .eq. 'M') then
                  ndxm = 1
                else
                  ndxm = index(line,' M')
                  if (ndxm .gt. 0) ndxm = ndxm+1
                end if
                if (ndxm .gt. 0) then
c
c  Set parameters, exit normally (but later check for leading part of 1st line
c
                  idxa = 1
                  ib = ndxm+1
                else
c
c  No "M", transfer entire line, loop
c
                  call allparts(line,128)           
                  go to 20
                end if
              end if
            else if (ia .eq. 10) then
c
c  Do not transfer MIDI command into parts
c
              go to 12
            else if (ia .eq. 5) then
c
c  First check for "AS", but S may come after other "A" options
c
              idxS = index(line(idxa+1:ib),'S')
              if (idxS .gt. 0) then
c
c  Get rid of the string. First check if that's all there is in A.
c
                if (ib-idxa .eq. nv+2) go to 12
                line = line(1:idxa+idxS-1)//line(idxa+idxS+nv+1:ib)
              end if
c
c  Check for user-defined part file name. Must start in column 1 and have
c    AN[n]"userfilename".  
c    idxa is position of "A"
c    ib is position of the next blank after "A"
c    Don't allow any blanks in user
c             
              if (line(idxa+1:idxa+1) .ne. 'N') go to 9 ! bail out
              if (idxa .ne. 1) then
                print*
                print*,
     * 'You entered "AN..." somewhere beyond first column; stopping.'
                stop 1
              end if
c
c  pmxa already checked for valid one- or 2-digit number, so get it
c
              if (line(4:4) .eq. '"') then
c
c  Single digit instrument number
c
                read(line(3:3),'(i1)')iudpfn
                idxstartname = 5
              else		              
                read(line(3:4),'(i2)')iudpfn
                idxstartname = 6
              end if
              ludpfn(iudpfn) = index(line(idxstartname:128),'"')-1
              if (ludpfn(iudpfn) .lt. 0) then
                print*
                print*,'User-defined part file name must be in quotes'
                stop 1
              end if
              idxendname = idxstartname+ludpfn(iudpfn)-1
              udpfnq(iudpfn) = line(idxstartname:idxendname)
c
c  Get a new line!
c             
              go to 4
            else if (ia .eq. 8) then
c
c Key change/transposition.
c If not instrument specific, copy to all parts
c
              if (line(idxa+1:idxa+1).ne.'i') go to 9
c
c Instrument-wise key/transposition(s): Ki[nn][+/-][dd}[+/-][dd]...
c
              ibb = idxa+2    ! Starts on digit after 'i'
40            continue
              read(line(ibb:ibb),'(i1)')iiinst ! 1st digit of iinst
              itemp = (index('01234567890',line(ibb+1:ibb+1)))
              if (itemp.gt.0) then
                ibb = ibb+1
                iiinst = 10*iiinst+itemp-1
              end if
              ibb = ibb+1  ! now at first +/-. Need end of 2nd number
              itemp = index(line(ibb+1:ib),'i') ! Rel pos'n of next 'i' or 0
              if (itemp .gt. 0) then
                iend = ibb+itemp-1
              else
                iend = ib-1
              end if
              write(10+iorig(iiinst),'(a)')'Ki1'//line(ibb:iend)
              if (itemp .gt. 0) then
                ibb = iend+2
                go to 40
              end if
              go to 12  ! Remove K command from string, go to next ia
            end if
9           continue
            call allparts(line(idxa:ib-1),ib-idxa)
12          continue
c
c  Remove the string from line
c
            if (idxa .eq. 1) then
              line = line(ib:128)
            else
              line = line(1:idxa-1)//line(ib:128)
            end if 
            lenline = lenstr(line,128)
c
c  Loop if only blanks are left
c
            if (lenline .eq. 0) go to 4
c
c  Must check for multiple "I" commands, so go to just after start of ia loop 
c
            if (ia .eq. 10) go to 24
c
c  Tack on front part from 1st line of saved macro
c
            if (lenhold .gt. 0) then
              line = holdln(1:lenhold)//' '//line(1:lenline)
              lenhold = 0
            end if
          end if
5       continue
      end if
c
c  Now a special loop to deal with 'X'.  If it was %[n]X..., will have been 
c  copied into part [n] already.  If no "B" or "P", remove.  If "P", just 
c  remove the "P" so pmxa/b will process.  If "B". do nothing.
c
10    continue
      nchk = 1
13    ntinx = nchk-1+ntindex(line(nchk:),'X')
      if (ntinx .gt. nchk-1) then 
c
c  There is a non-TeX 'X' at ntinx.  Loop if neither 1st nor after a blank.
c
        if (ntinx.gt.1) then
          if (line(ntinx-1:ntinx-1).ne.' ') then
c
c  The X is not 1st char of PMX command.  Advance starting point, loop. 
c
            nchk = ntinx+1
            go to 13
          end if
        end if
c
c  We now know the X at ntinx starts a PMX command.  Find next blank
c
        ib = ntinx+index(line(ntinx+1:),' ')
c
c  There must be a blank to right of "X", so ib>ntinx
c
c        locp = nchk-1+index(line(nchk:ib),'P')
        locp = ntinx+index(line(ntinx+1:ib),'P')
c
c  Did not need to use ntindex because we already know bounds of PMX command. 
c
c        if (locp .gt. nchk-1) then
        if (locp .gt. ntinx) then
c
c  Strip out the 'P'
c
          templine = line(1:locp-1)
          line = templine(1:locp-1)//line(locp+1:lenline)
          lenline = lenline-1
          ib = ib-1
        end if
        if (index(line(ntinx:ib),':').gt.0 .or. 
     *      index(line(ntinx:ib),'S').gt.0 .or. 
     *      index(line(ntinx:ib),'B').gt.0 .or. locp.gt.ntinx) then
c
c  The X command is a shift, "Both", or "Part".  Do not remove.
c
          nchk = ib+1
          go to 13
        end if 
c
c  Remove the X command.
c
        if (ntinx .eq. 1) then
          if (ib .lt. lenline) then
            line = line(ib+1:lenline)
          else
c
c  line contains ONLY the "X" command, so get a new line
c
            go to 4
          end if
        else
          templine = line(1:ntinx-1)
          if (ib .lt. lenline) then
            line = templine(1:ntinx-1)//line(ib+1:lenline)
          else
            line = templine(1:ntinx-1)
          end if
        end if 
c
c  Recompute lenline
c
        lenline = lenstr(line,128)
c
c  Resume checking after location of removed command.
c       
        nchk = ntinx
        go to 13
      end if
c
c  End of loop for X-checks
c
      oneof2 = ntindex(line,'//') .gt. 0
      if (termrpt .and. botv(iv) .and. frstln .and.
     *             line(lenline:lenline).eq.'/') then
c
c  Must add a terminal repeat before the slash
c
        if (oneof2) lenline = lenline-1
        if (lenline.gt.1) write(10+iorig(iinst),'(a)')line(1:lenline-1)
        if (.not. oneof2) then
          line = termsym//' /'
          lenline = 4
        else
          line = termsym//' //'
          lenline = 5
        end if
      end if
      if (termrpt .and. frstln .and. line(lenline:lenline).eq.'/' .and. 
     *    iv.eq.nvnow) termrpt = .false.
      write(10+iorig(iinst),'(a)')line(1:lenline)
      if (oneof2) then
        frstln = .false.
      else if (.not.frstln) then
        frstln = .true.
      end if
c      if (ntindex(line,'/').gt.0 .and. index(line,'//').eq.0) then
      if (ntindex(line,'/').gt.0 .and. ntindex(line,'//').eq.0) then
        iv = 1+mod(iv,nvnow)
        iinst = instnum(iv)
      end if
      go to 4
999   continue
      close(10)
c
c  In the mbrest checks, must run through ALL noinst files (not just noinow)
c
      do 11 iinst = 1 , noinst
ccc+++
cc
cc  Temporarily transfer entire scratch file to real file
cc
c        rewind(10+iinst)
c        open(40,file='s2pout'//char(48+iinst)//'.pmx')
c        do 50 m = 1 , 10000
c          read(10+iinst,'(a)',end=51)line
c          lenline = lenstr(line,128)
c          if (lenline .ge. 1) then
c            write(40,'(a)')line(1:lenline)
c          else
c            write(40,'(a)')' '
c          end if
c50      continue
c51      continue
c        close(40)
ccc+++
        if (nvi(iinst) .eq. 1) then
          call mbrests(iinst,jobname,ljob,ludpfn(iinst),udpfnq(iinst))
        else
c
c  Send a signal with ljob to bypass most mbrest processing
c
          call mbrests(iinst,jobname,-ljob,ludpfn(iinst),udpfnq(iinst))
        end if
11    continue
      end
      function lenstr(string,n)        
      character*(*) string
      do 1 lenstr = n , 1 , -1
        if (string(lenstr:lenstr) .ne. ' ') return
1     continue
      lenstr = 0
      return
      end 
      subroutine allparts(string,n)
      parameter (noimax=24)
      character*(*) string
      common /all/ noinow,iorig(noimax),noinst,insetup,replacing,
     *       instnum(noimax),botv(noimax),nvi(noimax),nsyst,nvnow
      logical insetup,replacing,botv
      len = lenstr(string,n)
      if (len .eq. 0) then
        len = 1
        string = ' '
      end if
      do 1 iinst = 1 , noinow
        write(10+iorig(iinst),'(a)')string(1:len)
1     continue
      return
      end
      subroutine mbrests(iv,jobname,ljob,ludpfn,udpfnq)
      character*128 line(50),line1,udpfnq
      character*80 sym
      character*12 jobname
      character*3 wbrsym(2)
      character*2 partq
      character*1 sq
      logical wbrest,alldone,rpfirst,newmtr,type4
      type4 = .false.
      sq = char(92)
      alldone = .false.
      rewind(10+iv)
      if (iv .lt. 10) then
        partq(1:1) = char(48+iv)
        lpart = 1
      else
        lpart = 2
        if (iv .lt. 20) then
          partq = '1'//char(38+iv)
        else
          partq = '2'//char(28+iv)
        end if
      end if
c 130327
c      open(40,file=jobname(1:abs(ljob))//partq(1:lpart)//'.pmx')
      if (ludpfn .eq. 0) then
        open(40,file=jobname(1:abs(ljob))//partq(1:lpart)//'.pmx')
      else
        open(40,file=udpfnq(1:ludpfn)//'.pmx')
      end if
      do 10 i = 1 , 10000
        read(10+iv,'(a)')line(1)
        if (line(1)(1:1).eq.'%' .or. line(1)(1:3).eq.'---'
     *           .or. type4) then
          len = lenstr(line(1),128)
          if (len .gt. 0) then
            write(40,'(a)')line(1)(1:len)
          else
            write(40,'(a)')' '
          end if
          if (line(1)(1:3).eq.'---') type4 = .not.type4
        else
          go to 11
        end if
10    continue
      print*,'You should not be here in scor2prt.  Call Dr. Don'
      stop
11    continue
c
c  Finished reading opening type4 TeX and comments.  Next line to be read
c  will contain the first of the input numbers
c
      call dosetup(iv,line(1),mtrnum,mtrden)
      do 1 i = 1 , 10000
13      read(10+iv,'(a)',end=999)line(1)
7       len = lenstr(line(1),128)
c
c  Pass-through (and copy into part file) if instrumnet has >1 voice.
c
        if (ljob .lt. 0) go to 2
        if (index('TtTiTch+h-h l ',line(1)(1:2)) .gt. 0) then
c
c  Traps titles, instruments, composers, headers, lower strings.  Read 2 lines.
c
          write(40,'(a)')line(1)(1:len)
          read(10+iv,'(a)')line(1)
          len = lenstr(line(1),128)
          go to 2
        end if
        if (i.eq.1 .or. (i.gt.5.and.line(1)(1:1).eq.'m')) then
c 
c  Either just starting, or a new meter is defined.
c  NOTE! The above test may be bogus.
c
          if (line(1)(1:1) .eq. '%') then
            write(40,'(a)')line(1)(1:len)
            go to 13
          end if
          if (i .ne. 1) then
c
c  New meter. Check for slashes (new meter change syntax)
c
            idxs = index(line(1),'/')
            idxb = index(line(1),' ')
            newmtr = idxs.gt.0 .and. (idxb.eq.0 .or. idxs.lt.idxb)
            if (.not.newmtr) then
c
c  Old way, no slashes, uses 'o' for lonesome '1' 
c
              icden = 3
              if (line(1)(2:2) .eq. 'o') then
                mtrnum = 1
              else
                mtrnum = ichar(line(1)(2:2))-48
                if (mtrnum .eq. 1) then
                  icden = 4
                  mtrnum = 10+ichar(line(1)(3:3))-48
                end if
              end if
              mtrden = ichar(line(1)(icden:icden))-48
            else
c
c  New way with slashes: idxs is index of 1st slash!
c
              read(line(1)(2:idxs-1),'(i'//char(48+idxs-2)//')')mtrnum
	        idxb = index(line(1)(idxs+1:),'/')
              read(line(1)(idxs+1:idxs+idxb-1),
     *          		'(i'//char(48+idxb-1)//')')mtrden
            end if
          end if
		lenbeat = ifnodur(mtrden,'x')
          lenmult = 1
          if (mtrden .eq. 2) then
            lenbeat = 16
            lenmult = 2
          end if
          lenbar = lenmult*mtrnum*lenbeat
          call fwbrsym(lenbar,nwbrs,wbrsym,lwbrs)
        end if
c
c Finished setting up meter stuff and defining whole-bar rest symbols
c
        ip1 = 0
        line1 = line(1)
        do 3 iw = 0 , nwbrs
          if (iw .gt. 0) then
            idx = ntindex(line1,wbrsym(iw)(1:lwbrs))
            if (idx .gt. 0) then
c
c  Check for blank or shifted rest, discount it if it's there
c
              if (line1(idx+lwbrs:idx+lwbrs).ne.' ') idx = 0
            end if
          else
            idx = ntindex(line1,'rp')
c
c  Check for raised rest
c
            if (idx.gt.0) then
              if (line1(idx+2:idx+2).ne.' ') idx = 0
            end if
          end if
          if (idx .gt. 0) then
            if (ip1 .eq. 0) then
              ip1 = idx
            else
              ip1 = min(ip1,idx) ! Maybe allows e.g. r0 rp ...
            end if
          end if
3       continue
        if (i.lt.5 .or. line(1)(1:1).eq.'%' .or. line(1)(1:2).eq.sq//sq
     *       .or. ip1.eq.0) go to 2
c
c  Switch to multibar rest search mode!!!  Start forward in line(1)
c
        rpfirst = line1(ip1:ip1+1) .eq. 'rp'
        iline = 1
        nmbr = 1
        if (rpfirst) then
          lwbrsx = 2
        else
          lwbrsx = lwbrs
        end if
        ipe = ip1+lwbrsx-1   ! ip at end of 1st wbrsym
4       if (ipe .eq. len) then
c
c  Need a new line 
c
          iline = iline+1
6         read(10+iv,'(a)',end=998)line(iline)
          len = lenstr(line(iline),128)
          if (line(iline)(1:1).eq.'%' ) then
            write(40,'(a)')'% Following comment has been moved forward'
            write(40,'(a)')line(iline)(1:len)
            go to 6
          end if
          ipe = 0
          go to 4
998       continue
c
c  No more input left
c
      print*,'All done!'
          alldone = .true.
          ipe = 0
          iline = iline-1
          len = lenstr(line(iline),128)
          go to 4
        else
          if (alldone) then
            sym(1:1) = ' '
          else
c
c  ipe<len here, so it's ok to get a symbol
c
            call nextsym(line(iline),len,ipe,ipenew,sym,lsym)
          end if
c
c  Check for end of block or bar line symbol
c
          if (index('/|',sym(1:1)) .gt. 0) then
            ipe = ipenew
            go to 4
          else 
            wbrest = .false.
            if (alldone) go to 12
            do 5 iw = 1 , nwbrs
              wbrest = wbrest .or. sym(1:lsym).eq.wbrsym(iw)(1:lwbrs)
5           continue
            wbrest = wbrest .or. (sym(1:lsym).eq.'r'.and.lwbrs.eq.2) 
     *                .or. (sym(1:lsym).eq.'rd'.and.lwbrs.eq.3) 
     *                .or. (sym(1:lsym).eq.'rp') 
     *                .or. (sym(1:lsym).eq.'r' .and. rpfirst)
12          if (wbrest) then
              ipe = ipenew
              nmbr = nmbr+1
              go to 4
            else 
c
c  AHA! Failed prev. test, so last symbol was *not* mbr.
c  It must be saved, and its starting position is ipenew-lsym+1
c
              if (nmbr .gt. 1) then
c
c  Write stuff up to start of mbr
c
                if (ip1 .gt. 1) write(40,'(a)')line(1)(1:ip1-1)
c
c  Insert mbr symbol.  Always end with a slash just in case next sym must be 
c  at start of block.  May think this causes undefined octaves, but
c  probably not since it's a single voice.
c
                ndig = int(alog10(nmbr+.01))+1
      print*,'Inserting rm, iv,nmbr:',iv,nmbr
                write(40,'(a2,i'//char(48+ndig)//',a2)')'rm',nmbr,' /'
                if (alldone) go to 999
                ipc = ipenew-lsym+1
                line(1) = line(iline)(ipc:len)
              else
c
c  Write old stuff up to end of original lonesome wbr, save the rest.
c  4 cases:  (wbr /) , (wbr line-end) , (wbr followed by other non-/ symbols) ,
c      alldone.
c  In 1st 2 will have gotten some other lines, so write all up to one b4 last 
c  non-comment line; then revert to normal mode on that.  In 3rd case must 
c  split line.
c
                if (alldone) then
                  write(40,'(a)')line(1)(1:len)
                  go to 999
                else if (iline .gt. 1) then
                  do 9 il = 1 , iline-1
                    len = lenstr(line(il),128)
                    write(40,'(a)')line(il)(1:len)
9                 continue
                  line(1) = line(iline)
                else
c
c  Since iline = 1 the wbr is not the last sym, so must split
c
                  write(40,'(a)')line(1)(1:ip1+lwbrsx-1)
                  line(1) = line(1)(ip1+lwbrsx+1:len)
                end if
              end if
c
c  Exit multibar mode
c
              go to 7
            end if
          end if
        end if
2       continue
        if (len .gt. 0) then
          write(40,'(a)')line(1)(1:len)
        else
          write(40,'(a)')' '
        end if
1     continue
999   continue
      close(10+iv)
      close(40)
      return
      end
      function ifnodur(idur,dotq)
        character*1 dotq
        if (idur .eq. 6) then
          ifnodur=1
        else if (idur .eq. 3) then
          ifnodur=2
        else if (idur.eq.1 .or. idur.eq.16) then
          ifnodur=4 
        else if (idur .eq. 8) then
          ifnodur=8  
        else if (idur .eq. 4) then
          ifnodur=16 
        else if (idur .eq. 2) then
          ifnodur=32
        else if (idur .eq. 0) then
          ifnodur=64
        else 
          print*,'You entered an invalid note-length value'
          stop
        end if
        if (dotq .eq. 'd') ifnodur = ifnodur*3/2
      return
      end
      subroutine fwbrsym(lenbar,nwbrs,wbrsym,lwbrs)
      character*3 wbrsym(2)
        nwbrs = 1
        lwbrs = 2
        if (lenbar .eq. 16) then
          wbrsym(1) = 'r4'
        else if (lenbar .eq. 32) then
          wbrsym(1) = 'r2'
        else if (lenbar .eq. 64) then
          wbrsym(1) = 'r0'
        else if (lenbar .eq. 8) then
          wbrsym(1) = 'r8'
        else if (lenbar .eq. 128) then
          wbrsym(1) = 'r9'
        else
          nwbrs = 2
          lwbrs = 3
          if (lenbar .eq. 24) then
            wbrsym(1) = 'rd4'
            wbrsym(2) = 'r4d'
          else if (lenbar .eq. 48) then
            wbrsym(1) = 'rd2'
            wbrsym(2) = 'r2d'
          else if (lenbar .eq. 96) then
            wbrsym(1) = 'rd0'
            wbrsym(2) = 'r0d'
          else
            write(*,'(33H Any whole-bar rests of duration ,i3,
     *        26H/64 will not be recognized)') lenbar
          end if
        end if
      return
      end           
      subroutine nextsym(line,len,ipeold,ipenew,sym,lsym)
c
c  Know its the last symbol if on return ipenew = len!.  So should never 
c    be called when ipstart=len.
c
        character*128 line
        character*80 sym
        if (ipeold .ge. len) then
          print*,'Called nextsym with ipstart>=len '
          print*,'Send files to Dr. Don at dsimons@logicon.com'
          stop
        end if
        do 1 ip = ipeold+1 , len
          if (line(ip:ip) .ne. ' ') then
c
c  symbol starts here (ip).  We're committed to exit the loop.
c
            if (ip .lt. len) then
              do 2 iip = ip+1 , len
                if (line(iip:iip) .ne. ' ') go to 2
c
c  iip is the space after the symbol
c
                ipenew = iip-1
                lsym = ipenew-ip+1
                sym = line(ip:ipenew)
                return
2             continue
c
c  Have len>=2 and ends on len
c
              ipenew = len
              lsym = ipenew-ip+1
              sym = line(ip:ipenew)
              return
            else 
c
c  ip = len
c
              ipenew = len
              lsym = 1
              sym = line(ip:ip)
              return
            end if 
          end if
1       continue        
      print*,'Error #3.  Send files to Dr. Don at dsimons@logicon.com'
      end                
      function ntindex(line,s2q)
c
c  Returns index(line,s2q) if NOT in TeX string, 0 otherwise
c
      character*(*) s2q
      character*128 line,tline
      logical intex
c
c     print*,'Starting ntindex.  s2q:',s2q,', line(1:79) is below'
c     print*,line(1:79)
c
c
c  Use a temporary string to store the input and test, so can zap D"..."
c
      tline = line
      ndxs2 = index(tline,s2q)
c
c  Return point below for rechecks after zapping D"  "
c
2     continue
      ndxbs = index(tline,char(92))
      if (ndxbs .gt. 0) then
c
c Special check in case \ is inside D"..."
c
        ndxDq1 = index(tline,'D"')
c
c If the following test fails, flow out of if block; else loop up to 2.
c
        if (ndxDq1 .gt. 0) then
c
c Find end of D"..."
c
          ndxDq2 = ndxDq1+1+index(tline(ndxDq1+2:128),'"')
          if (ndxDq2 .eq. ndxDq1+1) then
            print*,'Something is really wierd here'
            stop
          end if
          tline = tline(1:ndxDq1-1)
          do 3 ic = ndxDq1 , ndxDq2
            tline = tline(1:ic-1)//' '
3         continue
          tline = tline(1:ndxDq2)//line(ndxDq2+1:128)
          go to 2
        end if
      end if
      if (ndxbs.eq.0 .or. ndxs2.lt.ndxbs) then
        ntindex = ndxs2
c     print*,'No bs, or char is left of 1st bs, ntindex:',ntindex
      else
c
c  There are both bs and s2q, and bs is to the left of sq2. So check bs's to
c  right of first: End is '\ ', start is ' \' 
c
        len = lenstr(tline,128)
        intex = .true.
c     print*,'intex+>',intex
        do 1 ic = ndxbs+1 , len
          if (ic .eq. ndxs2) then
            if (intex) then
              ntindex = 0
              ndxs2 = index(tline(ic+1:len),s2q)+ic
c     print*,'ndxs2 =>',ndxs2
            else
              ntindex = ndxs2
              return
            end if
c     print*,'Internal exit, intex, ntindex:',intex,ntindex 
          else if (intex .and. tline(ic+1:ic+2).eq.char(92)//' ') then
            intex = .false.
c     print*,'intex+>',intex
          else if (.not.intex .and. tline(ic+1:ic+2).eq.' '//char(92)) 
     *           then
            intex = .true.
c     print*,'intex+>',intex
          end if
1       continue
c     print*,'Out end of loop 1'
      end if
c     print*,'Exiting ntindex at the end???'
      return
      end
      subroutine getchar(line,iccount,charq)
c
c  Gets the next character out of line*128.  If pointer iccount=128 on entry,
c  then reads in a new line.  Resets iccount to position of the new character.
c
      character*1 charq
      character*128 line
      if (iccount .eq. 128) then
        read(10,'(a)')line
        iccount = 0
      end if
      iccount = iccount+1
      charq = line(iccount:iccount)
      return
      end
      function readin(line,iccount,iread)
      parameter (noimax=24)
      common /all/ noinow,iorig(noimax),noinst,insetup,replacing,
     *       instnum(noimax),botv(noimax),nvi(noimax),nsyst,nvnow
      logical insetup,replacing,botv,goto999
c
c  Reads a piece of setup data from line, gets a new line from
c  file 10 (jobname.pmx) if needed, Transfers comment lines into all parts.
c
c  iread controls copying of values into scratch files for parts, but only
c  if not replacing.
c
c  iread input  value written 
c   -1   nvi      nothing (only used when noinst<0 initially)
c   0   various   value read
c   1    nv       -1 , replace later with nvi(i) 
c   2    noinst    1
c   3    np       -2 , replace later with (nsyst-1)/12+1
c   4  musicsize   20
c   5  fracondent  0.05
c
      character*128 line
      character*1 durq
4     if (iccount .eq. 128) then
        read(10,'(a)')line
        if (replacing) replacing = .false.
        call chkcom(line,goto999)
        iccount = 0
      end if
      iccount = iccount+1
c
c  Find next non-blank or end of line
c
      do 2 iccount = iccount , 127
        if (line(iccount:iccount) .ne. ' ') go to 3
2     continue
c
c  If here, need to get a new line
c
      iccount = 128
      go to 4
3     continue
c
c  iccount now points to start of number to read
c
      i1 = iccount
5     call getchar(line,iccount,durq)
c
c  Remember that getchar first increments iccount, *then* reads a character.
c
      if (index('0123456789.-',durq) .gt. 0) go to 5
      i2 = iccount-1
      if (i2 .lt. i1) then
        print*,'Found "'//durq//'" instead of number'
        stop 1
      end if
      icf = i2-i1+49
      read(line(i1:i2),'(f'//char(icf)//'.0)')readin
      if (.not.replacing) then
        if (iread .eq. 0) then
          call allparts(line(i1:i2),i2-i1+1) 
        else if (iread .eq. 1) then
          call allparts('-999',4)
        else if (iread .eq. 2) then
          call allparts('1',1)
        else if (iread .eq. 3) then
          call allparts('-998',4)
        else if (iread .eq. 4) then
          call allparts('20',2)
        else if (iread .eq. 5) then
          call allparts('.05',3)
        else if (iread .ne. -1) then
          print*,'Error with iread in readin'
          stop
        end if
      end if
      return
      end
      subroutine chkcom(line,goto999)
      parameter (noimax=24)
      common /all/ noinow,iorig(noimax),noinst,insetup,replacing,
     *       instnum(noimax),botv(noimax),nvi(noimax),nsyst,nvnow
      logical insetup,replacing,clefpend,botv,goto999
      character*128 line
c
c  Assume that line has just been read. No need to change iccount since we only
c  process full lines.
c
      goto999 = .false.
1     if (line(1:1) .ne. '%') return
c
c  If here, line has some sort of comment
c
      if (line(2:2) .eq. '%') then
        if (.not. insetup) then
c
c  Suck up a line, then flow out of "if" block to get another and loop
c
          read(10,'(a)')line

c++VV
c
c  UNLESS (a) it has a score-only "M" and changes # of inst's.
c
          if (index(line,'M') .gt. 0) then
            idxL = index(line,'L')
            idxM = index(line,'M')
            idxn = index(line,'n')
            idxb = index(line,' ')
            if (idxL.lt.idxM .and. idxM.lt.idxn .and. 
     *                (idxb.eq.0 .or. idxn.lt.idxb)) then
              noinow = ichar(line(idxn+1:idxn+1))-48
              clefpend = .true.
c
c  Next noinow digits are original inst. #'s of new inst. set.  Next noinow
c  char's after that are clefs
c
              nvnow = 0
              do 24 j = 1 , noinow
                iorig(j) = ichar(line(idxn+1+j:idxn+1+j))-48
                iposc0 = idxn+1+noinow
                do 25 k = 1 , nvi(iorig(j))
                  nvnow = nvnow+1
c                  clefq(nvnow) = line(iposc0+nvnow:iposc0+nvnow)
                  instnum(nvnow) = j
                  botv(nvnow) = k.eq.1 .and. j.ne.1
25              continue
24            continue
            end if
          end if
c
c  or if it's "h" or "l", need to suck up one more line
c
          if ((line(1:1).eq.'h'.and.index('+- ',line(2:2)).gt.0) .or.
c
c  4/29/00 check for T string also
c
     *         line(1:1).eq.'T' .or.   
     *         line(1:2).eq.'l ') read(10,'(a)')line
        else
c
c  In setup mode. Set flag, flow out and do use following line 
c
          replacing = .true.
        end if
      else if (line(2:2) .eq. '!') then
c
c  Copy to all parts        
c
        call allparts(line(3:128),125)
      else
c
c  Get value of hex integer 1,2,...,9,a,b,c in 2nd position, zero otherwise
cc  Get value of extended hex integer 1,2,...,9,a,b,c,...,o in 2nd position, zero otherwise
c
        ivq = index('123456789abcdefghijklmno',line(2:2))
c
c  Only treat as part-specific pmx line if number .le. noinst
c
        if (ivq.lt.1 .or. ivq.gt.noinst) then
c
c  Simple comment.  
c
          call allparts(line,128)
        else
c
c  Instrument comment, copy only to part
c
          lenline = lenstr(line,128)
          if (lenline .gt. 2) then
            write(10+ivq,'(a)')line(3:lenline)
          else 
c
c  Transferring blank line
c
            write(10+ivq,'(a)')' '
          end if
        end if
      end if
      read(10,'(a)',end=2)line
      call zapbl(line,128)
      go to 1
2     continue
      goto999 = .true.
      return
      end
      subroutine dosetup(iv,line,mtrnum,mtrden)
      parameter (noimax=24)
c
c  Transfers setup data from scratch file to real one for one instrument
c  Data may be mixed with comments, but on entry 1st item is a number.  
c  Write a comment when encountered, as it comes.
c  Write numbers one per line.
c  Three input data require special handling:
c    If not already replaced, i.e., if negative, then
c      iset(1) (nv) will be replaced with nvi(i)
c      iset(9) (npages) will be replaced with (nsyst-1)/12+1
c    iset(2), if negative, will be followed by extra numbers to be transf.
c
      common /all/ noinow,iorig(noimax),noinst,insetup,replacing,
     *       instnum(noimax),botv(noimax),nvi(noimax),nsyst,nvnow
      character*128 line
      logical insetup,replacing,botv
      iccount = 0
      do 1 iset = 1 , 12
        call partnum(iv,iccount,line,xdata)
        if (iset .eq. 2) then
          if (xdata .gt. 0) then
            write(40,'(i5)')int(xdata+.1)
          else
            noi = -xdata+.1
            write(40,'(i5)')noi
            do 2 ioi = 1 , noi
              call partnum(iv,iccount,line,xdata)
              write(40,'(i5)')int(xdata+.1)
2           continue
          end if
c        else if (iset.ne.8 .and. xdata.lt.0) then
        else if (iset.ne.8 .and. iset.ne.5 .and. xdata.lt.0) then
c
c  Must be either nv or npages
c
          if (int(-xdata+.1) .eq. 999) then
c
c  It's nv
c
            write(40,'(i5)')nvi(iv)
          else
c
c  npages must be computed
c
            write(40,'(i5)')(nsyst-1)/12+1
          end if
        else if (iset.ne.7 .and. iset.ne.12) then
c
c  write integer
c
          write(40,'(i5)')nint(xdata) 
        else
c
c  write floating number
c
          write(40,'(f5.2)')xdata
        end if
      if (iset .eq. 3) then
        mtrnum = nint(xdata)
      else if (iset .eq. 4) then
        mtrden = nint(xdata)
      end if
1     continue
      return
      end
      subroutine partnum(iv,iccount,line,xdata)
c
c  Simplified number parsing.  Only looks for comment lines and numbers.
c
      character*128 line
      character*1 durq
2     if (iccount .eq. 128) then
        read(10+iv,'(a)')line
        if (line(1:1) .eq. '%') then
          len = lenstr(line,128)
          write(40,'(a)')line(1:len)
          go to 2
        end if
        iccount = 0
      end if
      iccount = iccount+1
c
c  Find next non-blank or end of line
c
      do 4 iccount = iccount , 127
        if (line(iccount:iccount) .ne. ' ') go to 3
4     continue
c
c  If here, iccount=128 and need to get a new line
c
      go to 2
3     continue
c
c  iccount now points to start of number to read
c
      i1 = iccount
5     call getchar(line,iccount,durq)
c
c  Remember that getchar first increments iccount, *then* reads a character.
c
      if (index('0123456789.-',durq) .gt. 0) go to 5
      i2 = iccount-1
      if (i2 .lt. i1) then
        print*,'Found "'//durq//'" instead of number'
        stop 1
      end if
      icf = i2-i1+49
      read(line(i1:i2),'(f'//char(icf)//'.0)')xdata
      return
      end
      block data
      parameter (noimax=24)
      common /all/ noinow,iorig(noimax),noinst,insetup,replacing,
     *       instnum(noimax),botv(noimax),nvi(noimax),nsyst,nvnow
      logical insetup,replacing,botv
      data nvi /noimax*1/, noinst,noinow /2*noimax/
      data botv /noimax*.false./
      end
      subroutine zapbl(string,len)
      character*(*) string
      do 1 i = 1 , len
        if (string(i:i) .eq. ' ')go to 1
        if (i .eq. 1) return
        go to 2
1     continue
c
c  If line is all blank, leave it alone
c
      return
2     continue
      string = string(i:len)
      return
      end
      subroutine OddQuotesBefore(lineq,indx,yesodd)
c
c  This counts number of double quotes in lineq up to position indx-1, then
c    sets yesodd according to whether number is odd or even
c
      logical yesodd
      character*(*) lineq
      numdqs = 0
      do 1 i = 1 , indx-1
        if (lineq(i:i) .eq. '"') numdqs = numdqs+1
1     continue
      yesodd = mod(numdqs,2) .eq. 1
      return
      end





