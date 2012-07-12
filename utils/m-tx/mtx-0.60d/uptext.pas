unit uptext;
{ Insert and translate uptext }

interface

procedure initUptext;
procedure clearUptext;
function uptextLineNo(voice: integer): integer;
procedure setUptextLineNo(voice, lno: integer);
procedure addUptext(voice: integer; var no_uptext: boolean;
  var pretex: string);

implementation uses globals, strings, mtxline, utility;

type uptext_info = record
  uptext, uptext_adjust, uptext_lcz: integer;
  uptext_font: string;
  end;

var U: array[voice_index] of uptext_info;

function uptextLineNo(voice: integer): integer;
begin uptextLineNo := U[voice].uptext; end;

procedure setUptextLineNo(voice, lno: integer);
begin U[voice].uptext := lno; end;

procedure clearUptext;
  var voice: voice_index;
begin for voice:=1 to nvoices do U[voice].uptext:=0; end;

procedure initUptext;
  var voice: voice_index;
begin  for voice:=1 to nvoices do with U[voice] do
  begin
    uptext_adjust:=0; uptext_lcz:=3; uptext_font:='';
  end;
end;

procedure textTranslate(var uptext, font: string);
  var k: integer;
begin
  if uptext='' then exit;
  repeat k := pos1('%',uptext);
    if k>0 then uptext:=
      substr(uptext,1,k-1)+'{\mtxFlat}'+substr(uptext,k+1,length(uptext)-k);
  until k=0;
  repeat k := pos1('#',uptext);
    if k>0 then uptext:=
      substr(uptext,1,k-1)+'{\mtxSharp}'+substr(uptext,k+1,length(uptext)-k);
  until k=0;
  case uptext[1] of
  '<': if uptext='<' then uptext:='\mtxIcresc'
    else if uptext='<.' then uptext:='\mtxTcresc'
     else begin
       predelete(uptext,1); uptext:='\mtxCresc{'+uptext+'}'
     end;
  '>': if uptext='>' then uptext:='\mtxIdecresc'
    else if uptext='>.' then uptext:='\mtxTdecresc'
    else begin
      predelete(uptext,1); uptext:='\mtxDecresc{'+uptext+'}'
    end;
  else for k:=1 to length(uptext) do if pos1(uptext[k],'mpfzrs~')=0 then exit;
  end;
  font:='\mtxPF';
end;

procedure addUptext(voice: integer; var no_uptext: boolean;
  var pretex: string);
    var w, font: string;
        adj: integer;
    const default = 10;  under = -14;
          lcz: string[3] = 'lcz';
    procedure adjustUptext;
      var letter: char;
          force: boolean;
    begin  delete1(w,1); force:=false;
      while w<>'' do with U[voice] do
      begin letter:=w[1]; delete1(w,1); with U[voice] do
        case letter of
    '<':  if uptext_lcz>1 then dec(uptext_lcz);
    '>':  if uptext_lcz<3 then inc(uptext_lcz);
    '^':  uptext_adjust:=0;
    'v':  uptext_adjust:=under;
    '=':  force:=true;
'+','-':  begin if w<>'' then getNum(w,adj) else adj:=0;
            if letter = '-' then adj := -adj;
            if force then uptext_adjust := adj else inc(uptext_adjust,adj);
            w:='';
          end;
    else  error3(voice,'Unknown uptext adjustment');
        end;
      end;
      w:='!';
    end;

    begin  
    with U[voice] do begin
      if uptext=0 then no_uptext := true;
      if no_uptext then exit;
      repeat
        w := GetNextWord(P[uptext],blank,dummy);
        if (w=barsym) or (w='') then no_uptext:=true;
        if (w=tilde) or no_uptext then exit;
        if w[1]='!' then begin uptext_font:=w; uptext_font[1]:='\'; end;
        if w[1]='@' then adjustUptext;
      until w[1]<>'!';  { ! is a kludge, will get me in trouble later }
      font:=uptext_font;  textTranslate(w,font);
      if font<>'' then w:=font+'{'+w+'}';
      case lcz[uptext_lcz] of
'l':    w:='\mtxLchar{' + toString(default+uptext_adjust) + '}{' + w + '}';
'c':    w:='\mtxCchar{' + toString(default+uptext_adjust) + '}{' + w + '}';
'z':    w:='\mtxZchar{' + toString(default+uptext_adjust) + '}{' + w + '}';
      end;
      pretex:=pretex+w;
    end;
  end;

end.
