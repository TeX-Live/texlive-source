unit mtxline;
{ Break input up into words, classify them.  Supply words and
  information about them. }

interface  uses globals;

type
  music_word = (other, abcdefg, zword, lyrtag,
      lparen, rparen, rlparen, lbrac, rbrac,
      pmxprefix, pmxl, macro, endmacro, mword, oword, rword, nextvoice,
      barword, texword, atword, FirstOnly, err );

const
  bind_left: array[music_word] of boolean = (
        false, false, true, false,
        false, true, true, false, true,
        true, true, false, true, false, true, false, true,
        false, false, false, false, false );
{ changed bind_left[barword] to false }

var selected: array[voice_index] of boolean;

procedure error3(voice: voice_index; message: string );
procedure warning3(voice: voice_index; message: string );
procedure getNextMusWord (var buf, note: string; var nscan: music_word);
function MusicWord(voice, n: integer): string;
function nextMusicWord(voice: voice_index): string;
function thisNote(voice: voice_index): music_word;
function nextNote(voice: voice_index): music_word;
function getMusicWord(voice: voice_index): string;
procedure gotoBar(voice: voice_index; bar_no: integer);
function endOfBar(voice: voice_index; bar_no: integer): boolean;
function getBar(voice:voice_index; bar: integer): string;
function upper(voice: voice_index): boolean;
procedure clearLabels;
function findVoice(w: string): voice_index0;
procedure selectVoices(line: string);
procedure resetInfo(voice: voice_index; var buf: string);
procedure setVocal(voice: voice_index; voc: boolean);
function isVocal(voice: voice_index): boolean;
procedure setStavePos(voice: voice_index; stave, pos: stave_index);
function voiceStave(voice: voice_index): stave_index;
function voicePos(voice: voice_index): stave_index;
function aloneOnStave(stave: stave_index): boolean;
function companion(voice: voice_index): voice_index;
procedure appendNote(voice: voice_index; nscan: music_word);
procedure appendToLine(voice: voice_index; note: string);
procedure markBar(voice: voice_index);
function numberOfBars(voice: voice_index): integer;
procedure barForward(voice: voice_index; nbars: integer);
procedure regroup(voice: voice_index);

function beatsPerLine: integer;
procedure setExtraLength(voice: voice_index; ext: integer);
function ExtraLength(voice: voice_index): integer;
function musicLineNo(voice: voice_index): paragraph_index0;
procedure setMusicLineNo(voice: voice_index; lno: paragraph_index);
function chordLineNo(voice: voice_index): paragraph_index0;
procedure setChordLineNo(voice: voice_index; lno: paragraph_index);
procedure skipChordBar(voice: voice_index);

procedure describeVoice(voice: voice_index; describe_lyr: string);
function maybeMusicLine(l: string): boolean;
function musicLine(voice: voice_index): string;

implementation uses strings, notes, control, utility;

const

  name: array[music_word] of string[9] = (
        '?', 'note', 'znote', 'lyricsTag',
        '(', ')', ')(', '[', ']',
        '_','PMX<', 'macro', 'endMacro', 'meter', 'ornament', 'rest', '//',
        'BAR', 'TeX', '@', 'firstonly', 'ERROR');

type
  word_scan = array[1..max_words] of music_word;

  line_info = record
      here, nword: word_index0;
      nbar: bar_index0;
      voice_pos, voice_stave, mus, chord: paragraph_index0;
      extra: integer;
    vocal: boolean;
    bar_bound: array[bar_index0] of word_index0;
    word_bound, orig_word_bound: array[word_index0] of integer;
    scan: word_scan;
    end;

var info: array[voice_index] of line_info;

{ ------------------------------------------------------------- }

function beatsPerLine: integer;
  var voice: voice_index;
begin  for voice:=1 to nvoices do  with info[voice] do
  if (nbar>0) or (extra>0) then
  begin
    if extra mod one_beat > 0
      then error3(voice,'Line length not an integer number of beats');
    beatsPerLine := nbar*meternum + extra div one_beat;
  end;
end;

procedure skipChordBar(voice: voice_index);
begin with info[voice] do
  if chord>0 then if P[chord]=barsym then predelete(P[chord],1);
end;

function getBar(voice: voice_index; bar: integer): string;
begin  with info[voice] do
  getBar := substr(P[mus],word_bound[bar_bound[bar-1]]+1,
             word_bound[bar_bound[bar]]-word_bound[bar_bound[bar-1]]);
end;

function musicLine(voice: voice_index): string;
begin musicLine:=P[musicLineNo(voice)]; end;

function musicLineNo(voice: voice_index): paragraph_index0;
begin musicLineNo := info[voice].mus; end;

procedure setMusicLineNo(voice: voice_index; lno: paragraph_index);
begin info[voice].mus := lno; end;

function chordLineNo(voice: voice_index): paragraph_index0;
begin chordLineNo := info[voice].chord; end;

procedure setChordLineNo(voice: voice_index; lno: paragraph_index);
begin info[voice].chord := lno; end;

procedure setVocal(voice: voice_index; voc: boolean);
begin info[voice].vocal:=voc; end;

function isVocal(voice: voice_index): boolean;
begin isVocal := info[voice].vocal; end;

procedure setStavePos(voice: voice_index; stave, pos: stave_index);
begin with info[voice] do begin voice_pos:=pos; voice_stave:=stave; end;
end;

function voiceStave(voice: voice_index): stave_index;
begin voiceStave:=info[voice].voice_stave; end;

function voicePos(voice: voice_index): stave_index;
begin voicePos:=info[voice].voice_pos; end;

function companion(voice: voice_index): voice_index;
  var s: integer;
begin  s:=info[voice].voice_stave;
  if number_on_stave[s]=1 then companion:=voice
  else if info[voice].voice_pos=1 then companion:=voice+1
  else companion:=voice-1;
end;

procedure regroup(voice: voice_index);
  var i, j, j2: word_index0;
begin  j2 := 0; with info[voice] do
  begin if debugMode then write('Voice ',voice,' has ',nbar,' bars at ');
     if debugMode then for i:=1 to nbar+1 do write(bar_bound[i],' ');
    for i:=1 to nbar do
    begin  j2:=bar_bound[i]; j:=j2+1; 
      while (j<=here) and (bind_left[scan[j]] or (scan[j]=barword)) do
      begin inc(bar_bound[i]); inc(j) end;
    end;
    if extra>0 then bar_bound[nbar+1]:=here;
     if debugMode then 
     begin write(' regrouped to ');  
       for i:=1 to nbar+1 do write(bar_bound[i],' ')
     end;
     if debugMode then writeln;
    nword := here
  end;
end;

procedure resetInfo(voice: voice_index; var buf: string);
begin  with info[voice] do
  begin buf := P[mus];  P[mus]:='';
    bar_bound[0]:=0;  word_bound[0]:=0; orig_word_bound[0]:=0; 
    nbar:=0;  here := 0;
  end;
end;

procedure clearLabels;
  var voice: voice_index;
begin for voice:=1 to nvoices do  with info[voice] do
    begin  chord:=0;  mus:=0; end;
end;

procedure appendNote(voice: voice_index; nscan: music_word);
begin with info[voice] do
  begin  inc(here); if here>max_words then
    error3(voice,'Words per line limit exceeded');
    scan[here]:=nscan;  end;
end;

procedure appendToLine(voice: voice_index; note: string);
begin if note<>'' then with info[voice] do
  begin P[mus]:=P[mus]+note+blank; word_bound[here]:=length(P[mus]); 
    orig_word_bound[here]:=nextWordBound(orig_P[mus],note[1],
      orig_word_bound[here-1]);
  end;
end;

procedure markBar(voice: voice_index);
begin with info[voice] do
  if nbar=0 then error3(voice,'Empty bar') else bar_bound[nbar]:=here
end;

function numberOfBars(voice: voice_index): integer;
begin  numberOfBars := info[voice].nbar; end;

procedure barForward(voice: voice_index; nbars: integer);
begin  with info[voice] do
  begin
    if (nbar+nbars<0) then error3(voice, 'Next voice before bar is full');
    if nbar+nbars>max_bars then error3(voice,'Bars per line limit exceeded');
    inc(nbar,nbars);
    if nbars>0 then bar_bound[nbar]:=here;
  end;
end;

procedure setExtraLength(voice: voice_index; ext: integer);
begin with info[voice] do begin extra:=ext; scan[here+1]:=other; end;
end;

function ExtraLength(voice: voice_index): integer;
begin ExtraLength := info[voice].extra; end;

function findVoice(w: string): voice_index0;
  var i: integer;
begin  curtail(w,':');  findVoice:=0;
  for i:=1 to nvoices do
  if w=voice_label[i] then begin findVoice:=i; exit end;
  getNum(w,i); if i=0 then exit;
  if (i>0) and (i<=nvoices) then findVoice:=i
    else error('Numeric label outside range 1..nvoices',print);
end;

procedure info3(voice: voice_index);
  var p: integer;
begin  with info[voice] do
  begin
    writeln ('In voice "', voice_label[voice], '" near word ', here, ':');
    p:=orig_word_bound[here-1]-1;
    if p<0 then p:=0;
    writeln(' ':p,'V')
  end
end;

procedure error3(voice: voice_index; message: string );
begin  info3(voice); error('   '+message,print)  end;

procedure warning3(voice: voice_index; message: string );
begin  info3(voice);  warning('   '+message,print)  end;

function nextMusicWord(voice: voice_index): string;
begin nextMusicWord := MusicWord(voice,info[voice].here); end;

function MusicWord(voice, n: integer): string;
  begin  with info[voice] do  if (n>0) and (n<=nword) then
    MusicWord := substr(P[mus],word_bound[n-1]+1,
      word_bound[n]-word_bound[n-1]-1)
    else MusicWord := '';
  end;

function thisNote(voice: voice_index): music_word;
begin with info[voice] do thisNote := scan[here-1]; end;

function nextNote(voice: voice_index): music_word;
begin with info[voice] do nextNote := scan[here]; end;

function endOfBar(voice: voice_index; bar_no: integer): boolean;
begin with info[voice] do endOfBar := here > bar_bound[bar_no]; end;

procedure gotoBar(voice: voice_index; bar_no: integer);
begin with info[voice] do here := bar_bound[bar_no-1]+1; end;

function getMusicWord(voice: voice_index): string;
begin  with info[voice] do
  begin line_no := orig_line_no[mus];
    GetMusicWord := MusicWord(voice,here); inc(here);
  end
end;

function maybeMusicLine(l: string): boolean;
  var w: string;
      nscan: music_word;
begin  w:=GetNextWord(l,blank,dummy);  maybeMusicLine := false;
  if pos1(w[1],'abcdefgr()[]{}CMm')=0 then begin  maybeMusicLine := false; exit end;
  if pos1(':',w)=0 then begin maybeMusicLine := true; exit end;
  GetNextMusWord(w,l,nscan);  maybeMusicLine := nscan=abcdefg;
end;

const macro_initialized: boolean=false;
      nmacros = 99;
var macro_text: array[1..nmacros] of string; 
procedure macroInit;   
  var i: integer;
begin if macro_initialized then exit else macro_initialized:=true;
  for i:=1 to nmacros do macro_text[i]:=''
end;

function identifyMacro(s: string): integer;
    var k: integer;
begin predelete(s,2);  getNum(s,k); identifyMacro:=k
end; 

procedure GetNextMusWord (var buf, note: string; var nscan: music_word);
    var maybe_error: string;

  procedure expandThisMacro;
    var playtag: string[10];
        playID: integer;
        w: string;
  begin macroInit; if length(note)=1 then 
    error('Can''t terminate a macro that has not been started',print);
    playID:=identifyMacro(note); playtag:=toString(playID);
    if (playID<1) or (playID>99) then 
      error('Macro ID '+playtag+' is not in range 1..99',print);
    if note[2]='P' then 
    begin if macro_text[playID]='' then 
      warning('Macro '+playtag+
        ' inserts empty text: did you define it before use?',print);
      if length(macro_text[playID])+length(buf)>255
      then error('Expansion of macro '+playtag+' causes buffer overflow',print)
      else begin if debugMode then 
        begin writeln('Inserting macro '+playtag+' text "'
           +macro_text[playID]+'"');
           writeln('Buffer before insert: ',buf)
        end;
        buf:=macro_text[playID]+buf; exit 
      end
    end;
    if pos1(note[2],'SR')=0 then error('Second character '+note[2]+
      ' of macro word should be in "PRS"',print);
    macro_text[playID]:='';
    repeat w:=getNextWord(buf,blank,dummy);
      if w='' then error('Macro definition '+playtag+
        ' should be terminated on the same input line',print);
      if w='M' then begin
        if debugMode then writeln('Macro '+playtag+' is: '+macro_text[playID]);
        break
      end;
      if (w[1]='M') and (length(w)>1) then
      if (w[2]<>'P') or (identifyMacro(w)=playID) then
        error(w+' not allowed inside macro definition '+playtag,print);
      macro_text[playID]:=macro_text[playID]+w+' '
    until false;
    if note[2]='R' then begin note[2]:='P'; buf:=note+' '+buf end
  end;

  begin note:=GetNextWord(buf,blank,dummy);
    if note='' then exit;
    if (note[1]='M') and expandMacro then 
    begin expandThisMacro; GetNextMusWord(buf,note,nscan); exit end;
    if note[1]='\' then
    begin maybe_error:='';
      if note[length(note)]<>'\' then maybe_error:=note;
      while (buf<>'') and (note[length(note)]<>'\') do
      note:=note+' '+GetNextWord(buf,blank,dummy);
      if note[length(note)]<>'\' then
        error('Unterminated TeX literal',print);
      nscan:=texword;
      if maybe_error<>'' then
        warning('Possible unterminated TeX literal: '+maybe_error,print);
      exit;
    end;
    if solfaNoteNames then
    begin translateSolfa(note[1]); if note[1]='"' then predelete(note,1);
    end;
    case note[1] of
'_':	begin nscan:=pmxprefix;delete1(note,1); end;
'a'..'g': nscan:=abcdefg;
'z':    nscan:=zword;
'(':    nscan:=lparen;
'{':    if note[length(note)]='}' then nscan:=lyrtag else nscan:=lparen;
')':    if pos1('(',note)=0 then nscan:=rparen else nscan:=rlparen;
'}':    if pos1('{',note)=0 then nscan:=rparen else nscan:=rlparen;
'[':    nscan:=lbrac;
']':    nscan:=rbrac;
'@':    nscan:=atword;
'm':    nscan:=mword;
'r':    nscan:=rword;
'#','-','n','x','?','s','t','D':      nscan:=pmxl;
'M':    if note='M' then nscan:=endMacro else nscan:=macro;
'G':    if pos1('A',note)>0 then nscan:=pmxl else nscan:=other;
'1'..'9': if pos1('/',note)>0 then nscan:=mword else nscan:=pmxl;
'o':    nscan:=oword;
'A', 'V':      nscan:=FirstOnly;
'/':    if note='//' then nscan:=nextvoice
        else if note='/' then
        begin note:='//'; warning('/ changed to //',print); nscan:=nextvoice;
        end
        else begin error('Word starts with /: do you mean \?',print);
          nscan:=err;
        end;
      else if pos1('|',note)>0 then nscan:=barword else
      nscan:=other;
    end;
  end;

function upper(voice: voice_index): boolean;
begin with info[voice] do
  if (voice_pos=1) and (voice<nvoices) then
    upper:=(voice_stave=info[voice+1].voice_stave)
  else upper:=false;
end;

procedure describeVoice(voice: voice_index; describe_lyr: string);
  var bar, w: integer;
begin  with info[voice] do
  begin  write('Voice `',voice_label[voice],''' is on line ',mus);
    if vocal then
    begin  write(', is vocal');  writeln(describe_lyr); end
    else
    begin if chord>0 then write(' and has chords on line ',chord);
      writeln;
    end;
    if debugMode then with info[voice] do
    begin  write('Line has ', nbar, ' bars');
      if extra>0 then writeln(' + ', extra, '/64 notes') else writeln;
      write('Bars:');
      for bar:=1 to nbar do  write(' ',getBar(voice,bar),' ');
      writeln;  write('Word types:');
      for w:=1 to nword do write(' ',name[scan[w]]);  writeln;
    end;
  end;
end;

function aloneOnStave(stave: stave_index): boolean;
  var v: voice_index;
begin  
  if number_on_stave[stave]<>2 then aloneOnStave:=true else
  begin v:=first_on_stave[stave];
    aloneOnStave:= (info[v].mus=0) or (info[v+1].mus=0)
  end
end;

procedure selectVoices(line: string);
  var i, k: integer;
      word: string;
  begin  
    for k:=1 to nvoices do selected[k]:=false;
    write('Voice change to: ',line,' = ');
    i:=0;
    while i<nvoices do
    begin  word:=GetNextWord(line,blank,dummy);
      if word='' then begin writeln; exit; end;
      inc(i); write(word,' ');
      k:=findVoice(word);
      if k=0 then error('This voice is not in the style',print);
      selected[k]:=true;
    end;
    writeln;
  end;

end.
