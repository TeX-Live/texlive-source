unit strings;   { String handling primitives. }
  { These should be recoded in C instead of using the p2c code. }

interface

procedure scan1 (s: string; p: integer; var n: integer);
  { Read an integer at position p of s }
function startsWith (s1, s2: string): boolean;
function pos1(c: char; s: string): integer;
function posNot (c: char; var s: string): integer;
procedure insertChar (c: char; var s: string; p: integer);
function substr (var s: string; start, count: integer): string;
procedure getNum(line: string; var k: integer);
procedure getTwoNums(var line: string; var k1, k2: integer);
procedure toUpper(var s: string);
procedure delete1 (var s: string; p: integer);
procedure predelete (var s: string; l: integer);
procedure shorten (var s: string; new_length: integer);
function nextWordBound(s: string; trigger: char; p: integer): integer;
  { find end of first word starting with trigger after s[p] }

implementation

procedure scan1 (s: string; p: integer; var n: integer);
  begin  if length(s)<p then
    writeln('scan1: string too short');
      predelete(s,p-1); getNum(s,n);
  end;

function pos1(c: char; s: string): integer;
  begin  pos1:= pos(c, s);  end;

procedure delete1 (var s: string; p: integer);
  begin  if length(s)<p then
    writeln('delete1: string too short');
    delete(s,p,1);
  end;

procedure predelete (var s: string; l: integer);
  begin  if length(s)<l then
    writeln('predelete: string too short');
    delete(s,1,l);
  end;

procedure shorten (var s: string; new_length: integer);
  begin  if length(s)<new_length then
    writeln('shorten: string too short');
    s[0] := char(new_length);
  end;

function posNot(c: char; var s: string): integer;
  var i, l: integer;
begin  l:=length(s);
   for i:=1 to l do if s[i]<>c then
   begin posnot:=i; exit; end;
   posnot:=0;
end;

procedure strval(line: string; var num, p: integer);
  begin  val(line,num,p);  end;

procedure getNum(line: string; var k: integer);
  var code: integer;
begin  strval(line,k,code);
  if code>0 then strval(copy(line,1,code-1),k,code);
end;

procedure getTwoNums(var line: string; var k1, k2: integer);
  var num, code, p: integer;
begin
  strval(line,num,p); strval(copy(line,1,p-1),k1,code);
  strval(copy(line,p+1,255),k2,code);
  if code>0 then strval(copy(line,1,code-1),k2,code);
end;

procedure toUpper (var s: string);
  var i: integer;
begin
  for i:=1 to length(s) do s[i]:=upcase(s[i]);
end;

function startsWith (s1, s2: string): boolean;
  var i, l1, l2: integer;
  begin  l1:=length(s1);  l2:=length(s2);
    startsWith := false;  if l1<l2 then exit;
    for i:=1 to l2 do if s1[i]<>s2[i] then exit;
    startsWith := true;
  end;

procedure insertChar (c: char; var s: string; p: integer);
  begin  if length(s)<p-1 then
    writeln('insertchar: string too short');
    insert(c,s,p);
  end;

function substr (var s: string; start, count: integer): string;
  begin  if length(s)<start+count-1 then
    writeln('insertchar: string too short');
    substr := copy(s,start,count);
  end;

function nextWordBound(s: string; trigger: char; p: integer): integer;
 begin
  repeat p:=p+1 until (p>length(s)) or (s[p]=trigger);
  while (p<length(s)) and (s[p+1]<>' ') do p:=p+1;
  nextWordBound:=p;
end;

end.
