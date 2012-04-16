unit mtx;
{ Details of M-Tx -> PMX translation }

interface uses globals;

procedure scanMusic(voice: voice_index; var left_over: integer);
procedure markDebeamed(voice: voice_index; var note: string);
procedure checkOctave(voice: voice_index; var note: string);
function rests (len, meterdenom: integer; blind: boolean): string;
procedure initMTX;

implementation  uses control, mtxline, strings, status, notes;

const flagged = '8136';

type sticky = array['a'..'z'] of boolean;

var note_attrib, rest_attrib: array[voice_index] of sticky;

function logTwo(denom: integer): integer;
  var l: integer;
begin  l:=0; while denom>1 do begin inc(l); denom:=denom div 2; end;
  logtwo:=l;
end;

{ RESTS should have a third parameter OFFSET.  At present we in effect
  assume OFFSET+LENGTH to be a multiple of METERDENOM. }
function rests (len, meterdenom: integer; blind: boolean): string;
  var r, bl: string;
  function dotted(n: integer): boolean;
  begin  dotted:=false;  if n=0 then exit;
    if n<>3*(n div 3) then exit;
    n:=n div 3;  while n mod 2 = 0 do n:=n div 2;
    dotted := n=1;
  end;
  begin  if blind then bl:='b' else bl:='';
    if len=0 then rests:='' else
    if len=128*meterdenom then rests:='r9'+bl+' ' else
    if dotted(len) then
    begin r:=rests(len-len div 3,meterdenom,false);
      r[3]:='d'; rests:=r+bl+' ';
    end
    else if len>=64 div meterdenom then
      rests:=rests(len-64 div meterdenom,meterdenom,blind)+'r'
        +durations[whole+logtwo(meterdenom)]+bl+' '
    else rests:=rests(len,meterdenom*2,blind);
  end;

procedure checkSticky(var note: string; var attrib: sticky);
  var i,l: integer;
      c: char;
      a: string;
  function attribs(note: string): string;
    var a: string;
        i,l: integer;
        n: char;
  begin  a:='';  i:=2;  l:=length(note);
    while i<=l do
    begin  n:=note[i];
      if n='x' then i:=l else if (n>='a') and (n<='z') then a:=a+n;
      inc(i);
    end;
    attribs:=a;
  end;
begin  if note='' then exit;
  l:=length(note); i:=2;
  while i<=l do
  begin c:=note[i];
    if (c>='a') and (c<='z') then
      if (i<l) and (note[i+1]=':') then
        begin delete1(note,i+1); dec(l); attrib[c]:=not attrib[c]; end
      else attrib[c]:=false;
    inc(i);
  end;
  a:=attribs(note);
  for c:='z' downto 'a' do
  if attrib[c] and (pos1(c,a)=0) then insertchar(c,note,3);
end;

{ Routine was patched by Hiroaki Morimoto, revised 2003/08/15.  
  Rewritten by DPL 2004/05/16 to be more understandable. 
  octaveCode returns octave adjustments in the order 
  [absolute or =],[relative] }
procedure checkOctave(voice: voice_index; var note: string);
  var code: char;
begin  
  code:=octaveCode(note);   
  if code='=' then begin setOctave(voice); removeOctaveCode(code,note); end;
  if octave(voice)=blank then exit; 
  code:=octaveCode(note);
  if (code>='0') and (code<='9') then begin resetOctave(voice); exit end; 
  while (code='+') or (code='-') do
  begin 
    newOctave(voice,code); removeOctaveCode(code,note); code:=octaveCode(note)
  end;
  if code<>' ' then error3(voice,'You may have only one absolute octave assignment');
  insertOctaveCode(octave(voice),note); resetOctave(voice)
end;

procedure markDebeamed(voice: voice_index; var note: string);
begin
  if isVocal(voice) and (afterSlur(voice)=0) and unbeamVocal
     and (pos1(duration(voice),flagged)>0) then insertchar('a',note,2);
end;

function barLength (meter: string): integer;
  var n1, n2, pn1, pn2: integer;
begin  
  getMeter(meter,n1,n2,pn1,pn2); barLength := n1*(64 div n2);
end;

function isMultiBarRest(rest: string): boolean;
begin  isMultibarRest:=false;  if length(rest)<3 then exit;
  if rest[2]<>'m' then exit;
  if multi_bar_rest then error(
    'Only one multibar rest allowed per line',print);
  multi_bar_rest := true; isMultibarRest:=true;
end;

{ Double-length in xtuplet detected by a brute search for D anywhere.
  Could be more elegant. }

var macro_length: array[1..20] of integer;
const macro_ID: array[1..20] of string[2]=('1','2','3','4','5','6','7','8',
         '9','10','11','12','13','14','15','16','17','18','19','20');

procedure scanMusic(voice: voice_index; var left_over: integer);
  var buf, note, enote, xnote: string;
      has_next, done, doublex, store_macro: boolean;
      bar, old_length, bar_length, count, l, mlen, ngrace, nmulti,
        macroID, playID: integer;
      dur1, lastdur, macro_type: char;
      nscan: music_word;

  procedure decmulti;
  begin if doublex then dec(nmulti,2) else dec(nmulti)
  end;

  procedure countIt;
    procedure incbar(nl: integer);
    begin inc(bar_length,nl);
      if (bar_length>bar) and (meternum>0)
        then error3(voice,'Bar end occurs in mid-note');
    end;
  begin
    if ngrace>0 then dec(ngrace) else
    if nmulti>0 then decmulti else
    begin  if (count mod 3<>0) and (
        (note[1]<>rest) and note_attrib[voice,'d'] or
        (note[1]=rest) and rest_attrib[voice,'d']) then
        inc(count,count div 2);
      incbar(count);
      l := pos1(multi_group,note);
      if l>0 then
      begin  predelete(note,l);  getNum(note,nmulti); decmulti;
      end;
    end
  end;

  procedure maybeGroup;
  begin if note[1]=grace_group then
    begin  if length(note)=1 then ngrace:=1
      else ngrace := pos1(note[2],digits);  (* bug if ngrace>9 *)
      if ngrace>0 then dec(ngrace);
    end;
  end;
  
  function identifyMacro(s: string): integer;
    var k: integer;
  begin predelete(s,2); for k:=1 to 20 do if s=macro_ID[k] then 
    begin identifyMacro:=k; exit end;
    identifyMacro:=0
  end; 

  procedure examineMacro;
  begin if not countMacro then exit;
    if debugMode then write(note,': '); if length(note)=1 then 
    begin 
      mlen:=bar_length-old_length; 
      if (macroID<1) or (macroID>20) then error3(voice,'Invalid macro ID');
      macro_length[macroID]:=mlen; 
      if debugMode then
      writeln('Ending macro ',macroID,' of type ',macro_type,', length=', mlen);
      if macro_type='S' then bar_length:=old_length; store_macro:=false
    end
    else begin playID:=identifyMacro(note);
      if (playID<1) or (playID>20) then error3(voice,'Invalid macro ID');
      if note[2]='P' then 
      begin if debugMode then
        writeln('Playing macro ',playID,' of length ', 
          macro_length[playID]); 
        inc(bar_length,macro_length[playID])
      end
      else if pos1(note[2],'SR')>0 then 
      begin old_length:=bar_length; macro_type:=note[2]; macroID:=playID;
        store_macro:=true;
        if debugMode then writeln('Defining macro ',macroID,' of type ',note[2]) 
      end
    end
  end;

begin
  terminators:='d.x';
  resetInfo(voice, buf);  has_next := false;  left_over:=0;  store_macro:=false;
  bar:=full_bar;  bar_length:=0;  ngrace:=0;  nmulti:=0;
  if meternum=0 then bar:=32000;
  dur1:=duration(voice);  lastdur:=dur1;
  done:=false;
  repeat  GetNextMusWord(buf,note,nscan);  if length(note)=0 then break; count:=0;
{    if isNoteOrRest(note) and not (isPause(note) or isMultibarRest(note))
      then note:=toStandard(note); }
    doublex := pos1('D',note)>0; 
    if nscan=mword then
    begin 
      if length(note)=0 then 
         Error3(voice,'You may not end a line with a meter change');
      if bar_length>0 then
      error3(voice,'Meter change only allowed at start of bar')
      else bar:=barLength(note);
    end
    else if nscan=rword then
      if not (isPause(note) or isMultiBarRest(note)) then
      begin
        processNote(note,xnote,dur1,lastdur,count);
        checkSticky(note,rest_attrib[voice]);
      end;
    if note<>'' then  appendNote(voice,nscan);
    enote := note;
    if (nscan=macro) or (nscan=endMacro) then examineMacro;
    if nscan=abcdefg then
      if (not multi_bar_rest) and (ngrace + nmulti = 0) then
      begin
        processNote(enote,xnote,dur1,lastdur,count);
        if xnote<>'' then
        begin
          checkSticky(enote,note_attrib[voice]);
          appendToLine(voice,enote);
          appendNote(voice,nscan); enote := xnote;
        end;
        checkSticky(enote,note_attrib[voice]);
      end;
    appendToLine(voice,enote);
    if note='' then done:=true;  { !!! else word_bound[here]:=length(line); }
    if (note=barsym) then
    if meternum=0 then
      error3(voice,'You may not use bar lines in barless music')
    else if bar_length=0 then markBar(voice)
    else if (numberOfBars(voice)=0) and (bar_length<bar) then
    begin  if has_next then
      has_next:=false  {Should check whether pickups are equal}
      else if left_over>0 then error3(voice,'Bar is too short');
      left_over:=bar_length;  bar_length := 0;
    end;
    if nscan=nextvoice then
    begin  if bar_length>0 then error3(voice, 'Next voice before bar is full')
      else barForward(voice,-1);  has_next := true;
    end
    else  if isPause(note) then  inc(bar_length,bar)
    else  if multi_bar_rest then {do nothing}
    else if not done and isNoteOrRest(note) then countIt
      else maybeGroup;
    dur1:=lastdur; 
    if (bar_length>=bar) and (ngrace+nmulti=0) and not store_macro then
    begin if debugMode then writeln(voice,' ',bar_length);
      barForward(voice,bar_length div bar); bar_length:=bar_length mod bar
    end;
  until done;
  setExtraLength(voice,bar_length);
  resetDuration(voice,dur1);
  regroup(voice)
end;

procedure initMTX;
  var i: voice_index;  j: char;
begin  for i:=1 to maxvoices do  for j:='a' to 'z' do
  note_attrib[i,j] := false;
  rest_attrib := note_attrib;
end;

end.
