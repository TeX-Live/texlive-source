unit multfile;  (* DPL 2004-03-06 
(* Support for including files into an input stream.  
(* Intended for an application that does not require Pascal formatting,
(*   but reads complete lines one at a time.
(* You never actually work with any files, except by supplying the
(    filename when the file is opened.
(* At any stage, you can switch the input to a new file. 
(* When the new file is at EOF, and a "read" is issued, the file is
(*   closed and the previous one resumed transparently.    This inclusion 
(*   can be nested to any level, memory permitting.
(* --- Normal mode of operation, replacing the usual Pascal style ---
   Instead of:  assign(textfile,filename); reset(textfile)
          use:  pushFile(filename)
   When another file should be included before the current file is done,
          use:  pushFile(newfilename)
   Instead of:  readln(textfile,line)  
          use:  line:=readLine
   Instead of:  eof(textfilen)
           or:  eofAll;    {Are all files at EOF?}
(* --- Abnormal mode of operation ---
   To abort a file before EOF is reached:
          use:  popFile; 
   To abort all files:
          use:  closeAll;
   To test whether only the current file is at EOF:
          use:  eofCurrent;
(* Additional features:
   fileError: boolean function, was there an error during file open or read?
   currentFilename: string function, name of current file
   currentLineNo: integer function, number of line just read from current file
   isEmpty(var s: string): boolean function, is s empty?
   readData: string function, like readLine, but continue reading until 
     a non-blank line is found, return blank only at EOF
   skipBlanks: skip blank lines in input: next call to readLine will be
     non-blank unless EOF is encountered
   report(items): procedure to control which messages are printed, 
     "items" is the sum of the following options
     (constants with the appropriate values are defined in the interface)
     1: reportnewfile - file is opened
     2: reportoldfile - file is resumed
     4: reportclose - file is closed
     8: reporterror - a file error is encountered 
    16: reportrecursive - there is a recursive include
        The default value is items=27 (all the above except reportclose)
     At present you cannot turn reportrecursive off.
   *)
interface

  procedure pushFile(filename: string);
  procedure popFile;
  procedure closeAll;
  procedure report(items: integer);
  function currentFilename: string;
  function eofAll: boolean;
  function eofCurrent: boolean;
  function fileError: boolean;  
  function readLine: string;
  function readData: string;
  function isEmpty(var s: string): boolean;
  function currentLineNo: integer;
  procedure skipBlanks;
  const nextData: string = '';

const
  reportnewfile = 1;
  reportoldfile = 2;
  reportclose = 4;
  reporterror = 8;
  reportrecursive = 16;

implementation

  type 
  pfilenode = ^filenode;
  filenode = record
    name: string;
    actualfile: text;
    prev: pfilenode;
    lineno: integer;
  end;  

  const current: pfilenode = NIL;
    inputerror: boolean = false; 
    reportitem: integer = reportnewfile + reportoldfile 
     + reporterror + reportrecursive;

  procedure report(items: integer); begin reportitem := items end;

  function recursive (filename: string): boolean;
    var previous: pfilenode;
  begin if current=NIL then begin recursive:=false; exit; end;
    previous := current; recursive:=true;
    while previous <> NIL do
    begin
      if filename=previous^.name then exit;
      previous := previous^.prev;
    end;
    recursive:=false
  end;

  procedure pushFile(filename: string); 
    var newnode: pfilenode;
  begin  
    if recursive(filename) then
    begin writeln('===! Ignoring recursive include of file ',filename); exit; 
    end;
    new(newnode); newnode^.name := filename; newnode^.prev := current; 
    newnode^.lineno := 0;
{$I-}
    assign(newnode^.actualfile,filename);  reset(newnode^.actualfile);  
{$I+}
    inputerror := ioresult<>0; 
    if inputerror then dispose(newnode) else current := newnode;
    if not inputerror and ((reportitem and reportnewfile)>0) then writeln 
      ('==>> Input from file ',currentFilename);
    if inputerror and ((reportitem and reporterror)>0) then writeln
      ('==!! Could not open file ',filename);
  end;

  procedure popFile;
    var previous: pfilenode;
  begin  if current=NIL then exit;
    if (reportitem and reportclose)>0 then writeln
    ('==>> Closing file ',currentFilename,' at line number ', currentLineNo:1);
    close(current^.actualfile); previous := current^.prev; dispose(current);
    current := previous; 
    if (current<>NIL) and ((reportitem and reportoldfile)>0) then writeln
    ('==>> Resuming input from file ',currentFilename,' at line number ',
     currentLineNo:1);
  end;

  procedure closeAll;  begin  repeat popFile until current=NIL; end;

  function eofCurrent: boolean; 
  begin eofCurrent := eof(current^.actualfile);
  end;

  function readLine: string;
    var s: string;
  begin if nextData<>'' then 
    begin readLine:=nextData; nextData:=''; exit end; 
    if eofAll then begin readLine:=''; exit end; 
{$I-}
    readln(current^.actualfile,s); readLine:=s;
{$I+}
    inputerror := ioresult<>0;
    if not inputerror then inc(current^.lineno);
    if inputerror and ((reportitem and reporterror)>0) then writeln
      ('==!! Could not read from file ',currentFilename);
  end;

  function isEmpty(var s: string): boolean;
    var i: integer;
  begin if length(s)=0 then begin isEmpty:=true; exit; end;
    for i:=1 to length(s) do if s[i]<>' ' then 
      begin isEmpty:=false; exit; end;
    isEmpty:=true
  end;
 
  function readData: string;
    var s: string;
  begin if not isEmpty(nextData) then 
    begin readData:=nextData; nextData:=''; exit end;
    while not eofAll do
    begin  s:=readLine;
      if not isEmpty(s) then begin readData:=s; exit end;
    end;
    readData:='';
  end;

  procedure skipBlanks;
  begin while nextData='' do
    begin nextData:=readData; if eofAll then exit
    end
  end;

  function eofAll: boolean;
  begin eofAll := true;
    if current=NIL then exit else 
    if eofCurrent then begin popFile; eofAll:=eofAll; exit end;
    eofAll:=false
  end;

  function currentLineNo: integer;
  begin if current = NIL then currentLineNo := 0
    else currentLineNo := current^.lineno;
  end;

  function currentFilename: string;
  begin if current = NIL then currentFilename := 'No file open yet'
    else currentFilename := current^.name;
  end;

  function fileError: boolean;  begin fileError := inputerror; end;

end.
