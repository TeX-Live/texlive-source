[Setup]
AppName=gregorio
AppVersion=4.0.0-rc1
DefaultDirName={pf}\gregorio
DefaultGroupName=gregorio
SetupIconFile=gregorio.ico
Compression=lzma2
SolidCompression=yes
LicenseFile=../COPYING.md
AppCopyright=Copyright (C) 2006-2015 The Gregorio Project
AppComments=Software for engraving Gregorian Chant scores.
AppContact=gregorio-devel@gna.org
AppPublisher=The Gregorio Project
AppPublisherURL=https://github.com/gregorio-project/gregorio
AppReadmeFile=https://github.com/gregorio-project/gregorio
BackColor=$D4AE65
BackColor2=$FDF7EB
WizardSmallImageFile=gregorio-32.bmp
WizardImageFile=gregorio-image.bmp
ChangesAssociations=yes

[Registry]
Root: HKCR; Subkey: ".gabc"; ValueType: string; ValueName: ""; ValueData: "Gregorio"; Flags: uninsdeletevalue
Root: HKCR; Subkey: "Gregorio"; ValueType: string; ValueName: ""; ValueData: "Gregorio score"; Flags: uninsdeletekey
Root: HKCR; Subkey: "Gregorio\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\gregorio.ico"; Flags: uninsdeletekey
Root: HKCR; Subkey: "Gregorio\shell\open\command"; ValueType: string; ValueName: ""; ValueData: "texworks.exe ""%1"""; Flags: uninsdeletekey

[Dirs]
Name: "{app}\contrib"
Name: "{app}\examples"
Name: "{app}\texmf"
Name: "{app}\texmf\tex"
Name: "{app}\texmf\tex\luatex"
Name: "{app}\texmf\tex\luatex\gregoriotex"
Name: "{app}\texmf\fonts"
Name: "{app}\texmf\fonts\truetype"
Name: "{app}\texmf\fonts\truetype\public"
Name: "{app}\texmf\fonts\truetype\public\gregoriotex"
Name: "{app}\texmf\fonts\source"
Name: "{app}\texmf\fonts\source\gregoriotex"
Name: "{app}\texmf\doc"
Name: "{app}\texmf\doc\luatex"
Name: "{app}\texmf\doc\luatex\gregoriotex"

[Files]
Source: "../src/gregorio.exe"; DestDir: "{app}";
Source: "gregorio.ico"; DestDir: "{app}";
Source: "install-tl.lua"; DestDir: "{app}";
Source: "install-mt.lua"; DestDir: "{app}";
Source: "README.txt"; DestDir: "{app}"; Flags: isreadme
Source: "../CHANGELOG.md"; DestDir: "{app}";
Source: "../README.md"; DestDir: "{app}";
Source: "../CONTRIBUTORS.md"; DestDir: "{app}";
Source: "../UPGRADE.md"; DestDir: "{app}";
; PARSE_VERSION_FILE_NEXTLINE
Source: "../doc/GregorioRef-4_0_0-rc1.pdf"; DestDir: "{app}";
; PARSE_VERSION_FILE_NEXTLINE
Source: "../doc/GregorioNabcRef-4_0_0-rc1.pdf"; DestDir: "{app}";
Source: "../COPYING.md"; DestDir: "{app}";
Source: "../contrib/900_gregorio.xml"; DestDir: "{app}\contrib";
Source: "../contrib/system-setup.bat"; DestDir: "{app}";
Source: "../contrib/gregorio-scribus.lua"; DestDir: "{app}\contrib";
Source: "../contrib/*"; DestDir: "{app}\contrib";
Source: "../examples/*.gabc"; DestDir: "{app}\examples";
Source: "../examples/main-lualatex.tex"; DestDir: "{app}\examples";
Source: "../tex/gregoriotex.tex"; DestDir: "{app}\texmf\tex\luatex\gregoriotex";
Source: "../tex/gregoriotex.sty"; DestDir: "{app}\texmf\tex\luatex\gregoriotex";
Source: "../tex/gregoriotex.lua"; DestDir: "{app}\texmf\tex\luatex\gregoriotex";
Source: "../tex/gregoriotex-main.tex"; DestDir: "{app}\texmf\tex\luatex\gregoriotex";
Source: "../tex/gregoriotex-chars.tex"; DestDir: "{app}\texmf\tex\luatex\gregoriotex";
Source: "../tex/gregoriotex-signs.tex"; DestDir: "{app}\texmf\tex\luatex\gregoriotex";
Source: "../tex/gregoriotex-signs.lua"; DestDir: "{app}\texmf\tex\luatex\gregoriotex";
Source: "../tex/gregoriotex-spaces.tex"; DestDir: "{app}\texmf\tex\luatex\gregoriotex";
Source: "../tex/gregoriotex-syllable.tex"; DestDir: "{app}\texmf\tex\luatex\gregoriotex";
Source: "../tex/gregoriotex-symbols.tex"; DestDir: "{app}\texmf\tex\luatex\gregoriotex";
Source: "../tex/gregoriotex-nabc.tex"; DestDir: "{app}\texmf\tex\luatex\gregoriotex";
Source: "../tex/gregoriotex-nabc.lua"; DestDir: "{app}\texmf\tex\luatex\gregoriotex";
Source: "../tex/gsp-default.tex"; DestDir: "{app}\texmf\tex\luatex\gregoriotex";
Source: "../tex/gregorio-vowels.dat"; DestDir: "{app}\texmf\tex\luatex\gregoriotex";
Source: "../tex/gregoriosyms.sty"; DestDir: "{app}\texmf\tex\luatex\gregoriotex";
Source: "../fonts/greciliae.ttf"; DestDir: "{app}\texmf\fonts\truetype\public\gregoriotex";
Source: "../fonts/greciliae-op.ttf"; DestDir: "{app}\texmf\fonts\truetype\public\gregoriotex";
Source: "../fonts/gregorio.ttf"; DestDir: "{app}\texmf\fonts\truetype\public\gregoriotex";
Source: "../fonts/gregorio-op.ttf"; DestDir: "{app}\texmf\fonts\truetype\public\gregoriotex";
Source: "../fonts/greextra.ttf"; DestDir: "{app}\texmf\fonts\truetype\public\gregoriotex";
Source: "../fonts/parmesan.ttf"; DestDir: "{app}\texmf\fonts\truetype\public\gregoriotex";
Source: "../fonts/parmesan-op.ttf"; DestDir: "{app}\texmf\fonts\truetype\public\gregoriotex";
Source: "../fonts/gregall.ttf"; DestDir: "{app}\texmf\fonts\truetype\public\gregoriotex";
Source: "../fonts/gresgmodern.ttf"; DestDir: "{app}\texmf\fonts\truetype\public\gregoriotex";
Source: "../fonts/convertsfdtottf.py"; DestDir: "{app}\texmf\fonts\source\gregoriotex";
Source: "../fonts/greciliae-base.sfd"; DestDir: "{app}\texmf\fonts\source\gregoriotex";
Source: "../fonts/greextra.sfd"; DestDir: "{app}\texmf\fonts\source\gregoriotex";
Source: "../fonts/gregall.sfd"; DestDir: "{app}\texmf\fonts\source\gregoriotex";
Source: "../fonts/gresgmodern.sfd"; DestDir: "{app}\texmf\fonts\source\gregoriotex";
Source: "../fonts/gregorio-base.sfd"; DestDir: "{app}\texmf\fonts\source\gregoriotex";
Source: "../fonts/Makefile"; DestDir: "{app}\texmf\fonts\source\gregoriotex";
Source: "../fonts/parmesan-base.sfd"; DestDir: "{app}\texmf\fonts\source\gregoriotex";
Source: "../fonts/squarize.py"; DestDir: "{app}\texmf\fonts\source\gregoriotex";
Source: "../README.md"; DestDir: "{app}\texmf\doc\luatex\gregoriotex";

[Run]
Filename: "texlua.exe"; Parameters: """{app}\install-tl.lua"" > ""{app}\install-tl.log"""; StatusMsg: "Configuring TeXLive texmf..."; Description: "Add files to TeXLive texmf tree"; Flags: postinstall ; WorkingDir: "{app}";
Filename: "texlua.exe"; Parameters: """{app}\install-mt.lua"" > ""{app}\install-mt.log"""; StatusMsg: "Configuring MiKTeX texmf..."; Description: "Add files to MiKTeK texmf tree"; Flags: postinstall ; WorkingDir: "{app}";

[Code]
procedure URLLabelOnClickOne(Sender: TObject);
var
  ErrorCode: Integer;
begin
  ShellExec('open', 'https://www.tug.org/texlive/acquire.html', '', '', SW_SHOWNORMAL, ewNoWait, ErrorCode);
end;

procedure URLLabelOnClickTwo(Sender: TObject);
var
  ErrorCode: Integer;
begin
  ShellExec('open', 'http://gregorio-project.github.io/installation-windows.html', '', '', SW_SHOWNORMAL, ewNoWait, ErrorCode);
end;

procedure CreateTheWizardPages;
var
  Page: TWizardPage;
  StaticText: TNewStaticText;
begin
  Page := CreateCustomPage(wpWelcome, 'Installation Requirements', 'Please read the following important information before continuing.');

  StaticText := TNewStaticText.Create(Page);
  StaticText.Top := ScaleY(0);
  StaticText.Height := ScaleY(15);
  StaticText.Caption := 'You are about to install the gregorio software, which is working with a';
  StaticText.Parent := Page.Surface;
  
  StaticText := TNewStaticText.Create(Page);
  StaticText.Height := ScaleY(15);
  StaticText.Top := ScaleY(13);
  StaticText.Caption := 'typesetting software called LuaTeX.';
  StaticText.Parent := Page.Surface;
  
  StaticText := TNewStaticText.Create(Page);
  StaticText.Top := ScaleY(35);;
  StaticText.Caption := 'The installation and use of gregorio needs LuaTeX in order to work.';
  StaticText.Parent := Page.Surface;

  StaticText := TNewStaticText.Create(Page);
  StaticText.Top := ScaleY(60);;
  StaticText.Caption := 'The best way to install LuaTeX is to install the TeXLive distribution.';
  StaticText.Parent := Page.Surface;

  StaticText := TNewStaticText.Create(Page);
  StaticText.Top := ScaleY(75);;
  StaticText.Caption := 'If you have not already installed it, please do it before proceeding further!';
  StaticText.Parent := Page.Surface;

  StaticText := TNewStaticText.Create(Page);
  StaticText.Top := ScaleY(100);;
  StaticText.Caption := 'Note that you have to reboot your computer after having installed TeXLive';
  StaticText.Parent := Page.Surface;
  
  StaticText := TNewStaticText.Create(Page);
  StaticText.Top := ScaleY(115);;
  StaticText.Caption := 'and before installing Gregorio.';
  StaticText.Parent := Page.Surface;

  StaticText := TNewStaticText.Create(Page);
  StaticText.Top := ScaleY(145);;
  StaticText.Caption := 'https://www.tug.org/texlive/acquire.html';
  StaticText.Cursor := crHand;
  StaticText.OnClick := @URLLabelOnClickOne;
  StaticText.Parent := Page.Surface;
  StaticText.Font.Style := StaticText.Font.Style + [fsUnderline];
  StaticText.Font.Color := clBlue;
  
  StaticText := TNewStaticText.Create(Page);
  StaticText.Top := ScaleY(165);;
  StaticText.Caption := 'http://gregorio-project.github.io/installation-windows.html';
  StaticText.Cursor := crHand;
  StaticText.OnClick := @URLLabelOnClickTwo;
  StaticText.Parent := Page.Surface;
  StaticText.Font.Style := StaticText.Font.Style + [fsUnderline];
  StaticText.Font.Color := clBlue;
end;

procedure InitializeWizard();

begin
  CreateTheWizardPages;
end;
