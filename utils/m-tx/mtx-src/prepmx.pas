program prepmx;
uses control, strings, globals, preamble, lyrics, mtx, analyze,
  mtxline, status, uptext, notes, files, utility;

{ CMO: addition/change by Christian Mondrup }

{* M-Tx preprocessor to PMX     Dirk Laurie }
const version = '0.63a';
      version_date = '<8 April 2018>';

{* See file "Corrections" for updates }

{* To do next:
{* Current bugs:
{* Old bugs:
   Does not check PMX syntax
{* Weaknesses that may later cause bugs:
   Decisions sometimes made on note, sometimes on scan
   Can 'here' overrun?
   Where does one note stop and the next begin?
   See comment on '!' in uptext
}
      chord_flat = 't';
      chord_left = 'l';
      blind = true;
      visible = false;

var
  last_bar: boolean;
  repeat_sign: string;
  bar_of_line{, bars_of_rest}: integer;

{ --------------- Bars and rests --------------- }

procedure writeRepeat(var bar: string);
  var repcode: string;
  begin  if bar='' then exit;
    repcode:='';
    if bar='||' then repcode:='Rd'
    else if (bar='|]') then repcode:='RD'
    else if bar='|:' then repcode:='Rl'
    else if bar=':|:' then repcode:='Rlr'
    else if bar=':|' then repcode:='Rr'
    else if last_bar and (bar='|') then repcode:='Rb';
    if repcode<>'' then putLine(' '+repcode);
    bar:='';
  end;

procedure supplyRests(voice: voice_index);
begin
  if (bar_of_line=1) and pedanticWarnings then
  begin  write('Bar ', bar_no, ' Voice ', voice);
    warning(' Filling missing voice with rests',not print);
  end;
  if pickup>0 then
  put(rests(pickup,meterdenom,visible),nospace);
  put(pause+' ',putspace);
end;

{ ---------------------------------------------------------------- }

procedure processLine(voice: voice_index; bar_no: integer);
  var chords, note, cutnote, pretex: string;
      par_line: paragraph_index;
      ngrace, nmulti: integer;
      no_chords, no_uptext: boolean;

  procedure output(note: string);
  begin if note<>'' then
    begin  { pretex is saved up until a spacing note or rest comes }
      if endsWith(note,'\') and (note[2]<>'\') then
      begin  curtail(note,'\');  pretex:=pretex+note; note:=''; exit end;
      if (pretex<>'') and isNoteOrRest(note) then
      begin  note:=pretex+'\ '+note;  pretex:=''  end;
      if (outlen>0) and (note[1]<>' ') then note:=' '+note;
      put(note,nospace);
    end;
    if thisNote(voice)=nextvoice then putLine('');
  end;

  procedure maybeDotted(var note: string);
  begin if length(note)<2 then exit;
    if note[2]='"' then
    begin note[2]:=note[1]; pretex:=pretex+'\mtxDotted' end
  end;

  procedure addChords;
    var w, nt, labels: string;
        j, mus_line: integer;
        chord_line: paragraph_index0;
        arpeggio, tieme: boolean;
        lab: char;
        pitches: int5;        
    procedure outChord;
      var k: integer;
    begin if nt='' then exit; tieme:=nt[1]='{'; if tieme then predelete(nt,1);
      if (nt='') or (nt[1]<'a') or (nt[1]>'g') then 
        error('Must have a note name at the start of "'+nt
           +'", not "'+nt[1]+'"',print);      
      renewChordPitch(voice,nt); 
      chords := chords+' z'+nt;  nt:='';
      for k:=1 to length(labels) do if chordPitch(voice)=pitches[k] then
      begin chords:=chords+' s'+labels[k]+'t'; labels[k]:=' ' end; 
      if tieme then begin chordTie(voice,lab); chords:=chords+' s'+lab+'t' end
    end;
  begin  saveStatus(voice); 
    getChordTies(voice, pitches, labels);
    chords:=''; chord_line:=chordLineNo(voice);
    if chord_line=0 then no_chords:=true;
    if no_chords then exit;
    w := getNextWord(P[chord_line],blank,dummy); 
    if (w=barsym) or (w='') then no_chords:=true;
    if (w=tilde) or no_chords then exit;
    mus_line:=line_no; line_no:=orig_line_no[chord_line];
    arpeggio := w[1]='?'; if arpeggio then
    begin chords:=' ?'; predelete(w,1); end;
    j:=1;  nt:='';
    while j<=length(w) do
    begin   
      if (w[j]='{') or ((nt<>'{') and (w[j]>='a') and (w[j]<='g')) then outChord
      else if w[j]=chord_flat then w[j]:='f'
      else if w[j]=chord_left then w[j]:='e';
      nt:=nt+w[j]; j:=j+1
    end;
    if nt<>'' then outChord;  if arpeggio then chords:=chords+' ?';
    note:=note+chords;
    for j:=1 to length(labels) do if labels[j]<>' ' then
      error('Tie on chord note started but not finished',print);
    line_no:=mus_line;
  end;

  function processOther(note: string): string;
  begin
    case thisNote(voice) of
other: if note[1]=grace_group then
      begin if length(note)=1 then ngrace:=1
        else ngrace := pos1(note[2],digits);
        if ngrace>0 then dec(ngrace);
      end;
{ For a zword, take note of pitch but do not change the contents }
{ Add spurious duration because repitch expects duration to be present }
zword: begin cutnote:=note; predelete(cutnote,1); insertchar('4',cutnote,2);
         checkOctave(voice,cutnote); renewPitch(voice,cutnote)
       end;
lyrtag: extractLyrtag(voice,note);
rbrac:  endBeam(voice);
rparen, rlparen: endSlur(voice,note);
lbrac: beginBeam(voice,note);
lparen: begin maybeDotted(note); beginSlur(voice,note); end;
mword: error3(voice,'Meter change must be the first word of its bar');
atword: lyricsAdjust(voice, note);
    end;
    processOther:=note;
  end;

  procedure lookahead;
  begin while bind_left[nextNote(voice)] do
      note:=note+' '+processOther(getMusicWord(voice));
  end;

 var l: integer;
     in_group: boolean;

  procedure processUsual;
  begin  
    begin if hasVerseNumber(voice) then pretex:=pretex+'\mtxVerse';
      l := pos1(multi_group,note);  
      if l>0 then scan1(note,l+1,nmulti);   
      activateBeamsAndSlurs(voice); 
      in_group:=false;
      if ngrace>0 then begin in_group:=true; dec(ngrace) end else
      if nmulti>0 then begin in_group:=true; dec(nmulti);  end;
      checkOctave(voice,note);  renewPitch(voice,note); 
      if not in_group then
      begin resetDuration(voice,durationCode(note)); markDebeamed(voice,note) end;
      lookahead; getSyllable(voice,pretex);  
      addUptext(voice, no_uptext, pretex);
      addChords;
    end;
  end;

begin
  pretex:='';   no_chords:=false; no_uptext := false;
  par_line:=musicLineNo(voice);
  nmulti :=0;  ngrace := 0;  line_no:=orig_line_no[par_line];
  repeat note:=getMusicWord(voice);  if note='' then exit;
    { if debugMode then writeln(voice,' ',note); }
    case thisNote(voice) of
  rword: begin if multi_bar_rest<>'' then
           begin if uptextOnRests then
               addUptext(voice, no_uptext, pretex);
           end
           else begin
             l := pos1(multi_group,note);
             if l>0 then scan1(note,l+1,nmulti);
             if nmulti>0 then begin in_group:=true; dec(nmulti);  end;
             if uptextOnRests then
               addUptext(voice, no_uptext, pretex);
             if not (isPause(note) or in_group) {0.63: allow rests in xtuples}
               then resetDuration(voice,durationCode(note));
           end
         end;
  abcdefg: processUsual;
  barword: begin
      if voice=nvoices then
      if endOfBar(voice,bar_no) then repeat_sign := note
        else writeRepeat(note);
      if note<>barsym then note:='';
      no_chords:=false;
    end;
  FirstOnly: if voice<>nvoices then note:='' else
      note:=processOther(note);
      else note:=processOther(note);
    end;
    output(note); 
  until endOfBar(voice,bar_no);
  if not no_chords then skipChordBar(voice);
end;

{ ------------------------------------------------------------------- }

procedure getMeterChange(voice: voice_index; var new_meter: string);
  var pn1, pn2: integer;
      w, new_command: string;
begin  if nextNote(voice)<>mword then exit;
  w:=getMusicWord(voice);
  getMeter(w, meternum, meterdenom, pn1, pn2);
  full_bar := meternum*(64 div meterdenom);
  { CMO: process denominator value with function PMXmeterdenom }
  new_command := meterWord(meternum, PMXmeterdenom(meterdenom), pn1, pn2);
  if (new_meter<>'') and (new_meter<>new_command)
    then error3(voice,'The same meter change must appear in all voices');
  new_meter:=new_command;
end;

procedure musicParagraph;
  var j, nvoice: voice_index0;
      new_meter, lyrassign: string;

  procedure putPMXlines;
    var i: paragraph_index;
  begin  for i:=1 to para_len do
    if startsWith(P[i],double_comment) then
    begin  predelete(P[i],2);  putLine(P[i]);  P[i]:='%';  end;
  end;

  procedure processOneBar;
    var m, cm: paragraph_index0;
        voice, cvoice: voice_index;
        ignore_voice, wrote_repeat, alone: boolean;
  begin
    if bar_of_line>1 then putLine(comment+'Bar '+toString(bar_no));
    last_bar := (bar_of_line=nbars) and final_paragraph;
    if last_bar and (repeat_sign='|') then repeat_sign:='';
    writeRepeat(repeat_sign);  new_meter := '';
    for voice:=nvoices downto 1 do
    if musicLineNo(voice)>0 then
    begin gotoBar(voice,bar_of_line); getMeterChange(voice,new_meter);
    end;
    if last_bar and (new_meter='') and (nleft>pickup) and (meternum>0) then
      new_meter := meterChange(nleft,64,true);
    if new_meter<>'' then putLine(new_meter);
    wrote_repeat := false;  
    for voice:=nvoices downto 1 do
    begin  ignore_voice:=not selected[voice];  cvoice:=companion(voice);
      m:=musicLineNo(voice); cm:=musicLineNo(cvoice);  
      alone:=(voice=cvoice) or ((m>0) and (cm=0))
        or ((m=0) and (cm=0) and (voice<cvoice)) or not selected[cvoice];
      if selected[voice] then
      begin 
        if m>0 then processLine(voice,bar_of_line)
          else if alone then supplyRests(voice) else ignore_voice:=true; 
        if last_bar and (repeat_sign<>'') and not wrote_repeat then
        begin writeRepeat(repeat_sign); wrote_repeat := true; end;
        if not ignore_voice then
        if alone or (voicePos(voice)=1) then putLine(' /')
          else putLine(' //');
      end;
    end;
    inc(bar_no); pickup:=0; putLine('');
  end;

  procedure putMeter(new_meter_word: string);
  begin if new_meter_word<>old_meter_word then putLine(new_meter_word);
    old_meter_word := new_meter_word;
  end;

  procedure processMBR;
  var s, bars_of_rest: integer;
      mbr: string;
  begin  
    mbr := multi_bar_rest;  
    predelete(mbr,2); getNum(mbr,bars_of_rest);
    bar_no := bar_no + bars_of_rest;
    for s:=1 to nstaves do 
    begin
      if pickup>0 then put(rests(pickup,meterdenom,visible),nospace);
      putLine('rm' + toString(bars_of_rest) + ' /')
    end;
    putLine('')
  end;

begin
  paragraphSetup(nvoice);
  if nvoice=0 then begin nonMusic; exit end
  else if nvoice>nvoices then
  begin  if nvoice=0 then
    error('No voices! Did you remember to to supply a Style?',not print);
    error('Paragraph has '+toString(nvoice)+
    ' voices but Style allows only '+toString(nvoices),not print); exit;
  end;
  if first_paragraph then includeStartString;
  if pmx_preamble_done and (not final_paragraph or (nvoice>0)) then
    putLine(comment +' Paragraph ' + toString(paragraph_no) +
   ' line ' + toString(orig_line_no[1]) + ' bar ' + toString(bar_no));
  testParagraph; rememberDurations;
  if beVerbose then describeParagraph;
  { ---- Knowing the score, we can start setting music ---------------- }
  if not pmx_preamble_done then
  begin  doPMXpreamble;
     put( comment + ' Paragraph ' + toString(paragraph_no) + ' line ' +
       toString(orig_line_no[1]) + ' bar ',putspace);
     if pickup>0 then putLine('0') else putLine('1');
  end;
  putPMXlines;
  if must_restyle then restyle;
  if some_vocal and ((nvoice>0) or not final_paragraph) then
  for j:=1 to ninstr do
  begin  assignLyrics(j,lyrassign);
    if lyrassign<>'' then putLine('\\'+lyrassign+'\');
  end;
  if must_respace then respace;
  if (meternum=0) then putMeter(meterChange(beatsPerLine,meterdenom,true));
  if nleft > 0 then inc(nbars);
  if (nbars=0) and (multi_bar_rest<>'') then 
    processMBR
  else for bar_of_line:=1 to nbars do
    processOneBar;
  restoreDurations;
end;

{ ----------------------------------------------------------------------- }

var no_commands_yet: boolean;

procedure doMusic;
begin  first_paragraph:=true;  pmx_preamble_done:=false;  bar_no:=1;
  repeat_sign:='';  must_respace:=false; must_restyle:=false;
  repeat final_paragraph := endOfInfile;
    orig_P := P;
    if (para_len>0) and not ignore_input and thisCase then
    begin  if no_commands_yet then
      begin interpretCommands;  printFeatures(false);
        one_beat := 64 div meterdenom;  full_bar := meternum*one_beat;
        if nvoices>standardPMXvoices then warning('You have '
           +toString(nvoices)+' voices; standard PMX can only handle '
           +toString(standardPMXvoices),not print);
        initMTX; initUptext; initStatus; initLyrics;
        no_commands_yet:=false
      end;
      if startsWithBracedWord(P[1]) then lyricsParagraph else
      begin 
        musicParagraph;  first_paragraph:=false;
        writeRepeat(repeat_sign);
      end
    end;
    readParagraph(P,orig_line_no,para_len);
  until para_len=0;
end;

var control_paragraph, no_report_errors: boolean;

function isControlParagraph (var P: paragraph; para_len: paragraph_index)
                : boolean;
  var commands, labels, voices, guff, i: paragraph_index0;
      w: string;
begin isControlParagraph:=true; commands:=0; labels:=0; voices:=0; guff:=0;
  for i:=1 to para_len do
    if not (startsWith(P[i],'%')) then
    begin w:=nextWord(P[i],' ',':');
      if not endsWith(w,':') then inc(guff)
      else if (length(w)<3) or (findVoice(w)>0) then inc(voices)
      else if isCommand(w) then inc(commands)
      else inc(labels)
    end;
  if (voices+guff>commands) then isControlParagraph:=false
end;

procedure topOfPMXfile;
begin
  putLine('---');
  putLine('\def\mtxversion{'+version+'}');
  putline('\def\mtxdate{'+version_date+'}');
  putline('\input mtx');
end;

begin   { ---- Main program ------------------------ }
  this_version := version;  this_version_date := version_date;
  writeln ('==> This is M-Tx ' + version + ' (Music from TeXt) ' +
    version_date );

  mtxLevel(version);  OpenFiles;  no_commands_yet:=true;  preambleDefaults;
  no_report_errors:=false;
  topOfPMXfile;
  repeat readParagraph(P,orig_line_no,para_len);
    control_paragraph:=isControlParagraph(P,para_len);
    if control_paragraph then
    begin  augmentPreamble(no_report_errors);
      no_report_errors:=true;
      if para_len=0 then error('No music paragraphs!',not print)
    end  
  until not control_paragraph;
  doPreamble; doMusic;  
  if not pmx_preamble_done then error('No music paragraphs!',not print);
  putline('% Coded by M-Tx');
  CloseFiles;
  writeln ('PrePMX done.  Now run PMX.');  halt(0);
end.

