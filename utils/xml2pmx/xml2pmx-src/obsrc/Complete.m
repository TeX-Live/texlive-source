MODULE Complete;

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

IMPORT Strings := Strings1, Out;
TYPE LONGINT = INTEGER;
VAR pmxdur : ARRAY 10 OF ARRAY 4 OF CHAR; durvec : ARRAY 10 OF INTEGER; ixmax : LONGINT; 
notetypexml: ARRAY 10 OF ARRAY 8 OF CHAR; notetypepmx: ARRAY 10 OF CHAR;  
notetypei : ARRAY 10 OF INTEGER; (* inverse of notetype index 0 => 64 *)
	PROCEDURE erasetime*(nostaves : LONGINT;  VAR time: ARRAY OF ARRAY OF INTEGER );  
	VAR ps, i, j: LONGINT;  
	BEGIN 
		ps := nostaves - 1;  
		WHILE (ps >= 0) DO 

			i := 0;  j := LEN( time, 1 );  
			WHILE i < j DO time[ps, i] := 0;  INC( i ) END;  
			DEC( ps );  
		END;  
	END erasetime;  
	PROCEDURE divdur* (measuredur,div : INTEGER; VAR beats, beattype : INTEGER);
    BEGIN
    IF ( measuredur MOD div = 0 ) THEN
    	beattype := 4; beats := measuredur DIV div;
    ELSIF ( 2* measuredur MOD div = 0 ) THEN 
    	beattype := 8; beats :=  2* measuredur DIV div;
    ELSIF ( 4 * measuredur MOD div = 0 ) THEN
    	beattype := 16; beats := 4 * measuredur DIV div;
    ELSE
    	Out.Ln(); Out.String(" error in divdur ");
    END;
    Out.Ln(); Out.Int(measuredur,5); Out.Int(div,5); Out.Int(beats,5); Out.Int(beattype,5);
    END divdur;
    PROCEDURE testdivdur*;
    VAR beats, beattype : INTEGER;
    BEGIN
    divdur (4096, 1024, beats, beattype);
     divdur (192, 48, beats, beattype);
       divdur (144, 48, beats, beattype);
  divdur (216, 48, beats, beattype);
  divdur (72, 48, beats, beattype);
  divdur (96, 48, beats, beattype);
  divdur (240, 48, beats, beattype);

 divdur (4096, 1024, beats, beattype);

    END testdivdur;


 PROCEDURE CalcForward* (note,div : INTEGER; VAR res : ARRAY OF CHAR);
	VAR i : INTEGER; 	
	BEGIN	
	i := 0; WHILE i < 8 DO durvec[i]:= 0 ; INC(i) END; (* erase durvec *)


	Durit (note,div);
	
	blindrest(durvec,res); 
	
(*	Out.String("CalcForward res : ");Out.Ln();Out.String(res);  *)
	
	
	
	END CalcForward;
		PROCEDURE notetype;  
	(* Defines the MusicXML types of notes *)
	BEGIN 
		
		COPY( "64th", notetypexml[0] );  COPY( "32nd", notetypexml[1] );  COPY( "16th", notetypexml[2] );  
		COPY( "eighth", notetypexml[3] );  COPY( "quarter", notetypexml[4] );  COPY( "half", notetypexml[5] );  
		COPY( "whole", notetypexml[6] );  COPY( "breve", notetypexml[7] );  
		notetypepmx[0] := "6"; 	notetypepmx[1] := "3";  notetypepmx[2] := "1";  
		notetypepmx[3] := "8";  notetypepmx[4] := "4";  
		notetypepmx[5] := "2";  notetypepmx[6] := "0";  notetypepmx[7] := "9";  
		notetypei[0] := 64;	notetypei[1] := 32;notetypei[2] := 16;notetypei[3] := 8;
		notetypei[4] := 4;notetypei[5] :=2;notetypei[6] := 1;
	END notetype;  

		PROCEDURE tupletduration* ( div: INTEGER;  normaltype: ARRAY OF CHAR;  normal: INTEGER ): INTEGER;  
	(* calculates the duration of a tuplet, starting from divisions, normaltype, and normal count *)
	VAR i, fac: INTEGER;  
	BEGIN 
		fac := 1;  i := 0;  
		WHILE (i < 8) & (notetypexml[i] # normaltype) DO 
		fac := fac*2; INC( i ); END;  
	  IF i <  8 THEN	RETURN div*normal*fac DIV 16
	  ELSE Out.Ln(); Out.String("tupletduration normaltype "); Out.String(normaltype);Out.String("not found"); RETURN 0; END;
	END tupletduration;  
 
	
	PROCEDURE dur2beat* (duration,divisions : INTEGER; VAR beat, beattype : INTEGER);
	(* calculates beat and beattype from duration of a measure *)	
	VAR i : INTEGER;
	BEGIN
		i := 0; WHILE i < 8 DO durvec[i]:= 0 ; INC(i) END; (* erase durvec *)

	(* Out.Ln(); Out.String("dur2beat duration, divisions : ");Out.Int(duration, 5); Out.Int(divisions,5); *)
	
		IF ( duration MOD divisions = 0 )	THEN 
			beattype := 4; beat := duration DIV divisions	
		ELSIF (duration MOD (divisions DIV 2) ) = 0 THEN
			beattype :=8; beat := duration * 2 DIV divisions
		ELSIF (duration MOD (divisions DIV 4)) = 0 THEN
			beattype := 16; beat :=	duration * 4 DIV divisions
		END;
  (* Out.Ln(); Out.String("dur2beat beat / beattype : "); Out.Int(beat,5); Out.Int(beattype,5); *)
	
	END dur2beat;
	PROCEDURE testdur2beat*;
	VAR beat, beattype : INTEGER;
	BEGIN
	dur2beat(240,48, beat, beattype);
	dur2beat(216,48, beat, beattype);
	dur2beat(144,48, beat, beattype);
	dur2beat(36,48, beat, beattype);
	END testdur2beat;
	
	PROCEDURE Durit(note,div : INTEGER);
	VAR index, rest, i : INTEGER;
	BEGIN
	i := 0;
	REPEAT 
(*	Out.Ln(); Out.String("Durit : note, div "); Out.Int(note, 5); Out.Int(div, 5); *)
	FindDurit(note, div,index, rest);
	note := rest; INC(i);
	UNTIL (i > 4) OR (rest = 0);	
	END Durit;
		PROCEDURE FindDurit(note,div: INTEGER; VAR index,rest : INTEGER );
	(* distributes a given note duration into available note values and stores tthe distribution in xmldur. *)
	VAR 
	i : INTEGER; 
	xmldur  : ARRAY 9 OF INTEGER; (* change 25.April 2020 *)

	BEGIN
	xmldur[0] := 4 * div;	
	xmldur[1] := 2 * div;	
	xmldur[2] :=  div;	ixmax := 2;
	IF ( ( div MOD 2 ) =  0 ) THEN xmldur[3] := div DIV 2 ; ixmax := 3; END;
	IF ( ( div MOD 4 ) =  0 ) THEN xmldur[4] := div DIV 4;	ixmax := 4; END;
	IF ( ( div MOD 8 ) =  0 ) THEN xmldur[5] := div DIV 8;	ixmax := 5; END;
	IF ( ( div MOD 16 ) =  0 ) THEN xmldur[6] := div DIV 16;ixmax := 6; END;
	IF ( ( div MOD 32 ) =  0 ) THEN xmldur[7] := div DIV 32;ixmax := 7; END;
	IF ( ( div MOD 64 ) =  0 ) THEN xmldur[8] := div DIV 64;ixmax := 8; END;  (* change 25.April 2020 *)

	

	i := 0; WHILE ( i <  LEN(xmldur) - 1) & ( xmldur[i] > note ) DO INC(i); END; (* change 25.04.2020 *)
	IF xmldur[i] <= note THEN
	
	index := i; rest := note - xmldur[i];
(*	Out.Ln(); Out.String("FindDurit : "); Out.Int(note,5); Out.Int(div,5); Out.Int(index,5); Out.Int(rest,5);  *)
	INC(durvec[i], 1); END;

	END FindDurit;
	
	PROCEDURE blindrest (durvec : ARRAY OF INTEGER; VAR res : ARRAY  OF CHAR);
	VAR i,j : INTEGER; 
	BEGIN
	i := 0; WHILE i < 32 DO res[i] := 0X; INC(i) END;
	i := 0; WHILE ( i < 7 )  DO 
			j := 0; WHILE ( j < durvec[i] )  DO 
				Strings.AppendCh(res, " ");
				Strings.Append(res,pmxdur[i]);
				INC(j); END;
		INC(i) END; Strings.AppendCh(res," ");
	END blindrest;


PROCEDURE Int2br* ( divisions, intdur : INTEGER; VAR res : ARRAY OF CHAR);
(* converts an Integer Interval into a series of blind rests *)
VAR i : INTEGER;
BEGIN
i := 0;	WHILE i < 7 DO durvec[i] :=  0; INC(i); END;
i := 0;  WHILE i < LEN(res) DO res[i] := 0X; INC(i); END;
Durit(intdur, divisions);
blindrest (durvec, res);
(*  Out.Ln();Out.String("divisions : "); Out.Int(divisions,5); Out.String("duration : "); 
  Out.Int(intdur,5); Out.String("pmx-decomposition : "); Out.String(res); *)
END Int2br;

PROCEDURE Dur2PMX* ( divisions, intdur : INTEGER; VAR res : ARRAY OF CHAR);
(* converts an Integer Interval into a series of blind rests *)
VAR i : INTEGER;
BEGIN
i := 0;	WHILE i < 7 DO durvec[i] :=  0; INC(i); END;
i := 0;  WHILE i < LEN(res) DO res[i] := 0X; INC(i); END;
Durit(intdur, divisions);
i := 0; WHILE (durvec[i] = 0) DO INC(i) END;
IF durvec[i] = 1 THEN  res[0] := pmxdur[i,1]; 
	IF durvec[i+1] = 1  THEN Strings.AppendCh(res,"d");
			IF durvec[i+2] = 1 THEN Strings.AppendCh(res,"d"); END;
	END;
END;	
(*  Out.Ln();Out.String("divisions : "); Out.Int(divisions,5); Out.String("duration : "); 
  Out.Int(intdur,5); Out.String("pmx-decomposition : "); Out.String(res);  *)
	
END Dur2PMX;

PROCEDURE testDur2PMX*;
VAR res : ARRAY 32 OF CHAR;
BEGIN

Dur2PMX (256, 64,res);


Dur2PMX (256,32,res);

Dur2PMX (256,16,res);

Dur2PMX (256,8,res);
Dur2PMX (1024,768,res);

Dur2PMX (1024,1536,res);

END testDur2PMX;
PROCEDURE Complete* (from, to, divisions, measureduration : INTEGER; VAR before, after : ARRAY  OF CHAR); 
		(* solves the problem of dangling notes created with the backup or forward statement of MusicXML *)
	VAR beforedur, afterdur, i : INTEGER;
	BEGIN
	beforedur :=  from - 1;
	afterdur := measureduration - to;
 (* Out.Ln(); Out.String(" from, to, divisions, measureduration : ");
 Out.Int(from,5); Out.Int(to,5); Out.Int(divisions,5); Out.Int(measureduration,5);	
 Out.String(" Complete : beforedur, afterdur");  Out.Int(beforedur,5); Out.Int(afterdur,5); *)
     i := 0;	WHILE i < 7 DO durvec[i] :=  0; INC(i); END;before[0] := 0X; after[0] := 0X;
	 IF ( beforedur >  0 ) THEN Durit(beforedur,divisions);
	blindrest (durvec,before);
	END; 
	(* loesch durvec ! *) i := 0; WHILE i < 7 DO durvec[i] :=  0; INC(i); END;
	
	IF ( afterdur >  0 ) THEN Durit (afterdur,divisions); 
	blindrest (durvec, after); END; 
	(* Out.String(" before|after : "); Out.String(before); Out.Char("|");Out.String(after); *)
END Complete;
BEGIN
	notetype;  (*  initialize notes type conversion XML -> PMX *)
	pmxdur[0] := "r0b";
	pmxdur[1] := "r2b";	
	pmxdur[2] := "r4b";	
	pmxdur[3] :=  "r8b" ;
	pmxdur[4] := "r1b" ;	 
	pmxdur[5] :=  "r3b" ;	
	pmxdur[6] := "r6b";
	pmxdur[7] := "r7b";

END Complete.testdur2beat

Complete.testdivdur

Complete.TestFindDur


Complete.testDur2PMX

TestFindDur
	
	System.Free Complete
