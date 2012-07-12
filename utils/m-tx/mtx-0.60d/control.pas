unit control;   { DPL 2004-03-21 }
{ Controls which M-Tx features are enabled.
  The interface allows one to interrogate the feature state, to set
  features according to the M-Tx level, and to override features given
  the feature name as a character string.
}

interface 

procedure mtxLevel(level: string);
function setFeature(which: string; val: boolean): boolean;
  { return TRUE means OK, return FALSE means there is no such feature }
procedure printFeatures(anyway: boolean);
  { anyway TRUE means print all feature settings, FALSE means only those
    that were set via setFeature }

function multiFile: boolean;
function splitShortcut: boolean;
function newWordShortcut: boolean;
function doChords: boolean;
function doUptext: boolean;
function doLyrics: boolean;
function unbeamVocal: boolean;
function uptextOnRests: boolean;
function solfaNoteNames: boolean;
function pedanticWarnings: boolean;
function ignoreErrors: boolean;
function hideBlindSlurs: boolean;
function instrumentNames: boolean;
function beVerbose: boolean;
function debugMode: boolean;
function insertDuration: boolean;
function rearrangeNote: boolean;
function countMacro: boolean;
function expandMacro: boolean;
function checkPitch: boolean;

implementation uses utility;

type feature = (noSuchFeature, 
  FmultiFile,  { enables Include: directive }
  FsplitShortcut, { one-word shortcut is treated by simply inserting a blank }
  FnewWordShortcut, { detached form of shortcut allowed }
  FdoChords, { C: lines are taken into account } 
  FdoUptext, { U: lines are taken into account }
  FdoLyrics, { L: lines are taken into account }
  FunbeamVocal, { non-melismatic eighth and shorter notes in vocal lines are unbeamed }
  FhideBlindSlurs, { blind slurs are hidden }
  FuptextOnRests, { U: lines synchronize with words and rests }
  FinsertDuration, { always insert duration into notes }
  FcountMacro, { include macros in the count }
  FexpandMacro, { literally expand macros }
  FcheckPitch, { keep track of pitch and warn if unreal }
  FrearrangeNote, { transform note words to canical form }
  FsolfaNoteNames, { use solfa note names }
  FpedanticWarnings, { issue warnings even when default action is likely to be correct }
  FignoreErrors, { ignore all errors except fatal errors }
  FinstrumentNames, { set instrument names }
  FbeVerbose, { report what is being done }
  FdebugMode { write all possible infomative messages } );

  Tfeature = record
    tag: string[30];
    active, changed: boolean
    end;

const
  firstFeature = FmultiFile;
  lastFeature = FdebugMode;

  feat: array[feature] of Tfeature = (
    ( tag: ''; active: false; changed: false),
    ( tag: 'multiFile'; active: true; changed: false),
    ( tag: 'splitShortcut'; active: true; changed: false),
    ( tag: 'newWordShortcut'; active: true; changed: false),
    ( tag: 'doChords'; active: true; changed: false),
    ( tag: 'doUptext'; active: true; changed: false),
    ( tag: 'doLyrics'; active: true; changed: false),
    ( tag: 'unbeamVocal'; active: true; changed: false),
    ( tag: 'hideBlindSlurs'; active: true; changed: false),
    ( tag: 'uptextOnRests'; active: true; changed: false),
    ( tag: 'insertDuration'; active: true; changed: false),
    ( tag: 'countMacro'; active: false; changed: false),
    ( tag: 'expandMacro'; active: false; changed: false),
    ( tag: 'checkPitch'; active: true; changed: false),
    ( tag: 'rearrangeNote'; active: true; changed: false),
    ( tag: 'solfaNoteNames'; active: false; changed: false),
    ( tag: 'pedanticWarnings'; active: false; changed: false),
    ( tag: 'ignoreErrors'; active: false; changed: false),
    ( tag: 'instrumentNames'; active: false; changed: false),
    ( tag: 'beVerbose'; active: false; changed: false),
    ( tag: 'debugMode'; active: false; changed: false)
  );

procedure printFeatures(anyway: boolean);
  var i: feature;
begin for i:=firstFeature to lastFeature do with feat[i] do
  if changed or anyway then writeln(tag, ' = ', active)
end;

function featureNamed(s: string): feature;
  var i: feature;
begin  for i:=firstFeature to lastFeature do
  if equalsIgnoreCase(s,feat[i].tag) then begin featureNamed := i; exit end;
  featureNamed := noSuchFeature
end;

function setFeature(which: string; val: boolean): boolean;
  var f: feature;
begin setFeature := false; f := featureNamed(which);
  if f <> noSuchFeature then with feat[f] do
  begin active := val; changed := true; setFeature := true end;
  if (f=FdebugMode) and val then feat[FbeVerbose].active := true;      
  if (f=FbeVerbose) and not val then feat[FdebugMode].active := false;
end;

procedure mtxLevel(level: string);
begin
  if level<'0.57' then 
  begin 
    setFeature('splitShortcut',false); 
    setFeature('newWordShortcut',false);
  end
end;

{ Feature functions.  To add a new feature "newFeature":
  1. Insert a new value "FnewFeature" in the declaration of type "feature".
  2. Insert an entry for it in array "feat".
  3. Copy the template below and change "FEATURE" into "newFeature".
  4. Copy the function header to the interface section.
  5. (Optional) Insert code into "mtxLevel" to enable/disable the feature.

function FEATURE: boolean;
begin FEATURE := feat[FFEATURE].active end;
}

function checkPitch: boolean;
begin checkPitch := feat[FcheckPitch].active end;

function countMacro: boolean;
begin countMacro := feat[FcountMacro].active end;

function expandMacro: boolean;
begin expandMacro := feat[FexpandMacro].active end;

function insertDuration: boolean;
begin insertDuration := feat[FinsertDuration].active end;

function rearrangeNote: boolean;
begin rearrangeNote := feat[FrearrangeNote].active end;

function beVerbose: boolean;
begin beVerbose := feat[FbeVerbose].active end;

function debugMode: boolean;
begin debugMode := feat[FdebugMode].active end;

function instrumentNames: boolean;
begin instrumentNames := feat[FinstrumentNames].active end;

function hideBlindSlurs: boolean;
begin hideBlindSlurs := feat[FhideBlindSlurs].active end;

function doLyrics: boolean;
begin doLyrics := feat[FdoLyrics].active end;

function ignoreErrors: boolean;
begin ignoreErrors := feat[FignoreErrors].active end;

function pedanticWarnings: boolean;
begin pedanticWarnings := feat[FpedanticWarnings].active end;

function solfaNoteNames: boolean;
begin solfaNoteNames := feat[FsolfaNoteNames].active end;

function uptextOnRests: boolean;
begin uptextOnRests := feat[FuptextOnRests].active end;

function unbeamVocal: boolean;
begin unbeamVocal := feat[FunbeamVocal].active end;

function doChords: boolean;
begin doChords := feat[FdoChords].active end;

function doUptext: boolean;
begin doUptext := feat[FdoUptext].active end;

function newWordShortcut: boolean;
begin newWordShortcut := feat[FnewWordShortcut].active end;

function splitShortcut: boolean;
begin splitShortcut := feat[FsplitShortcut].active end;

function multiFile: boolean;
begin multiFile := feat[FmultiFile].active end;

end.

