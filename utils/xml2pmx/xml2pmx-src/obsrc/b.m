MODULE b;
(* Achtung "PrintRange" eingebaut, zusaetzliche Importe *)

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
	
IMPORT Files := MyFiles, Strings := Strings1, Fifo, Out;
TYPE LONGINT = INTEGER;
CONST CR= 0DX; NL = 0AX; BLANK = 20X; DBLQUOTE = 22X; TAB=09X;
TYPE
	Tag* = POINTER TO TagDesc;   (* List structure for data acquisition *)
	TagDesc* = RECORD 
				nr*: LONGINT;   (* counter of tags *)
				chord*: CHAR;  cue* : BOOLEAN;
				arpeggio*: BOOLEAN;  
				part*, staff*, voice*, voice12*, measure*, dirnum*, lastnote*, nextnote*, nextvoice*, attnum*, note*, novalues*: LONGINT;  
				(* to be used as indices  of ARRAY "notes" *)
				tagname*: ARRAY 32 OF CHAR;  
				endtag*: ARRAY 32 OF CHAR;  
				next*: Tag;  
				names*: ARRAY 10 OF ARRAY 32 OF CHAR;  
				values*: ARRAY 10 OF ARRAY 32 OF CHAR;  
				between*: ARRAY 64 OF CHAR;  
				used*: BOOLEAN;  
				duration*, backup*, voicetime*, from*, to*: INTEGER;  
				probj*: CHAR;   (* when "n" the complete object has to be removed *)
				grace*: INTEGER;   (* counts consecutive grace notes *)
			newclef* : CHAR;  (* only for notes or rests. the new clef is applied to this note. *)
			END;  
	FIFO* = RECORD 
				first*, last*: Tag;  
			END; 
	Node = POINTER TO NodeDesc;  
	NodeDesc = RECORD 
				key: ARRAY 64 OF CHAR;  
				count: INTEGER; 
				n : Tag; 
				next: Node;  
			END;  

			
VAR q* : FIFO; voutput* : BOOLEAN; nostaves* : LONGINT; unix* : BOOLEAN;
sout : ARRAY 64 OF CHAR; (* target file path and directory *)
tieunusdnum*: ARRAY 27 OF ARRAY 3 OF SET;
tieq: ARRAY 27 OF ARRAY 2 OF Fifo.FIFO; 

    akeys*, mkeys* : ARRAY 27 OF INTEGER;
  nstr : ARRAY 8 OF LONGINT;   (* global for number of verses in a liedtext.*)
	text: ARRAY 8 OF ARRAY 6  OF ARRAY 1024 OF CHAR;   (* texts for at most 6 verses and 8 voices choir; length limited to  1023 Chars *)
(*			PROCEDURE PrintRange*;
	VAR S : Texts.Scanner; von, bis, i : LONGINT; n : Tag;
	BEGIN
	Texts.OpenScanner (S, Oberon.Par.text, Oberon.Par.pos);
	Texts.Scan(S); von := S.i; Texts.Scan(S); bis  := S.i;
		n:= q.first; 
		WHILE (n.next # NIL) & (i < von ) DO n := n.next; INC(i); END;
	WHILE (n.next # NIL) & ( i < bis ) DO OutTag(n,TRUE); n := n.next; INC(i); END;
END PrintRange; *)

PROCEDURE slur2PMX* ( n: Tag;  VAR pmxslur: ARRAY OF CHAR ; outputset : SET);  
	(* Translates a beginning or ending slur from XML to PMX. *)
	
	VAR c, cs : CHAR;  
		type, number, placement: ARRAY 32 OF CHAR;  inumber : LONGINT;
	BEGIN 
		loesch( pmxslur );  FindAtt( n, "type", type );  FindAtt( n, "number", number );  
		IF ( number # "" ) THEN
				Strings.StrToInt(number, inumber);
				cs := CHR(inumber + 65)
				ELSE cs := "A"; 
		END;
		FindAtt( n, "placement", placement );  
		IF (type # "continue") THEN (* gibt es das ueberhapt? *)

			COPY( BLANK, pmxslur );  
			IF (type = "start") THEN c := "(";  
			ELSIF (type = "stop") THEN c := ")"
                        ELSE c := "?"
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
			
	END slur2PMX;  
			PROCEDURE tied2PMX* ( n: Tag;  type: ARRAY OF CHAR;  VAR pmxtied: ARRAY OF CHAR;  ps, voice: LONGINT);  
	(* Translates a beginning or ending tie from XML to PMX.
	    orientation = "d" => note with stem down yields tie with orientation "u".
	 orientation = "u" => note with stem up yields tie with orientation "l". *)
	VAR c: CHAR;  
		number: ARRAY 4 OF CHAR;  orient : ARRAY 10 OF CHAR; orientation : CHAR;
		nt: Fifo.Node;  
	
	BEGIN 
		loesch( pmxtied );  FindAtt( n, "number", number );
		FindAtt( n, "orientation", orient );  
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
			NEW( nt );  nt.key := Fifo.smallfree( tieunusdnum[ps, voice] );  
			EXCL( tieunusdnum[ps, voice], nt.key );  
			Fifo.Enqueue( tieq[ps, voice], nt );  
			Strings.IntToStr( nt.key, number );  
			Strings.Append( pmxtied, number );  
			Strings.AppendCh( pmxtied, orientation );  
		ELSIF (type = "stop") THEN c := "}";  Strings.AppendCh( pmxtied, c );  

			nt := Fifo.DequeuedNode( tieq[ps, voice] ); 
			IF nt # NIL THEN INCL( tieunusdnum[ps, voice], nt.key );  (* avoid nt undefined *)
			Strings.IntToStr( nt.key, number );  Strings.Append( pmxtied, number );  END;
		ELSE Out.String( "wrong type of tie " );  
		END;  
		Strings.AppendCh( pmxtied, BLANK );  
	END tied2PMX;  


	
PROCEDURE lyric*( ps: LONGINT;  VAR n: Tag );  
	VAR endtag: ARRAY 32 OF CHAR;  
		number, syllabic: ARRAY 10 OF CHAR;  istr : LONGINT;
	BEGIN 
	IF (n.tagname = "<lyric>") (* & (number = "1") *) THEN 

		COPY( n.endtag, endtag );  FindAtt( n, "number", number ); 
		Strings.StrToInt (number,istr);
		nstr[ps] := Max(istr,nstr[ps]); 
					
			WHILE (n.tagname # endtag) DO 
				IF n.tagname = "<syllabic>" THEN COPY( n.between, syllabic );  END;  
				IF n.tagname = "<text>" THEN Strings.Append( text[ps,istr], n.between );	END;  
				n := n.next;  
			END;  
			IF (syllabic = "single") OR (syllabic = "end") THEN Strings.AppendCh( text[ps,istr], " " );  END;  
			IF (syllabic = "begin") OR (syllabic = "middle") THEN Strings.AppendCh( text[ps,istr], "-" );  END;  
		END;  
	END lyric;  
		PROCEDURE writetext*;  
	VAR i, j, istr, column: LONGINT;  breakline: BOOLEAN;  f: Files.File;  r: Files.Rider;  
		s: ARRAY 4 OF CHAR;  
	BEGIN 
	Strings.Append(sout,"songtext.txt");
Out.Ln();	Out.String("Storage for verses : "); Out.String(sout);
		i := 0;  f := Files.New( sout );  Files.Set( r, f, 0 );  
		WHILE i < nostaves DO 
		istr := 1;
		WHILE istr <= nstr[i] DO (*$$$$$$$$$$$$$*)
			IF (Strings.Length( text[i,istr] ) > 0) THEN 
				
		(*		IF ~unix THEN Files.Write( r, CR );  	END;   *)
				
				Files.Write( r, NL );  Strings.IntToStr( i + 1, s ); 
				 Files.WriteBytes( r, s, Strings.Length( s ) ); 
				Strings.IntToStr(istr,s); Files.Write(r,"|"); 
				Files.WriteBytes( r, s, Strings.Length( s ) ); 
				 j := 0;  column := 0;  
				WHILE j < Strings.Length( text[i,istr] ) DO 
					
					REPEAT 
						IF (text[i,istr, j] # "_") THEN Files.Write( r, text[i,istr, j] );  END;  
						INC( j );  INC( column );  breakline := (column > 80) & (text[i,istr, j] = BLANK);  
						IF breakline THEN 
							IF ~unix THEN Files.Write( r, CR );  END;  
							Files.Write( r, NL );  breakline := FALSE;  column := 0;  
						END;  
					
					UNTIL (text[i,istr, j] = 0X) OR (text[i,istr, j] = BLANK);  
					IF text[i,istr, j] = BLANK THEN Files.Write( r, BLANK );  END;  
								
					INC( j );  
				END;  
			END; 
						
			
			INC(istr);	
				IF ~unix THEN Files.Write( r, CR );  END;  
				Files.Write( r, NL ); 
		END;
				IF ~unix THEN Files.Write( r, CR );  END;  				Files.Write( r, NL ); 
			INC( i )
		END;  
		Files.Close( f );  Files.Register( f );  
	END writetext;  





	PROCEDURE SortTags*;  
	(* Command to list the different tags after calling the comman "Testbed.AalyzeXML" *)
	VAR n: Tag;  first, st: Node;  
	BEGIN 
		first := NIL;
		n := q.first;  
		WHILE n # NIL DO NEW( st );  COPY( n.tagname, st.key );  InsertRanked( first, st );  n := n.next;  END;  
		st := first;  Out.Ln(); Out.String("===============================================");
		Out.Ln();  Out.String( " Sorted List of Tags " );  
		WHILE (st # NIL ) DO 
			IF (st.key[1] # "/") THEN Out.Ln();  Out.String( st.key );  Out.Int( st.count, 5 );  END;  
			st := st.next
		END;  
	
	END SortTags;  

	PROCEDURE InsertRanked( VAR first: Node;  new: Node );  
	(* Builds up a sorted list according to Node.key, counts the occurrences of the different keys *)
	VAR n: Node;  
	BEGIN  (* new # NIL *)
		IF (first = NIL ) OR (new.key < first.key) THEN new.next := first;  new.count := 1;  first := new 
		ELSE 
			n := first;  
			WHILE (n.next # NIL ) & (new.key >= n.next.key) DO n := n.next END;  
			IF (n.key # new.key) & (new.key[0] # 0X) THEN  (* store only distinct values *)
				new.next := n.next;  n.next := new;  new.count := 1;  
			ELSE INC( n.count )
			END;  
		END 
	END InsertRanked;  


(* PROCEDURE SortKey( q : FIFO  );
	(* Sort notes according to Measure (ascending), Part(descending), Staff(descending) and Voice(descending).            *)
	VAR n : Tag; st : Node;
	BEGIN
	n := q.first;
	WHILE n.next # NIL DO
	IF ( n.tagname = "<notes>" ) THEN
	NEW(st); 
	mpsvn2String(n.measure, n.part, n.staff, n.voice, n.note,st.key);
	st.n := n; maxtag := Max(n.nr,maxtag);
	InsertRanked (nfirst,st)
	END;
	n := n.next;
	END;
	END SortKey; *)

(*	PROCEDURE arplen*( n: Tag;  VAR maxarp: LONGINT );  
	(* When n is the first note of a sequence of arpeggionotes, then maxarpe is the number of arpeggio notes.*)
	VAR m: Tag;  
	BEGIN 
		maxarp := 0; (* OutTag( n, TRUE );   *)
		IF n.arpeggio = 1 THEN 
			REPEAT findnextnote( n, m );  maxarp := Max( maxarp, n.arpeggio );  n := m;  UNTIL (m.arpeggio = 0)   OR ( m = NIL  )
		
		END;  
	END arplen;  *)

	PROCEDURE gracelen*( n: Tag;  VAR maxgrace: LONGINT );  
	(* When n is the first note of a sequence of gracenotes, then maxgrace is the number of grace notes.*)
	(* n.grace numbers the grace notes in a concsecutive series *)
	VAR m: Tag;  
	BEGIN 
	
		maxgrace := 0; (*  OutTag( n, TRUE );  *)
		IF n.grace = 1 THEN 
			REPEAT findnextnote( n, m ); 
			 IF ( n.chord # "c" ) THEN maxgrace := Max( maxgrace, n.grace ); END; 
			 n := m;  UNTIL m.grace = 0;  
			(* IF statement to remove chord notes from a series of grace notes; 11.2.2017 *)
		
		END;  
	END gracelen;  

	PROCEDURE Outset*( s: SET;  VAR count: LONGINT;  VAR elemmap: ARRAY OF LONGINT );  
	(* prints a set in System.Log and returns the # of elements and a vector of elements in ascending order. *)
	VAR i, j: LONGINT;  
	BEGIN 
	IF voutput THEN	Out.Char( "{" ); END;  count := 0;  i := 0;  
		REPEAT INC( i ) UNTIL (i = 31) OR (i IN s);  
		IF voutput THEN Out.Int( i, 2 ); END;  
		IF (i IN s) THEN elemmap[count] := SHORT( i );  INC( count );  END;  
		INC( i );  
		WHILE i < 32 DO 
			IF i IN s THEN IF voutput THEN Out.Char( "," );  Out.Int( i, 2 ); END; elemmap[count] := SHORT( i );  INC( count );  END;  
			INC( i );  
		END;  
		IF voutput THEN 
			Out.Char( "}" );  Out.Int( count, 5 );  Out.Char( "|" );   j := 0;  
			WHILE j < count DO Out.Int( elemmap[j], 5 );  INC( j ) END;  
		END;
	END Outset;  

	PROCEDURE findnextnote*( n: Tag;  VAR m: Tag );  
	(* finds the next note "m" after "n", *)
	BEGIN 
		m := n; (* Korrektur 22.06.2019 *)
		REPEAT m := m.next 
		UNTIL (m.next = NIL ) OR (m.tagname = "<note>");  
		
		(*	Out.Ln();  Out.String( "findnextnote : " );  Out.Int(m.nr,5); Out.String( m.tagname );  *)
		
		
	END findnextnote;  
		PROCEDURE findnextnotestaff*( n: Tag;  VAR m: Tag;  number : LONGINT);  
	(* finds the next note "m" after "n", *)
	
	BEGIN 
		m := n; (* Korrektur 22.06.2019 *)
		REPEAT m := m.next; 
			UNTIL (m.next = NIL ) OR ( (m.tagname = "<note>") & ( m.staff = number ) ) ;  
		
		(*	Out.Ln();  Out.String( "findnextnote : " );  Out.Int(m.nr,5); Out.String( m.tagname );  *)
		
		
	END findnextnotestaff;
	
	PROCEDURE compareTag*;
	VAR n : Tag;
	VAR i : LONGINT;
	BEGIN
        i := 0;
	n := q.first;
	WHILE  i < 1000 DO
	OutTag(n,TRUE); 
	n := n.next;
	INC(i);
	END;

	  
	    
	END compareTag;  


PROCEDURE OutTag*( n: Tag;  ln: BOOLEAN );  
	(* Writes the properties of one tag into System.Log. *)
	VAR i: LONGINT;  
	
	BEGIN 
		IF (n # NIL ) (* & (n.part # 0) *) THEN 
			IF ln THEN Out.Ln();  END;  
			Out.Int( n.nr, 5 );  Out.String( " :  " );   (* 1 *)
			Out.Int( n.part, 5 );  Out.String( " :  " );   (* 2 *)
			Out.Int( n.staff, 5 );   (* 3 *)
			Out.Int( n.voice , 5 );   (* 4 *) 
		(*	Out.Char("|");	Out.Int( n.voice12 , 5 );   (* 4 *) *)

			Out.Int( n.measure, 5 );   (* 5 *)
			Out.Int( n.note, 5 );   (* 6 *)
			Out.Int( n.grace, 5 );  (* 7 *)
			Out.Int( n.from, 5 );  Out.Int( n.to, 5 );   (* 8, 9  *)
			IF n.chord # 0X THEN Out.Char( n.chord );  ELSE Out.Char("-") END;  (* 10 *)
	(*		IF n.arpeggio THEN Out.String(" ? "); END;  (* 11 *) *)
			Out.Int( n.dirnum, 5 );   (* 11  *)
			Out.Int( n.lastnote, 5 );   (* 12 *)
			Out.Int( n.nextnote, 5 );   (* 13 *)
			Out.Int( n.nextvoice, 5 );   (* 14 *)
			Out.Int( n.attnum, 5 );   (* 15 *)
			Out.Char(n.newclef); (* 17.10.2020 new implementation of clefs *)
	(*		Out.Int( n.arpeggio, 5 ); *)  
	Out.Char( n.probj );  (* 17 *)
(*			IF n.cue THEN Out.String("cue"); END; *)
			
	(*	Out.String("backup");	Out.Int(n.backup,5); *)
			(*		Out.String(" backup,duration,voicetime, from, to : ");Out.Int( n.backup, 5 ); 
				Out.Char("|"); Out.Int(n.duration,5);	Out.Char("|");Out.Int(n.voicetime,5);	
								Out.Char("|");Out.Int(n.from,5);Out.Char("|");Out.Int(n.to,5);  *)			
			Out.String( n.tagname );   (* Out.String( n.endtag );  *)
			i := 0;  
			WHILE (i < n.novalues) DO 
				Out.String( n.names[i] );  Out.Char( "=" );  Out.String( n.values[i] );  Out.Char( "|" );  INC( i )
			END;  
			
			Out.String( n.between );  
			
			IF n.used THEN Out.Char( "+" );  
			END;  
		
		ELSE Out.Ln();  Out.String( "Tag ist NIL. " );  
		
		END 
	END OutTag;  



		PROCEDURE StoreTag*( itags: LONGINT;  tag, endtag: ARRAY OF CHAR;  part, voice, measure, note, attnum, dirnum, lastnote : LONGINT;  
									  novalues: LONGINT;  names, values: ARRAY OF ARRAY OF CHAR;  between: ARRAY OF CHAR );  
	(* Stores a decoded MusicXML tag as element of a linked list. *)
	VAR new: Tag;  k: LONGINT;  
		printobj: ARRAY 32 OF CHAR;  
	BEGIN 
		NEW( new );  new.nr := itags;  COPY( tag, new.tagname );  COPY( endtag, new.endtag );  new.part := part;  
		new.voice := voice;  new.used := FALSE;  new.measure := 0;  
		(*			Out.Ln(); Out.String("StoreTag : itags, tag, endtag, part, measure, note, lastnote ");
Out.Int(itags,5); Out.String(tag); Out.Int(part,5); Out.Int(measure,5); Out.Int(note,5); Out.Int(lastnote,5); Out.Char("|");Out.String(between); 
			
			 *)
		
		
		IF (new.tagname # "<part>") THEN 
			new.measure := measure;  
			IF (new.tagname # "<measure>") THEN 
				new.note := note;  new.attnum := attnum;  new.dirnum := dirnum;  new.lastnote := lastnote;  
			END;  
		END;  
		k := 0;  new.novalues := novalues;  
		WHILE (k < novalues) DO COPY( names[k], new.names[k] );  COPY( values[k], new.values[k] );  INC( k );  

		END;  
		FindAtt( new, "print-object", printobj );  
		IF (new.tagname = "<note>") & (printobj = "no") THEN new.probj := "n" ELSE new.probj := BLANK;  END;  
		(* COPY( between, new.between );  *) ;  k := 0;  
		WHILE k < 64 DO new.between[k] := between[k];  INC( k );  END;  
		new.between[k - 1] := 0X;  
		(* Out.Ln();Out.String("**********************OutTag************************************"); Out.Ln(); 
			   *)
		(*	OutTag( new );    *)
		Enqueue( q, new );  
	END StoreTag;  
		PROCEDURE Enqueue( VAR q: FIFO;  n: Tag );  
	(* puts Tag "n" into the list "q" *)
	BEGIN 
		n.next := NIL;  
		IF q.first # NIL THEN q.last.next := n ELSE q.first := n END;  
		q.last := n
	END Enqueue;  


	PROCEDURE FindAtt*( n: Tag;  name: ARRAY OF CHAR;  VAR value: ARRAY OF CHAR );  
	(* Properties of a tag are stored as name-value pairs. The procedure finds the value of an attribute by its name. *)
	VAR i: LONGINT;  
	BEGIN 
		i := 0;  
		WHILE (i < n.novalues) & (n.names[i] # name) DO INC( i ) END;  
		IF (n.names[i] = name) THEN COPY( n.values[i], value );  
				ELSE  (* Out.Ln();  Out.String( " FindAtt : name not found " );  Out.String( name );  OutTag( n );  *)
		END;  
	END FindAtt;  
(*	PROCEDURE FindNextNote* (VAR n : Tag); (*  for removing cue notes ; finds next note after "n" or "n" when it is a note *)
	VAR
	BEGIN
	IF n.tagname # "<note>" THEN
		REPEAT n:= n.next UNTIL ( (n.tagname = "<note>" ) & ( n.grace = 0 ) )  OR ( n.next = NIL ) ; (* &&&&& *)
  ELSE 
  END;
  END FindNextNote; *)
	
PROCEDURE PosInStaff*(pitchnote : CHAR;pitchoctave : INTEGER; clef : CHAR) : LONGINT;
(* calculates the position of the note "pitchnote/pitchoctave" in the staff according to clef. *)
VAR diff : LONGINT;
BEGIN

CASE clef OF
"t" : diff := DiaDiff(pitchnote,pitchoctave,"g",4) + 2;
| "b" : diff := DiaDiff(pitchnote,pitchoctave,"f",3) + 6;
| "a" : diff := DiaDiff(pitchnote,pitchoctave,"c",4) + 4;
| "n" : diff := DiaDiff(pitchnote,pitchoctave,"c",4) + 6;
ELSE Out.Ln(); Out.String("PosinStaff: clef not implemented : "); Out.Char(clef);
END; RETURN diff;
END PosInStaff;
PROCEDURE DiaDiff(pitchnote : CHAR; pitchoctave : INTEGER; refnote : CHAR; refoctave : INTEGER) : LONGINT;
(* Calculates the diatonic difference between to notes in PMX notation. *)
VAR
diffnote, diffoctave : LONGINT;
BEGIN

diffoctave := pitchoctave - refoctave;
diffnote := ORD(pitchnote) - ORD(refnote);
RETURN diffnote + diffoctave * 7;
END DiaDiff;

PROCEDURE MinDist*(x : INTEGER; pos : ARRAY OF INTEGER; posmax : INTEGER) : INTEGER;
(* finds the index of "pos" with minimum difference to "x". *)
VAR i, imin, delta : INTEGER;

BEGIN
delta := ABS(x-pos[0]); imin := 0;
i := 1; 
WHILE i < posmax DO 
	IF ABS(x-pos[i]) < delta 
		THEN imin := i; delta := ABS(x-pos[i]); END; 
		INC(i) 
	END;
RETURN imin;
END 
MinDist;
PROCEDURE testMinDist*;
VAR pos : ARRAY 5 OF INTEGER;
	
BEGIN
pos[0] := 57;
pos[1] := 163;
pos[2] := 273;
pos[3] := 381;
Out.Ln(); Out.String("MinDIst test "); Out.Int (MinDist(279,pos,4),5);


END testMinDist;
	
PROCEDURE pmxTremolo* (pitchnote : CHAR; pitchoctave : INTEGER; stem, clef : CHAR; nobeams : LONGINT; pmxdur0 : CHAR; VAR res : ARRAY OF CHAR);
	VAR i,pos : LONGINT; offset : ARRAY 4 OF CHAR;
	BEGIN
	
	pos := PosInStaff(pitchnote,pitchoctave,clef);
	Strings.IntToStr(pos,offset);
	IF stem ="d" THEN stem:= "l"; 	END;
    IF stem = 0X THEN stem := "l" END; 
    IF (pmxdur0 = "0") 
    	THEN 
    		IF  (pos  >  4) THEN 
    			stem := "l" ELSE stem := "u"; END; 
    END;
	 	res[0] := "\";
	CASE pmxdur0 OF
	"0" : Strings.AppendCh(res,"T");	
	| "2","4" : Strings.AppendCh(res,"t");
 	ELSE
	Out.String(" pmxTremolo ; unknown pmx duration : "); Out.Char(pmxdur0);
	END;
	i := 0; WHILE i < nobeams DO Strings.AppendCh(res,"r"); INC(i) END;
	Strings.AppendCh(res,"m"); 
	Strings.AppendCh(res,stem); 
	Strings.AppendCh(res,"{"); 
	Strings.Append(res,offset); Strings.Append(res,"}\ ");
	
	END pmxTremolo;
	
	
PROCEDURE ReadStringUntil*(VAR W : Files.Rider; split : CHAR; VAR s : ARRAY OF CHAR);
(* Formatted read of STRING until "split" Character  from file *)
VAR i : LONGINT; c : CHAR;
BEGIN
loesch(s);
i := 0; Files.Read(W,c);
REPEAT
 	s[i] := c;
 	INC(i); 
 	Files.Read(W,c);  
	 UNTIL c = split;
s[i] := 0X; 
END ReadStringUntil;


	PROCEDURE APPzca* (VAR s,t : ARRAY OF CHAR);
	(* Appends a 2nd \zcharnote-element to the first one. *)
	VAR i,j : LONGINT; 
	BEGIN
	i := Strings.Length(s);
(*	Out.Ln(); Out.String("vorher : ");Out.String(s); Out.Ln(); Out.String(t); *)
	s[i-2] := BLANK; s[i-1] := 0X;

	j := 0; WHILE t[j] # "}" DO INC(j) END;
	i := i-1; j := j+2; REPEAT s[i] := t[j]; INC(i); INC(j) UNTIL t[j] = 0X ;
(*	Out.Ln(); Out.String("nachher : ");Out.String(s);  *)
	END APPzca;
	
	PROCEDURE IsSep(c : CHAR) : BOOLEAN;

BEGIN
RETURN ( c = 20X ) OR ( c = 22X ) OR ( c = "," )

END IsSep;
PROCEDURE FindToken* (VAR s, t : ARRAY OF CHAR; VAR pos : LONGINT);
VAR j : LONGINT;
BEGIN
IF IsSep(s[pos] ) THEN REPEAT INC(pos) UNTIL (pos = Strings.Length(s)-1 ) OR ~IsSep(s[pos]); (* first CHAR # separator found *)
END;
j := 0; t[j] := s[pos];

WHILE (pos < Strings.Length(s) )  & ~IsSep (s[pos] ) DO
t[j] := s[pos];
 INC(pos); INC(j);  END; 
  t[j] := 0X;

END FindToken;



	PROCEDURE strbetween*( s: ARRAY OF CHAR;  VAR name: ARRAY OF CHAR;  VAR pos: LONGINT );  
	VAR i, j: LONGINT;  
	BEGIN 
		i := pos;  
		WHILE (i < Strings.Length( s )) & (s[i] # DBLQUOTE) DO INC( i ) END;   (* Find first doublequote *)
		j := 0;  INC( i );  
		IF ( i = Strings.Length( s ) ) 
		THEN 
			pos := -1 
		ELSE
			WHILE (i < Strings.Length( s )) & (s[i] # DBLQUOTE) DO name[j] := s[i];  INC( i );  INC( j ) END;  
			pos := i + 1;  name[j + 1] := 0X;  
			IF ( i = Strings.Length( s ) ) THEN pos := -1  END
		
		END;
	END strbetween;  
	PROCEDURE Copywo*( VAR fin, fout: Files.File );  
	(* Copies a File and eliminates multiple BLANKs. *)
	VAR ch: CHAR;  rin, rout: Files.Rider;  column: LONGINT;  
	BEGIN 
		Files.Set( rin, fin, 0 );  Files.Set( rout, fout, 0 );  column := 0;  Files.Read( rin, ch );  
		WHILE ~rin.eof DO 
			Files.Write( rout, Strings.OberonToISO[ORD( ch )] );  INC( column );  
			IF (ch = NL) THEN column := 0;  END;  
			IF (column > 100) & (ch = BLANK) THEN column := 0;  Files.Write( rout, CR );  Files.Write( rout, NL );  END;  
			
			(* 	IF ch =Strings.CR THEN Files.Write(R,NL); END; *)
			IF (ch = BLANK) THEN 
				WHILE (ch = BLANK) DO Files.Read( rin, ch );  END;  
			ELSE Files.Read( rin, ch );  
			
			END;  ;  
		
		END;  
	END Copywo;  

	PROCEDURE close*( pattern: ARRAY OF CHAR;  VAR closetag: ARRAY OF CHAR );  
	(* creates the closing tag out of the start tag: e.g. </note> out of <note>  *)
	VAR i: INTEGER;  
	BEGIN 
		closetag[0] := "<";  closetag[1] := "/";  i := 1;  
		WHILE i < Strings.Length( pattern ) DO closetag[i + 1] := pattern[i];  INC( i );  END;  
		closetag[i + 1] := 0X;  
	END close; 
PROCEDURE clefPMX*( clefsign: CHAR;  clefline: INTEGER; VAR c, otherclef : CHAR);
	 
	BEGIN 
		c := "t";   (* default: violin clef *)
		
	(*	Out.Ln(); Out.String(" clefsign, clefline, otherclef, pmxsymbol : ");Out.Char(clefsign); Out.Int(clefline,5); Out.Char(otherclef); *)
		CASE clefsign OF 
		"C":  IF (clefline = 3) THEN c := "a"
	 			ELSIF (clefline = 4) THEN c := "n"
				ELSIF (clefline = 2) THEN c := "m"
				ELSIF (clefline = 1) THEN c := "s"
				ELSE Out.Ln();  Out.String( "Clef not implemented." );  
				END;  
		
		| "F": 
				IF (clefline = 3) THEN c := "r"
				ELSIF (clefline = 4) THEN c := "b"
				ELSE Out.Ln();  Out.String( "Clef not implemented." );  
				END;  
		
		| "G": 
				IF (clefline = 2) THEN c := "t"
				ELSIF (clefline = 1) THEN c := "f"
				ELSE Out.Ln();  Out.String( "Clef not implemented." );  
				END;  
		| "p":  c := "t"; 
				otherclef := "p"; Out.Ln(); Out.Char("p");
		
		ELSE Out.Ln();  Out.String( "unknown Clef symbol " );  
		END;  (* Out.Char(c); *)
	END clefPMX; 
PROCEDURE NewBeat*( beat, beattype: INTEGER;  VAR res: ARRAY OF CHAR; blind : BOOLEAN );  
	VAR sbeat, sbeattype: ARRAY 5 OF CHAR;  
	BEGIN 
		Strings.IntToStr( beat, sbeat );  Strings.IntToStr( beattype, sbeattype );  COPY( "m", res );  
		Strings.Append( res, sbeat );  Strings.AppendCh( res, "/" );  Strings.Append( res, sbeattype );  
		
		Strings.AppendCh( res, "/" );  
		IF ~blind THEN
		Strings.Append( res, sbeat );  Strings.AppendCh( res, "/" );  
		Strings.Append( res, sbeattype );  Strings.AppendCh( res, BLANK );  
		ELSE
		Strings.Append( res, "0" );  Strings.AppendCh( res, "/" );  
		Strings.Append( res, "0" );  Strings.AppendCh( res, BLANK );  
		END;
		
	  
	
	END NewBeat;   	
PROCEDURE loeschint* ( VAR string: ARRAY OF LONGINT );  
	(* resets an ARRAY OF LONGINT TO zero. *)
	VAR i : INTEGER;
	BEGIN 
		i := 0;  
		WHILE i < LEN( string ) DO string[i] := 0;  INC( i ) END;  
	END loeschint;  


	PROCEDURE loesch*( VAR string: ARRAY OF CHAR );  
	(* resets an ARRAY OF CHAR to "0X". *)
	VAR i: INTEGER;  
	BEGIN 
		i := 0;  
		WHILE i < LEN( string ) DO string[i] := 0X;  INC( i ) END;  
	END loesch;
PROCEDURE FindLetter*( s: ARRAY OF CHAR;  VAR pos: LONGINT;  VAR eor: BOOLEAN );  
	(* Returns the position of the next letter in the string "s" starting at position "pos". 
	    When  no letter has been found *)
	VAR i: LONGINT;  
	BEGIN 
		i := pos;  
		WHILE (s[i] # ">") & ~Strings.IsAlpha( s[i] ) DO INC( i ) END;  
		IF Strings.IsAlpha( s[i] ) THEN pos := i;  ELSE eor := TRUE END;  
	END FindLetter;  

	PROCEDURE ReadUntil*( s: ARRAY OF CHAR;  VAR pos: LONGINT;  stop: CHAR;  VAR word: ARRAY OF CHAR;  
									    VAR eor: BOOLEAN );  
	(* Reads a word starting at Pposition "pos" and ending before "stop"-character. BLANKs are excluded *)
	VAR i, j: LONGINT;  
	BEGIN 
		loesch( word );  i := pos;  j := 0;  
		WHILE ( j < LEN(word))  & (s[i] # ">") & (s[i] # stop) DO   (*correction 9.11.2016 *)
			IF (s[i] # BLANK) THEN word[j] := s[i];  INC( j ) END;  
			INC( i );  
		END;  
		word[j] := 0X;  pos := i;  
		(* Out.Ln(); Out.String("ReadUntil: pos : "); Out.Int(pos,5); Out.String("word : ");Out.String(word); *)
		
		IF (s[i] # stop) THEN 
			eor := TRUE;  
			(* Out.Ln(); Out.String("ReadUntil : stop character  |"); Out.Char(stop); Out.String("| not found."); *) END;  
	END ReadUntil;  

	PROCEDURE FindChar*( s: ARRAY OF CHAR;  VAR pos: LONGINT;  c: CHAR;  VAR eor: BOOLEAN );  
	(* Returns the position of the next occurence of character c in the string "s" starting at position "pos". 
	    When  no letter has been found *)
	VAR i: LONGINT;  
	BEGIN 
		i := pos;  
		WHILE (s[i] # ">") & (s[i] # c) DO INC( i ) END;  
		IF (s[i] = c) THEN pos := i;  ELSE eor := TRUE END;  
	END FindChar;  
	PROCEDURE Max*( i, j: LONGINT ): LONGINT;  
	BEGIN 
		IF i >= j THEN RETURN i ELSE RETURN j END;  
	END Max;  

	PROCEDURE Min*( i, j: LONGINT ): LONGINT;  
	BEGIN 
		IF i <= j THEN RETURN i ELSE RETURN j END;  
	END Min;
PROCEDURE ExtractDigits*( sin: ARRAY OF CHAR;  VAR sout: ARRAY OF CHAR );  
	(* finds the first sequence of digits in a string *)
	VAR i, j: LONGINT;  
	BEGIN 
		i := 0;  
		IF ~Strings.IsDigit( sin[i] ) THEN 
			i := 1;  
			WHILE ( i <  LEN(sin) ) &  ~Strings.IsDigit( sin[i] )  DO INC( i ) END 
		END;  
		(* sin[i] is a digit *)
		j := 0;  
		WHILE ( i < LEN(sin) ) & Strings.IsDigit( sin[i] ) DO sout[j] := sin[i];  INC( i );  INC( j ) END;  
		sout[j] := 0X;  
	END ExtractDigits;  

	PROCEDURE ExtractInt*( s: ARRAY OF CHAR ): INTEGER;  
	(* Converts String to INTEGERr s. *)
	VAR res: LONGINT;  shortres: INTEGER;  
	BEGIN 
		Strings.StrToInt( s, res );  shortres := SHORT( res );  RETURN shortres;  
	END ExtractInt;  
(*	PROCEDURE keytopsv* (key : ARRAY OF CHAR; c : CHAR; VAR p,s,v : LONGINT);
	(* splits key into components : part, staff, voice *)
	VAR i,j : LONGINT; pe : ARRAY 4 OF CHAR;
	BEGIN
	i := 0; WHILE key[i] # c DO pe[i] := key[i]; INC(i) END;
	Strings.StrToInt(pe,p); 
	INC(i); s := ORD(key[i]) - 48; v := ORD(key[i + 2]) - 48;
	
	
	END keytopsv; *)
(*	PROCEDURE keytopsm* (key : ARRAY OF CHAR; c : CHAR; VAR p,s,m : LONGINT);
	VAR i,j : LONGINT; pe : ARRAY 8 OF CHAR;
	BEGIN
	i := 0; WHILE key[i] # c DO pe[i] := key[i]; INC(i) END;
	Strings.StrToInt(pe,p); 
	INC(i); s := ORD(key[i]) - 48; i := i + 2; 
	IF key[i] = "0" THEN WHILE key = "0" DO INC(i) END; END;
	j := i;
	WHILE key[i] # c DO pe[i-j] := key[i];INC(i) END;
	Strings.StrToInt (pe,m);
	
	
	END keytopsm; *)

(*	PROCEDURE psv2String*( part, staff, voice: LONGINT;  VAR key: ARRAY OF CHAR );  
	(* stores part staff voice in a string separated by "/" *)
	VAR res: ARRAY 32 OF CHAR; partc : ARRAY  5 OF CHAR;
	BEGIN 
	IF part < 10 THEN key[0] := "0"; key[1] := 0X; END;
		Strings.IntToStr( part, partc );  Strings.Append(key,partc);
		Strings.AppendCh(key,"/");Strings.IntToStr( staff, res );  Strings.Append( key, res );  
		Strings.AppendCh(key,"/");
		Strings.IntToStr( voice, res );  Strings.Append( key, res );  
	END psv2String;  
	PROCEDURE psmv2String*( part, staff, voice, measure : LONGINT;  VAR key: ARRAY OF CHAR ); 
		(* stores part/ staff/ voice/ measure in a string separated by "/" *)
 
	VAR res: ARRAY 32 OF CHAR; partc : ARRAY  5 OF CHAR;
	BEGIN 
	    fill0 (part,2,partc);		  Strings.Append(key,partc);
		Strings.AppendCh(key,"/");Strings.IntToStr( staff, res );  Strings.Append( key, res );  
		Strings.AppendCh(key,"/");
		fill0 ( measure,4, res );  Strings.Append( key, res );  
		Strings.AppendCh(key,"/");
     	Strings.IntToStr( voice, res );  Strings.Append( key, res );  
	END psmv2String;  	
	
	PROCEDURE mpsvn2String*( measure, part, staff, voice, note : LONGINT;  VAR key: ARRAY OF CHAR ); 
		(* stores part/ staff/ voice/ measure in a string separated by "/" *)
 (* Vorsicht  Sortierung wie WritePmx *)
	VAR res: ARRAY 32 OF CHAR; partc : ARRAY  5 OF CHAR;
	BEGIN 
		fill0 ( measure,4, res );  			Strings.Append( key, res );  	Strings.AppendCh(key,"/");
		loesch(res);
	    fill0 (2-part,2,res);		  			Strings.Append(key,res);	Strings.AppendCh(key,"/");
	    loesch(res);
	    Strings.IntToStr( 2-staff, res );  	Strings.Append( key, res );  Strings.AppendCh(key,"/");
	    loesch(res);
     	Strings.IntToStr( 4-voice, res );  Strings.Append( key, res ); 	Strings.AppendCh(key,"/");
     	loesch(res);
 		fill0(note,2,res);   					Strings.Append( key, res ); 	Strings.AppendCh(key,"/");

 
	END mpsvn2String;  *)

	PROCEDURE  percussionclef* (ps : LONGINT; VAR perclef, perline : ARRAY OF CHAR);
	VAR psstring : ARRAY 4 OF CHAR;
	BEGIN
	
	COPY ("\\setclefsymbol{",perclef); Strings.IntToStr(ps,psstring); 
	Strings.Append(perclef,psstring);Strings.Append(perclef,"}\drumclef\");
	COPY ("\\setlines{",perline); Strings.Append(perline,psstring);Strings.Append (perline,"}{1}\");
	END percussionclef;
	PROCEDURE left*( rec: ARRAY OF CHAR;  anz: LONGINT;  pat: ARRAY OF CHAR ): BOOLEAN;  
	VAR i: LONGINT;  res: BOOLEAN;  
	BEGIN 
		i := 0;  res := TRUE;  
		WHILE i < anz DO res := res & (pat[i] = rec[i]);  INC( i ) END;  
		RETURN res;  
	END left; 
PROCEDURE FilterTeX*( in: ARRAY OF CHAR;  VAR out: ARRAY OF CHAR );  
	(* Removes certain characters like "&", "_" and "^" which may caus trouble in the TeX compilation; *)
	VAR i, j: LONGINT;  
	BEGIN 
		i := 0;  j := 0;  
		WHILE (i < Strings.Length( in )) DO 
			IF (in[i] # "&") & (in[i] # "_") & (in[i] # "^") THEN out[j] := in[i];  INC( j ) END;  
			INC( i );  
		END;  
	END FilterTeX;
	PROCEDURE fill0* (in : LONGINT; outdigits : LONGINT; VAR out : ARRAY OF CHAR);
	(* fills a positive integer "in" from left with zeros up to "outdigits" digits. *)
	VAR i, indigits : LONGINT; inc : ARRAY 12 OF CHAR;
	BEGIN
	Strings.IntToStr(in,inc); indigits := Strings.Length(inc); i := indigits;
	WHILE i < outdigits DO out[i-indigits] := "0"; INC(i); END;
	Strings.Append(out,inc);
	END fill0; 
	PROCEDURE ReadUntilTx*( VAR R: Files.Rider;  stop: CHAR; VAR s: ARRAY OF CHAR );
   (* Reads from a given file position until a stop character and stores in string, Mike Spivey. *)
   VAR i: LONGINT;  c: CHAR;
   BEGIN
       Files.Read( R, c );  i := 0;
       WHILE ~R.eof & (c # stop) DO
           IF ~WhiteSpace( c ) THEN s[i] := c; INC(i); END;
           Files.Read( R, c )
       END;
       s[i] := c; s[i+1] := 0X
   END ReadUntilTx; 
(*old version until 12.08.2020 
	PROCEDURE ReadUntilTx*( VAR R: Files.Rider;  stop: CHAR;  VAR s: ARRAY OF CHAR );  
	(* Reads from a given file position until a stop character and stores in string. *)
	VAR i: LONGINT;  c: CHAR;  
	BEGIN 
		Files.Read( R, c );  i := 0;
		REPEAT IF ~WhiteSpace(c) THEN s[i] := c; INC(i); END; Files.Read(R,c);  		
	
		UNTIL R.eof OR ( c = stop );
		
		s[i] := c;	s[i+1] := 0X;   
		
	END ReadUntilTx;  *)
	PROCEDURE WhiteSpace ( c : CHAR) : BOOLEAN;
	VAR ws : BOOLEAN;
	BEGIN
	ws := (c = BLANK) OR ( c = TAB ) OR ( c = CR ) OR ( c = NL );
	RETURN ws;
	END WhiteSpace;
	PROCEDURE testws*;
	VAR f : Files.File; R : Files.Rider; c : CHAR; rec : ARRAY 256 OF CHAR; i : LONGINT;
	BEGIN
	f := Files.Old ("d:/musix/xml/vivaldi.xml");
	Files.Set(R,f,0);
	WHILE ~R.eof DO
	Files.Read (R,c);
	IF c # "<" THEN REPEAT Files.Read(R,c) UNTIL R.eof OR (c = "<" ); END;
	i := 0; rec[i] := c; REPEAT Files.Read(R,c); INC(i); rec[i] := c; UNTIL R.eof OR (c = ">" ); 
	rec[i+1] := 0X;
	Out.Ln();Out.String(rec);
	END;
	END testws;
	PROCEDURE ReadRecn1*( VAR R: Files.Rider;  VAR rec: ARRAY OF CHAR;  VAR length: LONGINT );  
	(* Reads one record from the MusicXML file. removes leading BLANKs and TABs and CR, NL *)
	VAR i: LONGINT;  c: CHAR;  
	BEGIN 
		Files.Read( R, c );  
		IF WhiteSpace(c) THEN REPEAT Files.Read(R,c) UNTIL  ~WhiteSpace(c); END;
		i := 0;  
		WHILE (~R.eof) & (i < LEN( rec ) - 1) & (c # CR) & (c # NL) DO rec[i] := c;  Files.Read( R, c );  INC( i ) END;  
		rec[i] := 0X;  length := i;  
	END ReadRecn1;  
		PROCEDURE ReadRecn*( VAR R: Files.Rider;  VAR rec: ARRAY OF CHAR;  VAR length: LONGINT );  
	(* Reads one record from the MusicXML file. removes leading BLANKs and TABs and CR, NL *)
	VAR i: LONGINT;  c: CHAR;  
	BEGIN 
		Files.Read( R, c );  
		IF (c = NL) THEN Files.Read( R, c );  END;  
		(* Remove leading Blanks and Tabs *) ;  
		IF (c = BLANK) OR (c = TAB) THEN 
			WHILE (c = BLANK) OR (c = TAB) DO Files.Read( R, c );  END;  
		END;  
		i := 0;  
		WHILE (~R.eof) & (i < LEN( rec ) - 1) & (c # CR) & (c # NL) DO rec[i] := c;  Files.Read( R, c );  INC( i ) END;  
		rec[i] := 0X;  length := i;  
	END ReadRecn;  

	PROCEDURE SkipUntilPattern*( VAR R: Files.Rider;  pattern: ARRAY OF CHAR );  
	(* Skips records from XML file until pattern is reached. *)
	VAR pos: LONGINT;  
		rec: ARRAY 256 OF CHAR;  
	BEGIN 
		pos := -1;  
		WHILE ~R.eof & (pos = -1) DO (* ????????????????????? pos # -1 *)
			ReadRec( R, rec );   (* Out.Ln();Out.String("skip : ");Out.String(rec); *)
			Strings.Search( pattern, rec, pos )
		END;  
	END SkipUntilPattern;  
		PROCEDURE ReadRec*( VAR R: Files.Rider;  VAR rec: ARRAY OF CHAR );  
	(* Reads one record from the MusicXML file. removes leading BLANKs and TABs and CR NL *)
	VAR i: LONGINT;  c: CHAR;  
	BEGIN 
		SkipTextChar( R, BLANK, c );  i := 0;  
		WHILE (~R.eof) & (i < LEN( rec ) - 1) & (c # 0DX) & (c # 0AX) DO 
			IF (c # TAB) THEN rec[i] := c;  INC( i );  END;  
			Files.Read( R, c )
		END;  
		rec[i] := 0X;  
		IF (c = 0DX) OR (i = LEN( rec ) - 1) THEN 
			WHILE c # 0AX DO Files.Read( R, c );  END;  
		END;  
	
	END ReadRec;  

	PROCEDURE SkipTextChar*( VAR R: Files.Rider;  skip: CHAR;  VAR found: CHAR );  
	(* skips over leading characters  , e.g. Blanks in an XML-File and returns the first character # "skip"*)
	VAR c: CHAR;  
	BEGIN 
		Files.Read( R, c );  
		IF (c = skip) THEN 
			WHILE ~R.eof & (c = skip) DO Files.Read( R, c );  END;  
			IF (c # skip) THEN found := c;  ELSE found := 0X END;  
		ELSE found := c
		END;  
	END SkipTextChar;  
		PROCEDURE ReadfromtoString*( VAR s: ARRAY OF CHAR;  from, to: LONGINT;  VAR between: ARRAY OF CHAR);  
	(* Reads the substring "between" inside string "s", starting with "from" and ending with "to" -1 . *)
	VAR k, i: LONGINT;  ltins: BOOLEAN;  
	BEGIN 
		i := from;  k := 0;  ltins := FALSE;  
		WHILE (k < LEN( between ) ) & (i < to) DO  (* ltins := ltins OR (s[i] = "<") OR (s[i] = ">"); *)
			between[k] := s[i];  INC( k );  INC( i );  
		END;  
		between[k] := 0X;  
		(*	IF ltins THEN loesch( between ) END;  *)
	END ReadfromtoString; 
	PROCEDURE FindName*( s: ARRAY OF CHAR;  VAR pos: LONGINT;  VAR name: ARRAY OF CHAR;  VAR eor: BOOLEAN );  
	BEGIN 
		FindLetter( s, pos, eor );  
		IF ~eor THEN ReadUntil( s, pos, "=", name, eor );  END;  
	END FindName;  

	PROCEDURE FindValue*( s: ARRAY OF CHAR;  VAR pos: LONGINT;  VAR value: ARRAY OF CHAR;  VAR eor: BOOLEAN );  
	VAR 
	BEGIN 
		FindChar( s, pos, 22X, eor );   (* Out.Ln(); Out.String("FindValue: pos : "); Out.Int(pos,5); *) INC( pos );  
		IF ~eor THEN ReadUntil( s, pos, 22X, value, eor );  END;  
	END FindValue;  
		PROCEDURE AnalyzeTag2*( rec: ARRAY OF CHAR;  VAR tag, endtag, between: ARRAY OF CHAR;  
										    VAR names, values: ARRAY OF ARRAY OF CHAR;  VAR novalues: LONGINT );  
	(*  extracts the information from MusicXML-tags. 
  Depending on which tag, we have a number of name-value-pairs as well as the word between ">" and "<"*)
	VAR i, j, k, from, to, pos, isave: LONGINT;  eor: BOOLEAN;  
	BEGIN 
		i := 0;  loesch( tag );  loesch( endtag );  loesch( between );  novalues := 0;  j := 0;  
		WHILE j < 10 DO loesch( names[j] );  loesch( values[j] );  INC( j ) END;  
		(*	Out.Ln(); Out.String("AnalyzeTag2");Out.String(rec); Out.Int(ORD(rec[0]),5);Out.Ln();  *)
		WHILE (i < Strings.Length( rec )) & (rec[i] # 20X) & (rec[i] # ">") (* Find Blank  if it exists *)
		DO 
			tag[i] := rec[i];  INC( i );  
		END;  
		isave := i;  
		IF (rec[i] = 20X) THEN 
			IF rec[i + 1] = "/" THEN tag[i] := "/";  INC( i );  END;  
			tag[i] := ">";  tag[i + 1] := 0X;  k := 0;  

			eor := FALSE;  pos := i;  j := 0;  
			WHILE ~eor DO 
				FindName( rec, pos, names[j], eor );  
				(* Out.Ln();Out.String(" new algorithm : "); 			Out.String(" pos : "); Out.Int(pos,5); *)
				IF ~eor THEN FindValue( rec, pos, values[j], eor );  END;  
				(* Out.String(names[j]); Out.String(values[j]); *)
				
				INC( j );  
			END;  
			i := pos;  
			
			WHILE (i < Strings.Length( rec )) & (rec[i] # ">") DO INC( i );  
			END;  
			novalues := j - 1;  
		
		ELSE tag[isave] := ">";  tag[isave + 1] := 0X;  
		END;  
		
		close( tag, endtag );  from := i + 1;  to := from;  Strings.Search( endtag, rec, to );  loesch( between );  
		IF (to > -1) THEN ReadfromtoString( rec, from, to, between );  END;  
		(*	Out.String(tag); Out.Char("|"); Out.String(endtag); Out.Char("|");Out.String(between); Out.Char("|");
	i := 0; WHILE i < novalues DO Out.String(names[i]);Out.Char("="); Out.String(values[i]); INC(i); END;  *)
	END AnalyzeTag2;  
	PROCEDURE FindIProperty*( n: Tag;  tagname, pattern: ARRAY OF CHAR;  VAR res: LONGINT );  
	(* searches n.tagname for the occurrence of "pattern"  and finds Integer value of "pattern"		*)
	VAR endtag: ARRAY 32 OF CHAR;  
	BEGIN 
		
		IF (n.tagname = tagname) THEN 
			COPY( n.endtag, endtag );  
			
			WHILE (n.next # NIL ) & (n.tagname # endtag) & (n.tagname # pattern) DO 

				n := n.next
   			END;  
			IF (n.tagname = pattern) THEN Strings.StrToInt( n.between, res );  ELSE res := 1 END;   (* komischer ELSE Fall *)
			
		ELSE  (* Out.Ln();  Out.String( " FindProperty : object is not a  " );  Out.String( tagname );  *)
		END;  
	
	END FindIProperty;  

	PROCEDURE FindProperty*( n: Tag;  tagname, pattern: ARRAY OF CHAR;  VAR res: LONGINT );  
	(* Finds the value of an  integer property, corresponding to "pattern", e.g. "staff" or "voice".  		*)
	VAR endtag: ARRAY 32 OF CHAR;  
	BEGIN 
		
		IF (n.tagname = tagname) THEN 
			COPY( n.endtag, endtag );  
			
			WHILE (n.next # NIL ) & (n.tagname # endtag) & (n.tagname # pattern) DO 
				(*	IF (n.tagname = directiontag) THEN OutTag( n ) END;  *)
				n := n.next
			END;  
			IF (n.tagname = pattern) THEN res := 1;  ELSE res := -1 END;  
		
		END;  
	
	END FindProperty; 
	PROCEDURE testmakekey*(maxpart, measure : LONGINT; VAR keytotal : ARRAY OF CHAR);
	VAR keystr : ARRAY 16 OF CHAR; i : LONGINT;	
	BEGIN
(*	Out.Ln(); Out.String("1testmake");
	i := 1; WHILE i <= maxpart DO
	Out.Ln(); Out.Int(i,5); Out.Int (akeys[i],5); Out.Int(mkeys[i],5); INC(i); END; *)

	i := maxpart; loesch(keytotal); keytotal[0] := "K"; keytotal[1] := 0X;
	WHILE ( i > 0 ) DO
	    loesch(keystr);
		IF ( measure = 1 ) THEN
			IF (akeys[i] # akeys[maxpart]) THEN
			Makekeystr(maxpart,i,akeys[i],keystr);  END;
	
		ELSE
		IF ( mkeys[i]  # mkeys[maxpart] ) THEN
			Makekeystr(maxpart,i,mkeys[i],keystr);  END;
		

		END;

  	Strings.Append(keytotal,keystr);
	DEC(i);
END; IF (keytotal = "K") THEN keytotal[0] := 0X END;
 (*	Out.Ln();Out.String("keytotal : ");Out.String(keytotal);*)
	END testmakekey; 
	
	PROCEDURE Makekeystr* (maxpart,part : LONGINT; keys : INTEGER; VAR keystr : ARRAY OF CHAR);
	VAR  dummy : ARRAY 4 OF CHAR;  
	BEGIN
	       COPY ("Ki" , keystr); IF ( part # maxpart ) THEN COPY ("i", keystr); END;
	    Strings.IntToStr(maxpart-part+1,dummy); Strings.Append(keystr,dummy);
			         Strings.Append(keystr,"+0"); 
			         IF ( keys >= 0 ) THEN Strings.AppendCh(keystr,"+"); END;
					 Strings.IntToStr( keys, dummy );  
						Strings.Append( keystr, dummy ); 
	
	END Makekeystr; 
 	PROCEDURE metron2PMX* ( beatunit : ARRAY OF CHAR; perminute : ARRAY OF CHAR;
	                                            VAR sout :ARRAY OF CHAR);
	    (*  Generates the Metronome data for PMX. *)
	  BEGIN
	loesch (sout);

(*	COPY ("\zcharnote{12}{\metron", sout) ; *)
	COPY ("\metron", sout) ; 

	(* Out.Ln(); Out.String(sout); *)
	  IF beatunit = "half" THEN Strings.Append (sout,"{\hu}{") 
	  		ELSIF beatunit = "quarter" THEN Strings.Append(sout,"{\qu}{")
	  		            ELSIF beatunit = "eighth"   THEN Strings.Append(sout,"{\cu}{");
	  		            ELSE
	  		            Out.Ln(); 
	  		            Out.String("metron2PMX : beatunit ");
	  		            Out.String(beatunit); Out.String("unknown") 
	  		            END;
	  		      Strings.Append(sout,perminute);   Strings.AppendCh (sout,"}");                                                     
	           ;                                                                          
	                                                                                             
  END metron2PMX;
END b.testMinDist
