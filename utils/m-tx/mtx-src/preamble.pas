unit preamble;
{$X+}
{ Interpret preamble paragraph, compose PMX preamble }

interface  uses globals;

function thisCase: boolean;
procedure preambleDefaults;
procedure interpretCommands;
procedure doPreamble;
procedure doPMXpreamble;
procedure respace;
procedure restyle;
function startString(voice: voice_index0): string;
procedure augmentPreamble(control_para: boolean);
function omitLine(line: paragraph_index): boolean;
procedure nonMusic;
procedure setOnly(line: string);
function isCommand(command: string): boolean;

const known = true;

implementation  uses control, mtxline, strings, files, status, utility;

const blank = ' ';
      colon = ':';
      semicolon = ';';
      comma = ',';
      known_styles: integer = 12;
      max_styles = 24;
      warn_redefine: boolean = false;

type command_type =
     ( none, title, composer, pmx, tex, options, msize, bars, shortnote,
       style, sharps, flats, meter, space, pages, systems, width, height,
       enable, disable, range, name, indent,
       poet, part, only, octave, start );

   line_type = ( unknown, colon_line, command_line, comment_line,
       plain_line );

   style_index = 1..max_styles;
   style_index0 = 0..max_styles;

const c1 = title; cn = start;
      commands: array[command_type] of string[16] =
  ( 'NONE', 'TITLE', 'COMPOSER', 'PMX', 'TEX', 'OPTIONS',
    'SIZE', 'BARS/LINE', 'SHORT',
    'STYLE', 'SHARPS', 'FLATS', 'METER', 'SPACE', 'PAGES', 'SYSTEMS', 'WIDTH',
    'HEIGHT', 'ENABLE', 'DISABLE', 'RANGE',
    'NAME', 'INDENT', 'POET', 'PART', 'ONLY', 'OCTAVE', 'START');
     cline: array[command_type] of string =
   ( '', '', '', '', '', '', '', '', '1/4', (* short *)
     '', '0', '',  'C', '', '1', '1', (* systems *)
     '190mm', '260mm', '', '', '', '', '', '', '', '', '', '' );
      redefined: array[command_type] of boolean =
    ( false, false, false, false, false, false, false, false, false, false,
      false, false, false, false, false, false, false, false, false,
      false, false, false, false, false, false, false, false, false );

(** Known styles *)
      known_style: array[style_index] of string = (
'SATB:    Voices S,A T,B; Choral; Clefs G F',
'SATB4:   Voices S A T B; Choral; Clefs G G G8 F',
'SINGER:  Voices Voice; Vocal; Clefs G',
'PIANO:   Voices RH LH; Continuo; Clefs G F',
'ORGAN:   Voices RH LH Ped; Continuo; Clefs G F F',
'SOLO:    Voices V; Clefs G',
'DUET:    Voices V1 Vc; Clefs G F',
'TRIO:    Voices V1 Va Vc; Clefs G C F',
'QUARTET: Voices V1 V2 Va Vc; Clefs G G C F',
'QUINTET: Voices V1 V2 Va Vc1 Vc2; Clefs G G C F F',
'SEXTET:  Voices V1 V2 Va1 Va2 Vc1 Vc2; Clefs G G C C F F',
'SEPTET:  Voices V1 V2 Va1 Va2 Vc1 Vc2 Cb; Clefs G G C C F F F',
'', '', '', '', '', '', '', '', '', '', '', '');

var old_known_styles: style_index;
    style_used: array[style_index] of boolean;
    omit_line: array[paragraph_index] of boolean;
    orig_style_line: array[style_index] of style_index0;
    orig_range_line: integer;

var nclefs, n_pages, n_systems, n_sharps, ngroups: integer;
    part_line, title_line, composer_line, pmx_line, options_line,
      start_line, voices, clefs: string;
    group_start, group_stop: array[1..maxgroups] of integer;
    instr_name: array[stave_index] of string[40];
    style_supplied: boolean;

{ ------------------ Styles ------------------ }

function voiceCount(s: string): voice_index0;
  var i, l: integer;
  begin  l:=length(s);  for i:=1 to l do if s[i]=comma then s[i]:=blank;
    voiceCount:=wordCount(s);
  end;

function findStyle(s: string): style_index0;
  var i: style_index0;
begin
  i:=0; s:=s+colon; findStyle:=0;
  while i<known_styles do
  begin inc(i);
    if startsWithIgnoreCase(known_style[i],s) then
    begin findStyle:=i; exit; end;
  end;
end;

procedure addStyle (S: string);
  var sn: style_index0;
begin
  sn:=findStyle(NextWord(S,colon,dummy));
  if sn>0 then known_style[sn] := S
  else if known_styles < max_styles then
  begin  inc(known_styles);
    known_style[known_styles] := S;
  end
  else  error('Can''t add another style - table full',print);
end;

procedure readStyles;
  var eofstyle: boolean;
      S: string;
      l: style_index0;
begin  if styleFileFound then eofstyle:=true else eofstyle := eof(stylefile);
  l:=0;
  while not eofstyle do
  begin  readln(stylefile,S);  if S<>'' then
    begin addStyle(S); inc(l); orig_style_line[known_styles]:=l; end;
    eofstyle := eof(stylefile);
  end;
end;

procedure applyStyle(s, stylename: string;
                     first_inst, first_stave: stave_index);
  var clef, subline, subcommand: string;
      i, last_inst, last_stave: stave_index0;
      continuo, toosoon, vocal: boolean;
  begin last_inst:=first_inst-1;
    toosoon := false;  continuo:=false;  vocal := false;
    subline:=GetNextWord(s,blank,colon);
    while s<>'' do
    begin subline:=GetNextWord(s,semicolon,dummy);
      i:=curtail(subline,semicolon);
      subcommand:=GetNextWord(subline,blank,dummy);
      if equalsIgnoreCase(subcommand,'VOICES') then
        begin voices:=voices+' '+subline;
          last_stave := first_stave + wordCount(subline) - 1;
          last_inst := first_inst + voiceCount(subline) - 1;
        end
      else if equalsIgnoreCase(subcommand,'CLEFS')
        then
        begin clef:=subline; clefs:=clefs+' '+clef; end
      else if equalsIgnoreCase(subcommand,'VOCAL') then
        if last_inst<first_inst then toosoon:=true
        else begin  some_vocal := true;  vocal := true;
          for i:=first_inst to last_inst do setVocal(i,true);
        end
      else if equalsIgnoreCase(subcommand,'CHORAL') or
              equalsIgnoreCase(subcommand,'GROUP') then
        if last_inst<first_inst then toosoon:=true
        else begin
          if equalsIgnoreCase(subcommand,'CHORAL') then
          begin some_vocal := true;  vocal := true;
            for i:=first_inst to last_inst do setVocal(i,true);
          end;
          if ngroups=maxgroups then error('Too many groups',print)
          else begin inc(ngroups);
            group_start[ngroups] := first_stave;
            group_stop[ngroups] := last_stave;
          end
        end
      else if equalsIgnoreCase(subcommand,'CONTINUO') then continuo := true
      else error('Subcommand ' + subcommand + ' in STYLE unknown',print);
      if toosoon then
      error('You must first give VOICES before specifying ' + subcommand,
        print);
    end;
    if vocal and continuo then error(
      'A continuo instrument may not be vocal',print);
    if wordCount(clef)<>last_stave-first_stave+1 then error(
      'Number of clefs does not match number of voices',print);
    if (first_stave=last_stave) or continuo
      then instr_name[first_stave]:=stylename
    else for i:=first_stave to last_stave do  instr_name[i]:='';
    if continuo then
    begin inc(ninstr); stave[ninstr] := first_stave;
      for i:=first_stave to last_stave do instr[i]:=ninstr;
    end
    else for i:=first_stave to last_stave do
    begin inc(ninstr); stave[ninstr] := i; instr[i] := ninstr;
    end;
  end;

procedure applyStyles;
  var n1, n2, sn: integer;
      s: string;
  begin  voices:=''; clefs:=''; ninstr:=0;
    while cline[style]<>'' do
    begin  n1:=voiceCount(voices)+1;
      n2:=wordCount(voices)+1;
      s:=GetNextWord(cline[style],blank,comma);
      curtail(s,comma);
      sn:=findStyle(s); if sn=0 then error ('Style ' + s +' unknown',print);
      line_no := orig_style_line[sn]; applyStyle(known_style[sn],s,n1,n2);
      style_used[sn] := true;
    end;
  end;

{ ------------------------------------------------------------- }

procedure wipeCommands;
  var c: command_type;
begin  for c:=c1 to cn do cline[c]:='';
end;

function omitLine(line: paragraph_index): boolean;
begin omitLine := (line>0) and omit_line[line] 
end;

procedure setName;
  var i: integer;
begin if not redefined[name] then exit;
  setFeature('instrumentNames',true);
  for i:=1 to ninstr do
  instr_name[i] := getNextWord(cline[name],blank,dummy);
end;

procedure setIndent;
begin if redefined[indent] then fracindent := cline[indent];
end;

procedure setInitOctave;
begin if redefined[octave] then initOctaves(cline[octave]);
end;

procedure setVoices(var line: string);
  var k: integer;
      s, w: string;
  procedure checkLabel(w: string);
    var j: voice_index;
  begin
    for j:=1 to nvoices do if w=voice_label[j] then
    begin warning('Voice label '+w+' not unique',print); exit;
    end;
    if length(w)>2 then exit;
    if pos1(w[1],'CLU')>0 then
      if length(w)>1 then if pos1(w[2],'123456789')=0 then exit
        else
      else
    else if pos1(w[1],'123456789')=0 then exit;
    error('Voice label '+w+' conflicts with reserved label',print);
  end;
begin  nvoices:=0; nstaves:=0;
  repeat s:=GetNextWord(line,blank,dummy);
    if length(s)>0 then
    begin inc(nstaves); k:=0;
      first_on_stave[nstaves] := nvoices + 1;
      repeat
        w:=GetNextWord(s,blank,comma); curtail(w, comma);
        if w<>'' then
        begin
          inc(k);  if k<=2 then
          begin  inc(nvoices); checkLabel(w);
            voice_label[nvoices]:=w;
            if instr_name[nstaves]='' then instr_name[nstaves]:=w;
            setStavePos(nvoices,nstaves,k);
          end
        end;
      until w='';
      if k>2 then error('More than two voices per stave: ' + s,print);
      if k=2 then instr_name[nstaves]:='\mtxTwoInstruments{'
        + voice_label[nvoices-1] + '}{'+ voice_label[nvoices] + '}';
      number_on_stave[nstaves] := k;
    end
  until length(line)=0;
  for k:=1 to nvoices do selected[k]:=true;
end;

procedure setClefs(line: string);
  var s: string;
begin
  nclefs:=0;
  repeat
    s:=getnextword(line,blank,dummy);
    if s<>'' then
    begin inc(nclefs);
      if length(s)=1 then clef[nclefs]:=s[1] else clef[nclefs]:=s[2];
    end;
  until s='';
end;

procedure setDimension(line: string; lno: command_type);
  var l, n, p: integer;
begin
  if line = '' then exit;
  l := length(line);
  n := 0;
  p := 0;
  repeat n := n+1;
    if line[n]='.' then p:=p+1;
  until (n>l) or not ((line[n]='.') or (line[n]>='0') and (line[n]<='9'));
  if (n=p) or (p>1) or 
      not ((line[n]='i') or (line[n]='m') or (line[n]='p')) then
    error('Dimension must be a number followed by in, mm or pt',print);
  cline[lno] := 'w' + substr(line,1,n);
end;

procedure setSize(line: string);
  var i: stave_index0;
      word: string;
  begin  i:=0;
    while i<ninstr do
    begin  word:=GetNextWord(line,blank,dummy);
      if word='' then break;
      inc(i); getNum(word,musicsize);
      stave_size[i] := musicsize;
    end;
    if not (musicsize in [16,20]) then
    for i:=1 to ninstr do  
       if stave_size[i] = unspec then stave_size[i] := musicsize;
    if musicsize<16 then musicsize:=16
    else if musicsize>20 then musicsize:=20;
  end;

function findCommand(var command: string): command_type;
  var j: command_type;
begin curtail(command,':');
  if equalsIgnoreCase(command,'STYLE') then style_supplied := true;
  for j:=c1 to cn do  if equalsIgnoreCase(command,commands[j]) then
  begin  findCommand:=j; exit; end;
  findCommand:=none;
end;

function isCommand(command: string): boolean;
begin  isCommand:=findCommand(command)<>none  end;

function mustAppend(command: command_type): boolean;
begin mustAppend := command=tex end;

procedure doEnable(var line: string; choice: boolean);
  var word: string;
begin
  repeat word:=GetNextWord(line,blank,dummy);
    if word<>'' then if not setFeature(word,choice) then
      Error('No such feature: '+word,not print)
  until word=''
end;

procedure setRange(line: string);
  var v,p: integer;
      vl: string;
begin 
  line_no := orig_range_line;
  for v:=1 to nvoices do 
  begin vl := voice_label[v];
    p:=pos(vl+'=',line);
    if p>0 then
    begin
      if length(line)<p+6 then
        error('At least five characters must follow "'+vl+'="',
        print);
      defineRange(v,substr(line,p+1+length(vl),5));
    end
    else begin
      warning('No range defined for voice '+vl,print);
      defineRange(v,'');
    end
  end;
end;

{ TODO: This procedure should test for assertions in a comment
 or be removed }
function isAssertion(var line: string): boolean;
begin 
  isAssertion := false
end;  

function doCommand(line: string): line_type;
  var command: string;
    last_command: command_type;
    starts_with_note: boolean;
begin
  if (line[1]=comment) and not isAssertion(line) then
  begin doCommand:=comment_line; exit; end;
  starts_with_note := maybeMusicLine(line);
  command:=GetNextWord(line,blank,colon);
  if endsWith(command,colon) then
  begin last_command:=findCommand(command);
    doCommand:=command_line;
    if last_command = enable then doEnable(line,true)
    else if last_command = disable then doEnable(line,false)
    else if last_command = range then orig_range_line := line_no;
    if last_command<>none then
    begin 
      if mustAppend(last_command) and redefined[last_command] then
        begin
          if length(cline[last_command])+length(line)>254 then
          error('Total length of preamble command '+commands[last_command]+
            ' must not exceed 255',not print);
          cline[last_command]:=cline[last_command]+#10+line
        end
      else 
      begin cline[last_command]:=line;
        if warn_redefine and redefined[last_command] then
        warning('You have redefined preamble command '+command,print);
      end;
      if last_command=start then start_line:=line;
      redefined[last_command]:=true;
    end
    else begin doCommand:=colon_line;  addStyle(command+colon+' '+line);
      orig_style_line[known_styles] := line_no;
    end
  end
  else if starts_with_note then doCommand:=plain_line
  else doCommand:=unknown;
end;

procedure setOnly(line: string);
  var num, num1, num2, l: integer;
      s: string;
begin  if line='' then exit;
  if startsWithIgnoreCase(line,'only') then GetNextWord(line,colon,dummy);
  for l:=1 to lines_in_paragraph do omit_line[l]:=true;
  repeat  s:=GetNextWord(line,blank,comma); if s='' then exit;
    curtail(s, comma);
    if pos1('-',s)=0 then
    begin getNum(s,num);
      if (num>0) and (num<=lines_in_paragraph) then omit_line[num]:=false
        else warning('Invalid line number in Only: is skipped',print);
    end  else
    begin getTwoNums(s,num1,num2);
     if (num1>0) and (num2<=lines_in_paragraph) then
     for num:=num1 to num2 do omit_line[num]:=false
        else warning('Invalid line range in Only: is skipped',print);
   end;
  until false;
end;

procedure interpretCommands;
  var i, num, den, nbars: integer;
begin
  title_line := cline[title];
  part_line := cline[part];
  if (cline[poet]<>'') or (cline[composer]<>'') then
    composer_line:='\mtxComposerLine{'+cline[poet]+'}{'+cline[composer]+'}'
    else composer_line:='';
  pmx_line := cline[pmx];
  options_line := GetNextWord(cline[options],blank,dummy);
  for i:=1 to known_styles do style_used[i] := false;
  applyStyles; setVoices(voices);
  for i:=old_known_styles+1 to known_styles do
    if not style_used[i] then
    begin warning('The following style was supplied but not used',not print);
      writeln(known_style[i]);
    end;
  setClefs(clefs);
  if not redefined[meter] then warning(
    'You have not defined Meter, assuming "'+cline[meter]+'" ',not print);
  getMeter(cline[meter],meternum, meterdenom, pmnum, pmdenom);
  setDefaultDuration(meterdenom);
  if (meternum=0) and
    not (redefined[pages] or redefined[systems] or redefined[bars])
    then begin  cline[bars] := '1'; redefined[bars]:=true;  end;
  if redefined[pages] or redefined[systems] then
  begin  if redefined[bars]   then
    warning('BARS/LINE ignored since you specified PAGES or SYSTEMS',print);
    if redefined[systems] then getNum(cline[systems],n_systems)
      else warning('PAGES specified but not SYSTEMS',not print);
    if redefined[pages] then getNum(cline[pages],n_pages)
      else warning('SYSTEMS specified but not PAGES',not print);
  end
  else  if redefined[bars] then
  begin getNum(cline[bars],nbars);  if nbars>0 then
    begin n_pages:=0; n_systems:=nbars end;
  end;
  getNum(cline[sharps],n_sharps);
  setSpace(cline[space]);
  setSize(cline[msize]);
  getTwoNums(cline[shortnote],num, den);  if den=0 then den:=1;
    short_note := (num*64) div den;
  if cline[flats]<>'' then
  begin getNum(cline[flats],n_sharps); n_sharps:=-n_sharps; end;
  setName; setIndent; setInitOctave; setOnly(cline[only]);
  setRange(cline[range]);
  setDimension(cline[width],width);
  setDimension(cline[height],height);
  if options_line <>'' then begin
    warning('"Options" is cryptic and obsolescent.',  not print);
    writeln('  Use "Enable" and "Disable" instead.')
  end;
  for i:=1 to length(options_line) do processOption(options_line[i]);
end;

procedure preambleDefaults;
  var i: integer;
begin
  xmtrnum0:=0; fracindent:='0'; musicsize:=20; start_line:='';
  some_vocal:=false; ngroups:=0;
  style_supplied := false; 
  for i:=1 to maxvoices do setVocal(i,false);
  for i:=1 to maxstaves do stave_size[i]:=unspec;
  for i:=0 to maxstaves do nspace[i]:=unspec;
  { next line seems to be spurious.  0.63a RDT }
  { begin  nspace[i]:=unspec;  stave_size[i]:=unspec;  end; }  
  n_pages:=1; n_systems:=1;
  readStyles; old_known_styles := known_styles;
  for i:=1 to lines_in_paragraph do omit_line[i]:=false;
end;

procedure preambleGuess(maybe_voices: voice_index);
begin
  case maybe_voices of
  1: cline[style] := 'Solo';
  2: cline[style] := 'Duet';
  3: cline[style] := 'Trio';
  4: cline[style] := 'Quartet';
  5: cline[style] := 'Quintet';
  6: cline[style] := 'Sextet';
  7: cline[style] := 'Septet';
  else begin  error('I cannot guess a style',not print);  exit;  end;
  end;
  writeln('I guess this piece is a ',cline[style],
    ' for strings in C major.');
  writeln('  Why not provide a STYLE in the setup paragraph to make sure?');
end;

{ ------------------------------------------------------------------ }

procedure nonMusic;
  var i: paragraph_index;
begin  for i:=1 to para_len do doCommand(P[i]);
  setOnly(cline[only]); wipeCommands;
end;

function thisCase: boolean;
begin  thisCase:=true;
  if not startsWithIgnoreCase(P[1],'case:') then exit;
  thisCase:=(choice<>' ') and (pos1(choice,P[1])>0);  P[1]:='%';
end;

procedure augmentPreamble(control_para: boolean);
  var i: paragraph_index;
      l: line_type;
      s: array[line_type] of integer;
begin
  if not thisCase then exit;
  for l:=unknown to plain_line do s[l]:=0;
  for i:=1 to para_len do
  begin line_no:=orig_line_no[i];  l:=doCommand(P[i]);
    inc(s[l]);
    if (l=comment_line) and (P[i,2]=comment)
      then begin  predelete(P[i],2);  putLine(P[i]); end;
    if not control_para and (l=unknown) then
      error('Unidentifiable line',print);
  end;
  if not control_para and (s[command_line]>0) and (s[plain_line]>0)
    then error('Mixture of preamble commands and music',not print);
end;

procedure doPreamble;
  var i: paragraph_index;
      maybe_voices: voice_index0;
begin  maybe_voices:=0;
  if not style_supplied then
  begin  {augmentPreamble(not known);}
    if not style_supplied then warning('No STYLE supplied',not print);
    for i:=1 to para_len do if maybeMusicLine(P[i]) then
      inc(maybe_voices);
    if maybe_voices>0 then preambleGuess(maybe_voices)
    else error('No voices found',not print);
  end
end;

procedure respace;
  var i, j: stave_index;
  begin
     for i:=ninstr downto 2 do
     begin j:=ninstr-i+1;
	if nspace[j]<>unspec then TeXtype2('\mtxInterInstrument{'+toString(i-1)+
				       '}{'+toString(nspace[j])+'}');
     end;
     if nspace[ninstr]<>unspec then TeXtype2('\mtxStaffBottom{'+
					  toString(nspace[ninstr])+'}');
     must_respace:=false;
  end;

procedure restyle;
begin  
  must_restyle:=false;
end;

function clefno ( cl: char ): integer;
begin
  case cl of
'G','0','t','8': clefno:=0;
's','1': clefno:=1;
'm','2': clefno:=2;
'a','3': clefno:=3;
'n','4': clefno:=4;
'r','5': clefno:=5;
'F','b','6': clefno:=6;
'C': clefno:=3;
    else
    begin  warning('Unknown clef code "' + cl + '" - replaced by treble',print);
      clefno:=0;
    end;
  end
end;

procedure doTenorClefs;
  var i: voice_index;
      c: char;
begin  for i:=1 to nclefs do
  begin
    c:=clef[i];  if (c='8') or (c='t') then
    putLine('\\mtxTenorClef{' + toString(PMXinstr(i)) + '}\' );
  end;
end;

procedure insertTeX;
begin
  if redefined[tex] then TeXtype2(cline[tex]);
end;

procedure doPMXpreamble;
  const clefcode: string[8]='0123456';
  var i, j: integer;
      clefs: string;
 function pmxMeter: string;
    var denom, num: integer;
  begin  if meternum=0 then
    begin num := beatsPerLine; {* denom := 0; *}
      old_meter_word := meterChange(num,meterdenom,true);
    end
    else begin num := meternum;  {* denom := pmdenom; *} end;
  { CMO: unconditonally assign value of pmdenom to denom }
    denom := pmdenom;
    pmxMeter := toString(num) + ' ' + toString(PMXmeterdenom(meterdenom)) +
      ' ' + toString(pmnum) + ' ' + toString(denom);
  end;
  function sizecode(k: integer): string;
  begin sizecode:='\mtxNormalSize';
    case k of
13: if musicsize=20 then sizecode:='\mtxTinySize' else sizecode:='\mtxSmallSize';
16: if musicsize=20 then sizecode:='\mtxSmallSize';
20: if musicsize=16 then sizecode:='\mtxLargeSize';
24: if musicsize=20 then sizecode:='\mtxLargeSize' else sizecode:='\mtxHugeSize';
29: sizecode:='\mtxHugeSize';
    else error('Valid sizes are 13, 16, 20, 24, 29',print);
    end;
  end;
begin
  if composer_line <> '' then putline(composer_line);
  if title_line <> '' then putline('\mtxTitleLine{'+title_line+'}');
  putLine('---');
  if instrumentNames and not redefined[indent] then fracindent:='0.12';
  write(outfile, nstaves);
  write(outfile, ' ', -ninstr);
  stave[ninstr+1]:=nstaves+1;
  for j:=ninstr downto 1 do write(outfile, ' ', stave[j+1] - stave[j]);
  writeln(outfile, ' ', pmxMeter, ' ',xmtrnum0:8:5, ' ',n_sharps,
    ' ', n_pages, ' ',n_systems, ' ',musicsize, ' ',fracindent);
  for i:=1 to ninstr do  if not instrumentNames then putLine('')
    else putLine('\mtxInstrName{'+instr_name[ninstr+1-i]+'}');
  clefs:='';
  for i:=nclefs downto 1 do clefs:=clefs+clefcode[1+clefno(clef[i])];
  putLine(clefs);
  if texdir='' then texdir := './';
  putLine(texdir);

  pmx_preamble_done:=true; insertTeX; respace;

  for j:=1 to ngroups do
  writeln(outfile,'\\mtxGroup{'+toString(j)+'}{'
    +toString(ninstr+1-group_start[j])+'}{'
    +toString(ninstr+1-group_stop[j])+'}\');
  for j:=1 to ninstr do
    if (stave_size[j]<>unspec) then
    putLine(
      '\\mtxSetSize{'+toString(ninstr+1-j)+'}{'+sizecode(stave_size[j])+'}\');
  if part_line <> '' then
  begin putLine('Ti'); putLine(part_line); end;
  if composer_line <> '' then
  begin putLine('Tc'); putLine('\mtxPoetComposer'); end;
  if title_line <> '' then
  begin write(outfile,'Tt'); 
    if nspace[0] <> unspec then write(outfile,toString(nspace[0]));
    writeln(outfile);
    putLine('\mtxTitle'); 
  end;
  if pmx_line <> '' then putLine(pmx_line);
  doTenorClefs;
  if cline[width] <> '' then putLine(cline[width]);
  wipeCommands;
end;

function startString(voice: voice_index0): string;
  var s, w: string;
      j: voice_index;
begin  s:=start_line;
  for j:=1 to voice do w:=getNextWord(s,dummy,';');
  curtail(w,';');  if w<>'' then startString:=w+' ' else startString:=w;
end;

end.
