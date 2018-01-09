unit analyze;

{ Decide which lines of paragraphs go where; test total duration of lines. }

interface  uses control, globals;

procedure testParagraph;
procedure describeParagraph;
procedure paragraphSetup (var voice: voice_index0);
procedure includeStartString;

implementation  uses mtx, strings, lyrics, mtxline, uptext,
  preamble, utility;

procedure includeStartString;
  var voice: voice_index;
      mus: paragraph_index0;
begin for voice:=1 to nvoices do
  begin mus:=musicLineNo(voice);  if mus>0 then
    P[mus]:=startString(voice)+P[mus];
  end;
end;

function describe(nbar,extra: integer): string;
begin  describe:=toString(nbar)+' bar'+plural(nbar)+' + '+
        toString(extra)+'/64 notes';
end;

procedure testParagraph;
  var voice, leader, nv: voice_index0;
      mus: paragraph_index0;
      extra, l, nbar: integer;
begin
  nbars:=0; pickup:=0; nleft:=0; if top>bottom then exit;
  pickup:=0;  nv:=0;  leader:=0;  multi_bar_rest := '';
  for voice:=top to bottom do
  begin mus:=musicLineNo(voice);
    if mus>0 then   {* -------------- Voice is present  ---- }
    begin  inc(nv); line_no:=orig_line_no[mus];
      scanMusic(voice,l);
      if (multi_bar_rest<>'') and (nv>1) then error(
        'Multi-bar rest allows only one voice',print);
      if not pmx_preamble_done then
      if voice=top then pickup:=l
        else if pickup<>l then
        error3(voice,'The same pickup must appear in all voices');
      nbar := numberOfBars(voice);  extra := extraLength(voice);
      if (multi_bar_rest<>'') and ((nbar>0) or (extra>0)) then error3(voice,
        'Multi-bar rest allows no other rests or notes');
      if (nbar>nbars) or (nbar=nbars) and (extra>nleft) then
      begin nbars:=nbar; nleft:=extra; leader:=voice; end;
      if not final_paragraph and (meternum>0) and (extra>0) then
      begin  writeln('Line has ', describe(nbar,extra));
        error('   Line does not end at complete bar',print);
      end;
      if pmx_preamble_done and (l>0) and (meternum>0) then
begin
      writeln('l=',l,' meternum=',meternum);
      error3(voice,'Short bar with no meter change');
end
      end
  end;
  if not pmx_preamble_done then
  begin
    xmtrnum0 := pickup/one_beat;    { Don't want an integer result }
    if beVerbose then writeln ('Pickup = ', pickup, '/64');
  end;
  if leader>0 then
  for voice:=top to bottom do if musicLineNo(voice)>0 then
    if voice<>leader then
  begin  mus:=musicLineNo(voice);
    line_no:=orig_line_no[mus];
    if (numberOfBars(voice)<>numberOfBars(leader))
      or (extraLength(voice)<>extraLength(leader)) then
      begin
        writeln('Following line has ',
          describe(numberOfBars(voice), extraLength(voice)));
        writeln(musicLine(voice));
        writeln('Longest line has ',
          describe(numberOfBars(leader), extraLength(leader)));
        writeln(musicLine(leader));
        error('Line duration anomaly',print);
      end;
  end;
end;

procedure describeParagraph;
  var voice: voice_index0;
  begin  writeln('---- Paragraph ',paragraph_no,
      ' starting at line ', orig_line_no[1], ' bar ', bar_no);
    for voice:=top to bottom do describeVoice(voice,lyricsReport(voice));
  end;

procedure paragraphSetup (var voice: voice_index0);
  var l: integer;
      k: voice_index0;
      P_keep, w: string;
      is_labelled: boolean;

  procedure maybeUptext(i: integer);
  begin if not doUptext then exit;
    if (length(w)=1) then if (voice=nvoices) then
      warning('Uptext line below bottom voice should be labelled',print);
    if length(w)=1 then  {*  Standard chord line ------ }
    begin  k:=voice+1; if k>nvoices then dec(k);
      setUptextLineNo(k,i);
    end
    else  {*  Labelled chord line  ---- }
    begin  predelete(w,1); k:=findVoice(w);
      if k=0 then  error('Uptext line belongs to unknown voice',print)
      else  setUptextLineNo(k,i);
    end
  end;

  procedure maybeChords(i: integer);
  begin if not doChords then exit;
    if (length(w)=1) and (voice=0) and pedanticWarnings then
      warning('Chord line above top voice should be labelled',print);
    if length(w)=1 then  {*  Standard chord line ------ }
    begin  k:=voice; if k=0 then k:=1;
      setChordLineNo(k,i);
    end
    else  {*  Labelled chord line  ---- }
    begin  predelete(w,1); k:=findVoice(w);
      if k=0 then  error('Chord line belongs to unknown voice',print)
      else setChordLineNo(k,i);
    end
  end;

  procedure analyzeParagraph;
    var i: paragraph_index;
  begin  voice:=0; bottom:=0; top:=nvoices+1;
    clearLabels; clearTags; clearUptext;
    for i:=1 to para_len do  { ----- Paragraph analysis main loop ----- }
    if (P[i]<>'') and (P[i,1]<>comment) then
    begin         
      w:=NextWord(P[i],blank,colon);  line_no := orig_line_no[i];
      l:=length(w);
      is_labelled := (w[l]=colon) and (w[l-1]<>barsym);
      if is_labelled then
      begin  P_keep := P[i]; predelete(P[i],l); shorten(w,l-1); 
        k:=findVoice(w); { First look for a voice label }
        if k>0 then
        begin voice:=k; setMusicLineNo(voice,i);
        end
        else if w[1]='L' then maybeLyrics(voice,i,w)
        else if w[1]='C' then maybeChords(i)
        else if w[1]='U' then maybeUptext(i)
        else begin  {* ------------ Maybe Space command ------------ }
          if startsWithIgnoreCase(w,'SPACE') then
          begin  setSpace(P[i]);  must_respace:=true;
          end
          else {* ------------ Maybe Voices command ------------ }
          if startsWithIgnoreCase(w,'VOICES') then
          begin  selectVoices(P[i]);  must_restyle:=true;
          end
          else begin  {* Could be sticky attribute *}
            P[i] := P_keep;  is_labelled := false;
            if not isNoteOrRest(w) then error('Unknown line label',print);
          end
        end
      end;
      if not is_labelled then
      begin  inc(voice); setMusicLineNo(voice,i);
      end;
      if voice>bottom then bottom:=voice;
      if (voice>0) and (voice<top) then top:=voice;
    end;
  end;

  procedure obliterate;
    var i: paragraph_index;
        new_only: string;
  begin  new_only:='';
    for i:=1 to para_len do if startsWithIgnoreCase(P[i],'only:')
      then begin new_only:=P[i]; P[i]:='%'; end;
    if new_only<>'' then setOnly(new_only)
    else for i:=1 to para_len do if omitLine(i) then P[i]:='%';
  end;

begin  obliterate;  analyzeParagraph;  reviseLyrics;
end;

end.
