unit files;
{ Open and close files, interpret command line, do input, do most output}

interface  uses globals, multfile;

procedure readParagraph (var P: paragraph; var no: line_nos;
  var L: paragraph_index0);
function styleFileFound: boolean;
procedure processOption(j: char);
procedure OpenFiles;
procedure CloseFiles;
procedure putLine (line: string);
procedure put(line: string; putspace: boolean);
procedure tex3 (s: string);
function endOfInfile: boolean;

implementation uses control, strings, utility;

const  param_leader = '-';
       stylefilename: string = 'mtxstyle.txt';

var teststyle: integer;

function endOfInfile: boolean;
  begin endOfInfile:=eofAll; end;

procedure tex3 (s: string);
  begin
    if pmx_preamble_done then
      if s[1]='%' then putLine(s)
      else if first_paragraph then putLine('\'+s+'\')
      else putLine('\\'+s+'\')
    else putLine(s);
  end;

procedure putLine (line: string);
  begin  if outlen+length(line) > pmxlinelength-1 then writeln(outfile);
    writeln(outfile,line); outlen:=0;
  end;

procedure put(line: string; putspace: boolean);
  var l: integer;
  begin l:=length(line);
    if l>pmxlinelength then error('Item longer than PMX line length',print);
    if outlen+l > pmxlinelength-1 then
    begin putLine(''); put(line,false) end
    else begin if putspace and (outlen>0) and (line[1]<>' ') then
        line:=' '+line;
      write(outfile,line); inc(outlen,l);
    end
  end;

function styleFileFound: boolean;
begin styleFileFound := teststyle<>0; end;

procedure helpmessage;
begin 
  writeln('Usage: prepmx [-bcfnhimtuvwDH0123456789] MTXFILE [TEXDIR] [STYLEFILE]')
end;

procedure bighelpmessage;
begin helpmessage; writeln;
  writeln('MTXFILE: name of .mtx file without its extension');
  writeln('TEXDIR: directory where the TeX file made by PMX goes, default is ./');
  writeln('STYLEFILE: name of a file containing style definitions.  Default is');
  writeln('  mtxstyle.txt.  This feature is now deprecated; use Include: STYLEFILE');
  writeln('  in the preamble of the .mtx file instead.'); 
  writeln('Options:  (can also be entered separately: -b -c ...)');
  writeln('  -b: disable unbeamVocal');
  writeln('  -c: disable doChords');
  writeln('  -f: enable solfaNoteNames');
  writeln('  -h: display this message and quit');
  writeln('  -i: enable ignoreErrors');
  writeln('  -m: disable doLyrics');
  writeln('  -n: enable instrumentNames');
  writeln('  -t: disable doUptext');
  writeln('  -u: disable uptextOnRests');
  writeln('  -v: enable beVerbose');
  writeln('  -w: enable pedanticWarnings');
  writeln('  -D: enable debugMode');
  writeln('  -0123456789: select Case');
  writeln('  -H: print enabled status of all options');
  writeln('All the above, and some other, options can be enabled or disabled');
  writeln('  in the preamble.  What you do there overrides what you do here.')  
end;

procedure processOption(j: char);
  begin  case j of
  'b': setFeature('unbeamVocal',false);
  'c': setFeature('doChords',false);
  'f': setFeature('solfaNoteNames',true);
  'h': begin bighelpmessage; halt(255) end;
  'i': setFeature('ignoreErrors',true);
  'm': setFeature('doLyrics',false);
  'n': setFeature('instrumentNames',true);
  't': setFeature('doUptext',false);
  'u': setFeature('uptextOnRests',false);
  'v': setFeature('beVerbose',true);
  'w': setFeature('pedanticWarnings',true);
  'D': setFeature('debugMode',true);
  '0'..'9': choice:=j;
  'H': printFeatures(true);
  else write(j); error(': invalid option',not print);
  end;
end;

procedure OpenFiles;
var i, j, l, fileid, testin: integer;
    infilename, outfilename, basename, param, ext: string;
  procedure checkExistingFile;
    var tryfile: file;
  begin
{$I-}
    assign(tryfile,basename);  reset(tryfile);  testin := ioresult;
{$I+}
    if testin<>0 then exit else close(tryfile);
    writeln('There exists a file named ',basename,'.  I am treating this');
    error('  as a fatal error unless you specify -i',not print);
  end;
begin
  fileid:=0;  line_no:=0;  paragraph_no:=0;
  for i:=1 to ParamCount do
  begin param:=ParamStr(i);
    if param[1]=param_leader then
      for j:=2 to length(param) do processOption(param[j])
    else if fileid=0 then fileid:=i
    else if texdir='' then texdir:=param
    else stylefilename:=param;
  end;
  if fileid=0 then 
  begin helpmessage; writeln('Try "prepmx -h" for more information.'); halt(255) end
  else basename:=paramstr(fileid);
  l:=length(basename);
  if (l>4) and (basename[l-3]='.') then
  begin ext:=substr(basename,l-2,3); toUpper(ext); if ext='MTX' then
    begin warning('.mtx extension deleted from basename',not print);
      shorten(basename,l-4);
    end;
  end;
  if pos1('.',basename)>0 then checkExistingFile;
  infilename := basename+'.mtx';  outfilename := basename+'.pmx';
{$I-}
  pushFile(infilename); 
  assign(outfile,outfilename);  rewrite(outfile);
  assign(stylefile,stylefilename);  reset(stylefile);  teststyle := ioresult;
  if (teststyle<>0) and (stylefilename<>'mtxstyle.txt') then 
     writeln('Can''t read ',stylefilename);
{$I+}
  if fileError then fatalError('Input file '+infilename+' not found');
  outfile_open := true;  writeln('Writing to ',basename,'.pmx');
end;

procedure CloseFiles;
begin  close(outfile); closeAll; if teststyle=0 then close(stylefile);
end;

procedure readParagraph (var P: paragraph; var no: line_nos;
                         var L: paragraph_index0);
  var another: boolean;
      filename, buffer: string;
begin
  L:=0; buffer:=readData; line_no:=currentLineNo; 
  if isEmpty(buffer) then exit;
  if debugMode then writeln('>>>> ',buffer);
  inc(paragraph_no);  
{ Handle directives affecting the processing of the input file }
  repeat another:=false; 
    if startsWithIgnoreCase(buffer,'SUSPEND') then
      begin ignore_input:=true; another:=true; if beVerbose then
        writeln('-->> Suspending input file ',currentFilename, 
          ' at line ', line_no);
      end;
    if startsWithIgnoreCase(buffer,'RESUME') then
      begin ignore_input:=false; another:=true; if beVerbose then
        writeln('-->> Resuming input file ',currentFilename, 
          ' at line ', line_no);
    end;
    if startsWithIgnoreCase(buffer,'INCLUDE:') then 
    begin predelete(buffer,8); filename:=nextWord(buffer,' ',' ');
      pushfile(filename); another:=true; 
    end;
    if another then begin buffer:=readLine; line_no:=currentLineNo; end;
  until not another;
{ Normal paragraph input}
  repeat
    if L<lines_in_paragraph then 
      begin inc(L); P[L]:=buffer; buffer:=''; no[L]:=line_no; 
      end
    else warning('Paragraph too long: skipping line',not print);
    buffer:=readLine; line_no := currentLineNo;
    if debugMode then writeln(line_no,' >> ', buffer);
  until isEmpty(buffer);
  skipBlanks;  { needed to identify final paragraph }
end;

end.
