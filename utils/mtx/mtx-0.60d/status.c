/* Output from p2c 1.21alpha-07.Dec.93, the Pascal-to-C translator */
/* From input file "status.pas" */


#include "p2c.h"


#define STATUS_G
#include "status.h"


#ifndef GLOBALS_H
#include "globals.h"
#endif

#ifndef STRINGS_H
#include "strings.h"
#endif

#ifndef MTXLINE_H
#include "mtxline.h"
#endif

#ifndef CONTROL_H
#include "control.h"
#endif

#ifndef UTILITY_H
#include "utility.h"
#endif

#ifndef NOTES_H
#include "notes.h"
#endif


#define lowest_pitch    (-9)
#define highest_pitch   61


typedef struct line_status {
  short pitch, chord_pitch, octave_adjust, beam_level, slur_level, after_slur;
  Char octave, lastnote, chord_lastnote, duration, slurID, tieID;
  boolean beamnext, beamed, slurnext, slurred, no_beam_melisma;
  boolean no_slur_melisma[12];
  int5 chord_tie_pitch;
  Char chord_tie_label[6];
} line_status;


Static Char init_oct[256] = "";
Static line_status current[maxvoices];
Static Char lastdur[maxvoices];


Void chordTie(voice, lab)
short voice;
Char *lab;
{
  short n;
  line_status *WITH;

  WITH = &current[voice-1];
  n = strlen(WITH->chord_tie_label);
  if (n == 5)
    error3(voice, "Only five slur ties allowed per voice");
  if (n == 0)
    *lab = 'T';
  else
    *lab = WITH->chord_tie_label[n-1];
  (*lab)++;
  sprintf(WITH->chord_tie_label + strlen(WITH->chord_tie_label), "%c", *lab);
/* p2c: status.pas, line 66:
 * Note: Possible string truncation in assignment [145] */
  n++;
  WITH->chord_tie_pitch[n-1] = WITH->chord_pitch;
}


Void getChordTies(voice, pitches, labels)
short voice;
short *pitches;
Char *labels;
{
  line_status *WITH;

  WITH = &current[voice-1];
  memcpy(pitches, WITH->chord_tie_pitch, sizeof(int5));
  strcpy(labels, WITH->chord_tie_label);
  *WITH->chord_tie_label = '\0';
}


Void rememberDurations()
{
  voice_index v, FORLIM;

  FORLIM = nvoices;
  for (v = 1; v <= FORLIM; v++)
    lastdur[v-1] = duration(v);
}


Void restoreDurations()
{
  voice_index v, FORLIM;

  FORLIM = nvoices;
  for (v = 1; v <= FORLIM; v++)
    resetDuration(v, lastdur[v-1]);
}


Char duration(voice)
short voice;
{
  return (current[voice-1].duration);
}


Void resetDuration(voice, dur)
short voice;
Char dur;
{
  if (pos1(dur, durations) == 0) {
    printf("Trying to set duration to %c; ", dur);
    error3(voice, "M-Tx system error: resetDuration");
  }
  current[voice-1].duration = dur;
}


Void activateBeamsAndSlurs(voice)
short voice;
{
  line_status *WITH;

  WITH = &current[voice-1];
  if (WITH->beamnext) {
    WITH->beamed = true;
    WITH->beamnext = false;
  }
  if (WITH->slurnext) {
    WITH->slurred = true;
    WITH->slurnext = false;
  }
  if (WITH->slurred)
    WITH->after_slur++;
}


Void saveStatus(voice)
short voice;
{
  line_status *WITH;

  WITH = &current[voice-1];
  WITH->chord_pitch = WITH->pitch;
  WITH->chord_lastnote = WITH->lastnote;
}


boolean noBeamMelisma(voice)
short voice;
{
  return (current[voice-1].no_beam_melisma);
}


short afterSlur(voice)
short voice;
{
  short Result;
  line_status *WITH;

  WITH = &current[voice-1];
  Result = WITH->after_slur;
  if (WITH->after_slur > 0 && WITH->slur_level < 1)
    error3(voice, "M-Tx system error: afterSlur and slur_level incompatible)");
  return Result;
}


Char octave(voice)
short voice;
{
  return (current[voice-1].octave);
}


Void resetOctave(voice)
short voice;
{
  current[voice-1].octave = ' ';
}


Void initOctaves(octaves)
Char *octaves;
{
  short i = 1;

  strcpy(init_oct, octaves);
  while (i <= strlen(init_oct)) {
    if (init_oct[i-1] == ' ')
      delete1(init_oct, i);
    else
      i++;
  }
}


Static Char initOctave(voice_stave)
stave_index voice_stave;
{
  if (voice_stave > strlen(init_oct)) {
    if (pos1(clef[voice_stave-1], "Gt08") > 0)
      return '4';
    else
      return '3';
  } else
    return (init_oct[voice_stave-1]);
}


Void setOctave(voice)
short voice;
{
  current[voice-1].octave = initOctave(voiceStave(voice));
}


Void newOctave(voice, dir)
short voice;
Char dir;
{
  line_status *WITH;

  WITH = &current[voice-1];
  switch (dir) {

  case '+':
    WITH->octave++;
    break;

  case '-':
    WITH->octave--;
    break;
  }
}


Static short newPitch(voice, note_, pitch, lastnote)
short voice;
Char *note_;
short pitch;
Char lastnote;
{
  Char note[256];
  short interval, npitch;
  Char oct;

  strcpy(note, note_);
  /*if debugMode then
    write('New pitch for note ',note,' relative to ',lastnote,
       ' at pitch ',pitch);*/
  oct = octaveCode(note);
  if (oct == '=')
    oct = initOctave(voiceStave(voice));
  if (isdigit(oct)) {
    pitch = (oct - '0') * 7 - 3;
    lastnote = 'f';
    removeOctaveCode(oct, note);
    oct = octaveCode(note);
  }
  interval = note[0] - lastnote;
  if (interval > 3)
    interval -= 7;
  if (interval < -3)
    interval += 7;
  npitch = pitch + interval;   /*if debugMode then write(' was ',npitch);*/
  while (oct != ' ') {
    if (oct == '+')
      npitch += 7;
    else if (oct == '-')
      npitch -= 7;
    removeOctaveCode(oct, note);
    oct = octaveCode(note);
  }
  /*if debugMode then writeln(' is ',npitch);*/
  return npitch;
}


Local Void delins(note, c1, c2, l)
Char *note;
Char c1, c2;
short l;
{
  short i, n;

  n = strlen(note);
  i = pos1(c1, note);
  if (i == 0)
    i = n + 1;
  while (l > 0 && i <= n) {
    delete1(note, i);
    n--;
    l--;
  }
  i = pos1(c2, note);
  if (i == 0) {
    if (strlen(note) < 2)
      error("M-Tx program error", print);
    else
      i = 3;
  }
  while (l > 0) {
    insertChar(c2, note, i);
    l--;
  }
}


Static Void repitch(note, diff)
Char *note;
short diff;
{
  diff /= 7;
  if (diff > 0)
    delins(note, '-', '+', diff);
  else
    delins(note, '+', '-', -diff);
}


Void setUnbeamed(voice)
short voice;
{
  current[voice-1].beamed = false;
}


Void setUnslurred(voice)
short voice;
{
  line_status *WITH;

  WITH = &current[voice-1];
  WITH->slurred = false;
  WITH->after_slur = 0;
}


Void beginBeam(voice, note)
short voice;
Char *note;
{
  line_status *WITH;

  WITH = &current[voice-1];
  if (WITH->beamed)
    error3(voice, "Starting a forced beam while another is open");
  if (WITH->beam_level > 0)
    error3(voice,
	   "Starting a forced beam while another is open (beamlevel>0)");
  WITH->beam_level++;
  WITH->beamnext = true;
  WITH->no_beam_melisma = startsWith(note, "[[");
  if (WITH->no_beam_melisma)
    predelete(note, 1);
}


Void endBeam(voice)
short voice;
{
  line_status *WITH;

  WITH = &current[voice-1];
  if (WITH->beam_level < 1)
    error3(voice, "Closing a beam that was never opened");
  WITH->beam_level--;
  setUnbeamed(voice);
}


short slurLevel(voice)
short voice;
{
  return (current[voice-1].slur_level);
}


short beamLevel(voice)
short voice;
{
  return (current[voice-1].beam_level);
}


boolean noSlurMelisma(voice, history)
short voice, history;
{
  line_status *WITH;

  WITH = &current[voice-1];
  return (WITH->no_slur_melisma[WITH->slur_level + history - 1]);
}


Static Char *slurLabel(Result, voice, note)
Char *Result;
short voice;
Char *note;
{
  Char sl;

  if (*note == '\0')
    return strcpy(Result, "");
  if (strlen(note) < 2)
    return strcpy(Result, " ");
  if (note[1] >= '0' && note[1] <= 'Z')
    sl = note[1];
  else
    sl = ' ';
  if (sl >= 'I' && sl <= 'T')
    warning3(voice, "Slur label in the range I..T may cause conflict");
  sprintf(Result, "%c", sl);
  return Result;
}


Static Void labelSlur(voice, note)
short voice;
Char *note;
{
  Char sl;
  line_status *WITH;

  if (*note == '\0')
    return;
  WITH = &current[voice-1];
  if (note[0] == ')')
    WITH->slurID += 2;
  else if (note[0] == '}')
    WITH->tieID += 2;
  /** CMO 0.60a: replace assigning tieID to sl by space charater
    if (note[1]='(') or (note[1]=')') then sl:=slurID else sl:=tieID; */
  if (note[0] == '(' || note[0] == ')')
    sl = WITH->slurID;
  else
    sl = ' ';
  /** CMO 0.60d: omit insertchar in case of tie
    insertchar(sl,note,2); */
  if (note[0] == '(' || note[0] == ')')
    insertChar(sl, note, 2);
  if (note[0] == '(')
    WITH->slurID -= 2;
  else if (note[0] == '{')
    WITH->tieID -= 2;
  if (WITH->slurID < 'I')
    warning3(voice, "Too many nested slurs may cause conflict");
  if (WITH->tieID < 'I')
    warning3(voice, "Too many nested ties may cause conflict");
}


Void beginSlur(voice, note)
short voice;
Char *note;
{
  short posblind;
  line_status *WITH;
  Char STR1[256], STR2[256];

  WITH = &current[voice-1];
  WITH->slur_level++;
  if (WITH->slur_level > 12)
    error3(voice, "Too many open slurs");
  WITH->no_slur_melisma[WITH->slur_level - 1] = (startsWith(note, "((") ||
						 startsWith(note, "{{"));
  if (WITH->no_slur_melisma[WITH->slur_level - 1])
    predelete(note, 1);
  if (!strcmp(slurLabel(STR1, voice, note), "0"))
    delete1(note, 2);
  else {
    if (!strcmp(slurLabel(STR2, voice, note), " "))
      labelSlur(voice, note);
  }
  posblind = pos1('~', note);
  if (posblind > 0) {
    if (hideBlindSlurs())
      *note = '\0';
    else
      delete1(note, posblind);
  }
  WITH->slurnext = true;
}


Void endSlur(voice, note)
short voice;
Char *note;
{
  short poscontinue, posblind;
  Char contslur[256];
  line_status *WITH;
  Char STR1[256], STR2[256];

  WITH = &current[voice-1];
  *contslur = '\0';
  if (WITH->slur_level < 1)
    error3(voice, "Ending a slur that was never started");
  if (note[0] == ')')
    poscontinue = pos1('(', note);
  else if (note[0] == '}')
    poscontinue = pos1('{', note);
  if (poscontinue == 0)
    WITH->slur_level--;
  else {
    poscontinue--;
    strcpy(contslur, note);
    predelete(contslur, poscontinue);
    shorten(note, poscontinue);
  }
  if (WITH->slur_level == 0)
    setUnslurred(voice);
  if (!strcmp(slurLabel(STR1, voice, note), "0"))
    delete1(note, 2);
  else {
    if (!strcmp(slurLabel(STR2, voice, note), " "))
      labelSlur(voice, note);
  }
  if (!strcmp(slurLabel(STR1, voice, contslur), "0"))
    delete1(contslur, 2);
  else {
    if (!strcmp(slurLabel(STR2, voice, contslur), " "))
      labelSlur(voice, contslur);
  }
  if (poscontinue > 0) {
    if (note[0] == '}')
      strcat(note, "t");
    note[0] = 's';
    if (contslur[0] == '{')
      strcat(contslur, "t");
    contslur[0] = 's';
  }
  posblind = pos1('~', note);
  if (posblind > 0) {
    if (hideBlindSlurs())
      *note = '\0';
    else
      delete1(note, posblind);
  }
  if (*note != '\0' && *contslur != '\0')
    sprintf(note + strlen(note), " %s", contslur);
}


Void renewPitch(voice, note)
short voice;
Char *note;
{
  short pstat;
  line_status *WITH;

  WITH = &current[voice-1];
  pstat = newPitch(voice, note, WITH->chord_pitch, WITH->chord_lastnote);
  if (debugMode())
    printf("Current pitch in voice %d is %d, last note was %c, this note is %s",
	   voice, WITH->pitch, WITH->lastnote, note);
  WITH->pitch = newPitch(voice, note, WITH->pitch, WITH->lastnote);
  if (WITH->pitch != pstat)
    repitch(note, WITH->pitch - pstat);
  if (WITH->pitch < lowest_pitch && checkPitch()) {
    printf("Pitch of note %s following %c reported as %d",
	   note, WITH->lastnote, WITH->pitch);
    error3(voice, "Pitch too low");
  }
  if (WITH->pitch > highest_pitch && checkPitch()) {
    printf("Pitch of note %s following %c reported as %d",
	   note, WITH->lastnote, WITH->pitch);
    error3(voice, "Pitch too high");
  }
  WITH->lastnote = note[0];
  if (debugMode())
    printf(", repitched to %d\n", WITH->pitch);
  if (debugMode() && WITH->pitch != pstat)
    printf("Pitch from melodic line = %d   from last chordal note = %d\n",
	   WITH->pitch, pstat);
}


short chordPitch(voice)
short voice;
{
  return (current[voice-1].chord_pitch);
}


Void renewChordPitch(voice, note)
short voice;
Char *note;
{
  line_status *WITH;

  WITH = &current[voice-1];
  WITH->chord_pitch = newPitch(voice, note, WITH->chord_pitch,
			       WITH->chord_lastnote);
  if (WITH->chord_pitch < lowest_pitch)
    error3(voice, "Pitch in chord too low");
  if (WITH->chord_pitch > highest_pitch)
    error3(voice, "Pitch in chord too high");
  WITH->chord_lastnote = note[0];
}


Void initStatus()
{
  short voice, FORLIM;
  line_status *WITH;

  FORLIM = nvoices;
  for (voice = 1; voice <= FORLIM; voice++) {
    WITH = &current[voice-1];
    WITH->duration = default_duration;
    WITH->octave_adjust = 0;
    WITH->slur_level = 0;
    WITH->beam_level = 0;
    WITH->beamed = false;
    WITH->beamnext = false;
    WITH->slurred = false;
    WITH->slurnext = false;
    WITH->after_slur = 0;
    WITH->octave = initOctave(voiceStave(voice));
    WITH->slurID = 'S';
    WITH->tieID = 'T';
    WITH->lastnote = 'f';
    WITH->pitch = (WITH->octave - '0') * 7 - 3;
    *WITH->chord_tie_label = '\0';
    saveStatus(voice);
  }
}




/* End. */
