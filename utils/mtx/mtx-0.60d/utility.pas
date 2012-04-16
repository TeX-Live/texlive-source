unit utility;  { DPL 2004-03-22 }

{ Utilities, mainly aids to parsing }

interface

function equalsIgnoreCase(s1, s2: string): boolean;
function startsWithIgnoreCase(s1, s2: string): boolean;
function endsWith(s1, s2: string): boolean;
function startsWithBracedWord (P: string): boolean;
function GetNextWord (var s: string; Delim, Term: char): string;
function NextWord (s: string; Delim, Term: char): string;
function wordCount (s: string): integer;
function plural (n: integer): string;
function curtail (var s: string; c: char): integer;
  { Remove last character if it equals c and return its position;
    otherwise return 0 }
function toString(n: integer): string;
procedure trim(var s: string);
function digit(c: char): integer;
function match(source, pattern: string): boolean;
function translate(source, pattern, target: string): string;
procedure grep(var source, pattern, target: string);
  { See Implementation for what this currently does. }

implementation uses strings;

const blank = ' '; dummy = #0;

function wordCount (s: string): integer;
  var i, n: integer;
  begin  if length(s)=0 then
    begin wordCount:=0; exit; end;
    if s[1]=blank then n:=0 else n:=1;
    for i:=1 to length(s)-1 do
    if (s[i]=blank) and (s[i+1]<>blank) then inc(n);
    wordCount:=n;
  end;

function GetNextWord (var s: string; Delim, Term: char): string;
{ A delimiter is a character that separates words, but forms no part
   of them.  A terminator is a character that appears at the end of
   a word. }
  var n, start, last: integer;
begin  last:=length(s);  n:=1;
  while (n<=last) and (s[n] = Delim) do inc(n);
  start:=n;
  while (n<=last) and not (s[n] in [Delim,Term]) do inc(n);
  if (n<=last) and (s[n] = Term) then inc(n);
  GetNextWord:=substr(s,start,n-start);
  while (n<=last) and (s[n] = Delim) do inc(n);
  predelete(s,n-1);
end;

function NextWord (s: string; Delim, Term: char): string;
  begin NextWord:=GetNextWord(s,Delim,Term); end;

function plural (n: integer): string;
  begin  if n=1 then plural:='' else plural:='s'; end;

function curtail (var s: string; c: char): integer;
  var l: integer;
  begin l:=length(s); curtail:=0;
    if s[l]=c then begin shorten(s,l-1); curtail:=l; end;
  end;

function toString(n: integer): string;
  var s: string;
begin  str(n,s); toString:=s; end;

function digit(c: char): integer;
  begin digit:=ord(c)-ord('0'); end;

function equalsIgnoreCase(s1, s2: string): boolean;
begin  toUpper(s1); toUpper(s2); equalsIgnoreCase:=s1=s2;
end;

function startsWithIgnoreCase(s1, s2: string): boolean;
begin  toUpper(s1); toUpper(s2); startsWithIgnoreCase:=startsWith(s1,s2);
end;

function startsWithBracedWord (P: string): boolean;
  var w: string;
begin w := getNextWord(P,blank,dummy);
  startsWithBracedWord := (w[1]='{') and (w[length(w)]='}');
end;

procedure trim(var s: string);
  var k: integer;
  begin  k:=posnot(blank,s);
    if k>1 then predelete(s,k-1) else if k=0 then s:='';
  end;

function endsWith(s1, s2: string): boolean;
  var l1, l2: integer;
begin  l1:=length(s1); l2:=length(s2);
  if l1<l2 then begin endsWith:=false; exit end;
  predelete(s1,l1-l2); endsWith:=s1=s2
end;

{--- Match/Replace package --- }

{ Search and replace.  Stops when pattern no longer matches source.
          Pattern wildcards:
  ?   Any single character
  *   Any string
  #   An unsigned integer
  ##  A signed integer
  ### A signed number maybe with a decimal part
          Pattern metacharacters:
  \x  where x is any character, stands for that character
          Target wildcards
  \0 to \9  Value of corresponding source wildcard
          Target metacharacters
  \   When not followed by 0..9 or \, stands for itself
  \\  Backslash
}
procedure grep(var source, pattern, target: string);
  var p1, p2: array[0..9] of integer;
      i, j, p, s, t, index, reg: integer;
      product: string;
      trigger, matching: boolean;
  procedure remember(s1, s2: integer);
  begin if index>9 then halt(9999);
    p1[index] := s1; p2[index] := s2; s := s2+1; inc(index)
  end;
  procedure matchnum;
    var allowsign, allowpoint, quit: boolean;
        s0: integer;
  begin allowsign := false; allowpoint := false; matching := false; s0 := s;
    if p<length(pattern) then if pattern[p+1]='#' then
    begin inc(p); allowsign := true;
      if p<length(pattern) then if pattern[p+1]='#' then
      begin inc(p); allowpoint := true end
    end;
    if allowsign and ((source[s]='-') or (source[s]='+')) then
    begin inc(s); if s>length(source) then exit end;
    quit := false;
    while (not quit) and (s<=length(source)) do
    begin if source[s]='.' then if not allowpoint then quit := true else
      begin inc(s); allowpoint := false end;
      if (source[i]>='0') and (source[i]<='9') then
      begin inc(s); matching := true end
      else quit := true
    end;
    if matching then remember(s0,s-1)
  end;
  procedure matchmeta;
  begin if p<length(pattern) then inc(p);
    if source[s]=pattern[p] then begin inc(s); inc(p) end
    else matching := false
  end;
  procedure subgrep;
  begin matching := true;
    if pattern[p]='*' then begin remember(s,length(source)); inc(p) end
    else if pattern[p]='?' then begin remember(s,s); inc(p) end
    else if pattern[p]='#' then matchnum
    else if pattern[p]='\' then matchmeta
    else if source[s]=pattern[p] then begin inc(s); inc(p) end
    else matching := false
  end;
begin
  index := 0; s := 1; p := 1;
  for i:=0 to 9 do begin p1[i]:=1; p2[i]:=0 end;
  while matching and (p<=length(pattern)) and (s<=length(source)) do subgrep;
  product := ''; trigger := false;
  for t:=1 to length(target) do if trigger then
    begin reg := digit(target[t]); if (reg>=0) and (reg<=9) then
      for j:=p1[reg] to p2[reg] do product := product + source[j]
      else if target[t]='\' then product := product + '\'
      else product := product + '\' + target[t];
      trigger := false
    end
    else if (target[t]='\') and (t<length(target)) then trigger := true
    else product := product + target[t];
  source := substr(source,s,length(source));
  pattern := substr(pattern,p,length(pattern));
  target := product
end;


{ "match" tests whether the source matches the pattern exactly }
function match(source, pattern: string): boolean;
  const target: string = '';
begin grep(source, pattern, target);
  match := (source='') and (pattern='')
end;

{ "translate" replaces the pattern by the target in the source. }
function translate(source, pattern, target: string): string;
begin grep(source, pattern, target); translate := target;
end;

end.
