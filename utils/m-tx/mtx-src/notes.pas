unit notes;
{$V-}

interface uses control;

const count64: array['0'..'9'] of integer =
         ( 64, 4, 32, 2, 16, 0, 1, 0, 8, 128 );

procedure processNote(var note, xnote: string; dur1: char; var dur: char;
  var count: integer);
function durationCode (note: string): char;
function octaveCode (note: string): char;
procedure removeOctaveCode(code: char; var note: string);
procedure insertOctaveCode(code: char; var note: string);
procedure translateSolfa(var nt: char);

implementation uses strings, globals;

type 

parsedNote = record
  name: char;
  duration: string[1];
  octave: string[8];
  accidental, whatever, dotgroup, xtuplet: string[16];
  shortcut: string[32];
end;

procedure printNote(n: parsedNote);
begin with n do writeln(name,'|',duration,'|',octave,'|',accidental,'|',
  whatever,'|',dotgroup,'|',xtuplet,'|',shortcut)  
end;

{ If rearrangeNote is TRUE, translate original note to the following form:
  1. Note name.
  2. Duration.
  3. Octave adjustments.
  4. Everything except the other six items.
  5. Accidental with adjustments (rest: height adjustment)
  6. Dot with adjustments.
  7. Xtuplet group.
}

procedure translateSolfa(var nt: char);
  var k: integer;
begin  if solfaNoteNames then
  begin k:=pos1(nt,solfa_names);  if k>0 then nt:=has_duration[k]
  end
end;

function durationCode (note: string): char;
  var code: char;
  begin  durationCode:=unspecified;  if length(note)>1 then
    begin  code:=note[2]; if pos1(code,durations)>0 then durationCode:=code
    end
  end;

function half ( dur: char ) : char;
  var k: integer;
  begin  k:= pos1 (dur, durations );  half := dur;
    if k=0 then error ('Invalid duration '+dur,print)
    else if k>ndurs then error (dur+' is too short to halve',print)
    else half := durations[k+1];
  end;

procedure addDuration ( var note: string; dur: char);
begin if insertDuration then insertchar(dur,note,2); end;

{ Extract procedures.  All of these remove part of "note" (sometimes
  the part is empty) and put it somewhere else.  The part may be anywhere
  in "note", except when otherwise specified.}

{ Unconditionally extracts the first character. } 

procedure extractFirst(var note: string; var first: char);
begin first:=note[1];  predelete(note,1)
end;

{ Extracts at most one of the characters in "hits". }

procedure extractOneOf(var note: string; hits: string; var hit: string);
  var i, l: integer;
begin  l:=length(note); hit:='';
  for i:=1 to l do  if pos1(note[i],hits)>0 then
  begin hit:=note[i]; delete1(note,i); exit;
  end;
end;

{ Extracts contiguous characters in "hits" until no more are found.
  There may be more later. }

procedure extractContiguous(var note: string; hits: string; var hit: string);
  var i, l, len: integer;
begin  l:=length(note); len:=l; hit:='';
  for i:=1 to l do  if pos1(note[i],hits)>0 then
  begin
    repeat if pos1(note[i],hits)=0 then exit;
      hit:=hit+note[i]; delete1(note,i); dec(len)
    until len<i;
    exit;
  end;
end;

{ Extracts the specified character and everything after it. }

procedure extractAfter(var note: string; delim: char; var tail: string);
  var newlen: integer;
begin  newlen:=pos1(delim,note);  tail:='';  if newlen=0 then exit;
  dec(newlen); tail:=note; predelete(tail,newlen); note[0]:=char(newlen);
end;

{ Extracts the dot shortcut part of a note: comma shortcut is no problem
  because a comma cannot be part of a number. }

procedure extractDotShortcut(var note: string; var tail: string);
  var names, tail2: string;
      l, lt: integer;
      ch: char;
begin extractAfter(note,'.',tail); l:=1; lt:=length(tail);
  if (l<lt) and (tail[2]='.') then l:=2;
  if solfaNoteNames then names:=solfa_names else names:=has_duration;
  if (l<lt) and (pos1(tail[l+1],names)>0) then
    begin translateSolfa(tail[l+1]); exit end;
  if l=2 then error('".." followed by non-note',print);
  if l>=lt then begin note:=note+tail; tail:=''; exit end;
  ch:=tail[1]; predelete(tail,1);
  extractDotShortcut(tail,tail2); note:=note+ch+tail; tail:=tail2;
end;

{ Extracts a signed number. }

procedure extractSignedNumber(var note, number: string);
  var k: integer;
      note0: string;
begin  k:=pos1('+',note); if k=0 then k:=pos1('-',note);
  number:=''; if k=0 then exit;
  note0:=note;
  repeat number:=number+note[k]; delete1(note,k)
  until (k>length(note)) or (note[k]<>'0') and (pos1(note[k],digits)=0);
  if length(number)=1 then begin note:=note0; number:='' end
end;

{ Extracts a symbol followed by optional +- or <> shift indicators }

procedure extractGroup(var note: string; delim: char; var group: string);
  var gl, k, k0: integer;
      probe, nonumber: boolean;
      tail: string;
  procedure tryMore;
  begin  while (k<=gl) and (group[k]=group[1]) do inc(k) end;
  procedure try(s: string);
  begin  probe:=(k<gl) and (pos1(group[k],s)>0);  if probe then inc(k)
  end;
  procedure tryNumber;
    var dot: boolean;
  begin  nonumber:=true;  dot:=false;
    while (k<=gl) and (pos1(group[k],digitsdot)>0) do
    begin inc(k);  if group[k]='.' then
      if dot then error('Extra dot in number',print) else dot:=true
      else  nonumber:=false
    end
  end;
begin  extractAfter(note,delim,group); if group='' then exit;
  gl:=length(group); k:=2;
  if (gl>1) and (group[2]=':') then k:=3   else
  begin  tryMore;
    k0:=k; try('+-<>'); if probe then tryNumber;  if nonumber then k:=k0;
    k0:=k; try('+-<>'); if probe then tryNumber;  if nonumber then k:=k0;
  end;
  tail:=group; dec(k); group[0]:=char(k); predelete(tail,k);
  note:=note+tail
end;

procedure parseNote(note: string; var pnote: parsedNote);
var onlymidi: string;
begin  with pnote do
  begin
    shortcut:=''; xtuplet:=''; accidental:=''; dotgroup:=''; 
    duration:=''; octave:=''; onlymidi:=''; 
    extractFirst(note,name); 
    extractAfter(note,'x',xtuplet);
    extractAfter(note,',',shortcut);
    if shortcut='' then extractDotShortcut(note,shortcut);
    if name<>rest then
    begin extractGroup(note,'s',accidental);
      if accidental='' then extractGroup(note,'f',accidental);
      if accidental='' then extractGroup(note,'n',accidental);
    end;
{ Look for 'i' or 'c' anywhere in what is left of note.}
    if accidental<>'' then
    begin extractOneOf(note,'ic',onlymidi); accidental:=accidental+onlymidi
    end;
    extractGroup(note,'d',dotgroup);
    if name=rest then extractSignedNumber(note,accidental);
    extractOneOf(note,durations,duration);
    if note<>rest then extractContiguous(note,'=+-',octave);
    if (length(note)>0) and (note[1]>='0') and (note[1]<='9')
      then begin octave:=note[1]+octave; delete1(note,1) end;
    whatever := note
  end
end;

{ On input: "note" is a note word; "dur1" is the default duration.
  On output: "note" has possibly been modified;
    possibly been split into two parts, the second being "shortcut";
    "dur" is the suggested new default duration;
    "count" is the count of the total of "note" and "shortcut" }     
procedure processNote(var note, xnote: string; dur1: char; var dur: char;
  var count: integer);
var sc, origdur: string[2];
  multiplicity, l: integer;
  pnote: parsedNote;
begin xnote:=''; dur:=dur1;  
  if (note='') or not isNoteOrRest(note) or isPause(note) then exit;
  parseNote(note, pnote); 
  if debugMode then begin write(note,' => '); printNote(pnote) end;
  with pnote do
  begin
    if pos1('.',whatever)>0 then warning('Suspicious dot in word '+note,print);
    origdur := duration;
    if duration='' then dur:=dur1 else dur:=duration[1];
    count:=count64[dur]; if dotgroup<>'' then
    begin inc(count,count div 2);
      if startswith(dotgroup,'dd') then inc(count,count div 6)
    end;
    duration:=dur; if shortcut<>'' then
    begin  
      if dotgroup<>'' then
      error('You may not explicitly dot a note with a shortcut',print);
      sc:=shortcut[1]; predelete(shortcut,1);
      if sc='.' then
      begin  multiplicity:=1;
        if shortcut[1]='.' then
        begin inc(multiplicity); predelete(shortcut,1); sc:=sc+'.' end;
        inc(count,count);  dur1:=duration[1];
        for l:=1 to multiplicity do
        begin dotgroup:=dotgroup+dotcode; dur1:=half(dur1) end;
        addDuration(shortcut,dur1);
      end  else
      begin addDuration(shortcut,half(duration[1]));
        inc(count,count div 2)
      end
    end;
    if not insertDuration then duration := origdur;
    if rearrangeNote 
       then note := name + duration + octave + whatever 
         + accidental + dotgroup + xtuplet 
       else shortcut:=' ';
    if not insertDuration and (shortcut<>'') then shortcut:=sc+shortcut;
    xnote:=shortcut
  end
end;

function octaveCode (note: string): char; 
  var pnote: parsedNote;
begin {if debugMode then write('Octave code in note "',note,'" is ');}
  parseNote(note,pnote); with pnote do
  begin {if debugMode then writeln('"',octave,'"');} 
    if octave='' then octaveCode:=' ' else octaveCode:=octave[1]; end
end;

procedure removeOctaveCode(code: char; var note: string);
  var k, l: integer;
begin {if debugMode then writeln('remove ',code,' from ',note);} l:=length(note);
  for k:=1 to l do if note[k]=code then
    if (k=l) or (note[k+1]<'0') or (note[k+1]>'9') then
    begin delete1(note,k); exit end;
  fatalError('Code not found in note')
end;

procedure insertOctaveCode(code: char; var note: string);
  var l: integer;
begin {if debugMode then writeln('insert ',code,' into ',note); }
  l:=length(note);
  if (l<2) or (note[2]<'0') or (note[2]>'9') then 
    fatalError('Trying to insert octave into note without duration');
  if (l<=2) or (note[3]<'0') or (note[3]>'9') then insertChar(code,note,3)
  else writeln('Not inserting "',code,'", note already has octave code"')
end;

end.
