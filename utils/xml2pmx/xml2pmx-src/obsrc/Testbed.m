MODULE Testbed;  

(***************************************************************************)
(**************** Copyright 2015--2021 Dieter Gloetzel ********************)
(************************ Version016 multi platform  ***********************)
(***************************************************************************)
(*  This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>. *)

    (* corrections starting 09.07.2020    beam2pmx add "l" or "u" to "[j"    *)
 (*  IMPORT Texts, Strings, Files, Oberon, b, Complete, Out;  (*   Import fuer Oberon Version *) *)
 (*  IMPORT Args32, Strings, Files, Out := WCout, b, Complete, Fifo ;  *)  (* Import fuer Windows-Exe Version  *)
 IMPORT Args, Files := MyFiles, Strings := Strings1, Out, b, Complete;   (* Import for Linux-Version *)


CONST 
	NL = 0AX;  BLANK = 20X;  (* TAB = 09X; *)    CR = 0DX;  measurelimit = 600;  
TYPE 
	LONGINT = INTEGER;
	ControlDesc = RECORD  (* data structure of input data *)
				xml, dtd: ARRAY 256 OF CHAR;  
				title: ARRAY 128 OF CHAR;  
				composer, lyricist, info, software: ARRAY 128 OF CHAR;  
				rights: ARRAY 128 OF CHAR;  
				parts: ARRAY 30 OF ARRAY 16 OF CHAR;  
				instruments: ARRAY 30 OF ARRAY 32 OF CHAR;  
				worknumber: ARRAY 32 OF CHAR;  
				equalkeys : BOOLEAN;

			END;  
	AttributesDesc = RECORD  (* initial signature, meter , #staves,clef *)
			
				measure, note, pickup: LONGINT;  
				divisions, duration: INTEGER;  
				fifth: INTEGER;  
				beats: INTEGER;  
				beattype, mtrdenp: INTEGER;  
				staves: INTEGER;  
				clefsign: ARRAY 4 OF CHAR;   (* index is for staff *)
				clefline: ARRAY 4 OF LONGINT; 
				diatonic, chromatic: LONGINT;  
				octave: LONGINT;   (* parameters in <transpose> *)
			END;  
	DirectionDesc = RECORD  (* data structure for directions *)
				part, staff, measure, note, lastnote,   voice: LONGINT;   (* voice ? implizit ueber lastnote definiert *)
				before: BOOLEAN;  
				placement: CHAR;   (* "h" or  "l" for above and below *)
				dirtype: CHAR;   (* words, metronome, dynamics, wedge= hairpin, pedal, segno, coda, other *)
				wedgetype: ARRAY 32 OF CHAR;   (* crescendo, diminuendo, stop crescendo and stop diminuendo *)
				dyntype: ARRAY 16 OF CHAR;  
				pedaltype: ARRAY 16 OF CHAR;   (* start, stop, *)
				text: ARRAY 128 OF CHAR;  
				pmxdirection: ARRAY 128 OF CHAR;  
				defaultx: INTEGER;  
				used: BOOLEAN;   (* avoid duplication of dynamic marks in case of two voices in a staff 
				enable two directions at one note *)
			END;  
	NoteDesc = RECORD  (* properties of a note *)
		accidental: ARRAY 10 OF CHAR;  
		actual: INTEGER;  
	(*	arpeggio: BOOLEAN;  *)
	(*	base : INTEGER;  Base note of a chord *) 
		beam, closebeam: ARRAY 16 OF CHAR;  
		chord: CHAR; cue : BOOLEAN; 
		clef: CHAR;  
		clefchanged: CHAR;  
		defaultx: INTEGER;  
		dot: CHAR;  
		fermata: ARRAY 16 OF CHAR;  
		from, to, duration: INTEGER;  
		grace: INTEGER;  
		lefttext, righttext: ARRAY 256 OF CHAR;  
		maxbeam : LONGINT; (* number of beams for binary tremolo*)
		normal: INTEGER;  
		pitchoctave: INTEGER;  
		pitchstep: CHAR;  
		pmxgrace: ARRAY 32 OF CHAR;  (* Aenderung 16.11.2020 *)
		pmxnote: ARRAY 64 OF CHAR;  
		probj: BOOLEAN;   (* evaluates print-object = "no" TRUE means "print", FALSE don't print *)
		rbleft, rbright: ARRAY 32 OF CHAR;   (* blind rests to fill incomplete measures *)
		repeat: ARRAY 16 OF CHAR;  
		rest, blind: CHAR;  
		slur: ARRAY 2 OF ARRAY 8 OF CHAR;  
		staccato,  accent, trill, strongaccent: ARRAY 8 OF CHAR;  
		staff: INTEGER;  
		stem: CHAR;  
		tied: ARRAY 2 OF ARRAY 8 OF CHAR;  
		trembeam: LONGINT;  
		tremolotype : ARRAY 32 OF CHAR;
		tuplet: ARRAY 16 OF CHAR;  
		type, normaltype: ARRAY 8 OF CHAR;   (* begend begin and end of a series of Grace notes *)
		voicetime: INTEGER;  
	END;  
	MeasureDesc = RECORD  (* properties of measure *)
				beattype, beats, fifth: INTEGER;  
				repeat: ARRAY 16 OF CHAR;  
				ending: ARRAY 16 OF CHAR;  
				barstyle, meterchange: ARRAY 16 OF CHAR;  
				voicetime: ARRAY 27 OF ARRAY 4 OF INTEGER;  
				duration, keys: ARRAY 27 OF INTEGER;
				(* divisions : ARRAY 27 OF INTEGER;  *)
				dur : INTEGER; (* measure actual length of a measure *)
				clefchange : ARRAY 27 OF CHAR; (* Maerz 2018 *)
			END;  
	
VAR 
 
   uptomeasure : LONGINT; (* global variable for reducing the number of measures *)
	
	notes: POINTER TO ARRAY 24 OF ARRAY 3 OF ARRAY 600 OF ARRAY 83 OF POINTER TO NoteDesc;  (*20-10-2020 Lilypond2 *)
	
	(* 	notes: POINTER TO ARRAY 24  OF ARRAY 3 OF ARRAY 400 OF ARRAY 64 OF  NoteDesc;  *)

	keytotal : ARRAY 132 OF CHAR;
	unix: BOOLEAN;  outputcont : ARRAY 32 OF CHAR; outputset : SET;
	voicemeasure: POINTER TO ARRAY 30 OF ARRAY 600 OF SET;  
	voiceps: ARRAY 30 OF SET;  
	voicelimmps: ARRAY 30 OF LONGINT;  
	vmapps: ARRAY 30 OF ARRAY 5 OF LONGINT;  
	minvoice, maxvoice: POINTER TO ARRAY 27 OF ARRAY 600 OF LONGINT;  
	vmap: ARRAY 30 OF ARRAY 600 OF ARRAY 5 OF LONGINT;  
	(* notetypei : ARRAY 10 OF INTEGER;  inverse of notetype index 0 => 64 *)
	
	clefspec, lastclef: ARRAY 27 OF CHAR;   (*  special clef and valid clef in the actual measure *)
	
	voicelimm: ARRAY 30 OF ARRAY 600 OF LONGINT;  
	sout : ARRAY 128 OF CHAR; (* target file path and directory *)

	(* controls#  voices within a staff and measure*)
	voicecount: ARRAY 30 OF LONGINT;   (* counts different voices within a staff *) countnote, countattr, countdir : LONGINT;

	ingrace2pmx: LONGINT;  
	in, out: ARRAY 128 OF CHAR;  
	lastto, lastfrom: INTEGER;   (* global variable for time progress in measure *)
	pmxdur: ARRAY 16 OF CHAR;   (* global variable: duration of notes in PMX *)
	lastnote : LONGINT;   (* global variable for position of last note *)
	fi, fo: Files.File;  ri: Files.Rider;  
	(* VAR notes : notestype; *)
	voicetime: ARRAY 27 OF ARRAY 4 OF INTEGER;   (* progress  of notes duration in part,staff,voice, actual measure *)
	lasttype: ARRAY 32 OF CHAR;   (* beam type of preceding note *)
	laststaff : LONGINT;  
	closebeam: ARRAY 8 OF CHAR;   (* staff of preceding note for beam calculation *)
	(* q: FIFO; *)  (* global variable for sorted notes queue *)
	maxdir, i, j, part, staff, voice, measure, note, count, maxpart, maxmeasure, itags, maxgrace, controlpart, 
	nostaves, nties, lfdnr, ps, dirnum, lastdirnum, attnum, lastattnum, lastgrace: LONGINT;  
	lastdyn: ARRAY 32 OF CHAR;  
	(* Global variables *)
	stavesfound: BOOLEAN;  
	partlabel: ARRAY 27 OF ARRAY 5 OF CHAR;  
	directions: POINTER TO ARRAY 2000 OF ARRAY 5 OF DirectionDesc;  
	maxdirtype: ARRAY 2000 OF LONGINT;   (* vorlaeufig, nur zum Daten sammeln. *)
	attributes: ARRAY 30 OF AttributesDesc;  
	partstaff: ARRAY 30 OF ARRAY 2 OF LONGINT;  
	(*	notes: ARRAY 30 OF ARRAY 2 OF ARRAY 500 OF ARRAY 64 OF NoteDesc;   Aenderung wegen voice numerierung *)
	maxnote, maxnote0, maxnote1, minnote0, minnote1, minnote: ARRAY 30 OF ARRAY 3 OF ARRAY 600 OF LONGINT;  
	(* number of last note in part/staff/measure *)
	measures: POINTER TO ARRAY 600 OF MeasureDesc;  
	pmxcontrol: ControlDesc;  
	accidentaltag, parttag, measuretag, notetag, pitchtag, steptag, octavetag, durationtag, voicetag, dottag, stemtag, cleftag, clefoctavetag, staccatotag: ARRAY 32 OF CHAR;  
	attributestag, divisionstag, keytag, fifthstag, timetag, beatstag, beattypetag, stavestag, signtag, linetag, typetag, fermatatag, tuplettag: ARRAY 32 OF CHAR;  
	notationtag, lyrictag, syllabictag, texttag, resttag, chordtag, backuptag, repeattag, slurtag, stafftag, printtag, directiontag, tiedtag, 
	cleflinetag, actualtag, normaltag, beamtag, wordstag, eonotetag, eomeasuretag, barlinetag, endingtag, barstyletag, dynamicstag, 
	worktag, identificationtag, defaultstag, eoattributestag, forwardtag, eodirectiontag, credittag, gracetag, gracetag2, cuetag : ARRAY 32 OF CHAR;  
	staves: ARRAY 30 OF LONGINT;   (*  maximum staff of "part" *)

	beamopen : ARRAY 30 OF ARRAY 3 OF BOOLEAN;
(*	cl: Args32.LPSTR;  (*  Command line for Windows.Exe  *)  *)


	
	
	PROCEDURE pmxtype( xmltype: INTEGER ): INTEGER;  
	VAR i: INTEGER;  
	BEGIN 
		CASE xmltype OF 
		1:     i := 0;  
		| 2:   i := 2;  
		| 4:   i := 4;  
		| 8:   i := 8;  
		| 16: 
				i := 1;  
		| 32: 
				i := 3;  
		| 64: 
				i := 6;  
		ELSE 
			Out.Ln();  i := -1;  Out.String( "xml note type  : " );  Out.Int( xmltype, 5 );  
			Out.String( " not implemented." );  Out.Ln();  
		END;  
		RETURN i;  
	
	END pmxtype;  


	PROCEDURE FillRests( notefrom, noteto, measure, ps, voice: LONGINT );  
	(* Fills incomplete measures with blind rests, as needed *)
	VAR note, part, staff, lastnote: LONGINT;  delta, lastto: INTEGER;  (* global "lastnote" removed 29.11.2016 *)
	BEGIN 
		part := partstaff[ps, 0];  staff := partstaff[ps, 1];  
                lastto := 0; lastnote := 0;
		
		IF (measures[measure].voicetime[ps, voice] < measures[measure].duration[part]) THEN 
			  (* Out.Ln();  Out.String( "Fillrests: notefrom. noteto " );  Out.Int( notefrom, 5 );  Out.Int( noteto, 5 ); *)  note := notefrom;  
			WHILE note <= noteto DO 
				(* Out.Ln();  Out.Int( measure, 5 );  Out.Int( ps, 5 );  Out.Int( voice, 5 );  Out.Int( note, 5 );  *)
				(*	NoteOut(notes[ps,voice,measure,note]);  *)
				INC( note );  
			END;  
			
			
			(* left of first note *)
			note := notefrom;  
			IF (notes[ps, voice, measure, note] # NIL) & 
			(notes[ps, voice, measure, note].from > 0) THEN 

				delta := notes[ps, voice, measure, note].from - 1; (* Out.Ln();  Out.String( " note delta : " );  
				Out.Int( note, 5 );  Out.Int( delta, 5 );  *)
				IF delta > 0 THEN Complete.Int2br( attributes[part].divisions, delta, notes[ps, voice, measure, note].rbleft );  END;  
	
						lastnote := note;  lastto := notes[ps, voice, measure, note].to;  
			
			ELSE 
			END;  
			(* left of 2nd to last note *)
			INC( note );  
			
			WHILE (note <= noteto) DO 
			IF ( notes[ps, voice, measure, note] # NIL ) &
				 (notes[ps, voice, measure, note].from > 0) THEN 
					delta := notes[ps, voice, measure, note].from - lastto - 1;   
				(*	Out.Ln();  Out.String( " note delta : " );  	Out.Int( note, 5 );  Out.Int( delta, 5 );  *)
					
					IF delta > 0 THEN 
						Complete.Int2br( attributes[part].divisions, delta, notes[ps, voice, measure, note].rbleft );  	
						

					END;  
					lastnote := note;  
					IF notes[ps, voice, measure, note] # NIL
					THEN 
					lastto := notes[ps, voice, measure, note].to;  
					END 
				
				END;  
				
				INC( note );  
			END;  
			(* right of last note *)
			delta := measures[measure].duration[part] - lastto; 
			 (*  Out.Ln();  Out.Int(ps,5); Out.Int(measure,5);Out.String( " note delta : " );  
			 			Out.Ln(); Out.Int( lastnote, 5 );  Out.Int( delta, 5 );  *)
			IF delta > 0 THEN 
				Complete.Int2br( attributes[part].divisions, delta, notes[ps, voice, measure, lastnote].rbright );   

			END;  
		
		END;  
	
	END FillRests;  

	PROCEDURE PMXdyn( XMLdyn: ARRAY OF CHAR;  VAR out: ARRAY OF CHAR );   
	(* converts e.g. <f /> to " Df "*)
	BEGIN 
	(* doubke on purpose *)
		IF XMLdyn = "<f />" THEN COPY( " Df ", out );  END;  
		IF XMLdyn = "<ff />" THEN COPY( " Dff ", out );  END;  
		IF XMLdyn = "<fff />" THEN COPY( " Dfff ", out );  END;  
		IF XMLdyn = "<ffff/>" THEN COPY( " Dffff ", out );  END;  
		IF XMLdyn = "<mf />" THEN COPY( " Dmf ", out );  END;  
		IF XMLdyn = "<p />" THEN COPY( " Dp ", out );  END;  
		IF XMLdyn = "<pp />" THEN COPY( " Dpp ", out );  END;  
		IF XMLdyn = "<ppp />" THEN COPY( " Dppp ", out );  END;  
		IF XMLdyn = "<pppp />" THEN COPY( " Dpppp ", out );  END;  
		IF XMLdyn = "<mp />" THEN COPY( " Dmp ", out );  END; 
		IF XMLdyn = "<sf />"  THEN COPY( " Dsfz ", out );  END; 
		IF XMLdyn = "<fp />"  THEN COPY( " Dfp ", out );  END;  
		IF XMLdyn = "<f/>" THEN COPY( " Df ", out );  END;  
		IF XMLdyn = "<ff/>" THEN COPY( " Dff ", out );  END;  
		IF XMLdyn = "<fff/>" THEN COPY( " Dfff ", out );  END;  
		IF XMLdyn = "<ffff/>" THEN COPY( " Dffff ", out );  END;  
		IF XMLdyn = "<mf/>" THEN COPY( " Dmf ", out );  END;  
		IF XMLdyn = "<p/>" THEN COPY( " Dp ", out );  END;  
		IF XMLdyn = "<pp/>" THEN COPY( " Dpp ", out );  END;  
		IF XMLdyn = "<ppp/>" THEN COPY( " Dppp ", out );  END;  
		IF XMLdyn = "<pppp/>" THEN COPY( " Dpppp ", out );  END;
		IF XMLdyn = "<mp/>" THEN COPY( " Dmp ", out );  END;  
		IF XMLdyn = "<sf/>"  THEN COPY( " Dsfz ", out );  END;  
		IF XMLdyn = "<fp/>"  THEN COPY( " Dfp ", out );  END;  

 

		IF ( XMLdyn = "crescendo") & (lastdyn # " D<" ) THEN COPY( " D<", out );  COPY( out, lastdyn );  END;  (* 18.02.2017 *)
		
		IF ( XMLdyn = "diminuendo" ) & (lastdyn # " D>" ) THEN COPY( " D>", out );  COPY( out, lastdyn );  
		END;  
		IF XMLdyn = "stop" THEN COPY( lastdyn, out );IF lastdyn # "" THEN  Strings.Append( out, "+0+3" ); END;  
		b.loesch( lastdyn );  END;  
		(*	Out.String( XMLdyn );  Out.Char( "|" );  Out.String( out );   *)
	END PMXdyn;  

	PROCEDURE WriteLInt( VAR W: Files.Rider;  i: LONGINT );  
	VAR si: ARRAY 10 OF CHAR;  
	BEGIN 
		Strings.IntToStr( i, si );  Files.Write( W, BLANK );  WriteString( W, si );  
	END WriteLInt;  

	PROCEDURE WriteString( VAR W: Files.Rider;  s: ARRAY OF CHAR );  
	BEGIN 
		Files.WriteBytes( W, s, Strings.Length( s ) );  
	END WriteString;  


	PROCEDURE SetOutput*;
	VAR c : CHAR; i : LONGINT;
	BEGIN
	i := 0; WHILE i < Strings.Length(outputcont) DO
	c := outputcont[i];
	CASE c OF
 
    | "R"  : INCL(outputset,10);  (* remove notes with print-obj="no" in print phase; e.g. for  xml-files with programmed Trill 20-11-2019 *)  
    | "P" : INCL(outputset,9); (* parser output 0 durch 9 ersetzt 20.05.2019 *)
	| "D" : INCL(outputset,1); (* list directions *)
	| "V" :  INCL(outputset,2); (* list voices per measure and instrument *)
	| "A" : INCL(outputset,3); (* Statistics of MusicXML tags *)
	| "L" : INCL(outputset,4); (* store lyrics *)
	| "S" : INCL(outputset,5); (* remove all slurs *)
	| "T" : INCL(outputset,6); (* remove all ties *)
	| "G" : INCL(outputset,7); (* eliminate slurs around grace notes, replace by PMX internal Grace notes. *)
	| "X" : INCL(outputset,8); (* x-option for voice crossing slurs, only with PMX282 *)
	| "N"  :  (* no option chosen *)
	
	ELSE Out.String(" option not implemented.")
	END;
	INC(i); END;
	END SetOutput;

(*	PROCEDURE commandO*;   (* Command for Oberon-Version *)
	VAR R: Texts.Reader;  i: LONGINT;  c: CHAR;  
	BEGIN 
		(* 0. Read Filenames from Oberon.Par.text *)
		Texts.OpenReader( R, Oberon.Par.text, Oberon.Par.pos );  Texts.Read( R, c );  i := 0;  
		WHILE (c # "~") & (~R.eot) & (i < LEN( comline )) DO comline[i] := c;  Texts.Read( R, c );  INC( i ) END;  
		comline[i - 1] := 0X;  		Out.Ln(); Out.String("comline : ");Out.String (comline);
		Filenames(FALSE, comline, in, out, outputcont );  
		Stripfilename(out,sout); InOut( in, out );  

	END commandO;  *)
	
	(*	PROCEDURE commandX;  (* WIndows EXE *)
	VAR i: LONGINT;  c: CHAR;  
	BEGIN 
		Args32.Str( "Program XML2PMX.EXE Copyright 2016 Dieter Gloetzel" );  Args32.Ln();  

		cl := Args32.GetCommandLine();  Args32.CopyString( cl, comline );  Args32.Str( comline );  Args32.Ln();  
		Filenames(TRUE,comline,in,out,outputcont); 
		Stripfilename(out,sout); InOut( in, out );  
	END commandX;   *)
	PROCEDURE CommandU; 
	 VAR output : ARRAY 16 OF CHAR; kno : LONGINT;
	BEGIN
		kno := Args.argc;
		IF ( kno >= 3 ) THEN
			Args.GetArg(1, in);
			Args.GetArg(2, out);
			COPY(out,sout);
			Strings.ChangeSuffix(sout,"txt"); 
			IF ( kno >= 4 ) THEN Args.GetArg(3,output); END; 
			Strings.Upper(output,outputcont);
			SetOutput;       b.voutput := (2 IN outputset);
				IF ( kno = 5 ) THEN Args.GetArg(4,output); Strings.StrToInt(output,uptomeasure); END;
		Out.String( "Linux Binary XML2PMX Copyright 2015/2021 Dieter Gloetzel" );  Out.Ln();	
			InOut(in, out);
		ELSE
			Args.GetArg(0, in);
			Out.String(in); Out.String(" error: argument number < 2 "); Out.Ln;
		END;
	END CommandU; 
	
	PROCEDURE Voicing( q: b.FIFO);  
	(* calculates an array of Sets vontaining the voices for part, staff and measure. *)
	VAR n: b.Tag;  
	BEGIN 
		n := q.first;  
		WHILE (n.next # NIL ) DO 
			IF n.tagname = notetag THEN 
				ps := linstaff( nostaves, n.part, n.staff );  INCL( voicemeasure[ps, n.measure], n.voice );  
				INCL( voiceps[ps], n.voice );  maxvoice[ps, n.measure] := b.Max( maxvoice[ps, n.measure], n.voice );  
				minvoice[ps, n.measure] := b.Min( minvoice[ps, n.measure], n.voice );  
			END;  
			n := n.next;  
		END;  
	END Voicing;  

 PROCEDURE DeleteTag(deltag : b.Tag);
	VAR n : b.Tag;
	BEGIN
	n := b.q.first;
	REPEAT n := n.next UNTIL (n.next = NIL) OR (n.next = deltag);
	IF n.next = deltag THEN n.next := deltag.next END;
	(* Out.Ln(); Out.Int(deltag.nr, 5); Out.String("deleted");	*)
	END DeleteTag;
	
PROCEDURE DeleteTS (tag : ARRAY OF CHAR); (* deletes all ties or slurs according to "tag" *)
	VAR n : b.Tag;    count : LONGINT;
	BEGIN
		n:= b.q.first; count := 0;
		WHILE n.next # NIL DO 
		
		IF  (n.tagname = tag) (* tags of type "tag" will be removed. *)
		THEN 
	(*	b.OutTag(n, TRUE); Out.String("deleted"); *)
		    DeleteTag(n); INC(count);
		 END;		
		n:= n.next END;
		Out.Ln(); Out.String("********************************");
		Out.Int (count,5); Out.String(tag); Out.String ("deleted");
END DeleteTS; 

	
	

PROCEDURE InOut( infilename, outfilename: ARRAY OF CHAR );  
	VAR n: b.Tag;   (*  data structure for sorting notes *)
		outfilenameprep: ARRAY 64 OF CHAR;  
		fprep: Files.File;  rprep: Files.Rider;  res : INTEGER;
	BEGIN 
		Out.String( "This is XML2PMX Version 16. dyn. alloc." );  fi := Files.Old( infilename );  
	IF (fi # NIL ) THEN  	(* 1*)
				Files.Set( ri, fi, 0 );  
			(***************************************)
			(* 1. Analyze XML data *)         AnalyzeXML2( ri );  
			
			(*	n:= b.q.first; WHILE n.next # NIL DO b.OutTag(n, TRUE); n:= n.next END;  *)
				
			(*	DeleteTies or Slurs; *)
			IF (5 IN outputset) THEN DeleteTS("<slur>"); END;
			IF (6 IN outputset) THEN DeleteTS("<tied>"); END;
						
			(* 2. create temporary result storage for PMX-data *)
			COPY( outfilename, outfilenameprep );  Strings.Append( outfilenameprep, "prep" );  
			fprep := Files.New( outfilenameprep );  Files.Set( rprep, fprep, 0 );  
			 Out.Ln(); Out.String("Intermediate output file "); Out.String(outfilenameprep); Out.String(" created. ");
			 
				(* 3. map part/staff to a linear index. *) AllStaves( staves );  
				Out.Ln(); Out.String("3: Allstaves done!");
				(* 4. Extract control data  *) ControlProp;  
				Out.Ln(); Out.String("4: ControlProp done!");
				(* 5. Enrich Data *) Enrich( b.q ); 
					Out.Ln(); Out.String("5: Enrich done!");				
		(*		  n := b.q.first;  
				 WHILE n.next # NIL DO b.OutTag( n, TRUE );  n := n.next END;  *)
				(****************************************************************************)
				(*6.  Calculate data for mapping of voices between PMX and XML *)
				
				Voicing( b.q );   (* Store voices wrt part,stave and measure.  *)
					Out.Ln(); Out.String("6 : Voicing done!");

			IF (2 IN outputset)	THEN Out.Ln();  
			Out.String( "*************** voices per instrument (part/staff) " );  ps := 0;  
				Out.Ln(); Out.String( "nostaves" );  Out.Int( nostaves, 5 );  
				WHILE ps < nostaves DO 
					Out.Ln();  Out.Int( ps, 5 );  b.Outset( voiceps[ps], voicelimmps[ps], vmapps[ps] );  INC( ps );  
				END;  
			END;
				ps := 0;  
                                IF 2 IN outputset THEN
				  Out.Ln();
				  Out.String( "*************** voices per measure and  instrument (part/staff) " ); Out.Ln();
                                END;
				WHILE ps < nostaves DO;
					  measure := 1;  
					WHILE measure <= maxmeasure DO 
						IF (2 IN outputset) THEN 
						    Out.Ln();  Out.Int( ps, 5 );  Out.Int( measure, 5 );
						END;
						b.Outset( voicemeasure[ps, measure], voicelimm[ps, measure], vmap[ps, measure] );  
						(* Out.Int(minvoice[ps,measure],5); Out.Int(maxvoice[ps,measure],5); *)
						INC( measure );  
					END;  
					INC( ps );  
				END;  
				(****************************************************************************)
				
				
						
			(*	i := 1;  
			 	WHILE i <= maxpart DO 
					Out.Ln();  Out.String( "part, label : " );  Out.Int( i, 5 );  Out.String( partlabel[i] );  INC( i );  
				END;  *)
				(* 7. identify potential pickup *)  Pickup( b.q, attributes[1].pickup );   
					Out.Ln(); Out.String("7: Pickup done!");
					


				(* 8. Generate Control data for PMX (i.e everything before the notes and store in"outfilename"*)
				
				ControlData( rprep );
					Out.Ln(); Out.String("8: ControlData done!");
			
				

				
				(* *)
				
				(*9. Investigate time series of notes (e.g. incomplete measures)	*)
				 progress( b.q );  
					Out.Ln(); Out.String("9: progress done!");
	(*		listmeter;*)

										
			(*	 IF 1 = 0 THEN *)
				(* 10. Store everything in arrays measurewise *)
				EnumerateTags;  Out.Ln();  Out.String( "10: EnumerateTags done!" );  
			
				 IF 9 IN outputset THEN (* Option "p" *)
				   n := b.q.first;  WHILE (n.next # NIL) & (n.measure < uptomeasure) DO b.OutTag(n,TRUE);
				  				  	   n:= n.next; END;    
				 END; 

				(* listmeter;*)
				(* 11.. Generate PMX and store in outfilename *)
				(* 14.11.2020: Do not link directions to grace notes *)
				(* IF (voice = 0) & (notes[ps,voice,measure,note].grace = 0 ) THEN *) DistributeDirections;  (* END; *)
				WritePMX( rprep );  Out.Ln(); Out.String( "11: nach WritePMX" );  
				(* 12. Remove multiple Blanks from result file and break lines after 100 Chars *)
				Files.Register( fprep );  fo := Files.New( outfilename );  Copywo( fprep, fo, unix );  Files.Close( fi );  
				Files.Close(fprep);				Files.Delete( outfilenameprep, res );  
				 IF ( res = 0 ) THEN Out.Ln(); Out.String("Intermediate data deleted"); END;
				Files.Register( fo );  Files.Close( fo );   Out.Ln();  Out.String(out); Out.String( " registered" );   	 
				
	(*	  END; *)
		  
		IF (4 IN outputset) THEN b.writetext;  END; (* Lyrics is decoded in MODULE "b" and appears in "songtext.txt" in this directory. *)
		
		IF (3 IN outputset) THEN b.SortTags END; (* lists all occurences of XML-Tags. *)
		ELSE 
			Out.Ln(); Out.String("input file: "); Out.String(infilename); Out.String(" not found "); 
	 END;

	(*	Out.Ln(); Out.String("countclefchanges : ");

		Out.Int(countclefchanges,5); *)
                Out.Ln()
	END InOut;  


	PROCEDURE Copywo( VAR fin, fout: Files.File;  unix: BOOLEAN );  
	(* Copies a File and eliminates multiple BLANKs. *)
	VAR ch: CHAR;  rin, rout: Files.Rider;  column: LONGINT;  
	BEGIN 
		Files.Set( rin, fin, 0 );  Files.Set( rout, fout, 0 );  column := 0;  Files.Read( rin, ch );  
		WHILE ~rin.eof DO 
			IF (~unix) THEN Files.Write( rout, Strings.OberonToISO[ORD( ch )] );  INC( column );  
			ELSIF unix & (ch # CR) THEN Files.Write( rout, Strings.OberonToISO[ORD( ch )] );  INC( column );  
			END;  
			IF (ch = NL) THEN column := 0;  END;  
			IF (column > 100) & (ch = BLANK) THEN 
				column := 0;  
				IF (~unix) THEN Files.Write( rout, CR );  END;  
				Files.Write( rout, NL );  
			END;  
			
			(* 	IF ch =Strings.CR THEN Files.Write(R,NL); END; *)
			IF (ch = BLANK) THEN 
				WHILE (ch = BLANK) DO Files.Read( rin, ch );  END;  
			ELSE Files.Read( rin, ch );  
			
			END;  
		
		END;  
	END Copywo;  

	PROCEDURE DistributeDirections;  
	VAR idir, j, lastnote, lastidir, lastj, lastmeasure, lastlastnote, lastps, noteto, firstnote, count: LONGINT;  
		lastdirtype: CHAR;  
		posnote: ARRAY 64 OF INTEGER;  
	BEGIN 
                lastnote := 0;
		idir := 1;  
		WHILE idir <= maxdir DO 
			j := 1;  
			WHILE j <= maxdirtype[idir] DO 
		
			
			part := directions[idir, 0].part;  
			staff := directions[idir, 0].staff;  lastmeasure := measure;  
				measure := directions[idir, 0].measure;  
				lastlastnote := lastnote;  lastnote := directions[idir, 0].lastnote;  
				
					
					IF (1 IN outputset) THEN	 (* Print all directions *)			OutDir( idir, j ); 			END;  
				
				(*	IF lastnote = 0 THEN lastnote := 1 END; *)
						lastps := ps;  ps := linstaff( nostaves, part, staff );  firstnote := minnote0[part, staff, measure]; 
		IF (notes[ps,0,measure,firstnote].grace > 0) THEN  (* first note is grace note and cannot carry dynamic marks *) 
				
				(* Out.Ln(); Out.String("DistrDir : ");Out.Int(ps,5); Out.Int(measure,5); Out.Int(firstnote,5); Out.Char("|");
				Out.Int(notes[ps,0,measure,firstnote].grace,5 ); Out.Int(lastnote,5); *)
								
				REPEAT INC(firstnote) UNTIL notes[ps,0,measure,firstnote].grace = 0;
				Out.Ln(); Out.String(" first real note at ps, measure, note :"); Out.Int(ps,5); Out.Int(measure,5); Out.Int(firstnote,5); 
				Out.Char("|");	Out.Int(notes[ps,0,measure,firstnote].grace,5 );

		END; 
				CASE directions[idir, j].dirtype OF 
				
				"w":  (* words, i.e. text *)
						IF lastnote < firstnote THEN lastnote := firstnote END;  
						
						IF (notes[ps, 0, measure, lastnote].lefttext = "") THEN 
							Strings.Append( notes[ps, 0, measure, lastnote].lefttext, directions[idir, j].pmxdirection ); (* pmxdirection pruefen *)
							(* Out.Ln(); Out.String("idir,j,pmxdirection "); Out.Int(idir,5); Out.Char("|");Out.Int(j,5); Out.Char("|");
							Out.String(directions[idir,j].pmxdirection); *)
						
						ELSE 
							b.APPzca( notes[ps, 0, measure, lastnote].lefttext, directions[idir, j].pmxdirection );  
							b.loesch( directions[idir, j].pmxdirection );  
						END;  
				
				| "h":  (* hair pins *)
						IF lastnote < firstnote THEN lastnote := firstnote END;  
						IF ( lastnote > maxnote0[part,staff,measure] ) THEN lastnote := maxnote0[part,staff,measure] END; (* Modification 23.12.2016 *)
						Strings.Append( notes[ps, 0, measure, lastnote].righttext, directions[idir, j].pmxdirection );  
						lastdirtype := "h";  
				| "d": (* dynamic marks *)
						IF (directions[idir, j].lastnote = 0) & (directions[idir, j].defaultx = 0) THEN 
							directions[idir, j].lastnote := firstnote;  lastnote := directions[idir, j].lastnote
						ELSIF (directions[idir, j].lastnote > 0) & (directions[idir, j].lastnote < maxnote0[part, staff, measure]) THEN 
							INC( directions[idir, j].lastnote );  lastnote := directions[idir, j].lastnote
						ELSIF (directions[idir, j].lastnote = 0) & (directions[idir, j].defaultx > 0) THEN 
							(* Out.Ln();  Out.String( "Direction default-x: " );  Out.Int( directions[idir, j].defaultx, 5 );  *)
							note := minnote0[part, staff, measure];  noteto := maxnote0[part, staff, measure];  
							(* Out.String( "note : " );  Out.Int( note, 5 );  Out.String( "noteto : " );  Out.Int( noteto, 5 );  *)
							count := noteto - note + 1;  (* Out.String( "count : " );  Out.Int( count, 5 );  *)
							
							WHILE note <= noteto DO 
								posnote[note - minnote0[part, staff, measure]] := notes[ps, 0, measure, note].defaultx;  
								INC( note );  
							END;  
							lastnote := b.MinDist( directions[idir, j].defaultx, posnote, SHORT( count ) ) + 1;  
							(* Out.String( "lastnote : " );  Out.Int( lastnote, 5 );  *)
						
						END;  
						(*	IF (notes[ps, 0, measure, lastnote].rest = "r") THEN  	(* avoid dynamics on rests. *)
								REPEAT INC( lastnote ) UNTIL notes[ps, 0, measure, lastnote].rest # "r";   
							END;  *)
						IF notes[ps, 0, measure, lastnote] # NIL THEN
						Strings.Append( notes[ps, 0, measure, lastnote].righttext, directions[idir, j].pmxdirection );  
					(*				Out.Ln(); Out.String("DistrDirections , lastnote, righttext :"); 
									 Out.Int(measure,5);Out.Int(lastnote,5);  Out.String(notes[ps, 0, measure, lastnote].righttext); *)
										END;

						lastdirtype := "d";   		
					| "p": 
						INC( lastnote );  
						IF lastnote < firstnote THEN lastnote := firstnote END;  
						Strings.Append( notes[ps, 0, measure, lastnote].lefttext, directions[idir, j].pmxdirection );  
						lastdirtype := "p";  
			| "s":
			IF lastnote < firstnote THEN lastnote := firstnote END;  
						Strings.Append( notes[ps, 0, measure, lastnote].righttext, directions[idir, j].pmxdirection );  
						lastdirtype := "s";  

			
			| "c":
			IF lastnote < firstnote THEN lastnote := firstnote END;  
						Strings.Append( notes[ps, 0, measure, lastnote].righttext, directions[idir, j].pmxdirection );  
						lastdirtype := "c";  

				
				ELSE Out.Ln();  Out.String( "DistributeDirections : dirtype unknown." );  
				END;  
				(* Out.Ln(); Out.String(" ps,measure,lastnote ");Out.Int(ps,5); Out.Int(measure,5); Out.Int(lastnote,5); Out.Char("|");Out.String(directions[idir,j].pmxdirection); 
					
					*)
				lastj := j;  INC( j );  
			END;  
			
			lastidir := idir;  INC( idir );  
		END;  
	
	END DistributeDirections;  


	PROCEDURE PMXDuration( tuplettype,tremolotype : ARRAY OF CHAR;  div, xmldur: INTEGER;  actual, normal: INTEGER;  
											 VAR pmxdur: ARRAY OF CHAR;  istuplet: BOOLEAN;  type: ARRAY OF CHAR );  
	(* Calculates the pmx-duration of a note (0,2,4,8,1,3,6) or rest from the XML-duration. Takes into account one dot and two dots		*)
	VAR   double: CHAR;  tupletdur,noteduration,  i : INTEGER;
	
	
	BEGIN 
	
IF ( istuplet & (tremolotype = "") ) THEN (* 1 *)
			double := 0X;  
			noteduration := xmldur; 	
			
			 tupletdur := Complete.tupletduration(div,type,normal); (*  total duration of tuplet in terms of xml-duration *)
		IF tuplettype = "start" THEN (* tuplets *) (*  2 *)
		   Complete.Dur2PMX(div,tupletdur,pmxdur ); 
		   
		ELSE
		    i := 0; WHILE i < 16 DO pmxdur[i] := 0X; INC(i) END;
        END;		(*  2 *)	
		 IF  ABS(actual * noteduration - tupletdur) > 5 THEN (*  3 *)
				
			IF ABS(noteduration*actual - 2 * tupletdur) <  5 THEN double := "D"			END;  (* Das ist Swing *)
			
			IF ABS (noteduration*actual * 2 - 3 * tupletdur) <  5 THEN double := "d"			END;  (* Das ist Punktierung *)
			
			Strings.AppendCh( pmxdur, double );  
	 
        END; (*  3 *)
ELSIF istuplet & ( tremolotype ="start" ) THEN	 (* binary tremolo *) (* 1a *)

		tupletdur :=  Complete.tupletduration(div,type,normal);
		Complete.Dur2PMX(div,tupletdur,pmxdur );
		
 ELSIF ( tremolotype = "stop" ) THEN (* 1b *)
   	i := 0; WHILE i < 16 DO pmxdur[i] := 0X; INC(i) END;
	
ELSE (* normal notes *)
		Complete.Dur2PMX(div,xmldur,pmxdur ); 
	

END;	

END PMXDuration;  


	PROCEDURE DelTag*;  
	VAR nlast, n, m: b.Tag;  
	BEGIN 
                nlast := NIL;
		n := b.q.first;  
		WHILE n.next # NIL DO 
			IF (n.next.tagname = notetag) & (n.next.probj = "n") THEN 
			(*		OutTag(n);		OutTag(n.next); *)
				(* nlast := n; *) (* n.next ist Note *)
				m := n.next;  
				REPEAT m := m.next 
				UNTIL m.tagname = eonotetag;  
				(*		OutTag( m ); *) 
				nlast.next := m.next;  
				
				IF nlast.next.tagname ="<backup>"  THEN nlast.next :=  nlast.next.next.next END;
				(* eliminate backup *)
				
				(*	Out.Ln(); Out.String(" nlast.nr und m.next.nr : ");
		Out.Int(nlast.nr,5); Out.Char("|");Out.Int(m.next.nr,5); *)
				n := m;  
			ELSE 
				n := n.next;  nlast := n;  
				
				(*	Out.Ln(); Out.String("ELSE n, n.next, nlast : ");Out.Int(n.nr,5); Out.Int(n.next.nr,5);  Out.Int(nlast.nr,5); *)
			END;  
		
		END;  
		(* n := q.first; REPEAT n:= n.next UNTIL n.nr = 20530 ; WHILE i < 30 DO OutTag (n); n:= n.next; INC(i) END;*)
	END DelTag;  

	


	PROCEDURE WriteNote2PMX( VAR W: Files.Rider;  VAR Note: NoteDesc;  
												  ps, voice, voicefrom, measure, note: LONGINT;  VAR Dtext, Rtext: ARRAY OF CHAR;  
												  VAR istuplet: BOOLEAN );  
	(* Writes the data for one note or rest to Files.Rider "W"; called by PROC. W ritePMX; *)
	VAR pmxdur: ARRAY 16 OF CHAR;  sactual : ARRAY 4 OF CHAR;
		pmxnote: ARRAY 64 OF CHAR;  blindrest : ARRAY 32 OF CHAR;
		tremolo: ARRAY 32 OF CHAR;
		octave: CHAR;  stemchar: CHAR;  maxnotelastmeasure : LONGINT;
	
	BEGIN 

	
	IF ( Note.grace # -1 ) THEN (* avoid chords as grace notes *) 
	
(*	IF (Wpos := Files.Pos(W); *)
	(* remove duplicate grace notes *)
(* Out.Ln();	Out.Int(ps,5); Out.Int(voice,5); Out.Int(measure,5); Out.Int(note,5); Out.Int(Note.grace,5); *)
 (*IF Note.grace > 1 THEN
		Out.Ln();	Out.String("remove duplicates");Out.Int(Note.grace,5);

		IF (Note.pmxnote # 0X) THEN Out.String(Note.pmxnote); END;	
		b.loesch ( Note.pmxnote) ;	
		 Out.Ln();   IF (Note.pmxgrace # 0X) THEN Out.String(Note.pmxgrace);END;	
		   b.loesch ( Note.pmxgrace) ;
		   END; 	*)


			part := partstaff[ps, 0];  staff := partstaff[ps, 1];  pmxnote[0] := BLANK;  pmxnote[1] := 0X;  
		COPY( Note.lefttext, Dtext );  (* 07.04.2017 *)
		
		IF (Note.clefchanged # 0X) THEN Files.Write(W,BLANK); Files.Write(W,"C"); Files.Write(W,Note.clefchanged); Files.Write(W,BLANK); END;
		IF (Dtext[0] # 0X) THEN WriteString( W, Dtext );  END;  
		IF (Note.rbleft[0] # 0X) THEN WriteString( W, Note.rbleft );  END; 

		IF (Note.pitchstep # 0X) THEN 
		(* Branch for normal notes, tuplet notes and  grace notes , as opposed to rests*)
			(* 1 *)
			INC( count );  
		
			istuplet := Note.actual # 0;  
					
				IF Note.grace =  0 THEN (* d.h. es ist keine Gracenote! *)
		
				PMXDuration( Note.tuplet, Note.tremolotype, attributes[part].divisions,  
										Note.duration, Note.actual, Note.normal, pmxdur, istuplet, Note.type );  
			END;
					(* delete slur before gracenote *)
			IF (7 IN outputset) & (Note.grace =  0) OR ~(7 IN outputset) (* option control via outputset 24.04.2017*)
			 THEN  (* Aenderung 18.04.2017*)
					IF (Note.slur[0,1] = "(")  THEN  (* 3*)
									Strings.Append( pmxnote, Note.slur [0]);  
								END;
					
					
					IF (Note.slur[1,1] ="(") THEN 
											Strings.Append(pmxnote,Note.slur[1]); END;
			
					END; (*   3*)   
			
			IF (Note.grace = 1) THEN  (*  ist erste einer Folge von grace notes *)
				
				Strings.Append( pmxnote, Note.pmxgrace ); 
		(*		 Out.Ln();  Out.String( " $$$$pmxgrace " );  Out.String( pmxnote );  *)
			
			END;   (* 4*)
			
			IF (Note.tied[0, 1] = "{") THEN  (* 5*)
				
				Strings.Append( pmxnote, Note.tied[0] )
			ELSIF (Note.tied[1, 1] = "{") THEN Strings.Append( pmxnote, Note.tied[1] )
			
			END;   (* 5*)
	
	(*		IF ~istuplet & (Note.beam[1] = "[") & (Note.grace = 0) THEN  *)  (* 6  alter code 14.01.2017 *)
		IF (  ~istuplet OR ( istuplet	&    ( Note.tuplet = "start")	 ) )
		
		  & (Note.beam[1] = "[") & (Note.grace = 0) & (Note.tremolotype # "start")
				(* & ( ~beamopen[ps,voice] ) (*10. 05. 2017 *)
				&  ????????????? *)
		THEN  
					
					 Strings.Append( pmxnote, Note.beam );  
					 beamopen[ps,voice] := TRUE; (* 10.5.2017                                    *)
			
			END;   (* 6*)
			IF Note.trembeam > 0 THEN 

				b.pmxTremolo( Note.pitchstep, Note.pitchoctave, Note.stem, Note.clef, Note.trembeam, pmxdur[0], 
										  tremolo );  
				
				Strings.Append( pmxnote, tremolo );  
				
				Out.Ln();  Out.String( "tremolo&&&&" );  Out.Int( ps, 5 );  
				Out.Int( measure, 5 );  Out.String( tremolo );  
			
			END;  
			
			Strings.AppendCh( pmxnote, Note.chord );  
			IF Note.grace = 0 THEN  (* attention: suppress duplicate Grace notes *)  (* 7*) 
			(* also normale Note "0" 18.11.2020*)

				
				
				Strings.AppendCh( pmxnote, Note.pitchstep );  
			END;   (* 7*)                                                                     
			
			IF (Note.chord # "z") THEN Strings.Append( pmxnote, pmxdur );  END;   (* 8*)
			
			octave := CHR( Note.pitchoctave + 48 );  
			IF Note.grace =  0 THEN Strings.AppendCh( pmxnote, octave );  (* also normale Note "0" 18.11.2020*)

(*			IF Note.grace # 1 THEN Strings.AppendCh( pmxnote, octave );  (* also normale Note *) *)
			Strings.Append( pmxnote, Note.accidental ); 
			(* Out.Ln(); Out.String("accid in WriteNote2PMX"); Out.Int(measure,5);Out.String(Note.accidental); *)
			END;   (* Dont forget accidentals for gracenotes *)  (* 9*)
			(* test for basenote , find series of chord notes
			IF Note.base # 0 THEN noteno := Note.base; 
			Out.Ln(); Out.String("test for abse of chord ");WHILE notes[ps,voice,measure,noteno].base =  Note.base DO 
			 Out.Ln(); Out.Int(ps,5); Out.Int(voice,2); Out.Int(measure,5); Out.Int(noteno,5); 
			 Out.Int(notes[ps,voice,measure,noteno].base,5);INC(noteno); END; END; *)
			
			IF (Note.tuplet = "start") & (Note.actual # 0) 
				THEN  (* 10*)
				
				Strings.AppendCh( pmxnote, "x" ); Strings.IntToStr(Note.actual,sactual); 
				Strings.Append( pmxnote, sactual );  
			END;   (* 10*)
			(* binary tremolo *)
				IF (Note.tremolotype = "start") & (Note.actual # 0)  (* first note of binary tremolo *)
					THEN  (* 18*)
					
					Strings.AppendCh( pmxnote, "x" );
					Strings.AppendCh(pmxnote,"T");
					 Strings.IntToStr(Note.maxbeam,sactual);
					 Strings.Append( pmxnote, sactual );  Strings.AppendCh(pmxnote, "0");
				END;   (* 18*)

			
			IF (Note.chord # "z") & (Note.tuplet # "start") & (Note.grace = 0) & ( Note.tremolotype # "start" ) (* 11*)
			THEN 
	    (*  no stem direction specified eventuell switch einbauen !  *)
			stemchar := Note.stem;  
				IF stemchar = "d" THEN stemchar := "l" END;  
				Strings.AppendCh( pmxnote, stemchar );  
			END;   (* 11*)                                                                                 
			(*	IF (Note.pmxdyn[0] # 0X) THEN  
			WriteString( W, Note.pmxdyn );  
		END;   *)
			(* Suppress fermata in tuplet *)
			IF ( Note.actual = 0 ) THEN Strings.AppendCh( pmxnote, BLANK );  Strings.Append( pmxnote, Note.fermata );  END;
			Strings.AppendCh( pmxnote, BLANK );  Strings.Append( pmxnote, Note.staccato );  
			Strings.AppendCh( pmxnote, BLANK );  Strings.Append( pmxnote, Note.accent );  
			Strings.AppendCh( pmxnote, BLANK );  Strings.Append( pmxnote, Note.strongaccent ); 
			Strings.AppendCh( pmxnote, BLANK );  Strings.Append( pmxnote, Note.trill );  


		(*	IF Note.arpeggio  THEN Strings.Append( pmxnote, " ? " );  maxarp := Note.maxarp END;  (* ????????  22. Nov. 2019 *)
			IF (maxarp > 0) & (Note.maxarp = maxarp) THEN Strings.Append( pmxnote, " ? " );  maxarp := 0;  END;   *)
			
	(* No closing slur at gracenote or one after gracenote *)
			maxnotelastmeasure := maxnote0[part,staff,measure-1];
			IF voice = 1 THEN	maxnotelastmeasure := maxnote1[part,staff,measure-1]; END;
IF (7 IN outputset) & ( ( note = 1 ) & (Note.grace = 0) & (  notes[ps,voice,measure,maxnotelastmeasure].grace= 0 ) 
			OR  ( note >1 ) &( Note.grace = 0) & (notes[ps,voice,measure,note - 1].grace = 0 ) ) (* Aenderung wg. Don 17022017 *)
				OR ~(7 IN outputset)
				THEN   (* end of slur directly after gracenote not allowed  *) (* Blinder Versuch 7 statt 5 *)

			IF (Note.slur[0,1] = ")") THEN  (* 13*)
			IF Note.grace = 0 THEN
				Strings.Append( pmxnote, Note.slur[0] ); END;  END;
				IF (Note.slur[1, 1] = ")") THEN 
			IF Note.grace = 0 THEN

				Strings.Append( pmxnote, Note.slur[1] ); END; END;

			END;  (*  13*)   
			
			IF (Note.tied[0, 1] = "}") (* 14*)
			THEN 
				Strings.Append( pmxnote, Note.tied[0] );  
			ELSIF (Note.tied[1, 1] = "}") THEN Strings.Append( pmxnote, Note.tied[1] );  
			END;   (* 14*)
			
			
		(*	IF ~istuplet & (Note.beam[1] = "]") & (Note.grace = 0) THEN  (* 15*)  backchanged 16. 4. 2017 changed 14.01.2017 *)
		IF ( ~istuplet  OR ( istuplet	&  ( Note.tuplet = "stop" ) ) )
		& (Note.beam[1] = "]") & (Note.grace = 0 ) & (Note.tremolotype # "stop") 	
		THEN (* VErsion 12 *)
			IF beamopen[ps,voice] THEN	
				Strings.Append( pmxnote, Note.beam );   
				beamopen[ps,voice] := FALSE;
			END;
			END;   (* 15*)
			
			IF ~istuplet & (Note.closebeam[1] = "]") & (Note.grace = 0) THEN  (* 15a closing for single note beam *)
				IF beamopen[ps,voice] THEN	
						Strings.Append( pmxnote, Note.closebeam );  
						beamopen[ps,voice] := FALSE;
				END;
						
					


			END;   (* 15a*)
		
			
			
			(************************ clef changed *******************)
			
	(*		IF (Note.clefchanged # 0X) THEN  (* 16*) 
			  (* There is a clef change after this note. *)
			  Out.String("Note clefchanged"); Out.Char(Note.clefchanged);
			
		(*	INC(countclefchanges); *)
				(*	IF note < maxnote[part,staff,measure] THEN *)
				maxinote := maxnote0[part,staff,measure]; 
				 IF voice > voicefrom THEN maxinote := maxnote1[part, staff, measure] END;
		IF note <  maxinote THEN	
				
				Strings.Append( pmxnote, " C" );  Strings.AppendCh( pmxnote, Note.clefchanged );  
				Strings.AppendCh( pmxnote, BLANK ); 
				  
				 Out.Ln(); Out.Int(ps,5); Out.Int(voice,5); Out.Int(measure,5); Out.Int(note,5);
				Out.String( "cleffx" );  Out.Char( Note.clefchanged );  Out.Char("|"); Out.Char( Note.clef ); 
		ELSE
				Out.String( "clef change after last note "); Out.Int(maxinote,5); 
						(* activated 14. Maerz 2018 , verschiebt clefchange vom Ende des Taktes vor  die erste Note dwes folgenden Taktes*)
	
              measures[measure + 1].clefchange[ps] := Note.clefchanged;
              

              END; 
				(* NoteOut(Note); *)
		END;   (* 16*) *)
			
(*		Out.Ln();	Out.String("pmxnote vorher");Out.String(Note.pmxnote); *)
		
				COPY( pmxnote, Note.pmxnote );  WriteString( W, Note.pmxnote );  

(*			IF Note.grace >1 THEN b.loesch(Note.pmxnote); 
			b.loesch(Note.pmxgrace) END;  			Out.Ln(); Out.String(" Note.pmxnote" ); Out.Int(measure,5); Out.String( Note.pmxnote );
			Out.String( Note.pmxgrace ); Out.Char("|"); Out.Int (Note.grace,5); *)
 
			
			(* Out.Int( Note.staff, 5 );  Out.Char( "|" );  
		Out.Int( Note.voice, 5 );  *)
	
		ELSIF (Note.rest = "r") THEN  (* Branch for Rests *)  (* 1*)
			
			INC( count );   (* this note is a rest! *)
			
			istuplet := Note.actual # 0;  

			
			PMXDuration( Note.tuplet, Note.tremolotype, attributes[part].divisions,  Note.duration, Note.actual, Note.normal, 
							pmxdur, istuplet, Note.type );  
										pmxnote[0] := 20X;  
		IF ( Note.blind = "b" )	THEN 						
			
			Complete.CalcForward(Note.duration,attributes[part].divisions,blindrest); 
			
			Strings.Append(pmxnote,blindrest); 
		ELSE

			IF (Note.duration = attributes[part].duration) THEN  (* rest of type "rp" *)  (* 17*)
						Strings.Append( pmxnote, "rp" );  
			ELSE  (* 17*)
				Strings.AppendCh( pmxnote, Note.rest );  Strings.Append( pmxnote, pmxdur );  
				IF (Note.tuplet = "start") & (Note.actual # 0)  (* leading note of tuplet *)
					THEN  (* 18*)
					
					Strings.AppendCh( pmxnote, "x" ); Strings.IntToStr(Note.actual,sactual);
					 Strings.Append( pmxnote, sactual );  
				END;   (* 18*)
			
				
			END;   (* 17 *)
			(************************************************ first try for slur ending on rest ****************)
			IF (Note.slur[0,1] = ")") 
					THEN  (* 13*)
							Strings.Append( pmxnote, Note.slur[0] );  
					END;
				IF (Note.slur[1, 1] = ")") 
					THEN 
						Strings.Append( pmxnote, Note.slur[1] ); 
					END;

		(*	END;    13*)   

		
END;
	
			
			Strings.AppendCh( pmxnote, BLANK );  
		(*	IF (Note.clefchanged # 0X) THEN  (* 16*) (* clef change on the fly, activated 14. Maerz 2018 *)
				(*	IF note < maxnote[part,staff,measure] THEN *)
				Strings.Append( pmxnote, " C" );  Strings.AppendCh( pmxnote, Note.clefchanged );  
				Strings.AppendCh( pmxnote, BLANK );  
				Out.Ln(); Out.Int(ps,5); Out.Int(voice,5); Out.Int(measure,5); Out.Int(note,5);
				Out.String( "cleffy" );  Out.Char( Note.clefchanged );  Out.Char( Note.clef ); 
				(* NoteOut(Note); *)
			END;   (* 16*)  *)
			
			COPY( pmxnote, Note.pmxnote );  WriteString( W, pmxnote );  
		
		END;   (*1*)
		(*	IF (Note.direction > 0) THEN  (* 19*)
		
		(* OutDir( Note.direction );  *)
		 Files.Write( W, Note.direction );  Files.Write( W, CR );  Files.Write( W, NL );  
	END;   (* 19*) *)
		(*  directions within measures are written *)
		COPY( notes[ps, voice, measure, note].righttext, Rtext );  
	(*	 Out.Ln(); Out.String("WriteNote2PMX : "); Out.Int(ps,5); Out.Int(voice,5); Out.Int(measure,5); Out.Int(note,5); Out.String (Rtext); 
		
		Out.Int(notes[ps, voice, measure, note].grace,5); *)
		
		IF (Rtext[0] # 0X) (* & ( ~written ) *) THEN WriteString( W, Rtext );  
		END;  
		IF (Note.rbright[0] # 0X) THEN WriteString( W, Note.rbright );  END;  
		(*	Out.Ln();Out.String("measure,ps,voice,note");
		Out.Int(measure,5); Out.Int(ps,5); Out.Int(voice,5); Out.Int(note,5); Out.Int(Note.voice,5);Out.String(pmxnote); *)
			(* "maxnote" is the last note in part/staff/measure, minnote is the first note in part/staff/measure *)
		(*	IF ( note = maxnote[part, staff, measure]  )*)  (* Aenderung 22.12.2015: introduce voice dependence *)
		(* "maxnote" is the last note in part/staff/measure, minnote is the first note in part/staff/measure *)
		IF (note = maxnote[part, staff, measure]) OR ((note = maxnote0[part, staff, measure]) & (voice = 0)) OR 
		    ((note = maxnote1[part, staff, measure]) & (voice = 1))

		THEN 
		(*	WriteString( W, " | " );  not needed in PMX *)
			IF (ps = nostaves - 1) & (voice = voicefrom) THEN 
				IF (measures[measure].barstyle # 0X) THEN WriteString( W, measures[measure].barstyle ) END;  
				IF (measures[measure].repeat = " Rr ") THEN 
					Files.Write( W, CR );  Files.Write( W, NL );  WriteString( W, measures[measure].repeat );  
				END;  
			END;  
		
					Files.Write(W,BLANK); Files.Write( W, "/" );     (* Print "/" for  end of single  voice  . *)
		
		END;  
		IF Note.tuplet = "stop" THEN istuplet := FALSE END;  
	ELSE 
	Out.Ln(); Out.String("grace = -1"); Out.Int(ps,5); Out.Int(measure,5); Out.Int(note,5);END;
	END WriteNote2PMX;  


	


	PROCEDURE WritePMX ( VAR W: Files.Rider );  
	(* Creates and stores the notes part of the PMX file (starting with "% Bar 1"). after the Control Data *)
	VAR voice, staff, voicefrom, voiceto, nnotes, notefrom, noteto : LONGINT; concertkey : ARRAY 10 OF CHAR; 
		Dtext, Rtext: ARRAY 128 OF CHAR;  
		keychange, dummy: ARRAY 32 OF CHAR; keychanged : BOOLEAN; 		
		blindmeterchange: ARRAY 16 OF CHAR;  
		istuplet: BOOLEAN;  minmeasure: LONGINT;  ipickup: INTEGER;  
		(* decides whether direction is written before or after the note. *)
		restbefore, restafter: ARRAY 16 OF CHAR;  
	
	BEGIN 
		ipickup := 0; keychanged := FALSE;
		IF attributes[1].pickup > 0 THEN ipickup := 1;  END;  
		(* Write transposition string s *)
	(*	part := 1;  
		WHILE part <= maxpart DO 
			IF attributes[part].diatonic # 0 THEN WriteString( W, attributes[part].kstring ); 
			END;  
			Files.Write( W, CR );  Files.Write( W, NL );  INC( part );  Out.Ln(); Out.String("diatonic");
		END;  *)
		Files.Write( W, CR );  Files.Write( W, NL );  

		measure := 1;  note := 1;  
		
		IF (attributes[1].pickup > 0) THEN minmeasure := 1
		ELSE minmeasure := 0
		END;   (* do not extend 1st measure in case of pickup. *)
		IF ( uptomeasure > 0 ) & ( uptomeasure <= maxmeasure ) THEN maxmeasure := uptomeasure; END;
		WHILE measure <= maxmeasure DO 
		(*	Out.Ln(); Out.String("measure ="); Out.Int(measure,5); *)

		
		
			Complete.erasetime( nostaves, voicetime );  ps := nostaves - 1;  
			IF maxpart > 1 THEN  
	
			
			IF ( measure = 1 )  OR ( ( measure > 1 ) & ( measures[measure].fifth # measures[measure-1].fifth ))
			THEN 		
			
				keychanged := TRUE;  
			ELSIF measure > 1 THEN	i := 1 ; 
							REPEAT  
			 					 	 	 keychanged := (measures[measure].keys[i] # measures[measure].keys[maxpart] ) &  (* Aenderung2.2.2017 *)
					                         (measures[measure].keys[i] # measures[measure - 1].keys[i]);
															INC(i) ;  
				UNTIL ( i = maxpart ) OR keychanged  ; 
				END;
	(*		END; *)
					IF  (keychanged & ~ pmxcontrol.equalkeys) THEN 
							 		i := 1; WHILE i <=maxpart DO b.mkeys[i] :=  measures[measure].keys[i]; INC(i) END;
									b.testmakekey(maxpart,measure,keytotal); 
			           COPY("K+0",concertkey); 
  			             
			               IF measures[measure].keys[maxpart] >= 0 THEN Strings.AppendCh(concertkey,"+"); END;
			              Strings.IntToStr(measures[measure].keys[maxpart],dummy); Strings.Append(concertkey,dummy);
				           IF measure >= 1 THEN
				           WriteString( W, concertkey );   Files.Write( W, CR );  
							Files.Write( W, NL );   END;

			
			   			WriteString( W, keytotal );  Files.Write( W, CR );  
							Files.Write( W, NL );  END; 
			
			(*   Out.Ln(); Out.String("concertkey : "); Out.String(concertkey);
			   Out.Ln(); Out.String("keytotal : "); Out.String(keytotal); *)
   END;  
			
			WHILE (ps >= 0) DO 
				part := partstaff[ps, 0];  staff := partstaff[ps, 1];
				
							
				
				
				(* neuer Code: shift to voice := 0,1   *)
				voiceto := voicelimm[ps, measure] - 1;  voicefrom := 0; voice := voicefrom;   
			(*	IF voiceto < 0 THEN voiceto := 0; END; *)
				 
				  (*	Out.Ln();			Out.String("measure, part, staff, voicefrom, voiceto:");
				  	
				  	Out.Int(measure,5);				Out.Int(part,5);  Out.Int(staff,5);
				  	
				  	Out.Int(voicefrom,5); Out.Int(voiceto,5); *)
				   
				     
			(*	IF voiceto > 1 THEN voiceto := 1 END; (* this line only for testing BWV0826; to be removed. *) *)
				WHILE voice <= voiceto DO 
					nnotes := 0;  WriteString( W, "% " );  Files.Write( W, "(" );  WriteLInt( W, part );  
					Files.Write( W, "|" );  WriteLInt( W, staff );  Files.Write( W, "|" );  WriteLInt( W, vmap[ps, measure, voice] );  
					Files.Write( W, ")" );    WriteLInt( W, measure - ipickup );  Files.Write( W, CR );  
					Files.Write( W, NL );  


									
					
					b.loesch( Dtext );  b.loesch( Rtext );  
					(**********************  Meter Change *******************)
					IF (measure > 1) THEN 
						IF (measures[measure].beats # measures[measure - 1].beats) OR 
						    (measures[measure].beattype # measures[measure - 1].beattype) THEN 
						    IF measures[measure].beattype >0 THEN
							b.NewBeat( measures[measure].beats, measures[measure].beattype, 
												 measures[measure].meterchange, FALSE );  
					                                                                                           (* 20-10-2020 Lilypond *)
								attributes[part].duration := 
								measures[measure].beats*4* attributes[part].divisions  DIV measures[measure].beattype;  
								ELSE 
								 Out.Ln(); 
								 Out.String("PROC. WritePMX: measure = "); 
								 Out.Int(measure,5);
								  END; 

							(*	measures[measure].divisions[part] *)  					
									measures[measure].duration[part] := attributes[part].duration ; 
							IF (ps = nostaves - 1) & (voice = voicefrom) THEN 
								Files.Write( W, CR );  Files.Write( W, NL );  WriteString( W, measures[measure].meterchange );  
								Files.Write( W, CR );  Files.Write( W, NL );  
							END 
						END;  
					END;  
					(*	 blind meter change in case of Pickup.  *)
					IF (attributes[1].pickup > 0) & (measure = 2) & (ps = nostaves - 1) & (voice = voicefrom) THEN (* Change  09.05.2020 *)
						b.NewBeat( attributes[1].beats, attributes[1].beattype, blindmeterchange, TRUE );  

						WriteString( W, blindmeterchange );  Files.Write( W, CR );  Files.Write( W, NL );  
					END;  
					
					
					(********************** key change ***********************)
					IF pmxcontrol.equalkeys &(ps = nostaves - 1) & (measure > 1) & (voice = voicefrom) & (* change 03052018 *)
					    (measures[measure].fifth # measures[measure - 1].fifth) THEN 
						COPY( "K+0", keychange );  
						IF ( measures[measure].fifth >= 0 ) THEN Strings.Append(keychange,"+"); END;
						Strings.IntToStr( measures[measure].fifth, dummy );  
						Strings.Append( keychange, dummy );  WriteString( W, keychange );  Files.Write( W, CR );  
						Files.Write( W, NL );  
					END;  
					
					IF (measures[measure].ending # 0X) & (ps = nostaves - 1) THEN 
						WriteString( W, measures[measure].ending );  
					END;  
					
					IF (measures[measure].repeat = " Rl ") & (ps = nostaves - 1) & (voice = voicefrom) THEN 
						WriteString( W, measures[measure].repeat );  
					END;  
					notefrom := minnote0[part, staff, measure];  noteto := maxnote0[part, staff, measure];  
			(*		IF measure = 25 THEN Out.Ln(); Out.String("voice = 0");
						Out.Ln(); Out.String("notefrom(to : "); Out.Int(notefrom,5); Out.Int(noteto,5); END; *)

					IF (voice = 1) THEN 
						notefrom := minnote1[part, staff, measure];  
						noteto := maxnote1[part, staff, measure];   (* changed from note := 1;  22.12.2015 *)
			(*			IF measure = 25 THEN Out.Ln(); Out.String("voice = 1");
						Out.Ln(); Out.String("notefrom(to : "); Out.Int(notefrom,5); Out.Int(noteto,5); END; *)
					END;  
					note := notefrom;  
					
					IF (measure > minmeasure) THEN FillRests( notefrom, noteto, measure, ps, voice );  
					END;  
					
					(* do not extend 1st measure in case of pickup. *)
					
					WHILE note <= noteto DO  (* 1 *)
					IF (notes[ps, voice, measure, note] # NIL) &
						 (Strings.IsAlpha( notes[ps, voice, measure, note].pitchstep ) OR 	 (notes[ps, voice, measure, note].rest = "r")) 
						(*  note OR rest *)
											  

							   (* remove print-object="no". 26.07.2019*)
							  THEN (* CHANGE 11.Juli 2019: remove print-object = "no" notes *)
									IF ~ (10 IN outputset) OR ( notes[ps, voice, measure, note].probj = TRUE ) THEN 
													(* Option "r" :  remove notes with print_obj = "no"  04.05.2020 *) 
						
						
							WriteNote2PMX( W, notes[ps, voice, measure, note]^, ps, voice, voicefrom, measure, note, Dtext, 
														 Rtext, istuplet );  
							b.loesch( restbefore );  b.loesch( restafter );  

							INC( nnotes );  
							
							(* b.loesch(Rtext); b.loesch(Dtext); *)
							END; 
							
						END;  
						INC( note ); 	(* IF voice = 1 THEN Out.Ln(); Out.String("Count notes of 2nd voice. ");Out.Int(note,5);				END; *)
					END;   (* 1 *)  (* Loop over notes *) 

					(* Measure Properties go here *) ;  
					(*	WriteString(W," | /");  *)
					IF ( voicelimm[ps,measure] = 2 ) & (voice =  0) & (nnotes > 0) 
				  
				   &  (	((notes[ps,0,measure,minnote1[part,staff,measure]] # NIL)
				  & (notes[ps, 0, measure, minnote1[part,staff,measure]].probj = TRUE))
				  OR ~ ( 10 IN outputset ) )
					(* second slash removed because second voice eliminated. Change 04.05.2020 *)
		 			THEN 
								 
					Files.Write( W, "/" );  END;   (*  Print  second "/" for first  voice of two voices *)
					Files.Write( W, CR );  Files.Write( W, NL );  

					INC( voice );  

				END;   (* Loop over voices *)

				DEC( ps );  
			END;  
			INC( measure );  
		END; (* Out.Ln(); Out.String(" Divisions : ");
		i := 1; WHILE i <= maxpart DO Out.Ln(); Out.Int(i,5); Out.Int(attributes[i].divisions,5); INC(i);END;  *)
	END WritePMX;
	
(*	PROCEDURE ListMeasures*;
	VAR i,j : LONGINT;
	BEGIN
	Out.Ln(); Out.String("ListMeasures");
	i := 1; 
	WHILE i <= maxmeasure DO
	
		Out.Ln(); Out.Int(i, 5); Out.Int( measures[i].beats, 5); Out.Int(measures[i].beattype,5); Out.Int ( measures[i].fifth , 5); 
		j := 1;  
	
			WHILE j<=maxpart DO  Out.Int(measures[i].keys[j],5); INC(j); END;  INC(i); END;
	
	
	(*	j := 1;  WHILE j<maxpart DO  Out.Int(measures[i].duration[j],5); INC(j); END;  INC(i); END; *)

	
	
	END ListMeasures; *)
  




	
	PROCEDURE repeat2PMX( n: b.Tag;  VAR pmxrepeat: ARRAY OF CHAR );  
	(* translates a left or right repeat from XML to PMX *)
	VAR c: CHAR;  
		direction: ARRAY 32 OF CHAR;  
	BEGIN 
		b.loesch( pmxrepeat );  COPY( BLANK, pmxrepeat );  b.FindAtt( n, "direction", direction );  
		IF (direction = "backward") THEN Strings.Append( pmxrepeat, "Rr" );  
		ELSIF (direction = "forward") THEN Strings.Append( pmxrepeat, "Rl" );  
		END;  
		
		Strings.AppendCh( pmxrepeat, BLANK );  c := 0X;  Strings.AppendCh( pmxrepeat, c );  
	
	END repeat2PMX;  

	PROCEDURE ending2PMX( n: b.Tag;  VAR pmxending: ARRAY OF CHAR;  VAR type: ARRAY OF CHAR );  
	(* translates a Volta  from XML to PMX, possibly not general enough *)
	VAR c: CHAR;  
		number: ARRAY 32 OF CHAR;  
	BEGIN 
		COPY( BLANK, pmxending );  b.FindAtt( n, "number", number );  b.FindAtt( n, "type", type );  
		IF (type = "start") THEN 
			Strings.AppendCh( pmxending, "V" );  Strings.Append( pmxending, number );  
			IF (number = "2") THEN Strings.AppendCh( pmxending, "b" );  END;  
		END;  
		IF (type = "discontinue") THEN COPY ("Vx",pmxending); END;
		
		Strings.AppendCh( pmxending, BLANK );  c := 0X;  Strings.AppendCh( pmxending, c );  
	
	END ending2PMX;  

	PROCEDURE beam2PMX( n: b.Tag;  VAR pmxbeam: ARRAY OF CHAR;  stem: CHAR;  staff: LONGINT );  
	(* Translates a beginning or ending or continued beam from XML to PMX. staff is the staff of the beam element *)
	VAR c, j, stemj: CHAR;  m: b.Tag;  
		type, number: ARRAY 32 OF CHAR;  
	BEGIN 
		b.loesch( pmxbeam );  COPY( n.between, type );  b.FindAtt( n, "number", number );  b.loesch( closebeam );  j := 0X;  
		IF (number = "1") THEN  (* 1*)
			COPY( BLANK, pmxbeam );  
			IF (type = "begin") THEN c := "[";  
			ELSIF (type = "end") THEN 
				IF (lasttype = "continue") & (laststaff # staff) THEN
						 	c := "[";  j := "j";  COPY( " ] ", closebeam );  
				ELSE c := "]";  END;  
			ELSIF (type = "continue") THEN 
				IF (lasttype = "continue") & (laststaff # staff) THEN 
					c := "[";  j := "j";  
				ELSE 

					b.findnextnote( n, m );   (* Out.Char("|"); Out.Int(n.nr,5); Out.Int(staff,5); Out.Char("|");Out.Int(m.nr,5);Out.Int(m.staff,5); *)
					(*	Out.Ln(); Out.String(" beam : continue "); *)
					IF (m.staff # staff) THEN j := "j";  c := "]" ELSE c := "?" END;  
				END;  
                        ELSE c := "?"
			END;  
			Strings.AppendCh( pmxbeam, c );  
			IF (j = "j") (* neuer Code fuer joined beams *)
			THEN 
				Strings.AppendCh( pmxbeam, j );  
				IF c = "[" THEN Strings.AppendCh( pmxbeam, "f" );  (* flip l/u *)
				
					stemj := "u"; IF stem = "u" THEN stemj := "l"; END;  
					Strings.AppendCh( pmxbeam, stemj ); Strings.AppendCh( pmxbeam, BLANK );  END;  
				(*  Out.String("pmxbeam");  Out.String(pmxbeam); Out.Char("|"); *)
			ELSE 
				IF (c = "[") THEN 
					IF (stem = "d") THEN Strings.AppendCh( pmxbeam, "l" );  
					ELSIF (stem = "u") THEN Strings.AppendCh( pmxbeam, "u" );  
					END;  
					(*	 IF (stem = "u") THEN Strings.Append(pmxbeam,"+2");  
			ELSIF (stem = "d") THEN Strings.Append(pmxbeam,"-2"); END; *)
					Strings.AppendCh( pmxbeam, BLANK );  
				END;  
			END;  
		END;  
		IF (type = "continue") & (j # "j") OR (number > "1") THEN b.loesch( pmxbeam );  END;  
		COPY( type, lasttype );  laststaff := staff;  
	END beam2PMX;  

(*	PROCEDURE tied2PMX( n: b.Tag;  type: ARRAY OF CHAR;  VAR pmxtied: ARRAY OF CHAR;  voice: LONGINT);  
	(* Translates a beginning or ending tie from XML to PMX.
	    orientation = "d" => note with stem down yields tie with orientation "u".
	 orientation = "u" => note with stem up yields tie with orientation "l". *)
	VAR c: CHAR;  
		number: ARRAY 32 OF CHAR;  orient : ARRAY 10 OF CHAR; orientation : CHAR;
		nt: Fifo.Node;  
	
	BEGIN 
		b.loesch( pmxtied );  b.FindAtt( n, "number", number );
		b.FindAtt( n, "orientation", orient );  
		IF orient = "over" THEN orientation := "u";   (* IF orient = "" THEN follow stem direction *)
		ELSIF orient =  "under" THEN orientation := "l";  
		ELSE orientation := " ";
		END;  
		
		(* Out.Char("|"); Out.String("tied : "); Out.Char("|");Out.String(type);Out.Char("|");
		Out.String(number);Out.Char("|");Out.Char(orientation); *)
		COPY( BLANK, pmxtied );  
		IF (type = "start") THEN 
			c := "{";  
			Strings.AppendCh( pmxtied, c );  
			NEW( nt );  nt.key := Fifo.smallfree( b.tieunusdnum[ps, voice] );  
			EXCL( b.tieunusdnum[ps, voice], nt.key );  
			Fifo.Enqueue( b.tieq[ps, voice], nt );  
			Strings.IntToStr( nt.key, number );  
			Strings.Append( pmxtied, number );  
			Strings.AppendCh( pmxtied, orientation );  
		ELSIF (type = "stop") THEN c := "}";  Strings.AppendCh( pmxtied, c );  

			nt := Fifo.DequeuedNode( b.tieq[ps, voice] ); 
			IF nt # NIL THEN INCL( b.tieunusdnum[ps, voice], nt.key );  (* avoid nt undefined *)
			Strings.IntToStr( nt.key, number );  Strings.Append( pmxtied, number );  END;
		ELSE Out.String( "wrong type of tie " );  
		END;  
		Strings.AppendCh( pmxtied, BLANK );  
	END tied2PMX;  *)

(*	PROCEDURE slur2PMX( n: b.Tag;  VAR pmxslur: ARRAY OF CHAR );  
	(* Translates a beginning or ending slur from XML to PMX. *)
	
	VAR c, cs : CHAR;  
		type, number, placement: ARRAY 32 OF CHAR;  inumber : LONGINT; res : ARRAY 4 OF CHAR;
	BEGIN 
		b.loesch( pmxslur );  b.FindAtt( n, "type", type );  b.FindAtt( n, "number", number );  
		IF ( number # "" ) THEN
				Strings.StrToInt(number, inumber);
				cs := CHR(inumber + 65)
				ELSE cs := "A"; 
		END;
		b.FindAtt( n, "placement", placement );  
		IF (type # "continue") THEN (* gibt es das ueberhapt? *)

			COPY( BLANK, pmxslur );  
			IF (type = "start") THEN c := "(";  
			ELSIF (type = "stop") THEN c := ")"
			END;  
			Strings.AppendCh( pmxslur, c );  
		Strings.AppendCh( pmxslur, cs );  
		(* Achtung x-option for staff crossing slur *)
		IF 8 IN outputset THEN Strings.Append(pmxslur,"x"); END;
		
			IF (type = "start") THEN (* gibt es slurs ohne placement Angabe? *)
				IF (placement = "below") THEN Strings.AppendCh( pmxslur, "l" )
				ELSIF (placement = "above") THEN Strings.AppendCh( pmxslur, "u" );  
				END; 
			END;  
			Strings.AppendCh( pmxslur, BLANK );  c := 0X;  Strings.AppendCh( pmxslur, c );  

		END;  
		(* Out.Ln();  Out.String( "slur2pmx : " );  Out.String( pmxslur );  *)
		  
				(* Slur control  according to grace property *)
			
	END slur2PMX;  *)

	PROCEDURE grace( n: b.Tag;  ps, voice, measure, note, maxgrace: LONGINT;  VAR res: ARRAY OF CHAR );  
	(* counts the calls in "ingrace2pmx", prerequisite n.tagname = notetag  *)
	
	VAR nograce: LONGINT;  isgrace: BOOLEAN;  
		slash, type, stem: ARRAY 32 OF CHAR;  
		snograce,str,accpmx: ARRAY 4 OF CHAR;  ns : CHAR; no : LONGINT;
	BEGIN 
                isgrace := FALSE;
		b.loesch( res );  
		WHILE (n.next # NIL ) & (n.tagname # eonotetag) DO  (* loop over all tags of the note 1 *)
			(*	Out.Ln();  Out.String( n.tagname );  *)
			
			IF (n.tagname = gracetag) OR (n.tagname = gracetag2) THEN  (* 2 *)
				b.FindAtt( n, "slash", slash );  nograce := 0;  isgrace := TRUE;  
				
				(*	Out.Ln();  Out.String( " ***********************grace : " );  Out.String( slash );  *)
			END;  
			
			IF (n.tagname) = steptag THEN  (* 3 *)
				notes[ps, voice, measure, note].pitchstep := Strings.LowerCh( n.between[0] );  (* Out.String( n.between );  *)
			END;  
			
			IF (n.tagname) = octavetag THEN  (* 4 *)
				notes[ps, voice, measure, note].pitchoctave := b.ExtractInt( n.between );  (* Out.String( n.between );  *)
			END;  
			Accidentals(n,ps,voice,measure,note); 
			(* Out.Ln(); Out.String("note1 accidental"); Out.String(notes[ps, voice, measure, note].accidental); *)
			IF (n.tagname = "<type>") THEN 
				COPY( n.between, type ); (* Out.String( n.between ); *)   (* 6 *)
			END;  
			
			IF (n.tagname = "stem") THEN  (* 7 *)
				COPY( n.between, stem );   (* Out.Ln();  Out.String( n.between );  *)
			END;  
			
					
			n := n.next;  
		END;   (* 1 *) (* Daten der ersten Grace-Note *)
		
		
		nograce := maxgrace;  (* Out.Ln();  Out.String( "grace : n#gracenotes" );  Out.Int( nograce, 5 ); *)  res[0] := BLANK;  

		Strings.IntToStr( nograce, snograce);  res[0] := "G";  res[1] := 0X;  Strings.Append( res, snograce );  
		Strings.AppendCh( res, "m" );  
		IF (type = "eighth") THEN Strings.AppendCh( res, "1" )
		ELSIF (type = "16th") THEN Strings.AppendCh( res, "2" )
		ELSIF (type = "32nd") THEN Strings.AppendCh( res, "3" )
		END;  
		IF ( 7 IN outputset) THEN Strings.AppendCh( res, "s" ); END;  (* slur internal to grace note, goes from first grace note to first non-grace note *)
		IF (slash = "yes") THEN Strings.Append( res, "x" );  END;  
		IF (stem = "up") THEN Strings.AppendCh( res, "u" )
		ELSIF (stem = "down") THEN Strings.AppendCh( res, "l" )
		END;  
		
		Strings.AppendCh( res, notes[ps, voice, measure, note].pitchstep );  
		Strings.IntToStr( notes[ps, voice, measure, note].pitchoctave, str );  Strings.Append( res, str );  
	 Strings.Append(res,notes[ps,voice,measure,note].accidental); 	Strings.AppendCh( res, BLANK ); Strings.AppendCh( res, 0X ); 
	 (* 1st gracenote and config *)
		 n := n.next;	
		 (* b.OutTag(n,TRUE); *)
				
			WHILE (nograce >1) & isgrace DO 
		(*	Out.Ln(); Out.Int(nograce,5); *)
				Findnextgrace (n,isgrace,ns,no,accpmx);
         Strings.AppendCh(res,BLANK); Strings.AppendCh(res,ns);
		 Strings.IntToStr( no,str); Strings.Append(res,str); IF(accpmx[0] # 0X) THEN Strings.Append(res,accpmx) END;
		 n := n.next; DEC(nograce); (*	Out.Ln(); Out.String(res);  %%%%% *)
		 END;	 
		
	
	END grace; 
	PROCEDURE Findnextgrace* (VAR n : b.Tag; VAR isgrace : BOOLEAN; VAR notestep : CHAR; 
																		VAR noteoctave : LONGINT; VAR accpmx : ARRAY OF CHAR );
		(* Neu 14.11.2020: Anbindung der weiteren grace-Noten an Hauptnote 
		prerequisite n ist zweite Note der Verzierung *) 

	BEGIN
	notestep := 0X; noteoctave := -1; accpmx[0] := 0X; isgrace := FALSE;
		WHILE ( n.next # NIL ) & (n.tagname # eonotetag) DO
			IF n.tagname = gracetag THEN  isgrace := TRUE; END;
			IF n.tagname = steptag THEN notestep := Strings.LowerCh(n.between[0]);  END;
			IF n.tagname = octavetag THEN noteoctave := b.ExtractInt(n.between);  END;
			IF n.tagname = "<accidental>" THEN 
				IF( n.between = "sharp") THEN  COPY( "s", accpmx ); 	END;							
				IF ( n.between = "flat") THEN 		COPY( "f", accpmx ); END; 					 					
				IF (n.between = "double-sharp") OR (n.between = "sharp-sharp") THEN COPY( "ss", accpmx ); END;
				IF (n.between = "flat-flat") OR (n.between = "double-flat") THEN COPY( "ff", accpmx ); END;
				IF (n.between = "natural") THEN COPY( "n", accpmx );  END;
			END;
						  
		
					
			n:= n.next; 
		END; (* WHILE *)
		(*	 Out.Ln(); Out.String("Findnextgrace");  Out.Int(measure,5);
							 Out.Char(notestep); Out.Int(noteoctave,5);Out.String(accpmx); *)
END Findnextgrace;	
	
	
	 
	PROCEDURE OutDir( i, j: LONGINT );  
	BEGIN 
		Out.Ln();  Out.String( " OutDir : " );  Out.Int( i, 5 );  Out.Int( j, 5 );  Out.Int( directions[i, 0].part, 5 );  
		Out.Int( directions[i, 0].staff, 5 );  Out.Int( directions[i, 0].measure, 5 );  Out.Char( "|" );  
		Out.Int( directions[i, 0].lastnote, 5 );  		Out.Char( "|" );  	Out.Int( directions[i, 0].voice, 5 );  		Out.Char( "|" );
		Out.Int( directions[i, 0].note, 5 );  Out.Char( "|" );  
		Out.Char( directions[i, j].dirtype );  Out.Char( "|" );  Out.Char( directions[i, j].placement );  Out.Char( "|" );  
		Out.String( directions[i, j].wedgetype );  Out.Char( "|" );  Out.String( directions[i, j].dyntype );  Out.Char( "|" );  
		Out.String( directions[i, j].pedaltype );  Out.Char( "|" );  Out.String( directions[i, j].text );  Out.Char( "|" );  
		Out.String( directions[i, j].pmxdirection );  Out.Char( "|" );  Out.Int( directions[i, j].defaultx, 5 );  
		(* IF directions[i,j].used THEN Out.String(" direction used "); ELSE  Out.String(" direction not used ") END; *)
	END OutDir;  

	PROCEDURE NotesProp( part, staff, voice, measure: LONGINT;  VAR note: LONGINT;  VAR n: b.Tag );  
	(* stores notes information in an array with indices [ps,voice,measure,note] for later use in the generation measure 
		by measure; "part" and "staff" are combined in one index "ps". *)
	VAR pmxslur, pmxtied, pmxbeam, type, pmxrepeat, pmxgrace : ARRAY 32 OF CHAR;  
		number, ntype, mtype: ARRAY 32 OF CHAR;  placement : ARRAY 32 OF CHAR;
		m: b.Tag;  
		defaultxs: ARRAY 32 OF CHAR;  (* openslur : ARRAY 1000 OF OpenSlurDesc; *)
		ps, defaultx, long : LONGINT;  
	
	BEGIN 
		nties := 0;  ps := linstaff( nostaves, part, staff );  
		 NEW( notes[ps, voice, measure, note] ); 
		notes[ps, voice, measure, note].voicetime := n.voicetime;  
		notes[ps, voice, measure, note].cue := n.cue;
		notes[ps, voice, measure, note].from := n.from;  notes[ps, voice, measure, note].to := n.to;  
		(*	Out.Ln(); Out.String("NotesProp voice + voice : ");
		Out.Int(part,5);Out.Int(staff,5);Out.Int(measure,5);Out.Int(voice,5); Out.Int(n.voice,5); 
		Out.Int(note,5);Out.Int(n.from,5);Out.Int(n.to,5); Out.Int(n.voicetime,5); *)
		IF voice = 0 THEN 
			maxnote0[part, staff, measure] := b.Max( note, maxnote0[part, staff, measure] );  
			minnote0[part, staff, measure] := b.Min( note, minnote0[part, staff, measure] );  
		END;  
		IF voice = 1 THEN 
			maxnote1[part, staff, measure] := b.Max( note, maxnote1[part, staff, measure] );  
			minnote1[part, staff, measure] := b.Min( note, minnote1[part, staff, measure] );  
		END;  
		IF (n.probj = "n") THEN notes[ps, voice, measure, note].probj := FALSE
		 ELSE  notes[ps, voice, measure, note].probj :=TRUE; END;
		notes[ps, voice, measure, note].clef := lastclef[ps];  
		IF ( n.newclef # 0X ) THEN 
		notes[ps,voice, measure, note].clefchanged := n.newclef; (* 17.10.2020 new Implemtation od clef *)
		(* Out.Ln(); Out.String("in NotesProp : "); Out.Char(n.newclef); *)
		END;
		b.FindAtt( n, "default-x", defaultxs );  Strings.StrToInt( defaultxs, defaultx );  
		notes[ps, voice, measure, note].defaultx := SHORT( defaultx );  notes[ps, voice, measure, note].from := n.from;  
		notes[ps, voice, measure, note].to := n.to;  
		
		(* clef change on bar change 
			lastnotethismeasure := maxnote0[part,staff,measure];
			IF voice = 1 THEN lastnotethismeasure := maxnote1[part,staff,measure] END;
			firstnotenextmeasure := minnote0[part,staff,measure+1];
			IF voice = 1 THEN firstnotenextmeasure := minnote1[part,staff,measure+1] END;
			IF	( notes[ps,voice,measure,lastnotethismeasure].clefchanged # 0X ) 
			&  ( measure < maxmeasure )THEN

			notes[ps,voice,measure+1,firstnotenextmeasure].clefchanged :=
					notes[ps,voice,measure,lastnotethismeasure].clefchanged; END;

			Ende: clcef change on bar change *)

		
		
		
		
		
		WHILE (n.next # NIL ) & (n.tagname # eonotetag) DO 
			
			(*	OutTag( n );  &&&&&&&&&&&&&&&&&&& Alarm arpeggio *)
	(*		IF (n.arpeggio > 0) THEN notes[ps, voice, measure, note].arpeggio := n.arpeggio;  END;  
			IF (n.arpeggio = 1) THEN 
				b.arplen( n, maxarp );  notes[ps, voice, measure, note].maxarp := SHORT( maxarp );  Out.Ln();  
				Out.String( "maxarp" );  Out.Int( maxarp, 5 );  
			END;  *)
			IF (n.grace > 0) THEN notes[ps, voice, measure, note].grace := n.grace;  
			(* Out.Ln(); Out.String(" notesprop : "); Out.Int(ps,5); Out.Int(voice,5); Out.Int(measure,5); Out.Int(note,5); Out.Int(n.grace,5); *)
			END;  
			IF (n.grace > 0 ) & ( n.chord ="c" ) THEN notes[ps, voice, measure, note].grace :=-1 END;
			(* eliminate chord notes in grace *)
			IF (n.grace = 1) THEN b.gracelen( n, maxgrace );  (* Out.Ln();  Out.String( "maxgrace" );  Out.Int( maxgrace, 5 );  *)

				grace( n, ps, voice, measure, note, maxgrace, pmxgrace );  
				
		(*			Out.Ln();  Out.String( "NotesProp nach PROC grace : " );  Out.Int( ps, 5) ;   Out.Int( part, 5 );  
				Out.Int( staff, 5 );  Out.Int( voice, 5 );  Out.Int( measure, 5 );  Out.Int( note, 5 );
				 Out.Int(notes[ps, voice, measure, note].grace,5); Out.Char("|"); *)
				  
				
				COPY( pmxgrace, notes[ps, voice, measure, note].pmxgrace );  

	(*			Out.String( notes[ps, voice, measure, note].pmxgrace );  *)
			 


				
				(*		ELSIF n.grace > 1 THEN 
				
				Out.Ln();Out.String("n.grace > 1");Out.Int(ps,5); Out.Int(voice,5); Out.Int(measure,5); Out.Int(note,5); *)
			ELSE  (* not a grace note *)
				
				IF (n.tagname = "<forward>") THEN 
					notes[ps, voice, measure, note].blind := "b";   (* Out.Ln();  Out.String( "NotesProp : <forward>" );  
					Out.Int( part, 5 );  Out.Int( staff, 5 );  Out.Int( voice, 5 );  Out.Int( measure, 5 );  Out.Int( note, 5 );  *)
				END;  
				IF (n.tagname = steptag) THEN notes[ps, voice, measure, note].pitchstep := Strings.LowerCh( n.between[0] ) END;  
				
				IF (n.tagname = octavetag) THEN notes[ps, voice, measure, note].pitchoctave := b.ExtractInt( n.between )
				END;  
				IF (n.tagname = "<unpitched>") THEN 
					n := n.next;  
					IF (n.tagname = "<display-step>") THEN 
						notes[ps, voice, measure, note].pitchstep := Strings.LowerCh( n.between[0] )
					END;  
					n := n.next;  
					IF (n.tagname = "<display-octave>") THEN 
						notes[ps, voice, measure, note].pitchoctave := b.ExtractInt( n.between )
					END;  
				END;  
				IF (n.tagname = durationtag) THEN notes[ps, voice, measure, note].duration := b.ExtractInt( n.between );  END;  
				(*	IF (n.tagname = stafftag) THEN notes[ps, voice, measure, note].staff := b.ExtractInt( n.between ) ;	
				laststaff := notes[ps, voice, measure, note].staff		END;   *)
				(*	IF (n.tagname = voicetag) THEN notes[ps, voice, measure, note].voice := b.ExtractInt( n.between ) 		END;  *)
				IF (n.tagname = dottag) OR (n.tagname = "<dot />") OR (n.tagname = "<dot>") THEN 
					IF notes[ps, voice, measure, note].dot = "d" THEN notes[ps, voice, measure, note].dot := "D";  
					ELSE notes[ps, voice, measure, note].dot := "d"
					END;  
				END;  
				IF (n.tagname) = actualtag THEN 
				Strings.StrToInt(n.between, long);
				
					notes[ps, voice, measure, note].actual := SHORT(long);  
					(*	(Out.Ln(); Out.String("NotesProp .: ps, voice,measure, note, actual "); Out.Int(ps,5); Out.Int(voice,5); 
						Out.Int(measure,5); Out.Int(note,5); Out.Int(notes[ps, voice, measure, note].actual,5); *)
				END;  
				IF (n.tagname  = stemtag ) THEN notes[ps, voice, measure, note].stem := n.between[0]  END;  
				IF (n.tagname  = normaltag ) THEN Strings.StrToInt(n.between, long);
					notes[ps, voice, measure, note].normal := SHORT(long); END;  
				IF (n.tagname = "<type>") THEN COPY( n.between, notes[ps, voice, measure, note].type );  END;  
				
				IF (n.tagname = "<normal-type>") THEN 
					COPY( n.between, notes[ps, voice, measure, note].normaltype );  (* normal-type = type warum? 08.04.2017*)
				(*	IF notes[ps, voice, measure, note].normaltype[0] #0X THEN  *)
					COPY( n.between, notes[ps, voice, measure, note].type ); (*  END; *)
				END; 
		(*			IF (n.tagname = "<actual-type>") THEN 
					COPY( n.between, notes[ps, voice, measure, note].actualtype );  (* Baustelle Korrektur 24062019*)
				(*	IF notes[ps, voice, measure, note].normaltype[0] #0X THEN  *)
					COPY( n.between, notes[ps, voice, measure, note].type ); (*  END; *)
				END;  *)
 
				
				Accidentals (n,ps,voice,measure,note); 
				
							IF (n.tagname = resttag) OR (n.tagname = "<rest>") OR (n.tagname = "</rest>") THEN 
					notes[ps, voice, measure, note].rest := "r";  
				END;  
				(* IF (n.tagname # chordtag) & ( n.tagname # "<chord>" ) THEN  lastbase := SHORT( note ) END;  store main note of a chord *)
				IF (n.tagname = chordtag) OR (n.tagname = "<chord>") THEN notes[ps, voice, measure, note].chord := "z";  
				(*  notes[ps, voice, measure, note].base  := lastbase; *)  END;  
				IF (n.tagname = fermatatag)  THEN  (* keine Fermata im Tuplett. *)
					b.FindAtt( n, "type", type );  COPY( " of", notes[ps, voice, measure, note].fermata );
					  
					IF ( type = "inverted")  THEN Strings.Append( notes[ps, voice, measure, note].fermata, "d " );  END;  
				END;  
				(* prepare for binary tremolo ! *)
				IF (n.tagname = beamtag) THEN  (* counts the beams in one note *)
						b.FindAtt(n, "number",number);
						Strings.StrToInt(number,notes[ps,voice,measure,note].maxbeam)
				END;
				IF ( n.tagname = "<tremolo>" ) (* & (measure = 166) *) THEN
					b.FindAtt(n, "type",notes[ps,voice,measure,note].tremolotype);
					 Out.Ln(); Out.String("tremolotype : "); Out.String(notes[ps,voice,measure,note].tremolotype); 
					Out.Ln(); Out.String("BEams in tremolo : "); Out.String(n.between); (*&&&&&&&*)
					Strings.StrToInt(n.between, notes[ps,voice,measure,note].maxbeam);
					
				END; 

				IF (n.tagname = slurtag)  THEN					
						b.slur2PMX( n, pmxslur , outputset);  	COPY(pmxslur,  notes[ps, voice, measure, note].slur[0] ) ;
					 (*   Out.Ln(); Out.Int(ps,5); Out.Int(voice,5); Out.Int(measure,5); Out.Int(note,5); Out.String(pmxslur);
					    openslur.ps := ps; openslur.voice := voice; openslur.measure := measure; openslur.note := note; 
					    openslur.grace := notes[ps,voice,measure,note].grace;
								Out.Int(notes[ps,voice,measure,note].grace,5);		*)			
						n := n.next;  
						IF (n.tagname = slurtag) THEN
							b.slur2PMX(n,pmxslur, outputset);  COPY(pmxslur,  notes[ps, voice, measure, note].slur[1] ) ;
											(*		Out.Ln(); Out.Int(ps,5); Out.Int(voice,5); Out.Int(measure,5); Out.Int(note,5); Out.String(pmxslur);
								Out.Int(notes[ps,voice,measure,note].grace,5);		*)				
						 
							n := n.next;
						END;
						
				
				END;
							
							
 



(*				END;  *)
				
				IF (n.tagname = tiedtag) THEN  (* 1*)
					b.FindAtt( n, "type", ntype );  
					
					IF ntype = "start" THEN  (* 2 *)
						b.tied2PMX( n, ntype, pmxtied, ps,voice);  
						COPY( pmxtied, notes[ps, voice, measure, note].tied[0] );  
					ELSIF ntype = "stop" THEN 
						m := n.next;  
						IF m.tagname = tiedtag THEN  (* 3 *)
						(*	Out.Ln();  Out.String( "================== found 2nd tie" );  *)

							b.FindAtt( m, "type", mtype );  
							IF mtype = "start" THEN  (* 4 *)
								b.tied2PMX( m, mtype, pmxtied, ps, voice);  
								COPY( pmxtied, notes[ps, voice, measure, note].tied[0] );  
								b.tied2PMX( n, ntype, pmxtied, ps,voice);  
								COPY( pmxtied, notes[ps, voice, measure, note].tied[1] );  
							ELSE 
								Out.Ln();  Out.String( "===========Notesprop: tie inconsistent" );  Out.Int( ps, 5 );  
								Out.Int( voice, 5 );  Out.Int( measure, 5 );  
							END;   (* 4 *)
							
							
						ELSE  (* next Tag is not a tiedtag! *)
							(* ntype = "stop" *)
							b.tied2PMX( n, ntype, pmxtied, ps,voice);  
							COPY( pmxtied, notes[ps, voice, measure, note].tied[1] );  
						END;   (* 3 *)
						IF m.tagname = tiedtag THEN n := n.next END;  
					END;   (* 2 *)
				END;   (*  1 *) 
				IF (n.tagname = beamtag) THEN 
					b.FindAtt( n, "number", number );  
					
					IF (number = "1") THEN (* PMX needs only one beam *)
						beam2PMX( n, pmxbeam, notes[ps, voice, measure, note].stem, staff );  
						
						IF notes[ps, voice, measure, note].chord # "z" THEN 
							COPY( pmxbeam, notes[ps, voice, measure, note].beam );  
							COPY( closebeam, notes[ps, voice, measure, note].closebeam );  
						
						END;  
					END;  
				END;  
			 IF ( n.tagname = "<trill-mark>" ) OR ( n.tagname = "<trill-mark/>" ) THEN COPY (" oT0 ", notes[ps, voice, measure, note].trill); END; 
				IF n.tagname = staccatotag THEN COPY( " o. ", notes[ps, voice, measure, note].staccato );  END;  
					IF (n.tagname = "<accent>" ) OR (n.tagname = "<accent />" ) OR  (n.tagname = "<accent/>" ) 
					THEN 
       					b.FindAtt( n, "placement", placement );				
					   COPY( " o>", notes[ps, voice, measure, note].accent );  
					   	IF ( placement = "below") 		THEN 
						Strings.Append(notes[ps, voice, measure, note].accent,"-12 ") END;
						Strings.AppendCh(notes[ps, voice, measure, note].accent," ");
					END;    
								IF ( n.tagname = "<strong-accent>") OR ( n.tagname = "<strong-accent/>") OR ( n.tagname = "<strong-accent />")
					 			  THEN COPY( " o^ ", notes[ps, voice, measure, note].strongaccent );  END;  

			(*		IF n.tagname = "<arpeggiate>" THEN notes[ps,voice,measure,note].arpeggio := TRUE; END;  (* 22. November 2019 $$$$$$$ *) *)
				IF n.tagname = tuplettag THEN 
					b.FindAtt( n, "type", type );  COPY( type, notes[ps, voice, measure, note].tuplet )
				END;  
				IF (n.tagname = stafftag) THEN notes[ps, voice, measure, note].staff := ORD( n.between[0] ) - 48;  END;  
				IF (n.tagname = repeattag) THEN 
					repeat2PMX( n, pmxrepeat );  COPY( pmxrepeat, notes[ps, voice, measure, note].repeat );  
				END;  
			END;  
			
			IF (n.tagname = "<tremolo>") & ( notes[ps, voice, measure, note].tremolotype ="single"  )
				THEN Strings.StrToInt( n.between, notes[ps, voice, measure, note].trembeam );  

			(*	Out.Ln();  Out.String( " NotesProp-tremolo-beams : " );  Out.Int( ps, 5 );  Out.Int( measure, 5 );  
				Out.Int( notes[ps, voice, measure, note].trembeam, 5 );  *)
			END;   (* not a grace note *)
			
		IF (4 IN outputset ) & ( n.tagname = lyrictag ) THEN	b.lyric( ps, n );  END;  (* stores lyric of a song *)
			IF (n.tagname # eonotetag) THEN n := n.next;  END;  
		END;  
		(*	Enumerate(part,staff,voice,measure,note); *) 
	END NotesProp;  


	PROCEDURE MeasureProp( measure: LONGINT;  n: b.Tag );  
	(* stores measure information in an array with index [measure] for later use in the PMX generation. *)
	VAR pmxrepeat, pmxending: ARRAY 16 OF CHAR;  
		type: ARRAY 32 OF CHAR;  
	BEGIN 
(*	Out.Ln(); Out.String("in measure prop"); *)
		IF (n.tagname = barstyletag) THEN 
			IF (n.between = "light-light") THEN COPY( " Rd ", measures[measure].barstyle )
			(* ELSIF (n.between = "light-heavy") THEN COPY( " RD ", measures[measure].barstyle ) RD not needed in PMX *)
			END;  
		END;  
		IF (n.tagname = repeattag) THEN repeat2PMX( n, pmxrepeat );  COPY( pmxrepeat, measures[measure].repeat );  END;  
		IF (n.tagname = endingtag) THEN 
			ending2PMX( n, pmxending, type );  
			IF type = "start" THEN COPY( pmxending, measures[measure].ending );  END; 
			IF type = "discontinue" THEN COPY (pmxending,measures[measure+1].ending); END; 
		END;  
	
	END MeasureProp;  

	PROCEDURE ReadClef( VAR n: b.Tag;  VAR sign: CHAR;  VAR line, staff: LONGINT );  
	VAR endtag, number: ARRAY 32 OF CHAR;  
	
	BEGIN 
		IF (n.tagname = cleftag) THEN 
			b.loesch( number );  b.FindAtt( n, "number", number );   (* Out.Ln();  Out.String( " ReadClef : " );  
			Out.Int( n.nr, 5 );  Out.Char( "|" );  Out.String( number );  *)
			IF number[0] # 0X THEN Strings.StrToInt( number, staff );  ELSE staff := 1 END;  
			COPY( n.endtag, endtag );  
			WHILE (n.next # NIL ) & (n.tagname # endtag) DO 
				(*		OutTag( n );   *)
				IF (n.tagname = signtag) THEN sign := n.between[0];  END;  (* "p" by percussion *)
				IF (n.tagname = cleflinetag) THEN Strings.StrToInt( n.between, line );  END;  
							n := n.next;  
			END;  
		ELSE Out.String( "Readclef n.tagname = " );  Out.String( n.tagname )
		END;  
	END ReadClef;  

	PROCEDURE AttributesProp( VAR n: b.Tag );  
	(* stores initial structure ( divisions, fifths,beats,beattype,staves,clef and transposition) 
		in an array with index [part] for later use in the PMX generation. *)
	VAR staff: LONGINT;  
		endtag, symbol: ARRAY 32 OF CHAR;  
		sign: CHAR;  line: LONGINT;  
	BEGIN 
	
		IF (n.tagname = attributestag) THEN 
			COPY( n.endtag, endtag );  n.used := TRUE;  stavesfound := FALSE;  
			WHILE (n.next # NIL ) & (n.tagname # endtag) DO 
				part := n.part;  attributes[part].measure := n.measure;  measure := n.measure;   (* OutTag( n );  *)
				
				attributes[part].note := n.note;   (* Out.Ln();  Out.String( "AttributesProp : part  " );  Out.Int( part, 5 );  *)
				IF (n.tagname = divisionstag) THEN attributes[part].divisions := b.ExtractInt( n.between ) ; END;
	
				IF (n.tagname = fifthstag) THEN 
					attributes[part].fifth := b.ExtractInt( n.between );  measures[measure].keys[part] := attributes[part].fifth;
									b.akeys[part] := attributes[part].fifth; 
													END;  
				IF (n.tagname = "<time>") THEN 
					b.FindAtt( n, "symbol", symbol );  (* Out.Ln();  Out.String( "att. symbol :****************" );  *)
					Out.String( symbol );  
					IF symbol = "common" THEN attributes[part].mtrdenp := 6;  
					ELSIF symbol = "cut" THEN attributes[part].mtrdenp := 5;  
					ELSE attributes[part].mtrdenp := 0;  
					END;  
				END;  
				
				IF (n.tagname = beatstag) THEN 
					attributes[part].beats := b.ExtractInt( n.between );  (* measures[measure].beats := attributes[part].beats;  versuch: 19.5.2017*)
				END;  
				IF (n.tagname = beattypetag) THEN 
					attributes[part].beattype := b.ExtractInt( n.between );  
				
				END;  
				
				IF (n.tagname = stavestag) THEN 
					attributes[part].staves := b.ExtractInt( n.between );  stavesfound := TRUE;  
					staves[n.part] := attributes[part].staves;  
				END;  
				IF (n.tagname = "<diatonic>") THEN 
					Out.Ln();  Out.String( " diatonic" );  Out.Ln();  Strings.StrToInt( n.between, attributes[part].diatonic )
				END;  
				IF (n.tagname = "<chromatic>") THEN Strings.StrToInt( n.between, attributes[part].chromatic ) END;  
				IF (n.tagname = "<octave-change>") THEN Strings.StrToInt( n.between, attributes[part].octave ) END;  
				
				IF (n.tagname = cleftag) THEN 
					ReadClef( n, sign, line, staff );  attributes[part].clefsign[staff] := sign;  
					attributes[part].clefline[staff] := line; END;  
				
				n := n.next;  
			
			END;  
			IF (stavesfound = FALSE ) THEN attributes[part].staves := 1;  staves[n.part] := attributes[part].staves;  END;  
			
			IF (attributes[part].diatonic # 0) & (attributes[part].chromatic # 0) THEN 
				Out.Ln();  Out.String( " Transposition : part, fifths, dia, chrom, oct " );  Out.Int( part, 5 );  
				Out.Int( attributes[part].fifth, 5 );  Out.Int( attributes[part].diatonic, 5 );  

				Out.Int( attributes[part].chromatic, 5 );  Out.Int( attributes[part].octave, 5 );  
			END;  
		
		END; 
			END AttributesProp;  



	PROCEDURE Enrich2( q: b.FIFO );  
	(* determines last note with voice "0" for distributing directions. *)
	(* n.voice12 has the value "1" for the upper voice and  "2" for the lower voice in one staff. *)
	VAR n: b.Tag;  lastnote: LONGINT;  
	BEGIN 
		n := q.first; lastnote := 0;  
		WHILE n.next # NIL DO 
			IF (n.tagname = measuretag) THEN lastnote := 0;  END;  
			IF (n.tagname = notetag) THEN 
				part := n.part;  staff := n.staff;  ps := linstaff( nostaves, part, staff );  
				n.voice12 := VOICE01( ps, n.voice, n.measure ) + 1;  
		(*		IF (n.chord # "z") THEN n.lastnote := lastnote;  END;   Change: 26.07.2019 *)
					IF (n.chord # "c") THEN n.lastnote := lastnote;  END;  
				(* OutTag(n,TRUE); *)
			END;  
			IF n.voice12 = 1 THEN lastnote := n.note END;  
			n := n.next;  
		END;  
	
	END Enrich2;  
		PROCEDURE Enrich( q: b.FIFO );  
	(* 1. Read voice and staff; store as notes attribute;
	    2. Calculates maxnote, minvoice,maxvoice,minVoice,maxVoice; *)
	VAR n, m: b.Tag;  notevoice, notestaff : LONGINT;  duration, backup,  B: LONGINT;   
	
	BEGIN 
		
		b.loeschint( voicecount );  
		n := q.first;   (* Out.Ln();  Out.String( "part, staff, measure, voice, note, minvoice " ); Out.Ln();  *)
		WHILE n.next # NIL DO 
			
			IF (n.tagname = notetag) THEN 
				
				
				IF (n.next.tagname = gracetag) OR (n.next.tagname = gracetag2) THEN 
					INC( lastgrace );  n.grace := SHORT( lastgrace ); (*b.OutTag(n,TRUE); Out.String("ist grace"); 	*)
								ELSE lastgrace := 0
				END;  
				
				b.FindProperty( n, notetag, "<arpeggiate>", B );  
				IF B > 0 THEN   n.arpeggio := TRUE ELSE n.arpeggio := FALSE  END;  
				
				b.FindIProperty( n, notetag, voicetag, voice );  n.voice := voice;  notevoice := voice;  

				b.FindIProperty( n, notetag, stafftag, staff );  n.staff := staff;  notestaff := staff;  note := n.note;  

				b.FindIProperty( n, notetag, durationtag, duration );  n.duration := SHORT( duration );  

				b.FindProperty( n, notetag, chordtag, B );  
				IF B > 0 THEN n.chord := "c" END;  
				
				measure := n.measure;  part := n.part;  

				maxnote[part, staff, measure] := b.Max( note, maxnote[part, staff, measure] );  
				minnote[part, staff, measure] := b.Min( note, minnote[part, staff, measure] );  
				
				(*	NEW( st );  b.psv2String( part, staff, voice, st.key );  InsertRanked( first, st );  NEW( psmv );  
				b.psmv2String( part, staff, voice, measure, psmv.key );  InsertRanked( firstpsmv, psmv );  *)
				
			END;  
			IF (n.tagname = directiontag) THEN b.FindIProperty( n, directiontag, stafftag, staff );  n.staff := staff;  
			b.findnextnote (n,m); IF m # NIL THEN n.nextnote := m.note; 
				b.FindIProperty( m, notetag, voicetag, voice );  n.nextvoice := voice;   END; (* voice of next note "m"&&&&& *)
			END;  
			IF (n.tagname = backuptag) THEN 
			(* Out.Ln();Out.String("******************* Backup"); *)
				b.FindIProperty( n, backuptag, durationtag, backup );  n.backup := SHORT( backup );  m := n;  
				WHILE (m.next # NIL ) & (m.tagname # notetag) DO 
		(*		 Out.Ln(); Out.String("backup-m");					b.OutTag(m,FALSE);  *)
					m := m.next ;
			 END;  (* hier werden Daten weggelesen, bis zur naechsten Note *)
						                                                                                                        (*   und dann der backup der Note zugeordnet *)
			(*	IF m.grace > 0 THEN 
					WHILE 
						m.grace > 0 DO b.OutTag(m,TRUE); m := m.next; 
					END; 
				END;	(* Aenderung wg ala tiurca grace notes Takt 28 ff *)	   *)                                                                                                     
				(* Store backup at next note *) ;  m.backup := SHORT( backup ); (* b.OutTag(m,TRUE); *)
			END;  
			
			(*	 OutTag(n);  *)
			n := n.next;  
		END;  
		Enrich2(b.q);


			
	END Enrich;  

	PROCEDURE VOICE01( ps, voice, measure: LONGINT ): LONGINT;  
	BEGIN 
		IF vmap[ps, measure, 0] = voice THEN RETURN 0
		ELSIF vmap[ps, measure, 1] = voice THEN RETURN 1
		ELSE RETURN 2
		END;  
	
	END VOICE01;  


	 PROCEDURE Accidentals(n : b.Tag; ps, voice, measure, note : LONGINT);			
	VAR   
		cautionary, editorial, parentheses 	: ARRAY 32 OF CHAR;  
	BEGIN			
			IF (n.tagname) = accidentaltag THEN  (* 5 *)
				b.FindAtt( n, "editorial", editorial ); 
				b.FindAtt( n, "cautionary", cautionary );  
				b.FindAtt( n, "parentheses", parentheses );  					 
				IF n.between = "sharp" THEN 
					COPY( "s", notes[ps, voice, measure, note].accidental );
					IF editorial = "yes" THEN Strings.Append( notes[ps, voice, measure, note].accidental, " oes " ) END;  					
				ELSIF n.between = "flat" THEN 
					COPY( "f", notes[ps, voice, measure, note].accidental );
					IF editorial = "yes" THEN Strings.Append( notes[ps, voice, measure, note].accidental, " oef " ) END;  					
				ELSIF (n.between = "double-sharp") OR (n.between = "sharp-sharp") THEN 
					COPY( "ss", notes[ps, voice, measure, note].accidental )
				ELSIF (n.between = "flat-flat") OR (n.between = "double-flat") THEN 
					COPY( "ff", notes[ps, voice, measure, note].accidental )
				ELSIF n.between = "natural" THEN COPY( "n", notes[ps, voice, measure, note].accidental ); 
				IF editorial = "yes" THEN Strings.Append( notes[ps, voice, measure, note].accidental, " oen " ) END; 				
				ELSE 
				END;  				
				IF ( cautionary = "yes" ) OR ( parentheses = "yes") 
					THEN Strings.AppendCh( notes[ps, voice, measure, note].accidental, "c" );  

				END;  
			
			END; 

	END Accidentals;			



	PROCEDURE EnumerateTags*;  
	(* runs through the list of tags and calls NotesProp and MeasuresProp and lists the tag-data in System.Log *)
	VAR n: b.Tag;   voice01: LONGINT;  
	BEGIN 
		n := b.q.first; voice01 := 0;
		WHILE n.next # NIL DO  (* 1 *)
			part := n.part;  measure := n.measure;  note := n.note;   (* voice := n.voice; staff := n.staff; *)
			
					
			IF (n.tagname = notetag) THEN  (* 3 *)
				
				voice := n.voice;  staff := n.staff; 
				(* store cue-property 11.07.2019 *)
				  IF ( n.next.tagname = "<cue/>" ) THEN n.cue := TRUE; END;

				ps := linstaff( nostaves, part, staff );  voice01 := VOICE01( ps, voice, measure );  
				
				(* directions are coded after notes in upper voice *)
								NotesProp( part, staff, voice01, measure, note, n );  INC(countnote); 
							(*	(* IF (countnote <10) THEN *) Out.Ln(); Out.String("cnote");Out.Int(countnote,5) (* END; *) ; *)
									

			END;   (* 3 *)
			
			IF (n.tagname = attributestag) THEN AttributesPropM( voice01, measure, n );  
			INC (countattr); END;  
					(* geaendert 8.1.2016: lastnote -> note *)
	(*				IF 1 =0 THEN *)
			IF (n.tagname = directiontag) THEN DirectionProp( n,voice01 ); 	INC(countdir) ;END;  
			MeasureProp( measure, n );   (* OutTag( n ); *)
			n := n.next
		END;   (* 1 *)
	(*	FillMeasures;  
					Out.Ln(); Out.String(" nach attributesprom : "); ; ListMeasures; *)
		
		(*	ListDir;  $$$$$$$$$$$$$$$$ *)
(*		Out.Ln();  Out.String( "part, transposition: fifth diatonic chromatic octave ");  
INC
		part := 1;  
		WHILE part <= maxpart DO 
			Out.Ln();  Out.Int( part, 5 );  Out.Int( attributes[part].fifth, 5 );  Out.Int( attributes[part].diatonic, 5 );  
			Out.Ln(); Out.String("divisions in enumerate : "); Out.Int(attributes[part].divisions,5);
	(*		Out.Int( attributes[part].chromatic, 5 );  Out.Int( attributes[part].octave, 5 );  *)
			(* pitch.pmxtranspose(part,attributes[part].fifth,  remove automatic transposition based on XML-transpose
		attributes[part].diatonic,attributes[part].chromatic,attributes[part].octave,kstring);
		Out.Char("|"); Out.String(kstring); COPY(kstring,attributes[part].kstring); *)
			INC( part );  
		END;                                                                                                                                            *)
		ps := 0;  
		WHILE ps < nostaves DO part := partstaff[ps, 0];  
   (* Calculate durations for all instruments and measures *)
			measure := 1;  
			WHILE measure <= maxmeasure DO 
						
						IF measures[measure].beattype > 0 THEN   (* 20.10.2020 Lilypond *)
					
						measures[measure].duration[part] := 
					measures[measure].beats*4*attributes[part].divisions 
								DIV measures[measure].beattype;   (* Division durch 0 *)

			 
						END; 
				INC( measure );  
			END;  
			INC( ps );  
		END; 
(*	 END; (* Fake End *) *)
(* 	ListMeasures;   $$$$ *)
		(* prvoicetime;  *)
	(*	CopyClefVoice;  *)
	END EnumerateTags;  



	PROCEDURE DirectionProp( VAR n: b.Tag; voice01 : LONGINT );  
	VAR endtag, placement, type, res: ARRAY 128 OF CHAR;  
		dirnum, dirtypenr : LONGINT;  defaultx: LONGINT;  sbeatunit : ARRAY 32 OF CHAR;
		defaultxs: ARRAY 32 OF CHAR;  rehtext : ARRAY 32 OF CHAR; (* fuer rehearsal marks *)
	BEGIN 
	(* Out.Ln(); Out.String("in direction prop"); *)

		IF (n.tagname = directiontag) THEN  (* 1 *)
		
		(*	b.OutTag( n, TRUE );  *)
			COPY( n.endtag, endtag );  part := n.part;  measure := n.measure;  dirnum := n.dirnum;  staff := n.staff;  
			directions[dirnum, 0].part := n.part;  directions[dirnum, 0].measure := n.measure;  voice := n.voice;  
			directions[dirnum, 0].staff := staff;  directions[dirnum, 0].used := FALSE;   (* allow usage of direction only once. *)
			maxdirtype[dirnum] := 0;  ps := linstaff( nostaves, part, staff );  (* voice01 := VOICE01( ps, voice, measure );  changed 22.12.2016 *)
			(* Voice01( ps, voice );  *)
			
			directions[dirnum, 0].note := n.note;  directions[dirnum, 0].lastnote := n.lastnote; (* -1 ;   Versuch 21. Mai  => Absturz *)
			directions[dirnum,0].voice := voice01; (* changed 22.12.2016 *)
			
			IF measure = 96 THEN Out.Ln(); Out.String("DirectionProp : dirnum, note | lastnote "); 
			Out.Int(dirnum,5); Out.Int(n.note,5); Out.Char("|");Out.Int(n.lastnote,5); 
						END; 

			(* CheckVoice(ps,measure,n.lastnote); *)
			(* the direction is stored at this note *)
			
			(*	Out.Ln();  Out.String( "DirectionProp : lastnote " );  Out.Int( dirnum, 5 );    
			Out.Int( directions[dirnum, 0].lastnote, 5 );  *)
			directions[dirnum, 0].before := FALSE;   (*$$$$$$$$$$$$$$$$$$$$$$*)
			
			
			b.FindAtt( n, "placement", placement );  dirtypenr := 0;  
			(* placement is valid for all direction-types under the direction *)
			WHILE (n.next # NIL ) & (n.tagname # endtag) DO  (* 2 *)
				IF (n.tagname = "<direction-type>") THEN  (* 3 *)
					INC( dirtypenr );  n := n.next;   (*  OutTag( n );   *)
					directions[dirnum, dirtypenr].lastnote := directions[dirnum, 0].lastnote;  
					IF (n.tagname = "<dynamics>") THEN  (* 3 .1*)
						
						b.FindAtt( n, "default-x", defaultxs );  Strings.StrToInt( defaultxs, defaultx );  
						directions[dirnum, dirtypenr].defaultx := SHORT( defaultx );  

						n := n.next;   (* OutTag( n ); *) directions[dirnum, dirtypenr].dirtype := "d";  
						COPY( n.tagname, directions[dirnum, dirtypenr].dyntype );  
						directions[dirnum, dirtypenr].before := FALSE;  
						IF directions[dirnum, dirtypenr].note = 0 THEN 
							directions[dirnum, dirtypenr].note := 1;  directions[dirnum, dirtypenr].placement := "l";  
						END;  
					
					ELSIF (n.tagname = "<words>") THEN  (* 3 .2*)
						IF (placement = "above") OR (placement = "") THEN 
							directions[dirnum, dirtypenr].placement := "h";   (* Aenderung 19. Mai *)
							
						ELSIF (placement = "below") THEN directions[dirnum, dirtypenr].placement := "l";  
						END;  
						
						directions[dirnum, dirtypenr].dirtype := "w";  COPY( n.between, directions[dirnum, dirtypenr].text );  

						directions[dirnum, dirtypenr].before := TRUE;  
					
					ELSIF (n.tagname = "<pedal>") THEN  (* 3 .3*)
						directions[dirnum, dirtypenr].dirtype := "p";  b.FindAtt( n, "type", type );  
						directions[dirnum, dirtypenr].before := TRUE;  

						COPY( type, directions[dirnum, dirtypenr].pedaltype );  
					
					ELSIF (n.tagname = "<wedge>") THEN  (* 3 .4*)
						directions[dirnum, dirtypenr].dirtype := "h";  b.FindAtt( n, "type", type );  
						COPY( type, directions[dirnum, dirtypenr].wedgetype );  directions[dirnum, dirtypenr].before := FALSE;  
						IF directions[dirnum, dirtypenr].note = 0 THEN directions[dirnum, dirtypenr].note := 1;  END;  
						directions[dirnum, dirtypenr].placement := "l";  
					ELSIF (n.tagname = "<metronome>") THEN  (* 3 .5*)
						directions[dirnum, dirtypenr].placement := "h";  directions[dirnum, dirtypenr].dirtype := "w";  (* Metronom ist "w" *)
						WHILE (n.next # NIL ) & (n.tagname # "</metronome>") DO 
							IF n.tagname = "<beat-unit>" THEN COPY (n.between,sbeatunit) END;  
							  
							IF n.tagname = "<per-minute>" THEN b.metron2PMX (sbeatunit,n.between,directions[dirnum,dirtypenr].text);
							(*	Out.String( "metronome : " ); *) 
							(* 	Out.String( directions[dirnum, dirtypenr].text );  *)
								directions[dirnum, dirtypenr].before := TRUE;  
							END;  
							
							n := n.next;  
						END;
						ELSIF (n.tagname = "<segno>"  ) THEN directions[dirnum, dirtypenr].dirtype := "s";
							directions[dirnum, dirtypenr].before := FALSE;  
						ELSIF (n.tagname = "<coda>"  ) THEN directions[dirnum, dirtypenr].dirtype := "c";
							directions[dirnum, dirtypenr].before := FALSE;  
                        ELSIF (n.tagname = "<rehearsal>"  ) THEN 
                        directions[dirnum, dirtypenr].placement := "h";
                        directions[dirnum, dirtypenr].dirtype := "w"; (* 14.10.2020  "rehearsal" *)
							directions[dirnum, dirtypenr].before := TRUE;   
							COPY (" \boxit{",rehtext); 
							Strings.Append (rehtext,n.between);
							b.loesch(directions[dirnum,dirtypenr].text) ;
							Strings.Append(rehtext," } ")     ;  
							
							COPY(rehtext,directions[dirnum,dirtypenr].text);
						
								
							           Out.Ln(); Out.String( "rehearsal : "); Out.String(rehtext);    
							           Out.Ln();Out.String("text :"); Out.String  (  directions[dirnum,dirtypenr].text  ) ; 


					ELSE 
						Out.Ln();  Out.String( "<direction-type> " );  Out.String( n.tagname );  
						Out.String( " not implemented. " );  
					END;  
					maxdirtype[dirnum] := b.Max( dirtypenr, maxdirtype[dirnum] );  
					 (* Out.Ln(); Out.String("dirtypenr : ");				Out.Int(dirtypenr,5);*)
					pmxDirections( dirnum, dirtypenr, res );  COPY( res, directions[dirnum, dirtypenr].pmxdirection );  
					(*  OutDir(dirnum,dirtypenr);  *)
					
		(*		 Out.Int(dirnum,5); Out.String("| in directionprop "); 	Out.String(res); Out.String(directions[dirnum, dirtypenr].pmxdirection); *)
					
				END;   (* 3 *)
				(*		IF (n.tagname = stafftag) THEN Strings.StrToInt( n.between, directions[dirnum].staff ) END;  ??????? *)
				n := n.next;   (* OutTag( n );  *)
			END;   (* 2 *)
		END;   (* 1 *)
		
	END DirectionProp;  

	PROCEDURE AttributesPropM ( voice, measure: LONGINT;  VAR n: b.Tag );  
	(*****************  voice = voice01 ********************)
	VAR sign: CHAR;  line, number, part : LONGINT;  clef: CHAR;  m : b.Tag; staff : LONGINT;
		(* takes care of changes: 
								- clefchange [part,staff] in the flow of notes
								- meter change: beats and beattype [measure]
								- fifth change 
								- change of divisions
								links the changes to a note *)
	BEGIN (* Out.Ln(); Out.String(" in AttirbutesPropM ");*)
		IF (n.tagname = attributestag) & ~n.used THEN  (* 1 *)
			part := n.part;  staff := n.staff;  (* REparaturversuch 31.10.2020 *)
			(* ps := linstaff( nostaves, part, staff ); *)
WHILE n.tagname # "</attributes>" DO  (* 2*)
		(*	Out.Ln(); Out.String(n.tagname);	*)
		IF (n.tagname = cleftag) THEN       
		(*     Out.Ln(); Out.String(cleftag);
	    	Out.Ln(); Out.String("part,staff,ps, measure  :");Out.Int(part,5); Out.Int(staff,5); Out.Int(ps,5); Out.Int(measure,5); *)
		
		                                                                                                        (* neu Oktober 2020) find next note of same staff. *)
					ReadClef( n, sign, line, number ); 
					 b.clefPMX( sign, SHORT( line ), clef, clefspec[ps] );   
					b.findnextnotestaff  ( n, m ,number); m.newclef := clef; 
					 (* lastclef[ps] := clef; *)
					(* notes[ps, voice, measure, n.lastnote].clefchanged := clef;  *)  

					
			(*		b.OutTag(m,TRUE);  *) (* INC (countclefchanges); Out.Ln();Out.Int(countclefchanges,5); *)
					
					END;
		
		(*			Out.Ln(); Out.String("AttPropM"); Out.Char("|"); Out.Char( clef ); 
					Out.Char("|"); 				(*	Out.Char( clefspec[ps] );   *)
					
					Out.Ln();Out.String("n. lastnote"); Out.Int(n.lastnote,5);
					
					IF ( n.lastnote # 0 ) THEN
							lastclef[ps] := clef;  notes[ps, voice, measure, n.lastnote].clefchanged := clef; 
							
							Out.Ln(); Out.String("AttributesProp: n.lastnote # 0 "); Out.Int(measure,5); Out.Char("|"); Out.Char(clef);
					ELSE
					(* n.lastnote = 0 *)
							measures[measure ].clefchange[linstaff(nostaves,part,number)] := clef; (* Maerz 2018 *)
							
							
							Out.Ln(); Out.String("AttributesProp: n.lastnote  =  0 "); Out.Int(measure,5); Out.Char("|"); Out.Char(clef); 
					END;	*)
				(*	 Out.Ln();  
					Out.String( "clefs *******" );  Out.Int( ps, 5 );  Out.Int( voice, 5 );  Out.Int( measure, 5 );  
					Out.Int( n.lastnote, 5 );  Out.Char( clef );   *)
					 
				(*	IF part = 1 THEN                          Korrektur 12. Januar 2016: Taktwechsel bei Takt 209 Vivaldi fehlt in der zweiten und driitten 
					Stimme. 
					*)
				IF (n.tagname = beatstag) THEN  (* change 19.5.2017 *)

					measures[measure].beats := b.ExtractInt( n.between );   (* Out.String( " PropM : measure = " );  
					Out.Int( measure, 5 );  Out.Int( measures[measure].beats, 5 ); *)
					END;  
				IF (n.tagname = beattypetag) THEN 
					measures[measure].beattype := b.ExtractInt( n.between );  
					(* Out.Int( measures[easure].beattype, 5 );  *)
				END;  
				IF (n.tagname = fifthstag) THEN 
					attributes[part].fifth := b.ExtractInt( n.between ); 
					measures[measure].keys[part] := attributes[part].fifth; 
					IF (part = maxpart) THEN measures[measure].fifth := attributes[part].fifth END;
				 i := measure+1;
				 WHILE i <= maxmeasure DO measures[i].keys[part] := attributes[part].fifth; 
				 		 IF ( part = maxpart ) THEN
                					 measures[i].fifth := attributes[maxpart].fifth; (* store concertkey *)
               			END;
				  
				    	INC(i); 
				    END;
				    
		         b.mkeys[part] := attributes[part].fifth;
                
                 END;
                 
				  
				IF (n.tagname = divisionstag) THEN attributes[part].divisions := b.ExtractInt( n.between ) ;		 END;  
				(*		END; *)
				n := n.next;  
			END  (*  2 *)
			
		ELSE  (* 1 *)
			
		(*	Out.Ln();  Out.String( " AttributesPropM : attributstag not found or already used ! " );  
			Out.String( n.tagname );  *)
		
		END;   (* 1 *)
			
	END AttributesPropM;  

	PROCEDURE Pickup( q: b.FIFO;  VAR duration: LONGINT );  
	(* finds out whether first measure is smaller, i.e. we have a pickup. *)
	VAR n: b.Tag;  isnote: BOOLEAN;  
		endtag: ARRAY 32 OF CHAR;  
		chord: CHAR;  noteduration: LONGINT;  
		durationsv: ARRAY 25 OF ARRAY 10 OF LONGINT;  
	BEGIN 
		n := q.first;   isnote := FALSE;   chord := 0X;
		WHILE (n.next # NIL ) & (n.tagname # measuretag) DO n := n.next;  END;   (* first measure of first part found *)
	(*	 b.OutTag( n, TRUE );  *)   COPY( n.endtag, endtag );  duration := 0;  
		WHILE (n.next # NIL ) & (n.tagname # endtag) DO  (* Loop over all elements of 1st measure / 1st instrument /1st staff *)
			IF (n.tagname = notetag) THEN 
				isnote := TRUE;  part := n.part;  staff := n.staff;  voice := n.voice;  chord := n.chord;  (* b.OutTag( n, TRUE );  *)
			END;  
			IF (n.tagname = eonotetag) THEN isnote := FALSE;  END;  
			IF (n.tagname = durationtag) & isnote & (chord # "c") THEN 
				Strings.StrToInt( n.between, noteduration );  
				(* Out.Ln(); Out.String("staff :"); Out.Int(staff,5); Out.String("voice :"); Out.Int(voice,5); 
				Out.String("duration : "); Out.Int(noteduration,5); *)

				INC( durationsv[staff, voice], noteduration );  
			(*	b.OutTag( n, TRUE );  *)
			(*	IF isnote THEN Out.String( "ist Note" );  END;  *)
			END;  
			n := n.next;  
		END;  
		
		IF (duration = attributes[1].duration) THEN duration := 0;  
		END;  
		(* Out.Ln();  Out.String( "beats, beattype and divisions : " );  Out.Int( attributes[1].beats, 5 );  Out.Int( attributes[1].beattype, 5 );  
		Out.Int( attributes[1].divisions, 5 );  

		Out.Ln();  Out.String( "Pickup : " );  Out.Int( duration, 5 );  Out.Int( attributes[1].duration, 5 );  *)
	END Pickup;  




	PROCEDURE ControlProp*;  
	(* Extracts metadata from XML file for later use in PMX *)
	VAR type, id, digits: ARRAY 32 OF CHAR;  
		dummy: ARRAY 128 OF CHAR;  
		n: b.Tag;  part: LONGINT;  
	BEGIN 
		n := b.q.first;  
		WHILE (n.next # NIL ) & (n.tagname # "</part-list>") DO 
			IF n.tagname = "<work-number>" THEN COPY( n.between, pmxcontrol.worknumber ) END;  
			IF n.tagname = "<work-title>" THEN b.FilterTeX( n.between, dummy );  COPY( dummy, pmxcontrol.title );  END;  
			b.loesch( type );  
			IF n.tagname = "<movement-title>" THEN COPY( n.between, pmxcontrol.title );  END;  
			IF n.tagname = "<creator>" THEN 
				b.FindAtt( n, "type", type );  
				IF type = "composer" THEN COPY( n.between, pmxcontrol.composer );  
				ELSIF (type = "lyricist") THEN COPY( n.between, pmxcontrol.lyricist )
				END;  
			END;  
			IF (n.tagname = "<rights>") THEN COPY( n.between, pmxcontrol.rights );  END;  
			IF n.tagname = "<encoding-date>" THEN 
				COPY( "encoding date : ", pmxcontrol.info );  Strings.Append( pmxcontrol.info, n.between );  
				Strings.AppendCh( pmxcontrol.info, BLANK );  
			END;  
			IF n.tagname = "<software>" THEN 
				IF pmxcontrol.software[0] = 0X THEN 
					COPY( "Software : ", pmxcontrol.software );  Strings.Append( pmxcontrol.software, n.between );  
				ELSE 
					Strings.AppendCh( pmxcontrol.software, BLANK );  Strings.Append( pmxcontrol.software, n.between );  
				END;  
				Strings.AppendCh( pmxcontrol.info, BLANK );  
			END;  
			(* parts and instruments *)
			IF n.tagname = "<score-part>" THEN 
				b.FindAtt( n, "id", id );  b.ExtractDigits( id, digits );  Strings.StrToInt( digits, controlpart );  
				
				
				(*	controlpart := ORD( id[1] ) - 48;  old version single digit only *)
				
				COPY( id, pmxcontrol.parts[controlpart] );  
			END;  
			IF n.tagname = "<instrument-name>" THEN b.FilterTeX( n.between, pmxcontrol.instruments[controlpart] );  END;  
			n := n.next;  
		END;  
		
		part := 1;  pmxcontrol.equalkeys := TRUE;
		WHILE (part <= maxpart) DO 
			WHILE (n.next # NIL ) & (n.tagname # parttag) DO n := n.next END;   (* FInd next <part> ***************)
			WHILE (n.next # NIL ) & (n.tagname # attributestag) DO n := n.next END;  
			(* FInd next <attributes> *********)
			IF (n.tagname = attributestag) THEN AttributesProp( n );  
				(* determine length of measure in divisions. *******************************)
				attributes[part].duration := 
					attributes[part].divisions*attributes[part].beats*4 DIV attributes[part].beattype;  
				(*	Out.Ln();  Out.String( "AttributesProp : Part | XML-duration of measure : " );  Out.Int( part, 5 );  Out.Char("|");
				Out.Int( attributes[part].duration, 5 );  *) 
			(*	measures[1].duration[part] := attributes[part].duration; *)
			END;  
			(* check for transposing instruments! *)
			IF ( part > 1 ) THEN pmxcontrol.equalkeys := pmxcontrol.equalkeys & ( attributes[part].fifth = attributes[part-1].fifth ) END;

			INC( part );  		END;  
			IF ~ pmxcontrol.equalkeys THEN	b.testmakekey(maxpart,1,keytotal); END; (* different keys in different parts  *)
	END ControlProp;  

	PROCEDURE StaffInd( nostaves, part, staff: LONGINT ): LONGINT;  
	(* finds the linear index for [part,staff] *)
	VAR i, dist: LONGINT;  
	BEGIN 
		i := -1;  
		REPEAT INC( i );  dist := ABS( partstaff[i, 0] - part ) + ABS( partstaff[i, 1] - staff );  
		UNTIL (i = nostaves - 1) OR (dist = 0);  
		RETURN i;  
	END StaffInd;  

	PROCEDURE AllStaves( VAR staves: ARRAY OF LONGINT );  
	(* maps part/staff to a linear index. *)
	VAR part, staff, i, nostaves: LONGINT;  
	BEGIN 
		i := 0;  part := 1;  
		WHILE part <= maxpart DO 
			staff := 1;  
			WHILE staff <= staves[part] DO partstaff[i, 0] := part;  partstaff[i, 1] := staff;  INC( staff );  INC( i );  END;  
			
			INC( part );  
		END;  
		nostaves := i;  i := 0;  b.nostaves := nostaves; (* inform MODULE "b" about # of staves *)
	END AllStaves;  

	PROCEDURE progress( q: b.FIFO );  
	(* Calculates the horizontal position of each note in a measure and stores it in the tag.  *)
	VAR n: b.Tag;  ps: LONGINT;    
	BEGIN 
                ps := 0;
		n := q.first;  
		
		WHILE (n.next # NIL ) DO (* 1 *)
			IF (n.tagname = measuretag) THEN Complete.erasetime( nostaves,voicetime );  lastto := 0;  lastfrom := 0;    END;  (* 2 *)
			IF (n.tagname = notetag) THEN  (* 3 *)
				part := n.part;  staff := n.staff;  measure := n.measure;  ps := StaffInd( nostaves, part, staff );  
				voice := VOICE01( ps, n.voice, n.measure );  
				
				IF (n.chord # "c") & ( n.grace = 0 ) THEN  (* 4 *) (* war frueher "&" statt "OR" *)
				(* INC(measures[measure].dur,n.duration); Out.Int(n.duration,5); *)
					INC( voicetime[ps, voice], n.duration );  n.voicetime := voicetime[ps, voice];  
					measures[measure].voicetime[ps, voice] := n.voicetime;  n.from := lastto + 1 - n.backup;  
					n.to := n.from + n.duration - 1;  lastto := n.to;  lastfrom := n.from;  
				ELSIF  (n.chord = "c" ) THEN n.voicetime := lastto;  n.to := lastto;  n.from := lastfrom;  
				ELSIF ( n.grace = 1  ) THEN n.from := lastto  -  n.backup; n.to := n.from ; lastto := n.to;
				ELSIF ( n.grace > 1 ) THEN n.from := lastto;  n.to := n.from; lastto := n.to;
				ELSE
				END;  (*  4 *)
					
			END;  (* 3 *) 
			IF ( n.tagname = eomeasuretag ) & (ps = nostaves - 1) THEN  (* 5 *)
			measures[measure].dur := lastto;
			(* Out.Char("|");Out.Int(measure,5);Out.Int(ps,5);Out.Int(measures[measure].dur,5); Out.Ln(); *) END; (* 5 *)
			
			n := n.next;  
		END;  (* 1 *)
		(* Out.Ln(); Out.String("measure duration"); *)
		 measure := 1; WHILE measure <= maxmeasure DO (* Out.Ln(); Out.Int(measure,5);
		Out.Int (measures[measure].dur ,5);  *)
		Complete.dur2beat(measures[measure].dur,attributes[part].divisions,
										measures[measure].beats, measures[measure].beattype);
		(* Out.Int(measures[measure].beats,5); Out.Int(measures[measure].beattype,5); *)
		INC(measure) END; 
	
	END progress;  


	PROCEDURE linstaff( nostaves, part, staff: LONGINT ): LONGINT;  
	(* calculates the linear index of the combination "part and staff" *)
	VAR i: LONGINT;  
	BEGIN 
		i := 0;  
		WHILE (i < nostaves) & (partstaff[i, 0] # part) DO INC( i ) END;  
		WHILE (i < nostaves) & (partstaff[i, 0] = part) & (partstaff[i, 1] # staff) DO INC( i ) END;  
		
		IF (i < nostaves) THEN RETURN i;  
		ELSE RETURN -1;  
		END;  
	END linstaff;  


	PROCEDURE ControlData( VAR W: Files.Rider );  
	(* Writes PMX header information to intermediate Files.File "fprep" with Files.Rider "rprep" *)
	VAR i: LONGINT;  xmtrnum0: LONGREAL;  clef: CHAR;  
		perclef, perline: ARRAY 32 OF CHAR;  
		zeit, tag, xmtrnum0s: ARRAY 16 OF CHAR;  
	BEGIN 
		
		
		(* TeX Input type 4 *)
		WriteString( W, "---" );  Files.Write( W, CR );  Files.Write( W, NL );   
			WriteString( W, "%\input musixlyr" );  Files.Write( W, CR );  Files.Write( W, NL );  
		WriteString( W, "%\special{papersize=297mm,420mm}%" );  Files.Write( W, CR );  Files.Write( W, NL );  
		WriteString( W, "---" );  Files.Write( W, CR );  Files.Write( W, NL );  

		Files.Write( W, "%" );  
		(* DateTime(zeit,tag); *)
		WriteString( W, tag );  Files.Write( W, "|" );  WriteString( W, zeit );  Files.Write( W, CR );  Files.Write( W, NL );  

		Files.Write( W, "%" );  

		WriteString( W, pmxcontrol.xml );  Files.Write( W, CR );  Files.Write( W, NL );  Files.Write( W, "%" );  

		WriteString( W, pmxcontrol.dtd );  Files.Write( W, CR );  Files.Write( W, NL );  Files.Write( W, "%" );  

		WriteString( W, pmxcontrol.title );  Files.Write( W, CR );  Files.Write( W, NL );  Files.Write( W, "%" );  
		
		IF (pmxcontrol.worknumber[0] # 0X) THEN 
			WriteString( W, "%  work number : " );  WriteString( W, pmxcontrol.worknumber );  Files.Write( W, CR );  
			Files.Write( W, NL );  Files.Write( W, "%" );  
		END;  
		
		WriteString( W, pmxcontrol.composer );  Files.Write( W, CR );  Files.Write( W, NL );  Files.Write( W, "%" );  
		
		IF (pmxcontrol.lyricist[0] # 0X) THEN 
			WriteString( W, pmxcontrol.lyricist );  Files.Write( W, CR );  Files.Write( W, NL );  

			Files.Write( W, "%" );  
		END;  
		
		WriteString( W, pmxcontrol.info );  Files.Write( W, CR );  
		Files.Write( W, NL );  Files.Write( W, "%" );  
		
		IF (pmxcontrol.rights[0] # 0X) THEN 
			WriteString( W, " Rights : " );  WriteString( W, pmxcontrol.rights );  Files.Write( W, CR );  Files.Write( W, NL );  
			Files.Write( W, "%" );  
		END;  
		WriteString( W, pmxcontrol.software );  Files.Write( W, CR );  Files.Write( W, NL );  Files.Write( W, "%" );  

		part := 1;  nostaves := 0;  
		WHILE part <= maxpart DO 

			WriteString( W, "Part " );  WriteString( W, pmxcontrol.parts[part] );  WriteString( W, " : " );  
			WriteString( W, pmxcontrol.instruments[part] );  Files.Write( W, CR );  Files.Write( W, NL );  
			Files.Write( W, "%" );  nostaves := nostaves + attributes[part].staves;  INC( part );  
		END;  
		
		AllStaves( staves );  Files.Write( W, CR );  Files.Write( W, NL );  Files.Write( W, "%" );  Files.Write( W, CR );  
		Files.Write( W, NL );  

		WriteString( W, "%  nv, -noinst, nostaves per instrument" );  Files.Write( W, CR );  Files.Write( W, NL );  

		WriteLInt( W, nostaves );   (* nv *)
		WriteLInt( W, -maxpart );   (* noinst *)
		i := maxpart;  
		WHILE i > 0 DO WriteLInt( W, staves[i] );  DEC( i ) END;  
		Files.Write( W, CR );  Files.Write( W, NL );  

		WriteString( W, "% mtrnuml,mtrdenl,mtrnmp,mtrdnp,xmtrnum0,isig" );  Files.Write( W, CR );  
		Files.Write( W, NL );  

		WriteLInt( W, attributes[1].beats );   (* mtrnuml *)
		WriteLInt( W, pmxtype( attributes[1].beattype ) );   (* mtrdenl *)
		IF attributes[1].mtrdenp # 0 THEN WriteLInt( W, 0 ) ELSE WriteLInt( W, attributes[1].beats );  END;   (* mtrnump *)
		IF attributes[1].mtrdenp # 0 THEN WriteLInt( W, attributes[1].mtrdenp ) 
		ELSE WriteLInt( W, attributes[1].beattype );  
		END;   (* mtrdenp *)
		IF (attributes[1].pickup > 0) THEN 
			xmtrnum0 := attributes[1].pickup/attributes[1].divisions;  
			IF attributes[1].beattype = 2 THEN xmtrnum0 := xmtrnum0/2;  
			ELSIF attributes[1].beattype = 8 THEN xmtrnum0 := xmtrnum0*2;  
			ELSIF attributes[1].beattype = 16 THEN xmtrnum0 := xmtrnum0*4;  
			ELSE 
			END;  
		
		ELSE xmtrnum0 := 0.;  
		END;  
		Strings.RealToStr( xmtrnum0, xmtrnum0s );  Files.Write( W, BLANK );  
		WriteString( W, xmtrnum0s );   (*xmtrnum0 *)
		WriteLInt( W, attributes[maxpart].fifth );   (* isig *) (* this is the concert key *)
		Files.Write( W, CR );  Files.Write( W, NL );  Files.Write( W, "%" );  Files.Write( W, CR );  Files.Write( W, NL );  

		WriteString( W, "% npages,nsyst,musicsize,fracindent " );  Files.Write( W, CR );  Files.Write( W, NL );  

		WriteLInt( W, 0 );  WriteLInt( W, 5 );  WriteLInt( W, 16 );  Files.Write( W, BLANK );  WriteString( W, "0.07" );  
		part := maxpart;  
		WHILE (part >= 1) DO 
			Files.Write( W, CR );  Files.Write( W, NL );  
			IF pmxcontrol.instruments[part] = "Acoustic Grand Piano" THEN 
				COPY( "Piano", pmxcontrol.instruments[part] );  
			END;  
			WriteString( W, pmxcontrol.instruments[part] );  DEC( part );  
		END;  
		Files.Write( W, CR );  Files.Write( W, NL );  

		i := nostaves - 1;  
		WHILE i >= 0 DO part := partstaff[i, 0];  staff := partstaff[i, 1]; (* Out.Ln();  

			 Out.String( "control data :  i, part, staff, sign, line, clef special clef " );  Out.Int( i, 5 );  Out.Int( part, 5 );  
			Out.Int( staff, 5 );  *)

			b.clefPMX( attributes[part].clefsign[staff], SHORT( attributes[part].clefline[staff] ), clef, clefspec[i] );  
			lastclef[i] := clef;  Files.Write( W, clef );
			DEC( i );  
		END;  
		
		Files.Write( W, CR );  Files.Write( W, NL );  WriteString( W, "./" );  Files.Write( W, CR );  Files.Write( W, NL );  

		WriteString( W, "Tt" );  Files.Write( W, CR );  Files.Write( W, NL );  

		WriteString( W, pmxcontrol.title );  Files.Write( W, CR );  Files.Write( W, NL );  

		WriteString( W, "Tc" );  Files.Write( W, CR );  Files.Write( W, NL );  

		WriteString( W, pmxcontrol.composer );  Files.Write( W, CR );  Files.Write( W, NL );  WriteString( W, "Abepl" );  
		Files.Write( W, CR );  Files.Write( W, NL );  i := 0;  
		WriteString(W,"\\input musixper\"); 	Files.Write( W, CR );  Files.Write( W, NL );
		WHILE i < nostaves DO 
			IF clefspec[i] = "p" THEN  Out.Ln(); Out.String("percussion");Out.Int(i,5); Out.Char(clefspec[i]); Out.Int(maxpart-i,5);
				b.percussionclef( maxpart - i , perclef, perline );  WriteString( W, perclef );  Files.Write( W, CR );  
				Files.Write( W, NL );  WriteString( W, perline );  Files.Write( W, CR );  
				Files.Write( W, NL );  

			END;  
			INC( i );  
		END;  
		(*	IF pmxcontrol.a3paper THEN *)
		WriteString( W, "%%w277m" );  Files.Write( W, CR );  Files.Write( W, NL );  WriteString( W, "%%h390m" );  
		Files.Write( W, CR );  Files.Write( W, NL );  
		(*	END; *)
		
		
		(*	Outclef; *)
		(* Files.Close( f );  Files.Register( f );  *)
	END ControlData;  


	PROCEDURE pmxDirections( idir, dirtypenr: LONGINT;  VAR res: ARRAY OF CHAR );  
	(* determines the Direction-string to be appended to a note in the PMX file. *)
	VAR dirtype: CHAR;  
		height: ARRAY 3 OF CHAR;  
	BEGIN 
		b.loesch( res );  dirtype := directions[idir, dirtypenr].dirtype;  
		
		CASE dirtype OF 
		"w": 
				IF (Strings.Length( directions[idir, dirtypenr].text ) > 0) THEN 
					IF (directions[idir, dirtypenr].note = 0) THEN directions[idir, dirtypenr].note := 1 END;  (* &&&&& *) 
					IF directions[idir, dirtypenr].placement = "h" THEN COPY( "12", height );  
					ELSIF directions[idir, dirtypenr].placement = "l" THEN COPY( "-6", height );  
					END;  
					COPY( " \zcharnote{", res );  Strings.Append( res, height );  Strings.AppendCh( res, "}" );  
					Strings.AppendCh( res, "{" );  Strings.Append( res, directions[idir, dirtypenr].text );  
					Strings.Append( res, "}\" );  
				END;  
		| "d": 
			  
				PMXdyn( directions[idir, dirtypenr].dyntype, res );  
		| "h": 
				PMXdyn( directions[idir, dirtypenr].wedgetype, res );  
		| "p": 
				b.loesch( res );  
				IF directions[idir, dirtypenr].pedaltype = "start" THEN COPY( " \PED\ ", res );  
				ELSIF directions[idir, dirtypenr].pedaltype = "stop" THEN COPY( " \DEP\ ", res );  
				END;  
		| "s": b.loesch(res); COPY (" oG ",res);
		
		
		| "c": b.loesch(res); COPY (" oC ",res);
		ELSE Out.Ln();  Out.String( "Direction type " );  Out.Char( dirtype );  Out.String( " not implemented." )
		
		END;  
	END pmxDirections;  

	PROCEDURE AnalyzeXML2( VAR R: Files.Rider );  
	(* Decodes MusicXML and stores data in a linked list. *)
	VAR dummy: ARRAY 256 OF CHAR;  (* geaendert von 128. 28.07.2019 *)
		rec, rec1: ARRAY 256 OF CHAR;  
		
		tag, endtag, between: ARRAY 256 OF CHAR;  
		note, voice, staff, savenote, novalues, length: LONGINT;  

		names, values: ARRAY 10 OF ARRAY 32 OF CHAR;   (* Change 9.11.2016 32->64 *)
		
		
	BEGIN 
		
		attnum := 0;  dirnum := 0;  part := 0;  measure := 0;  note := 0;  voice := 0;
		
		(******************** Read XML-Information ****************************)
		b.ReadUntilTx( R, ">", pmxcontrol.xml );  Out.Ln();  Out.String( pmxcontrol.xml );  
		b.ReadUntilTx( R, NL, dummy );  

		b.ReadUntilTx( R, ">", pmxcontrol.dtd );  Out.Ln();  Out.String( pmxcontrol.dtd );  
		b.ReadUntilTx( R, NL, dummy );  
		
		WHILE ~R.eof (* &  ( itags < 10) *) DO  (* Loop over tags *)
			b.ReadRecn( R, rec, length );   (*  Files.Read(R,c);  Versuch zur Loesung eines Problems *)
			
			(*	Out.Ln(); Out.String("nach ReadRecn : ");  Out.Int(itags,5); Out.Char("|"); Out.String(rec); Out.Char("|");  *)
			IF length = 0 THEN Out.Ln(); Out.String("empty record ");  
			ELSIF length > 0 THEN 
				
				IF ~R.eof & (length > 0) & (rec[Strings.Length( rec ) - 1] # ">") THEN 
					b.ReadRecn( R, rec1, length );  Strings.AppendCh( rec, BLANK );  Strings.Append( rec, rec1 );  
					length := Strings.Length( rec );  
				END;  
				(*	 Out.Ln(); Out.Int(itags,5); Out.String(" rec : "); Out.String(rec);  *)
				IF  (* (rec[1] # "/") & *) (rec[1] # "!") THEN INC( itags );  
					(* problem with lilypond test data *)
					IF tag = "<identification>" THEN b.SkipUntilPattern( R, endtag );  END;  
					(*****************************************************************)
					b.AnalyzeTag2( rec, tag, endtag, between, names, values, novalues );  
					
					 (* Out.Ln();Out.Int(zaehler,6); Out.String(rec); Out.String(tag); INC(zaehler);  *)
					 (*+******************************************************************)
					(*	Out.Ln();Out.Int(itags,5); Out.Char("|");Out.String(tag); *)
					
					(*	IF (tag = printtag) THEN SkipUntilPattern( R, endtag ) END; *)
					(* print information not useful for PMX *)
					(*  Verursacht ABbruch bei bestimmten Scans *)
					IF (tag = credittag) THEN b.SkipUntilPattern( R, endtag ) END;   (* credit information not useful for PMX *)
					IF (tag = defaultstag) THEN b.SkipUntilPattern( R, endtag ) END;  
					(* defaults information not useful for PMX *)
					IF (tag = "<appearance>") THEN b.SkipUntilPattern( R, endtag ) END;  
					(* appearance information not useful for PMX *)
					
					
					IF (tag = parttag) THEN 
						INC( part );  COPY( values[0], partlabel[part] );  
						(* part := 
						ORD( values[0, 1] ) - 48;  old code, only one digit for part *)
						(*	ExtractDigits( values[0], digits );  Strings.StrToInt( digits, part );  *)
						(*  Out.Ln(); Out.String (" global id of part : "); Out.Int(part,5);  *)
						note := 0;  maxpart := b.Max( part, maxpart );  voice := 0;  staff := 0;  measure := 0;  savenote := 0;  
						staves[part] := 1;  
					END;  
					IF (tag = measuretag) THEN 
						(*	Strings.StrToInt( values[0], measure );   *)
						IF measure < measurelimit THEN 
							INC( measure );  lastnote := 0;  note := 0;  maxnote[part, 1, measure] := 0;  
							maxnote0[part, 1, measure] := 0;  maxnote[part, 2, measure] := 0;  voice := 0;  
							minnote[part, 1, measure] := 100;  maxnote0[part, 2, measure] := 0;  
							minnote[part, 2, measure] := 100;  
							(* maxnote total count of notes/rests/forwards in part,staff,measure. 
							   maxnote0 total count of notes/rests/forwards in part,staff,measure for voice=0. *)
							
							
							(*	 Out.Ln(); Out.String (" global id of measure : "); Out.Int(measure,5);  *)
							maxmeasure := b.Max( measure, maxmeasure );  voice := 0;  staff := 0;  
							(* attnum := 0; dirnum := 0; *)
						ELSE Out.Ln();  Out.String( "maximum measure reached; HALT! " );  HALT( 20 );  
						END;  
					END;  
					IF (tag = stavestag) THEN staves[part] := 1;  Strings.StrToInt( between, staves[part] );  END;  
					IF (tag = directiontag) THEN dirnum := lastdirnum;  INC( dirnum );  maxdir := b.Max( maxdir, dirnum );  END;  
					IF (tag = eodirectiontag) THEN lastdirnum := dirnum;  dirnum := 0;  END;  
					(*********************************************************************************)
					IF (tag = attributestag) THEN attnum := lastattnum;  INC( attnum );  END;  
					IF (tag = eoattributestag) THEN lastattnum := attnum;  attnum := 0;  END;  
					(**********************************************************************************)
					IF (tag = notetag) OR (tag = forwardtag) THEN note := lastnote;  INC( note );  END;   (* Aenderung27. April *)
					IF (tag = eonotetag) THEN lastnote := note;  note := 0 END;  
					(**********************************************************************************)
					IF (tag = "<forward>") THEN (* <forward ist ein blind rest> *)
						note := lastnote;  INC( note );  INC( lastnote ); (* Out.Ln();  Out.String( "note after forward " );  
						Out.Int( part, 5 );  Out.Int( staff, 5 );  Out.Int( voice, 5 );  Out.Int( measure, 5 );  Out.Int( note, 5 );  *)
						b.StoreTag( itags, notetag, eonotetag, part, voice, measure, note, attnum, dirnum, lastnote, novalues, names, values, between );  
						(********************************************************************************)
						(*************************Store Information in  Tag "new"******************************)
						INC( itags );  
						b.StoreTag( itags, "<rest/>", eonotetag, part, voice, measure, note, attnum, dirnum, lastnote, novalues, names, values, between );  

						INC( itags );  
					END;  
					b.StoreTag( itags, tag, endtag, part, voice, measure, note, attnum, dirnum, lastnote, novalues, names, values, between );  
					IF (tag = "</forward>") THEN 
						INC( itags );  
						b.StoreTag( itags, eonotetag, eonotetag, part, voice, measure, note, attnum, dirnum, lastnote, novalues, names, values, between );  
					END;  
				END;  
			END;  
		END;  
		part := 1;  
		WHILE part <= maxpart DO 
			nostaves := nostaves + staves[part];   (* Aenderung 5.11.2015 *)
			INC( part );  
		END;  
	
	END AnalyzeXML2;  
(* PROCEDURE ListMeasure*;
		VAR i, measure : INTEGER;
		BEGIN
		Out.Ln(); Out.String("ListMeasure"); Out.Ln();
		
			i := 1; WHILE i < maxmeasure DO Out.Ln(); Out.Int(i,5); INC(i); measure := i; 
		Out.Char(	measures[measure].clefchange[1]);
			Out.Char(	measures[measure].clefchange[2]);
			Out.Char(	measures[measure].clefchange[3]);
			Out.Char(	measures[measure].clefchange[4]);END;
		END ListMeasure; *)




BEGIN 
	COPY( "<part>", parttag );  COPY( "<measure>", measuretag );  COPY( "<attributes>", attributestag );  
	COPY( "</attributes>", eoattributestag );  COPY( "<divisions>", divisionstag );  COPY( "<key>", keytag );  
	COPY( "<fifths>", fifthstag );  COPY( "<time>", timetag );  COPY( "<beats>", beatstag );  
	COPY( "<beat-type>", beattypetag );  COPY( "<staves>", stavestag );  COPY( "<clef>", cleftag );  
	COPY( "<sign>", signtag );  COPY( "<line>", linetag );  COPY( "<clef-octave-change>", clefoctavetag );  
	COPY( "<note>", notetag );  COPY( "<pitch>", pitchtag );  COPY( "<step>", steptag );  
	COPY( "<octave>", octavetag );  COPY( "<duration>", durationtag );  COPY( "<voice>", voicetag );  
	COPY( "<type>", typetag );  COPY( "<dot/>", dottag );  COPY( "<stem>", stemtag );  
	COPY( "<notations>", notationtag );  COPY( "<lyric>", lyrictag );  COPY( "<syllabic>", syllabictag );  
	COPY( "<text>", texttag );  COPY( "<accidental>", accidentaltag );  COPY( "<rest/>", resttag );  
	COPY( "<chord/>", chordtag );  COPY( "<backup>", backuptag );  COPY( "<slur>", slurtag );  
	COPY( "<staff>", stafftag );  COPY( "<fermata>", fermatatag );  
	COPY( "<print>", printtag );  COPY( "<direction>", directiontag );  COPY( "<tied>", tiedtag );  
	COPY( "<beam>", beamtag );  COPY( "<tuplet>", tuplettag );  COPY( "<staccato/>", staccatotag );  
	COPY( "<words>", wordstag );  COPY( "<repeat>", repeattag );  COPY( "</note>", eonotetag );  
	COPY( "</measure>", eomeasuretag );  COPY( "<barline>", barlinetag );  COPY( "<ending>", endingtag );  
	COPY( "<line>", cleflinetag );  COPY( "<actual-notes>", actualtag );  COPY( "<normal-notes>", normaltag );  
	COPY( "<bar-style>", barstyletag );  COPY( "<dynamics>", dynamicstag );  COPY( "<work>", worktag );  
	COPY( "<identification>", identificationtag );  COPY( "<defaults>", defaultstag );  COPY( "<credit>", credittag );  
	COPY( "<grace/>", gracetag );  COPY( "</direction>", eodirectiontag );  COPY( "<grace>", gracetag2 );  
	COPY("<cue/>",cuetag);
	 (*  initialize notes type conversion XML -> PMX *)
	pmxdur[0] := "0";  pmxdur[1] := "2";  pmxdur[2] := "4";  pmxdur[3] := "8";  pmxdur[4] := "1";  pmxdur[5] := "3";  
	pmxdur[6] := "6";  pmxdur[7] := "7"; (*128th note ? *)
	
	(* Initialization of voices count *)
	i := 0;  j := 0;  ingrace2pmx := 0;  NEW( directions );  NEW( measures );  NEW( voicemeasure );  
	NEW( minvoice );  NEW( maxvoice );   NEW( notes ) ; 

	itags := 0;  lfdnr := 1;   (* outarray(minvoice); *)
	(* Initialize tie numbering *)
	i := 0;  
	WHILE i < 24 DO 
		j := 0;  
		WHILE j < 10 DO INCL( b.tieunusdnum[i, 0], j );  INCL( b.tieunusdnum[i, 1], j );  INC( j );  END;  
		INC( i );  
	END;  
	(* Initialization of notes counter *)
	
	part := 0;  
	WHILE part < 24 DO 
		measure := 1;  
		WHILE measure < measurelimit DO 
			maxnote[part, 1, measure] := 0; 
			maxnote[part, 2, measure] := 0; 
			minnote0[part, 1, measure] := 100;  
			minnote0[part, 2, measure] := 100;  
			minnote1[part, 1, measure] := 100;  
			minnote1[part, 2, measure] := 100;  
			minnote[part, 1, measure] := 100;  
			minnote[part, 2, measure] := 100;  

			maxnote0[part, 2, measure] := 0;  
			maxnote0[part, 1, measure] := 0;  
			maxnote1[part, 2, measure] := 0;  
			maxnote1[part, 1, measure] := 0;  
			minvoice[part, measure] := 10;  
			maxvoice[part, measure] := 0;  
			INC( measure );  
		END;  
		INC( part );  
	END;  
	unix := TRUE;   (* CRLF end of line; for LF end of line set unix := TRUE. *)
	b.unix := unix; (* inform MODULE "b" about end of line character. *)
	(********************* Read CommandLine ***************)
	(* commandO; never activate   call externally for Oberon-Version   ***********)
	(*****************************************************)
	(*  commandX;    *)  (* activate for Windows-Exe Version *******)
	(**************** ******************************)
	CommandU;     (* activate for Linux-Binary Version *************)
	
END Testbed.
