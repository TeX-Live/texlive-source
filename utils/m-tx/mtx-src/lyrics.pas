unit lyrics;
{ Everything related to lyrics }

interface  uses globals;

procedure initLyrics;
procedure clearTags;
procedure reviseLyrics;
procedure lyrTranslate(var P: string; var numbered: boolean);
procedure getSyllable(voice: voice_index; var pretex: string);
function lyricsReport (voice: voice_index): string;
procedure maybeLyrics (voice: voice_index; parline: paragraph_index;
  w: string);
procedure extractLyrtag (voice: voice_index; var note: string);
procedure lyricsAdjust (voice: voice_index; var note: string);
procedure assignLyrics(stave: stave_index; var lyrassign: string);
procedure lyricsParagraph;
function hasVerseNumber(voice: voice_index): boolean;

implementation uses control, strings, mtxline, status, files, utility;

{ Symbols used in strings of melismatype }
const
  beam_melisma = '[';
  inhibit_beam_melisma = '<';
  slur_melisma = '(';
  inhibit_slur_melisma = '{';

type
  other_index = 1..10;
  other_index0 = 0..10;
  melismatype = string;
  haslyrtype = (nolyr, haslyr);
  auxtype= (normal, aux);
  assigntype = (asbefore, newassign);
  inittype = (virgin,active);
  lyrinfotype = record
    lyr_adjust, slur_level, slur_change, beam_level, beam_change: integer;
    melisma: melismatype;
    numbered: boolean;
  end;

  lyrlinetype = (none_given, global_lyrics, local_lyrics);
  lyrtagtype = record
    lyrsource: lyrlinetype;
    has_lyrics: haslyrtype;
    auxiliary: auxtype;
    new_assign: assigntype;
    initialized: inittype;
    linecount: integer;
    tags: string;
  end;

const maxLyrNums=64;
      lyrtaglength=40;
		  LyrNum: integer=0;

var lyrmodealter, oldlyrmodealter: array[stave_index] of boolean;
    tag, oldtag: array[voice_index] of lyrtagtype;
    lyrinfo: array[voice_index] of lyrinfotype;
    numberedParagraph: array[1..maxLyrNums] of string[lyrtaglength];
  
const  lyr_adjust_undef = -12345;
       interstave = 24;

{ Information flow for verse numbers: at the SetLyrics stage, the first
line of each lyrics paragraph is checked for a verse number, and if
found, the paragraph tag is remembered.  At the AssignLyrics stage,
the first time that paragraph is encountered, the particular voice
is marked in lyrinfo as being numbered.  When that voice is later processed,
the mark in lyrinfo is interrogated and turned off: if it was on, the
mtxVerse pre-TeX string is put into the output. }

function hasVerseNumber(voice: voice_index): boolean;
begin hasVerseNumber:=lyrinfo[voice].numbered; 
  lyrinfo[voice].numbered:=false;
end;  

function isNumberedLyricsParagraph(tag: string): boolean;
  var i: integer;
begin  
   for i:=1 to LyrNum do if tag=numberedParagraph[i] then
   begin isNumberedLyricsParagraph:=true; exit; end;
   isNumberedLyricsParagraph:=false;
end;

function anyTagNumbered(tags: string): boolean;
  var s: string;
begin  anyTagNumbered:=false;
  if curtail(tags,'}')>0 then delete1(tags,1);
  repeat s:=getNextWord(tags,',',dummy); 
    if s='' then exit; 
    if isNumberedLyricsParagraph(s) then 
    begin anyTagNumbered:=true; exit; end;
  until false; 
end;

{ Save the tag in the numberedParagraph list, stripping off braces if any }
procedure markNumbered(tag: string);
begin  if LyrNum >= maxLyrNums then
  begin error('Too many numbered lines in the lyrics',not print); exit; end;
  inc(LyrNum); 
  if curtail(tag,'}')>0 then delete1(tag,1);
  numberedParagraph[LyrNum] := tag;
end;

{ --- end of procedures to keep track of verse numbering --- }

procedure lyricsParagraph;
  var first, w: string;
      l, line: paragraph_index0;
      i, nother: other_index0;
      other: array[other_index] of string[lyrtaglength];
      numbered: boolean;
begin  if not doLyrics then exit;
  if para_len<2 then fatalerror('empty lyrics paragraph');
  w:=NextWord(P[1],blank,dummy);  l:=length(w);  line_no := orig_line_no[1];
  if w[l]<>'}' then  w:=w+'}';
  first:=GetNextWord(w,dummy,'}');  nother:=0;
  while w<>'' do
  begin if w[1]='=' then predelete(w,1);
    if w[1]<>'{' then w:='{'+w;
    inc(nother);  other[nother]:=GetNextWord(w,dummy,'}');
  end;
  if beVerbose then
  begin  write('---- Paragraph ', paragraph_no, ' starting at line ', line_no,
    ' has lyrics headed ',first);
    for i:=1 to nother do  write('=',other[i]);  writeln;
  end;
  TeXtype2(comment+' Paragraph '+toString(paragraph_no)+' line '+
     toString(line_no)+' bar '+toString(bar_no));
  TeXtype2('\mtxSetLyrics'+first+'{%');
  for line:=2 to para_len do
  begin  lyrTranslate(P[line],numbered);  
    if numbered then if (line>2) then  warning(
      'Verse number not in first line of paragraph treated as lyrics',print)
      else
    begin  markNumbered(first); for i:=1 to nother do markNumbered(other[i]);
    end;
    line_no:=orig_line_no[line];
    if (length(P[line])>max_lyrics_line_length) and pmx_preamble_done
      then error('Lyrics line too long',print);
    if pmx_preamble_done then
      if line=2 then put('\\\:'+P[line],putspace)
        else put('\\\ ' + P[line],putspace)
        else put(P[line],putspace);
    if line<para_len then
      if pmx_preamble_done then putLine(' %\')
        else putLine(' %')
    else if pmx_preamble_done then putLine('}\')
        else putLine('}')
  end;
  for i:=1 to nother do TeXtype2('\mtxCopyLyrics'+first+other[i]);
end;

function songraise(voice: voice_index): string;
  var s: string;
begin with tag[voice] do with lyrinfo[voice] do
  if (initialized=virgin) and (lyr_adjust=0) then songraise:='' else
  begin
    if auxiliary=aux then s:='Aux' else s:='';
    songraise:='\mtx'+s+'LyricsAdjust{'+toString(PMXinstr(voiceStave(voice)))
      +'}{'+toString(lyr_adjust)+'}';
  end;
end;

function lyricsReport (voice: voice_index): string;
  var l: string;
begin  with tag[voice] do
  if (has_lyrics=nolyr) or (lyrsource=none_given) and (tags='') then
    lyricsReport := ' but has no own lyrics'
  else begin
    l := ' with ';
    if auxiliary=aux then l := l + 'auxiliary ';
    l := l + 'lyrics ';
    if lyrsource=local_lyrics then l:= l + 'locally defined as "'
    else l := l + 'labelled "';
    l := l + tags + '"';
    if anyTagNumbered(tags) then l := l + ' with verse numbers';
    lyricsReport := l;
  end;
end;

procedure initLyrics;  { at the start only }
  var stave: stave_index;
      voice: voice_index;
begin  for voice:=1 to nvoices do
  with lyrinfo[voice] do  with tag[voice] do
  begin has_lyrics:=nolyr;  lyr_adjust:=lyr_adjust_undef;
    melisma:=''; slur_level:=0; beam_level:=0; auxiliary:=aux;
    lyrsource:=none_given;  new_assign := asbefore; initialized := virgin;
  end;
  for stave:=1 to nstaves do oldlyrmodealter[stave] := false;
end;

procedure registerLyrics (voice: voice_index; w: string);
begin
  with tag[voice] do
  begin  oldtag[voice].tags:=tags;
    oldtag[voice].lyrsource:=lyrsource;
    lyrsource:=global_lyrics; 
    case length(w) of 
   0: tags := '';
   1: FatalError('M-Tx system error in registerLyrics');
   else tags := w;
    end;
  end;
end;

procedure extractLyrtag (voice: voice_index; var note: string);
  { inline lyrics change }
begin  with tag[voice] do
  if has_lyrics=nolyr then
    error3(voice,'Inline lyrics change on no-lyrics line')
  else begin  registerLyrics(voice,note); { was: ''); }
    note:='\mtxAssignLyrics{'+toString(PMXinstr(voiceStave(voice)))+'}'+note;
    if auxiliary=aux then note:='\mtxAuxLyr{'+note+'}\' else note:=note+'\';
  end;
end;

procedure clearTags;  { at start of paragraph analyis }
  var voice: voice_index;
begin
  oldtag:=tag;
  for voice:=1 to nvoices do  with tag[voice] do
  begin
    lyrsource:=none_given;  tags := '';  linecount:=0;
  end;
end;

procedure maybeLyrics (voice: voice_index; parline: paragraph_index;
  w: string);
  { during paragraph analysis, parline had L:, already stripped }
  var k: voice_index0;
  procedure convertlyrics(var lyn: string; voice: voice_index; mx: auxtype);
    const setlyr: string = '%%\\\mtxSetLyrics';
          maxlyrlen = PMXlinelength-15;
    var btag, thistag, w: string;
        numbered: boolean;
  begin
    w:=NextWord(lyn,blank,dummy);  with tag[voice] do
    begin
      if w='' then  tags:=''
      else begin
        has_lyrics:=haslyr; auxiliary:=mx;
        if w[1]='{' then registerLyrics(voice,w)
        else begin  lyrsource:=local_lyrics;
          inc(linecount); thistag:=toString(10*voice+linecount);
          btag := '{'+thistag+'}';
          if tags='' then tags:=btag
          else begin tags[length(tags)]:=','; tags:=tags+thistag+'}'; end;
          trim(lyn);  lyrTranslate(lyn,numbered);  
          if numbered then markNumbered(thistag);
          if length(lyn)+length(btag)>maxlyrlen then
          lyn := setlyr + btag + '{\'^J'\\\:' + lyn +'}\'
          else lyn:= setlyr + btag + '{' + lyn +'}\';
        end;
      end;
    end;
  end;
begin  if not dolyrics then exit;
  if (length(w)=1) and (voice=0) then
    warning('Lyrics line above top voice should be labelled',print);
  if length(w)=1 then   {*  Standard lyrics line -------- }
  begin  k:=voice; if k=0 then k:=1; convertlyrics(P[parline],k,normal);
  end
  else  {*  Labelled lyrics line -- }
  begin  predelete(w,1); k:=findVoice(w);
    if k=0 then  error('Lyrics line belongs to unknown voice',print)
    else  convertlyrics(P[parline],k,aux);
  end;
end;

procedure reviseLyrics;  { after paragraph analysis }
  var voice : voice_index;
      stave : stave_index;
begin
  for voice:=1 to nvoices do  with tag[voice] do
  begin  if (oldtag[voice].lyrsource=global_lyrics) and
    (lyrsource=none_given) then
    begin tags:=oldtag[voice].tags; lyrsource:=global_lyrics; end;
    new_assign :=
      assigntype(ord((has_lyrics=haslyr) and (tags<>oldtag[voice].tags))); 
    if tags='' then has_lyrics := nolyr;
    oldtag[voice].tags := tags;
    oldtag[voice].lyrsource := lyrsource;
  end;
  for stave:=1 to nstaves do  with tag[first_on_stave[stave]] do
    lyrmodealter[stave] := not aloneOnStave(stave) and
      (has_lyrics=haslyr) and (auxiliary=normal);
end;

procedure assignLyrics(stave: stave_index; var lyrassign: string);
  { at start of new block }
  var atag, instr, l: string;
      v1, v2, voice: voice_index;
  begin  lyrassign:='';  instr := toString(pmxinstr(stave));
    v1:=first_on_stave[stave]; v2:=v1+number_on_stave[stave]-1;
    if v1<v2 then  { don't reassign if other voice takes over };
      if (tag[v1].auxiliary=tag[v2].auxiliary) and
         (tag[v1].has_lyrics<>tag[v2].has_lyrics) then
      for voice:=v1 to v2 do with tag[voice] do
        if new_assign=newassign then new_assign:=assigntype(has_lyrics);
    for voice:=v1 to v2 do lyrinfo[voice].numbered:=false;
    for voice:=v1 to v2 do
    with tag[voice] do  if new_assign=newassign then
    begin  atag:=tags;  if atag='' then atag:='{}';
      l := '\mtxAssignLyrics{' + instr + '}' + atag;
      if auxiliary=aux then l:='\mtxAuxLyr{' + l + '}';
      lyrassign := lyrassign + l;  
      if tags='' then has_lyrics:=nolyr;
      if (has_lyrics=haslyr) and (initialized=virgin) then
      with lyrinfo[voice] do
      begin  if (auxiliary=aux) and upper(voice) then
        lyr_adjust:=interstave else lyr_adjust:=0;
        lyrassign := lyrassign + songraise(voice);
        initialized := active;
      end;
      if anyTagNumbered(atag) then lyrinfo[voice].numbered:=true;
    end;
    if lyrmodealter[stave]<>oldlyrmodealter[stave] then
    begin  if lyrmodealter[stave] then
      lyrassign := lyrassign + '\mtxLyrModeAlter{' + instr + '}'
      else lyrassign := lyrassign + '\mtxLyrModeNormal{' + instr + '}';
      oldlyrmodealter[stave] := lyrmodealter[stave];
    end;
  end;

procedure lyricsAdjust (voice: voice_index; var note: string);
  { inline at-word }
  var adj: integer;
      force, put_above, put_below: boolean;
begin  with lyrinfo[voice] do with tag[voice] do
  begin predelete(note,1);
    force:=note[1]='=';  if force then predelete(note,1);
    put_above:=note[1]='^';  if put_above then predelete(note,1);
    put_below:=note[1]='v';  if put_below then predelete(note,1);
    if note<>'' then getNum(note,adj) else adj:=0;
    if has_lyrics=nolyr then begin note:=''; exit; end;
    if put_above then lyr_adjust:=interstave
      else if put_below then lyr_adjust:=0;
    if force then lyr_adjust:=adj else inc(lyr_adjust,adj);
    note:=songraise(voice);  if note<>'' then note:='\\'+note+'\';
  end
end;

procedure lyrTranslate(var P: string; var numbered: boolean);
  var k, l, number: integer;
      Q, w: string;
begin  { Test for starting number }  w:=NextWord(P,blank,dummy);
  numbered:=false;
  if endsWith(w,'.') then begin getNum(w,number); numbered:=number<>0 end;
  {Translate lyrics link }  Q := '';  l:=length(P);
  for k:=1 to l-1 do  
    if (P[k]<>'_') or (P[k+1]='_') or (P[k+1]=' ') or (pos1(P[k+1],digits)>0) 
      then Q := Q+P[k]
    else if (k>1) and (P[k-1]='\') then Q := Q+'mtxLowLyrlink '
    else Q := Q+'\mtxLyrlink ';
  Q:=Q+P[l];
  P := Q;
end;

type melismaEnd = (beam, slur);

function removeLast(var s: string; t: string): char;
  var i,l: integer;
begin l:=length(s);
  for i:=l downto 1 do  if pos1(s[i],t)>0 then
  begin  removeLast:=s[i]; delete1(s,i); exit; end;
  removeLast := dummy;
end;

function OpenMelisma(s: string): boolean;
begin  OpenMelisma := (pos1(slur_melisma,s)>0) or (pos1(beam_melisma,s)>0);
end;

{ Append mtxBM or mtxEM to pretex when appropriate }
procedure getSyllable(voice: voice_index; var pretex: string);
  var BM, EM: boolean;
  procedure startMelismas (t: string);
    var open_before, open_now: boolean;
        i, k: integer;
  begin  k:=length(t);  
    for i:=1 to k do with lyrinfo[voice] do
      begin open_before := OpenMelisma(melisma);
        melisma := melisma + t[i];
        open_now := OpenMelisma(melisma);
        BM := BM or (open_now and not open_before) 
      end;
  end;
  function endMelisma(voice: integer): boolean;
    var found: char;
        i: integer;
        count: array[melismaEnd] of integer;
        t: melismaEnd;
  begin  EM := false; 
    count[slur] := -lyrinfo[voice].slur_change; 
    count[beam] := -lyrinfo[voice].beam_change;
    for t:=beam to slur do for i:=1 to count[t] do with lyrinfo[voice] do  
    begin  
      case t of
  slur: found := removeLast(melisma,slur_melisma+inhibit_slur_melisma);
  beam: found := removeLast(melisma,beam_melisma+inhibit_beam_melisma);
      end;
      if found=dummy then
        error3(voice,'Ending a melisma that was never started');
      EM := EM or (not openMelisma(melisma)
        and (pos1(found,slur_melisma+beam_melisma)>0))
    end;
    endMelisma := EM
  end;
  procedure startSlurMelisma(voice: voice_index);
    const start: array[boolean] of char = (slur_melisma,inhibit_slur_melisma);
    var slurs: string;
        k: integer;
  begin  slurs:='';
    for k:=1 to lyrinfo[voice].slur_change do 
      slurs:=start[noSlurMelisma(voice,1-k)]+slurs;
    startMelismas(slurs)
  end;
  procedure startBeamMelisma(voice: voice_index);
    const start: array[boolean] of char = (beam_melisma,inhibit_beam_melisma);
    var beams: string;
        k: integer;
  begin beams:='';
    for k:=1 to lyrinfo[voice].beam_change do 
      beams:=start[noBeamMelisma(voice)]+beams;
    startMelismas(beams)
  end;
  function startMelisma(voice: integer): boolean;
  begin BM:=false; startSlurMelisma(voice); startBeamMelisma(voice);
    startMelisma := BM
  end;
  var t: integer;
begin  with tag[voice] do with lyrinfo[voice] do
  begin if has_lyrics<>haslyr then exit;  
    t:=slur_level; slur_level:=slurLevel(voice); slur_change:=slur_level-t;
    t:=beam_level; beam_level:=beamLevel(voice); beam_change:=beam_level-t;
    if startMelisma(voice) then if auxiliary=aux 
      then pretex:=pretex+'\mtxAuxBM' else pretex:=pretex+'\mtxBM';
    if endMelisma(voice) then if auxiliary=aux 
      then pretex:=pretex+'\mtxAuxEM' else pretex:=pretex+'\mtxEM'
  end
end;

procedure getSyllable1(voice: voice_index; var pretex: string);
begin with lyrinfo[voice] do 
  begin 
    writeln('voice=',voice, ', slurchange=',slur_change,
   ', melisma=',melisma, ', beamchange=', beam_change) 
  end;
  getSyllable1(voice,pretex);
  if pretex<>'' then writeln('pretex = ',pretex)
end;

end.
