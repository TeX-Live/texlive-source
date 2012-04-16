unit status;
{ Keep track of duration, octave, slur and beam status. }

interface

procedure initStatus;
procedure saveStatus(voice: integer);

procedure resetDuration (voice: integer; dur: char);
function duration(voice: integer): char;

function slurLevel(voice: integer): integer;
function beamLevel(voice: integer): integer;
function noBeamMelisma(voice: integer): boolean;
function noSlurMelisma(voice, history: integer): boolean;
function afterSlur(voice: integer): integer;
procedure setUnbeamed(voice: integer);
procedure setUnslurred(voice: integer);
procedure beginBeam(voice: integer; var note: string);
procedure endBeam(voice: integer);
procedure beginSlur(voice: integer; var note: string);
procedure endSlur(voice: integer; var note: string);
procedure activateBeamsAndSlurs(voice: integer);

procedure setOctave(voice: integer);
procedure resetOctave(voice: integer);
function octave(voice: integer): char;
procedure newOctave(voice: integer; dir: char);
procedure initOctaves(octaves: string);

procedure renewPitch (voice: integer; var note: string);
function chordPitch(voice: integer): integer;
procedure renewChordPitch (voice: integer; note: string);
procedure rememberDurations;
procedure restoreDurations;
procedure chordTie(voice: integer; var lab: char);
type int5 = array[1..5] of integer;
procedure getChordTies(voice: integer; var pitches: int5; var labels: string);

implementation uses globals, strings, mtxline, control, utility, notes;

const
  init_oct: string = '';
  lowest_pitch = -9;
  highest_pitch = 61;

type
  line_status = record
    pitch, chord_pitch, octave_adjust, beam_level, slur_level, after_slur: integer;
    octave, lastnote, chord_lastnote, duration, slurID, tieID: char;
    beamnext, beamed, slurnext, slurred, no_beam_melisma: boolean;
    no_slur_melisma: array[1..12] of boolean; 
    chord_tie_pitch: int5;
    chord_tie_label: string[5];
  end;

var current: array[voice_index] of line_status;
    lastdur: array[voice_index] of char;

procedure chordTie(voice: integer; var lab: char);
  var n: integer;
begin with current[voice] do
  begin n:=length(chord_tie_label);
    if n=5 then error3(voice,'Only five slur ties allowed per voice');
    if n=0 then lab:='T' else lab:=chord_tie_label[n];
    inc(lab); chord_tie_label:=chord_tie_label+lab; 
    inc(n); chord_tie_pitch[n]:=chord_pitch
  end
end;
  
procedure getChordTies(voice: integer; var pitches: int5; var labels: string);
begin with current[voice] do
  begin pitches:=chord_tie_pitch; labels:=chord_tie_label; chord_tie_label:='' end
end;

procedure rememberDurations;
  var v: voice_index;
begin for v:=1 to nvoices do lastdur[v]:=duration(v) end;

procedure restoreDurations;
  var v: voice_index;
begin for v:=1 to nvoices do resetDuration(v,lastdur[v]) end;

function duration(voice: integer): char;
begin duration := current[voice].duration; end;

procedure resetDuration(voice: integer; dur: char);
begin if pos1(dur,durations)=0 then 
  begin write('Trying to set duration to ',dur,'; ');
    error3(voice,'M-Tx system error: resetDuration');
  end;
  current[voice].duration := dur
end;


procedure activateBeamsAndSlurs(voice: integer);
begin  with current[voice] do
  begin
    if beamnext then begin beamed:=true; beamnext:=false; end;
    if slurnext then begin slurred:=true; slurnext:=false; end;
    if slurred then inc(after_slur);
  end
end;

procedure saveStatus(voice: integer);
begin  with current[voice] do
  begin chord_pitch := pitch;  chord_lastnote := lastnote; end;
end;

function noBeamMelisma(voice: integer): boolean;
begin noBeamMelisma := current[voice].no_beam_melisma; end;

function afterSlur(voice: integer): integer;
begin with current[voice] do 
  begin afterSlur := after_slur; if (after_slur>0) and (slur_level<1) then
    error3(voice,'M-Tx system error: afterSlur and slur_level incompatible)')
  end
end;

function octave(voice: integer): char;
begin octave := current[voice].octave; end;

procedure resetOctave(voice: integer);
begin current[voice].octave := ' '; end;

procedure initOctaves(octaves: string);
  var i: integer;
begin init_oct:=octaves;
  i:=1;
  while i<=length(init_oct) do
    if init_oct[i]=' ' then delete1(init_oct,i) else inc(i);
end;

function initOctave(voice_stave: stave_index): char;
begin  
  if voice_stave>length(init_oct) then  
  if pos1(clef[voice_stave],'Gt08')>0
  then initOctave:='4' else initOctave:='3'
  else initOctave:=init_oct[voice_stave];
end;

procedure setOctave(voice: integer);
begin current[voice].octave:=initOctave(voiceStave(voice)); end;

procedure newOctave(voice: integer; dir: char);
begin with current[voice] do case dir of
  '+': inc(octave);
  '-': dec(octave);
  end;
end;

function newPitch (voice: integer; note: string; pitch: integer; 
   lastnote: char): integer;
  var interval, npitch: integer;
      oct: char;
begin {if debugMode then
  write('New pitch for note ',note,' relative to ',lastnote,
     ' at pitch ',pitch);}
  oct:=octaveCode(note); 
  if oct='=' then oct:=initOctave(voiceStave(voice));
  if (oct>='0') and (oct<='9') then
  begin pitch:=7*(ord(oct)-ord('0'))-3; lastnote:='f'; 
    removeOctaveCode(oct,note); oct:=octaveCode(note)
  end;
  interval := ord(note[1])-ord(lastnote);
  if interval>3 then dec(interval,7);
  if interval<-3 then inc(interval,7);
  npitch:=pitch+interval; {if debugMode then write(' was ',npitch);}
  while oct<>' ' do 
  begin if oct='+' then inc(npitch,7) else if oct='-' then dec(npitch,7);
    removeOctaveCode(oct,note); oct:=octaveCode(note)
  end;
  {if debugMode then writeln(' is ',npitch);}
  newPitch:=npitch
end;

procedure repitch(var note: string; diff: integer);
  procedure delins(var note: string; c1, c2: char; l: integer);
    var i, n: integer;
  begin  n:=length(note); i:=pos1(c1,note); if i=0 then i:=n+1;
    while (l>0) and (i<=n) do begin delete1(note,i); dec(n); dec(l); end;
    i:=pos1(c2,note);
    if i=0 then if length(note)<2 then error('M-Tx program error',print)
      else i:=3;
    while l>0 do begin insertchar(c2,note,i); dec(l); end;
  end;
  begin  diff:=diff div 7;
    if diff>0 then delins(note, '-','+',diff)
    else delins(note, '+','-',-diff);
  end;

procedure setUnbeamed(voice: integer);
begin  current[voice].beamed:=false  end;

procedure setUnslurred(voice: integer);
begin with current[voice] do
  begin slurred:=false; after_slur:=0; end;
end;

procedure beginBeam(voice: integer; var note: string);
begin  with current[voice] do
  begin  if beamed then 
      error3(voice, 'Starting a forced beam while another is open');
    if beam_level>0 then error3(voice,
    'Starting a forced beam while another is open (beamlevel>0)');
    inc(beam_level);
    beamnext := true;  no_beam_melisma:=startsWith(note,'[[');
    if no_beam_melisma then predelete(note,1);
  end;
end;

procedure endBeam(voice: integer);
begin with current[voice] do
  begin if beam_level<1 then error3(voice, 'Closing a beam that was never opened');
    dec(beam_level)
  end;
  setUnbeamed(voice)
end;

function slurLevel(voice: integer): integer;
begin slurLevel := current[voice].slur_level; end;

function beamLevel(voice: integer): integer;
begin beamLevel := current[voice].beam_level; end;

function noSlurMelisma(voice, history: integer): boolean;
begin with current[voice] do
  noSlurMelisma := no_slur_melisma[slur_level+history];
end;

function slurLabel(voice: integer; note: string): string;
  var sl: char;
begin if note='' then begin slurLabel:=''; exit end; 
  if length(note)<2 then begin slurLabel:=' '; exit end;
  if (note[2]>='0') and (note[2]<='Z') then sl:=note[2] else sl:=' ';
  if (sl>='I') and (sl<='T') then 
    warning3(voice,'Slur label in the range I..T may cause conflict');
  slurLabel:=sl
end;

procedure labelSlur(voice: integer; var note: string);
  var sl: char;
begin if note='' then exit;
  with current[voice] do
  begin
    if note[1]=')' then inc(slurID,2) else if note[1]='}' then inc(tieID,2);
    if (note[1]='(') or (note[1]=')') then sl:=slurID else sl:=tieID;
    insertchar(sl,note,2);
    if note[1]='(' then dec(slurID,2) else if note[1]='{' then dec(tieID,2);
    if slurID<'I' then warning3(voice,'Too many nested slurs may cause conflict');
    if tieID<'I' then warning3(voice,'Too many nested ties may cause conflict')
  end
end;

procedure beginSlur(voice: integer; var note: string);
  var posblind: integer;
begin  
  with current[voice] do
  begin 
    inc(slur_level); if slur_level>12 then Error3(voice,'Too many open slurs');
    no_slur_melisma[slur_level] := startsWith(note,'((') or startsWith(note,'{{');
    if no_slur_melisma[slur_level] then predelete(note,1);
    if slurLabel(voice,note)='0' then delete1(note,2) else
    if slurLabel(voice,note)=' ' then labelSlur(voice,note);
    posblind:=pos1('~',note); if posblind>0 then
      if hideBlindSlurs then note:='' else delete1(note,posblind);
    slurnext := true; 
  end;
end;

procedure endSlur(voice: integer; var note: string);
  var poscontinue, posblind: integer;
      contslur: string;
begin  with current[voice] do
  begin contslur:='';
    if slur_level<1 then Error3(voice,'Ending a slur that was never started');
    if note[1]=')' then poscontinue:=pos1('(',note)
    else if note[1]='}' then poscontinue:=pos1('{',note);    
    if poscontinue=0 then dec(slur_level) else 
    begin dec(poscontinue); contslur:=note; predelete(contslur,poscontinue);
       shorten(note,poscontinue); 
    end;
    if slur_level=0 then setUnslurred(voice);
    if slurLabel(voice,note)='0' then delete1(note,2) else
    if slurLabel(voice,note)=' ' then labelSlur(voice,note);
    if slurLabel(voice,contslur)='0' then delete1(contslur,2) else
    if slurLabel(voice,contslur)=' ' then labelSlur(voice,contslur);
    if poscontinue>0 then 
    begin if note[1]='}' then note:=note+'t'; note[1]:='s';
      if contslur[1]='{' then contslur:=contslur+'t'; contslur[1]:='s';
    end;
    posblind:=pos1('~',note); if posblind>0 then
      if hideBlindSlurs then note:='' else delete1(note,posblind);
    if (note<>'') and (contslur<>'') then note := note + ' ' + contslur; 
  end;
end;

procedure renewPitch (voice: integer; var note: string);
  var pstat: integer;
begin with current[voice] do
  begin  pstat:=newPitch (voice, note, chord_pitch, chord_lastnote);
    if debugMode then write('Current pitch in voice ',voice,' is ',pitch,
      ', last note was ',lastnote,', this note is ',note);
    pitch := newPitch (voice, note, pitch, lastnote);
    if pitch<>pstat then repitch(note,pitch-pstat);
    if (pitch<lowest_pitch) and checkPitch then 
    begin write('Pitch of note ',note,' following ',lastnote,' reported as ',pitch);
      error3(voice,'Pitch too low')
    end;
    if (pitch>highest_pitch) and checkPitch then 
    begin write('Pitch of note ',note,' following ',lastnote,' reported as ',pitch);
      error3(voice,'Pitch too high')
    end;
    lastnote:=note[1];
    if debugMode then writeln(', repitched to ',pitch);
    if debugMode and (pitch<>pstat) then
      writeln('Pitch from melodic line = ',pitch,
      '   from last chordal note = ', pstat);
  end;
end;

function chordPitch(voice: integer): integer; 
begin chordPitch:=current[voice].chord_pitch end;

procedure renewChordPitch (voice: integer; note: string);
begin  with current[voice] do
  begin  chord_pitch:=newPitch(voice,note,chord_pitch,chord_lastnote);
    if chord_pitch<lowest_pitch then error3(voice,'Pitch in chord too low');
    if chord_pitch>highest_pitch then error3(voice,'Pitch in chord too high');
    chord_lastnote:=note[1];
  end
end;

procedure initStatus;
  var voice: integer;
begin  for voice:=1 to nvoices do with current[voice] do
  begin duration:=default_duration;
    octave_adjust:=0; slur_level:=0; beam_level:=0;
    beamed:=false;  beamnext:=false;  
    slurred:=false;  slurnext:=false;  after_slur:=0;
    octave:=initOctave(voiceStave(voice)); slurID:='S'; tieID:='T';
    lastnote:='f'; pitch:=7*(ord(octave)-ord('0'))-3;
    chord_tie_label:='';
    saveStatus(voice);
  end;
end;

end.
