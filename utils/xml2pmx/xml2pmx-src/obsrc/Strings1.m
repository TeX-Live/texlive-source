MODULE Strings1;	(** portable *) (* ejz, *)

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

TYPE LONGINT = INTEGER;

(** Strings is a utility module that provides procedures to manipulate strings.
		Note: All strings MUST be 0X terminated. *)
	VAR
		isAlpha*: ARRAY 256 OF BOOLEAN; (** all letters in the oberon charset *)
		ISOToOberon*, OberonToISO*: ARRAY 256 OF CHAR; (** Translation tables for iso-8859-1 to oberon ascii code. *)

(** Length of str. *)
	PROCEDURE Length*(VAR str(** in *): ARRAY OF CHAR): LONGINT;
		VAR i, l: LONGINT;
	BEGIN
		l := LEN(str); i := 0;
		WHILE (i < l) & (str[i] # 0X) DO
			INC(i)
		END;
		RETURN i
	END Length;

(** Append this to to. *)
	PROCEDURE Append*(VAR to(** in/out *): ARRAY OF CHAR; this: ARRAY OF CHAR);
		VAR i, j, l: LONGINT;
	BEGIN
		i := 0;
		WHILE to[i] # 0X DO
			INC(i)
		END;
		l := LEN(to)-1; j := 0;
		WHILE (i < l) & (this[j] # 0X) DO
			to[i] := this[j]; INC(i); INC(j)
		END;
		to[i] := 0X
	END Append;

(** Append this to to. *)
	PROCEDURE AppendCh*(VAR to(** in/out *): ARRAY OF CHAR; this: CHAR);
		VAR i: LONGINT;
	BEGIN
		i := 0;
		WHILE to[i] # 0X DO
			INC(i)
		END;
		IF i < (LEN(to)-1) THEN
			to[i] := this; to[i+1] := 0X
		END
	END AppendCh;

(** TRUE if ch is a decimal digit. *)
	PROCEDURE IsDigit*(ch: CHAR): BOOLEAN;
	BEGIN
		RETURN (ch >= "0") & (ch <= "9")
	END IsDigit;

(** TRUE if ch is a letter. *)
	PROCEDURE IsAlpha*(ch: CHAR): BOOLEAN;
	BEGIN
		RETURN isAlpha[ORD(ch)]
	END IsAlpha;

(** If ch is an upper-case letter return the corresponding lower-case letter. *)
	PROCEDURE LowerCh*(ch: CHAR): CHAR;
	BEGIN
		CASE ch OF
			"A" .. "Z": ch := CHR(ORD(ch)-ORD("A")+ORD("a"))
			|80X: ch := 83X
			|81X: ch := 84X
			|82X: ch := 85X
		ELSE
		END;
		RETURN ch
	END LowerCh;

(** If ch is an lower-case letter return the corresponding upper-case letter. *)
	PROCEDURE UpperCh*(ch: CHAR): CHAR;
	BEGIN
		CASE ch OF
			"a" .. "z": ch := CAP(ch)
			|83X: ch := 80X
			|84X: ch := 81X
			|85X: ch := 82X
			|86X: ch := "A"
			|87X: ch := "E"
			|88X: ch := "I"
			|89X: ch := "O"
			|8AX: ch := "U"
			|8BX: ch := "A"
			|8CX: ch := "E"
			|8DX: ch := "I"
			|8EX: ch := "O"
			|8FX: ch := "U"
			|90X: ch := "E"
			|91X: ch := "E"
			|92X: ch := "I"
			|93X: ch := "C"
			|94X: ch := "A"
			|95X: ch := "N"
			|96X: ch := "S"
		ELSE
		END;
		RETURN ch
	END UpperCh;

(** Convert str to all lower-case letters. *)
	PROCEDURE Lower*(VAR str(** in *), lstr(** out *): ARRAY OF CHAR);
		VAR i: LONGINT;
	BEGIN
		i := 0;
		WHILE str[i] # 0X DO
			lstr[i] := LowerCh(str[i]); INC(i)
		END;
		lstr[i] := 0X
	END Lower;

(** Convert str to all upper-case letters. *)
	PROCEDURE Upper*(VAR str(** in *), ustr(** out *): ARRAY OF CHAR);
		VAR i: LONGINT;
	BEGIN
		i := 0;
		WHILE str[i] # 0X DO
			ustr[i] := UpperCh(str[i]); INC(i)
		END;
		ustr[i] := 0X
	END Upper;

(** Change the suffix of str to suf. *)
	PROCEDURE ChangeSuffix*(VAR str(** in/out *): ARRAY OF CHAR; suf: ARRAY OF CHAR);
		VAR i, j, l, dot: LONGINT;
	BEGIN
		dot := -1; i := 0;
		WHILE str[i] # 0X DO
			IF str[i] = "." THEN
				dot := i
			ELSIF str[i] = "/" THEN
				dot := -1
			END;
			INC(i)
		END;
		IF dot > 0 THEN
			l := LEN(str)-1; i := dot+1; j := 0;
			WHILE (i < l) & (suf[j] # 0X) DO
				str[i] := suf[j]; INC(i); INC(j)
			END;
			str[i] := 0X
		END
	END ChangeSuffix;

(** Search in src starting at pos for the next occurrence of pat.  Returns pos=-1 if not found. *)
	PROCEDURE Search*(pat: ARRAY OF CHAR; VAR src(** in *): ARRAY OF CHAR; VAR pos(** in/out *): LONGINT);
		CONST MaxPat = 128;
		VAR
			buf: ARRAY MaxPat OF CHAR;
			len, i, srclen: LONGINT;
		PROCEDURE Find(beg: LONGINT);
			VAR
				i, j, b, e: LONGINT;
				ch: CHAR;
				ref: ARRAY MaxPat OF CHAR;
		BEGIN
			ch := src[pos]; INC(pos);
			ref[0] := ch;
			i := 0; j := 0; b := 0; e := 1;
			WHILE (pos <= srclen) & (i < len) DO
				IF buf[i] = ch THEN
					INC(i); j := (j + 1) MOD MaxPat
				ELSE
					i := 0; b := (b + 1) MOD MaxPat; j := b
				END;
				IF j # e THEN
					ch := ref[j]
				ELSE
					IF pos >= srclen THEN
						ch := 0X
					ELSE
						ch := src[pos]
					END;
					INC(pos); ref[j] := ch; e := (e + 1) MOD MaxPat; INC(beg);
				END
			END;
			IF i = len THEN
				pos := beg-len
			ELSE
				pos := -1
			END
		END Find;
	BEGIN
		len := Length(pat);
		IF MaxPat < len THEN
			len := MaxPat
		END;
		IF len <= 0 THEN
			pos := -1;
			RETURN
		END;
		i := 0;
		REPEAT
			buf[i] := pat[i]; INC(i)
		UNTIL i >= len;
		srclen := Length(src);
		IF pos < 0 THEN
			pos := 0
		ELSIF pos >= srclen THEN
			pos := -1;
			RETURN
		END;
		Find(pos)
	END Search;

(** Convert a string into an integer. Leading white space characters are ignored. *)
	PROCEDURE StrToInt*(VAR str: ARRAY OF CHAR; VAR val: LONGINT);
		VAR i, d: LONGINT; ch: CHAR; neg: BOOLEAN;
	BEGIN
		i := 0; ch := str[0];
		WHILE (ch # 0X) & (ch <= " ") DO
			INC(i); ch := str[i]
		END;
		neg := FALSE; IF ch = "+" THEN INC(i); ch := str[i] END;
		IF ch = "-" THEN neg := TRUE; INC(i); ch := str[i] END;
		WHILE (ch # 0X) & (ch <= " ") DO
			INC(i); ch := str[i]
		END;
		val := 0;
		WHILE (ch >= "0") & (ch <= "9") DO
			d := ORD(ch)-ORD("0");
			INC(i); ch := str[i];
			IF val <= ((MAX(LONGINT)-d) DIV 10) THEN
				val := 10*val+d
			ELSIF neg & (val = 214748364) & (d = 8) & ((ch < "0") OR (ch > "9")) THEN
				val := MIN(LONGINT); neg := FALSE
			ELSE
				HALT(99)
			END
		END;
		IF neg THEN val := -val END
	END StrToInt;

(** Convert an integer into a string. *)
	PROCEDURE IntToStr*(val: LONGINT; VAR str: ARRAY OF CHAR);
		VAR
			i, j: LONGINT;
			digits: ARRAY 16 OF LONGINT;
	BEGIN
		IF val = MIN(LONGINT) THEN
			COPY("-2147483648", str);
			RETURN
		END;
		IF val < 0 THEN
			val := -val; str[0] := "-"; j := 1
		ELSE
			j := 0
		END;
		i := 0;
		REPEAT
			digits[i] := val MOD 10; INC(i); val := val DIV 10
		UNTIL val = 0;
		DEC(i);
		WHILE i >= 0 DO
			str[j] := CHR(digits[i]+ORD("0")); INC(j); DEC(i)
		END;
		str[j] := 0X
	END IntToStr;

        PROCEDURE RealToStr*(x: LONGREAL; VAR s: ARRAY OF CHAR);
          VAR n, m, k, pos: INTEGER; tmp: ARRAY 16 OF CHAR;

          PROCEDURE W(ch: CHAR);
          BEGIN
            IF pos < LEN(s)-1 THEN
              s[pos] := ch; pos := pos+1
            END
          END W;

        BEGIN
          pos := 0;
          IF x < 0 THEN W('-'); x := -x END;
          ASSERT (x < 1.0E9);

          n := ENTIER(x);
          m := ENTIER(1.0E6 * (x - n) + 0.5);
          IF m = 1000000 THEN n := n+1; m := 0 END;

          IntToStr(n, tmp);
          FOR k := 0 TO Length(tmp)-1 DO W(tmp[k]) END;

          IF m > 0 THEN
            W('.');
            k := 100000;
            WHILE m > 0 DO
              W(CHR(m DIV k + ORD('0')));
              m := m MOD k; k := k DIV 10
            END
          END;

          s[pos] := 0X
        END RealToStr;          

	PROCEDURE Init();
		VAR i: LONGINT; 
	BEGIN
	
		FOR i := 0 TO 255 DO
			isAlpha[i] := ((i >= ORD("A")) & (i <= ORD("Z"))) OR ((i >= ORD("a")) & (i <= ORD("z")))
		END;
		isAlpha[ORD(80X)] := TRUE; isAlpha[ORD(81X)] := TRUE; isAlpha[ORD(82X)] := TRUE;
		isAlpha[ORD(83X)] := TRUE; isAlpha[ORD(84X)] := TRUE; isAlpha[ORD(85X)] := TRUE;
		isAlpha[ORD(86X)] := TRUE; isAlpha[ORD(87X)] := TRUE; isAlpha[ORD(88X)] := TRUE;
		isAlpha[ORD(89X)] := TRUE; isAlpha[ORD(8AX)] := TRUE; isAlpha[ORD(8BX)] := TRUE;
		isAlpha[ORD(8CX)] := TRUE; isAlpha[ORD(8DX)] := TRUE; isAlpha[ORD(8EX)] := TRUE;
		isAlpha[ORD(8FX)] := TRUE; isAlpha[ORD(90X)] := TRUE; isAlpha[ORD(91X)] := TRUE;
		isAlpha[ORD(92X)] := TRUE; isAlpha[ORD(93X)] := TRUE; isAlpha[ORD(94X)] := TRUE;
		isAlpha[ORD(95X)] := TRUE; isAlpha[ORD(96X)] := TRUE;
		FOR i := 0 TO 255 DO
			ISOToOberon[i] := CHR(i); OberonToISO[i] := CHR(i)
		END;
		ISOToOberon[8] := 7FX;
		ISOToOberon[146] := 27X;
		ISOToOberon[160] := 20X;
		ISOToOberon[162] := 63X;
		ISOToOberon[166] := 7CX;
		ISOToOberon[168] := 22X;
		ISOToOberon[169] := 63X;
		ISOToOberon[170] := 61X;
		ISOToOberon[171] := 3CX;
		ISOToOberon[173] := 2DX;
		ISOToOberon[174] := 72X;
		ISOToOberon[175] := 2DX;
		ISOToOberon[176] := 6FX;
		ISOToOberon[178] := 32X;
		ISOToOberon[179] := 33X;
		ISOToOberon[180] := 27X;
		ISOToOberon[183] := 2EX;
		ISOToOberon[185] := 31X;
		ISOToOberon[186] := 30X;
		ISOToOberon[187] := 3EX;
		ISOToOberon[192] := 41X;
		ISOToOberon[193] := 41X;
		ISOToOberon[194] := 41X;
		ISOToOberon[195] := 41X;
		ISOToOberon[196] := 80X; OberonToISO[128] := 0C4X;
		ISOToOberon[197] := 41X;
		ISOToOberon[198] := 41X;
		ISOToOberon[199] := 43X;
		ISOToOberon[200] := 45X;
		ISOToOberon[201] := 45X;
		ISOToOberon[202] := 45X;
		ISOToOberon[203] := 45X;
		ISOToOberon[204] := 49X;
		ISOToOberon[205] := 49X;
		ISOToOberon[206] := 49X;
		ISOToOberon[207] := 49X;
		ISOToOberon[208] := 44X;
		ISOToOberon[209] := 4EX;
		ISOToOberon[210] := 4FX;
		ISOToOberon[211] := 4FX;
		ISOToOberon[212] := 4FX;
		ISOToOberon[213] := 4FX;
		ISOToOberon[214] := 81X; OberonToISO[129] := 0D6X;
		ISOToOberon[215] := 2AX;
		ISOToOberon[216] := 4FX;
		ISOToOberon[217] := 55X;
		ISOToOberon[218] := 55X;
		ISOToOberon[219] := 55X;
		ISOToOberon[220] := 82X; OberonToISO[130] := 0DCX;
		ISOToOberon[221] := 59X;
		ISOToOberon[222] := 50X;
		ISOToOberon[223] := 96X; OberonToISO[150] := 0DFX;
		ISOToOberon[224] := 8BX; OberonToISO[139] := 0E0X;
		ISOToOberon[225] := 94X; OberonToISO[148] := 0E1X;
		ISOToOberon[226] := 86X; OberonToISO[134] := 0E2X;
		ISOToOberon[227] := 61X;
		ISOToOberon[228] := 83X; OberonToISO[131] := 0E4X;
		ISOToOberon[229] := 61X;
		ISOToOberon[230] := 61X;
		ISOToOberon[231] := 93X; OberonToISO[147] := 0E7X;
		ISOToOberon[232] := 8CX; OberonToISO[140] := 0E8X;
		ISOToOberon[233] := 90X; OberonToISO[144] := 0E9X;
		ISOToOberon[234] := 87X; OberonToISO[135] := 0EAX;
		ISOToOberon[235] := 91X; OberonToISO[145] := 0EBX;
		ISOToOberon[236] := 8DX; OberonToISO[141] := 0ECX;
		ISOToOberon[237] := 69X;
		ISOToOberon[238] := 88X; OberonToISO[136] := 0EEX;
		ISOToOberon[239] := 92X; OberonToISO[146] := 0EFX;
		ISOToOberon[240] := 64X;
		ISOToOberon[241] := 95X; OberonToISO[149] := 0F1X;
		ISOToOberon[242] := 8EX; OberonToISO[142] := 0F2X;
		ISOToOberon[243] := 6FX;
		ISOToOberon[244] := 89X; OberonToISO[137] := 0F4X;
		ISOToOberon[245] := 6FX;
		ISOToOberon[246] := 84X; OberonToISO[132] := 0F6X;
		ISOToOberon[248] := 6FX;
		ISOToOberon[249] := 8FX; OberonToISO[143] := 0F9X;
		ISOToOberon[250] := 75X;
		ISOToOberon[251] := 8AX; OberonToISO[138] := 0FBX;
		ISOToOberon[252] := 85X; OberonToISO[133] := 0FCX;
		ISOToOberon[253] := 79X;
		ISOToOberon[254] := 70X;
		ISOToOberon[255] := 79X;
	END Init;

BEGIN
	Init()
END Strings1.

