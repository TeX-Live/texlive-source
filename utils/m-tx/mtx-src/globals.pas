unit globals;

{ 1. All global variables.
  2. Miscellaneous other procedures required by several Units.
}

{ CMO: addition/change by Christian Mondrup }

interface

const PMXlinelength = 128;
{ !!! One or more of the following constants should be reduced if this
  program is to be compiled by a 16-bit compiler (e.g. Turbo Pascal),
  otherwise you get a "Data segment too large" error }
      lines_in_paragraph = 100;
      max_words = 128;
      max_notes = 128;
{ Christian Mondrup's suggestion to reduce data segment size:
      lines_in_paragraph = 50;
      max_words = 64;
      max_notes = 64;
}
      max_bars = 16;
      maxstaves = 15;
      maxvoices = 15;
      maxgroups = 3;
      standardPMXvoices = 12;
      max_lyrics_line_length = PMXlinelength-4;
      inf = 32000;
      unspec = 1000;
      default_size = 20;

      start_beam = '[';
      stop_beam = ']';
      rest = 'r';
      pause = 'rp';
      dotcode = 'd';
      grace_group = 'G';
      multi_group = 'x';
      barsym = '|';
      comment = '%';
      double_comment: string[2] = '%%';
      blank = ' ';
      dot = '.';
      comma = ',';
      colon = ':';
      tilde = '~';
      dummy = #0;

      ndurs = 8;
      durations: string[ndurs] = '90248136';
      unspecified = '5';  { Not a valid duration }
      whole = 2;  { position of '0' in durations }
      terminators: string = '.x';
      digits = '123456789';
      digitsdot = '0123456789.';
      has_duration: string[8] = 'abcdefgr';
      solfa_names: string[7] = 'drmfslt';

      choice: char = ' ';
      outfile_open: boolean = false;
      texdir: string = '';
      old_meter_word: string = '';
      outlen: integer = 0;
      putspace = true;
      nospace = false;
      ignore_input: boolean = false;
      print = true;

type
  paragraph_index = 1..lines_in_paragraph;
  voice_index = 1..maxvoices;
  stave_index = 1..maxstaves;
  bar_index0 = 0..max_bars;
  word_index0 = 0..max_words;
  paragraph_index0 = 0..lines_in_paragraph;
  voice_index0 = 0..maxvoices;
  stave_index0 = 0..maxstaves;
  paragraph = array[paragraph_index] of string;
  line_nos = array[paragraph_index] of integer;

var  voice_label: array[voice_index] of string;
     clef: array[stave_index] of char;
     instr, stave, first_on_stave, number_on_stave:
       array[stave_index] of voice_index0;
     stave_size: array[stave_index] of integer;
     nspace: array[stave_index0] of integer;
     nvoices, nstaves, ninstr, bottom, top: voice_index0;
     one_beat, full_bar, line_no, short_note, musicsize,
       meternum, meterdenom, pmnum, pmdenom, paragraph_no,
       bar_no, pickup, nbars, nleft: integer;
     para_len: paragraph_index0;
     xmtrnum0: real;
     P, orig_P: paragraph;
     orig_line_no: line_nos;
     infile, outfile, stylefile: text;
     default_duration: char;
     fracindent, this_version, this_version_date, multi_bar_rest: string;
     pmx_preamble_done, first_paragraph, final_paragraph, must_respace,
       must_restyle, some_vocal: boolean;

procedure error(message: string; printLine: boolean);
procedure fatalerror(message: string);
procedure warning(message: string; printLine: boolean);
function PMXinstr (stave: integer): integer;
procedure setDefaultDuration(meterdenom: integer);
procedure getMeter(line: string; 
  var meternum, meterdenom, pmnum, pmdenom: integer);
procedure setSpace(line: string);
function meterChange(n1, n2: integer; blind: boolean): string;
function meterWord (num, denom, pnum, pdenom: integer): string;
procedure cancel(var num,denom: integer; lowest: integer);
function isNoteOrRest(w: string): boolean;
function isPause(note: string): boolean;
{ CMO: }
function PMXmeterdenom(denom: integer): integer;

implementation  uses strings, control, utility;

function isNoteOrRest(w: string): boolean;
begin isNoteOrRest := pos1(w[1],has_duration)>0; end;

function isPause(note: string): boolean;
begin isPause:=startsWith(note,pause); end;

procedure cancel(var num,denom: integer; lowest: integer);
begin  while (num mod 2 = 0) and (denom>lowest) do
  begin num:=num div 2; denom:=denom div 2; end;
end;

function meterWord (num,denom,pnum,pdenom: integer): string;
begin  meterWord := 'm' + toString(num) + '/' + toString(denom) +
      '/' + toString(pnum) + '/' + toString(pdenom);
end;

function meterChange(n1, n2: integer; blind: boolean): string;
  var f, l: integer;
begin 
  if blind then
  begin  f:=64;  l:=n1 * (64 div n2);
    cancel(l,f,meterdenom);
    { CMO: process denominator value with function PMXmeterdenom }
    meterChange := meterWord (l,PMXmeterdenom(f),0,0);
    if meternum>0 then
    writeln('Blind meter change to ', l, '/', f, ' on line ', line_no);
  end
  { CMO: process denominator value with function PMXmeterdenom }
  else  meterChange := meterWord(n1,PMXmeterdenom(n2),0,0);
end;

procedure setSpace(line: string);
  var i: integer;
      word: string;
  begin 
    i := pos1(';',line);   
    if i>0 then
    begin
      getNum(substr(line,1,i-1),nspace[0]);      
      predelete (line,i)
    end;
    i:=0;
    while i<ninstr do
    begin  word:=GetNextWord(line,blank,dummy);
      if word='' then exit;
      inc(i); getNum(word,nspace[i]);
    end;
  end;
           
procedure onumber (s: string; var j, n1: integer);
  begin  if s[j]='o' then n1:=1 else if s[j]='1' then
    begin n1:=10+digit(s[j+1]); inc(j); end
    else n1:=digit(s[j]);
    inc(j);
  end;

procedure extractNumber(var s: string; var k: integer);
  var w: string;
begin  w:=getNextWord(s,'/',dummy);  getNum(w,k);
end;

procedure readMeter (meter: string;
    var meternum, meterdenom, pmnum, pmdenom: integer);
  var j: integer;
begin if meter[1]='m' then
  if pos1('/',meter)=0 then
    begin j:=2; onumber(meter,j,meternum); onumber(meter,j,meterdenom);
      onumber(meter,j,pmnum); onumber(meter,j,pmdenom);
    end
    else begin predelete(meter,1);
      extractNumber(meter,meternum); extractNumber(meter,meterdenom);
      extractNumber(meter,pmnum); extractNumber(meter,pmdenom);
    end
  else begin getTwoNums(meter, meternum, meterdenom);
    pmnum:=meternum; pmdenom:=meterdenom;
  end;
end;

procedure getMeter(line: string; 
     var meternum, meterdenom, pmnum, pmdenom: integer);
  var meter: string;
begin  meter:=GetNextWord(line,blank,dummy);
  if (meter='C/') or (meter='mC/')  then
  begin meternum:=2; meterdenom:=2; pmdenom:=5; pmnum:=0; end
  else if (meter='C') or (meter='mC') then
  begin meternum:=4; meterdenom:=4; pmdenom:=6; pmnum:=0; end
  else readMeter(meter,meternum,meterdenom,pmnum,pmdenom);
  if meterdenom=0 then Error
    (meter+': Meter denominator must be nonzero',print);
   { CMO: Convert PMX syntax meter denominator '0' to '1' to be used for
     prepmx duration checks }
  { if meterdenom=0 then meterdenom:=1; }
end;

function whereInParagraph(l: integer): integer;
  var j: integer;
begin whereInParagraph:=0;
  for j:=1 to para_len do if orig_line_no[j]=l then
    begin whereInParagraph:=j; exit end;
end;

procedure fatalerror(message: string);
begin setFeature('ignoreErrors',false); error(message,not print) end;

procedure error(message: string; printline: boolean);
  var j: integer;
begin 
  j:=whereInParagraph(line_no); if (j>0) and printline then 
    writeln(orig_P[j]);
  writeln (message, ': ERROR on line ', line_no);
  if (j>0) and printline then
  begin
    writeln ('The line has been modified internally to:');
    writeln(P[j]);
  end;
  if not ignoreErrors then
  begin if outfile_open then
    begin close(outfile); rewrite(outfile); close(outfile); end;
    if line_no=0 then line_no:=10000;
    halt(line_no);
  end;
end;

procedure warning(message: string; printline: boolean);
  var j: integer;
begin
  if line_no>0 then
  begin  writeln (message, ': WARNING on line ', line_no);
     if not printline then exit;
     j:=whereInParagraph(line_no); if j>0 then writeln(P[j])
  end
  else writeln (message, ': WARNING in preamble');
end;

function PMXinstr (stave: integer): integer;
begin  PMXinstr := ninstr + 1 - instr[stave];  end;

procedure setDefaultDuration(meterdenom: integer);
begin
  case meterdenom of
      1: default_duration:='0';
      2: default_duration:='2';
      4: default_duration:='4';
      8: default_duration:='8';
     16: default_duration:='1';
     32: default_duration:='3';
     64: default_duration:='6';
   end;
end;

function PMXmeterdenom(denom: integer): integer;
begin
   { CMO: Convert M-Tx meter denominators to PMX syntax }
   case denom of
     1	: PMXmeterdenom:=0;
     16	: PMXmeterdenom:=1;
     32	: PMXmeterdenom:=3;
     64	: PMXmeterdenom:=6;
   else 
     PMXmeterdenom:=denom;
   end;
end;

end.
