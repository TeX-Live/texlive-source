      program pmxab
c
c This program, PMX, developed by Don Simons
c (dsimons@roadrunner.com), is a preprocessor for MusiXTeX. In concert with
c MusiXTeX and TeX, its purpose is to allow the user to create high-quality
c typeset musical scores by including a sequence of PMX commands in an ASCII
c input file.
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
c
      character*9 date
	character*5 version,versionc
	common /comver/ versionc
c
c To compile with gfortran:
c   1. Merge all files using copy *.for epmx[nnnn].for
c   2. Search and replace all character*65536 with character*131072
c   3. Comment/uncomment getarg lines
c   4. gfortran -O pmx[nnnn].for -o pmxab.exe
c
c To do
c   Correct Rainer's email address in manual
c   Linesplit (\\) in h
c   Tt at start of a movement.
c   Toggle midi on or off; allow midi only.
c   Page number printed on 1st page even if 1 system.
c   Still need inserted space for chordal accidentals
c   Voicewise transposition.
c   better segno
c   coda
c   duevolte
c   Fix xtup bracket direction in 2-line staves?? (maybe leave as is)
c   Sticky ornaments with shifts.
c   Deal with Werner's vertical spacing thing associated with title.
c   Multiple ties in midi
c   Werner's missing c in MIDI due to start/stop ties on same note.
c   Beams with single 64ths
c   128ths and/or dotted 64ths
c   Close out MIDI with integral # of beats?
c   Increase ast dimensions or redo logic.
c   Does slur direction get set for user-defined single-note stem dir'ns?
c   Transpose by sections.
c   Optimization loop over sections only
c   Command-line option to read nbarss in. Write out nbarss when optimizing.
c     (or just read from .plg?)
c   Beams over bar lines.
c   2-digit figures
c   A real coule (slanted line between notes in a chord)
c   Dotted slurs for grace notes.
c   Undotted chord notes with dotted main note.
c   Forced line break without line number
c   Fix dot moving when 2nds in chord get flipped
c   To do: increase length on notexq in dodyn
c 2.94
c   Allow moving dots on main and chord note in 2-note termolos
c   Add definitions of \hbp and \hbpp to pmx.tex
c 2.91
c   Tweak: insert \stdstemfalse before every user-shortened or lengthened stem. 
c     This cancels defaul extensions to middle line and also (with 'L') allows
c     stems that are longer than 4.66 but still don't reach middle line.
c   Allow alteration of number height in multibar rest, option n.
c   Fix bug introduced in 2.89 so that the "o" in "mo800" is now OK.
c 2.90
c   Many tweaks to allow gaps at end or middle of a system using 
c     L[n]S[x] and LC[y]. New pmx.tex.
c 2.88
c   Comment out print*,"Changed pathname to..." since it was going 
c     iteration when optimizing linebreaks with the undocumented option Ao.
c   Add nv back in as argument for getmidi, so loop transferring data
c     from midivel to midvelc can be executed. This corrects bug and
c     allows per-instrument change in midi velocities at the start
c     of any block
c   Add subroutine inst2chan to transfer midi data as noted above. Works with
c     Iv but buggy when used with IT and Ib
c 2.87
c   Allow changes to and from octave treble clef. Instrument with it
c     must only have one staff.
c   Some fixes for beam multiplicity changes at staff jumps. Must still
c     use inline for mult. increase at downward staff jump. See sjb.pmx.
c 2.84
c   Bug fix: dots in chordal seconds
c   Bug fix: Initialize ihornb for non-beamed, down xtup
c   Bug fix: When using "AT" with 2-note x3c rD. 
c 2.83
c   Fix problems when changing to or from octave treble clef.
c   Fix beaming (or not) and number location for xtups with
c     multiple rests. Still some problems with number height but
c     can be band-aided with number height tweak option.
c   Tweak error messages for options on "R" command.
c   Allow forced beam height and slope tweaks with 2-note tremolos
c   Allow chordal note with 2-note tremolos, adding dots as needed.
c   Fix call to ncmid in beamstrt when setting start height for beam of 
c     2-note trem, by changing arg from ivx to iv
c 2.82
c   bugfix: beams with rests, not confused with xtups.
c   x option after slur index to allow slurs to go from one voice to another.
c   x option also for ties
c 2.81
c   Allow string of rests to end xtup
c   in chordal half-note 2-note tremolo, make chord notes open.
c 2.80
c   Add 2-note tremolos
c 2.78
c   Expand bufq to 131072 bytes
c   Expand maxblks tp 9600
c   Allow up to 75 pages
c   Index postscript hairpins from 1 up instead of by voice #.
c   Add option 'o' to forced beam for open notehead (\hb); set ipl(3)
c   Add option T[n], n=1,2,3 for single stem tremolo
c 2.77
c   Enable AV+/-x+/-y to add vskip bigh before or after \eject
c 2.76
c 2.75
c   Bugfix: unbeamed xtups with dots: in beamn1 and beamid allow dotted 16th, and
c     2 or 3 flags on shortened note. 
c 2.74
c   Bugfix: allow "0" as argument of @ command on lyrics string.
c   Check for and allow "\ in centered page headings with P command.
c   Check for and allow "\ in text dynamics with D command.
c   For lyrics string starting in xtuplet, insert check for inputting musixlyr.
c   For staff-crossing beamed xtuplet chords, if 2nd segment of a joined beam
c     starts with a blank rest, put '\sk' into the TeX. 
c     To enable high-to-both beamed etup staff-crossing chord, for blank
c     rest at start of forced beam, shift \sk's from before to after \ib..;
c     so \ib is at start and note in upper voice has a beam to connect to.
c   Expand range of vertical xtup number shift, now stored in mult(16-22) 
c   Check for and allow \" within lyrics strings, for umlauts.
c 2.73 (160121)
c   Dirk's "..." command, to convert "text" into \pmxlyr{text}\ and insert as
c     inline TeX. Replace all '~' inside "..." and not preceded with '\', by 
c     '\lk '. Right after 2nd ", replace @[a,b][+,-]n with \at{[a,b][+,-]n}\
c     Include definitions of \ly and \at in pmx.tex (2 Feb 16). After first ",
c     add (as type 2 string) '\\input musixlyr \'
c   After inputting pathname, change any '\' to '/', for Linux compatibility.
c 2.72 (160110)
c   Really finalize \mbrest...go back to 3 args to deal with clef changes.
c   Fine tune centered whole bar rests to deal with clef changes
c   Fix bug in wsclef when >1 staff per instrument, since \setclef
c     needs to know instrument and specify clefs on all staves for that
c     instrument. Ones that were not changed by user will not be printed,
c     and will be kept the same as before.
c   Fix bug with arpegii from one voice to another in same staff.
c 2.71 (151226)
c   Finalize mbrest mods
c 2.705
c   Fix error message
c 2.704 (140614) 
c   Octave treble clef
c   Make horizontal ornament shift (ornhshft) floating
c 2.703 (140323)
c   Option v[-]n at end of any figure will alter figdrop for rest of system
c 2.702
c   Stem slurs. Only for ps, assume no other pos'n adjustments. Option "v"
c 2.701
c   oC = coda (\code{10})
c     Move iornq(29) (blank barline) to ipl(0) (changes in pmxb, getnote)
c   oG = new seqno
c   To do: fix grace note spacing problem (partially done)
c 2.622
c   Redefine midtc(..) and miditran(..); clean up all transpositions/key changes
c   Kn[+/-...] \ignorenats at signature changes
c   Fix tie checks in doslur() and dopsslur() to subtract iTransAmt from nolevs
c     before checking and setting pitch levels levson() and levsoff()
c   Define midisig separately from isig. Put in common commidisig.
c     Use for explicit midi signature and for accid corrections to midi piches
c     in addmidi.
c 2.621
c   Make keyboard rest option work in xtuplets. Created subroutine
c     chkkbdrests, modified make2bar to include calls to chkkbdrests as rqd.
c 2.620
c   Allow user-defined rest height tweaks at start of beam.
c 2.619
c   At movement break, change \nbinstruments in \newmovement macro; add
c     3rd arg to \newmovement in pmx.tex; modify pmxb.for and getnote.for
c     to remove call to newnoi and change call to \newmovement
c 2.618
c   Add option Ac[l,4] to set vert and horiz size and offsets to properly
c     center page for letter or a4 paper.
c 2.617
c   In g1etnote, change if-check for note to use index(...) instead
c     of ichar(charq) since it was messing up gfortran optimizer
c   After pmxa, search for and remove penultimate line <blank><blank>/
c     because it was screwing up linux-compiled versions
c   Bugfix: Increase dimension of kicrd from 7 to 10 in crdaccs(...)
c 2.616 (111110)
c   Allow hairpins to span multiple notes groups (gulps).
c 2.615+ (110810)
c   Fix midi when some instruments are transposed, by subtracting
c     iTransAmt(instno(iv)) from pitch values sent to via addmidi in
c     make2bar.for (for main notes) and docrd (for chord notes)
c 2.615 (110725)
c   Fig bug with size-setting (in topfile) when instrument has >1 staves 
c 2.615 (110724)
c   Make AS[0|-|s|t]... really set sizes
c 2.614
c   Mod notex.for to fix Terry's bug with raised dotted rests (caused 
c     by double-hboxing). 
c 2.613
c   Bugfix: In pmxa, change "do while" limit to keep from overwriting instno.
c 2.612
c   Enhance AS to allow s or t for smallsize or tinysize
c 2.611
c   Error trap for "D" before any notes in a block.
c 2.610
c   Instrument-wise key changes and transposition (incomplete)
c 2.603
c   101211 In getpmxmod.for, decreased nline by 2 to fix locating errors 
c     following reading in an include file.
c   101121 Added some error messages in g1etset.for setup data 
c 2.602
c   Correct slur indexing in linebreakslurs. 
c   Account for comment lines in line count for error messages
c 2.601
c   Bug fix: allow 24 slurs with graces
c 2.60 Changes made make it really big
c   increase mv (size of midi) ? Note: MIDI can't have >16 voices w/o 
c     major reprogramming, and 16 may also be a problem (icmm)
c   nm=24 (voices) done
c   24 slurs done
c   24 simultaneous beams (Replace index 24 by 0, so get 0-23)
c   bufq*131072 (gfortran only)
c   getarg syntax (gfortran only)
c 2.523+
c   Fix voice numbering for normal dynamics and text dynamics
c 2.523
c   Version of bigpmx first posted to Hiroaki's web site.
c 2.522
c   5/26/10 Replace ipl bits 0-7 with ipl2, add new common for it.
c   With 2.521+ as starting version, incorporate bigpmx mods to allow 24 voices.
c   5/13/10 Fix log2 function
c   5/15/10 Fix bitwise storage for dynamics, fix segnoo string length. 
c 2.521+
c   091025 Enable dotting 2nd part of linebreak slur or tie.
c   To adjust barno height due to linebreak slur, use \bnrs instead of
c     explicitly redefining \raisebarno (pmxb) 
c 2.521
c   Bugfix
c 2.520
c   090519 Enable ligfonts (special figured bass characters)
c 2.519
c   Fix another bug which kept \sk from being output so misaligned some notes.
c 2.518
c   Fix bugs: referencing fig data, char declaration for member of 
c      common/comfig/
c 2.517
c   Allow figures in voice 1 + any one other.
c 2.516
c   Allow figures in voice #2
c 2.515+ to do: Change manual and activate rule against clef change in voice #2.
c 2.515
c   071222 Changes in getnote to allow auto forced beams to start anywhere.
c   071206 In make2bar, shift fermataup7 to left over centered pause.
c   070901 In doslur, check for nolev <=2 in case slur ends on rest in 2-line
c       staff (it was screwing up vertical justification).
c   n34 for tweaks to midi durations of quarter note septuplets.
c   To do: In ref250.tex, the tables where 's,t,)' is explained, the line
c       [+,- i]   ... Vertical adjustment of the start of second segment
c     should it be replaced by
c       [s +,- i]   ... Vertical adjustment of the start of second segment
c 2.514
c   Changes in make2bar to get horizontal spacing right when normal grace
c     follows after grace
c   Changes in dograce to get octaves right for any material entered inside
c     \gaft, which shields transpose register changes from the outside world.
c 2.513
c   In make1bar, near end, for forced beams starting with a rest, copy hgt and
c     slope tweaks to ALL notes after first, not just the second one, so if
c     there's more than one rest at start the tweaks are still observed.
c   In beamid and beamend, add stand-alone triply-flagged notes for xtups.
c 2.512
c   Near end of pmxb, fix error trap to allow redundant 'RD'
c   Enable multiplicity down-up '][' within xtup.
c 2.511
c   Introduce eskz2 for xtup #'s and bracket lengths, to remove bug caused by 
c     adjusteskz as in bar 7 of barsant.
c 2.510a
c   Test: remove restriction on tempo changes in MIDI macros
c     Send to CM for beta testing.
c 2.509+
c   To do: Correct manual on AS. "-" is for smaller staves.
c 2.510
c   Forgot to declare litq, voltxtq as character in subroutine getgrace
c 2.509
c   Corrected small bug in arpeggio shifting (ivx <= iv in call putarp)
c 2.508
c   Allow graces in xtups. New subroutine getgrace. 
c 2.507
c   To do: Raise/lower figures.
c   To do: Add 24, 29 to list of musicsizes in manual
c   New sub adjusteskz to account for ask's when computing lengths of
c     brackets for unbeamed xtups, slopes and horizontal posn's of number 
c   Bug fix: in beamn1, beamid, and beamend, allow unbeamed xtups w/ 2 flags
c   Add look-left option for keyboard rests, "L" in rest command, set iornq(30)
c 2.506
c   Fix bug with AK, when simultaneous rests have same duration, use defaults.
c 2.505
c   Keyboard rests AK
c 2.504
c   Space after normal grace: option X[n]
c   Fixed og when nv .ne. noinst, by using sepsymq instead of '&'
c   (To do) length of xtup bracket when there is added non-collision space.
c   Trap musicsize if .ne. 16,20,24,29.
c 2.503
c   Enable arpeggio left shift with ?-x
c   To do: In manual, arpeggio over 2 staves.
c   Allow musicsize of 24 and 29. Had to define meter font size explicitly,
c     also change font size for text dynamics, but not much else so far.
c   Bugfix in beamstrt, introduced in 2415, ip was changed before putxtn
c     was called, causing error in printing replacement number.
c 2.502
c   Incorporate Dirk Laurie's patch to use { , } , for ties.
c   Figure height adjustment: append +[n]
c   Change ec font stuff in pmx.tex per Olivier Vogel's comment (CM email?)
c 2.501
c   Readjust horizontal offset back to .8 in LineBreakTies
c   Fix zero-subscript (iudorn) in putorn
c 2.50
c   Increase number of text-dynamics (dimension of txtdynq) per block 
c     from 12 to 41.
c   Slur option n to override altered default curvature.
c   Allow default ps slur curvature tweaks with Ap+/-c
c 2.416
c   Increase length of textdynq from 24 to 128
c   (Todo) Add comment in manual about blank lines at end.
c   Configuration file: Define subroutine getpmxmod, check path in environment
c     variable pmxmoddir, check existence, read lines into bufq after setup. 
c   Increase dimension on idynn in dodyn from 4 to 10 for max number 
c     of marks in a bar
c   Increase allowable # of lines from 2000 to 4000.
c   (To do) Replace definition of \liftpausc per Olivier.
c   (To do) Fix extraneous error message if RD is placed at very end.
c 2.415
c   Fix "AT" option: replace putxtn,topfile,beamstrt,beamid to use \xnumt
c     instead of redefining \xnum. Change font used to \smallfont (as for
c     normal xtups,
c   Allow slur to start on rest.
c 2.414
c   Correct bug in crdacc when adding accidental to boundary causes number of
c     segments to decrease
c   Special rule for 3-accidental chords: If no 2nds, place them in order
c     top, bottom, middle.
c 2.413
c   Correct bugs in chordal accidentals, related to left-shifted noteheads
c     (a) Special problems with downstem when main note needs shifting
c     (b) Assign 0 rank to boundary segs due to left-shifted noteheads
c 2.412
c   Change default horiz shift of start of seg 2 of linebreak slurs:
c     -.7 for slurs, -1.2 for ties,
c   Use height of start of seg 1 slur itself for end of 1 and start of 2.
c 2.411
c   "Apl" activates special treatment of linebreak slur/tie's; breaks all in 2.
c   "s" option in start of slur/tie as precursor to vert/horiz tweaks for end
c      of seg 1. of linebreak slur/tie, 2nd "s" for start of seg2.
c   With "Apl", curvature adjustments on starting slur command apply to seg 1, 
c      those on ending command to seg 2.
c 2.410
c   "AT" to allow Col. S.'s tuplet option. Simply input tuplet.tex and redefine
c      \xnum, \unbkt, \ovbkt. 
c   "s" option in main xtup input after "x": slope tweak for bracket. mult(4) is
c      flag, mult(5-9) is tweak value+16
c 2.409
c   Bugfix in docrd for MIDI: Use original pitch in case main/chord were
c     switched due to 2nds.
c   Remove "o" from error message for "A" command.
c   New syntax: optional instrument number separator ":" in movement 
c     break command to precede a 2-digit instrument. 
c   Conditional output formats for \setname at movement break to allow 
c     instrument numbers >9.
c   Bugfix in coding to raise barno due to slur over line break (pmxb)
c   Move date/version data statement in pmxab to a better place.
c 2.408
c   Allow pnotes{x} when x>9.995 (mod is only to format stmt in make2bar).
c   Bug fix in \liftPAusep in notex.for and in pmx.tex
c   Character variables for version and date
c   For up-stem single graces slurred to down-stem, shift slur start left by 
c     0.8 so slur doesn't get too short.
c   Initialize and slide mult, same as other full-program variables in /all/.
c 2.407
c   Allow AN[n]"[partname]" to be parsed by scor2prt as filename for part n,
c 2.406
c   Alter PMX: put \dnstrut into \znotes in \starteq (for system spacing
c     equalization).
c   Put dimensions of double sharps and flats in crdacc (for chords).
c   Bugfix: Use sepsymq in LineBreakTies(..) instead of '&'
c   Use only first 4 bits of mult for multiplicity+8, so rest can be used
c     for other stuff.
c   Move stemlength stuff in nacc(27-30) to mult(27-30) to remove conflict.
c 2.405: Not published but saved for safety.
c   Option Aph to write \special{header=psslurs.pro} top of each page, so
c     dviselec will work OK.
c 2.404
c   Allow slur to end on rest, but not start on a rest.  Efaults height 
c     of ending is default height of start (before any automatic or user-
c     defined djustments). User may adjust height as normal from default.
c 2.403
c   Bugfix: turn off repeated beaming patterns.at end of non-last voice.
c 2.402
c   Automatic repeated forced beams.  Start with "[:"  End with next explicit 
c     forced beam or end of input block.
c   Increase # of forced beams per line of music per input block from 20 to 40
c 2.401
c   Optional K-Postscript Linebreak Ties, Apl. New subroutine LineBreakTies.
c     Makes 1st part normal shape, and starts 2nd part a little further left.
c   Enable arpeggios in xtuplets.  Had to make time itar(narp) a real.
c 2.40
c   Set up WrotePsslurDefaults (logical) so only write defaults on 1st Ap.
c   Fix non-ps-slur input to \midslur (third signed integer). Do not reverse
c     sign for down-slurs.
c 2.359
c   Add error exit subroutine stop1 to make exit status g77-compatible..
c   Absolute octave on xtup chord note was 2 octave too high, fixed in getnote
c   Fermata on vertically shifted rest: special trap in putorn() to set height.
c   Correct multiple grace note spacing for small staves (in dograce, 
c        define wheadpt1 depending on staff size)
c 2.358
c   Allow curvature corrections at start of postscript slur, in dopsslur()
c   Local slur options p[+|-][s|t] for [nos|s]luradjust,[not|t]ieadjust
c   Options for [Nos|S]luradjust,[Not|T]ieadjust,[noh|h]alfties: Ap[+|-][s|t|h]
c   Make t[ID] act like s[ID]t, most mods in spsslur().
c   Add spsslur() to read in data for ps slurs, call from getnote.
c   In beamstrt, save args for SetupB in common comipb to save them for 
c      2nd call when xtup starts with rest
c   Add spacing for ornament ")" as if it were accidental, in make2bar().
c   Horiz shift start and end of ps ties, dep. on stem dir'n, in dopsslur()
c   Horiz. shift start of ps grace slur, 2 places in dograce().
c   Horiz shift end of grace slur in endslur()
c   Make st slurs into postscript ties.  Separate subroutine dopsslur(),    
c   Non-beamed xtup: "a" in 1st note or rest, before "x" (sets drawbm=.false.)
c   Allow two D"x" on same note. Introduced jtxtdyn1 in dodyn.
c 2.357a
c   Fix missing "end" in backfill.com, too-long lines in g1etnote, getnote
c 2.357
c   Increase dimension for # of lit TeX strings from 52 to 83.
c   Allow blank rest in middle of xtuplet. Only mods in g*etnote().
c 2.356
c   Increased some dimensions from 30 to 40 to allow up to 40 pages.
c   In unbeamed xtups, "n" did not suppress bracket.  Fixed in beamstrt().
c   Fix parsing of "f,h,H,HH" in sslur.
c   Fix bug with cdot, note-level for slur termination (in getnote)
c 2.355
c   Midi transposition:  IT[+|-][n1][+|-][n2]...[+|-][n(noinst)], 
c      n=# of half-steps.  Restrict to mult. of 12 now, to avoid key-sig issues
c   Make midi recognize ps ties in doslur.
c   Correct ttieforsl so that it eats 2nd argument properly, using \zcharnote 
c      to get octave right.
c 2.354
c   With postscript slurs, make t-slurs real ties by inserting replacement 
c     macros \tieforisu, etc, defined in pmx.tex
c   Check for open cresc or decresc at end of input block, using list[de]cresc
c   Hairpin syntax conditional on postscript slurs. Backup to fill in start 
c     level, using new backfill(...).  Separate height tweaks for 
c     start and finish.
c 2.353
c   K-0+n to transpose by half step (rather than just change key)
c   Allow "rm[n]" when nv>1.  Require it in all parts. Just write a stack of 
c     \mbrest's
c   Enable "Rz"; define \setzalaligne in pmx.tex. Special treatment at end
c     of input block before movement break, and at start of block after 
c     movement break, using \newmovement rather than \setzalaligne, since
c     former already redefines \stoppiece. In second case, set rptfg2='z'.
c   Make clefq(nm) common between pmxb and getnote; change references in 
c     getnote at 'M' to array elements, setting all new clefs as you go.
c 2.352
c   Remove \parskip redefinition from pmx.tex; write it into TeX file when
c     "Ae" is invoked.
c   Ap to activate postscript slurs. Add macro \psforts to pmx.tex to redefine 
c     \tslur in case \midslur was used.  Allow slur inputs 'f','h','H','HH',
c     translate them thru mapping to (1,4,5,6) as \midslur params, then let
c     \psforts translate them back to ps slur macors.
c 2.351
c   Number slurs from 0 up instead of 11 down, anticipating postscript slurs.
c   Write "\eightrm" instead of "\cmr8" for \figfont with small baseline size.
c   Increase length of basenameq to 44 characters everywhere.
c   Increase dimension of mcpitch (midi-chord-pitch) to 20.
c   Set default systems per page to 1 if nv>7
c   In pmxb, move place where isystpg is reset to 0, so that \eject gets
c     written when there is just one system per page.
c 2.35
c   Cautionary accidentals with 'c' anywhere in note symbol.
c   NEW pmx.tex with \resetsize to set size to normal or small depending on 
c     current \internote.  Used with new coding in dograce() to get right
c     new size in case user has \setsize'ed some lines to \smallvalue. For
c     \smallvalue-sized staves, redefine \tinynotesize to give 11-pt font. 
c     Affects pmx.tex.
c   Continuation figure with fractional length. May now mix with other figures.
c     If another figure follow Cont-fig, separate with colon.   
c 2.342
c   Bugfix in getnote to recognize relative octave shift in grace at start of  
c     input block.
c   In make2bar, initialize islhgt=0 earlier than before (possible solution
c     to Suse g77 compile problem that I could not reproduce)..
c   Bugfix in beamstrt & beamn1 for r2x6 c4D d d d
c 2.341
c   Syntax check: Forced page break page number must be > than previous.
c   Bugfix: Define ivx when "sliding down" breath/caesure data in pmxb.
c 2.34
c   New pmx.tex with redefined liftpausc
c   Bug fix with dotted, non-beamed xtups.
c 2.332
c   Fix bugs in horizonal shifts, spacing, for accid's, graces, noteheads.
c   Allow arbitrary pos. input to W in g1etnote and getnote.
c 2.331
c   Bug-fix in dodyn(..): typo on length of arg of txtdyn
c 2.33
c   Caesura (oc), breath (ob).  Set iornq(28), store rest of data in ibcdata()
c 2.321
c   Rescale accidental shifts. Still use 7 bits but now map (0,127) 
c      onto (-1.,5.35)
c   Fix ihornb bug in dodyn, seen with dynamics on lower-voice non-beamed xtups
c 2.32 (Noticed after posting)
c   Prohibit "/" as figure.
c 2.32 (Posted)
c   Tidied up accidentals in chords, do spacing.
c   Still to do: 
c       check for "(" on chord notes in spacing algo
c       small accids
c       double accids
c       autoshift slurs
c 2.310
c   Extra call to precrd ahead of spacing chk, and single-note crd/acc
c      shifts seem OK, but not multiple.  crd/acc shifts not recorded 1st time. 
c 2.309
c   Alternate algo for accid shifts in chords.
c 2.308
c   Auto horiz. notehead shifting added to precrd. 
c 2.307
c   Auto shifting of multiple accidentals in chords.
c   "Ao" in main chord note to keep accidentals in order. Set nacc(28).
c   If there are any manual main or chord note shifts, then
c      If any manual shift is preceded by "A" then
c         1. Auto-shifting proceeds
c         2. "A"-shifts add to autoshifts
c         3. non-"A" shifts are ignored!
c      Else (>0 man shifts, none has "A")
c         No auto-ordering, No autoshifts, 
c      End if
c   End if 
c 2.306
c   Initialize legacy note level to middle C in case user forgets to set 
c     octave.
c   Shift xtup note?
c   Shift in elemskips rather than noteheads?
c 2.305
c   Stop pmxb from multiple endvolta's at start of new page.
c 2.304
c   "Sx" in a note means shorten stemlength by x \internotes.  "Sx:" turn on
c       for multiple notes in the voice, "S:" last shortened note.
c 2.303
c   vshrink stuff all OK? Description is in pmxb.  
c 2.302
c   Toggle vshrink with "Av". vshrink normally kicks in when \interstaff 
c     hits 20. This still needs work.
c   Add " /" to last line if last char is not % or /.
c 2.301
c   Check in beamn1 for single note before multiplicity down-up.
c   allow '.PMX' as well as '.pmx'
c 2.299
c   Correct typo in pmxb involving PMXbarnotrue.
c   Replacement printed number for xtup: Unsigned integer after 'n' after 'x'
c   Minor upgrade parsing xtuplet options 'x...'
c   Correct dimension of nxtinbm in make2bar.
c 2.298
c   Account for doubled xtup notes in subroutine getx (user-defined spaces), 
c     by adding ndoub as an argument..
c 2.297
c   Created and solved compiler problem.  Put drawbm(NM) in its own common.
c   Add new def'ns [\a|PA]usc, \lift[pa|PA]usc to pmx.tex, use them in make2bar
c     when \centerbar is used.
c   Modify \mbrest & \CenterBar in pmx.tex to use \volta@endcor etc.  Have PMX
c     use right 2nd and 3rd args for \mbrest when key, meter, or clef changes.
c 2.296
c   Correct printed numbers for forced beams with multiple xtups. For each beam
c     make list in setupb by voice of eloff (h-offset) and mtupv (printed #)
c   Increase lengths of jobname and infileq by 20 characters
c   Enable whole notes and breves as 1st or last note of xtup in beamn1 and
c     beamend, and wholes in beamid.
c 2.295
c   Midi balance Ib[n1]:[n2]:...[nn]
c   Single-slope beam groups [...]-[...]
c   Trap "i" unless after accidental (main notes, xtups, chord notes)
c 2.294
c   Unequal xtups with "D" to double a note in an xtup.
c   As above, "F" will (a) increase multiplicity by 1 for marked note and next
c     one and (b) add a dot to the first one. 
c   Fix bug with e.g. c84 [ .d e.f ] by checking whether forced beam is on 
c     when "." is encountered, then correcting beam start time.(end of getnote)
c   MIDI velocity (volume) set: Iv[n1]:[n2]:[n3]...
c 2.293
c   Check for single notes spanning bar lines.
c   Correct various bugs with staff-jumping beams. (1) for 2nd segment, vxtup 
c     must be set in make2bar since beamstrt is not called, fixing problem with
c     dot at end. (2) add ivjb2 to flag which voice has 2nd segment and fix 
c     problem when >2 staves.
c   Add nodur to args of dodyn, so can check if stemless and avoid height tweak
c   Correct bug in getdyn setting flag in idynda2(0) for manual horiz. tweak
c 2.292a
c   Undo syntax check for Type 2 or 3 TeX string starting in column 1. 
c     Meanwhile, Werner's problem with a mid-line Type 3 string has gone away?! 
c 2.292
c   Allow comments in xtuplets
c   Enable multiple octave jumps in grace notes.
c   Allow dynamics in xtuplets.
c   Fix bug in getdyn searching for end of text string (correct length of lineq
c     to 128)
c   Fix bug in dodyn, must ignore horiz. interaction tweak for 
c     user-text (idno = 0)
c   Syntax check for Type 2 or 3 TeX string starting in column 1 
c     (NOTE: later undone!)
c   Syntax check for page number > npages at forced line break.
c 2.291
c   Fix error in AS command (accid spacing for small systems), making only
c     one spec per staff, nv total.
c   Stop using MIDI channel 10
c 2.29
c   Fix error in console output format for # of bytes used in MIDI file.
c   Fix bug in dograce so no space is added between grace and main note when
c       there is a MIDI-only accidental.
c   Fix bug so oes?+4 works.  It was too ugly to explain. 
c     ...Different ways of storing accidental specs on input and output.
c   No longer zap \writezbarno in special situations.
c   Fix bug in dyntxt level on rest
c   Line spacing equalization.  Add macros \starteq, \endeq, \spread, etc.
c     Activate with Ae.  (Maybe later could input alternate values for
c     \upamt, \dnamt, \parskip).  Put \starteq on 1st note in voice 1 
c     in the page, and \endeq on 1st note of next-to-last line in page.
c 2.28
c   Flip direction of forced beam "[f..."
c   Fix beam numbering for staff jumping beams. Uses irest(23,24,29,30) 
c   Fix bug in sliding ip's for txtdyn's
c   In dyn's allow vert. offsets +/-64, horiz +/-25.6 (store in idnyda2(1-99)
c 2.27
c   Comment out lines in dodyn checking number of dynamic marks found.  Voice
c     order may not be monotonic if two lines on a staff.
c   Literal dynamic: D"[text]"
c 2.26
c   Allow hairpin start-stop on same note by disabling auto-tweaks in dodyn,
c     increasing dimension of idynn to 4 to allow 4 symbols on same note.
c   Increase voltxtq length from 10 to 20.
c   AS[-/0][-/0]...  to inform PMX that "-" voices are small, and rough 
c      accounting for ast's is done by defining effective headwidth 
c      whead1 in makebar2 to be 0.8*whead.  
c 2.25
c   Fix logic bug with sepsym's when # of instruments changes.
c   Slight increases in default offsets for hairpin starts after "p"
c 2.24
c   Hairpins D< or D> as toggle.
c   Many automatic position tweaks for letter-group dynamics and hairpins.
c 2.23
c   Continued rhythmic shortcuts: space followed by "." or ","
c 2.22
c   In call to doslur, change tno(...) to tnote(...).  This was only
c     used when checking to slurs per stem directions, and should have been
c     the note duration all along.
c   MIDI-only accidental, bit 17 in nacc, or 27 in icrdat. 
c       Use "i" anywhere in note symbol.
c 2.21
c   Increase from 20 to 30 dimensions for movement breaks and midi sections.
c   Fix out-of-order declarations per mutex comments
c   Add "Bad error" and "Kluging" messages to log file.
c 2.197
c   add /comips/ to save tie-check midi variables
c   For spacing of clef changes at start of input block, changed integer time
c     lastnodur to prevtn, so it works with xtups. Possible incompatibility!
c 2.196
c   Fix Ickbug with time check in ncmid()
c   Interchange \fermataup7 and \pausec to get proper alignment
c   Enable French violin clef "f",  number 7 in PMX, but 9 in MusiXTeX.
c   Add defn's of \hsp, \hspp to pmx.tex 
c   Fix pre-slurs on xtup chord notes.
c   Fixed raised PAuse, define \liftPAuse
c   Replace \zbreve\sk with \breve.
c   Made "1" work as mtrdenl by doubling it and mtrnuml.  BUT WAIT...what 
c     about "o" and 1 as shorthand for 16???? Search for "Kluge"
c   Added "vo" (voice) as MIDI instrument 55 
c   Allow 3-digit page numbers (search for "toppageno")
c   Fix bug caused by prior fix (cancelling accid after bar line was ignored).
c   Fix double accids in chords
c 2.194
c   Fix bug with accid/tie/barline/chord in addmidi by restructuring accid if 
c     block.
c   Add meter to MIDI file with every pause
c   Purify FORTRAN?
c 2.193
c   Increased # of in-line TeX strings from 36 to 52.
c   Fix entry of # of bytes in header of tempo/meter/key track to allow >255.
c 2.191
c   Event track: Tempos, meters, keys all together.  Data in comevent
c 2.15
c   Pretty good midi capability.  Still no attention to slurs on chord notes.
c 2.11
c   11 Dec 99 c   rm1
c   11 Dec 99 "oes?", "oe?"
c   11 Dec 99 Cancel slur horizontal tweaks with non-stemmed notes
c   11 Dec 99 Error message for shifted, repeated ornaments.
c 2.10 (Version 2.1)
c   Fix bug with lowdot and xtuplets
c 2.09
c   Fix bug with multiple ornament heights over beams, when one is . or _
c   Error message from pmxa if rest on last note of xtup.
c   Enable 12 slurs.
c   Reinstate multiple rests at start of xtup.
c 2.07
c   Combine consecutive type-1 TeX strings.
c   \midslur and \curve as 3rd signed digit in slur termination, + 2 opt.int's.
c   Fixed breve chord notes in docrd
c   Check irest(28) as well as vxtup when setting nodur for chord notes, since
c     vxtup isn't set until 1st *main* note in xtup
c   Vectorize nolev1, slope, ixrest.  Klug fix for xtups with variable spacing.
c 2.06+
c   Make deterministic the beam slope calculation when there are an even # of
c     slopes in list and middle two are equal magnitude but opposite sign.
c   pmxa Trap for "o:" before 1st note in block
c   Partial bug fix for 64th notes in xtuplets.
c   Make ixrest a vector, since with new time scheme may not finish xtup in
c     same notes block.
c   Increase max # of pages from 20 to 30 (dimensions of nsystp,..., in pmxb)
c 2.06
c   Account for changes in nv when computing \interstaff. Add a counter 
c     nistaff(iflb) = # of interstaff spaces per system = nv-1.  Set whenever
c     setting isysflb(iflb). Note nv can only change at a forced line break.  
c     Note also, iflb starts at 0!
c 2.05
c   Automatic start of new notes group with part 2 of staff-jump beam
c     In make1bar, set irest bit 29 of lowest-voice note at same time,
c     use as flag when making notes groups.
c   For now, remove dummy blank line at end...it zaps terminal repeats.
c 2.02
c   Fixed slur-counting bug for multiple, slurred, aftergraces.
c 2.01
c  Increase to ask(1400)
c  Increase max forced page breaks to 18
c  Define pausc for centered pause
c 2.0a
c  Insert dummy blank line at very end to handle input files w/o terminal CR-LF
c pmx03r
c   Option m[n] in S symbol to change musicsize (for parts)
c   Double dotted rests now work.
c   Write file name to log file
c   Check existence of input file
c   Allow 24-char jobname, may end with ".pmx"
c   Comment out time stuff
c   Replace 3-argument getarg with 2-argument + iargc
c   Fix bug with negative noinst due to nint<=int replacement
c   move lovation of iv in isdat1 to allow iv>7.
c   Set nm=12 
c pmx03q
c   replace int(x+.001) with nint(x)
c   Write TeX file name to screen and to pml.
c   Replace char(...) with chax(...) to sovle msdev bug.
c   Bug fix: macro terminations when M is on a line by itself.
c   Bug fix: don't accumulate space for XS in pmxa.
c   Streamline Macros: use pointers to bufq instead of scratch files
c pmx03p
c   Store input file in single character array bufq. 
c     lbuf(i)*2 is length of line i
c     ipbuf is position just before next line to be read.
c pmx03
c   Optimize read/writes
c pmx02
c   Fix line count (for errors) when there are saved macros
c pmx01
c   In optimize mode, open/close macros (Watch out for residual zz files!)
c   Command line input
c   Option Ao to optimize, otherwise normal processing
c
ccccccc
      parameter (nks=125,nm=24,mv=24576,maxblks=9600)
      character*128 lnholdq
      character*131072 bufq
      integer*2 lbuf(maxblks)
      common /comevent/ miditime,lasttime
      logical slmon,dbltie
      common /comslm/ levson(0:nm),levsoff(0:nm),imidso(0:nm),
     *       naccbl(0:nm),laccbl(0:nm,10),jaccbl(0:nm,10),nusebl,
     *       slmon(0:nm),dbltie
      integer*2 mmidi
      logical restpend,relacc,notmain,twoline,ismidi,crdacc
      common /commidi/ imidi(0:nm),trest(0:nm),mcpitch(20),mgap,
     *       iacclo(0:nm,6),iacchi(0:nm,6),midinst(nm),
     *       nmidcrd,midchan(nm,2),numchan,naccim(0:nm),
     *       laccim(0:nm,10),jaccim(0:nm,10),crdacc,notmain,
     *       restpend(0:nm),relacc,twoline(nm),ismidi,mmidi(0:nm,mv),
     *       debugmidi
      logical debugmidi
      common /commvel/ midivel(nm),midvelc(0:nm),midibal(nm),midbc(0:nm)
     *                ,miditran(nm),midtc(0:nm),noinst,iinsiv(nm)
      integer*2 iinsiv
      common /inbuff/ ipbuf,ilbuf,nlbuf,lbuf,bufq
      common /commus/ musize,whead20
      integer*4 nbars0(nks),nbars(nks),ipoe(nks),nbari(nks)
      real*4 poe0(nks),poe(nks)
      logical isfirst,optimize
      logical*4 fexist
      character*44 jobname
      character*47 infileq
      common /a1ll/ iv,ivxo(600),ipo(600),to(600),tno(600),nnl(nm),
     *   nv,ibar,mtrnuml,nodur(nm,200),lenbar,iccount,
     *   idum,itsofar(nm),nib(nm,15),nn(nm),
     *   rest(nm,200),lenbr0,lenbr1,firstline,newmeter
      logical rest,firstline,newmeter
      common /comdiag/ n69(0:nm),n34(0:nm)
      logical mmacrec,gottempo
      common /commmac/ mmacstrt(0:nm,20),mmacend(0:nm,20),immac,
     *       mmactime(20),nmidsec,msecstrt(0:nm,60),msecend(0:nm,60),
     *       mmacrec,gottempo
      common /truelinecount/ linewcom(20000)
c
c Added 130302 only to get nsperi from g1etnote, for use in midi setup
c
      common /c1omget/ lastchar,fbon,issegno,ihead,isheadr,nline,isvolt,
     *     fracindent,nsperi(nm),linesinpmxmod,line1pmxmod,lenbuf0
      logical lastchar,fbon,issegno,isheadr,isvolt
c
c  immac(i) is the index of i-th macro, i=1,nmac.  Also make a list containing
c   nmidsec  section starts and stops based on PLAYING macros (not recording).
c 
ccccccccccccccccccccccccc
c
	data date /'17 Mar 20'/
	data version /'2.94'/
c
ccccccccccccccccccccccccc
      data maxit,ncalls /200,0/
      data isfirst /.true./
c      itstart = mytime()
      versionc = version
c
c  Initialize midi parameters
c
      gottempo = .false.
      ismidi = .false.
      debugmidi = .false.
      relacc = .false.
      mmacrec = .false.
      nmidsec = 1
      mgap = 10
      miditime = 0
      lasttime = 0
      nmidcrd = 0
      nusebl = 0
      notmain = .false.
      do 3 ivx = 1 , nm
        twoline(ivx) = .false.
        midinst(ivx) = 6
        midivel(ivx) = 127
        midibal(ivx) = 64
        miditran(ivx) = 0
3     continue
      do 12 icm = 0 , nm
        imidi(icm) = 0
        restpend(icm) = .false.
        trest(icm) = 0.
        levson(icm) = 0
        levsoff(icm) = 0
        slmon(icm) = .false.
        naccbl(icm) = 0
        n69(icm) = 0
        n34(icm) = 0
        msecstrt(icm,1) = 1
12    continue
c
c  End of midi parameter initialization
c
      musize = 0
      optimize = .false.
      numargs = iargc()
      if (numargs .eq. 0) then
        print*,'You could have entered a jobname on the command line,'
        print*,'      but you may enter one now:'
        read(*,'(a)')jobname
        numargs = 1
      else 
c        call getarg(1,jobname,idum) ! May need to replace this w/ next line 
        call getarg(1,jobname) 
      end if
10    ljob = lenstr(jobname,44)
      if (ljob .gt. 44) then
        print*,'Jobname is too long. Try again.'
        call stop1()
      else if (ljob .eq. 0) then
        print*,'No was jobname entered. Try again.'
        call stop1()
      else if (numargs .eq. 2) then
        if (ljob.eq.2 .and. jobname(1:2).eq.'-o') then
          optimize = .true.
c          call getarg(2,jobname,idum) ! May need to replace this w/ next line 
          call getarg(2,jobname) 
          numargs = 1 
          go to 10
        else
          print*,'Illegal option on command line'
          call stop1()
        end if
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
        inquire(file=jobname(1:ljob)//'.PMX',EXIST=fexist)
        if (.not.fexist) then
          print*,'Cannot find file '//infileq
          call stop1()
        else
         infileq = jobname(1:ljob)//'.PMX'
        end if      
      end if
c
c  Open a log file
c
      open(15,file=jobname(1:ljob)//'.pml')
      call printl('This is PMX, Version '//version//', '//date)
      ljob4 = ljob
      call printl('Opening '//infileq)
      open(18,file=infileq)
c
c  Copy input file into common buffer 
c
      ipbuf = 0
      linewcom(1) = 1
      do 8 ilbuf = 1 , maxblks
        ncomments = 0
14      read(18,'(a)',end=9)lnholdq
        lbuf(ilbuf) = lenstr(lnholdq,128)
        if (lbuf(ilbuf) .eq. 0) then
c
c  Blank line.  Make it a single blank with length 1
c
          lbuf(ilbuf) = 1
          lnholdq = ' '
        end if
c
c  Now line has at least one non blank character. Check for comment
c  As of Version 260, do not copy comments into bufq
c  But need to count %'s for error messaging
c        if (lnholdq(1:1).eq.'%') go to 14           
        if (lnholdq(1:1).eq.'%') then
          ncomments = ncomments+1
          go to 14           
        end if
c
c  When here, have counted all preceding comments and have a real line
c
        if (ilbuf .gt. 1) then
          linewcom(ilbuf) = linewcom(ilbuf-1)+1+ncomments
        else
          linewcom(1) = 1+ncomments
        end if
        if (ipbuf+lbuf(ilbuf).gt.131072) then
          print*,'Too many characters in file, stopping'
          call stop1()
        end if
        bufq(ipbuf+1:ipbuf+lbuf(ilbuf)) = lnholdq
        ipbuf = ipbuf+lbuf(ilbuf)
8     continue
      call printl('Too many lines in input file')
      call stop1()
9     continue
c
c  Insert dummy line to handle input files w/o CR-LF at end.
c
      nlbuf = ilbuf-1
c      nlbuf = ilbuf
c      bufq(ipbuf+1:ipbuf+3) = ' / '
c      lbuf(nlbuf) = 3
      close(18)
      do 6 numit = 1 , maxit
        if (optimize) call printl('Starting an iteration')
c
c  When isfirst=.true., pmxa() generates linebreaks normally, output in nbars0.
c    Otherwise, nbars0 is the input
c  When islast=.false., pmxb only returns poe's, otherwise does whole job
c
        call pmxa(jobname,ljob4,isfirst,nsyst,nbars0,optimize)
        if (.not.optimize) then
          if (ismidi) then
c
c  This was moved here from writemidi 130302 to allow midivel,bal,tran, to be
c    set up here as functions of instrument rather than iv (staff).
c  Count up staves(iv,nv) vs instruments.  Store instr# for iv in iinsiv(iv)
c
            nstaves = 0
            ivt = 0
            do 16 iinst = 1 , nm
              nstaves = nstaves+nsperi(iinst)
              do 17 ivtt = 1 , nsperi(iinst)
                ivt = ivt+1
                iinsiv(ivt) = iinst
17            continue
              if (nstaves .eq. nv) go to 18
16          continue
            print*,'Screwup!'
            call stop1()
18          continue
c
c  Set up channel numbers for midi. 
c
            numchan = 0
            do 11 iv = nv , 1 , -1
              if (twoline(iv)) then
                midchan(iv,2) = numchan
                numchan = numchan+1
              end if
              midchan(iv,1) = numchan
              numchan = numchan+1
11          continue
c
c  numchan will now be the number of channels, but max channel # is numchan-1
c
c  Set up velocities, balances, and midi-transpositions
c
            do 13 iv = nv , 1 , -1
              if (twoline(iv)) then
c  130302 Make these functions of instrument rather than staff (iv)
c                midvelc(midchan(iv,2)) = midivel(iv)
c                midbc(midchan(iv,2)) = midibal(iv)
c                midtc(midchan(iv,2)) = miditran(iv)
                midvelc(midchan(iv,2)) = midivel(iinsiv(iv))
                midbc(midchan(iv,2)) = midibal(iinsiv(iv))
                midtc(midchan(iv,2)) = miditran(iinsiv(iv))
              end if
c              midvelc(midchan(iv,1)) = midivel(iv)
c              midbc(midchan(iv,1)) = midibal(iv)
c              midtc(midchan(iv,1)) = miditran(iv)
              midvelc(midchan(iv,1)) = midivel(iinsiv(iv))
              midbc(midchan(iv,1)) = midibal(iinsiv(iv))
              midtc(midchan(iv,1)) = miditran(iinsiv(iv))
13          continue
          end if
c
c  TEMPORARY!!!
c
      write(15,*)'nlbuf: ',nlbuf
      ip1 = 1
      do 10000 ilb = 1 , nlbuf
c        write(15,'(2i5,a40,3i5)')ilb,lbuf(ilb),
c     *     bufq(ip1:ip1+lbuf(ilb)-1),
c     *     (ichar(bufq(ip1+lbuf(ilb)-k:ip1+lbuf(ilb)-k)),
c     *      k=min(3,lbuf(ilb)),1,-1)
        ip1 = ip1+lbuf(ilb)
10000 continue
      iplast = ip1-1
c
c  Check to see if (1) last line is "<blank><blank>/" and (2) next to last
c    line is "/"
c
      if (bufq(iplast+1-lbuf(nlbuf):iplast) .eq. '  /') then
        if (bufq(iplast-lbuf(nlbuf):iplast-lbuf(nlbuf)).eq.'/') then
          print*,'Removing last line of "<blank><blank>/"'
          write(15,*)'Removing last line of "<blank><blank>/"'
          nlbuf = nlbuf-1
        end if
      end if
c
          call pmxb(.true.,poe0,ncalls,optimize)
          if (ismidi) then
c
c  Write midi file
c
            open(51,file=jobname(1:ljob)//'.mid')
            if (debugmidi) open(52,file=jobname(1:ljob)//'.dbm')
            call printl(' ')
            call printl('Writing '//jobname(1:ljob)//'.mid')
            call writemidi(jobname,ljob)
          end if
          close(15)
          stop
        end if
      write(15,*)'nlbuf: ',nlbuf
      ip1 = 1
        call pmxb(.false.,poe0,ncalls,optimize)
        call poestats(nsyst,poe0,poebar0,devnorm0)
c
c  Save initial deviation and line breaks for later comparison
c
        if (numit .eq. 1) then
          devpmx = devnorm0
          do 20 isys = 1 , nsyst 
            nbari(isys) = nbars0(isys)
20        continue
        end if
        call sortpoe(nsyst,poe0,ipoe)
        do 1 iupord = nsyst , 1 , -1
          isysu = ipoe(iupord)
          print*,'isysu=',isysu
          write(15,*)'isysu=',isysu
c
c  Skip if system isysu has poe0 < avg or isysd has poe0 > avg
c
          if (poe0(isysu).lt.poebar0) go to 1
          do 5 idnord = 1 , nsyst
            isysd = ipoe(idnord)
            if (isysu.eq.isysd .or. nbars0(isysd).eq.1
     *                     .or. poe0(isysd).gt.poebar0) go to 5
            do 2 isyst = 1 , nsyst
              nbars(isyst) = nbars0(isyst)
              if (isyst .eq. isysu) then
                nbars(isyst) = nbars(isyst)+1
              else if (isyst .eq. isysd) then
                nbars(isyst) = nbars(isyst)-1
              end if
2           continue
            call pmxa(jobname,ljob4,isfirst,nsyst,nbars,optimize)
            call pmxb(.false.,poe,ncalls,optimize)
            call poestats(nsyst,poe,poebar,devnorm)
            if (devnorm .lt. devnorm0) then
              devnorm0 = devnorm
              poebar0 = poebar
              do 4 isys = 1 , nsyst
                nbars0(isys) = nbars(isys)
                poe0(isys) = poe(isys)
4             continue
              print*,'Improved with iup,idown,devnorm:',
     *                 isysu,isysd,devnorm0
              write(15,*)'Improved with iup,idown,devnorm:',
     *                 isysu,isysd,devnorm0
              write(*,'(5x,20i3)')(nbars0(isys),isys=1,nsyst)
              write(15,'(5x,20i3)')(nbars0(isys),isys=1,nsyst)
              call sortpoe(nsyst,poe0,ipoe)
              go to 6
            end if
5         continue
1       continue
c
c  If we get here, must have gone thru all switches and found nothing better,
c  so done!
c
        go to 7
6     continue
7     continue
      print*,'Optimum located, numit:',numit,',  ncalls:',ncalls
      write(15,*)'Optimum located, numit:',numit,',  ncalls:',ncalls
      print*,'Final error:',devnorm0,', initial error:',devpmx
      write(15,*)'Final error:',devnorm0,', initial error:',devpmx
      print*,'Percentage improvement:',100.*(1-devnorm0/devpmx)
      write(15,*)'Percentage improvement:',100.*(1-devnorm0/devpmx)
      call printl('Initial bars/system:')
      write(*,'(5x,20i3)')(nbari(isys),isys=1,nsyst)
      write(15,'(5x,20i3)')(nbari(isys),isys=1,nsyst)
      call printl('Final bars/system:')
      write(*,'(5x,20i3)')(nbars0(isys),isys=1,nsyst)
      write(15,'(5x,20i3)')(nbars0(isys),isys=1,nsyst)
      call pmxa(jobname,ljob4,.false.,nsyst,nbars0,optimize)
      call pmxb(.true.,poe0,ncalls,optimize)
      close(15)
      end
      subroutine accsym(nacc,acsymq,lacc)
      character*3 acsymq
      iacc = iand(nacc,7)
      if (iacc .eq. 1) then
        acsymq = 'fl'
        lacc = 2
      else if (iacc .eq. 2) then
        acsymq = 'sh'
        lacc = 2
      else if (iacc .eq. 3) then
        acsymq = 'na'
        lacc = 2
      else if (iacc .eq. 5) then
        acsymq = 'dfl'
        lacc = 3
      else if (iacc .eq. 6) then
        acsymq = 'dsh'
        lacc = 3
      else
        print*,'bad accidental: ',iacc
      end if
      return
      end
      subroutine addask(taskn,waskn,elaskn,
     *                  fixednew,scaldold,tglp1,scfac,isudsp)
      parameter (nm=24)
      logical isudsp
      common /comas1/ naskb,task(40),wask(40),elask(40)
      common /comudsp/udsp(50),tudsp(50),nudsp,udoff(nm,20),nudoff(nm)
      common /comtol/ tol
      scoarg = scaldold*scfac
      if (isudsp) then
c
c  Find which udsp we're dealing with
c
        do 1 iudsp = 1 , nudsp
          if (abs(taskn+tglp1-tudsp(iudsp)) .lt. tol) go to 2
1       continue
        print*,'You should note BEEE here in addask!'
        call stop1()
2       continue
c
c  Fixednew and scaldold must not be changed, since udsp's are already included
c  in fsyst from pmxa, and udsp don't involve scaled space..
c
        if (naskb.gt.0 .and. abs(taskn-task(max(1,naskb))).lt.tol) then
c
c  Must add user-defined space to what's there already.
c
          wask(naskb) = wask(naskb)+udsp(iudsp)
        else
c
c  This place has no other space.
c
          naskb = naskb+1
          task(naskb) = taskn
          wask(naskb) = udsp(iudsp)
          elask(naskb) = 0.
        end if
      else
c 130330 start
      oldwask = 0.
      oldelask = 0.
c 130330 end
c
c  This is a normal space, no effect if smaller than existing space
c
        if (naskb.gt.0 .and. abs(taskn-task(max(1,naskb))).lt.tol) then
c
c  We already put in some space at this time
c  Check if new one needs more space than old one at same time
c
          if (waskn .gt. wask(naskb)) then
c
c 130330 We were double counting the larger space when it came 2nd
c Need to fix but don't see how yet. Assume times came in order and 
c that last naskb defined spaces that need updating
c            
            oldwask = wask(naskb)
            oldelask = elask(naskb)
c End of 130330 insertions
            naskb = naskb-1
          else
            return
          end if
        end if
        naskb = naskb+1
        task(naskb) = taskn
        wask(naskb) = waskn
        elask(naskb) = elaskn
c 130330 start
c        fixednew = fixednew+waskn
c        scaldold = scaldold+elaskn
        fixednew = fixednew+waskn-oldwask
        scaldold = scoarg+elaskn-oldelask
c 130330 end
      end if
      return
      end
      subroutine addblank(noteq,lnoten)
      character*8 noteq
      character*1 tchar
      tchar = noteq(1:1)
      noteq = ' '//tchar
      lnoten = 2
      return
      end
      subroutine addfb(nfb,iv,tnew,t1fb,t2fb,ulfbq,ifbadd)
      parameter (nm=24)
      integer nfb(nm)
      common /comtol/ tol
      real*4 t1fb(nm,20),t2fb(nm,20)
      character*1 ulfbq(nm,20)
      ifbadd = 1
      nfb(iv) = nfb(iv)+1
      do 1 ifb = nfb(iv)-1 , 1 , -1
        if (tnew .lt. t1fb(iv,ifb)-tol) then
          t1fb(iv,ifb+1) = t1fb(iv,ifb)
          t2fb(iv,ifb+1) = t2fb(iv,ifb)
          ulfbq(iv,ifb+1) = ulfbq(iv,ifb)
        else
          ifbadd = ifb+1
          go to 2
        end if
1     continue
2     continue
      t1fb(iv,ifbadd) = tnew
      ulfbq(iv,ifbadd) = 'x'
      return
      end
      subroutine addmidi(icm,nolev,iacc,midisig,time,rest,endrest)
c      subroutine addmidi(icm,nolev,iacc,isig,time,rest,endrest)
      parameter(nm=24,mv=24576)
      integer*2 mmidi,itk(25)
      integer*4 itiesav(5,100)
      character*1 notenumq
      logical endrest,eximacc,it1found
      logical rest
      logical restpend,relacc,notmain,twoline,ismidi,crdacc
      common /commidi/ imidi(0:nm),trest(0:nm),mcpitch(20),mgap,
     *       iacclo(0:nm,6),iacchi(0:nm,6),midinst(nm),
     *       nmidcrd,midchan(nm,2),numchan,naccim(0:nm),
     *       laccim(0:nm,10),jaccim(0:nm,10),crdacc,notmain,
     *       restpend(0:nm),relacc,twoline(nm),ismidi,mmidi(0:nm,mv),
     *       debugmidi
      logical debugmidi
      common /commvel/ midivel(nm),midvelc(0:nm),midibal(nm),midbc(0:nm)
     *                ,miditran(nm),midtc(0:nm),noinst,iinsiv(nm)
      integer*2 iinsiv
      logical slmon,dbltie
      common /comslm/ levson(0:nm),levsoff(0:nm),imidso(0:nm),
     *       naccbl(0:nm),laccbl(0:nm,10),jaccbl(0:nm,10),nusebl,
     *       slmon(0:nm),dbltie
      common /comevent/ miditime,lasttime
      common /comdiag/ n69(0:nm),n34(0:nm)
c      common /commidisig/ midisig(nm)
c
c  Following variables are local but must be saved.  I hope they are.
c  (3/18/00) With g77 they are not, so add a common block here.
c
c      integer*2 ipslon(0:nm),lusebl(10),jusebl(10),icmm(0:12)
      integer*2 ipslon(0:nm),lusebl(10),jusebl(10),icmm(0:15)
      common /comips/ ipslon,lusebl,jusebl
c      data icmm /0,1,2,3,4,5,6,7,8,10,11,12,13/
      data icmm /0,1,2,3,4,5,6,7,8,10,11,12,13,14,15,16/
c
c  Cancel out barline accidentals if there's a rest.
c
      if (rest) naccbl(icm) = 0
c
c  Special path to insert dummy rest at end of a section
c
      if (endrest) go to 20
c
      do 7 ion = 0 , nmidcrd
c
c  check if this is only to get pitch of a chord note
c
        if (notmain) go to 6
c
c  check for rest
c
        if (rest) then
c
c  Will not put in a note, but must update timing
c
          if (.not.restpend(icm)) then
c
c  First rest in sequence, save the time
c
            restpend(icm) = .true.
            trest(icm) = time
          else 
            trest(icm) = trest(icm)+time
          end if
c
c  Note: code checkers don't like the above due to calling addmidi(trest(icm))
c    but this only happens if rest at end of section (endrest=.true.) (called
c    from getmidi(), in which case these above lines are bypassed.
c
          call chkimidi(icm)
          return
        end if
c
c  time tics
c
        if (imidi(icm).gt.0 .and. ion.eq.0) then
          idur = mgap
        else
          idur = 0
        end if
        if (restpend(icm)) then
          restpend(icm) = .false.
          idur = idur+nint(15*trest(icm))
        end if
c
c  time to start of note
c
        idurvar = isetvarlen(idur,nby2on)
        if (nby2on .gt. 4) then
          print*,'You got >4 bytes, something is bogus.'
          call stop1()
        end if
        imidi(icm) = imidi(icm)+1
        do 2 i = 1 , nby2on
c
c  imidi points to cell before highest (leftmost) byte.  Start with lowest byte 
c    at far right, fill in backwards
c
          mmidi(icm,imidi(icm)+nby2on-i) = mod(idurvar,256)
          if (nby2on .gt. 1) idurvar = idurvar/256
2       continue
        imidi(icm) = imidi(icm)+nby2on-1
c
c  Note-on signal
c  
        imidi(icm) = imidi(icm)+1
        mmidi(icm,imidi(icm)) = 9*16+icmm(icm)
c
c  Entry point for chord note pitch determination
c
6       continue
c
c  Get midi pitch.  On chord iteration, only do this first time (main note),
c  since pitch was already computed for nonmain chord notes.
c
        if (ion .eq. 0) then
          ipsav = nolev*12./7+11
          ipsav0 = ipsav
          if (midisig .ne. 0) then
c
c  Adjust for signature 
c
            notenumq = char(48+mod(nolev,7))
            if (midisig.ge.index('4152630',notenumq)) then
              ipsav = ipsav+1
            else if (-midisig.ge.index('0362514',notenumq)) then
              ipsav = ipsav-1
            end if
          end if
c
c  Deal with accidentals.  
c
c  iacc   0   1   2   3   4   5   6   7
c effect  X   fl  sh  na  X  dfl dsh  X
c iashft  X   -1  1   0   X  -2   2   X
c
          jacc = 0
          eximacc = .false.
          if (iacc .gt. 0) then
c
c  Adjust key-sig-adjusted pitch for explicit accidental (and exit)
c
            jacc = iashft(iacc)
            eximacc = .true.
            if (.not.relacc) jacc = jacc+ipsav0-ipsav
c		   
c  (Above) Shift applies to diatonic pitch but will be added to adjusted one
c
          else if (naccim(icm) .gt. 0) then
c
c  Possible implicit accidental from earlier in the bar
c    Check for prior accid in this bar at this note level
c
            do 3 kacc = 1 , naccim(icm)
              if (laccim(icm,kacc) .eq. nolev) then
                jacc = jaccim(icm,kacc)
                eximacc = .true.
                if (.not.relacc) jacc = jacc+ipsav0-ipsav
                go to 4
              end if
3           continue
4           continue
          end if
c
c  Must split off the following if block from those above because chord
c  notes can cause naccim>0, forcing us to miss other chord note's
c  accross-bar-line accidental
c
          if (naccbl(icm).gt.0 .and. .not.eximacc) then
c
c  Possible carryover accid from prior bar (or prior same-pitch note).    
c
            do 21 kacc = 1 , naccbl(icm)
              if (laccbl(icm,kacc) .eq. nolev) then
                jacc = jaccbl(icm,kacc)
c
c  Since we are *using* the bar-line accid, must flag it to be saved for next.
c 
                nusebl = nusebl+1
                jusebl(nusebl) = jacc
                lusebl(nusebl) = nolev
                if (.not.relacc) jacc = jacc+ipsav0-ipsav
                go to 22
              end if
21          continue
22          continue
          end if
          ipsav = ipsav+jacc
        end if
        if (notmain) then
          mcpitch(nmidcrd) = ipsav
c
c  Save pitch for tie checks
c
          if (levson(icm).eq.nolev.and..not.slmon(icm)) 
     *           ipslon(icm) = ipsav
        else 
          imidi(icm) = imidi(icm)+1
          if (ion.eq.0) then
            mmidi(icm,imidi(icm)) = ipsav
            if (levson(icm).eq.nolev.and..not.slmon(icm)) 
     *           ipslon(icm) = ipsav
          else
            mmidi(icm,imidi(icm)) = mcpitch(ion)
          end if
        end if
        if (ion .eq. 0) then
c
c  Only record accids for non-chords, main chord note during chord iteration
c    and chordnotes on first call but not during iteration
c
          if (iacc.gt.0) then
c
c  Set marker for accidental for possible continuations later this bar
c    but first check and clear earlier ones on same note. 
c
            do 23 kacc = 1 , naccim(icm)
              if (laccim(icm,kacc) .eq. nolev) then
                do 24 macc = kacc , naccim(icm)-1
                  laccim(icm,macc) = laccim(icm,macc+1)
                  jaccim(icm,macc) = jaccim(icm,macc+1)
24              continue
                go to 25
              end if
23          continue
            go to 26
25          continue
            naccim(icm) = naccim(icm)-1
26          continue
c
c  Flag new accidental
c
            naccim(icm) = naccim(icm)+1
            laccim(icm,naccim(icm)) = nolev
            jaccim(icm,naccim(icm)) = iashft(iacc)
          end if
c
c  Bail if this is a chord note on the first call (from docrd)
c
          if (notmain) then
            call chkimidi(icm)
		  return
          end if
        end if
c
c  Vel
c
        imidi(icm) = imidi(icm)+1
        mmidi(icm,imidi(icm)) = midvelc(icm)
        call chkimidi(icm)
7     continue
c
c  For tie checks
c
      if (levson(icm).gt.0.and..not.slmon(icm)) imidso(icm) = imidi(icm)
c
c  Entry point for special rests at section ends (endrest=T)
c
20    continue
c
c  Now insert all the ends
c
      do 8 ioff = 0 , nmidcrd
        if (ioff .eq. 0) then
c
c  time to end
c
          idur1 = nint(15*time)    
          if (.not.endrest .or. miditime.eq.nint(15*trest(icm))) then 
            idur = idur1-mgap
          else
            idur = idur1
          end if
c
c  Deal with roundoff problems with 7-tuplets on half or quarters
c
          if (idur1 .eq. 69) then
            n69(icm) = n69(icm)+1
c            if (mod(n69(icm)+6,7) .gt. 3) idur = 58
            if (mod(n69(icm)+6,7) .gt. 3) idur = idur1-mgap-1
          else if (idur1 .eq. 34) then
            n34(icm) = n34(icm)+1
            if (mod(n34(icm)+6,7) .gt. 4) idur = idur1-mgap+1
          end if
          idurvar = isetvarlen(idur,nby2off)
          if (nby2off .gt. 4) then
            print*,'You got >4 bytes, something is bogus.'
            call stop1()
          end if
          imidi(icm) = imidi(icm)+1
          call chkimidi(icm)
          do 1 i = 1 , nby2off
            mmidi(icm,imidi(icm)+nby2off-i) = mod(idurvar,256)
            if (nby2off .gt. 1) idurvar = idurvar/256
1         continue
          imidi(icm) = imidi(icm)+nby2off-1
        else
c
c  Inserting end of chord note, delta time is 0
c
          imidi(icm) = imidi(icm)+1
          mmidi(icm,imidi(icm)) = 0
        end if
c
c  Note off
c
        imidi(icm) = imidi(icm)+1
        mmidi(icm,imidi(icm)) = 8*16+icmm(icm)
c
c  Pitch
c
        imidi(icm) = imidi(icm)+1
        if (ioff .eq. 0) then
          mmidi(icm,imidi(icm)) = ipsav
        else
          mmidi(icm,imidi(icm)) = mcpitch(ioff)
        end if
c
c  Vel
c
        imidi(icm) = imidi(icm)+1
        mmidi(icm,imidi(icm)) = 0
        call chkimidi(icm)
        if (endrest) then
          return
        end if
8     continue
      naccbl(icm) = nusebl
      if (nusebl .gt. 0) then
c
c  Fix tables of "bar-line" accids that are saved due to consecutive notes.
c
        do 30 kacc = 1 , nusebl
          laccbl(icm,kacc) = lusebl(kacc)
          jaccbl(icm,kacc) = jusebl(kacc)
30      continue 
        nusebl = 0
      end if
c
c  Begin tie checks
c
      if (slmon(icm)) then
c
c  Prior note had a slur start
c
        if (levson(icm).eq.levsoff(icm) .and. iacc.eq.0) then
c
c  We have a tie! (Assumed there would be no accidental on tie-ending note)
c  Make a list of times of all events back to the one starting at imidso+1, 
c    which is at or before where the tie started.  Ident tie start and stop by
c    comparing pitches.  Save the 4 pieces of data in itiesav(1...4,nsav4tie)
c    Store actual time in itiesav(5,nsav4tie), using itiesav(1,1) as initial 
c    time.
          nsav4tie = 0
          imidt = imidso(icm)
10        nsav4tie = nsav4tie+1
          itiesav(1,nsav4tie) = igetvarlen(mmidi,icm,imidt,nbytes)
          imidt = imidt+nbytes
          do 11 j = 1 , 3
            itiesav(j+1,nsav4tie) = mmidi(icm,imidt+j)
11        continue
          imidt = imidt+3
          if (nsav4tie .eq. 1) then
            itiesav(5,1) = itiesav(1,1)
          else
            itiesav(5,nsav4tie) = itiesav(1,nsav4tie)+
     *                              itiesav(5,nsav4tie-1)
          end if
          if (imidt .ne. imidi(icm)) go to 10
c
c  Find which two pitches agree with saved slur pitch.
c
          it1found = .false.
          do 12 it2 = 1 , nsav4tie
            if (itiesav(3,it2) .eq. ipslon(icm)) then
              if (it1found) go to 13
              it1 = it2
              it1found = .true.
            end if
12        continue
          call printl(
     *          'Program error, tied notes, send source to Dr. Don')
          it1 = nsav4tie+1
          it2 = nsav4tie+1
13        continue
c
c  List the positions we want to keep
c
          jsav = 0
          do 14 isav = 1 , nsav4tie
            if (isav.eq.it1 .or. isav.eq.it2) go to 14
            jsav = jsav+1
            itk(jsav) = isav
14        continue
          nsav4tie = nsav4tie-2
c
c  Now dump events it1 & it2, recompute times, restack mmidi.         
c
          imidi(icm) = imidso(icm)
          do 15 isav = 1 ,nsav4tie
            if (isav .eq. 1) then
              idurvar = isetvarlen(itiesav(5,itk(isav)),nbytes)
            else
              idurvar = isetvarlen(itiesav(5,itk(isav))-
     *                             itiesav(5,itk(isav-1)),nbytes)
            end if
            imidi(icm) = imidi(icm)+1
            do 16 i = 1 , nbytes
              mmidi(icm,imidi(icm)+nbytes-i) = mod(idurvar,256)
              if (nbytes .gt. 1) idurvar = idurvar/256
16          continue
            imidi(icm) = imidi(icm)+nbytes-1
            do 17 i = 2 , 4
              imidi(icm) = imidi(icm)+1
              mmidi(icm,imidi(icm)) = itiesav(i,itk(isav))
17          continue
15        continue
        end if
        slmon(icm) = .false.
        levsoff(icm) = 0
        if (.not.dbltie) levson(icm) = 0
      end if
      if (levson(icm).gt.0) slmon(icm) = .true.
      if (nmidcrd .gt. 0) nmidcrd = 0
      call chkimidi(icm)
      return
      end
      subroutine addstr(notexq,lnote,soutq,lsout)
      common /comlast/ islast,usevshrink
      logical islast,usevshrink
        character*(*) notexq
        character*80 soutq
        if (lsout+lnote .gt. 72) then
          if (islast) write(11,'(a)')soutq(1:lsout)//'%'
          lsout = 0
        end if
        if (lsout .gt. 0) then
          soutq = soutq(1:lsout)//notexq(1:lnote)
        else
          soutq = notexq(1:lnote)
        end if
        lsout = lsout+lnote
      return
      end
      subroutine adjusteskz(ib,istart,poenom)
c
c  For block ib, this adds accidental spaces to eskz, for use in getting
c  length of xtup bracket and slopes of brackets and beams.
c
      parameter (nm=24)
      common /comas1/ naskb,task(40),wask(40),elask(40)
      integer*4 istart(80)
      common /comnsp/ space(80),nb,prevtn(nm),
     *    flgndv(nm),flgndb,eskgnd,ptsgnd,ivmxsav(nm,2),nvmxsav(nm)
      common /all/ mult(nm,200),iv,nnl(nm),nv,ibar,
     *   ivxo(600),ipo(600),to(600),tno(600),tnote(600),eskz(nm,200),
     *   ipl(nm,200),ibm1(nm,9),ibm2(nm,9),nolev(nm,200),ibmcnt(nm),
     *   nodur(nm,200),jn,lenbar,iccount,nbars,itsofar(nm),nacc(nm,200),
     *   nib(nm,15),nn(nm),lenb0,lenb1,slfac,musicsize,stemmax,
     *   stemmin,stemlen,mtrnuml,mtrdenl,mtrnmp,mtrdnp,islur(nm,200),
     *   ifigdr(2,125),iline,figbass,figchk(2),firstgulp,irest(nm,200),
     *   iornq(nm,0:200),isdat1(202),isdat2(202),nsdat,isdat3(202),
     *   isdat4(202),beamon(nm),isfig(2,200),sepsymq(nm),sq,ulq(nm,9)
      character*1 ulq,sepsymq,sq
      logical beamon,firstgulp,figbass,figchk,isfig
      common /comeskz2/ eskz2(nm,200)
      common /comtol/ tol
      common /comntot/ ntot
      inmin = istart(ib)+1
      do 10 iaskb = 1 , naskb
        if (task(iaskb) .lt. to(istart(ib))-tol) go to 10
        eskadd = wask(iaskb)/poenom-elask(iaskb) 
        do 11 in = inmin , ntot 
          if (to(in) .gt. task(iaskb)-tol) then
            eskz2(ivxo(in),ipo(in)) = eskz2(ivxo(in),ipo(in))+eskadd
            if (abs(to(in)-task(iaskb)).lt. tol) inmin=inmin-1
          else
            inmin = inmin+1
          end if
11      continue
10    continue
      return
      end
      subroutine askfig(pathnameq,lpath,basenameq,lbase,figbass,istype0)
      logical figbass,ispoi,topmods,istype0,done,isbbm
      common /comhsp/ hpttot(176)
      common /compoi/ ispoi
      common /combbm/ isbbm
      common /comas3/ ask(2500),iask,topmods
      character*40 pathnameq
      character*44 basenameq
      character*1 sq,chax
      character*129 outq
      sq = chax(92)
      open(12,file=pathnameq(1:lpath)//basenameq(1:lbase)//'.tex')
c
c  Transfer first 5 lines of main internal TeX file
c
      do 11 il = 1 , 5
        call moveln(11,12,done)
11    continue
      if (istype0) then
c
c  Transfer literal TeX stuff from special scratch file
c
        rewind(17)
10      call moveln(17,12,done)
        if (.not.done) go to 10
        close(17)
      end if
c
c  Transfer next 2 lines from main scratch file
c
      do 3 il = 1 , 2
        call moveln(11,12,done)
3     continue
      if (ispoi) write(12,'(a)')sq//'input musixpoi'
      if (isbbm) write(12,'(a)')sq//'input musixbbm'
      if (figbass) then
c
c  Transfer .fig data from scratch (unit 14) into external .tex (unit 12)
c
4       call moveln(14,12,done)
        if (.not.done) go to 4
        close(14)
      end if
      iask = 0
      ihs = 0
1     read(11,'(a129)',end=999)outq
c
c  Hardspaces.
c
      if (outq(1:5) .eq. sq//'xard') then
        ihs = ihs+1
        outq(2:2) = 'h'
        write(outq(12:15),'(f4.1)')hpttot(ihs)
        lenout = 19
        go to 9
      end if
c
c  This part hard-wires ask's into new .tex file as ast's
c
2     indxask = index(outq,sq//'ask')
      if (indxask .ne. 0) then
        iask = iask+1
        call putast(ask(iask),indxask,outq)
        go to 2
      end if
      lenout = llen(outq,129)
9     continue
      write(12,'(a)')outq(1:lenout)
c
c  If this is the line with "readmod", check for topmods.
c
      if (topmods .and. outq(2:8).eq.'readmod') then
        topmods = .false.
        rewind(16)
        do 7 il = 1 , 1000
          read(16,'(a129)',end=8)outq
          lenout = llen(outq,129)
c
c  We inserted the '%' in subroutine littex, to guarantee including blank.
c
          write(12,'(a)')outq(1:lenout)
7       continue
8       continue
        close(16)
      end if
      go to 1
999   close(11)
      close(12)
      return
      end
      subroutine backfill(iunit,oldq,lenold,newq,lennew)
c
c  In iunit, looks backward for oldq, overwrites newq
c  Safest if both are same length!
c
      character*128 lineq(200),nowq
	character*(*) oldq,newq
      linesback = 0
1     continue
      backspace(iunit)
	read(iunit,'(a)')nowq
	ndx = index(nowq,oldq(1:lenold))
c
c  Save the line just read
c
      linesback = linesback+1
	lineq(linesback) = nowq
      if (ndx .eq. 0) then
        backspace(iunit)
	  go to 1
	end if
c
c  If here, it's replacement time.
c
      lineq(linesback) = nowq(1:ndx-1)//newq(1:lennew)
     *  //nowq(ndx+lenold:128)
      backspace(iunit)
      do 2 line = linesback , 1 , -1
        write(iunit,'(a128)')lineq(line)
2     continue
      return
      end
      subroutine beamend(notexq,lnote)
      parameter (nm=24)
      common /all/ mult(nm,200),iv,nnl(nm),nv,ibar,
     *   ivxo(600),ipo(600),to(600),tno(600),tnote(600),eskz(nm,200),
     *   ipl(nm,200),ibm1(nm,9),ibm2(nm,9),nolev(nm,200),ibmcnt(nm),
     *   nodur(nm,200),jn,lenbar,iccount,nbars,itsofar(nm),nacc(nm,200),
     *   nib(nm,15),nn(nm),lenb0,lenb1,slfac,musicsize,stemmax,
     *   stemmin,stemlen,mtrnuml,mtrdenl,mtrnmp,mtrdnp,islur(nm,200),
     *   ifigdr(2,125),iline,figbass,figchk(2),firstgulp,irest(nm,200),
     *   iornq(nm,0:200),isdat1(202),isdat2(202),nsdat,isdat3(202),
     *   isdat4(202),beamon(nm),isfig(2,200),sepsymq(nm),sq,ulq(nm,9)
      character*1 ulq,sepsymq,sq,ulqq,chax
      logical beamon,firstgulp,figbass,figchk,flipend,btest,
     *        isfig,vxtup,isdotm,isbjmp,isbj2,drawbm
      common /combjmp/ ivbj1,ivbj2,isbjmp,isbj2,multbj1
      common /comoct/ noctup
      common /comxtup/ ixtup,vxtup(nm),ntupv(nm,9),nolev1(nm),
     *                 mtupv(nm,9),nxtinbm(nm),
     *                 islope(nm),xelsk(24),eloff(nm,9),
     *                 nssb(nm),issb(nm),lev1ssb(nm,20)
      common /comdraw/ drawbm(nm)
      common /commvl/ nvmx(nm),ivmx(nm,2),ivx
      common /strtmid/ ihnum3,flipend(nm),ixrest(nm)
      character*4 tempq
      character*8 noteq
      character*79 notexq
      ip = ipo(jn)
      multip = iand(mult(ivx,ip),15)-8
      lnote = 0
      if (ixrest(ivx) .eq. 4) then
c
c  This is the LAST note in the xtup (i.e., all rests before).  Make single.
c
        nodur(ivx,ip) = 2**(4-multip)
        call notex(notexq,lnote)
        ixrest(ivx) = 0
        return
      end if
      nole = nolev(ivx,ip)
c
c  Check for special situations with 2nds (see precrd)
c
      if (btest(nacc(ivx,ip),30)) then
        nole = nole - 1
      else if (btest(nacc(ivx,ip),31)) then
        nole = nole + 1
      end if
c
c  Terminate indented beams for 2-note tremolo if needed
c
      if (btest(irest(ivx,ip-1),2) .and. 
     *            igetbits(irest(ivx,ip-1),2,5) .gt. 0) then
        nindent = igetbits(irest(ivx,ip-1),2,5)
        if (ulq(ivx,ibmcnt(ivx)) .eq. 'u') then
          addoff = -1-.5*nindent
        else
          addoff = 1+.5*nindent
        endif
c        addoff = addoff+(.595-.065*abs(islope(ivx)))*islope(ivx)
        addoff = addoff+.0822*islope(ivx)
        if (addoff .lt. -.05) then
          write(tempq,'(f4.1)')addoff
        else 
          write(tempq,'(f4.2)')addoff
        end if
        notexq = sq//'raise'//tempq(1:4)//sq//'internote'//sq//'hbox{'
     *        //sq//'loffset{.7}{'//sq//'tb'//ulq(ivx,ibmcnt(ivx))//
     *        '0}}'
        lnote = 46
      end if
      if (.not.drawbm(ivx)) then
c
c  Xtuplet with no beam, just put in the right kind of note
c
        if (btest(irest(ivx,ip),0)) then
c
c Rest at end of unbeamed xtup
c
          lnote = 3
          if (btest(islur(ivx,ip),29)) then
            notexq = sq//'sk'
c
c  180106 There was a problem with nolev(ivx,ip) not being set to 0 for
c  a blank rest ending xtup, but hopefully returning from here will handle it.
c
            return  
          else if (multip .eq. 0) then
            notexq = sq//'qp'
          else if (multip .eq. -1) then
            notexq = sq//'hp'
          else if (multip .eq. 1) then
            notexq = sq//'ds'
          else if (multip .eq. 2) then
            notexq = sq//'qs'
          else
            notexq = sq//'hs'
          end if
c
c 180106 Deal with possible level tweak
c
          nole = mod(nolev(ivx,ip)+50,100)-50
          if (nole .ne. 0) then
            if (abs(nole) .lt. 10) then
              noteq = chax(48+abs(nole))
              lnoten = 1
            else
              write(noteq(1:2),'(i2)')abs(nole)
              lnoten = 2
            end if
            if (nole .gt. 0) then
              notexq = sq//'raise'//noteq(1:lnoten)//sq//'internote'
     *                 //notexq(1:lnote)
            else
              notexq = sq//'lower'//noteq(1:lnoten)//sq//'internote'
     *                 //notexq(1:lnote)
            end if
            lnote = 16+lnoten+lnote
          end if            
          return
        end if
        if (btest(islur(ivx,ip),30)) then
c
c  Forced stem direction
c
          ndsav = nodur(ivx,ip)
          nodur(ivx,ip) = 2**(4-multip)
          if (btest(nacc(ivx,ip-1),27)) 
     *            nodur(ivx,ip)=nodur(ivx,ip)/2
          call notex(notexq,lnote)
          nodur(ivx,ip) = ndsav
        else
          call notefq(noteq,lnoten,nole,ncmid(iv,ip))
          if (lnoten .eq. 1) call addblank(noteq,lnoten)
c
c  To reduce confusion due to this early update of lnote, do it
c    below, separately in each case/
c          lnote = lnoten+3
          if (.not.btest(nacc(ivx,ip-1),27)) then
c
c  Prior note is not regular-dotted
c              
            if (btest(irest(ivx,ip-1),2) .and. 
     *         igetbits(irest(ivx,ip-1),2,5) .gt. 0) then
c
c  Unbeamed tremolo with indented beams. Put termination in right here
c
              nindent = igetbits(irest(ivx,ip-1),2,5)
              if (ulq(ivx,ibmcnt(ivx)) .eq. 'u') then
                addoff = -1-.5*nindent
              else
                addoff = 1+.5*nindent
              endif
c
c  Is there an islope here, for unbeamed?
c
              if (addoff .lt. -.05) then
                write(tempq,'(f4.1)')addoff
              else 
                write(tempq,'(f4.2)')addoff
              end if
              notexq = sq//'raise'//tempq(1:4)//sq//'internote'//sq//
     *         'hbox{'
     *         //sq//'loffset{.7}{'//sq//'tb'//ulq(ivx,ibmcnt(ivx))//
     *         '0}}'
              lnote = 46
            end if
            if (multip .eq. 0) then
              if (btest(irest(ivx,ip-1),2) .and.
     *                       nodur(ivx,ip).gt.24) then
c
c 2nd note of unbeamed half-note trem; make open
c But it's not clear if unbeamed half-note tremolo is Kosher,
c   so don't worry about stem lengths here now.
c
                if (lnote .eq. 0) then
                  notexq = sq//'h'//ulq(ivx,ibmcnt(ivx))//noteq
                else
                  notexq = notexq(1:46)//
     *                sq//'h'//ulq(ivx,ibmcnt(ivx))//noteq
                  lnote = 46
                end if
              else
                if (btest(irest(ivx,ip-1),2) .and.
     *               nodur(ivx,ip).eq.24 .or. nodur(ivx,ip).eq.12) then
c
c Need a dot.
c
                  lnote = 46
                  if (lnoten .eq. 1) then
                    noteq = ' '//noteq(1:1)
                    lnoten = 2
                  end if
c
c  Insert stemlength stuff here for unbeamed dotted tremolo. 
c  May later combine with below to avoid repeat. But need to
c    return to normal stem length after note is set.
c
                  nindent = igetbits(irest(ivx,ip-1),2,5)
                  if (ulq(ivx,ibmcnt(ivx)) .eq. 'u') then
                    slen = (4.5+nindent+nolev1(ivx)-nole
     *                +1.3*(eskz(ivx,ip)-eskz(ivx,ip-1)-.7)
     *                                    *islope(ivx)/slfac)*.6667
                  else
                    slen = (4.5+nindent-nolev1(ivx)+nole
     *                -1.3*(eskz(ivx,ip)-eskz(ivx,ip-1)-.7)
     *                                    *islope(ivx)/slfac)*.6667
                  end if
                  write(tempq,'(f4.1)')slen
                  notexq = sq//'slx{'//tempq//'}'//notexq(1:lnote)
                  lnote = lnote+10
                end if
c
c Next steps are a historical kluge to distinguish dotted unbeamed 2-note trem
c (needs \qup) from normal xtup on dotted note (eg e44dx2 f, wants no dot)               
c
                if (btest(irest(ivx,ip-1),2)) then 
                  if (lnote .eq. 0) then
c                  notexq = sq//'q'//ulq(ivx,ibmcnt(ivx))
                    notexq = sq//'q'//ulq(ivx,ibmcnt(ivx))//'p'
     *                     //noteq(1:lnoten)
                  else                  
                    notexq = notexq(1:lnote)//sq//'q'
c     *              //ulq(ivx,ibmcnt(ivx))//noteq(1:lnoten)
     *                //ulq(ivx,ibmcnt(ivx))//'p'//noteq(1:lnoten)
                  end if
c                lnote = lnote+3+lnoten
                  lnote = lnote+4+lnoten
                else
                  if (lnote .eq. 0) then
                    notexq = sq//'q'//ulq(ivx,ibmcnt(ivx))
     *                     //noteq(1:lnoten)
                  else                  
                    notexq = notexq(1:lnote)//sq//'q'
     *                //ulq(ivx,ibmcnt(ivx))//noteq(1:lnoten)
                  end if
                  lnote = lnote+3+lnoten
                end if
                if (btest(irest(ivx,ip-1),2) .and.
     *               nodur(ivx,ip).eq.24 .or. nodur(ivx,ip).eq.12) then
                  notexq = notexq(1:lnote)//sq//'slz'
                  lnote=lnote+4
                end if
              end if
            else if (btest(irest(ivx,ip-1),2)) then
c
c 2nd note of unbeamed quarter or 8th trem; make quarter note
c Get stemlength change
c
              lnote = 46
              nindent = igetbits(irest(ivx,ip-1),2,5)
              if (ulq(ivx,ibmcnt(ivx)) .eq. 'u') then
                slen = (4.5+nindent+nolev1(ivx)-nole
     *                +1.3*(eskz(ivx,ip)-eskz(ivx,ip-1)-.7)
     *                                    *islope(ivx)/slfac)*.6667
              else
                slen = (4.5+nindent-nolev1(ivx)+nole
     *                -1.3*(eskz(ivx,ip)-eskz(ivx,ip-1)-.7)
     *                                    *islope(ivx)/slfac)*.6667
              end if
              write(tempq,'(f4.1)')slen
              notexq = notexq(1:46)//sq//'slx{'//tempq//'}'
              lnote = lnote+10
c              if (lnote .eq. 0) then
c                notexq = sq//'h'//ulq(ivx,ibmcnt(ivx))//noteq
c              else
c                notexq = notexq(1:46)//
c     *              sq//'h'//ulq(ivx,ibmcnt(ivx))//noteq
c              end if
c
c
c Check for dotted unbeamed tremolo
c
              if (abs(nodur(ivx,ip)/12.-nodur(ivx,ip)/12).lt..001) then
c
c Need a dot
c
                if (lnoten .eq. 1) then
                  noteq = ' '//noteq(1:1)
                  lnoten = 2
                end if
                if (lnote .eq. 0) then
                  notexq = sq//'pt'//noteq(1:lnoten)
                else
                  notexq = notexq(1:lnote)//sq//'pt'//noteq(1:lnoten)
                end if
                lnote = lnote+3+lnoten
                call notefq(noteq,lnoten,nole,ncmid(iv,ip))
                if (lnoten .eq. 1) then
                  noteq = ' '//noteq(1:1)
                  lnoten = 2
                end if
              end if               
              notexq = notexq(1:lnote)//
     *                  sq//'q'//ulq(ivx,ibmcnt(ivx))//noteq(1:lnoten)
c     *                  //sq//'stemcut'
     *                  //sq//'slz'
c              lnote=lnote+3+lnoten+8
              lnote=lnote+3+lnoten+4
            else if (multip .eq. -1) then
              notexq = sq//'h'//ulq(ivx,ibmcnt(ivx))//noteq(1:lnoten)
              lnote = lnoten+3
            else if (multip .eq. 1) then
              notexq = sq//'c'//ulq(ivx,ibmcnt(ivx))//noteq(1:lnoten)
              lnote = lnoten+3
            else if (multip .eq. 2) then
              notexq = sq//'cc'//ulq(ivx,ibmcnt(ivx))//noteq(1:lnoten)
              lnote = lnoten+4
            else if (multip .eq. 3) then
              notexq = sq//'ccc'//ulq(ivx,ibmcnt(ivx))//noteq(1:lnoten)
              lnote = lnoten+5
            else if (multip .eq. -2) then
              notexq = sq//'wh'//noteq(1:lnoten)
              lnote = lnoten+3
            else if (multip .eq. -3) then
              notexq = sq//'breve'//noteq(1:lnoten)
              lnote = lnoten+6
            else
              print*
              print*,'(Error in beamend, send source to Dr. Don)'
              call stop1()
            end if
          else
c
c  Prior note is regular-dotted so this one is halved
c
            lnote = lnoten+3
            if (multip .eq. 0) then
              notexq = sq//'c'//ulq(ivx,ibmcnt(ivx))//noteq
            else if (multip .eq. -1) then
              notexq = sq//'q'//ulq(ivx,ibmcnt(ivx))//noteq
            else if (multip .eq. -2) then
              notexq = sq//'h'//ulq(ivx,ibmcnt(ivx))//noteq
            else if (multip .eq. 1) then
              notexq = sq//'cc'//ulq(ivx,ibmcnt(ivx))//noteq
              lnote = lnoten+4
            else if (multip .eq. 2) then
              notexq = sq//'ccc'//ulq(ivx,ibmcnt(ivx))//noteq
              lnote = lnoten+5
            end if
          end if
        end if
      return
      end if
c
c End of block for unbeamed. Problem if beamed but ends w/ rest. Try just 
c skipping the call in that case.
c
      if (.not.btest(irest(ivx,ip),0)) then
        call notefq(noteq,lnoten,nole,ncmid(iv,ip))
      end if
c      call notefq(noteq,lnoten,nole,ncmid(iv,ip))
c      lnote = 0
c
c New way, with flipend, which was computed in beamstrt.
c
      if (flipend(ivx) .and. btest(ipl(ivx,ip),30)) then
        ulq(ivx,ibmcnt(ivx)) = chax(225-ichar(ulq(ivx,ibmcnt(ivx))))
        flipend(ivx) = .false.
      end if
      if (ip .gt. ibm1(ivx,ibmcnt(ivx))) then
c
c This is not a one-noter from beam-jump.  Check if multiplicity has increased
c
        if (btest(irest(ivx,ip-1),0)) then
c
c  Prior note is a rest, check one before that
c
          mp = iand(mult(ivx,ip-2),15)-8
        else
          mp = iand(mult(ivx,ip-1),15)-8
        end if
        if (multip .gt. mp) then
c
c  Assume 1-3, 2-3, or 1-2
c
          do 2 imp = multip , mp+1 , -1
            call ntrbbb(imp,'t',ulq(ivx,ibmcnt(ivx)),ivx,notexq,lnote)
2         continue
        else if (btest(nacc(ivx,ip-1),27)) then
c
c  2nd member of dotted xtup
c
          call ntrbbb(multip+1,'t',
     *                       ulq(ivx,ibmcnt(ivx)),ivx,notexq,lnote)
        end if
      end if
c
c Beam termination and direction analysis
c
      if (btest(irest(ivx,ip),23) .and. .not.isbjmp) then
c
c This is the end of the first segment in a jump-beam.  ivbj1=ivx will be number 
c   of the jump-beam.  ivbj2 will be tested along with isbjmp to see if in the
c   voice of the 2nd part of jumped beam.  (May need special treatment for
c   multi-segment jump-beams
c
        isbjmp = .true.
        ivbj1 = ivx
        multbj1 = iand(15,mult(ivx,ip)-8)      
        ivbj2 = 0      
      end if
      if (.not.btest(irest(ivx,ip),23)) then
c
c This is either a normal beamend or end of a sequence of jump-beam segments, 
c (170409) or rest at end of xtup
c so some sort of termination is required
c
        ulqq = ulq(ivx,ibmcnt(ivx))
        if (.not.isbjmp .or. ivx.ne.ivbj2) then
          if (btest(irest(ivx,ip),0)) then
c
c Xtup ends with rest
c
            if (multip .eq. 1) then
              notexq = sq//'ds'
              lnote = 3
            else if (multip .eq. 2) then
              notexq = sq//'qs'
              lnote = 3
            else if (multip .eq. 3) then
              notexq = sq//'hs'
              lnote = 3
            end if
c
c  170801 Borrowed from main rest entry way down below to get level adjustment:
c  BUT nole is like 102, not 2, so subtracted 100 for nole. Why different???
c  "...  Now raise if necc."
c
            if (btest(islur(ivx,ip),29)) then
c
c  Blank rest
c
              notexq = sq//'sk'
              lnote = 3
            else if (nole .ne. 0) then
c
c  Bandaid. Odd case with rests in xtups + 2 voices where came thru here with
c    nolev=-4 but expected 100+/-. Try to fix.
c
              if (abs(nole).lt.30) nole = 100+nole
c
              if (abs(nole-100) .lt. 10) then
                noteq = chax(48+abs(nole-100))
                lnoten = 1
              else
                write(noteq(1:2),'(i2)')abs(nole-100)
                lnoten = 2
              end if
c  ???              if (nole .gt. 0) then
              if (nole .gt. 100) then
                notexq = sq//'raise'//noteq(1:lnoten)//sq//'internote'
     *                 //notexq(1:lnote)
              else
                notexq = sq//'lower'//noteq(1:lnoten)//sq//'internote'
     *                 //notexq(1:lnote)
              end if
              lnote = 16+lnoten+lnote
            end if
            return
          else
c
c Normal termination
c
            call ntrbbb(1,'t',ulqq,mod(ivx,24),notexq,lnote)
          end if
        else 
c
c Terminate a sequence of jump-beam segments.
c
          ulqq = chax(225-ichar(ulqq)) 
          call ntrbbb(1,'t',ulqq,mod(ivbj1,24),notexq,lnote)
        end if
      end if
c
c Check for end of 2nd seg of staff-jump xtup chord blank rest
c
c      if (isbjmp.and.ivx.eq.ivbj2
      if (isbjmp
     *    .and.btest(islur(ivx,ip),29)) then
        notexq = notexq(1:lnote)//sq//'sk'
        return
      end if
c
c  And now the note, checking for open-head beamed tremolo
c
      if (btest(irest(ivx,ip-1),2)) then
c
c Check for dotted tremolo
c
        if (abs(nodur(ivx,ip)/12.-nodur(ivx,ip)/12).lt..001) then
c
c Need a dot
c
          if (nodur(ivx,ip).eq.24 .or. nodur(ivx,ip).eq.12) then 
c
c Solid notehead
c
            notexq = notexq(1:lnote)//sq//'qbp'
          else
c
c Assuming open notehead and nodur = 48
c
            notexq = notexq(1:lnote)//sq//'hbp'
          end if
          lnote = lnote+4
        else  
          if (nodur(ivx,ip).eq.32 .or. nodur(ivx,ip).eq.48) then
            if (lnote .gt. 0) then
              notexq = notexq(1:lnote)//sq//'hb'
            else
              notexq = sq//'hb'
            end if
          else
            if (lnote .gt. 0) then
              notexq = notexq(1:lnote)//sq//'qb'
            else
              notexq = sq//'qb'
            end if
          end if
          lnote = lnote+3
        end if
      else      
c
c No tremolo
c
        if (lnote .gt. 0) then
          notexq = notexq(1:lnote)//sq//'qb'
        else
          notexq = sq//'qb'
        end if
        lnote = lnote+3
      end if
      isdotm = .false.
      if (.not.vxtup(ivx)) then
        if (2**log2(nodur(ivx,ip)) .ne. nodur(ivx,ip)) then
          if (.not.btest(iornq(ivx,ip),13)) then
            notexq = notexq(1:lnote)//'p'
          else
            notexq = notexq(1:lnote)//'m'
            isdotm = .true.
          end if
          lnote = lnote+1
        end if
      end if
c
c  5/25/08 Allow >12
c  5/9/10 Up to 24; replace 24 with 0
c
      if (.not.(isbjmp.and.ivx.eq.ivbj2)) then
        call istring(mod(ivx,24),tempq,len)
      else
        call istring(mod(ivbj1,24),tempq,len)
      end if
      if (isbjmp .and. ivx.eq.ivbj2 
     *     .and..not.btest(irest(ivx,ip),23)) isbjmp=.false.      
      notexq = notexq(1:lnote)//tempq(1:len)
      lnote = lnote+len
      notexq = notexq(1:lnote)//noteq(1:lnoten)
      lnote = lnote+lnoten
      if (isdotm) then
        if (lnoten .eq. 1) then
          notexq = notexq(1:lnote)//'{'//noteq(1:1)//'}'
          lnote = lnote+3
        else
          notexq = notexq(1:lnote)//noteq(lnoten-1:lnoten-1)
          lnote = lnote+1
        end if
      end if
      return
      end
      subroutine beamid(notexq,lnote)
      parameter (nm=24)
      common /all/ mult(nm,200),iv,nnl(nm),nv,ibar,
     *   ivxo(600),ipo(600),to(600),tno(600),tnote(600),eskz(nm,200),
     *   ipl(nm,200),ibm1(nm,9),ibm2(nm,9),nolev(nm,200),ibmcnt(nm),
     *   nodur(nm,200),jn,lenbar,iccount,nbars,itsofar(nm),nacc(nm,200),
     *   nib(nm,15),nn(nm),lenb0,lenb1,slfac,musicsize,stemmax,
     *   stemmin,stemlen,mtrnuml,mtrdenl,mtrnmp,mtrdnp,islur(nm,200),
     *   ifigdr(2,125),iline,figbass,figchk(2),firstgulp,irest(nm,200),
     *   iornq(nm,0:200),isdat1(202),isdat2(202),nsdat,isdat3(202),
     *   isdat4(202),beamon(nm),isfig(2,200),sepsymq(nm),sq,ulq(nm,9)
      character*1 ulq,sepsymq,sq,ulqq,chax
      logical beamon,firstgulp,figbass,figchk,flipend,btest,
     *        isfig,vxtup,isdotm,isbjmp,bar1syst,isdotted,isbj2,drawbm
      character*79 notexq
      common /combjmp/ ivbj1,ivbj2,isbjmp,isbj2,multbj1
      common /comoct/ noctup
      common /strtmid/ ihnum3,flipend(nm),ixrest(nm)
      common /comxtup/ ixtup,vxtup(nm),ntupv(nm,9),nolev1(nm),
     *                 mtupv(nm,9),nxtinbm(nm),
     *                 islope(nm),xelsk(24),eloff(nm,9),
     *                 nssb(nm),issb(nm),lev1ssb(nm,20)
      common /comdraw/ drawbm(nm)
      common /commvl/ nvmx(nm),ivmx(nm,2),ivx
      common /comask/ bar1syst,fixednew,scaldold,
     *                wheadpt,fbar,poenom
      character*8 noteq
      character*4 tempq
        lnote = 0
        ip = ipo(jn)
        nole = nolev(ivx,ip)
c
c  Check for special situations with 2nds (see precrd)
c
        if (btest(nacc(ivx,ip),30)) then
          nole = nole - 1
        else if (btest(nacc(ivx,ip),31)) then
          nole = nole + 1
        end if
        if (.not.btest(irest(ivx,ip),0)) then
          multip = iand(mult(ivx,ip),15)-8
c          if (btest(islur(ivx,ip-1),3)) multip = multip+1
c
c  (Above test OK since must have ip>1).  Double dotted note preceding
c
c  Move the following, because can't ask for note until after checking for
c  embedded xtup with number, due to ordering/octave feature.
c
c         call notefq(noteq,lnoten,nolev(ivx,ip),ncmid(iv,ip))
        end if
        if (btest(irest(ivx,ip),28)) vxtup(ivx) = .true.
        if (vxtup(ivx)) then
c
c  In an xtup
c
          if (btest(irest(ivx,ip),0)) then
c
c  Intermediate rest in xtup, put in the rest.  Reset nodur so notex works OK
c
            nodur(ivx,ip) = 2**(4-(iand(mult(ivx,ip),15)-8))
            call notex(notexq,lnote)
c
c  Re-zero so next note does not get confused
c
            nodur(ivx,ip) = 0
            return
          end if
          if (.not.drawbm(ivx)) then
c
c  Xtuplet with no beam, just put in the right kind of note
c
            if (btest(islur(ivx,ip),30)) then
c
c  Forced stem direction
c
              ndsav = nodur(ivx,ip)
              nodur(ivx,ip) = 2**(4-multip)
              if (btest(nacc(ivx,ip),19) .or. 
     *                     btest(nacc(ivx,ip),27)) then
                nodur(ivx,ip)=3*nodur(ivx,ip)/2
              else if (btest(nacc(ivx,ip-1),27)) then
                nodur(ivx,ip)=nodur(ivx,ip)/2
              end if
              call notex(notexq,lnote)
              nodur(ivx,ip) = ndsav
            else
c
c  Use ulq for stem direction
c
              call notefq(noteq,lnoten,nole,ncmid(iv,ip))
              if (lnoten .eq. 1) call addblank(noteq,lnoten)
              lnote = 3
              if (.not.btest(nacc(ivx,ip-1),27)) then
c
c  Prior note of xtup is not regular-dotted
c              
                if (multip .eq. 0) then
                  notexq = sq//'q'//ulq(ivx,ibmcnt(ivx))
                else if (multip .eq. -1) then
                  notexq = sq//'h'//ulq(ivx,ibmcnt(ivx))
                else if (multip .eq. 1) then
                  notexq = sq//'c'//ulq(ivx,ibmcnt(ivx))
                else if (multip .eq. 2) then
                  notexq = sq//'cc'//ulq(ivx,ibmcnt(ivx))
                  lnote = 4
                else if (multip .eq. 3) then
                  notexq = sq//'ccc'//ulq(ivx,ibmcnt(ivx))
                  lnote = 5
                else if (multip .eq. -2) then
                  notexq = sq//'wh'
                end if
                if (btest(nacc(ivx,ip),27)) then
c
c  This xtup note is regular dotted non-beamed xtup
c
c                  notexq = notexq(1:3)//'p'
c                  lnote = 4
                  notexq = notexq(1:lnote)//'p'
                  lnote = lnote+1
                end if
              else
c
c  Prior note of xtup is regular-dotted so this one is halved
c
                if (multip .eq. 2) then
                  notexq = sq//'ccc'//ulq(ivx,ibmcnt(ivx))
                  lnote = 5
                else if (multip .eq. 1) then
                  notexq = sq//'cc'//ulq(ivx,ibmcnt(ivx))
                  lnote = 4
                else if (multip .eq. 0) then
                  notexq = sq//'c'//ulq(ivx,ibmcnt(ivx))
                else if (multip .eq. -1) then
                  notexq = sq//'q'//ulq(ivx,ibmcnt(ivx))
                else if (multip .eq. -2) then
                  notexq = sq//'h'//ulq(ivx,ibmcnt(ivx))
                end if
              end if
              notexq = notexq(1:lnote)//noteq
              lnote = lnote+lnoten
            end if
            return
          else if (nodur(ivx,ip).eq.0) then
c
c  In the beamed xtup but not the last note
c
            if (nodur(ivx,ip-1).gt.0) then
c
c  Embedded Xtup, mult>0, starts here.  Put in number if needed
c
              nxtinbm(ivx) = nxtinbm(ivx)+1
              iud = 1
              if (ulq(ivx,ibmcnt(ivx)) .eq. 'u') iud = -1
c
c  Get ip#, notelevel of middle note (or gap) in xtup
c
              ipmid = ip+ntupv(ivx,nxtinbm(ivx))/2
              xnlmid = levrn(nolev(ivx,ipmid),irest(ivx,ipmid),iud,
     *                       ncmid(iv,ipmid),iand(15,mult(ivx,ipmid))-8)
              if (mod(ntupv(ivx,nxtinbm(ivx)),2).eq.0) xnlmid = (xnlmid+
     *            levrn(nolev(ivx,ipmid-1),irest(ivx,ipmid-1),iud,
     *            ncmid(iv,ipmid-1),iand(15,mult(ivx,ipmid-1))-8))/2
              iflop = 0
              if (abs(xnlmid-ncmid(iv,ip)).lt.3.) iflop = -iud
              iup = iud+2*iflop
              if (btest(irest(ivx,ip),14)) then
                iup = -iup
                iflop = 0
                if (iud*iup .lt. 0) iflop = iup
              end if
c
c  Place number if needed
c
              if (.not.btest(islur(ivx,ip),31)) then
                mprint = igetbits(nacc(ivx,ip),5,22)
                if (mprint.eq.0) mprint=mtupv(ivx,nxtinbm(ivx))
                call putxtn(mprint,iflop,multip,iud,wheadpt,poenom,
     *            nolev1(ivx),islope(ivx),slfac,
     *            xnlmid,islur(ivx,ip),lnote,notexq,ncmid(iv,ip),nlnum,
     *            eloff(ivx,nxtinbm(ivx)),iup,irest(ivx,ip),
     *            mult(ivx,ip),.false.)
                end if
              call notefq(noteq,lnoten,nole,ncmid(iv,ip))
            else
c
c  Intermediate note of xtup
c
              call notefq(noteq,lnoten,nole,ncmid(iv,ip))
            end if
          else
c
c  Last note of xtup (but not last note of beam!)
c
            call notefq(noteq,lnoten,nole,ncmid(iv,ip))
          end if
        else if (btest(irest(ivx,ip),0)) then
          call notex(notexq,lnote)
          return
        else
          call notefq(noteq,lnoten,nole,ncmid(iv,ip))
        end if
c
c Check for string of rests up to and including last note in xtup.
c
c Replace next 2 lines to keep from doing this block
c   when in second part of staff-jumping chordal xtup. This fix could
c   break some unaccounted non-chordal staff-jum xtup situations.
c        if (vxtup(ivx) .and. btest(irest(ivx,ip+1),0) .and. 
c     *           .not.btest(irest(ivx,ip),0)) then
        if (vxtup(ivx) .and. btest(irest(ivx,ip+1),0) .and. 
     *           .not.btest(irest(ivx,ip),0) .and.
     *           .not.(isbjmp.and.ivx.eq.ivbj2)) then
c
c This note is not a rest but next is a rest. Do rests continue to
c   end of xtup, where nodur>0
c
          do 3 ipnow = ip+1 , ip+24
            if (nodur(ivx,ipnow).gt.0) go to 4  ! This is last of xtup
            if (.not.btest(irest(ivx,ipnow+1),0)) go to 5  ! Next is not rest
c
c  If I don't go to 5, know next note IS a rest!
c
3         continue
4         continue
          call ntrbbb(1,'t',ulq(ivx,ibmcnt(ivx)),ivx,notexq,lnote)
          notexq = notexq(1:lnote)//sq//'qb'
          lnote = lnote+3
          call istring(ivx,tempq,len)
          notexq = notexq(1:lnote)//tempq(1:len)
          lnote = lnote+len
          notexq = notexq(1:lnote)//noteq(1:lnoten)
          lnote = lnote+lnoten
          return
5         continue
c
c Check if multiplicity changes in a way requiring action,
c unless (160211) it's blank rest on start of 2nd seg of joined beam
c
        else if(.not.btest(irest(ivx,ip-1),24)
     *         .or..not.btest(islur(ivx,ip-1),29)) then
          ipleft = ip-1
          if (btest(irest(ivx,ipleft),0)) ipleft = ipleft-1
          if (.not.btest(islur(ivx,ipleft),20)) then
            multl = iand(15,mult(ivx,ipleft))-8
          else
            multl = 1
          end if
          mub = multip - multl
          ipright = ip+1
          if (btest(irest(ivx,ipright),0)) ipright = ipright+1
          if (.not.btest(islur(ivx,ip),20)) then
            multr = iand(15,mult(ivx,ipright))-8
          else
            multr = 1
          end if
          mua = multr-multip
          if (mub.gt.0 .or. mua .lt. 0) then
c
c  Multiplicity has increased from left or will decrease to right. Need action.
c
            if (isbjmp .and. ivx.eq.ivbj2) then
              ivb = ivbj1
              ulqq = chax(225-ichar(ulq(ivx,ibmcnt(ivx))))
            else
              ivb = ivx
              ulqq = ulq(ivx,ibmcnt(ivx))
            end if
            if (mua .ge. 0) then
              call ntrbbb(multip,'n',ulqq,ivb,notexq,lnote)
c
c  Test for next note being blank rest, assuming staff-crossing
c    xtup chord
c
            else if (multl .ge. multr .and.
     *               .not.btest(islur(ivx,ip+1),29)) then
              do 1 im = multip , 1+multr, -1
                call ntrbbb(im,'t',ulqq,ivb,notexq,lnote)
1             continue
c            else
c  Test for next note being blank rest, assuming staff-crossing
c    xtup chord
c
            else if (.not.btest(islur(ivx,ip+1),29)) then
              do 2 im = 1+multr, multip
                call ntrbbb(im,'r',ulqq,ivb,notexq,lnote)
2             continue
              call ntrbbb(multr,'n',ulqq,ivb,notexq,lnote)
            end if
          else if (ip .gt. 1) then
c
c  Check for 2nd member of dotted xtup
c
            if (btest(nacc(ivx,ip-1),27)) call ntrbbb(multip+1,'t',
     *                       ulq(ivx,ibmcnt(ivx)),ivx,notexq,lnote)
          end if
        end if
c
c Now put in the note
c
        if (lnote .gt. 0) then
          notexq = notexq(1:lnote)//sq//'qb'
        else
          notexq = sq//'qb'
        end if
        lnote = lnote+3
        isdotm = .false.
        if (isdotted(nodur,ivx,ip)) then
c
c rule out ')'
c
          if (.not.btest(iornq(ivx,ip),13)) then
            if (.not.btest(islur(ivx,ip),3)) then
              notexq = notexq(1:lnote)//'p'
            else
c
c  Double dot
c
              notexq = notexq(1:lnote)//'pp'
              lnote = lnote+1
            end if
          else
            notexq = notexq(1:lnote)//'m'
            isdotm = .true.
          end if
          lnote = lnote+1
        else if (btest(nacc(ivx,ip),19) 
     *           .or. btest(nacc(ivx,ip),27)) then
c
c  Special dotted notation for 2:1 xtup, or normal dot in xtup
c
          notexq = notexq(1:lnote)//'p'
          lnote = lnote+1
        end if
c
c 5/25/08 Allow >12
c
        if (.not.(isbjmp.and.ivx.eq.ivbj2)) then
c          call istring(mod(ivx,12),tempq,len)
          call istring(mod(ivx,24),tempq,len)
        else
c          call istring(mod(ivbj1,12),tempq,len)
          call istring(mod(ivbj1,24),tempq,len)
        end if
        notexq = notexq(1:lnote)//tempq(1:len)
        lnote = lnote+len
        notexq = notexq(1:lnote)//noteq(1:lnoten)
        lnote = lnote+lnoten
        if (isdotm) then
          if (lnoten .eq. 2) then
            notexq = notexq(1:lnote)//'{'//noteq(2:2)//'}'
            lnote = lnote+3
          else
            notexq = notexq(1:lnote)//noteq(lnoten-1:lnoten-1)
            lnote = lnote+1
          end if
        end if
      return
      end
      subroutine beamn1(notexq,lnote)
      parameter (nm=24)
      common /all/ mult(nm,200),iv,nnl(nm),nv,ibar,
     *   ivxo(600),ipo(600),to(600),tno(600),tnote(600),eskz(nm,200),
     *   ipl(nm,200),ibm1(nm,9),ibm2(nm,9),nolev(nm,200),ibmcnt(nm),
     *   nodur(nm,200),jn,lenbar,iccount,nbars,itsofar(nm),nacc(nm,200),
     *   nib(nm,15),nn(nm),lenb0,lenb1,slfac,musicsize,stemmax,
     *   stemmin,stemlen,mtrnuml,mtrdenl,mtrnmp,mtrdnp,islur(nm,200),
     *   ifigdr(2,125),iline,figbass,figchk(2),firstgulp,irest(nm,200),
     *   iornq(nm,0:200),isdat1(202),isdat2(202),nsdat,isdat3(202),
     *   isdat4(202),beamon(nm),isfig(2,200),sepsymq(nm),sq,ulq(nm,9)
      character*1 ulq,sepsymq,sq
      logical beamon,firstgulp,figbass,figchk,btest,
     *        isfig,vxtup,isdotm,isbjmp,isbj2,drawbm
      logical gotnote
      common /comoct/ noctup
      common /combjmp/ ivbj1,ivbj2,isbjmp,isbj2,multbj1
      common /comxtup/ ixtup,vxtup(nm),ntupv(nm,9),nolev1(nm),
     *                 mtupv(nm,9),nxtinbm(nm),
     *                 islope(nm),xelsk(24),eloff(nm,9),
     *                 nssb(nm),issb(nm),lev1ssb(nm,20)
      common /comdraw/ drawbm(nm)
      common /commvl/ nvmx(nm),ivmx(nm,2),ivx
      character*8 noteq,tempq,numq
      character*79 notexq
      gotnote = .false.
      lnoten = 0
      ip1 = ipo(jn)
      multip = iand(15,mult(ivx,ip1))-8
      if (.not.drawbm(ivx) .and. btest(irest(ivx,ip1),0)) then
        lnote = 0
c
c  The rest was already written in beamstrt, so just get out of here
c
        return
      end if
      nole = nolev(ivx,ipo(jn))
c
c  Check for special situations with 2nds (see precrd)
c
      if (btest(nacc(ivx,ipo(jn)),30)) then
        nole = nole - 1
      else if (btest(nacc(ivx,ipo(jn)),31)) then
        nole = nole + 1
      end if
      if (vxtup(ivx) .and. .not.drawbm(ivx)) then
c
c  Xtuplet with no beam, just put in the right kind of note
c
        if (btest(islur(ivx,ip1),30)) then
c
c  Forced stem direction
c
          ndsav = nodur(ivx,ip1)
          nodur(ivx,ip1) = 2**(4-multip)
          if (btest(nacc(ivx,ip1),19) .or. 
     *       btest(nacc(ivx,ip1),27)) nodur(ivx,ip1)=3*nodur(ivx,ip1)/2
          call notex(notexq,lnote)
          nodur(ivx,ip1) = ndsav
        else
          call notefq(noteq,lnoten,nole,ncmid(iv,ip1))
          gotnote = .true.
          if (lnoten .eq. 1) call addblank(noteq,lnoten)
          lnote = 3
          if (multip .le. 0) then
            if ((btest(irest(ivx,ip1),2) .and.
     *                 nodur(ivx,ip1+1).ge.32)
     *                     .or. multip .eq. -1) then
c
c 1st note of unbeamed half-note trem; make open
c
              notexq = sq//'h'//ulq(ivx,ibmcnt(ivx))
            else if (multip .eq. -2) then
              notexq = sq//'wh'
            else
              notexq = sq//'q'//ulq(ivx,ibmcnt(ivx))
            end if
c
c  Check for dot
c
            if (btest(irest(ivx,ip1),2) .and. 
     *       abs(nodur(ivx,ip1+1)/12.-nodur(ivx,ip1+1)/12).lt..001) then
c
c Need a dot. already called addblank for noteq
c
              if (lnote .eq. 0) then
                notexq = sq//'pt'//noteq(1:lnoten)
                lnote = 3+lnoten
              else
                notexq = notexq(1:lnote)//'p'
                lnote = lnote+1
              end if
            end if
c
c  Insert the stemlength calcs here for dotted, unbeamed. 
c  Later may combine with below to avoid repeat.
c
            if (btest(irest(ivx,ip1),2)) then
              nindent = igetbits(irest(ivx,ip1),2,5)
              if (ulq(ivx,ibmcnt(ivx)) .eq. 'u') then
                slen = (4.5+nindent+nolev1(ivx)-nole
     *               -.4*islope(ivx)/slfac)*.6667
              else
                slen = (4.5+nindent-nolev1(ivx)+nole
     *               +.4*islope(ivx)/slfac)*.6667 
              end if
              write(tempq,'(f4.1)')slen
              notexq = sq//'slx{'//tempq(1:4)//'}'//
     *          notexq(1:lnote)
              lnote = lnote+10        
            end if
          else if (btest(irest(ivx,ip1),2)) then
c
c 1st note of unbeamed quarter or 8th trem, make a quarter note
c
c Stem length calcs here. The .2 factor is empirical, but
c   slfac accounts for musicsize. 
c
            nindent = igetbits(irest(ivx,ip1),2,5)
            if (ulq(ivx,ibmcnt(ivx)) .eq. 'u') then
              slen = (4.5+nindent+nolev1(ivx)-nole
     *               -.4*islope(ivx)/slfac)*.6667
            else
              slen = (4.5+nindent-nolev1(ivx)+nole
     *               +.4*islope(ivx)/slfac)*.6667 
            end if
            write(tempq,'(f4.1)')slen
            notexq = sq//'slx{'//tempq(1:4)//'}'//
     *          sq//'q'//ulq(ivx,ibmcnt(ivx))
            lnote = 13        
c
c Check for dotted 2-note trem; prepend dot to notexq if needed
c
            if (nodur(ivx,ip1+1).eq.12 .or. 
     *                nodur(ivx,ip1+1).eq.24) then
              if (lnoten .eq. 1) then
                noteq = ' '//noteq(1:1)
                lnoten = 2
              end if
              notexq = sq//'pt'//noteq(1:lnoten)//notexq(1:lnote)
              lnote = lnote+3+lnoten
              call notefq(noteq,lnoten,nole,ncmid(iv,ip1))
              gotnote = .true.
              if (lnoten .eq. 1) then
                noteq = ' '//noteq(1:1)
                lnoten = 2
              end if
            end if
          else if (multip .eq. -1) then
            notexq = sq//'h'//ulq(ivx,ibmcnt(ivx))
          else if (multip .eq. 1) then
            notexq = sq//'c'//ulq(ivx,ibmcnt(ivx))
          else if (multip .eq. 2) then
            notexq = sq//'cc'//ulq(ivx,ibmcnt(ivx))
            lnote = 4
          else if (multip .eq. 3) then
            notexq = sq//'ccc'//ulq(ivx,ibmcnt(ivx))
            lnote = 5
          else if (multip .eq. -2) then
            notexq = sq//'wh'
          else if (multip .eq. -3) then
            notexq = sq//'breve'
            lnote = 6
          else
            print*
            print*,'(Error in beamn1, send source to Dr. Don)'
            call stop1()
          end if
          if (btest(nacc(ivx,ip1),19) .or. btest(nacc(ivx,ip1),27)) then 
c            notexq = notexq(1:3)//'p'
c            lnote = 4
            notexq = notexq(1:lnote)//'p'
            lnote = lnote+1
          end if
          notexq = notexq(1:lnote)//noteq
          lnote = lnote+lnoten
        end if
        return
      end if
c
c  Check if mult. decreases from 1st note to 2nd
c
      if (ibm2(ivx,ibmcnt(ivx)).gt.ip1 
     *           .or. btest(islur(ivx,ip1),20)) then
c
c  More than one note or single-note before a multiplicity-down-up "]["
c
        if (btest(islur(ivx,ip1),20)) then
          multr = 1
        else if (.not.btest(irest(ivx,ip1+1),0)) then
          multr = iand(15,mult(ivx,ip1+1))-8
        else
          multr = iand(15,mult(ivx,ip1+2))-8
        end if
        lnote = 0
c
c  Check if staff-jumper
c
        if (isbjmp .and. ivbj2.gt.0) then
          ivxp = ivbj1
        else
          ivxp = ivx
        end if
        if (multr .lt. multip) then
          do 1 im = multip , multr+1 , -1
c
c Right-shifted 'termination'
c
            if (isbjmp .and. ivbj2.gt.0) then
c
c Jump beam
c
              call ntrbbb(im,'t',ulq(ivxp,ibmcnt(ivx)),
     *                    ivxp,notexq,lnote)
            else
c
c Same staff
c
            call ntrbbb(im,'r',ulq(ivx,ibmcnt(ivx)),ivxp,notexq,lnote)
            end if 
1         continue
        end if
      end if
c
c  Check for beamed, dotted 2-note tremolo
c
      if (btest(irest(ivx,ip1),2) .and. 
     *      abs(nodur(ivx,ip1+1)/12.-nodur(ivx,ip1+1)/12).lt..001) then
        call notefq(noteq,lnoten,nole,ncmid(iv,ip))
        gotnote = .true.
        if (lnoten .eq. 1) then
          noteq = ' '//noteq(1:1)
          lnoten = 2
        end if
      end if
c
c  Put in the note, but check first for open-head beamed tremolo.
c
      if (btest(irest(ivx,ip1),2) .and. nodur(ivx,ip1+1).ge.32) then
c
c 2-note open head tremolo
c
        if (lnote .gt. 0) then
          notexq = notexq(1:lnote)//sq//'hb'
        else
          notexq = sq//'hb'
        end if
      else
        if (lnote .gt. 0) then
          notexq = notexq(1:lnote)//sq//'qb'
        else
          notexq = sq//'qb'
        end if
      end if
      lnote = lnote+3
      if (btest(irest(ivx,ip1),2) .and. 
     *    (nodur(ivx,ip1+1).eq.24.or.nodur(ivx,ip1+1).eq.12
     *     .or.nodur(ivx,ip1+1).eq.48)) then
c
c 2-note trem on dotted note
c
        notexq = notexq(1:lnote)//'p'
        lnote = lnote+1
      end if
c
c  Check for dot
c
      isdotm = .false.
      if (.not.vxtup(ivx)) then
        nd = nodur(ivx,ipo(jn))
        if (nd.ne.0) then
          if (2**log2(nd).ne.nd) then
            if (.not.btest(iornq(ivx,ip1),13)) then
              if (.not.btest(islur(ivx,ip1),3)) then
                notexq = notexq(1:lnote)//'p'
              else
c
c  Double dot
c
                notexq = notexq(1:lnote)//'pp'
                lnote = lnote+1
              end if
            else
              notexq = notexq(1:lnote)//'m'
              isdotm = .true.
            end if
            lnote = lnote+1
          end if
        end if
      else if (btest(nacc(ivx,ip1),19) .or. 
     *               btest(nacc(ivx,ip1),27)) then
c
c  In an xtup with special 2:1 notation with a dot on 1st note, or normal dot
c
        notexq = notexq(1:lnote)//'p'
        lnote = lnote+1
      end if
c
c  Do the number; 0 if 12
c  5/25/08 allow >12
c
      if (.not.btest(irest(ivx,ip1),24)) then
        call istring(mod(ivx,24),numq,len)
      else
c
c  1st note of staff-jumping beam
c
        call istring(mod(ivbj1,24),numq,len)
      end if
      notexq = notexq(1:lnote)//numq(1:len)
      lnote = lnote+len
      if (.not.gotnote) then
        call notefq(noteq,lnoten,nole,ncmid(iv,ip1))
      end if
      notexq = notexq(1:lnote)//noteq(1:lnoten)
      lnote = lnote+lnoten
      if (isdotm) then
        if (lnoten .eq. 1) then
          notexq = notexq(1:lnote)//'{'//noteq(1:1)//'}'
          lnote = lnote+3
        else
          notexq = notexq(1:lnote)//noteq(lnoten-1:lnoten-1)
          lnote = lnote+1
        end if
      end if
      return
      end
      subroutine beamstrt(notexq,lnote,nornb,ihornb,space,squez,ib)
      parameter (nm=24)
      common /all/ mult(nm,200),iv,nnl(nm),nv,ibar,
     *   ivxo(600),ipo(600),to(600),tno(600),tnote(600),eskz(nm,200),
     *   ipl(nm,200),ibm1(nm,9),ibm2(nm,9),nolev(nm,200),ibmcnt(nm),
     *   nodur(nm,200),jn,lenbar,iccount,nbars,itsofar(nm),nacc(nm,200),
     *   nib(nm,15),nn(nm),lenb0,lenb1,slfac,musicsize,stemmax,
     *   stemmin,stemlen,mtrnuml,mtrdenl,mtrnmp,mtrdnp,islur(nm,200),
     *   ifigdr(2,125),iline,figbass,figchk(2),firstgulp,irest(nm,200),
     *   iornq(nm,0:200),isdat1(202),isdat2(202),nsdat,isdat3(202),
     *   isdat4(202),beamon(nm),isfig(2,200),sepsymq(nm),sq,ulq(nm,9)
      character*1 ulq,sepsymq,sq
      character*40 restq
      character*79 inameq
      logical beamon,firstgulp,figbass,figchk,btest,
     *        isfig,vxtup,bar1syst,addbrack,flipend,xto,drawbm
      common /comeskz2/ eskz2(nm,200)
      common /comoct/ noctup
      common /strtmid/ ihnum3,flipend(nm),ixrest(nm)
      common /comxtup/ ixtup,vxtup(nm),ntupv(nm,9),nolev1(nm),
     *                 mtupv(nm,9),nxtinbm(nm),
     *                 islope(nm),xelsk(24),eloff(nm,9),
     *                 nssb(nm),issb(nm),lev1ssb(nm,20)
      common /comdraw/ drawbm(nm)
      common /comas1/ naskb,task(40),wask(40),elask(40)
      common /comask/ bar1syst,fixednew,scaldold,
     *                wheadpt,fbar,poenom
      common /comtop / itopfacteur,ibotfacteur,interfacteur,isig0,
     *   isig,lastisig,fracindent,widthpt,height,hoffpt,voffpt,idsig,
     *   lnam(nm),inameq(nm)
      common /commvl/ nvmx(nm),ivmx(nm,2),ivx
      common /comtrill/ ntrill,ivtrill(24),iptrill(24),xnsktr(24),
     *                ncrd,icrdat(193),icrdot(193),icrdorn(193),nudorn,
     *                kudorn(63),ornhshft(63),minlev,maxlev,icrd1,icrd2
c
c  The following is just to save the outputs from SetupB for the case of
c  xtups starting with a rest, where beamstrt is called twice.
c
      common /comipb/ nnb,sumx,sumy,ipb(24),smed
      character*1 chax
      character*8 noteq
      character*79 notexq,tempq
      integer nornb(nm),ihornb(nm,24)
      real*4 space(80),squez(80)
      logical novshrinktop,cstuplet,usexnumt,writebrests
      common /comnvst/ novshrinktop,cstuplet
      common /comfig/ itfig(2,74),figq(2,74),ivupfig(2,74),nfigs(2),
     *                fullsize(nm),ivxfig2,ivvfig(2,74)
      character*10 figq
      common /xjbeambrests/ nbrests
      writebrests = .true.
      ibc = ibmcnt(ivx)
      ipb1 = ibm1(ivx,ibc)
      multb = iand(15,mult(ivx,ipb1))-8
      ip = ipo(jn)
      lnote = 0
      nvtrem = 0  ! Vertical adjustment beams for tremolo
c
c  Compute slopes and note offsets from start of beam.  Inside SetupB, for each
c  xtup in the beam, set eloff,mtupv (in comxtup) for printed number. Also
c  gets islope(ivx), transferred in common.
c
      if (ixrest(ivx).eq.0 .and. .not.btest(nacc(ivx,ip),21)) 
     *      call SetupB(xelsk,nnb,sumx,sumy,ipb,smed,ixrest(ivx))
c
c Will always come past here after entering beamstrt for 2-note tremolo, and
c slope will have been computed, even for unbeamed. So start the indented 
c beams here. The indented tremolo bars will be added to the notexq output
c string, under the tacet assumption that there is no horizontal shift
c enacted by the action just before and after the call to beamstrt.
c
      if (btest(irest(ivx,ipb1),2) .and. 
     *            igetbits(irest(ivx,ipb1),2,5) .gt. 0) then
        nindent = igetbits(irest(ivx,ipb1),2,5)
        tempq = sq//'roffset{.7}{'//sq//'ib'
        ltemp = 16
        if (nindent .eq. 2) then
          tempq = tempq(1:16)//'b'
          ltemp = 17
        else if (nindent .eq. 3) then
          tempq = tempq(1:16)//'bb'
          ltemp = 18
        end if
c
c Use beam # 0 for indented beams
c
        tempq = tempq(1:ltemp)//ulq(ivx,ibc)//'0'
        ltemp = ltemp+2
c
c Get adjustment to nolev1 for main beam based on nindent 
c
        if (ulq(ivx,ibc) .eq. 'u') then
          nvtrem = nindent-1
        else
          nvtrem = 1-nindent
        end if
c	
c Get numerical position of initiation for main beam. Will adjust height at
c   closing to account for nindent
c
        numinit = nolev1(ivx)-ncmid(iv,ipb1)+4
c
c Get forced beam height tweak, apply here for indented beams
c
        iadj = igetbits(ipl(ivx,ipb1),6,11)-30
        if (iadj .ne. -30) numinit = numinit+iadj
        if (numinit.ge.0 .and. numinit.le.9) then
          write(noteq,'(i1)') numinit
          lnoten = 1
        else
          write(noteq,'(a1,i2,a1)')'{',numinit,'}'
          lnoten = 4
        end if
        tempq = tempq(1:ltemp)//noteq(1:lnoten)
        ltemp = ltemp+lnoten
c
c Now do the slope. 
c 170408 Baseline is in islope(ivx). Apply slope tweak if present.
c
        numinit = islope(ivx)
        iadj = igetbits(ipl(ivx,ipb1),6,17)-30
        if (iadj .ne. -30) numinit = numinit+iadj
        if (numinit.ge.0 .and. numinit.le.9) then
          write(noteq,'(i1,a1)') numinit,'}'
          lnoten = 2
        else
          write(noteq,'(a1,i2,a2)')'{',numinit,'}}'
          lnoten = 5
        end if
        tempq = tempq(1:ltemp)//noteq(1:lnoten)
        ltemp = ltemp+lnoten
        notexq = tempq
        lnote = ltemp
      end if
      if (btest(nacc(ivx,ip),21)) then
c
c  This is start of later segment of single-slope beam group so use slope and 
c    height from prior beam.   Slope is already OK.
c
        issb(ivx) = issb(ivx)+1  
        nolev1(ivx) = lev1ssb(ivx,issb(ivx))
      end if
c
c  Move this up to before indented beams for tremolo
c      lnote = 0
      drawbm(ivx) = .true.
      if (btest(irest(ivx,ipb1),28) .and. ixrest(ivx).ne.2) then
        vxtup(ivx) = .true.
        nxtinbm(ivx) = nxtinbm(ivx)+1
c
c  irest(28)=>Xtup starts on this note.  Set up for xtuplet.
c  Number goes on notehead side at middle note (or gap) of xtup, unless that
c  puts it in staff, then it flops to stem (or beam) side.
c               __          __
c         |    |  |   O    |  |
c         O      |    |      O
c       |___|    O  |__|     |
c
c  iud   -1     -1    1      1    ...stem direction
c iflop   0      1   -1      0    ...direction of flop
c  iup   -1      1   -1      1    ...direction of number and bracket
c
        iud = 1
        if (ulq(ivx,ibc) .eq. 'u') iud = -1
c
c  Get ip#, note level of middle note (or gap) in xtup
c
        ipmid = ipb1+ntupv(ivx,nxtinbm(ivx))/2
c
c  130129 If middle note is a rest, go to next note. Note last note cannot
c    be a rest
c
14      continue
        if (btest(irest(ivx,ipmid),0)) then
          ipmid = ipmid+1
          go to 14
        end if
        xnlmid = levrn(nolev(ivx,ipmid),irest(ivx,ipmid),iud,
     *                 ncmid(iv,ipmid),iand(15,mult(ivx,ipmid))-8)
        if (mod(ntupv(ivx,nxtinbm(ivx)),2) .eq. 0) xnlmid = (xnlmid+
     *            levrn(nolev(ivx,ipmid-1),irest(ivx,ipmid-1),iud,
     *            ncmid(iv,ipmid-1),iand(15,mult(ivx,ipmid-1))-8))/2
        iflop = 0
        if (abs(xnlmid-ncmid(iv,ipb1)).lt.3.) iflop = -iud
        iup = iud+2*iflop
        if (btest(irest(ivx,ipb1),14)) then
c
c  Alter iud, iflop, iup to flip number/bracket. (Stare at above pic)
c
          iup = -iup
          iflop = 0
          if (iud*iup .lt. 0) iflop = iup
        end if
c
c  Determine if a beam is to be drawn.
c    Had problem w/ half-note 2-note xtups always F, so add a test
c
        if (igetbits(irest(ivx,ipb1),2,3) .ne. 0) then
          drawbm(ivx) = .true.
          go to 6
        end if
        do 5 ipp = ibm1(ivx,ibc),ibm2(ivx,ibc)
          if (iand(15,mult(ivx,ipp))-8 .le. 0) then
            drawbm(ivx) = .false.
            go to 6
          end if
5       continue
        drawbm(ivx) = .not.btest(islur(ivx,ibm1(ivx,ibc)),18)
6       continue
c
c Are we using tuplet.tex?
c
        usexnumt = cstuplet .and. .not.drawbm(ivx)
c
c Check for single note xtup 171217
c
        if (btest(irest(ivx,ipb1),28) .and. nnb.eq.1) then
          drawbm(ivx) = .false.
        end if
c
c  Place xtup number if needed
c
        if (.not.btest(islur(ivx,ipb1),31) .or. multb.le.0) then 
          mprint = igetbits(nacc(ivx,ip),5,22)
          if (mprint.eq.0) mprint=mtupv(ivx,nxtinbm(ivx))
c Stab in the dark! Note: tried setting unbeam flag islur18 but no go.
          if (nnb .eq. 1) then
            xnlmid = nolev1(ivx)
            islope(ivx) = 0
c
c Move up, outside number check block, so all 1-note xtups ar unbeamed 
c            drawbm(ivx) = .false.
c
          end if
          call putxtn(mprint,iflop,multb,iud,wheadpt,
     *      poenom,nolev1(ivx),islope(ivx),slfac,xnlmid,islur(ivx,ipb1),
     *      lnote,notexq,ncmid(iv,ipb1),nlnum,eloff(ivx,nxtinbm(ivx)),
     *      iup,irest(ivx,ipb1),mult(ivx,ipb1),usexnumt)
        end if
        if (.not.drawbm(ivx)) then
c
c  Xtuplet with no beam
c
          if (.not.btest(islur(ivx,ipb1),31)) then
c
c  Number printing has not been suppressed, so put in the bracket. 
c    scale = stretch factor for bracket if there are asx's
c    xnsk = length of the bracket in \noteskips = (\elemskips)/(eon)
c
            xnsk = (eskz2(ivx,ipb1+ntupv(ivx,nxtinbm(ivx))-1)
     *          -eskz2(ivx,ipb1))/squez(ib)/feon(space(ib)/squez(ib))
            if (iup .eq. 1) then
              if (lnote .gt. 0) then
                notexq = notexq(1:lnote)//sq//'ovbkt'
              else
                notexq = sq//'ovbkt'
              end if
            else
              if (lnote .gt. 0) then
                notexq = notexq(1:lnote)//sq//'unbkt'
              else
c
c  Introduced 12/5/98, req'd due to possible presence of in-line TeX
c
                notexq = sq//'unbkt'
              end if
            end if
            lnote = lnote+6
            if (iline.eq.1) then
              smed = smed/(1.-fracindent)
            end if
            xslope = 1.8*smed*slfac
            islope(ivx) = nint(xslope)
            nolev1(ivx) = nlnum - nint(smed*eloff(ivx,1))
            if (islope(ivx) .eq. 0) nolev1(ivx) = nolev1(ivx)-1
            if (iup .eq. 1) nolev1(ivx) = nolev1(ivx)+4
            levbracket = nolev1(ivx)
            if (iup.eq.1 .and. cstuplet) levbracket = levbracket-1
            call notefq(noteq,lnoten,levbracket,ncmid(iv,ipb1))
            if (lnoten .eq. 1) call addblank(noteq,lnoten)
            notexq = notexq(1:lnote)//noteq(1:lnoten)//'{'
            lnote = lnote+lnoten+1
            if (xnsk .lt. 0.995) then
              write(notexq(lnote+1:lnote+4),'(i1,f3.2)')0,xnsk
              lnote = lnote+4
            else if (xnsk .lt. 9.995) then
              write(notexq(lnote+1:lnote+4),'(f4.2)')xnsk
              lnote = lnote+4
            else
              write(notexq(lnote+1:lnote+5),'(f5.2)')xnsk
              lnote = lnote+5
            end if
            notexq = notexq(1:lnote)//'}'
            lnote = lnote+1
            if (btest(mult(ivx,ipb1),4)) then
c
c Tweak slope of bracket
c
              islope(ivx) = islope(ivx)+igetbits(mult(ivx,ipb1),5,5)-16
            end if
            if (islope(ivx).lt.0 .or. islope(ivx).ge.10) then
              notexq = notexq(1:lnote)//'{'
              lnote = lnote+1
              if (islope(ivx) .lt. -9) then
                write(notexq(lnote+1:lnote+3),'(i3)')islope(ivx)
                lnote = lnote+3
              else
                write(notexq(lnote+1:lnote+2),'(i2)')islope(ivx)
                lnote = lnote+2
              end if
              notexq = notexq(1:lnote)//'}'
              lnote = lnote+1
            else
              write(notexq(lnote+1:lnote+1),'(i1)')islope(ivx)
              lnote = lnote+1
            end if
c
c  Done with bracket
c
          end if
          if (ixrest(ivx) .eq. 1) then
c
c  Put in the rest.  Possible problem: Rest is a spacing char, but between
c  beamstrt and beamn1 some non-spacing chars. are inserted.
c
c  130126 Deal with vertical shifts of rest starting xtuplet
c
            lrest = 3
            if (btest(islur(ivx,ip),29)) then
              restq = sq//'sk'
c            if (multb .eq. 0) then
            else if (multb .eq. 0) then
              restq = sq//'qp'
            else if (.not.drawbm(ivx).and.multb.eq.1) then
              restq = sq//'ds'
            else if (.not.drawbm(ivx).and.multb.eq.2) then
              restq = sq//'qs'
            else if (.not.drawbm(ivx).and.multb.eq.3) then
              restq = sq//'hs'
            else
              restq = sq//'hpause'
              lrest = 7
            end if
            if (btest(nacc(ivx,ipb1),18) .and.
     *             btest(nacc(ivx,ipb1),19)) then
c
c  VERY special case of rest at start of F-tuplet, needs dot
c
              restq = sq//'pt4'//restq(1:lrest)
              lrest = lrest+4
            end if

            nole = mod(nolev(ivx,ip)+20,100)-20
c            if (nole .eq. 0) then
            if (nole.eq.0 .or. btest(islur(ivx,ip),29)) then
c
c Rest blank or is not raised
c
              notexq = notexq(1:lnote)//restq
              lnote = lnote+lrest
            else
              if (abs(nole) .lt. 10) then
                tempq = chax(48+abs(nole))
                ltemp = 1
              else
                write(tempq(1:2),'(i2)')abs(nole)
                ltemp = 2
              end if
              if (nole .gt. 0) then
                tempq = sq//'raise'//tempq(1:ltemp)//sq//'internote'
              else
                tempq = sq//'lower'//tempq(1:ltemp)//sq//'internote'
              end if
              ltemp = 16+ltemp
              notexq = notexq(1:lnote)//tempq(1:ltemp)//restq(1:lrest)
              lnote = lnote+ltemp+lrest
            end if
c
c  No need to come back through this subroutine (as would if rest starts bar
c  & multb>0), so do not advance ibm1. But must check in beamn1 and do nothing.
c
            ixrest(ivx) = 0
          end if
          return
        end if
c
c  End if block for non-beamed xtup start...note we returned
c
        if (ixrest(ivx) .eq. 1) then
c
c  Insert rest at start of beamed xtup.  See above note for possible problem.
c  But first check if blank rest and if in forced beam (assuming xtuplet),
c    and if so, count rest from beginning, add \sk's AFTER starting beam '\ib*'
c
          if (btest(islur(ivx,ip),29) .and. btest(ipl(ivx,ip),30)) then
            nbrests = nbrests+1
            writebrests = .false.
          else
            nodur(ivx,ipb1) = 2**(4-multb)
            call notex(tempq,ltemp)
            if (lnote .gt. 0) then
              notexq = notexq(1:lnote)//tempq(1:ltemp)
            else
              notexq = tempq(1:ltemp)
            end if
            lnote = lnote+ltemp
          end if
c
c  Re-zero just in case!
c
          nodur(ivx,ipb1) = 0
          ibm1(ivx,ibc) = ibm1(ivx,ibc)+1
c
c  See if next note is a non-rest
c
          if (.not.btest(irest(ivx,ipb1+1),0)) then
            ixrest(ivx) = 2
          else
c
c  Suppress reprinting xtup number next time through beamstrt
c
            islur(ivx,ipb1+1) = ibset(islur(ivx,ipb1+1),31)
c
c  Set new xtup start flag
c
            irest(ivx,ipb1+1) = ibset(irest(ivx,ipb1+1),28)
          end if
          return
        end if
      end if
c
c  Just ended if block for xtups
c
      if (vxtup(ivx) .and. ipb1.eq.ibm2(ivx,ibc)) then
c
c  Move actual note writing to beamend
c
        ixrest(ivx) = 4
        return
      end if
      if (issb(ivx) .eq. 0) then
c
c  1st bmstrt in single-slope bm grp, Adjust start level(s) and slope if needed
c
        iadj = igetbits(ipl(ivx,ipb1),6,11)-30
        if (iadj .ne. -30) then 
          nolev1(ivx) = nolev1(ivx)+iadj
          do 2 isssb = 1 , nssb(ivx)
            lev1ssb(ivx,isssb) = lev1ssb(ivx,isssb)+iadj
2         continue
        end if
        iadj = igetbits(ipl(ivx,ipb1),6,17)-30
        if (iadj .ne. -30) then
          islope(ivx) = islope(ivx)+iadj
          if (abs(islope(ivx)) .gt. 9) islope(ivx) = sign(9,islope(ivx))
          if (nssb(ivx) .gt. 0) then
c
c  Cycle thru non-rest notes in SSBG, looking for bmstrts.
c
            isssb = 0
            do 4 inb = 2, nnb
              if (btest(nacc(ivx,ipb(inb)),21)) then
c
c  Beam segment start.  New start level              
                isssb = isssb+1
                lev1ssb(ivx,isssb) = 
     *           lev1ssb(ivx,isssb)+islope(ivx)*xelsk(inb)/slfac
              end if
4           continue
          end if
        end if
      end if
      iadj = igetbits(islur(ivx,ipb1),2,27)
      addbrack = .false.
      if (btest(ipl(ivx,ipb1),30)) then
c
c  Check for altered starting polarity.  Only in forced beams. Nominal start
c  level is nolev1. So beam level is nolev1 +/- 6, to be compared w/ nolev(.,.).
c
        if (ulq(ivx,ibc).eq.'u' .and.
     *                  nolev1(ivx)+6.lt.nolev(ivx,ipb1)) then
          if (lnote .eq. 0) then
            notexq = sq//'loff{'
          else
            notexq = notexq(1:lnote)//sq//'loff{'
          end if
          lnote = lnote+6
          addbrack = .true.
        else if (ulq(ivx,ibc).eq.'l' .and.
     *                  nolev1(ivx)-6.gt.nolev(ivx,ipb1)) then
          if (lnote .eq. 0) then
            notexq = sq//'roff{'
          else
            notexq = notexq(1:lnote)//sq//'roff{'
          end if
          lnote = lnote+6
          addbrack = .true.
        end if
c
c  Check end level for possible flipping in forced beam.  Have to do it 
c  here since with multiple voices, xelsk will not be preserved.
c
        if (ulq(ivx,ibmcnt(ivx)) .eq. 'u') then
          bmlev = nolev1(ivx)+6+islope(ivx)*xelsk(nnb)/slfac
          flipend(ivx) = bmlev.lt.nolev(ivx,ibm2(ivx,ibc))
        else if (ulq(ivx,ibmcnt(ivx)) .eq. 'l') then
          bmlev = nolev1(ivx)-6+islope(ivx)*xelsk(nnb)/slfac
          flipend(ivx) = bmlev.gt.nolev(ivx,ibm2(ivx,ibc))
        end if
      end if
      multbb = multb+iadj
c
c Tremolo starting?
c
      if (btest(irest(ivx,ipb1),2)) multbb = 
     *     igetbits(irest(ivx,ipb1),2,3)
      call ntrbbb(multbb,'i',ulq(ivx,ibc),ivx,notexq,lnote)
c
c   Put in name of start level and slope, after correcting nolev1 if xtup
c   started with a rest.
c
      if (ixrest(ivx).eq.2) nolev1(ivx) = 
     *          nint(nolev1(ivx)+xelsk(1)*islope(ivx)/slfac)
c
c  Add adjustment nvtrem to main beam to account for tremolo indented beams
c
      call notefq(noteq,lnoten,nolev1(ivx)+nvtrem,ncmid(iv,ipb1))
      if (islope(ivx) .lt. 0) then
        notexq = notexq(1:lnote)//noteq(1:lnoten)//'{'
        lnote = lnote+4+lnoten
        write(notexq(lnote-2:lnote),'(i2,a1)')islope(ivx),'}'
      else
        notexq = notexq(1:lnote)//noteq(1:lnoten)
        lnote = lnote+1+lnoten
        write(notexq(lnote:lnote),'(i1)')islope(ivx)
      end if
c
c  Check for beam-thk fine-tuning
c
      if (iadj .gt. 0) then
        do 1 imp = multb+iadj , multb+1 , -1
          call ntrbbb(imp,'t',ulq(ivx,ibc),ivx,notexq,lnote)
1       continue
      end if
c
c  If we shifted, must close with right bracket
c
      if (addbrack) then
        notexq = notexq(1:lnote)//'}'
        lnote = lnote+1
      end if
c
c  Add in \sk's for very special case of staff-crossing xtup chords
c  Assumes we are in lower (first) voice of up-to-both beamed xtup
c    that starts with blank rests (notes in upper voice here).
c
      if (nbrests.gt.0 .and. writebrests) then
        do 7 isk = 1 , nbrests
          notexq = notexq(1:lnote)//chax(92)//'sk'
          lnote = lnote+3
7       continue
        nbrests = 0
      end if
c
c  Get 'floor' zmin for figures
c  Note: Will not come thru here on 1st note of unbeamed xtup, so figure height
c    won't be adjusted. If anyone ever needs that, need to duplicate this
c    functionality up above, before exiting.
c
      if (figbass .and. (ivx.eq.1 .or. ivx.eq.ivxfig2)) then
        if (ivx .eq. 1) then
          ivf = 1
        else
          ivf = ivxfig2
        end if
        zmult = 1.2*(multb-1)
        ymin = 100.
        do 3 inb = 1, nnb
          if (isfig(ivf,ipb(inb))) then
            if (ulq(iv,ibc) .eq. 'u') then
              ybot = nolev(iv,ipb(inb))
            else
              ybot = islope(ivx)/slfac*xelsk(inb)+nolev1(ivx)
     *                -stemlen-zmult
            end if
            ymin = min(ymin,ybot)
          end if
3       continue
        maxdrop = ncmid(iv,ipb1)-4-ymin+5.01
        ifigdr(ivf,iline) = max(ifigdr(ivf,iline),maxdrop)
      end if
c
c  Compute ornament levels if needed
c
      NomOrnLev = ncmid(iv,ipb1)+5
      iorn = 0
      do 8 inb = 1 , nnb
        ip = ipb(inb)
        if (.not.btest(iornq(ivx,ip),23)) go to 8
        if (btest(irest(ivx,ip),26) .and. ulq(ivx,ibc).eq.'l') then
c
c  letter-dynamic or hairpin ending under down-beamed 
c
          iorn = iorn+1
          ybeam = nolev1(ivx)-stemlen+islope(ivx)*xelsk(inb)/slfac+1
     *                  -1.2*(multb-1)
          ihornb(ivx,iorn) = min(nint(ybeam-3.),NomOrnLev-10)
        else if (.not.btest(ipl(ivx,ip),10)) then
c
c  Bits 0-13: (stmgx+Tupf._) , 14: Down fermata, was F
c  15: Trill w/o "tr", was U , 16-18 Editorial s,f,n , 19-21 TBD
c
c  Non-chord.  There IS an ornament.  Need ihornb only if upbeam, and if
c  ornament is 1,2,3,5,6,7,8,9,10,15-21 (up- but not domn ferm.)
c
          if (ulq(ivx,ibc).eq.'u' .and.
     *          iand(iornq(ivx,ipb(inb)),4163566).gt.0) then
            iorn = iorn+1
            iornq(ivx,ip) = ibset(iornq(ivx,ip),22)
            ybeam = nolev1(ivx)+stemlen+islope(ivx)*xelsk(inb)/slfac-1
     *                  +1.2*(multb-1)
            ihornb(ivx,iorn) = max(nint(ybeam+3.),NomOrnLev)
          end if
        else
c
c In a chord.  Orn may be on main note or non-main or both.  Set ihornb if
c upbeam and highest note has orn, or down beam and lowest.  Find 1st chord note
c
          do 10 icrd1 = 1 , ncrd
            if (iand(255,icrdat(icrd1)) .eq. ip  .and.
     *           iand(15,ishft(icrdat(icrd1),-8)) .eq. ivx) go to 11
10        continue
11        continue
c
c Find outermost note, min or max depending on beam direction ulq.  xto is true
c if there's an ornament on that note.  Expand orn list to include ._, since if
c on extreme chord note in beam, will move.
c So ornaments are all except 0,4,13 (,g,)
c
          levx = nolev(ivx,ip)
          xto = iand(iornq(ivx,ipb(inb)),4186094).gt.0
          icrdx = 0
          do 12 icrd = icrd1 , ncrd
            if (iand(255,icrdat(icrd)) .ne. ip  .or.
     *               iand(15,ishft(icrdat(icrd),-8)) .ne. ivx) go to 13
            levc = iand(127,ishft(icrdat(icrd),-12))
            if ((ulq(ivx,ibc).eq.'u'.and.levc.gt.levx) .or.
     *            (ulq(ivx,ibc).eq.'l'.and.levc.lt.levx)) then
              levx = levc
              icrdx = icrd
              xto = iand(icrdorn(icrd),4186094).gt.0
            end if
12        continue
13        continue
c
c  If there's orn on extreme note, do stuff
c
          if (xto) then
            iorn = iorn+1
            if (ulq(ivx,ibc) .eq. 'u') then
              ybeam = nolev1(ivx)+stemlen+islope(ivx)*xelsk(inb)/slfac-1
     *                  +1.2*(multb-1)
              ihornb(ivx,iorn) = max(nint(ybeam+3.),NomOrnLev)
            else
              ybeam = nolev1(ivx)-stemlen+islope(ivx)*xelsk(inb)/slfac+1
     *                  -1.2*(multb-1)
              ihornb(ivx,iorn) = min(nint(ybeam-3.),NomOrnLev-10)
            end if
            if (icrdx .eq. 0) then
c
c  Affected ornament is on main note
c
              iornq(ivx,ip) = ibset(iornq(ivx,ip),22)
            else
              icrdorn(icrdx) = ibset(icrdorn(icrdx),22)
            end if
          end if
        end if
8     continue
c
c  Henceforth nornb will be a counter.
c
      if (iorn .gt. 0) nornb(ivx)=1
      if (ixrest(ivx) .eq. 2) ixrest(ivx) = 0
      return
      end
      block data
	parameter (nm=24)
      common /comtol/ tol
      common /comtrans/ cheadq
      common /compage/ widthpt,ptheight,hoffpt,voffpt,
     *      nsyst,nflb,ibarflb(0:40),
     *      isysflb(0:40),npages,nfpb,ipagfpb(0:18),isysfpb(0:18),
     *      usefig,fintstf,gintstf,fracsys(30),nmovbrk,isysmb(0:30),
     *      nistaff(0:40)
      common /cblock/
     *      etatop,etabot,etait,etatc,etacs1,hgtin,hgtti,hgtco,
     *      xilbn,xilbtc,xilhdr,xilfig,a,b,inhnoh
      common /cominbot/ inbothd
      logical usefig
      common /comstart/ facmtr
      character*60 cheadq
      character*120 instrq,titleq,compoq
      logical headlog
      common /comtitl/ instrq,titleq,compoq,headlog,inskip,ncskip,
     *    inhead
      common /spfacs/ grafac,acgfac,accfac,xspfac,xb4fac,clefac,emgfac,
     *                flagfac,dotfac,bacfac,agc1fac,gslfac,arpfac,
     *                rptfac,lrrptfac,dbarfac,ddbarfac,dotsfac,upstmfac,
     *                rtshfac
      common /combmh/ bmhgt,clefend
      common /comdyn/ ndyn,idyndat(99),levdsav(nm),ivowg(12),hoh1(12),
     *        hoh2(12),hoh2h1(2),ntxtdyn,ivxiptxt(41),txtdynq(41),
     *        idynda2(99),levhssav(nm),listcresc,listdecresc
      character*128 txtdynq
      logical kbdrests
      common /comkbdrests/ levbotr(8),levtopr(8),kbdrests
      common /comInstTrans/ iInstTrans(nm),iTransKey(nm),iTransAmt(nm),
     *  instno(nm),nInstTrans,EarlyTransOn,LaterInstTrans
      logical EarlyTransOn,LaterInstTrans
      common /comsize/ isize(nm)
c
      data tol /.001/
      data cheadq 
     *  /'                                                            '/
      data gslfac /9./
      data instrq,titleq,compoq,headlog /3*' ',.false./
c
c  meter space (pts) = xb4mbr = musicsize*facmtr
c
      data facmtr /0.55/
      data grafac,acgfac,accfac,xspfac,xb4fac,clefac,emgfac,
     *                flagfac,dotfac,bacfac,agc1fac,clefend,bmhgt
     * / 1.3333,.4   ,   .7 ,  .3  ,  0.2 ,  2.0 , 1.0 ,
     *               .7,.7 ,.9 ,.5 ,2.3 ,1.1  /
      data rptfac,lrrptfac,dbarfac,ddbarfac,dotsfac,upstmfac,arpfac
     *  /   1.32 ,  2.25   ,  0.47 , 0.83  ,   0.17 , 0.5 ,  1.7   /
      data rtshfac / 1.0 /
      data isize /nm*0/
c
c  From other
c
      data ptheight,widthpt,hoffpt,voffpt,
     *      etatop,etabot,etait,etatc,etacs1,hgtin,hgtti,hgtco,
     *      xilbn,xilbtc,xilhdr,xilfig,a,b,inbothd,inhnoh
     *   / 740. , 524. , 0., 0.,
     *   .50  ,  .25  , 0.4 , 0.4  , 0.2 , 12. ,21. , 12.,
     *    4   ,   1.6 ,5.,5.7,1.071,2.714,16 ,16 /
      data fracsys /30*0./
	data ivowg,hoh1,hoh2,hoh2h1 /0,0,0,0,0,1,1,1,1,1,1,1,
c     *   2.0,1.5,1.0,0.5,1.3,1.3,0.4,0.8,1.2,0.8,1.2,1.6,
     *   2.2,1.7,1.2,0.7,1.3,1.3,0.4,0.8,1.2,0.8,1.2,1.6,
     *   -2.7,-2.2,-1.7,-1.2,-2.3,-2.1,-1.0,-1.7,-2.1,-1.6,-1.9,-2.3,
     *   -0.3,0.3 / 
      data kbdrests /.false./, 
     *       levtopr /9,7,5,5,7,5,6,6/, levbotr /0,0,0,2,1,4,5,4/
      data EarlyTransOn, LaterInstTrans /2*.false./
      data iInstTrans /nm*0/
      data iTransAmt /nm*0/
      end
      subroutine catspace(space,squez,nnsk)
      parameter (nkb=3999,maxblks=9600)
      common /c1omnotes/ nnodur,wminnh(nkb),nnpd(maxblks),durb(maxblks),
     *     iddot,nptr(nkb),ibarcnt,mbrest,ibarmbr,
c     *     ibaroff,udsp(nkb),wheadpt,gotclef,sqzb(maxblks)
     *     ibaroff,udsp(nkb),wheadpt,sqzb(maxblks)
      common /comtol/ tol
c      logical gotclef
      do 16 iptr = nptr(ibarcnt) , nptr(ibarcnt+1)-1
        if (abs(space-durb(iptr)) .lt. tol) then
          if (abs(squez-sqzb(iptr)) .lt. tol) then
c
c  Increment pre-existing entry
c
            nnpd(iptr) = nnpd(iptr)+nnsk
            return
          end if
        end if
16    continue
c
c  Didn't find current duration & squez, so add a new entry.
c  No particular reason to keep in order, so add at the end.
c
      nnpd(nptr(ibarcnt+1)) = nnsk
      durb(nptr(ibarcnt+1)) = space
      sqzb(nptr(ibarcnt+1)) = squez
      nptr(ibarcnt+1) = nptr(ibarcnt+1)+1
      return
      end
      character*1 function chax(n)
c
c The only reason for this seemingly do-nothing function is to get around an
c  apparent bug in the Visual Fortran Standard Edition 5.0.A compiler!
c
      chax = char(n)
      return
      end
      subroutine checkdyn(lineq,iccount,ibar)
      character*128 lineq
      character*1 durq,chax
      character*4 dynsymq
      logical txtdyn
      txtdyn = .false.
c
c  On entry, iccount is on "D"
c
      if (lineq(iccount+1:iccount+1) .eq. '"') then
c
c  Dynamic text
c 

        istart = iccount+2  ! 1 past 1st quote
3       continue
        iend = index(lineq(istart:128),'"')
        if (lineq(istart+iend-2:istart+iend-2) .eq. '\') then
          istart = iccount+iend+2
          go to 3
        end if
        if (iend .eq. 0) then
          call errmsg(lineq,iccount+1,ibar,
     *         'Dynamic text must be terminated with double quote!')
          call stop1()
        end if
c
c  Set iccount to character after 2nd ", and set ipm
c
c        iccount = iccount+iend+2 
        iccount = istart+iend 
        ipm = index('- +',lineq(iccount:iccount))
        if (ipm .eq. 0) then
          call errmsg(lineq,iccount,ibar,
     *                'Expected "-", "+", or blank here!')
          call stop1()
        end if
      else
c
c  Expect ordinary dynamic
c
        do 1 iend = iccount+2 , 128
          ipm = index('- +',lineq(iend:iend))
          if (ipm .gt. 0) go to 2
1       continue
2       continue
        if (iend-iccount.gt.5 .or. iend-iccount.lt.2) then
          call errmsg(lineq,iend-1,ibar,
     *                'Wrong length for dynamic mark!')
          call stop1()
        end if
        read(lineq(iccount+1:iend-1),'(a'//chax(47+iend-iccount)//')')
     *     dynsymq
        idno = (index(
     *    'ppppppp pp  p   mp  mf  f   fp  sfz ff  fff ffff<   >   ',
     *         dynsymq)+3)/4
        if (idno .eq. 0) then
          call errmsg(lineq,iccount+1,ibar,'Illegal dynamic mark!')
          call stop1()
        end if
        iccount = iend
      end if
      if (ipm .ne. 2) then
c
c  There is a vertical shift, have "+" or "-"
c
        iccount = iccount+1
        if (index('0123456789',lineq(iccount:iccount)) .eq. 0) then
          call errmsg(lineq,iccount,ibar,
     *       'Expected integer here for vertical offset!')
          call stop1()
        end if
        call readnum(lineq,iccount,durq,fnum)
        idno = nint(fnum)
        if (idno .gt. 63) then
          call errmsg(lineq,iccount-1,ibar,
     *      'Vertical offset for dynamic mark must be (-63,63)!')
          call stop1()
        end if
        ipm = index('- +',durq)
        if (ipm .eq. 0) then
          call errmsg(lineq,iccount,ibar,
     *      'Expected "+", "-", or blank here!')
          call stop1()
        end if
        if (ipm .ne. 2) then
c
c  There is a horizontal shift
c
          iccount = iccount+1
          if (index('.0123456789',lineq(iccount:iccount)) .eq. 0) then
            call errmsg(lineq,iccount,ibar,
     *       'Expected number here for horizontal offset!')
            call stop1()
          end if
          call readnum(lineq,iccount,durq,fnum)
          idno = nint(10*fnum)
          if (idno .gt. 255) then
            call errmsg(lineq,iccount-1,ibar,
     *       'Horizontal offset for dynamic mark must be (-25.5,25.5)!')
            call stop1()
          else if (durq .ne. ' ') then
            call errmsg(lineq,iccount,ibar,
     *        'There should be a blank here!')
            call stop1()
          end if
        end if
c
c  iccount should be on the blank at the end of the entire symbol
c
      end if
      return
      end
      subroutine chkarp(ncrd,icrdat,ivx,ip,iscacc,isarp)
c      subroutine chkarp(found1,ncrd,icrdat,icrdot,ivx,ip,isacc,isarp,
c     *                  icashft)
      logical found1,iscacc,isarp,btest
      integer icrdat(193)
      found1 = .false.
c
c  icashft will be max left shift of accid's in chord notes.
c  Used only for spacing checks.
c  Will include left shift of chord note itself.
c  Rezero after use.
c
      do 18 icrd = 1 , ncrd
c
c  This if block cycles thru all chord notes on ivx,ip; then returns.
c
        if (iand(255,icrdat(icrd)) .eq. ip  .and.
     *          iand(15,ishft(icrdat(icrd),-8)) .eq. ivx) then
          found1 = .true.
          iscacc = iscacc .or. 
     *       (btest(icrdat(icrd),19).and..not.btest(icrdat(icrd),27)) 
c
c  Accid on this chord note, and it's not midi-only.
c
c            irshft = igetbits(icrdot(icrd),7,20)
cc
cc  Include increment for notehead shift
cc
c            if (btest(icrdat(icrd),23)) then
c              if (irshft .eq. 0) then
c                irshft = 44
c              else
c                irshft=irshft-20
c              end if
c            end if
c            if (irshft .ne. 0) then
cc
cc  Accid on chord note is shifted.  Include only left shift, in 20ths.
cc
c              if (irshft .lt. 64) icashft = max(icashft,64-irshft)
c            end if
c          end if
          isarp = isarp .or. btest(icrdat(icrd),25)
        else if (found1) then
          return
        end if
18    continue
      return
      end
      subroutine chkimidi(icm)
      parameter (nm=24,mv=24576)
      integer*2 mmidi
      logical restpend,relacc,notmain,twoline,ismidi,crdacc
      common /commidi/ imidi(0:nm),trest(0:nm),mcpitch(20),mgap,
     *       iacclo(0:nm,6),iacchi(0:nm,6),midinst(nm),
     *       nmidcrd,midchan(nm,2),numchan,naccim(0:nm),
     *       laccim(0:nm,10),jaccim(0:nm,10),crdacc,notmain,
     *       restpend(0:nm),relacc,twoline(nm),ismidi,mmidi(0:nm,mv),
     *       debugmidi
      logical debugmidi
      if (imidi(icm) .ge. mv) then
        print*
        print*,'Midi file is too long! It will be corrupted or worse'
        write(*,'(a6,2x,4i8)')
     *        'imidi:',imidi(0),imidi(1),imidi(2),imidi(3)
      end if
      return
      end
      subroutine chkkbdrests(ip,iv,ivx,nn,islur,irest,nolev,ivmx,
c     * nib,nv,ibar,tnow,tol,nodur,mode,levtopr,levbotr,mult)
     * nib,nv,ibar,tnow,tol,nodur,mode,levtopr,levbotr,mult,ipl)
      parameter (nm=24)
      integer*4 nn(nm),islur(nm,200),irest(nm,200),
     * nolev(nm,200),nib(nm,15),nodur(nm,200),levbotr(8),levtopr(8),
     * ivmx(nm,2),mult(nm,200),ipl(nm,200)
c
c  On 130127 put this code, formerly in make2bar right before calling notex for
c  a single note/rest, into this subroutine, so the same logic could also be
c  with the calls to beamstrt/mid/end to adjust height of rests in xtups if the
c  keyboard rest option is selected
c
c  mode=1 if called as before, 2 if for an xtup. Only affects check for
c    quarter rests, which will fix later.
c
c  Get reference level: next following note if no intervening blank rests, 
c    otherwise next prior note. Relative to bottom line.
c
c      if (ip.ne.nn(ivx).and..not.btest(iornq(ivx,ip),30)) then
      if (ip.ne.nn(ivx).and..not.btest(ipl(ivx,ip),1)) then
c
c  Not the last note and not "look-left" for level
c
        do 8 kkp = ip+1 , nn(ivx)
          if (btest(islur(ivx,kkp),29)) go to 4
          if (.not.btest(irest(ivx,kkp),0)) then
            levnext = nolev(ivx,kkp)-ncmid(iv,kkp)+4 ! Relative to bottom line
            go to 9
          end if
8       continue
      end if
4     continue
c
c  If here, there were no following notes or came to a blank rest, or
c    "look-left" option set. So look before
c
c      if (ip .eq. 1) go to 2 ! Get out if this is the first note.
      if (ip .eq. 1) return ! Get out if this is the first note.
      do 3 kkp = ip-1, 1, -1
        if (.not.btest(irest(ivx,kkp),0)) then
          levnext = nolev(ivx,kkp)-ncmid(iv,kkp)+4 ! Relative to bottom line
          go to 9
        end if
3     continue
c      go to 2  ! Pretty odd, should never be here, but get out if so.
      return  ! Pretty odd, should never be here, but get out if so.
9     continue
c
c  Find note in other voice at same time
c
      iupdown = sign(1,ivx-nv-1)
      ivother = ivmx(iv,(3-iupdown)/2)
      tother = 0.
      do 5 kkp = 1 , nib(ivother,ibar)
        if (abs(tother-tnow) .lt. tol) go to 6
        tother = tother+nodur(ivother,kkp)
5     continue
c
c  If here, then no note starts in other voice at same time, so set default
c
      levother = -iupdown*50
      go to 7
6     continue
c
c  If here, have just identified a simultaneous note or rest in other voice
c
      if (.not.btest(irest(ivother,kkp),0)) then ! Not a rest, use it
        levother = nolev(ivother,kkp)-ncmid(iv,ip)+4
      else
        if (nodur(ivother,kkp) .eq. nodur(ivx,ip)) then
c
c  Rest in other voice has same duration, get out (so defualt spacing is used)
c
c          go to 2
          return
        end if
        levother = -iupdown*50
      end if
7     continue
      if (mode.eq.1) then
        indxr = log2(nodur(ivx,ip))+1
      else
c        nodu = 2**(4-(iand(mult(ivx,ip),15)-8))
        indxr = 4-(iand(mult(ivx,ip),15)-8)+1      
      end if
      if (iupdown .lt. 0) then
        levtop = levtopr(indxr)
        iraise1 = levother-levtop-3  ! Based on other note
        iraise2 = levnext-levtop     ! Based on following note
        if (indxr.eq.5 .and. levnext.lt.1) iraise2=iraise2+2
        iraise = min(iraise1,iraise2)
        if (mod(iraise+50,2).eq.1 .and. 
     *                iraise+levtop.gt.-1) iraise = iraise-1
      else
        levbot = levbotr(indxr)
        iraise1 = levother-levbot+3
        iraise2 = levnext-levbot
        if (indxr.eq.5 .and. levnext.gt.8) iraise2=iraise2-1
        iraise = max(iraise1,iraise2)
        if (mod(iraise+50,2).eq.1 .and. 
     *                iraise+levbot.le.9) iraise = iraise-1
      end if
      nolev(ivx,ip) = 100+iraise
      return
      end
      subroutine chklit(lineq,iccount,literr)
      character*128 lineq
      character*1 charq,chax
      literr = 0
      itype = 1
17    call g1etchar(lineq,iccount,charq)
      if (charq .eq. chax(92)) then
        itype = itype+1
        if (itype .gt. 3) then
          literr = 1
          return
        end if
        go to 17
      end if
      lenlit = itype
18    call g1etchar(lineq,iccount,charq)
      if (charq.eq.chax(92)) then
        call g1etchar(lineq,iccount,charq)
        if (charq .ne. ' ') then
c
c  Starting a new tex command withing the string
c
          lenlit = lenlit+2
          if (lenlit .gt. 128) then
            literr = 2
            return
          end if
          go to 18
        end if
      else
        lenlit = lenlit+1
        if (lenlit .gt. 128) then
          literr = 2
          return
        end if
        go to 18
      end if
      return
      end
      subroutine chkpm4ac(lineq,iccount,nacc,moved)
c
c  Called after getting +/-/</> in a note (not rest).  iccount is on the +-<>.
c  Sets moved=.true. and sets move parameters in nacc if necc: horiz only (bits
c    10-16) if < or >,  horiz and vert (bits 4-9) if two consecutive signed
c    numbers.  If moved=.true., iccount on exit is on end of last number.
c    If moved=.false., iccount still on +/-
c
      logical moved,ishorz
      character*128 lineq
      character*1 durq
      if (index('sfnA',lineq(iccount-1:iccount-1)).gt.0 .and.
     *      index('0123456789.',lineq(iccount+1:iccount+1)).gt.0) then
c
c  Prior char was accid & next is #; this may be start of accidental shift.
c  Must test for "." above in case we get "<" or ">"
c
        ipm = index('- +< >',lineq(iccount:iccount))-2
        if (lineq(iccount+1:iccount+1).eq.'.' .and.
     *      index('0123456789',lineq(iccount+2:iccount+2)).eq.0) then
c
c  Rare case of [accid][+/-].[letter].  Bail out
c
          moved = .false.
          return
        end if
        ishorz = ipm .gt. 1
c
c  Save iccount in case it's not accid shift and we have to reset.
c
        icsav = iccount
        iccount = iccount+1
        call readnum(lineq,iccount,durq,fnum)
        if (ishorz .or. index('+-',durq).gt.0) then
c
c  This has to be accidental shift.  Set vert. shift.
c
          if (.not.ishorz) then
c
c  +/- syntax, both shifts set, vertical first
c
            call setbits(nacc,6,4,int(ipm*fnum+32.5))
            ipm = index('- +',durq)-2
            iccount = iccount+1
            call readnum(lineq,iccount,durq,fnum)
          else
c
c  </> syntax, only horiz set
c
            ipm = ipm-3
          end if
c
c  Set horiz. shift
c
          call setbits(nacc,7,10,nint(20*(ipm*fnum+5.35)))
          iccount = iccount-1
          moved = .true.
        else
c
c  False alarm.  Reset everything and flow onward
c
          moved = .false.
          iccount = icsav
        end if
      else
c
c Either prior char was not 'sfn' or next was not digit, so take no action
c
        moved = .false.
      end if
      return
      end
      subroutine clefsym(isl,notexq,lnote,nclef)
c
c  Returns string calling Don's TeX macro \pmxclef, for drawing small clefs.
c
      character*(*) notexq
      character*1 chax
        nclef = iand(ishft(isl,-12),7)
        if (nclef .eq. 0) then
c
c  treble
c
          nlev = 2
        else if (nclef .gt. 6) then
c
c  French violin
c
          nlev = 0
        else if (nclef .lt. 5) then
c
c  C-clef
c
          nlev = 2*nclef-2
        else
c
c  F-clef
c
          nlev = 2*nclef-6
        end if
        notexq = chax(92)//'pmxclef'//chax(48+min(nclef,7))
     *             //chax(48+nlev)
        lnote = 10
      return
      end
      subroutine crdacc(nacc,naccid,micrd,nolevm,rmsshift,lasttime,
     *       levmaxacc,icrdot0,segrb0,ksegrb0,nsegrb0,twooftwo,icashft)  
c
c       nacc = accidental bitmap for main note
c       naccid = # of accid's in chord
c       micrd = array with icrd #'s for notes w/ acc's, 0=>main note
c       nolevm = level of main note
c       segrb(1|2,.) x|y-coord of right-bdry segment
c       ksegrb(.) internal use; tells what defined this segment
c           -2: Left-shifted notehead
c           -1: Original right boundary
c            0: Main note accidental
c         icrd: Chord-note accidental        
c       isetshft(i),i=1,naccid: what set shift for this accid, same codes
c       icrdot0 = top-down level-rank of main note among accid-notes
c       icrdot(icrd)(27-29) = level rank of chord-note among accid-notes
c       twooftwo will be true 2nd time thru; signal to store shifts w/ notes
c
      common /comtrill/ ntrill,ivtrill(24),iptrill(24),xnsktr(24),
     *                ncrd,icrdat(193),icrdot(193),icrdorn(193),nudorn,
     *                kudorn(63),ornhshft(63),minlev,maxlev,icrd1,icrd2
      real*4 segar(5,2,6),segal(5,2,6),segrb(2,50),segrb0(2,50)
      integer*4 nsegar(5),nsegal(5),micrd(10),iacctbl(6),ksegrb(50),
     *          isetshft(10),ksegrb0(50)
      logical lasttime,mainnote,twooftwo
      data nsegar / 3,4,3,3,2 /, nsegal / 2,4,3,3,2 /
      data segar /
c
c  Fancy sharp boundary. fl,sh,na,dfl,dsh
c
     * -0.05,-0.38,-0.34,-.05, -.15,-1.4,-2.9,-3.0, -1.4, -1.2,   
c     * -0.75,-0.20,-0.80, 0. , 0. , .96,-1.04,1.6, 0. , 0. ,   
c           meas value for y, natural is 1.6
     * -0.75,-0.20,-0.80,-.75,   0. , .96,-1.04,1.48, .96, 1.2,   
     *  0.00,-0.38, 0.00, 0. ,   0. ,3.15, 1.64,3.0, 3.15, 0. ,   
     * 0. ,   0.00, 0. ,  0. ,   0. , 0. , 2.90, 0. , 0. , 0. ,
     * 0. ,   0.  , 0. ,  0. ,   0. , 0. , 0. ,  0. , 0. , 0. ,  
     * 0. ,   0.  , 0. ,  0. ,   0. , 0. , 0. ,  0. , 0. , 0. /
      data segal /
c     *  0.00, 0.00,-1.04, 0. , 0. ,3.15, 2.9,-1.6, 0. , 0. ,
cc		(meas. value is 3.08)    ^^^^
cc  Raise top of flat so it interferes with bottom of sharp        
     * -1.00,-1.02,-0.60,-1.65, -1.2,-1.4,-2.9, -3.0, -1.4, -1.2,   
     *  0.00,-1.20,-1.04, 0.  ,  0. ,3.15,-1.64,-1.48,3.15, 1.2,
     *  0.  ,-1.02, 0.00, 0.  ,  0. , 0. , 1.04, 3.0,  0. , 0. ,   
     *  0. ,  0.00, 0. ,  0.  ,  0. , 0. , 2.9 , 0. ,  0. , 0. ,  
     *  0. ,  0.  , 0. ,  0.  ,  0. , 0. , 0. ,  0. ,  0. , 0. ,  
     *  0. ,  0.  , 0. ,  0.  ,  0. , 0. , 0. ,  0. ,  0. , 0. /
c
c  iacctbl(i) = internal accid # (1-5) when i=extern accid # (1,2,3,5,6)
c
      data iacctbl / 1 , 2 , 3 , 0 , 4 , 5 /
c
c  Set up barrier segrb(iseg,ipoint) to define coords of corner points 
c    on stem+notes
c
      do 11 iseg = 1 , nsegrb0
        segrb(1,iseg) = segrb0(1,iseg)
        segrb(2,iseg) = segrb0(2,iseg)
        ksegrb(iseg) = ksegrb0(iseg)
11    continue
      nsegrb = nsegrb0
      rmsshift = 0.
      shiftmin = 1000.
	do 1 iwa = 1 , naccid
c
c  Initialize shift for this note
c
        shift = 0.
        mainnote = micrd(iwa).eq.0
        isetshft(iwa) = -1
c
c Get note level and accidental type
c
        if (mainnote) then
          nolev = nolevm
          iacctype = iacctbl(igetbits(nacc,3,0))
        else
          nolev = igetbits(icrdat(micrd(iwa)),7,12)
          iacctype = iacctbl(igetbits(icrdat(micrd(iwa)),3,20))
        end if
c
c  Cycle thru segments on right edge of this accidental
c
        do 2 isega = 1 , nsegar(iacctype)-1
          ybotaseg = nolev+segar(iacctype,2,isega)
          ytopaseg = nolev+segar(iacctype,2,isega+1)
c
c  Cycle thru segments of right-hand barrier
c
	    do 3 isegrb = 1 , nsegrb-1
c
c  Must find all barrier segments that start below ytopseg & end above ybotseg 
c
            if (segrb(2,isegrb) .lt. ytopaseg) then
c
c  Barrier seg starts below top of accid
c  Check if barrier seg ends above bottom of accid
c
              if (segrb(2,isegrb+1) .gt. ybotaseg) then
                if (shift .gt. 
     *                segrb(1,isegrb)-segar(iacctype,1,isega)) then
                  shift = segrb(1,isegrb)-segar(iacctype,1,isega)
c
c  Record the cause of the shift
c
                  isetshft(iwa) = ksegrb(isegrb)
                end if
              end if
c
c  Does barrier segment end above top of accid seg?
c
              if (segrb(2,isegrb+1) .gt. ytopaseg) go to 4
            end if
3         continue
4         continue
2       continue
        if (.not.btest(nacc,28) .and. abs(shift).gt.0.0001
     *       .and. .not.lasttime) then
c          if (nolev .eq. levmaxacc) then
          if (nolev.eq.levmaxacc .and. isetshft(iwa).eq.-1) then
            rmsshift = 1000.
            return
          end if
c
c  Does the following properly account for left-shifted noteheads?
c
c  Top-down rank of this note we just shifted
c
          if (mainnote) then 
            irank = icrdot0
          else 
            irank = igetbits(icrdot(micrd(iwa)),3,27)
          end if
c
c  Compare level-rank of this note vs. that of note that caused the shift. 
c    This has effect of checking for basic interferences from top down.
c
c       ksegrb(.) internal use; tells what defined this segment
c           -2: Left-shifted notehead
c           -1: Original right boundary
c            0: Main note accidental
c         icrd: Chord-note accidental        
c       isetshft(i),i=1,naccid: what set shift for this accid, same codes
c
          if (isetshft(iwa) .lt. 0) then
            iranksetter = 0
          else if (isetshft(iwa) .eq. 0) then
            iranksetter = icrdot0
          else
            iranksetter = igetbits(icrdot(isetshft(iwa)),3,27)
          end if
          if (iranksetter.ne.0 .and. irank.ne.iranksetter+1) then
            rmsshift = 1000.
            return
          end if
        end if
        rmsshift = rmsshift+shift**2        
        if (lasttime .and. abs(shift).gt..0001) then
          if (mainnote) then
            if (.not.btest(nacc,29)) go to 10
          else
            if (.not.btest(icrdat(micrd(iwa)),29)) go to 10
          end if
c
c  If here, "A" was set on a manual shift, so must cumulate the shift.  Note that if there
c    was a manual shift but auto-shift was zero, will not come thru here, but shift value
c    will be left intact.
c
          if (mainnote) then
            shift = shift+.05*(igetbits(nacc,7,10)-107)
          else
            shift = shift+.05*(igetbits(icrdot(micrd(iwa)),7,20)-107)
          end if
10        continue
          if (twooftwo) then
c
c  Record the shift for this accidental
c         
            if (shift .lt. -5.35) then
              call printl(' ')
              call printl('WARNING: auto-generated accidental '//
     *                               'shift too big for PMX, ignoring')
            else
              ishift = nint(20*(shift+5.35))
              if (mainnote) then
                call setbits(nacc,7,10,ishift)
              else
                call setbits(icrdot(micrd(iwa)),7,20,ishift)
              end if
            end if
          else
c
c  This is the earlier call to precrd, so need minimum shift
c
            shiftmin = min(shiftmin,shift)
          end if
        end if
c
c  Bail out if this is the last accidental to check
c
        if (iwa .eq. naccid) go to 1
c
c  Add this accidental to the right barrier! Count down from highest barrier segment,
c    find 1st one starting below top of accid, and first one starting below bot.
c  
        do 5 ibelowtop = nsegrb , 1 , -1
          if (segrb(2,ibelowtop) .lt. 
     *          nolev+segal(iacctype,2,nsegal(iacctype))) then
            do 9 ibelowbot = ibelowtop , 1 , -1
              if (segrb(2,ibelowbot) .lt. 
     *            nolev+segal(iacctype,2,1)) go to 6
9           continue
            print*,'Oops2!'
            call stop1()
          end if
5       continue
        print*,'Ugh0! in crdaccs'
        call stop1()
6       continue
        netgain = nsegal(iacctype)-ibelowtop+ibelowbot
c
c  Shift high segments up
c
        if (netgain .ge. 0) then
          do 7 isegrb = nsegrb , ibelowtop+1 , -1
            segrb(1,isegrb+netgain) = segrb(1,isegrb)
            segrb(2,isegrb+netgain) = segrb(2,isegrb)
            ksegrb(isegrb+netgain) = ksegrb(isegrb)
7         continue
c
c  Set up 1st segment above accid
c
          segrb(1,ibelowtop+netgain) = segrb(1,ibelowtop)
          segrb(2,ibelowtop+netgain) = 
     *         nolev+segal(iacctype,2,nsegal(iacctype))
          ksegrb(ibelowtop+netgain) = ksegrb(ibelowtop)
        else
c
c  netgain<0, must remove segments. Use same coding but reverse order,
c    work from bottom up
c
          segrb(1,ibelowtop+netgain) = segrb(1,ibelowtop)
          segrb(2,ibelowtop+netgain) = 
     *         nolev+segal(iacctype,2,nsegal(iacctype))
          ksegrb(ibelowtop+netgain) = ksegrb(ibelowtop)
          do 12 isegrb = ibelowtop+1 , nsegrb 
            segrb(1,isegrb+netgain) = segrb(1,isegrb)
            segrb(2,isegrb+netgain) = segrb(2,isegrb)
            ksegrb(isegrb+netgain) = ksegrb(isegrb)
12        continue
        end if  
c
c  Insert new segments
c
        do 8 isega = 1 , nsegal(iacctype)-1
          segrb(1,ibelowbot+isega) = shift+segal(iacctype,1,isega)
          segrb(2,ibelowbot+isega) = nolev+segal(iacctype,2,isega)
          if (mainnote) then 
            ksegrb(ibelowbot+isega) = 0
          else
            ksegrb(ibelowbot+isega) = micrd(iwa)
          end if
8       continue
c
c  Update number of barrier segments
c
        nsegrb = nsegrb+netgain
cc
cc  Temporary printout for boundary segments as built up
cc
c      write(15,'()')
c      write(15,'(a/(2f8.2,i5))')'  y       x       kseg',
c     *    (segrb(2,iseg),segrb(1,iseg),ksegrb(iseg),iseg=1,nsegrb)
c      write(15,'(a/(2i5))')' micrd isetshft',
c     *    (micrd(iwa1),isetshft(iwa1),iwa1=1,iwa)
cc
1     continue   ! next accidental 
      if (lasttime .and. .not.twooftwo) then
c
c  This is the final call on the pre-ask pass, so compute left-shift rqmt.
c
	  icashft = nint(-20*shiftmin)
      end if
cc
cc  Temporary printout for boundary segments
cc
c      if (twooftwo) then
c      write(15,'()')
c      write(15,'(a/(2f8.2,i5))')'  y       x       kseg',
c     *    (segrb(2,iseg),segrb(1,iseg),ksegrb(iseg),iseg=1,nsegrb)
c      write(15,'(a/(2i5))')' micrd isetshft',
c     *    (micrd(iwa),isetshft(iwa),iwa=1,naccid)
c      end if
cc
      return
      end
      subroutine crdaccs(nacc,ipl,irest,naccid,kicrd,nolevm,
     *                   levmaxacc,levminacc,icrdot0,twooftwo,icashft)
c
c       nacc = accidental bitmap for main note
c       naccid = # of accid's in chord
c       kicrd = array with icrd #'s for notes w/ acc's, 0=>main note
c       nolevm = level of main note
c
c  This is called once per multi-accidental chord.  In here, loop over all
c  permutations of the order of accidental as stored in kicrd.  Each time thru
c  loop, call crdacc once, get rms shift.  Only save permutation and rms value
c  if it is less than old value.
c
      common /comtrill/ ntrill,ivtrill(24),iptrill(24),xnsktr(24),
     *                ncrd,icrdat(193),icrdot(193),icrdorn(193),nudorn,
     *                kudorn(63),ornhshft(63),minlev,maxlev,icrd1,icrd2
c
c  Make consistent? 120106
c      integer*4 kicrd(7),iperm(7),micrd(10),ipermsav(7),ksegrb0(50)
      integer*4 kicrd(10),iperm(7),micrd(10),ipermsav(7),ksegrb0(50)
      real*4 segrb0(2,50)
      logical btest,tagged,manual,lshift,twooftwo
cc
cc  Temporary printout of level-rankings
cc
c      write(15,'()')
c      do 98 iacc = 1 , naccid
c        if (kicrd(iacc) .eq. 0) then
c          write(15,'(3i5)')nolevm,icrdot0
c        else
c          write(15,'(2i5)')igetbits(icrdat(kicrd(iacc)),7,12),
c     *                     igetbits(icrdot(kicrd(iacc)),3,27)
c        end if
c98    continue
cc
c
c  Initialize right-barrier
c
      segrb0(1,1) = 0.
      segrb0(2,1) = -1000.
      segrb0(1,2) = 0.
      segrb0(2,2) = 1000.
      nsegrb0 = 2
      ksegrb0(1) = -1
      ksegrb0(2) = -1
c
c  Search for left-shifted notes, Make up the initial right-barrier, which
c     will be good for all permutations.
c     irest()(27) is set if any notes are left-shifted
c     Must use ALL chord notes, not just ones w/ accid's.
c
      if (btest(irest,27)) then
        do 15 icrd = icrd1-1, icrd2
          if (icrd .eq. icrd1-1) then
c
c  Main note
c
c            lshift = btest(ipl,8) 
            lshift = btest(ipl,8) .or. btest(nacc,31) 
            if (lshift) nolev = nolevm
          else
c
c  Chord note
c
            lshift = btest(icrdat(icrd),23)
c            if (lshift) nolev = igetbits(icrdat(icrd),7,12)
            if (lshift) then
              nolev = igetbits(icrdat(icrd),7,12)
              if (btest(nacc,31) .and. nolev.eq.nolevm+1) then
c
c  This note is not really shifted, It is the upper of a 2nd with the main
c    note on an upstem, and Main note must be shifted. 
c    nacc(31) signals the real truth.
c
                lshift = .false.
              end if
            end if
          end if
          if (lshift) then
            do 16 isegrb = 1 , nsegrb0-1
              if (segrb0(2,isegrb+1) .gt. nolev-1) then
c
c  Add this notehead to the right boundary here.  Move all higher segs up 2.
c
                do 17 iiseg = nsegrb0 , isegrb+1 , -1
                  segrb0(1,iiseg+2) = segrb0(1,iiseg)   
                  segrb0(2,iiseg+2) = segrb0(2,iiseg)
                  ksegrb0(iiseg+2) = ksegrb0(iiseg)
17              continue
                go to 18
              end if
16          continue
18          continue
c
c  Insert notehead into list. Set kseg=-2 to signal notehead shift.
c
            iiseg = isegrb+1
            segrb0(1,iiseg) = -1.2
            segrb0(2,iiseg) = nolev-1.
            ksegrb0(iiseg) = -2
            segrb0(1,iiseg+1) = 0.
            segrb0(2,iiseg+1) = nolev+1.
            ksegrb0(iiseg+1) = -1
            nsegrb0 = nsegrb0+2
          end if
15      continue
      end if
c
c  Done setting right barrier for left-shifted noteheads
c
      tagged = .false.
      manual = .false.
c
c  Preprocess to check for manual shifts.
c   If are manual main [nacc(10-16)] or chord note [icrdot(20-26)]shifts, then
c      If any manual shift is preceded by "A" [nacc(29), icrdat(29)] then
c         1. Auto-shifting proceeds
c         2. "A"-shifts add to autoshifts
c         3. non-"A" shifts are ignored!
c      Else (>0 man shifts, none has "A")
c         No auto-ordering, No autoshifts, Observe all manual shifts.
c      End if
c   End if
c
	maxmanshft = 0
      do 13 i = 1 , naccid
        if (kicrd(i) .eq. 0) then
c
c  Main note
c		
          manshft = igetbits(nacc,7,10)
          if (manshft .ne. 0) then
            manual = .true.
            if (btest(nacc,29)) then
              tagged = .true.
            else
c              maxmanshft = max(maxmanshft,64-manshft)              
              maxmanshft = max(maxmanshft,107-manshft)              
            end if
          end if
        else
c
c  Chord note
c
          manshft = igetbits(icrdot(kicrd(i)),7,20)
          if (manshft .ne. 0) then
            manual = .true.
            if (btest(icrdat(kicrd(i)),29)) then
              tagged = .true.
            else
c              maxmanshft = max(maxmanshft,64-manshft)              
              maxmanshft = max(maxmanshft,107-manshft)              
            end if
          end if
        end if
13    continue
      if (manual) then
        if (tagged) then
c
c  zero out all untagged shifts
c
          do 14 i = 1 , naccid
            if (kicrd(i) .eq. 0) then
              if (.not.btest(nacc,29)) call setbits (nacc,7,10,0)
            else
              if (.not.btest(icrdat(kicrd(i)),29)) 
     *               call setbits (icrdot(kicrd(i)),7,20,0)
            end if
14        continue
        else
c
c  There are manual shifts but none tagged. Only proceed if "Ao" was entered 
c
          if (.not.btest(nacc,28)) then
            icashft = maxmanshft
            return
          end if
        end if
      end if
      if (btest(nacc,28)) then
c
c  Take the accidentals in order as originally input, then exit.
c
        call crdacc(nacc,naccid,kicrd,nolevm,rmsshift,.true.,
     *           idummy,idummy,segrb0,ksegrb0,nsegrb0,twooftwo,icashft)  
        return
c      end if
      else if (naccid .eq. 3) then
c
c  Special treatment if 3 accidentals in chord. If there aren't accids on a 2nd
c    then place in order top, bottom, middle. 
c
        do 20 i = 1 , 3
          if (kicrd(i) .eq. 0) then
            irank = icrdot0
            nolev = nolevm
		else
            irank = igetbits(icrdot(kicrd(i)),3,27)
            nolev = igetbits(icrdat(kicrd(i)),7,12) 
          end if
          if (irank .eq. 1 ) then
            micrd(1) = kicrd(i)
          else
            micrd(5-irank) = kicrd(i)
          end if
	    if (irank .eq. 2) then
		  levmidacc = nolev 
          end if
20      continue 
        if (levmaxacc.ne.levmidacc+1 .and. 
     *              levmidacc.ne.levminacc+1) then
          call crdacc(nacc,naccid,micrd,nolevm,rmsshift,.true.,
     *            idummy,idummy,segrb0,ksegrb0,nsegrb0,twooftwo,icashft)  
          return
        end if
      end if
      rmsmin = 100000.
c
c  Initialize permutation array
c
      do 7 i = 1 , naccid 
        iperm(i) = i
7     continue
c
c  Start looping over permutations
c
      do 8 ip = 1 , 5041
        if (ip .ne. 1) then
c
c  Work the magic algorithm to get the next permutation
c
          do 1 k = naccid-1 , 1 , -1
            if (iperm(k) .le. iperm(k+1)) go to 2
1         continue
c
c  If here, we just got the last permutation, so exit the loop over permutations
c
          go to 10
2         continue
          do 3 j = naccid , 1 , -1
            if (iperm(k) .le. iperm(j)) go to 4
3         continue
4         continue
          it = iperm(j)
          iperm(j) = iperm(k)
          iperm(k) = it
          is = k+1
          do 5 ir = naccid , 1 , -1
            if (ir .le. is) go to 6
            it = iperm(ir)
            iperm(ir) = iperm(is)
            iperm(is) = it
            is = is+1
5         continue
6         continue
        end if
c
c  New we have a permutation.  Take icrd values out of kicrd and put them into
c  micrd in the order of the permutation
c
        do 9 i = 1 , naccid
          micrd(i) = kicrd(iperm(i))
9       continue
cc
cc  Temporary printout
cc
c      write(15,'(/a6,10i3)')'perm:',(iperm(i),i=1,naccid)
cc
        call crdacc(nacc,naccid,micrd,nolevm,rmsshift,.false.,
     *       levmaxacc,icrdot0,segrb0,ksegrb0,nsegrb0,twooftwo,icashft)  
cc
cc  Temporary printout
cc
c      write(15,*)'perm done, rmsshift:',rmsshift
cc
        if (rmsshift .lt. rmsmin) then
c
c  Save this permutation, reset minrms
c
          do 11 i = 1 , naccid
            ipermsav(i) = iperm(i)
            rmsmin = rmsshift
11        continue
        end if
8     continue          
      print*,'Should not BEEEEEE here!'
      call stop1()
10    continue          
c
c  Done looping, get info for the final choice
c
      do 12 i = 1 , naccid
        micrd(i) = kicrd(ipermsav(i))
12    continue
cc
cc  Temporary printout
cc
c      write(15,'(/a6,10i3)')'Final perm:',(ipermsav(i),i=1,naccid)
cc
      call crdacc(nacc,naccid,micrd,nolevm,rmsshift,.true.,
     *            idummy,idummy,segrb0,ksegrb0,nsegrb0,twooftwo,icashft)  
      return
      end          
      subroutine doacc(ihshft,ivshft,notexq,lnote,nacc,nolev,ncm,caut)
      character*1 sq,chax
      character*3 acsymq
      character*8 noteq
      character*79 notexq
      logical btest,caut
      sq = chax(92)
      if (ihshft .eq. -107) ihshft=0
cc
cc  If main note shifted left, so shift accid.  Terminate below, when acc. is done.
cc
      if (ihshft .ne. 0) then
c
c  Accid must be shifted horizontally
c
        if (ihshft .lt. 0) then
          notexq = sq//'loffset{'
          ihshft = -ihshft
        else
          notexq = sq//'roffset{'
        end if
        hshft = .05*ihshft
        if (hshft .lt. 1.) then
          write(notexq(10:12),'(f3.2)')hshft
          lnote = 12
        else
          write(notexq(10:13),'(f4.2)')hshft
          lnote = 13
        end if
        notexq = notexq(1:lnote)//'}{'//sq
        lnote = lnote+3
      else
        notexq = sq
        lnote = 1
      end if
      if (btest(nacc,3)) then
        notexq = notexq(1:lnote)//'big'
        lnote = lnote+3
      end if
      if (caut) then
c
c  Cautionary accidental.  Need to define bigcna,... in pmx.tex
c
        notexq = notexq(1:lnote)//'c'
        lnote = lnote+1
      end if
      call accsym(nacc,acsymq,lacc)
      notexq = notexq(1:lnote)//acsymq(1:lacc)
      lnote = lnote+lacc
      noleva = nolev
      if (ivshft .ne. 0) noleva = noleva+ivshft-32
      call notefq(noteq,lnoten,noleva,ncm)
      if (lnoten .eq. 1) call addblank(noteq,lnoten)
      notexq = notexq(1:lnote)//noteq(1:lnoten)
      lnote = lnote+lnoten
      if (ihshft .ne. 0) then
c
c  Terminate horizontal shift
c
        notexq = notexq(1:lnote)//'}'
        lnote = lnote+1
      end if
      return
      end
      subroutine docrd(ivx,ip,nodu,ncm,iv,tnow,soutq,lsout,ulq,ibmcnt,
     *                 islur,nvmx,nv,beamon,nolevm,ihornb,nornb,stemlen,
     *                 dotxtup,nacc,irest)
      parameter (nm=24,mv=24576)
      integer*4 irest(nm,24)
      common /comarp/ narp,tar(8),ivar1(8),ipar1(8),levar1(8),ncmar1(8),
     *                xinsnow,lowdot
      common /comtrill/ ntrill,ivtrill(24),iptrill(24),xnsktr(24),
     *                ncrd,icrdat(193),icrdot(193),icrdorn(193),nudorn,
     *                kudorn(63),ornhshft(63),minlev,maxlev,icrd1,icrd2
      common /spfacs/ grafac,acgfac,accfac,xspfac,xb4fac,clefac,emgfac,
     *                flagfac,dotfac,bacfac,agc1fac,gslfac,arpfac,
     *                rptfac,lrrptfac,dbarfac,ddbarfac,dotsfac,upstmfac,
     *                rtshfac
      integer*2 mmidi,iinsiv
      logical restpend,relacc,notmain,twoline,ismidi,crdacc
      common /commidi/ imidi(0:nm),trest(0:nm),mcpitch(20),mgap,
     *       iacclo(0:nm,6),iacchi(0:nm,6),midinst(nm),
     *       nmidcrd,midchan(nm,2),numchan,naccim(0:nm),
     *       laccim(0:nm,10),jaccim(0:nm,10),crdacc,notmain,
     *       restpend(0:nm),relacc,twoline(nm),ismidi,mmidi(0:nm,mv),
     *       debugmidi
      logical debugmidi
      logical lowdot,dotxtup
      integer*4 ihornb(nm,24),nornb(nm)
      character*1 ulq(nm,9),chax
      character*7 nosymq
      character*8 noteq
      character*79 notexq,outq
      character*80 soutq
      logical btest,isleft,isright,beamon
      character*79 inameq
      common /comtop / itopfacteur,ibotfacteur,interfacteur,isig0,
     *   isig,lastisig,fracindent,widthpt,height,hoffpt,voffpt,idsig,
     *   lnam(nm),inameq(nm)
      common /comInstTrans/ iInstTrans(nm),iTransKey(nm),iTransAmt(nm),
     *  instno(nm),nInstTrans,EarlyTransOn,LaterInstTrans
      logical EarlyTransOn,LaterInstTrans
      common /commidisig/ midisig
      common /commvel/ midivel(nm),midvelc(0:nm),midibal(nm),midbc(0:nm)
     *                ,miditran(nm),midtc(0:nm),noinst,iinsiv(nm)
      common /comcc/ ncc(nm),tcc(nm,10),ncmidcc(nm,10),
     *               maxdotmv(nm),ndotmv(nm),updot(nm,20),rtdot(nm,20)
c
c  This subr. once produced notexq for entire chord.  10/18/97 altered to write
c    chord notes as we go.  10/22/97 find range of icrd first.
c    2/25/98 moved rangefinding to precrd so done before slurs, so now
c    on entry, icrd1, icrd2 define range of icrd for this chord.
c
c  Set counter (for this note) for chord notes present.  Set notmain=T.
c    Will test for notmain=.true. in addmidi to tell whether to save pitch.  
c
      nmidcrd = 0
      notmain = .true.
      crdacc = .false.
      do 5 icrd = icrd1 , icrd2
        lnote = 0
        nolev = igetbits(icrdat(icrd),7,12)
c
c  3/8/03 save original pitch to use in midi, in case 2nds alter things.
c
        nolevo = nolev
c
c  Check for special situations with 2nds (see precrd).
c
        if (btest(nacc,30) .and. nolev.eq.nolevm-1) then
c
c Upstem, 2nd with upper as main, interchange pitches, 
c   rt-shift upper (now chord note). Lower (orig chord, now main)
c   if dotted, probably had shifted dot, dot parameters must be moved 
c   from chord to main
c 
          nolev = nolevm
          if (btest(icrdat(icrd),26)) then
c
c  Orig. chord note dot shift, must transfer to main.
c
            icrdotsav = icrdot(icrd)
            icrdatsav = icrdat(icrd)
            if (btest(irest(ivx,ip),19)) then
c
c Main note (upper) had a dot shift, must move it to chord
c
              call setbits(icrdat(icrd),1,26,1)
              call setbits(icrdot(icrd),7,0,
     *                    nint(updot(ivx,ndotmv(ivx)+1)*10)+64)
              call setbits(icrdot(icrd),7,7,
     *                    nint(rtdot(ivx,ndotmv(ivx)+1)*10)+64)
c
c May need to worry about other chord params (accid shefts etc) later
c
            else
              icrdat(icrd) = ibclr(icrdat(icrd),26)
            end if
c
c We are adding a main note dot shift, so push any later ones back
c 

            if (.not.btest(irest(ivx,ip),19)) then
              maxdotmv(ivx) = maxdotmv(ivx)+1
              do 1 indm = maxdotmv(ivx) , ndotmv(ivx)+2 , -1
                updot(ivx,indm) = updot(ivx,indm-1)
                rtdot(ivx,indm) = rtdot(ivx,indm-1)
1             continue
            end if
            irest(ivx,ip) = ibset(irest(ivx,ip),19)
            updot(ivx,ndotmv(ivx)+1) = 0.1*(iand(127,icrdotsav)-64)
            rtdot(ivx,ndotmv(ivx)+1) = 
     *                     0.1*(iand(127,ishft(icrdotsav,-7))-64)
          end if   
        else if (btest(nacc,31) .and. nolev.eq.nolevm+1) then
c
c Downstem, 2nd with lower as main, interchange pitches, 
c   left-shift lower (now chord note). Lower (orig main, now chord)
c   probably had shifted dot, dot parameters must be moved from 
c   main to chord
c
          nolev = nolevm
          if (btest(irest(ivx,ip),19)) then
            icrdotsav = icrdot(icrd)
            icrdatsav = icrdat(icrd)
            icrdat(icrd) = ibset(icrdat(icrd),26)
c bits in icrdot
c     0-6   10*abs(vertical dot shift in \internote) + 64
c     7-13  10*abs(horizontal dot shift in \internote) + 64
c  Assuming >0 for now! 
            call setbits(icrdot(icrd),7,0,
     *                    nint(updot(ivx,ndotmv(ivx)+1)*10)+64)
            call setbits(icrdot(icrd),7,7,
     *                    nint(rtdot(ivx,ndotmv(ivx)+1)*10)+64)
c
c  Must also set dot shift for (now) main note (orig. chord note).
c
            if (btest(icrdatsav,26)) then
              updot(ivx,ndotmv(ivx)+1) = 0.1*(iand(127,icrdotsav)-64)
              rtdot(ivx,ndotmv(ivx)+1) = 
     *                  0.1*(iand(127,ishft(icrdotsav,-7))-64)
            else
c
c No dot move on original chord (now main) note, 
c
              updot(ivx,ndotmv(ivx)+1) = 0.
              rtdot(ivx,ndotmv(ivx)+1) = 0.
            end if
          end if
        end if
c
c  Lower dot for lower-voice notes?.  Conditions are:
c   1. Dotted time value
c   2. Lower voice of two
c   3. Note is on a line
c   4. Not a rest (cannot be a rest in a chord!)
c.  5. Flag (lowdot) is set to true
c
        if (lowdot .and. nvmx.eq.2 .and. ivx.le.nv) then
	  if (2**log2(nodu).ne.nodu .and. mod(nolev-ncm,2).eq.0) then
	    if (btest(icrdat(icrd),26)) then
c
c  Note already in movdot list.  Drop by 2.
c
	      call setbits(icrdot(icrd),7,0,
     *                      igetbits(icrdot(icrd),7,0)-20)
            else
c
c  Not in list so just move it right now
c
	      call dotmov(-2.,0.,soutq,lsout,igetbits(islur,1,3))
	    end if
          end if
        end if
        if (btest(icrdat(icrd),26)) then
c
c  Move the dot. Basic call for chord notes, not main note.
c
          updotc = 0.1*(iand(127,icrdot(icrd))-64)
          rtdotc = 0.1*(iand(127,ishft(icrdot(icrd),-7))-64)
          call dotmov(updotc,rtdotc,soutq,lsout,igetbits(islur,1,3))
        end if
        isleft = btest(icrdat(icrd),23)
        isright = btest(icrdat(icrd),24)
c
c  Check for ornament in chord.
c
        if (icrdorn(icrd) .gt. 0) then
          call putorn(icrdorn(icrd),nolev,nolevm,nodu,nornb,ulq,
     *       ibmcnt,ivx,ncm,islur,nvmx,nv,ihornb,stemlen,outq,lout,
     *       ip,0,beamon,.true.)
          call addstr(outq,lout,soutq,lsout)
        end if
c
c  Chord-note symbol.  First check for breve
c
        if (nodu .eq. 128) then
          nosymq = chax(92)//'zbreve'
          lsym = 7
        else
c
c  Not a breve chord.  Get first letters in chord-note symbol
c
          if (isleft) then
            nosymq = chax(92)//'l'
          else if (isright) then
            nosymq = chax(92)//'r'
          else
            nosymq = chax(92)//'z'
          end if
          if (nodu .ge. 64) then
            nosymq = nosymq(1:2)//'w'
          else if (nodu .ge. 32) then
            nosymq = nosymq(1:2)//'h'
          else
            nosymq = nosymq(1:2)//'q'
          end if
          if (2**log2(nodu).eq.nodu .and. .not.dotxtup) then
            lsym = 3
          else if (.not.btest(islur,3) .or. dotxtup) then
c
c  Single dot
c
            nosymq = nosymq(1:3)//'p'
            lsym = 4
          else
c
c  Double dot
c
            nosymq = nosymq(1:3)//'pp'
            lsym = 5
          end if
        end if
        if (btest(icrdat(icrd),19).and..not.btest(icrdat(icrd),27)) then
c
c  Accidental and not MIDI-only.  Build up bits 0-3 of nacc
c
          nactmp = igetbits(icrdat(icrd),3,20)
c
c  Kluge for bigness.  Only means 'As' has not been issued
c
          if (bacfac .ne. 1.e6) nactmp = nactmp+8
          call doacc(igetbits(icrdot(icrd),7,20)-107,
     *        igetbits(icrdot(icrd),6,14),
c     *        notexq,lnote,nactmp,nolev,ncmid(iv,ip))
c  Get original nolev, not altered to deal with 2nds
     *        notexq,lnote,nactmp,igetbits(icrdat(icrd),7,12),
c     *        ncmid(iv,ip))
     *        ncmid(iv,ip),btest(icrdat(icrd),31))
          notexq = notexq(1:lnote)//nosymq
          crdacc = .true.
        else
          notexq = nosymq
        end if
        lnote = lnote+lsym
c
c  Get note name (again if accid, due to possible octave jump)
c
        call notefq(noteq,lnoten,nolev,ncm)
        if (lnoten.eq.1) call addblank(noteq,lnoten)
c
c  Put in note name
c
        notexq = notexq(1:lnote)//noteq
        lnote = lnote+lnoten
        if (btest(icrdat(icrd),25)) then
c
c  Arpeggio signal
c
          call putarp(tnow,ivx,ip,nolev,ncm,soutq,lsout)
        end if
        call addstr(notexq,lnote,soutq,lsout)
        if (ismidi) then
c
c  Here is where we collect MIDI pitch info for the chord note.  By checking
c    notmain, addmidi(...) knows to just compute the 
c    pitch number and store it in mcpitch(nmidcrd).  Then on call to addmidi()
c    for MAIN note, will put in note codes for all chord notes + main note.
c   
          kv = 1
          if (ivx .gt. iv) kv=2
          nmidcrd = nmidcrd+1
          if (nmidcrd .gt. 20) then
            print*
            print*,'21 chord notes is too many for midi processor'
            call stop1()
          end if
c
c  Use original saved pitch level, unaltered by 2nds logic.
c
          call addmidi(midchan(iv,kv),nolevo+miditran(instno(iv)),
     *      igetbits(icrdat(icrd),3,20),midisig,1.,
     *      .false.,.false.)
        end if
5     continue
      notmain = .false.
      return
      end
      subroutine dodyn(ivx,ip,nolev,ncm,ipl,islur,irest,nvmx,nv,
     *                beamon,ihornb,nornb,ulq,ibmcnt,nostem,soutq,lsout) 
      parameter (nm=24)
c
c  Inputs are array *elements* except ihornb,nornb,ulq
c
      common /commus/ musize,whead20
      common /comdyn/ ndyn,idyndat(99),levdsav(nm),ivowg(12),hoh1(12),
     *        hoh2(12),hoh2h1(2),ntxtdyn,ivxiptxt(41),txtdynq(41),
     *        idynda2(99),levhssav(nm),listcresc,listdecresc
      character*128 txtdynq
      common /comtrill/ ntrill,ivtrill(24),iptrill(24),xnsktr(24),
     *                ncrd,icrdat(193),icrdot(193),icrdorn(193),nudorn,
     *                kudorn(63),ornhshft(63),minlev,maxlev,icrd1,icrd2
      logical btest,beamon,upstem,nostem
      common /comslur/ listslur,upslur(nm,2),ndxslur,fontslur
     *                 ,WrotePsslurDefaults,SlurCurve
      logical fontslur,upslur,WrotePsslurDefaults
      character*1 udqq,chax,ulq(nm,9)
      character*4 dynstrq
      character*5 numpq
      character*48 dyntablq,tempq
      character*79 notexq
      character*80 soutq
      integer*4 ihornb(nm,24),nornb(nm),idynn(10)
      common /comhair/ ihairuse,idhair(nm)
      data dyntablq /'ppppppp pp  p   mp  mf  f   fp  sfz ff  fff ffff'/
      numdyn = 0
c
c  Find dynamics for (ivx,ip) in list.  May be as many as 4.  Store idyn values 
c      in idynn(1...4)
c
      do 1 idyn = 1 , ndyn
        ivxtent = iand(idyndat(idyn),15)
     *                 +16*igetbits(idynda2(idyn),1,10)
	  if (ivxtent .eq. ivx) then
          iptent = igetbits(idyndat(idyn),8,4)
          if (iptent .eq. ip) then
            numdyn = numdyn+1
            idynn(numdyn) = idyn	  
          else if (iptent .gt. ip) then
c
c  I don't think there are any more possible for this ivx,ip, so exit loop
c
            go to 2
          end if
        end if
1     continue
2     continue
c
c  At this point there is a list of idyn's in idynn(1...numdyn)
c  Compute level, and stem-dir'n-based horizontal tweaks
c
      hoffsd = 0.
c
c  Set upstem to false as default
c
      upstem = .false.
      if (btest(irest,0)) then
c
c  It's a rest.  Assume it doesn't go below the staff
c
        lbot = ncm-4  
      else if (.not.beamon) then
        if (udqq(nolev,ncm,islur,nvmx,ivx,nv).eq.'u' .or. nostem) then
          upstem = .true.
          if (.not.btest(ipl,10)) then
            lbot = min(nolev-1,ncm-4)
          else
            lbot = min(minlev-1,ncm-4)
          end if
        else
          hoffsd = -.5
          if (.not.btest(ipl,10)) then
            lbot = min(nolev-7,ncm-4)
          else
            lbot = min(minlev-7,ncm-4)
          end if
        end if
      else
        if (ulq(ivx,ibmcnt) .eq. 'u') then
          upstem = .true.
          if (.not.btest(ipl,10)) then
            lbot = min(nolev-1,ncm-4)
          else
            lbot = min(minlev-1,ncm-4)
          end if
        else
          hoffsd = -.5
c
c 171230 Desperation
c          lbot = ihornb(ivx,nornb(ivx))+1
          if (nornb(ivx).eq.0) then
            lbot = 1
          else
            lbot = ihornb(ivx,nornb(ivx))+1
          end if
          if (lbot .eq. 1) then
c
c Kluge for non-beamed, down xtup, for which ihorb was never set.
c Assumes stem is shortened.
c
            lbot = nolev-5
          end if
          nornb(ivx) = nornb(ivx)+1
        end if
      end if
      lbot = lbot-5
      jtxtdyn1 = 1
c
c  Now ready to loop over current dyn's
c
      do 3 icntdyn = 1 , numdyn
        idynd = idyndat(idynn(icntdyn))
        idynd2 = idynda2(idynn(icntdyn))
        idno = igetbits(idynd,4,12)
c        ivx = iand(15,idynd)
        ivx = iand(15,idynd)+16*igetbits(idynd2,1,10)
c
c  Build the command into notex in stages. Insert name & rq'd args in order:
c
c    Command name
c	 hpstrt, hpcend, hpdend, pmxdyn
c    ivx
c        X       X       X
c    level
c                X       X       X
c    hoff
c        X       X       X       X
c    d-mark
c                                X
c
        if (idno .eq. 0) then
c
c  Text-dynamic
c
          notexq = chax(92)//'txtdyn'
          lnote = 7
        else if (idno .le. 12) then
c
c  Letter-group
c
          notexq = chax(92)//'pmxdyn'
          lnote = 7
        else if (fontslur) then
          lnote = 7
          if (idno .eq. 13) then
c
c  Start a font-based hairpin
c
            notexq = chax(92)//'hpstrt'
          else if (idno .eq. 14) then
c
c  End crescendo
c
            notexq = chax(92)//'hpcend'
          else
c
c  End decrescendo
c
            notexq = chax(92)//'hpdend'
          end if
c
c  Put in voice number as ID for font-based hairpin
c
          if (ivx .le. 9) then
            notexq = notexq(1:lnote)//char(48+ivx)
            lnote = lnote+1   
          else if (ivx .le. 19) then
            notexq = notexq(1:lnote)//'{1'//char(38+ivx)//'}'
            lnote = lnote+4
          else
            notexq = notexq(1:lnote)//'{2'//char(28+ivx)//'}'
            lnote = lnote+4
          end if
        else
c
c  Postscript hairpins
c          
          lnote = 7
          if (idno .eq. 13) then
            notexq = chax(92)//'Icresc'
          else if (idno .eq.14) then
            notexq = chax(92)//'Idecresc'
            lnote = 9
          else		         
            notexq = chax(92)//'Tcresc'
          end if
          if (idno .le. 14) then
c
c  Get and record ID no for start of ps hairpin
c  Find first unused ID
c         
            do 8 idh = 1 , 24
              if (.not.btest(ihairuse,idh)) go to 9
8           continue
            call printl('Bad place in putdyn, call Dr. Don')
            call stop1()
9           continue
            ihairuse = ibset(ihairuse,idh)    
            idhair(ivx) = idh
          else
c
c  Unrecord ID no for end of ps hairpin
c         
            call setbits(ihairuse,1,idhair(ivx),0)
          end if                     
c
c  Write ID # for start or end of ps hairpin
c
          idh = idhair(ivx)
          if (idh .le. 9) then
            notexq = notexq(1:lnote)//char(48+idh)
            lnote = lnote+1   
          else if (idh .le. 19) then
            notexq = notexq(1:lnote)//'{1'//char(38+idh)//'}'
            lnote = lnote+4
          else
            notexq = notexq(1:lnote)//'{2'//char(28+idh)//'}'
            lnote = lnote+4
          end if
        end if
c
c  Begin setting level
c
        lbot1 = lbot
        if (idno.gt.0 .and. idno.le.5) then
c
c  All letters are short so raise a bit.
c
          lbot1 = lbot1+1
        else if (idno .ge. 13) then
          lbot1 = lbot1+2
        end if
c
c  Convert so reference is bottom line
c
        lbot1 = lbot1-ncm+4
        if ((fontslur.and.idno.eq.13) .or. 
     *      ((.not.fontslur).and.(idno.eq.13.or.idno.eq.14))) then
c
c  Hairpin start.  Save level and user-tweak before applying user tweak.
c
          levdsav(ivx) = lbot1
          levhssav(ivx) = 0
          if (btest(idynd,16)) levhssav(ivx) = -64+igetbits(idynd,7,17) 
        else if ((fontslur.and.idno.ge.14) .or. idno.eq.15) then
c
c  Hairpin end; Compare level with saved start level before user-tweaks
c
          lbot1 = min(lbot1,levdsav(ivx))         
c
c  Save pre-tweak level
c
          lpretweak = lbot1
        end if
c
c  Check for user-defined vertical tweak
c
        if (btest(idynd,16)) lbot1 = lbot1-64+igetbits(idynd,7,17)  
c
c  Now horizontal stuff
c
        hoff = hoffsd
c
c  Some special horizontal tweaks
c
        if (upstem .and. idno.gt.0 .and. 
     *      (idno.le.4 .or. idno.eq.8 .or. idno.eq.9)) hoff = hoff+.4
c
c  User-defined tweaks
c
        if (btest(idynd2,0)) 
     *          hoff = hoff+(igetbits(idynd2,9,1)-256)*.1
        if (numdyn .gt. 1) then
c
c  Horizontal-interaction-based tweaks.  
c
c  Cases:
c  numdyn  type1   type2    type3    data used
c     2    wrd-grp hrpnstrt -        ivowg(1...12),hoh1(1...12)
c     2	 hrpnend wrd-grp  -		   ivowg,hoh2
c     2    hrpnend hrpnstrt -		   hoh2h1(1...2)
c     3    hrpnend wrd-grp  hrpnstrt ivowg,hoh2,hoh1
c
          if (idno.gt.0 .and. idno.le.12) then
c
c  Word-group, may need vertical tweak to line up.
c
            lbot1 = lbot1+ivowg(idno)
c
c  Protecting against hp start-stop on same note
c
          else if (((fontslur.and.idno.ge.14).or.idno.eq.15)
     *                .and. icntdyn.lt.numdyn) then
c
c  Hairpin ending, check next type 
c
            if ((fontslur .and. 
     *             igetbits(idyndat(idynn(icntdyn+1)),4,12).eq.13)
     *          .or. (.not.fontslur .and.   
     *             ((igetbits(idyndat(idynn(icntdyn+1)),4,12).eq.13) 
     *                .or.     
     *              (igetbits(idyndat(idynn(icntdyn+1)),4,12).eq.14)))) 
     *                     then
c
c  Hairpin end then hairpin start, no words, (remember dealing with end now)
c
              hoff = hoff+hoh2h1(1)
            else	       
c
c  Hairpin end then word-group, need idno for w-g to set hp offset
c
              hoff = hoff +
     *                hoh2(igetbits(idyndat(idynn(icntdyn+1)),4,12))
            end if	       
c
c  Protecting against hp start-stop on same note
c
          else if (icntdyn.gt.1 .and. idno.gt.0 .and. 
     *            ((fontslur.and.idno.lt.14).or.
     *                    (.not.fontslur.and.idno.lt.15))) then
c
c  Hairpin start, check prior type
c
            if ((fontslur.and.
     *               igetbits(idyndat(idynn(icntdyn-1)),4,12).ge.14) 
     *          .or.
     *          (.not.fontslur.and.
     *               igetbits(idyndat(idynn(icntdyn-1)),4,12).eq.15))
     *          then
c
c  Hairpin end then hairpin start, (remember dealing with start now)
c
              hoff = hoff+hoh2h1(2)
            else	       
c
c  Hairpin start after word-group, need idno for w-g to set hp offset
c
              hoff = hoff+
     *                hoh1(igetbits(idyndat(idynn(icntdyn-1)),4,12))
            end if
          end if
        end if
c
c  End of if-block for 2- or 3-way interactions.
c
        if ((.not.fontslur) .and. idno.ge.13) 
c
c  Slur font and hairpin. Add hoff, and change from \interneote to \qn@width 
c
     *          hoff = (hoff+.5)*6./2.5
c
c  Position corrections all done now.  Put in the level.
c
        if ((fontslur.and.idno.eq.13) .or. 
     *      ((.not.fontslur).and.(idno.eq.13.or.idno.eq.14))) then
c
c  Hairpin start.  
c
          if (.not.fontslur) then
c
c  Postscript hairpin start...inset placeholder for start level.

            notexq = notexq(1:lnote)//'{   }'
		  lnote = lnote+5
          end if
        else 
c
c  Insert actual level in all cases except hairpin start
c  Create string with level in it
c
          if (lbot1 .gt. 9) then
            numpq = '{'
            write(numpq(2:3),'(i2)')lbot1
            numpq = numpq(1:3)//'}'
            lnumpq = 4
          else if (lbot1 .gt. -1) then
            numpq = char(48+lbot1)
            lnumpq = 1
          else if (lbot1 .gt. -10) then
            numpq = '{'
            write(numpq(2:3),'(i2)')lbot1
            numpq = numpq(1:3)//'}'
            lnumpq = 4
          else
            numpq = '{'
            write(numpq(2:4),'(i3)')lbot1
            numpq = numpq(1:4)//'}'
            lnumpq = 5
          end if
c
c  Level has now been computed and stored in numpq
c  Append the level
c
          notexq = notexq(1:lnote)//numpq(1:lnumpq)
          lnote = lnote+lnumpq
        end if
        if (abs(hoff) .lt. .001) then
c
c  No horiz offset  
c
          notexq = notexq(1:lnote)//'0'
          lnote = lnote+1
        else
c
c  Horizontal tweak
c
          lform = lfmt1(hoff)
          notexq = notexq(1:lnote)//'{'
          lnote = lnote+1
          write(notexq(lnote+1:lnote+lform),
     *             '(f'//chax(48+lform)//'.1)')hoff
          lnote = lnote+lform
          notexq = notexq(1:lnote)//'}'
          lnote = lnote+1
        end if
        if (idno .eq. 0) then
c
c  text-dynamic.  Find the string and append it
c
          do 4 jtxtdyn = jtxtdyn1 , ntxtdyn
c            ivxip = ivx+16*ip
            ivxip = ivx+32*ip
            if (ivxip .eq. ivxiptxt(jtxtdyn)) go to 5
4         continue
          call printl('Abnormal stop in putdyn')
          call stop1()
5         continue
          ltxtdyn = lenstr(txtdynq(jtxtdyn),128)
c
c  Font size based on musicsize
c
c          if (musize .eq. 20) then
c            notexq = notexq(1:lnote)//'{'//char(92)//'medtype'
c     *               //char(92)//'it '
c            lnote = lnote+13   
c          else if (musize .eq. 16) then
c            notexq = notexq(1:lnote)//'{'//char(92)//'normtype'
c     *               //char(92)//'it '
c            lnote = lnote+14   
c          else if (musize .eq. 24) then
c            notexq = notexq(1:lnote)//'{'//char(92)//'bigtype'
c     *               //char(92)//'it '
c            lnote = lnote+13   
c          else if (musize .eq. 29) then
c            notexq = notexq(1:lnote)//'{'//char(92)//'Bigtype'
c     *               //char(92)//'it '
c            lnote = lnote+13   
c          end if
c
c Do this to insert 1st 2 args of \txtdyn, allow 3rd to be longer (on next line)
c
          call addstr(notexq(1:lnote),lnote,soutq,lsout)
          if (musize .eq. 20) then
            notexq = '{'//char(92)//'medtype'//char(92)//'it '
            lnote = 13   
          else if (musize .eq. 16) then
            notexq = '{'//char(92)//'normtype'//char(92)//'it '
            lnote = 14   
          else if (musize .eq. 24) then
            notexq = '{'//char(92)//'bigtype'//char(92)//'it '
            lnote = 13   
          else if (musize .eq. 29) then
            notexq = '{'//char(92)//'Bigtype'//char(92)//'it '
            lnote = 13   
          end if
c
          notexq = notexq(1:lnote)//txtdynq(jtxtdyn)(1:ltxtdyn)//'}'
          lnote = lnote+ltxtdyn+1
c
c  Reset jtxtdyn1 just in case >1 txtdyn on same note.
c
          jtxtdyn1 = jtxtdyn+1
        else if (idno .le. 12) then
c
c  Letter-group dynamic.  Append the letter-group command
c
          id = 4*idno
          dynstrq = dyntablq(id-3:id)
          id = lenstr(dynstrq,4)
          notexq = notexq(1:lnote)//chax(92)//dynstrq(1:id)
          lnote = lnote+1+id
        end if 
        call addstr(notexq(1:lnote),lnote,soutq,lsout)
        if ((.not.fontslur).and.idno.eq.15) then
c
c  PS slurs on, hairpin is ending.  Go back and set height at beginning.
c  Add user-defined tweak to default level
c
          lbot1 = lpretweak+levhssav(ivx)
          if (lbot1 .gt. 9) then
            numpq = '{'
            write(numpq(2:3),'(i2)')lbot1
            numpq = numpq(1:3)//'}'
            lnumpq = 4
          else if (lbot1 .gt. -1) then
            numpq = char(48+lbot1)
            lnumpq = 1
          else if (lbot1 .gt. -10) then
            numpq = '{'
            write(numpq(2:3),'(i2)')lbot1
            numpq = numpq(1:3)//'}'
            lnumpq = 4
          else
            numpq = '{'
            write(numpq(2:4),'(i3)')lbot1
            numpq = numpq(1:4)//'}'
            lnumpq = 5
          end if
c
c  Construct string to search backwards for placeholder
c
          if (idh .le. 9) then
            tempq = 'cresc'//char(48+idh)//'{   }'
            ltemp = 11   
          else if (idh .le. 19) then
            tempq = 'cresc'//'{1'//char(38+idh)//'}{   }'
            ltemp = 14   
          else
            tempq = 'cresc'//'{2'//char(28+idh)//'}{   }'
            ltemp = 14   
          end if
          write(11,'(a)')soutq(1:lsout)//'%'
          lsout = 0
          call backfill(11,tempq,ltemp,
     *           tempq(1:ltemp-5)//numpq(1:lnumpq),ltemp-5+lnumpq)
        end if
3     continue
c
c  Shrink arrays, decrease ndyn 111109
c
      do 6 icntdyn = numdyn, 1 , -1
        do 7 jdyn = idynn(icntdyn) , ndyn-1
          idyndat(jdyn) = idyndat(jdyn+1) 
          idynda2(jdyn) = idynda2(jdyn+1)
7       continue
        ndyn = ndyn-1
6     continue
      end
      subroutine dograce(ivx,ip,ptgr,soutq,lsout,ncm,nacc,ig,ipl,
     *  farend,
     *  beamon,nolev,ncmidx,islur,nvmx,nv,ibmcnt,tnote,ulq,instno)
c
c  ip will be one LESS than current note, for way-after's before bar-end,
c    It is only used to find ig.
c  ig is returned to makeabar in case there's a slur that needs to be ended
c
      parameter (nm=24)
      logical beamon,stemup
      common /comgrace/ ivg(37),ipg(37),nolevg(74),itoff(2,74),aftshft,
     *                nng(37),ngstrt(37),ibarmbr,mbrest,xb4mbr,
     *                noffseg,ngrace,nvolt,ivlit(83),iplit(83),nlit,
     *                graspace(37),
     *                lenlit(83),multg(37),upg(37),slurg(37),slashg(37),
     *                naccg(74),voltxtq(6),litq(83)
      common /spfacs/ grafac,acgfac,accfac,xspfac,xb4fac,clefac,emgfac,
     *                flagfac,dotfac,bacfac,agc1fac,gslfac,arpfac,
     *                rptfac,lrrptfac,dbarfac,ddbarfac,dotsfac,upstmfac,
     *                rtshfac
      common /comask/ bar1syst,fixednew,scaldold,
     *                wheadpt,fbar,poenom
      common /comslur/ listslur,upslur(nm,2),ndxslur,fontslur
     *                 ,WrotePsslurDefaults,SlurCurve
      common /comoct/ noctup
      logical upg,slurg,slashg,bar1syst,upslur,btest,isgaft,iswaft,
     *        normsp,farend,fontslur,WrotePsslurDefaults
      real*4 ptgr(37)
      character*80 soutq
      character*128 litq
      character*79 notexq
      character*20 voltxtq
      character*10 figq
      character*8 noteq,noteqGA
      character*3 acsymq
      character*1 sq,chax,udqq,ulq(nm,9)
      common /comfig/ itfig(2,74),figq(2,74),ivupfig(2,74),nfigs(2),
     *                fullsize(nm),ivxfig2,ivvfig(2,74)
      sq = chax(92)
      isgaft = btest(ipl,29)
      iswaft = btest(ipl,31)
      normsp = .not. isgaft
c
c  Find ig.
c
      do 120 ig = 1 , ngrace
        if (ipg(ig).eq.ip .and. ivg(ig).eq.ivx) go to 121
120   continue
      print*,'Problem finding grace index in dograce'
      stop
121   continue
      ngs = ngstrt(ig)
      mg = multg(ig)
c	wheadpt1 = wheadpt*fullsize(ivx)
	wheadpt1 = wheadpt*fullsize(instno)
c
c  For way-after-graces at end of bar, must set the octave.
c
      if (farend) then
        noctup = 0
        if (ncm .eq.23) noctup = -2
      end if
      if (slurg(ig) .and. .not.iswaft .and..not.isgaft) then
         if (listslur .eq. 16777215) then
          print*
          print*,'You defined the twentyfifth slur, one too many!'
          write(15,'(/,a)')
     *           'You defined the twentyfifth slur, one too many!'
          call stop1()
        end if
c
c  Slur on fore-grace.  Get index of next slur not in use, from 23 down.
c
        ndxslur = log2(16777215-listslur)

      end if
      if (nng(ig) .eq. 1) then
c
c  Single grace.
c
        if (normsp) then
c
c  Anything but GA
c
          call addstr(sq//'shlft',6,soutq,lsout)
          niptgr = nint(ptgr(ig))
c
c  Empirical tweak for postscript.
c
C          if (.not.fontslur) niptgr = niptgr+nint(wheadpt*.3)
c++
          if (niptgr .lt. 10) then
            call addstr(chax(48+niptgr)//'{',2,soutq,lsout)
          else if (niptgr .lt. 100) then
            write(notexq(1:2),'(i2)')niptgr
            call addstr('{'//notexq(1:2)//'}{',5,soutq,lsout)
          else
            print*,
     *  'Call Dr. Don if you really want grace note group > 99 pt'
            stop
          end if
        else
          call addstr(sq//'gaft{1.5}{',11,soutq,lsout)
c
c  GA.  Compute aftshft, for later use.
c
          aftshft = grafac
          if (naccg(ngstrt(ig)).gt.0) aftshft = aftshft+agc1fac
          aftshft = aftshft*wheadpt
        end if
        if (slurg(ig) .and. .not.isgaft .and..not.iswaft) then
c
c  Start slur on pre-grace.  No accounting needed since will be ended very soon.
c
          call notefq(noteq,lnoten,nolevg(ngs),ncm)
          if (fontslur) then
            if (upg(ig)) then
              call addstr(sq//'islurd',7,soutq,lsout)
            else
              call addstr(sq//'isluru',7,soutq,lsout)
            end if
          else
c
c  Start Postscript slur. 
c
            if (upg(ig)) then
              call addstr(sq//'isd',4,soutq,lsout)
            else
              call addstr(sq//'isu',4,soutq,lsout)
            end if
          end if
c
c  Print slur number, 23-ndxslur
c
          lnote = 0
          if (23-ndxslur .lt. 10) then
c              notexq = notexq(1:lnote)//chax(59-ndxslur)
            notexq = chax(71-ndxslur)
            lnote = 1
          else if (23-ndxslur .lt. 20) then
c              notexq = notexq(1:lnote)//'{1'//chax(49-ndxslur)//'}'
            notexq = '{1'//chax(61-ndxslur)//'}'
            lnote = 4
          else 
            notexq = notexq(1:lnote)//'{2'//chax(51-ndxslur)//'}'
            lnote = 4
          end if
          call addstr(notexq(1:lnote)//noteq(1:lnoten),
     *       lnote+lnoten,soutq,lsout)
	    if (.not.fontslur) then
c
c  Horizontal tweaks for postscript slur on single grace
c
            stemup = .true.
            if (upg(ig)) then
c
c  Check for up-grace + down stem. Get stem direction
c
              if (.not.beamon) then
c
c  Separate note.  Get stem direction.
c
                stemup = udqq(nolev,ncmidx,
     *                     islur,nvmx,ivx,nv) .eq. 'u'
              else
c
c  In a beam
c
                stemup = ulq(ivx,ibmcnt) .eq. 'u'
              end if
c
c  Stop the shift if whole note
c
              stemup = stemup .or. tnote.gt.63
            end if
            if (stemup) then
              call addstr('{-.3}',5,soutq,lsout)
            else
              call addstr('{-.8}',5,soutq,lsout)
            end if
          end if
        end if
        if (naccg(ngs) .gt. 0) then
          call notefq(noteq,lnoten,nolevg(ngs),ncm)
c
c Save for checking octave shifts in GA
c
          if (isgaft) then
            lnotenGA = lnoten
            noteqGA = noteq
          end if
c
          if (lnoten .eq. 1) call addblank(noteq,lnoten)
          call accsym(naccg(ngs),acsymq,lacc)
          call addstr(sq//'big'//acsymq(1:lacc)//
     *         noteq(1:lnoten),4+lacc+lnoten,soutq,lsout)
        end if
        if (slashg(ig)) then
          notexq = sq//'grc'
          lnote = 4
        else if (mg .eq. 0) then
          notexq = sq//'zq'
          lnote = 3
        else
          notexq = sq//'zc'
          do 61 i = 2 , mg
            notexq = notexq(1:i+1)//'c'
61        continue
          lnote = mg+2
        end if
        if (upg(ig)) then
          notexq = notexq(1:lnote)//'u'
        else
          notexq = notexq(1:lnote)//'l'
        end if
        call addstr(notexq,lnote+1,soutq,lsout)
        call notefq(noteq,lnoten,nolevg(ngs),ncm)
c
        if (isgaft .and. naccg(ngs).eq.0) then
          lnotenGA = lnoten
          noteqGA = noteq
        end if
c
        if (lnoten .eq. 1) call addblank(noteq,lnoten)
        call addstr(noteq,lnoten,soutq,lsout)
        if (slashg(ig)) call addstr(sq//'off{-'//sq//
     *      'noteskip}',16,soutq,lsout)
c
c  Above code needed since slashg causes spacing
c
        if (slurg(ig) .and. (iswaft.or.isgaft)) then
c
c  Terminate slur on single after-grace
c
          ndxslur = igetbits(ipl,5,23)
          call notefq(noteq,lnoten,nolevg(ngs),ncm)
          call addstr(sq//'tslur',6,soutq,lsout)
c
c  Print 24-ndxslur
c
c          if (11-ndxslur .lt. 10) then
          if (23-ndxslur .lt. 10) then
c            call addstr(chax(59-ndxslur)//noteq(1:lnoten),
            call addstr(chax(71-ndxslur)//noteq(1:lnoten),
     *              1+lnoten,soutq,lsout)
          else if (23-ndxslur .lt. 20) then
            call addstr('{2'//chax(61-ndxslur)//'}'//noteq(1:lnoten),
     *              4+lnoten,soutq,lsout)
          else
c            call addstr('{1'//chax(49-ndxslur)//'}'//noteq(1:lnoten),
            call addstr('{1'//chax(51-ndxslur)//'}'//noteq(1:lnoten),
     *              4+lnoten,soutq,lsout)
          end if
          slurg(ig) = .false.
          listslur = ibclr(listslur,ndxslur)
        end if
        call addstr('}',1,soutq,lsout)
c
c+++  Try to fix loss of octave with single gaft
c
        if (isgaft) then
          itrans = 0
          do 1 i = 1 , lnotenGA
            if (noteqGA(i:i) .eq. chax(39)) then
               itrans = itrans+7
            else if (noteqGA(i:i) .eq. chax(96)) then
               itrans = itrans-7
            end if
1         continue
          if (itrans. eq. -14) then
            call addstr(sq//'advance'//sq//'transpose-14',21,
     *                  soutq,lsout)                       
          else if (itrans .eq. -7) then
            call addstr(sq//'advance'//sq//'transpose-7',20,
     *                  soutq,lsout)                       
          else if (itrans .eq. 7) then
            call addstr(sq//'advance'//sq//'transpose7',19,
     *                  soutq,lsout)                       
          else if (itrans .eq. 14) then
            call addstr(sq//'advance'//sq//'transpose14',20,
     *                  soutq,lsout)                       
          end if
        end if
      else
c
c  Multiple grace.  Put in literally.  Compute beam stuff
c
        sumx = 0.
        sumy = 0.
        sumxy = 0.
        sumxx = 0.
        sumyy = 0.
        x = 0.
        do 118 ing = ngs , ngs+nng(ig)-1
          if (ing.gt.ngs .and. naccg(ing).gt.0) x = x+acgfac
          y = nolevg(ing)
          sumx = sumx + x
          sumy = sumy + y
          sumxy = sumxy + x*y
          sumxx = sumxx + x*x
          sumyy = sumyy + y*y
          x = x+emgfac
118     continue
        delta = nng(ig)*sumxx-sumx*sumx
        em = (nng(ig)*sumxy-sumx*sumy)/delta
        islope = nint(0.5*em*gslfac)
        if (iabs(islope) .gt. 9) islope = isign(9,islope)
        beta = (sumy-islope/gslfac*sumx)/nng(ig)
        nolev1 = nint(beta)
c
c  Back up
c
        notexq = sq//'settiny'//sq//'off{'
        if (normsp) then
          write(notexq(14:18),'(a1,f4.1)')'-',ptgr(ig)
          call addstr(notexq(1:18)//'pt}',21,soutq,lsout)
          finalshift = ptgr(ig)
        else
          aftshft = wheadpt*1.33
          if (naccg(ngstrt(ig)).gt.0) aftshft = aftshft+wheadpt*0.5
          write(notexq(14:17),'(f4.1)')aftshft
          call addstr(notexq(1:17)//'pt}'//sq//'bsk',24,soutq,lsout)
        end if
c
c  Start the beam
c
        notexq = sq//'ib'
        do 119 ing = 2 , mg
           notexq = notexq(1:ing+1)//'b'
119     continue
        if (upg(ig)) then
          notexq = notexq(1:mg+2)//'u'
        else
          notexq = notexq(1:mg+2)//'l'
        end if
        notexq = notexq(1:mg+3)//'0'
c
c  Get starting note for beam
c
        call notefq(noteq,lnoten,nolev1,ncm)
        call addstr(notexq(1:mg+4)//noteq(1:lnoten),
     *    mg+4+lnoten,soutq,lsout)
c
c  Put in the slope
c
        if (islope .ge. 0) then
          call addstr(chax(48+islope),1,soutq,lsout)
        else
          call addstr('{-'//chax(48-islope)//'}',4,soutq,lsout)
        end if
c
c  Start a slur on multiple fore-grace
c
        if (slurg(ig) .and. .not.isgaft .and. .not.iswaft) then
          call notefq(noteq,lnoten,nolevg(ngs),ncm)
	    if (fontslur) then
            if (upg(ig)) then
              call addstr(sq//'islurd',7,soutq,lsout)
            else
              call addstr(sq//'isluru',7,soutq,lsout)
            end if
          else
c
c  Need a tweak for postscript slur
c
            if (upg(ig)) then
              call addstr(sq//'isd',4,soutq,lsout)
            else
              call addstr(sq//'isu',4,soutq,lsout)
            end if
          end if
c
c  Print 23-ndxslur
c
          if (23-ndxslur .lt. 10) then
            call addstr(chax(71-ndxslur)//noteq(1:lnoten),1+lnoten,
     *                 soutq,lsout)
          else if (23-ndxslur .lt. 2) then
            call addstr('{1'//chax(61-ndxslur)//'}'//noteq(1:lnoten),
     *                  4+lnoten,soutq,lsout)
          else
            call addstr('{1'//chax(51-ndxslur)//'}'//noteq(1:lnoten),
     *                  4+lnoten,soutq,lsout)
          end if
c
c  Put in tweak for postscript slur 
c
          if (.not.fontslur) call addstr('{-.3}',5,soutq,lsout)
        end if
c
c  Put in first note.  Call notefq again in case octave changed
c
        call notefq(noteq,lnoten,nolevg(ngs),ncm)
        if (naccg(ngs) .eq. 0) then
          notexq = sq//'zqb0'//noteq(1:lnoten)
          lnote = 5+lnoten
        else
          if (lnoten .eq. 1) call addblank(noteq,lnoten)
          call accsym(naccg(ngs),acsymq,lacc)
          notexq = sq//'big'//acsymq(1:lacc)//noteq(1:lnoten)
          lnote = 4+lacc+lnoten
          call notefq(noteq,lnoten,nolevg(ngs),ncm)
          notexq =notexq(1:lnote)//sq//'zqb0'//noteq(1:lnoten)
          lnote = lnote+5+lnoten
        end if
        call addstr(notexq,lnote,soutq,lsout)
        do 127 ing = ngs+1 , ngs+nng(ig)-1
c
c  Skip
c
          ptoff = wheadpt1*emgfac
          if (naccg(ing).gt.0) ptoff = ptoff+wheadpt1*acgfac
          if (isgaft .and. .not.iswaft) aftshft = aftshft+ptoff
          notexq = sq//'off{'
          write(notexq(6:8),'(f3.1)')ptoff
          if (normsp) finalshift = finalshift-ptoff
          call addstr(notexq(1:8)//'pt}',11,soutq,lsout)
          if (ing .eq. ngs+nng(ig)-1) then
c
c  Terminate beam if needed
c
            if (upg(ig)) then
              call addstr(sq//'tbu0',5,soutq,lsout)
            else
              call addstr(sq//'tbl0',5,soutq,lsout)
            end if
c
c  Terminate after slur if needed
c
            if ((isgaft.or.iswaft) .and. slurg(ig)) then
c              if (iswaft) ndxslur = igetbits(ipl,4,23)
              if (iswaft) ndxslur = igetbits(ipl,5,23)
              call notefq(noteq,lnoten,nolevg(ing),ncm)
              call addstr(sq//'tslur',6,soutq,lsout)
c
c  Print 11-ndxslur
cc  Print 23-ndxslur
c
              if (23-ndxslur .lt. 10) then
                call addstr(chax(71-ndxslur)//noteq(1:lnoten),
     *              1+lnoten,soutq,lsout)
              else if (23-ndxslur .lt. 20) then
                call addstr('{2'//chax(61-ndxslur)//'}'
     *                      //noteq(1:lnoten),4+lnoten,soutq,lsout)
              else
                call addstr('{1'//chax(51-ndxslur)//'}'
     *                      //noteq(1:lnoten),4+lnoten,soutq,lsout)
              end if
c
c  Stop slur terminator after exit from this subroutine
c
              listslur = ibclr(listslur,ndxslur)
              slurg(ig) = .false.
            end if
          end if
c
c  Accidental if needed
c
          if (naccg(ing).gt.0) then
            call notefq(noteq,lnoten,nolevg(ing),ncm)
            if (lnoten .eq. 1) call addblank(noteq,lnoten)
            call accsym(naccg(ing),acsymq,lacc)
            call addstr(sq//'big'//acsymq(1:lacc)
     *                  //noteq(1:lnoten),4+lacc+lnoten,soutq,lsout)
          end if
c
c  Put in the (beamed) grace note
c
          call notefq(noteq,lnoten,nolevg(ing),ncm)
          call addstr(sq//'zqb0'//noteq(1:lnoten),5+lnoten,
     *                soutq,lsout)
127     continue
c
c  Terminate the grace
c
c        notexq = sq//'normalnotesize'//sq//'off{'
c        lnote = 20
c        notexq = '}'//sq//'off{'
c        lnote = 6
        notexq = sq//'off{'
        lnote = 5
        ptoff = wheadpt*emgfac
        if (iand(nacc,3).gt.0 .and. .not.btest(nacc,17)) 
     *       ptoff = ptoff+wheadpt*accfac
        if (isgaft .and. .not.iswaft) then
          notexq = notexq(1:5)//'-'
          lnote = 6
          ptoff = aftshft
        end if
        if (normsp) ptoff = finalshift
        if (ptoff .lt. 9.95) then
          write(notexq(lnote+1:lnote+3),'(f3.1)')ptoff
          lnote = lnote+3
        else if (ptoff .lt. 99.95) then
          write(notexq(lnote+1:lnote+4),'(f4.1)')ptoff
          lnote = lnote+4
	  else 
          write(notexq(lnote+1:lnote+5),'(f5.1)')ptoff
          lnote = lnote+5
        end if
        call addstr(notexq(1:lnote)//'pt}',lnote+3,soutq,lsout)
        if (isgaft.and..not.iswaft) call addstr(sq//'sk',3,soutq,lsout)
        call addstr(sq//'resetsize',10,soutq,lsout)
      end if
      return
      end
      subroutine dopsslur(nolev,isdat1,isdat2,isdat3,isdat4,nsdat,ip,
     *                    iv,kv,nv,beamon,ncm,soutq,lsout,ulq,islur,
     *                    ipl,iornq,islhgt,tno,nacc)
c
c  Called once per main note.  
c  12 May 2002  Create this subroutine to isolate postscript slurs/ties.
c    Always set \Nosluradjust\Notieadjust
c
      parameter (nm=24,mv=24576)
      integer*4 isdat1(202),isdat2(202),isdat3(202),isdat4(202)
      common /comslur/ listslur,upslur(nm,2),ndxslur,fontslur
     *                 ,WrotePsslurDefaults,SlurCurve
      common /commvl/ nvmx(nm),ivmx(nm,2),ivx
      common /comtrill/ ntrill,ivtrill(24),iptrill(24),xnsktr(24),
     *                ncrd,icrdat(193),icrdot(193),icrdorn(193),nudorn,
     *                kudorn(63),ornhshft(63),minlev,maxlev,icrd1,icrd2
      common /comsln/ is1n1,is2n1,irzbnd,isnx
      character*1 ulq,slurudq,udfq,udqq,chax
      character*79 notexq
      character*8 noteq
      character*80 soutq
      logical upslur,beamon,btest,stemup,iscrd,
     *        settie,fontslur,pstie,WrotePsslurDefaults
      logical slmon,dbltie
      common /comslm/ levson(0:nm),levsoff(0:nm),imidso(0:nm),
     *       naccbl(0:nm),laccbl(0:nm,10),jaccbl(0:nm,10),nusebl,
     *       slmon(0:nm),dbltie
      integer*2 mmidi,iinsiv
      logical restpend,relacc,notmain,twoline,ismidi,crdacc
      common /commidi/ imidi(0:nm),trest(0:nm),mcpitch(20),mgap,
     *       iacclo(0:nm,6),iacchi(0:nm,6),midinst(nm),
     *       nmidcrd,midchan(nm,2),numchan,naccim(0:nm),
     *       laccim(0:nm,10),jaccim(0:nm,10),crdacc,notmain,
     *       restpend(0:nm),relacc,twoline(nm),ismidi,mmidi(0:nm,mv),
     *       debugmidi
      logical debugmidi
      common /comInstTrans/ iInstTrans(nm),iTransKey(nm),iTransAmt(nm),
     *  instno(nm),nInstTrans,EarlyTransOn,LaterInstTrans
      logical EarlyTransOn,LaterInstTrans
c 130316
      common /commvel/ midivel(nm),midvelc(0:nm),midibal(nm),midbc(0:nm)
     *                ,miditran(nm),midtc(0:nm),noinst,iinsiv(nm)
c
c  Bits in isdat1:
c  13-17    iv
c  3-10     ip
c  11       start/stop switch
c  12       kv-1
c  19-25    ichar(code$)
c  26       force direction?

c  27       forced dir'n = up if on, set in sslur; also
c           final direction, set in doslur when beam is started, used on term.
c  28-31    mod(ndxslur,16), set in doslur when slur is started, used on term.
c  18       int(ndxslur/16), ditto. So this allows ndxslur>15.
c  2        stem slur flag
c  1        flag for "x" slur (voice-independent)
c
c  Bits in isdat2
c  0        Chord switch.  Not set on main note.
c  1-2      left/right notehead shift.  Set only for chord note.
c  3        tie positioning
c  4        dotted flag
c  6-11     voff1 1-63  =>  -31...+31
c  12-18    hoff1 1-127 => -6.3...+6.3
c  19-25    nolev
c  26       \sluradjust    (p+s)
c  27       \nosluradjust  (p-s)
c  28       \tieadjust     (p+t)
c  29       \notieadjust   (p-t)
c
c  Bits in isdat3: Only used for slur endings
c  0        set if midslur (at least one argument)
c  1        set if curve (2 more args)
c  2-7      32+first arg (height correction) (1st arg may be negative)
c  8-10     second arg (initial slope)
c  11-13    third arg (closing slope)
c  14-21    tie level for use in LineBreakTies
c  22-29    ncm for use in LineBreakTies
c
c  Bits in isdat4  Only used for linebreak slurs
c  0-5      Linebreak seg 1 voff 1-63  =>  -31...+31  
c  6-12     Linebreak seg 1 hoff 1-127 => -6.3...+6.3
c  16-21    Linebreak seg 2 voff 1-63  =>  -31...+31
c  22-28    Linebreak seg 2 hoff 1-127 => -6.3...+6.3
c
c  In listslur bit ib is on if slur index ib is in use, ib=0-13.
c  ndxslur = slur index
c  Height of slur is nole+ivoff+iupdn.  iupdn is +/-1 if t&s slurs on same note,
c  s-slur is blank (idcode=32), t-slur is idcode=1.
c  ivoff is user-defined shift or shift due to . or _ , or chord adjustment.
c  Ivoff will be set for ./_ only if no user-defined shift is specified.
c  If highest note has upslur, save slur height in islhgt in case
c  ornament must be moved.
c
      islhgt = 0
      if (beamon) then
        stemup = ulq .eq. 'u'
      else if (nvmx(iv) .eq. 2) then
        if (.not.btest(islur,30)) then
c
c  Single note, 2 lines of music, stem direction not forced
c
          stemup = ivx .gt. nv
        else
          stemup = btest(islur,17)
        end if
      else
        stemup = udqq(nolev,ncm,islur,nvmx(iv),ivx,nv) .eq. 'u'
      end if
      iscrd = btest(ipl,10)
      if (ismidi) then
        settie = .false.
        dbltie = .false.
      end if
      do 1 isdat = 1 , nsdat
        isdata = isdat1(isdat)
        if (iv .eq. igetbits(isdata,5,13) .and.
     *      ip .eq. igetbits(isdata,8,3) .and.
c     *            kv .eq. igetbits(isdata,1,12)+1) then
     *       (kv .eq. igetbits(isdata,1,12)+1 .or.
     *        btest(isdata,1))) then
c
c  Since iv and kv match, ivx will be correct
c
          idcode = igetbits(isdata,7,19)
          ivoff = igetbits(isdat2(isdat),6,6)-32
          ihoff = igetbits(isdat2(isdat),7,12)-64
          iupdn = 0
          slurudq = 'd'
          nolevs = igetbits(isdat2(isdat),7,19)
          pstie = btest(isdat2(isdat),3) .or. idcode.eq.1
          if (btest(isdata,11)) then
c
c  Turnon
c  Get slur direction
c
            if (btest(isdata,26)) then
c
c  Force slur direction
c
              if (btest(isdata,27)) slurudq = 'u'
            else if (nvmx(iv) .eq. 1) then
c
c  Only one voice per line
c
              if (.not.beamon) then
c
c  Separate note.
c
                slurudq = udfq(nolev,ncm)
              else
c
c  In a beam
c
                if (ulq .ne. 'u') slurudq = 'u'
              end if
              if (iscrd) then
                if (nolevs .gt. ncm) then
                  slurudq = 'u'
                else
                  slurudq = 'd'
                end if
              end if
            else
c
c  Two voices per line.  Get default
c
              if (ivx .gt. nv) slurudq = 'u'
c
c  Upper voice of the two, so up slur
c
            end if
            if (btest(isdata,2)) then
c
c  ADjust for stem slur. ASSUME this is the ONLY pos'n adjustment.
c
              if (stemup) then
                slurudq = 'u'
                ivoff = ivoff+4
              else
                slurudq = 'd'
                ivoff = ivoff-4
              end if
            end if
c
c  Set level for slur starting on rest
c
            if (nolevs.eq.0 .or. nolevs.gt.60) then
              if (slurudq .eq. 'u') then
                nolevs = ncm+2
              else
                nolevs = ncm-2
              end if
            end if
c
c  Save up/down-ness for use at termination
c
            if (slurudq .eq. 'u') isdata = ibset(isdata,27)
c
c  End of section for setting slur direction, still in "Turnon" if-block.
c
            if (btest(iornq,11).or.btest(iornq,12)) then
c
c  Raise or lower slur by one unit provided . or _ is on same side as slur
c
              ivoffinc = 0
              if ((stemup .and. slurudq.eq.'d') .or.
     *             (.not.stemup .and. slurudq.eq.'u')) then
c
c  Must move the slur for _ or .
c
                if (stemup) then
                  ivoffinc = -1
                else
                  ivoffinc = 1
                end if
                if (((stemup      .and. nolev.ge.ncm-2) .or.
     *              (.not.stemup .and. nolev.le.ncm+2)) .and.
     *                mod(abs(ncm-nolev),2).eq.0) ivoffinc = 2*ivoffinc
                ivoff = ivoff+ivoffinc
              end if
            end if
            if (listslur .eq. 16777215) then
              print*
              print*,'You1 defined the twentyfifth slur, one too many!'
              write(15,'(/,a)')
     *               'You defined the twentyfifth slur, one too many!'
              call stop1()
            end if
c
c  Get index of next slur not in use, starting from 12 down
c
            ndxslur = log2(16777215-listslur)
c
c  Record slur index
c
            listslur = ibset(listslur,ndxslur)
c
c  Save for use on termination
c
c            call setbits(isdata,4,28,ndxslur)
c  080531  Allow >16 slurs
            call setbits(isdata,4,28,mod(ndxslur,16))
            call setbits(isdata,1,18,ndxslur/16)
c
c  Shift for stem?
c
            if (stemup .and. slurudq.eq.'u' .and. tno.lt.63.) then
              if (.not.pstie) then
                ihoff = ihoff+8
              else
                ihoff = ihoff+2
              end if
	    end if
            if (iscrd) then
c
c  Additional horiz shifts for h-shifted noteheads?
c
              if (btest(isdat2(isdat),1)) then
c
c  Slur start on left-shifted chord notehead.  ASSUME downstem.
c
                if (nolevs.eq.minlev .and. slurudq.eq.'d') then
                  ihoff = ihoff-2
                else
                  ihoff = ihoff-10
                end if
              else if (btest(isdat2(isdat),2)) then
c
c  Right shifted chord notehead.  ASSUME upstem.
c
                if (nolevs.eq.maxlev .and. slurudq.eq.'u') then
                  ihoff = ihoff+2
                else
                  ihoff = ihoff+10
                end if
              end if
            end if
            notexq = chax(92)
            lnote = 1
c
c  Check for local adjustment default changes
c
            if (btest(isdat2(isdat),26)) then
              notexq = chax(92)//'sluradjust'//chax(92)
	        lnote = 12
            else if (btest(isdat2(isdat),27)) then
              notexq = chax(92)//'nosluradjust'//chax(92)
	        lnote = 14
            else if (btest(isdat2(isdat),28)) then
              notexq = chax(92)//'tieadjust'//chax(92)
	        lnote = 11
            else if (btest(isdat2(isdat),29)) then
              notexq = chax(92)//'notieadjust'//chax(92)
	        lnote = 13
            end if
            if (ihoff .eq. 0) then
c
c  Write stuff for non-shifted start
c
              notexq = notexq(1:lnote)//'islur'//slurudq
              lnote = lnote+6
            else
              notexq = notexq(1:lnote)//'is'//slurudq
              lnote = lnote+3
            end if
c
c  Prepend postscript tie switch
c
            if (pstie) then 
              notexq = chax(92)//'tieforis'//slurudq//notexq(1:lnote)
              lnote = lnote+10
            end if
            if (btest(isdat2(isdat),4)) then
c
c  Dotted slur
c
c              noteq = notexq
c              notexq = chax(92)//'dotted'//noteq
              notexq = chax(92)//'dotted'//notexq(1:lnote)
              lnote = lnote+7
            end if
c
c  Add slur index to string
c  Print 23-ndxslur
c
            if (23-ndxslur .lt. 10) then
c
c  5/25/08 Allow 24 slurs
c
              notexq = notexq(1:lnote)//chax(71-ndxslur)
              lnote = lnote+1
            else if (23-ndxslur .lt. 20) then
              notexq = notexq(1:lnote)//'{1'//chax(61-ndxslur)//'}'
              lnote = lnote+4
            else 
              notexq = notexq(1:lnote)//'{2'//chax(51-ndxslur)//'}'
              lnote = lnote+4
            end if
c
c  Add note name to string
c
            islhgt = nolevs+iupdn+ivoff
            call notefq(noteq,lnoten,islhgt,ncm)
            notexq = notexq(1:lnote)//noteq(1:lnoten)
            lnote = lnote+lnoten
c
c  Store height and staff mid level for use with LineBreakTies 
c
            call setbits(isdat3(isdat),8,14,islhgt)
            call setbits(isdat3(isdat),8,22,ncm)
c
c  Save height (for ornament and barnobox interference) if topmost slur is up
c
            if (slurudq.eq.'u' .and.
     *           (.not.btest(isdat2(isdat),0).or.nolevs.eq.maxlev)) then
              islhgt = nolevs+iupdn+ivoff
c
c  Save height & idcode if top voice and slur start
c
              if (ivx.eq.ivmx(nv,nvmx(nv)) .and. islhgt.gt.is1n1) then
                is1n1 = islhgt
                is2n1 = idcode
              end if
            end if
            if (ihoff .ne. 0.) then
              shift = ihoff*0.1
              notexq = notexq(1:lnote)//'{'
              lnote = lnote+1
              lform = lfmt1(shift)
              write(notexq(lnote+1:lnote+lform),'(f'//
     *                  chax(48+lform)//'.1)') shift
              lnote = lnote+lform
              notexq = notexq(1:lnote)//'}'
              lnote = lnote+1
            end if
            call addstr(notexq,lnote,soutq,lsout)
c
c  Zero out ip1 to avoid problems if slur goes to next input blk.
c
            call setbits(isdata,8,3,0)
c
c  Set slur-on data for midi.  Only treat null-index slurs and ps ties for now.
c
            if (ismidi .and. (idcode.eq.32 .or. idcode.eq.1)) then
c              levson(midchan(iv,kv)) = nolevs
c 130316
              levson(midchan(iv,kv)) = nolevs+miditran(instno(iv))
              if (settie) dbltie = .true.
c
c  Only way settie=T is if we just set a tie ending.  So there's also a slur
c   start here, so set a flag telling addmidi not to zero out levson
c 
            end if
          else
c
c  Slur is ending.  Back thru list to find starting slur
c
            do 3 j = isdat-1 , 1 , -1
              if (iv.eq.igetbits(isdat1(j),5,13) .and.
c     *            kv.eq.igetbits(isdat1(j),1,12)+1) then
     *            (kv.eq.igetbits(isdat1(j),1,12)+1
     *           .or. btest(isdat1(j),1))) then
                if (idcode .eq. igetbits(isdat1(j),7,19)) then
                  ndxslur = igetbits(isdat1(j),4,28)
c
c  080531 Allow >16 slurs
c
     *                          +16*igetbits(isdat1(j),1,18)
                  if (btest(isdat1(j),27)) slurudq = 'u'
                  go to 4
                end if
              end if
3           continue
            print*,'Bad place in doslur'
            call stop1()
4           continue
c
c  Bugfix 070901 for slur ending on rest in 2-voice staff
c
            if (nolevs.le.2 .or. nolevs.gt.60) then
c
c  Ending is on a rest, reset nolevs to default starting height
c
              nolevs = igetbits(isdat2(j),7,19)
            end if
            if (btest(isdat3(isdat),0) .or. btest(isdat3(j),0)) then
c
c  Deal with \curve or \midslur. isdat is ending, j is start.
c
              if (btest(isdat3(isdat),0)) then
                imid = igetbits(isdat3(isdat),6,2)-32 
              else
                imid = igetbits(isdat3(j),6,2)-32 
              end if
c
c  Postscript slurs, and \midslur adjustment is needed.  Invoke macro 
c   (from pmx.tex) that redefines \tslur as r'qd.  Tentative mapping:
c       Abs(imid)  Postscript slur type
c          1          f
c          2-3        default
c          4          h
c          5          H
c          6+         HH
c
              call addstr(chax(92)//'psforts'//
     *                      chax(48+min(abs(imid),6)),9,soutq,lsout)
            end if
            if (btest(isdata,2)) then
c
c  ADjust for stem slur. 
c
              if (stemup) then
                slurudq = 'u'
                ivoff = ivoff+4
              else
                slurudq = 'd'
                ivoff = ivoff-4
              end if
            end if
c
c  Shift slur ending for stem on any note?
c
            if (.not.stemup .and. slurudq.eq.'d' .and. tno.lt.63.) then
              if (.not.pstie) then
                ihoff = ihoff-8
              else
                ihoff = ihoff-3
              end if
            end if
            if (iscrd) then
c
c  Shift termination for shifted notehead?
c
              if (btest(isdat2(isdat),1)) then
c
c  Left-shifted chord notehead.  ASSUME downstem.
c
                if (nolevs.eq.minlev .and. slurudq.eq.'d') then
                  ihoff = ihoff-2
                else
                  ihoff = ihoff-10
                end if
              else if (btest(isdat2(isdat),2)) then
c
c  Right shifted chord notehead.  ASSUME upstem.
c
                if (nolevs.eq.maxlev .and. slurudq.eq.'u') then
                  ihoff = ihoff+2
                else
                  ihoff = ihoff+10
                end if
              end if
            end if
            if (ihoff .eq. 0) then
              notexq = chax(92)//'tslur'
              lnote = 6
            else
c
c  Shift needed
c
              notexq = chax(92)//'ts'
              lnote = 3
            end if
c
c  Switch to postscript tie
c
            if (pstie) then
              notexq = chax(92)//'tieforts'//notexq(1:lnote)
              lnote = lnote+9
            end if
c
c  Print 13-ndxslur
c  5/25/08 Allow 14 slurs
c
            if (23-ndxslur .lt. 10) then
              notexq = notexq(1:lnote)//chax(71-ndxslur)
              lnote = lnote+1
            else if (23-ndxslur .lt. 20) then
              notexq = notexq(1:lnote)//'{1'//chax(61-ndxslur)//'}'
              lnote = lnote+4
            else 
              notexq = notexq(1:lnote)//'{2'//chax(51-ndxslur)//'}'
              lnote = lnote+4
            end if
            if (btest(iornq,11).or.btest(iornq,12)) then
c
c  Raise or lower slur by one unit provided . or _ is on same side as slur
c
              ivoffinc = 0
              if ((stemup .and. slurudq.eq.'d') .or.
     *                 (.not.stemup .and. slurudq.eq.'u')) then
                if (stemup) then
                  ivoffinc = -1
                else
                  ivoffinc = 1
                end if
                if (((stemup .and. nolev.ge.ncm-2) .or.
     *              (.not.stemup .and. nolev.le.ncm+2)) .and.
     *               mod(abs(ncm-nolev),2).eq.0) ivoffinc = 2*ivoffinc
              end if
              ivoff = ivoff+ivoffinc
            end if
            call notefq(noteq,lnoten,nolevs+iupdn+ivoff,ncm)
            if (slurudq.eq.'u' .and.
     *           (.not.btest(isdat2(isdat),0).or.nolevs.eq.maxlev)) then
              islhgt = nolevs+iupdn+ivoff
c
c  If topvoice, upslur, and idcode checks, no more need to keep hgt for barno.
c
              if (ivx.eq.ivmx(nv,nvmx(nv)) .and. is1n1.gt.0) then
                if (idcode .eq. is2n1) is1n1=0
              end if
            end if
            notexq = notexq(1:lnote)//noteq(1:lnoten)
            lnote = lnote+lnoten
            if (ihoff .ne. 0) then
              shift = ihoff*0.1
              notexq = notexq(1:lnote)//'{'
              lnote = lnote+1
              lform = lfmt1(shift)
              write(notexq(lnote+1:lnote+lform),
     *            '(f'//chax(48+lform)//'.1)')shift
              lnote = lnote+lform
              notexq = notexq(1:lnote)//'}'
              lnote = lnote+1
            end if
            call addstr(notexq,lnote,soutq,lsout)
c
c  Clear the bit from list of slurs in use
c
            listslur = ibclr(listslur,ndxslur)
c
c  Zero out the entire strings for start and stop
c
            isdata = 0
            isdat2(isdat) = 0
            isdat3(isdat) = 0
            isdat4(isdat) = 0
            isdat1(j) = 0
            isdat2(j) = 0
            isdat3(j) = 0
            isdat4(j) = 0
c
c  Set midi info for slur ending
c
            if (ismidi .and. (idcode.eq.32 .or. idcode.eq.1)) then  
              icm = midchan(iv,kv)
              if (slmon(icm)) then
                if (nolevs+miditran(instno(iv)).eq.levson(icm) .and.
     *                iand(7,nacc).eq.0) then
c
c  There is a tie here.  NB!!! assumed no accidental on 2nd member of tie.
c
                  levsoff(icm) = nolevs+miditran(instno(iv))
                  settie = .true.
                else
                  levsoff(icm) = 0
                  levson(icm) = 0
                  slmon(icm) = .false.
                end if
              end if
            end if 
          end if
          isdat1(isdat) = isdata
        end if
1     continue
c
c  Clear and collapse the slur data list
c
      numdrop = 0
      do 2 isdat  = 1 , nsdat
        if (isdat1(isdat) .eq. 0) then
          numdrop = numdrop+1
        else if (numdrop .gt. 0) then
          isdat1(isdat-numdrop) = isdat1(isdat)
          isdat2(isdat-numdrop) = isdat2(isdat)
          isdat3(isdat-numdrop) = isdat3(isdat)
          isdat4(isdat-numdrop) = isdat4(isdat)
          isdat1(isdat) = 0
          isdat2(isdat) = 0
          isdat3(isdat) = 0
          isdat4(isdat) = 0
        end if
2     continue
      nsdat = nsdat-numdrop
c      call report(nsdat,isdat1,isdat2)
      return
      end
      subroutine doslur(nolev,isdat1,isdat2,isdat3,nsdat,ip,iv,kv,nv,
     *     beamon,ncm,soutq,lsout,ulq,islur,ipl,iornq,islhgt,tno,nacc)
c
c  Called once per main note.  (5/26/02) for non-ps slurs only
c
      parameter (nm=24,mv=24576)
      integer*4 isdat1(202),isdat2(202),isdat3(202)
      common /comslur/ listslur,upslur(nm,2),ndxslur,fontslur
     *                 ,WrotePsslurDefaults,SlurCurve
      common /commvl/ nvmx(nm),ivmx(nm,2),ivx
      common /comtrill/ ntrill,ivtrill(24),iptrill(24),xnsktr(24),
     *                ncrd,icrdat(193),icrdot(193),icrdorn(193),nudorn,
     *                kudorn(63),ornhshft(63),minlev,maxlev,icrd1,icrd2
      common /comsln/ is1n1,is2n1,irzbnd,isnx
      character*1 ulq,slurudq,udfq,udqq,chax
      character*79 notexq
      character*8 noteq
      character*80 soutq
      logical upslur,beamon,btest,stemup,iscrd,sfound,tfound,tmove,
     *        settie,fontslur,WrotePsslurDefaults
      logical slmon,dbltie
      common /comslm/ levson(0:nm),levsoff(0:nm),imidso(0:nm),
     *       naccbl(0:nm),laccbl(0:nm,10),jaccbl(0:nm,10),nusebl,
     *       slmon(0:nm),dbltie
      integer*2 mmidi,iinsiv
      logical restpend,relacc,notmain,twoline,ismidi,crdacc
      common /commidi/ imidi(0:nm),trest(0:nm),mcpitch(20),mgap,
     *       iacclo(0:nm,6),iacchi(0:nm,6),midinst(nm),
     *       nmidcrd,midchan(nm,2),numchan,naccim(0:nm),
     *       laccim(0:nm,10),jaccim(0:nm,10),crdacc,notmain,
     *       restpend(0:nm),relacc,twoline(nm),ismidi,mmidi(0:nm,mv),
     *       debugmidi
      logical debugmidi
      common /comInstTrans/ iInstTrans(nm),iTransKey(nm),iTransAmt(nm),
     *  instno(nm),nInstTrans,EarlyTransOn,LaterInstTrans
      logical EarlyTransOn,LaterInstTrans
c 130316
      common /commvel/ midivel(nm),midvelc(0:nm),midibal(nm),midbc(0:nm)
     *                ,miditran(nm),midtc(0:nm),noinst,iinsiv(nm)
c
c  Bits in isdat1:
c  13-17    iv
c  3-10     ip
c  11       start/stop switch
c  12       kv-1
c  19-25    ichar(code$)
c  26       force direction?
c  27       forced dir'n = up if on, set in sslur; also
c           final direction, set in doslur when beam is started, used on term.
c  28-31    ndxslur, set in doslur when beam is started, used on term.
c
c  Bits in isdat2
c  0        Chord switch.  Not set on main note.
c  1-2      left/right notehead shift.  Set only for chord note.
c  3        tie positioning
c  4        dotted flag
c  6-11     voff1 1-63  =>  -31...+31
c  12-18    hoff1 1-127 => -6.3...+6.3
c  19-25    nolev
c
c  Bits in isdat3: Only used for slur endings
c  0        set if midslur (at least one argument)
c  1        set if curve (2 more args)
c  2-7      32+first arg (height correction) (1st arg may be negative)
c  8-10     second arg (initial slope)
c  11-13    third arg (closing slope)
c
c  In listslur bit ib is on if slur index ib is in use, ib=0-23.
c  ndxslur = slur index
c  Height of slur is nole+ivoff+iupdn.  iupdn is +/-1 if t&s slurs on same note,
c  s-slur is blank (idcode=32), t-slur is idcode=1.
c  ivoff is user-defined shift or shift due to . or _ , or chord adjustment.
c  Ivoff will be set for ./_ only if no user-defined shift is specified.
c  If highest note has upslur, save slur height in islhgt in case
c  ornament must be moved.
c
      islhgt = 0
      if (beamon) then
        stemup = ulq .eq. 'u'
      else if (nvmx(iv) .eq. 2) then
        if (.not.btest(islur,30)) then
c
c  Single note, 2 lines of music, stem direction not forced
c
          stemup = ivx .gt. nv
        else
          stemup = btest(islur,17)
        end if
      else
        stemup = udqq(nolev,ncm,islur,nvmx(iv),ivx,nv) .eq. 'u'
      end if
      iscrd = btest(ipl,10)
      if (btest(islur,1)) then
c
c  't'-slur (idcode=1) somewhere on this note.  Find it, check height against
c    's'-slur (idcode=32)
c
        sfound = .false.
        tfound = .false.
        tmove = .false.
        do 5 isdat = 1 , nsdat
          if (iv .eq. igetbits(isdat1(isdat),5,13) .and.
     *          ip .eq. igetbits(isdat1(isdat),8,3) .and.
     *              kv .eq. igetbits(isdat1(isdat),1,12)+1) then
            if (.not.tfound) then
              tfound = igetbits(isdat1(isdat),7,19).eq.1
              if (tfound) then
                nolevt = igetbits(isdat2(isdat),7,19)
                isdatt = isdat
                if (sfound) go to 6
              end if
            end if
            if (.not.sfound) then
              sfound = igetbits(isdat1(isdat),7,19).eq.32
              if (sfound) then
                nolevs = igetbits(isdat2(isdat),7,19)
                isdats = isdat
                if (tfound) go to 6
              end if
            end if
          end if
5       continue
c
c  Will come thru here if there is a t with no s, so comment out the following
c        print*,'Did not find s+t-slurs in doslur'
c
6       continue
        if (sfound .and. tfound)
     *    tmove = nolevs.eq.nolevt .and.
c
c  Check if 2 starts or two stops
c
     *     ((btest(isdat1(isdats),11).and.btest(isdat1(isdatt),11)) .or.
     *      (.not.btest(isdat1(isdats),11).and.
     *                               .not.btest(isdat1(isdatt),11)) )
c
c  This is a flag for later changing slur level, after we know slur dir'n.
c
      end if
      if (ismidi) then
        settie = .false.
        dbltie = .false.
      end if
      do 1 isdat = 1 , nsdat
        isdata = isdat1(isdat)
        if (iv .eq. igetbits(isdata,5,13) .and.
     *      ip .eq. igetbits(isdata,8,3) .and.
     *            kv .eq. igetbits(isdata,1,12)+1) then
c
c  Since iv and kv match, ivx will be correct
c
          idcode = igetbits(isdata,7,19)
          ivoff = igetbits(isdat2(isdat),6,6)-32
          ihoff = igetbits(isdat2(isdat),7,12)-64
          iupdn = 0
          slurudq = 'd'
          nolevs = igetbits(isdat2(isdat),7,19)
          if (btest(isdata,11)) then
c
c  Turnon, 
c
            if (nolevs.eq.0 .or. nolevs.gt.60) then
c
c  Note was a rest, cannot start slur on rest.
c
              print*
              call printl('Cannot start slur on a rest')
              call stop1()
            end if
c
c  Get slur direction
c
            if (btest(isdata,26)) then
c
c  Force slur direction
c
              if (btest(isdata,27)) slurudq = 'u'
            else if (nvmx(iv) .eq. 1) then
c
c  Only one voice per line
c
              if (.not.beamon) then
c
c  Separate note.
c
                slurudq = udfq(nolev,ncm)
              else
c
c  In a beam
c
                if (ulq .ne. 'u') slurudq = 'u'
              end if
              if (iscrd) then
                if (nolevs .gt. ncm) then
                  slurudq = 'u'
                else
                  slurudq = 'd'
                end if
              end if
            else
c
c  Two voices per line.  Get default
c
              if (ivx .gt. nv) slurudq = 'u'
c
c  Upper voice of the two, so up slur
c
            end if
c
c  Save up/down-ness for use at termination
c
            if (slurudq .eq. 'u') isdata = ibset(isdata,27)
c
c  End of section for setting slur direction, still in "Turnon" if-block.
c
            if (idcode.eq.1 .and. tmove) then
              iupdn = 1
              if (slurudq .eq. 'd') iupdn = -1
            end if
            if (btest(iornq,11).or.btest(iornq,12)) then
c
c  Raise or lower slur by one unit provided . or _ is on same side as slur
c
              ivoffinc = 0
              if ((stemup .and. slurudq.eq.'d') .or.
     *             (.not.stemup .and. slurudq.eq.'u')) then
c
c  Must move the slur for _ or .
c
                if (stemup) then
                  ivoffinc = -1
                else
                  ivoffinc = 1
                end if
                if (((stemup      .and. nolev.ge.ncm-2) .or.
     *              (.not.stemup .and. nolev.le.ncm+2)) .and.
     *                mod(abs(ncm-nolev),2).eq.0) ivoffinc = 2*ivoffinc
                ivoff = ivoff+ivoffinc
              end if
            end if
            if (listslur .eq. 16777215) then
              print*
              print*,'You1 defined the twenty-fifth slur, one too many!'
              write(15,'(/,a)')
     *               'You2 defined the twenty-fifth slur, one too many!'
              call stop1()
            end if
c
c  Get index of next slur not in use, starting from ? down
c
            ndxslur = log2(16777215-listslur)
c
c  Record slur index
c
            listslur = ibset(listslur,ndxslur)
c
c  Save for use on termination
c
c            call setbits(isdata,4,28,ndxslur)
c  080531  Allow >16 slurs
            call setbits(isdata,4,28,mod(ndxslur,16))
            call setbits(isdata,1,18,ndxslur/16)
c
c  Shift for stem?
c
            if (stemup .and. slurudq.eq.'u' .and. tno.lt.63.) 
     *              ihoff = ihoff+8
            if (btest(isdat2(isdat),3)) then
c
c  Tie spacing, (slur start)
c
              if (slurudq.eq.'d') then
                ivoff = ivoff+1
                ihoff = ihoff+8
              else if (slurudq.eq.'u') then
                ivoff = ivoff-1
                if (.not.(stemup.and.tno.lt.63.)) ihoff = ihoff+8
c
c  (already shifted if (stemup.and.tno.gt.63.) and slurudq='u')
c
              end if
            end if
            if (iscrd) then
c
c  Additional horiz shifts for h-shifted noteheads?
c
              if (btest(isdat2(isdat),1)) then
c
c  Slur start on left-shifted chord notehead.  ASSUME downstem.
c
                if (nolevs.eq.minlev .and. slurudq.eq.'d') then
                  ihoff = ihoff-2
                else
                  ihoff = ihoff-10
                end if
              else if (btest(isdat2(isdat),2)) then
c
c  Right shifted chord notehead.  ASSUME upstem.
c
                if (nolevs.eq.maxlev .and. slurudq.eq.'u') then
                  ihoff = ihoff+2
                else
                  ihoff = ihoff+10
                end if
              end if
            end if
            if (ihoff .eq. 0) then
c
c  Write stuff for non-shifted start
c
              notexq = chax(92)//'islur'//slurudq
              lnote = 7
            else
              notexq = chax(92)//'is'//slurudq
              lnote = 4
            end if
            if (btest(isdat2(isdat),4)) then
c
c  Dotted slur
c
              noteq(1:8) = notexq
              notexq = chax(92)//'dotted'//noteq
              lnote = lnote+7
            end if
c
c  Add slur index to string
cc  Print 11-ndxslur
c  Print 23-ndxslur
c
c            if (11-ndxslur .lt. 10) then
            if (23-ndxslur .lt. 10) then
c
c  5/25/08 Allow 24 slurs
c
c              notexq = notexq(1:lnote)//chax(59-ndxslur)
              notexq = notexq(1:lnote)//chax(71-ndxslur)
              lnote = lnote+1
            else if (23-ndxslur .lt. 20) then
c              notexq = notexq(1:lnote)//'{1'//chax(49-ndxslur)//'}'
              notexq = notexq(1:lnote)//'{1'//chax(61-ndxslur)//'}'
              lnote = lnote+4
            else 
              notexq = notexq(1:lnote)//'{2'//chax(51-ndxslur)//'}'
              lnote = lnote+4
            end if
c
c  Add note name to string
c
            call notefq(noteq,lnoten,nolevs+iupdn+ivoff,ncm)
            notexq = notexq(1:lnote)//noteq(1:lnoten)
            lnote = lnote+lnoten
c
c  Save height (for ornament and barnobox interference) if topmost slur is up
c
            if (slurudq.eq.'u' .and.
     *           (.not.btest(isdat2(isdat),0).or.nolevs.eq.maxlev)) then
              islhgt = nolevs+iupdn+ivoff
c
c  Save height & idcode if top voice and slur start
c
              if (ivx.eq.ivmx(nv,nvmx(nv)) .and. islhgt.gt.is1n1) then
                is1n1 = islhgt
                is2n1 = idcode
              end if
            end if
            if (ihoff .ne. 0.) then
              shift = ihoff*0.1
              notexq = notexq(1:lnote)//'{'
              lnote = lnote+1
              lform = lfmt1(shift)
              write(notexq(lnote+1:lnote+lform),'(f'//
     *                  chax(48+lform)//'.1)') shift
              lnote = lnote+lform
              notexq = notexq(1:lnote)//'}'
              lnote = lnote+1
            end if
            call addstr(notexq,lnote,soutq,lsout)
c
c  Zero out ip1 to avoid problems if slur goes to next input blk.
c
            call setbits(isdata,8,3,0)
c
c  Set slur-on data for midi.  Only treat null-index slurs and ps ties for now.
c
            if (ismidi .and. idcode.eq.32) then
              levson(midchan(iv,kv)) = nolevs+miditran(instno(iv))
              if (settie) dbltie = .true.
c
c  Only way settie=T is if we just set a tie ending.  So there's also a slur
c   start here, so set a flag telling addmidi not to zero out levson
c 
            end if
          else
c
c  Slur is ending.  Back thru list to find starting slur
c
            do 3 j = isdat-1 , 1 , -1
              if (iv.eq.igetbits(isdat1(j),5,13) .and.
     *            kv.eq.igetbits(isdat1(j),1,12)+1) then
                if (idcode .eq. igetbits(isdat1(j),7,19)) then
                  ndxslur = igetbits(isdat1(j),4,28)
c
c  080531 Allow >16 slurs
c
     *                          +16*igetbits(isdat1(j),1,18)
                  if (btest(isdat1(j),27)) slurudq = 'u'
                  go to 4
                end if
              end if
3           continue
            print*,'Bad place in doslur'
            call stop1()
4           continue
            if (nolevs.eq.0 .or. nolevs.gt.60) then
c
c  Ending is on a rest, reset nolevs to default starting height
c
              nolevs = igetbits(isdat2(j),7,19)
            end if
            if (btest(isdat3(isdat),0)) then
c
c  Deal with \curve or \midslur
c
              imid = igetbits(isdat3(isdat),6,2)-32 
c
c  Remember, only dealing with non-ps slurs
c
c  Who knows where the following line came from.  Removed it 6/30/02 to 
c  restore behavior of non-ps slurs to old way
c              if (slurudq .eq. 'd') imid = -imid
c  3/8/03 added the following
c
              if (slurudq .eq. 'd') imid = -abs(imid)
c
              if (btest(isdat3(isdat),1)) then
                notexq = chax(92)//'curve'
                lnote = 6
              else
                notexq = chax(92)//'midslur'
                lnote = 8
              end if
              if (imid.lt.0 .or. imid.gt.9) then
c
c  Need brackets
c
                notexq = notexq(1:lnote)//'{'
                lnote = lnote+1
                if (imid .lt. -9) then
                  write(notexq(lnote+1:lnote+3),'(i3)')imid
                  lnote = lnote+3
                else if (imid.lt.0 .or. imid.gt.9) then
                  write(notexq(lnote+1:lnote+2),'(i2)')imid
                  lnote = lnote+2
                else
                  write(notexq(lnote+1:lnote+1),'(i1)')imid
                  lnote = lnote+1
                end if
                notexq = notexq(1:lnote)//'}'
                lnote = lnote+1
              else
c
c  1=<imid=<9, no brackets
c
                notexq = notexq(1:lnote)//char(48+imid)
                lnote = lnote+1
              end if
              if (btest(isdat3(isdat),1)) then
c
c  \curve; 3 args
c
                notexq = notexq(1:lnote)
     *                      //char(48+igetbits(isdat3(isdat),3,8))
                notexq = notexq(1:lnote+1)
     *                      //char(48+igetbits(isdat3(isdat),3,11))
                lnote = lnote+2
              end if 
              call addstr(notexq,lnote,soutq,lsout)
            end if
c
c  Shift slur ending for stem on any note?
c
            if (.not.stemup .and. slurudq.eq.'d' .and. tno.lt.63.) 
     *                ihoff = ihoff-8
            if (btest(isdat2(isdat),3)) then
c
c  Shift ending for tie spacing
c
              if (slurudq .eq. 'u') then
                ihoff = ihoff-8
                ivoff = ivoff-1
              else if (slurudq .eq. 'd') then
                ivoff = ivoff+1
                if (stemup.or. tno.gt.63.) ihoff = ihoff-8
              end if
            end if
            if (iscrd) then
c
c  Shift termination for shifted notehead?
c
              if (btest(isdat2(isdat),1)) then
c
c  Left-shifted chord notehead.  ASSUME downstem.
c
                if (nolevs.eq.minlev .and. slurudq.eq.'d') then
                  ihoff = ihoff-2
                else
                  ihoff = ihoff-10
                end if
              else if (btest(isdat2(isdat),2)) then
c
c  Right shifted chord notehead.  ASSUME upstem.
c
                if (nolevs.eq.maxlev .and. slurudq.eq.'u') then
                  ihoff = ihoff+2
                else
                  ihoff = ihoff+10
                end if
              end if
            end if
            if (ihoff .eq. 0) then
              notexq = chax(92)//'tslur'
              lnote = 6
            else
c
c  Shift needed
c
              notexq = chax(92)//'ts'
              lnote = 3
            end if
c
c  Print 23-ndxslur
c  5/25/08 Allow 14 slurs (???????????)
c
            if (23-ndxslur .lt. 10) then
              notexq = notexq(1:lnote)//chax(71-ndxslur)
              lnote = lnote+1
            else if (23-ndxslur .lt. 20) then
              notexq = notexq(1:lnote)//'{1'//chax(61-ndxslur)//'}'
              lnote = lnote+4
            else 
              notexq = notexq(1:lnote)//'{2'//chax(51-ndxslur)//'}'
              lnote = lnote+4
            end if
            if (btest(iornq,11).or.btest(iornq,12)) then
c
c  Raise or lower slur by one unit provided . or _ is on same side as slur
c
              ivoffinc = 0
              if ((stemup .and. slurudq.eq.'d') .or.
     *                 (.not.stemup .and. slurudq.eq.'u')) then
                if (stemup) then
                  ivoffinc = -1
                else
                  ivoffinc = 1
                end if
                if (((stemup .and. nolev.ge.ncm-2) .or.
     *              (.not.stemup .and. nolev.le.ncm+2)) .and.
     *               mod(abs(ncm-nolev),2).eq.0) ivoffinc = 2*ivoffinc
              end if
              ivoff = ivoff+ivoffinc
            end if
            if (idcode.eq.1 .and. tmove) then
c
c  t-slur height adjustment
c
              iupdn = 1
              if (slurudq .eq. 'd') iupdn = -1
            end if
            call notefq(noteq,lnoten,nolevs+iupdn+ivoff,ncm)
            if (slurudq.eq.'u' .and.
     *           (.not.btest(isdat2(isdat),0).or.nolevs.eq.maxlev)) then
              islhgt = nolevs+iupdn+ivoff
c
c  If topvoice, upslur, and idcode checks, no more need to keep hgt for barno.
c
              if (ivx.eq.ivmx(nv,nvmx(nv)) .and. is1n1.gt.0) then
                if (idcode .eq. is2n1) is1n1=0
              end if
            end if
            notexq = notexq(1:lnote)//noteq(1:lnoten)
            lnote = lnote+lnoten
            if (ihoff .ne. 0) then
              shift = ihoff*0.1
              notexq = notexq(1:lnote)//'{'
              lnote = lnote+1
              lform = lfmt1(shift)
              write(notexq(lnote+1:lnote+lform),
     *            '(f'//chax(48+lform)//'.1)')shift
              lnote = lnote+lform
              notexq = notexq(1:lnote)//'}'
              lnote = lnote+1
            end if
            call addstr(notexq,lnote,soutq,lsout)
c
c  Clear the bit from list of slurs in use
c
            listslur = ibclr(listslur,ndxslur)
c
c  Zero out the entire strings for start and stop
c
            isdata = 0
            isdat2(isdat) = 0
            isdat1(j) = 0
            isdat2(j) = 0
            isdat3(isdat) = 0
c
c  Set midi info for slur ending
c
            if (ismidi .and. idcode.eq.32) then 
              icm = midchan(iv,kv)
              if (slmon(icm)) then
                if (nolevs+miditran(instno(iv)).eq.levson(icm) .and.
     *              iand(7,nacc).eq.0) then
c
c  There is a tie here.  NB!!! assumed no accidental on 2nd member of tie.
c
                  levsoff(icm) = nolevs+miditran(instno(iv))
                  settie = .true.
                else
                  levsoff(icm) = 0
                  levson(icm) = 0
                  slmon(icm) = .false.
                end if
              end if
            end if 
          end if
          isdat1(isdat) = isdata
        end if
1     continue
c
c  Clear and collapse the slur data list
c
      numdrop = 0
      do 2 isdat  = 1 , nsdat
        if (isdat1(isdat) .eq. 0) then
          numdrop = numdrop+1
        else if (numdrop .gt. 0) then
          isdat1(isdat-numdrop) = isdat1(isdat)
          isdat2(isdat-numdrop) = isdat2(isdat)
          isdat3(isdat-numdrop) = isdat3(isdat)
          isdat1(isdat) = 0
          isdat2(isdat) = 0
          isdat3(isdat) = 0
        end if
2     continue
      nsdat = nsdat-numdrop
c      call report(nsdat,isdat1,isdat2)
      return
      end
      subroutine dotmov(updot,rtdot,soutq,lsout,iddot)
c
c  iddot = 0 for single dot, 1 for double
c
      character*80 soutq,notexq
      character*1 sq,chax
      sq = chax(92)
      lfmtup = lfmt1(updot)
      lfmtrt = lfmt1(rtdot)
      write(notexq,'(a37,f'//chax(48+lfmtup)//'.1,a2,f'//chax(48+lfmtrt)
     *    //'.1,a15)')
     * sq//'makeatletter'//sq//'def'//sq//'C@Point#1#2{'//sq//
     * 'PMXpt{',updot,'}{',rtdot,'}'//chax(48+iddot)//'}'//sq
     * //'makeatother'
c
c   Example of string just created:
c   \makeatletter\def\C@Point#1#2{\PMXpt{.5}{.5}}\makeatother\
c
      lnote = 54+lfmtup+lfmtrt
      call addstr(notexq(1:lnote),lnote,soutq,lsout)
      return
      end
      subroutine dotrill(iv,ip,iornq,noteq,lnoten,notexq,lnote)
      common /comtrill/ ntrill,ivtrill(24),iptrill(24),xnsktr(24),
     *                ncrd,icrdat(193),icrdot(193),icrdorn(193),nudorn,
     *                kudorn(63),ornhshft(63),minlev,maxlev,icrd1,icrd2
      character*1 chax
      character*8 noteq
      character*79 notexq
      logical tronly,btest
      do 1 itr = 1 , ntrill
        if (iv.eq.ivtrill(itr) .and. ip.eq.iptrill(itr)) go to 2
1     continue
      print*,'Problem in dotrill.  Call Dr. Don'
      stop
2     continue
      tronly = xnsktr(itr) .lt. 0.01
      if (tronly) then
        notexq = chax(92)//'zcharnote'
        lnote = 10
      else if (btest(iornq,7)) then
        notexq = chax(92)//'Trille'
        lnote = 7
      else
        notexq = chax(92)//'trille'
        lnote = 7
      end if
      notexq = notexq(1:lnote)//noteq(1:lnoten)//'{'
      lnote = lnote+lnoten+1
c
c  Write trill duration to nearest tenth of a noteskip
c
      if (tronly) then
        notexq = notexq(1:lnote)//chax(92)//'it tr}'
        lnote = lnote+7
        return
      end if
      if (xnsktr(itr) .lt. .95) then
        nfmt = 2
      else if (xnsktr(itr) .lt. 9.95) then
        nfmt = 3
      else
        nfmt = 4
      end if
      write(notexq(lnote+1:lnote+nfmt),'(f'//chax(48+nfmt)//'.1)')
     *   xnsktr(itr)
      lnote = lnote+nfmt
      notexq = notexq(1:lnote)//'}'
      lnote = lnote+1
      return
      end
      subroutine endslur(stemup,upslur,nolev,iupdn,ndxslur,ivoff,ncm,
     *  soutq,lsout,fontslur)
c
c  Only called to end slur started in dograce.
c
      logical shift,stemup,upslur,fontslur
      character*80 soutq
      character*79 notexq
      character*8 noteq
      character*1 chax
      shift = .not.stemup .and. .not.upslur
      if (.not.shift) then
c
c  No shift needed
c
         notexq = chax(92)//'tslur'
         lnote = 6
      else
c
c  Shift needed
c
         notexq = chax(92)//'ts'
         lnote = 3
      end if
c      if (ndxslur .lt. 10) then
c        notexq = notexq(1:lnote)//chax(48+ndxslur)
c        lnote = lnote+1
c      else
c        notexq = notexq(1:lnote)//'{1'//chax(38+ndxslur)//'}'
c        lnote = lnote+4
c      end if
c
cc  Print 11-ndxslur
c  Print 23-ndxslur
c
c      if (11-ndxslur .lt. 10) then
      if (23-ndxslur .lt. 10) then
c        notexq = notexq(1:lnote)//chax(59-ndxslur)
        notexq = notexq(1:lnote)//chax(71-ndxslur)
        lnote = lnote+1
      else if (23-ndxslur .lt. 20) then
c              notexq = notexq(1:lnote)//'{1'//chax(49-ndxslur)//'}'
        notexq = notexq(1:lnote)//'{1'//chax(61-ndxslur)//'}'
        lnote = lnote+4
      else 
        notexq = notexq(1:lnote)//'{2'//chax(51-ndxslur)//'}'
        lnote = lnote+4
      end if
      call notefq(noteq,lnoten,nolev+iupdn+ivoff,ncm)
      notexq = notexq(1:lnote)//noteq(1:lnoten)
      lnote = lnote+lnoten
      if (shift) then
        if (fontslur) then 
          notexq = notexq(1:lnote)//'{-.6}'
        else
          notexq = notexq(1:lnote)//'{-.8}'
        end if
        lnote = lnote+5
      end if
      call addstr(notexq,lnote,soutq,lsout)
      return
      end
      subroutine errmsg(lineq,iccount,ibarno,msgq)
      parameter(nm=24)
      common /c1omget/ lastchar,fbon,issegno,ihead,isheadr,nline,isvolt,
     *     fracindent,nsperi(nm),linesinpmxmod,line1pmxmod,lenbuf0
      logical lastchar,fbon,issegno,isheadr,isvolt
      character*128 lineq
      character*78 outq
      character*(*) msgq
      character*1 chax
      common /truelinecount/ linewcom(20000)
      if (iccount .le. 78) then
        outq = lineq(1:78)
        iposn = iccount
      else
        outq = '... '//lineq(55:128)
        iposn = iccount-50
      end if
      print*
      ibarnop = ibarno
      if (linesinpmxmod.eq.0 .or. 
     *    nline .gt. line1pmxmod+linesinpmxmod) then
c
c  Error is in main .pmx file
c
c        nlinep = nline-linesinpmxmod
c       Correct for comments not copied into buffer
        nlinep = linewcom(nline)-linesinpmxmod
      else
c
c  Error is in include file
c
        ibarnop = 0
        nlinep = nline-line1pmxmod+1
        call printl(
     *    'ERROR in include file named above, description given below')
      end if
      open(19,file='pmxaerr.dat')
      write(19,'(i6)') nlinep
      close(19)
      ndigbn = max(1,int(log10(ibarnop+.1)+1))
      ndignl = int(log10(nlinep+.1)+1)
      lenmsg = index(msgq,'!')-1
c
c  Split off msgq(..) since UNIX compilers don't allow concat substring!!!
c
      write(*,'(/,a15,i'//chax(48+ndignl)//',a6,i'//chax(48+ndigbn)//
     *',$)')' ERROR in line ',nlinep,', bar ',ibarnop
      write(*,'(1x,a)')msgq(1:lenmsg)
      write(15,'(/,a15,i'//chax(48+ndignl)//',a6,i'//chax(48+ndigbn)//
     *',$)')' ERROR in line ',nlinep,', bar ',ibarnop
      write(15,'(a)')msgq(1:lenmsg)
      i10 = iposn/10
      i1 = iposn-10*i10
      write(*,'('//chax(48+i10)//chax(48+i1)//'x,a)')'v'
      write(15,'('//chax(48+i10)//chax(48+i1)//'x,a)')'v'
      print*,outq(1:78)
      write(15,'(a)')' '//outq(1:78)
      write(*,'('//chax(48+i10)//chax(48+i1)//'x,a)')'^'
      write(15,'('//chax(48+i10)//chax(48+i1)//'x,a)')'^'
      return
      end
      subroutine eskb4(ip,ivx,in,ib,space,tstart,fbar,itrpt,esk)
c
c  Get elemskips to previous note.  Called only for graces, no xtups involved.
c
      parameter (nm=24)
      common /all/ mult(nm,200),iv,nnl(nm),nv,ibar,
     *   ivxo(600),ipo(600),to(600),tno(600),tnote(600),eskz(nm,200),
     *   ipl(nm,200),ibm1(nm,9),ibm2(nm,9),nolev(nm,200),ibmcnt(nm),
     *   nodur(nm,200),jn,lenbar,iccount,nbars,itsofar(nm),nacc(nm,200),
     *   nib(nm,15),nn(nm),lenb0,lenb1,slfac,musicsize,stemmax,
     *   stemmin,stemlen,mtrnuml,mtrdenl,mtrnmp,mtrdnp,islur(nm,200),
     *   ifigdr(2,125),iline,figbass,figchk(2),firstgulp,irest(nm,200),
     *   iornq(nm,0:200),isdat1(202),isdat2(202),nsdat,isdat3(202),
     *   isdat4(202),beamon(nm),isfig(2,200),sepsymq(nm),sq,ulq(nm,9)
      character*1 ulq,sepsymq,sq
      logical beamon,firstgulp,figbass,figchk,isfig
      real*4 tstart(80),space(80)
      common /comtol/ tol
      itnd = nint(to(in))
      if (ip.eq.1 .or. itnd.eq.itrpt) then
c
c  Start of bar or after rpt.
c
        esk = fbar
        return
      else
        esk = 0.
        itprev = itnd-nodur(ivx,ip-1)
        do 1 iib = ib , 1 , -1
          if (tstart(iib) .lt. itprev+tol) then
c
c  This is the block
c
            nnsk = nint(float((itnd-itprev))/space(iib))
            esk = esk+nnsk*feon(space(iib))
            return
          else
            nnsk = nint((itnd-tstart(iib))/space(iib))
            esk = esk+nnsk*feon(space(iib))
            itnd = nint(tstart(iib))
          end if
1       continue
      end if
      print*,'Problem in eskb4.  Send files to Dr. Don'
      stop
      end
      function f1eon(time)
        f1eon = sqrt(time/2)
      return
      end
      function feon(time)
        common /comeon/ eonk,ewmxk
        feon = sqrt(time/2)**(1.-eonk)*ewmxk
      return
      end
      subroutine findbeam(ibmrep,numbms,mapfb)
c
c  Called once per voice per bar, after setting forced beams.
c
      parameter (nm=24)
      dimension mask(49,3),nummask(3),eqonly(49,3)
c      integer numbms(nm),ipr(48),nip1(0:47),nip2(0:47),mapfb(16),
c     *       itr(48),nodue(48)
c      logical short(48),eqonly
      integer numbms(nm),ipr(248),nip1(0:247),nip2(0:247),mapfb(16),
     *       itr(248),nodue(248)
      logical short(248),eqonly
      common /all/ mult(nm,200),iv,nnl(nm),nv,ibar,
     *   ivxo(600),ipo(600),to(600),tno(600),tnote(600),eskz(nm,200),
     *   ipl(nm,200),ibm1(nm,9),ibm2(nm,9),nolev(nm,200),ibmcnt(nm),
     *   nodur(nm,200),jn,lenbar,iccount,nbars,itsofar(nm),nacc(nm,200),
     *   nib(nm,15),nn(nm),lenb0,lenb1,slfac,musicsize,stemmax,
     *   stemmin,stemlen,mtrnuml,mtrdenl,mtrnmp,mtrdnp,islur(nm,200),
     *   ifigdr(2,125),iline,figbass,figchk(2),firstgulp,irest(nm,200),
     *   iornq(nm,0:200),isdat1(202),isdat2(202),nsdat,isdat3(202),
     *   isdat4(202),beamon(nm),isfig(2,200),sepsymq(nm),sq,ulq(nm,9)
      common /comipl2/ ipl2(nm,200)
      common /combeam/ ibmtyp
      common /commvl/ nvmx(nm),ivmx(nm,2),ivx
      common /comtol/ tol
      character*1 ulq,sepsymq,sq
      logical beamon,firstgulp,figbass,figchk,
     *        isfig,btest
      data nip1,nip2 /496*0/
      data nummask / 29 , 49 , 12 /
      data mask
     * /  65535,     4095,    65520,      255,    65280,       63,
     *      252,    16128,    64512,       15,      240,     3840,
     *    61440,        7,       14,      112,      224,     1792,
     *     3584,    28672,    57344,        3,       12,       48,
     *      192,      768,     3072,    12288,    49152, 20*0 ,
     * 16777215,    65535, 16776960,     4095,    65520,  1048320,
     * 16773120,      255,    65280, 16711680,       63,      252,
     *    16128,    64512,  4128768, 16515072,       15,       60,
     *      240,     3840,    15360,    61440,   983040,  3932160,
     * 15728640,        7,       14,      112,      224,     1792,
     *     3584,    28672,    57344,   458752,   917504,  7340032,
     * 14680064,        3,       12,       48,      192,      768,
     *     3072,    12288,    49152,   196608,   786432,  3145728,
     * 12582912,
     *     4095,      255,     4080,       15,      240,     3840,
     *        3,       12,       48,      192,      768,     3072,
     *  37*0 /
      data eqonly /3*.true.,46*.false.,7*.true.,91*.false./
      ip = 0
      nreal = 0
      itnow = 0
1     continue
        ip = ip+1
        if (ip .gt. nn(ivx)) go to 9
11      if (nodur(ivx,ip).eq.0) then
c
c  Ignore all xtup notes except the last, the one with nodur > 0 .
c  Xtups are irrelevant here since they are already all in forced beams.
c  Will update itnow by nodur at the END of this loop
c
          ip = ip+1
          go to 11
        end if
        nreal = nreal+1
        nodue(nreal) = nodur(ivx,ip)
        short(nreal) = nodue(nreal).lt.16 .and.
     *                     .not.btest(irest(ivx,ip),0)
c
c  Rule out notes that have 'alone'-flag set
c
     *   .and..not.btest(islur(ivx,ip),18)
        ipr(nreal) = ip
        itr(nreal) = itnow
        if (nodue(nreal) .eq. 1) then
c
c  64th gap
c
          if (mod(itnow,2) .eq. 0) then
c
c  Start of 32nd gap, lump with following note
c
            ip = ip+1
		  nodue(nreal) = 1+nodur(ivx,ip)
            itnow = itnow+nodue(nreal)
          else
c
c  End of 32nd gap, lump with preceeding note
c
            nreal = nreal-1
            nodue(nreal) = 1+nodue(nreal)
            itnow = itnow+1
          end if
        else
          itnow = itnow+nodur(ivx,ip)
        end if
      go to 1
9     continue
      ir1 = 1
      itseg = lenbar/ibmrep
      do 13 irep = 1 , ibmrep
c
c  Set bitmaps for all shorts neighbored by a short. Each bit represents a
c  span of 32nd note.  maps, mapm, mape record start, full duration, and end
c  of consecutive span of beamable (<1/4) notes.
c
        maps = 0
        mapm = 0
        mape = 0
        itend = itseg*irep
        itoff = itend-itseg
        do 2 ir = ir1 , nreal
          it2 = itr(ir)+nodue(ir)-2
          if (it2 .ge. itend) then
            ir1 = ir
            go to 14
          end if
c         if (short(ir).and.((ir.gt.1.and.short(ir-1)).or.(ir.lt.nreal
          if (short(ir).and.((ir.gt.1.and.short(max(ir-1,1))).or.
     *          (ir.lt.nreal.and.short(ir+1)))) then
            ib1 = (itr(ir)-itoff)/2
            ib2 = (it2-itoff)/2
            if (max(ib1,ib2).gt.47 .or. ir.gt.48 .or.
     *                       min(ib1,ib2).lt.0) return
c
c  Must have an odd number obe beats in a long bar.  Auto-beam won't work
c
            nip1(ib1) = ipr(ir)
            nip2(ib2) = ipr(ir)
c
c  nip1,2(ib) = 0 unless a real note starts,ends on bit ib; then = ip
c
            maps = ibset(maps,ib1)
            mape = ibset(mape,ib2)
            do 3 ib = ib1 , ib2
              mapm = ibset(mapm,ib)
3           continue
          end if
2       continue
14      continue
        if (mapm .eq. 0) go to 13
c
c  Zero out bits from forced beams
c
        maps = iand(maps,not(mapfb(irep)))
        mapm = iand(mapm,not(mapfb(irep)))
        mape = iand(mape,not(mapfb(irep)))
c
c  Compare map with template.
c
        do 4 it = 1 , nummask(ibmtyp)
          masknow = mask(it,ibmtyp)
          if (iand(masknow,mapm) .eq. masknow) then
c
c  Find least significant bit in the mask to check start time
c
            mtemp = masknow
            maskm = masknow
            do 5 is1 = 0 , 47
              if (iand(1,mtemp) .eq. 1) go to 6
              mtemp = ishft(mtemp,-1)
5           continue
6           continue
            if (iand(ishft(1,is1),maps) .eq. 0) go to 4
c
c  is1 is the bit where the beam starts.  Continue shifting to
c  find most significant bit in the mask to check ending time
c
            do 7 is2 = is1 , 47
              mtemp = ishft(mtemp,-1)
              if (iand(1,not(mtemp)) .eq. 1) go to 8
7           continue
8           continue
c
c  is2 is now the bit on which the beam ends.
c
            if (iand(ishft(1,is2),mape) .eq. 0) go to 4
c
c  Did we pick out a single note from the middle of a longer sequence?
c
            if (nip1(is1) .eq. nip2(is2)) go to 4
c
c  We almost have a beam.  Check equality of notes if needed.
c
            if (eqonly(it,ibmtyp)) then
              do 10 ip = nip1(is1) , nip2(is2)
                if (nodur(ivx,ip) .ne. 8) then
c
c  There is a non-1/8th note in this beam. Exit if not 2 quarters
c
                  if (is2-is1 .ne. 15) go to 4
c
c  Beam is 2 quarters long.  Check if can split in half.
c
                  ithalf = 0
                  do 20 iip = nip1(is1) , nip2(is2)
                    ithalf = ithalf+nodur(ivx,iip)
                    if (ithalf .gt. 16) go to 4
                    if (ithalf .eq. 16) go to 21
20                continue
                  print*,'Problem in findbeam, please call Dr. Don'
                  go to 4
21                continue
c
c  Otherwise, split in half by keeping only the first half.  Other half will
c  be picked up later, assuming masks are listed longest first.
c
                  is2 = is1+7
c
c  Reset maskm (since only used part of mask), used later to zero out
c  bits that contain beams
c
                  maskm = 0
                  do 15 is = is1 , is2
                    maskm = ibset(maskm,is)
15                continue
                  go to 16
                end if
10            continue
            end if
16          continue
c
c  This is a beam.  If last "effective" ends on odd 64th, add 1 more
c
c            if (abs(mod(to(iand(255,ipl(ivx,nip2(is2))))
c     *           +nodur(ivx,nip2(is2)),2.)) .gt. tol) then
            if (abs(amod(to(ipl2(ivx,nip2(is2)))
     *           +nodur(ivx,nip2(is2))+.5*tol,2.)) .gt. tol) then
              nip2(is2) = nip2(is2)+1
            end if
            numbms(ivx) = numbms(ivx)+1
            numnew = numbms(ivx)
            call logbeam(numnew,nip1(is1),nip2(is2))
c
c  Zero out the appropriate bits so these notes don't get used again
c
            mapm = iand(mapm,not(maskm))
            if (mapm.eq.0) go to 13
            maps = iand(maps,not(maskm))
            mape = iand(mape,not(maskm))
          end if
4       continue
13    continue
      return
      end
      subroutine findeonk(nptr1,nptr2,wovera,xelsk,dtmin,dtmax,eonk0)
      parameter (nkb=3999,maxblks=9600)
c
c  Compute an exponent eonk for use in the "flattened" formula for elemskips
c   vs time.  We must solve the eqution f = 0.  Initial quess is eonk0.
c
c      logical gotclef
      common /c1omnotes/ nnodur,wminnh(nkb),nnpd(maxblks),durb(maxblks),
     *     iddot,nptr(nkb),ibarcnt,mbrest,ibarmbr,
c     *     ibaroff,udsp(nkb),wheadpt,gotclef,sqzb(maxblks)
     *     ibaroff,udsp(nkb),wheadpt,sqzb(maxblks)
      common /comtol/ tol
      common /comeon/ eonk,ewmxk
      eonk = eonk0
      niter = 0
1     continue
      ewmxk = f1eon(dtmax)**eonk
      niter = niter+1
      esum = 0.
      desum = 0.
      do 2 iptr = nptr1 , nptr2
        targ = durb(iptr)/sqzb(iptr)
        esum = esum+nnpd(iptr)*sqzb(iptr)*feon(targ)
        detarg = sqrt(targ/2*(dtmax/targ)**eonk)*alog(dtmax/targ)
        desum = desum+nnpd(iptr)*sqzb(iptr)*detarg
2     continue
      f = wovera*feon(dtmin)-xelsk-esum
      fp = wovera*sqrt(dtmin/2*(dtmax/dtmin)**eonk)*alog(dtmax/dtmin)
     *     -desum 
      if (abs(fp).lt.tol .or. abs(eonk-.5).gt..5 .or. niter.gt.100) then
        call printl(
     *     'Error in findeonk.  Please send source to Dr. Don')
        eonk = 0.
        ewmxk = 1.
        return
      end if
      dsoln = -f/fp
      if (abs(dsoln) .lt. .1*tol) return
c
c  Not converged yet, try again
c
      eonk = eonk+dsoln
      go to 1
      end
      function fnote(nodur,ivx,ip,nacc)
c
c  This return the real duration of a note
c
      parameter (nm=24)
      integer*4 nodur(nm,200),nacc(nm,200)
      logical btest
      ipback = ip
      if (nodur(ivx,ip) .gt. 0) then
        if (ip .gt. 1) then
c
c  Check if this is last note of xtup
c
          if (nodur(ivx,ip-1).eq.0) then
            ipback = ip-1
            go to 2
          end if
        end if
        fnote = nodur(ivx,ip)
        return
      end if
2     continue
c
c  Count back to prior non zero note. Start at ip to avoid neg index if ip=1.
c  Count how many doubled xtups notes there are from ip-1 to first note. 
c
      ndoub = 0
      do 1 ip1m1 = ipback , 1 , -1
        if (nodur(ivx,ip1m1) .gt. 0) go to 4
        if (ip1m1.lt.ip .and. btest(nacc(ivx,ip1m1),18)) ndoub=ndoub+1
1     continue
4     continue      
c
c  count forward to next non-0 nodur. Start at ip in case last note of xtup. 
c
      do 3 iip = ip , 200
c
c  Count doubled xtup notes from ip to end.
c
        if (btest(nacc(ivx,iip),18)) ndoub = ndoub+1
        if (nodur(ivx,iip) .gt. 0) then
c          fnote = nodur(ivx,iip)/float(iip-ip1m1)
          fnote = nodur(ivx,iip)/float(iip-ip1m1+ndoub)
          if (btest(nacc(ivx,ip),18)) then
            fnote = 2*fnote
          else if (btest(nacc(ivx,ip),27)) then
            fnote = 1.5*fnote
          else if (ip .gt. 1) then
            if (btest(nacc(ivx,ip-1),27)) fnote = .5*fnote
          end if 
          return
        end if
3     continue
      print*,' '
      call printl
     *   ('Probable misplaced barline or incorrect meter, stopping')
      print*,'ivx,ip:',ivx,ip
      call stop1()
      end
      subroutine g1etchar(lineq,iccount,charq)
      parameter(nm=24)
      common /c1omget/ lastchar,fbon,issegno,ihead,isheadr,nline,isvolt,
     *     fracindent,nsperi(nm),linesinpmxmod,line1pmxmod,lenbuf0
      logical lastchar,issegno,isheadr,isvolt,fbon
      common /commac/ macnum,mrecord,mplay,macuse,icchold,lnholdq,endmac
      logical mrecord,mplay,endmac
      character*1 charq
      character*128 lineq,lnholdq
c
c  Gets the next character out of lineq*128.  If pointer iccount=128 on entry,
c  then reads in a new line.  Resets iccount.  Ends program if no more input.
c
      if (iccount .eq. 128) then
        call read10(lineq,lastchar)
        if (lastchar) return
        if (.not.endmac) then
          iccount = 0
          if (.not.mplay) nline = nline+1
        else
          endmac = .false.
          iccount = icchold
          lineq = lnholdq
        end if
        if (mrecord) then
          call m1rec1(lineq,iccount,ibarcnt,ibaroff,nbars,ndxm)
        end if
      end if
      iccount = iccount+1
      charq = lineq(iccount:iccount)
      return
      end
      subroutine g1etnote(loop,ifig,optimize,fulltrans)
      parameter (nm=24,nkb=3999,mv=24576,maxblks=9600)
      character*1 chax
      logical twotrem
      common /a1ll/ iv,ivxo(600),ipo(600),to(600),tno(600),nnl(nm),
     *   nv,ibar,mtrnuml,nodur(nm,200),lenbar,iccount,
     *   nbars,itsofar(nm),nib(nm,15),nn(nm),
     *   rest(nm,200),lenbr0,lenbr1,firstline,newmeter
      common /c1omnotes/ nnodur,wminnh(nkb),nnpd(maxblks),durb(maxblks),
     *     iddot,nptr(nkb),ibarcnt,mbrest,ibarmbr,
c     *     ibaroff,udsp(nkb),wheadpt,gotclef,sqzb(maxblks)
     *     ibaroff,udsp(nkb),wheadpt,sqzb(maxblks)
      common /c1omget/ lastchar,fbon,issegno,ihead,isheadr,nline,isvolt,
     *     fracindent,nsperi(nm),linesinpmxmod,line1pmxmod,lenbuf0
      common /compage/ widthpt,ptheight,hoffpt,voffpt,
     *      nsyst,nflb,ibarflb(0:40),
     *      isysflb(0:40),npages,nfpb,ipagfpb(0:18),isysfpb(0:18),
     *      usefig,fintstf,gintstf,fracsys(30),nmovbrk,isysmb(0:30),
     *      nistaff(0:40)
      common /c1ommvl/ nvmx(nm),ivmx(nm,2),ivx,fbar,nacc(nm,200)
      common /comkeys/ nkeys,ibrkch(18),newkey(18),iskchb,idsig,isig1,
     *      mbrestsav,kchmid(18),ornrpt,shifton,barend,noinst,stickyS
      logical lastchar,firstline,rest,loop,newmeter,fbon,issegno,barend,
     *        isheadr,fulbrp,usefig,isvolt,iskchb,kchmid,plusmin,ornrpt,
     *        stickyS
      common /commac/ macnum,mrecord,mplay,macuse,icchold,lnholdq,endmac
      common /commus/ musize,whead20
      logical mrecord,mplay,endmac,shifton,gotclef,optimize
      character*128 lineq,lnholdq
      character*1 charq,dotq,dumq,durq,charlq
      integer*2 mmidi
      logical restpend,relacc,notmain,twoline,ismidi,crdacc,cdot
      common /commidi/ imidi(0:nm),trest(0:nm),mcpitch(20),mgap,
     *       iacclo(0:nm,6),iacchi(0:nm,6),midinst(nm),
     *       nmidcrd,midchan(nm,2),numchan,naccim(0:nm),
     *       laccim(0:nm,10),jaccim(0:nm,10),crdacc,notmain,
     *       restpend(0:nm),relacc,twoline(nm),ismidi,mmidi(0:nm,mv),
     *       debugmidi
      logical debugmidi
      character*131072 bufq
      integer*2 lbuf(maxblks)
      common /inbuff/ ipbuf,ilbuf,nlbuf,lbuf,bufq
      logical novshrinktop,upslur,fontslur,ztrans,
     *        WrotePsslurDefaults,cstuplet
      common /comnvst/ novshrinktop,cstuplet
      common /comslur/ listslur,upslur(nm,2),ndxslur,fontslur
     *                 ,WrotePsslurDefaults,SlurCurve
      character*51 literq(3),lyrerq(5)
      common /comligfont/ isligfont
      logical isligfont
      common /comInstTrans/ iInstTrans(nm),iTransKey(nm),iTransAmt(nm),
     *  instno(nm),nInstTrans,EarlyTransOn,LaterInstTrans
      logical EarlyTransOn,LaterInstTrans
      logical fulltrans
      common /comsize/ isize(nm)
      common /commidisig/ midisig
      common /comis4bignv/ is4bignv,AIset
      logical is4bignv,AIset
      common /comshort/ shortfrac,codafrac,ishort,mbrsum,nmbr,nocodabn,
     *  poefa
      real*4 poefa(125)
      logical nocodabn
      data literq
     *   /'Literal TeX string cannot start with 4 backslashes!',
     *    'TeX string must have <129 char, end with backslash!',
     *    'Type 2 or 3 TeX string can only start in column 1!'/
      data lyrerq
     *   /'pmxlyr string must end with " followed by blank!',
     *    'pmxlyr string cannot extend past position 120!',
     *    'There must be "a" or "b" here!',
     *    'There must be "+" or "-" here!',
     *    'There must be an integer here!'/
      cdot = .false.
      twotrem = .false.
1     call g1etchar(lineq,iccount,charq)
      if (charq .ne. ' ') charlq = charq
      if (lastchar) then
        if (index('/%',charlq) .eq. 0) then
          print*
          print*,'WARNING:'
          print*,'Last non-blank character is "',charlq,'", not "/,%"'
          print*,'ASCII code:',ichar(charlq)
          write(15,'(/a)')'Last non-blank character is "'//charlq//
     *       '", not "/,%"'
          write(15,'(a11,2x,i3)')'ASCII code:',ichar(charlq)
c
c  Append " /" to last line.  NB lastchar=.true. => ilbuf=nlbuf+1.
c
          ilbuf = ilbuf-1
          lbuf(ilbuf) = lbuf(ilbuf)+2
          bufq = bufq(1:ipbuf)//' /'
          write(15,*)'appending <blank>/'
          print*,'appending <blank>/'
          lineq = lineq(1:iccount)//' /'
          lastchar = .false.
          go to 1 
        end if
        return
      end if
      if (charq .eq. ' ') then
        go to 1
      else if (charq.eq.'%' .and. iccount.eq.1) then
        iccount = 128
        go to 1
c
c  Replacement 1/22/12 since gfortran 4.7 with -O was choking here!
c
c      else if ((ichar(charq).ge.97.and.ichar(charq).le.103) .or.
      else if (index('abcdefg',charq).gt.0 .or.
     *       charq.eq.'r') then
c
c  This is a note/rest. gotclef is only used for checking for clef before "/"
c
        if (cdot) go to 28
c        if (gotclef) gotclef=.false.
        idotform = 0
        numnum = 0
        plusmin = .false.
28      nnl(ivx) = nnl(ivx)+1
        if (nnl(ivx) .gt. 200) then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *      '>200 notes in line of music. Use smaller blocks!')
          call stop1()
        end if
        dotq = 'x'
c
c  Check if this is 'r ' and previous note was full-bar-pause
c
        fulbrp = charq.eq.'r' .and. lineq(iccount+1:iccount+1) .eq.' '
     *    .and. nnl(ivx).gt.1 .and. rest(ivx,max(1,nnl(ivx)-1)) .and.
     *    nodur(ivx,max(1,nnl(ivx)-1)) .eq. lenbar
2       call g1etchar(lineq,iccount,durq)
        ic = ichar(durq)
        if (ic.le.57 .and. ic.ge.48) then
c
c  Digit
c
          if (numnum .eq. 0) then
            nnodur = ic-48
            numnum = 1
            go to 2
          else if (numnum .eq. 1) then
            if (charq .eq. 'r') then
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           'Only one digit allowed after rest symbol "r"!')
              call stop1()
            end if
            numnum = 2
            if (plusmin) then
              print*
              print*,'*********WARNING*********'
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           'Before version 1.2, +/- was ignored if octave was!')
              print*,
     *           'explicitly specified.  May need to edit old editions'
            end if
            go to 2
          else
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *             '>2 digits in note symbol!')
            call stop1()
          end if
        else if (durq.eq.'d') then
          dotq = durq
          if (lineq(iccount+1:iccount+1) .eq. 'd') then
            iddot = 1
            iccount = iccount+1
c
c  Since we flow out, double dots won't work with other dot options
c
          end if
          if (index('+-',lineq(iccount+1:iccount+1)) .gt. 0) then
c
c  move a dot, provided a number follows.
c
            call g1etchar(lineq,iccount,durq)
            call g1etchar(lineq,iccount,durq)
            if (index('0123456789-.',durq) .eq. 0) then
c
c  Backup, exit the loop normally
c
              iccount = iccount-2
              go to 2
            end if
            call readnum(lineq,iccount,dumq,fnum)
            if (index('+-',dumq) .gt. 0) then
c
c  Vertical shift also
c
              call g1etchar(lineq,iccount,durq)
              if (index('0123456789-.',durq) .eq. 0) then
                call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *               'Expected number after 2nd +/- (shift dot)!')
                call stop1()
              end if
              call readnum(lineq,iccount,durq,fnum)
            end if
            iccount = iccount-1
          end if
          go to 2
        else if (index('<>',durq) .gt. 0) then
c
c  Accidental shift
c
c          if (index('fsn',lineq(iccount-1:iccount-1)) .eq. 0) then 
          if (index('fsnA',lineq(iccount-1:iccount-1)) .eq. 0) then 
            call errmsg(lineq,iccount-1,ibarcnt-ibaroff+nbars+1,
c     *         'Expected "f", "s", or "n" before "<" or ">"!')
     *         'Expected "f", "s", "n" or "A" before "<" or ">"!')
            call stop1()
          end if
          ipm = 1
          if (durq .eq. '<') ipm=-1
          call g1etchar(lineq,iccount,durq)
          if (index('123456789.0',durq) .eq. 0) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *         'Expected number after </> (accidental shift)!')
            call stop1()
          end if
          call readnum(lineq,iccount,durq,fnum)
          fnum = ipm*fnum
          if (fnum.lt.-5.35 .or. fnum.gt.1.0) then
            call errmsg(lineq,iccount-1,ibarcnt-ibaroff+nbars+1,
     *       'Horizontal accidental shift must be >-5.35 and <1.0!')
            call stop1()
          end if
          iccount = iccount-1
          go to 2
        else if (index('+-',durq) .gt. 0) then
          if (charq .ne. 'r') then
            if (index('fsnA',lineq(iccount-1:iccount-1)) .gt. 0) then
              ipm = 1
              if (durq .eq. '-') ipm=-1
              if (index('0123456789',lineq(iccount+1:iccount+1))
     *                                                    .gt.0) then
c
c  This may be start of accidental shift, but may be octave jump; then duration
c
                icsav = iccount
                iccount = iccount+1
                call readnum(lineq,iccount,durq,fnum)
                if (index('+-',durq) .gt. 0) then
c
c  This is an accid shift since there's a 2nd consecutive signed number.
c  Check size of 1st number.
c
                  if (fnum .gt. 30.5) then
                    call errmsg(lineq,iccount-1,ibarcnt-ibaroff+nbars+1,
     *                'Vertical accidental shift must be less than 31!')
                    call stop1()
                  end if
                  ipm = 1
                  if (durq .eq. '-') ipm = -1
                  call g1etchar(lineq,iccount,durq)
                  if (index('1234567890.',durq) .eq. 0) then
                    call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *               'Expected 2nd number of accidental shift)!')
                    call stop1()
                  end if
                  call readnum(lineq,iccount,durq,fnum)
                  fnum = ipm*fnum
                  if (fnum.lt.-5.35 .or. fnum.gt.1.0) then
                    call errmsg(lineq,iccount-1,ibarcnt-ibaroff+nbars+1,
     *             'Horiz. accidental shift must be >-5.35 and <1.0!')
                    call stop1()
                  end if
                  iccount = iccount-1
                  go to 2
                else
c
c  Not accid shift, reset, then flow out
c
                  iccount = icsav
                end if
              end if
            end if
            plusmin = .true.
            if (numnum .eq. 2) then
              print*
              print*,'*********WARNING*********'
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           'Before version 1.2, +/- was ignored if octave was!')
              print*,
     *           'explicitly specified.  May need to edit old editions'
            end if
            go to 2
c
c  It's a rest containing +|- .  Must refer to a vertical shift.  Read past.
c
          else
            call g1etchar(lineq,iccount,durq)
            call readnum(lineq,iccount,durq,dum)
            if (lineq(iccount-1:iccount-1).eq.'.') iccount=iccount-1
            iccount = iccount-1
            go to 2
          end if
c        else if (index('ulare',durq) .gt. 0) then
        else if (index('ularec',durq) .gt. 0) then
          go to 2
        else if (index('LS',durq) .gt. 0) then
c
c Stemlength change
c
          call g1etchar(lineq,iccount,durq)
          if (index('.0123456789:',durq) .eq. 0) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *         'There must be a number or colon here!')
            call stop1()
          end if
          if (durq .eq. ':') then
            if (.not.stickyS) then
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *       'Turned off sticky stemlegth changes without turning on!')
              call stop1()
            end if
            stickyS = .false.
            go to 2
          end if
          call readnum(lineq,iccount,durq,dum)
c          if (dum.lt..5 .or. dum.gt.4.) then
          if ((durq.eq.'L'.and.dum.gt.20.).or.
     *                (durq.eq.'S'.and.dum.gt.4.)) then
            call errmsg(lineq,iccount-1,ibarcnt-ibaroff+nbars+1,
     *         'Stemlength change amount too big!')
            call stop1()
          end if
          if (durq .ne. ':') then
            iccount = iccount-1
          else
            if (stickyS) then
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *         'Turned on sticky stemshrinks when already on!')
              call stop1()
            end if
            stickyS = .true.
          end if
          go to 2                    
        else if (index('fsn',durq) .gt. 0) then
c
c Check for midi-only accid. CANNOT coesist with accidental position tweaks, so
c   MUST come right after "f,s,n"
c
          if (lineq(iccount+1:iccount+1) .eq. 'i') iccount=iccount+1
          go to 2
        else if (durq .eq. 'p') then
          fulbrp = charq.eq.'r'
          if (.not. fulbrp) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *         'The option "p" only works with "r" (rest)!')
            call stop1()
          end if
          go to 2
        else if (durq .eq. 'b') then
          if (charq .ne. 'r') then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *             'You entered "b"; I expected "rb"!')
            call stop1()
          else if (numnum .eq. 2) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *             'You entered "r" & "b" with two numbers!')
          end if
          go to 2
        else if (durq .eq. 'x') then
c
c  Xtuplet. Count number of doubled notes (for unequal xtups)
c
          if (btest(nacc(ivx,nnl(ivx)),18)) then
            ndoub = 1
          else
            ndoub = 0
          end if
c
c  Will set all durations to 0 except last one.
c
          call g1etchar(lineq,iccount,durq)
          if (index('123456789T',durq) .eq. 0) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *        'First char after "x" in xtuplet must be "1"-"9" or "T"!')
            call stop1()
          end if
          if (durq .eq. 'T') then
c
c  Set a flag for checking 2nd note inputs if dot is moved
c
            twotrem = .true.
c
c  Check all x-tremolo inputs here; set fnum=2
c
            fnum = 2
            call getchar(lineq,iccount,durq)
            if (index('0123 ',durq).eq.0) then
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *      'First char after "T" in xtuplet must be "0"-"3" or blank!')
              call stop1()
            else if (durq .ne. ' ') then
              call getchar(lineq,iccount,durq)
              if (index('0123 ',durq).eq.0) then
                call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *          'This char must be "0"-"3" or blank!')
                call stop1()
              else if (durq .ne. ' ') then
                call getchar(lineq,iccount,durq)
c
c  Probably blank unles other options entered
c
              end if
            end if
          else
c
c durq is digit, normal xtup
c
            call readnum(lineq,iccount,durq,fnum)
c
c Leaves durq at next char after number
c
            if (fnum .gt. 99) then
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *             'Xtuplet cannot have more than 99 notes!')
              call stop1()
            else if (index(' DFnd',durq) .eq. 0) then
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *               'Only legal characters here are " ","D","F","n"!')
              call stop1()
            end if
          end if
c
c  End of mandatory xtup inputs.  Check for options. Note D,F,d must precede n.
c
          if (index('DF',durq) .gt. 0) then
c
c  Double xtup note to make an un= xtup. Here, number already set, but may also
c    have used this before number was set.
c
            nacc(ivx,nnl(ivx)) = ibset(nacc(ivx,nnl(ivx)),18)
            ndoub = 1
            call g1etchar(lineq,iccount,durq)
          else if (durq .eq. 'd') then
            nacc(ivx,nnl(ivx)) = ibset(nacc(ivx,nnl(ivx)),27)
            call g1etchar(lineq,iccount,durq)
          end if
          if (durq .eq. 'n') then
c
c  Number alteration stuff.  After 'n', require '+-123456789fs ', no more 'DF'.
c
            numshft = 0
30          call g1etchar(lineq,iccount,durq)
            if (durq .eq. 'f') then
              go to 30
            else if (index('+-',durq) .gt. 0) then
              numshft = numshft+1
              if (numshft .eq. 3) then
                call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           'Only 2 shifts are allowed after "n" in xtup!')
                call stop1()
              end if
              call g1etchar(lineq,iccount,durq)
              if (index('0123456789.',durq) .eq. 0) then
                call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           'This character should be a digit or "."!')
                call stop1()
              end if
              call readnum(lineq,iccount,durq,snum)
              iccount = iccount-1
c              if ((numshft.eq.1 .and. snum.gt.15.1) .or. 
              if ((numshft.eq.1 .and. snum.gt.64.) .or. 
     *            (numshft.eq.2 .and. snum.gt.1.51)) then
                call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *              'Shift number after "n" in xtup is out of range!')
                call stop1()
              end if
              go to 30
            else if (durq .eq. 's') then
c
c  Slope alteration for bracket
c
              call getchar(lineq,iccount,durq)
              if (index('+-',durq) .eq. 0) then
                call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *      'For slope adjustment, this character must be "+" or "-"!')
                call stop1()
              end if
              call g1etchar(lineq,iccount,durq)
              if (index('123456789',durq) .eq. 0) then
                call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           'This character should be a digit!')
                call stop1()
              end if
              call readnum(lineq,iccount,durq,snum)
              iccount = iccount-1
              if (nint(snum) .gt. 15) then
                call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           'Slope adjustment cannot exceed 15!')
                call stop1()
              end if
              go to 30
            else if (index('123456789',durq) .gt. 0) then
c
c Unsigned integer => alternate printed number
c
              call readnum(lineq,iccount,durq,snum)
              if (snum .gt. 15.1) then
                call errmsg(lineq,iccount-1,ibarcnt-ibaroff+nbars+1,
     *              'Alternate xtup number after "n" must be <16!')
                call stop1()
              end if
              iccount = iccount-1
              go to 30
            else if (durq .ne. ' ') then
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *             'Illegal character after "n" in xtup!')
              call stop1()
            end if
          end if
          ntup = nint(fnum)
          do 6 itup = 2 , ntup
            nodur(ivx,nnl(ivx)) = 0
            nnl(ivx) = nnl(ivx)+1
110         call g1etchar(lineq,iccount,durq)
            if (durq.eq.' ') then
              go to 110
            else if (durq .eq. 'o') then
c
c  Ornament in xtup.  "o" symbol must come AFTER the affected note
c
              call g1etchar(lineq,iccount,dumq)
              if (index('(stmx+Tup._)e:>^bc',dumq) .eq. 0 ) then
                if (index('fg',dumq) .gt. 0) then
                  call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *               'Fermata or segno not allowed in xtuplet!')
                else
                  call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *                 'Illegal ornament!')
                end if
                call stop1()
              end if
              if (dumq .eq. 'T') then
c
c  Trill.  may be followed by 't' and/or number.  read 'til blank
c
29              call g1etchar(lineq,iccount,dumq)
                if (dumq .ne. ' ') go to 29
              else if (dumq .eq. 'e') then
                call g1etchar(lineq,iccount,dumq)
                if (index('sfn?',dumq) .eq. 0) then
                  call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *            'Illegal character after "e" in edit. accid. symbol!')
                  call stop1()
                end if
                call g1etchar(lineq,iccount,dumq)
                if (dumq .eq. '?') call g1etchar(lineq,iccount,dumq)
              else if (dumq .eq. ':') then
                if (lineq(iccount+1:iccount+1) .ne. ' ') then
                  call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *            '":" must be followed by blank in "o: "!')
                  call stop1()
                else if (.not.ornrpt) then
                  call errmsg(lineq,iccount-1,ibarcnt-ibaroff+nbars+1,
     *            'Turned off repeated ornaments before they were on!')
                  call stop1()
                end if
                ornrpt = .false.
              else
                call g1etchar(lineq,iccount,dumq)
              end if
              if (index('+- :',dumq) .eq. 0) then
                call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *            'Illegal character in ornament symbol!')
                call stop1()
              end if
              if (dumq .eq. ':') then
                if (lineq(iccount+1:iccount+1) .ne. ' ') then
                  call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *            '":" must be followed by blank in "o: "!')
                  call stop1()
                else if (ornrpt) then
                  call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *               'Turned on repeated ornaments but already on!')
                  call stop1()
                end if
                ornrpt = .true.
              end if
              if (index('+-',dumq) .gt. 0) then
                if (index('0123456789',lineq(iccount+1:iccount+1))
     *                                                   .eq. 0) then
                  call errmsg(lineq,iccount+1,ibarcnt-ibaroff+nbars+1,
     *             'There should be an integer here!')
                  call stop1()
                end if
                call readnum(lineq,iccount,durq,fnum)
                if (durq .eq. ':') then
                  call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *               'Cannot shift AND repeat an ornament!')
                  call stop1()
                end if
c
c  12/7/03 Allow horizontal shift on any ornament, not just breath and ceas.
c
                if (index('+-',durq) .gt. 0) then
                  if (index('.0123456789',lineq(iccount+1:iccount+1))
     *                                                   .eq. 0) then
                    call errmsg(lineq,iccount+1,
     *                            ibarcnt-ibaroff+nbars+1,
     *                            'There should be a number here!')
                    call stop1()
                  end if
                  call readnum(lineq,iccount,durq,fnum)
                end if
              end if
              go to 110
            else if (index('st(){}',durq) .gt. 0) then
c
c  Slur in xtup
c
              iposn = 0
              numint = 0
15            call g1etchar(lineq,iccount,dumq)
              iposn = iposn+1
              if (index('udlbfnhtv',dumq) .gt. 0) then
                if (dumq.eq.'t' .and. durq.eq.'t') then
                  call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *              'Cannot use "t" as an option on a tie!')
                  call stop1()
                end if
                go to 15
              else if (index('+-',dumq) .gt. 0) then
                numint = numint+1
                iccount = iccount+1
                call readnum(lineq,iccount,durq,fnum)
                if (numint .eq. 1) then
                  if (nint(fnum) .gt. 30) then
                    call errmsg(lineq,iccount-1,ibarcnt-ibaroff+nbars+1,
     *       'Magnitude of slur height adjustment cannot exceed 30!')
                    call stop1()
                  end if
                else if (numint .eq. 2) then
                  if (abs(fnum).gt.6.3) then
                    call errmsg(lineq,iccount-1,ibarcnt-ibaroff+nbars+1,
     *              'Slur horiz shift must be in the range (-6.3,6.3)!')
                    call stop1()
                  end if
                else
c
c  Third signed integer, must be a midslur or curve spec.
c
                  if (abs(fnum).gt.31) then
                    call errmsg(lineq,iccount-1,ibarcnt-ibaroff+nbars+1,
     *                  'Slur midheight must be in the range (-31,31)!')
                    call stop1()
                  end if
                  if (durq .eq. ':') then
c
c  Expecting curve parameters.  Get two numbers
c
                    do 40 i = 1 , 2
                      iccount = iccount+1
                      fnum = ichar(lineq(iccount:iccount))-48
                      if (abs(fnum-3.5) .gt. 3.6) then
                        call errmsg(lineq,iccount,
     *                      ibarcnt-ibaroff+nbars+1,
     *           'Slur curve parameter must be in range (0,7)!')
                        call stop1()
                      end if
40                  continue
                    iccount = iccount+1
                  end if
                end if
                iccount = iccount-1
                go to 15

              else if (dumq .eq. 's') then
c
c What follows should be one or two signed numbers for adjustment of line break
c slur, end of 1st segment or start of second.
c
                if (fontslur) then
                  call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *    'May not use linebreak slur options with font-based slurs!')
                  call stop1()
                end if
                call g1etchar(lineq,iccount,dumq)
                if (index('+-',dumq) .eq. 0) then
                  call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *              'This character must be "+" or "-"!')
                  call stop1()
                end if
                iccount = iccount+1
                call readnum(lineq,iccount,dumq,fnum)
                if (nint(fnum) .gt. 30) then
                  call errmsg(lineq,iccount-1,ibarcnt-ibaroff+nbars+1,
     *     'Magnitude of slur height adjustment cannot exceed 30!')
                  call stop1()
                end if
                if (index('+-',dumq) .gt. 0) then
                  iccount = iccount+1
                  call readnum(lineq,iccount,dumq,fnum)
                  if (abs(fnum) .gt. 6.3) then
                    call errmsg(lineq,iccount-1,ibarcnt-ibaroff+nbars+1,
     *         'Slur horiz shift must be in range (-6.3,6.3)!')
                    call stop1()
                  end if
                end if
                iccount = iccount-1
                go to 15
              else if (dumq .eq. 'H' .and. iposn.gt.1) then
                if (lineq(iccount+1:iccount+1) .eq. 'H') 
     *                                     iccount=iccount+1
                go to 15
              else if (dumq .eq. 'p') then
c
c  local change in postscript slur/tie adjustment default
c
                if (fontslur) then
                  call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           'Must use postscript slurs ("Ap") to use this option!')
                  call stop1()                   
                end if
                call g1etchar(lineq,iccount,dumq)
                if (index('+-',dumq) .eq. 0) then                
                  call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *              'Expected "+" or "-" here!')
                  call stop1()                   
                end if
                call g1etchar(lineq,iccount,dumq)
                if (index('st',dumq) .eq. 0) then                
                  call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *              'Expected "s" or "t" here!')
                  call stop1()                   
                end if
	          go to 15
              else if (dumq .ne. ' ') then
                ic = ichar(dumq)
                if ((ic.ge.48.and.ic.le.57) .or.
     *                    (ic.ge.65.and.ic.le.90)) then
                  if (iposn .eq. 1) then
                    if (durq.eq.'t' .and. fontslur) then
                      call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *                  'Slur ID not allowed on non-postscript tie!')
                      call stop1()                   
                    end if 
                    if (lineq(iccount+1:iccount+1).eq.'x')
     *                  iccount = iccount+1
                    go to 15
                  end if
                  call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *              'Slur ID must be 2nd character in slur symbol!')
                  call stop1()
                end if
                call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *                   'Illegal character in slur symbol!')
                call stop1()
              end if
              go to 110
            else if (index('0123456789#-nx_',durq) .gt. 0) then
c
c  We have a figure.  Only allow on 1st note of xtup
c
              if (itup .ne. 2) then
                call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           'Figure in xtup only allowed on 1st note!')
                call stop1()
              else if (durq.eq.'x') then
                call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *            'No floating figures in xtuplets!')
                call stop1()
              end if
              if (usefig .and. ivx.eq.1) ifig = 1
26            call g1etchar(lineq,iccount,durq)
c              if (index('0123456789#-n_.:',durq) .gt. 0) then
              if (index('0123456789#-n_.:v',durq) .gt. 0) then
                go to 26
              else if (durq .eq. 's') then
                isligfont = .true.
                go to 26
              else if (durq .eq. '+') then
c
c vertical offset, must be integer then blank
c               
                call g1etchar(lineq,iccount,durq)
                if (index('123456789',durq) .ne. 0) then
                  call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *             'Integer for vertical offset expected here!')
                  call stop1()
                end if
                call readnum(lineq,iccount,durq,fnum)
                if (durq .ne. ' ') then
                  call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *             'Vertical offset must terminate figure!')
                  call stop1()
                end if
                iccount = iccount-1
                go to 26                  
              else if (durq .ne. ' ') then
                call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *            'Illegal character in figure in xtuplet!')
                call stop1()
              end if
              go to 110
            else if (durq .eq. 'G') then
              ngr = 1
79            call g1etchar(lineq,iccount,charq)
              if (index('123456789',charq) .gt. 0) then
                call readnum(lineq,iccount,durq,fnum)
                ngr = nint(fnum)
                iccount = iccount-1
                go to 79
              else if (index('AWulxs',charq) .gt. 0) then
                go to 79
              else if (charq .eq. 'm') then
                call g1etchar(lineq,iccount,charq)
                if (index('01234',charq) .eq. 0) then
                  call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *         'A digit less than 5 must follow "m" in a grace note!')
                  call stop1()
                end if
                go to 79
              else if (charq .eq. 'X') then
c
c Space before main note
c
                call g1etchar(lineq,iccount,charq)
                if (index('0123456789.',charq) .gt. 0) then
                  call readnum(lineq,iccount,durq,fnum)
                  iccount = iccount-1
                  go to 79
                else
                  call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *               'A number must follow "X" in a grace note!')
                  call stop1()
                end if
              end if
c
c  At this point, charq is first note name in rest (grace?)
c
              do 71 igr = 1 , ngr
                numnum = 0
                if (igr .gt. 1) then
75                call g1etchar(lineq,iccount,charq)
                  if (charq .eq. ' ') go to 75
                end if
                if (index('abcdefg',charq) .eq. 0) then
                  call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *               'In grace, expected "a"-"g"!')
                  call stop1()
                end if
78              call g1etchar(lineq,iccount,charq)
                if (charq .ne. ' ') then
                 if (index('1234567',charq) .gt. 0) then
                    if (numnum .eq. 1) then
                      call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *                 'Only one of "+-1234567" allowed here in grace!')
                      call stop1()
                    end if
                    numnum = 1
                    go to 78
                  else if (index('+-nfs',charq) .gt. 0) then
                    go to 78
                  end if
c
c  Digits are possible octave numbers
c
                  call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *               'Illegal character after note name in grace!')
                  call stop1()
                end if
71            continue
              go to 110
            else if (durq .eq. chax(92)) then
              call chklit(lineq,iccount,literr)
              if (literr .gt. 0) then
                call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *               literq(literr))
                call stop1()
              end if
              go to 110
            else if (durq .eq. '"') then
c
c  pmx lyric
c
              call chkpmxlyr(lineq,iccount,lyrerr)
              if (lyrerr .gt. 0) then
                call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *               lyrerq(lyrerr))
                call stop1()
              end if
              go to 110
            else if (durq .eq. 'M') then
c
c  Temporary trap until I get around putting this in pmxb
c
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *          'Macros not yet allowed in xtuplets!')
              call stop1()
            else if (durq .eq. 'X') then
              call g1etx(lineq,iccount,shifton,
     *            ibarcnt-ibaroff+nbars+1,udsp(ibarcnt+nbars+1),wheadpt)
              go to 110
            else if (durq .eq. 'z') then
c
c  Chord note in xtup.  Read past for now.
c
33            call g1etchar(lineq,iccount,durq)
              if (durq .ne. ' ') go to 33
              go to 110
            else if (durq .eq. 'D') then
c
c  Dynamic mark
c
              call checkdyn(lineq,iccount,ibarcnt-ibaroff+nbars+1)
              go to 110
            else if (durq .eq. '%') then
              if (iccount .ne. 1) then
                call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *            'Comment must have "%" in column 1!')
                call stop1()
              end if
              iccount = 128
              go to 110
            else if (durq .eq. '?') then
              call getchar(lineq,iccount,durq)
              if (durq .eq. ' ') then
                iccount = iccount-1
                go to 110
              end if
              if (durq .ne. '-') then
                call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *                'Expecting "-"')
                call stop1()
              end if
              call getchar(lineq,iccount,durq)
              if (index('0123456789.',durq) .eq. 0) then
                call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *                'Expecting number')
                call stop1()
              end if
              call readnum(lineq,iccount,durq,fnum)
              iccount = iccount-1
              go to 110
c
c  140215 Allow clef change inside xtup
c
            else if (durq .eq. 'C') then
              call g1etchar(lineq,iccount,durq)
              if (.not.(index('tsmanrbf',durq).gt.0 .or.
c     *            (ichar(durq).ge.48 .and. ichar(durq).le.55))) then
     *            (ichar(durq).ge.48 .and. ichar(durq).le.56))) then
                call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           'Must have t,s,m,a,n,r,b,f or 0-8 after C!')
                call stop1()
              end if
c              gotclef = .true.
              go to 110
c+++
            else if (durq.eq.']' .and. lineq(iccount+1:iccount+1).eq.'['
     *           .and. lineq(iccount+2:iccount+2).eq.' ') then
              iccount = iccount+2
              go to 110
c+++
c
c Added 200118 to allow dot to be moved on 2nd note of 2-note tremolo
c
            end if
c
c  End of xtup options. At this point symbol can only be note or rest
c
            if (index('abcdefgr',durq) .eq. 0) then
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *         'In xtup, this character is not allowed!')
              call stop1()
            end if
7           call g1etchar(lineq,iccount,durq)
            if (index('12345678ulcb',durq) .gt. 0) then
              go to 7
            else if (index('sfn',durq) .gt. 0) then
c
c  Check for MIDI-only accidental. Cannot coexist with accid. pos'n shift.
c
              if (lineq(iccount+1:iccount+1) .eq. 'i') iccount=iccount+1
              go to 7
            else if (index('+-<>',durq) .gt. 0) then
c
c  May have either octave jump or shifted accid. on main xtup note
c
              if (index('+-',durq).gt.0 .and.
     *          index('01234567890',lineq(iccount+1:iccount+1)).eq.0)
     *          go to 7
              iccount = iccount+1
              call readnum(lineq,iccount,durq,fnum)
              iccount = iccount-1
              go to 7
            else if (index('DF',durq) .gt. 0) then
c
c  Double an xtup note to make an unequal xtup
c
              nacc(ivx,nnl(ivx)) = ibset(nacc(ivx,nnl(ivx)),18)
              ndoub = ndoub+1
              go to 7
            else if (durq .eq. 'd') then
              if (twotrem) then
c
c  2-note trem, get shift
c               
                call g1etchar(lineq,iccount,durq)
                  if (index('+-',durq) .eq. 0) then
                    call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           'Expected +/- for shifted dot on end of 2-note trem!')
                    call stop1()
                  end if
                call g1etchar(lineq,iccount,durq)
                if (index('0123456789.',durq) .eq. 0) then
                  call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *               'Expected number here!')
                  call stop1()
                end if                      
                call readnum(lineq,iccount,dumq,fnum)
                if (index('+-',dumq) .gt. 0) then
c
c  Vertical shift also
c
                  call g1etchar(lineq,iccount,durq)
                  if (index('0123456789-.',durq) .eq. 0) then
                    call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *               'Expected number after 2nd +/- (shift dot)!')
                    call stop1()
                  end if
                  call readnum(lineq,iccount,durq,fnum)
                end if
                iccount = iccount-1
                go to 7
              else            
                nacc(ivx,nnl(ivx)) = ibset(nacc(ivx,nnl(ivx)),27)
              end if
              go to 7
            else if (durq .ne. ' ') then
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *          'Illegal option on xtuplet note!')
              call stop1()
            end if
            if (itup .eq. ntup-ndoub) go to 3
6         continue
3         continue
c
c  6==End of loop for xtuplet input
c
        else if (durq .eq. 'm') then
c
c  Multi-bar rest: next 1 or two digits are # of bars.
c
          if (mod(itsofar(iv),lenbar) .ne. 0) then
            call errmsg(lineq,iccount-1,ibarcnt-ibaroff+nbars+1,
     *             'Multibar rest must start at beginning of bar!')
            call stop1()
          else if (iv.eq.1.and.ibarmbr.gt.0) then
            call errmsg(lineq,iccount-1,ibarcnt-ibaroff+nbars+1,
     *         'Multibar rest only OK at one time per block!')
            call stop1()
          end if
c
c  For some purposes, pretend its one bar only
c
          nodur(iv,nnl(iv)) = lenbar
          ibarmbr = nbars+1
          mbrest = 0
c20        call g1etchar(lineq,iccount,durq)
          call g1etchar(lineq,iccount,durq)
          if (index('123456789',durq) .eq. 0) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *        'Expected an integer after "rm"!')
            call stop1()
          end if
          call readnum(lineq,iccount,durq,fnum)
          mbrest = nint(fnum)
c          iccount = iccount-1
          if (nv .gt. 1) then
            if (iv .eq. 1) then
              mbrestsav = mbrest
            else 
              if (mbrest .ne. mbrestsav) then
                call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           'Must enter same multi-bar rest in every voice!')
                call stop1()
              end if
            end if
c
c  Zero out mbrestsav so can check at end of input block whether 
c    all voices have one
c
            if (iv .eq. nv) mbrestsav=0
          end if
          if (durq .eq. 'n') then
c
c  Get new height
c
            call g1etchar(lineq,iccount,durq)
            if (index('+-123456789',durq) .eq. 0) then
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *          'Expected an integer after "rm[x]n"!')
              call stop1()
            end if
            if (index('+-',durq).ne.0) iccount = iccount+1
            call readnum(lineq,iccount,durq,fnum)
          end if
          if (durq .ne. ' ') then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *             'Illegal character after "rm"!')
            call stop1()
          end if
        else if (durq .eq. '.') then
c
c  Dotted pattern.  Close out note.  Mult time by 3/4.
c  Set time for next note to 1/4.  Start the note.
c
          idotform = 1
        else if (durq .eq. ',') then
          idotform = 3
c
c  Now flow to duration setting, as if durq=' '
c
        else if (index('oL',durq) .gt. 0) then
c
c  Suppress full bar rest, or look left for height
c
          if (charq .ne. 'r') then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           '"o","L" options only legal for rest, not note!')
            call stop1()
          end if
          go to 2
        else if (index('DF',durq) .gt. 0) then
c
c  Double note for xtup.  Must check here in case "D" or "F" came before "x" or on
c  last note of xtup.   Need to flag it in pmxa since affects horiz. spacing.
c
          nacc(ivx,nnl(ivx)) = ibset(nacc(ivx,nnl(ivx)),18)
          go to 2
        else if (durq .eq. 'A') then
c
c  Main note accidental option
c
          call getchar(lineq,iccount,durq)
          if (index('o+-<>',durq) .eq. 0) then 
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           '"o",+","-","<",">" are the only legal options here!')
            call stop1()
          end if
c
c  Need more stuff here
c
          if (durq .ne. "o") then
c
c  Back up 1, flow out, will get +|-|<|> next loop preceded by "A", and will
c    proceed to number input checking
c
            iccount = iccount-1
          end if
          go to 2
        else if (durq .eq. 'T') then
c
c  Single stem tremolo. Only option (optional) is 1,2,3, or 4.
c
          call getchar(lineq,iccount,durq)
          if (index('1234',durq) .eq. 0) iccount = iccount-1
          go to 2
	else if (durq .ne. ' ') then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           'Illegal character!')
          print*,'ASCII code:',ichar(durq)
          call stop1()
        end if
c
c  End of block for note options.
c
c  Set the duration
c
        if (idotform .gt. 0) then
          if (idotform .eq. 1) then
            nodur(ivx,nnl(ivx)) = i1fnodur(nnodur,dotq)*3/2
          else if (idotform .eq. 2) then
            nodur(ivx,nnl(ivx)) = nodur(ivx,nnl(ivx)-1)/3
          else if (idotform .eq. 3) then
            nodur(ivx,nnl(ivx)) = i1fnodur(nnodur,dotq)
          else if (idotform .eq. 4) then
            nodur(ivx,nnl(ivx)) = nodur(ivx,nnl(ivx)-1)/2
          end if
        else if (ibarmbr.ne.nbars+1 .and. .not.fulbrp) then
          nodur(ivx,nnl(ivx)) = i1fnodur(nnodur,dotq)
c
c  Check for double dot
c
          if (iddot .eq. 1) then
            nodur(ivx,nnl(ivx)) = nodur(ivx,nnl(ivx))*7/6
            iddot = 0
          end if
        else if (fulbrp) then
          nodur(ivx,nnl(ivx)) = lenbar
c
c  Use a one-line function to set nnodur.  It gives inverse of ifnodur.
c
          nnodur = index('62514x0x37',
     *                   chax(48+int(log(.1+lenbar)/.69315)))-1
          fulbrp = .false.
        end if
        rest(ivx,nnl(ivx)) = charq.eq.'r'
c
c  If inside forced beam, check if note is beamable
c
        if (fbon) then
          if (nodur(ivx,nnl(ivx)) .lt. 16) go to 120
          if (nnl(ivx) .gt. 1) then
            if (nodur(ivx,nnl(ivx)-1) .eq. 0) go to 120
          end if
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           'Unbeamable thing in forced beam!')
          call stop1()
        end if
120     continue
c
c Get number of prior bars for later check on whether note spans bar line
c
        nbb4 = itsofar(ivx)/lenbar
        itsofar(ivx) = itsofar(ivx)+nodur(ivx,nnl(ivx))
        if (mod(itsofar(ivx),lenbar) .eq. 0) then
          nbars = nbars+1
          if (shifton) barend = .true.
c
c  Will check barend when 1st note of next bar is entered.
c
          if (nbars .gt. 15) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *         'Cannot have more than 15 bars in an input block!')
            call stop1()
          end if
          nib(ivx,nbars) = nnl(ivx)
          if (firstline .and. lenbar.ne.lenbr1) then
c
c  Just finished the pickup bar for this voice.
c
            if (itsofar(ivx) .ne. lenbr0) then
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *               'Pickup bar length disagrees with mtrnum0!')
              call stop1()
            end if
            lenbar = lenbr1
            itsofar(ivx) = 0
          end if
        else if (barend) then
          if (shifton) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *         'Bar ended with user-defined shift still on!')
            call stop1()
          end if
          barend = .false.
        else if (itsofar(ivx)/lenbar .gt. nbb4) then
          call errmsg(lineq,iccount-1,ibarcnt-ibaroff+nbars+1,
     *           'This note spans a bar line!')
          call stop1()
        end if
        if (idotform.eq.1 .or. idotform.eq.3) then
          call g1etchar(lineq,iccount,charq)
          if (index('abcedfgr',charq) .eq. 0) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *        'Expected note name or "r" here!')
            call stop1()
          end if
          idotform = idotform+1
          numnum = 1
          go to 28
        end if
c
c  End of sub block for note-rest
c
      else if (charq .eq. 'z') then
        call g1etchar(lineq,iccount,charq)
        if (index('abcdefg',charq) .eq. 0) then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *      'Expected chord note name here!')
          call stop1()
        end if
25      call g1etchar(lineq,iccount,durq)
c        if (index('dre12345678',durq) .gt. 0) then
        if (index('dre12345678c',durq) .gt. 0) then
          go to 25
        else if (index('fsn',durq) .gt. 0) then
c
c Check for midi-only accid. CANNOT coesist with accidental position tweaks, so
c   MUST come right after "f,s,n"
c
          if (lineq(iccount+1:iccount+1) .eq. 'i') iccount=iccount+1
          go to 25
        else if (durq .eq. 'A') then
          if (index('fsn',lineq(iccount-1:iccount-1)) .eq. 0) then
            call errmsg(lineq,iccount-1,ibarcnt-ibaroff+nbars+1,
     *        'Must have "f,s,n" before "A" in chord note!')
            call stop1()
          end if
          go to 25
        else if (index('<>',durq) .gt. 0) then
          if (index('fsnA',lineq(iccount-1:iccount-1)) .eq. 0) then
c          if (index('fsncA',lineq(iccount-1:iccount-1)) .eq. 0) then ! Causes problems
            call errmsg(lineq,iccount-1,ibarcnt-ibaroff+nbars+1,
     *        'Must have "f,s,n,A" before "<" or ">"!')
            call stop1()
          end if
          call g1etchar(lineq,iccount,durq)
          if (index('1234567890.',durq) .eq. 0) then
            call errmsg(lineq,iccount-1,ibarcnt-ibaroff+nbars+1,
     *        'Expected a number to start here for accidental shift!')
            call stop1()
          end if
          call readnum(lineq,iccount,durq,fnum)
          iccount = iccount-1
          go to 25
        else if (index('+-',durq) .gt. 0) then
          if (index('1234567890.',lineq(iccount+1:iccount+1)) .eq. 0)
     *         go to 25
c
c  Number or '.' (durq) follows +/- .   Get it.
c
          call g1etchar(lineq,iccount,durq)
          if (durq .eq. '.' .and. index('1234567890',
     *                  lineq(iccount+1:iccount+1)) .eq. 0) then
            call errmsg(lineq,iccount-1,ibarcnt-ibaroff+nbars+1,
     *        '"." here must be followed by a digit!')
            call stop1()
          else if (index('sfndA',lineq(iccount-2:iccount-2)).eq.0) then
            call errmsg(lineq,iccount-1,ibarcnt-ibaroff+nbars+1,
     *        'Number after +/- must follow "d,s,f,n,A"!')
            call stop1()
          end if
          call readnum(lineq,iccount,durq,fnum)
          if (index('+-',durq) .eq. 0) then
            iccount = iccount-1
            go to 25
          end if
c
c  2nd +/-
c
          call g1etchar(lineq,iccount,durq)
          if (durq .eq. '.') call g1etchar(lineq,iccount,durq)
          if (index('1234567890',durq) .eq. 0) then
            call errmsg(lineq,iccount-1,ibarcnt-ibaroff+nbars+1,
     *        'Expected a number here!')
            call stop1()
          end if
          call readnum(lineq,iccount,durq,fnum)
          iccount = iccount-1
          go to 25
        else if (durq .ne. ' ') then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *      'Illegal character in chord note!')
          call stop1()
        end if
      else if (charq .eq. 'G') then
        ngr = 1
9       call g1etchar(lineq,iccount,charq)
        if (index('123456789',charq) .gt. 0) then
          call readnum(lineq,iccount,durq,fnum)
          ngr = nint(fnum)
          iccount = iccount-1
          go to 9
        else if (index('AWulxs',charq) .gt. 0) then
          go to 9
        else if (charq .eq. 'm') then
          call g1etchar(lineq,iccount,charq)
          if (index('01234',charq) .eq. 0) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *         'A digit less than 5 must follow "m" in a grace note!')
            call stop1()
          end if
          go to 9
        else if (charq .eq. 'X') then
c
c Space before main note
c
          call g1etchar(lineq,iccount,charq)
          if (index('0123456789.',charq) .gt. 0) then
            call readnum(lineq,iccount,durq,fnum)
            iccount = iccount-1
            go to 9
          else
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *         'A number must follow "X" in a grace note!')
            call stop1()
          end if
        end if
c
c  At this point, charq is first note name in rest (grace?)
c
        do 19 igr = 1 , ngr
          numnum = 0
          if (igr .gt. 1) then
55          call g1etchar(lineq,iccount,charq)
            if (charq .eq. ' ') go to 55
          end if
          if (index('abcdefg',charq) .eq. 0) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *         'In grace, expected "a"-"g"!')
            call stop1()
          end if
18        call g1etchar(lineq,iccount,charq)
          if (charq .ne. ' ') then
            if (index('1234567',charq) .gt. 0) then
              if (numnum .eq. 1) then
                call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *            'Only one of "+-1234567" allowed here in grace!')
                call stop1()
              end if
              numnum = 1
              go to 18
c            else if (index('nfs',charq) .gt. 0) then
            else if (index('+-nfs',charq) .gt. 0) then
              go to 18
            end if
c
c  Digits are possible octave numbers
c
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *         'Illegal character after note name in grace!')
            call stop1()
          end if
19      continue
      else if (charq .eq. chax(92)) then
        call chklit(lineq,iccount,literr)
        if (literr .gt. 0) then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *               literq(literr))
          call stop1()
        end if
      else if (charq .eq. '"') then
c
c  pmx lyric
c
        call chkpmxlyr(lineq,iccount,lyrerr)
        if (lyrerr .gt. 0) then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *         lyrerq(lyrerr))
          call stop1()
        end if
      else if (charq .eq. 'o') then
c
c  Ornament on non-xtup note. "o" symbol must come AFTER the affected note
c
        if (nnl(ivx) .eq. 0) then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *     '"o" must be in same input block, after affected note!')
          call stop1()
        end if
        call g1etchar(lineq,iccount,dumq)
c        if (index('(stmgx+Tupf._)e:>^bc',dumq) .eq. 0 ) then
        if (index('(stmgx+Tupf._)e:>^bcCG',dumq) .eq. 0 ) then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           'Illegal ornament!')
          call stop1()
        end if
        if (dumq .eq. ':') then
          call g1etchar(lineq,iccount,dumq)
          if (dumq .ne. ' ') then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *      'Expected blank after "o:"!')
            call stop1()
          else if (.not.ornrpt) then
            call errmsg(lineq,iccount-1,ibarcnt-ibaroff+nbars+1,
     *            'Turned off repeated ornaments before they were on!')
            call stop1()
          end if
          ornrpt = .false.
        else if (dumq .eq. 'g') then
          if (issegno) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *             'Sorry, only one "segno" per input block!')
            call stop1()
          else if (ivx .ne. 1) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *             'segno can only be in voice 1!')
            call stop1()
          end if
          issegno = .true.
12        call g1etchar(lineq,iccount,dumq)
          if (dumq.eq.'-' .or.
     *        (ichar(dumq).ge.48.and.ichar(dumq).le.58)) go to 12
          if (dumq .ne. ' ') then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *             'Illegal character in segno ornament symbol!')
            call stop1()
          end if
        else if (dumq .eq. 'T') then
c
c  Trill.  may be followed by 't' and/or number.  read 'til blank
c
22        call g1etchar(lineq,iccount,dumq)
          if (dumq .eq. ':') then
            if (lineq(iccount+1:iccount+1) .ne. ' ') then
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *       'Expected blank after ":"!')
              call stop1()
            end if
            go to 32
          else if (dumq .ne. ' ') then
            go to 22
          end if
        else if (dumq .eq. 'f') then
          call g1etchar(lineq,iccount,dumq)
          if (index(' d+-:',dumq) .eq. 0) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *       'Illegal character after "f" in fermata ornament symbol!')
            call stop1()
          end if
          if (dumq .eq. 'd') call g1etchar(lineq,iccount,dumq)
          if (dumq .eq. ':') go to 32
        else if (dumq .eq. 'e') then
          call g1etchar(lineq,iccount,dumq)
          if (index('sfn?',dumq) .eq. 0) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *       'Illegal character after "e" in edit. accid. symbol!')
            call stop1()
          end if
          call g1etchar(lineq,iccount,dumq)
          if (dumq .eq. '?') call g1etchar(lineq,iccount,dumq)
        else
          call g1etchar(lineq,iccount,dumq)
        end if
        if (index('+- :',dumq) .eq. 0) then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *      'Illegal character in ornament symbol!')
          call stop1()
        end if
        if (index('+-',dumq) .gt. 0) then
          if (index('0123456789',lineq(iccount+1:iccount+1)).eq.0) then
            call errmsg(lineq,iccount+1,ibarcnt-ibaroff+nbars+1,
     *       'There should be an integer here!')
            call stop1()
          end if
          call readnum(lineq,iccount,durq,fnum)
          if (durq .eq. ':') then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *        'Cannot shift AND repeat an ornament!')
            call stop1()
          end if
c
c  12/7/03 Allow horizontal shift on any ornament, not just breath and caes.
c
          if (index('+-',durq) .gt. 0) then
            if (index('.0123456789',lineq(iccount+1:iccount+1)) 
     *                                                   .eq. 0) then
              call errmsg(lineq,iccount+1,ibarcnt-ibaroff+nbars+1,
     *           'There should be a number here!')
              call stop1()
            end if
            call readnum(lineq,iccount,durq,fnum)
          end if
        end if
32      continue
        if (dumq .eq. ':') then
          if (lineq(iccount+1:iccount+1) .ne. ' ') then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *      '":" must be followed by blank in "o: "!')
            call stop1()
          else if (ornrpt) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *               'Turned on repeated ornaments but already on!')
            call stop1()
          end if
          ornrpt = .true.
        end if
      else if (index('st(){}',charq) .gt. 0) then
        numint = 0
        iposn = 0
8       call g1etchar(lineq,iccount,dumq)
        iposn = iposn+1
        if (charq.eq.'t' .and. dumq.eq.'t') then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *        'Cannot use "t" as an option on a tie!')
          call stop1()
        end if
        if (index('udltb+-fnhHpsv ',dumq) .eq. 0) then
c
c  Check for explicit ID code.
c
          ic = ichar(dumq)
          if (ic.lt.48 .or. (ic.gt.57.and.ic.lt.65) .or.
     *              ic.gt.90) then
c
c  Not 0-9 or A-Z, so exit
c
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *                   'Illegal character in slur symbol!')
            call stop1()
          else
c
c  It is a possible ID code.  Right place?
c
            if (iposn .ne. 1) then
c
c  Slur ID is not 2nd!
c
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *            'Slur ID must be second character in slur symbol!')
              call stop1()
            else if (charq.eq.'t' .and. fontslur) then 
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *          'Slur ID not allowed on non-postscript tie!')
              call stop1()                   
            else if (lineq(iccount+1:iccount+1).eq.'x') then
              iccount = iccount+1
            end if
          end if
c
c  Slur ID is OK. Note it cannot be "H" at this point..
c
          go to 8
        else if (dumq .eq. 'H') then
          if (iposn .eq. 1) go to 8  
c
c  "H" is NOT an ID code.
c
          if (.not.fontslur .and. charq.eq.'t') then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *          'Cannot reshape postscript ties this way!')
            call stop1()                   
          end if        
	    if (lineq(iccount+1:iccount+1) .eq. 'H') then
            iccount=iccount+1
            iposn = iposn+1
          end if
          go to 8
        else if (index('fh',dumq).gt.0 .and. .not.fontslur 
     *                                  .and. charq.eq.'t') then
c
c  3/9/03 Can't reshape postscript tie.
c
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *      'Cannot reshape postscript ties this way!')
          call stop1()                   
        else if (dumq .eq. 'p') then
c
c  local change in postscript slur/tie adjustment default
c
          if (fontslur) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *        'Must use postscript slurs ("Ap") to use this option!')
            call stop1()                   
          end if
          call g1etchar(lineq,iccount,dumq)
          if (index('+-',dumq) .eq. 0) then                
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *              'Expected "+" or "-" here!')
            call stop1()                   
          end if
          call g1etchar(lineq,iccount,dumq)
          if (index('st',dumq) .eq. 0) then                
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *              'Expected "s" or "t" here!')
            call stop1()                   
          end if
          iposn = iposn+2
          go to 8
        end if
        if (index('udltbfnh',dumq) .gt. 0) then
          go to 8
        else if (index('+-',dumq) .gt. 0) then
          numint = numint+1
          if (fontslur .and. charq.eq.'t') then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *     '"+|-" for slur height only allowed in "s"-slurs!')
            call stop1()
          end if
          iccount = iccount+1
          call readnum(lineq,iccount,durq,fnum)
          if (numint .eq. 1) then
            if (nint(fnum) .gt. 30) then
              call errmsg(lineq,iccount-1,ibarcnt-ibaroff+nbars+1,
     *       'Magnitude of slur height adjustment cannot exceed 30!')
              call stop1()
            end if
          else if (numint .eq. 2) then
            if (abs(fnum) .gt. 6.3) then
              call errmsg(lineq,iccount-1,ibarcnt-ibaroff+nbars+1,
     *       'Slur horiz shift must be in range (-6.3,6.3)!')
              call stop1()
            end if
          else
c
c  Third signed integer, must be a midslur or curve spec.
c
            if (abs(fnum).gt.31) then
              call errmsg(lineq,iccount-1,ibarcnt-ibaroff+nbars+1,
     *                  'Slur midheight must be in the range (-31,31)!')
              call stop1()
            end if
            if (durq .eq. ':') then
c
c  Expecting curve parameters.  Get two numbers
c
              do 41 i = 1 , 2
                iccount = iccount+1
                fnum = ichar(lineq(iccount:iccount))-48
                if (abs(fnum-3.5) .gt. 3.6) then
                  call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *                 'Slur curve parameter must be in range (0,7)!')
                  call stop1()
                end if
41            continue
              iccount = iccount+1
            end if
          end if
          iccount = iccount-1
          go to 8
        else if (dumq .eq. 's') then
c
c What follows should be one or two signed numbers for adjustment of line break
c slur, end of 1st segment or start of second.
c
          if (fontslur) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *    'May not use linebreak slur options with font-based slurs!')
            call stop1()
          end if
          call g1etchar(lineq,iccount,dumq)
          if (index('+-',dumq) .eq. 0) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *        'This character must be "+" or "-"!')
            call stop1()
          end if
          iccount = iccount+1
          call readnum(lineq,iccount,dumq,fnum)
          if (nint(fnum) .gt. 30) then
            call errmsg(lineq,iccount-1,ibarcnt-ibaroff+nbars+1,
     *     'Magnitude of slur height adjustment cannot exceed 30!')
            call stop1()
          end if
          if (index('+-',dumq) .gt. 0) then
            iccount = iccount+1
            call readnum(lineq,iccount,dumq,fnum)
            if (abs(fnum) .gt. 6.3) then
              call errmsg(lineq,iccount-1,ibarcnt-ibaroff+nbars+1,
     *         'Slur horiz shift must be in range (-6.3,6.3)!')
              call stop1()
            end if
          end if
          iccount = iccount-1
          go to 8
	  else if (dumq .eq. 'H' .and. iposn.gt.1) then
          if (lineq(iccount+1:iccount+1) .eq. 'H') iccount=iccount+1
          go to 8
        end if
      else if (charq .eq. '?') then
        call getchar(lineq,iccount,durq)
        if (durq .eq. ' ') then
          iccount = iccount-1
        else
          if (durq .ne. '-') then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           'Expecting "-"!')
            call stop1()
          end if
          call getchar(lineq,iccount,durq)
          if (index('0123456789.',durq) .eq. 0) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *                'Expecting number!')
            call stop1()
          end if
          call readnum(lineq,iccount,durq,fnum)
          iccount = iccount-1
        end if
      else if ((ichar(charq).ge.48.and.ichar(charq).le.57) .or.
     *    index('#-nx_',charq) .gt. 0) then
c
c  We have a figure.  Must come AFTER the note it goes under
c
        if (itsofar(ivx).eq.0 .and.
     *    (.not.firstline.or.lenbr0.eq.0.or.lenbar.eq.lenbr0)) then
c
c  Figure before first note in block
c
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *          'Cannot put figure before first note in block!')
          call stop1()
        end if
        if (charq.eq.'x') then
          indxb = index(lineq(iccount:128),' ')
          if (indxb .lt. 5) then
            call errmsg(lineq,iccount+indxb-1,ibarcnt-ibaroff+nbars+1,
     *           'Cannot have a blank here in floating figure!')
            call stop1()
          end if
        end if
        if (usefig) ifig = 1
5       call g1etchar(lineq,iccount,charq)
        if (index(' 0123456789#-nx_.:+sv',charq) .eq. 0) then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *      'Illegal character in figure!')
          call stop1()
        else if (charq .eq. '+') then
c
c vertical offset, must be integer, then blank
c               
          call g1etchar(lineq,iccount,charq)
          if (index('123456789',charq) .eq. 0) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *             'Integer for vertical offset expected here!')
            call stop1()
          end if
          call readnum(lineq,iccount,charq,fnum)
          if (charq .ne. ' ') then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *             'Vertical offset must terminate figure!')
            call stop1()
          end if
          iccount = iccount-1
          go to 5                  
        else if (charq .eq. 's') then
          isligfont = .true.
        end if
        if (charq .ne. ' ') go to 5
      else if (charq .eq. '[') then
        if (fbon) then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           'Started forced beam while another was open!')
          call stop1()
        end if
        fbon = .true.
17      call g1etchar(lineq,iccount,charq)
        if (index('uljhf:',charq) .gt. 0) then
          go to 17
        else if (index('+-',charq) .gt. 0) then
          iccount = iccount+1
          call readnum(lineq,iccount,durq,fnum)
          iccount = iccount-1
          go to 17
        else if (charq .eq. 'm') then
c
c  Forced multiplicity, next char should be 1-4
c
          call g1etchar(lineq,iccount,charq)
          if (index('1234',charq) .eq. 0) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *        'Forced multiplicity for a beam must be 1, 2, 3, or 4!')
            call stop1()
          end if
          go to 17
        else if (charq .ne. ' ') then
          if (index('0123456789',charq) .gt. 0) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *      'After "[", digits must now be preceeded by "+" or "-"!')
        print*,'You will have to edit older sources to meet this rqmt,'
        print*,'but it was needed to allow 2-digit height adjustments.'
        print*,'Sorry for the inconvenience.  --The Management'
          else
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *             'Illegal character after [!')
          end if
        call stop1()
        end if
      else if (charq .eq. ']') then
        if (.not.fbon) then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           'Forced beam stop with no corresponding start!')
          call stop1()
        end if
        call g1etchar(lineq,iccount,charq)
	  if (charq .eq. '-') then
          if (lineq(iccount+1:iccount+2) .ne. '[ ') then
            call errmsg(lineq,iccount+1,ibarcnt-ibaroff+nbars+1,
     *        'Only sequence allowed here is "[ "!')
            call stop1()
          else 
            iccount = iccount+2
          end if            
        else if (charq .eq. '[') then
          if (lineq(iccount+1:iccount+1) .ne. ' ') then
            call errmsg(lineq,iccount+1,ibarcnt-ibaroff+nbars+1,
     *           'This character must be a blank!')
            call stop1()
          end if
        else
c
c  Forced beam is really ending
c
          fbon = .false.
          if (charq .eq. 'j') then
            if (lineq(iccount+1:iccount+1) .ne. ' ') then
              call errmsg(lineq,iccount+1,ibarcnt-ibaroff+nbars+1,
     *           'This character must be a blank!')
              call stop1()
            end if
          else if (charq .ne. ' ') then           
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *         '"]" must be followed by blank, "j", "-", or "["!')
            call stop1()
          end if
        end if
      else if (charq .eq. 'D') then
c
c  Dynamic mark
c
        if (nnl(ivx) .eq. 0) then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *      '"D" must not come before any notes have been entered!')
          call stop1()
        end if
        call checkdyn(lineq,iccount,ibarcnt-ibaroff+nbars+1)
      else if (index('lhw',charq) .gt. 0) then
c
c  Save position for later check
c
        icclhw = iccount
        call g1etchar(lineq,iccount,durq)
        if (index('0123456789.+- ',durq) .eq. 0) then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *       'Illegal character after "l", "w", or "h"!')
          call stop1()
        end if
        isheadr = isheadr .or. charq .eq. 'h'
        if (index(' +-',durq) .gt. 0) then
c
c  There is a header (or lower string?)
c
          if (index('+-',durq) .gt. 0) then
c
c  User-defined vert offset (\internote).
c
            if (charq .ne. 'h') then
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *               '"+" or "-" not permitted here!')
              call stop1()
            end if
c
c  Have "h" followed by +/- .  Check for digit.
c     Can blow durq since not using fnum for now, but...
c
            call g1etchar(lineq,iccount,durq)
            if (index('123456789',durq) .eq. 0) then
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           'There must be a digit here!')
              call stop1()
            end if
c
c  Have "h" followed by +/- followed by a digit.  No need to get the number.
c
c           call readnum(lineq,iccount,durq,fnum)
          end if
          if (charq .ne. 'w') then
c
c  Header or lower string.
c
            if (icclhw .ne. 1) then
              call errmsg(lineq,iccount-1,ibarcnt-ibaroff+nbars+1,
     *               '"h" or "l" must be first character in line!')
              call stop1()
            end if
c
c  Read past the next line, which has the string.
c
            call read10(charq,lastchar)
            nline = nline+1
            iccount = 128
          else
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *             'Symbol "w" (width) must be followed by a digit!')
            call stop1()
          end if
        else
c
c  Height or width change spec.  Check if at start of piece.
c
          if (ibarcnt .gt. 0) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *             'Symbol must go at top of first input block!')
            call stop1()
          end if
          call readnum(lineq,iccount,durq,dimen)
c
c  Check units.  Convert to points
c
          if (durq .eq. ' ' .or. durq .eq. 'p') then
            dimen = dimen+.5
          else if (durq .eq. 'i') then
            dimen = dimen*72+.5
          else if (durq .eq. 'm') then
            dimen = dimen/25.4*72+.5
          else
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *          'Illegal unit; must be "p","i",or"m"!')
            call stop1()
          end if
c
c 151211 fix. May have extra character. 
c
          if (index('pim',durq) .gt. 0) then
c
c Get another character, see if it's blank
c
            if (lineq(iccount+1:iccount+1) .ne. ' ') then
              call errmsg(lineq,iccount+1,ibarcnt-ibaroff+nbars+1,
     *          'This character should be a blank!')
              call stop1()
            end if       
          end if
          if (charq .eq. 'h') then
            ptheight = int(dimen)
          else
            widthpt = int(dimen)
          end if
        end if
      else if (charq .eq. 'm') then
c
c  Time signature change.  Only allow at beginning of block.
c    mtrnuml, mtrdenl (logical) and p (printable) will be input.
c    mtrnuml=0 initially. (In common)
c
c  Check whether at beginning of a block
c
        if (ivx.ne.1 .or. nnl(1).ne.0) then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *     'Meter change only OK in voice 1, at start of block!')
          print*,'voice number is',ivx
          call stop1()
        end if
        newmeter = .true.
        if (index('o0123456789',lineq(iccount+1:iccount+1)).eq.0) then
          call errmsg(lineq,iccount+1,ibarcnt-ibaroff+nbars+1,
     *       'Illegal character in "m" command for meter change!')
          call stop1()
        end if
        call readmeter(lineq,iccount,mtrnuml,mtrdenl)
        if (mtrnuml .eq. 0) then
          call errmsg(lineq,iccount-1,ibarcnt-ibaroff+nbars+1,
     *     'Digit 0 not allowed here!')
          call stop1()
        end if
        call readmeter(lineq,iccount,mtrnmp,mtrdnp)
c
c  Read past printed time signature; not used in pmxa.
c
        lenbeat = i1fnodur(mtrdenl,'x')
        lenmult = 1
        if (mtrdenl .eq. 2) then
          lenbeat = 16
          lenmult = 2
        end if
        lenbar = lenmult*mtrnuml*lenbeat
        mtrnuml = 0
      else if (charq .eq. 'C') then
        call g1etchar(lineq,iccount,durq)
        if (.not.(index('tsmanrbf',durq).gt.0 .or.
     *      (ichar(durq).ge.48 .and. ichar(durq).le.56))) then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           'Must have t,s,m,a,n,r,b,f or 0-8 after C!')
          call stop1()
        end if
      else if (charq .eq. 'R') then
        if (ivx .ne. 1) then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           'Repeats can only go in voice 1!')
          call stop1()
        end if
c10      call g1etchar(lineq,iccount,durq)
c        if (index('lrdDbz',durq) .gt. 0) go to 10
c        if (durq .ne. ' ') then
c          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
c     *           'Illegal character after "R" (repeat/double bar)!')
c          call stop1()
c        end if
        call g1etchar(lineq,iccount,durq)
        call g1etchar(lineq,iccount,dumq)
        if (index('l Xr Xd XD Xb Xz XlrXdlX',durq//dumq) .eq. 0) then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           'Illegal character after "R*" (repeat/double bar)!')
          call stop1()
        end if
        if (dumq .ne. ' ') then
          call g1etchar(lineq,iccount,durq)
          if (durq .ne. ' ') then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           'Must have blank after "R**" (repeat/double bar)!')
            call stop1()
          end if
        end if
      else if (charq .eq. 'V') then
c
c  Ending
c
        if (iv .ne. 1) then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           'Voltas are only allowed in voice #1!')
          call stop1()
        else if (isvolt) then
          print*
          print*,'*******WARNING********'
          write(15,'(/,a)')'*******WARNING********'
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *      'There is more than one volta in this input block.!')
          print*,'This may work in a score, but WILL NOT work in parts.'
          print*,
     *'Safest to have only 1 volta per block, at the start of the block'
          write(15,'(a)')
     *           'This may work in a score, but WILL NOT work in parts.'
          write(15,'(a)')
     *'Safest to have only 1 volta per block, at the start of the block'
        end if
        isvolt = .true.
        lvoltxt = 0
11      call g1etchar(lineq,iccount,durq)
        if (durq .ne.' ') then
          go to 11
        end if
      else if (charq .eq. 'B') then
        continue
      else if (charq .eq. 'P') then
        if (ivx.ne.1 .or. nnl(1).ne.0) then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           'Only allowed at beginning of block!')
          call stop1()
        end if
16      call g1etchar(lineq,iccount,durq)
        if (durq.eq.'l'.or.durq.eq.'r'.or.(ichar(durq).ge.48 .and.
     *       ichar(durq).le.57)) go to 16
        if (durq .eq. 'c') then
c
c  Expect a centered name, and it has to be last option
c
          call g1etchar(lineq,iccount,durq)
          if (durq .eq. '"') then
c
c  Quoted name, go to next quote mark
c
            do 35 iccount = iccount+1 , 127
              if (lineq(iccount:iccount).eq.'"' .and. 
     *            lineq(iccount-1:iccount-1).ne.'\') go to 36
35          continue
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *        'Missing close quote after page number command (P)!')
            call stop1()
36          continue
          else if (durq .ne. ' ') then
c
c  Space-delimited name, look for next blank
c
            do 37 iccount = iccount+1 , 127
              if (lineq(iccount:iccount) .eq. ' ') go to 38
37          continue
38          continue
          end if
        else if (durq .ne. ' ') then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           'Only "l","r","c" or digit allowed after "P"!')
          call stop1()
        end if
      else if (charq .eq. 'W') then
        call g1etchar(lineq,iccount,durq)
        if (index('.0123456789',durq) .eq. 0) then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           'Expected a number to start here!')
          call stop1()
        end if
        call readnum(lineq,iccount,durq,wminnh(ibarcnt+nbars+1))
      else if (charq .eq. 'T') then
c
c  Titles
c
        call g1etchar(lineq,iccount,durq)
        if (index('itc',durq) .eq. 0) then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           'Must put "i", "t", or "c" after "T"!')
          call stop1()
        end if
        ihead = ihead+2**(index('itc',durq)-1)
c
c  Maybe a number after 'Tt', but ignore here.  Read past string on next line.
c
        call read10(charq,lastchar)
        nline = nline+1
        iccount = 128
      else if (charq .eq. 'A') then
27      call g1etchar(lineq,iccount,durq)
c        if (index('rbsdeK',durq) .gt. 0) then
        if (index('rbsdK',durq) .gt. 0) then
          go to 27
        else if (durq .eq. 'e') then
c
c Check for is4bignv. Must do here to catch first \internote, written in topfile
c   before ever calling getnote. Initialize as .false. in pmxa. Make it true only 
c   if nv>7, AI not set, Ai not set. 
c
          is4bignv = nv.gt.7 .and. .not.AIset
          go to 27
        else if (durq .eq. 'v') then
          if (ibarcnt .eq. 0) novshrinktop = .true.
          go to 27
        else if (durq .eq. 'a') then
          call g1etchar(lineq,iccount,durq)
          if (index('0123456789.',durq) .eq. 0) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           'After "Aa", need decimal number!')
            call stop1()
          end if
          call readnum(lineq,iccount,durq,fbar)
          iccount = iccount-1
          go to 27
        else if (durq .eq. 'i') then
          is4bignv = .false.
          AIset = .true.
          call g1etchar(lineq,iccount,durq)
c
c  Local interstaff correction.  Set to -1. if not specifiec, or after use,
c  or anytime except at top, since pmxb handles all times except at top.
c
          call readnum(lineq,iccount,durq,tintstf)
          if (ibarcnt .eq. 0) fintstf = tintstf
          iccount = iccount-1
          go to 27
        else if (durq .eq. 'I') then
c
c  Global interstaff correction.  Use in place of fintstf if fintstf<0
c
          is4bignv = .false.
          AIset = .true.
          call g1etchar(lineq,iccount,durq)
          call readnum(lineq,iccount,durq,gintstf)
          iccount = iccount-1
          go to 27
        else if (durq .eq. 'o') then
          optimize = .true.
          go to 27
        else if (durq .eq. 'S') then
          do 50 iiv = 1 , noinst
            call g1etchar(lineq,iccount,durq)
            if (index('-0st',durq) .eq. 0) then
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           'After "AS", need nv instances of "s,t,-,0"!')
              call stop1()
            end if
            if (durq.eq.'-'.or.durq.eq.'s') then
              isize(iiv) = 1
            else if (durq.eq.'t') then
              isize(iiv) = 2
            end if
50        continue
          go to 27
        else if (durq .eq. 'p') then
          fontslur = .false.
42        continue
          call g1etchar(lineq,iccount,durq)
          if (index('+-',durq) .gt. 0) then
c
c  Characters to change defaults for ps slurs
c
            call g1etchar(lineq,iccount,durq)
            if (index('shtc',durq) .eq. 0) then
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *          'Only letters allowed here are "s","h","t","c"!')
              call stop1()
            end if
c
c  Now check for another default modifier
c
            go to 42
          else if (index('lh',durq) .gt. 0) then
c
c  Flags for optional linebreak ties or header specials
c
            go to 42
          else
            iccount = iccount-1
          end if
          go to 27
        else if (durq .eq. 'N') then
c
c  Override default name for a part file. Must have part number, then
c    partname in quotes. Must be on line by itself, and start in column 1. 
c    Will only be passed thru to scor2prt.
c
          if (iccount .ne. 2) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *        '"AN" must start in column 1!')
            call stop1()
          end if
          ndxquote = index(lineq,'"')
          if (ndxquote.lt.4 .or. ndxquote.gt.5 .or. 
     *        index('123456789',lineq(3:3)).eq.0 .or.
     *        (ndxquote.eq.5.and.index('012',lineq(4:4)).eq.0)) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *        '"AN" must be followed by inst. #, then quote!')
            call stop1()
          end if
          ndxquote = index(lineq(ndxquote+1:128),'"')
          if (ndxquote .eq. 0) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *        'In "AN", file name must be in quotes!')
            call stop1()
          end if
          iccount = 128
        else if (durq .eq. 'T') then
          cstuplet = .true.
        else if (durq .eq. 'R') then
c
c  Get full name of normal include file; must occupy remainder of line
c
          call getpmxmod(.false.,lineq(iccount+1:128))
          iccount = 128
        else if (durq .eq. 'c') then
          call g1etchar(lineq,iccount,durq)
          if (index('l4',durq) .eq. 0) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *       'Only "l" or "4" is allowed here!')
            call stop1()
          end if
          if (durq .eq. 'l' ) then
            hoffpt = -25
            voffpt = -45
          else if (durq .eq. '4') then
            ptheight = 745
            widthpt = 499
            hoffpt = -24
            voffpt = -24
          end if
          go to 27
        else if (durq .eq. 'V') then
          call g1etchar(lineq,iccount,durq)
          if (index('+-',durq) .eq. 0) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *       'Only "+" or "-" is allowed here!')
            call stop1()
          end if
          call g1etchar(lineq,iccount,durq)
          if (index('0123456789.',durq) .eq. 0) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *       'A number for vert shift before \eject must start here!')
            call stop1()
          end if
          call readnum(lineq,iccount,durq,fnum)
          if (index('+-',durq) .eq. 0) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *       'Only "+" or "-" is allowed here!')
            call stop1()
          end if
          call g1etchar(lineq,iccount,durq)
          if (index('0123456789.',durq) .eq. 0) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *       'A number for vert shift after \eject must start here!')
            call stop1()
          end if
          call readnum(lineq,iccount,durq,fnum)
          iccount = iccount-1
          go to 27
        else if (durq .ne. ' ') then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
c     *       'After "A" must follow one of the letters abcdeiINprRsST!')
     *     'After "A" must follow one of abcdeiILNprRsSTvV!')
          print*,'For AS, since ver. 2.7, must only have noinst args.'
          write(15,'(a)')
     *           'For AS, since ver. 2.7, must only have noinst args.'
          call stop1()
        end if
      else if (charq .eq. 'K') then
c
c  Rules and function of K command
c
c  Only 1 K +/-n +/-m  allowed per block if n.ne.0 (transposition).  isig1 is
c  initial sig, and must be passed to pmxb because it is needed when topfile
c  is called, which is before the K+n+m command is read in pmxb.  Also, we
c  compute and save ibrkch and newkey for each syst, accounting for key changes,
c  then adjust fbar to make poenom much more accurate.
c  Jan 02: Now K-0+[n] is used to transpose e.g. from f to f#.
c
77      continue
        call g1etchar(lineq,iccount,durq)
c        if (index('+-i',durq) .eq. 0) then
        if (index('+-in',durq) .eq. 0) then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *   '"K" (transpose or key change) must be followed by "+,-,i,n"!')
          call stop1()
        end if
        if (durq .eq. 'n') go to 77
        if (durq .ne. 'i') then
c
c Normal key change and/or transposition)
c
c          iccount = iccount+1
          num1 = 44-ichar(durq)
c
c  num1= +1 or -1
c
          ztrans = num1.eq.-1 
          call g1etchar(lineq,iccount,durq)
          if (index('0123456789',durq) .eq. 0) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *      '1st +/- must be followed by a number!')
            call stop1()
          end if
c          iccount = iccount+1
          call readnum(lineq,iccount,durq,fnum)
          num1 = nint(fnum)*num1
          ztrans = ztrans .and. num1.eq.0
          if (index('+-',durq) .eq. 0) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *      '1st number after "K" must be followed by "+,-"!')
            call stop1()
          end if
          iccount = iccount+1
          num2 = 44-ichar(durq)
          call readnum(lineq,iccount,durq,fnum)
          num2 = num2*int(fnum+.1)
          if (num1.eq.0 .and. .not.ztrans) then
c
c  Key change, only one per block allowed
c
            if (iskchb) then
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           'Only one key change allowed per input block!')
              call stop1()
            end if
            iskchb = .true.
            nkeys = nkeys+1
            kchmid(nkeys) = mod(itsofar(ivx),lenbar).ne.0
c
c  Make ibrkch = barnum-1 if at start of bar, so fsyst advances ok at linebreak.
c
            ibrkch(nkeys) = ibarcnt+nbars
            if (kchmid(nkeys)) ibrkch(nkeys) = ibrkch(nkeys)+1
            newkey(nkeys) = num2+idsig
c 130316
c            do 43 iinst = 1 , noinst
              midisig = newkey(nkeys)
c43          continue
          else
c
c  Transposition
c
            fulltrans = .true.
            if (ibarcnt .gt. 0) then
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *         'Transposition must be at top of first input block!')
              call stop1()
            end if
            isig1 = num2
            idsig = isig1-newkey(1)
c
c  idsig is the difference between sig after transposition, and sig in setup.
c  It may alter # of accid's in key changes if there is transposition.
c
          end if
        else   ! durq='i'
c
c  110522/110529
c  Instrument-wise transposition Ki[iInstTrans][+/-][iTransAmt][+/-][iTransKey]
c    and repeat i[...] for multiple instruments. Store info here if ibarcnt=0
c    so can pass to topfile (via comInstTrans), which is called before getnote. 
c    Otherwise, will store info from getnote. Initialize EarlyTransOn and
c    LaterInstTrans to .false. in blockdata. Set EarlyTransOn from here; 
c    LaterInstTrans from g1etnote. Zero both out after use. nInstTrans really
c    only needed for instrument-signatures, not transpositions. iTransAmt is
c    ALWAYS active per instrument. Set up instno(iv) so can fetch iTransAmt for
c    each staff.
c
          call GetiTransInfo(.true.,ibarcnt,lineq,iccount,
     *                         ibaroff,nbars,noinst)
        end if
      else if (charq .eq. '|') then
c
c  Optional bar symbol
c
        if (mod(itsofar(ivx),lenbar).ne.0) then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *       'Bar line marker out of place!')
          call stop1()
        else if (shifton) then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *      'Bar ended with user-defined shift still on!')
          call stop1()
        end if
      else if (charq .eq. '/') then
        if (ornrpt) then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *        'WARNING: Block ended with repeated ornament still on!')
          ornrpt = .false.
        end if
        if (stickyS) then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *        'WARNING: Block ended with sticky stemshrink still on!')
          stickyS = .false.
        end if
        if (fbon) then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *      'Block ended with forced beam open!')
          call stop1()
        else if (shifton) then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *       'Bar ended with user-defined shift still on!')
          call stop1()
c
c 140215 Temporary to allow clef change in stup
c
c        else if (gotclef) then
c          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
c     *      'May not enter clef at end of input block!')
c          call stop1()
        end if
        barend = .false.
c
c  Perform time checks
c
        if (mod(itsofar(ivx),lenbar).ne.0) then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           'Block duration not divisible by lenbar!')
          print*,'lenbar, timesofar are ',lenbar,itsofar(ivx)
          call stop1()
        else if (ivx.gt.1 .and. itsofar(ivx).ne.itsofar(1)) then
          print*
          print*,'No of bars in voice 1, current voice:',
     *        itsofar(1)/lenbar,itsofar(ivx)/lenbar
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           'Block duration not equal to voice 1!')
          call stop1()
        end if
        call g1etchar(lineq,iccount,durq)
        if (durq .eq. ' ' .and. iv.eq.nv) then
c
c  End of input block
c
          loop = .false.
        else
c
c  Start a new voice
c
          if (lenbr0.ne.0 .and. firstline) lenbar = lenbr0
          nbars = 0
          if (durq .eq. ' ') then
c
c  New voice is on next staff
c
            iv = iv+1
            ivx = iv
          else
c
c  New voice is on same staff.  Set up for it
c
            ivx = nv+1
            do 23 iiv = 1 , nv
              if (nvmx(iiv) .eq. 2) ivx = ivx+1
23          continue
            if (ivx .gt. nm) then
              write(*,'(1x,a21,i3,a23)')'Cannot have more than',nm,
     *          ' lines of music at once'
              call stop1()
            end if
            nvmx(iv) = 2
            ivmx(iv,2) = ivx
            itsofar(ivx) = 0
            nnl(ivx) = 0
            do 24 j = 1 , 200
              rest(ivx,j) = .false.
              nacc(ivx,j) = 0
24          continue
c
c  For midi stuff, record that there is a 2nd line of music in this voice
c
            if (ismidi) twoline(iv) = .true.
          end if
        end if
        iccount = 128
      else if (charq .eq. 'S') then
c
c  New nsyst: for use with partmaker scor2prt, for parts w/ diff # of systs.
c
        if (ibarcnt .gt. 0) then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           '"S" can only be in first input block!')
          call stop1()
        end if
        call g1etchar(lineq,iccount,durq)
        if (index('123456789 ',durq) .eq. 0) then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           'A digit must follow "S"!')
          call stop1()
        end if
        call readnum(lineq,iccount,durq,fnsyst)
        nsyst = nint(fnsyst)
14      continue
        if (durq .eq. 'P') then
c
c  New npages for parts.  
c
          call g1etchar(lineq,iccount,durq)
          if (index('123456789 ',durq) .eq. 0) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *             'Must have a number here!')
            call stop1()
          end if
          call readnum(lineq,iccount,durq,fnsyst)
          npages = nint(fnsyst)
          go to 14
        else if (durq .eq. 'm') then
c
c  Reset musize (musicsize).
c
          call g1etchar(lineq,iccount,durq)
          if (index('123456789 ',durq) .eq. 0) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *             'Must have a number here!')
            call stop1()
          end if
          call readnum(lineq,iccount,durq,fnsyst)
          musize = nint(fnsyst)
          wheadpt = whead20*musize
          go to 14
        else if (durq .ne. ' ') then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           'Illegal character in "S" symbol!')
          call stop1()
        end if
      else if (charq .eq. 'L') then
c
c  Force line break
c
        call g1etchar(lineq,iccount,durq)
        if (durq .eq. 'C') then
c
c  Coda, no real line break, just get coda length
c
          if (ishort .ne. 1) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *          'Cannot use "LC" without first using "L[n]S"!')
            call stop1()
          end if
          ishort = 0
          call g1etchar(lineq,iccount,durq)
          if (index('1234567890.',durq) .eq. 0) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *          'Need number to define coda length!')
            call stop1()
          end if
          call readnum(lineq,iccount,durq,codafrac)
          poefa(isysflb(nflb)) = poefa(isysflb(nflb))+codafrac
          if (index(' n',durq) .eq. 0) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *          'Need blank or "n" here!')
            call stop1()
          end if
          return
        end if 
        nflb = nflb+1
        ibarflb(nflb) = ibarcnt+nbars+1
        if (index('123456789',durq) .eq. 0) then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *          'Need integer to define forced line break!')
          call stop1()
        end if
        call readnum(lineq,iccount,durq,sysflb)
        isysflb(nflb) = nint(sysflb)
        if (isysflb(nflb) .eq. 1) then
          call errmsg(lineq,iccount-1,ibarcnt-ibaroff+nbars+1,
     *        'For now, "L1" not allowed!')
          call stop1()
        end if
        if (nflb .gt. 1) then
c
c  Check if new number is > prior one
c
          if (isysflb(nflb) .le. isysflb(nflb-1)) then
            call errmsg(lineq,iccount-1,ibarcnt-ibaroff+nbars+1,
     *       'You already forced a line break at a later line!')
            call stop1()
          end if
        end if
        if (npages .eq. 0) then
          print*
          print*,'WARNING! You forced a line break at line ',
     *      isysflb(nflb),' but npage = 0.  Continue?'
          read(*,'(a)') charq
          if (index('yY',charq) .eq. 0) call stop1()
        else if (isysflb(nflb) .gt. nsyst) then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           'Forced line break at line num > nsyst!')
          call stop1()
        else if (index(' PMS',durq) .eq. 0) then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *      'Must have " ", "P", "M", or "S" here!')
          call stop1()
        end if
49      continue ! Transfer up from below to allow S after M
        if (durq .eq. 'S') then
c
c  Shortened line, get shortening fraction
c
          ishort = 1
          call g1etchar(lineq,iccount,durq)
          if (index('1234567890.',durq) .eq. 0) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *          'Need number to define line shortening fraction!')
            call stop1()
          end if
          call readnum(lineq,iccount,durq,shortfrac)
          poefa(isysflb(nflb)) = shortfrac
          if (durq .ne. ' ') then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *          'Need blank here!')
          end if
        end if
        if (durq .eq. 'P') then
c
c  Forced page break here, get page number.
c
          call g1etchar(lineq,iccount,durq)
          if (index('123456789',durq) .eq. 0) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *          'Need integer to define forced page break!')
            call stop1()
          end if
          call readnum(lineq,iccount,durq,fnum)
          nfpb = nfpb+1
          ipagfpb(nfpb) = nint(fnum)
          isysfpb(nfpb) = isysflb(nflb)
          if (ipagfpb(nfpb) .gt. npages) then
            call errmsg(lineq,iccount-1,ibarcnt-ibaroff+nbars+1,
     *           'Forced page break at page num > npages!')
            call stop1()
          else if (nfpb .gt. 1) then
            if (ipagfpb(nfpb) .le. (ipagfpb(nfpb-1))) then
              call errmsg(lineq,iccount-1,ibarcnt-ibaroff+nbars+1,
     *           'Forced page break numbers must increase!')
              call stop1()
            end if
          end if
        end if
        if (index(' M',durq) .eq. 0) then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *        'Illegal character in linebreak symbol!')
          call stop1()
        else if (durq .eq. 'M') then
          nmovbrk = nmovbrk+1
          isysmb(nmovbrk) = isysflb(nflb)
          call g1etchar(lineq,iccount,durq)
31        if (durq .eq. '+') then
c
c  Vertical spacing, read past number.
c
            call g1etchar(lineq,iccount,durq)
            if (index('123456789',durq) .eq. 0) then
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *          'Integer required here!')
              call stop1()
            end if
            call readnum(lineq,iccount,durq,fnum)
            go to 31
          else if (durq .eq. 'i') then
c
c  Change indentation,
c
            call g1etchar(lineq,iccount,durq)
            if (index('.123456789',durq) .eq. 0) then
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *          'Decimal number required here!')
              call stop1()
            end if
c
c  fracsys was initialized in block data to all 0.'s
c
            call readnum(lineq,iccount,durq,fracsys(nmovbrk))
            go to 31
          else if (durq .eq. 'c') then
            call g1etchar(lineq,iccount,durq)
            go to 31
          else if (durq .eq. 'r') then
            call g1etchar(lineq,iccount,durq)
            if (index('+-',durq) .eq. 0) then
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *       'Must have "+" or "-" after "r" as movement break option!')
              call stop1()
            end if
            call g1etchar(lineq,iccount,durq)
            go to 31
          else if (durq .eq. 'n') then
c
c  Change # of voices.  Input ninow, iorig(1...ninow).  Will use names,
c  staves per inst. and clefs  corr. to iorig in original list of instruments.
c
            nv = 0
            call g1etchar(lineq,iccount,durq)
            if (durq .eq. ':') then
c
c  Signals a 2-digit number, get next two characters
c
              call g1etchar(lineq,iccount,durq)
              call g1etchar(lineq,iccount,dumq)
              if (index('12',durq).eq.0
     *                   .or.index('0123456789',dumq).eq.0) then
                call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *            'Illegal new number of instruments '//durq//dumq//
     *               ' at movement break!')
                call stop1()
              end if
              read(lineq(iccount-1:iccount),'(i2)')ninow
            else
c
c  durq is a single digit number for noinow
c
              if (index('123456789',durq) .eq. 0) then
                call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *            'Illegal new number of instruments '//durq//
     *                                  ' at movement break!')
                call stop1()
              end if
              ninow = ichar(durq)-48
            end if
            if (ninow.gt.noinst) then
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *            'New number of instruments must be <= original!')
              call stop1()
            end if
            do 63 iinow = 1 , ninow
              call g1etchar(lineq,iccount,durq)
              if (durq .eq. ':') then
c
c  Signals a 2-digit number
c
                call g1etchar(lineq,iccount,durq)
                call g1etchar(lineq,iccount,dumq)
                if (index('12',durq).eq.0
     *                   .or.index('0123456789',dumq).eq.0) then
                  call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *              'Illegal 2-digit instrument number '//durq//dumq//
     *               ' at movement break!')
                  call stop1()
                end if
                read(lineq(iccount-1:iccount),'(i2)')iorig
              else
c
c  durq is a single digit number for iorig
c
                if (index('123456789',durq) .eq. 0) then
                  call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *              'Illegal instrument number '//durq//
     *                                  ' at movement break!')
                  call stop1()
                end if
                iorig = ichar(durq)-48
              end if
              if (iorig .gt. noinst) then
                call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *            'New instrument number must be <= original noinst!')
                call stop1()
              end if
              nv = nv+nsperi(iorig)
63          continue
            do 61 iiv = 1 , nv
c
c  Get clef names
c
              call g1etchar(lineq,iccount,durq)
              if (.not.(index('tsmanrbf',durq).gt.0 .or.
c     *             (ichar(durq).ge.48 .and. ichar(durq).le.55))) then
     *             (ichar(durq).ge.48 .and. ichar(durq).le.56))) then
                call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *          'Must have t,s,m,a,n,r,b,f or 1-7 as clef symbol here!')
                call stop1()
              end if
c
c  Initialize new voices
c
              nvmx(iiv) = 1
              ivmx(iiv,1) = iiv
              itsofar(iiv) = 0
              nnl(iiv) = 0
              do 62 j = 1 , 200
                rest(iiv,j) = .false.
62            continue
61          continue
c
c  Loop back up, this might not be last option in M
c
            call g1etchar(lineq,iccount,durq)
            go to 31
          else if (durq .eq. 'S') then
            go to 49
          else if (durq .ne. ' ') then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *        'Illegal character after Movement break symbol!')
            call stop1()
          end if
          if (fracsys(nmovbrk) .lt. 0.001) then
c
c  Since fracsys was not explicitly set, set it to prior value.
c
            if (nmovbrk .eq. 1) then
              fracsys(nmovbrk) = fracindent
            else
              fracsys(nmovbrk) = fracsys(nmovbrk-1)
            end if
          end if
        end if
c
c  Just before exiting if-block for forced line breaks, set counter to use when 
c  dealing with vertical space calcs
c
        nistaff(nflb) = nv-1
      else if (charq .eq. 'F') then
        usefig = .false.
      else if (charq .eq. 'X') then
        call g1etx(lineq,iccount,shifton,ibarcnt-ibaroff+nbars+1,
     *             udsp(ibarcnt+nbars+1),wheadpt)
      else if (charq .eq. 'I') then
c
c  MIDI settings.  
c
        if (ivx.ne.1 .or. nnl(1).ne.0) then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *     'MIDI stuff only allowed at start of block!')
          call stop1()
        end if
        if (nv .gt. 15) then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *     'Sorry but MIDI does not work with more than 15 voices!')
          call stop1()
        end if
        ismidi = .true.
        call getmidi(noinst,lineq,iccount,ibarcnt,ibaroff,nbars,lenbar,
     *               mtrdenl,nv,.true.)
      else if (charq .eq. 'M') then
        call setmac(lineq,iccount,ibarcnt,ibaroff,nbars,charq,durq,ivx,
     *                 nline)
      else if (index(',.',charq) .gt. 0) then
c
c  Continued rhythmic shortcut
c
        idotform = index('. ,',charq)
        if (idotform .eq. 1) then
c
c  Change duration of prior note 
c
          itsofar(ivx) = itsofar(ivx)-nodur(ivx,nnl(ivx))
          nodur(ivx,nnl(ivx)) = nodur(ivx,nnl(ivx))*3/2 
          itsofar(ivx) = itsofar(ivx)+nodur(ivx,nnl(ivx))
        end if
        idotform = idotform+1
        numnum = 1
        cdot = .true.
        go to 1
      else
        print*,'ASCII code:',ichar(charq)
        call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *         'This character is not allowed here!')
        print*,'ASCII code:',ichar(charq)
        call stop1()
      end if
      return
      end
      subroutine g1etset(nv,noinst,mtrnuml,mtrdenl,mtrnmp,mtrdnp,
     *    xmtrnum0,newkey,npages,nsyst,musize,bottreb)
      parameter(nm=24,maxblks=9600)
      character*131072 bufq
      character*1 chax
      integer*2 lbuf(maxblks)
      common /inbuff/ ipbuf,ilbuf,nlbuf,lbuf,bufq
      character*128 lineq
      logical lastchar,issegno,isheadr,isvolt,fbon,bottreb,
     *        newway
      common /c1omget/ lastchar,fbon,issegno,ihead,isheadr,nline,isvolt,
     *     fracindent,nsperi(nm),linesinpmxmod,line1pmxmod,lenbuf0
      common /commidisig/ midisig
c
c  Get the first line
c
      iccount = 0
      nline = 1
9     call getbuf(lineq)
      if (lineq(1:1) .eq. '%') then
        nline = nline+1
        go to 9
      end if
      if (lineq(1:3) .eq. '---') then
c
c  Have TeX input until next line that starts with '---'
c
3       nline = nline+1
        call getbuf(lineq)
        if (ilbuf .gt. nlbuf) go to 1
        go to 2
1       print*,'You did not terminate type 0 TeX input with "---"'
        call stop1()
2       continue
        if (lineq(1:3) .ne. '---') go to 3
c
c  Force a new line read on first call to readin
c
        iccount = 128
      end if
c
c  Here, lineq and nline are first non-TeX lines.
c
      nv = nint(readin(lineq,iccount,nline))
      if (nv .gt. nm) then
        write(*,'(1x,a46,i3)')
     *     'In setup data, number of voices cannot exceed',nm
        call stop1()
      end if
      noinst = nint(readin(lineq,iccount,nline))
      if (noinst .gt. nv) then
        write(*,'(a)')
     *    'In setup data, cannot have more instruments than staves'
        call stop1()
      end if
      newway = noinst.le.0
      if (newway) noinst = -noinst
      do 10 iinst = 1 , noinst
c
c  Seve # of staves per inst in case later drop some inst's.
c
        if (newway) then
          nsperi(iinst) = nint(readin(lineq,iccount,nline))
        else if (iinst .gt. 1 ) then
          nsperi(iinst) = 1
        else
          nsperi(iinst) = nv-noinst+1
        end if
10    continue
      mtrnuml = nint(readin(lineq,iccount,nline))
      mtrdenl = nint(readin(lineq,iccount,nline))
cc
cc  Kluge!!!
cc
c      if (mtrdenl .eq. 1) then
c        mtrdenl = 2
c        mtrnuml = mtrnuml*2
c      end if
      mtrnmp = nint(readin(lineq,iccount,nline))
      mtrdnp = nint(readin(lineq,iccount,nline))
      if (mtrnmp.eq.0 .and. mtrdnp .ge. 8) then
        write(*,'(a)')'In setup data, with mtrnmp=0, mtrdnp must be <8'
        call stop1()
      end if
      xmtrnum0 = readin(lineq,iccount,nline)
      newkey = nint(readin(lineq,iccount,nline))
c 130316
c      do 11 iinst = 1 , noinst
        midisig = newkey 
c11    continue
      npages = nint(readin(lineq,iccount,nline))
      nsyst = nint(readin(lineq,iccount,nline))
      musize = nint(readin(lineq,iccount,nline))
      fracindent = readin(lineq,iccount,nline)
      if (fracindent .ge. 1.) then
        write(*,'(a)')'In setup data, fracindent must be <1'
        call stop1()
      end if
      if (npages .gt. nsyst) then
        print*,'Error in input file: npages > nsyst'
        call stop1()
      else if((musize-16)*(musize-20)*(musize-24)*(musize-29).ne.0) then
        call printl('Musicsize must be 16, 20, 24, or 29')
        call stop1() 
      end if
c
c  Next noinst non-comment lines are names of instruments.
c
      do 4 i = 1 , abs(noinst)
5       call getbuf(lineq)
        nline = nline+1
        if (lineq(1:1) .eq. '%') go to 5
4     continue
c
c  Mext non-comment line has nv clef names
c
6     call getbuf(lineq)
      nline = nline+1
      if (lineq(1:1) .eq. '%') go to 6
      do 7 iv = 1 , nv
c        if (index('brnamstf01234567',lineq(iv:iv)) .eq. 0) then
        if (index('brnamstf012345678',lineq(iv:iv)) .eq. 0) then
          call errmsg(lineq,iv,0,
     *       'There should be a clef symbol here!')
          call stop1()
        end if
7     continue
      if (lineq(nv+1:nv+1) .ne. ' ') then
        call errmsg(lineq,nv+1,0,
     *    'There should be a blank here!')
        call stop1()
      end if
c
c Set flag if voice 1 is treble, since it affects vertical spacing
c
c      bottreb = lineq(1:1).eq.'t'
      bottreb = index('t08',lineq(1:1)).gt.0
c
c  Next non-comment line has path name
c
8     call getbuf(lineq)
      nline = nline+1
      if (lineq(1:1) .eq. '%') go to 8
      lpath = index(lineq,' ')-1
      if (index('/:'//chax(92),lineq(lpath:lpath)) .eq. 0) then
        call errmsg(lineq,lpath,0,
     *       'Last character of pathname is not "/",":", or "'
     *                                        //chax(92)//'"!')
        call stop1()
      end if
      return
      end
      subroutine g1etx(lineq,iccount,shifton,ibar,udsp,wheadpt)
c
c  Parse "X" commands.  Ignore all "B"; "P" means to ignore whole symbol.
c  In scor2prt, must strip out "P", copy only "B" and "P"-type "X"-symbols.
c
      logical shifton,number,btest
      character*128 lineq
      character*1 charq,dumq
      number = .false.
      nPBSc = 0
1     call g1etchar(lineq,iccount,charq)
      if (index('PBS:',charq) .gt. 0) then
c
c  Continue checking here even if "P".
c
        ipbsc = index('PBS:',charq)
        if (btest(nPBSc,ipbsc)) then
          call errmsg(lineq,iccount,ibar,'Only one allowed per symbol!')
          call stop1()
        end if
        nPBSc = ibset(nPBSc,ipbsc)
        go to 1
      else if (index('+-.0123456789',charq) .gt. 0) then
        number = .true.
        if (index('+-',charq) .gt. 0) then
          call g1etchar(lineq,iccount,dumq)
          if (index('.0123456789',dumq) .eq. 0) then
            call errmsg(lineq,iccount,ibar,'Expected a number here!')
            call stop1()
          end if
        end if
        call readnum(lineq,iccount,dumq,fnum)
        if (charq.eq.'-') fnum = -fnum
        if (dumq .ne. 'p') then
          iccount = iccount-1
          fnum = fnum*wheadpt
        end if
        go to 1
      else if (charq .ne. ' ') then
        call errmsg(lineq,iccount,ibar,'Not allowed in "X" symbol!')
        call stop1()
      end if
c
c  Done with parsing.  Other checks
c
      if (iand(6,nPBSc).eq.6 .or. iand(24,nPBSc).eq.24) then
        call errmsg(lineq,iccount-1,ibar,
     *     'Cannot have both "P" and "B" or "S" and ":"!')
        call stop1()
      end if
      if (btest(nPBSc,4)) then
        if (number) then
          if (shifton) then
            call errmsg(lineq,iccount-1,ibar,
     *            'Started a group shift without stopping prior one!')
            call stop1()
          else
            shifton = .true.
          end if
        else
          if (.not. shifton) then
            call errmsg(lineq,iccount-1,ibar,
     *            'Ended a group shift without starting one!')
            call stop1()
          else
            shifton = .false.
          end if
        end if
      end if
c
c  P off, S off, c off => normal user-defined space.  Add to udsp (later fsyst)
c
      if (iand(nPBSc,26).eq.0) udsp = udsp+fnum
      if (.not.number .and. .not.btest(nPBSc,4)) then
        call errmsg(lineq,iccount-1,ibar,
     *      'Must have either a number or a colon in "X" symbol!')
        call stop1()
      end if
      return
      end
c      integer*4 function mytime()
c      CHARACTER(10) tq
c      CALL DATE_AND_TIME(TIME=tq)
c      read(tq,'(2i2,f6.3)')ih,im,ts
c      mytime = 1000*(ts+60*(im+60*ih))
c      return
c      end  
      subroutine getbuf(lineq)
      parameter (maxblks=9600)
      character*(*) lineq
      character*131072 bufq
      integer*2 lbuf(maxblks)
      common /inbuff/ ipbuf,ilbuf,nlbuf,lbuf,bufq
      lineq = bufq(ipbuf+1:ipbuf+lbuf(ilbuf))
      ipbuf = ipbuf+lbuf(ilbuf)
      ilbuf = ilbuf+1
      return
      end
      subroutine getchar(lineq,iccount,charq)
      parameter (nm=24)
c
c  Gets the next character out of lineq*128.  If pointer iccount=128 on entry,
c  then reads in a new line.  Resets iccount.  Ends program if no more input.
c
      common /comget/ lastchar,rptnd1,sluron(nm,2),fbon,ornrpt,stickyS,
     *       movbrk,movnmp,movdnp,movgap,parmov,fintstf,gintstf,
     *       rptprev,equalize,rptfq1,rptfq2
      logical lastchar,rptnd1,sluron,fbon,rptprev,ornrpt
      common /commac/ macnum,mrecord,mplay,macuse,icchold,lnholdq,endmac
      logical mrecord,mplay,endmac,equalize,stickyS
      character*1 charq,rptfq1,rptfq2
      character*128 lineq,lnholdq
      if (iccount .eq. 128) then
        call read10(lineq,lastchar)
        if (lastchar) return
        if (.not. endmac) then
          iccount = 0
        else
          endmac = .false.
          iccount = icchold
          lineq = lnholdq
        end if
        if (mrecord) then
          call mrec1(lineq,iccount,ndxm)
        end if
      end if
      iccount = iccount+1
      charq = lineq(iccount:iccount)
      return
      end
      subroutine getdyn(ivx,ip,irest,iornq,lineq,iccount)
      parameter (nm=24)
      common /comdyn/ ndyn,idyndat(99),levdsav(nm),ivowg(12),hoh1(12),
     *        hoh2(12),hoh2h1(2),ntxtdyn,ivxiptxt(41),txtdynq(41),
     *        idynda2(99),levhssav(nm),listcresc,listdecresc
      character*128 txtdynq
      common /comslur/ listslur,upslur(nm,2),ndxslur,fontslur
     *                 ,WrotePsslurDefaults,SlurCurve
      logical fontslur,upslur,WrotePsslurDefaults
      character*1 durq,chax
      character*4 dynsymq
      character*128 lineq
c
c  Get info for dynamic mark.  Enter after getting "D", iccount sits on "D"
c  Bits in idyndat are as follows
c    00-03 ivx
c    04-11 ip
c    12-15 code for type of mark
c      0 => arbitrary text
c      1-12 => pppp,ppp,pp,p,mp,mf,f,fp,sfz,ff,fff,ffff
c      If (.not. fontslur)
c        13 => hairpin start, 14,15 => <,> (ending)
c      else 
c         13 < start, 14 > start, 15 ending
c      end if
c    16    flag for vertical offset
c    17-23 vertical offset + 64 , \internote
c    31    Hairpin start (0), stop (1)
c
c  idynda2
c
c    00    flag for horizontal offset
c    01-09 (horizontal offset)/10 + 25.6 notehead widths
c    10  5th bit for ivx (5/15/10)
c
      irest = ibset(irest,26)
      ndyn = ndyn+1
      idyn = ivx
      idynda2(ndyn) = 0
      if (ivx .ge. 16) call setbits(idynda2(ndyn),1,10,1)
      call setbits(idyn,8,4,ip)
      if (lineq(iccount+1:iccount+1) .eq. '"') then
c
c  text-dynamic
c
        ntxtdyn = ntxtdyn+1
        iccountt = iccount
3       continue
        iend = iccountt+index(lineq(iccountt+2:128),'"')+2
        if (lineq(iend-2:iend-2) .eq. '\') then
          iccountt = iend-2
          go to 3
        end if
        txtdynq(ntxtdyn) = lineq(iccount+2:iend-2)
c
c  Store ivx, ip in bits 0-12
c
        ivxiptxt(ntxtdyn) = ivx+32*ip
        ipm = index('- +',lineq(iend:iend))
        idno = 0
      else
c
c  Word-group or hairpin
c
        do 1 iend = iccount+2 , 128
          ipm = index('- +',lineq(iend:iend))
c
c  Exit the loop at first blank, "+", or "-" 
c
          if (ipm .gt. 0) go to 2
1       continue
2       continue
        read(lineq(iccount+1:iend-1),'(a'//chax(47+iend-iccount)//')')
     *     dynsymq
        idno = (index(
     *   'ppppppp pp  p   mp  mf  f   fp  sfz ff  fff ffff    <   >   ',
     *           dynsymq)+3)/4
c
c  Save for later down
c
        idno1 = idno
      end if
c
c  Set flag to check level later if in beam
c
      iornq = ibset(iornq,23) 
      if (idno.ge.14) then
c
c  Hairpin here.  Check if opposite type from one that's already on 
c
        if (idno.eq.14.and.btest(listdecresc,ivx) .or. 
     *      idno.eq.15.and.btest(listcresc,ivx)) then
	    call printl(' ')
          call printl('Started one kind of hairpin while other is on')
          call stop1()
        end if         
c
c  Start or stop?
c
        if (btest(listcresc,ivx) .or. btest(listdecresc,ivx)) then
c
c  Cresc/decresc is on, this is an ending. If fontslur, leave idno as is.
c
          if (.not.fontslur) idno = 15
        else if (fontslur) then
c
c  Start of font slur  
c
          idno = 13
        else
c
c  Start of postscript slur
c
          idno = idno-1
        end if
      end if
c
c  Now that we used list[de]cresc, update 
c
      if (idno .ge.13) then
        if (idno.eq.15 .or. (fontslur.and.idno.eq.14)) then
c
c  Something's ending
c
          if (btest(listcresc,ivx)) then
c
c  It's a cresc!
c
            listcresc = ibclr(listcresc,ivx)
          else
            listdecresc = ibclr(listdecresc,ivx)
          end if
        else 
c
c  Something's starting
c
          if (idno1 .eq. 14) then
c
c  It's a cresc!
c
            listcresc = ibset(listcresc,ivx)
          else
            listdecresc = ibset(listdecresc,ivx)
          end if
        end if
      end if
      call setbits(idyn,4,12,idno)
      iccount = iend
      if (ipm .ne. 2) then
c
c  There is a vertical shift
c
        idyn = ibset(idyn,16)
        iccount = iccount+1
        call readnum(lineq,iccount,durq,fnum)
        idno = nint(fnum)
        call setbits(idyn,7,17,(ipm-2)*idno+64)
        ipm = index('- +',durq)
        if (ipm .ne. 2) then
c
c  There is a horizontal shift
c
c          idynda2(ndyn) = ibset(idyn,23)
          idynda2(ndyn) = ibset(idynda2(ndyn),0)
          iccount = iccount+1
          call readnum(lineq,iccount,durq,fnum)
          idno = nint(10*fnum)
          call setbits(idynda2(ndyn),9,1,(ipm-2)*idno+256)
        end if
c
c  iccount should be on the blank at the end of the entire symbol
c
      end if
      idyndat(ndyn) = idyn
      return
      end
      subroutine getfig(itoff,charq,lineq,iccount,isfig,itfig,
c     *   itsofar,nodur,figq,ivupfig,nfigs)
     *   itsofar,nodur,figq,ivupfig,ivvfig,nfigs)
      logical isfig
      character*1 charq
      character*10 figq
      character*128 lineq
      nfigs = nfigs+1
      ivupfig = 0
      ivvfig = 0
      itoff = 0
      if (charq .eq. 'x') then
c
c  Floating figure.
c
        call getchar(lineq,iccount,charq)
        read(charq,'(i1)')noff
        call getchar(lineq,iccount,charq)
        read(charq,'(i1)')loff
        itoff = noff*ifnodur(loff,'x')
        call getchar(lineq,iccount,charq)
      else
c
c  Figure on a note
c
        isfig = .true.
      end if
      itfig = itsofar+itoff-nodur
      lfig = 1
      figq = charq
5     call getchar(lineq,iccount,charq)
c      if (index(' +',charq) .eq. 0) then
      if (index(' +v',charq) .eq. 0) then
        figq = figq(1:lfig)//charq
        lfig = lfig+1
        go to 5
      else if (charq .eq. '+') then
c
c  Get vertical offset for figure. Next character after number has to be blank.
c
        iccount = iccount+1
        call readnum(lineq,iccount,charq,fnum)
        ivupfig = nint(fnum)
      else if (charq .eq. 'v') then
c
c  Get vertical change in figdrop. Must be last item in figure word.
c
        isign = 1
        call getchar(lineq,iccount,charq)
        if (charq .eq. '-') then
          isign=-1
          call getchar(lineq,iccount,charq)
        end if
        ivvfig = isign*(ichar(charq)-48)
      end if
      return
      end
      subroutine getgrace(ivx,nnl,lineq,iccount,islur,iornq,ipl,ndlev,
     *                    lastlev,iv,nv)
      parameter (nm=24)
      common /comgrace/ ivg(37),ipg(37),nolevg(74),itoff(2,74),aftshft,
     *                nng(37),ngstrt(37),ibarmbr,mbrest,xb4mbr,
     *                noffseg,ngrace,nvolt,ivlit(83),iplit(83),nlit,
     *                graspace(37),
     *                lenlit(83),multg(37),upg(37),slurg(37),slashg(37),
     *                naccg(74),voltxtq(6),litq(83)
      logical upg,slurg,slashg
      character*1 charq,durq
      character*20 voltxtq
      character*128 lineq,litq
      integer*4 islur(nm,200),iornq(nm,0:200),ipl(nm,200),nnl(nm),
     *          ndlev(nm,2)
      common /comInstTrans/ iInstTrans(nm),iTransKey(nm),iTransAmt(nm),
     *  instno(nm),nInstTrans,EarlyTransOn,LaterInstTrans
      logical EarlyTransOn,LaterInstTrans
c
c Grace, comes *before* main note:
c UNLESS there's an 'A' or 'W' after the 'G'
c   ngrace = # of grace note groups so far in block
c   ivg(ngrace), ipg(ngrace)
c   nng(ngrace) = # of notes in this group: default = 1
c   ngstrt(ngrace) = starting position in nolevg of levels for this grace
c   multg(ngrace) = multiplicity: default = 1;  input as 'm(digit)'
c   upg(ngrace) = logical for beam or stem dirn: default T, input'u,l'
c   slurg(ngrace) = logical for slur; default F, input 's'
c   slashg(ngrace) = T if slash; default is F, input 'x'
c These data MUST precede note name of first note
c   nolevg, naccg: lists of levels and accid's, indexed as described above.
c
      ngrace = ngrace+1
      ivg(ngrace) = ivx
      ipg(ngrace) = nnl(ivx)+1
      if (ngrace .eq. 1) then
        ngstrt(ngrace) = 1
      else
        ngstrt(ngrace) = ngstrt(ngrace-1)+nng(ngrace-1)
      end if
      islur(ivx,nnl(ivx)+1) = ibset(islur(ivx,nnl(ivx)+1),4)
      nng(ngrace) = 1
      multg(ngrace) = 1
      upg(ngrace) = .true.
      slurg(ngrace) = .false.
      slashg(ngrace) = .false.
18    call getchar(lineq,iccount,charq)
      if (index('WA',charq) .gt. 0) then
c
c  Grace is on note that was already done, so shift flags forward one note.
c  This puts flag on actual note with grace; later for W will go ahead one more.
c
        ipg(ngrace) = nnl(ivx)
        islur(ivx,nnl(ivx)+1) = ibclr(islur(ivx,nnl(ivx)+1),4)
        islur(ivx,nnl(ivx)) = ibset(islur(ivx,nnl(ivx)),4)
        if (slurg(ngrace))
     *      iornq(ivx,nnl(ivx)) = ibset(iornq(ivx,nnl(ivx)),24)
        if (charq .eq. 'A') then
c
c  close After, clear way-after bit, to ensure priority of most recent A/W
c
          ipl(ivx,nnl(ivx)) = ibset(ibclr(ipl(ivx,nnl(ivx)),31),29)
        else
c
c  Way after; later assign to following note, and position like normal grace.
c
          ipl(ivx,nnl(ivx)) = ibset(ibclr(ipl(ivx,nnl(ivx)),29),31)
        end if
      else if (charq .eq. 'm') then
        call getchar(lineq,iccount,charq)
        multg(ngrace) = ichar(charq)-48
      else if (index('123456789',charq) .gt. 0) then
        call readnum(lineq,iccount,durq,fnum)
        iccount = iccount-1
        nng(ngrace) = nint(fnum)
      else if (charq .eq. 'l') then
        upg(ngrace) = .false.
      else if (charq .eq. 's') then
        slurg(ngrace) = .true.
        if (nnl(ivx) .gt. 0) then
c
c  If A- or W-grace, set signal to start slur on main note.
c
          if(btest(ipl(ivx,nnl(ivx)),31) .or.
     *               btest(ipl(ivx,nnl(ivx)),29))
     *       iornq(ivx,nnl(ivx))=ibset(iornq(ivx,nnl(ivx)),24)
        end if
      else if (charq .eq. 'x') then
        slashg(ngrace) = .true.
      else if (charq .eq. 'u') then
      else if (charq .eq. 'X') then
c
c Space before main note of grace. Number will come next.
c
        iccount = iccount+1
        call readnum(lineq,iccount,durq,graspace(ngrace))
        iccount = iccount-1
      end if
      if (index('abcdefg',charq) .eq. 0) go to 18
c
c  At this point, charq is first note name in grace
c
      do 19 ing = ngstrt(ngrace), ngstrt(ngrace)+nng(ngrace)-1
        naccg(ing) = 0
        ioct = 0
        if (ing .gt. ngstrt(ngrace)) then
55        call getchar(lineq,iccount,charq)
          if (charq .eq. ' ') go to 55
        endif
        iclastlev = 0
9       call getchar(lineq,iccount,durq)
        if (durq .ne. ' ') then
          if (durq.eq.'+') then
            lastlev = lastlev+7
            iclastlev = iclastlev+7
          else if (durq.eq.'-') then
            lastlev = lastlev-7
            iclastlev = iclastlev-7
          else if (index('fsn',durq) .gt. 0) then
            if (naccg(ing) .eq. 0) then
              naccg(ing) = index('fsn',durq)
            else
c
c  Double accidental
c
              naccg(ing) = ibset(naccg(ing),2)
            end if
          else
            ioct = ichar(durq)-48
          end if
          go to 9
        end if
        if (ioct .gt. 0) then
          lastlev = ifnolev(charq,ioct,iTransAmt(instno(iv)))
        else
          if (nnl(ivx).eq.0 .and. ing.eq.ngstrt(ngrace)) then
            if (ivx .le. nv) then
              kv = 1
            else
              kv = 2
            end if
            lastlev = ndlev(iv,kv)+iclastlev
          end if
          lastlev = lastlev-3
     *        +mod(ifnolev(charq,10,iTransAmt(instno(iv)))-lastlev+3,7)
        end if
        nolevg(ing) = lastlev
19    continue
c
c  Grace could come before first note of block, so reset end level.
c
      if (nnl(ivx).eq.0) then
        if (ivx .le. nv) then
          kv = 1
        else
          kv = 2
        end if
        ndlev(iv,kv) = lastlev
      end if
      return
      end
      subroutine GetiTransInfo(From1,ibarcnt,lineq,iccount,
     *                         ibaroff,nbars,noinst)
cccccccccccccccccccccccc
cc
cc GetiTransInfo.for
cc
cccccccccccccccccccccccc
c
c  Called from both g1etnote and getnote, after first 'i' in Ki[...]
c  On entry, iccount points to last char retrieved, which is 'i'
c  
c  From1: locgical, true if called from g1etnote
c  ibarcnt: tells whether to set EarlyTransOn to true. 
c  EarlyTransOn set false in blkdata, true here, back to false in topfile.  
c
c  110522/110529
c  Instrument-wise transposition Ki[iInstTrans][+/-][iTransAmt][+/-][iTransKey]
c    and repeat i[...] for multiple instruments. Store info in g1etnot if ibarcnt=0
c    so can pass to topfile (via comInstTrans), which is called before getnote. 
c    Otherwise, will store info from getnote. Initialize EarlyTransOn and
c    LaterInstTrans to .false. in blockdata. Set EarlyTransOn from g1etnote; 
c    LaterInstTrans from getnote. Zero both out after use. nInstTrans really
c    only needed for instrument-signatures, not transpositions. iTransAmt is
c    ALWAYS active per instrument. Set up instno(iv) so can fetch iTransAmt for
c    each staff.
c
c  iTransAmt stored as fn of instrument #, not like iTransKey which is 
c    fn. of nm, just a counter, where corr. inst num is iInstTrans(nm). This
c    simplifies use of iTransAmt for all calls to ifnolev. 
c
      parameter (nm=24)
      logical From1
      common /comInstTrans/ iInstTrans(nm),iTransKey(nm),iTransAmt(nm),
     *  instno(nm),nInstTrans,EarlyTransOn,LaterInstTrans
      logical EarlyTransOn,LaterInstTrans,store
      character*128 lineq
      character*1 durq 
      durq = 'x'  ! Can't initialize in declaration stmt, only works once.
      if(.not.EarlyTransOn) EarlyTransOn = From1 .and. ibarcnt.eq.0
      store = (EarlyTransOn.and.ibarcnt.eq.0) .or. 
     *            (ibarcnt.gt.0.and..not.From1)
      LaterInstTrans = .not.From1 .and. ibarcnt.gt.0
      if (store) nInstTrans = 0
1     continue
      if (durq .eq. ' ') return
      call g1etchar(lineq,iccount,durq)
      if (index('123456789',durq) .eq. 0) then
        call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *      'There must be an instrument number here!')
        call stop1()
      end if
      if (store) nInstTrans = nInstTrans+1
      call readnum(lineq,iccount,durq,fnum)
      instn = nint(fnum)
      if (instn.gt.noinst) then
        call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *      'Instrument number out of range!')
        call stop1()
      end if
      if (store) iInstTrans(nInstTrans) = instn
c
c  durq is +/- following inst # (for iTransAmt), iccount is on it.
c
      if (index('+-',durq) .eq. 0) then
        call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *      '1st character after instrument number must be "+,-"!')
        call stop1()
      end if
      itramt = 44-ichar(durq) ! +1/-1 for itramt
      call g1etchar(lineq,iccount,durq)
      if (index('0123456789',durq) .eq. 0) then
        call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *      'There must be a transposition amount here!')
        call stop1()
      end if
      call readnum(lineq,iccount,durq,fnum)
      if (store) iTransAmt(instn) = nint(fnum)*itramt
c
c  durq is +/- following iTransAmt (for iTransKey), iccount is on it.
c
      if (index('+-',durq) .eq. 0) then
        call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *      '1st character after transposition amount must be "+,-"!')
        call stop1()
      end if
      ikey = 44-ichar(durq)  ! +1/-1
      call g1etchar(lineq,iccount,durq)
      if (index('0123456789',durq) .eq. 0) then
        call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *      'There must be a key indicator here!')
        call stop1()
      end if
      call readnum(lineq,iccount,durq,fnum)
      if (store) iTransKey(nInstTrans) = nint(fnum)*ikey
c
c  durq is now 1st character after iTransKey, should be either 'i' or ' '
c
      if (durq.ne.'i'.and.durq.ne.' ') then
        call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *      'There must be blank or "i" here!')
        call stop1()
      end if
      go to 1
      end
      subroutine getmidi(noinstarg,lineq,iccount,ibarcnt,ibaroff,nbars,
     *                   lenbar,mtrdenl,nv,first)
c
c  Use this from both pmxa and pmxb to input and check midi data. "first" tells
c  whether pmxa or pmxb.  If .not.first, then tempo and pause commands cause
c  things to be written immediately into the midi storage buffers.
c
      parameter (nm=24,mv=24576)
      common /comevent/ miditime,lasttime
      logical mmacrec,gottempo
      common /commmac/ mmacstrt(0:nm,20),mmacend(0:nm,20),immac,
     *       mmactime(20),nmidsec,msecstrt(0:nm,60),msecend(0:nm,60),
     *       mmacrec,gottempo
c
c  immac(i) is the index of i-th macro, i=1,nmac.  Also make a list containing
c   nmidsec  section starts and stops based on PLAYING macros (not recording).
c 
      integer*2 mmidi,midinum(26)
      logical restpend,relacc,notmain,twoline,ismidi,crdacc
      common /commidi/ imidi(0:nm),trest(0:nm),mcpitch(20),mgap,
     *       iacclo(0:nm,6),iacchi(0:nm,6),midinst(nm),
     *       nmidcrd,midchan(nm,2),numchan,naccim(0:nm),
     *       laccim(0:nm,10),jaccim(0:nm,10),crdacc,notmain,
     *       restpend(0:nm),relacc,twoline(nm),ismidi,mmidi(0:nm,mv),
     *       debugmidi
      logical debugmidi
      common /commvel/ midivel(nm),midvelc(0:nm),midibal(nm),midbc(0:nm)
     *                ,miditran(nm),midtc(0:nm),noinstdum,iinsiv(nm)
      integer*2 iinsiv
      character*1 durq
      character*2 instq
      character*128 lineq
      logical first
      common /comdiag/ n69(0:nm),n34(0:nm)
c
c      Instrument codes
c
      data midinum   
     *    / 1, 5, 7, 13,20,25,33,41,42,43,44,57,58,59,61,65,66,67,
c         XXpiXrhXhaXmaXorXguXabXvlXvaXvcXcbXtrXtbXtuXfrXsoXalXteX
c
     *      68,69,71,72,74,75, 8,55 /
c           bsXobXbaXclXflXreXctXvo
c
1     call getchar(lineq,iccount,durq)
      if (durq .eq. 't') then
c
c  Tempo in beats ber minute
c
        call getchar(lineq,iccount,durq)
        if (index('0123456789',durq) .eq. 0) then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *       'Expected an integer here for the pause!')
          call stop1()
        end if
        call readnum(lineq,iccount,durq,qpm)
        iccount = iccount-1
        if (.not.first) then
          call midievent('t',nint(qpm),0)
          gottempo = .true.
        end if
        go to 1
      else if (durq .eq. 'p') then
c
c  Insert a pause.  pausemid = pause in 1/4's
c
        call getchar(lineq,iccount,durq)
        if (index('0123456789.',durq) .eq. 0) then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *       'Expected a number here for the pause!')
          call stop1()
        end if
        call readnum(lineq,iccount,durq,pausemid)
        iccount = iccount-1
        if (.not.first) then
c
c  Compute a meter for the pause.  This is only to keep MidiNotate on track.
c  Round pause to nearest 16th.  Let denominator always be 16.
c
          numb16 = nint(pausemid*4)
          call midievent('m',numb16,16)
c
c  Put in pausemid beats of rest 
c
          do 3 icm = 0 , numchan-1
            call addmidi(icm,0,0,0,4.*numb16,.true.,.false.)
3         continue
          miditime = miditime+nint(240*pausemid)
c
c  Restore meter
c
          call midievent('m',mtrdenl*lenbar/64,mtrdenl)
        end if
        go to 1
      else if (durq .eq. 'i') then
c
c  Instrument numbers or letters.  Expect noinst of them.
c
        do 2 ivx = 1 , noinstarg
          call getchar(lineq,iccount,durq)
          if (ichar(durq) .gt. 96) then
c
c  It's a lowercase letter.  Get another, find corr. instrument #.
c
            instq(1:1) = durq
            call getchar(lineq,iccount,durq)
            instq = instq(1:1)//durq
            iname = index('XXpiXrhXhaXmaXorXguXabXvlXvaXvcXcbXtrXtbX'//
     *                'tuXfrXsoXalXteXbsXobXbaXclXflXreXctXvo',instq)/3
            if (iname .eq. 0) then
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *          'Unrecognized 2-letter midi instrument name!')
              call stop1()
            end if
            midinst(ivx) = midinum(iname)-1
          else
c
c  Expect a number, followed by ":" if that is followed by another number.
c  I.e., if after call to readnum, durq is not ":", it must be either blank
c  or next instrument letter.
c
            if (index('123456789',durq) .eq. 0) then
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *                 'Expected a midi instrument number here!')
              call stop1()
            end if
            call readnum(lineq,iccount,durq,fnum)
            midinst(ivx) = nint(fnum)-1
            if (midinst(ivx).lt.0 .or. midinst(ivx).gt.255) then
              call errmsg(lineq,iccount-1,ibarcnt-ibaroff+nbars+1,
     *           'Midi instrument number must be in range 1-128!')
              call stop1()
            end if
            if (durq .ne. ':') iccount = iccount-1
          end if
2       continue
        go to 1
      else if (durq .eq. 'v') then 
c
c Get volumes for each instrument.  Expect noinst of them.  
c    Follow same pattern as for insttrument numbers above.
c 	
        do 7 ivx = 1 , noinstarg
          call getchar(lineq,iccount,durq)
          if (index('123456789',durq) .eq. 0) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *         'Expected a midi velocity number here!')
            call stop1()
          end if
          call readnum(lineq,iccount,durq,fnum)
          midivel(ivx) = nint(fnum)-1
          if (midivel(ivx).lt.0 .or. midivel(ivx).gt.127) then
            call errmsg(lineq,iccount-1,ibarcnt-ibaroff+nbars+1,
     *           'Midi velocity must be in range 1-128!')
            call stop1()
          end if
          if (durq .ne. ':') iccount = iccount-1
7       continue
        if (.not. first) then
          call inst2chan(midvelc,midivel,midchan,nv,iinsiv,twoline)
        end if
        go to 1
      else if (durq .eq. 'b') then 
c
c Get balance for each instrument.  Expect noinst of them.  
c    Follow same pattern as for instrument numbers above.
c 	
        do 8 ivx = 1 , noinstarg
          call getchar(lineq,iccount,durq)
          if (index('123456789',durq) .eq. 0) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *         'Expected a balance number here!')
            call stop1()
          end if
          call readnum(lineq,iccount,durq,fnum)
          midibal(ivx) = nint(fnum)-1
          if (midibal(ivx).lt.0 .or. midibal(ivx).gt.127) then
            call errmsg(lineq,iccount-1,ibarcnt-ibaroff+nbars+1,
     *           'Midi balance must be in range 1-128!')
            call stop1()
          end if
          if (durq .ne. ':') iccount = iccount-1
8       continue
        if (.not. first) then
          call inst2chan(midbc,midibal,midchan,nv,iinsiv,twoline)
        end if
        go to 1
      else if (durq .eq. 'T') then 
c
c Get transposition for each instrument.  Expect noinst of them.  
c    Follow similar pattern as above, but separator is +|-.
c 	
        do 9 ivx = 1 , noinstarg
          call getchar(lineq,iccount,durq)
          ipm = index('-+',durq)
          if (ipm .eq. 0) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *         'Expected "+" or "-" for midi transposition here!')
            call stop1()
          end if
          ipm = 2*ipm-3
          call getchar(lineq,iccount,durq)
          if (index('0123456789',durq) .eq. 0) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *         'Expected a number here!')
            call stop1()
          end if
          call readnum(lineq,iccount,durq,fnum)
          miditran(ivx) = ipm*nint(fnum)
c          if (mod(miditran(ivx),12).ne. 0) then
c            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
c     *         'Midi transposition limited to multiples of 12!')
c            call stop1()
c          end if
          iccount = iccount-1
9       continue
        if (.not. first) then
          call inst2chan(midtc,miditran,midchan,nv,iinsiv,twoline)
        end if
        go to 1
      else if (durq .eq. 'g') then
        call getchar(lineq,iccount,durq)
        if (index('0123456789',durq) .eq. 0) then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *       'Expected an integer here for the midi gap!')
          call stop1()
        end if
        call readnum(lineq,iccount,durq,fnum)
        mgap = nint(fnum)
        iccount = iccount-1
        go to 1
      else if (durq .eq. 'M') then
c
c  MidiMacros
c
        call getchar(lineq,iccount,durq)
        if (durq .eq. 'R') then
c
c  Start recording
c
          if (mmacrec) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *       'You tried to record a MidiMacro while already recording!')
            call stop1()
          end if
          mmacrec = .true.
          call getchar(lineq,iccount,durq)
          if (index('123456789',durq) .eq. 0) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *        'Expected MidiMacro ID number here!')
            call stop1()
          end if
          call readnum(lineq,iccount,durq,fnum)
          iccount = iccount-1
          if (.not.first) then
            immac = nint(fnum)
            if (immac .gt. 20) then
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *          'MidiMacro ID cannot exceed 20!')
              call stop1()
            end if
c
c  Save the start time
c
            mmactime(immac) = miditime
            do 4 icm = 0 , numchan
              if (icm .lt. numchan) then
                if (restpend(icm)) then
c
c  Adjust if there's a rest at end of prior section.  Insert dummy turnoff.
c    (This causes two turn-offs in a row, which testmidi sees as an error).
c
c  Before:    section1  ------rest-------  section2(to be recorded)
c  After:     section1  rest1  now  rest2  section2(recorded)
c
                  call addmidi(icm,30,0,0,trest(icm),.false.,.true.)
                  trest(icm) = 0
                  restpend(icm) = .false.
                end if
              else
                if (miditime .gt. lasttime) then
c
c  Insert a dummy turnoff in conductor track
c
                  call addmidi(icm,30,0,0,(miditime-lasttime)/15.,
     *                         .false.,.true.)
                  lasttime = miditime
                end if
              end if
              mmacstrt(icm,immac) = imidi(icm)+1
4           continue
          end if
          go to 1
        else if (index('123456789P',durq) .eq. 0) then
c
c  End recording; close the open macro.  Get immac from common.
c
          if (.not.mmacrec) then
            call errmsg(lineq,iccount-1,ibarcnt-ibaroff+nbars+1,
     *       'You tried to end a MidiMacro before starting one!')
            call stop1()
          end if
          mmacrec = .false.
          iccount = iccount-1
          if (.not.first) then
c
c  Save the macro duration
c
            mmactime(immac) = miditime-mmactime(immac)
            do 5 icm = 0 , numchan
              if (icm .lt. numchan) then
                if (restpend(icm)) then
                  call addmidi(icm,30,0,0,trest(icm),.false.,.true.)
                  trest(icm) = 0
                  restpend(icm) = .false.
                end if
              else
                if (miditime .gt. lasttime) then
c
c  Insert a dummy turnoff in conductor track if needed.
c
                  call addmidi(icm,30,0,0,(miditime-lasttime)/15.,
     *                         .false.,.true.)
                  lasttime = miditime
                end if
              end if
              mmacend(icm,immac) = imidi(icm)
5           continue
          end if
          if (durq .ne. ' ')go to 1
        else if (durq .eq. 'P') then
c
c  Play Back a Macro
c
          call getchar(lineq,iccount,durq)
          if (index('123456789',durq) .eq. 0) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *        'Expected MidiMacro ID number here!')
            call stop1()
          end if
          if (mmacrec) then
            call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *       'You tried to play a MidiMacro before ending recording!')
            call stop1()
          end if
          call readnum(lineq,iccount,durq,fnum)
          iccount = iccount-1
          if (.not.first) then
            immac = nint(fnum)
            if (mmactime(immac) .eq. 0) then
              call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *          'Cannot play a MIDI macro before recording it!')
              call stop1()
            end if
            do 6 icm = 0 , numchan
              if (icm .lt. numchan) then
                if (restpend(icm)) then
                  call addmidi(icm,30,0,0,trest(icm),.false.,.true.)
                  trest(icm) = 0.
                  restpend(icm) = .false.
                end if
              else
                if (miditime .gt. lasttime) then
c
c  Insert a dummy turnoff in conductor track
c
                  call addmidi(icm,30,0,0,(miditime-lasttime)/15.,
     *                         .false.,.true.)
                end if
              end if
              msecend(icm,nmidsec) = imidi(icm)
              msecstrt(icm,nmidsec+1) = mmacstrt(icm,immac)
              msecend(icm,nmidsec+1) = mmacend(icm,immac)
              msecstrt(icm,nmidsec+2) = imidi(icm)+1
6           continue
            nmidsec = nmidsec+2
c
c  Update running time
c
            miditime = miditime+mmactime(immac) 
            lasttime = miditime
          end if
          go to 1 
        else
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *      'Illegal character in MidiMacro sub-command!')
          call stop1()
        end if
      else if (durq .eq. 'd') then
        debugmidi = .true.
        go to 1
      else if (durq .ne. ' ') then
        call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *      'Illegal character in MIDI input data!')
        write(*,'(a)')
     *    'May be too many args to i,v,b, or T. As of Ver. 2.7, '//
     *    'should be noinst, not nv'   
        write(15,'(a)')
     *    'May be too many args to i,v,b, or T. As of Ver. 2.7, '//
     *    'should be noinst, not nv'   
        call stop1()
      end if
      if (.not.gottempo .and. .not.first) then
c
c  If no tempo is set on first call on the pmxb pass, then set it
c
        call midievent('t',96,0)
        gottempo = .true.
      end if
      return
      end
      subroutine getnote(loop)
      parameter (nm=24,mv=24576)
      common /comlast/ islast,usevshrink
      logical islast,usevshrink,ignorenats,newmbrhgt
      common /comignorenats/ mbrhgt,newmbrhgt,ignorenats
      common /all/ mult(nm,200),iv,nnl(nm),nv,ibar,
     *   ivxo(600),ipo(600),to(600),tno(600),tnote(600),eskz(nm,200),
     *   ipl(nm,200),ibm1(nm,9),ibm2(nm,9),nolev(nm,200),ibmcnt(nm),
     *   nodur(nm,200),jn,lenbar,iccount,nbars,itsofar(nm),nacc(nm,200),
     *   nib(nm,15),nn(nm),lenb0,lenb1,slfac,musicsize,stemmax,
     *   stemmin,stemlen,mtrnuml,mtrdenl,mtrnmp,mtrdnp,islur(nm,200),
     *   ifigdr(2,125),iline,figbass,figchk(2),firstgulp,irest(nm,200),
     *   iornq(nm,0:200),isdat1(202),isdat2(202),nsdat,isdat3(202),
     *   isdat4(202),beamon(nm),isfig(2,200),sepsymq(nm),sq,ulq(nm,9)
      character*10 figq
      character*1 ulq,sepsymq,sq,rptfq1,rptfq2,clefq,chax
      logical beamon,firstgulp,figbass,figchk,isfig
      character*60 cheadq
      character*120 instrq,titleq,compoq
      integer*2 mmidi
      logical restpend,relacc,notmain,twoline,ismidi,crdacc,autofbon
      common /commidi/ imidi(0:nm),trest(0:nm),mcpitch(20),mgap,
     *       iacclo(0:nm,6),iacchi(0:nm,6),midinst(nm),
     *       nmidcrd,midchan(nm,2),numchan,naccim(0:nm),
     *       laccim(0:nm,10),jaccim(0:nm,10),crdacc,notmain,
     *       restpend(0:nm),relacc,twoline(nm),ismidi,mmidi(0:nm,mv),
     *       debugmidi
      logical debugmidi
      common /combjmp/ ivbj1,ivbj2,isbjmp,isbj2,multbj1
      common /comtitl/ instrq,titleq,compoq,headlog,inskip,ncskip,
     *    inhead
      common /cominbot/ inbothd
      common /comfig/ itfig(2,74),figq(2,74),ivupfig(2,74),nfigs(2),
     *                fullsize(nm),ivxfig2,ivvfig(2,74)
      common /comnotes/ nnodur,lastlev,ndlev(nm,2),shifton,setis,notcrd,
     *       npreslur,was2(nm),ninow,nobar1,nsystp(75),ipage,
     *       OptLineBreakTies,HeaderSpecial
      common /comget/ lastchar,rptnd1,sluron(nm,2),fbon,ornrpt,stickyS,
     *       movbrk,movnmp,movdnp,movgap,parmov,fintstf,gintstf,
     *       rptprev,equalize,rptfq1,rptfq2
      common /comhead/ ihdht,lower,headrq,lowerq,ihdvrt
      common /comfb/ nfb(nm),t1fb(nm,40),t2fb(nm,40),ulfbq(nm,40),ifb,
     *               tautofb,autofbon,t1autofb
      common /comgrace/ ivg(37),ipg(37),nolevg(74),itoff(2,74),aftshft,
     *                nng(37),ngstrt(37),ibarmbr,mbrest,xb4mbr,
     *                noffseg,ngrace,nvolt,ivlit(83),iplit(83),nlit,
     *                graspace(37),
     *                lenlit(83),multg(37),upg(37),slurg(37),slashg(37),
     *                naccg(74),voltxtq(6),litq(83)
      logical upg,slurg,slashg,fbon,ornrpt,shifton,setis,notcrd,isbjmp,
     *        was2,nobar1,quoted,isbj2,stickyS,
     *        OptLineBreakTies,HeaderSpecial
      common /comtrill/ ntrill,ivtrill(24),iptrill(24),xnsktr(24),
     *                ncrd,icrdat(193),icrdot(193),icrdorn(193),nudorn,
     *                kudorn(63),ornhshft(63),minlev,maxlev,icrd1,icrd2
      character*128 litq
      common /comcc/ ncc(nm),tcc(nm,10),ncmidcc(nm,10),
     *               maxdotmv(nm),ndotmv(nm),updot(nm,20),rtdot(nm,20)
      common /combc/ bcspec
      common /comas3/ ask(2500),iask,topmods
c
c  nvmx is either 1 or 2.  ivmx(iv,1)=iv, ; ivmx(iv,2)>nv if defined
c  ivx is current ivmx, and is the index for all notes, acc's etc.
c
      common /commvl/ nvmx(nm),ivmx(nm,2),ivx
      common /spfacs/ grafac,acgfac,accfac,xspfac,xb4fac,clefac,emgfac,
     *                flagfac,dotfac,bacfac,agc1fac,gslfac,arpfac,
     *                rptfac,lrrptfac,dbarfac,ddbarfac,dotsfac,upstmfac,
     *                rtshfac
      common /comtrans/ cheadq
      common /comtop / itopfacteur,ibotfacteur,interfacteur,isig0,
     *   isig,lastisig,fracindent,widthpt,height,hoffpt,voffpt,idsig,
     *   lnam(nm),inameq(nm)
      common /commac/ macnum,mrecord,mplay,macuse,icchold,lnholdq,endmac
      common /comudsp/udsp(50),tudsp(50),nudsp,udoff(nm,20),nudoff(nm)
      common /comask/ bar1syst,fixednew,scaldold,
     *                wheadpt,fbar,poenom
      common /comslur/ listslur,upslur(nm,2),ndxslur,fontslur
     *                 ,WrotePsslurDefaults,SlurCurve
      common /comarp/ narp,tar(8),ivar1(8),ipar1(8),levar1(8),ncmar1(8),
     *                xinsnow,lowdot
      common /comnvi/ nsperi(nm),nspern(nm),rename,iiorig(nm)
      common /c1ommac/ ip1mac(20),il1mac(20),ip2mac(20),il2mac(20),
     *                 ic1mac(20),ilmac,iplmac
      common /comdyn/ ndyn,idyndat(99),levdsav(nm),ivowg(12),hoh1(12),
     *        hoh2(12),hoh2h1(2),ntxtdyn,ivxiptxt(41),txtdynq(41),
     *        idynda2(99),levhssav(nm),listcresc,listdecresc
      common /comclefq/ clefq(nm)
      common /comtol/ tol
      common /comArpShift/NumArpShift,IvArpShift(20),IpArpShift(20),
     *                    ArpShift(20)
      character*128 txtdynq
      logical mrecord,mplay,endmac,lowdot,rename,bar1syst,upslur,
     *        fontslur,WrotePsslurDefaults
      character*128 lineq,lnholdq,lineqt
      character*80 headrq, lowerq
      character*79 inameq
      character*59 hdlndq
      character*20 voltxtq
      character*1 charq,dotq,durq,ulfbq,dumq
      logical loop,lastchar,lower,rptnd1,rptprev,bcspec,moved,
     *        topmods,headlog,btest,sluron,cdot,equalize
      character*24 tempq
      logical novshrinktop,cstuplet
      common /comnvst/ novshrinktop,cstuplet
      logical kbdrests
      common /comkbdrests/ levbotr(8),levtopr(8),kbdrests
      common /comInstTrans/ iInstTrans(nm),iTransKey(nm),iTransAmt(nm),
     *  instno(nm),nInstTrans,EarlyTransOn,LaterInstTrans
      logical EarlyTransOn,LaterInstTrans
      common /comkeys/ nkeys,ibrkch(18),newkey(18),iskchb,idumm1,isig1,
     *     mbrestsav,kchmid(18),logdumm1,logdumm2,barend,noinst,logdumm3
      logical barend,iskchb,kchmid,logdumm1,logdumm2,logdumm3
      common /combibarcnt/ ibarcnt
      common /commidisig/ midisig
      common /comlyr/ inputmlyr
      logical inputmlyr
      common /combottop/ botamt,topamt,bottopgap
      logical bottopgap
      logical inxtup
      common /comshort/ shortfrac,codafrac,ishort,mbrsum,nmbr,nocodabn,
     *  poefa
      real*4 poefa(125)
      logical nocodabn
      cdot = .false.
      inxtup = .false.
1     call getchar(lineq,iccount,charq)
      if (lastchar) return
      if (charq .eq. ' ') go to 1
      if (charq .eq. '%') then
c
c  Check for a bar number format:
c
        if (iccount.eq.1 .and. lineq(2:2).eq.' ' .and.
     *      index('bB1234567890',lineq(3:3)).gt.0) then
          if (islast) write(11,'(a)')lineq(1:lenstr(lineq,128))
        end if
        iccount = 128
        go to 1
      end if
c
c  Closing repeat iff charq='/' and the prev. char was 'R' with 'd' or 'r'
c
      if (rptprev) then
        rptnd1 = charq .eq. '/'
        rptprev = .false.
      end if
c
c  Repeat at end of a piece
c
      if ((ichar(charq).ge.97.and.ichar(charq).le.103) .or.
     *       charq.eq.'r') then
        if (cdot) go to 28
c
c  This is a note/rest.  
c
        idotform = 0
        numnum = 0
c
c  If start of line of music, set pitch from previous
c
        if (ivx .le. nv) then
          kv = 1
        else
          kv = 2
        end if
        if (nnl(ivx).eq.0) lastlev = ndlev(iv,kv)
c
c  notcrd is used to tell if orn. goes on main note or chord note
c
c        notcrd = .true.   !Move dow.  Was not observed if dotted shortcut.
c
c  Increase note count, then loop 'til blank. Label 28 is for dotted shortcuts.
c
28      continue
c
c  Moved this from just above, 2 Feb 02
c
        notcrd = .true.
        nnl(ivx) = nnl(ivx)+1
        if (ornrpt) then
c
c  Replicate ornament bits, also bit 23 for beam handling if chord.
c
          iornq(ivx,nnl(ivx)) = ior(iornq(ivx,nnl(ivx)),iornq(ivx,0))
          if (iand(iornq(ivx,nnl(ivx)),32896) .gt. 0) then
c
c  This is a trill (bit 7 or 15) so must dup the parameters
c
            ntrill = ntrill + 1
            ivtrill(ntrill) = ivx
            iptrill(ntrill) = nnl(ivx)
            xnsktr(ntrill) = xnsktr(ntrill-1)
          end if
        end if
        if (stickyS) then
c
c  Grab stemlength shortening parameters from prior note
c
          mult(ivx,nnl(ivx)) = ibset(mult(ivx,nnl(ivx)),27)
c          call setbits (mult(ivx,nnl(ivx)),3,28,
c     *                  igetbits(mult(ivx,nnl(ivx)-1),3,28))
          call setbits (mult(ivx,nnl(ivx)),6,10,
     *                  igetbits(mult(ivx,nnl(ivx)-1),6,10))
        end if
        if (autofbon .and. tautofb.gt.tol .and. .not.fbon) then 
c
c  Doing auto forced beams, and period has been set, so check if this note
c    starts a period.
c
c          if (mod(1.*itsofar(ivx),tautofb) .lt. tol) then
          if (mod(itsofar(ivx)-t1autofb,tautofb) .lt. tol) then
c
c  Start a forced beam here
c
            nfb(ivx) = nfb(ivx)+1
            fbon = .true.
            ulfbq(ivx,nfb(ivx)) = 'x'
            t1fb(ivx,nfb(ivx)) = itsofar(ivx)
          end if
        end if
        if (fbon) ipl(ivx,nnl(ivx)) = ibset(ipl(ivx,nnl(ivx)),30)
        dotq = 'x'
        if (charq.eq.'r')
     *      irest(ivx,nnl(ivx)) = ibset(irest(ivx,nnl(ivx)),0)
        if (btest(irest(ivx,nnl(ivx)),0)) then
c
c  Rest stuff.  First check if previous note was full-bar-pause
c
          if (lineq(iccount+1:iccount+1) .eq.' '
     *                    .and. nnl(ivx).gt.1) then
            if (btest(islur(ivx,nnl(ivx)-1),19))
     *          islur(ivx,nnl(ivx)) = ibset(islur(ivx,nnl(ivx)),19)
          end if
c
c  Set default rest level at 0 unless 2 voices/staff in which case it's -4 or 2
c  for voice a or b.  Set a-types at 0 as encountered and adjust later
c  after '//'.  (Override heights will be set to 100+offset)
c
          if (ivx .le. nv) then
            nolev(ivx,nnl(ivx)) = 0
          else
            nolev(ivx,nnl(ivx)) = 2
          end if
        end if
2       call getchar(lineq,iccount,durq)
        ic = ichar(durq)
        if (ic.le.57 .and. ic.ge.48) then
c
c  Digit
c
          if (numnum .eq. 0) then
            nnodur = ic-48
            numnum = 1
            go to 2
          else if (numnum .eq. 1) then
            ioct = ic-48
            numnum = 2
            go to 2
          else
            print*,'>2 digits in note sym., ivx,nn:',ivx,nnl(ivx)
            call stop1()
          end if
        else if (durq .eq. 'd') then
          dotq = durq
          if (lineq(iccount+1:iccount+1) .eq. 'd') then
c
c  Double dot.
c
            iccount = iccount+1
            islur(ivx,nnl(ivx)) = ibset(islur(ivx,nnl(ivx)),3)
          end if
          if (index('+-',lineq(iccount+1:iccount+1)) .gt. 0) then
c
c  move a dot, unless next char is not part of a number
c
            if (index('0123456789.',lineq(iccount+2:iccount+2)) .eq. 0)
     *         go to 2
            irest(ivx,nnl(ivx)) = ibset(irest(ivx,nnl(ivx)),19)
            call getchar(lineq,iccount,durq)
            ndotmv(ivx) = ndotmv(ivx)+1
            iccount=iccount+1
            call readnum(lineq,iccount,dumq,updot(ivx,ndotmv(ivx)))
            if (durq .eq. '-')
     *         updot(ivx,ndotmv(ivx)) = -updot(ivx,ndotmv(ivx))
            if (index('+-',dumq) .gt. 0) then
c
c  Vertical shift also
c
              iccount=iccount+1
              call readnum(lineq,iccount,durq,rtdot(ivx,ndotmv(ivx)))
              if (dumq .eq. '-')
     *         rtdot(ivx,ndotmv(ivx)) = -rtdot(ivx,ndotmv(ivx))
            else
              rtdot(ivx,ndotmv(ivx)) = 0.
            end if
            iccount = iccount-1
          end if
          go to 2
        else if (durq .eq. 'p') then
c
c  Full-bar rest as pause
c
          islur(ivx,nnl(ivx)) = ibset(islur(ivx,nnl(ivx)),19)
          go to 2
        else if (durq .eq. 'b') then
c
c  Blank rest
c
          islur(ivx,nnl(ivx)) = ibset(islur(ivx,nnl(ivx)),29)
          go to 2
        else if (index('fsn',durq) .gt. 0) then
c
c  Accidental
c
c          if (nacc(ivx,nnl(ivx)) .eq. 0) then
c  171209 May have set bit 18 earlier if D or F preceded accidental
          if (igetbits(nacc(ivx,nnl(ivx)),2,0) .eq. 0) then
c
c  No accidental has been set yet
c
c            nacc(ivx,nnl(ivx)) = index('fsn',durq)
            nacc(ivx,nnl(ivx)) = 
     *          ior(nacc(ivx,nnl(ivx)),index('fsn',durq))
          else
c
c  Repeated accid, so must be double
c
            nacc(ivx,nnl(ivx)) = ibset(nacc(ivx,nnl(ivx)),2)
          end if
          go to 2
        else if (durq .eq. 'i') then
c
c  Set flag for MIDI-only accidental.
c
          nacc(ivx,nnl(ivx)) = ibset(nacc(ivx,nnl(ivx)),17)
          go to 2
        else if (durq .eq. 'c') then
c
c  Set flags for cautionary accidental
c
          irest(ivx,nnl(ivx)) = ibset(irest(ivx,nnl(ivx)),31)
          iornq(ivx,nnl(ivx)) = ibset(iornq(ivx,nnl(ivx)),31)
          go to 2
        else if (index('+-<>',durq) .gt. 0) then
          ipm = index('- +',durq)-2
          if (.not.btest(irest(ivx,nnl(ivx)),0)) then
c
c  A note, not a rest.
c
            call chkpm4ac(lineq,iccount,nacc(ivx,nnl(ivx)),moved)
            if (moved) go to 2
c
c  Octave jump with a note
c
            if (numnum .lt. 2) then
              lastlev = lastlev+ipm*7
            else
              ioct = ioct+ipm*1
            end if
            go to 2
          else
c
c  Override default height of a rest
c
            iccount = iccount+1
            call readnum(lineq,iccount,durq,fnum)
            if (lineq(iccount-1:iccount-1) .eq. '.') then
c
c  Kluge in case there is a shortcut ".". It will have been sucked up by
c  readnum.  (Same doesn't hold for ",")
c
               iccount = iccount-1
c               go to 2
            end if
            nolev(ivx,nnl(ivx)) = 100+ipm*nint(fnum)
c
c  There may be more characters for this rest
c
            iccount = iccount-1
            go to 2
          end if
        else if (durq .eq. 'x') then
c
c  Xtuplet.  Count number of doubled notes (for unequal xtups)
c
          if (btest(nacc(ivx,nnl(ivx)),18)) then
            ndoub = 1
          else
            ndoub = 0
          end if
c
c  Initialize counter for # of non-rests, so can later unbeam if = 1.
c
          inxtup = .true.
          note1xtup = nnl(ivx)
          nnb = 0
          if (.not.btest(irest(ivx,nnl(ivx)),0)) nnb = 1
c
c  Will set all durations to 0 except last one.  Set flag on this note.
c
          irest(ivx,nnl(ivx)) = ibset(irest(ivx,nnl(ivx)),28)
c
c  Next input will be digit unless its a "T"
c
          iccount = iccount+1
          if (lineq(iccount:iccount) .eq. 'T') then
c
c  Set up tremolo
c
            irest(ivx,nnl(ivx)) = ibset(irest(ivx,nnl(ivx)),2)
            ntup = 2
c
c  Set default beaming, based on Wikipedia article
c  May be a problem here is nnodur is inherited
c
            if (nnodur .eq. 4) then
              nsolid = 0
              nindent = 3
            else if (nnodur .eq. 2) then
              nsolid = 3
              nindent = 0
            else if (nnodur .eq. 8) then
              nsolid = 1
              nindent = 2
            end if
            call getchar(lineq,iccount,durq)
            if (index('0123456789',durq) .gt. 0) then
              nsolid = index('0123456789',durq)-1
              if (nsolid.eq.0 .and. nnodur.eq.2) then
                print*,''
                print*,'Unbeamed half-note 2-note tremolo forbidden.'
                call stop1()
              end if   
              call getchar(lineq,iccount,durq)
              if (index('0123456789',durq) .gt. 0) then
                nindent = index('0123456789',durq)-1
              end if
            end if 
            call setbits(irest(ivx,nnl(ivx)),2,3,nsolid)
            call setbits(irest(ivx,nnl(ivx)),2,5,nindent)
            if (nsolid .eq. 0) then
              islur(ivx,nnl(ivx)) =
     *            ibset(islur(ivx,nnl(ivx)),18)
            end if
            islur(ivx,nnl(ivx)) = ibset(islur(ivx,nnl(ivx)),31)
            islur(ivx,nnl(ivx)+1) = ibset(islur(ivx,nnl(ivx)+1),21)
            call setbits(islur(ivx,nnl(ivx)+1),3,22,nsolid)
c
c  Set some force-beam parameters for the tremolo
c
c  Need to check if there's already a forced beam explicitly set here.
c  If there was, shouldn't do any harm resetting parameters.
c
            if (nfb(ivx).eq.0 .or. t1fb(ivx,nfb(ivx)).ne.itsofar(ivx))
     *           nfb(ivx) = nfb(ivx)+1
            fbon = .true.
            ulfbq(ivx,nfb(ivx)) = 'x'
            t1fb(ivx,nfb(ivx)) = itsofar(ivx)
            t2fb(ivx,nfb(ivx)) = itsofar(ivx) + ifnodur(nnodur,dotq)
            nadj = 0
c
c Set open beamed notehead flag for half-note tremolo
c   Just gave 2 quarters with or without flag set. Need to fix
c
          else
            call readnum(lineq,iccount,durq,fnum)
            ntup = nint(fnum)
          end if
          if (index('DF',durq) .gt. 0) then
c
c
c  Double xtup note to make an un= xtup. Here xtup number already set but may also
c    have this command before.
c
            nacc(ivx,nnl(ivx)) = ibset(nacc(ivx,nnl(ivx)),18)
            if (durq .eq. 'F') nacc(ivx,nnl(ivx)) = 
     *                  ibset(nacc(ivx,nnl(ivx)),19)
            ndoub = 1
            call getchar(lineq,iccount,durq)
          else if (durq .eq. 'd') then
            nacc(ivx,nnl(ivx)) = ibset(nacc(ivx,nnl(ivx)),27)
            call getchar(lineq,iccount,durq)
          end if
c
c  Only other possibilities here are ' ' or 'n'
c
          if (durq .eq. 'n') then
c
c  Alter xtup number
c
            if (lineq(iccount+1:iccount+1) .eq. ' ') then
c
c  If the only modifier is 'n', cancel the number
c
              islur(ivx,nnl(ivx)) = ibset(islur(ivx,nnl(ivx)),31)
            else
              numshft = 0
30            call getchar(lineq,iccount,durq)
              if (durq .eq. 'f') then
c
c  Flip up-down-ness
c
                irest(ivx,nnl(ivx)) = ibset(irest(ivx,nnl(ivx)),14)
                go to 30
              else if (index('+-',durq) .gt. 0) then
c
c  Vertical or horiz shift
c
                numshft = numshft+1
                iofforn = 1
                if (durq .eq. '-') iofforn = -1
                iccount = iccount+1
                call readnum(lineq,iccount,durq,xofforn)
                iccount = iccount-1
                if (numshft .eq. 1) then
c
c  Vertical shift
c  160214 Allow (-64,64)
c                  iofforn = iofforn*nint(xofforn) + 16
                  iofforn = iofforn*nint(xofforn) + 64
c
cc  Turn on bit 1; set bits 2-6 to iofforn
c  Turn on bit 1 of irest; set bits 16-22 of mult to iofforn
c
c                  irest(ivx,nnl(ivx)) =
c     *               ior(irest(ivx,nnl(ivx)),2+4*iofforn)
                  irest(ivx,nnl(ivx)) = ibset(irest(ivx,nnl(ivx)),1)
                  call setbits(mult(ivx,nnl(ivx)),8,16,iofforn)
                else
c
c  Horizontal shift
c
                  iofforn = iofforn*nint(xofforn*10)+16
                  irest(ivx,nnl(ivx)) = ibset(irest(ivx,nnl(ivx)),7)
                  call setbits(irest(ivx,nnl(ivx)),5,9,iofforn)
                end if
                go to 30
              else if (durq .eq. 's') then
c
c Slope adjustment for bracket
c
                mult(ivx,nnl(ivx)) = ibset(mult(ivx,nnl(ivx)),4)
                call getchar(lineq,iccount,durq)
                iofforn = index('- +',durq)-2
                iccount = iccount+1
                call readnum(lineq,iccount,durq,xofforn)
                iccount = iccount-1
                iofforn = nint(iofforn*xofforn+16)
                call setbits(mult(ivx,nnl(ivx)),5,5,iofforn)
              else if (index('123456789',durq) .gt. 0) then
c
c  Replacement printed number
c
                call readnum(lineq,iccount,durq,xofforn)
                call setbits(nacc(ivx,nnl(ivx)),5,22,nint(xofforn))             
                iccount = iccount-1
                go to 30
              end if
            end if
          end if
c
c  Set note level of 1st note of xtup, provided not a rest
c
          if (.not.btest(irest(ivx,nnl(ivx)),0)) then
            if (numnum .eq. 2) then
              lastlev = ifnolev(charq,ioct,iTransAmt(instno(iv)))
              nolev(ivx,nnl(ivx)) = lastlev
            else
              lastlev = lastlev-3
     *         +mod(ifnolev(charq,10,iTransAmt(instno(iv)))-lastlev+3,7)
              nolev(ivx,nnl(ivx)) = lastlev
            end if
          end if
          do 40 npreslur = npreslur , 1 , -1
c
c  Set note level for preslur on starting note of xtuplet
c
            call setbits(isdat2(nsdat-npreslur+1),7,19,lastlev)
40        continue
          numnum = 0
          nodur(ivx,nnl(ivx)) = 0
          do 6 itup = 2 , ntup
            if (ornrpt) then
              iornq(ivx,nnl(ivx)) = ior(iornq(ivx,nnl(ivx)),
     *           iand(iornq(ivx,nnl(ivx)-1),10026991))
              if (iand(iornq(ivx,nnl(ivx)),32896) .gt. 0) then
c
c  This is a trill (bit 7 or 15) so must dup the parameters
c
                ntrill = ntrill + 1
                ivtrill(ntrill) = ivx
                iptrill(ntrill) = nnl(ivx)
                xnsktr(ntrill) = xnsktr(ntrill-1)
              end if
            end if
            nnl(ivx) = nnl(ivx)+1
            if (fbon) ipl(ivx,nnl(ivx)) = ibset(ipl(ivx,nnl(ivx)),30)
7           call getchar(lineq,iccount,charq)
            if (charq .eq. ' ') then
              go to 7
            else if (charq .eq. '%') then
              iccount = 128
              go to 7
            else if (charq .eq. 'o') then
c
c  Ornament in xtuplet.  "o" symbol must come AFTER the affected note
c
              if (notcrd) then
                nole = nolev(ivx,nnl(ivx)-1)
              else
                nole = iand(127,ishft(icrdat(ncrd),-12))
              end if
              call getorn(lineq,iccount,iornq(ivx,nnl(ivx)-1),
     *             iornq(ivx,0),ornrpt,noffseg,
     *             nnl(ivx)-1,ivx,.false.,notcrd,nole)
              go to 7
            else if (index('st(){}',charq) .gt. 0) then
              nnlivx = nnl(ivx)-1
              if (charq.eq.'(' .or. charq.eq.'{') then
c
c  Detected preslur in xtuplet loop, non-chord note
c
                nnlivx = nnlivx+1
                npreslur = npreslur+1
              end if
              islur(ivx,nnlivx) = ibset(islur(ivx,nnlivx),0)
              if (charq.eq.'t') islur(ivx,nnlivx) =
     *                    ibset(islur(ivx,nnlivx),1)
              if (ivx .le. nv) then
                kv = 1
              else
                kv = 2
              end if
              if (fontslur) then
                call sslur(lineq,iccount,iv,kv,nnlivx,isdat1,isdat2,
     *                   isdat3,nsdat,notcrd,nolev(ivx,nnlivx),charq)
              else
                call spsslur(lineq,iccount,iv,kv,nnlivx,isdat1,isdat2,
     *               isdat3,isdat4,nsdat,notcrd,nolev(ivx,nnlivx),charq)
              end if
              go to 7
            else if (charq .eq. 'G') then
c
c Kluge to get grace in xtup at right location
c
              nnl(ivx) = nnl(ivx)-1
              call getgrace(ivx,nnl,lineq,iccount,islur,iornq,ipl,ndlev,
     *                lastlev,iv,nv)
              nnl(ivx) = nnl(ivx)+1
              go to 7
            else if (charq .eq. sq) then
              call littex(islur,nnl(ivx),ivx,topmods,lineq,iccount)
              go to 7
            else if (charq .eq. '"') then
c
c  pmxlyr string in xtup. Expand "..." to \pmxlyr{...}\
c
              if (.not. inputmlyr) then
                ictemp = 0
                lineqt = sq//sq//sq//'input musixlyr '//sq
                call littex(islur,nnl(ivx)+1,ivx,topmods,lineqt,ictemp)
                inputmlyr = .true.
              end if
              call dopmxlyr(lineq,iccount)
              charq = sq
              call littex(islur,nnl(ivx),ivx,topmods,lineq,iccount)
              go to 7
            else if (index('0123456789#-nx_',charq) .gt. 0) then
c
c  Figure.  Must come AFTER the first note of xtup
c
              ivf = 1
              if (ivx .gt. 1) then
                if (ivxfig2 .eq. 0) then
                  ivxfig2 = ivx
                else if (ivx .ne. ivxfig2) then
                  print*
                  print*,'Figures not allowed in >1 voice above first'
                  stop
                end if
                ivf = 2
              end if
              nfig1 = nfigs(ivf)+1
			call getfig(itoff(ivf,nfig1),charq,lineq,iccount,
     *             isfig(ivf,nnl(ivx)-1),itfig(ivf,nfig1),itsofar(ivx),
c     *             0,figq(ivf,nfig1),ivupfig(ivf,nfig1),nfigs(ivf))
     *           0,figq(ivf,nfig1),ivupfig(ivf,nfig1),ivvfig(ivf,nfig1),
     *           nfigs(ivf))
              go to 7
            else if (charq .eq. 'X') then
              call getx(lineq,iccount,irest(ivx,max(1,nnl(ivx)-1)),
     *            shifton,wheadpt,iornq(ivx,nnl(ivx)),ivx,
     *            irest(ivx,nnl(ivx)),itsofar(ivx),ntup,itup,nnodur,
     *            dotq,ndoub)
              go to 7
            else if (charq .eq. 'z') then
c
c  Chord note in xtup.  Goes with *prior* note.
c
              notcrd = .false.
              ncrd = ncrd+1
              ipl(ivx,nnl(ivx)-1) = ibset(ipl(ivx,nnl(ivx)-1),10)
              numnum = 0
c              icrdat(ncrd) = ior(nnl(ivx)-1,ishft(ivx,8))
              icrdat(ncrd) = nnl(ivx)-1
              call setbits(icrdat(ncrd),4,8,mod(ivx,16))
              if (ivx .ge. 16) icrdat(ncrd) = ibset(icrdat(ncrd),28)
              icrdorn(ncrd) = 0
c
c  Get note name
c
              call getchar(lineq,iccount,charq)
c
c  Get optional inputs
c
34            call getchar(lineq,iccount,durq)
c
c  When chord note is done, will get ' ', making ndx=0, so go past this block
c
              ndx = index('fsn+-<>12345678reicd',durq)
              if (ndx .gt. 0) then
                if (ndx .le. 3) then
                  if (.not.btest(icrdat(ncrd),19)) then
                    icrdat(ncrd) = ibset(icrdat(ncrd),19)
                    icrdat(ncrd) = ior(icrdat(ncrd),ishft(ndx,20))
                  else
                    icrdat(ncrd) = ibset(icrdat(ncrd),22)
                  end if
                else if (durq .eq. 'd') then
c
c Get dot on chord note in xtup. Assume +n+n
c
              icrdat(ncrd) = ibset(icrdat(ncrd),26)
              call getchar(lineq,iccount,durq)
              iccount=iccount+1
              call readnum(lineq,iccount,dumq,fnum)
              if (durq .eq. '+') then
                icrdot(ncrd) = ior(icrdot(ncrd),nint(fnum*10)+64)
              else
                icrdot(ncrd) = ior(icrdot(ncrd),-nint(fnum*10)+64)
              end if
              if (index('+-',dumq) .gt. 0) then
c
c  Vertical shift specified also
c
                iccount=iccount+1
                call readnum(lineq,iccount,durq,fnum)
                if (dumq .eq. '+') then
                  ifnum = nint(fnum*10)+64
                else
                  ifnum = -nint(fnum*10)+64
                end if
              else
                ifnum = 64
              end if
              icrdot(ncrd) = ior(icrdot(ncrd),ishft(ifnum,7))
              iccount = iccount-1
c           iccount = iccount+4
c
c
c
                else if (ndx .eq. 19) then
c
c  Set flags for cautionary accidental
c
                  icrdat(ncrd) = ibset(icrdat(ncrd),31)
                  iornq(ivx,nnl(ivx)-1) = 
     *                       ibset(iornq(ivx,nnl(ivx)-1),31)
                else if (ndx .le. 7) then
c
c +/-/</> Check whether octave or accidental shift
c
                  nactmp = 0
                  call chkpm4ac(lineq,iccount,nactmp,moved)
                  if (moved) then
c
c  Transfer accidental shift values
c
                    call setbits(icrdot(ncrd),6,14,
     *                              igetbits(nactmp,6,4))
                    call setbits(icrdot(ncrd),7,20,
     *                              igetbits(nactmp,7,10))
                  else
                    if (durq .eq. '+') then
                      lastlev = lastlev+7
                    else if (durq .eq. '-') then
                      lastlev = lastlev-7
                    end if
                  end if
                else if (durq .eq. 'e') then
                  icrdat(ncrd) = ibset(icrdat(ncrd),23)
                  irest(ivx,nnl(ivx)) = ibset(irest(ivx,nnl(ivx)),27)
                else if (durq .eq. 'r') then
                  icrdat(ncrd) = ibset(icrdat(ncrd),24)
                  irest(ivx,nnl(ivx)-1) =
     *                      ibset(irest(ivx,nnl(ivx)-1),20)
                else if (durq .eq. 'i') then
c
c  Midi-only accidental
c
                  icrdat(ncrd) = ibset(icrdat(ncrd),27)
                else
c
c must be a number, save it in ioct
c
                  numnum = 1
                  ioct = ndx-7
                end if
                go to 34
              end if
              if (numnum .eq. 1) then
                lastlev = ifnolev(charq,ioct,iTransAmt(instno(iv)))
              else
                lastlev = lastlev-3
     *         +mod(ifnolev(charq,10,iTransAmt(instno(iv)))-lastlev+3,7)
              end if
              icrdat(ncrd) = ior(icrdat(ncrd),ishft(lastlev,12))
              do 41 npreslur = npreslur , 1 , -1
c
c  Set note level for preslur on chord note in xtup
c
                call setbits (isdat2(nsdat-npreslur+1),7,19,lastlev)
c
c  Following lines copied from loop for non-xtup, chord note, preslur
c  Initially I assigned the slur(s) to next note, so fix.
c
                islur(ivx,nnl(ivx)) = ibclr(islur(ivx,nnl(ivx)),0)
                islur(ivx,nnl(ivx)-1) = ibset(islur(ivx,nnl(ivx)-1),0)
                isdat2(nsdat-npreslur+1) = 
     *                               ibset(isdat2(nsdat-npreslur+1),0)
                call setbits(isdat1(nsdat-npreslur+1),8,3,
     *               igetbits(isdat1(nsdat-npreslur+1),8,3)-1)
41            continue
              go to 7
            else if (charq .eq. '?') then
c
c  Arpeggio
c
              if (btest(ipl(ivx,nnl(ivx)-1),10)) then
c
c  This is a chordal note.  Set a bit in icrdat.  But if *main* (spacing) note
c  of chord, will not set icrdat(25), but iornq(27)
c
                icrdat(ncrd) = ibset(icrdat(ncrd),25)
              else
                iornq(ivx,nnl(ivx)-1) = ibset(iornq(ivx,nnl(ivx)-1),27)
              end if
c
c  Check for shift
c
              call getchar(lineq,iccount,durq)
              if (durq .eq. ' ') then
                iccount = iccount-1
              else
c
c  durq must be "-"
c
                iccount = iccount+1
                call readnum(lineq,iccount,durq,fnum)
                iccount = iccount-1
c
c  record the shift
c
                NumArpShift = NumArpShift+1
                IvArpShift(NumArpShift) = ivx
                IpArpShift(NumArpShift) = nnl(ivx)-1
                ArpShift(NumArpShift) = fnum
              end if
              go to 7
            else if (charq .eq. 'D') then
              call getdyn(ivx,nnl(ivx)-1,irest(ivx,nnl(ivx)-1),
     *              iornq(ivx,nnl(ivx)-1),lineq,iccount)
              go to 7
c
c  140215 Allow clef change inside xtuplet. With normal code, came out one
c         note too late, so try making it come earlier.
c
            else if (charq .eq. 'C') then
c
c  Clef change on next note.  Set bits 11-15.  Won't allow in 2nd line of music.
c
              if (nnl(iv)-1 .gt. 0) ncc(iv) = ncc(iv)+1
c
c  140218 Need to get time differently inside xtup, since itsofar doesn't get
c  updated until after xtup is done
c
              nodurt = ifnodur(nnodur,dotq)
              tcc(iv,ncc(iv)) = itsofar(iv)+int(1.*nodurt/ntup*(itup-1))
              isl = ibset(islur(iv,nnl(iv)),11)
              call getchar(lineq,iccount,durq)
c
c  Store clef number, or 7 if clef number = 9 (French violin clef)
c
              isl = ior(isl,ishft(min(numclef(durq),7),12))
              ncmidcc(iv,ncc(iv)) = ncmidf(durq)
              if (durq .eq. '8') then
                ipl(iv,nnl(iv))=ibset(ipl(iv,nnl(iv)),2)
                iTransAmt(instno(iv)) = 7+iTransAmt(instno(iv))
              end if
c
c  Set marker on note with lowest voice # starting at same time.
c
              if (iv .eq. 1) then
                isl = ibset(isl,15)
              else
                do 70 iiv = 1 , iv
c                  nnliiv = nnl(iiv)
                  nnliiv = nnl(iiv)-1
                  if (iiv .eq. iv) nnliiv = nnliiv+1
                  itother = 0
                  do 71 iip = 1 , nnliiv
                    if (itother .lt. itsofar(iv)) then
                      itother = itother+nodur(iiv,iip)
                      go to 71
                    else if (itother .eq. itsofar(iv)) then
                      islur(iiv,iip) = ibset(islur(iiv,iip),15)
                      go to 72
                    end if
71                continue
70              continue
72              continue
              end if
c
c  Need 'or' since may have set bit 15 in the above loop
c
c              islur(iv,nnl(iv)+1) = ior(isl,islur(iv,nnl(iv)+1))
              islur(iv,nnl(iv)) = ior(isl,islur(iv,nnl(iv)))
              go to 7
c+++
            else if (charq .eq. ']') then
c
c  Multiplicity up-down, must have '][ '
c
              islur(ivx,nnl(ivx)-1) = ibset(islur(ivx,nnl(ivx)-1),20)
              iccount = iccount+2
              go to 7
cc+++
            end if
c
c  End of loop for xtup options. If here, charq must be a (non-crd) note name.
c  or rest
c
            if (charq .eq. 'r') then
c
c  Rest in xtup
c
              irest(ivx,nnl(ivx)) = ibset(irest(ivx,nnl(ivx)),0)
              if (index('+-b',lineq(iccount+1:iccount+1)) .gt. 0) then
                call getchar(lineq,iccount,durq)
                if (durq .eq. 'b') then
c
c  Blank rest in middle of xtup
c
                  islur(ivx,nnl(ivx)) = ibset(islur(ivx,nnl(ivx)),29)
                else
c
c  Override height of embedded xtup rest
c
                  ipm = index('- +',durq)-2
                  iccount = iccount+1
                  call readnum(lineq,iccount,durq,fnum)
                  nolev(ivx,nnl(ivx)) = 100+ipm*nint(fnum)
                  iccount = iccount-1
                end if 
              else if (ivx .le. nv) then
                nolev(ivx,nnl(ivx)) = 0
              else
                nolev(ivx,nnl(ivx)) = 2
              end if
            else
c
c Counter for non-rests
c
              nnb = nnb+1
            end if
            notcrd = .true.
8           call getchar(lineq,iccount,durq)
            if (durq .ne. ' ') then
              if (index('+-<>',durq) .gt. 0) then
c
c  Accidental horizontal shift
c
                call chkpm4ac(lineq,iccount,nacc(ivx,nnl(ivx)),moved)
                if (.not.moved) then
                  if (durq.eq.'+') then
                    lastlev = lastlev+7
                  else if (durq.eq.'-') then
                    lastlev = lastlev-7
                  end if
                end if
              else if (index('fsn',durq) .gt. 0) then
c
c                if (nacc(ivx,nnl(ivx)) .eq. 0) then
c  May have set other bits earlier 
                if (igetbits(nacc(ivx,nnl(ivx)),2,0) .eq. 0) then
c
c  No accid set yet
c
c                  nacc(ivx,nnl(ivx)) = index('fsn',durq)
                  nacc(ivx,nnl(ivx)) = 
     *                  ior(nacc(ivx,nnl(ivx)),index('fsn',durq))
                else
c
c  Symbol must be repeated, so it's a double
c
                  nacc(ivx,nnl(ivx)) = ibset(nacc(ivx,nnl(ivx)),2)
                end if
              else if (durq .eq. 'i') then
c
c  Set flag for midi-only accidental
c
                nacc(ivx,nnl(ivx)) = ibset(nacc(ivx,nnl(ivx)),17) 

              else if (durq .eq. 'c') then
c
c  Set flags for cautionary accidental
c
                irest(ivx,nnl(ivx)) = ibset(irest(ivx,nnl(ivx)),31)
                iornq(ivx,nnl(ivx)) = ibset(iornq(ivx,nnl(ivx)),31)
              else if (index('ul',durq) .gt. 0) then
c
c  Force stem direction for non-beamed xtup note
c
                islur(ivx,nnl(ivx)) = ibset(islur(ivx,nnl(ivx)),30)
                if (durq .eq. 'u') islur(ivx,nnl(ivx)) =
     *                ibset(islur(ivx,nnl(ivx)),17)
              else if (durq .eq. 'e') then
c
c  Left-shift main xtup note
c
                ipl(ivx,nnl(ivx)) = ibset(ipl(ivx,nnl(ivx)),8)
                irest(ivx,nnl(ivx)) = ibset(irest(ivx,nnl(ivx)),27)
              else if (durq .eq. 'r') then
c
c  Right-shift main xtup note
c
                ipl(ivx,nnl(ivx)) = ibset(ipl(ivx,nnl(ivx)),9)
                irest(ivx,nnl(ivx)) = ibset(irest(ivx,nnl(ivx)),20)
              else if (index('DF',durq) .gt. 0) then
c
c  Double an xtup note to make an unequal xtup
c
                nacc(ivx,nnl(ivx)) = ibset(nacc(ivx,nnl(ivx)),18)
                if (durq .eq. 'F') nacc(ivx,nnl(ivx)) = 
     *                  ibset(nacc(ivx,nnl(ivx)),19)
                ndoub = ndoub+1
              else if (durq .eq. 'd') then
c
c  Dotted xtup note
c
c                if (btest(irest(ivx,nnl(ivx))-1,2)) then
                if (btest(irest(ivx,nnl(ivx)-1),2)) then
c
c Move dot on 2nd note of 2-note tremolo
c
                  ndotmv(ivx) = ndotmv(ivx)+1
                  irest(ivx,nnl(ivx)) = ibset(irest(ivx,nnl(ivx)),19)
                  call g1etchar(lineq,iccount,durq) ! Will be + or -
                  iccount = iccount+1
                  call readnum(lineq,iccount,dumq,
     *                          updot(ivx,ndotmv(ivx)))
                  if (durq .eq. '-') updot(ivx,ndotmv(ivx)) =
     *                                -updot(ivx,ndotmv(ivx))
                  if (index('+-',dumq) .gt. 0) then
c
c  Vertical shift also
c
                    call readnum(lineq,iccount,durq,
     *                             rtdot(ivx,ndotmv(ivx)))
                    if (durq .eq. '-') rtdot(ivx,ndotmv(ivx))
     *                               = -rtdot(ivx,ndotmv(ivx))
                  end if
                  iccount = iccount-1
                else
c
c Dot for 3:1 pair of xtup notes
c
                  nacc(ivx,nnl(ivx)) = ibset(nacc(ivx,nnl(ivx)),27)
                end if
              else
c
c  Must be an octave number
c
                lastlev = ifnolev(charq,ichar(durq)-48
     *                          ,iTransAmt(instno(iv)))
              end if
              go to 8
            end if
            if (itup .lt. ntup) then
c
c  Last note is handled *after* flowing out of the xtup if block, but still
c    within block for a note-rest.  Set note level now (rest already done).
c    Could have problem here if rests & doubled notes are combined in xtup,
c    since might exit the loop at the wrong place.  Worry about it later.
c
              if (.not.btest(irest(ivx,nnl(ivx)),0)) then
                lastlev = lastlev-3+mod(ifnolev(charq,10,
     *                      iTransAmt(instno(iv)))-lastlev+3,7)
                nolev(ivx,nnl(ivx)) = lastlev
              end if
              nodur(ivx,nnl(ivx)) = 0
              do 42 npreslur = npreslur , 1 , -1
c
c  Set note level for preslur on internal xtup note
c
                call setbits (isdat2(nsdat-npreslur+1),7,19,lastlev)
42            continue
            end if
            if (itup .eq. ntup-ndoub) go to 12
6         continue
12        continue
          if (ornrpt) then
            iornq(ivx,nnl(ivx)) = ior(iornq(ivx,nnl(ivx)),
     *           iand(iornq(ivx,nnl(ivx)-1),10026991))
            if (iand(iornq(ivx,nnl(ivx)),32896) .gt. 0) then
c
c  This is a trill (bit 7 or 15) so must dup the parameters
c
              ntrill = ntrill + 1
              ivtrill(ntrill) = ivx
              iptrill(ntrill) = nnl(ivx)
              xnsktr(ntrill) = xnsktr(ntrill-1)
            end if
          end if
c
c  End of if-block for xtuplet input
c
        else if (durq .eq. 'm') then
c
c  Multi-bar rest: next 1 or two digits are # of bars.
c  For some purposes, pretend its one bar only
c
          nodur(ivx,nnl(ivx)) = lenbar
          ibarmbr = nbars+1
          mbrest = 0
          xb4mbr = 0.
20        call getchar(lineq,iccount,durq)
          if (ichar(durq).ge.48.and.ichar(durq).le.57) then
            mbrest = 10*mbrest+ichar(durq)-48
            go to 20
          end if
c
c  durq will either be blank or 'n'
          if (durq .eq. 'n') then
c
c  Get new height
c
            iplmi = 1
            call g1etchar(lineq,iccount,durq)
            if (durq .eq. '-') then
              iplmi = -1
              iccount = iccount+1
            end if
            call readnum(lineq,iccount,durq,hgt)
            mbrhgt = nint(iplmi*hgt)
            newmbrhgt = .true.
          end if
        else if (index('ul',durq) .gt. 0) then
c
c  Set stem flipper
c
          islur(ivx,nnl(ivx)) = ibset(islur(ivx,nnl(ivx)),30)
          if (durq .eq. 'u') islur(ivx,nnl(ivx)) =
     *                ibset(islur(ivx,nnl(ivx)),17)
          go to 2
        else if (durq .eq. 'a') then
c
c  "Alone", i.e., prohibit beam
c
          islur(ivx,nnl(ivx)) = ibset(islur(ivx,nnl(ivx)),18)
          go to 2
        else if (durq .eq. 'r') then
c
c  Right offset by one notehead
c
          ipl(ivx,nnl(ivx)) = ibset(ipl(ivx,nnl(ivx)),9)
          irest(ivx,nnl(ivx)) = ibset(irest(ivx,nnl(ivx)),20)
          go to 2
        else if (durq .eq. 'e') then
c
c  Left offset by one notehead
c
          ipl(ivx,nnl(ivx)) = ibset(ipl(ivx,nnl(ivx)),8)
          irest(ivx,nnl(ivx)) = ibset(irest(ivx,nnl(ivx)),27)
          go to 2
        else if (index('LS',durq) .gt. 0) then
c
c  Stemlength change.  Get dstemlen in \internotes.  Allowable values are -4 to 27.5
c    Set mult(27).  Map value to 0 to 63, store in mult(10-15).  Later convert to
c    interbeams = internotes*2/3.
c
c
          isign = 1
          if (durq .eq. 'S') isign = -1
          mult(ivx,nnl(ivx)) = ibset(mult(ivx,nnl(ivx)),27)
          call getchar(lineq,iccount,durq)
          if (durq .eq. ':') then
c
c  End stickyS.  Grab data now from prior note, since we have to shut off stickyS.
c
            call setbits (mult(ivx,nnl(ivx)),6,10,
     *                    igetbits(mult(ivx,nnl(ivx)-1),6,10))
            stickyS = .false.  
            go to 2
          end if
c
c  If durq .ne. ':' then iccount is now on the start of the number
c
          call readnum(lineq,iccount,durq,dum)
          dum = isign*dum
          call setbits (mult(ivx,nnl(ivx)),6,10,nint((dum+4.)*2))
          if (durq .eq. ':') then
            stickyS = .true.
          else
            iccount = iccount-1
          end if
          go to 2                    
        else if (durq .eq. ',') then
c
c  2:1 pattern
c
          idotform = 3
c
c  Now flow to duration setting, as if durq=' '
c
        else if (durq .eq. '.') then
c
c  Dotted pattern.  Close out note.  Mult time by 3/4.
c  Set time for next note to 1/4.  Start the note.
c
          idotform = 1
        else if (durq .eq. 'o') then
c
c  Suppress rest centering
c
          irest(ivx,nnl(ivx)) = ibset(irest(ivx,nnl(ivx)),25)
          go to 2
        else if (durq .eq.'L') then
c
c  With keyboard rest option, look left
c
c          iornq(ivx,nnl(ivx)) = ibset(iornq(ivx,nnl(ivx)),30)
          ipl(ivx,nnl(ivx)) = ibset(ipl(ivx,nnl(ivx)),1)
          go to 2
        else if (index('DF',durq) .gt. 0) then
c
c  Double note for xtup.  Must check here in case "D" came before "x" or on
c  last note of xtup.   Need to flag it in pmxa since affects horiz. spacing.
c
          nacc(ivx,nnl(ivx)) = ibset(nacc(ivx,nnl(ivx)),18)
          if (durq .eq. 'F') nacc(ivx,nnl(ivx)) = 
     *                  ibset(nacc(ivx,nnl(ivx)),19)
          go to 2
        else if (durq .eq. 'A') then
c
c  Accidental option
c
          call getchar(lineq,iccount,durq)
c
          if (durq .eq. 'o') then
c
c  Ordered accidentals in a chord.  Mark the main note.
c
            nacc(ivx,nnl(ivx)) = ibset(nacc(ivx,nnl(ivx)),28)
          else
c
c  Only other possibility is +-<> . Set tag, reduce iccount and loop to get #'s
c
            nacc(ivx,nnl(ivx)) = ibset(nacc(ivx,nnl(ivx)),29)
            iccount = iccount-1
          end if            
          go to 2
        else if (durq .eq. 'T') then
          call getchar(lineq,iccount,durq)
          multtrem = index('1234',durq)
          if (multtrem .eq. 0) then
            iccount = iccount-1
            multtrem = 1
          end if
          ipl(ivx,nnl(ivx)) = ibset(ipl(ivx,nnl(ivx)),4)
          call setbits(ipl(ivx,nnl(ivx)),2,5,multtrem-1)
          go to 2
        else if (durq .ne. ' ') then
          print*,'Illegal character in note: ',durq,', ivx,nn:',
     *        ivx,nnl(ivx)
          call stop1()
        end if
c
c  Done with note/rest options.  Set level and duration.
c
        if (.not.btest(irest(ivx,nnl(ivx)),0)) then
          if (numnum .eq. 2) then
            lastlev = ifnolev(charq,ioct,iTransAmt(instno(iv)))
            nolev(ivx,nnl(ivx)) = lastlev
          else
            lastlev = lastlev-3
     *         +mod(ifnolev(charq,10,iTransAmt(instno(iv)))-lastlev+3,7)
            nolev(ivx,nnl(ivx)) = lastlev
          end if
          do 43 npreslur = npreslur , 1 , -1
c
c  Set level for preslur on normal note, non-chord
c
            call setbits(isdat2(nsdat-npreslur+1),7,19,lastlev)
43        continue
c
c  Only matters if last note, non-rest of xtuplet
c
          nnb = nnb+1
        end if
c
c  Set flag for unbeamed xtup if it just has 1 note
c
        if (inxtup .and. nnb.eq.1) then
          islur(ivx,note1xtup) = ibset(islur(ivx,note1xtup),18)
        end if
        if (idotform .gt. 0) then
          if (idotform .eq. 1) then
            nodur(ivx,nnl(ivx)) = ifnodur(nnodur,dotq)*3/2
          else if (idotform .eq. 2) then
            nodur(ivx,nnl(ivx)) = nodur(ivx,nnl(ivx)-1)/3
          else if (idotform .eq. 3) then
            nodur(ivx,nnl(ivx)) = ifnodur(nnodur,dotq)
          else if (idotform .eq. 4) then
            nodur(ivx,nnl(ivx)) = nodur(ivx,nnl(ivx)-1)/2
          end if
        else if (btest(islur(ivx,nnl(ivx)),19)) then
c
c  Set duration of full-bar rest as pause
c
          nodur(ivx,nnl(ivx)) = lenbar
c
c  Use a one-line function to set nnodur.  It gives inverse of ifnodur.
c
          nnodur = index('62514x0x37',
     *                   chax(48+int(log(.1+lenbar)/.69315)))-1
        else if (ibarmbr.ne.nbars+1) then
          nodur(ivx,nnl(ivx)) = ifnodur(nnodur,dotq)
          if (btest(islur(ivx,nnl(ivx)),3))
     *      nodur(ivx,nnl(ivx)) = nodur(ivx,nnl(ivx))*7/6
        end if
        if (shifton .and. .not.btest(irest(ivx,nnl(ivx)),16)) then
c
c  Shift is on, and this is not first shifted note.  Check for duration change
c
          if (nodur(ivx,nnl(ivx)) .ne. nodur(ivx,nnl(ivx)-1)) then
c
c  Must stop and restart the offset.
c
            irest(ivx,nnl(ivx)-1) = ibset(irest(ivx,nnl(ivx)-1),17)
            irest(ivx,nnl(ivx)) = ibset(irest(ivx,nnl(ivx)),16)
            nudoff(ivx) = nudoff(ivx)+1
            udoff(ivx,nudoff(ivx)) = udoff(ivx,nudoff(ivx)-1)
          end if
        end if
        itsofar(ivx) = itsofar(ivx)+nodur(ivx,nnl(ivx))
        if (autofbon .and. tautofb.gt.tol .and. fbon) then
c
c  Check to see if need to terminate auto forced beam
c
          if (mod(itsofar(ivx)-t1autofb,tautofb) .lt. tol) then
c
c  Terminate autofb
c
            t2fb(ivx,nfb(ivx)) = itsofar(ivx)
            fbon = .false.
          end if
        end if
        if (mod(itsofar(ivx)-lenb0,lenbar) .eq. 0) then
c
c  Finished a bar
c
          nbars = nbars+1
          nib(ivx,nbars) = nnl(ivx)
          if (firstgulp.and.lenb0.ne.0.and.nbars.eq.1) then
c
c  Just finished the pickup bar for this voice.
c
            lenbar = lenb1
          end if
        end if
        if (idotform .eq. 1) then
          call getchar(lineq,iccount,charq)
          idotform = 2
          numnum = 1
          go to 28
        else if (idotform .eq. 3) then
          call getchar(lineq,iccount,charq)
          idotform = 4
          numnum = 1
          go to 28
        end if
c
c  End of sub block for note-rest
c
      else if (charq .eq. 'z') then
c
c  Chord note.  Must have note name, may have octave#,+,-,s,f,n,d
c  Actually the 'd' is not used, since time value comes from
c    basic note. Unless dot is to be shifted!
c  Doesn't increase # of notes, so must handle separately
c  ncrd: index of crd
c  Set bit 10 of ipl on main note as flag
c  Bits in icrdat:
c     0-7   ip within voice
c     8-11  ivx
c     12-18 note level
c     19    accidental?
c     20-22 accidental value (1=natural, 2=flat, 3=sharp, 6=dflat, 7=dsharp)
c     23    shift left
c     24    shift right
c     25    arpeggio start or stop
c     26    flag for moved dot (here, not icrdot, since this is always reset!)
c     27    Midi-only accidental
c     29    Tag for accidental shift...means add to autoshifts.
c     31    Cautionary accidental 
c
c  Bits in icrdot:
c     0-6   10*abs(vertical dot shift in \internote) + 64
c     7-13  10*abs(horizontal dot shift in \internote) + 64
c     14-19 vert accidental shift-32
c     20-26 20*(horiz accidental shift+3.2)
c     27-29 top-down level rank of chord note w/accid. Set in crdaccs.
c
c  Bits in icrdorn are same as in iornq, even tho most orns won't go in crds.
c
        ncrd = ncrd+1
        ipl(ivx,nnl(ivx)) = ibset(ipl(ivx,nnl(ivx)),10)
        numnum = 0
c        icrdat(ncrd) = ior(nnl(ivx)-1,ishft(ivx,8))
        icrdat(ncrd) = nnl(ivx)
	  call setbits(icrdat(ncrd),4,8,mod(ivx,16))
        if (ivx .ge. 16) icrdat(ncrd) = ibset(icrdat(ncrd),28)
        icrdot(ncrd) = 0
        icrdorn(ncrd) = 0
c
c  Get note name
c
        call getchar(lineq,iccount,charq)
c
c  Get optional inputs
c
25      call getchar(lineq,iccount,durq)
c        ndx = index('fsn+-<>12345678rediA',durq)
        ndx = index('fsn+-<>12345678rediAc',durq)
        if (ndx .eq. 20) then
c
c  Expect +|-|<|> , set tag, loop
c
          icrdat(ncrd) = ibset(icrdat(ncrd),29)
          go to 25
	  else if (ndx .gt. 0) then
          if (ndx .le. 3) then
            if (.not.btest(icrdat(ncrd),19)) then
              icrdat(ncrd) = ibset(icrdat(ncrd),19)
              icrdat(ncrd) = ior(icrdat(ncrd),ishft(ndx,20))
            else
              icrdat(ncrd) = ibset(icrdat(ncrd),22)
            end if
          else if (ndx .eq. 21) then
c
c  Set flags for cautionary accidental
c
            icrdat(ncrd) = ibset(icrdat(ncrd),31)
            iornq(ivx,nnl(ivx)) = ibset(iornq(ivx,nnl(ivx)),31)
          else if (ndx .le. 7) then
c
c +/-/</> Check whether octave or accidental shift
c
            nactmp = 0
            call chkpm4ac(lineq,iccount,nactmp,moved)
            if (moved) then
c
c  Transfer accidental shift values
c
              call setbits(icrdot(ncrd),6,14,igetbits(nactmp,6,4))
              call setbits(icrdot(ncrd),7,20,igetbits(nactmp,7,10))
            else
              if (durq .eq. '+') then
                lastlev = lastlev+7
              else if (durq .eq. '-') then
                lastlev = lastlev-7
              end if
            end if
          else if (durq .eq. 'e') then
            icrdat(ncrd) = ibset(icrdat(ncrd),23)
            irest(ivx,nnl(ivx)) = ibset(irest(ivx,nnl(ivx)),27)
          else if (durq .eq. 'r') then
            icrdat(ncrd) = ibset(icrdat(ncrd),24)
            irest(ivx,nnl(ivx)) = ibset(irest(ivx,nnl(ivx)),20)
          else if (durq .eq. 'i') then
c
c  Midi-only accidental on chord note
c
            icrdat(ncrd) = ibset(icrdat(ncrd),27)
          else if (durq .eq. 'd') then
c
c  Must keep 'd' optional (backward compatibility), unless it is moved!
c
            if (index('+-',lineq(iccount+1:iccount+1)) .gt. 0) then
c
c  move a dot, unless next char is not part of a number
c
              if (index('0123456789.',lineq(iccount+2:iccount+2))
     *              .eq. 0) go to 25
              icrdat(ncrd) = ibset(icrdat(ncrd),26)
              call getchar(lineq,iccount,durq)
              iccount=iccount+1
              call readnum(lineq,iccount,dumq,fnum)
              if (durq .eq. '+') then
                icrdot(ncrd) = ior(icrdot(ncrd),nint(fnum*10)+64)
              else
                icrdot(ncrd) = ior(icrdot(ncrd),-nint(fnum*10)+64)
              end if
              if (index('+-',dumq) .gt. 0) then
c
c  Vertical shift specified also
c
                iccount=iccount+1
                call readnum(lineq,iccount,durq,fnum)
                if (dumq .eq. '+') then
                  ifnum = nint(fnum*10)+64
                else
                  ifnum = -nint(fnum*10)+64
                end if
              else
                ifnum = 64
              end if
              icrdot(ncrd) = ior(icrdot(ncrd),ishft(ifnum,7))
              iccount = iccount-1
            end if
          else
c
c must be a single digit, save it in ioct
c
            numnum = 1
            ioct = ndx-7
          end if
          go to 25
        end if
        if (numnum .eq. 1) then
          lastlev = ifnolev(charq,ioct,iTransAmt(instno(iv)))
        else
          lastlev = lastlev-3
     *         +mod(ifnolev(charq,10,iTransAmt(instno(iv)))-lastlev+3,7)
        end if
        icrdat(ncrd) = ior(icrdat(ncrd),ishft(lastlev,12))
        do 44 npreslur = npreslur , 1 , -1
          call setbits (isdat2(nsdat-npreslur+1),7,19,lastlev)
c
c  Set level for chord note.
c  Initially I assigned the slur(s) to next note, so fix.
c
          islur(ivx,nnl(ivx)+1) = ibclr(islur(ivx,nnl(ivx)+1),0)
          islur(ivx,nnl(ivx)) = ibset(islur(ivx,nnl(ivx)),0)
          isdat2(nsdat-npreslur+1) = ibset(isdat2(nsdat-npreslur+1),0)
          call setbits(isdat1(nsdat-npreslur+1),8,3,
     *               igetbits(isdat1(nsdat-npreslur+1),8,3)-1)
44      continue
        if (notcrd) then
c
c  This is the first chord note in this chord.
c
          minlev = min(nolev(ivx,nnl(ivx)),lastlev)
          maxlev = max(nolev(ivx,nnl(ivx)),lastlev)
        else
          minlev = min(minlev,lastlev)
          maxlev = max(maxlev,lastlev)
        end if
        notcrd = .false.
      else if (charq .eq. 'G') then
        call getgrace(ivx,nnl,lineq,iccount,islur,iornq,ipl,ndlev,
     *                lastlev,iv,nv)
c
c Grace, comes *before* main note:
c UNLESS there's an 'A' or 'W' after the 'G'
c   ngrace = # of grace note groups so far in block
c   ivg(ngrace), ipg(ngrace)
c   nng(ngrace) = # of notes in this group: default = 1
c   ngstrt(ngrace) = starting position in nolevg of levels for this grace
c   multg(ngrace) = multiplicity: default = 1;  input as 'm(digit)'
c   upg(ngrace) = logical for beam or stem dirn: default T, input'u,l'
c   slurg(ngrace) = logical for slur; default F, input 's'
c   slashg(ngrace) = T if slash; default is F, input 'x'
c These data MUST precede note name of first note
c   nolevg, naccg: lists of levels and accid's, indexed as described above.
c
      else if (charq .eq. sq) then
c
c  Literal TeX string
c
        call littex(islur,nnl(ivx)+1,ivx,topmods,lineq,iccount)
      else if (charq .eq. '"') then
c
c  pmxlyr string. Expand "..." to \pmxlyr{...}\
c
        if (.not. inputmlyr) then
          ictemp = 0
          lineqt = sq//sq//sq//'input musixlyr '//sq
          call littex(islur,nnl(ivx)+1,ivx,topmods,lineqt,ictemp)
          inputmlyr = .true.
        end if
        call dopmxlyr(lineq,iccount)
        charq = sq
        call littex(islur,nnl(ivx)+1,ivx,topmods,lineq,iccount)
      else if (charq .eq. 'o') then
c
c  Ornament on non-xtup note.  Symbol must come AFTER the affected note
c
        if (notcrd) then
          nole = nolev(ivx,nnl(ivx))
        else
          nole = iand(127,ishft(icrdat(ncrd),-12))
        end if
        call getorn(lineq,iccount,iornq(ivx,nnl(ivx)),
     *       iornq(ivx,0),ornrpt,noffseg,nnl(ivx),ivx,
     *      .true.,notcrd,nole)
      else if (index('st(){}',charq) .gt. 0) then
        nnlivx = nnl(ivx)
        if (charq.eq.'(' .or. charq.eq.'{') then
c
c Detect preslur on normal non-chord note
c
          nnlivx = nnlivx+1
          npreslur = npreslur+1
        end if
        islur(ivx,nnlivx) = ibset(islur(ivx,nnlivx),0)
        if (charq.eq.'t')
     *    islur(ivx,nnlivx) = ibset(islur(ivx,nnlivx),1)
        if (ivx .le. nv) then
          kv = 1
        else
          kv = 2
        end if
        if (fontslur) then
          call sslur(lineq,iccount,iv,kv,nnlivx,isdat1,isdat2,isdat3,
     *             nsdat,notcrd,nolev(ivx,nnlivx),charq)
        else
          call spsslur(lineq,iccount,iv,kv,nnlivx,isdat1,isdat2,isdat3,
     *             isdat4,nsdat,notcrd,nolev(ivx,nnlivx),charq)
        end if
      else if (charq .eq. '?') then
c
c  Arpeggio
c
        if (btest(ipl(ivx,nnl(ivx)),10)) then
c
c  This is a chordal note.  Set a bit in icrdat.  But if *main* (spacing) note
c  of chord, will not set icrdat(25), but iornq(27)
c
          icrdat(ncrd) = ibset(icrdat(ncrd),25)
        else
          iornq(ivx,nnl(ivx)) = ibset(iornq(ivx,nnl(ivx)),27)
        end if
c
c  Check for shift
c
        call getchar(lineq,iccount,durq)
        if (durq .eq. ' ') then
          iccount = iccount-1
        else
c
c  durq must be "-"
c
          iccount = iccount+1
          call readnum(lineq,iccount,durq,fnum)
          iccount = iccount-1
c
c  record the shift
c
          NumArpShift = NumArpShift+1
          IvArpShift(NumArpShift) = ivx
          IpArpShift(NumArpShift) = nnl(ivx)
          ArpShift(NumArpShift) = fnum
        end if
      else if (index('0123456789#-nx_',charq) .gt. 0) then
c
c  We have a figure.  Must come AFTER the note it goes under
c

        ivf = 1
        if (ivx .gt. 1) then
          if (ivxfig2 .eq. 0) then
            ivxfig2 = ivx
          else if (ivx .ne. ivxfig2) then
            print*
            print*,'Figures not allowed in >1 voice above first'
            stop
          end if
          ivf = 2
        end if
        nfig1 = nfigs(ivf)+1
        call getfig(itoff(ivf,nfig1),charq,lineq,iccount,
     *             isfig(ivf,nnl(ivx)),itfig(ivf,nfig1),itsofar(ivx),
     *             nodur(ivx,nnl(ivx)),figq(ivf,nfig1),
c     *             ivupfig(ivf,nfig1),nfigs(ivf))
     *             ivupfig(ivf,nfig1),ivvfig(ivf,nfig1),nfigs(ivf))
      else if (charq .eq. '[') then
c
c  Start forced beam.  Record barno & time since start of inp. blk.  Set signal
c
        nfb(ivx) = nfb(ivx)+1
        fbon = .true.
        ulfbq(ivx,nfb(ivx)) = 'x'
        t1fb(ivx,nfb(ivx)) = itsofar(ivx)
        nadj = 0
        if (autofbon) then
          autofbon = .false.
        end if
17      call getchar(lineq,iccount,charq)
        if (index('ulf',charq) .gt. 0) then
          ulfbq(ivx,nfb(ivx)) = charq
          go to 17
        else if (charq .eq. 'j') then
c
c  Continuing a jumped beam here
c
          irest(ivx,nnl(ivx)+1) = ibset(irest(ivx,nnl(ivx)+1),24)
c
c  Set flag to watch for END of this forced beam, so can set flag rest(30) on
c  NEXT note as signal to start a new notes group there.
c
          isbj2 = .true.
          go to 17
        else if (charq .eq. 'h') then
          islur(ivx,nnl(ivx)+1) = ibset(islur(ivx,nnl(ivx)+1),2)
          go to 17
        else if (charq .eq. 'm') then
c
c  Force multiplicity.  Next input is digit
c
          call getchar(lineq,iccount,charq)
          islur(ivx,nnl(ivx)+1) = ibset(islur(ivx,nnl(ivx)+1),21)
          call setbits(islur(ivx,nnl(ivx)+1),3,22,ichar(charq)-48)
          go to 17
        else if (charq .eq. ':') then
c
c  Start auto forced beam pattern
c
          autofbon = .true.
c
c  When forced later beam ends, check whether tautofv <=0; if so set it.
c
          tautofb = -itsofar(ivx)
          t1autofb = itsofar(ivx)
          go to 17
        else if (charq .ne. ' ') then
c
c  Must be '+/-' for height or slope shift
c
          nadj = nadj+1
c
c  nadj = 1,2, or 3 for normal start level, slope, or beam-thk start level.
c
          iccount = iccount+1
          call readnum(lineq,iccount,durq,fnum)
          iccount = iccount-1
          iadj = nint(fnum)
          if (charq .eq. '-') iadj = -iadj
          if (nadj .eq. 1) then
c
c  This is a level shift.  Note if 0 was entered, iadj = 30
c
            call setbits(ipl(ivx,nnl(ivx)+1),6,11,iadj+30)
          else if (nadj .eq. 2) then
c
c  Must be a slope shift
c
            call setbits(ipl(ivx,nnl(ivx)+1),6,17,iadj+30)
          else
c
c  Beam-thk fine tune
c
            call setbits(islur(ivx,nnl(ivx)+1),2,27,iadj)
          end if
          go to 17
        end if
      else if (charq .eq. ']') then
        if (autofbon .and. tautofb.lt.tol) then
          tautofb=itsofar(ivx)+tautofb
        end if
        call getchar(lineq,iccount,charq)
        if (index('j ',charq) .gt. 0) then
c
c  Since ']' comes AFTER note, itsofar has been updated.  Set ending signal.
c
          t2fb(ivx,nfb(ivx)) = itsofar(ivx)
          fbon = .false.
          if (charq .eq. 'j') then 
            irest(ivx,nnl(ivx)) = ibset(irest(ivx,nnl(ivx)),23)
          end if
          if (isbj2) then
c
c  This is the end of a fb segment of a jump beam.  Set flag on NEXT note to
c  force start of new notes group, provided this is not last note in bar.
c
            if (mod(itsofar(ivx),lenbar) .ne. 0) 
     *         irest(ivx,nnl(ivx)+1) = ibset(irest(ivx,nnl(ivx)+1),30)
            isbj2 = .false.
          end if
        else if (charq .eq. '[') then
c
c  Multiplicity down-up signal
c
          islur(ivx,nnl(ivx)) = ibset(islur(ivx,nnl(ivx)),20)
        else if (charq .eq. '-') then
c
c  Set signals for gap in single-slope beam [...]-[...]
c
          nacc(ivx,nnl(ivx)) = ibset(nacc(ivx,nnl(ivx)),20)
          nacc(ivx,nnl(ivx)+1) = ibset(nacc(ivx,nnl(ivx)+1),21)
c
c  Next two characters must be "[ ".  Skip over them.
c
          iccount = iccount+2
        end if
      else if (charq .eq. 'D') then
        call getdyn(ivx,nnl(ivx),irest(ivx,nnl(ivx)),
     *              iornq(ivx,nnl(ivx)),lineq,iccount)
      else if (charq .eq. 'h') then
c
c  Heading or height.  For heading, only OK if at start of block
c  Check whether at beginning of a block
c
        if (iv.ne.1 .or. nnl(1).ne.0) then
          print*,'You entered "h" not at beginning of block'
          call stop1()
        end if
        call getchar(lineq,iccount,durq)
        ihdvrt = 0
        if (index('+-',durq) .gt. 0) then
c
c Vertical offset
c
          iccount = iccount+1
          call readnum(lineq,iccount,charq,fnum)
          ihdvrt = fnum+.1
          if (durq .eq. '-') ihdvrt = -ihdvrt
          durq = charq
        end if
        if (durq .ne. ' ') then
c
c  Height symbol.  Read past (until next blank)
c
3         call getchar(lineq,iccount,durq)
          if (durq .ne. ' ') go to 3
        else
c
c  Set flag for header & read it in
c
          ihdht = 16
          call getbuf(headrq)
          iccount = 128
        end if
      else if (charq .eq. 'L') then
c
c  Linebreak, already handled some in pmxa, but need to get data for 
c    shortened line or coda lengths 
c
        iccount = iccount+1
        if (lineq(iccount:iccount) .eq. 'C') then
c
c  Coda, get length
c
          iccount = iccount+1
          call readnum(lineq,iccount,durq,codafrac)
          ishort = 3
          if (durq .eq. 'n') nocodabn = .true.
          return
        end if
c
c  Get line number
c
        call readnum(lineq,iccount,durq,fmovbrk)
48      continue  ! Allow transfer up from below if S follows M
        if (durq .eq. 'S') then
c
c  Shortened line, get shortening fraction
c
          iccount = iccount+1
          call readnum(lineq,iccount,durq,shortfrac)
          ishort = 1
          codafrac = 0.
        end if
c
c  Begin check for movement break
c
        if (durq .eq. 'P') then
          iccount = iccount+1
          call readnum(lineq,iccount,durq,fnum)
        end if
        if (durq .eq. 'M') then
          movbrk = nint(fmovbrk)
          movgap = 0
          parmov = -1.
          call getchar(lineq,iccount,durq)
31        if (durq .eq. '+') then
c
c  Get vertical space (\internotes)
c
            iccount = iccount+1
            call readnum(lineq,iccount,durq,fnum)
            movgap = nint(fnum)
            go to 31
          else if (durq .eq. 'i') then
            iccount = iccount+1
            call readnum(lineq,iccount,durq,parmov)
            go to 31
          else if (durq .eq. 'c') then
            nobar1 = .true.
            call getchar(lineq,iccount,durq)
            go to 31
          else if (durq .eq. 'r') then
c
c  "rename" can be set on or off.
c
            call getchar(lineq,iccount,durq)
            rename = durq.eq.'+'
            call getchar(lineq,iccount,durq)
            go to 31
          else if (durq .eq. 'n') then
c
c  Change # of voices.  Input ninow, iiorig(1...ninow).  Will use names,
c  staves per inst. and clefs  corr. to iiorig in original list of instruments.
c
            nvold = nv
            nv = 0
            rename = .true.
            call getchar(lineq,iccount,durq)
            if (durq .eq. ':') then
              iccount = iccount+2
              read(lineq(iccount-1:iccount),'(i2)')ninow
            else
              ninow = ichar(durq)-48
            end if
            iiv = 0
            do 63 iinow = 1 , ninow
              call getchar(lineq,iccount,durq)
              if (durq .eq. ':') then
                iccount = iccount+2
                read(lineq(iccount-1:iccount),'(i2)')iiorig(iinow)
              else
                iiorig(iinow) = ichar(durq)-48
              end if
              nspern(iinow) = nsperi(iiorig(iinow))
              nv = nv+nspern(iinow)
              sepsymq(iiv+nspern(iinow)) = '&'
              if (nspern(iinow) .gt. 1) then
                do 64 iis = 1 , nspern(iinow)-1
                  sepsymq(iiv+iis) = '|'
64              continue
              end if
              iiv = iiv+nspern(iinow)
63          continue
c
c 120818 Per Rainer's suggestion, defer changing \nbinstruments until issuing \newmovement
c
c            if (islast) then
c              if (ninow .lt. 10) then
c                write(11,'(a)')sq//'newnoi{'//chax(ninow+48)//'}%'
c              else
c                write(11,'(a8,i2,a2)')sq//'newnoi{',ninow,'}%'
c              end if
c            end if
            if (nv.eq.1 .and. nvold.gt.1) then
              if (islast) write(11,'(a)')sq//'nostartrule'
            else if (nv.gt.1 .and. nvold.eq.1) then
              if (islast) write(11,'(a)')sq//'startrule'
            end if
            iiv = 0
            do 60 iinow = 1 , ninow
              do 61 iis = 1 , nspern(iinow)
                iiv = iiv+1
c
c  May not really need to re-enter clefs, but it's easier to program since
c  clef names are not saved but are needed in newvoice to set ncmidcc.
c
                call getchar(lineq,iccount,clefq(iiv))
                call newvoice(iiv,clefq(iiv),.true.)
                if (nspern(iinow) .eq. 1) then
                  hdlndq = chax(48+numclef(clefq(iiv)))
                  lclf = 1
                else if (iis .eq. 1) then
                  hdlndq = '{'//chax(48+numclef(clefq(iiv)))
                  lclf = 2
                else if (iis .lt. nspern(iinow)) then
                  hdlndq = hdlndq(1:lclf)//chax(48+numclef(clefq(iiv)))
                  lclf = lclf+1
                else
                  hdlndq = 
     *                hdlndq(1:lclf)//chax(48+numclef(clefq(iiv)))//'}'
                  lclf = lclf+2
                end if
61            continue
c
c  setstaffs & setclef go by instrument, not voice
c
              if (islast) then
                if (iinow .lt. 10) then
                  write(11,'(a)')sq//'setstaffs'
     *              //chax(48+iinow)//chax(48+nspern(iinow))//'%'
                  write(11,'(a)')sq//'setclef'//chax(48+iinow)
     *              //hdlndq(1:lclf)//'%'
                  if (fullsize(iiorig(iinow)).gt.0.9) then
                    write(11,'(a)')sq//'setsize'//chax(48+iinow)
     *                  //sq//'normalvalue%'
c
c 140526 Account for staff size specs when noinst changes. May be confusion
c   here if more than one staff per instrument
c
                  else if (fullsize(iiorig(iinow)).gt..7) then
                    write(11,'(a)')sq//'setsize'//chax(48+iinow)
     *                  //sq//'smallvalue%'
                  else
                    write(11,'(a)')sq//'setsize'//chax(48+iinow)
     *                  //sq//'tinyvalue%'
                  end if
                else
                  write(11,'(a11,i2,a)')sq//'setstaffs{',iinow,
     *                 '}'//chax(48+nspern(iinow))//'%'
                  write(11,'(a9,i2,a)')sq//'setclef{',iinow,
     *                 '}'//hdlndq(1:lclf)//'%'
                end if
              end if
60          continue
c
c  Loop back up, this may not be last option in M.  Note flow out if durq=' '
c
            call getchar(lineq,iccount,durq)
            go to 31
          else if (durq .eq. 'S') then
            go to 48
          end if
c
c  Write instrument names
c
          if (.not.rename) then
            do 62 iinst = 1 , ninow
              if (islast) then
                if (iinst .lt. 10) then
                  write(11,'(a8,i1,a3)')sq//'setname',iinst,'{}%'
                else
                  write(11,'(a9,i2,a4)')sq//'setname{',iinst,'}{}%'
                end if
              end if
62          continue
          else
            do 65 iinst = 1 , ninow
              if (islast) then
                if (iinst .lt. 10) then
                  write(11,'(a8,i1,a)')sq//'setname',iinst,'{'//
     *              inameq(iiorig(iinst))(1:lnam(iiorig(iinst)))//'}%'
                else
                  write(11,'(a9,i2,a)')sq//'setname{',iinst,'}{'//
     *              inameq(iiorig(iinst))(1:lnam(iiorig(iinst)))//'}%'
                end if
              end if
65          continue
            if (ishort.ne.4.and.ishort.ne.2) rename = .false.
c
c Reset later and rewrite setname{} in shortening case mcm, since if
c written here it will be inside {{...}} and will be ignored
c 
          end if
        end if
      else if (charq .eq.'|' ) then
c
c  End of bar symbol.  Check about end of bar hardspace.
c
        if (btest(iornq(ivx,nnl(ivx)+1),26)) then
c
c  There was a hardspace followed by a bar line.  Remove it from the hardspace
c  list, store with shifts instead, set special bit.  Need to repeat this code
c  at '/'.
c
          irest(ivx,nnl(ivx)) = ibset(irest(ivx,nnl(ivx)),18)
          nudoff(ivx) = nudoff(ivx)+1
          udoff(ivx,nudoff(ivx)) = udsp(nudsp)
          nudsp = nudsp-1
          iornq(ivx,nnl(ivx)+1) = ibclr(iornq(ivx,nnl(ivx)+1),26)
        end if
      else if (index('wS',charq) .gt. 0) then
c
c  Width symbol or new nsyst.  Read past (until blank)
c
4       call getchar(lineq,iccount,durq)
        if (durq .ne. ' ') go to 4
      else if (charq .eq. 'l') then
c
c  Lower string.  Only OK if at start of block
c  Check whether at beginning of a block
c
        if (iv.ne.1 .or. nnl(1).ne.0) then
          print*,'You entered "l" not at beginning of block'
          call stop1()
        end if
c
c  Set flag for lower string & read it in
c
        lower = .true.
        call getbuf(lowerq)
        iccount = 128
      else if (charq .eq. 'm') then
c
c  Meter change.  Only allow at beginning of block.
c    mtrnuml, mtrdenl (logical) and p (printable) will be input.
c    mtrnuml=0 initially. (In common)
c
c  Check whether at beginning of a block
c
        if (iv.ne.1 .or. nnl(1).ne.0) then
          print*,'You entered "m" not at beginning of block'
          call stop1()
        end if
        call readmeter(lineq,iccount,mtrnuml,mtrdenl)
        call readmeter(lineq,iccount,mtrnmp,mtrdnp)
        lenbeat = ifnodur(mtrdenl,'x')
        if (mtrdenl .eq. 2) lenbeat = 16
        lenbar = mtrnuml*lenbeat
        if (mtrdenl .eq. 2) lenbar = lenbar*2
        lenb1 = lenbar
        lenb0 = 0
        if (ismidi) call midievent('m',mtrnuml,mtrdenl)
      else if (charq .eq. 'C') then
c
c  Clef change on next note.  Set bits 11-15.  Won't allow in 2nd line of music.
c
        if (nnl(iv) .gt. 0) ncc(iv) = ncc(iv)+1
        tcc(iv,ncc(iv)) = itsofar(iv)
        isl = ibset(islur(iv,nnl(iv)+1),11)
        call getchar(lineq,iccount,durq)
c
c  Store clef number, or 7 if clef number = 9 (French violin clef)
c
        isl = ior(isl,ishft(min(numclef(durq),7),12))
        ncmidcc(iv,ncc(iv)) = ncmidf(durq)
        if (durq .eq. '8') then
          ipl(iv,nnl(iv)+1)=ibset(ipl(iv,nnl(iv)+1),2)
          iTransAmt(instno(iv)) = 7+iTransAmt(instno(iv))
          lastlev = lastlev+7
          if (nnl(iv).eq.0) then
            ndlev(iv,1) = ndlev(iv,1)+7
            ndlev(iv,2) = ndlev(iv,2)+7
          end if
        else
c
c  This won't work if you really had an octave transposition with some
c  other clef. Need a check on whether prior clef was an octave clef.
c
          if (iTransAmt(instno(iv)).eq.7) then
            iTransAmt(instno(iv))=iTransAmt(instno(iv))-7
            lastlev = lastlev-7
            if (nnl(iv).eq.0) then
              ndlev(iv,1) = ndlev(iv,1)-7
              ndlev(iv,2) = ndlev(iv,2)-7
            end if
          end if
        end if
c
c  Set marker on note with lowest voice # starting at same time.
c
        if (iv .eq. 1) then
          isl = ibset(isl,15)
        else
          do 13 iiv = 1 , iv
            nnliiv = nnl(iiv)
            if (iiv .eq. iv) nnliiv = nnliiv+1
            itother = 0
            do 14 iip = 1 , nnliiv
              if (itother .lt. itsofar(iv)) then
                itother = itother+nodur(iiv,iip)
                go to 14
              else if (itother .eq. itsofar(iv)) then
                islur(iiv,iip) = ibset(islur(iiv,iip),15)
                go to 15
              end if
14          continue
13        continue
15        continue
        end if
c
c  Need 'or' since may have set bit 15 in the above loop
c
        islur(iv,nnl(iv)+1) = ior(isl,islur(iv,nnl(iv)+1))
      else if (charq .eq. 'R') then
c
c  Repeats.  set bits 5, 6, and/or 8 of islur(1,ip+1)
c
10      call getchar(lineq,iccount,durq)
c
c  Save designator in case it's a terminal Rr or Rd
c
        if (durq .eq. 'l') then
          islur(1,nnl(1)+1) = ibset(islur(1,nnl(1)+1),5)
          go to 10
        else if (index('rdDbz',durq) .gt. 0) then
          if (durq .eq. 'r') then
            islur(1,nnl(1)+1) = ibset(islur(1,nnl(1)+1),6)
          else if (durq .eq. 'd') then
            islur(1,nnl(1)+1) = ibset(islur(1,nnl(1)+1),8)
          else if (durq .eq. 'D') then
            islur(1,nnl(1)+1) = ibset(islur(1,nnl(1)+1),26)
          else if (durq .eq. 'b') then
            islur(1,nnl(1)+1) = ibset(islur(1,nnl(1)+1),25)
          else if (durq .eq. 'z') then
c            iornq(1,nnl(1)+1) = ibset(iornq(1,nnl(1)+1),29)
            ipl(1,nnl(1)+1) = ibset(ipl(1,nnl(1)+1),0)
          end if
          rptprev = .true.
          rptfq1 = durq
          go to 10
        end if
      else if (charq .eq. 'V') then
c
c  Ending
c
        nnnl = nnl(1)+1
        lvoltxt = 0
11      call getchar(lineq,iccount,durq)
        if (durq.eq.'b' .or. durq .eq.'x') then
c
c  End Volta, set bit9, and bit10 on if 'b' (end w/ box)
c
          islur(1,nnnl) = ibset(islur(1,nnnl),9)
          if (durq .eq. 'b') islur(1,nnnl) = ibset(islur(1,nnnl),10)
          go to 11
        else if (durq .ne. ' ') then
c
c  Start volta; Get text
c
          if (lvoltxt .eq. 0) then
c
c  First character for text
c
            lvoltxt = 1
            islur(1,nnnl) = ibset(islur(1,nnnl),7)
            nvolt = nvolt+1
            voltxtq(nvolt) = durq
          else
            voltxtq(nvolt) = voltxtq(nvolt)(1:lvoltxt)//durq
            lvoltxt = lvoltxt+1
          end if
          go to 11
        end if
      else if (charq .eq. 'B') then
        bcspec = .not.bcspec
      else if (charq .eq. 'P') then
c
c  Page numbers.  Print stuff right now.
c
        npg1 = 0
c
c  Will use ltopnam to signal whether there's a centered heading
c
        ltopnam = 0
        ipg1r = 0
16      call getchar(lineq,iccount,durq)
        if (ichar(durq).ge.48 .and. ichar(durq).le.57) then
          npg1 = npg1*10+ichar(durq)-48
          go to 16
        else if (durq .eq. 'l') then
          if (npg1.eq.0 .or. mod(npg1,2).eq.1) ipg1r = 1
          go to 16
        else if (durq .eq. 'r') then
          if (npg1.gt.0 .and. mod(npg1,2).eq.0) ipg1r = 1
          go to 16
        else if (durq .eq. 'c') then
c
c  Top-centered name.  Assume this is last option.  Read the name.
c  May surround name in double quotes (to allow blanks).
c
          call getchar(lineq,iccount,durq)
          if (durq .eq. ' ') then
            ltopnam = lenstr(cheadq,60)
          else
            namstrt = iccount
            if (durq .eq. '"') then
c
c  Using quote delimiters.
c
              quoted = .true.
              namstrt = namstrt+1
            else
              quoted = .false.
            end if
            do 35 iccount = namstrt+1, 128
c              if ((quoted .and. lineq(iccount:iccount) .eq. '"') .or.
              if ((quoted .and. lineq(iccount:iccount).eq.'"' .and.
     *             lineq(iccount-1:iccount-1).ne.'\') .or.
     *           (.not.quoted .and. lineq(iccount:iccount) .eq. ' '))
     *            go to 36
c
c  On exit, iccount is OK, and name is in (namstrt:iccount-1)
c
35          continue
            print*,'Awww, cmon, should not be here.'
            call stop1()
36          continue
            ltopnam = iccount-namstrt
            cheadq = lineq(namstrt:iccount-1)
          end if
        end if
c
c  Done getting data, now assemble the command
c
        if (npg1.eq.0) npg1=1
c
c  2/23/03 Don't use \atnextline if on first page and only one system
c
c        if (ipage.gt.1 .or. nsystp(1).gt.1) then
          hdlndq = sq//'def'//sq//'atnextline{'//sq//'toppageno{'
          lhead = 27
c        else
c          hdlndq = sq//'toppageno{'
c          lhead = 11
c        end if
        if (npg1 .lt. 10) then
c
c  Note we are overwriting the last "{"
c
          write(hdlndq(lhead:lhead),'(i1)')npg1
        else if (npg1 .lt. 100) then
          lhead = lhead+3
          write(hdlndq(lhead-2:lhead),'(i2,a1)')npg1,'}'
        else 
          lhead = lhead+4
          write(hdlndq(lhead-3:lhead),'(i3,a1)')npg1,'}'
        end if
        hdlndq = hdlndq(1:lhead)//chax(ipg1r+48)//'{'
        lhead = lhead+2
c        if (ipage.gt.1 .or. nsystp(1).gt.1) then
          if (ltopnam .eq. 0) then
            if (islast) write(11,'(a)')hdlndq(1:lhead)//'}}%'
          else
            if (islast)
     *          write(11,'(a)')hdlndq(1:lhead)//cheadq(1:ltopnam)//'}}%'
          end if
c        else
c          if (ltopnam .eq. 0) then
c            if (islast) write(11,'(a)')hdlndq(1:lhead)//'}%'
c          else
c            if (islast)
c     *          write(11,'(a)')hdlndq(1:lhead)//cheadq(1:ltopnam)//'}%'
c          end if
c        end if
      else if (charq .eq. 'W') then
c
c  Just eat the number that must follow, it was used in pmxa
c
        iccount = iccount+1
        call readnum(lineq,iccount,durq,fnum)
      else if (charq .eq. 'T') then
        headlog = .true.
        inhead = 0
        call getchar(lineq,iccount,durq)
        if (durq .eq. 'i') then
          call getbuf(instrq)
c
c  A kluge for parts from separate score file for later movements.
c
          if (instrq(1:1) .eq. ' ') headlog = .false.
          cheadq = instrq(1:60)
        else if (durq .eq. 't') then
          call getchar(lineq,iccount,durq)
c
c  Optionally can include extra vertical \internotes above inbothd
c
          if (index('-+0123456789',durq) .gt. 0) then
            ipm = 1
            if (index('+-',durq) .gt. 0) then
c
c  Don't trust readnum to round this negative integer properly
c
              iccount = iccount+1
              if (durq .eq. '-') ipm = -1
            end if
            call readnum(lineq,iccount,durq,fnum)
            inhead = ipm*nint(fnum)
          end if
          call getbuf(titleq)
        else
          call getbuf(compoq)
        end if
        inhead = inhead+inbothd
        iccount = 128
      else if (charq .eq. 'A') then
c
c  Accidental handling etc.
c
27      call getchar(lineq,iccount,durq)
        if (durq .eq. 'r') then
          if (islast) then
            relacc = .true.
            write(11,'(a)')sq//'relativeaccid%'
          end if
        else if (durq .eq. 's') then
          bacfac = 1.e6
        else if (durq .eq. 'b') then
          if (islast) write(11,'(a)')sq//'bigaccid%'
          accfac = bacfac
        else if (durq .eq. 'a') then
          call getchar(lineq,iccount,durq)
          call readnum(lineq,iccount,durq,fnum)
          iccount = iccount-1
        else if (durq .eq. 'i') then
          call getchar(lineq,iccount,durq)
          call readnum(lineq,iccount,durq,tintstf)
          if (.not.firstgulp) fintstf = tintstf
c
c  Local corrections for first page were handled by pmxa
c
          iccount = iccount-1
        else if (durq .eq. 'I') then
          call getchar(lineq,iccount,durq)
          call readnum(lineq,iccount,durq,gintstf)
          iccount = iccount-1
        else if (durq .eq. 'd') then
          lowdot = .true.
        else if (durq .eq. 'o') then
          continue
        else if (durq .eq. 'S') then
          do 50 iiv = 1 , noinst
            call getchar(lineq,iccount,durq)
            if (index('-s',durq) .gt. 0) then
              fullsize(iiv) = 0.8
            else if (durq .eq. 't') then
              fullsize(iiv) = 0.64
            else
c              fullsize(ivx) = 1.0
              fullsize(iiv) = 1.0
            end if
50        continue
        else if (durq .eq. 'e') then
c
c  Line-spacing equalization
c
          equalize = .true.
c
c  The following redefinition of \parskip was put into pmx.tex in version 2.25 or so.
c    But it causes problems with some older scores and when excerpts are combined
c    with LaTeX.  So as of 2.352 we write it here.
c
          write(11,'(a)')sq//'global'//sq//'parskip 0pt plus 12'//sq
     *        //'Interligne minus 99'//sq//'Interligne%'
          tempq = sepsymq(1)
          lentemp = 1
          do 51 iiv = 2 , nv-1
            tempq = tempq(1:lentemp)//sepsymq(iiv)
            lentemp = lentemp+1
51        continue
          write(11,'(a)')sq//'def'//sq//'upstrut{'//sq//'znotes'
     *         //tempq(1:lentemp)//sq//'zcharnote{'//sq//'upamt}{~}'
     *         //sq//'en}%'
        else if (durq .eq. 'v') then
c
c  Toggle usevshrink
c
          usevshrink = .not.usevshrink
        else if (durq .eq.'p') then
c
c  Postscript slurs. fontslur is already false (set in g1etnote) 
c
          if (.not.WrotePsslurDefaults) then
c
c  Set postscrirpt slur adjustment defaults
c 
            write(11,'(a)')sq//'Nosluradjust'//sq//'Notieadjust'
     *                       //sq//'nohalfties' 
            WrotePsslurDefaults = .true.
          end if
52        continue
          call g1etchar(lineq,iccount,durq)  ! might be "+", "-", "h" or "l"
          if (index('+-',durq) .gt. 0) then
c
c  Characters to change defaults for ps slurs
c
            call g1etchar(lineq,iccount,charq)  ! charq will be "s,t,h,c"
            if (durq .eq. '+') then
              if (charq .eq. 's') then
                write(11,'(a)')sq//'Sluradjust'
              else if (charq .eq. 't') then   
                write(11,'(a)')sq//'Tieadjust'
              else if (charq .eq. 'h') then
                write(11,'(a)')sq//'halfties'
              else
                SlurCurve = SlurCurve + 1
                if (SlurCurve .gt. 3.1) then
                  call printl('WARNING!') 
                  call printl
     *            ('Default slur curvature advanced past HH, resetting')
                  SlurCurve = 3
                end if
              end if
            else
              if (charq .eq. 's') then
                write(11,'(a)')sq//'Nosluradjust'
              else if (charq .eq. 't') then   
                write(11,'(a)')sq//'Notieadjust'
              else if (charq .eq. 'h') then
                write(11,'(a)')sq//'nohalfties'
              else
                SlurCurve = SlurCurve - 1
                if (SlurCurve .lt. -1.1) then
                  call printl('WARNING!') 
                  call printl(
     *          'Default slur curvature decremented below f, resetting')
                  SlurCurve = -1
                end if
              end if
            end if
            go to 52  ! Check for another set of default changes
          else if (durq .eq. 'l') then
c
c  Set optional linebreak ties
c
            OptLineBreakTies = .true.
            go to 52
          else if (durq .eq. 'h') then
c
c  Set flag to write header special on every page
c
            HeaderSpecial = .true.
            go to 52
          else
            iccount = iccount-1
          end if
        else if (durq .eq. 'K') then
c
c Toggle keyboard rest placement flag
c
          kbdrests = .not.kbdrests
        else if (durq .eq. 'c') then
          call g1etchar(lineq,iccount,durq)
c
c Just eat the input; it was used in pmxa
c
          go to 27
        else if (durq .eq. 'V') then
          bottopgap = .true.
          topamt = 0.
          call getchar(lineq,iccount,durq)
          pmfac = index('- +',durq)-2
          call g1etchar(lineq,iccount,durq)
          call readnum(lineq,iccount,durq,botamt)
          botamt = botamt*pmfac
          pmfac = index('- +',durq)-2
          call g1etchar(lineq,iccount,durq)
          call readnum(lineq,iccount,durq,topamt)
          topamt = topamt*pmfac
          iccount = iccount-1
          go to 27
        end if
        if (index('NR',durq) .gt. 0) then
c
c  Override default part names for scor2prt, or normal include file. 
c  Just bypass rest of input line
c
          iccount = 128
        else if (durq .ne. ' ') then 
          go to 27
        end if         
      else if (charq .eq. 'K') then
77      continue
        call getchar(lineq,iccount,durq)
        if (durq .eq. 'n') then
          ignorenats = .true.
          go to 77
        end if
        if (durq .ne. 'i') then
c
c Normal, full-score key change and/or transposition
c
          num1 = 44-ichar(durq)
          iccount = iccount+1
          call readnum(lineq,iccount,durq,fnum)
          num1 = num1*nint(fnum)
c
c  On exit, durq='+','-'.  But only need isig if after start, else done in pmxa
c
          iccount = iccount+1
          call readnum(lineq,iccount,charq,fnum)
          if (ismidi) then
            midisig = nint(fnum)
            if (durq.eq.'-') midisig = -midisig
c 130317
            midisig = midisig+idsig
            call midievent('k',midisig,0)
            
          end if 
c70        continue
          if (num1 .eq. 0) then
c
c  Key change, not transposition.
c
            ipl(ivx,nnl(ivx)+1) = ibset(ipl(ivx,nnl(ivx)+1),28)
            lastisig = isig
            isig = nint(fnum)
            if (durq .eq. '-') isig = -isig
            isig = isig+idsig
            if (ismidi) call midievent('k',isig,0)
          else
c
c  num1 .ne. 0, so transposition, so must be at beginning.  isig came with K...
c  but was passed to pmxb through pmxtex.dat.  isig0 comes from setup data
c  (signature before transposition).  idsig must be added to future key changes.
c
            jv = 0
            do while (jv .lt. nm)
              jv = jv+1
              iTransAmt(jv) = num1
            end do
            idsig = isig-isig0
          end if
        else
c
c Instrument specific transposition. 
c
          call GetiTransInfo(.false.,ibarcnt,lineq,iccount,
     *                         ibaroff,nbars,noinst)
c
c  The sig parameters will have been set 1st time but that's OK
c
        end if
      else if (charq .eq. '/') then
        if (btest(iornq(ivx,nnl(ivx)+1),26)) then
c
c  There was a hardspace followed by end of block.  Remove it from the hardspace
c  list, store with shifts instead, set special bit.  This code also at '|'
c
          irest(ivx,nnl(ivx)) = ibset(irest(ivx,nnl(ivx)),18)
          nudoff(ivx) = nudoff(ivx)+1
          udoff(ivx,nudoff(ivx)) = udsp(nudsp)
          nudsp = nudsp-1
          iornq(ivx,nnl(ivx)+1) = ibclr(iornq(ivx,nnl(ivx)+1),26)
        end if
        call getchar(lineq,iccount,durq)
c
c  Save ending note level:
c
        if (ivx .le. nv) then
c
c  This is the first line of music on this staff.  If previous block had only 1
c  voice, save last pitch from line 1 of prev. block to line 2, in case a
c  2nd line is started just below
c
          if (.not.was2(iv)) ndlev(iv,2) = ndlev(iv,1)
          was2(iv) = .false.
          ndlev(iv,1) = lastlev
        else
c
c  This is the 2nd line of music on this staff.
c
          was2(iv) = .true.
          ndlev(iv,2) = lastlev
        end if
        if (durq .eq. ' ' .and. iv.eq.nv) then
c
c  End of input block
c
          loop = .false.
        else
c
c  Start a new line of music
c
          if (lenb0.ne.0 .and. firstgulp) lenbar = lenb0
          nbars = 0
          if (durq .eq. ' ') then
c
c  New line of music is on next staff
c
            iv = iv+1
            ivx = iv
          else
c
c  durq must be 2nd '/'.  New line of music is on same staff.  Set up for it
c
            ivx = nv+1
            do 23 iiv = 1 , nv
              if (nvmx(iiv) .eq. 2) ivx = ivx+1
23          continue
            nvmx(iv) = 2
            ivmx(iv,2) = ivx
            itsofar(ivx) = 0
            nnl(ivx) = 0
            nfb(ivx) = 0
            nudoff(ivx) = 0
            ndotmv(ivx) = 0
            do 24 j = 1 , 200
              irest(ivx,j) = 0
              islur(ivx,j) = 0
              nacc(ivx,j) = 0
              iornq(ivx,j) = 0
              ipl(ivx,j) = 0
              mult(ivx,j) = 0
24          continue
c
c  Go back and lower the rests in voice "a" that don't have over-ridden heights
c
            do 26 j = 1 , nnl(iv)
              if (btest(irest(iv,j),0) .and. nolev(iv,j).eq.0)
     *            nolev(iv,j) = -4
26          continue
          end if
        end if
        iccount = 128
      else if (charq .eq. 'X') then
c
c  3rd arg is only for termination of group shifts.  Use "max" to avoid zero index, 
c    which only happens for normal X at block start, and we took special measures to
c    keep group shifts for crossing block boundaries.  
c
        call getx(lineq,iccount,irest(ivx,max(1,nnl(ivx))),
     *            shifton,wheadpt,iornq(ivx,nnl(ivx)+1),ivx,
     *            irest(ivx,nnl(ivx)+1),itsofar(ivx),0,0,0,' ',ndoub)
      else if (charq .eq. 'I') then
c
c  Midi controls.  
c
        call getmidi(noinst,lineq,iccount,ibarcnt,ibaroff,nbars,lenbar,
     *               mtrdenl,nv,.false.)
      else if (charq .eq. 'M') then
c
c  Macro action
c
        call getchar(lineq,iccount,charq)
        if (index('RS',charq) .gt. 0) then
c
c  Record or save a macro.  Get the number of the macro.
c
          call getchar(lineq,iccount,durq)
          call readnum(lineq,iccount,durq,fnum)
          macnum = nint(fnum)
          macuse = ibset(macuse,macnum)
          if (charq .eq. 'R') then
            call mrec1(lineq,iccount,ndxm)
          else
c
c  Save (Record but don't activate)
c
5           call mrec1(lineq,iccount,ndxm)
            if (mrecord) then
              call getbuf(lineq)
              iccount = 0
              go to 5
            end if
            iccount = iccount+ndxm+1
          end if
        else if (charq .eq. 'P') then
c
c  Playback the macro
c
          call getchar(lineq,iccount,charq)
          call readnum(lineq,iccount,durq,fnum)
          macnum = nint(fnum)
          icchold = iccount
          lnholdq = lineq
          iccount = 128
          ilmac = il1mac(macnum)
          mplay = .true.
        end if
      else if (index(',.',charq) .gt. 0) then
c
c  Continued rhythmic shortcut
c
        idotform = index('. ,',charq)
        if (idotform .eq. 1) then
c
c  Check for start of forced beam on 2nd member of dotform=1 shortcut
c
          if (fbon) then
            if (t1fb(ivx,nfb(ivx)).eq.itsofar(ivx)) 
     *            t1fb(ivx,nfb(ivx)) = 
     *                t1fb(ivx,nfb(ivx))+nodur(ivx,nnl(ivx))/2
          end if
c		
c  Change duration of prior note 
c
          itsofar(ivx) = itsofar(ivx)-nodur(ivx,nnl(ivx))
          nodur(ivx,nnl(ivx)) = nodur(ivx,nnl(ivx))*3/2 
          itsofar(ivx) = itsofar(ivx)+nodur(ivx,nnl(ivx))
        end if
        idotform = idotform+1
        numnum = 1
        cdot = .true.
        go to 1
      end if
      return
      end
      subroutine getorn(lineq,iccount,iornq,iornq0,ornrpt,noffseg,
     *                  ip,ivx,noxtup,notcrd,nole)
c
c  iornq: Main note.  Do not alter if chord note, except turn on bit 23
c  iornq0: Store iorni + bit 23, in case of repeated ornaments
c  iorni: Internal use, 1st 21 bits of iornq or icrdorn, dep. on notcrd.
c  noffseg: horiz. offset for segno
c  nole: level of note w/ orn, used to ID the note/orn if there's a level shift.
c							

      common /comivxudorn/ivxudorn(63)
      common /comtrill/ ntrill,ivtrill(24),iptrill(24),xnsktr(24),
     *                ncrd,icrdat(193),icrdot(193),icrdorn(193),nudorn,
     *                kudorn(63),ornhshft(63),minlev,maxlev,icrd1,icrd2
      common /comcb/ nbc,ibcdata(36)
      character*1 charq,durq
      character*128 lineq
      logical ornrpt,negseg,noxtup,notcrd
c
c  Bits 0-13: (stmgx+Tupf._), 14: Down fermata, was F, 15: Trill w/o "tr", was U
c  16-18 Editorial sharp, flat, natural "oes,f,n"; 19-20: >^, 21 ? for ed. accid.
c
      call getchar(lineq,iccount,charq)
      if (index('bc',charq) .gt. 0) then
c
c  caesura or breath, handle specially and exit. Set up data in ibcdata(1...nbc)
c      ivx(0-3,28), ip(4-12),
c      vshift (vshift+32 in bits 13-18),
c      hshift (nint(10*vshift)+128 in bits 19-26)
c      bit 27 = 0 if caesura, 1 if breath
c      bit 28: 5th bit of ivx 
c
        iornq = ibset(iornq,28)
        nbc = nbc+1
c        ibcdata(nbc) = ivx+16*ip
        ibcdata(nbc) = mod(ivx,16)+16*ip
        if (ivx.ge.16) ibcdata(nbc) = ibset(ibcdata(nbc),28)
        if (charq .eq. 'b') ibcdata(nbc) = ibset(ibcdata(nbc),27)
        call getchar(lineq,iccount,durq)
        if (index('+-',durq) .gt. 0) then
c
c  We have a vertical shift, get it
c
          iccount = iccount+1
          call readnum(lineq,iccount,charq,fnum)
          if (durq .eq. '-') fnum=-fnum
          call setbits(ibcdata(nbc),6,13,nint(32+fnum))
          if (index('+-',charq) .gt. 0) then
c
c  Horizontal shift, get it
c
            iccount = iccount+1
            call readnum(lineq,iccount,durq,fnum)
            if (charq .eq. '-') fnum=-fnum
            call setbits(ibcdata(nbc),8,19,nint(10*fnum)+128)
          end if
        end if
        return
      end if
c
c  Set signal on main note that some note at this time has ornament.  ONLY used
c  in beamstrt to activate further tests for whether ihornb is needed.
c
      iornq = ibset(iornq,23)
c
c  Isolate 21 bits defining exisiting ornaments
c
      if (notcrd) then
c        iorni = iand(4194303,iornq)
c        iorni = iand(541065215,iornq)
        iorni = iand(1614807039,iornq)
      else
        iorni = iand(4194303,icrdorn(ncrd))
      end if
c      korn = index('stmgx+Tupf._)e:XXX>^',charq)
      korn = index('stmgx+Tupf._)e:XXX>^XXXXXXXXCG',charq)
      if (korn .ne. 15) iorni = ibset(iorni,korn)
c
c  Note that korn=0 => charq='(', and we set bit 0.  if "e" (14), alter later
c    as follows: korn=16-18 for sfn, and or 21 for bare ?.
c  When this if-block is done, korn will = bit# of actual ornament (unless "?").
c
      if (korn .eq. 15) then
c
cc  Turn off repeated ornament ('o:'), Replicate bits 0-3,5-15,19-20 prev iornq
cc  Turn off repeated ornament ('o:'), Replicate bits 0-3,5-15,19-21 prev iornq
c  Turn off repeated ornament ('o:'), Replicate bits 0-3,5-15,19-21,29 prev iornq
c
c        iorni = ior(iorni,iand(iornq0,1638383))
c        iorni = ior(iorni,iand(iornq0,3735535))
c        iorni = ior(iorni,iand(iornq0,540606447))
        iorni = ior(iorni,iand(iornq0,1614348271))
        ornrpt = .false.
        call getchar(lineq,iccount,durq)
c
c  durq will be ' '
c
      else if (korn .eq. 14) then
c
c  Editorial accidental
c
        call getchar(lineq,iccount,durq)
c        korn = 15+index('sfn',durq)
        korn = 15+index('sfn?',durq)
        if (korn .eq. 19) korn=21
        iorni = ibset(ibclr(iorni,14),korn)
        call getchar(lineq,iccount,durq)
        if (durq .eq. '?') then
c
c  This is "oe[s|f|n]?".  Set 21st bit also.
c
          iorni = ibset(iorni,21)
          korn = korn+6
          call getchar(lineq,iccount,durq)
        end if
c        iorni = ibset(ibclr(iorni,14),korn)
      else if (korn.eq.4 .and. noxtup) then
c
c  segno. Check in pmxa for just 1/block & notcrd.  Get horiz. offset in points
c
        noffseg = 0
        negseg = .false.
        call getchar(lineq,iccount,durq)
        if (durq .ne. ' ') then
c
c  Segno shift is specified
c
          if (durq .eq. '-') then
            negseg = .true.
            call getchar(lineq,iccount,durq)
          end if
          call readnum(lineq,iccount,durq,fnum)
          noffseg = int(fnum)
          if (negseg) noffseg = -noffseg
        end if
      else if (korn .eq. 7) then
c
c  Trill.  Check in pmxa for notcrd.  Default is 1 noteskip long, with "tr"
c
        ntrill = ntrill + 1
        ivtrill(ntrill) = ivx
        iptrill(ntrill) = ip
        xnsktr(ntrill) = 1.
        call getchar(lineq,iccount,durq)
        if (durq .eq. 't') then
c
c  Convert to new internal symbol for non-'"tr" trill
c
          korn = 15
          iorni = ibset(ibclr(iorni,7),15)
          call getchar(lineq,iccount,durq)
        end if
        if (index('0123456789.',durq) .gt. 0) then
c
c  We have a number for the length
c
          call readnum(lineq,iccount,durq,xnsktr(ntrill))
        end if
      else if (korn.eq.10 .and. noxtup) then
c
c  Fermata
c
        call getchar(lineq,iccount,durq)
        if (durq .eq. 'd') then
          korn = 14
          iorni = ibset(ibclr(iorni,10),14)
          call getchar(lineq,iccount,durq)
        end if
      else
        call getchar(lineq,iccount,durq)
      end if
      if (index('+- :',durq) .eq. 0) then
        print*,'Unexpected character at end of ornament: ',durq
        call stop1()
      end if
      if (index('+-',durq) .gt. 0) then
c
c  Shift ornament up or down
c
        nudorn = nudorn+1
c
c  Set bit 25 in iorni as a signal.  This may not really be necessary.
c
        iorni = ibset(iorni,25)
c
c  Assemble info to put in kudorn(nudorn) Bits 0-7:ip, 8-11:ivx, 12-18:nolev,
c     19-24: type of ornament to be shifted, 25-30: shift+32, 31:h-shft present
c
        xofforn = 44-ichar(durq)
        iccount = iccount+1
        call readnum(lineq,iccount,durq,fnum)
        iofforn = nint(xofforn*fnum)
        kudorn(nudorn) = ip+ishft(mod(ivx,16),8)+ishft(nole,12)
     *        +ishft(korn,19)+ishft(iofforn+32,25)
        ivxudorn(nudorn) = ivx
        if (index('+-',durq) .gt. 0) then
c
c  Horizontal shift
c
          kudorn(nudorn) = ibset(kudorn(nudorn),31)
          xofforn = 44-ichar(durq)
          iccount = iccount+1
          call readnum(lineq,iccount,durq,fnum)
c 141226         ornhshft(nudorn) = nint(xofforn*fnum)
          ornhshft(nudorn) = xofforn*fnum
        end if
      else if (durq .eq. ':') then
c
c  Turn on repeated ornaments
c
        ornrpt = .true.
c
c  Save the ornament value just set
c
        iornq0 = iorni
      end if
      if (notcrd) then
        iornq = ior(iornq,iorni)
      else
        icrdorn(ncrd) = ior(icrdorn(ncrd),iorni)
      end if
      return
      end
      subroutine getpmxmod(global,includeq)
c
c  If global=.true., checks for environment variable with path to pmx.mod. 
c    Then, if variable exists and points to pmx.mod, insert lines from 
c    pmx.mod into buffer
c  If global=.false., checks for existence of includeq and uses it.
c
c  lenbuf0 = total length of bufq on entry
c  lbuf(i) = length of line (i)
c  nlbuf = number of lines stored in bufq 
c  ilbuf = index of first line after setup stuff (on entry). In general, index of 
c          next line to be sucked from buffer.
c  ilbufmod = counter for lines in pmx.mod as they are grabbed.
c             Starts at ilbuf. Points to position of next line after 
c             pmx.mod stuff in bufq on exiting loop 1 
c  ilbuff = transient counter for shifting operations
c  ipbuf = on entry, points to last character in setup stuff. In general, points
c          to last character of most recent line sucked from buffer.
c  ipbufmod = points to last character of most recent inserted line
c             from pmx.mod
c
      parameter (maxblks=9600,nm=24)
      common /c1omget/ lastchar,fbon,issegno,ihead,isheadr,nline,isvolt,
     *     fracindent,nsperi(nm),linesinpmxmod,line1pmxmod,lenbuf0
      logical lastchar,fbon,issegno,isheadr,isvolt
      character*(*)includeq
      character*80 pmxmoddirq
      character*128 lnholdq
      character*131072 bufq
      integer*2 lbuf(maxblks)
      common /inbuff/ ipbuf,ilbuf,nlbuf,lbuf,bufq
      logical fexist,global
      line1pmxmod = ilbuf
      if (.not.global) then
        inquire(file=includeq,EXIST=fexist)
c
c  Transfer includeq to temporary char variable with known length
c
        pmxmoddirq = includeq
        lpmxmoddirq = lenstr(pmxmoddirq,80)
        print*
        write(15,'()')
        if (.not.fexist) then
          call printl('Could not find '//pmxmoddirq(1:lpmxmoddirq)
     *           //', checking further.')
c
c  File named includeq doesn't not exist. Get directory from PMXMODDIR and
c    see if it's there
c
          call getenv('PMXMODDIR',pmxmoddirq)
          lpmxmoddirq = lenstr(pmxmoddirq,80)
          if (lpmxmoddirq. gt. 0) then
            pmxmoddirq = pmxmoddirq(1:lpmxmoddirq)//includeq
            lpmxmoddirq = lenstr(pmxmoddirq,80)
          else
            call printl(
     *            'No other directory defined by PMXMODDIR, stopping')
            call stop1()
          end if
          inquire(file=pmxmoddirq,EXIST=fexist)
          if (.not.fexist) then
            call printl('Could not find '//pmxmoddirq(1:lpmxmoddirq)
     *           //', stopping.')
            call stop1()
          end if
        end if
        call printl('Opening normal include file '
     *                 //pmxmoddirq(1:lpmxmoddirq))
        open(18,file=pmxmoddirq)
      else
c
c  Check for existence of pmx.mod
c
        call getenv('PMXMODDIR',pmxmoddirq)
        lpmxmoddirq = lenstr(pmxmoddirq,80)
        if (lpmxmoddirq .eq. 0) return
        pmxmoddirq = pmxmoddirq(1:lpmxmoddirq)//'pmx.mod'
        lpmxmoddirq = lpmxmoddirq+7
        inquire(file=pmxmoddirq,EXIST=fexist)
        if (.not.fexist) return
        call printl('Opening global include file '
     *               //pmxmoddirq(1:lpmxmoddirq))
        open(18,file=pmxmoddirq(1:lpmxmoddirq))
      end if
      call printl('Adding include data')
c
c  Read lines in from pmx.mod one at a time
c
      ipbufmod = ipbuf
      lenbufmod = lenbuf0
      do 1 ilbufmod = ilbuf , maxblks
        read(18,'(a)',end=3)lnholdq
c
c  A line was read. Slide all existing lengths from here forward ahead by 1
c
        do 2 ilbuff = nlbuf , ilbufmod , -1 
          lbuf(ilbuff+1) = lbuf(ilbuff)
2       continue
c
c  Get length of line from include file
c
        lenmodline = lenstr(lnholdq,128)
        if (lenmodline .eq. 0) then
c
c  Blank line.  Make it a single blank with length 1
c
          lenmodline = 1
          lnholdq = ' '
        end if
        lbuf(ilbufmod) = lenmodline
        call printl(lnholdq(1:lenmodline))
c
c  Insert new stuff into bufq
c
        bufq = bufq(1:ipbufmod)//lnholdq(1:lenmodline)//
     *           bufq(ipbufmod+1:lenbufmod)
c
c  Update internal parameters
c
        ipbufmod = ipbufmod+lbuf(ilbufmod)
        lenbufmod = lenbufmod+lbuf(ilbufmod)
        nlbuf = nlbuf+1
1     continue
3     continue
      call printl('Closing '//pmxmoddirq(1:lpmxmoddirq))
      close(18)        
      linesinpmxmod = linesinpmxmod+ilbufmod-ilbuf
      lenbuf0 = lenbufmod
c
c  Fix Andre's error reporting problem 101211 leading to log(neg#) due
c  to nline being 2 bigger than it should be
c
      nline=nline-2
c
      return 
      end
      subroutine getset(nv,noinst,mtrnuml,mtrdenl,mtrnmp,mtrdnp,
     *    xmtrnum0,npages,nsyst,musicsize,fracindent,istype0,
     *    inameq,clefq,sepsymq,pathnameq,lpath,isig0)
      parameter (nm=24)
      common /comnvi/ nsperi(nm),nspern(nm),rename,iiorig(nm)
      character*1 clefq(nm),sepsymq(nm)
      character*40 pathnameq
      character*79 inameq(nm)
      character*128 lineq
      logical istype0,newway,rename
      common /commidisig/ midisig
c
c  Get the first line
c
      iccount = 0
9     call getbuf(lineq)
      if (lineq(1:1) .eq. '%') go to 9
      istype0 = lineq(1:3).eq.'---'
      if (istype0) then
c
c  Have TeX input until next line that starts with '---'.  Save in scratch.
c
        open(17,status='SCRATCH')
3       call getbuf(lineq)
        if (lineq(1:3) .ne. '---') then
          write(17,'(a)')lineq
          go to 3
        end if
c
c  Force a new line read on first call to readin
c
        iccount = 128
      end if
c
c  Here, lineq is first line w/ numerical setup data.
c
      nv = nint(readin(lineq,iccount,nline))
      noinst = nint(readin(lineq,iccount,nline))
      newway = noinst.le.0
      if (newway) noinst = -noinst
      do 2 iinst = 1 , noinst
c
c  Seve # of staves per inst in case later drop some inst's.
c
        if (newway) then
c
c  Read in nvi for each instrument
c
          nsperi(iinst) = nint(readin(lineq,iccount,nline))
        else if (iinst .gt. 1 ) then
          nsperi(iinst) = 1
        else
          nsperi(iinst) = nv-noinst+1
        end if
        iiorig(iinst) = iinst
        nspern(iinst) = nsperi(iinst)
2     continue
      mtrnuml = nint(readin(lineq,iccount,nline))
      mtrdenl = nint(readin(lineq,iccount,nline))
cc
cc  Kluge to make mtrdenl work
cc
c      if (mtrdenl .eq. 1) then
c        mtrdenl = 2
c        mtrnuml = mtrnuml*2
c      end if
      mtrnmp = nint(readin(lineq,iccount,nline))
      mtrdnp = nint(readin(lineq,iccount,nline))
      xmtrnum0 = readin(lineq,iccount,nline)
c
c  Original key sig (before any trnasposition) in next position.  Transposed
c  sig for topfile was transferred thru pmxtex.dat.  Need isig0 for key
c  changes if transposed.
c
      isig0 = nint(readin(lineq,iccount,nline))
c 130316
c      do 11 iinst = 1 , noinst
c        midisig(iinst) = isig0
        midisig = isig0
c11    continue
      npages = nint(readin(lineq,iccount,nline))
      nsyst = nint(readin(lineq,iccount,nline))
      musicsize = nint(readin(lineq,iccount,nline))
      fracindent = readin(lineq,iccount,nline)
c
c  Next noinst non-comment lines are names of instruments.
c
      do 4 i = 1 , noinst
5       call getbuf(inameq(i))
        if (inameq(i)(1:1) .eq. '%') go to 5
4     continue
c
c  Next non-comment line has nv clef names
c
6     call getbuf(lineq)
      if (lineq(1:1) .eq. '%') go to 6
      iv = 0
      nvsofar = 0
      do 1 jinst = 1 , noinst
        nvsofar = nvsofar+nsperi(jinst)
        do 10 ivi = 1 , nsperi(jinst)
          iv = iv+1
          clefq(iv) = lineq(iv:iv)
          if (iv .eq. nvsofar) then
            sepsymq(iv) = '&'
          else
            sepsymq(iv) = '|'
          end if
10      continue
1     continue
c
c  Mext non-comment line has path name
c
8     call getbuf(pathnameq)
      if (pathnameq(1:1) .eq. '%') go to 8
      lpath = index(pathnameq,' ')-1
c
c 160130 Replace '\' by '/'
c
12    ipos = index(pathnameq,'\')
      if (ipos .gt. 0) then
        pathnameq(ipos:ipos)='/'
c        print*,'Changed pathname to ',pathnameq(1:lpath)
        go to 12
      end if
      return
      end
      function getsquez(n,ntot,space,tnote,to)
      real*4 tnote(600),to(600)
      common /comtol/ tol
c
c  Get the squez factor by checking space against tgovern=minimum duration
c    of all notes sounding at time of n-th note in the list.  
c  The starting time of base increment is to(n) and ending time is to(n)+space
c  Sounding notes are those that start at or before to(n) .and. end at or
c    after tend=to(n)+space
c  Since notes are ordered by increasing start times, as soon as we find one 
c    that starts too late, we are done checking.
c  
      tgovern = 1000.
      tend = to(n)+space
      do 1 in = 1 , ntot
c
c  Since to() is ordered by start times, exit loop after first note that 
c    starts later than note of interest.
c
        if (to(in) .gt. to(n)+tol) go to 2
        if (to(in)+tnote(in) .gt. tend-tol) then
c
c  If here, this note overlaps and must be tested.  
c
          tgovern = min(tgovern,tnote(in))
        end if
1     continue
2     continue
      getsquez = space/tgovern
      return
      end
      subroutine getx(lineq,iccount,irest,shifton,wheadpt,iornq1,ivx,
     *                     irest1,itsofar,ntup,itup,nnodur,dotq,ndoub)
      parameter(nm=24)
c
c  Parse "X" commands.  Ignore all "B"; "P" means to ignore whole symbol.
c  In scor2prt, must strip out "P", copy only "B" and "P"-type "X"-symbols.
c  Since during getnote phase time is integer itsofar, which is not updated 
c    during xtups, we use itup and ntup to get actual time.  On entry, ntup=0 if
c    not in xtup.
c      
      common /comudsp/udsp(50),tudsp(50),nudsp,udoff(nm,20),nudoff(nm)
      logical shifton,colon,ess,number
      character*128 lineq
      character*1 charq,durq,dotq
      colon = .false.
      ess = .false.
      number = .false.
      nextbl = iccount+index(lineq(iccount:128),' ')-1
      if (index(lineq(iccount:nextbl),'P') .gt. 0) then
c
c  "Parts only", ignore entire symbol
c
        iccount = nextbl
        return
      end if
1     call getchar(lineq,iccount,charq)
      if (charq.eq.'B') then
c
c  "Both parts and score," ignore character
c
        go to 1
      else if (charq .eq. ':') then
        colon = .true.
        go to 1
      else if (charq .eq. 'S') then
        ess = .true.
        go to 1
      else if (index('+-.0123456789',charq) .gt. 0) then
        number = .true.
        if (charq.eq.'-') iccount = iccount+1
        call readnum(lineq,iccount,durq,fnum)
        if (charq .eq. '-') fnum = -fnum
        if (durq .ne. 'p') then
          fnum = fnum*wheadpt
          iccount = iccount-1
        end if
        go to 1
      end if
c
c  charq must be blank, so done parsing
c
      if (.not.ess .and. .not.colon) then
c
c  Ordinary hardspace.  Goes before next note.
c   (Later, at "|" or "/", check for presence and switch to udoff if there!)
c
        nudsp = nudsp+1
        iornq1 = ibset(iornq1,26)
        udsp(nudsp) = fnum
        tudsp(nudsp) = itsofar
        if (ntup .gt. 0) tudsp(nudsp) = tudsp(nudsp)
c     *                       +float(itup-1)/ntup*ifnodur(nnodur,dotq)
     *         +float(itup-1+ndoub)/ntup*ifnodur(nnodur,dotq)
      else if (.not.number) then
c
c  Must be "X:"  End a group offset.
c
        irest = ibset(irest,17)
        shifton = .false.
        return
      else
c
c  Only other possibility is start offset, "S" for single, ':' for multiple
c
        nudoff(ivx) = nudoff(ivx)+1
        udoff(ivx,nudoff(ivx)) = fnum
        if (ess) then
          irest1 = ibset(irest1,15)
        else
          irest1 = ibset(irest1,16)
          shifton = .true.
        end if
      end if
      return
      end
      function i1fnodur(idur,dotq)
        character*1 dotq
        if (idur .eq. 6) then
          i1fnodur=1
        else if (idur .eq. 3) then
          i1fnodur=2
        else if (idur .eq. 1) then
          i1fnodur=4
        else if (idur .eq. 8) then
          i1fnodur=8
        else if (idur .eq. 4) then
          i1fnodur=16
        else if (idur .eq. 2) then
          i1fnodur=32
        else if (idur .eq. 0) then
          i1fnodur=64
        else if (idur .eq. 16) then
c
c  Only used for denominator of time signatures, not for notes
c
          i1fnodur=4
        else if (idur .eq. 9) then
          i1fnodur = 128
        else
          print*
          print*,'You entered an invalid note-length value:',idur
          call stop1()
        end if
        if (dotq .eq. 'd') i1fnodur = i1fnodur*3/2
      return
      end
c      integer*4 function longi(ishort)
c      integer*2 ishort
c      longi = ishort
c      return
c      end
      function iashft(nacc)
      integer*4 ias(6)
      data ias /-1,1,0,0,-2,2/
      iashft = ias(nacc)
      return
      end
      function ifnodur(idur,dotq)
        character*1 dotq
        if (idur .eq. 6) then
          ifnodur=1
        else if (idur .eq. 3)then
          ifnodur=2
        else if(idur .eq. 1) then
          ifnodur=4
        else if(idur .eq. 8) then
          ifnodur=8
        else if(idur .eq. 4) then
          ifnodur=16
        else if(idur .eq. 2) then
          ifnodur=32
        else if(idur .eq. 0) then
          ifnodur=64
        else if(idur .eq. 9) then
          ifnodur=128
        else if (idur .eq. 16) then
c
c  Only used for denominator of time signatures, not for notes
c
          ifnodur=4
        else
          print*,'You entered an invalid note value'
          stop
        end if
        if (dotq .eq. 'd') ifnodur = ifnodur*3/2
      return
      end
      function ifnolev(noq,oct,ntrans)
        character*1 noq
        integer oct
        ifnolev = 7*oct+mod(ichar(noq)-92,7)+1+ntrans
      return
      end
c      subroutine report(nsdat,isdat1,isdat2)
c      integer*4 isdat1(202),isdat2(202)
c      write(*,'(a)')
c     *  ' isd on? iv  kv   ip  id ud1 ud2 ndx ivo iho lev crd lhd rhd'
c      do 1 isdat = 1 , nsdat
c        isdata = isdat1(isdat)
c        ionoff = igetbits(isdata,1,11)
cc        iv = iand(7,isdata)
c        iv = igetbits(isdata,5,13)
c        kv = igetbits(isdata,1,12)+1
c        ip = igetbits(isdata,8,3)
c        idcode = igetbits(isdata,7,19)
c        iud1 = igetbits(isdata,1,26)
c        iud2 = igetbits(isdata,1,27)
c        ndxslur = igetbits(isdata,4,28)
c        isdatb = isdat2(isdat)
c        ivo = igetbits(isdatb,6,6)-32
c        iho = igetbits(isdatb,7,12)-64
c        lev = igetbits(isdatb,7,19)
c        icrd = igetbits(isdatb,1,0)
c        lhd = igetbits(isdatb,1,1)
c        irhd = igetbits(isdatb,7,2)
c        write(*,'(17i4)')isdat,ionoff,iv,kv,ip,idcode,iud1,iud2,ndxslur,
c     *                     ivo,iho,lev,icrd,lhd,irhd
c1     continue
c      print*
c      return
c      end
      function igetbits(isdata,iwidbit,ishift)
c
c  Extracts integer given by iwidbit bits of isdata, shifted by ishift, and
c  then added to ioff
c
      igetbits = iand(2**iwidbit-1,ishft(isdata,-ishift))
      return
      end
      function igetvarlen(mmidi,icm,imidi,nbytes)
c
c  Gets variable-length integer starting in mmidi at imidi+1. Returns nbytes.
c
      parameter (nm=24,mv=24576)
      integer*2 mmidi(0:nm,mv)
      igetvarlen = 0
      do 1 nbytes = 1 , 4
        igetvarlen = 128*igetvarlen
     *               +iand(127,mmidi(icm,imidi+nbytes))
        if (.not.btest(mmidi(icm,imidi+nbytes),7)) return
1     continue
      print*,'Messup in igetvarlen'
      call stop1()
      end
      logical function isdotted(nodur,ivx,ip)
      parameter (nm=24)
c
c  Function returns true if note is dotted or double-dotted.
c    Return false for any xtuplet.
c
      common /comtol/ tol
      integer*4 nodur(nm,200)
      if (nodur(ivx,ip) .eq. 0) then
        isdotted = .false.
        return
      else if (ip .gt. 1) then
        if (nodur(ivx,ip-1) .eq. 0) then
          isdotted = .false.
          return
        end if
      end if
c
c  Ruled out all xtups, so is dotted or double-dotted if not a power of 2.
c
      isdotted = 
     *    mod(alog(float(nodur(ivx,ip)))/.69314718+.5*tol,1.) .gt. tol
      return
      end
      function isetvarlen(idur,nbytes)
      isetvarlen = 0
      itemp = idur
      do 1 nbytes = 1 , 4
        isetvarlen = isetvarlen + iand(itemp,127)*256**(nbytes-1)
        itemp = ishft(itemp,-7)
        if (itemp .gt. 0) then
          isetvarlen = isetvarlen+2**(8*nbytes+7)
        else 
          return
        end if
1     continue
      print*,'Problem in fn. isetvarlen'
      call stop1()
      end
      subroutine istring(i,string,len)
c
c  Returns string with integer only if length is 1, otherwise enclosed in
c   brackets.  
c
      character*(*) string
      if (i .ne. 0) then
        len = alog10(abs(i)*1.0001)+1
        if (i .lt. 0) len = len+1
      else
        string = '0'
        len = 1
        return
      end if
      if (len .eq. 1) then
        string = char(48+i)
      else
        string = '{'
        write(string(2:1+len),'(i'//char(48+len)//')')i
        string = string(1:len+1)//'}'
        len = len+2
      end if
      return
      end
      function lenstr(string,n)
      character*(*) string
      do 1 lenstr = n , 1 , -1
        if (string(lenstr:lenstr) .ne. ' ') return
1     continue
      lenstr = 0
      return
      end
      function levrn(nolev,irest,iud,ncm,mult)
c
c  Used for placing numbers in xtups.  Returns note level if not a rest,
c  else level of top or bottom of rest symbol opposite beam.  iud=-1 for upstm.
c
      logical btest
      if (.not.btest(irest,0)) then
        levrn = nolev
      else
c
c  Restlevel is -4, 0, 2 or 100+offset.  First get offset from 1-voice default.
c
        if (mult .gt. 0) then
          if (mult .eq. 2) then
            ioff = -1+2*iud
          else if (mult .ne. 4) then
            ioff = iud*mult
          else
            ioff = 1+4*iud
          end if
        else
c
c  May need to futz with this later for non-beamed xtups (quarter, half rests)
c
          ioff = iud*2
        end if
        levrn = mod(nolev+20,100)-20+ncm+ioff
      endif
      return
      end
      function lfmt1(x)
c
c  Computes total length of an "f" format with one decimal place.
c  First round to nearest 0.1
c
      if (abs(x) .lt. .001) then
        lfmt1 = 2
      else
        y = sign(.1*int(10*abs(x)+.5),x)
        lfmt1 = int(log10(1000*abs(y)+.001))
        if (y .lt. 0) lfmt1 = lfmt1+1
      end if
      return
      end
      subroutine LineBreakTies(isdat1,isdat2,isdat3,isdat4,nsdat,
     *                         ispstie,sepsymq)
      parameter (nm=24)
      common /comslur/ listslur,upslur(nm,2),ndxslur,fontslur
     *                 ,WrotePsslurDefaults,SlurCurve
      common /comoct/ noctup
      integer*4 isdat1(202),isdat2(202),isdat3(202),isdat4(202)
      logical fontslur,WrotePsslurDefaults,btest,upslur,ispstie,tie
      character*1 udq,chax,sepsymq(nm)
      character*8 noteq
	character*128 notexq
c
c  This is called twice from pmxb after having input an entire block, before 
c    making a bar that starts a new system.  So nsdat reflects all slur starts
c    and stops in new block, while listslur, which is only set when bars are
c    made, reflects only open slurs from the old block.  So we must check 
c    listslur to find open ties, not all nsdat.
c  First of two calls (ispstie=.false. on entry) terminates tie at end of line.
c    Second (ispstie=.true. on entry) restarts tie at start of new line. Only
c    need data from original tie-start for both of these. Tie/slur data from
c    closing of full tie are not used except for shape alterations.
c
c	do 1 ndx = 0 , 11
	do 1 ndx = 0 , 23
        if (btest(listslur,ndx)) then
c
c  Slur or tie with index ndx is open. Find the one with right ndxb, see if tie
c
          do 2 isdat = 1 , nsdat
c            if (igetbits(isdat1(isdat),4,28) .ne. ndx) go to 2 ! Wrong index
            if (igetbits(isdat1(isdat),4,28)
     *         +16*igetbits(isdat1(isdat),1,18) .ne. ndx) go to 2 ! Wrong index
            if (.not.btest(isdat1(isdat),11)) go to 2 ! Bypass if stop
            if (btest(isdat2(isdat),3)) go to 3       ! "st"
            idcode = igetbits(isdat1(isdat),7,19)    
            if (idcode.eq.1) go to 3                  ! "t" 
            tie = .false.
            go to 5
2         continue
        end if
        go to 1
3       continue
        tie = .true.
5       continue
c
c  A slur or tie is open, with index ndx
c
        iv = igetbits(isdat1(isdat),5,13)
        kv = igetbits(isdat1(isdat),1,12)+1
        udq = 'd'
        if (btest(isdat1(isdat),27)) udq='u'
        notexq = chax(92)//'znotes'
        lnote = 7
        do 4 iiv = 1 , iv-1
          notexq = notexq(1:lnote)//sepsymq(iiv)
          lnote = lnote+1
4       continue
        if (kv .eq. 2) then
          notexq = notexq(1:lnote)//chax(92)//'nextvoice'
          lnote = lnote+10
        end if          
c
c  Compute horiz and vert offsets
c
c        nolev = igetbits(isdat2(isdat),7,19)
        islhgt = igetbits(isdat3(isdat),8,14)
        ilb12 = 0
        if (ispstie) ilb12 = 1
        ivoff = igetbits(isdat4(isdat),6,ilb12*16)-32
        if (ivoff .eq. -32) ivoff = 0
C        nolev = nolev+ivoff
        islhgt = islhgt+ivoff
        ihoff = (igetbits(isdat4(isdat),7,ilb12*16+6)-64)  ! This is 10X hoff
        if (ihoff .eq. -64) ihoff = 0
c
c  Add starting stuff for command
c
        if (.not.ispstie) then                    ! End 1st segment
          notexq = notexq(1:lnote)//chax(92)//'roffset{'
          lnote = lnote+9
c          hoff = ihoff*.1-.5
c          hoff = ihoff*.1-.8
          hoff = ihoff*.1-.4
          if (hoff .lt. 0) then
            hoff = -hoff
            notexq = notexq(1:lnote)//'-'
            lnote = lnote+1
          end if
          call writflot(hoff,notexq,lnote)
          notexq = notexq(1:lnote)//'}{'
          lnote = lnote+2
        else
          notexq = notexq(1:lnote)//chax(92)//'off{-'
     *              //chax(92)//'afterruleskip}'
          lnote = lnote+21
c
c 091025 add dotting for 2nd segment if needed
c
          if (btest(isdat2(isdat),4)) then
            notexq = chax(92)//'dotted'//notexq(1:lnote)
            lnote = lnote+7
          end if  
        end if
        if (ispstie .and. tie) then
          notexq = notexq(1:lnote)//chax(92)//'tieforis'//udq
          lnote = lnote+10
        end if
        if (btest(isdat3(isdat),0)) then
c
c  Curvature tweak on termination of 1st seg
c
          imid = igetbits(isdat3(isdat),6,2)-32 
c
c  Invoke macro (from pmx.tex) that redefines \tslur as r'qd.  mapping:
c       Abs(imid)  Postscript slur type
c          1          f
c          4          h
c          5          H
c          6         HH
c
          notexq = notexq(1:lnote)//chax(92)//'psforts'//chax(48+imid)
          lnote = lnote+9
c
c  Zero out the flag in case there's a different curv on term of 2nd,
c
          isdat3(isdat) = ibclr(isdat3(isdat),0)
        end if
c
c  Add the command name
c
        if (ispstie) then
          notexq = notexq(1:lnote)//chax(92)//'is'//udq
          lnote = lnote+4
        else if (tie) then
          notexq = notexq(1:lnote)//chax(92)//'ttie'
          lnote = lnote+5
        else 
          notexq = notexq(1:lnote)//chax(92)//'tslur'
          lnote = lnote+6
        end if
c
c  Add index
c
c        if (11-ndx .lt. 10) then
c          notexq = notexq(1:lnote)//chax(59-ndx)
c          lnote = lnote+1
c        else
c          notexq = notexq(1:lnote)//'{1'//chax(49-ndx)//'}'
c          lnote = lnote+4
c        end if
        if (23-ndx .lt. 10) then
          notexq = notexq(1:lnote)//chax(71-ndx)
          lnote = lnote+1
        else if (23-ndx .lt. 20) then
          notexq = notexq(1:lnote)//'{1'//chax(61-ndx)//'}'
          lnote = lnote+4
        else 
          notexq = notexq(1:lnote)//'{2'//chax(51-ndx)//'}'
          lnote = lnote+4
        end if
        if (ispstie .or. .not.tie) then
c
c  Add note name for slur height
c
          noctup = 0
          ncm = igetbits(isdat3(isdat),8,22)
          if (ncm .eq. 23) noctup = -2
c          call notefq(noteq,lnoten,nolev,ncm)
          call notefq(noteq,lnoten,islhgt,ncm)
          notexq = notexq(1:lnote)//'{'//noteq(1:lnoten)//'}'
          lnote = lnote+1+lnoten+1
        end if
        if (ispstie) then
c
c  Horizontal shift start of new thing
c
          notexq = notexq(1:lnote)//'{'
          lnote = lnote+1
c          ihoff = ihoff-13
          if (tie) then
            ihoff = ihoff-12
          else
            ihoff = ihoff-7
          end if
          if (ihoff .lt. 0) then
            ihoff = -ihoff
            notexq = notexq(1:lnote)//'-'
            lnote = lnote+1
          end if
          call writflot(ihoff*.1,notexq,lnote)
          notexq = notexq(1:lnote)//'}'
          lnote = lnote+1          
        end if
c
c  Add closing stuff
c
        if (ispstie) then
          notexq = notexq(1:lnote)//chax(92)//'off{'
     *              //chax(92)//'afterruleskip}'
          lnote = lnote+20
        else
          notexq = notexq(1:lnote)//'}'
          lnote = lnote+1
        end if
        notexq = notexq(1:lnote)//chax(92)//'en%'
        lnote = lnote+4
        write(11,'(a)')notexq(1:lnote)
1     continue
      ispstie = .not.ispstie
      return
      end
      subroutine littex(islur,nnl,iv,topmods,lineq,iccount)
      parameter (nm=24)
      common /comlast/ islast,usevshrink
      logical islast,usevshrink
      integer islur(nm,200)
      logical topmods
      common /comgrace/ ivg(37),ipg(37),nolevg(74),itoff(2,74),aftshft,
     *                nng(37),ngstrt(37),ibarmbr,mbrest,xb4mbr,
     *                noffseg,ngrace,nvolt,ivlit(83),iplit(83),nlit,
     *                graspace(37),
     *                lenlit(83),multg(37),upg(37),slurg(37),slashg(37),
     *                naccg(74),voltxtq(6),litq(83)
      logical upg,slurg,slashg,merge
      character*128 lineq,litq
      character*20 voltxtq
      character*1 durq,chax
      merge = .false.
      if (nlit .gt. 0) then
        merge = iv.eq.ivlit(nlit) .and. nnl.eq.iplit(nlit)
      end if
      nlit = nlit+1
      ivlit(nlit) = iv
      iplit(nlit) = nnl
      itype = 1
17    call getchar(lineq,iccount,durq)
      if (durq .eq. chax(92)) then
        itype=itype+1
        go to 17
      end if
      litq(nlit) = chax(92)//durq
      lenlit(nlit) = 2
18    call getchar(lineq,iccount,durq)
      if (durq.eq.chax(92)) then
        call getchar(lineq,iccount,durq)
        if (durq .ne. ' ') then
c
c  Starting a new tex command within the string
c
          litq(nlit) = litq(nlit)(1:lenlit(nlit))//chax(92)//durq
          lenlit(nlit) = lenlit(nlit)+2
          go to 18
        end if
      else
        litq(nlit) = litq(nlit)(1:lenlit(nlit))//durq
        lenlit(nlit) = lenlit(nlit)+1
        go to 18
      end if
c
c  If here, just read backslash-blank so string is done
c
      if (itype .eq. 1) then
        islur(iv,nnl) = ibset(islur(iv,nnl),16)
        if (merge) then
c
c  There are 2 separate strings on the same note, so merge them.
c
          nlit = nlit-1
          litq(nlit) = litq(nlit)(1:lenlit(nlit))
     *                //litq(nlit+1)(1:lenlit(nlit+1))
          lenlit(nlit) = lenlit(nlit)+lenlit(nlit+1)
          if (lenlit(nlit) .gt. 128) then
            print*
            print*,
     *       'Merged type-1 TeX strings longer than 128 characters'
            write(15,'(/,a)')
     *       'Merged type-1 TeX strings longer than 128 characters'
            call stop1()
          end if  
        end if
      else
c
c  Type 2 or 3.
c
        if (itype .eq. 3) then
c
c  Write the string NOW
c
          if (islast)
     *          write(11,'(a)')litq(nlit)(1:lenlit(nlit))//'%'
        else
c
c  Must go at top
c
          if (.not.topmods) then
            topmods = .true.
            open(16,status='SCRATCH')
          end if
c
c  Must write '%' here rather than later, in case string ends with blank.
c
          write(16,'(a)')litq(nlit)(1:lenlit(nlit))//'%'
        end if
        nlit = nlit-1
      end if
      return
      end
      function llen(strq,n)
        character*129 strq
        do 1 llen = n , 0 , -1
          if (strq(llen:llen) .ne. ' ') return
1       continue
      end
      function log2(n)
c
c 5/25/08 Modify to allow more slurs
c
c        log2 = alog(1.*n)/0.6931472+.0001
c        log2 = dlog(1.d0*n)/0.693147181d0+.00000001d0
        log2 = dlog(1.d0*n)/0.693147181d0+.00000002d0
      return
      end
      subroutine logbeam(numnew,nip1,nip2)
      parameter (nm=24)
      common /all/ mult(nm,200),iv,nnl(nm),nv,ibar,
     *   ivxo(600),ipo(600),to(600),tno(600),tnote(600),eskz(nm,200),
     *   ipl(nm,200),ibm1(nm,9),ibm2(nm,9),nolev(nm,200),ibmcnt(nm),
     *   nodur(nm,200),jn,lenbar,iccount,nbars,itsofar(nm),nacc(nm,200),
     *   nib(nm,15),nn(nm),lenb0,lenb1,slfac,musicsize,stemmax,
     *   stemmin,stemlen,mtrnuml,mtrdenl,mtrnmp,mtrdnp,islur(nm,200),
     *   ifigdr(2,125),iline,figbass,figchk(2),firstgulp,irest(nm,200),
     *   iornq(nm,0:200),isdat1(202),isdat2(202),nsdat,isdat3(202),
     *   isdat4(202),beamon(nm),isfig(2,200),sepsymq(nm),sq,ulq(nm,9)
      common /comfb/ nfb(nm),t1fb(nm,40),t2fb(nm,40),ulfbq(nm,40),ifb,
     *               tautofb,autofbon,t1autofb
      common /commvl/ nvmx(nm),ivmx(nm,2),ivx
      character*1 ulq,sepsymq,sq,ulfq,ulfbq
      logical beamon,firstgulp,figbass,figchk,isfig,isxtup,btest,
     *        autofbon
      ibm1(ivx,numnew) = nip1
      ibm2(ivx,numnew) = nip2
      numnow = numnew
      if (numnew .gt. 1) then
c
c  If it starts before any others, must put it in order
c
        do 11 ib = numnew-1 , 1 , -1
          if (ibm1(ivx,ib) .lt. nip1) go to 12
          ibm1(ivx,ib+1) = ibm1(ivx,ib)
          ibm2(ivx,ib+1) = ibm2(ivx,ib)
          ulq(ivx,ib+1) = ulq(ivx,ib)
          ibm1(ivx,ib) = nip1
          ibm2(ivx,ib) = nip2
          numnow = ib
11      continue
12      continue
      end if
      sum = 0.
c
c Beam has non-xtup within
c
      nrests = 0
      isxtup = .false.
      do 9 iip = nip1 , nip2
        if (btest(islur(ivx,nip1),21)) then
c
c  Forced multiplicity
c
          call setbits(mult(ivx,iip),4,0,
     *                      igetbits(islur(ivx,nip1),3,22)+8)
        else if (.not.isxtup) then
          if (nodur(ivx,iip) .gt. 0) then
            call setbits(mult(ivx,iip),4,0,4-log2(nodur(ivx,iip))+8)
          else
c
c  Start xtup within forced beam
c
            isxtup = .true.
            iip1 = iip
          end if
        else if (isxtup .and. nodur(ivx,iip).gt.0) then
c
c  End of xtup within forced beam.  Must count doubled notes
c
          ndoub = 0
          do 1 iiip = iip1 , iip
            if (btest(nacc(ivx,iiip),18)) ndoub = ndoub+1
1         continue
          multx = int(10.5+2.929+(0.952*alog(1.+iip-iip1+ndoub)-
     *                   alog(nodur(ivx,iip)/2.))/0.69315)-10
          do 74 iiip = iip1 , iip
            call setbits(mult(ivx,iiip),4,0,multx+8)
c
c  Note the following still works after making mult only the 1st 4 bits.
c
            if (btest(nacc(ivx,iiip),18)) 
     *        mult(ivx,iiip) = mult(ivx,iiip)-1
            if (btest(nacc(ivx,iiip),19)) then 
              mult(ivx,iiip) = mult(ivx,iiip)+1
            else if (iiip .gt. 1) then
              if (btest(nacc(ivx,iiip-1),19))
     *                mult(ivx,iiip) = mult(ivx,iiip)+1
            end if  
74        continue
          isxtup = .false.
        end if
        if (btest(irest(ivx,iip),0)) then
          nrests = nrests+1
        else
          sum = sum+nolev(ivx,iip)
        end if
9     continue
c
c  Set beam up-down-ness
c
      if (ifb.gt.0 .and. ulfbq(ivx,max(1,ifb)).ne.'x') then
        if (ulfbq(ivx,ifb) .eq. 'f') then
c
c  Get default, then trade "l" and "u"
c
          ulq(ivx,numnow) = char(225-ichar(
     *      ulfq(sum/(nip2-nip1+1-nrests),ncmid(iv,nip1))))
        else
          ulq(ivx,ifb) = ulfbq(ivx,ifb)
        end if
c
c  This probably works only because forced beams are done first, so they
c  don't have to be re-sorted within each voice. ????
c
      else if (nvmx(iv) .eq. 2) then
c
c  Multi-voice per staff
c
        if (ivx .le. nv) then
          ulq(ivx,numnow) = 'l'
        else
          ulq(ivx,numnow) = 'u'
        end if
      else
c
c  Defaults
c
        ulq(ivx,numnow) =
     *      ulfq(sum/(nip2-nip1+1-nrests),ncmid(iv,nip1))
      end if
      return
      end
      subroutine m1rec1(lineq,iccount,ibarcnt,ibaroff,nbars,ndxm)
c
c  This is called when (a) macro recording is just starting and
c  (b) at the start of a new line, if recording is on
c
      parameter (maxblks=9600)
      character*128 lineq,lnholdq
      common /commac/ macnum,mrecord,mplay,macuse,icchold,lnholdq,endmac
      common /c1ommac/ ip1mac(20),il1mac(20),ip2mac(20),il2mac(20),
     *                 ic1mac(20),ilmac,iplmac
      logical mrecord,mplay,endmac
      character*131072 bufq
      integer*2 lbuf(maxblks)
      common /inbuff/ ipbuf,ilbuf,nlbuf,lbuf,bufq
	lbuf(1) = lbuf(1)
      if (.not.mrecord) then
c
c  Starting the macro
c
        ip1mac(macnum) = ipbuf-lbuf(ilbuf-1)+iccount
        il1mac(macnum) = ilbuf-1
        ic1mac(macnum) = iccount
        mrecord = .true.
      end if
      if (iccount .lt. 128) then
        ndxm = index(lineq(iccount+1:128),'M')
        if (ndxm .gt. 0) ndxm = ntindex(lineq(iccount+1:128),'M',
     *                                  128-iccount)
        if (ndxm .gt. 0) then
c
c  This line ends the macro.
c
          if (lineq(iccount+ndxm+1:iccount+ndxm+1) .ne. ' ') then
            call errmsg(lineq,iccount+ndxm+1,ibarcnt-ibaroff+nbars+1,
     *           'Improper macro termination!')
            call stop1()
          end if
          ip2mac(macnum) = ipbuf-lbuf(ilbuf-1)+iccount+ndxm
          il2mac(macnum) = ilbuf-1
          mrecord = .false.
        end if
      end if
      return
      end
      subroutine make1bar(ibmrep,tglp1,tstart,cwrest,squez,
     *    istop,numbms,istart)
      parameter (nm=24,mv=24576)
      common /all/ mult(nm,200),iv,nnl(nm),nv,ibar,
     *   ivxo(600),ipo(600),to(600),tno(600),tnote(600),eskz(nm,200),
     *   ipl(nm,200),ibm1(nm,9),ibm2(nm,9),nolev(nm,200),ibmcnt(nm),
     *   nodur(nm,200),jn,lenbar,iccount,nbars,itsofar(nm),nacc(nm,200),
     *   nib(nm,15),nn(nm),lenb0,lenb1,slfac,musicsize,stemmax,
     *   stemmin,stemlen,mtrnuml,mtrdenl,mtrnmp,mtrdnp,islur(nm,200),
     *   ifigdr(2,125),iline,figbass,figchk(2),firstgulp,irest(nm,200),
     *   iornq(nm,0:200),isdat1(202),isdat2(202),nsdat,isdat3(202),
     *   isdat4(202),beamon(nm),isfig(2,200),sepsymq(nm),sq,ulq(nm,9)
      common /comipl2/ ipl2(nm,200)
      character*10 figq
      character*1 ulq,sepsymq,sq
      logical beamon,firstgulp,figbass,figchk,isfig,bar1syst
      common /comeskz2/ eskz2(nm,200)
      common /comfig/ itfig(2,74),figq(2,74),ivupfig(2,74),nfigs(2),
     *                fullsize(nm),ivxfig2,ivvfig(2,74)
      common /combeam/ ibmtyp
      common /comask/ bar1syst,fixednew,scaldold,
     *                wheadpt,fbar,poenom
      common /comas1/ naskb,task(40),wask(40),elask(40)
      common /comas2/ nasksys,wasksys(800),elasksys(800)
      common /comhead/ ihdht,lower,headrq,lowerq,ihdvrt
      common /comfb/ nfb(nm),t1fb(nm,40),t2fb(nm,40),ulfbq(nm,40),ifb,
     *               tautofb,autofbon,t1autofb
      common /comsln/ is1n1,is2n1,irzbnd,isnx
      common /comgrace/ ivg(37),ipg(37),nolevg(74),itoff(2,74),aftshft,
     *                nng(37),ngstrt(37),ibarmbr,mbrest,xb4mbr,
     *                noffseg,ngrace,nvolt,ivlit(83),iplit(83),nlit,
     *                graspace(37),
     *                lenlit(83),multg(37),upg(37),slurg(37),slashg(37),
     *                naccg(74),voltxtq(6),litq(83)
      logical upg,slurg,slashg,autofbon
      character*128 litq
      common /comcc/ ncc(nm),tcc(nm,10),ncmidcc(nm,10),
     *               maxdotmv(nm),ndotmv(nm),updot(nm,20),rtdot(nm,20)
      integer cnn(nm),istart(80),istop(80),nxtnow(nm),ifbnow(nm),
     *        numbms(nm),mapfb(16)
      real*4 xit(nm),t1xtup(20),tstart(80),squez(80)
      character*1 ulfbq
      character*79 inameq
      character*20 voltxtq
      common /comoct/ noctup
      character*80 headrq,lowerq
      logical cwrferm
      common /comcwrf/ cwrferm(nm)
      common /comnsp/ space(80),nb,prevtn(nm),
     *    flgndv(nm),flgndb,eskgnd,ptsgnd,ivmxsav(nm,2),nvmxsav(nm)
      logical lower,cwrest(nm),vxtup,flgndb,btest,upslur,
     *        infbmx(nm),lowdot,inxtup(nm),drawbm
      common /comxtup/ ixtup,vxtup(nm),ntupv(nm,9),nolev1(nm),
     *                 mtupv(nm,9),nxtinbm(nm),
     *                 islope(nm),xelsk(24),eloff(nm,9),
     *                 nssb(nm),issb(nm),lev1ssb(nm,20)
      common /comdraw/ drawbm(nm)
      common /comstart/ facmtr
      common /spfacs/ grafac,acgfac,accfac,xspfac,xb4fac,clefac,emgfac,
     *                flagfac,dotfac,bacfac,agc1fac,gslfac,arpfac,
     *                rptfac,lrrptfac,dbarfac,ddbarfac,dotsfac,upstmfac,
     *                rtshfac
c
c  Above are factors for grace note, clef spacing. (fraction of wheadpt)
c  In 1.04, moved to block data subprogram
c
      common /comslur/ listslur,upslur(nm,2),ndxslur,fontslur
     *                 ,WrotePsslurDefaults,SlurCurve
      common /commvl/ nvmx(nm),ivmx(nm,2),ivx
      common /comtop / itopfacteur,ibotfacteur,interfacteur,isig0,
     *   isig,lastisig,fracindent,widthpt,height,hoffpt,voffpt,idsig,
     *   lnam(nm),inameq(nm)
      common /comudsp/udsp(50),tudsp(50),nudsp,udoff(nm,20),nudoff(nm)
      common /comarp/ narp,tar(8),ivar1(8),ipar1(8),levar1(8),ncmar1(8),
     *                xinsnow,lowdot
      common /comtrill/ ntrill,ivtrill(24),iptrill(24),xnsktr(24),
     *                ncrd,icrdat(193),icrdot(193),icrdorn(193),nudorn,
     *                kudorn(63),ornhshft(63),minlev,maxlev,icrd1,icrd2
      common /comtol/ tol
      integer*2 mmidi
      logical restpend,relacc,notmain,twoline,ismidi,crdacc,fontslur,
     *        WrotePsslurDefaults
      common /commidi/ imidi(0:nm),trest(0:nm),mcpitch(20),mgap,
     *       iacclo(0:nm,6),iacchi(0:nm,6),midinst(nm),
     *       nmidcrd,midchan(nm,2),numchan,naccim(0:nm),
     *       laccim(0:nm,10),jaccim(0:nm,10),crdacc,notmain,
     *       restpend(0:nm),relacc,twoline(nm),ismidi,mmidi(0:nm,mv),
     *       debugmidi
      logical debugmidi
      common /comntot/ ntot
      if (ismidi) then
c
c  Initialize for this bar the accidental counter for the midi file.  
c    naccim(icm) = # of accidentals from earlier in the bar
c
        do 45 iv = 1 , nv
        do 45 kv = 1 , nvmx(iv)
          naccim(midchan(iv,kv)) = 0
45      continue
      end if
c
c  Time from start of gulp to end of bar, used with forced beams
c
      tglp2 = lenb0+ibar*lenb1
      if (lenb0 .gt. 0) tglp2 = tglp2-lenb1
      tglp1 = tglp2-lenbar
c
c  infbmx will only be true if in xtup that is NOT in explicit forced beam.
c
      do 1 iv = 1 , nv
      do 1 kv = 1 , nvmx(iv)
        ivx = ivmx(iv,kv)
        cwrest(ivx) = .false.
        infbmx(ivx) = .false. 
        inxtup(ivx) = .false.
        if (ibar .gt. 1) then
          nn(ivx) = nib(ivx,ibar)-nib(ivx,ibar-1)
        else
          nn(ivx) = nib(ivx,ibar)
        end if
1     continue
c
c initialize list note counter, time(iv), curr. note(iv).  The loop to 4
c   ONLY initializes each voice.
c
      in = 1
      nxtup = 0
      narp = 0
      do 4 iv = 1 , nv
      do 4 kv = 1 , nvmx(iv)
        ivx = ivmx(iv,kv)
        cwrferm(ivx) = .false.
        cnn(ivx) = 1
        ivxo(in) = ivx
        ipo(in) = cnn(ivx)
        tnote(in) = fnote(nodur,ivx,1,nacc)
        to(in) = 0.
        xit(ivx) = tnote(in)
c
c  Note that xit(ivx) is to END of note in voice, but it1xtup is start time.
c
        if (nodur(ivx,ipo(in)) .eq. 0) then
c
c  First note of xtuplet at start of bar in voice ivx.
c
          nxtup = nxtup+1
          nxtnow(ivx) = nxtup
          inxtup(ivx) = .true.
          t1xtup(nxtup) = 0.
c
c  Xtup at start of bar.  If no explicit forced beam, start one, set 
c  signal infbmx, and save number ifbnow for use at termination.
c
          if (nfb(ivx) .gt. 0) then
            do 60 ifb = 1 , nfb(ivx)
              if (t1fb(ivx,ifb) .gt. tglp1+xit(ivx)+tol) then
c
c  No explicit fb here; so exit loop and insert one.  
c
                go to 61
              else if (t1fb(ivx,ifb) .lt. tglp1+xit(ivx)+tol .and.
     *                t2fb(ivx,ifb) .gt. tglp1+xit(ivx)+tol) then
c
c  IS explicit fb here; must NOT insert one
c
                go to 62
              end if
60          continue
          end if
61        continue
c
c  If here, xtup isn't in explicit fb, so must insert one
c
          infbmx(ivx) = .true.
          call addfb(nfb,ivx,t1xtup(nxtup)+
     *            tglp1,t1fb,t2fb,ulfbq,ifbadd)
          ifbnow(ivx) = ifbadd
        end if
62      continue
        if (abs(xit(ivx)-lenbar) .lt. tol) xit(ivx) = 1000.
        in = in+1
4     continue
c
c  Build the list:  This is a manual loop starting at 5
c
5     continue
c
c  Determine which voice comes next from end of notes done so far.
c  tmin is the earliest ending time of notes done so far
c
      tmin = 1000.
      do 6 iiv = 1 , nv
      do 6 kv = 1 , nvmx(iiv)
        iivx = ivmx(iiv,kv)
        tminn = min(tmin,xit(iivx))
        if(tminn .lt. tmin-tol) then
          tmin = tminn
          ivx = iivx
        end if
6     continue
      if (abs(tmin-1000.) .lt. tol) go to 7
      ivxo(in) = ivx
      cnn(ivx) = cnn(ivx)+1
      ipo(in) = cnn(ivx)
      to(in) = tmin
c
c  Check if this voice is done
c
      tnote(in) = fnote(nodur,ivx,cnn(ivx),nacc)
      if (cnn(ivx) .eq. nn(ivx)) then
        xit(ivx) = 1000.
      else
        xit(ivx) = xit(ivx)+tnote(in)
      end if
c
c  Flag xtups
c
      if (nodur(ivx,cnn(ivx)) .eq. 0) then
        if (.not.inxtup(ivx)) then
c
c  First note of xtup, not at start of bar.
c
          nxtup = nxtup+1
          nxtnow(ivx) = nxtup
          inxtup(ivx) = .true.
          t1xtup(nxtup) = xit(ivx)-tnote(in)
c
c  (Note: can't be on last note in voice, so xit(ivx) <> 1000)
c  Put xtuplet in a forced beam if not already in forced beam
c
          if (nfb(ivx) .gt. 0) then
            do 70 ifb = 1 , nfb(ivx)
              if (t1fb(ivx,ifb) .gt. tglp1+xit(ivx)+tol) then
c
c  NO explicit bm; put one in
c
                go to 71
              else if (t1fb(ivx,ifb) .lt. tglp1+xit(ivx)+tol .and.
     *              t2fb(ivx,ifb) .gt. tglp1+xit(ivx)+tol) then
c
c  IS explicit bm.  Don't put one
c
                go to 72
              end if
70          continue
          end if
71        continue
c
c  If here, no explicit bm, so put one in
c
          infbmx(ivx) = .true.
          call addfb(nfb,ivx,t1xtup(nxtup)+tglp1,
     *             t1fb,t2fb,ulfbq,ifbadd)
          ifbnow(ivx) = ifbadd
        end if
72      continue
      else if (inxtup(ivx)) then
c
c  This test is sufficient because already know nodur>0
c
        inxtup(ivx) = .false.
        if (infbmx(ivx)) then
c
c  Xtup is in auto-forced beam, so end it  
c
          t2fb(ivx,ifbnow(ivx)) = 
     *            t1xtup(nxtnow(ivx))+nodur(ivx,cnn(ivx))+tglp1
          infbmx(ivx) = .false.
        end if
      end if
      if (btest(irest(ivxo(in),ipo(in)),24) .or.
     *    btest(irest(ivxo(in),ipo(in)),30)) then
c
c  For staff jumped beam, flag the first note (lowest voice) at same time.
c  Later will start new notes group here.
c
        inj = in
        if (ivxo(in) .gt. 1) then
          do 40 iin = in-1 , 1 , -1
            if (to(iin)+tol .lt. to(in)) go to 41
            if (abs(to(iin)-to(in)) .lt. tol) then
              inj = iin
              go to 40
            end if
40        continue
        end if
41      continue
        irest(ivxo(inj),ipo(inj)) = ibset(irest(ivxo(inj),ipo(inj)),29)
      end if
      in = in+1
      go to 5
7     continue
      ntot = in-1
      do 8 in = 1 , ntot-1
        tno(in) = to(in+1)-to(in)
8     continue
      tno(ntot) = tnote(ntot)
c
c  Debug writes
c
c      write(*,'()')
c      write(*,'(a)')' Greetings from PMXB'
c      write(*,'(16i5)')(ivxo(in),in=1,ntot)
c      write(*,'(16i5)')(ipo(in),in=1,ntot)
c      write(*,'(16f5.1)')(to(in),in=1,ntot)
c      write(*,'(16f5.1)')(tno(in),in=1,ntot)
c      write(*,'(16i5)')(nodur(ivxo(in),ipo(in)),in=1,ntot)
c      write(*,'(16f5.1)')(fnote(nodur,ivxo(in),ipo(in),nacc),in=1,ntot)
c
c  Done w/ list. Loop for parsing into note blocks:
c
      ib = 1
      istart(1) = 1
      space(1) = 0.
      in = 1
c
c  A manual loop to set space(ib) and istop(ib)
c
9     continue

        ivx = ivxo(min(in+1,ntot))
        ip = ipo(min(in+1,ntot))
        isl = islur(ivx,ip)
        if (in.eq.ntot .or. ((ivx.eq.1 .and.
     *       (iand(isl,67109216).gt.0 .or. btest(ipl(1,ip),28)
c    *      .or. ornq(1,ip).eq.'g')) .or. btest(isl,15) )) then
c  Bits 1-13: stmgx+Tupf._)
c  14: Down fermata, was F
c  15: Trill w/o "tr", was U
     *      .or. btest(iornq(1,ip),4))) .or. btest(isl,15) )
c
c  Checking for start of 2nd part of jumped beam
c
     *      .or. btest(irest(ivx,ip),29) ) then
c
c  Bar end, segno, int. rpt or sig change, clef,end of 1st part of jumped beam;
c    flow out of if-loop and into block-wrapup
c
c  10/18/97:  Problem with clef alignment.  Got isl{15} set on lowest-numbered
c  voice, but it wasn't first in the list at the same time.  So check if
c  prior notes in list have same time
c  5/25/98: This stuff causes trouble with just "c2 Ct c", maybe when clef
c  changes on last note in the list?
c
          if (btest(isl,15) .and. in.lt.ntot) then
            do 50 iin = in , 1 , -1
              if (tno(iin) .gt. tol) then
                in = iin
                islur(ivx,ip) = ibclr(islur(ivx,ip),15)
                islur(ivxo(in+1),ipo(in+1)) =
     *             ibset(islur(ivxo(in+1),ipo(in+1)),15)
                go to 51
              end if
50          continue
51          continue
          end if
          if (space(ib) .lt. tol) then
            space(ib) = tno(in)
            squez(ib) = 1.
          end if
          istop(ib) = in
        else if (space(ib) .lt. tol) then
c
c  space hasn't been set yet, so tentatively set:
c
          space(ib) = tno(in)
          if (space(ib) .lt. tol) then
            in=in+1
          else
            squez(ib) = getsquez(in,ntot,space(ib),tnote,to)
            istop(ib) = in
          end if
          go to 9
        else if (tno(in+1) .lt. tol) then
c
c  This is not the last note in the group, so
c
          in = in+1
          go to 9
        else if (abs(tno(in+1)-space(ib)) .lt. tol) then
          xsquez = getsquez(in+1,ntot,space(ib),tnote,to)
          if (abs(xsquez-squez(ib)) .lt. tol) then
c
c  Keep spacing the same, update tentative stop point
c
            in = in+1
            istop(ib) = in
            go to 9
          end if
        end if
c
c At this point istart and istop are good, so finalize block
c
        tstart(ib) = to(istart(ib))
        if (istop(ib) .eq. ntot) go to 15
        ib = ib+1
        istart(ib) = istop(ib-1)+1
        in = istart(ib)
c
c Set tentative block space and squeeze-factor for upcoming block
c
        space(ib) = tno(in)
        if (space(ib).gt.tol) 
     *        squez(ib)= getsquez(in,ntot,space(ib),tnote,to)
        istop(ib) = in
      go to 9
15    continue
      nb = ib
c
c  Invert the list of places into ipl(0-7), making it easier to analyze a voice
c
      do 13 in = 1 , ntot
c
c ??? This may fix extra \loff's (bit 8 of ipl) in measures with >255 notes. 
c
c        ipl(ivxo(in),ipo(in)) = ior(ipl(ivxo(in),ipo(in)),in)
        ipl2(ivxo(in),ipo(in)) = in
13    continue
c
c  Compute elemskips from start of bar to each note in the bar, for beam slopes
c
      eskzb = 0.
      ib = 1 
      do 30 in = 1 , ntot
        if (in .eq. istart(ib)) then
          deskb = squez(ib)*feon(space(ib)/squez(ib))
        else if (tno(in-1) .gt. tol) then 
          eskzb = eskzb+deskb
        end if
        eskz(ivxo(in),ipo(in)) = eskzb
        eskz2(ivxo(in),ipo(in)) = eskzb
        if (in .eq. istop(ib)) then
          eskzb = eskzb+deskb
          ib = ib+1
        end if
30    continue
c
c  Analyze for beams.
c
      do 20 iv = 1 , nv
      do 20 kv = 1 , nvmx(iv)
        ivx = ivmx(iv,kv)
        numbms(ivx) = 0
        mapfb(1) = 0
        mapfb(2) = 0
        mapfb(3) = 0
        mapfb(4) = 0
c
c  First forced beams.
c
        if (nfb(ivx) .gt. 0) then
c
c  tglp2 is time from start of gulp to end of current bar.
c
          nfbbar = 0
          do 80 ifb = 1 , nfb(ivx)
            if (t1fb(ivx,ifb).gt.tglp2-tol) go to 81
            nfbbar = nfbbar+1
            numbms(ivx) = numbms(ivx)+1
            numnew = numbms(ivx)
c
c  Times from beginning of bar
c
            itbb1 = int(t1fb(ivx,ifb)-tglp1+tol)
            itbb2 = int(t2fb(ivx,ifb)-tglp1+tol)
            do 83 ip = 1 , nn(ivx)
              if (int(to(ipl2(ivx,ip))+tol) .eq. itbb1) then
                nip1fb = ip
                do 84 ip1 = ip , nn(ivx)
                  inip1 = ipl2(ivx,ip1)
                  if (abs(to(inip1)+tnote(inip1)-itbb2) .lt. tol) then
                    nip2fb = ip1
                    itbb3 = itbb2-2
                    go to 85
                  end if
84              continue
              end if
83          continue
            print*
            print*,'Timing problem w/ forced beams'
            write(15,'(/a)')'Timing problem w/ forced beams'
85          continue
            call logbeam(numnew,nip1fb,nip2fb)
c
c  Set up mapfb for forced beam just logged:
c
            ib1 = itbb1/2
            ib2 = itbb3/2
            ibrep = lenbar/ibmrep/2
            do 86 irep = 1 , ibmrep
              ib1now = max(0,ib1-(irep-1)*ibrep)
              ib2now = min(irep*ibrep-1,ib2-(irep-1)*ibrep)
              mapnow = 0
              do 87 ib = ib1now , ib2now
                mapnow = ibset(mapnow,ib)
87            continue
              mapfb(irep) = ior(mapfb(irep),mapnow)
86          continue
c
c  Since we are cycling thru forced beams, for those that start with a rest and 
c    have height & slope adjustments, move adjustments to next note.
c  060924: Copy to ALL later notes in fb, in case there's more than 1 rest at
c    start of beam
c
            if (btest(irest(ivx,nip1fb),0)) then
              do 88 kp = nip1fb+1 , nip2fb
                call setbits(ipl(ivx,kp),6,11,
     *                 igetbits(ipl(ivx,nip1fb),6,11))
                call setbits(ipl(ivx,kp),6,17,
     *                 igetbits(ipl(ivx,nip1fb),6,17))
                call setbits(islur(ivx,kp),2,27,
     *                 igetbits(islur(ivx,nip1fb),2,27))
88            continue
            end if
80        continue
81        continue
c
c  Slide down, reduce nfb(ivx).  This lets us count up from 1 for each new bar.
c  Remember, makeabar is called 1/bar, and it calls findbeam once per voice.
c
          if (nfbbar .gt. 0) then
            nfb(ivx) = nfb(ivx)-nfbbar
            do 82 ifb = 1 , nfb(ivx)
              t1fb(ivx,ifb) = t1fb(ivx,ifb+nfbbar)
              t2fb(ivx,ifb) = t2fb(ivx,ifb+nfbbar)
              ulfbq(ivx,ifb) = ulfbq(ivx,ifb+nfbbar)
82          continue
          end if
        end if
        ifb = 0
c
c  Done with forced beam masks for this bar and voice.  Now get normal beams.
c
        call findbeam(ibmrep,numbms,mapfb)
20    continue
      return
      end
      subroutine make2bar(ninow,tglp1,tstart,cwrest,squez,
     *    istop,numbms,istart,clefq)
      parameter (nm=24,mv=24576)
      common /comlast/ islast,usevshrink
      logical islast,usevshrink
      common /all/ mult(nm,200),iv,nnl(nm),nv,ibar,
     *   ivxo(600),ipo(600),to(600),tno(600),tnote(600),eskz(nm,200),
     *   ipl(nm,200),ibm1(nm,9),ibm2(nm,9),nolev(nm,200),ibmcnt(nm),
     *   nodur(nm,200),jn,lenbar,iccount,nbars,itsofar(nm),nacc(nm,200),
     *   nib(nm,15),nn(nm),lenb0,lenb1,slfac,musicsize,stemmax,
     *   stemmin,stemlen,mtrnuml,mtrdenl,mtrnmp,mtrdnp,islur(nm,200),
     *   ifigdr(2,125),iline,figbass,figchk(2),firstgulp,irest(nm,200),
     *   iornq(nm,0:200),isdat1(202),isdat2(202),nsdat,isdat3(202),
     *   isdat4(202),beamon(nm),isfig(2,200),sepsymq(nm),sq,ulq(nm,9)
      common /comipl2/ ipl2(nm,200)
      character*10 figq
      character*1 ulq,sepsymq,sq,chax
      logical beamon,firstgulp,figbass,figchk,isfig,bar1syst,isbjmp,
     *        isbj2
      common /combjmp/ ivbj1,ivbj2,isbjmp,isbj2,multbj1
      common /comfig/ itfig(2,74),figq(2,74),ivupfig(2,74),nfigs(2),
     *                fullsize(nm),ivxfig2,ivvfig(2,74)
      common /combeam/ ibmtyp
      common /comask/ bar1syst,fixednew,scaldold,
     *                wheadpt,fbar,poenom
      common /comas1/ naskb,task(40),wask(40),elask(40)
      common /comas2/ nasksys,wasksys(800),elasksys(800)
      common /comhead/ ihdht,lower,headrq,lowerq,ihdvrt
      common /comfb/ nfb(nm),t1fb(nm,40),t2fb(nm,40),ulfbq(nm,40),ifb,
     *               tautofb,autofbon,t1autofb
      common /comsln/ is1n1,is2n1,irzbnd,isnx
      common /comgrace/ ivg(37),ipg(37),nolevg(74),itoff(2,74),aftshft,
     *                nng(37),ngstrt(37),ibarmbr,mbrest,xb4mbr,
     *                noffseg,ngrace,nvolt,ivlit(83),iplit(83),nlit,
     *                graspace(37),
     *                lenlit(83),multg(37),upg(37),slurg(37),slashg(37),
     *                naccg(74),voltxtq(6),litq(83)
      logical upg,slurg,slashg,issig,rpndot,autofbon
      character*128 litq
      common /comcc/ ncc(nm),tcc(nm,10),ncmidcc(nm,10),
     *               maxdotmv(nm),ndotmv(nm),updot(nm,20),rtdot(nm,20)
      integer istart(80),istop(80),iaskb(nm),numbms(nm),
     *        nornb(nm),ihornb(nm,24),lcwr(nm),ibmcnt1(nm),ifig(2)
      character*1 ulfbq,udqq,clefq(nm),slurudq,udq
      character*8 noteq
      character*79 cwrq(nm),notexq,inameq
      character*20 voltxtq
      common /comoct/ noctup
      character*80 soutq,headrq,lowerq
      common /comnsp/ space(80),nb,prevtn(nm),
     *    flgndv(nm),flgndb,eskgnd,ptsgnd,ivmxsav(nm,2),nvmxsav(nm)
      logical bspend,lower,cwrest(nm),vxtup,
     *        isgrace,isclef,nofirst,iscln,isdot,isflag,
     *        isacc,isfirst,flgndb,btest,stemup,upslur,
     *        isgaft,isarp,isrshft,isaccs,iscacc,
     *        lowdot,flipend,iscwr,isdotted,beamon1(nm)
      real*4 ptgr(37),ptclef(nm),ptsndg(nm),eskndg(nm),tstart(80),
     *       squez(80)
      logical cwrferm,drawbm
      common /comcwrf/ cwrferm(nm)
      common /comxtup/ ixtup,vxtup(nm),ntupv(nm,9),nolev1(nm),
     *                 mtupv(nm,9),nxtinbm(nm),
     *                 islope(nm),xelsk(24),eloff(nm,9),
     *                 nssb(nm),issb(nm),lev1ssb(nm,20)
      common /comdraw/ drawbm(nm)
      common /comstart/ facmtr
      common /spfacs/ grafac,acgfac,accfac,xspfac,xb4fac,clefac,emgfac,
     *                flagfac,dotfac,bacfac,agc1fac,gslfac,arpfac,
     *                rptfac,lrrptfac,dbarfac,ddbarfac,dotsfac,upstmfac,
     *                rtshfac
c
c  Factors for grace note, clef spacing. (fraction of wheadpt)
c  In 1.04, moved to block data subprogram
c
      common /comslur/ listslur,upslur(nm,2),ndxslur,fontslur
     *                 ,WrotePsslurDefaults,SlurCurve
      common /commvl/ nvmx(nm),ivmx(nm,2),ivx
      common /comtop / itopfacteur,ibotfacteur,interfacteur,isig0,
     *   isig,lastisig,fracindent,widthpt,height,hoffpt,voffpt,idsig,
     *   lnam(nm),inameq(nm)
      common /comudsp/udsp(50),tudsp(50),nudsp,udoff(nm,20),nudoff(nm)
      common /comarp/ narp,tar(8),ivar1(8),ipar1(8),levar1(8),ncmar1(8),
     *                xinsnow,lowdot
      common /comtrill/ ntrill,ivtrill(24),iptrill(24),xnsktr(24),
     *                ncrd,icrdat(193),icrdot(193),icrdorn(193),nudorn,
     *                kudorn(63),ornhshft(63),minlev,maxlev,icrd1,icrd2
      common /strtmid/ ihnum3,flipend(nm),ixrest(nm)
      common /comtol/ tol
      common /comignorenats/ mbrhgt,newmbrhgt,ignorenats
      logical ignorenats,newmbrhgt
      integer*2 mmidi,iinsiv
      logical restpend,relacc,notmain,twoline,ismidi,crdacc,fontslur
     *       ,WrotePsslurDefaults
      common /commidi/ imidi(0:nm),trest(0:nm),mcpitch(20),mgap,
     *       iacclo(0:nm,6),iacchi(0:nm,6),midinst(nm),
     *       nmidcrd,midchan(nm,2),numchan,naccim(0:nm),
     *       laccim(0:nm,10),jaccim(0:nm,10),crdacc,notmain,
     *       restpend(0:nm),relacc,twoline(nm),ismidi,mmidi(0:nm,mv),
     *       debugmidi
      logical debugmidi
      logical slmon,dbltie
      common /comslm/ levson(0:nm),levsoff(0:nm),imidso(0:nm),
     *       naccbl(0:nm),laccbl(0:nm,10),jaccbl(0:nm,10),nusebl,
     *       slmon(0:nm),dbltie
      common /comevent/ miditime,lasttime
      logical kbdrests
      common /comkbdrests/ levbotr(8),levtopr(8),kbdrests
      logical secondgrace
      common /comInstTrans/ iInstTrans(nm),iTransKey(nm),iTransAmt(nm),
     *  instno(nm),nInstTrans,EarlyTransOn,LaterInstTrans
      logical EarlyTransOn,LaterInstTrans
      common /commidisig/ midisig
c 130316
      common /commvel/ midivel(nm),midvelc(0:nm),midibal(nm),midbc(0:nm)
     *                ,miditran(nm),midtc(0:nm),noinst,iinsiv(nm)
      common /comclefrests/ centrests
      logical centrests
      common /xjbeambrests/ nbrests
      common /comc8flag/ c8flag(nm)
      logical c8flag
      common /comnvi/ nsperi(nm),nspern(nm),rename,iiorig(nm)
      character*4 ivxq
      logical rename
      common /comshort/ shortfrac,codafrac,ishort,mbrsum,nmbr,nocodabn,
     *  poefa
      real*4 poefa(125)
      logical nocodabn
      nbrests = 0
c
c  Set up main ib loop within which a block (notes group) is written
c
      do 25 iv = 1 , nv
      do 25 kv = 1 , nvmx(iv)
        ivx = ivmx(iv,kv)
        ibmcnt(ivx) = 1
        ibmcnt1(ivx) = 1
        beamon(ivx) = .false.
        beamon1(ivx) = .false.
        nornb(ivx) = 0
        iaskb(ivx) = 1
        vxtup(ivx) = .false.
        drawbm(ivx) = .true.
25    continue
      naskb = 0
      ifig(1) = 1
      ifig(2) = 1
      ixtup = 0
      bspend = .false.
      iscwr = .false.
      rpndot = .false.
      do 16 ib = 1 , nb
c
c  Check for segno
c
        if (btest(iornq(1,ipo(istart(ib))),4) .and.
     *       ivxo(istart(ib)).eq.1) then
          if (noffseg.le.-10) then
            write(noteq(1:5),'(1H{,i3,1H})')noffseg
            lnoten = 5
          else if (noffseg.lt.0 .or. noffseg.ge.10) then
            write(noteq(1:4),'(1H{,i2,1H})')noffseg
            lnoten = 4
          else
            write(noteq(1:1),'(i1)')noffseg
            lnoten = 1
          end if
          notexq = sq//'znotes'//sq//'segnoo'//noteq(1:lnoten)//'9'
          lnote = 15+lnoten
          do 130 iv = 2 , nv
            if (lnote .gt. 60) then
              if (islast) write(11,'(a)')notexq(1:lnote)//'%'
              notexq = sepsymq(iv-1)//sq//'segnoo'
     *                           //noteq(1:lnoten)//'9'
              lnote = lnoten+9                 
            else
              notexq = notexq(1:lnote)//sepsymq(iv-1)//sq//'segnoo'
     *               //noteq(1:lnoten)//'9'
              lnote = lnote+lnoten+9
            end if
130       continue
          if (islast) write(11,'(a)')notexq(1:lnote)//sq//'en'
          lnote = 0
        end if
c
c  Check for new clefs
c
        isclef = .false.
        if (btest(islur(ivxo(istart(ib)),ipo(istart(ib))),15))
     *       then
c
c  In preceding line, fl32 gave wrong result for ... .gt.0 !!!
c
          do 140 in = istart(ib) , istop(ib)
            if (btest(islur(ivxo(in),ipo(in)),11)) then
              call wsclef(ivxo(in),ninow,
     *          iand(ishft(islur(ivxo(in),ipo(in)),-12),7))
c
c If clefq = '8', must add eg \settrebleclefsymbol3\treblelowoct%
c
c              if (clefq(ivxo(in)) .eq. '8') then
              if (btest(ipl(ivxo(in),ipo(in)),2)) then
c
c Find instrument number for voice ivso(in)
c
                iv1 = 1
                do 1111 iinst = 1 , ninow
                  if (ivxo(in) .lt. iv1+nspern(iinst)) go to 2222
                  iv1 = iv1+nspern(iinst)
1111            continue
                print*
                print*,'Should not be here in make2bar!'
                call stop1()
2222            continue
                if (iinst .le. 9) then
                  write(11,'(a20,i1,a)')sq//'settrebleclefsymbol',
     *              iinst,sq//'treblelowoct%'
                else
                  write(11,'(a20,i2,a)')sq//'settrebleclefsymbol',
     *              iinst,sq//'treblelowoct%'
                end if
                c8flag(ivxo(in)) = .true.
              end if
            end if
140       continue
          if (islast) write(11,'(a)')sq//'pmxnewclefs'
          isclef = .true.
        end if
c
c  Start a notes group.  We're just gonna define every one using pnotes{n}, 
c    where \def\pnotes#1{\vnotes#1\elemskip}
c
        soutq = sq//'pnotes{'
        eonsqz = squez(ib)*feon(space(ib)/squez(ib))
        if (eonsqz .gt. 9.995) then
          write(soutq(9:12),'(f4.1)')eonsqz
        else if (eonsqz .gt. 0.995) then
          write(soutq(9:12),'(f4.2)')eonsqz
        else if (eonsqz .gt. 0.095) then
          soutq = soutq(1:8)//'0.'
          write(soutq(11:12),'(i2)')nint(100*eonsqz)
        else
          soutq = soutq(1:8)//'0.0'
          write(soutq(12:12),'(i1)')nint(100*eonsqz)
        end if
        soutq = soutq(1:12)//'}'
        lsout = 13
c
c  Check whole block, flag accidentals etc that are too close, one per *time*.
c  Note about bar starts and after rpt's/boublebars: There is an afterruleskip
c    (fbar*wheadpt) following, but rpts seem to occupy some of that gap, so
c    (dotsfac*wheadpt) is presumed to be filled up.
c
        in = istart(ib)-1
        itrpt = -1
        itsig = -1
c
c  Begin big manual loop over notes in this block; ends at 112
c
111     in = in+1
        if (in .gt. istop(ib)) go to 112
        ip = ipo(in)
        ivx = ivxo(in)
        if (ivx .le. nv) then
          iv = ivx
        else
          do 128 iv = 1 , nv
            if (nvmx(iv) .eq. 2 .and. ivmx(iv,2).eq. ivx) go to 129
128       continue
        print*,'Trouble finding iv!, ivx,nvmx,ivmx:',ivx,nvmx(1),nvmx(2)
        print*,ivmx(1,1),ivmx(1,2),ivmx(2,1),ivmx(2,2)
          stop
        end if
129     continue
c
c  Call precrd here so we know how much space to add for accid's in chords
c    After calling precrd, icashft>0 means there is a shifted chordal accid (incl.
c    main note.
c
c  To call precrd, need up-downness, so must track if in beam.
c
c  Deal w/ staff-jumping beams later
c         if ((numbms(ivx).gt.0 .and. ibmcnt(ivx).le.numbms(ivx)
c     *          .and. ibm1(ivx,ibmcnt(ivx)) .eq. ip) .or. 
c     *          btest(nacc(ivx,ip),21)) then
c           if (.not.btest(irest(ivx,ip),24)) then
        if ((numbms(ivx).gt.0 .and. ibmcnt1(ivx).le.numbms(ivx)
     *        .and. ibm1(ivx,ibmcnt1(ivx)) .eq. ip)) then
          beamon1(ivx)=.true.
        end if
        icashft = 0
        if (btest(ipl(ivx,ip),10)) then
c
c  There is a chord on this note. Need up-down-ness in precrd to auto shift for 2nds.
c
          if (beamon1(ivx)) then 
             call precrd(ivx,ip,nolev(ivx,ip),nacc(ivx,ip),
     *          ipl(ivx,ip),irest(ivx,ip),ulq(ivx,ibmcnt1(ivx)),
     *          .false.,icashft)
          else
            call precrd(ivx,ip,nolev(ivx,ip),nacc(ivx,ip),
     *              ipl(ivx,ip),irest(ivx,ip),udqq(nolev(ivx,ip),
     *              ncmid(iv,ip),islur(ivx,ip),nvmx(iv),ivx,nv),
     *            .false.,icashft)
          end if
        end if
c
c  Turn beam off?
c
        if (beamon1(ivx) .and. ibm2(ivx,ibmcnt1(ivx)).eq.ip) then
          beamon1(ivx) = .false.
          ibmcnt1(ivx) = ibmcnt1(ivx)+1
        end if
c
c  Remember, rpts & internal sigs can only come at start of (internal) block
c
        isacc = iand(nacc(ivx,ip),3).gt.0 
     *            .and. .not.btest(nacc(ivx,ip),17)
     *            .and. .not.btest(ipl(ivx,ip),10)
c
c  i.e., do not set for chord. Now check for "(" as ornament on main note, 
c
c!!!  Need to do this for chord notes too.  Maybe in chkarp?
c 
        isaccs = isacc .or. btest(iornq(ivx,ip),0)
c
c  5/15/02 Add check for ) ornament of prior note.
c  5/16 Nope...fails when grace intervenes.
c        if (ip .gt. 1) then
c          isaccs = isaccs .or. btest(iornq(ivx,ip-1),13)
c        end if
        isarp = btest(iornq(ivx,ip),27)
        iscacc = .false.
        if (btest(ipl(ivx,ip),10)) then
c
c  There is a chord here; check for arpeggios and accidentals. Note accid shifts are
c    not of concern here, only whether there's an accid, whick causes iscacc=.true.
c
          iscacc = igetbits(nacc(ivx,ip),3,0).gt.0 .and. 
     *                .not.btest(nacc(ivx,ip),17)
          call chkarp(ncrd,icrdat,ivx,ip,iscacc,isarp)
        end if
c
c  When we get motivated, will do spacing for arpeggios here.
c
        if (ivx.eq.1 .and. iand(islur(ivx,ip),96).gt.0)
     *        itrpt = nint(to(in))
        issig  = btest(ipl(ivx,ip),28)
        if (ivx.eq.1 .and. issig) itsig = nint(to(in))
        isgrace = btest(islur(ivx,ip),4) .and.
     *    .not.btest(ipl(ivx,ip),29) .and. .not.btest(ipl(ivx,ip),31)
        isgaft = .false.
        if (ip .gt. 1) then
          xnd = tnote(ipl2(ivx,ip-1))
          isgaft = btest(ipl(ivx,ip-1),29) .or. btest(ipl(ivx,ip-1),31)
          isgrace = isgrace .or. isgaft
        end if
        iscln = isclef .and. btest(islur(ivx,ip),11)
c
c  Is prev. note non-beamed, up-stemmed, & flagged? Recall if ip>1, have nd
c
        isflag = ip.gt.1 .and. xnd.gt.tol .and. xnd.lt.16.-tol
        if (isflag)
     *       isflag = .not.btest(irest(ivx,ip-1),0)
     *          .and. udqq(nolev(ivx,ip-1),
     *         ncmid(iv,ip-1),islur(ivx,ip-1),nvmx(iv),ivx,nv).eq.'u'
        if (isflag) then
          do 116 ibmchk = 1 , numbms(ivx)
            if (ip-1 .lt. ibm1(ivx,ibmchk)) then
              go to 117
c
c  Add check for non-beamed xtuplets. May be problem with stem direction.
c
            else if (ip-1.le.ibm2(ivx,ibmchk) .and. 
     *                .not.btest(islur(ivx,ibm1(ivx,ibmchk)),18)) then
              isflag = .false.
              go to 117
            end if
116       continue
        end if
117     continue
c
c  If isflag, then won't need to check for dot on prev. note.
c
c  5/16/02 ???  Try using this for ) ornament.
c
        isflag = isflag .or. btest(iornq(ivx,ip-1),13)
        isdot = ip.gt.1
        if (isdot) isdot = isdotted(nodur,ivx,ip-1)
        isrshft = ip .gt. 1
        if (isrshft) isrshft = btest(irest(ivx,ip-1),20)
        if (.not.(isaccs.or.isgrace.or.iscln.or.isflag.or.isrshft.or.
     *          isdot.or.btest(iornq(ivx,ip),26).or.
     *          btest(irest(ivx,ip),21).or.isarp.or.
     *          btest(irest(ivx,ip),27).or.iscacc)) go to 111
c
c  Here is an accid,grace,clef,flag,rtshft,dot,udsp,arpeg,left-shift.
c  Compute pts, the total occupied space including prior notehead.
c
c 130324
c        wheadpt1 = wheadpt*fullsize(iv)
        wheadpt1 = wheadpt*fullsize(instno(iv))
        pts = wheadpt1
c
c  Set up for possible cautionary accidental here
c
        if (isaccs .or. iscacc) then
          if (.not.btest(iornq(ivx,ip),31)) then
            taccfac = accfac
          else
            taccfac = 1.4*accfac ! cautionary accidental
          end if
        end if
        if (isgrace) then
          secondgrace = .false.
          do 122 ig = 1 , ngrace
            if (.not. isgaft) then
              if (ipg(ig).eq.ip .and. ivg(ig).eq.ivx) go to 123
            else if (ip .gt. 1) then
              if (ipg(ig).eq.ip-1 .and. ivg(ig).eq.ivx) go to 123
            end if
122       continue
          print*,'Problem finding grace index in makeabar'
          stop
123       continue
c
c  wgr = distance to backspace (in headwidths), less main acc.
c  ptgr = same in pts,+ main acc.  Not used for after-grace. Distance to backspace.
c  spgr = total space needed (w/o main acc).
c   Also, spgr is same for b4 or after, but xb4fac-space will be in diff. place.
c
          if (nng(ig) .eq. 1) then
            wgr = grafac
            if (multg(ig) .eq. 0) wgr = wgr-.4
          else
            wgr = nng(ig)*emgfac
            do 126 ing = 2 , nng(ig)
              if (naccg(ngstrt(ig)-1+ing) .gt. 0) wgr = wgr+acgfac
126         continue
          end if
          if (graspace(ig) .gt. 0.) then
c
c  User-defined space before grace
c
            wgr = wgr+graspace(ig)
          end if
          ptgr(ig) = wgr*wheadpt1
          spgr = ptgr(ig)+xb4fac*wheadpt1
c
c!!! May need to mod for chord accid's
c
          if (isaccs .or. iscacc) ptgr(ig) = ptgr(ig) + taccfac*wheadpt1
          if (naccg(ngstrt(ig)) .gt. 0) spgr = spgr+wheadpt1*agc1fac
          pts = pts+spgr
c
c  Special check for after-grace on ip-1 and normal on ip. Must go back thru
c   loop again for the normal grace.
c
          if (isgaft .and. ig.lt.ngrace .and. .not.secondgrace) then
            if (ipg(ig+1).eq.ip) then  
              secondgrace = .true.
              ig = ig+1
              go to 123
            end if
          end if
        end if
        if (iscln) then
          pts = pts+clefac*wheadpt1
c
c  How far to backspace when printing the clef
c
          ptclef(ivx) = 0.
c
c!!! May need to mod for chord accid's
c
          if (isaccs .or. iscacc)
     *          ptclef(ivx) = ptclef(ivx)+taccfac*wheadpt1
          if (isgrace) ptclef(ivx) = ptclef(ivx)+spgr
        end if
        if (isrshft) then
          pts = pts+rtshfac*wheadpt1
        else if (isflag) then
          pts = pts+flagfac*wheadpt1
        else if (isdot) then
          pts = pts+dotfac*wheadpt1
        end if
        if (abs(to(in)-itrpt) .lt. tol) then
c
c  Repeat, need a little extra space
c
          pts = pts+dotsfac*wheadpt1
        end if
        if (isarp) then
          pts = pts+arpfac*wheadpt1
        end if
c
c  Add in padding space
c
        pts = pts+xspfac*wheadpt1
c
c  Now done with all items needing space except accidentals, 
c    accidental shifts, and left-notehead-shifts, and will later 
c    subtract a notehead if at start of bar.
c
c  Get available space in elemskips (esk)
c
        isfirst = ip.eq.1 .or. abs(to(in)-itrpt).lt.tol .or.
     *        abs(to(in)-itsig).lt.tol
        if (isfirst) then
c
c  At start of bar or after repeat sign or new signature
c
          if (abs(to(in)-itsig).lt.tol) then
            esk = 0.
          else
            esk = fbar
          end if
        else
c
c  Not 1st note of bar
c
          esk = eskz(ivx,ip)-eskz(ivx,ip-1)
        end if
        if (isgrace) then
c
c  Since graces can be very long, cannot assume no interference if prior
c  note uses >1 noteskip.  So must get elsk's back to prior note, whether or
c  not it used only one noteskip.  
c  <<But if it was xtup. don't need to call eskb4.>>????
c
c  10/8/05 Kluge to not zero out esk if in xtup
c
          esksav = esk
          if ((ip.le.2 .or. nodur(ivx,max(1,ip-2)).gt.0) .and.
c     *                                to(in).ne.itsig)
     *                                abs(to(in)-itsig).gt.tol)
     *          call eskb4(ip,ivx,in,ib,space,tstart,fbar,itrpt,esk)
          if (abs(esk) .lt. tol) esk = esksav
        end if
c
c  Done getting available elemskips.  Remove headwidth if first.  Must do here
c  rather than earlier since check uses isfirst
c
        if (isfirst) pts = pts-wheadpt1
c
c  Deal with accidental shifts and left-notehead shifts
c
        if (btest(ipl(ivx,ip),10)) then
c
c  In a chord
c
          ptsl = 0.
          if (btest(irest(ivx,ip),27)) ptsl = wheadpt1
          ptsadd = max(ptsl,icashft*.05*wheadpt1)
c
c  Note: may have icashft=-20000 (if shftmin=-1000 in crdacc) but that's OK
c 
        else
c
c  Not in a chord
c
          ihshft = 0
          if (isaccs) then
            ihshft = igetbits(nacc(ivx,ip),7,10)
c            if (ihshft .ne. 0) ihshft = max(0,64-ihshft)
            if (ihshft .ne. 0) ihshft = max(0,107-ihshft)
          end if
c
c Check for left-shifted main note
c
          if (btest(ipl(ivx,ip),8)) ihshft = max(20,ihshft)
          ptsadd = ihshft*.05*wheadpt1
        end if
        pts = pts+ptsadd
        if (isgrace) ptgr(ig) = ptgr(ig)+ptsadd
        if (iscln) ptclef(ivx) = ptclef(ivx)+ptsadd
c
c  Left-shifted, non-chord note before?
c
        if (ip .gt. 1) then
          if (.not.btest(ipl(ivx,ip-1),10) .and. 
     *         btest(irest(ivx,ip-1),27)) pts = pts-wheadpt1
        end if
c
c  Try big accidentals first
c
        ptbneed = pts
        if (isaccs .or. iscacc) then
          ptbneed = ptbneed+wheadpt1*bacfac
        end if
        if (poefa(iline)*poenom*esk .gt. ptbneed) then
c
c  Set flag for big accidental
c
          if (isacc) nacc(ivx,ip) = ibset(nacc(ivx,ip),3)
          go to 99
        end if
c
c  Cannot use big, so try small
c
        ptsneed = pts
        if (isaccs .or. iscacc) then
          ptsneed = ptsneed+taccfac*wheadpt1
        end if
        if (poefa(iline)*poenom*esk .lt. ptsneed) then
          call addask(to(in),ptsneed,poefa(iline)*esk,
     *           fixednew,scaldold,0.,poefa(iline),.false.)
        end if
99      continue
        if (btest(iornq(ivx,ip),26)) then
c
c  User-defined space.  Warning, "zero" may change value in addask!
c
          zero = 0.
          call addask(to(in),ptsneed,zero,
     *                fixednew,scaldold,tglp1,1.,.true.)
        end if
c
c  End of big manual loop over "in" for accidental checking
c
        go to 111
112     continue
c
c End of ask analysis for this block.  
c
c Adjust eskz if there are added spaces. Corrects length of xtup brackets.
c
        if (naskb.gt.0) call adjusteskz(ib,istart,poenom)
c 
c Check for internal repeat or sig change.
c
        if (ib.gt.1 .and. ivxo(istart(ib)).eq.1) then
          iirpt = iand(islur(1,ipo(istart(ib))),67109216)
          if (iirpt .gt. 0) then
c
c Internal repeat
c
            if (islast) write(11,'(a)')sq//'advance'//sq//'barno-1%'
            if (iirpt .eq. 96) then
              if (islast) write(11,'(a)')sq//'leftrightrepeat'
              fixednew = fixednew+lrrptfac*wheadpt
            else if (btest(iirpt,5)) then
              if (islast) write(11,'(a)')sq//'leftrepeat'
              fixednew = fixednew+rptfac*wheadpt
            else if (btest(iirpt,6)) then
              if (islast) write(11,'(a)')sq//'rightrepeat'
              fixednew = fixednew+rptfac*wheadpt
            else if (btest(iirpt,8)) then
              if (islast) write(11,'(a)')sq//'doublebar'
            else
              print*,'Unexpected mid-bar repeat command R*'
              call stop1()
            end if
            scaldold = scaldold-fbar
          end if
          if (btest(ipl(1,ipo(istart(ib))),28)) then
c
c  Internal signature change.
c
            notexq = sq//'generalsignature{'
            lnote = 18
            if (isig .lt. 0) then
              notexq = notexq(1:lnote)//'-'
              lnote = lnote+1
            end if
            if (islast) write(11,'(a)')notexq(1:lnote)
     *          //chax(48+abs(isig))//'}%'
            if (islast .and. ignorenats) 
     *             write(11,'(a)')sq//'ignorenats%'              
            if (islast) write(11,'(a)')sq//'zchangecontext'//sq
     *          //'addspace{-.5'//sq//'afterruleskip}%'
            lnote = 0
          end if
        end if
        flgndb = .false.
c
c  Done with start-of-block stuff.  Begin main loop over voices.
c
        do 11 iv = 1 , nv
        do 11 kv = 1 , nvmx(iv)
          ivx = ivmx(iv,kv)
          icm = midchan(iv,kv)
c
c  A rather klugey way to set flag for figure in this voice
c  Must always check figbass before figchk.
c
          if (figbass) then
            ivf = 0
            if (ivx .eq. 1) then
              ivf = 1
            else if (ivx .eq. ivxfig2) then
              ivf = 2
            end if
            if (ivf.gt.0) figchk(ivf) = nfigs(ivf).gt.0
          end if
          if (ivx .gt. 1) then
            if (ivx .le. nv) then
              call addstr(sepsymq(iv-1),1,soutq,lsout)
            else
              call addstr(sq//'nextvoice',10,soutq,lsout)
            end if
          end if
          if (ihdht.gt.0 .and. ivx.eq.nv) then
c
c  Write header.  First adjust height if needed to miss barno.
c
            if (bar1syst .and. iline.ne.1) then
              ihdht = 15+irzbnd+isnx
            end if
c
c  Add user-defined vertical shift
c
            ihdht = ihdht+ihdvrt
            lchead = lenstr(headrq,80)
            notexq = sq//'zcharnote{'
            write(notexq(12:13),'(i2)')ihdht
            notexq = notexq(1:13)//'}{'//sq//'bigfont'//sq//'kern-30pt '
            call addstr(notexq,34,soutq,lsout)
            call addstr(headrq(1:lchead)//'}',1+lchead,soutq,lsout)
            ihdht = 0
          end if
          if (lower .and. ivx.eq.nv) then
            lclow = lenstr(lowerq,80)
            call addstr(sq//'zcharnote{-6}{'//
     *        sq//'tempo'//sq//'kern-10pt '//
     *        lowerq(1:lclow)//'}',33+lclow,soutq,lsout)
            lower=.false.
          end if
          tnow = tstart(ib)
          nofirst = .true.
c
c  Done setting up voice ivx for start of block ib.  Loop over notes in voice.
c
          do 10 jn = istart(ib), istop(ib)
            if (ivxo(jn) .ne. ivx) go to 10
            ip = ipo(jn)
c
c  May have problem with not initializing islhgt, so do it here
c
            islhgt = 0
c
            if (nofirst) then
              noctup = 0
              if (ncmid(iv,ip) .eq. 23) noctup = -2
              nofirst = .false.
            end if
c
c  Check for internal floating figure (before last note of group).
c
12          if (figbass) then
              if (ivx.eq.1 .or. ivx.eq.ivxfig2) then
                ivf = 1
                if (ivx .gt. 1) ivf = 2
                if (figchk(ivf) .and. itfig(ivf,ifig(ivf)).lt.tnow-tol) 
     *                            then
c
c  Bypassed figure location. Backup, place fig, return.
c
                  offnsk = (tnow-itfig(ivf,ifig(ivf)))/space(ib)
                  call putfig(ivf,ifig(ivf),offnsk,figchk(ivf),soutq,
     *              lsout)
                  go to 12
                end if
              end if
            end if
c
c  Put in \sk if needed
c
            if (to(jn) .gt. tnow+tol) then
              call addstr(sq//'sk',3,soutq,lsout)
              tnow = tnow+space(ib)
              go to 12
            end if
c
c  Check for user-defined shifts
c
            if (btest(irest(ivx,ip),15).or.btest(irest(ivx,ip),16))
     *             call putshft(ivx,.true.,soutq,lsout)
21          if (iaskb(ivx).le.naskb .and.
     *            tnow.gt.task(iaskb(ivx))-tol) then
              if (task(iaskb(ivx)) .gt. tstart(ib)-tol) then
c
c  Insert placeholder for accidental skip
c
                call addstr(sq//'ask     ',9,soutq,lsout)
                nasksys = nasksys+1
                wasksys(nasksys) = wask(iaskb(ivx))
                if (wask(iaskb(ivx)) .gt. 0.) then
                  elasksys(nasksys) = elask(iaskb(ivx))
                else
c
c  This is a signal to permit negative ask's.  Should really have elask>=0.
c
                  elasksys(nasksys) = -elask(iaskb(ivx))
                end if
              end if
c
c  May have skipped some task's in earlier blocks (due to void voice)
c
              iaskb(ivx) = iaskb(ivx)+1
              go to 21
            end if
            if (figbass) then
              if (ivx.eq.1 .or. ivx.eq.ivxfig2) then
                ivf = 1
                if (ivx .gt. 1) ivf = 2
                if (figchk(ivf) .and. 
     *                 abs(itfig(ivf,ifig(ivf))-tnow).lt.tol) then
c
c  Figure on a note.  NB: later special check for late figs.
c
                  call putfig(ivf,ifig(ivf),0.,figchk(ivf),soutq,lsout)
                end if
              end if
            end if
c
c  Check for new clef here.
c
            if (isclef .and. btest(islur(ivx,ip),11)) then
              if (ptclef(iv) .gt. 0.) then
                notexq = sq//'off{-'
                if (ptclef(iv) .lt. 9.95) then
                  write(notexq(7:9),'(f3.1)')ptclef(iv)
                  lnote = 9
                else
                  write(notexq(7:10),'(f4.1)')ptclef(iv)
                  lnote = 10
                end if
                notexq = notexq(1:lnote)//'pt}'
                lnote = lnote+3
                call addstr(notexq,lnote,soutq,lsout)
              end if
              call clefsym(islur(iv,ip),notexq,lnote,nclef)
c
c 151220 If clef is treblelowoct, change '0' in pos'n 9 to '8'
c
              if (btest(ipl(ivx,ip),2)) notexq = 
     *                   notexq(1:8)//'8'//notexq(10:10)
              call addstr(notexq,lnote,soutq,lsout)
              if (ptclef(iv) .gt. 0.) then
                notexq = sq//'off{'
                if (ptclef(iv) .lt. 9.95) then
                  write(notexq(6:8),'(f3.1)')ptclef(iv)
                  lnote = 8
                else
                  write(notexq(6:9),'(f4.1)')ptclef(iv)
                  lnote = 9
                end if
                notexq = notexq(1:lnote)//'pt}'
                lnote = lnote+3
                call addstr(notexq,lnote,soutq,lsout)
              end if
            end if
c
c  Checking for literal TeX string BEFORE starting beams!!
c
            if (btest(islur(ivx,ip),16)) then
              do 124 il = 1 , nlit
                if (iplit(il).eq.ip .and. ivlit(il).eq.ivx) go to 125
124           continue
              print*,'Problem finding index for literal string'
              call stop1()
125           continue
c
c  Write a type 1 tex string.
c
              if (lenlit(il) .lt. 71) then
c
c  Add normally
c
                call addstr(litq(il),lenlit(il),soutq,lsout)
              else
c
c  Longer than 71.  Write souq, Write string, start new soutq.
c
                if (islast) write(11,'(a)')soutq(1:lsout)//'%'
                if (islast) write(11,'(a)')litq(il)(1:lenlit(il))//'%'
                lsout = 0
              end if
            end if
c
c  Arpeggio on a main (non-chordal) note?
c
            if (btest(iornq(ivx,ip),27)) then
c              call putarp(tnow,iv,ip,nolev(ivx,ip),ncmid(iv,ip),
              call putarp(tnow,ivx,ip,nolev(ivx,ip),ncmid(iv,ip),
     *                    soutq,lsout)
            end if
c
c  See if a beam starts here
c
            if ((numbms(ivx).gt.0 .and. ibmcnt(ivx).le.numbms(ivx)
     *          .and. ibm1(ivx,ibmcnt(ivx)) .eq. ip) .or. 
     *          btest(nacc(ivx,ip),21)) then
              if (.not.btest(irest(ivx,ip),24)) then
c
c  Not a jump start
c
                if (kbdrests .and. btest(irest(ivx,ip),0) .and.
     *            .not.btest(islur(ivx,ip),29).and. nvmx(iv).eq.2 .and. 
     *            nolev(ivx,ip).le.50) 
     *            call chkkbdrests(ip,iv,ivx,nn,islur,irest,nolev,
     *                ivmx,nib,nv,ibar,tnow,tol,nodur,2,levtopr,levbotr,
     *                mult,ipl)
                call beamstrt(notexq,lnote,nornb,ihornb,space,squez,ib)
c
c  Shift beam start if notehead was shifted
c
                if (btest(ipl(ivx,ip),8)) then
                  call addstr(sq//'loff{',6,soutq,lsout)
                else if (btest(ipl(ivx,ip),9)) then
                  call addstr(sq//'roff{',6,soutq,lsout)
                end if
                if (lnote .gt. 0) call addstr(notexq,lnote,soutq,lsout)
                if (btest(ipl(ivx,ip),8) .or.
     *            btest(ipl(ivx,ip),9)) call addstr('}',1,soutq,lsout)
              else
c
c  Jump start.  Set marker for second part of a jump beam. Note ivbj2 was set 
c  to 0 at end of first part of jump beam
c
                ivbj2 = ivx
c
c  Check for xtup since we bypassed beamstrt wherein vxtup is normally set
c
                if (btest(irest(ivx,ip),28) .and. ixrest(ivx).ne.2)
     *                                             vxtup(ivx) = .true.
c
c  Since beamstrt is not called, and drawbm is normally set there, need to set
c    it here.  This could cause problems if someone tries a staff-jumping,
c    unbarred beam, which I'll deal with when it comes up.
c
                drawbm(ivx) = .true.
              end if
              if (ixrest(ivx) .eq. 0) then
                beamon(ivx) = .true.
                bspend = .true.
                if (.not.btest(irest(ivx,ip),24))bspend = .true.
              end if
            end if
c
c  Setup for chords and possible slurs in chords
c
            if (btest(ipl(ivx,ip),10)) then
c
c  There is a chord on this note. Just rerun precrd. Klunky, but saves
c    me from tracking down errors instroduced when I moved 1st call 
c    forward for accidental spacing analysis.
c
              if (beamon(ivx)) then 
                call precrd(ivx,ip,nolev(ivx,ip),nacc(ivx,ip),
     *            ipl(ivx,ip),irest(ivx,ip),ulq(ivx,ibmcnt(ivx)),
     *            .true.,icashft)
              else
                call precrd(ivx,ip,nolev(ivx,ip),nacc(ivx,ip),
     *              ipl(ivx,ip),irest(ivx,ip),udqq(nolev(ivx,ip),
     *               ncmid(iv,ip),islur(ivx,ip),nvmx(iv),ivx,nv),
     *              .true.,icashft)
              end if
            end if
c
c  Is there slur or grace activity?
c
            isgrace = btest(islur(ivx,ip),4)
            if (ip .gt. 1) isgrace = isgrace.or.btest(ipl(ivx,ip-1),31)
c
c  isgrace if not 1st note in bar and previous note has Way-after grace.
c
            if (btest(islur(ivx,ip),0) .or. isgrace) then
              if (btest(islur(ivx,ip),0)) then
                if (fontslur) then
c
c  Call routine for non-postscript slurs
c
                  call doslur(nolev(ivx,ip),isdat1,isdat2,isdat3,nsdat,
     *           ip,iv,kv,nv,beamon(ivx),ncmid(iv,ip),soutq,lsout,
     *           ulq(ivx,ibmcnt(ivx)),islur(ivx,ip),ipl(ivx,ip),
     *           iornq(ivx,ip),islhgt,tnote(ipl2(ivx,ip)),
     *           nacc(ivx,ip))
                else
c
c  Postscript slurs
c
                 call dopsslur(nolev(ivx,ip),isdat1,isdat2,isdat3,
     *           isdat4,nsdat,
     *           ip,iv,kv,nv,beamon(ivx),ncmid(iv,ip),soutq,lsout,
     *           ulq(ivx,ibmcnt(ivx)),islur(ivx,ip),ipl(ivx,ip),
     *           iornq(ivx,ip),islhgt,tnote(ipl2(ivx,ip)),
     *           nacc(ivx,ip))
                end if
	        end if
              if (isgrace) then
c
c Grace note.
c
                iphold = ip
                isgrace = .false.
                if (ip .gt. 1) isgrace = btest(ipl(ivx,ip-1),31)
                if (isgrace) iphold = iphold-1
                isgrace = isgrace .or. (.not.btest(ipl(ivx,ip),31)
     *            .and..not.btest(ipl(ivx,ip),29))
c
c Place grace now if (a) Way-after from prev note and ip>1 or (b) Pre-grace
c   on current note.  Do A-grace on current note, and W-grace at barend, later.
c
                if (isgrace) then
                  call dograce(ivx,iphold,ptgr,soutq,lsout,ncmid(iv,ip),
     *                 nacc(ivx,ip),ig,ipl(ivx,iphold),.false.,
     *                 beamon(ivx),nolev(ivx,ip),ncmid(iv,ip),
     *                 islur(ivx,ip),nvmx(iv),nv,ibmcnt(ivx),
c 130324
c     *                 tnote(ipl2(ivx,ip)),ulq)
     *                 tnote(ipl2(ivx,ip)),ulq,instno(iv))
                  if (slurg(ig)) then
c
c Terminate slur started in dograce.  Get direction of main note stem
c
                    if (.not.beamon(ivx)) then
c
c  Separate note.  Get stem direction.
c
                      stemup = udqq(nolev(ivx,ip),ncmid(iv,ip),
     *                     islur(ivx,ip),nvmx(iv),ivx,nv) .eq. 'u'
                    else
c
c  In a beam
c
                      stemup = ulq(ivx,ibmcnt(ivx)) .eq. 'u'
                    end if
c
c  Stop the shift if whole note
c
                    stemup = stemup .or. 
     *                       tnote(ipl2(ivx,ip)).gt.63
                    call endslur(stemup,.not.upg(ig),nolev(ivx,ip),0,
     *                 ndxslur,0,ncmid(iv,ip),soutq,lsout,fontslur)
                  end if
                end if
              end if
              if (btest(iornq(ivx,ip),24)) then
c
c  Start slur on main note for After- or Way-after-grace.
c
c????                ndxslur = log2(33554431-listslur)
                ndxslur = log2(16777215-listslur)
c
c  Get note name
c
                call notefq(noteq,lnoten,nolev(ivx,ip),ncmid(iv,ip))
c
c  Get slur direction
c
                slurudq = 'u'
                if (.not.beamon(ivx)) then
                  if (udqq(nolev(ivx,ip),ncmid(iv,ip),islur(ivx,ip),
     *                  nvmx(iv),ivx,nv) .eq. 'u') slurudq = 'd'
                else
                   if (ulq(ivx,ibmcnt(ivx)) .eq. 'u') slurudq = 'd'
                end if
c
cc  Replace ndxslur by 11-ndxslur when printing only.
c  Replace ndxslur by 23-ndxslur when printing only.
c
c                if (11-ndxslur .lt. 10) then
                if (23-ndxslur .lt. 10) then
c                  notexq = sq//'islur'//slurudq//chax(59-ndxslur)
                  notexq = sq//'islur'//slurudq//chax(71-ndxslur)
     *              //noteq(1:lnoten)
                  call addstr(notexq,8+lnoten,soutq,lsout)
                else if (23-ndxslur .lt. 20) then
                  notexq = sq//'islur'//slurudq//'{1'//chax(61-ndxslur)
     *              //'}'//noteq(1:lnoten)
                  call addstr(notexq,11+lnoten,soutq,lsout)
                else
                  notexq = sq//'islur'//slurudq//'{2'//chax(51-ndxslur)
     *              //'}'//noteq(1:lnoten)
                  call addstr(notexq,11+lnoten,soutq,lsout)
                end if
c                call setbits(ipl(ivx,ip),4,23,ndxslur)
                call setbits(ipl(ivx,ip),5,23,ndxslur)
                if (btest(ipl(ivx,ip),31))
c
c  Starting slur on W-grace on THIS note.  Record ndxslur.
c
     *                listslur = ibset(listslur,ndxslur)
              end if
            end if
c
c  Process dynamic marks
c
            if (btest(irest(ivx,ip),26)) then
		    call dodyn(ivx,ip,
     *          nolev(ivx,ip),ncmid(iv,ip),ipl(ivx,ip),islur(ivx,ip),
     *          irest(ivx,ip),nvmx(iv),nv,beamon(ivx),ihornb,nornb,ulq,
     *          ibmcnt(ivx),nodur(ivx,ip).ge.64,soutq,lsout)
            end if
c
c  Check for chord notes.  Moved up from below, 10/27/96 so chord orns done 1st.
c
            if (btest(ipl(ivx,ip),10)) then
c
c  Need a duration to set type of note head
c
c  Clumsy test, but vxtup is not set until main note is processed.
c
              if (.not.(vxtup(ivx).or.btest(irest(ivx,ip),28))) then
                nodu = nodur(ivx,ip)
              else if (btest(irest(ivx,ip),2) .or. 
     *                 (ip.gt.1.and.btest(irest(ivx,ip-1),2))) then
c
c In a 2-note tremolo
c               
                if (btest(irest(ivx,ip),2)) then
c
c First note of tremolo, duration is on next note
c
                  nodu = nodur(ivx,ip+1)
                else
                  nodu = nodur(ivx,ip)
                end if
              else if (iand(mult(ivx,ip),15)-8 .lt. 0) then
                nodu = 32
              else
                nodu = 16
              end if
              call docrd(ivx,ip,nodu,ncmid(iv,ip),iv,tnow,soutq,lsout,
     *                   ulq,ibmcnt(ivx),islur(ivx,ip),nvmx(iv),nv,
     *                   beamon(ivx),nolev(ivx,ip),ihornb,nornb,stemlen,
     *                   btest(nacc(ivx,ip),27),nacc(ivx,ip),
     *                   irest)
            end if
c
c  Now that chords are done, add stuff to midi file
c
            if (ismidi) call addmidi(icm,
c 130316
     *          nolev(ivx,ip)+miditran(instno(iv)),
     *          iand(nacc(ivx,ip),7),midisig,
     *          tnote(ipl2(ivx,ip)),
     *          btest(irest(ivx,ip),0),.false.)
c
c  Check for breath or caesura
c
            if (btest(iornq(ivx,ip),28)) then
              call putcb(ivx,ip,notexq,lnote)
              call addstr(notexq,lnote,soutq,lsout)
            end if
c
c  Check for main-note ornaments. ')' on dotted notes go in with note, not here.
c  Bits 0-13: (stmgx+Tupf._) ; 14: Down fermata, was F
c  15: Trill w/o "tr", was U , 16-18 edit. accid., 19-21 TBD
c
c            isacc = iand(iornq(ivx,ip),4194287) .gt. 0
c            isacc = iand(iornq(ivx,ip),541065199) .gt. 0
            isacc = iand(iornq(ivx,ip),1614807023) .gt. 0
c
c  isacc=.true. if any ornament except segno
c
            if (btest(iornq(ivx,ip),13) .and. nodur(ivx,ip).gt.0) then
c
c  If ).  is only ornament, bypass.  If with others, temporarirly zero the bit.
c
              if (2**log2(nodur(ivx,ip)) .ne. nodur(ivx,ip)) then
                if (iand(iornq(ivx,ip),516079) .eq. 0) then
c
c  ). is the only non-segno ornament
c
                  isacc = .false.
                else
c
c  There are other ornaments in addition
c
                  rpndot = .true.
                  iornq(ivx,ip) = ibclr(iornq(ivx,ip),13)
                end if
              end if
            end if
            if (isacc .and. .not.cwrferm(ivx)) then
c
c  Check for centered whole-bar rest with fermata (bits 10 or 14).
c
              if (iand(iornq(ivx,ip),17408).gt.0
     *                               .and. btest(irest(ivx,ip),0) .and.
     *             nodur(ivx,ip).eq.lenbar .and.
     *             .not.(firstgulp.and.ibar.eq.1.and.lenb0.gt.0)) then
                cwrferm(ivx) = .true.
                go to 30
              end if
              call putorn(iornq(ivx,ip),nolev(ivx,ip),nolev(ivx,ip),
     *             nodur(ivx,ip),nornb,ulq,ibmcnt(ivx),ivx,ncmid(iv,ip),
     *             islur(ivx,ip),nvmx(iv),nv,ihornb,stemlen,
     *             notexq,lnote,ip,islhgt,beamon(ivx),
     *             btest(ipl(ivx,ip),10))
              call addstr(notexq,lnote,soutq,lsout)
            end if
            if (rpndot) then
              iornq(ivx,ip) = ibset(iornq(ivx,ip),13)
              rpndot = .false.
            end if
30          continue
c
c  Check for main note accidental
c
            if (iand(nacc(ivx,ip),3).gt.0 .and.
     *            .not.btest(nacc(ivx,ip),17)) then
              ihshft = igetbits(nacc(ivx,ip),7,10)
              if (ihshft .ne. 0) ihshft=ihshft-107
              if (.not.btest(ipl(ivx,ip),10) .and. btest(ipl(ivx,ip),8)) 
c
c Not a chord, and left-shifted main note, so left-shift accid
c
     *               ihshft = ihshft-20
              call doacc(ihshft,igetbits(nacc(ivx,ip),6,4),
     *           notexq,lnote,nacc(ivx,ip),nolev(ivx,ip),ncmid(iv,ip),
     *           btest(irest(ivx,ip),31))
              call addstr(notexq,lnote,soutq,lsout)
            end if
c
c  Lower dot for lower-voice notes.  Conditions are:
c   1. Dotted time value
c   2. Lower voice of two
c   3. Note is on a line
c   4. Not a rest
c.  5. Flag (lowdot) is set to true
c   6. Not in an xtuplet
c
            if (lowdot .and. nvmx(iv).eq.2 .and. ivx.le.nv
     *           .and. nodur(ivx,ip).ne.0) then
              if (.not.btest(irest(ivx,ip),0) .and.
     *            2**log2(nodur(ivx,ip)).ne.nodur(ivx,ip) .and.
     *            mod(nolev(ivx,ip)-ncmid(ivx,ip),2).eq.0) then
                if (btest(irest(ivx,ip),19)) then
c
c  Note already in movdot list.  Drop by 2.
c
                  updot(ivx,ndotmv(ivx)+1) =
     *                   updot(ivx,ndotmv(ivx)+1)-2.
                else
c
c  Not in list so just move it right now
c
                  call dotmov(-2.,0.,soutq,lsout,
     *                          igetbits(islur(ivx,ip),1,3))
                end if
              end if
            end if
c
c  Check for dotted main notes with moved dots. Chord notes done elsewhere.
c  Added check rules out special chordal 2nds, but later must check 
c  substituted chord note for dot shift that now applies to main note.
c
            if (btest(irest(ivx,ip),19)) then
              ndotmv(ivx) = ndotmv(ivx)+1
              call dotmov(updot(ivx,ndotmv(ivx)),rtdot(ivx,ndotmv(ivx)),
     *           soutq,lsout,igetbits(islur(ivx,ip),1,3))
            end if
c
c  Stemlength changes
c
            if (btest(mult(ivx,ip),27)) then
              dstemlen = igetbits(mult(ivx,ip),6,10)*.5-4. 
              call addstr(sq//'stdstemfalse',13,soutq,lsout)
              stemshort = 4.66+.667*dstemlen
              call addstr(sq//'stemlength{',12,soutq,lsout)
              write(notexq,'(f4.1)')stemshort
              call addstr(notexq(1:4)//'}',5,soutq,lsout)
            else if (ip .gt. 1) then
              if (btest(mult(ivx,ip-1),27))
c
c  Cancel shortening.  Looks like it gets automatically restored if new inst. or
c    new line, so no need to worry about affecting other lines
c
     *           call addstr(sq//'stemlength{4.66}',17,soutq,lsout)
            end if
c
c  Zero out slur-height marker for raising ornaments
c
            islhgt = 0
c
c  Now start with spacing notes.  Is a beam start pending?
c
            if (bspend .and.
     *            ibm2(ivx,ibmcnt(ivx)).gt.ibm1(ivx,ibmcnt(ivx))) then
              if (ixrest(ivx) .eq. 4) then
c
c  Special path for single note at end of otherwise beamed xtup
c
                ixrest(ivx) = 0
              else
                if (kbdrests .and. btest(irest(ivx,ip),0) .and.
     *            .not.btest(islur(ivx,ip),29).and. nvmx(iv).eq.2 .and. 
     *            nolev(ivx,ip).le.50) 
     *            call chkkbdrests(ip,iv,ivx,nn,islur,irest,nolev,
     *                ivmx,nib,nv,ibar,tnow,tol,nodur,2,levtopr,levbotr,
     *                mult,ipl)
                if (btest(islur(ivx,ip),29) 
     *               .and.btest(irest(ivx,ip),24)) then
                  notexq = chax(92)//'sk'
                  lnote = 3
                else
                  call beamn1(notexq,lnote)
c                  if (isbjmp .and. 
                  if (isbjmp .and. ivbj2.ne.0 .and.
     *                iand(mult(ivx,ip),15)-8 .gt. multbj1) then
c
c  Need to increase multiplicity at the beam jump
c      
c  ibmcnt was increased by 1 at the end of first seg of jump beam!? 
c  Try adding e.g. \nbbu1 if needed to increase multiplicity. Add
c  character in reverse order to start of notexq.
c
                    if (ivbj1 .lt. 9) then
                      notexq = char(48+ivbj1)//notexq(1:lnote)
                      lnote = lnote+1
                    else
                      notexq = '{1'//char(38+ivbj1)//'}'
     *                          //notexq(1:lnote)
                      lnote = lnote+4
                    end if
                    notexq = 'bb'//ulq(ivbj1,ibmcnt(ivbj1)-1)
     *                          //notexq(1:lnote)
                    lnote = lnote+3
                    if (iand(15,mult(ivx,ip))-8 .eq. 3) then
                      notexq = 'b'//notexq(1:lnote)
                      lnote = lnote+1
                    end if
                    notexq = sq//'n'//notexq(1:lnote)
                    lnote = lnote+2
                  end if
                end if
              end if
              bspend = .false.
c
c  Is a beam ending?
c
            else if (numbms(ivx).gt.0 .and. ibmcnt(ivx).le.numbms(ivx)
     *           .and. (ibm2(ivx,ibmcnt(ivx)).eq.ip .or.
     *             btest(nacc(ivx,ip),20))) then
              if (bspend) then
c
c  Must be a single-note ending of a jump-beam
c
                bspend = .false.
              end if
              call beamend(notexq,lnote)
              if (isbjmp .and. ivx.eq.ivbj1) then
c
c  Jump beam segment is ending, check if multiplicity DECREASES
c
                if (space(ib+1) .gt. space(ib)) then
c
c  Decrease multiplicity 3-1, 3-2, or 2-1. Spaces are 2,4, or 8.
c
                  if (ivx .lt. 10) then
                    ivxq = char(ivx+48)
                    livx = 1
                  else
                    ivxq = '{1'//char(ivx-10)//'}'
                    livx = 4
                  end if
c  Changes to get staffcrossall.pmx to work. Not clear how.
c                  if (nint(space(ib+1)-space(ib)) .eq. 6) then
                  if (space(ib+1).eq.8..and.space(ib).eq.2.) then
                    notexq = sq//'tbbb'//ulq(ivx,ibmcnt(ivx))//
     *               ivxq(1:livx)//sq//'tbb'//
     *               ulq(ivx,ibmcnt(ivx))//ivxq(1:livx)//notexq(1:lnote)
                    lnote = lnote+11+2*livx
c                  else if (nint(space(ib+1)-space(ib)) .eq. 4) then
                  else if (space(ib+1).eq.8..and.space(ib).eq.4.) then
                    notexq = sq//'tbb'//ulq(ivx,ibmcnt(ivx))//
     *               ivxq(1:livx)//notexq(1:lnote)
                    lnote = lnote+5+livx
c                  else 
                  else if (space(ib+1).eq.4..and.space(ib).eq.2.) then
                    notexq = sq//'tbbb'//ulq(ivx,ibmcnt(ivx))//
     *               ivxq(1:livx)//notexq(1:lnote)
                    lnote = lnote+6+livx
                  end if
                end if
              end if              
              if (.not.btest(nacc(ivx,ip),20)) then
                vxtup(ivx) = .false.
                nornb(ivx) = 0
                ibmcnt(ivx) = ibmcnt(ivx)+1
                beamon(ivx) = .false.
              end if
c
c  Or if we're in the middle of a beam
c
            else if (numbms(ivx).gt.0 .and. beamon(ivx)) then
c
c  Added 130127 
c          
              if (kbdrests .and. btest(irest(ivx,ip),0) .and.
     *            .not.btest(islur(ivx,ip),29).and. nvmx(iv).eq.2 .and. 
     *            nolev(ivx,ip).le.50) 
     *            call chkkbdrests(ip,iv,ivx,nn,islur,irest,nolev,
     *                ivmx,nib,nv,ibar,tnow,tol,nodur,2,levtopr,levbotr,
     *                mult,ipl)
              call beamid(notexq,lnote)
c
c      Or whole-bar rest
c
            else if (btest(irest(ivx,ip),0)
     *          .and. nodur(ivx,ip).eq.lenbar
     *          .and..not.(firstgulp.and.ibar.eq.1.and.lenb0.gt.0)
     *          .and..not.btest(irest(ivx,ip),25)
     *          .and..not.btest(islur(ivx,ip),29)) then
c
c  Rule out pickup bar, blank rests, non-centered.  Remember islur b19=> rp
c
              cwrest(ivx) = .true.
              iscwr = .true.
              call notex(cwrq(ivx),lcwr(ivx))
              tnow = tnow+lenbar
              go to 10
            else if (ixrest(ivx) .eq. 0) then
c
c  Before writing note or rest, check for keyboard rest height adjustment. 
c  Conditions are 0. This is a non-blank rest 
c                 1. kbdrests = .true.
c                 2. There are two voices on the staff
c                 3. No user-def height adjustments have been applied (nolev<50)
c
              if (kbdrests .and. btest(irest(ivx,ip),0) .and.
     *            .not.btest(islur(ivx,ip),29).and. nvmx(iv).eq.2 .and. 
     *             nolev(ivx,ip).le.50) then
                call chkkbdrests(ip,iv,ivx,nn,islur,irest,nolev,
     *            ivmx,nib,nv,ibar,tnow,tol,nodur,1,levtopr,levbotr,
     *                mult,ipl)
              end if
              if (btest(ipl(ivx,ip),4)) then
c
c  Deal with single stem tremolo
c  Get up-downness, borrowed from notex
c
                if (udqq(nolev(ivx,ip),ncmid(iv,ip),islur(ivx,ip),
     *               nvmx(iv),ivx,nv).eq.'u') then
                  udq = 'u'
                else
                  udq = 'l'
                end if
                multtrem = igetbits(ipl(ivx,ip),2,5)+1
                if (nodur(ivx,ip) .lt. 64) then
                  notexq = sq//'tr'
                else
                  notexq = sq//'Tr'
                end if
                lnote = 3
                if (multtrem .eq. 2) then
                  notexq = notexq(1:3)//'r'
                  lnote = 4
                else if (multtrem .eq.3) then
                  notexq = notexq(1:3)//'rr'
                  lnote = 5
                end if
c
c Get a numerical pitch argument
c
                lineno = nolev(ivx,ip)-ncmid(iv,ip)+4
                if (lineno .lt.0 .or. lineno .gt.9) then
                  write(noteq,'(a1,i2,a1)')'{',lineno,'}'
                  lnoten = 4
                else
                  noteq = char(lineno+48)
                  lnoten = 1
                end if
                notexq = notexq(1:lnote)//'m'//udq//noteq(1:lnoten)
                lnote = lnote+2+lnoten
                call addstr(notexq,lnote,soutq,lsout)
              end if
c
c  Get code for a separate note or rest
c  If nacc(ivx,ip)(30-31)=1 (dotted chordal 2nd including main note), 
c    pitch will be shifted +/-1 inside notex.
c
              call notex(notexq,lnote)
            end if
c
c  Right offset?  This may cause trouble
c
            if (btest(ipl(ivx,ip),8)) then
              call addstr(sq//'loff{',6,soutq,lsout)
            else if (btest(ipl(ivx,ip),9)) then
              call addstr(sq//'roff{',6,soutq,lsout)
            end if
            if (ixrest(ivx).eq.0 .and. lnote.gt. 0) then
              call addstr(notexq,lnote,soutq,lsout)
            end if
            if (btest(ipl(ivx,ip),8) .or.
     *          btest(ipl(ivx,ip),9)) call addstr('}',1,soutq,lsout)
c
c  Terminate user-defined offsets.  Fix format
c
            if (btest(irest(ivx,ip),15) .or. btest(irest(ivx,ip),17))
     *          call putshft(ivx,.false.,soutq,lsout)
c
c  Deal with After- and Way-after-graces.  First, if end of bar, compute space
c    needed since it wasn't done during general ask-checks. If extra space is
c    rq'd, convert GW to GA.  Therefore GW at end of bar never needs extra sp.
c    But will still need to add extra space as hardspace.
c
            if (ip.eq.nn(ivx) .and. (btest(ipl(ivx,ip),31).or.
     *                               btest(ipl(ivx,ip),29))) then
              do 77 ig = 1 , ngrace
                if (ipg(ig).eq.ip .and. ivg(ig).eq.ivx) go to 78
77            continue
              print*,'Problem finding grace index at "do 77"'
              call stop1()
78            continue
c
c  Get elemskip to end of bar.  WON'T WORK IF XTUPS !!
c
              esk = 0.
              do 40 iib = ib , nb
                if (iib .eq. ib) then
                  itleft = nint(to(ipl2(ivx,ip)))
                else
                  itleft = nint(tstart(ib))
                end if
                if (iib .lt. nb) then
                  itright = nint(tstart(iib+1))
                else
                  itright = lenbar
                end if
                esk = esk+feon(space(ib))*
     *                          (itright-itleft)/space(ib)
40            continue
              ptsavail = poenom*esk-wheadpt
              if (nng(ig) .eq. 1) then
                wgr = grafac
              else
                wgr = nng(ig)*emgfac
                do 41 ing = 1 , nng(ig)
                  if (naccg(ngstrt(ig)-1+ing) .gt. 0) wgr = wgr+acgfac
41              continue
              end if
              ptgr(ig) = wgr*wheadpt
              ptsneed = (wgr+0.5)*wheadpt
              ptsndg(ivx) = 0.
              if (ptsavail .lt. ptsneed) then
                ptsndg(ivx) = ptsneed
                eskndg(ivx) = esk
                if (btest(ipl(ivx,ip),31)) then
c
c  Convert GW to GA
c
                  ipl(ivx,ip) = ibset(ibclr(ipl(ivx,ip),31),29)
                end if
              end if
            end if
c
c  Check for GA
c
            if (btest(ipl(ivx,ip),29))
     *        call dograce(ivx,ip,ptgr,soutq,lsout,ncmid(iv,ip),
     *            nacc(ivx,ip),ig,ipl(ivx,ip),.false.,
c 130324
c     *            .false.,0,0,0,0,0,0,0.,ulq)
     *            .false.,0,0,0,0,0,0,0.,ulq,instno(iv))
c
c  Update running time
c
            tnow = tnow+space(ib)
10        continue
c
c  Have finished last note in this voice and block
c
          itendb = nint(to(istop(ib))+space(ib))
          if (figbass .and. ivx.eq.1 .or. ivx.eq.ivxfig2) then
            ivf = 1
            if (ivx .gt. 1) ivf = 2
17          if (figchk(ivf) .and. itfig(ivf,ifig(ivf)).lt.itendb) then
c
c  There's at least one figure left. offnsk could be <0
c
              offnsk = (tnow-itfig(ivf,ifig(ivf)))/space(ib)
              call putfig(ivf,ifig(ivf),offnsk,figchk(ivf),soutq,lsout)
              go to 17
            end if
          end if
c
c  Check for flag, dot, or upstem on last note of bar.
c
          if (ib .eq. nb) then
            ip = ipo(ipl2(ivx,nn(ivx)))
            flgndv(ivx) = 0.
            if (abs(tnote(ipl2(ivx,ip))-space(ib)).lt.tol) then
              if (space(ib) .lt. 16.-tol) then
c
c  Note in last space, smaller than a quarter note.
c
                if ((.not.btest(irest(ivx,ip),0) .and.
     *             udqq(nolev(ivx,ip),ncmid(iv,ip),islur(ivx,ip),
     *             nvmx(iv),ivx,nv).eq.'u')
     *            .or. isdotted(nodur,ivx,ip)) then
c
c  Upstem non-rest, or dotted
c
                  if (numbms(ivx).gt.0
     *              .and. ip.eq.ibm2(ivx,max(1,numbms(ivx)))
     *              .and. .not.isdotted(nodur,ivx,ip))
     *               then
c
c  In beam and not dotted, so use smaller space
c
                    flgndv(ivx) = upstmfac
                  else
                    flgndv(ivx) = flagfac
                  end if
                end if
              else
c
c  Last space, nonflagged (no beam) only worry dot or up
c
                if (isdotted(nodur,ivx,ip)) then
                  flgndv(ivx) = flagfac
                else if (tnote(ipl2(ivx,ip)).lt.64 .and.
     *              udqq(nolev(ivx,ip),ncmid(iv,ip),islur(ivx,ip),
     *                   nvmx(iv),ivx,nv).eq.'u') then
c
c  Upstem on last note , non-flagged
c
                  flgndv(ivx) = upstmfac
                end if
              end if
            end if
c
c  Check for right-shifted chordal note
c
            if (btest(irest(ivx,ip),20)) flgndv(ivx) = rtshfac
            flgndb = flgndb .or. flgndv(ivx).gt.0.
            if (ismidi) then
c
c  For midi, set flags for accidentals on last note of bar.  Assume they affect 
c    first note of next bar whether or not tied.  
c  Note has already been done, so next entry into addmidi is 1st in new bar.
c  First do main note, then chord notes
c
c  Gyrations needed to account for multi-bar tied full-bar notes?
cc  Old old    lbacc(icm) = iand(nacc(ivx,ip),7)
c   New old    if (lbacc(icm).eq.0) lbacc(icm) = iand(nacc(ivx,ip),7)
c
              if (iand(nacc(ivx,ip),7) .gt. 0) then
c
c  Explicit accidental on last main note in bar
c
                do 55 kacc = 1 , naccbl(icm)
                  if (laccbl(icm,kacc) .eq. nolev(ivx,ip)) go to 56
55              continue
                naccbl(icm) = naccbl(icm)+1
                laccbl(icm,naccbl(icm)) = nolev(ivx,ip)
                jaccbl(icm,naccbl(icm)) = iashft(iand(nacc(ivx,ip),7))
              end if
56            continue
              if (btest(ipl(ivx,ip),10) .and. crdacc) then
                do 57 icrd = icrd1 , icrd2
                  iacc = igetbits(icrdat(icrd),3,20)
                  if (iacc .gt. 0) then
c
c  Explicit accidental on chord note at end of bar
c
                    nolevc = igetbits(icrdat(icrd),7,12)
                    do 58 kacc = 1 , naccbl(icm)
                      if (laccbl(icm,kacc) .eq. nolevc) go to 59 
58                  continue
                    naccbl(icm) = naccbl(icm)+1
                    laccbl(icm,naccbl(icm)) = nolevc
                    jaccbl(icm,naccbl(icm)) = iashft(iacc)
                  end if
59                continue
57              continue
              end if
c              if (lbacc(icm).eq.0 .and. accb4(icm)) then
              do 65 kacc = 1 , naccim(icm) 
c
c  If naccim(icm)>0, 
c  possible implicit accidental from earlier in the bar.  Check for prior accid 
c  in this bar at relevant note levels, main and chord notes.  Only act if no
c  explicit action from just above.  Assuming any accid on last note in bar,
c  either explicit or implicit, has same effect on 1st note of next bar. 
c
                if (nolev(ivx,ip) .eq. laccim(icm,kacc)) go to 66
                if (btest(ipl(ivx,ip),10)) then
                  do 67 icrd = icrd1 , icrd2
                    if (iand(ishft(icrdat(icrd),-12),127) .eq.
     *                                 laccim(icm,kacc)) go to 66
67                continue
                end if
                go to 65
66              continue
c
c  So far we know there is a main or chord note at level laccim(icm,kacc). So
c   it will get a bl-accid if it didn't just already get one.
c
                do 68 macc = 1 , naccbl(icm)
                  if (laccbl(icm,macc) .eq. laccim(icm,kacc)) go to 65 
68              continue
                naccbl(icm) = naccbl(icm)+1
                laccbl(icm,naccbl(icm)) = laccim(icm,kacc)
                jaccbl(icm,naccbl(icm)) = jaccim(icm,kacc)
65            continue
            end if
          end if
11      continue
c
c  Close out the notes group
c
        call addstr(sq//'en',3,soutq,lsout)
        if (islast.and.lsout .gt. 0) write(11,'(a)')soutq(1:lsout)//'%'
16    continue
c
c  Check for way-after graces at end of bar.  We could not link these to notes
c  as in midbar since there is no note following grace!  Also, set flag if
c  hardspace is needed. Also, save nvmx, ivmx for use in space checks on reloop.
c
      isgrace = .false.
      do 75 iv = 1 , nv
      nvmxsav(iv) = nvmx(iv)
      do 75 kv = 1 , nvmx(iv)
        ivmxsav(iv,kv) = ivmx(iv,kv)
        ivx = ivmx(iv,kv)
        ptsgnd = 0.
        if ((btest(ipl(ivx,nn(ivx)),29).or.btest(ipl(ivx,nn(ivx)),31))
     *      .and. ptsndg(ivx).gt.0.) then
          flgndb = .true.
          if (ptsndg(ivx) .gt. ptsgnd) then
            ptsgnd = ptsndg(ivx)
            eskgnd = eskndg(ivx)
          end if
        end if
        if (btest(ipl(ivx,nn(ivx)),31)) then
c
c  This voice has a way-after grace here at end of bar
c
          if (.not.isgrace) then
c
c  This is the first one, so set up the string
c
            isgrace = .true.
            ivlast = 1
            soutq = sq//'znotes'
            lsout = 7
          end if
          do 76 iiv = ivlast , iv-1
            call addstr(sepsymq(iiv),1,soutq,lsout)
76        continue
          ivlast = iv
c
c  No need to put in 'nextvoice', even if 2 lines/staff
c
          call dograce(ivx,nn(ivx),ptgr,soutq,lsout,ncmid(iv,nn(ivx)),
     *      nacc(ivx,nn(ivx)),ig,ipl(ivx,nn(ivx)),.true.,
c 130324
c     *      .false.,0,0,0,0,0,0,0.,ulq)
     *      .false.,0,0,0,0,0,0,0.,ulq,instno(iv))
        end if
75    continue
      if (isgrace) then
        call addstr(sq//'en%',4,soutq,lsout)
        if (islast .and. lsout .gt. 0) write(11,'(a)')soutq(1:lsout)
      end if
      lsout = 0
c
c  Write multibar rest.  Assuming nv = 1  and do not worry about cwbrest
c  This has to be the only use of atnextbar
c
      if (ibar.eq.ibarmbr .and. islast) then
        if (newmbrhgt) then
          soutq = sq//'def'//sq//'mbrhgt{'
          if (mbrhgt .le. -10) then
            write(soutq(13:15),'(i3)')mbrhgt
            lsout = 15
          else if (mbrhgt.ge.10.or.mbrhgt.le.-1) then        
            write(soutq(13:14),'(i2)')mbrhgt
            lsout = 14
          else
            soutq = soutq(1:12)//char(48+mbrhgt)
            lsout = 13
          end if
          soutq = soutq(1:lsout)//'}%'
          write(11,'(a)')soutq(1:lsout+2)
          newmbrhgt = .false.
        end if
        soutq = sq//'def'//sq//'atnextbar{'//sq//'znotes'
        lsout = 22
        notexq = sq//'mbrest'
        lnote = 7
        call istring(mbrest,noteq,len)
        notexq = notexq(1:lnote)//noteq(1:len)
        lnote = lnote+len
        mtrspc = nint(xb4mbr)
        xb4mbr = 0.
        call istring(mtrspc,noteq,len)
        notexq = notexq(1:lnote)//noteq(1:len)//'0'
        lnote = lnote+len+1
        do 62 iv = 1 , nv
          call addstr(notexq,lnote,soutq,lsout)
          if (iv .lt. nv) call addstr(sepsymq(iv),1,soutq,lsout)
62      continue
        call addstr(sq//'en}%',5,soutq,lsout)        
        write(11,'(a)')soutq(1:lsout)
        lsout = 0
        if (ishort .gt. 0) then 
          mbrsum = mbrsum+mbrest
          nmbr = nmbr+1
        end if
        if (mbrest .gt. 1) then
          ndig = int(alog10(mbrest-1+.01))+1
          write(11,'(a14,i'//chax(48+ndig)//',a1)')
     *             sq//'advance'//sq//'barno',mbrest-1,'%'
        end if
      else if (iscwr) then
c
c  Centered whole-bar rests. Set flag to pass to pmxb at start of NEXT bar
c    to check for new clef and add space by redefining \CenterBar
c
        centrests = .true.
        call addstr(sq//'def'//sq//'value{0}',13,soutq,lsout)
        call addstr(sq//'def'//sq//'atnextbar{'//sq//'znotes',22,
     *              soutq,lsout)
        do 60 iv = 1 , nv
c added
          lnote = 0
c
          do 61 kv = 1 , nvmx(iv)
            ivx = ivmx(iv,kv)
            if (cwrest(ivx)) then
              call addstr(sq//'CenterBar{',11,soutq,lsout)              
              if (.not.cwrferm(ivx)) then
                if (lcwr(ivx).ge.11 .and. cwrq(ivx)(11:11).ne.'p') then
c
c  Kluge to use new definitions for centered, stacked rests
c
                  if (cwrq(ivx)(2:10).eq.'liftpause' .or.
     *                cwrq(ivx)(2:10).eq.'liftPAuse') 
     *                                   cwrq(ivx)(10:10) = 'c'
                end if
                call addstr(cwrq(ivx)(1:lcwr(ivx)),lcwr(ivx),soutq,
     *             lsout)
                call addstr('}{-2}{'//sq//'value}',13,soutq,lsout)
              else
c
c  Fermata on centered rest.  Will need to fix up level.
c  12/6/07 shift it left so it's centered over rest
c
                notexq = notexq(1:lnote)//sq//'loffset{.33}{'
     *                   //sq//'fermataup7}'//cwrq(ivx)(1:lcwr(ivx))
     *                   //'}{-2}{'//sq//'value}'
                lnote = lnote+26+lcwr(ivx)+13
                call addstr(notexq,lnote,soutq,lsout)
                cwrferm(ivx) = .false.
              end if
            end if
61        continue
          if (iv .ne. nv) call addstr(sepsymq(iv),1,soutq,lsout)
60      continue
        if (islast) then
          call addstr(sq//'en}%',5,soutq,lsout)
          if (lsout.gt.0)write(11,'(a)')soutq(1:lsout)
        end if
      end if
c
c  End of block for centered whole-bar rests and multi-bar rests
c
c  If at end of block, save durations of last notes in bar, for possible use
c  if clef changes at start of next bar
c
      if (ibar .eq. nbars) then
        do 63 iv = 1 , nv
        do 63 kv = 1 , nvmx(iv)
          ivx = ivmx(iv,kv)
          prevtn(ivx) = tnote(ipl2(ivx,nn(ivx)))
63      continue
      end if
c
c  Update time for midi.  This is only used for the event track
c
      if (ismidi) then
        miditime = miditime+15*lenbar
c
c  If pickup, write the real time signature to the event track.  Cannot use
c    mtrnuml since it was reset to 0, have to recompute it
c
        if (lenb0 .eq. lenbar) 
     *        call midievent('m',mtrdenl*lenb1/64,mtrdenl)
      end if
      return
      end
      subroutine makeabar()
c
c  On input, have pseudo-durations in nodur(ivx,ip).  Not real durations for
c    xtups, since only last note of xtup gets non-zero nodur, which 
c    corresponds to single note of full length of xtup.
c  In this subroutine we make an ordered list of all notes in all voices.
c    ilnc      = list note counter
c    ivxo(ilnc), ipo(ilnc) = voice# and position in voice of ilnc-th note.
c    to(ilnc)  = real start time of note in PMX-units (64=whole note)
c    tno(ilnc) = time to next event in the bar.   
c    tnote(ilnc) = actual duration of note
c  Then run thru list, grouping consecutive notes into \notes groups ib.  
c    space(ib) = real time unit for the \notes group 
c    squez(ib) = factor on space to get effective space.  This will be 1 if
c                there is a note exactly spanning each interval of space, and
c                <1 if not.  
c  Details:  let eon = elemskips per noteskip (like length).  Basic formula is
c    eon = sqrt(space/2.)  
c  If tgovern >= space, then 
c    eon = sqrt(tgovern/2)*(space/tgovern) = space/sqrt(2*tgovern). 
c  Time needed to give this eon using basic formula is
c    teq = space**2/tgovern
c  Factor on space to get teq is
c    squez(ib) = space/tgovern
c  The eon for each ib can then be computed based on time of space*squez.
c  Iff squez = 1, there is a note spanning every increment in the \notes group.
c
c    tnminb = minimum time span in the bar for increments spanned by notes,
c             i.e., with squez=1.  Use after parsing into line to decide if
c             spacing needs to be "flattened" among notes groups.   
c
      common /comtol/ tol
      parameter (nm=24,nkb=3999,maxblks=9600)
      common /a1ll/ iv,ivxo(600),ipo(600),to(600),tno(600),nnl(nm),
     *   nv,ibar,mtrnuml,nodur(nm,200),lenbar,iccount,
     *   nbars,itsofar(nm),nib(nm,15),nn(nm),
     *   rest(nm,200),lenbr0,lenbr1,firstline,newmeter
      common /linecom/ elskb,tnminb(nkb)
      common /c1omnotes/ nnodur,wminnh(nkb),nnpd(maxblks),durb(maxblks),
     *     iddot,nptr(nkb),ibarcnt,mbrest,ibarmbr,
c     *     ibaroff,udsp(nkb),wheadpt,gotclef,sqzb(maxblks)
     *     ibaroff,udsp(nkb),wheadpt,sqzb(maxblks)
      common /c1ommvl/ nvmx(nm),ivmx(nm,2),ivx,fbar,nacc(nm,200)
c      logical rest,firstline,newmeter,gotclef
      logical rest,firstline,newmeter
      integer cnn(nm),istart(80),istop(80)
      real*4 xit(nm),space(80),tstart(80),squez(80),tnote(600)
      elskb = 0.
      tnminb(ibarcnt) = 1000.
      do 1 iv = 1 , nv
      do 1 kv = 1 , nvmx(iv)
        ivx = ivmx(iv,kv)
        if (ibar .gt. 1) then
          nn(ivx) = nib(ivx,ibar)-nib(ivx,ibar-1)
        else
          nn(ivx) = nib(ivx,ibar)
        end if
1     continue
c
c initialize list note counter, time(iv), curr. note(iv)
c
      ilnc = 1
      do 4 iv = 1 , nv
      do 4 kv = 1 , nvmx(iv)
        ivx = ivmx(iv,kv)
        cnn(ivx) = 1
        ivxo(ilnc) = ivx
        ipo(ilnc) = 1
        to(ilnc) = 0.
        tnote(ilnc) = fnote(nodur,ivx,1,nacc)
        xit(ivx) = tnote(ilnc)
        if (abs(xit(ivx)-lenbar).lt.tol) xit(ivx) = 1000.
        ilnc = ilnc+1
4     continue
c
c  Build the list
c
5     continue
c
c  Determine which voice comes next from end of notes done so far.
c  tmin is the earliest ending time of notes done so far
c
      tmin = 1000.
      do 6 iv = 1 , nv
      do 6 kv = 1 , nvmx(iv)
        ivx = ivmx(iv,kv)
        tminn = min(tmin,xit(ivx))
        if(tminn .lt. tmin) then
          tmin = tminn
          ivnext = ivx
        end if
6     continue
      if (tmin .gt. 999.) go to 7
      ivxo(ilnc) = ivnext
      cnn(ivnext) = cnn(ivnext)+1
      ipo(ilnc) = cnn(ivnext)
      to(ilnc) = tmin
c
c  Check if this voice is done
c
      tnote(ilnc) = fnote(nodur,ivnext,cnn(ivnext),nacc)
      if (cnn(ivnext) .eq. nn(ivnext)) then
        xit(ivnext) = 1000.
      else
        xit(ivnext) = xit(ivnext)+tnote(ilnc)
      end if
      ilnc = ilnc+1
      go to 5
7     continue
      ntot = ilnc-1
      if (ntot .gt. 600) then
        call printl(' ')
        call printl('Cannot have more than 600 notes per bar, stopping')
        call stop1()
      end if
      do 8 in = 1 , ntot-1
        tno(in) = to(in+1)-to(in)
8     continue
      tno(ntot) = fnote(nodur,ivxo(ntot),ipo(ntot),nacc)
      tnote(ntot) = tno(ntot)
c
c  Debug writes
c
c      write(*,'()')
c      write(*,'(16i5)')(ivxo(in),in=1,ntot)
c      write(*,'(16i5)')(ipo(in),in=1,ntot)
c      write(*,'(16f5.1)')(to(in),in=1,ntot)
c      write(*,'(16f5.1)')(tno(in),in=1,ntot)
c      write(*,'(16i5)')(nodur(ivxo(in),ipo(in)),in=1,ntot)
c      write(*,'(16f5.1)')(tnote(in),in=1,ntot)
c
c  Done w/ list.  Initialize loop for building note blocks:
c
      ib = 1
      istart(1) = 1
      space(1) = 0.
      in = 1
c
c  Start the loop
c
9     continue
        if (in .eq. ntot) then
          if (space(ib) .lt. tol) then
            space(ib) = tno(in)
c
c  Last gap in bar is spanned by a note, so cannot need a squeeze. 
c
            squez(ib) = 1.
          end if
          istop(ib) = ntot
c
c From here flow out of this if block and into block-setup
c
        else if (space(ib) .lt. tol) then
c
c space hasn't been set yet, so tentatively set:
c
          space(ib) = tno(in)
          if (space(ib) .lt. tol) then
            in=in+1
          else
c
c  Tentative space tno(in) is non-zero. Set squez, which will be kept (since
c    it is a unique property of the particular increment starting here) :
c
            squez(ib) = getsquez(in,ntot,space(ib),tnote,to)
            istop(ib) = in
          end if
          go to 9
        else if (tno(in+1) .lt. tol) then
c
c This is not the last note in the space, so
c
          in = in+1
          go to 9
        else if (abs(tno(in+1)-space(ib)) .lt. tol) then
c
c  Next increment has same space.  Does it have same squez?
c
          xsquez = getsquez(in+1,ntot,space(ib),tnote,to)
c
c  If it does have the same squez, loop, otherwise flow out
c
          if (abs(xsquez-squez(ib)) .lt. tol) then
c
c Keep spacing the same, update tentative stop point
c
            in = in+1
            istop(ib) = in
            go to 9
          end if
        end if
c
c At this point istart, istop, space, and squez are good, so close out block
c
        tstart(ib) = to(istart(ib))
c
c  Compute elemskips assuming no flattening to increase min space.  The formula
c  is only correct if f1eon(t) = sqrt(t/2); more generally (after possible
c  flattening in pmxb), elsperns = squez*feon(space/squez)
c
        elsperns = f1eon(space(ib)*squez(ib))
        if (istop(ib) .eq. ntot) then
          nnsk = nint((lenbar-tstart(ib))/space(ib))
        else
          nnsk = nint((to(istop(ib)+1)-tstart(ib))/space(ib))
        end if
        elskb = elskb+elsperns*nnsk
        if (nptr(ibarcnt+1) .gt. nptr(ibarcnt)) then
          call catspace(space(ib),squez(ib),nnsk)
        else
c
c  This is the first entry for this bar
c
          nnpd(nptr(ibarcnt)) = nnsk
          durb(nptr(ibarcnt)) = space(ib)
          sqzb(nptr(ibarcnt)) = squez(ib)
          nptr(ibarcnt+1) = nptr(ibarcnt+1)+1
        end if
c
c  Update minimum space spanned by a note
c
        if (abs(squez(ib)-1) .lt. tol) 
     *       tnminb(ibarcnt) = min(tnminb(ibarcnt),space(ib))
        if (istop(ib) .eq. ntot) go to 15
c
c  End of spatial accounting for now
c
        ib = ib+1
        istart(ib) = istop(ib-1)+1
        in = istart(ib)
c
c Set tentative block space for new block
c
        space(ib) = tno(in)
        if (space(ib).gt.tol) 
     *        squez(ib)= getsquez(in,ntot,space(ib),tnote,to)
        istop(ib) = in
      go to 9
15    continue
c     nb = ib
c
c  Debug writes
c
c      write(*,'(16i5)')(istart(ib),ib=1,nb)
c      write(*,'(16i5)')(istop(ib),ib=1,nb)
c      write(*,'(16f5.1)')(space(ib),ib=1,nb)
c      write(*,'(16f5.1)')(squez(ib),ib=1,nb)
c
      return
      end
      subroutine midievent(typeq,in1,in2)
c
c  We now store "conductor" events in mmidi(numchan,.), and count bytes
c    with imidi(numchan)
c
      parameter (nm=24,mv=24576)
      integer*2 mmidi
      logical restpend,relacc,notmain,twoline,ismidi,crdacc
      common /commidi/ imidi(0:nm),trest(0:nm),mcpitch(20),mgap,
     *       iacclo(0:nm,6),iacchi(0:nm,6),midinst(nm),
     *       nmidcrd,midchan(nm,2),numchan,naccim(0:nm),
     *       laccim(0:nm,10),jaccim(0:nm,10),crdacc,notmain,
     *       restpend(0:nm),relacc,twoline(nm),ismidi,mmidi(0:nm,mv),
     *       debugmidi
      logical debugmidi
      common /comevent/ miditime,lasttime
      character*1 typeq
      idur = isetvarlen(miditime-lasttime,nbytes)
      imidi(numchan) = imidi(numchan)+nbytes+1
      do 1 i = 1 , nbytes
        mmidi(numchan,imidi(numchan)-i) =  mod(idur,256)
        idur = ishft(idur,-8)
1     continue
      mmidi(numchan,imidi(numchan)) = 255
      if (typeq .eq. 't') then
c
c  Tempo event.  in1 = quarters per minute (integer)
c
        mmidi(numchan,imidi(numchan)+1) = 5*16+1
        mmidi(numchan,imidi(numchan)+2) = 3
        misperq = nint(60000000./in1)
        do 2 i = 1 , 3
          mmidi(numchan,imidi(numchan)+6-i) =  mod(misperq,256)
          misperq = ishft(misperq,-8)
2       continue
        imidi(numchan) = imidi(numchan)+5
      else if (typeq .eq. 'm') then
c
c  Meter event.  in1=numerator, in2=denom (power of 2)
c
        mmidi(numchan,imidi(numchan)+1) = 5*16+8
        mmidi(numchan,imidi(numchan)+2) = 4
        mmidi(numchan,imidi(numchan)+3) = in1
        if (in2 .gt. 0) then
          mmidi(numchan,imidi(numchan)+4) = log2(in2)
        else
          mmidi(numchan,imidi(numchan)+4) = 0
        end if
        mmidi(numchan,imidi(numchan)+5) = 24
        mmidi(numchan,imidi(numchan)+6) = 8
        imidi(numchan) = imidi(numchan)+6
      else if (typeq .eq. 'k') then
c
c  Keychange event.  in1 = isig
c
        mmidi(numchan,imidi(numchan)+1) = 5*16+9
        mmidi(numchan,imidi(numchan)+2) = 2
        mmidi(numchan,imidi(numchan)+3) = mod(256+in1,256)
        mmidi(numchan,imidi(numchan)+4) = 0
        imidi(numchan) = imidi(numchan)+4
      else
        print*,'Program flameout in midievent'
        call stop1()
      end if
      lasttime = miditime
      return
      end
      subroutine moveln(iuin,iuout,done)
      logical done
      character*129 outq
      done = .false.
      read(iuin,'(a)',end=1) outq
      lenout = llen(outq,129)
      write(iuout,'(a)') outq(1:lenout)
      return
1     done = .true.
      return
      end
      subroutine mrec1(lineq,iccount,ndxm)
c
c  This is called when (a) macro recording is just starting and
c  (b) at the start of a new line, if recording is on
c
      parameter (maxblks=9600)
      character*128 lineq,lnholdq
      common /commac/ macnum,mrecord,mplay,macuse,icchold,lnholdq,endmac
      logical mrecord,mplay,endmac
      common /c1ommac/ ip1mac(20),il1mac(20),ip2mac(20),il2mac(20),
     *                 ic1mac(20),ilmac,iplmac
      character*131072 bufq
      integer*2 lbuf(maxblks)
      common /inbuff/ ipbuf,ilbuf,nlbuf,lbuf,bufq
      if (.not.mrecord) then
c
c  Starting the macro
c
        ip1mac(macnum) = ipbuf-lbuf(ilbuf-1)+iccount
        il1mac(macnum) = ilbuf-1
        ic1mac(macnum) = iccount
        mrecord = .true.
      end if
      if (iccount .lt. 128) then
        ndxm = index(lineq(iccount+1:128),'M')
        if (ndxm .gt. 0) ndxm = ntindex(lineq(iccount+1:128),'M',
     *                                  128-iccount)
        if (ndxm .gt. 0) then
c
c  This line ends the macro.
c
          ip2mac(macnum) = ipbuf-lbuf(ilbuf-1)+iccount+ndxm
          il2mac(macnum) = ilbuf-1
          mrecord = .false.
        end if
      end if
      return
      end
      function ncmid(iv,ip)
      parameter (nm=24)
      common /all/ mult(nm,200),jv,nnl(nm),nv,ibar,
     *   ivxo(600),ipo(600),to(600),tno(600),tnote(600),eskz(nm,200),
     *   ipl(nm,200),ibm1(nm,9),ibm2(nm,9),nolev(nm,200),ibmcnt(nm),
     *   nodur(nm,200),jn,lenbar,iccount,nbars,itsofar(nm),nacc(nm,200),
     *   nib(nm,15),nn(nm),lenb0,lenb1,slfac,musicsize,stemmax,
     *   stemmin,stemlen,mtrnuml,mtrdenl,mtrnmp,mtrdnp,islur(nm,200),
     *   ifigdr(2,125),iline,figbass,figchk(2),firstgulp,irest(nm,200),
     *   iornq(nm,0:200),isdat1(202),isdat2(202),nsdat,isdat3(202),
     *   isdat4(202),beamon(nm),isfig(2,200),sepsymq(nm),sq,ulq(nm,9)
      common /comipl2/ ipl2(nm,200)
      character*1 ulq,sepsymq,sq
      logical beamon,firstgulp,figbass,figchk,isfig
      common /comcc/ ncc(nm),tcc(nm,10),ncmidcc(nm,10),
     *               maxdotmv(nm),ndotmv(nm),updot(nm,20),rtdot(nm,20)
      common /commvl/ nvmx(nm),ivmx(nm,2),ivx
	common /comtol/ tol
        if (ncc(iv) .eq. 1) then
          ncmid = ncmidcc(iv,1)
        else
          xtime = to(ipl2(ivx,ip))
          do 1 icc = ncc(iv) , 1 , -1
            if (xtime .gt. tcc(iv,icc)-tol) then
              ncmid = ncmidcc(iv,icc)
              return
            end if
1         continue
          print*,'Problem in ncmid()'
          stop
        end if
      end
      function ncmidf(clefq)
c
c  Return middle line of a clef
c
      character*1 clefq
      if (clefq .eq. '8') then
c
c  treblelowoct; will treat as treble for this purpose
c
        ncmidf = 35
      else
        ncmidf = 21+index(' b6r5n4a3m2s1t0f7',clefq)/2*2
      end if
      return
      end
      subroutine newvoice(jv,clefq,change)
      parameter (nm=24)
      common /all/ mult(nm,200),iv,nnl(nm),nv,ibar,
     *   ivxo(600),ipo(600),to(600),tno(600),tnote(600),eskz(nm,200),
     *   ipl(nm,200),ibm1(nm,9),ibm2(nm,9),nolev(nm,200),ibmcnt(nm),
     *   nodur(nm,200),jn,lenbar,iccount,nbars,itsofar(nm),nacc(nm,200),
     *   nib(nm,15),nn(nm),lenb0,lenb1,slfac,musicsize,stemmax,
     *   stemmin,stemlen,mtrnuml,mtrdenl,mtrnmp,mtrdnp,islur(nm,200),
     *   ifigdr(2,125),iline,figbass,figchk(2),firstgulp,irest(nm,200),
     *   iornq(nm,0:200),isdat1(202),isdat2(202),nsdat,isdat3(202),
     *   isdat4(202),beamon(nm),isfig(2,200),sepsymq(nm),sq,ulq(nm,9)
      character*1 ulq,sepsymq,sq
      logical beamon,firstgulp,figbass,figchk,isfig
      common /commvl/ nvmx(nm),ivmx(nm,2),ivx
      common /comfb/ nfb(nm),t1fb(nm,40),t2fb(nm,40),ulfbq(nm,40),ifb,
     *               tautofb,autofbon,t1autofb
      character*1 ulfbq,clefq
      common /comcc/ ncc(nm),tcc(nm,10),ncmidcc(nm,10),
     *               maxdotmv(nm),ndotmv(nm),updot(nm,20),rtdot(nm,20)
      common /comudsp/udsp(50),tudsp(50),nudsp,udoff(nm,20),nudoff(nm)
      logical change,autofbon
      nvmx(jv) = 1
      ivmx(jv,1) = jv
      itsofar(jv) = 0
      nnl(jv) = 0
      nfb(jv) = 0
      if (firstgulp .or. change) then
        ncmidcc(jv,1) = ncmidf(clefq)
      else
        ncmidcc(jv,1) = ncmidcc(jv,ncc(jv))
      end if
      tcc(jv,1) = 0.
      ncc(jv) = 1
      nudoff(jv) = 0
      ndotmv(jv) = 0
      do 5 j = 1 , 200
        irest(jv,j) = 0
        islur(jv,j) = 0
        ipl(jv,j) = 0
        nacc(jv,j) = 0
        iornq(jv,j) = 0
        mult(jv,j) = 0
        if (jv.le.2) isfig(jv,j) = .false.
5     continue
      return
      end
      subroutine notefq(noteq,lnote,nolev,ncmid)
c
c  Returns name of note level with octave transpositions, updates noctup.
c
      common /comoct/ noctup
      character*8 noteq
      character*1 upcaseq,noteqt,chax
      nupfroma = mod(nolev+1,7)
      iname = 97+nupfroma
      ioctup = (nolev+1)/7-4
      noteqt = chax(iname)
      if (ncmid .eq. 23) noteqt = upcaseq(noteqt)
      if (ioctup .eq. noctup) then
        noteq = noteqt
        lnote = 1
c
c  Must ALWAYS check if lnote=1 for use with functions requiring a blank
c
      else if (ioctup .gt. noctup) then
c
c  Raise octave.  Encase in {}
c
        write(noteq,'(8a1)')'{',(chax(39),i=noctup,ioctup-1),
     *                        noteqt,'}'
        lnote = 3+ioctup-noctup
        noctup = ioctup
      else
c
c  Lower octave
c
        write(noteq,'(8a1)')'{',(chax(96),i=ioctup,noctup-1),
     *                        noteqt,'}'
        lnote = 3+noctup-ioctup
        noctup = ioctup
      end if
      return
      end
      subroutine notex(notexq,lnote)
c
c  Returns non-beamed full note name
c
      parameter (nm=24)
      common /all/ mult(nm,200),iv,nnl(nm),nv,ibar,
     *   ivxo(600),ipo(600),to(600),tno(600),tnote(600),eskz(nm,200),
     *   ipl(nm,200),ibm1(nm,9),ibm2(nm,9),nolev(nm,200),ibmcnt(nm),
     *   nodur(nm,200),jn,lenbar,iccount,nbars,itsofar(nm),nacc(nm,200),
     *   nib(nm,15),nn(nm),lenb0,lenb1,slfac,musicsize,stemmax,
     *   stemmin,stemlen,mtrnuml,mtrdenl,mtrnmp,mtrdnp,islur(nm,200),
     *   ifigdr(2,125),iline,figbass,figchk(2),firstgulp,irest(nm,200),
     *   iornq(nm,0:200),isdat1(202),isdat2(202),nsdat,isdat3(202),
     *   isdat4(202),beamon(nm),isfig(2,200),sepsymq(nm),sq,ulq(nm,9)
      common /commvl/ nvmx(nm),ivmx(nm,2),ivx
      character*1 ulq,sepsymq,sq
      logical beamon,firstgulp,figbass,figchk,
     *        isfig,ispoi,btest,even
      character*1 udq,udqq,chax
      character*4 dotq
      character*2 numq
      character*79 notexq
      character*8 noteq
      character*40 restq
      common /compoi/ ispoi
      common /comfig/ itfig(2,74),figq(2,74),ivupfig(2,74),nfigs(2),
     *                fullsize(nm),ivxfig2,ivvfig(2,74)
      character*10 figq
      ip = ipo(jn)
      nole = nolev(ivx,ip)
c
c  Check for special situations with 2nds (see precrd)
c
      if (btest(nacc(ivx,ip),30)) then
        nole = nole - 1
      else if (btest(nacc(ivx,ip),31)) then
        nole = nole + 1
      end if
      nodu = nodur(ivx,ip)
      if (.not.btest(irest(ivx,ip),0)) then
        udq = udqq(nole,ncmid(iv,ip),
     *       islur(ivx,ip),nvmx(iv),ivx,nv)
      end if
c
c  Check figure level
c
c      if (figbass .and. isfig(ivx,ip)
c     *                    .and. .not.btest(irest(ivx,ip),0)) then
      if (figbass .and. .not.btest(irest(ivx,ip),0) .and.
     *    ((ivx.eq.1.and.isfig(1,ip))
     *       .or.(ivx.eq.ivxfig2.and.isfig(2,ip)))) then
        if (udq .eq. 'u' .or. nodu.ge.64) then
c
c  Upper or no stem, fnole (in noleunits) set by notehead
c
          fnole = nole
        else
c
c  Lower stem, fnole set by bottom of stem
c
          fnole = nole-stemlen
        end if
        zmin = fnole-ncmid(ivx,ip)+4
        if (ivx .eq. 1) then
          ifigdr(1,iline) = max(ifigdr(1,iline),nint(4-zmin))
        else
          ifigdr(2,iline) = max(ifigdr(2,iline),nint(4-zmin))
        end if
      end if
      if (.not.btest(irest(ivx,ip),0)) then
        call notefq(noteq,lnoten,nole,ncmid(iv,ip))
        if (lnoten .eq. 1) call addblank(noteq,lnoten)
        if (nodu .eq. 1) then
          notexq =sq//'cccc'//udq//noteq(1:lnoten)
          lnote = lnoten+6
        else if (nodu .eq. 2) then
          notexq =sq//'ccc'//udq//noteq(1:lnoten)
          lnote = lnoten+5
        else if (nodu .eq. 4) then
          notexq =sq//'cc'//udq//noteq(1:lnoten)
          lnote = lnoten+4
        else if (nodu .eq. 8) then
          notexq =sq//'c'//udq//noteq(1:lnoten)
          lnote = lnoten+3
        else if (nodu .eq. 16) then
          notexq =sq//'q'//udq//noteq(1:lnoten)
          lnote = lnoten+3
        else if (nodu .eq. 32) then
          notexq =sq//'h'//udq//noteq(1:lnoten)
          lnote = lnoten+3
        else if (nodu .eq. 64) then
          notexq =sq//'wh'//noteq(1:lnoten)
          lnote = lnoten+3
        else if (nodu .eq. 128) then
c          notexq =sq//'zbreve'//noteq(1:lnoten)//sq//'sk'
c          lnote = lnoten+10
          notexq =sq//'breve'//noteq(1:lnoten)
          lnote = lnoten+6
        else
          dotq = 'p'
          ldot = 1
          if (btest(iornq(ivx,ip),13)) then
c
c  Dotted note with ')' ornament
c
            dotq='m'
          else if (btest(islur(ivx,ip),3)) then
c
c  Double dot
c
            dotq = 'pp'
            ldot = 2
          end if
          if (nodu .ge. 192) then
            notexq =sq//'breve'//dotq(1:ldot)//noteq(1:lnoten)
            lnote = lnoten+6+ldot
          else if (nodu .ge. 96) then
            notexq =sq//'wh'//dotq(1:ldot)//noteq(1:lnoten)
            lnote = lnoten+3+ldot
          else if (nodu .ge. 48) then
            notexq =sq//'h'//udq//dotq(1:ldot)//noteq(1:lnoten)
            lnote = lnoten+3+ldot
          else if (nodu .ge. 24) then
            notexq =sq//'q'//udq//dotq(1:ldot)//noteq(1:lnoten)
            lnote = lnoten+3+ldot
          else if (nodu .ge. 12) then
            notexq =sq//'c'//udq//dotq(1:ldot)//noteq(1:lnoten)
            lnote = lnoten+3+ldot
          else if (nodu .ge. 6) then
            notexq =sq//'cc'//udq//dotq(1:ldot)//noteq(1:lnoten)
            lnote = lnoten+4+ldot
            ispoi = .true.
          else
            notexq =sq//'ccc'//udq//dotq(1:ldot)//noteq(1:lnoten)
            lnote = lnoten+5+ldot
            ispoi = .true.
          end if
          if (dotq(1:1) .eq. 'm') then
c
c  Need another call to the note, in case the first one has octave shifts
c
            if (lnoten .eq. 2) then
              notexq = notexq(1:lnote)//'{'//noteq(2:2)//'}'
              lnote = lnote+3
            else
              notexq = notexq(1:lnote)//noteq(lnoten-1:lnoten-1)
              lnote = lnote+1
            end if
          end if
        end if
      else if (btest(islur(ivx,ip),29)) then
c
c  Blank rest
c
        notexq = sq//'sk'
        lnote = 3
      else
c
c  Non-blank rest
c
        lnote = 0
        nole = mod(nole+20,100)-20
c
c  Kluge to get pause symbol for rp:
c
        if (btest(islur(ivx,ip),19)) nodu = 64
        if (nodu .le. 28) then
c
c  Normal rest < or = double-dotted quarter
c
          lrest = 3
          if (nodu .gt. 14) then
            restq =sq//'qp'
          else if (nodu .gt. 7) then
            restq =sq//'ds'
          else if (nodu .gt. 3) then
            restq =sq//'qs'
          else if (nodu .gt. 1) then
            restq =sq//'hs'
          else
            restq =sq//'qqs'
            lrest = 4
          end if
c
c  Special case for rest at start of F-tuplet inside a forced beam
c
          if (btest(nacc(ivx,ip),18) .and.
     *             btest(nacc(ivx,ip),19)) then
            restq = sq//'pt4'//restq(1:lrest)
            lrest = lrest+4
          end if
          notexq = restq
          lnote = lrest
c
c  At this point notexq=restq,lnote=lrest are name of rest.  Now raise if necc.
c
          if (nole .ne. 0) then
            if (abs(nole) .lt. 10) then
              noteq = chax(48+abs(nole))
              lnoten = 1
            else
              write(noteq(1:2),'(i2)')abs(nole)
              lnoten = 2
            end if
            if (nole .gt. 0) then
              notexq = sq//'raise'//noteq(1:lnoten)//sq//'internote'
            else
              notexq = sq//'lower'//noteq(1:lnoten)//sq//'internote'
            end if
            lnote = 16+lnoten
            notexq = notexq(1:lnote)//restq(1:lrest)
            lnote = lnote+lrest
          end if
          if (2**log2(nodu) .ne. nodu) then
c
c  Deal with dots (on rests shorter than half rest)
c
            restq = sq//'pt'
            lrest = 3
            if (2*nodu .gt. 3*2**log2(nodu)) then
              restq = sq//'ppt'
              lrest = 4
            end if
            nole = nole+4
            raisedot = 0
c
c  Tweak dot positions for special cases
c
            even = (mod(100+nole,2).eq.0)
c            if (.not.even.and.nodu.gt.8.and.
c     *          (nole.lt.0.or.nole.gt.8)) then
            if (.not.even .and. (nole.lt.0.or.nole.gt.8)) then
              raisedot = 1
            end if
            if (nole.ge.10 .or. nole.le.-1) then
              write(numq,'(i2)')nole
              restq = restq(1:lrest)//'{'//numq//'}'
              lrest = lrest+4
            else
              restq = restq(1:lrest)//chax(nole+48)
              lrest = lrest+1
            end if
            if (raisedot.gt.0) then
              restq = sq//'raise'//sq//'internote'//
     *                    sq//'hbox{'//restq(1:lrest)//'}'
              lrest = lrest+23
            end if
            notexq = restq(1:lrest)//notexq(1:lnote)
            lnote = lnote+lrest
          end if
        else
c
c  Half rest or longer
c
          if (nole .eq. 0) then
c
c  Half or longer rest is not raised or lowered
c
            if (nodu .le. 56) then
              notexq =sq//'hpause'
              lnote = 7
            else if (nodu .le. 112) then
              if (.not.btest(islur(ivx,ip),19) .or. 
     *                      btest(irest(ivx,ip),25)) then
                notexq = sq//'pause'
              else
                notexq = sq//'pausc'
              end if
              lnote = 6
            else
              notexq =sq//'PAuse'
              lnote = 6
            end if
            if (2**log2(nodu) .ne. nodu) then
c
c  Dotted rest, hpause or longer
c
              notexq = notexq(1:lnote)//'p'
              lnote = lnote+1
              if (2*nodu .gt. 3*2**log2(nodu)) then
c
c  Double dotted long rest
c
                notexq = notexq(1:lnote)//'p'
                lnote = lnote+1
              end if
            end if
          else
c
c  Raised or lowered half or whole rest
c
            if (nodu .eq. 32) then
              notexq = sq//'lifthpause'
              lnote = 11
            else if (nodu .eq. 48) then
              notexq = sq//'lifthpausep'
              lnote = 12
            else if (nodu .eq. 56) then
              notexq = sq//'lifthpausepp'
              lnote = 13
            else if (nodu .eq. 64) then
              notexq = sq//'liftpause'
              lnote = 10
            else if (nodu .eq. 96) then
              notexq = sq//'liftpausep'
              lnote = 11
            else if (nodu .eq. 112) then
              notexq = sq//'liftpausepp'
              lnote = 12
            else if (nodu .eq. 128) then
              notexq = sq//'liftPAuse'
              lnote = 10
            else
c
c  Assume dotted double whole rest
c
              notexq = sq//'liftPAusep'
              lnote = 11
            end if
c
c  Set up height spec
c
            nole = sign(abs(nole)/2,nole)
            if (nole.le.9 .and. nole.ge.0) then
              noteq = chax(48+nole)
              lnoten = 1
            else
              noteq = '{'
              if (nole .ge. -9) then
                write(noteq(2:3),'(i2)')nole
                lnoten = 3
              else
                write(noteq(2:4),'(i3)')nole
                lnoten = 4
              end if
              noteq = noteq(1:lnoten)//'}'
              lnoten = lnoten+1
            end if
            notexq = notexq(1:lnote)//noteq(1:lnoten)
            lnote = lnote+lnoten
          end if
        end if
      end if
      return
      end
      function ntindex(line,s2q,lenline)
c
c  Returns index(line,s2q) if NOT in TeX string, 0 otherwise
c
      character*(*) s2q,line
      character*1 chax
      logical intex
      ndxs2 = index(line,s2q)
      ndxbs = index(line,chax(92))
      if (ndxbs.eq.0 .or. ndxs2.lt.ndxbs) then
        ntindex = ndxs2
      else
c
c  There are both bs and s2q, and bs is to the left of sq2. So check bs's to
c  right of first: End is '\ ', start is ' \'
c
        len = lenstr(line,lenline)
        intex = .true.
        do 1 ic = ndxbs+1 , len
          if (ic .eq. ndxs2) then
            if (intex) then
              ntindex = 0
              ndxs2 = index(line(ic+1:len),s2q)+ic
            else
              ntindex = ndxs2
              return
            end if
          else if (intex .and. line(ic+1:ic+2).eq.chax(92)//' ') then
            intex = .false.
          else if (.not.intex .and. line(ic+1:ic+2).eq.' '//chax(92))
     *           then
            intex = .true.
          end if
1       continue
      end if
      return
      end
      subroutine ntrbbb(n,char1q,ulqq,iv,notexq,lnote)
c
c  This appends to notexq e.g. '\ibbbu1'
c
      common /combbm/ isbbm
      logical isbbm
      character*79 notexq
      character*1 char1q,ulqq,chax
      character*4 tempq
      if (n .ge. 5) isbbm = .true.
      if (lnote .gt. 0) then
        notexq = notexq(1:lnote)//chax(92)//char1q
      else
        notexq = chax(92)//char1q
      end if
      lnote = lnote+2
      do 3 im = 1 , n
        notexq = notexq(1:lnote)//'b'
        lnote = lnote+1
3     continue
c
c  add the number, 0 if 12
c
c  5/25/08 Allow >12
c
c      call istring(mod(iv,12),tempq,len)
      if (iv .lt. 24) then
        call istring(iv,tempq,len)
      else if (iv .eq. 24) then
        tempq(1:1) = '0'
        len = 1
      else
        call printl('Sorry, too man open beams')
        call stop1()
      end if
      notexq = notexq(1:lnote)//ulqq//tempq(1:len)
      lnote = lnote+1+len
      return
      end
      function numclef(clefq)
c
c  Returns the number to be used as argument of \setclef for MusiXTeX 
c  For input 0-6 or tsmanrb return 0-7
c            7      f              9
c            8                     0
c
        character*1 clefq
        if (ichar(clefq) .lt. 55) then
c        if (ichar(clefq) .le. 55) then
          numclef = ichar(clefq)-48
c          if (numclef .eq. 7) numclef = 9
        else if (clefq .eq. '7') then
          numclef = 9
        else if (clefq .eq. '8') then 
          numclef = 0
        else
          numclef = index('tsmanrbxxf',clefq)-1
        end if
      return
      end
      subroutine outbar(i,jlast)
      character*1 chax
      nfmt = log10(i+.5)+2
      if (jlast+5+nfmt .lt. 80) then
        write(*,'(a5,i'//chax(48+nfmt)//',$)')'  Bar',i
        write(15,'(a5,i'//chax(48+nfmt)//',$)')'  Bar',i
        jlast = jlast+5+nfmt
      else
        write(*,'(/,a5,i'//chax(48+nfmt)//',$)')'  Bar',i
        write(15,'(/,a5,i'//chax(48+nfmt)//',$)')'  Bar',i
        jlast = 5+nfmt
      end if
      return
      end
      subroutine pmxa(basenameq,lbase,isfirst,nsyout,nbarss,optimize)
cccccccccccccccccccccccccccccccccccccccccccccccc
cc                                            cc
cc Subroutine, combine with pmxb.for
cc
cccccccccccccccccccccccccccccccccccccccccccccccc
cc
cc  Need to consider X spaces in xtuplets when getting poenom, and
cc      maybe fbar?
cc  mx06a
cc    ID numbers for voices when number of voices is reduced.
cc
cc  mx03a
cc    account for new fracindent for new movements.
cc
cc  Known changes since pmxa. Version 1.1b (see pmxb for longer list)
cc
cc  Check ID codes for slurs.
cc  Version 1.24 still does not have details for spacing/positioning
cc    arpeggios if there are accidentals or shifted notes or crowded scores.
cc  Fix problem in 1.22 with arpeggios across multi-line staves
cc  Fix problem in 1.22 with flat key signatures
cc  Read setup data as strings
cc  Warning for octave designation plus +/-
cc  Don't pause for volta warning,
cc  Macros
cc  Correct fsyst to account for transposition and key changes.
cc  Check for nbars > nsyst
cc
ccccccccccccccccccccccccccccccccccc
      parameter (nm=24,nkb=3999,nks=125,maxblks=9600)
      logical loop,usefig,isfirst,optimize
      character*131072 bufq
      integer*2 lbuf(maxblks)
      common /inbuff/ ipbuf,ilbuf,nlbuf,lbuf,bufq
      integer nn(nm),nodur(nm,200),ivxo(600),ipo(600),
     *        nnl(nm),itsofar(nm),nib(nm,15),lastbar(0:nks),nbarss(nks)
      common /c1omnotes/ nnodur,wminnh(nkb),nnpd(maxblks),durb(maxblks),
     *     iddot,nptr(nkb),ibarcnt,mbrest,ibarmbr,
c     *     ibaroff,udsp(nkb),wheadpt,gotclef,sqzb(maxblks)
     *     ibaroff,udsp(nkb),wheadpt,sqzb(maxblks)
      common /compage/ widthpt,ptheight,hoffpt,voffpt,
     *      nsyst,nflb,ibarflb(0:40),
     *      isysflb(0:40),npages,nfpb,ipagfpb(0:18),isysfpb(0:18),
     *      usefig,fintstf,gintstf,fracsys(30),nmovbrk,isysmb(0:30),
     *      nistaff(0:40)
      common /comkeys/ nkeys,ibrkch(18),newkey(18),iskchb,idsig,isig1,
     *      mbrestsav,kchmid(18),ornrpt,shifton,barend,noinst,stickyS
      real*4 elsk(nkb),celsk(0:nkb),elss(nks),to(600),tno(600)
      character*128 lnholdq
      character*44 basenameq
      logical rest(nm,200),firstline,fbon,isvolt,iskchb,kchmid,ornrpt,
     *        stickyS
      common /c1omget/ lastchar,fbon,issegno,ihead,isheadr,nline,isvolt,
     *     fracindent,nsperi(nm),linesinpmxmod,line1pmxmod,lenbuf0
      logical lastchar,newmeter,newmb(nkb),issegno,bottreb,isheadr,
     *     shifton,barend
      common /a1ll/ iv,ivxo,ipo,to,tno,nnl,nv,ibar,mtrnuml,
     *   nodur,lenbar,iccount,nbars,itsofar,nib,nn,
     *   rest,lenbr0,lenbr1,firstline,newmeter
      common /linecom/ elskb,tnminb(nkb)
      common /cblock/
     *      etatop,etabot,etait,etatc,etacs1,hgtin,hgtti,hgtco,
     *      xilbn,xilbtc,xilhdr,xilfig,a,b,inhnoh
      common /cominbot/ inbothd
      common /c1ommvl/ nvmx(nm),ivmx(nm,2),ivx,fbar,nacc(nm,200)
      common /commac/ macnum,mrecord,mplay,macuse,icchold,lnholdq,endmac
      common /commus/ musize,whead20
      common /comeon/ eonk,ewmxk
c      logical mrecord,mplay,endmac,gotclef,cstuplet
      logical mrecord,mplay,endmac,cstuplet
      logical novshrinktop,upslur,fontslur,WrotePsslurDefaults
      common /comnvst/ novshrinktop,cstuplte
      common /comslur/ listslur,upslur(nm,2),ndxslur,fontslur
     *                 ,WrotePsslurDefaults,SlurCurve
      common /comligfont/ isligfont
      logical isligfont
      common /comInstTrans/ iInstTrans(nm),iTransKey(nm),iTransAmt(nm),
     *  instno(nm),nInstTrans,EarlyTransOn,LaterInstTrans
      logical EarlyTransOn,LaterInstTrans
      logical fulltrans
      common /comis4bignv/ is4bignv,AIset
      logical is4bignv,AIset
      common /comshort/ shortfrac,codafrac,ishort,mbrsum,nmbr,nocodabn,
     *  poefa
      real*4 poefa(125)
      logical nocodabn
      data wtimesig,wclef,wkeysig
     *   / 0.72  , 0.8 ,  0.28 /
c      data poefa /125*1./
c This corrected a problem in Bob Tennent's compilation of pmx2.94
c
      do 97 i = 1 , 125
        poefa(i) = 1.
97    continue
      is4bignv = .false.
      AIset = .false.
      whead20 = 0.3
      ishort = 0
      if (.not.optimize) then
        print*
        print*,'Starting first PMX pass'
        write(15,'(a)') ' Starting first PMX pass'
      end if
      if (isfirst) then
        open(19,file='pmxaerr.dat')
        write(19,'(i6)') 0
        close(19)
      end if
      if (.not.optimize) jprntb = 81
      macuse = 0
      ornrpt = .false.
      stickyS = .false.
      mrecord = .false.
      mplay = .false.
      lastchar = .false.
      novshrinktop = .false.
	cstuplet = .false.
      fontslur = .true.
      isligfont = .false.
      fulltrans = .false.
      do 42 ibarcnt = 1 , nkb
        udsp(ibarcnt) = 0.
        wminnh(ibarcnt) = -1.
42    continue
c
c  Initialize input buffer
c
      lenbuf0 = ipbuf
      ipbuf = 0
      ilbuf = 1
      call g1etset(nv,noinst,mtrnuml,mtrdenl,mtrnmp,mtrdnp,
     *    xmtrnum0,newkey(1),npages,nsyst,musize,bottreb)
c
c  Set up list of instrument numbers (iv)
c
      ivnow = 0
      do 13 instnow = 1 , noinst
        do 14 iscount = 1 , nsperi(instnow)
          ivnow = ivnow+1
          instno(ivnow) = instnow
14      continue
13    continue
c
c  Save initial meter for midi
c
      if (.not.isfirst .and. npages.eq.0) then
        print*,'Sorry, must have npages>0 for optimization.'
        call stop1()
      end if
      nsyout = nsyst
c
c  isig1 will be changed in getnote if there is a transposition
c
      isig1 = newkey(1)
      if (npages .gt. nsyst) then
        call printl('npages > nsyst in input.  Please fix the input.')
        call stop1()
      end if
c
c  fbar = afterruleskip/elemskip
c  apt = width of small accidental + space in points (= 6 at 20pt) =wheadpt
c
      fbar = 1.
      wheadpt = whead20*musize
      ifig = 0
      usefig = .true.
      lenbeat = i1fnodur(mtrdenl,'x')
      lenmult = 1
      if (mtrdenl .eq. 2) then
        lenbeat = 16
        lenmult = 2
      end if
      lenbr1 = lenmult*mtrnuml*lenbeat
      lenbr0 = nint(lenmult*xmtrnum0*lenbeat)
      mtrnuml = 0
      if (lenbr0 .ne. 0) then
        ibaroff = 1
        lenbar = lenbr0
      else
        ibaroff = 0
        lenbar = lenbr1
      end if
      ibarcnt = 0
      nptr(1) = 1
      iccount = 128
      nmovbrk = 0
      nflb = 0
      nfpb = 0
      ipagfpb(0) = 1
      isysfpb(0) = 1
      ibarflb(0) = 1
      isysflb(0) = 1
      nistaff(0) = nv-1
c
c  Check for pmx.mod
c
      linesinpmxmod = 0
c      line1pmxmod = ilbuf
      call getpmxmod(.true.,' ')
      if (.not.isfirst .and. linesinpmxmod .gt. 0) then
        print*,'Sorry, cannot optimize if there is a pmx.mod file'
        call stop1()
      end if
c
c  Initialize for loop over lines
c
      nkeys = 1
      ibrkch(1) = 1
      mbrestsav = 0
      shifton = .false.
      firstline = .true.
      newmeter = .false.
      ihead = 0
      isheadr = .false.
c      gotclef = .false.
      idsig = 0
      iddot = 0
      fintstf = -1.
      gintstf = 1.0
      listcresc = 0
      listdecresc = 0
30    loop = .true.
      iskchb = .false.
      issegno = .false.
      nbars = 0
      ibarmbr = 0
      do 4 iv = 1 , nv
        nvmx(iv) = 1
        ivmx(iv,1) = iv
        itsofar(iv) = 0
        nnl(iv) = 0
        do 5 j = 1 , 200
          rest(iv,j) = .false.
          nacc(iv,j) = 0.
5       continue
4     continue
      iv = 1
      ivx = 1
      fbon = .false.
      barend = .false.
      isvolt = .false.
2     if (loop) then
c
c  Within this short loop, nv voices are filled up for the duration of a block.
c  On exit (loop=.false.) the following are set: nnl(nv),itsofar(nv)
c  nodur(..),rest(..).  nnl will later be
c  increased and things slid around as accidental skips are added.
c
        call g1etnote(loop,ifig,optimize,fulltrans)
        if (lastchar) go to 20
        go to 2
      end if
      if (mbrestsav .gt. 0) then
        call printl(' ')
        call printl(
     *        'You must enter the same multibar rest in ALL parts')
        call stop1()
      end if
      do 10 ibar = 1 , nbars
        ibarcnt = ibarcnt+1
c
c  The following is just a signal to start a new bar when cataloging spaces 
c    for catspace(...)
c
        nptr(ibarcnt+1) = nptr(ibarcnt)
        newmb(ibarcnt) = .false.
        if (newmeter.and.ibar.eq.1) newmb(ibarcnt) = .true.
c
c  Above is only for spacing calcs later on.  Remember new meter can only occur
c  at START of a new input line (ibar = 1)
c
        if (ibar .ne. ibarmbr) then
          if (.not.optimize) call outbar(ibarcnt-ibaroff,jprntb)
        else
          if (.not.optimize) then
            write(15,'(/,a20,i4,a1,i4)')' Multibar rest, bars',
     *        ibarcnt-ibaroff,'-',ibarcnt-ibaroff+mbrest-1
            write(*,'(/,a20,i4,a1,i4)')' Multibar rest, bars',
     *        ibarcnt-ibaroff,'-',ibarcnt-ibaroff+mbrest-1
            jprntb = 0
          end if
          ibaroff = ibaroff-mbrest+1
        end if
        if (firstline .and. lenbr0.ne.0) then
          if (ibar .eq. 1) then
            lenbar = lenbr0
          else
            lenbar = lenbr1
          end if
        end if
        if (ibar .gt. 1) then
c
c  For bars after first, slide all stuff down to beginning of arrays
c
          do 11 iv = 1 , nv
          do 11 kv = 1 , nvmx(iv)
            ivx = ivmx(iv,kv)
            ioff = nib(ivx,ibar-1)
            do 12 ip = 1 , nib(ivx,ibar)-ioff
              nodur(ivx,ip) = nodur(ivx,ip+ioff)
              rest(ivx,ip) = rest(ivx,ip+ioff)
              nacc(ivx,ip) = nacc(ivx,ip+ioff)
12          continue
11        continue
        end if
        do 67 iv = 1 , nv
        do 67 kv = 1 , nvmx(iv)
          ioff= 0
          if(ibar.gt.1)ioff = nib(ivmx(iv,kv),ibar-1)
67      continue
        call makeabar()
        elsk(ibarcnt) = elskb+fbar
10    continue
      newmeter = .false.
      firstline = .false.
      go to 30
20    continue
c
c Vertical analysis.
c
      if (npages .eq. 0) then
        if (nsyst .eq. 0) then
          print*,'When npages=0, must set nsyst=bars/syst, not 0'
          call stop1()
        end if
        nsyst = (ibarcnt-1)/nsyst+1
        if (nv .eq. 1) then
          nsystpp = 12
        else if (nv .eq. 2) then
          nsystpp = 7
        else if (nv .eq. 3) then
          nsystpp = 5
        else if (nv .eq. 4) then
          nsystpp = 3
        else if (nv .le. 7) then
          nsystpp = 2
        else
          nsystpp = 1
        end if
        npages = (nsyst-1)/nsystpp+1
      end if
c
c  Check nsyst vs ibarcnt
c
      if (nsyst .gt. ibarcnt) then
        print*
        print*,'nsyst,ibarcnt:',nsyst,ibarcnt
        print*,'There are more systems than bars.'
        write(15,'(a,2i5)')' nsyst,ibarcnt:',nsyst,ibarcnt
        write(15,'(a)')' There are more systems than bars.'
        call stop1()
      end if
c
c  Set up dummy forced line & page breaks after last real one
c
      nflb = nflb+1
      ibarflb(nflb) = ibarcnt+1
      isysflb(nflb) = nsyst+1
      nfpb = nfpb+1
      ipagfpb(nfpb) = npages+1
      isysfpb(nfpb) = nsyst+1
      heightil = ptheight*4./musize
      open(12,status='SCRATCH')
      write(12,'(a)')basenameq(1:lbase)
      write(12,*)lbase
c
c Pass to pmxb the initial signature, including effect of transposition.
c
      write(12,'(6f10.5/f10.5,3i5)')fbar,wheadpt,etait,
     *     etatc,etacs1,etatop,etabot,inbothd,inhnoh,isig1
      write(12,*)npages,widthpt,ptheight,hoffpt,voffpt,nsyst
      iflbnow = -1
      isysb4 = 0
      do 8 ifpb = 1 , nfpb
c
c  Each time thru this loop is like a single score with several pages
c
        npages = ipagfpb(ifpb)-ipagfpb(ifpb-1)
        nsyst = isysfpb(ifpb)-isysfpb(ifpb-1)
        nomnsystp = (nsyst-1)/npages+1
        nshort = nomnsystp*npages-nsyst
        do 7 ipage = 1 , npages
          nsystp = nomnsystp
          if (ipage .le. nshort) nsystp = nsystp-1
c
c  Last system number on this page:
          isysendpg = isysb4+nsystp
          nintpg = 0
          do 15 isy = isysb4+1, isysendpg
            if (isysflb(iflbnow+1) .eq. isy) iflbnow = iflbnow+1
            nintpg = nintpg+nistaff(iflbnow)
15        continue
          xilfrac = 0.
          xiltxt = 0.
          if (ipage.eq.1 .and. ihead.gt.0) then
c
c  Needn't zero out ihead after printing titles if we only allow titles at top?
c
            if (iand(ihead,1) .eq. 1) then
              xiltxt = xiltxt+hgtin*4/musize
              xilfrac = xilfrac+etait
            end if
            if (iand(ihead,2) .eq. 2) then
              xiltxt = xiltxt+hgtti*4/musize
              xilfrac = xilfrac+etatc
            end if
            if (iand(ihead,4) .eq. 4) then
              xiltxt = xiltxt+hgtco*4/musize
              xilfrac = xilfrac+etacs1
            else
c
c Use double the title-composer space if there is no composer
c
              xilfrac = xilfrac+etatc
            end if
          end if
          D = xilfrac+nsystp-1+etatop+etabot
c          C = nsystp*(nv-1)
          C = nintpg
c          xN = heightil - xiltxt - 4*nsystp*nv - (nsystp-1)*xilbn
          xN = heightil - xiltxt - 4*(nintpg+nsystp) - (nsystp-1)*xilbn
          if (bottreb) xN = xN-(nsystp-1)*xilbtc
          if (ihead.eq.0 .and. isheadr) xN = xN - xilhdr
          if (ifig .eq. 1) then
            xN = xN - nsystp*xilfig
          end if
          glueil = (xN-b*C)/(D+a*C)
          omegaG = (b*D+a*xN)/(D+a*C)
c
c  G = \interlines between systems
c  omega*G = \interlines between staves of the same system
c  \interstaff = 4+omega*G
c  C = total number of interstaff spaces in the page
c  D = omega-indep factors for scalable height = nsy-1 (intersystem glue)
c      + etatop + etabot + etatxt +
c  N = scaleable height (\interlignes) = height - htext - staff heights - xil
c  xil = extra interliges = (nsy-1)*xilbn + 10 if header and no titles
c                          + (nsy-1)*xiltcb    for treble clef bottoms
c                          + nsy*xilfig        for figures
c  G = N/(D + omega * C) = glueil,   (1)
c  But (empirically)  omega*G = a*G + b (2)
c      with a=1.071 and b=2.714
c  Solving (1) and (2) gives
c      G = (N-b*C)/(D+a*C) , omega*G = (b*D+a*N)/(D+a*C)
c  Pass to pmxb    omega*G (=\interstaff-4)
c                  (etatop,bot,it,tc,cx)*G as inputs to \titles
c
c       glueil = (heightil-xiltxt-nsystp*(xil+4*nv))
c    *             /(nsystp*(1+gfact*(nv-1))-1+etatop+etabot+xilfrac)
c       xnsttop = glueil*etatop
c       xintstaff = 4+gfact*glueil
c
c  Only the first page will get local adjustment now if needed, others in pmxb
c
          if (ifpb.eq.1 .and. ipage.eq.1 .and. fintstf.gt.0.) then
            facins = fintstf
            fintstf = -1.
          else
c
c  gintstf = 1.0 by default, but may be changed with AI<x>
c
            facins = gintstf
          end if
          write(12,*)nsystp,max(0.,etatop*glueil),facins*(omegaG+4)
          ihead = 0
          isheadr = .false.
          isysb4 = isysendpg
7       continue
8     continue
c
c  Done with vertical, now do horizontals
c
      celsk(1) = elsk(1)
      do 21 ibar = 2 , ibarcnt
        celsk(ibar) = celsk(ibar-1)+elsk(ibar)
21    continue
      lastbar(0) = 0
      ibar1 = 1
      wmins = -1.
      iflb = 1
      imovbrk = 0
      ikey = 1
c
c  Return nsyst to its *total* value
c
      nsyst = isysfpb(nfpb)-1
      do 22 isyst = 1 , nsyst
        if (isyst .eq. isysflb(iflb)) iflb = iflb+1
        if (nmovbrk.gt.0 .and. imovbrk.lt.nmovbrk) then
          if (isyst .eq. isysmb(imovbrk+1)) imovbrk = imovbrk+1
        end if
        ibarb4 = lastbar(isyst-1)
        if (isyst .eq. 1) then
          if (isfirst) elsstarg = celsk(ibarflb(1)-1)/
     *            (isysflb(1)-1-fracindent)*(1-fracindent)
          celskb4 = 0.
        else
          celskb4 = celsk(ibarb4)
c
c  Must dimension isysmb(0:*) just so I can execute this test!
c
          if (isfirst) then
            if (nmovbrk.gt.0 .and. isyst.eq.isysmb(imovbrk)) then
c
c  First syst after forced line break.  There may be indentation.
c
              elsstarg = (celsk(ibarflb(iflb)-1)-celskb4)
     *          /(isysflb(iflb)-isyst-fracsys(imovbrk))
     *          *(1-fracsys(imovbrk))
            else
c
c  There is no indentation to deal with
c
              elsstarg = (celsk(ibarflb(iflb)-1)-celskb4)
     *                           /(isysflb(iflb)-isyst)
            end if
          end if
        end if
        if (isfirst) then
          diff1 = abs(elsstarg-elsk(ibarb4+1))
          do 23 ibar = ibarb4+2 , ibarcnt
            diff = elsstarg-(celsk(ibar)-celskb4)
            if (abs(diff) .ge. diff1) go to 24
            diff1 = abs(diff)
23        continue
24        ibar = ibar-1
          lastbar(isyst) = ibar
          nbarss(isyst) = ibar-ibarb4
        else
c
c  nbarss is given as an input, must compute lastbar and ibar
c
          lastbar(isyst) = nbarss(isyst)+ibarb4
          ibar = lastbar(isyst)
        end if
c
c  elss is # of elemskip in the syst. from notes & ars's, not ruleskips, ask's.
c
        elss(isyst) = celsk(ibar)-celskb4
        write(12,'(i5)')lastbar(isyst-1)+1
c
c  Transposed sigs are isig1, newkey(2,3,...).
c
        if (ikey .eq. 1) then
          key1 = isig1
        else
          key1 = newkey(ikey)
        end if
        fsyst = wclef+abs(key1)*wkeysig+2./musize
        xelsk = 0.
1       if (ikey.lt.nkeys) then
          if (ibrkch(ikey+1).le.lastbar(isyst)) then
c
c  Add space for all key changes
c
            ikey = ikey+1
            key2 = newkey(ikey)
            naccs = max(abs(key2-key1),max(abs(key1),abs(key2)))
            fsyst = fsyst+naccs*wkeysig
c
c  Account for afterruleskips (fbar)
c
            xelsk = xelsk+fbar/2
            if (ibrkch(ikey).lt.lastbar(isyst) .and. .not.kchmid(ikey))
     *          xelsk = xelsk-1.
            key1 = key2
            go to 1
          end if
        end if
c
c  Add extra fixed space for double bar
c
        if (isyst .eq. nsyst) then
          fsyst = fsyst+4.5/musize
        end if
c
c  Add extra fixed space for initial time signature
c
        if (isyst .eq. 1) then
          fsyst = fsyst+wtimesig
        end if
c
c  Add extra fixed space for time signature changes & user-defined spaces
c
        do 26 ibars = ibarb4+1 , lastbar(isyst)
          if (newmb(ibars)) fsyst = fsyst+wtimesig
          fsyst = fsyst+udsp(ibars)/musize
26      continue
        if (isyst .eq. 1) then
          wdpt = widthpt*(1-fracindent)
        else
          if (nmovbrk.gt.0 .and. imovbrk.gt.0 .and.
     *                isyst.eq.isysmb(imovbrk)) then
            wdpt = widthpt*(1-fracsys(imovbrk))
          else
            wdpt = widthpt
          end if
        end if
        wsyspt = wdpt-fsyst*musize-0.4*nbarss(isyst)
c
c  Checks for min spacing
c  Get min allowable space
c
        dtmin = 1000.
        do 45 ibar = ibar1 , ibar1+nbarss(isyst)-1
          dtmin = min(dtmin,tnminb(ibar))
          if (wminnh(ibar).ge.0.) wmins = wminnh(ibar)
45      continue
        if (wmins .lt. 0) wmins = 0.3
        wminpt = (1+wmins)*0.3*musize
c
c  Find max duration & # of notes for this system
c
        dtmax = 0.
        nns = 0
        do 43 iptr = nptr(ibar1) , nptr(ibar1+nbarss(isyst))-1
          dtmax = max(dtmax,durb(iptr))
          nns = nns + nnpd(iptr)
43      continue
        elmin0 = wsyspt*f1eon(dtmin)/(elss(isyst)+xelsk)
        if (elmin0 .ge. wminpt) then
c
c  Subtract out fbar stuff to keep old way of passing sumelsk to pmxb;
c    there is no need to "flatten"
c
          sumelsk = elss(isyst)-fbar*nbarss(isyst)
          eonk = 0.
          ewmxk = 1.
        else
          elmin1 = wsyspt/((fbar*nbarss(isyst)+xelsk)/f1eon(dtmax)+nns)
          if (elmin1 .le. wminpt) then
c            print*
c            print*,'In system #',isyst,' cannot meet min. space rqmt'
c            write(15,'(/a,i5,a)')
c     *         'In system #',isyst,' cannot meet min. space rqmt'
            eonk = 0.9
          else
c
c  Find eonk by Newton method
c
            call findeonk(nptr(ibar1),nptr(ibar1+nbarss(isyst))-1,
     *         wsyspt/wminpt,fbar*nbarss(isyst)+xelsk,dtmin,dtmax,
     *         (wminpt-elmin0)/(elmin1-elmin0))
            eonk = min(.9,eonk)
          end if
          ewmxk = f1eon(dtmax)**eonk
c
c  Recompute poenom!
c
          sumelsk = 0
          do 44 iptr = nptr(ibar1) , nptr(ibar1+nbarss(isyst))-1
            sumelsk = sumelsk
     *       + nnpd(iptr)*sqzb(iptr)*feon(durb(iptr)/sqzb(iptr))
44        continue
        end if
        poenom = wsyspt/(sumelsk+fbar*nbarss(isyst)+xelsk)
c
c Set fracindent for output: orig if isyst=1, fracsys(imovbrk) if movbrk, else 0
c
        if (isyst .gt. 0) then
          if (nmovbrk.gt.0 .and. imovbrk.gt.0 .and.
     *        isyst.eq.isysmb(imovbrk)) then
            fracindent = fracsys(imovbrk)
          else
            fracindent = 0.
          end if
        end if
        write(12,'(1pe12.5/i5,5e12.3)') poenom,nbarss(isyst),
     *       sumelsk,fsyst,fracindent,eonk,ewmxk
        ibar1 = ibar1+nbarss(isyst)
22    continue
      rewind(12)
      open(13,status='SCRATCH')
      write(13,'(i5)')ifig
      rewind(13)
      ilbuf = 1
      ipbuf = 0
      if (.not.optimize) then
        write(*,'(/,a)')' Done with first pass'
        print*
        write(15,'(/,a)')' Done with first pass'
        write(15,'()')
      end if
c
c  Following syntax is needed since pmxa is called with literal argument .false.
c
      if (isfirst) isfirst = .false.
      return
      end
      subroutine pmxb(inlast,poevec,ncalls,optimize)
cccccccccccccccccccccccccc
cc
cc To Do
cc
cc  Resolve disagreement in final poe for 1st system, compared with *.mx2
cc  Shift slurs on right- or left-shifted main notes (2/7/99)
cc  Various end-of-input-block repeat problems (ick142.pmx).
cc  Force multiplicity for un-beamed xtups.
cc  Clef change at end of piece
cc  Global "A" option to maximize "X" at a given time tick.
cc  Tighten test for end-of-bar hardspace, flgndv(ivx) due to right-shifted
cc       note.  See trubl18.pmx
cc  Tab character as space.
cc  Clef interference with second line of music.
cc  Add space for interferences between *different* lines of music?
cc  Shift arpeggios, both automatic and manual.
cc  Different musicsize for different instruments.
cc  Spacing checks for accid's on left-shifted chord notes
cc  Spacing checks for double dots
cc  Allow forced line breaks w/o setting nsyst.
cc  Cresc-Decresc. (Enhance MusiXTeX first?)
cc  Dynamic Marks.
cc  Bug with Voltas at line end (MusiXTeX problem?).
cc  Subtle bug w/ slur hgt over line brk, see trubl15.pmx
cc  Stem-end slurs.
cc  Allow units in indentation.
cc  Make inline TeX more context sensitive.
cc  Werner's 4/2/98 problem with "o?"
cc  Scor2prt converts e.g. "r0+0" into "r0 0", which seems to be wrong.
cc    converts e.g. "r2db" into "r2d", which might be wrong.
cc  Werner's generalsignature problem with Key change and new transposition.
cc    (wibug8.pmx)
cc  Unequal xtuplets
cc  Print both sets of bar #'s in tex file.
cc  Make barlines invisible \def\xbar{\empty} , fix fbar.
cc  Auto-tie slurs  'At'
cc  Forced line break anywhere (e.g. at a mid-bar repeat).
cc  Clef change at very start of file.
cc  Tighten test for M as macro terminator.
cc  Fix title so not separate limit on author length + composer length.
cc  Arpeggios in xtups.
cc
cc mx10b
cc  Option for instrument name at top center. Last item in P command:
cc    P[n]c         text is instrument name (use in parts)
cc    P[n]cstuff    text is stuff (up to 1st blank)
cc    P[n]c"stuff with spaces"   text is stuff with spaces
cc
cc Post version 1.43
cc  Reduced space rqmt for multiplicity-0 graces (no flag)
cc  Removed last sepsym in centered whole-bar rests, fixes volta height bug.
cc
cc Version 1.43
cc  Fix spacing for end-of-line signature change.
cc  Adjust left-shift of grace group for shifted accidentals.
cc  Put in extra space for left-shifted accidentals.
cc  Fix bug with dot-shift before accid-shift on chord note.
cc  Space-check for right-shifted main notes.
cc  Enable forcing stem direction of single notes in non-beamed xtups.
cc  Disallow clef change after last note before end of input block (pmxa)
cc  Print meter change before page break
cc  increase length of strings for \titles macro
cc version 1.42
cc  Loosen up input syntax for "X" commands.  Subroutine getx()
cc  "B" and "P" in "X" commands
cc mx09b
cc  Allow multiple rests at start of xtup
cc  Add 64th rest
cc  Fix xtup numbers over rests. (subroutine levrn)
cc  Initialize notcrd=.false. every gulp. Avoids undefined state with e.g.
cc    c za / ( c a ...
cc  Allow double dots to be shifted.
cc  Fix spacing with double dotted notes; permit splitting small note.
cc  Fix \dotted printout so it works with old compiler
cc mx08b
cc  Automatic spaces if needed for shifted accidentals.
cc  Some Dynamics
cc  Increase accid. horiz. shift resolution to .o5 (use one more bit in nacc)
cc version 1.41
cc  Allow ":" as last char of path name
cc  Dotted slurs "sb"
cc  Continue bar numbering at movement break "L[integer]Mc"
cc mx07b
cc  Whole-bar rests with double lines of music.  Fixed all options ?
cc  Shift accidentals, either [+|-][integer][+|-][number] or [<|>][number].
cc  Option to suppress centering full-bar rests. "o"
cc mx06b
cc  Shift accid on left-shifted chord note.
cc  Rest as first note of xtup.
cc  Wrong slopes with small widths.  Scale slfac1 by widthpt_default/widthpt
cc  Allow Rb for single bar at movemnet break or end of piece. (islur(25))
cc  Change # of inst at a movement break.  noinst is total # and must be used
cc    in 1st movement.  ninow is current.  nspern(1,...,ninow) is current
cc    staves/inst, nsperi(1,...,noinst) is original.  rename tells whether to
cc    reprint names in parindent at a movement break.  Default is .false.
cc    unless ninow changes, then .true.  But can force either with r+/- as
cc    option in 'M'
cc mx04b
cc  Double-dotted notes, separate+beamed, main+chord, still no extra space.
cc  ???  Don't shift slur ends on whole notes.
cc  (pmxa) Write line number of error in pmxaerr.dat
cc mx02b
cc  Admit "RD" before "/" (search for "rptfq2:" )
cc  In doslur, for multi-line staves, single notes, check forced stem dir'n
cc    before setting stemup (used to set horiz offset).
ccccccccccccccccccccccccccccccc
      parameter (nm=24,nks=125,mv=24576,maxblks=9600)
c
c  FYI /all/ differs in appearance in function ncmid
c
      common /all/ mult(nm,200),iv,nnl(nm),nv,ibar,
     *   ivxo(600),ipo(600),to(600),tno(600),tnote(600),eskz(nm,200),
     *   ipl(nm,200),ibm1(nm,9),ibm2(nm,9),nolev(nm,200),ibmcnt(nm),
     *   nodur(nm,200),jn,lenbar,iccount,nbars,itsofar(nm),nacc(nm,200),
     *   nib(nm,15),nn(nm),lenb0,lenb1,slfac,musicsize,stemmax,
     *   stemmin,stemlen,mtrnuml,mtrdenl,mtrnmp,mtrdnp,islur(nm,200),
     *   ifigdr(2,125),iline,figbass,figchk(2),firstgulp,irest(nm,200),
     *   iornq(nm,0:200),isdat1(202),isdat2(202),nsdat,isdat3(202),
     *   isdat4(202),beamon(nm),isfig(2,200),sepsymq(nm),sq,ulq(nm,9)
      common /comipl2/ ipl2(nm,200)
ccccccccccccc
cc  islur  cc
ccccccccccccc
c  bit  meaning
c  0     slur activity on this note
c  1     t-slur here.
c  2     force 0-slope beam starting on this note
c  3     Double dotted note!
c  4     grace before main note
c  5     left repeat
c  6     right repeat
c  7     start Volta
c  8     doublebar
c  9     end Volta
c  10    on=>endvoltabox
c  11    on=>clefchange
c  12-14 0=>treble, ... , 6=>bass
c  15    on=> start new block for clef change (maybe diff. voice)
c  16    literal TeX string
c  17    1=up, 0=down stem for single note (override) See bit 30!
c  18    if on, prohibit beaming
c  19    if on, full bar rest as pause
c  20    Beam multiplicity down-up
c  21    Forced multiplicity for any beam including xtups
c  22-24 Value of forced multiplicity
c  25    single barline at movement break
c  26    doubleBAR (see bits 5,6,8)
c  27-28 Forced beam fine-tune height (1 to 3)
c  29    Blank rest
c  30    If on, get stem dir'n from bit 17
c  31    If on, suppress printing number with xtuplet starting here
ccccccccccccc
cc  ipl    cc
ccccccccccccc
c  0     blank barline (must be iv=1) (would have used islur but no room)
c  1     look left for K rests, moved here from iornq(30)
c  2     treblelowoct
c  3     Open notehead in forced beam xtup
c  4     Set if single stem tremolo
c  5-6   termulo multiplicity - 1
c  7 unused
c  8     left offset main note one headwidth
c  9     right offset main note one headwidth
c  10    chord present?
c  11-16 Forced beam height adjustment (-30 to +30)
c  17-22 Forced beam slope adjustment (-30 to +30)
c  23-26 Slur index for Way-after grace.  Inserted when slur is started.
c  27 5th bit for slur index for Way-after grace (100712)
c  28    key change: only in voice 1
c  29    Grace after main note. (Type A)
c  30    In forced beam.  Signals need to check beam heights
c  31    Grace way after main note. (stretch to next note, type W)
ccccccccccccc
cc  iornq  cc
ccccccccccccc
c  0     Ornament "(".  Was user-defined horizontal slur shift on this note
c               until 9/24/97; changed that to irest(21)
c  1-13  stmgx+Tupf._)
c  14    Down fermata, was F
c  15    Trill w/o "tr", was U
c  16-18 Editorial s,f,n
c  19-20 >^
c  21    "?" for editorial accid, w/ or w/o s,f,n
c  22    Set if ihornb governs ornament height.  Same in icrdorn.
c  23    Set in getorn if ANY note at time of this main note has ornament.
c             This is ONLY used in beamstrt to signal whether to do more
c             tests for whether ihornb is needed.  (ihornb is only needed
c             if nonchord+upbm, chord+upbm+top_note, chord+dnbm+bot_note)
c     (7/1/00)Also set if any dynamic, as ihornb will be needed when dnbm. 
c  24    Slur on after or way-after grace.  Use as signal to START slur.
c  25    Tweak orn ht. Same in icrdorn for chord note
c  26    Insert user-defined space before this note (was 22)
c  27    Arpeggio stop or start (if 2 at same time), or all-in-this-chord
c  28    caesura or breath
cc  29    blank barline (must be iv=1) (would have used islur but no room)
c  29    coda
cc  30    "Look-left" option for keyboard rest
c  30    Part-by-part segno oG
c  31    Set if any note (main or chord) has cautionary accid, for space checks
ccccccccccccc
cc  irest  cc
ccccccccccccc
c  0        rest=1, no rest = 0
c  1        There will be a vertical shift for number of this xtup
c  2        Set if 2-note tremolo starts here
c  3-4      nsolid, # of solid beams in 2-note tremolo
c  5-6      nindent, # of indented beams in 2-note tremolo
c  7        There is a horizontal shift for xtup number
c  9-13     Horiz shift, 1=>-1.5, ... , 31=>+1.5
c  14       Flip up/down-ness of xtup number
c  15       Single-voice, single note shift  X(...)[p]S
c  16       Start single-voice, multinote shift with this note X(...)[p]:
c  17       End single-voice, multinote shift after this note. Enter symbol
c              after note. X:
c  18       User-defined hardspace after last note of bar, *after* this note.
c              Value still stored in udoff(ivx,nudoff(ivx)), not with other
c              hardspaces in udsp, to avoid confusion with time checks.
c  19       Move the dot.  Data stored in ndotmv,updot,rtdot
c  20       Set if right-shifted main or chord note here.  Use for space checks.
c  21       User-defined hardspace in xtup
c  22       User-defined slur shift horizontal slur shift.
c  23       Set on last note before staff-jumping a beam.
c  24       Set on first note after staff-jumping a beam
c  25       Suppress rest centering. "ro"
c  26       Dynamic on this note
c  27       Set if left-shifted main or chord note here.  Use for space checks.
c  28       Set if xtup starts on this note.
c  29       Set on lowest-voice note at same time as 1st note after jump-beam. 
c  30       Set on note after end of jump-beam segment, to force new note group
c  31       Flag for cautionary accidental
ccccccccccccc
cc  nacc   cc
ccccccccccccc
c  0-1      0=no accid, 1=fl, 2=sh, 3=na
c  2        double
c  3        big
c  4-9      vertshift-32
c  10-16    20*(horiz. shift + 5.35) (Recentered ver 2.32) 
c  17       Midi-only accidental
c  18       2:1 xtup
c  19       Together with nacc(18), increase multiplicity by 1 and dot 1st note.
c  20       Set on last note of each seg except last seg of single-slope beam.
c  21       Set on 1st note of each seg except 1st seg of single-slope beam.
c  22-26    If .ne.0, printed xtup number for xtup starting on this note.       
c  27       Set for dotted xtup note.  Mult dur by 1.5, mult next by .5 & increase
c             multiplicity by 1
c  28       Set on main note of chord if accidentals are ordered.
c  29       Tag for chordal accidental shift...means add to autoshifts.
c  30-31    Set 30|31 if main note in a chord is part of a 2nd and needs to be shifted.
c             If upstem|downstem, main is upper|lower member of 2nd
c             Action is to interchange pitches only when notes are placed. 
ccccccccccccc
cc  mult   cc
ccccccccccccc
c  0-3      Multiplicity+8 (mult= # of flags)
c  4        Set if slope adjustment for xtup bracket
c  5-9      16+slope adjustment
c  10-15    New stem length, [0-63] => (-4,0,+27.5)
c  16-22    64+Vertical offset of xtup #
c  27       Stemlength override
cc  28-30    New stem length.
ccccccccccccc
cc  isdat1 cc
ccccccccccccc
c  13-17    iv
c  3-10     ip
c  11       start/stop switch
c  12       kv-1
c  19-25    ichar(code$)
c  26       force direction?
c  27       forced dir'n = up if on, set in sslur; also
c           final direction, set in doslur when beam is started, used on term.
c  28-31    ndxslur, set in doslur when beam is started, used on term.
ccccccccccccc
cc  isdat2 cc
ccccccccccccc
c  0        Chord switch.  Not set on main note.
c  1-2      left/right notehead shift.  Set only for chord note.
c  3        tie positioning
c  4        dotted slur
c  6-11     voff1 1-63  =>  -31...+31
c  12-18    hoff1 1-127 => -6.3...+6.3
c  19-25    nolev
c  26       \sluradjust    (p+s)
c  27       \nosluradjust  (p-s)
c  28       \tieadjust     (p+t)
c  29       \notieadjust   (p-t)
ccccccccccccc
cc  isdat3 cc
ccccccccccccc
c  0        set if midslur (at least one argument)
c  1        set if curve (2 more args)
c  2-7      32+first arg (height correction) (1st arg may be negative)
c  8-10     second arg (initial slope)
c  11-13    third arg (closing slope)
c  14-21    tie level for use in LineBreakTies
c  22-29    ncm for use in LineBreakTies
ccccccccccccc
cc  isdat4 cc  Set these all at turn-on using s option 
ccccccccccccc
c  0-5      Linebreak seg 1 voff 1-63  =>  -31...+31  
c  6-12     Linebreak seg 1 hoff 1-127 => -6.3...+6.3
c  16-21    Linebreak seg 2 voff 1-63  =>  -31...+31
c  22-28    Linebreak seg 2 hoff 1-127 => -6.3...+6.3
ccccccccccccc
c  icrdat   c
ccccccccccccc
c     0-7   ip within voice
c     8-11  ivx (together with 28th bit)
c     12-18 note level
c     19    accidental?
c     20-22 accidental value (1=natural, 2=flat, 3=sharp, 6=dflat, 7=dsharp)
c     23    shift left
c     24    shift right
c     25    arpeggio start or stop
c     26    flag for moved dot (here, not icrdot, since this is always reset!)
c     27    Midi-only accidental
c     28    (6/27/10) 5th bit for ivx, to allow up to 24 voices 
c     29    Tag for accidental shift...means add to autoshifts. 
c     31    Flag for cautionary accidental on chord note
ccccccccccccc
c  icrdot   c:
ccccccccccccc
c     0-6   10*abs(vertical dot shift in \internote) + 64
c     7-13  10*abs(horizontal dot shift in \internote) + 64
c     14-19 vert accidental shift-32
c     20-26 20*(horiz accidental shift+3.2)
c     27-29 top-down level rank of chord note w/accid. Set in crdaccs.
c
c  Bits in icrdorn are same as in iornq, even tho most orns won't go in crds.
c
cccccccccccccccccccccccccccccccc
      character*131072 bufq
      integer*2 lbuf(maxblks)
      common /inbuff/ ipbuf,ilbuf,nlbuf,lbuf,bufq
      character*10 figq
      common /comignorenats/ mbrhgt,newmbrhgt,ignorenats
      logical ignorenats,newmbrhgt
      character*1 ulq,sepsymq,sq,chax
      logical beamon,firstgulp,figbass,figchk,
     *        isfig,rptnd1,rptprev
      logical loop,lastchar,slint,svolta,evolta,onvolt,
     *        cwrest(nm),islast,inlast,optimize
      integer istop(80),numbms(nm),istart(80)
      real*4 xnsttop(75),xintstaff(75),hesk(23),hpts(23),poevec(nks),
     *        tstart(80),squez(80)
      character*1  clefq(nm),ulfbq,rptfq1,rptfq2,charq
      character*79 notexq,inameq
      character*40 pathnameq
      character*44 basenameq
      character*24 fmtq
      character*20 voltxtq
      character*120 instrq,titleq,compoq
      common /comlast/ islast,usevshrink
      logical usevshrink,stickyS,OptLineBreakTies,autofbon
      common /combjmp/ ivbj1,ivbj2,isbjmp,isbj2,multbj1
      common /comtitl/ instrq,titleq,compoq,headlog,inskip,ncskip,
     *    inhead
      common /cominbot/ inbothd
      common /comfig/ itfig(2,74),figq(2,74),ivupfig(2,74),nfigs(2),
     *                fullsize(nm),ivxfig2,ivvfig(2,74)
      common /comnsp/ space(80),nb,prevtn(nm),
     *    flgndv(nm),flgndb,eskgnd,ptsgnd,ivmxsav(nm,2),nvmxsav(nm)
      common /comstart/ facmtr
      common /comfb/ nfb(nm),t1fb(nm,40),t2fb(nm,40),ulfbq(nm,40),ifb,
     *               tautofb,autofbon,t1autofb
      common /comget/ lastchar,rptnd1,sluron(nm,2),fbon,ornrpt,stickyS,
     *       movbrk,movnmp,movdnp,movgap,parmov,fintstf,gintstf,
     *       rptprev,equalize,rptfq1,rptfq2
      common /combeam/ ibmtyp
      common /comnotes/ nnodur,lastlev,ndlev(nm,2),shifton,setis,notcrd,
     *       npreslur,was2(nm),ninow,nobar1,nsystp(75),ipage,
     *       OptLineBreakTies,HeaderSpecial
      common /comtop / itopfacteur,ibotfacteur,interfacteur,isig0,
     *   isig,lastisig,fracindent,widthpt,height,hoffpt,voffpt,idsig,
     *   lnam(nm),inameq(nm)
      common /comas1/ naskb,task(40),wask(40),elask(40)
      common /comas2/ nasksys,wasksys(800),elasksys(800)
      common /comas3/ ask(2500),iask,topmods
      common /comask/ bar1syst,fixednew,scaldold,
     *                wheadpt,fbar,poenom
      common /comslur/ listslur,upslur(nm,2),ndxslur,fontslur
     *                 ,WrotePsslurDefaults,SlurCurve
      common /comsln/ is1n1,is2n1,irzbnd,isnx
      common /comgrace/ ivg(37),ipg(37),nolevg(74),itoff(2,74),aftshft,
     *                nng(37),ngstrt(37),ibarmbr,mbrest,xb4mbr,
     *                noffseg,ngrace,nvolt,ivlit(83),iplit(83),nlit,
     *                graspace(37),
     *                lenlit(83),multg(37),upg(37),slurg(37),slashg(37),
     *                naccg(74),voltxtq(6),litq(83)
      logical upg,slurg,slashg,fbon,ornrpt,shifton,isbjmp,notcrd,was2,
     *        isbj2,fontslur,WrotePsslurDefaults,HeaderSpecial
      common /comivxudorn/ivxudorn(63)
      common /comtrill/ ntrill,ivtrill(24),iptrill(24),xnsktr(24),
     *                ncrd,icrdat(193),icrdot(193),icrdorn(193),nudorn,
     *                kudorn(63),ornhshft(63),minlev,maxlev,icrd1,icrd2
      character*128 litq,lnholdq
      logical upslur,bar1syst,vshrink,lrpt,rrpt,lrptpend,
     *   ispoi,bcspec,topmods,headlog,clchb,clchv(nm),flgndb,
     *   btest,sluron,setis,nobar1
      common /comcc/ ncc(nm),tcc(nm,10),ncmidcc(nm,10),
     *               maxdotmv(nm),ndotmv(nm),updot(nm,20),rtdot(nm,20)
      common /compoi/ ispoi
      common /combc/ bcspec
      common /comeon/ eonk,ewmxk
      common /spfacs/ grafac,acgfac,accfac,xspfac,xb4fac,clefac,emgfac,
     *                flagfac,dotfac,bacfac,agc1fac,gslfac,arpfac,
     *                rptfac,lrrptfac,dbarfac,ddbarfac,dotsfac,upstmfac,
     *                rtshfac
      common /comhsp/ hpttot(176)
      common /combmh/ bmhgt,clefend
      common /commvl/ nvmx(nm),ivmx(nm,2),ivx
      common /commac/ macnum,mrecord,mplay,macuse,icchold,lnholdq,endmac
      common /comudsp/udsp(50),tudsp(50),nudsp,udoff(nm,20),nudoff(nm)
      common /strtmid/ ihnum3,flipend(nm),ixrest(nm)
      common /comarp/ narp,tar(8),ivar1(8),ipar1(8),levar1(8),ncmar1(8),
     *                xinsnow,lowdot
      common /comnvi/ nsperi(nm),nspern(nm),rename,iiorig(nm)
      common /commus/ musize,whead20
      common /comtol/ tol
      common /comdyn/ ndyn,idyndat(99),levdsav(nm),ivowg(12),hoh1(12),
     *        hoh2(12),hoh2h1(2),ntxtdyn,ivxiptxt(41),txtdynq(41),
     *        idynda2(99),levhssav(nm),listcresc,listdecresc
      character*128 txtdynq
      logical mrecord,mplay,endmac,flipend,istype0,lowdot,rename,ispstie
      integer*2 mmidi
      logical restpend,relacc,notmain,twoline,ismidi,crdacc,equalize,
     *        ismbr,putmbr
      common /commidi/ imidi(0:nm),trest(0:nm),mcpitch(20),mgap,
     *       iacclo(0:nm,6),iacchi(0:nm,6),midinst(nm),
     *       nmidcrd,midchan(nm,2),numchan,naccim(0:nm),
     *       laccim(0:nm,10),jaccim(0:nm,10),crdacc,notmain,
     *       restpend(0:nm),relacc,twoline(nm),ismidi,mmidi(0:nm,mv),
     *       debugmidi
      logical debugmidi
      common /comcb/ nbc,ibcdata(36)
      common /comclefq/ clefq
      common /comArpShift/NumArpShift,IvArpShift(20),IpArpShift(20),
     *                    ArpShift(20)
      common /comligfont/ isligfont
      logical isligfont
      common /comInstTrans/ iInstTrans(nm),iTransKey(nm),iTransAmt(nm),
     *  instno(nm),nInstTrans,EarlyTransOn,LaterInstTrans
      logical EarlyTransOn,LaterInstTrans
      common /combibarcnt/ ibarcnt
      character*40 nmq
      common /commidisig/ midisig
      common /comclefrests/ centrests
      logical newclef, centrests
      common /comlyr/ inputmlyr
      logical inputmlyr
      common /combottop/ botamt,topamt,bottopgap
      logical bottopgap
      common /comis4bignv/ is4bignv,AIset
      logical is4bignv,AIset
      common /comhair/ idhairuse,idhair(nm)
      common /comc8flag/ c8flag(nm)
      logical c8flag
      common /comshort/ shortfrac,codafrac,ishort,mbrsum,nmbr,nocodabn,
     *  poefa
      real*4 poefa(125)
      logical nocodabn
      character*3 shortfraq,rendq
      bottopgap = .false.
      inputmlyr = .false.
      idhairuse = 0
      if (.not.optimize) then
        print*
        print*,'Starting second PMX pass'
        print*
        write(15,'(a)')'Starting second PMX pass'
      end if
      newclef = .false.
      centrests = .false.
      ncalls = ncalls+1
      islast = inlast
      macuse = 0
      isyscnt = 0
      stemmax = 8.2
      stemmin = 3.9
      stemlen = 6.0
      sq = chax(92)
      ignorenats = .false.
      newmbrhgt = .false.
      bcspec = .true.
      topmods = .false.
	ismbr = .false.
      read(12,'(a)')basenameq
      read(12,*)lbase
      read(12,*)fbar,wheadpt,etait,etatc,etacs1,etatop,
     *         etabot,inbothd,inhnoh,isig
      ilbuf = 1
      ipbuf = 0
      call getset(nv,noinst,mtrnuml,mtrdenl,mtrnmp,mtrdnp,xmtrnum0,
     *            npages,nsyst,musicsize,fracindent,istype0,inameq,
     *            clefq,sepsymq,pathnameq,lpath,isig0)
      if (ismidi) then
c
c  Initial key signature and meter for pickup bar
c  130313 Unless explicit miditranspose for all parts (to be dealt with later),
c    want concert sig (isig0) here. K+n+m will have changed sig to isig 
c        call midievent('k',isig,0)
c 130316 
c        call midievent('k',isig0,0)
c        call midievent('k',midisig,0)
c
c  Above is probably cosmetic
c        call midievent('k',midisig,0)
        if (xmtrnum0 .gt. tol) then
c
c  We have a pickup.  Some tricky stuff to get a meter:
c
          xntrial = xmtrnum0
          do 5 ip2 = 0 , 5
            if (abs(mod(xntrial,1.)) .lt. tol) go to 6
            xntrial = xntrial*2
5         continue
          print*,'Problem finding meter for pickup bar'
          xntrial = 1.
          ip2 = 0          
6         continue
          call midievent('m',nint(xntrial),2**ip2*mtrdenl)
        else
c
c  No pickup, enter the starting meter
c
          call midievent('m',mtrnuml,mtrdenl)
        end if          
      end if
c
c  Set musicsize from value passed in common, due to possible reset by S[n]m16
c
      musicsize = musize
      read(12,*)npages,widthpt,height,hoffpt,voffpt,nsyst,
     *   (nsystp(ipa),xnsttop(ipa),xintstaff(ipa),ipa=1,npages),iauto
c
c  If default width ever changes, must adjust this stmt.
c
      slfac1 = 0.00569*524./widthpt
      figbass = .false.
      read(13,*)ifig
      if (ifig .eq. 1) then
        figbass = .true.
        open(14,status='SCRATCH')
        write(14,'(a)')sq//'def'//sq//'fixdrop{'//sq//'advance'//sq//
     *      'sysno by 1'//sq//'ifcase'//sq//'sysno%'
      end if
      lastchar = .false.
      ibcoff = 0
      if (xmtrnum0 .gt. 0.) ibcoff = -1
      open(11,status='SCRATCH')
c
c  vshrink for the first page is calculated in topfile,
c  and if true set interstaff=10.  vshrink affects Titles.
c  Must also save vshrink for page ending.
c
      call topfile(basenameq,lbase,nv,clefq,noinst,musicsize,
     *      xintstaff(1),mtrnmp,mtrdnp,vshrink,fbar,fontslur)
c
c  ninow is working value of # of instruments.  noinst is max #, and # at start.
c
      ninow = noinst
c
c  Save original printed meter in case movement breaks
c
      movnmp = mtrnmp
      movdnp = mtrdnp
c
      if (islast .and. figbass .and. musicsize.eq.16) 
     *    write(11,'(a)')sq//'def'//sq//'figfont{'//sq//'eightrm}%'
c
      if (islast .and. isligfont) then
        if (musicsize .eq. 16) then
          write(11,'(a)')sq//'font'//sq//'ligfont=cmrj at 8pt%'
        else
          write(11,'(a)')sq//'font'//sq//'ligfont=cmrj at 10pt%'
        end if
      end if
      lenbeat = ifnodur(mtrdenl,'x')
      if (mtrdenl .eq. 2) lenbeat = 16
      lenb1 = mtrnuml*lenbeat
      if (mtrdenl .eq. 2) lenb1 = lenb1*2
      call setmeter(mtrnuml,mtrdenl,ibmtyp,ibmrep)
      lenb0 = nint(xmtrnum0*lenbeat)
      if (mtrdenl .eq. 2) lenb0 = lenb0*2
      if (lenb0 .ne. 0) then
        if (islast) write(11,'(a)')sq//'advance'//sq//'barno by -1'
        lenbar = lenb0
      else
        lenbar = lenb1
      end if
c
c  Initialize full-program variables
c
      fixednew = 0.
      scaldold = 0.
      fintstf = -1.
      gintstf = 1.
      nasksys = 0
      ibarcnt = 0
      iline = 0
      movbrk = 0
      isystpg = 0
      ipage = 1
      iccount = 128
      iask = 0
      nhstot = 0
      nb = 1
      if (.not.optimize) jprntb = 81
      idsig = 0
      iflagbot = 0
c
c  Next 5 are raise-barno parameters.  irzbnd is integer part of default level.
c
      irzbnd = 3
      if (isig.eq.3 .and. clefq(nv).eq.'t') irzbnd = 4
      is1n1 = 0
      isnx = 0
      SlurCurve = 0.
      ishort = 0
      nocodabn = .false.
      mbrsum = 0
      nmbr = 0
c
c 111109 Made global rather than per gulp
c
      ndyn = 0
      ispoi = .false.
      slint = .false.
      lrptpend = .false.
      rptnd1 = .false.
      rptfq2 = 'E'
      rptprev = .false.
      onvolt = .false.
      flgndb = .false.
      fbon = .false.
      shifton = .false.
      ornrpt = .false.
      setis = .false.
      lowdot = .false.
      rename = .false.
      nobar1 = .false.
      equalize = .false.
      usevshrink = .true.
	WrotePsslurDefaults = .false.
      OptLineBreakTies = .false.
      HeaderSpecial = .false.
c
c  vshrink is initialized in topfile
c
      stickyS = .false.
c
c  ixrest = 1 or 2 if xtup has started with a rest
c
      do 1 ivx = 1 , nm
        ixrest(ivx) = 0
        fullsize(ivx) = 1.
c
c  Set legacy note level to middle c as default
c
        ndlev(ivx,1) = 29
        ndlev(ivx,2) = 29
1     continue
      npreslur = 0
      nhssys = 0
      listslur = 0
      do 31 i = 1 , 202
        isdat1(i) = 0
        isdat2(i) = 0
31    continue
      nsdat = 0
c
c  Initialize flag for figures in any other voice than 1
c
      ivxfig2 = 0
c
c  Initialize for loop over gulps
c
      firstgulp = .true.
c
c  Start a gulp
c
30    loop = .true.
      notcrd = .true.
      isbjmp = .false.
      isbj2 = .false.
      autofbon = .false.
      tautofb = 0.
      nbars = 0
      nfigs(1) = 0
      nfigs(2) = 0
      ngrace = 0
      ntrill = 0
      ncrd = 0
      nudorn = 0
      nlit = 0
      nvolt = 0
      ibarmbr = 0
      nudsp = 0
c      ndyn = 0   ! 111109
      ntxtdyn = 0
      nbc = 0
      NumArpShift = 0
      do 3 i = 1 , 37
        graspace(i) = 0.
3     continue
c
c  Now initialize up to nv.  Do it in getnote as r'qd for 2nd voices per syst.
c  and also if nv increases in an 'M' directive.
c
      do 4 iv = 1 , nv
        call newvoice(iv,clefq(iv),.false.)
4     continue
c
c  Check if endsymbol was set earlier
c
      if (rptnd1) then
        rptnd1 = .false.
        rptfq2 = rptfq1
      else
c
c  Only use if movbrk>0, to signal default ('RD')
c
        rptfq2 = 'E'
      end if
      iv = 1
      ivx = 1
2     if (loop) then
c
c  Within this loop, nv voices are filled up for the duration of the block.
c  On exit (loop=.false.) the following are set: nnl(nv),itsofar(nv)
c  nolev(nv,nnl(nv)),nodur(..),accq(..),irest(..).
c  nbars is for this input block.
c  Only at the beginning of an input block will there be a possible mtr change,
c  signalled by a nonzero mtrnuml. (which will be re-zeroed right after change)
c
        call getnote(loop)
        if (lastchar) go to 40
        go to 2
      end if
c
c  Finished an input block (gulp).
c
      if (ismidi) then
c
c  Put rests into midi array for 2nd lines that were not used in this gulp.
c
        do 60 iv = 1 , nv
          if (twoline(iv) .and. nvmx(iv).eq.1) then
            if (firstgulp .and. lenb0.ne.0) then
              call addmidi(midchan(iv,2),0,0,0,
     *                   (nbars-1.)*lenbar+lenb0,.true.,.false.)
            else
              call addmidi(midchan(iv,2),0,0,0,1.*nbars*lenbar,.true.,
     *                     .false.)
            end if
          end if
60      continue
      end if
      nvolt = 0
      do 28 iv = 1 , nm
        nudoff(iv) = 0
        maxdotmv(iv) = ndotmv(iv)
        ndotmv(iv) = 0
28    continue
c
c  Put stuff at top of p.1.  Must wait until now to have read title info.
c
      if (ibarcnt .eq. 0) then
        call puttitle(inhnoh,xnsttop(ipage),
     *     etatop,sq,etait,etatc,etacs1,nv,vshrink,sepsymq)
        if (HeaderSpecial) 
c
c  Write special header for first page
c
     *       write(11,'(a)')chax(92)//'special{header=psslurs.pro}%'
      end if
      do 10 ibar = 1 , nbars
        ibarcnt = ibarcnt+1
        bar1syst = ibarcnt .eq. iauto
        ndig = max(0,int(log10(.001+ibarcnt+ibcoff)))
        if (islast)
     *    write(11,'(a11,i'//chax(50+ndig)//')')
     *              '% Bar count',ibarcnt+ibcoff
        if (ibar .ne. ibarmbr) then
          if (.not.optimize) call outbar(ibarcnt+ibcoff,jprntb)
        else
          if (.not.optimize) then
            write(*,'(/,a20,i4,a1,i4)')' Multibar rest, bars',
     *         ibarcnt+ibcoff,'-',ibarcnt+ibcoff+mbrest-1
            write(15,'(/,a20,i4,a1,i4)')' Multibar rest, bars',
     *         ibarcnt+ibcoff,'-',ibarcnt+ibcoff+mbrest-1
            jprntb = 0
          end if
          ibcoff = ibcoff+mbrest-1
c          if (ibar.eq.1 .and. firstgulp .and.
c     *       .not.btest(islur(1,1),5)) xb4mbr = facmtr*musicsize
          if (ibar.eq.1 .and. firstgulp .and.
     *       .not.btest(islur(1,1),5)) xb4mbr = -.2*musicsize
        end if
c
c  Move the read to after end-of-bar hardspace checks, so we get right poenom
c  at end of a line.
c       if (bar1syst) read(12,*) poenom
c
c  Check for clef at start of bar.  No slide yet.  Also flags at end of prev.
c  bar.  This block is run at the start of every bar.  May fail for flag at
c  end of last bar.  To account for necc. hardspaces, compute and store
c    nhssys = # of hard spaces for this system
c    hesk(nhssys) = elemskips avialable
c    hpts(nhssys) = hard points needed, including notehead
c  Here, merely insert placeholder into output.  Later, when poe is computed,
c  compute additional pts and store them in hpttot(1...nhstot).  Finally in
c  subroutine askfig, write true pts where placeholders are.
c
        ioff = 0
        if (ibar .gt. 1) ioff = nib(1,ibar-1)
        clchb = btest(islur(1,ioff+1),15)
        putmbr = .false.
        if (ismbr) then
          if (clchb) then
ccc
ccc  Clef change and multi-bar rest coming up.  Kluge to get space at end of rest.
ccc
            write(11,'(a)')sq//'let'//sq//'mbrt'//sq//'mbrest'
     *       //sq//'def'//sq//'mbrest#1#2#3{%'
            write(11,'(a14,f4.1,a)')sq//'mbrt{#1}{#2}{',musicsize*.55,
     *        '}'//sq//'global'//sq//'let'//sq//'mbrest'//sq//'mbrt}%'
cc
cc  RDT suggestion is inset blank barline - 160103 Abandoned
cc
c            write(11,'(a)')sq//'setemptybar'//sq//'bar'//sq//'qspace'
c     *        //sq//'advance'//sq//'barno-1'
          end if
          ismbr = .false.
          putmbr = .true.
        end if
        if (ibar .eq. ibarmbr) ismbr = .true. 
c
c  Set flag here so at start of next bar, if there's a clef change, can add space
c    after the mbr with the above kluge
c
        if (.not.(clchb .or. flgndb)) go to 23
c
c  Must check available space
c
        ptsndb = 0.
c
c  Zero out block signal
c
        if (clchb) islur(1,ioff+1) = ibclr(islur(1,ioff+1),15)
c
c  In this loop, we determine how much hardspace is needed (if any)
c  9/7/97  Note that for last bar in input block, if number of lines of
c    music decreases in new block, highest numbered ones won't be checked
c    since the loop below covers the new nvmx(iv), not necessarily the old
c    one.
c  4/18/98 Apparently nmxsav was a solution to the above problem
c
        do 16 iv = 1 , nv
        do 16 kv = 1 , nvmxsav(iv)
          ivx = ivmxsav(iv,kv)
          ptsndv = flgndv(ivx)*wheadpt
          ioff = 0
          if (ibar .gt. 1) then
            ioff = nib(ivx,ibar-1)
            ip = ioff
            if (ibar.gt.2) ip = ioff-nib(ivx,ibar-2)
c            prevtn(ivx) = tnote(iand(ipl(ivx,ip),255))
            prevtn(ivx) = tnote(ipl2(ivx,ip))
c
c If ibar=1 (1st bar in input block), prevtn(ivx) was set at end of makeabar.
c
          end if
c
c  Only allow clef changes when ivx <= nv
c
          if (ivx .le. nv) then
            clchv(iv) = clchb .and. btest(islur(iv,ioff+1),11)
            if (clchv(iv)) then
c
c  Clef change in this voice.  Turn off signal.  Get space avail.
c
              islur(iv,ioff+1) = ibclr(islur(iv,ioff+1),11)
              if (abs(prevtn(iv)-space(nb)).lt.tol) ptsndv =
     *             ptsndv+clefend*wheadpt
            end if
          end if
          ptsndb = max(ptsndb,ptsndv+wheadpt*xspfac)
16      continue
c
c ????  where is nb set???  nb probably in left over from makeabar
c
        esk = feon(space(nb)*squez(nb))
        ptsdflt = esk*poenom-wheadpt
c        if ((ptsndb.gt.ptsdflt.or.ptsgnd.gt.0.) .and. movbrk.eq.0) then
        if ((ptsndb.gt.ptsdflt.or.ptsgnd.gt.0.) .and. movbrk.eq.0
     *       .and. .not.putmbr) then
c
c  Must ADD hardspace!  So put in a placeholder, and store params for later.
c
          if (islast) write(11,'(a)')sq//'xardspace{    pt}%'
          nhssys = nhssys+1
          if (ptsndb-ptsdflt .gt. ptsgnd-poenom*eskgnd) then
            hesk(nhssys) = esk
            hpts(nhssys) = ptsndb+wheadpt
          else
            hesk(nhssys) = eskgnd
            hpts(nhssys) = ptsgnd+wheadpt
          end if
          fixednew = fixednew+hpts(nhssys)
          scaldold = scaldold+hesk(nhssys)
        end if
        if (clchb) then
          do 17 iv = 1 , nv
            if (clchv(iv)) then
              notexq = sq//'znotes'
              lnote = 7
              do 24 iiv = 2 , iv
                notexq = notexq(1:lnote)//sepsymq(iiv-1)
                lnote = lnote+1
24            continue
c
c  Recompute ioff since it will vary from voice to voice
c
              if (ibar .eq. 1) then
                ioff = 0
              else
                ioff = nib(iv,ibar-1)
              end if
c
c  Must call clefsym to get nclef, even if there is a movement break
c
              call clefsym(islur(iv,ioff+1),fmtq,lclef,nclef)
c
c If clefq = '8', must change '0' in pos'n 9 to '8'
c
              if (btest(ipl(iv,ioff+1),2)) fmtq = 
     *                           fmtq(1:8)//'8'//fmtq(10:10)
              if (movbrk.eq.0 .and.
     *            islast)
     *          write(11,'(a)')notexq(1:lnote)//fmtq(1:lclef)//sq//'en%'
              call wsclef(iv,ninow,nclef)
c
c  Set new flag to be used just outside this loop, to kluge
c    any calls to \CenterBar for full-bar rests, to make room for clef.
c
              newclef = .true.              
c
c 151220
c wrong test:
c If clefq = '8', must add eg \settrebleclefsymbol3\treblelowoct%
c Replaced with right one. But also, clefq(iv) seems to stay at 8 here,
c   and if we change from TLO to normal clef, need to resetclefsymbols
c
c              if (clefq(iv) .eq. '8') then
              if (btest(ipl(iv,ioff+1),2)) then
c
c Find instrument number for voice iv
c
                iv1 = 1
                do 1111 iinst = 1 , ninow
                  if (iv .lt. iv1+nspern(iinst)) go to 2222
                  iv1 = iv1+nspern(iinst)
1111            continue
                print*
                print*,'Should not be here in pmxb!'
                call stop1()
2222            continue
                if (iinst .le. 9) then
                  write(11,'(a20,i1,a)')sq//'settrebleclefsymbol',iinst,
     *              sq//'treblelowoct%'
                else
                  write(11,'(a20,i2,a)')sq//'settrebleclefsymbol',iinst,
     *              sq//'treblelowoct%'
                end if
                c8flag(iv) = .true.
              else
                if (clefq(iv) .eq. '8') then
                  write(11,'(a20)')sq//'resetclefsymbols'
                end if
              end if
            end if
17        continue
          if (islast) write(11,'(a)')sq//'pmxnewclefs'
        end if
23      continue
c
c  Kluge \CenterBar for whole bar rests if necessary
c
        if (newclef .and. centrests) then
c           write(11,'(a)')sq//'def'//sq//'value{11}%'
          nvalue = nint(.55*musicsize)
          if (nvalue .gt. 10) then
            write(11,'(a11,i2,a2)')
     *            sq//'def'//sq//'value{',nvalue,'}%'
          else
            write(11,'(a11,i1,a2)')
     *            sq//'def'//sq//'value{',nvalue,'}%'
          end if
        end if
        newclef = .false.
        centrests = .false.
c
c  End of loop for end-of-bar hardspaces and non-movbrk clef symbol.
c
        if (bar1syst) then
          read(12,*) poenom
        end if
c
c  Repeat symbols.  Haven't slid down yet, so use islur(1,nib(1,ibar-1)+1)
c
        if (ibar .eq. 1) then
          islnow = islur(1,1)
c          iornqnow = iornq(1,1)
          iplnow = ipl(1,1)
        else
          islnow = islur(1,nib(1,ibar-1)+1)
c          iornqnow = iornq(1,nib(1,ibar-1)+1)
          iplnow = ipl(1,nib(1,ibar-1)+1)
        end if
c
c  Check for R-symbols set at end of prior input block
c
        if (movbrk.eq.0 .and. rptfq2.ne.'E') then
          if (rptfq2 .eq. 'D') then
            islnow = ibset(islnow,26)
          else if (rptfq2 .eq. 'r') then
            islnow = ibset(islnow,6)
          else if (rptfq2 .eq. 'd') then
            islnow = ibset(islnow,8)
          else if (rptfq2 .eq. 'b') then
            islnow = ibset(islnow,25)
          else
            print*
            print*,'Illegal symbol with "R" at end of input block:',
     *        rptfq2
            call stop1()
          end if
          rptfq2 = 'E'
        end if
        if (iand(islnow,352) .ne. 0) then
c
c  Bit 5(lrpt), 6(rrpt), or 8(doublebar) has been set
c
          lrpt = btest(islnow,5)
          rrpt = btest(islnow,6)
          lrptpend = lrpt.and.bar1syst
          if (lrpt .and. .not.lrptpend) then
            if (rrpt) then
              if (islast) write(11,'(a)')sq//'setleftrightrepeat'
              fixednew = fixednew+wheadpt*lrrptfac-0.4
            else
              if (islast) write(11,'(a)')sq//'setleftrepeat'
              fixednew = fixednew+wheadpt*rptfac-0.4
            end if
          else if (rrpt) then
            if (islast) write(11,'(a)')sq//'setrightrepeat'
            fixednew = fixednew+wheadpt*rptfac-0.4
          else if (btest(islnow,8)) then
            if (islast) write(11,'(a)')sq//'setdoublebar'
            fixednew = fixednew+wheadpt*dbarfac-0.4
          end if
        else if (btest(islnow,26)) then
c
c  doubleBAR
c
          if (islast) write(11,'(a)')sq//'setdoubleBAR'
          fixednew = fixednew+wheadpt*ddbarfac-0.4
c        else if (btest(iornqnow,29)) then
        else if (btest(iplnow,0)) then
c
c  no bar line
c
c--        if (islast) write(11,'(a)')sq//'setzalaligne'
c++ 
          if (islast) then
            if (movbrk .eq. 0) then
              write(11,'(a)')sq//'setzalaligne'
            else
c
c  Encountered "Rz" at start of input block at start of new movement,  Must
c    use newmovement macro with arg 4 rather than setzalaligne, since former 
c    already redefines stoppiece.
c
              rptfq2 = 'z'
            end if
          end if
c++
          fixednew = fixednew-0.4
        end if
c
c  1st and 2nd endings
c
        svolta = btest(islnow,7)
        evolta = btest(islnow,9)
        if (evolta) then
          if (btest(islnow,10)) then
            if (islast) write(11,'(a)')sq//'endvoltabox'
          else
            if (islast) write(11,'(a)')sq//'endvolta'
          end if
          onvolt = .false.
        end if
        if (svolta) then
          nvolt = nvolt+1
          lvoltxt = index(voltxtq(nvolt),' ')-1
          if (lvoltxt .eq. 1) then
            if (islast) write(11,'(a)')sq//'Setvolta'
     *          //voltxtq(nvolt)(1:1)//'%'
          else
            if (islast) write(11,'(a)')sq//'Setvolta'//'{'//
     *        voltxtq(nvolt)(1:lvoltxt)//'}%'
          end if
          onvolt = .true.
        end if
        if (ibar .gt. 1) then
          ipnow = nib(1,ibar-1)+1
        else
          ipnow = 1
        end if
        iplnow = ipl(1,ipnow)
        if (bar1syst) then
c
c  If listslur>0, then there is at least one slur or tie carried over the break
c
          ispstie = .false.
          if (OptLineBreakTies .and. .not.fontslur .and. listslur.ne.0
     *         .and. islast) 
     *          call LineBreakTies(isdat1,isdat2,isdat3,isdat4, nsdat,
     *                             ispstie,sepsymq)
          iline = iline+1
c
c  End an old system, Start a new system
c
c  Reduce space before mbrest at start of system
c  Need this even if no accidentals in key signature  
c
          if (ibar.eq.ibarmbr) xb4mbr = -.2*musicsize
c
          if (iline .ne. 1) then
c
c  Not first line.
c  Get corrected poe = points/elemskip for *previous* system
c
            wdpt = widthpt*(1-fracindent)
            poe = (wdpt-fsyst*musicsize-0.4*nbarss-fixednew)/
     *            (elsktot+fbar*nbarss-scaldold)
            isyscnt = isyscnt+1
            poevec(isyscnt) = poe
c
c  Transfer data for system into global arrays to hold until very end
c
            do 9 ia = 1 , nasksys
              iask = iask+1
              ask(iask) = (wasksys(ia)/poe-abs(elasksys(ia)))
     *                    /poefa(iline-1)
c
c  Only admit negative ask if it was user-defined space, signalled by elask<=0.
c
              if (elasksys(ia).gt.0) ask(iask)=dim(ask(iask),0.)
9           continue
            do 25 ia = 1 , nhssys
              nhstot = nhstot+1
              hpttot(nhstot) = max(hpts(ia)-hesk(ia)*poe,0.)
25          continue
c
c  Reset counters for new system
c
            scaldold = 0.
            fixednew = 0.
            nasksys = 0
            nhssys = 0
          end if
c
c  End of if block for first bar of non-first system. Still 1st bar, any system
c
          if (islast.and.figbass) write(11,'(a)')sq//'fixdrop%'
          isystpg = isystpg+1
c
c  Try moving the next stmt way down, to fix a bug and get \eject printed at
c  end of single-system page.
c          if (isystpg .eq. nsystp(ipage)) isystpg = 0
          read(12,*)nbarss,elsktot,fsyst,frac,eonk,ewmxk
          if (iline .gt. 1) fracindent=frac
          if (figbass) then
            ifigdr(1,iline) = 4
            ifigdr(2,iline) = 4
          end if
          slfac = slfac1*musicsize*elsktot
          if (iline .ne. 1) then
c
c  For the line just _finished_, put figdrop in separate file.
c
            if (figbass) write(14,'(a9,i2,a10,i2,1x,a4)')
     *          sq//'figdrop=',ifigdr(1,iline-1),
     *         ' '//sq//'figdtwo=',ifigdr(2,iline-1),sq//'or%'
c
c  Check slurs in top staff for interference w/ barno. Only check when
c  # if digits in barno >= |isig|  But to keep on/off phasing, must ALWAYS
c  keep track of ons and offs when |isig|<=3.
c
            ndigbn = int(alog10(ibarcnt+ibcoff+.01))+1
            isnx = 0
            if (ndigbn.ge.iabs(isig) .and. is1n1.gt.0) then
c
c  There's a slur in top voice over the line break, hgt=is1n1, idcode=is2n1
c  Look for termination in remainder of this input block.  If not found,
c  just use is1n1.  Remember, haven't slid down yet.
c
              ioff = 0
              if (ibar .gt. 1) ioff = nib(ivmx(nv,nvmx(nv)),ibar-1)
              do 50 isdat = 1 , nsdat
                if (igetbits(isdat1(isdat),5,13).eq.ivmx(nv,nvmx(nv))
     *              .and. .not.btest(isdat1(isdat),11)
     *              .and. igetbits(isdat1(isdat),7,19).eq.is2n1) then
c
c  Found slur ending.  Just check note height, can't do fine adjustments.
c
c                 is1n1 = max(is1n1,igetbits(isdat2(nsdat),7,19))
                  is1n1 = max(is1n1,igetbits(isdat2(isdat),7,19))
                  go to 51
                end if
50            continue
c
c  If exiting loop normally, did not find end of slur.  c'est la vie.
c
51            continue
              isnx = idim(is1n1,ncmid(nv,1)+1+irzbnd)
              if (isnx .gt. 0) then
c
c  AHA! Slur likely to interfere with barno.
c
c  Modified 090525 to use \bnrs
c
                slint = .true.
                fmtq = '(a16,i1,a14)'
                if (irzbnd+isnx .gt. 9) fmtq = '(a16,i2,a14)'
                if (islast) write(11,fmtq)sq//'def'//sq//'raisebarno{',
     *             irzbnd+isnx,'.5'//sq//'internote}%'
c                if (islast) then
c                  if (isnx .le. 9) then
c                    write(11,'(a5,i1,a2)')sq//'bnrs',isnx,'0%'
c                  else
c                    write(11,'(a6,i2,a3)')sq//'bnrs{',isnx,'}0%'
c                  end if
c                end if
c
              end if
            end if
            if (movbrk .gt. 0) then
c
c              movbrk = 0
c  Move the reset down, so can use movbrk>0 to stop extra meter prints.
c
c  New movement.  Redefine stoppiece, contpiece.  These will be called either
c     explicitly or as part of alaligne.
c  indsym = 0,1,2 for doubleBAR , doublebar, rightrepeat.
c     This is passed to \newmovement.
c
              if (rptfq2 .eq. 'E') rptfq2 = 'D'
              indsym = index('Ddrbz',rptfq2)-1
              rptfq2 = 'E'
c
c  Also check for Rd or Rr set the normal way
c
              if (btest(islnow,8)) then
                indsym = 1
              else if (btest(islnow,6)) then
                indsym = 2
              end if
              if (indsym .lt. 0) then
                print*
                print*,'Illegal end symbol before "/"'
                call stop1()
              end if
c
c  Check for continuation (no bar number reset)
c
              if (islast.and.nobar1) then
                write(11,'(a)')sq//'advance'//sq//'barno1'
     *            //sq//'global'//sq//'startbarno'//sq//'barno%'
c
c  Need above for shortening case mcm with Mc
c
              end if
c
c Per Rainer's suggestion, changing \nbinstruments via 3rd arg of \newmovement
c
c              if (movgap .lt. 10) then
c                if (islast) write(11,'(a12,2i1,a1)')
c     *                sq//'newmovement',movgap,indsym,'%'
c              else
c                if (islast) write(11,'(a13,i2,a1,i1,a1)')
c     *                sq//'newmovement{',movgap,'}',indsym,'%'
c              end if
              if (islast) then
                nmq = sq//'newmovement'
                lnmq = 12
                if (movgap .lt. 10) then
                  lnmq = 14
                  write(nmq(13:14),'(2i1)')movgap,indsym
                else
                  lnmq = 17
                  write(nmq(13:17),'(a1,i2,a1,i1)')'{',movgap,'}',indsym
                end if
                if (ninow .lt. 10) then
                  lnmq = lnmq+1
                  write(nmq(lnmq:lnmq),'(i1)')ninow
                else
                  lnmq = lnmq+4
                  write(nmq(lnmq-3:lnmq),'(a1,i2,a1)')'{',ninow,'}'
                end if
                lnmq = lnmq+1
                write(nmq(lnmq:lnmq),'(a1)')'%'
                if (ishort .eq. 4) then
c
c Ending Short-blank-coda
c 
                  write(11,'(a)')sq//'Endpiece}}%'
                end if
                write(11,'(a)')nmq(1:lnmq)
                if (ishort .eq. 2) then
                  write(11,'(a)')sq//'stoppiece}}%'
                  ishort = 5
                else if (ishort .eq. 4) then
                  write(11,'(a)')sq//'let'//sq//'stoppiece'//sq//
     *                           'holdstop%'
c
c  Since we bypass newmovement, need to set the vertical gap.
c
                  if (movgap .gt. 10) then
                    write(11,'(a6,i2,a11)')
     *                sq//'vskip',movgap,sq//'internote%'
                  else if (movgap .gt. 0) then
                    write(11,'(a6,i1,a11)')
     *                sq//'vskip',movgap,sq//'internote%'
                  end if
                  ishort = 6
                end if
                if ((ishort.eq.5.or.ishort.eq.6).and..not.rename) then
                  do 62 iinst = 1 , ninow
                    if (islast) then
                      if (iinst .lt. 10) then
                        write(11,'(a8,i1,a3)')sq//'setname',iinst,'{}%'
                      else
                        write(11,'(a9,i2,a4)')sq//'setname{',iinst,
     *                                                           '}{}%'
                      end if
                    end if
62                continue
                end if
                rename = .false.
              end if
c
c  Change generalmeter if necessary
c
              if (islast) then
                call wgmeter(mtrnmp,mtrdnp)
              end if
              mtrnuml = 0
c
c  (Moved all name-writing to getnote, right when 'M' is detected)
c
              if (btest(iplnow,28)) then
c
c  Key signature at movement break
c
                iplnow = ibclr(iplnow,28)
                if (isig .gt. 0) then
                  if (islast) write(11,'(a18,i1,a2)')
     *                sq//'generalsignature{',isig,'}%'
                else
                  if (islast) write(11,'(a18,i2,a2)')
     *                sq//'generalsignature{',isig,'}%'
                end if
                if (islast .and. LaterInstTrans) then
                  call Writesetsign(nInstTrans,iInstTrans,iTransKey,
     *              LaterInstTrans)
                end if              
              end if
              if (parmov .ge. -.1) then
c
c  Resent paragraph indentation
c
                ipi = parmov*widthpt+.1
                if (ipi .lt. 10) then
                  if (islast) write(11,'(a11,i1,a2)')
     *                sq//'parindent ',ipi,'pt'
                else if (ipi .lt. 100) then
                  if (islast)
     *              write(11,'(a11,i2,a2)')sq//'parindent ',ipi,'pt'
                else
                  if (islast)
     *              write(11,'(a11,i3,a2)')sq//'parindent ',ipi,'pt'
                end if
              end if
              if (ishort .eq. 6) then
                if (iflagbot .eq. 1) then
                  write(11,'(a)')sq//'vfill'//sq//'eject'
c                  iflagbot = 0
c                  ishort = 0
                else
                  write(11,'(a)')sq//'contpiece'
                end if
c
c  Zero ishort later trying to write alaligne
c
              end if
            end if  ! End of movement break stuff
            if (isystpg .eq. 1) then
c
c  First line on a page (not 1st page, still first bar).  Tidy up old page
c  then eject.
c
c  Removed this 5/13/01 as it was causing double endvoltas.  This probably
c  is only needed at the end in case there is no endvolta specified.
c              if (onvolt) then
cc                if (islast) write(11,'(a)')sq//'endvoltabox%'
cc                onvolt = .false.
c              end if
c
c             
c  Check for meter change at start of a new PAGE
c
              if (mtrnuml .gt. 0) then
c
c  Meter change at start of a new page.  Ugly repeated coding here.
c
                mtrnms = mtrnuml
                call setmeter(mtrnuml,mtrdenl,ibmtyp,ibmrep)
                mtrnuml = mtrnms
                if (movbrk .eq. 0 .and. islast) then 
                  call wgmeter(mtrnmp,mtrdnp)
                end if
              end if
c
c  Key signature change?
c
              if (btest(iplnow,28) .and. movbrk.eq.0) then
                notexq = sq//'xbar'//sq//'addspace{-'//sq//
     *            'afterruleskip}'//sq//'generalsignature{'
                lnote = 49
                if (isig .lt. 0) then
                  notexq = notexq(1:49)//'-'
                  lnote = 50
                end if
                if (islast) write(11,'(a)')
     *              notexq(1:lnote)//chax(48+abs(isig))//'}%'
                if (islast .and. LaterInstTrans) then
                  call Writesetsign(nInstTrans,iInstTrans,iTransKey,
     *              LaterInstTrans)
                end if
                if (islast .and. ignorenats)
     *             write(11,'(a)')sq//'ignorenats%'              
                if (islast) write(11,'(a)')sq//'zchangecontext'//sq
     *              //'addspace{-'//sq//'afterruleskip}'
     *              //sq//'zstoppiece'//sq//'PMXbarnotrue%'
              else if (mtrnuml.gt.0 .and. movbrk.eq.0) then
c
c  Meter change but no signature change
c
                if (islast)
     *          write(11,'(a)')sq//'xchangecontext'//sq//'addspace{-'//
     *              sq//'afterruleskip}'//sq//'let'//sq//'bnat'//sq//
     *              'barnoadd%'
                if (islast)
     *          write(11,'(a)')sq//'def'//sq//'barnoadd{'//sq//'let'//
     *              sq//'barnoadd'//sq//'bnat}'//sq//'zstoppiece%'
              else
                if (islast) then
                  if ((ishort.eq.4.or.ishort.eq.2) 
     *                         .and. iflagbot.eq.1) then
                    ntmp = ibarcnt-ibarcnt0-nmbr+mbrsum
                    if (ntmp .le. 9) then 
                      shortfraq(1:1) = char(48+ntmp)
                      lntmp = 1
                    else
                      write(shortfraq(1:2),'(i2)')ntmp
                      lntmp = 2
                    end if
                    write(11,'(a)')sq//'endpiece}}'//sq//'advance'//sq
     *                //'barno'//shortfraq(1:lntmp)
c
c Move down so can insert fil@begin
c                    iflagbot = 0
c                    ishort = 0
c
                  else if ((ishort.ne.6.and.ishort.ne.2) .or.
     *                       iflagbot.ne.1) then
                    write(11,'(a)')sq//'stoppiece%'
                  end if
                end if
              end if
c
c  This is the key spot when vshrink is used.  Value of vshrink here comes from 
c  just after the prior pagebreak, i.e., it is not affected by "Av" 
c  that may have been entered at this pagebreak, since that only affects usevshrink.
c  So choose page *ending* (with or without \vfill) depending on old vshrink.  Then
c  check value of usevshrink to reset vshrink if necessary for the new page, where
c  we have to set \interstaff and later call puttitle.  
c  Top of first page needs special treatment.  For this we use
c  novshrinktop, which was set in g1etnote on the first pass, since on 
c  second pass, vshrink at top of page one is dealt with in topfile, which is called
c  *before* any reading in any "Av" at the top of the first input block.
c
              if (.not.vshrink) then
                if (islast .and. bottopgap) then
                  if (abs(botamt) .gt. 1.e-6) then
                    if (botamt .gt. 9.95) then
                      fmtq = '(a,f4.1,a)'
                    else if (botamt .gt. 0.) then 
                      fmtq = '(a,f3.1,a)'
                    else if (botamt .gt. -9.95) then 
                      fmtq = '(a,f4.1,a)'
                    else
                      fmtq = '(a,f5.1,a)'
                    end if
                    write(11,fmtq)sq//'null'//sq//'vskip',botamt,
     *                         sq//'Internote%'                                    
                  end if
                end if
                xnstbot = xnsttop(ipage)*etabot/etatop
                if (xnstbot .lt. 9.95) then
                  fmtq = '(a,f3.1,a)'
                else
                  fmtq = '(a,f4.1,a)'
                end if
                if (islast) write(11,fmtq)sq//'vskip',xnstbot,
     *              sq//'Interligne'//sq//'eject%'
                if (islast .and. bottopgap) then
                  if (abs(topamt) .gt. 1.e-6) then
                    if (topamt .gt. 9.95) then
                      fmtq = '(a,f4.1,a)'
                    else if (topamt .gt. 0.) then 
                      fmtq = '(a,f3.1,a)'
                    else if (topamt .gt. -9.95) then 
                      fmtq = '(a,f4.1,a)'
                    else
                      fmtq = '(a,f5.1,a)'
                    end if
                    write(11,fmtq)sq//'null'//sq//'vskip',topamt,
     *                       sq//'Internote%'                                    
                  end if
                  bottopgap = .false.
                end if
              else
                if (islast) then
                  if (iflagbot.eq.1.and.ishort.eq.6)then
c
c cleanup for case bcm only
c
                    iflagbot = 0
                    ishort = 0
                  else
                    write(11,'(a)') sq//'vfill'//sq//'eject%'
                    if (iflagbot.eq.1.and.ishort.eq.2) then
c Clean up case bsl only
                      write(11,'(a)')sq//'makeatletter'//sq//'fil@begin'
     *                      //sq//'makeatother'
                      iflagbot = 0
                      ishort = 0
                    end if
                  end if
                end if
              end if
              ipage = ipage+1
c
c  Now that page is ejected, compute new vshrink
c
              vshrink = xintstaff(ipage).gt.20 .and. usevshrink
              if (vshrink) then
                xinsnow = 10
              else
                xinsnow = xintstaff(ipage)
              end if
              if (fintstf.gt.0 .and. ipage.gt.1) then
                xinsnow = xinsnow*fintstf/gintstf
                fintstf = -1.
              end if
              if (xinsnow .lt. 9.95) then
                fmtq = '(a,f3.1,a)'
              else if (xinsnow .lt. 99.95) then
                fmtq = '(a,f4.1,a)'
              else
                fmtq = '(a,f5.1,a)'
              end if
c
c  Vertical spacing parameters, then restart
c
              if (is4bignv) xinsnow = .95*xinsnow
              if (islast) then
                write(11,fmtq)sq//'interstaff{',xinsnow,
     *            '}'//sq//'contpiece'
              end if
c
c  Check for meter change at start of a new PAGE
c
              if (mtrnuml .gt. 0) then
c
c  Meter change at start of a new page
c
                call setmeter(mtrnuml,mtrdenl,ibmtyp,ibmrep)
                if (movbrk .eq. 0) then
                  if (islast) then
                    call wgmeter(mtrnmp,mtrdnp)
                  end if
                  if (mtrdnp .gt. 0) then
                    if (islast) write(11,'(a)')sq//'newtimes2%'
                    if (ibar .eq. ibarmbr) xb4mbr = -.2*musicsize
                  end if
                end if
              end if
c
c  If no real titles here, which there probably will never be, make vertical
c  space at page top with \titles{...}.  headlog=.false.<=>no real titles
c
              if (ishort.eq.4 .and. iflagbot.eq.1) then
                ishort = 0
                iflagbot = 0
                write(11,'(a)')sq//'makeatletter'//sq//'fil@begin'//sq//
     *                         'makeatother'
              end if
              call puttitle(inhnoh,xnsttop(ipage),etatop,
     *           sq,etait,etatc,etacs1,nv,vshrink,sepsymq)
              if (HeaderSpecial) 
c
c  Write special header for first page
c
     *           write(11,'(a)')chax(92)//'special{header=psslurs.pro}%'
            else
c
c  First bar of system, not a new page, force line break
c
              if (btest(iplnow,28)) then
c
c  Signature change
c
                notexq = sq//'xbar'//sq//'addspace{-'//sq//
     *            'afterruleskip}'//sq//'generalsignature{'
                lnote = 49
                if (isig .lt. 0) then
                  notexq = notexq(1:49)//'-'
                  lnote = 50
                end if
                if (islast) write(11,'(a)')notexq(1:lnote)
     *                   //chax(48+abs(isig))//'}%'
                if (islast .and. LaterInstTrans) then
                  call Writesetsign(nInstTrans,iInstTrans,iTransKey,
     *              LaterInstTrans)
                end if              
                if (islast) write(11,'(a)')sq//'advance'//sq//'barno-1%'
                if (mtrnuml .ne. 0) then
c
c  Meter+sig change, new line, may need mods if movement break here.
c
                  call setmeter(mtrnuml,mtrdenl,ibmtyp,ibmrep)
                  if (islast) then
                    call wgmeter(mtrnmp,mtrdnp)
                    if (ignorenats) write(11,'(a)')sq//'ignorenats%'
                    write(11,'(a)')sq//'xchangecontext'//sq//
     *                'addspace{-'//sq//'afterruleskip}'
     *      //sq//'zstoppiece'//sq//'PMXbarnotrue'//sq//'contpiece%'
                    write(11,'(a)')sq//'addspace{-'//sq
     *                //'afterruleskip}%'
                    call wgmeter(mtrnmp,mtrdnp)
                    if (ignorenats) write(11,'(a)')sq//'ignorenats%'
                    write(11,'(a)')sq//'zchangecontext'
                  end if
                else
                 if (islast .and. ignorenats) 
     *                 write(11,'(a)')sq//'ignorenats%'
                 if (islast) write(11,'(a)')sq//'xchangecontext'//sq//
     *               'addspace{-'//sq//'afterruleskip}'
     *      //sq//'zstoppiece'//sq//'PMXbarnotrue'//sq//'contpiece%'
                end if
              else if (mtrnuml.eq.0.and.(ishort.eq.0.or.ishort.eq.6))
     *                          then
c
c  No meter change
c
                if (islast) then
                  if (ishort .ne. 6) then
                    write(11,'(a)')sq//'alaligne'
                  else
                    ishort = 0
                  end if
                end if
              else if (ishort .ne. 0) then
                if (ishort .eq. 1) then      ! Start short line
                    write(shortfraq,'(f3.2)')shortfrac
                    write(11,'(a)')sq//'stoppiece'//sq//'parindent 0pt'
                    write(11,'(a)')sq//'vskip'//sq//'parskip'
                    write(11,'(a)')sq//'hbox to'//sq//
     *                'hsize{'//sq//'vbox{'//
     *                sq//'hsize='//shortfraq//sq//'hsize%'
                    write(11,'(a)')sq//'contpiece'
                    write(11,'(a)')sq//'makeatletter'//sq//'fil@begin'
     *                //sq//'makeatother'
                    ibarcnt0 = ibarcnt
                    mbrsum = 0
                    ishort = 2
                    nmbr = 0
                else if (ishort .eq. 2) then ! End short line, no new mvt
                  ntmp = ibarcnt-ibarcnt0-nmbr+mbrsum
                  if (ntmp .le. 9) then 
                    shortfraq(1:1) = char(48+ntmp)
                    lntmp = 1
                  else
                    write(shortfraq(1:2),'(i2)')ntmp
                    lntmp = 2
                  end if
                  write(11,'(a)')sq//'stoppiece}}'//sq//'advance'//sq
     *                //'barno'//shortfraq(1:lntmp)
c
c  Not clear why we needed this
c                     //sq//'startbarno'//sq//'barno%'
                  write(11,'(a)')sq//'contpiece'
                  write(11,'(a)')sq//'makeatletter'//sq//'fil@begin'
     *                //sq//'makeatother'
                  ishort = 0
                else if (ishort .eq. 5) then ! End short line, new mvt
                  if (nobar1) then  ! Continue bar numbering
                    write(11,'(a)')sq//'advance'//sq
     *                //'barno'//char(ibarcnt-ibarcnt0+48)
     *                //sq//'startbarno'//sq//'barno%'
c
c Must leave in \startbarno to get msm with LMc to work.
c
                  end if
                  write(11,'(a)')sq//'startpiece'//sq//'addspace'//sq
     *                //'afterruleskip%'
                  ishort = 0
                else if (ishort .eq. 3) then ! Mid line gap, start coda
c May never come thru here. Stuff is done later at least for mcl.pmx
                    write(shortfraq,'(f3.2)')codafrac
                    write(11,'(a)')sq//'endpiece}'//sq//'advance'//sq
     *                //'barno'//char(ibarcnt-ibarcnt0+48)//sq//
     *                'startbarno'//sq//'barno%'
                    write(11,'(a)')sq//'hfill'//sq//
     *                'vbox{'//sq//'hsize='//shortfraq//'%'
                    write(11,'(a)')sq//'contpiece'
                    write(11,'(a)')sq//'makeatletter'//sq//'fil@begin'
     *                //sq//'makeatother'
                    ishort = 4
                else if (ishort .eq. 4) then ! End coda
                  ntmp = ibarcnt-ibarcnt0-nmbr+mbrsum
                  if (ntmp .le. 9) then 
                    shortfraq(1:1) = char(48+ntmp)
                    lntmp = 1
                  else
                    write(shortfraq(1:2),'(i2)')ntmp
                    lntmp = 2
                  end if
                  write(11,'(a)')sq//'endpiece}}'//sq//'advance'//sq
     *                //'barno'//shortfraq(1:lntmp)
                  write(11,'(a)')sq//'contpiece'
                  write(11,'(a)')sq//'makeatletter'//sq//'fil@begin'
     *                //sq//'makeatother'
                  ishort = 0
                  mbrsum = 0
                  nmbr = 0
                end if
              else if (mtrnuml .ne. 0) then
             
c
c  New meter, no new sig, end of line, not new page.
c
c \generalmeter{\meterfrac{3}{4}}%
c \xchangecontext\addspace{-\afterruleskip}%
c \zalaligne\generalmeter{\meterfrac{3}{4}}\addspace{-\afterruleskip}%
c \zchangecontext
c
                call setmeter(mtrnuml,mtrdenl,ibmtyp,ibmrep)
                if (movbrk .eq. 0) then
                  if (islast) then
                    call wgmeter(mtrnmp,mtrdnp)
                  end if
                  if (mtrdnp .gt. 0) then
                    if (islast) then
                      write(11,'(a)')
     *                    sq//'let'//sq//'bnat'//sq//'barnoadd'
     *                    //sq//'def'//sq//'barnoadd{'//sq//'empty}%'
                      write(11,'(a)')sq//'xchangecontext'//sq//
     *                   'addspace{-'//sq//'afterruleskip}'//sq//
     *                'zalaligne'//sq//'let'//sq//'barnoadd'//sq//'bnat'
                      call wgmeter(mtrnmp,mtrdnp)
                      write(11,'(a)')sq//'addspace{-'//sq//
     *                   'afterruleskip}'//sq//'zchangecontext'
                    end if
                    if (ibar .eq. ibarmbr) xb4mbr = -.2*musicsize
                  else
                    if (islast) write(11,'(a)')sq//'alaligne'
                  end if
                else
                  if (islast) write(11,'(a)')sq//'alaligne'
                end if
              end if
            end if
c
c  Modified 090525 to use \bnrs
c
            if (slint) then
              slint = .false.
              if (islast) write(11,'(a16,i1,a14)')sq//'def'//sq//
     *              'raisebarno{',irzbnd,'.5'//sq//'internote}%'
            end if
            movbrk = 0
          end if
c
c  Clean up if we squelched bar number reset at movement break
c
          if (nobar1) then
            if (islast) write(11,'(a)')sq//'startbarno1'
            nobar1 = .false.
          end if
          read(12,*,end=14)iauto
14        continue
c
c  We come thru here for the 1st bar of every system, so initialize is1n1
c
          is1n1 = 0
c
c  Brought down from above 
c
          if (isystpg .eq. nsystp(ipage)) then
            isystpg = 0
c
c  The following is to avoid moving this reset of isystpg, but still send a signal
c  down below for last system shortening events
c
            if (ishort.eq.2) iflagbot = 1
          end if
c
c  Check for linebreak ties
c
          if (ispstie) 
     *         call LineBreakTies(isdat1,isdat2,isdat3,isdat4,nsdat,
     *                             ispstie,sepsymq)
        else
c
c  Not first bar of system
c
          if (btest(iplnow,28)) then
c
c  Signature change
c
            if (mtrnuml .ne. 0) then
c
c  Meter+signature change mid line, assume no movement break
c
              call setmeter(mtrnuml,mtrdenl,ibmtyp,ibmrep)
              if (islast) call wgmeter(mtrnmp,mtrdnp)
              notexq = sq//'generalsignature{'
              lnote = 18
              if (isig .lt. 0) then
                notexq = notexq(1:18)//'-'
                lnote = 19
              end if
              if (islast) then
                iptemp = 48+abs(isig)
                charq = chax(iptemp)
                notexq = notexq(1:lnote)//charq//'}%'
                lnote = lnote+3
                write(11,'(a)')notexq(1:lnote)
                if (islast .and. LaterInstTrans) then
                  call Writesetsign(nInstTrans,iInstTrans,iTransKey,
     *              LaterInstTrans)
                end if
              if (ignorenats) write(11,'(a)')sq//'ignorenats%'              
              write(11,'(a)')sq//'xchangecontext%'
              end if
              if (ibar .eq. ibarmbr) then
c
c  Compute space for multibar rest
c
                if (lastisig*isig .ge. 0) then
                  naccs = max(abs(lastisig),abs(isig))
                else
                  naccs = abs(lastisig-isig)
                end if
c                xb4mbr = (facmtr+naccs*.24)*musicsize
                xb4mbr = -.2*musicsize
              end if
            else
c
c  Signature change only
c
              notexq = sq//'xbar'//sq//'addspace{-'//sq//
     *            'afterruleskip}'//sq//'generalsignature{'
              lnote = 49
              if (isig .lt. 0) then
                notexq = notexq(1:49)//'-'
                lnote = 50
              end if
              if (islast)
     *          write(11,'(a)')notexq(1:lnote)//chax(48+abs(isig))//'}%'
              if (islast .and. LaterInstTrans) then
                  call Writesetsign(nInstTrans,iInstTrans,iTransKey,
     *              LaterInstTrans)
              end if              
              if (islast .and. ignorenats)
     *             write(11,'(a)')sq//'ignorenats%'              
              if (islast) write(11,'(a)')sq//'zchangecontext'//sq
     *            //'addspace{-.5'// sq//'afterruleskip}%'
              if (ibar .eq. ibarmbr) then
c
c  Compute space for multibar rest
c
                if (lastisig*isig .ge. 0) then
                  naccs = max(abs(lastisig),abs(isig))
                else
                  naccs = abs(lastisig-isig)
                end if
c                xb4mbr = naccs*.24*musicsize
                xb4mbr = -.2*musicsize
              end if
            end if
          else if (ishort .eq. 3) then
c
c  Gap before coda, assumed no signature change!
c
            ntmp = ibarcnt-ibarcnt0-nmbr+mbrsum
            if (ntmp .le. 9) then 
              shortfraq(1:1) = char(48+ntmp)
              lntmp = 1
            else
              write(shortfraq(1:2),'(i2)')ntmp
              lntmp = 2
            end if
            write(11,'(a)')sq//'endpiece}'//sq//'advance'//sq
     *        //'barno'//shortfraq(1:lntmp)
            if (nocodabn) then
              write(11,'(a)')sq//'nobarno'
              nocodabn = .false.
            end if
            write(shortfraq,'(f3.2)')codafrac
            write(11,'(a)')sq//'hfill'//sq//
     *        'vbox{'//sq//'hsize='//shortfraq//sq//'hsize%'
            write(11,'(a)')sq//'contpiece'
            write(11,'(a)')sq//'makeatletter'//sq//'fil@begin'
     *                //sq//'makeatother'
            ishort = 4
          else if (mtrnuml .eq. 0) then
c
c  No meter change
c
            if (islast) write(11,'(a)')sq//'xbar'
          else
c
c  Change meter midline
c
            call setmeter(mtrnuml,mtrdenl,ibmtyp,ibmrep)
            if (movbrk .eq. 0) then
              if (islast) then
                call wgmeter(mtrnmp,mtrdnp)
              end if
              if (mtrdnp .gt. 0) then
                if (islast) then
                  write(11,'(a,2i1,a)')sq//'newtimes0%'
                end if
c                if (ibar .eq. ibarmbr) xb4mbr = facmtr*musicsize
                if (ibar .eq. ibarmbr) xb4mbr = -.2*musicsize
              else
                if (islast) write(11,'(a)')sq//'xbar'
              end if
            end if
          end if
        end if
c
c  Now that xbar's are written, can put in left-repeats at line beginnings
c
        if (lrptpend) then
          if (islast) write(11,'(a)')sq//'advance'//sq//'barno-1'
     *        //sq//'leftrepeat'
          lrptpend = .false.
        end if
        if (ibar .gt. 1) then
c
c  For bars after first, slide all stuff down to beginning of arrays
c
          do 11 iv = 1 , nv
          do 11 kv = 1 , nvmx(iv)
            ivx = ivmx(iv,kv)
            ioff = nib(ivx,ibar-1)
            do 12 ip = 1 , nib(ivx,ibar)-ioff
              nolev(ivx,ip) = nolev(ivx,ip+ioff)
              nodur(ivx,ip) = nodur(ivx,ip+ioff)
              nacc(ivx,ip) = nacc(ivx,ip+ioff)
              irest(ivx,ip) = irest(ivx,ip+ioff)
              islur(ivx,ip) = islur(ivx,ip+ioff)
              ipl(ivx,ip) = ipl(ivx,ip+ioff)
              iornq(ivx,ip) = iornq(ivx,ip+ioff)
              mult(ivx,ip) = mult(ivx,ip+ioff)
              if (figbass .and. ivx.eq.1 .or. ivx.eq.ivxfig2) then
                if (ivx.eq.1) then
                  isfig(1,ip) = isfig(1,ip+ioff)
                else
                  isfig(2,ip) = isfig(2,ip+ioff)
                end if
              end if
12          continue
            if (ivx.le.nv .and. ncc(iv).gt.1) then
              islide = 0
              do 13 icc = 1 , ncc(iv)
                if (tcc(iv,icc) .le. lenbar) then
c
c  This time will drop <=0 when slid.
c
                  islide = icc-1
                  ncmidcc(iv,1) = ncmidcc(iv,icc)
                else
                  tcc(iv,icc-islide) = tcc(iv,icc)-lenbar
                  ncmidcc(iv,icc-islide) = ncmidcc(iv,icc)
                end if
13            continue
              ncc(iv) = ncc(iv)-islide
              tcc(iv,1) = 0.
            end if
11        continue
          do 15 ig = 1 , ngrace
            ipg(ig) = ipg(ig)-nib(ivg(ig),ibar-1)
            if (ibar .gt. 2) ipg(ig) = ipg(ig)+nib(ivg(ig),ibar-2)
15        continue
          do 21 il = 1 , nlit
            iplit(il) = iplit(il)-nib(ivlit(il),ibar-1)
            if (ibar .gt. 2) iplit(il) = iplit(il)+nib(ivlit(il),ibar-2)
21        continue
          do 22 it = 1 , ntrill
            iptrill(it) = iptrill(it)-nib(ivtrill(it),ibar-1)
            if (ibar .gt. 2) iptrill(it) =
     *                    iptrill(it)+nib(ivtrill(it),ibar-2)
22        continue
          do 27 icrd = 1 , ncrd
            ivx = iand(15,ishft(icrdat(icrd),-8))
     *               +16*igetbits(icrdat(icrd),1,28)
            ipnew = iand(255,icrdat(icrd))-nib(ivx,ibar-1)
            if (ibar .gt. 2) ipnew = ipnew+nib(ivx,ibar-2)
            icrdat(icrd) = iand(not(255),icrdat(icrd))
            icrdat(icrd) = ior(max(0,ipnew),icrdat(icrd))
27        continue
          do 29 iudorn = 1 , nudorn
            ivx = ivxudorn(iudorn)
            ipnew = iand(255,kudorn(iudorn))-nib(ivx,ibar-1)
            if (ibar .gt. 2) ipnew = ipnew+nib(ivx,ibar-2)
            kudorn(iudorn) = iand(not(255),kudorn(iudorn))
            kudorn(iudorn) = ior(max(0,ipnew),kudorn(iudorn))
29        continue
          do 42 idyn = 1 , ndyn
            idynd = idyndat(idyn)
            ivx = iand(15,idynd)+16*igetbits(idynda2(idyn),1,10)
            ipnew = igetbits(idynd,8,4)-nib(ivx,ibar-1)
c
c The following construction avoids array bound errors in some compilers
c
            if (ibar .gt. 2) then
              ipnew = ipnew+nib(ivx,ibar-2)
            end if
            ipnew = dim(ipnew,0)
            call setbits(idynd,8,4,ipnew)
            idyndat(idyn) = idynd
42        continue
          do 43 itxtdyn = 1 , ntxtdyn
            idynd = ivxiptxt(itxtdyn)
            ivx = iand(31,idynd)
            ipnew = igetbits(idynd,8,5)-nib(ivx,ibar-1)
            if (ibar .gt. 2) then
              ipnew = ipnew+nib(ivx,ibar-2)
            end if
            ipnew = dim(ipnew,0)
c            call setbits(idynd,8,4,ipnew)
            call setbits(idynd,8,5,ipnew)
            ivxiptxt(itxtdyn) = idynd
43        continue
          do 41 isdat = 1 , nsdat
            isdata = isdat1(isdat)
            ivx = ivmx(igetbits(isdata,5,13),igetbits(isdata,1,12)+1)
            ipnew = igetbits(isdata,8,3)-nib(ivx,ibar-1)
            if (ibar .gt. 2) then
              ipnew = ipnew+nib(ivx,ibar-2)
            end if
            ipnew = dim(ipnew,0)
            call setbits(isdata,8,3,ipnew)
            isdat1(isdat) = isdata
41        continue
          do 44 ibc = 1 , nbc
            ivx = iand(15,ibcdata(ibc))+16*igetbits(ibcdata(ibc),1,28)
            ipnew = igetbits(ibcdata(ibc),8,4)-nib(ivx,ibar-1)
            if (ibar .gt. 2) then
              ipnew = ipnew+nib(ivx,ibar-2)
            end if
            ipnew = dim(ipnew,0)
            call setbits(ibcdata(ibc),8,4,ipnew)
44        continue
          do 45 iarps = 1 , NumArpShift
            IpArpShift(iarps) = 
     *           IpArpShift(iarps)-nib(IvArpShift(iarps),ibar-1)
            if (ibar .gt. 2) then
              IpArpShift(iarps) = 
     *           IpArpShift(iarps)+nib(IvArpShift(iarps),ibar-2)
            end if
45        continue  
c
c  Bookkeeping for figures.  This will set nfigs = 0 if there are no figs left.
c  If there are figs left, it will reset all times relative to start of
c  current bar.
c
          do 46 ivx = 1 , 2
            if (figbass) then
              islide = 0
              do 20 jfig = 1 , nfigs(ivx)
                if (itfig(ivx,jfig) .lt. lenbar) then
c
c  This figure was already used
c
                  islide = jfig
                else
                  itfig(ivx,jfig-islide) = itfig(ivx,jfig)-lenbar
                  figq(ivx,jfig-islide) = figq(ivx,jfig)
                  itoff(ivx,jfig-islide) = itoff(ivx,jfig)
                  ivupfig(ivx,jfig-islide) = ivupfig(ivx,jfig)
                  ivvfig(ivx,jfig-islide) = ivvfig(ivx,jfig)
                end if
20            continue
              nfigs(ivx) = nfigs(ivx)-islide
            end if
            if (nfigs(2) .eq. 0) go to 47
46        continue
47        continue
        end if
c
c  End of sliding down for bars after first in gulp.
c
c  The following may not be needed by makeabar, but just in case...
c
        if (firstgulp .and. lenb0.ne.0) then
          if (ibar .eq. 1) then
            lenbar = lenb0
          else
            lenbar = lenb1
          end if
        end if
c
c  Equal line spacing stuff
c
        if (equalize .and. bar1syst) then
          if (isystpg .eq. 1) then
            write(11,'(a)')sq//'starteq%'
          else if (isystpg .eq. nsystp(ipage)-1) then
            write(11,'(a)')sq//'endeq%'
          end if
        end if
        call make1bar(ibmrep,tglp1,tstart,cwrest,squez,
     *      istop,numbms,istart)
        call make2bar(ninow,tglp1,tstart,cwrest,squez,
     *      istop,numbms,istart,clefq)
c
c  Hardspace before barline?
c
        hardb4 = 0.
        do 35 iv = 1 , nv
        do 35 kv = 1 , nvmx(iv)
          ivx = ivmx(iv,kv)
          if (btest(irest(ivx,nn(ivx)),18)) then
            nudoff(ivx) = nudoff(ivx)+1
            hardb4 = max(hardb4,udoff(ivx,nudoff(ivx)))
          end if
35      continue
        if (hardb4 .gt. 0.) then
          if (islast) write(11,'(a11,f5.1,a4)')sq
     *        //'hardspace{',hardb4,'pt}%'
c
c This was causing an incorrect poe in an example, which did not affect main
c   spacing, but did cause an extra accidental space to be too small
c
          fixednew = fixednew-hardb4
        end if
10    continue
      firstgulp = .false.
      lenb0 = 0
      go to 30
40    close(12)
      close(13)
      ilbuf = 1
      ipbuf = 0
      wdpt = widthpt
      if (iline .eq. 1) wdpt = widthpt*(1-fracindent)
      poe = (wdpt-fsyst*musicsize-0.4*nbarss-fixednew)/
     *            (elsktot+fbar*nbarss-scaldold)
      poevec(nsyst) = poe
      if (.not.islast) then
        close(11)
        close(16)
        if (figbass) close(14)
        return
      end if
      do 19 ia = 1 , nasksys
        iask = iask+1
        ask(iask) = (wasksys(ia)/poe-abs(elasksys(ia)))/poefa(iline)
        if (elasksys(ia).gt.0) ask(iask)=dim(ask(iask),0.)
19    continue
      do 26 ia = 1 , nhssys
        nhstot = nhstot+1
        hpttot(nhstot) = max(hpts(ia)-hesk(ia)*poe,0.)
26    continue
      if (islast .and.
     *    onvolt) write(11,'(a)')sq//'endvoltabox'
      rendq = '%  '
      if (ishort .ne. 0) rendq = '}}%'
      if (rptfq2 .ne. 'E') then
c
c Terminal repeat.  Right or double?
c
        if (rptfq2 .eq. 'r') then
          if (islast) write(11,'(a)')sq//'setrightrepeat'
     *       //sq//'endpiece'//rendq
        else if (rptfq2 .eq. 'd') then
          if (islast) write(11,'(a)')sq//'setdoublebar'//sq//'endpiece'
     *       //rendq
        else if (rptfq2 .eq. 'b') then
          if (islast) write(11,'(a)')sq//'endpiece'//rendq
        else if (rptfq2 .eq. 'z') then
          if (islast) write(11,'(a)')sq//'setzalaligne'//sq//'Endpiece'
     *       //rendq
        else
          print*
          print*,'R? , ? not "d","r",or"b","z"; rptfq2:',rptfq2
          write(15,*)'R? , ? not "d","r",or"b","z"; rptfq2:',rptfq2
          if (islast) write(11,'(a)')sq//'Endpiece'
        end if
      else
        write(11,'(a)')sq//'Endpiece'//rendq
      end if
      if (.not.vshrink) then
        if (islast .and. bottopgap) then
          if (abs(botamt) .gt. 1.e-6) then
            if (botamt .gt. 9.95) then
              fmtq = '(a,f4.1,a)'
            else if (botamt .gt. 0.) then 
              fmtq = '(a,f3.1,a)'
            else if (botamt .gt. -9.95) then 
              fmtq = '(a,f4.1,a)'
            else
              fmtq = '(a,f5.1,a)'
            end if
            write(11,fmtq)sq//'null'//sq//'vskip',botamt,
     *                         sq//'Internote%'                                    
          end if
        end if
        xnstbot = xnsttop(ipage)*etabot/etatop
        if (xnstbot .lt. 9.95) then
          fmtq = '(a,f3.1,a)'
        else
          fmtq = '(a,f4.1,a)'
        end if
        if (islast) write(11,fmtq)sq//'vskip',xnstbot,
     *      sq//'Interligne'//sq//'eject'//sq//'endmuflex'
        if (islast) write(11,'(a)')sq//'bye'
      else
        if (islast) write(11,'(a)')
     *     sq//'vfill'//sq//'eject'//sq//'endmuflex'
        if (islast) write(11,'(a)')sq//'bye'
      end if
      rewind(11)
      if (figbass) then
        write(14,'(a9,i2,a10,i2,1x,a5)')
     *     sq//'figdrop=',ifigdr(1,iline),
     *    ' '//sq//'figdtwo=',ifigdr(2,iline),sq//'fi}%'
        rewind(14)
      end if
      call askfig(pathnameq,lpath,basenameq,lbase,figbass,istype0)
      if (.not.optimize) then
        print*
        print*,'Writing '
     *            //pathnameq(1:lpath)//basenameq(1:lbase)//'.tex'
        print*,'Done with second PMX pass.'
        write(15,'(/,a)')'Writing '
     *            //pathnameq(1:lpath)//basenameq(1:lbase)//'.tex'
        write(15,'(a)')' Done with second PMX pass.  Now run TeX'
      end if
      return
      end
      subroutine poestats(nsyst,poe,poebar,devnorm)
      parameter (nks=125)
c
c  Compute avg. & norm. std. dev. of poe.
c
      real*4 poe(nks)
      sumx = 0.
      sumxx = 0.
      do 1 isyst = 1 , nsyst
        sumx = sumx+poe(isyst)
        sumxx = sumxx+poe(isyst)**2
1     continue
      devnorm = sqrt(nsyst*sumxx/sumx**2-1)
      poebar = sumx/nsyst
      return
      end
      subroutine precrd(ivx,ip,nolevm,nacc,ipl,irest,udq,
     *                  twooftwo,icashft)
c
c  Analyzes chords, data to be used with slurs on chords and plain chords.
c  Check for 2nds, shift notes if neccesary.  
c       ipl(10) chord present
c       irest(20) set if any note is right shifted
c       irest(27) set if any note is left shifted
c       ipl(8|9) left|right shift main note
c       icrdat(23|24)   ditto     chord note
c       udq is updown-ness, needed to analyze 2nds.
c       levtabl(i)=0 if no note at this level, -1 if main note, icrd if chord note.
c       icrdot(icrd)(27-29) sequence order of chord note if accid, top down
c
      common /comtrill/ ntrill,ivtrill(24),iptrill(24),xnsktr(24),
     *                ncrd,icrdat(193),icrdot(193),icrdorn(193),nudorn,
     *                kudorn(63),ornhshft(63),minlev,maxlev,icrd1,icrd2
      logical btest,is2nd,twooftwo
      integer*4 kicrd(10),levtabl(88)
      character*1 udq
      do 11 i = 1 , 88
        levtabl(i) = 0
11    continue
      do 1 icrd1 = 1 , ncrd
        ivx1 = iand(15,ishft(icrdat(icrd1),-8))
     *       +16*igetbits(icrdat(icrd1),1,28)
        ip1 = iand(255,icrdat(icrd1))
        if (ip1.eq.ip .and. ivx1.eq.ivx) go to 2
1     continue
      print*
      print*,
     * 'Cannot find first chord note in precrd. Send source to Dr. Don!'
      call stop1()
2     continue
      maxlev = nolevm
      minlev = nolevm
      levtabl(nolevm) =  -1
      is2nd = .false.
	naccid = 0
      levmaxacc = -100
      levminacc = 1000
c
c  Check 1st 3 bits of nacc for accid on main note of chord. 
c
      if (iand(7,nacc) .gt. 0) then
        naccid = 1
c
c  Start list of notes with accid's.  There will be naccid of them. kicrd=0 if main,
c    otherwise icrd value for note with accidental.
c
        kicrd(1) = 0
        levmaxacc = nolevm
        levminacc = nolevm
c
c  Start the level-ranking
c
        icrdot0 = 1
      end if
      do 3 icrd2 = icrd1 , ncrd
        nolev = igetbits(icrdat(icrd2),7,12)
        levtabl(nolev) = icrd2
        maxlev = max(maxlev,nolev)
        minlev = min(minlev,nolev)
c
c  Check for accidental
c
        if (btest(icrdat(icrd2),19)) then
          naccid = naccid+1
          levmaxacc = max(levmaxacc,nolev)
          levminacc = min(levminacc,nolev)
c
c  Add this icrd to list of values for notes with accid's.
c
          kicrd(naccid) = icrd2
          if (.not.btest(nacc,28)) then
c
c  Order not forced, so get the level-ranking, top down
c
            iorder = 1
            do 12 iaccid = 1 , naccid-1
              if (kicrd(iaccid) .eq. 0) then
                if (nolevm .gt. nolev) then
                  iorder = iorder+1
                else
                  icrdot0 = icrdot0+1
                end if
              else
                if (igetbits(icrdat(kicrd(iaccid)),7,12) 
     *                                          .gt. nolev) then
                  iorder = iorder+1
                else
                  iold = igetbits(icrdot(kicrd(iaccid)),3,27)
                  call setbits(icrdot(kicrd(iaccid)),3,27,iold+1)
                end if
              end if
12          continue
            call setbits(icrdot(icrd2),3,27,iorder)
          end if
        end if
c
c  Exit loop if last note in this chord
c
        if (icrd2 .eq. ncrd) go to 4
        if (igetbits(icrdat(icrd2+1),8,0) .ne. ip  .or.
c     *      igetbits(icrdat(icrd2+1),4,8) .ne. ivx) go to 4
     *      igetbits(icrdat(icrd2+1),4,8)
     *        +16*igetbits(icrdat(icrd2+1),1,28) .ne. ivx) go to 4
3     continue
      print*
      print*,'Failed to find last chord note. Send source to Dr. Don!'
      call stop1()
4     continue
c
c  Now icrd1, icrd2 define range of icrd for this chord.
c
c  Bypass autos-shifting if any manual shifts are present
c
      if (btest(irest,20) .or. btest(irest,27)) go to 10
c
c  Check for 2nds
c
      do 5 ilev = 1 , 87
        if (levtabl(ilev).ne.0 .and. levtabl(ilev+1).ne.0) then
c
c  There is at least one 2nd..
c
          if (udq .eq. 'u') then
c
c  Upstem. Start with 2nd just found and go up, rt-shifting upper 
c     member of each pair  
c
            ile = ilev
c
c  Set main-note flag for ANY right-shift
c
            irest = ibset(irest,20)
7           continue
            if (levtabl(ile+1) .lt. 0) then
c
c  Upstem, & upper member is main so must be rt-shifted. This would move
c    stem too, so we rt-shift the OTHER note, and set flag that signals
c    to interchange pitches just when these two notes are placed.
c
              nacc = ibset(nacc,30)
              icrdat(levtabl(ile)) = ibset(icrdat(levtabl(ile)),24)
            else
c
c  Upper member is chord note, must be rt-shifted
c
              icrdat(levtabl(ile+1)) = ibset(icrdat(levtabl(ile+1)),24)
            end if
            ile = ile+1
8           continue
            ile = ile+1
            if (ile .lt. 87) then
              if (levtabl(ile).ne.0 .and. levtabl(ile+1).ne.0) then
                go to 7                
              else
                go to 8  
              end if
            end if
            go to 10
          else
c
c  Downstem. Start at top and work down, left-shifting lower member of each pair.
c     We know that lowest pair is at (ilev,ilev+1).
c
            ile = 88
c
c  Set main-note flag for ANY right-shift
c
            irest = ibset(irest,27)
9           continue
            if (levtabl(ile).ne.0 .and. levtabl(ile-1).ne.0) then
              if (levtabl(ile-1) .lt. 0) then      
c                ipl = ibset(ipl,8)
c
c  Dnstem, & lower member is main so must be left-shifted. This would move
c    stem too, so we left-shift the OTHER note, and set flag that signals
c    to interchange pitches just when these two notes are placed.
c
                nacc = ibset(nacc,31)
                icrdat(levtabl(ile)) = ibset(icrdat(levtabl(ile)),23)
              else
c
c  Lower member is chord note, must be shifted
c
                icrdat(levtabl(ile-1)) = 
     *                               ibset(icrdat(levtabl(ile-1)),23)
              end if
              ile = ile-1
            end if
            ile = ile-1
            if (ile .ge. ilev+1) go to 9
            go to 10
          end if
        end if
5     continue
10    continue
c
c  Done with 2nds, now do accid's.  Call even if just one, in case left shifts.
c
c      if (naccid .gt. 1) call crdaccs(nacc,ipl,irest,naccid,
      if (naccid .ge. 1) call crdaccs(nacc,ipl,irest,naccid,
     *        kicrd,nolevm,levmaxacc,levminacc,icrdot0,twooftwo,icashft)
      return
      end
      subroutine printl(string)
      character*(*) string
c
c  Send string to console and to log file
c
      print*,string
      write(15,'(a)')string
      return
      end
      subroutine putarp(tnow,iv,ip,nolev,ncm,soutq,lsout)
      parameter (nm=24)
      common /comarp/ narp,tar(8),ivar1(8),ipar1(8),levar1(8),ncmar1(8),
     *                xinsnow,lowdot
      common /comtol/ tol
      common /comArpShift/NumArpShift,IvArpShift(20),IpArpShift(20),
     *                    ArpShift(20)
      common /commvl/ nvmx(nm),ivmx(nm,2),ivx
      logical lowdot,IsArpShift
      character*80 soutq
      character*79 notexq
      character*8 symq(2)
      character*1 chax
      data symq /'raisearp','arpeggio'/
c
c  NOTE iv in arg list is really ivx, referring to voice rather than staff.
c
c  Find which iarp, if any
c
      do 1 iarp = 1 , narp
        if (abs(tnow-tar(iarp)).lt.tol) go to 2
1     continue
c
c  If here, this is the *first* call for this arp.
c
      narp = narp+1
      tar(narp) = tnow+tol*.5
      ivar1(narp) = iv
      ipar1(narp) = ip
      levar1(narp) = nolev
      ncmar1(narp) = ncm
      return
2     continue
c
c  If here, this is *second* call at this time, narp points to values from 1st.
c
c  Check for shift. Set IsArpShift and iarps
c
      do 3 iarps = 1 , NumArpShift
        if (IvArpShift(iarps) .eq. ivar1(iarp) .and.  
     *      IpArpShift(iarps) .eq. ipar1(iarp)) then
          IsArpShift = .true.
          notexq = chax(92)//'loffset{'
          write(notexq(10:14),'(f3.1,a2)') Arpshift(iarps),'}{'
          lnote = 14
          go to 4
        end if
3     continue
      IsArpShift = .false.
      lnote = 0
4     continue
      if (iv .eq. ivar1(iarp)) then
c
c  Arp is in a single voice.
c
        levbot = min(levar1(iarp),nolev)-ncm+3
        invert = abs(levar1(iarp)-nolev)+1
      else
c
c  Arp covers >1 voice.  Lower voice has to be the first, upper is current and
c    is where the symbol will be written.
c  Check whether ivx's ivar1(iarp) & iv are in same staff
c
        do 5 iiv = 1 , nm
          if (ivmx(iiv,1).eq.ivar1(iarp) .or. (nvmx(iiv).eq.2.and.
     *         ivmx(iiv,2).eq.ivar1(iarp))) then
            iivivx1 = iiv
            go to 6
          end if
5       continue
        print*,'Screwup#1 in putarp'
        stop
6       continue
        do 7 iiv = 1 , nm
          if (ivmx(iiv,1).eq.iv .or. (nvmx(iiv).eq.2.and.
     *         ivmx(iiv,2).eq.iv)) go to 8
c
c  Found iv for both voices (iivivx1, iiv), done looking
c
7       continue
        print*,'Screwup#2 in putarp'
        stop
8       continue
        if (iivivx1 .eq. iiv) then
          nstaffshift = 0
        else
          nstaffshift = -nint(2*xinsnow)
        end if
c        levbot = -nint(2*xinsnow)+3+levar1(iarp)-ncmar1(iarp)
        levbot = nstaffshift+3+levar1(iarp)-ncmar1(iarp)
        invert = -levbot+4+nolev-ncm
      end if
c
c  isym will be (1,2) if invert is (even,odd).  If even, raise .5\internote
c
      isym = mod(invert,2)+1
      ilvert = (invert+1)/2
      if (levbot .ge. 0 .and. levbot .le. 9) then
c
c  Single digit
c
        if (.not.IsArpShift) then
          notexq = chax(92)//symq(isym)//chax(48+levbot)
        else
          notexq = notexq(1:lnote)//
     *                chax(92)//symq(isym)//chax(48+levbot)
        end if
        lnote = lnote+10
      else
        if (.not.IsArpShift) then
          notexq = chax(92)//symq(isym)//'{'
        else
          notexq = notexq(1:lnote)//chax(92)//symq(isym)//'{'
        end if
        lnote = lnote+10
        if (levbot .ge. -9) then
c
c  Need two spaces for number
c
          write(notexq(lnote+1:lnote+3),'(i2,a1)')levbot,'}'
          lnote = lnote+3
        else
          write(notexq(lnote+1:lnote+4),'(i3,a1)')levbot,'}'
          lnote = lnote+4
        end if
      end if
c      if (ilvert .le. 9) then
c        call addstr(notexq(1:lnote)//chax(48+ilvert),lnote+1,
c     *              soutq,lsout)
c      else
c        write(notexq(lnote+1:lnote+4),'(a1,i2,a1)')'{',ilvert,'}'
c        call addstr(notexq(1:lnote+4),lnote+4,soutq,lsout)
c      end if

      if (ilvert .gt. 9) then
        write(notexq(lnote+1:lnote+4),'(a1,i2,a1)')'{',ilvert,'}'
        lnote = lnote+4
	else
        notexq = notexq(1:lnote)//chax(48+ilvert)
        lnote = lnote+1
      end if
      if (IsArpShift) then
        notexq = notexq(1:lnote)//'}'
        lnote = lnote+1
      end if
      call addstr(notexq(1:lnote),lnote,soutq,lsout)
c
c  cancel out the stored time, to permit two arps at same time!
c
      tar(iarp) = -1.
      return
      end
      subroutine putast(elask,indxask,outq)
      character*129 outq,tag
      character*9 fmtq
      if (elask .ge. 0.) then
        if (elask.lt.0.995) then
          lp = 3
        else if (elask .lt. 9.995) then
          lp = 4
        else
          lp = 5
        end if
        write(fmtq,'(a5,i1,a3)')'(a2,f',lp,'.2)'
      else
        lp = 5
        fmtq = '(a2,f5.1)'
      end if
c
c  Overwrite as follows:  ...xyz*ask     *lmnop... ->
c                         ...xyz*ast{.nn}*lmnop...
c                         ...xyz*ast{n.nn}*lmnop...
c                         ...xyz*ast{nn.nn}*lmnop...
c  or for negative,       ...xyz*ast{-nn.n}*lmnop...
        tag = outq(indxask+9:)
        write(outq(indxask+3:),fmtq)'t{',elask
        outq = outq(1:indxask+4+lp)//'}'//tag
      return
      end
      subroutine putcb(ivx,ip,notexq,lnote)
      common /comcb/ nbc,ibcdata(36)
      character*79 notexq
      logical btest
c      ivxip = ivx+16*ip
	do 1 ibc = 1 , nbc
c        if (ivxip .eq. iand(1023,ibcdata(ibc))) go to 2
c        if (ivx.eq.iand(15,ibcdata(ibc))+16*igetbits(ibcdata(ibc),1,28)
c     *    .and. ip.eq.iand(1008,ibcdata(ibc))) go to 2
         ivxbc = iand(15,ibcdata(ibc))+16*igetbits(ibcdata(ibc),1,28)
         ipbc = igetbits(ibcdata(ibc),8,4)
         if (ivx .eq. ivxbc .and. ip .eq.ipbc) go to 2
1     continue
      call printl('Error in putbc, Call Dr. Don')
      stop
2     continue
      if (btest(ibcdata(ibc),27)) then
        lnote = 8
        notexq = char(92)//'pbreath'
      else
        lnote = 9
        notexq = char(92)//'pcaesura'
      end if
      ivshft = igetbits(ibcdata(ibc),6,13)
c??      ivshft = igetbits(ibcdata(ibc),6,13)-32
      if (ivshft .gt. 0) ivshft = ivshft-32
      if (ivshft.lt.0 .or. ivshft.gt.9) then
        notexq = notexq(1:lnote)//'{'
        lnote = lnote+1
        if (ivshft .lt. -9) then
          write(notexq(lnote+1:lnote+3),'(i3)')ivshft
          lnote = lnote+3
        else
          write(notexq(lnote+1:lnote+2),'(i2)')ivshft
          lnote = lnote+2
        end if        
        notexq = notexq(1:lnote)//'}'
        lnote = lnote+1
      else
        notexq = notexq(1:lnote)//char(48+ivshft)
        lnote = lnote+1
      end if
      ihshft = igetbits(ibcdata(ibc),8,19)
      if (ihshft .eq. 0) then
        notexq = notexq(1:lnote)//'0'
        lnote = lnote+1
      else
        hshft = .1*(ihshft-128)
c
c  -12.8<hshft<12.8
c
        notexq = notexq(1:lnote)//'{'
        lnote = lnote+1
        if (hshft .lt. -9.95) then
          write(notexq(lnote+1:lnote+5),'(f5.1)')hshft
          lnote = lnote+5
        else if (hshft.lt.-0.05 .or. hshft.gt.9.95) then
          write(notexq(lnote+1:lnote+4),'(f4.1)')hshft
          lnote = lnote+4
        else
          write(notexq(lnote+1:lnote+3),'(f3.1)')hshft
          lnote = lnote+3
        end if
        notexq = notexq(1:lnote)//'}'
        lnote = lnote+1
      end if
      return
	end
      subroutine putfig(ivx,ifig,offnsk,figcheck,soutq,lsout)
      parameter (nm=24)
      common /comfig/ itfig(2,74),figqq(2,74),ivupfig(2,74),nfigs(2),
     *                fullsize(nm),ivxfig2,ivvfig(2,74)
      character*10 figqq,figq
      character*80 soutq,notexq
      character*1 ch1q,ch2q,sq,chax
      character*5 nofq,nofaq
      character*4 Figutq,Conttq
      logical figcheck
      if (ivx .eq. 1) then
        Figutq = 'Figu'
        Conttq = 'Cont'
      else
        Figutq = 'Figt'
        Conttq = 'Cott'
      end if
      sq = chax(92)
      if (ivvfig(ivx,ifig) .ne. 0) then
c
c  Alter figdrop
c
        notexq = sq//'global'//sq//'advance'//sq//'figdrop'
        lnote = 23
        if (ivvfig(ivx,ifig) .lt. 0) then
          notexq = notexq(1:lnote)//'-' 
          lnote = lnote+1
        endif
        notexq = notexq(1:lnote)//char(48+abs(ivvfig(ivx,ifig))) 
        lnote = lnote+1
        call addstr(notexq,lnote,soutq,lsout)
      end if
      if (abs(offnsk) .gt. .0001) then
c
c  Write offset for floating figure, to two decimal places
c
        notexq = sq//'off{'
        if (-offnsk .lt. -9.995) then
          write(notexq(6:11),'(f6.2)')-offnsk
          lnote = 11
        else if (-offnsk.lt.-.995 .or. -offnsk.gt.9.995) then
          write(notexq(6:10),'(f5.2)')-offnsk
          lnote = 10
        else if (-offnsk.lt.-.0001 .or. -offnsk.gt..995) then
          write(notexq(6:9),'(f4.2)')-offnsk
          lnote = 9
        else
          write(notexq(6:8),'(f3.2)')-offnsk
          lnote = 8
        end if
        notexq = notexq(1:lnote)//sq//'noteskip}'
        call addstr(notexq,lnote+10,soutq,lsout)
      end if
      figq = figqq(ivx,ifig)
      ic = 1
c      nof = 0
c      nofa = -1
      nof = -ivupfig(ivx,ifig)
      nofa = -ivupfig(ivx,ifig)-1
c
c  Beginning of manual loop
c
1     ch1q = figq(ic:ic)
c
c  Exit when first blank is encountered
c
        if (ch1q .eq. ' ') go to 2
c
c  Starting a level.  Set up vertical offset.
c
c        lnof = 1
c        nofq = chax(nof+48)
c        if (nof .gt. 9) then
c          lnof = 2
c          nofq = '1'//chax(nof-10+48)
c        end if
c        if (nofa .eq.-1) then
c          lnofa = 2
c          nofaq = '-1'
c        else if (nofa .lt. 10) then
c          lnofa = 1
c          nofaq = chax(nofa+48)
c        else
c          lnofa = 2
c          nofaq = '1'//chax(nofa+38)
c        end if
        call istring(nof,nofq,lnof)
        call istring(nofa,nofaq,lnofa)
        if (ch1q .eq. '0') then
c
c  Continuation figure.  Next number is length (in noteskips). The number will 
c    end at the first blank or char that is not digit or decimal point. If 
c    colon, it is a separator and must be skipped
c
          icnum = ic+1
3         continue
          ic = ic+1
          if (index('0123456789.',figq(ic:ic)) .gt. 0) go to 3
          lnum = ic-icnum
          call addstr(sq//Conttq//nofq(1:lnof)//'{'
     *             //figq(icnum:ic-1)//'}',7+ic-icnum+lnof,soutq,lsout)
          if (figq(ic:ic) .ne. ':') ic=ic-1
        else if (ch1q.eq.'#'.or.ch1q.eq.'-'.or.ch1q.eq.'n') then
          ic = ic+1
          ch2q = figq(ic:ic)
          if (ch2q .eq. ' ') then
c
c  Figure is a stand-alone accidental, so must be centered
c
            if (ch1q .eq. '#') then
              call addstr(sq//Figutq//nofaq(1:lnofa)//
     *          '{'//sq//'sharpfig}',16+lnofa,soutq,lsout)
            else if (ch1q .eq. '-') then
              call addstr(sq//Figutq//nofaq(1:lnofa)//
     *          '{'//sq//'flatfig}',15+lnofa,soutq,lsout)
            else if (ch1q .eq. 'n') then
              call addstr(sq//Figutq//nofaq(1:lnofa)//
     *                '{'//sq//'natfig}',14+lnofa,soutq,lsout)
            end if
            go to 2
          else
c
c  Figure is an accidental followed by a number
c  First put the accidental (offset to the left)
c
            if (ch1q .eq. '#') then
              call addstr(sq//Figutq//
     *               nofaq(1:lnofa)//'{'//sq//'fsmsh}',
     *               13+lnofa,soutq,lsout)
            else if (ch1q .eq. '-') then
              call addstr(sq//Figutq//
     *               nofaq(1:lnofa)//'{'//sq//'fsmfl}',
     *               13+lnofa,soutq,lsout)
            else if (ch1q .eq. 'n') then
              call addstr(sq//Figutq//
     *               nofaq(1:lnofa)//'{'//sq//'fsmna}',
     *               13+lnofa,soutq,lsout)
            end if
c
c  Now put the number
c
            call addstr(sq//Figutq//nofq(1:lnof)//'{'//ch2q//'}',
     *           8+lnof,soutq,lsout)
          endif
        else if (ch1q .eq. '_') then
c
c  Placeholder only (for lowering a figure).  Don't do anything!
c
          continue
        else
c
c  Figure is a single number, maybe with s after
c
          call addstr(sq//Figutq//nofq(1:lnof)//'{',
     *           6+lnof,soutq,lsout)
          ch2q = figq(ic+1:ic+1)
          if (ch2q .eq. 's') then
c
c  Use a special character. Insert font call
c
            ic = ic+1
            call addstr(sq//'ligfont',8,soutq,lsout)
          end if             
          call addstr(ch1q//'}',2,soutq,lsout)
c          call addstr(sq//Figutq//nofq(1:lnof)//'{'//ch1q//'}',
c     *           8+lnof,soutq,lsout)
        end if
        if (ic .ge. 10) go to 2
        ic = ic+1
        nof = nof+4
        nofa = nofa+4
        go to 1
2     continue
      if (abs(offnsk) .gt. .0001) then
        notexq = sq//'off{'
        if (offnsk .lt. -9.995) then
          write(notexq(6:11),'(f6.2)')offnsk
          lnote = 11
        else if (offnsk.lt.-.995 .or. offnsk.gt.9.995) then
          write(notexq(6:10),'(f5.2)')offnsk
          lnote = 10
        else if (offnsk.lt.-.0001 .or. offnsk.gt..995) then
          write(notexq(6:9),'(f4.2)')offnsk
          lnote = 9
        else
          write(notexq(6:8),'(f3.2)')offnsk
          lnote = 8
        end if
        notexq = notexq(1:lnote)//sq//'noteskip}'
        call addstr(notexq,lnote+10,soutq,lsout)
      end if
      if (ifig .lt. nfigs(ivx)) then
        ifig = ifig+1
      else 
        nfigs(ivx) = 0
        figcheck = .false.
      end if
      return
      end
      subroutine putorn(iornq,nolev,nolevm,nodur,nornb,ulq,ibmcnt,ivx,
     *     ncm,islur,nvmx,nv,ihornb,stemlin,outq,lout,ip,islhgt,
     *     beamon,iscrd)
c
c  All args are individual array element *values* except nornb,ihornb,ulq.
c  notcrd = .true. if ornament is on main note.
c    nolevm is level of main note (for chords)
c
      parameter (nm=24)
      character*1 ulpzq,ulq(nm,9),udqq,sq,chax
      character*8 noteq
      character*79 notexq,outq
      integer*4 ihornb(nm,24),nornb(nm)
      logical btest,iscrd,usehornb,beamon
      common /comivxudorn/ivxudorn(63)
      common /comtrill/ ntrill,ivtrill(24),iptrill(24),xnsktr(24),
     *                ncrd,icrdat(193),icrdot(193),icrdorn(193),nudorn,
     *                kudorn(63),ornhshft(63),minlev,maxlev,icrd1,icrd2
      sq = chax(92)
      lout = 0
      usehornb = .false.
      if (nodur .lt. 64) then
        stemlen = stemlin
      else
        stemlen = 0.
      end if
c
c  Get up-downness. ulpzq is opposite from stem direction for both beams and
c    non beams.  Can use in name of ornament [ . or _ ]
c
      if (beamon) then
        if (ulq(ivx,ibmcnt) .eq. 'u') then
          ulpzq = 'l'
        else
          ulpzq = 'u'
        end if
      else
        if (udqq(nolevm,ncm,islur,nvmx,ivx,nv).eq.'l' ) then
          ulpzq = 'u'
        else
          ulpzq = 'l'
        end if
      end if
c
c  To enable >1 ornament on a note, next line is top of manual loop.
c
2     continue
c
cc  Bit # of last ornament (last of bits 0-21)
cc  Bit # of last ornament (last of bits 0-24)
c  Bit # of last ornament (last of bits 0-24,29-30)
      if (btest(iornq,29)) then
        ibit = 29
      else if (btest(iornq,30)) then
        ibit = 30
      else
        ibit = log2(iand(iornq,4194303))
c      ibit = log2(iand(iornq,33554431))
      end if
      iornt = 2**ibit
c
c  Begin routine to set height.  Bits 0-13: (stmgx+Tupf._)
c  14: Down fermata, was F  15: Trill w/o "tr", was U, 16-18: edit. s,f,n
c  19-20: >^, 21: ? (with or w/o 16-18)
c  and 29-30: C (coda), G (new segno)
c
c  Do not use beam height for . or _
c
      if (btest(iornq,22) .and. iand(iornt,6144).eq.0) then
c
c  Height is set by special beam stuff.  
c  Do not leave ihorn set, do separately for every ornament
c
        ihorn = ihornb(ivx,nornb(ivx))
        if (ulpzq .eq. 'u') ihorn = ihorn-2
c
c  Following flag tells whether to increment nornb when exiting the subroutine.
c
        usehornb = .true.
      else if (ibit .eq. 14) then
c
c  Down fermata.  Don't worry about upper chord notes.
c
        if (ulpzq .eq. 'l') then
          ihorn = min(nolev,ncm-3)
        else
          ihorn = min(nolev-stemlen,ncm-3.)
        end if
      else if (btest(iornt,13) .or. btest(iornt,0)) then
c
c  ( or )
c
        ihorn = nolev
      else if (iand(iornt,6144) .gt. 0) then
c
cc  Staccato . or tenuto _ , but not special beam stuff.  Need up-down info
c  NOTE: removed .&_ from special beam treatment.
c  Staccato . or tenuto _  Need up-down info
c
        if (.not.iscrd .or. (maxlev.ne.nolev.and.ulpzq.eq.'l') .or.
     *                      (minlev.ne.nolev.and.ulpzq.eq.'u')) then
          ihorn = nolev
        else if (maxlev .eq. nolev) then
          ulpzq = 'u'
          ihorn = max(nolev+stemlen,ncm+3.)
        else
          ulpzq = 'l'
          ihorn = min(nolev-stemlen,ncm-3.)
        end if
      else if (iscrd .and. nolev .eq. minlev) then
        if (ulpzq .eq. 'l') then
          ihorn = min(nolev-3,ncm-6)
        else
          ihorn = min(nolev-nint(stemlen)-3,ncm-6)
        end if
      else if (ibit.eq.10 .and. nolev.gt.90) then
c
c  Special treatment for fermata on a shifted rest
c
        ihorn = ncm+5
      else if (ulpzq.eq.'l') then
c
c  (iscrd and nolev=maxlev) or (.not.iscrd)
c
        ihorn = max(nolev+stemlen+2,ncm+5.)
      else if (ibit.eq.29 .or. ibit.eq.30) then
c
c  coda or new segno, no height tweak
c
        ihorn = ncm+5
      else
        ihorn = max(nolev+2,ncm+5)
      end if
      ioff = 0
      iclracc = 0
c
c  Begin routine to set name.  Bits 0-13: (stmgx+Tupf._)
c  14: Down fermata, was F  15: Trill w/o "tr", was U, 16-18: edit. s,f,n
c
      if (btest(iornt,2)) then
        notexq = sq//'shake'
        lnote = 6
      else if (btest(iornt,3)) then
        notexq = sq//'mordent'
        lnote = 8
      else if (btest(iornt,1)) then
        notexq = sq//'mtr'
        lnote = 4
      else if (btest(iornt,5)) then
        notexq = sq//'xtr'
        lnote = 4
      else if (btest(iornt,6)) then
        notexq = sq//'ptr'
        lnote = 4
      else if (btest(iornt,13)) then
        notexq = sq//'rpn'
        lnote = 4
      else if (btest(iornt,0)) then
        notexq = sq//'lpn'
        lnote = 4
      else if (btest(iornt,12)) then
        notexq = sq//ulpzq//'st'
        lnote = 4
      else if (btest(iornt,11)) then
        notexq = sq//ulpzq//'pz'
        lnote = 4
      else if (btest(iornt,8)) then
        notexq = sq//'upz'
        lnote = 4
        ioff = -2
      else if (btest(iornt,9)) then
        notexq = sq//'uppz'
        lnote = 5
        ioff = -2
      else if (btest(iornt,10)) then
        if (nodur .lt. 48) then
          notexq = sq//'fermataup'
        else
          notexq = sq//'Fermataup'
        end if
        lnote = 10
        ioff = -2
      else if (btest(iornt,14)) then
        if (nodur .lt. 48) then
          notexq = sq//'fermatadown'
        else
          notexq = sq//'Fermatadown'
        end if
        lnote = 12
      else if (btest(iornt,21)) then
c
c  "?" in editorial ornament.  Clear bit 16-18 after use, since ibit=21
c
        if (btest(iornq,16)) then
          notexq = sq//'qsharp'
          lnote = 7
          ioff = 2
          iornq = ibclr(iornq,16)
          iclracc = 16 
        else if (btest(iornq,17)) then
          notexq = sq//'qflat'
          lnote = 6
          ioff = 1
          iornq = ibclr(iornq,17)
          iclracc = 17
        else if (btest(iornq,18)) then
          notexq = sq//'qnat'
          lnote = 5
          ioff = 2
          iornq = ibclr(iornq,18)
          iclracc = 18
        else
          notexq = sq//'qedit'
          lnote = 6
          ioff = 0
        end if
      else if (btest(iornt,16)) then
        notexq = sq//'esharp'
        lnote = 7
        ioff = 2
      else if (btest(iornt,17)) then
        notexq = sq//'eflat'
        lnote = 6
        ioff = 1
      else if (btest(iornt,18)) then
        notexq = sq//'enat'
        lnote = 5
        ioff = 2
      else if (btest(iornt,19)) then
        notexq = sq//'usf'
        lnote = 4
        ioff = -2
      else if (btest(iornt,20)) then
        notexq = sq//'usfz'
        lnote = 5
        ioff = -2
      else if (btest(iornt,29)) then
        notexq = sq//'ccoda'
        lnote = 6
      else if (btest(iornt,30)) then
        notexq = sq//'ssegno'
        lnote = 7
      end if
      iudorn = 0
c
c  User-defined level shift of ornament from default?
c
      if (btest(iornq,25)) then
c
c  Find which (if any) element of kudorn has the shift.
c
        do 3 iudorn = 1 , nudorn
c          if (ibit .lt. 21) then
c          if (ibit.lt.21 .or. ibit.eq.29) then
          if (ibit.lt.21 .or. ibit.eq.29 .or. ibit.eq.30) then
            ibitt = ibit
c
c  Could have oes, but not oe? or oes?
c
          else if (iclracc .gt. 0) then
c
c  Earlier cleared edit. accid, meaning it was oes?
c
            ibitt = iclracc+6
          else
            ibitt = 21
          end if
          ibitt=ip+ishft(mod(ivx,16),8)+ishft(nolev,12)+ishft(ibitt,19)
c          if (ibitt .eq. iand(33554431,kudorn(iudorn))) go to 4
          if (ibitt .eq. iand(33554431,kudorn(iudorn)) 
     *      .and. ivx.eq.ivxudorn(iudorn)) go to 4
3       continue
c
c  Nothing shifted on this note; exit this if block
c
        iudorn = 0
        go to 5
4       continue
        ioffinc = iand(63,ishft(kudorn(iudorn),-25))-32
        if (ibit.eq.19 .and. ioffinc .lt. -7) then
c
c  Convert usf to lsf.  The reason has to do with positioning being impossile
c  for some mysterious reason when you drop \usf below the staff
c
          notexq = sq//'lsf'
          ioffinc = ioffinc+6
        end if
        ioff = ioff+ioffinc
      end if
5     continue
c
c  Shift level to avoid slur.  Conditions are
c   1.  There is a slur
c   2.  No user-defined orn height shift (btest(iornq,25))
c   3.  upslur (islhgt>0)
c   4.  ornament is not segno(4), ._)(11-13), down ferm(14) or "(" (0) Bin=30737
c   5.  islhgt+3 >=  height already computed.
c
      if (.not.btest(iornq,25) .and.
     *                islhgt.gt.0 .and. iand(iornt,30737).eq.0)
     *  ioff = ioff+dim(islhgt+3,ihorn     )
      call notefq(noteq,lnoten,ihorn+ioff,ncm)
      if (lnoten.eq.1) call addblank(noteq,lnoten)
      if (iand(iornt,32896) .gt. 0) then
c
c  T-trill or trill w/o "tr"
c
        call dotrill(ivx,ip,iornt,noteq,lnoten,notexq,lnote)
      else
        notexq = notexq(1:lnote)//noteq(1:lnoten)
        lnote = lnote+lnoten
      end if
      if (iudorn .gt. 0) then
        if (btest(kudorn(iudorn),31)) then
c
c  Horizontal shift
c
          lform = lfmt1(ornhshft(iudorn))
          write(noteq(1:lform),'(f'//chax(48+lform)//'.1)')
     *        ornhshft(iudorn)
          notexq = sq//'roffset{'//noteq(1:lform)//'}{'
     *           //notexq(1:lnote)//'}'
          lnote = lnote+lform+12
          ornhshft(iudorn) = 0.
          kudorn(iudorn) = ibclr(kudorn(iudorn),31)
        end if
      end if
c
c  Zero out the bit for ornament just dealt with.
c
      iornq = ibclr(iornq,ibit)
      if (lout .eq. 0) then
        outq = notexq(1:lnote)
      else
        outq = outq(1:lout)//notexq(1:lnote)
      end if
      lout = lout+lnote
c
c  Check bits 0-21, go back if any are still set
c  If bit 29 or 30 had been set, would have been used first time thru,
c    and you wouldn't use both coda and segno on same note
c
      if (iand(iornq,4194303) .gt. 0) go to 2
      if (usehornb) nornb(ivx) = nornb(ivx)+1
      return
      end
      subroutine putshft(ivx,onoff,soutq,lsout)
      parameter (nm=24)
      common /comudsp/udsp(50),tudsp(50),nudsp,udoff(nm,20),nudoff(nm)
      character*80 soutq,notexq
      character*1 sq,chax
      logical onoff
      sq = chax(92)
c
c  Start user-defined offsets X(...): or X(...)S
c
      if (onoff) nudoff(ivx) = nudoff(ivx)+1
c
c  Xoff is in pts.  Round off to nearest .1.  Will use at end of shift.
c
      xoff = udoff(ivx,nudoff(ivx))
      xoff = sign(int(10.*abs(xoff)+.5)/10.,xoff)
      if (.not.onoff) xoff = -xoff
      if (xoff .lt. -9.95) then
        ifmt = 5
      else if (xoff.lt.-.95 .or. xoff.gt.9.95) then
        ifmt = 4
      else
        ifmt = 3
      end if
      write(notexq,'(f'//chax(48+ifmt)//'.1)')xoff
         call addstr(sq//'off{'//notexq(1:ifmt)//'pt}',8+ifmt,
     *                   soutq,lsout)
      return
      end
      subroutine puttitle(inhnoh,xnsttop,etatop,sq,etait,etatc,
     *  etacs1,nv,vshrink,sepsymq)
c
c  Called once per page, at top of page!  If vshrink, only called for p.1.
c  Actual titles only allowed on p.1. (set by headlog).
c  3/18/01:  The above comment is probably bogus...can use Tt on later pages.
c
      common /comlast/ islast,usevshrink
      logical islast,usevshrink
      common /comtitl/ instrq,titleq,compoq,headlog,inskip,ncskip,
     *    inhead
      common /cominbot/ inbothd
      character*127 notexq
      character*120 instrq,titleq,compoq
      parameter (nm=24)
      character*1 sq,sepsymq(nm),chax
      logical headlog,vshrink
      notexq = sq//'znotes'
      lenline = 7
      do 22 iv = 1 , nv-1
        notexq = notexq(1:lenline)//sepsymq(iv)
        lenline = lenline+1
22    continue
      notexq = notexq(1:lenline)//sq//'zcharnote{'
      lenline = lenline+11
      if (.not.headlog) then
        inhead = inhnoh
      end if
      if (vshrink .and. usevshrink) then
        inhead = 16
      end if
      ndig = int(alog10(inhead+.01))+1
      write(notexq(lenline+1:lenline+ndig+10),'(i'//chax(48+ndig)//
     *    ',a10)')inhead,'}{'//sq//'titles{'
      lenline = lenline+ndig+10
c
c  Vertical skip at top of page (\Il) = etatop*glueil.  Needed whether
c    headers are present or not.
c
      glueil = xnsttop/etatop
      vskil = etatop*glueil
      if (vshrink .and. usevshrink) vskil = 2
      call writflot(vskil,notexq,lenline)
      if (.not.headlog) then
        if (islast) write(11,'(a)')notexq(1:lenline)
     *     //'}{}{0}{}{0}{}{0}}'//sq//'en%'
      else
        notexq = notexq(1:lenline)//'}{'
        lenline = lenline+2
        lcq = lenstr(instrq,120)
        if (lcq .gt. 0) then
          xitil = etait*glueil
          if (vshrink .and. usevshrink) xitil = 2
          notexq = notexq(1:lenline)//instrq(1:lcq)//'}{'
c
c Null out instrument once used
c
          instrq = ' '
          lenline = lenline+lcq+2
          call writflot(xitil,notexq,lenline)
        else
          notexq = notexq(1:lenline)//'}{0'
          lenline = lenline+3
        end if
        if (islast) write(11,'(a)')notexq(1:lenline)//'}%'
        notexq = '{'
        lenline = 1
        lcq = lenstr(titleq,120)
        if (lcq .gt. 0) then
          notexq = notexq(1:lenline)//titleq(1:lcq)
          lenline = lenline+lcq
        else
          call printl(' ')
          call printl('WARNING')
          call printl(
     *      '  In a title block, you have specified instrument and/or')
          call printl(
     *      '  composer but no title for the piece.')
        end if
        notexq = notexq(1:lenline)//'}{'
        lenline = lenline+2
        xtcil = etatc*glueil
        lcq = lenstr(compoq,120)
        if (lcq .eq. 0) xtcil = 2*xtcil
        if (vshrink .and. usevshrink) xtcil = 2
        call writflot(xtcil,notexq,lenline)
        notexq = notexq(1:lenline)//'}{'
        lenline = lenline+2
        if (lcq .gt. 0) then
          notexq = notexq(1:lenline)//compoq(1:lcq)//'}{'
          lenline = lenline+2+lcq
c
c  Null out compoq so it does not get written later
c
          compoq = ' '
          xcsil = etacs1*glueil
          if (vshrink .and. usevshrink) xcsil = 2
          call writflot(xcsil,notexq,lenline)
        else
          notexq = notexq(1:lenline)//'}{0'
          lenline = lenline+3
        end if
        if (islast) write(11,'(a)')notexq(1:lenline)//'}}'//sq//'en%'
        headlog = .false.
      end if
      return
      end
      subroutine putxtn(ntupv,iflop,multb,iud,wheadpt,poenom,
     *  nolev1,islope,slfac,xnlmid,islur,lnote,notexq,ncmid,nlnum,
c     *  eloff,iup,irest,usexnumt)
     *  eloff,iup,irest,mult,usexnumt)
c
c  Places digit for xtuplet.
c
      character*1 chax
      character*8 noteq
      character*79 notexq
      logical btest,usexnumt
      if (iflop.ne.0 .and. multb.gt.0) then
c
c Number goes on beam side, move R/L by .5 wheadpt for upper/lower
c
        eloff = eloff-0.5*iud*wheadpt/poenom
c
c  Number goes on beam side, must use beam parameters to set pos'n
c
        nlnum = nolev1+islope/slfac*eloff+iup*(multb+8)
        if (multb .ge. 2) nlnum = nlnum+iup
      else
        nlnum = nint(xnlmid-1+3*iud+iflop*11)
      end if
      if (.not.btest(islur,31)) then
c
c  Only print number when wanted.  First check vert, horiz offset
c
        if (btest(irest,1)) nlnum = nlnum+igetbits(mult,8,16)-64
        if (btest(irest,7)) eloff = eloff+
     *          (.1*iand(31,ishft(irest,-9))-1.6)*wheadpt/poenom
        if (.not.usexnumt) then
          notexq = chax(92)//'xnum{'
          lnote = 10
          istrtn = 7
        else
          notexq = chax(92)//'xnumt{'
          lnote = 11
          istrtn = 8
        end if
        if (eloff .lt. 0.995) then
          write(notexq(istrtn:istrtn+3),'(i1,f3.2)')0,eloff
        else if (eloff .lt. 9.995) then
          write(notexq(istrtn:istrtn+3),'(f4.2)')eloff
        else
          write(notexq(istrtn:istrtn+4),'(f5.2)')eloff
          lnote = lnote+1
        end if
        call notefq(noteq,lnoten,nlnum,ncmid)
        notexq = notexq(1:lnote)//'}'//noteq(1:lnoten)
        lnote = lnote+1+lnoten
        if (ntupv .lt. 10) then
          write(notexq(lnote+1:lnote+1),'(i1)')ntupv
          lnote = lnote+1
        else
          notexq = notexq(1:lnote)//'{'
          write(notexq(lnote+2:lnote+3),'(i2)')ntupv
          notexq = notexq(1:lnote+3)//'}'
          lnote = lnote+4
        end if
      end if
      return
      end
      subroutine read10(string,lastchar)
      parameter (maxblks=9600)
      character*131072 bufq
      integer*2 lbuf(maxblks)
      common /inbuff/ ipbuf,ilbuf,nlbuf,lbuf,bufq
      character*(*) string
      character*128 lnholdq
      logical lastchar
      common /commac/ macnum,mrecord,mplay,macuse,icchold,lnholdq,endmac
      logical mrecord,mplay,endmac
      common /c1ommac/ ip1mac(20),il1mac(20),ip2mac(20),il2mac(20),
     *                 ic1mac(20),ilmac,iplmac
      if (.not.mplay) then
        if (ilbuf .gt. nlbuf) go to 999
        call getbuf(string)
        return
999     lastchar = .true.
        return
      else
c
c  Play a macro.  Set pointer to first character needed in buffer
c
        if (ilmac .eq. il1mac(macnum)) then
c
c  Getting first line of macro
c
          ip1 = ip1mac(macnum)
          iplmac = ip1-ic1mac(macnum) 
        else if (ilmac .le. il2mac(macnum)) then
c
c  Beyond first line of macro.  Advance line-start pointer.
c
          iplmac = iplmac+lbuf(ilmac-1)
          ip1 = iplmac+1
        else
c
c  Beyond last line of macro.  Terminate it!
c
          mplay = .false.
          endmac = .true.
          return
        end if
        if (ilmac .eq. il2mac(macnum)) then
c
c  Getting last line of macro.
c
          ip2 = ip2mac(macnum)
        else 
c
c  Getting line before last line of macro.
c
          ip2 = iplmac+lbuf(ilmac)
        end if
        if (ip2 .ge. ip1) then
          string = bufq(ip1:ip2)
        else
c
c  Kluge for when macro start is on a line by itself
c
          string = ' '
        end if
        ilmac = ilmac+1
        return
      end if
      end
      function readin(lineq,iccount,nline)
c
c  Reads a piece of setup data from file lineq, gets a new lineq from
c  file 10 (jobname.pmx) and increments nline if needed,  passes over
c  comment lines
c
      character*128 lineq
      character*1 durq,chax
4     if (iccount .eq. 128) then
1       call getbuf(lineq)
        nline = nline+1
        if (lineq(1:1) .eq. '%') go to 1
        iccount = 0
      end if
      iccount = iccount+1
c
c  Find next non-blank or end of line
c
      do 2 iccount = iccount , 127
        if (lineq(iccount:iccount) .ne. ' ') go to 3
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
5     call getchar(lineq,iccount,durq)
c
c  Remember that getchar increments iccount, then reads a character.
c
      if (index('0123456789.-',durq) .gt. 0) go to 5
      i2 = iccount-1
      if (i2 .lt. i1) then
        print*,'Found "'//durq//'" instead of number'
        call stop1()
      end if
      icf = i2-i1+49
      read(lineq(i1:i2),'(f'//chax(icf)//'.0)')readin
      return
      end
      subroutine readmeter(lineq,iccount,mtrnum,mtrden)
      character*128 lineq
      character*1 durq,chax
      if (index(lineq(iccount+1:iccount+3),'/') .eq. 0) then
c
c  No slashes, so use old method
c
        call getchar(lineq,iccount,durq)
        if (durq .eq. '-') then
c
c  Negative numerator is used only to printed; signals vertical slash
c
          call getchar(lineq,iccount,durq)
          mtrnum = -(ichar(durq)-48)
        else if (durq .eq. 'o') then
c
c  Numerator is EXACTLY 1
c
          mtrnum = 1
        else
          mtrnum = ichar(durq)-48
          if (mtrnum .eq. 1) then
c
c  Numerator is >9
c
            call getchar(lineq,iccount,durq)
            mtrnum = 10+ichar(durq)-48
          end if
        end if
        call getchar(lineq,iccount,durq)
        if (durq .eq. 'o') then
          mtrden = 1
        else
          mtrden = ichar(durq)-48
          if (mtrden .eq. 1) then
            call getchar(lineq,iccount,durq)
            mtrden = 10+ichar(durq)-48
          end if
        end if
      else
c
c  Expect the form m[n1]/[n2]/[n3]/[n4] . Advance iccount by one from '/' or 'm'
c
        iccount = iccount+1
        ns = index(lineq(iccount:128),'/')
        read(lineq(iccount:iccount+ns-2),'(i'//chax(47+ns)//')')mtrnum
c
c  Reset iccount to start of second integer
c
        iccount = iccount+ns
c
c  There must be either a slash or a blank at pos'n 2 or 3
c
        ns = index(lineq(iccount:iccount+2),'/')
        if (ns .eq. 0) ns = index(lineq(iccount:iccount+2),' ')
        read(lineq(iccount:iccount+ns-2),'(i'//chax(47+ns)//')')mtrden
c
c  Set iccount to last character used
c
        iccount = iccount+ns-1
      end if
      return
      end
      subroutine readnum(lineq,iccount,durq,fnum)
c
c  This reads a number starting at position iccount.  Remember that on exit,
c  getchar leaves iccount at the last character retrieved.  So must only
c  call this routine *after* detecting a number or decimal.
c  On exit, durq is next character after end of number.
c
      character*128 lineq
      character*1 durq,chax
      i1 = iccount
1     call getchar(lineq,iccount,durq)
      if (index('0123456789.',durq) .gt. 0) go to 1
      i2 = iccount-1
      if (i2 .lt. i1) then
        print*,'Found "'//durq//'" instead of number'
        call stop1()
      else if (lineq(i1:i1).eq.'.'.and.lineq(i2:i2).eq.'.') then
        i2 = i2-1
        iccount = iccount-1
      end if
      icf = i2-i1+49
      read(lineq(i1:i2),'(f'//chax(icf)//'.0)')fnum
      return
      end
      subroutine setbits(isdata,iwidbit,ishift,ivalue)
c
c  Sets iwidbits of isdata, shifted by ishift, to ivalue
c
      ibase = 2**iwidbit-1
      isdata = iand(not(ishft(ibase,ishift)),isdata)
      isdata = ior(isdata,ishft(ivalue,ishift))
      return
      end
      subroutine setbm2(xelsk,nnb,sumx,sumy,ipb,islope,nolev1)
c
c The MEAN SQUARE slope algorithm
c
      parameter (nm=24)
      common /all/ mult(nm,200),iv,nnl(nm),nv,ibar,
     *   ivxo(600),ipo(600),to(600),tno(600),tnote(600),eskz(nm,200),
     *   ipl(nm,200),ibm1(nm,9),ibm2(nm,9),nolev(nm,200),ibmcnt(nm),
     *   nodur(nm,200),jn,lenbar,iccount,nbars,itsofar(nm),nacc(nm,200),
     *   nib(nm,15),nn(nm),lenb0,lenb1,slfac,musicsize,stemmax,
     *   stemmin,stemlen,mtrnuml,mtrdenl,mtrnmp,mtrdnp,islur(nm,200),
     *   ifigdr(2,125),iline,figbass,figchk(2),firstgulp,irest(nm,200),
     *   iornq(nm,0:200),isdat1(202),isdat2(202),nsdat,isdat3(202),
     *   isdat4(202),beamon(nm),isfig(2,200),sepsymq(nm),sq,ulq(nm,9)
      common /commvl/ nvmx(nm),ivmx(nm,2),ivx
      character*1 ulq,sepsymq,sq
      logical beamon,firstgulp,figbass,figchk,
     *        isfig
      real*4 xelsk(24)
      integer ipb(24)
        ibc = ibmcnt(ivx)
        sumxx = 0.
        sumxy = 0.
        do 2 inb = 1 , nnb
          sumxx = sumxx+xelsk(inb)**2
          sumxy = sumxy+xelsk(inb)*nolev(ivx,ipb(inb))
2       continue
        delta = nnb*sumxx-sumx*sumx
        em = (nnb*sumxy-sumx*sumy)/delta
        islope = nint(0.5*em*slfac)
        if (iabs(islope) .gt. 9) islope = isign(9,islope)
        beta = (sumy-islope/slfac*sumx)/nnb
        nolev1 = nint(beta)
c
c   Check if any stems are too short
c
        smin = 100.
        iul = -1
        if (ulq(ivx,ibc) .eq. 'u') iul = 1
        do 4 inb = 1 , nnb
          ybeam = nolev1+iul*stemlen+islope*xelsk(inb)/slfac
          ynote = nolev(ivx,ipb(inb))
          smin = min(smin,iul*(ybeam-ynote))
4       continue
        if (smin .lt. stemmin) then
          deficit = stemmin-smin
          nolev1 = nint(nolev1+iul*deficit)
        end if
      return
      end
      subroutine setmac(lineq,iccount,ibarcnt,ibaroff,nbars,charq,durq,
     *                  ivx,nline)
      character*1 charq,durq
      common /commac/ macnum,mrecord,mplay,macuse,icchold,lnholdq,endmac
      logical mrecord,mplay,endmac,btest
      character*128 lnholdq,lineq
      common /c1ommac/ ip1mac(20),il1mac(20),ip2mac(20),il2mac(20),
     *                 ic1mac(20),ilmac,iplmac
c
c  Macro action
c
      call g1etchar(lineq,iccount,charq)
      if (charq.eq.'S' .and. ivx.ne.1) then
        print*
        print*
        print*,'*********WARNING*********'
        call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *    '"MS..." only put in parts by scor2prt if in voice #1!')
      end if
      if (index('RSP ',charq) .eq. 0) then
        call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           'Illegal character after "M" (macro)!')
        call stop1()
      else if (charq .ne. ' ') then
c
c  Record or playback a macro.  Get the number of the macro.
c
        call g1etchar(lineq,iccount,durq)
        if (index('123456789',durq) .eq. 0) then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           'Must input number after "MR","MP", or "MS"!')
          call stop1()
        end if
        call readnum(lineq,iccount,durq,fnum)
        macnum = nint(fnum)
        if (durq .ne. ' ') then
          call errmsg(lineq,iccount,ibarcnt-ibaroff+nbars+1,
     *           'Macro number must be followed by a blank!')
          call stop1()
        end if
        if (index('RS',charq ).gt. 0) then
c
c  Record or save a macro
c
          if (macnum.lt.1 .or. macnum.gt.20) then
            call errmsg(lineq,iccount-1,ibarcnt-ibaroff+nbars+1,
     *           'Macro number not in range 1-20!')
            call stop1()
          end if
          macuse = ibset(macuse,macnum)
          if (charq .eq. 'R') then
            call m1rec1(lineq,iccount,ibarcnt,ibaroff,nbars,ndxm)
          else if (charq .eq. 'S') then
c
c  Save (Record but don't activate)
c
1           call m1rec1(lineq,iccount,ibarcnt,ibaroff,nbars,ndxm)
            if (mrecord) then
              call getbuf(lineq)
              nline = nline+1
              iccount = 0
              go to 1
            end if
            iccount = iccount+ndxm+1
          end if
        else
c
c  Playback the macro
c
          if (.not.btest(macuse,macnum)) then
            call errmsg(lineq,iccount-1,ibarcnt-ibaroff+nbars+1,
     *           'Cannot play a macro that has not been recorded!')
            call stop1()
          end if
          icchold = iccount
          lnholdq = lineq
          iccount = 128
          mplay = .true.
          ilmac = il1mac(macnum)
        end if
      end if
      return
      end
      subroutine setmeter(mtrnuml,mtrdenl,ibmtyp,ibmrep)
c
c  Sets last 2 args depending on 1st 2, (logical) num, denom.
c  ibmtyp = 1, 2, or 3 defines set of masks for beam groupings.
c  1: all duple meters
c  2: triple w/ denom=4, subdivide in groups of 2 8ths
c  3: triple w/ denom=8, subdivide in groups of 3 8ths
c  Note that lenbar is set at top or when 'm' symbol is read in getnote
c
      if (mtrdenl .eq. 4) then
        if (mod(mtrnuml,3) .eq. 0) then
          ibmtyp = 2
          ibmrep = mtrnuml/3
        else
          ibmtyp = 1
          ibmrep = mtrnuml/2
        end if
      else if (mtrdenl .eq. 2) then
        ibmtyp = 1
        if (mtrnuml .eq. 3) then
          ibmrep = 3
        else
          ibmrep = 2*mtrnuml/mtrdenl
        end if
      else if (mtrdenl.eq.8 .and. (mtrnuml.eq.8.or.mtrnuml.eq.4)) then
c
c  Added 170726
c
        ibmtyp = 1
        ibmrep = mtrnuml/4
      else
c
c  Assumes mtrdenl=8 and 3/8, 6/8, 9/8, or 12/8
c
        ibmtyp = 3
        ibmrep = mtrnuml/3
      end if
c
c  Reset so we don't keep writing new meters
c
      mtrnuml = 0
c
c  Prevent ibmrep=0.  Needed for odd bars, e.g. 1/8, where beams don't matter
c
      ibmrep = max(ibmrep,1)
      return
      end
      subroutine SetupB(xelsk,nnb,sumx,sumy,ipb,smed,ixrest)
c
c The outer combo algorithm
c
      parameter (nm=24)
      common /all/ mult(nm,200),iv,nnl(nm),nv,ibar,
     *   ivxo(600),ipo(600),to(600),tno(600),tnote(600),eskz(nm,200),
     *   ipl(nm,200),ibm1(nm,9),ibm2(nm,9),nolev(nm,200),ibmcnt(nm),
     *   nodur(nm,200),jn,lenbar,iccount,nbars,itsofar(nm),nacc(nm,200),
     *   nib(nm,15),nn(nm),lenb0,lenb1,slfac,musicsize,stemmax,
     *   stemmin,stemlen,mtrnuml,mtrdenl,mtrnmp,mtrdnp,islur(nm,200),
     *   ifigdr(2,125),iline,figbass,figchk(2),firstgulp,irest(nm,200),
     *   iornq(nm,0:200),isdat1(202),isdat2(202),nsdat,isdat3(202),
     *   isdat4(202),beamon(nm),isfig(2,200),sepsymq(nm),sq,ulq(nm,9)
      common /comipl2/ ipl2(nm,200)
      character*1 ulq,sepsymq,sq
      logical beamon,firstgulp,figbass,figchk,btest,
     *        isfig,vxtup,l1ng,l2ng,bar1syst,drawbm
      common /comeskz2/ eskz2(nm,200)
      real*4 slope(800),xelsk(24)
      integer ipb(24)
      common /comask/ bar1syst,fixednew,scaldold,
     *                wheadpt,fbar,poenom
      common /comas1/ naskb,task(40),wask(40),elask(40)
      common /comxtup/ ixtup,vxtup(nm),ntupv(nm,9),nolev1(nm),
     *                 mtupv(nm,9),nxtinbm(nm),
     *                 islope(nm),xels11(24),eloff(nm,9),
     *                 nssb(nm),issb(nm),lev1ssb(nm,20)
      common /comdraw/ drawbm(nm)
      common /combmh/ bmhgt,clefend
      common /commvl/ nvmx(nm),ivmx(nm,2),ivx
      common /comtol/ tol
      ibc = ibmcnt(ivx)
      nxtinbm(ivx) = 0
      n1 = ipl2(ivx,ibm1(ivx,ibc))
c
c  Initialize counters used in this subroutine, and then later during actual
c    beam drawing, to count later segments of single-slope beam groups
c
      nssb(ivx) = 0
      issb(ivx) = 0
c
c  Set flag for xtup beam starting with rest (no others can start with rest)
c
      if (btest(irest(ivx,ipo(n1)),0)) ixrest = 1
c
c Figure how many elemskips to each note. Use the list, counting only non-rests.
c
      eskz0 = eskz(ivx,ibm1(ivx,ibc))
      nnb = 0
      sumx = 0.
      sumy = 0.
      ipxt1 = 0
      iplast = ibm2(ivx,ibc)
      do 2 ip = ibm1(ivx,ibc) , iplast
        if (.not.btest(irest(ivx,ip),0)) then
          nnb = nnb+1
          ipb(nnb) = ip
          xelsk(nnb) = eskz(ivx,ip)-eskz0
          sumx = sumx+xelsk(nnb)
          sumy = sumy+nolev(ivx,ipb(nnb))
          if (btest(nacc(ivx,ip),21)) then
c
c  This is the starting note of later segment of single-slope beam group
c  Temporarily store ip here.
c
            nssb(ivx) = nssb(ivx)+1
            lev1ssb(ivx,nssb(ivx)) = nnb
          end if
        end if
c
c  New xtup stuff here.  Final object is to get distance from start of xtup
c    to number. xtinbm counts xtups in this beam only.  mtupv is the printed
c    number.  ntupv is number of notes in xtup, and is only used to get
c    eloff, the distance from start of xtup to the number.
c
        if (btest(nacc(ivx,ip),18)) ndoub = ndoub+1
        if (ipxt1.eq.0 .and. nodur(ivx,ip).eq.0) then
c
c  Xtup is starting here
c
          nxtinbm(ivx) = nxtinbm(ivx)+1
          ipxt1  = ip
          if (btest(nacc(ivx,ip),18)) then
            ndoub = 1
          else
            ndoub = 0
          end if
        else if (ipxt1.gt.0 .and. nodur(ivx,ip).gt.0) then
c
c  Xtup ends here.  Set total number of notes in xtup.
c
          ntupv(ivx,nxtinbm(ivx)) = ip+1-ipxt1
c
c  Set printed number for embedded xtup.
c
          mtupv(ivx,nxtinbm(ivx)) = ntupv(ivx,nxtinbm(ivx))+ndoub
c
c  Middle note of xtup if ntupv odd, note to left of gap if even.  
c
          ipxtmid = (ip+ipxt1)/2
          eloff(ivx,nxtinbm(ivx)) = eskz2(ivx,ipxtmid)-eskz2(ivx,ipxt1)
          if (mod(ntupv(ivx,nxtinbm(ivx)),2) .eq. 0) 
     *        eloff(ivx,nxtinbm(ivx)) = eloff(ivx,nxtinbm(ivx))+
     *        .5*(eskz2(ivx,ipxtmid+1)-eskz2(ivx,ipxtmid))
          ipxt1 = 0
        end if
2     continue
c
c  Reset nxtinbm for use as counter as #'s are posted by putxtn(..)
c
      nxtinbm(ivx) = 0
      smed = 0.
c      if (.not.btest(islur(ivx,ipb(1)),2)) then
      if (.not.btest(islur(ivx,ipb(1)),2) .and. nnb.gt.1) then
c
c No forced 0 slope
c
c        if (nnb .eq. 1) go to 6
        nsc = 0
        do 5 inb = 1 , nnb-1
        do 5 jnb = inb+1 , nnb
          nsc = nsc+1
          slope(nsc) = (nolev(ivx,ipb(jnb))-nolev(ivx,ipb(inb)))/
     *                   (xelsk(jnb)-xelsk(inb))
          if (abs(slope(nsc)) .lt. 1.e-4) then
            nsc = nsc+1
            slope(nsc) = slope(nsc-1)
            nsc = nsc+1
            slope(nsc) = slope(nsc-1)
          end if
5       continue
        if (nsc .eq. 1) then
          smed = slope(1)
          go to 6
        end if
        nscmid = nsc/2+1
        do 7 i = 1 , nscmid
          do 7 j = i+1 , nsc
            if (slope(j) .lt. slope(i)) then
              t = slope(j)
              slope(j) = slope(i)
              slope(i) = t
            end if
7       continue
        smed = slope(nscmid)
        if (nsc .eq. 2*(nsc/2)) then 
c
c  Even number of slopes in the list, so median is ambiguous
c
          if (abs(slope(nscmid-1)) .lt. abs(slope(nscmid))-tol) then
c
c  Lower-numbered one is truly less in absolute value, so use it
c
            smed=slope(nscmid-1)
          else if (abs(slope(nscmid-1)+slope(nscmid)) .lt. tol) then
c
c  Two slopes are effectively equal.  Take the one with sign of the average
c
            sum = 0.
            do 1 i = 1 , nsc
              sum = sum+slope(i)
1           continue
            smed = sign(smed,sum)
          end if
        end if
6       continue
        islope(ivx) = nint(0.5*smed*slfac)
        if (iabs(islope(ivx)) .gt. 9) 
     *         islope(ivx) = isign(9,islope(ivx))
      else
c
c  Forced horizontal beam
c
        islope(ivx) = 0
      end if
      beta = (sumy-islope(ivx)/slfac*sumx)/nnb
c
c  If ixrest>0, this is a virtual nolev1 at location of rest.  Will first use
c  as is for placing xtup number and/or bracket, then reset it for start of
c  actual beam
c
      nolev1(ivx) = nint(beta)
c
c  Check if any stems are too short
c
      smin = 100.
      iul = -1
      if (ulq(ivx,ibc) .eq. 'u') iul = 1
      ssq = 0.
      syb = 0.
      yb1 = nolev1(ivx)
     *       +iul*(stemlen+bmhgt*(iand(15,mult(ivx,ipb(1)))-8-1))
      do 4 inb = 1 , nnb
        ybeam = yb1+islope(ivx)*xelsk(inb)/slfac
     *              -iul*bmhgt*(iand(15,mult(ivx,ipb(inb)))-8-1)
        syb = syb+ybeam
        ynote = nolev(ivx,ipb(inb))
        off = ybeam-ynote
        if (inb .eq. 1) then
          off1 = off
        else if (inb .eq. nnb) then
          off2 = off
        end if
        ssq = ssq+off*off
        smin = min(smin,iul*off)
4     continue
      dnolev = 0.
      if (smin .lt. stemmin) then
        deficit = stemmin-smin
        nolevo = nolev1(ivx)
        nolev1(ivx) = nint(nolev1(ivx)+iul*deficit)
        dnolev = nolev1(ivx)-nolevo
        off1 = off1+dnolev
        off2 = off2+dnolev
      end if
      ssq = ssq+2*dnolev*(syb-sumy)+dnolev**2
      if (.not.vxtup(ivx) .and. sqrt(ssq/nnb) .gt. stemmax .and.
     *    (abs(off1).lt.stemmax .or. abs(off2).lt.stemmax)
c     *    .and. .not.btest(islur(ivx,ipb(1)),2)) then
     *    .and. .not.btest(islur(ivx,ipb(1)),2)
     *    .and. nnb.ne.1) then
c
c  The first check is to save trouble of putting xtup's in setbm2.
c  The penultimate check is that first and last stems aren't both excessive.
c  The last check is that a 0 slope has not been forced
c
        call setbm2(xelsk,nnb,sumx,sumy,ipb,islope(ivx),nolev1(ivx))
      end if
c
c  Check if beam starts or ends too high or low.
c
      xboff = bmhgt*(iand(15,mult(ivx,ipb(1)))-8-1)
      l1ng = iul*(nolev1(ivx)-ncmid(iv,ipb(1)))+xboff+7 .lt. 0.
      xnolev2 = nolev1(ivx)+islope(ivx)/slfac*xelsk(nnb)
      l2ng = iul*(xnolev2-ncmid(iv,ipb(nnb)))+xboff+7 .lt. 0
      if (l1ng .or. l2ng) then
c
c  Need to correct start or stop, also slope
c
        if (l1ng) then
          nolev1(ivx) = nint(ncmid(iv,ipb(1))-(7.+xboff)*iul)
        end if
        if (l2ng) then
          xnolev2 = nint(ncmid(iv,ipb(nnb))-(7.+xboff)*iul)
        end if
c
c  Since one or the other end has changed, need to change slope
c
c        if (.not.btest(islur(ivx,ipb(1)),2))
        if (.not.btest(islur(ivx,ipb(1)),2) .and. nnb.ne.1)
     *       islope(ivx) = nint(slfac*(xnolev2-nolev1(ivx))/xelsk(nnb))
      end if
      if (nssb(ivx) .gt. 0) then
c
c  This is a single-slope beam group.  Store start heights for later segs.
c
        do 3 issbs = 1 , nssb(ivx)
          lev1ssb(ivx,issbs) = nolev1(ivx)+islope(ivx)/slfac*
     *                                  xelsk(lev1ssb(ivx,issbs))
3       continue
      end if
      return
      end
      subroutine sortpoe(nsyst,poe,ipoe)
      parameter (nks=125)
      real*4 poe(nks)
      integer*4 ipoe(nks)
c
c  Initialize ipoe:
c
      do 3 iord = 1 , nsyst
        ipoe(iord) = iord
3     continue
c
c  Construct ipoe vector with pairwise interchanges.  When done, ipoe(1) will
c  be index of smallest poe, and ipoe(nsyst) will be index of biggest poe.
c
      do 4 io1 = 1 , nsyst-1
        do 5 io2 = io1+1 , nsyst
          if (poe(ipoe(io1)) .gt. poe(ipoe(io2))) then
c
c Interchange the indices
c
            itemp = ipoe(io1)
            ipoe(io1) = ipoe(io2)
            ipoe(io2) = itemp
          end if
5       continue
4     continue
      return
      end
      subroutine spsslur(lineq,iccount,iv,kv,ip,isdat1,isdat2,isdat3,
     *                   isdat4,nsdat,notcrd,nolev,starter)
      parameter (nm=24)
c
c  Reads in slur data.  Record all h/v-shifts for non-chords, user-specified
c  ones for chords.  
c  5/26/02  This subr is called ONLY for postscript slurs.
c
c  See subroutine doslur for bit values in isdat1,2,3
c
      common /comtrill/ ntrill,ivtrill(24),iptrill(24),xnsktr(24),
     *                ncrd,icrdat(193),icrdot(193),icrdorn(193),nudorn,
     *                kudorn(63),ornhshft(63),minlev,maxlev,icrd1,icrd2
      integer*4 isdat1(202),isdat2(202),isdat3(202),isdat4(202)
      logical notcrd,btest
      character*128 lineq
      character*1 durq,dumq,starter
      common /comslur/ listslur,upslur(nm,2),ndxslur,fontslur
     *                 ,WrotePsslurDefaults,SlurCurve
      logical upslur,fontslur,WrotePsslurDefaults
c
c  Counter for signed integers.  1st is height, 2nd is horiz, 3rd is curve
c
      numint = 0
      ivoff = 0
      ihoff = 0
      nsdat = nsdat+1
      if (starter.eq.'{' .or. starter.eq.'}') 
     *           isdat2(nsdat)=ibset(isdat2(nsdat),3)
      call setbits(isdat1(nsdat),5,13,iv)
      call setbits(isdat1(nsdat),1,12,kv-1)
      call setbits(isdat1(nsdat),8,3,ip)
      isdat3(nsdat) = 0
	isdat4(nsdat) = 0
	ilb12 = 0 ! flag for tweaks of 1st or 2nd (0|1) seg of linebreak slur 
c
c  Get ID code
c
      call getchar(lineq,iccount,durq)
c      if (index('uldtb+-hfnHps ',durq) .gt. 0) then
      if (index('uldtb+-hfnHpsv ',durq) .gt. 0) then
c
c  Null id. Note for ps slurs, 'H' cannot be an ID
c
        iccount = iccount-1
        if (lineq(iccount:iccount) .eq. 't') then
          idcode = 1
        else
          idcode = 32
        end if
      else
c
c  Set explicit idcode
c
        idcode = ichar(durq)
        if (lineq(iccount-1:iccount-1) .eq. 't') then
c
c  Make t[ID] look like s[ID]t
c
          isdat2(nsdat) = ibset(isdat2(nsdat),3)  
        end if
        if (lineq(iccount+1:iccount+1).eq.'x') then
c
c  Flag for 2-voice, indexed slurs
c
          iccount = iccount+1
          call setbits(isdat1(nsdat),1,1,1)
        end if
      end if
      call setbits(isdat1(nsdat),7,19,idcode)
c
c  Set start/stop: look thru list from end for same idcode,iv,kv
c
      do 2 isdat = nsdat-1 , 1 , -1
        if (idcode .eq. igetbits(isdat1(isdat),7,19) .and.
     *      iv .eq. igetbits(isdat1(isdat),5,13) .and.
c     *      kv-1 .eq. igetbits(isdat1(isdat),1,12)) then
     *      (kv-1 .eq. igetbits(isdat1(isdat),1,12)
     *       .or. btest(isdat1(isdat),1)))             then
c
c  Matched idcode & ivx.  On/off?.  If on, new is turnoff, leave bit 11 at 0.
c
          if (btest(isdat1(isdat),11)) go to 3
c
c  Found slur is a turnoff, so new one is a turnon.  Jump down to set bit
c
          go to 4
        end if
2     continue
c
c  If here, this is turnon.
c
4     continue
      isdat1(nsdat) = ibset(isdat1(nsdat),11)
3     continue
c
c  Now done with initial turnon- or turnoff-specifics.  
c
      if (nint(SlurCurve).ne.0 .and. btest(isdat1(nsdat),11)) then
c
c  There's a default curvature tweak
c
        icurv1 = nint(SlurCurve)+3
        if (icurv1 .eq. 2) icurv1 = 1
        isdat3(nsdat) = ibset(isdat3(nsdat),0)
        call setbits(isdat3(nsdat),6,2,32+icurv1)
      end if
c
c  Loop for rest of input
c
1     call getchar(lineq,iccount,durq)
      if (index('uld',durq) .gt. 0) then
c
c  Force direction
c
        isdat1(nsdat) = ibset(isdat1(nsdat),26)
        if (durq .eq. 'u') isdat1(nsdat) = ibset(isdat1(nsdat),27)
        go to 1
      else if (index('+-',durq) .gt. 0) then
        numint = numint+1
        if (numint .eq. 1) then
c
c  Vertical offset
c
          iccount = iccount+1
          call readnum(lineq,iccount,dumq,fnum)
          iccount = iccount-1
          ivoff = nint(fnum)
          if (durq .eq. '-') ivoff = -ivoff
        else if (numint .eq. 2) then
c
c  Horizontal offset
c
          iccount = iccount+1
          call readnum(lineq,iccount,dumq,fnum)
          iccount = iccount-1
c
c  fnum is abs(hshift), must be 0 to 6.3
c
          ihoff = fnum*10 + .5
          if (durq .eq. '-') ihoff = -ihoff
c
c  Later will set bits to 1...127 to represent -6.3,...+6.3
c
        else
c
c  Must be the 3rd signed integer, so it's a curve specification
c
          isdat3(nsdat) = ibset(isdat3(nsdat),0)
          iccount = iccount+1
          call readnum(lineq,iccount,dumq,fnum)
          icurv1 = nint(fnum)
          if (durq .eq. '-') icurv1 = -icurv1
          call setbits(isdat3(nsdat),6,2,32+icurv1)
          if (dumq .ne. ':') then 
c
c  Back up the pointer and loop for more input
c
            iccount = iccount-1
          else
c
c  Expect two single digits as parameters for curve
c
            isdat3(nsdat) = ibset(isdat3(nsdat),1)
            call setbits(isdat3(nsdat),3,8,
     *            ichar(lineq(iccount+1:iccount+1))-48)
            call setbits(isdat3(nsdat),3,11,
     *            ichar(lineq(iccount+2:iccount+2))-48)
            iccount = iccount+2
          end if             
        end if
        go to 1
      else if (durq .eq. 't') then
        isdat2(nsdat) = ibset(isdat2(nsdat),3)
        go to 1
      else if (durq .eq. 'b') then
        isdat2(nsdat) = ibset(isdat2(nsdat),4)
        go to 1
      else if (durq .eq. 's') then
c
c  Endpoint tweaks for linebreak slurs. 
c
        call getchar(lineq,iccount,durq) ! Must be +|-
c
c  Next is vertical offset
c
        iccount = iccount+1
        call readnum(lineq,iccount,dumq,fnum)
        if (durq .eq. '-') fnum=-fnum
        call setbits(isdat4(nsdat),6,ilb12*16,nint(fnum)+32)        
        if (index('+-',dumq) .gt. 0) then
c
c  Also a horizontal offset
c
          iccount = iccount+1
          call readnum(lineq,iccount,durq,fnum)
          if (dumq .eq. '-') fnum=-fnum
          call setbits(isdat4(nsdat),7,ilb12*16+6,nint(10*fnum)+64)
        end if
        iccount = iccount-1
        ilb12 = 1
        go to 1
      else if (index('fnhH',durq).gt.0) then
c
c  Special ps slur curvatures.  
c  Translate to old \midslur args (1,4,5,6)
c
        icurv1 = index('fnxhH',durq)
        if (icurv1 .eq. 5) then
c
c  check for 2nd H
c
          if (lineq(iccount+1:iccount+1) .eq. 'H') then
            iccount = iccount+1
            icurv1 = 6
          end if   
        end if
        isdat3(nsdat) = ibset(isdat3(nsdat),0)
        call setbits(isdat3(nsdat),6,2,32+icurv1)
        go to 1
      else if (durq .eq. 'p') then        ! Local adjustment  
        call getchar(lineq,iccount,durq)  !  +|-
        call getchar(lineq,iccount,dumq)  !  s|t
c  26       \sluradjust    (p+s)
c  27       \nosluradjust  (p-s)
c  28       \tieadjust     (p+t)
c  29       \notieadjust   (p-t)
        if (durq .eq. '+') then
          if (dumq .eq. 's') then
	      isdat2(nsdat) = ibset(isdat2(nsdat),26)
          else
	      isdat2(nsdat) = ibset(isdat2(nsdat),28)
          end if
        else
          if (dumq .eq. 's') then
	      isdat2(nsdat) = ibset(isdat2(nsdat),27)
          else
	      isdat2(nsdat) = ibset(isdat2(nsdat),29)
          end if
        end if
        go to 1
      else if (durq .eq. 'v') then        ! Stem slur
        isdat1(nsdat) = ibset(isdat1(nsdat),2)  
        go to 1
      end if
c
c  Record shifts
c
      call setbits(isdat2(nsdat),6, 6,ivoff+32)
      call setbits(isdat2(nsdat),7,12,ihoff+64)
c
c  Record chord flag, note level, notehead shift
c
      if (notcrd) then
        call setbits(isdat2(nsdat),7,19,nolev)
      else
        nolevc = igetbits(icrdat(ncrd),7,12)
        call setbits(isdat2(nsdat),7,19,nolevc)
        isdat2(nsdat) = ibset(isdat2(nsdat),0)
        call setbits(isdat2(nsdat),2,1,igetbits(icrdat(ncrd),2,23))
      end if
      return
      end
      subroutine sslur(lineq,iccount,iv,kv,ip,isdat1,isdat2,isdat3,
     *                 nsdat,notcrd,nolev,starter)
      parameter (nm=24)
c
c  Reads in slur data.  Record all h/v-shifts for non-chords, user-specified
c  ones for chords.
c  5/26/02 now only for non-postscript slurs, use spsslur() for postscript
c
c  See subroutine doslur for bit values in isdat1,2,3
c
      common /comtrill/ ntrill,ivtrill(24),iptrill(24),xnsktr(24),
     *                ncrd,icrdat(193),icrdot(193),icrdorn(193),nudorn,
     *                kudorn(63),ornhshft(63),minlev,maxlev,icrd1,icrd2
      integer*4 isdat1(202),isdat2(202),isdat3(202)
      logical notcrd,btest
      character*128 lineq
      character*1 durq,dumq,starter
      common /comslur/ listslur,upslur(nm,2),ndxslur,fontslur
     *                 ,WrotePsslurDefaults,SlurCurve
      logical upslur,fontslur,WrotePsslurDefaults
c
c  Counter for signed integers.  1st is height, 2nd is horiz, 3rd is curve
c
      numint = 0
      ivoff = 0
      ihoff = 0
      nsdat = nsdat+1
      if (starter.eq.'{' .or. starter.eq.'}') 
     *           isdat2(nsdat)=ibset(isdat2(nsdat),3)
      call setbits(isdat1(nsdat),5,13,iv)
      call setbits(isdat1(nsdat),1,12,kv-1)
      call setbits(isdat1(nsdat),8,3,ip)
      isdat3(nsdat) = 0
c
c  Get id letter
c
      if (lineq(iccount:iccount) .eq. 't') then
c
c  Old-style t-slur. Use special idcode = 1
c
        idcode = 1
      else
        call getchar(lineq,iccount,durq)
        if (index('uldtb+-hf ',durq) .gt. 0) then
c
c  Null id
c
          idcode = 32
          iccount = iccount-1
        else if (durq.eq.'H') then
c
c  Postscript slur, cannot use 'H' as code, must check for 2nd 'H'
c
          idcode = 32
          iccount = iccount-1
c
c  There may be another "H", but no need to deal with it yet
c
        else
c
c  Set explicit idcode
c
          idcode = ichar(durq)
        end if
      end if
      call setbits(isdat1(nsdat),7,19,idcode)
c
c  Set start/stop: look thru list from end for same idcode,iv,kv
c
      do 2 isdat = nsdat-1 , 1 , -1
        if (idcode .eq. igetbits(isdat1(isdat),7,19) .and.
     *      iv .eq. igetbits(isdat1(isdat),5,13) .and.
     *      kv-1 .eq. igetbits(isdat1(isdat),1,12)) then
c
c  Matched idcode & ivx.  On/off?.  If on, new is turnoff, leave bit 11 at 0.
c
          if (btest(isdat1(isdat),11)) go to 3
c
c  Found slur is a turnoff, so new one is a turnon.  Jump down to set bit
c
          go to 4
        end if
2     continue
c
c  If here, this is turnon.
c
4     continue
      isdat1(nsdat) = ibset(isdat1(nsdat),11)
3     continue
c
c  Now done with initial turnon- or turnoff-specifics.  Loop for rest of input
c
1     call getchar(lineq,iccount,durq)
      if (index('uld',durq) .gt. 0) then
c
c  Force direction
c
        isdat1(nsdat) = ibset(isdat1(nsdat),26)
        if (durq .eq. 'u') isdat1(nsdat) = ibset(isdat1(nsdat),27)
        go to 1
      else if (index('+-',durq) .gt. 0) then
        numint = numint+1
        if (numint .eq. 1) then
c
c  Vertical offset
c
          iccount = iccount+1
          call readnum(lineq,iccount,dumq,fnum)
          iccount = iccount-1
          ivoff = nint(fnum)
          if (durq .eq. '-') ivoff = -ivoff
        else if (numint .eq. 2) then
c
c  Horizontal offset
c
          iccount = iccount+1
          call readnum(lineq,iccount,dumq,fnum)
          iccount = iccount-1
c
c  fnum is abs(hshift), must be 0 to 6.3
c
          ihoff = fnum*10 + .5
          if (durq .eq. '-') ihoff = -ihoff
c
c  Later will set bits to 1...127 to represent -6.3,...+6.3
c
        else
c
c  Must be the 3rd signed integer, so it's a curve specification
c
          isdat3(nsdat) = ibset(isdat3(nsdat),0)
          iccount = iccount+1
          call readnum(lineq,iccount,dumq,fnum)
          icurv1 = nint(fnum)
          if (durq .eq. '-') icurv1 = -icurv1
          call setbits(isdat3(nsdat),6,2,32+icurv1)
          if (dumq .ne. ':') then 
c
c  Back up the pointer and loop for more input
c
            iccount = iccount-1
          else
c
c  Expect two single digits as parameters for curve
c
            isdat3(nsdat) = ibset(isdat3(nsdat),1)
            call setbits(isdat3(nsdat),3,8,
     *            ichar(lineq(iccount+1:iccount+1))-48)
            call setbits(isdat3(nsdat),3,11,
     *            ichar(lineq(iccount+2:iccount+2))-48)
            iccount = iccount+2
          end if             
        end if
        go to 1
      else if (durq .eq. 't') then
        isdat2(nsdat) = ibset(isdat2(nsdat),3)
        go to 1
      else if (durq .eq. 'b') then
        isdat2(nsdat) = ibset(isdat2(nsdat),4)
        go to 1
      else if (index('fhH',durq) .gt. 0) then
c
c  Special ps slur curvatures.  Translate to old \midslur args (1,4,5,6)
c
        icurv1 = 2+index('fhH',durq)
        if (icurv1 .eq. 3) then
          icurv1 = 1
        else if (icurv1 .eq. 5) then
c
c  check for 2nd H
c
          if (lineq(iccount+1:iccount+1) .eq. 'H') then
            iccount = iccount+1
            icurv1 = 6
          end if   
        end if
        isdat3(nsdat) = ibset(isdat3(nsdat),0)
c
c  Must change sign if downslur, but cannot do it now since we don't know
c    slur direction for sure.
c
        call setbits(isdat3(nsdat),6,2,32+icurv1)
        go to 1
      end if
c
c  Record shifts
c
      call setbits(isdat2(nsdat),6, 6,ivoff+32)
      call setbits(isdat2(nsdat),7,12,ihoff+64)
c
c  Record chord flag, note level, notehead shift
c
      if (notcrd) then
        call setbits(isdat2(nsdat),7,19,nolev)
      else
        nolevc = igetbits(icrdat(ncrd),7,12)
        call setbits(isdat2(nsdat),7,19,nolevc)
        isdat2(nsdat) = ibset(isdat2(nsdat),0)
        call setbits(isdat2(nsdat),2,1,igetbits(icrdat(ncrd),2,23))
      end if
      return
      end
      subroutine stop1()
        call exit(1)
      end
      subroutine topfile(basenameq,lbase,nv,clefq,noinst,musicsize,
     *                   xinstf1,mtrnmp,mtrdnp,vshrink,fbar,fontslur)
      parameter (nm=24)
      common /comlast/ islast,usevshrink
      logical islast,usevshrink,fontslur
      common /comtop / itopfacteur,ibotfacteur,interfacteur,isig0,
     *   isig,lastisig,fracindent,widthpt,height,hoffpt,voffpt,idsig,
     *   lnam(nm),inameq(nm)
      common /comarp/ narp,tar(8),ivar1(8),ipar1(8),levar1(8),ncmar1(8),
     *                xinsnow,lowdot
      common /comnvi/ nsperi(nm),nspern(nm),rename,iiorig(nm)
      character*79 inameq
      character*44 basenameq
      character*24 fmtq
      character*5 fbarq
      character*1 clefq(nm),sq,chax,clefqiv
      common /comstart/ facmtr
      logical vshrink,lowdot,rename
      logical novshrinktop,cstuplet
      common /comnvst/ novshrinktop,cstuplet
      common /comInstTrans/ iInstTrans(nm),iTransKey(nm),iTransAmt(nm),
     *  instno(nm),nInstTrans,EarlyTransOn,LaterInstTrans
      logical EarlyTransOn,LaterInstTrans
      common /comsize/ isize(nm)
      common /comis4bignv/ is4bignv,AIset
      logical is4bignv,AIset
      common /comc8flag/ c8flag(nm)
      logical c8flag
      sq = chax(92)
      vshrink = xinstf1.gt.20. .and. .not.novshrinktop 
      if (vshrink) then
        xinsnow = 10.
      else
        xinsnow = xinstf1
      end if
      if (.not.islast) return
c
c Initialize octave treble clef tracker
c
      do 6 im = 1 , nm
        c8flag(im) = .false.
6     continue
      write(11,'(a)')'%%%%%%%%%%%%%%%%%'
      write(11,'(a)')'%'
      write(11,'(a)')'% '//basenameq(1:lbase)//'.tex'
      write(11,'(a)')'%'
      write(11,'(a)')'%%%%%%%%%%%%%%%%'
      write(11,'(a)')sq//'input musixtex'
      write(11,'(a)')sq//'input pmx'
      write(11,'(a)')
     *     sq//'setmaxslurs{24}'//sq//'setmaxinstruments{24}%' 
      if (.not.fontslur) write(11,'(a)')sq//'input musixps'
c
c Need to input musixmad to permit more slurs.
c
      if (musicsize .eq. 20) then
        write(11,'(a)')sq//'normalmusicsize%'
      else if (musicsize .eq. 16) then
        write(11,'(a)')sq//'smallmusicsize%'
      else if (musicsize .eq. 24) then
        write(11,'(a)')sq//'largemusicsize%'
c
c  Eliminate, per Tennent's analysis and musixtex changes since first input
c
c        write(11,'(a)')sq//'def'//sq//'meterfont{'//sq//
c     *                  'meterlargefont}%'
      else if (musicsize .eq. 29) then
        write(11,'(a)')sq//'Largemusicsize%'
c        write(11,'(a)')sq//'def'//sq//'meterfont{'//sq//
c     *                   'meterLargefont}%'
      end if
c
c  Set sizes. Have sizes per staff in isize(.) and noinst per staff in
c    nsperi(.)
c
c 130324
c      iiv = 1
      do 5 iinst = 1 , noinst
c        if (isize(iiv) .eq. 1) then
        if (isize(iinst) .eq. 1) then
          if(iinst.le.9) then
            write(11,'(a8,i1,a)')
     *          sq//'setsize',iinst,sq//'smallvalue%'
          else
            write(11,'(a9,i2,a)')
     *          sq//'setsize{',iinst,'}'//sq//'smallvalue%'
          end if
c        else if (isize(iiv) .eq. 2) then
        else if (isize(iinst) .eq. 2) then
          if(iinst.le.9) then
            write(11,'(a8,i1,a)')
     *          sq//'setsize',iinst,sq//'tinyvalue%'
          else
            write(11,'(a9,i2,a)')
     *          sq//'setsize{',iinst,'}'//sq//'tinyvalue%'
          end if 
        end if
c        iiv = iiv+nsperi(iinst)
5     continue
      write(fbarq,'(f5.3)')fbar
      write(11,'(a)')sq//'nopagenumbers'
      write(11,'(a)')sq//'tracingstats=2'//sq//'relax'
      write(11,'(a7,i3,a2)')sq//'hsize=',nint(widthpt),'pt'
      write(11,'(a6,i'//chax(49+int(log10(height+.1)))//',a2)')
     *    sq//'vsize',int(height+.1),'pt'
      if (abs(hoffpt) .gt. 0.1) then
        if (hoffpt .le. -10.) then
          write(11,'(a8,i3,a2)')sq//'hoffset',nint(hoffpt),'pt'
        else if (hoffpt .lt. 0.) then
          write(11,'(a8,i2,a2)')sq//'hoffset',nint(hoffpt),'pt'
        else if (hoffpt .lt. 10.) then
          write(11,'(a8,i1,a2)')sq//'hoffset',nint(hoffpt),'pt'
        else
          write(11,'(a8,i2,a2)')sq//'hoffset',nint(hoffpt),'pt'
        end if
      end if
      if (abs(voffpt) .gt. 0.1) then
        if (voffpt .le. -10.) then
          write(11,'(a8,i3,a2)')sq//'voffset',nint(voffpt),'pt'
        else if (voffpt .lt. 0.) then
          write(11,'(a8,i2,a2)')sq//'voffset',nint(voffpt),'pt'
        else if (voffpt .lt. 10.) then
          write(11,'(a8,i1,a2)')sq//'voffset',nint(voffpt),'pt'
        else
          write(11,'(a8,i2,a2)')sq//'voffset',nint(voffpt),'pt'
        end if
      end if
c
c  The default  raisebarno=3.5 internote, set in pmx.tex.  Increase to 4.5 if
c  3 sharps and treble clef, to avoid vertical clash with top space g#
c
      if (isig.eq.3 .and. clefq(nv).eq.'t') write(11,'(a)')
     *     sq//'def'//sq//'raisebarno{4.5'//sq//'internote}'
      if (noinst .lt. 10) then
        write(11,'(a19,i1,a1)')sq//'def'//sq//'nbinstruments{',
     *       noinst,'}'
      else
        write(11,'(a19,i2,a1)')sq//'def'//sq//'nbinstruments{',
     *     noinst,'}'
      end if
      iv = 0
      do 1 iinst = 1 , noinst
        nstaves = nsperi(iinst)
        if (iinst .lt. 10) then
          write(11,'(a)')sq//'setstaffs'//chax(48+iinst)
     *        //chax(48+nstaves)
        else
          write(11,'(a11,i2,a)')sq//'setstaffs{',iinst,'}'
     *        //chax(48+nstaves)
        end if
        iv = iv+1
        clefqiv = clefq(iv)
        if (clefqiv.eq.'8') then
          clefqiv='t'
          iTransAmt(instno(iv)) = 7+iTransAmt(instno(iv))
          c8flag(iv) = .true.
        end if
        if (nstaves .eq. 1) then
          fmtq = chax(48+numclef(clefq(iv)))
          lfmtq = 1
        else
          fmtq = '{'//chax(48+numclef(clefq(iv)))
          lfmtq = 2
          do 2 k = 2 , nstaves
            iv = iv+1
            fmtq = fmtq(1:lfmtq)//chax(48+numclef(clefq(iv)))
            lfmtq = lfmtq+1
2         continue
          fmtq = fmtq(1:lfmtq)//'}'
          lfmtq = lfmtq+1
        end if
        if (iinst .lt. 10) then
          write(11,'(a)')sq//'setclef'//chax(48+iinst)//fmtq(1:lfmtq)
        else
          write(11,'(a9,i2,a)')sq//'setclef{',iinst,'}'//fmtq(1:lfmtq)
        end if
        if (clefq(iv) .eq. '8') then
          if (iinst .lt. 10) then
            write(11,'(a)')sq//'settrebleclefsymbol'//chax(48+iinst)//
     *        sq//'treblelowoct%'
          else
            write(11,'(a21,i2,a)')sq//'settrebleclefsymbol{',iinst,'}'//
     *        sq//'treblelowoct%'
          end if
          c8flag(iv) = .true.
        end if
        do 3 lname = 79 , 2 , -1
          if (inameq(iinst)(lname:lname) .ne. ' ') go to 4
3       continue
4       continue
        lnam(iinst) = lname
        if (iinst .lt. 10) then
          write(11,'(a8,i1,a)')sq//'setname',iinst,
     *         '{'//inameq(iinst)(1:lname)//'}'
        else
          write(11,'(a9,i2,a)')sq//'setname{',iinst,
     *         '}{'//inameq(iinst)(1:lname)//'}'
        end if
1     continue
      write(11,'(a18,i2,a2)')sq//'generalsignature{',isig,'}%'
      if (EarlyTransOn) 
     *  call Writesetsign(nInstTrans,iInstTrans,iTransKey,EarlyTransOn)
      call wgmeter(mtrnmp,mtrdnp)
      ipi = nint(fracindent*widthpt)
      if (ipi .lt. 10) then
        write(11,'(a11,i1,a2)')sq//'parindent ',ipi,'pt'
      else if (ipi .lt. 100) then
        write(11,'(a11,i2,a2)')sq//'parindent ',ipi,'pt'
      else
        write(11,'(a11,i3,a2)')sq//'parindent ',ipi,'pt'
      end if
      write(11,'(a)')sq//'elemskip1pt'//sq//'afterruleskip'
     *      //fbarq//'pt'//sq//'beforeruleskip0pt'//sq//'relax'
      if (.not.vshrink) then
        if (xinstf1 .lt. 9.95) then
          fmtq = '(a,f3.1,a)'
        else
          fmtq = '(a,f4.1,a)'
        end if
        facis = 1.
        if (is4bignv) facis = .95
        write(11,fmtq)sq//'stafftopmarg0pt'//sq//'staffbotmarg0pt'
     *       //sq//'interstaff{',xinstf1*facis,'}'//sq//'relax'
      else 
        write(11,'(a)')sq//'stafftopmarg0pt'//
     *         sq//'staffbotmarg5'//sq//'Interligne'//sq//
     *         'interstaff{10}'//sq//'relax'
      end if
      if (nv.eq.1) write(11,'(a)')sq//'nostartrule'
      write(11,'(a)')sq//'readmod{'//basenameq(1:lbase)//'}'
      if (cstuplet) then     
        write(11,'(a)')sq//'input tuplet'
     *    //sq//'def'//sq//'xnumt#1#2#3{'//sq//'zcharnote{#2}{~}'
     *      //sq//'def'//sq//'tuplettxt{'//sq//'smalltype'//sq//'it{#3}'
     *      //sq//'/'//sq//'/}}%'
        write(11,'(a)')sq//'let'//sq//'ovbkt'//sq//'uptuplet'
     *    //sq//'let'//sq//'unbkt'//sq//'downtuplet%'
      end if
      write(11,'(a)')sq//'startmuflex'//sq//'startpiece'//sq//
     *      'addspace'//sq//'afterruleskip%'
      return
      end
      character*1 function udfq(nolev,ncm)
c
c  Slur directions
c
      common /combc/ bcspec
      logical bcspec
        ntest = nolev-ncm
        if (ntest.lt.0 .or.
     *         (ntest.eq.0 .and. bcspec .and. ncm.eq.23) ) then
          udfq = 'd'
        else
          udfq = 'u'
        end if
      return
      end
      character*1 function udqq(nole,ncm,isl,nvmx,ivx,nv)
c
c  Stem direction for single notes
c
      character*1 ulfq
      character*1 udqqq
      logical btest
      if (btest(isl,30)) then
c
c  Absolute override
c
        if (btest(isl,17)) then
          udqqq = 'u'
        else
          udqqq = 'l'
        end if
      else if (nvmx .eq. 1) then
c
c  Single voice per staff, default
c
        udqqq = ulfq(1.*nole,ncm)
      else
c
c  Multi-voice per staff, 1st is lower, 2nd upper
c
        if (ivx .le. nv) then
          udqqq = 'l'
        else
          udqqq = 'u'
        end if
      end if
      udqq = udqqq
      return
      end
      character*1 function ulfq(xnolev,ncm)
c
c  Stem directions
c
      common /combc/ bcspec
      logical bcspec
        test = xnolev-ncm
        if (test.lt.-.001 .or.
     *         (test.lt..001.and.bcspec.and.ncm.eq.23) ) then
          ulfq = 'u'
        else
          ulfq = 'l'
        end if
      return
      end
      function upcaseq(chq)
      character*1 chq,upcaseq,chax
        if (ichar(chq).ge.61.and.ichar(chq).lt.122) then
          upcaseq = chax(ichar(chq)-32)
        else
          upcaseq = chq
          print*,'Warning, upcaseq was called with improper argument: '
     *            //chq
          stop
        end if
      return
      end
      subroutine wgmeter(mtrnmp,mtrdnp)
c
c  Writes meter stuff to file 11, so only called if islast=.true.
c
      character*1 sq,chax
      if (mtrdnp .eq. 0) return
      sq=chax(92)
      if (mtrnmp.gt.0 .and. mtrnmp.le.9) then
        if (mtrdnp .lt. 10) then
          write(11,'(a25,i1,a2,i1,a3)')
     *           sq//'generalmeter{'//sq//'meterfrac{',
     *           mtrnmp,'}{',mtrdnp,'}}%'
        else
          write(11,'(a25,i1,a2,i2,a3)')
     *           sq//'generalmeter{'//sq//'meterfrac{',
     *           mtrnmp,'}{',mtrdnp,'}}%'
        end if
      else if (mtrnmp .ge. 10) then
        if (mtrdnp .lt. 10) then
          write(11,'(a25,i2,a2,i1,a3)')
     *           sq//'generalmeter{'//sq//'meterfrac{',
     *           mtrnmp,'}{',mtrdnp,'}}%'
        else
          write(11,'(a25,i2,a2,i2,a3)')
     *           sq//'generalmeter{'//sq//'meterfrac{',
     *           mtrnmp,'}{',mtrdnp,'}}%'
        end if
      else if (mtrnmp .lt. 0) then
        write(11,'(a26,i1,a2,i1,a3)')
     *           sq//'generalmeter{'//sq//'meterfracS{',
     *           -mtrnmp,'}{',mtrdnp,'}}%'
      else if (mtrdnp .le. 4) then
          write(11,'(a21,i1,a2)')
     *             sq//'generalmeter{'//sq//'meterN',mtrdnp,'}%'
      else if (mtrdnp .eq. 5) then
        write(11,'(a)')sq//'generalmeter'//sq//'allabreve%'
      else if (mtrdnp .eq. 6) then
        write(11,'(a)')sq//'generalmeter'//sq//'meterC%'
      else if (mtrdnp .eq. 7) then
        write(11,'(a)')sq//'generalmeter'//sq//'meterIIIS%'
      end if
      return
      end
      subroutine writemidi(jobname,ljob)
      parameter(nm=24,mv=24576)
      common /all/ mult(nm,200),iv,nnl(nm),nv,ibar,
     *   ivxo(600),ipo(600),to(600),tno(600),tnote(600),eskz(nm,200),
     *   ipl(nm,200),ibm1(nm,9),ibm2(nm,9),nolev(nm,200),ibmcnt(nm),
     *   nodur(nm,200),jn,lenbar,iccount,nbars,itsofar(nm),nacc(nm,200),
     *   nib(nm,15),nn(nm),lenb0,lenb1,slfac,musicsize,stemmax,
     *   stemmin,stemlen,mtrnuml,mtrdenl,mtrnmp,mtrdnp,islur(nm,200),
     *   ifigdr(2,125),iline,figbass,figchk(2),firstgulp,irest(nm,200),
     *   iornq(nm,0:200),isdat1(202),isdat2(202),nsdat,isdat3(202),
     *   isdat4(202),beamon(nm),isfig(2,200),sepsymq(nm),sq,ulq(nm,9)
      character*1 ulq,sepsymq,sq
      logical beamon,firstgulp,figbass,figchk,isfig
      character*1 byteq(4),char,chax
      character*10 tempoq,instq
      character*44 jobname
      integer*2 mmidi
      logical restpend,relacc,notmain,twoline,ismidi,crdacc
      common /commidi/ imidi(0:nm),trest(0:nm),mcpitch(20),mgap,
     *       iacclo(0:nm,6),iacchi(0:nm,6),midinst(nm),
     *       nmidcrd,midchan(nm,2),numchan,naccim(0:nm),
     *       laccim(0:nm,10),jaccim(0:nm,10),crdacc,notmain,
     *       restpend(0:nm),relacc,twoline(nm),ismidi,mmidi(0:nm,mv),
     *       debugmidi
      logical debugmidi
      common /commvel/ midivel(nm),midvelc(0:nm),midibal(nm),midbc(0:nm)
     *                ,miditran(nm),midtc(0:nm),noinst,iinsiv(nm)
      common /comevent/ miditime,lasttime
      logical mmacrec,gottempo
      common /commmac/ mmacstrt(0:nm,20),mmacend(0:nm,20),immac,
     *       mmactime(20),nmidsec,msecstrt(0:nm,60),msecend(0:nm,60),
     *       mmacrec,gottempo
      character*79 inameq
      common /comtop / itopfacteur,ibotfacteur,interfacteur,isig0,
     *   isig,lastisig,fracindent,widthpt,height,hoffpt,voffpt,idsig,
     *   lnam(nm),inameq(nm)
      logical rename
      common /comnvi/ nsperi(nm),nspern(nm),rename,iiorig(nm)
c
c  Used to be icmm(0:nm); did midi fail when nv>16?
c
      integer*2 iinsiv,icmm(0:15)
	character*5 versionc
	common /comver/ versionc
c
c  These are not consecutive because channel 9 is reserved for percussion.
c
      data icmm /0,1,2,3,4,5,6,7,8,10,11,12,13,14,15,16/
c
c  Write Header
c
      write(51,'(a,$)')'MThd'//char(0)//char(0)//char(0)//char(6)
     *  //char(0)//char(1)//char(0)//char(numchan+1)//char(0)//char(240)
      if (debugmidi)
     *  write(52,'(a6,10Z4)')'"MThd"',0,0,0,6,
     *  0,1,0,numchan+1,0,240
c
c  Write the "conductor" track, for keys, meter, and tempos
c  Get the number of bytes in the conductor event stream
c
      ndata = 1+imidi(numchan)-msecstrt(numchan,nmidsec)
      do 15 isec = 1 , nmidsec-1
        ndata = ndata+1+msecend(numchan,isec)-msecstrt(numchan,isec)
15    continue
c      ib1 = (4+ljob+26+ndata+4)/256
c      ib0 = 4+ljob+26+ndata+4-256*ib1
      ib1 = (4+ljob+27+ndata+4)/256
      ib0 = 4+ljob+27+ndata+4-256*ib1
      write(51,'(a,$)')'MTrk'//char(0)//char(0)//char(ib1)//char(ib0)
c
c  Text header
c
c     *  //char(0)//char(255)//char(1)//char(ljob+26)
     *  //char(0)//char(255)//char(1)//char(ljob+27)
      if (debugmidi)
     *  write(52,'(a6,8z4)')'"MTrk"',0,0,ib1,ib0,
     *         0,255,1,ljob+27
      write(51,'(a,$)')jobname(1:ljob)
      if (debugmidi) write(52,'(a)')'"'//jobname(1:ljob)//'"'
c
c  (separate writes are needed to defeat compiler BUG!!!)
c
c      write(51,'(a,$)')'.mid, produced by PMX 2.30'
      write(51,'(a,$)')'.mid, produced by PMX '//versionc
      if (debugmidi) write(52,'(a)')
     *  '".mid, produced by PMX '//versionc//'"'
c
c  Conductor event data: Loop over sections. 
c
      do 16 isec = 1 , nmidsec
        if (isec .lt. nmidsec) then
          mend = msecend(numchan,isec)
        else
          mend = imidi(numchan)
        end if
        do 17 i = msecstrt(numchan,isec) , mend
          write(51,'(a,$)')char(mmidi(numchan,i))
          if (debugmidi) write(52,'(z4)')mmidi(numchan,i)
17      continue
16    continue
c
c  And close out the time sig / tempo track.
c
      write(51,'(a,$)')char(0)//char(255)//char(2*16+15)//char(0)
      if (debugmidi) write(52,'(4z4)')0,255,2*16+15,0
c
c  Loop over track for each voice:  The following sets up iv.
c
      iv = nv
      if (twoline(nv)) then
        kv = 2
      else
        kv = 1
      end if
c
      do 5 icm = 0 , numchan-1
c
c  Get the number of bytes in the data stream
c
        ndata = 1+imidi(icm)-msecstrt(icm,nmidsec)
        do 11 isec = 1 , nmidsec-1
          ndata = ndata+1+msecend(icm,isec)-msecstrt(icm,isec)
11      continue
c
c  Add 3 for instrum, 4 for bal,  plus 4 (for closing) to byte count, 
c
        ndata = ndata+11
c
c  Add 4+lnam(iinsiv(iv)) if lnam>0 ,
c
        if (lnam(iinsiv(iv)).gt.0) ndata = ndata+4+lnam(iinsiv(iv))
c
c  Separate total byte counts into 4 bytes
c
        do 2 ibyte = 1 , 4
          if (ndata .gt. 0) then
            byteq(ibyte) = char(mod(ndata,256))
            ndata = ishft(ndata,-8)
          else
            byteq(ibyte) = char(0)
          end if
2       continue
c
c  Now write front stuff for this track
c
        write(51,'(a,$)')'MTrk'//byteq(4)//byteq(3)//byteq(2)//byteq(1)
     *    //char(0)//char(12*16+icmm(icm))//char(midinst(iinsiv(iv)))
     *    //char(0)//char(11*16+icmm(icm))//char(10)//char(midbc(icm))
        if (debugmidi) write(52,'(a4,z2,a7,11z4)')'icm=',icm,
     *    ' "MTrk"',ichar(byteq(4)),ichar(byteq(3)),ichar(byteq(2)),
     *    ichar(byteq(1)),0,12*16+icmm(icm),midinst(iinsiv(iv)),
     *    0,11*16+icmm(icm),10,midbc(icm)
        if (lnam(iinsiv(iv)) .gt. 0) then
c
c  Add instrument name as sequence name
c
          write(51,'(a,$)')char(0)//char(255)//char(3)
     *        //char(lnam(iinsiv(iv)))
          if (debugmidi) write(52,'(4z4)')0,255,3,lnam(iinsiv(iv))
          write(51,'(a,$)')
     *        inameq(iinsiv(iv))(1:lnam(iinsiv(iv)))
          if (debugmidi) write(52,'(a)')
     *        '"'//inameq(iinsiv(iv))(1:lnam(iinsiv(iv)))//'"'
        end if
        write(tempoq,'(i2)')icm
        write(instq,'(i3)')midinst(iinsiv(iv))
        call printl('MIDI instrument '//tempoq(1:2)//' is '//instq(1:3))
c
c  Notes: Loop over sections. 
c
        do 9 isec = 1 , nmidsec
          if (isec .lt. nmidsec) then
            mend = msecend(icm,isec)
          else
            mend = imidi(icm)
          end if
          do 10 i = msecstrt(icm,isec) , mend
            write(51,'(a,$)')char(mmidi(icm,i))
            if (debugmidi) write(52,'(z4)')mmidi(icm,i)
10        continue
9       continue
c
c  Closing 4 bytes
c
        write(51,'(a,$)')chax(0)//char(255)//char(2*16+15)//char(0)
        if (debugmidi) write(52,'(4z4)')0,255,2*16+15,0
        if (kv .eq. 2) then
          kv = 1
        else if (iv .eq. 1) then
          go to 5
        else
          iv = iv-1
          if (twoline(iv)) kv=2 
        end if
5     continue
      write(*,'(1x,a12,(10i6))')'Bytes used:',(imidi(icm),icm=0,numchan)
      write(15,'(1x,a12,(10i6))')
     *                          'Bytes used:',(imidi(icm),icm=0,numchan)
      close(51)
      if (debugmidi) close(52)
      return
      end
      subroutine Writesetsign(nInstTrans,iInstTrans,iTransKey,flag)
      parameter(nm=24)
      integer*4 iInstTrans(nm),iTransKey(nm)
      character*79 notexq
      character*1 chax
	logical flag
c
c  Assumes notexq is blank
c
      do 1 i = 1 , nInstTrans
        notexq = chax(92)//'setsign'
        lnote = 8
        if (iInstTrans(i) .lt. 10) then
          notexq = notexq(1:lnote)//chax(48+iInstTrans(i))
          lnote = lnote+1
        else
          write(notexq(lnote+1:lnote+4),'(a1,i2,a1)')
     *         '{',iInstTrans(i),'}'
          lnote = lnote+4
        end if
        if (iTransKey(i) .lt. 0) then
          write(notexq(lnote+1:lnote+4),'(a1,i2,a1)')
     *          '{',iTransKey(i),'}'
          lnote = lnote+4
        else
          notexq = notexq(1:lnote)//chax(48+iTransKey(i))
          lnote = lnote+1
        end if
        write(11,'(a)')notexq(1:lnote)//'%'
1     continue
      flag = .false.
	return
      end
      subroutine writflot(x,notexq,lenline)
      character*(*) notexq
      if (x .lt. 0.95) then
        write(notexq(lenline+1:lenline+2),'(f2.1)')x
        lenline = lenline+2
      else if (x .lt. 9.95) then
        write(notexq(lenline+1:lenline+3),'(f3.1)')x
        lenline = lenline+3
      else
        write(notexq(lenline+1:lenline+4),'(f4.1)')x
        lenline = lenline+4
      end if
      return
      end
      subroutine wsclef(iv,ninow,nclef)
c      subroutine wsclef(iv,ninow,clefq,nclef)
c
c  Writes \setclef for instrument containing *staff* iv
c    
      parameter (nm=24)
      common /comlast/ islast,usevshrink
      logical islast,usevshrink
      common /comnvi/ nsperi(nm),nspern(nm),rename,iiorig(nm)
      logical rename
c
c In pmx271, had removed clefq, so with 2 or more staves in
c   an instrument, had problems. So replace in 272
c
      common /comclefq/ clefq(nm)      
      character*1 clefq,chax
      character*40 temq
      common /comc8flag/ c8flag(nm)
      logical c8flag
      if (nclef .lt. 7) then
        clefq(iv) = chax(48+nclef)
      else
        clefq(iv)='9'
      end if
      if (.not.islast) return
      iv1 = 1
      do 1 iinst = 1 , ninow
        if (iv .lt. iv1+nspern(iinst)) go to 2
        iv1 = iv1+nspern(iinst)
1     continue
      print*
      print*,'Should not be here in wsclef!'
      call stop1()
2     continue
c
c  Here, iinst is the instrument number with staff of clef change
c
      iv2 = iv1+nspern(iinst)-1
      if (iinst .lt. 10) then
        temq = chax(92)//'setclef'//chax(48+iinst)
        ltem = 9
      else
        write(temq,'(a9,i2,a1)') chax(92)//'setclef{',iinst,'}'
        ltem = 12
      end if
      if (iv1 .eq. iv2) then
c
c  Only one staff (iv) in instrument with clef change
c
        write(11,'(a)')temq(1:ltem)//clefq(iv)//'%'
      else
        temq = temq(1:ltem)//'{'
        ltem = ltem+1
c
c  Loop over staves, but clefq has only changed for one of them
c
        do 3 iiv = iv1 , iv2
          temq = temq(1:ltem)//chax(48+numclef(clefq(iiv)))
          ltem = ltem+1
3       continue
        write(11,'(a)')temq(1:ltem)//'}%'
      end if
      if (c8flag(iv)) then
c
c If we change FROM octave treble clef to some other, need the following.
c
        write(11,'(a)')char(92)//'settrebleclefsymbol'//chax(48+iinst)
     *                 //char(92)//'trebleclef%'
        c8flag(iv) = .false.
      end if
      return
      end
      subroutine chkpmxlyr(lineq,iccount,lyrerr)
      character*128 lineq
      character*1 charq
      lyrerr = 0
c
c  On entry, last char was "
c
18    call g1etchar(lineq,iccount,charq)
      if (iccount .eq. 121) then
        lyrerr = 2
        return
c      else if (charq.eq.'"') then
      else if (charq.eq.'"' .and. 
     *  .not.(lineq(iccount-1:iccount-1).eq.char(92))) then
        call g1etchar(lineq,iccount,charq)
c
c  Check for raise/lower command
c
        if (charq .eq. '@') then
c
c   @  positions lyrics vertically for current voice
c     [a,b]  above or below of the staff
c        +/- i offset, \internotes
c
          call g1etchar(lineq,iccount,charq)
          if (index('ab',charq).eq.0) then
            lyrerr = 3
            return
          end if
          call g1etchar(lineq,iccount,charq)
          if (index('+-',charq).eq.0) then
            lyrerr = 4
            return
          end if
          call g1etchar(lineq,iccount,charq)
          if (index('0123456789',charq).eq.0) then
            lyrerr = 5
            return
          end if
        else if (charq .ne. ' ') then
c
c  2nd " must be followed by ' '
c
          lyrerr = 1
          return
        end if
        return
      end if
      go to 18
      end
      subroutine dopmxlyr(lineq,iccount)
c
c lineq has " at iccount. Find end of lyrics string, replace "..." 
c   with \pmxlyr{...}\, but also look for ~ in lyrics and replace with '\ll ',
c   (unless preceded with '\'), check length
c
      character*128 lineq,lineqt
      character*1 sq,chax
      sq = chax(92)
      iend = lenstr(lineq,128)
c
c      i2nd = iccount+index(lineq(iccount+1:128),'"')
c Find position of closing '"'; must bypass any \" which is used for umlaut
c
      i2nd = iccount+index(lineq(iccount+1:128),'"')
2     continue
      if (lineq(i2nd-1:i2nd-1).eq.char(92)) then
        i2nd = i2nd+index(lineq(i2nd+1:128),'"')
        go to 2
      end if
      istart = iccount
1     itilde = istart+index(lineq(istart+1:i2nd-1),'~')
      if (itilde.gt.istart.and.itilde.lt.i2nd) then
c
c  Replace tilde if not preceded by \
c
        if (iend .ge. 117) then
          print*,'Sorry, lyric string is too long, stopping'
          call stop1()
        else if (lineq(itilde-1:itilde-1) .eq. sq) then
          istart = itilde
          go to 1
        end if
        lineqt = lineq(1:itilde-1)//sq//'lk '
     *                             //lineq(itilde+1:iend)
        iend = lenstr(lineqt,128)
        i2nd = i2nd+3
        lineq = lineqt
        go to 1
      end if
      if (iccount .eq. 1) then
        lineqt = sq//'pmxlyr{'//lineq(2:i2nd-1)//'}'//sq
     *             //lineq(i2nd+1:128)
      else
        lineqt = lineq(1:iccount-1)//sq//'pmxlyr{'
     *             //lineq(iccount+1:i2nd-1)//'}'//sq
     *             //lineq(i2nd+1:128)
      end if
      i2nd = i2nd+8
      lineq = lineqt
      if (lineq(i2nd+1:i2nd+1) .eq. '@') then
        lineqt = lineq(1:i2nd)//'at{'//lineq(i2nd+2:i2nd+4)//'}'//sq
     *           //lineq(i2nd+5:128)
        lineq = lineqt
      end if
      return
      end
      subroutine inst2chan(midc,midi,midchan,nv,iinsiv,twoline)
c      propagate per-instrument quantities to per-channel ones
      parameter (nm=24)
      integer*4 midc(0:nm),midi(nm),midchan(nm,2)
      integer*2 iinsiv(nm)
      logical twoline(nm)
      do iv = nv, 1, -1
        if (twoline(iv)) then
          midc(midchan(iv,2)) = midi(iinsiv(iv))
        end if
        midc(midchan(iv,1)) = midi(iinsiv(iv))
      end do
      end


