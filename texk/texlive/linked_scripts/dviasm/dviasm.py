#! /usr/bin/env python3
# -*- coding: utf-8 -*-
#
# This is DVIasm, a DVI utility for editing DVI files directly.
#
# Copyright (C) 2007-2008 by Jin-Hwan Cho <chofchof@ktug.or.kr>
# Copyright (C) 2011-2017 by Khaled Hosny <khaledhosny@eglug.org>
# Copyright (C) 2019      by Arthur Reutenauer <arthur@reutenauer.eu>
# Copyright (C) 2019-2024 by Hironobu Yamashita <h.y.acetaminophen@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

import sys, os.path
from optparse import OptionParser

# Global variables
is_ptex = False
is_subfont = False
cur_font = None
cur_dsize = 0
cur_ssize = 0
subfont_idx = 0
subfont_list = ['cyberb', 'outbtm', 'outbtb', 'outgtm', 'outgtb']

# DVI opcodes
SET_CHAR_0 = 0; SET_CHAR_127 = 127;
SET1 = 128; SET2 = 129; SET3 = 130; SET4 = 131;
SET_RULE = 132;
PUT1 = 133; PUT2 = 134; PUT3 = 135; PUT4 = 136;
PUT_RULE = 137;
NOP = 138;
BOP = 139; EOP = 140;
PUSH = 141; POP = 142;
RIGHT1 = 143; RIGHT2 = 144; RIGHT3 = 145; RIGHT4 = 146;
W0 = 147; W1 = 148; W2 = 149; W3 = 150; W4 = 151;
X0 = 152; X1 = 153; X2 = 154; X3 = 155; X4 = 156;
DOWN1 = 157; DOWN2 = 158; DOWN3 = 159; DOWN4 = 160;
Y0 = 161; Y1 = 162; Y2 = 163; Y3 = 164; Y4 = 165;
Z0 = 166; Z1 = 167; Z2 = 168; Z3 = 169; Z4 = 170;
FNT_NUM_0 = 171; FNT_NUM_63 = 234;
FNT1 = 235; FNT2 = 236; FNT3 = 237; FNT4 = 238;
XXX1 = 239; XXX2 = 240; XXX3 = 241; XXX4 = 242;
FNT_DEF1 = 243; FNT_DEF2 = 244; FNT_DEF3 = 245; FNT_DEF4 = 246;
PRE = 247; POST = 248; POST_POST = 249;
# DVIV opcodes
DIR = 255;
# DVI-IVD opcodes
BEGIN_REFLECT = 250; END_REFLECT = 251;
# XDV opcodes
NATIVE_FONT_DEF = 252;
GLYPHS = 253;
TEXT_GLYPHS = 254;
# XDV flags
XDV_FLAG_VERTICAL = 0x0100;
XDV_FLAG_COLORED = 0x0200;
XDV_FLAG_EXTEND = 0x1000;
XDV_FLAG_SLANT = 0x2000;
XDV_FLAG_EMBOLDEN = 0x4000;
# DVI identifications
DVI_ID = 2; DVIV_ID = 3; XDVI_ID = 6; XDV_ID = 7;
DVI_IDS = (DVI_ID, DVIV_ID, XDVI_ID, XDV_ID)

def warning(msg):
  sys.stderr.write('warning: %s\n' % msg)

def ValidID(dvi_id):
    if dvi_id not in DVI_IDS:
        return False
    return True

def BadDVI(msg):
  raise AttributeError('Bad DVI file: %s!' % msg)

def GetByte(fp): # { returns the next byte, unsigned }
  try: return ord(fp.read(1))
  except: return -1

def SignedByte(fp): # { returns the next byte, signed }
  try: b = ord(fp.read(1))
  except: return -1
  if b < 128: return b
  else: return b - 256

def Get2Bytes(fp): # { returns the next two bytes, unsigned }
  try: a, b = fp.read(2)
  except: BadDVI('Failed to Get2Bytes()')
  return (a << 8) + b

def SignedPair(fp): # {returns the next two bytes, signed }
  try: a, b = fp.read(2)
  except: BadDVI('Failed to SignedPair()')
  if a < 128: return (a << 8) + b
  else: return ((a - 256) << 8) + b

def Get3Bytes(fp): # { returns the next three bytes, unsigned }
  try: a, b, c = fp.read(3)
  except: BadDVI('Failed to Get3Bytes()')
  return (((a << 8) + b) << 8) + c

def SignedTrio(fp): # { returns the next three bytes, signed }
  try: a, b, c = fp.read(3)
  except: BadDVI('Failed to SignedTrio()')
  if a < 128: return (((a << 8) + b) << 8) + c
  else: return ((((a - 256) << 8) + b) << 8) + c

def Get4Bytes(fp): # { returns the next four bytes, unsigned }
  try: a, b, c, d = fp.read(4)
  except: BadDVI('Failed to Get4Bytes()')
  return (((((a << 8) + b) << 8) + c) << 8) + d

def SignedQuad(fp): # { returns the next four bytes, signed }
  try: a, b, c, d = fp.read(4)
  except: BadDVI('Failed to get SignedQuad()')
  if a < 128: return (((((a << 8) + b) << 8) + c) << 8) + d
  else: return ((((((a - 256) << 8) + b) << 8) + c) << 8) + d

def PutByte(q):
  return bytes.fromhex('%02x' % (q & 0xff))

def Put2Bytes(q):
  return PutByte(q>>8) + PutByte(q)

def Put3Bytes(q):
  return PutByte(q>>16) + PutByte(q>>8) + PutByte(q)

def PutSignedQuad(q):
  if q < 0: q += 0x100000000
  return PutByte(q>>24) + PutByte(q>>16) + PutByte(q>>8) + PutByte(q)

def PutUnsigned(q):
  if q >= 0x1000000: return (3, PutSignedQuad(q))
  if q >= 0x10000:   return (2, Put3Bytes(q))
  if q >= 0x100:     return (1, Put2Bytes(q))
  return (0, PutByte(q))

def PutSigned(q):
  if q < -0x800000 or q >= 0x800000:  return (3, PutSignedQuad(q))
  if q >= 0x8000:                     return (2, Put3Bytes(q))
  if q < -0x8000:     q += 0x1000000; return (2, Put3Bytes(q))
  if q >= 0x80:                       return (1, Put2Bytes(q))
  if q < -0x80:       q += 0x10000;   return (1, Put2Bytes(q))
  return (0, PutByte(q))

def PutGlyphs(width, glyphs):
  s = []
  length = len(glyphs)
  s.append(PutSignedQuad(width))
  s.append(Put2Bytes(length))
  for glyph in glyphs:
    s.append(PutSignedQuad(glyph["x"]))
    s.append(PutSignedQuad(glyph["y"]))
  for glyph in glyphs:
    s.append(Put2Bytes(glyph["id"]))

  return b''.join(s)

def PutTextGlyphs(text, width, glyphs):
  s = []
  length = len(text)
  s.append(Put2Bytes(length))
  for ch in text:
    s.append(Put2Bytes(ch))
  s.append(PutGlyphs(width, glyphs))

  return b''.join(s)

def GetInt(s):
  try: return int(s)
  except: return -1

def GetStrASCII(s): # used in Parse()
  if len(s) > 1 and ((s[0] == "'" and s[-1] == "'") or (s[0] == '"' and s[-1] == '"')):
    return [ord(c) for c in s[1:-1].decode('unicode_escape')]
  else: return ''

def UCS2toJIS(c):
  try:
    s = c.encode('iso2022-jp')
  except UnicodeEncodeError:
    s = c.encode('raw_unicode_escape')
  if len(s) == 1: return ord(s)
  else:           return (s[3] << 8) + s[4]

def XUnicodeDecode(s): # dirty hack to handle >= 0x110000
  t = []
  i = 1
  while i < len(s)-1:
    c = -1
    if s[i] == '\\':
      if s[i+1] == 'x':
        try:
          c = int(s[i+2:i+4],16)
          i += 4
        except ValueError:
          warning('Invalid escape sequence \\x ignored!')
          i += 2
      elif s[i+1] == 'u':
        try:
          c = int(s[i+2:i+6],16)
          i += 6
        except ValueError:
          warning('Invalid escape sequence \\u ignored!')
          i += 2
      elif s[i+1] == 'U':
        try:
          c = int(s[i+2:i+10],16)
          i += 10
        except ValueError:
          warning('Invalid escape sequence \\U ignored!')
          i += 2
      else:
        if i+1 < len(s)-1:
          c = ord(s[i+1])
          i += 2
        else:
          warning('Invalid escape character \\ ignored!')
          i += 1
    else:
          c = ord(s[i])
          i += 1
    if is_ptex: c = UCS2toJIS(chr(c))
    if c >= 0: t.append(c)
  return t

def GetStrUTF8(s): # used in Parse()
  if len(s) > 1 and ((s[0] == "'" and s[-1] == "'") or (s[0] == '"' and s[-1] == '"')):
    try: # This should work for valid Unicode
      t = s[1:-1].encode('raw_unicode_escape').decode('unicode_escape')
      if is_ptex: return [UCS2toJIS(c) for c in t]
      else:       return [ord(c)       for c in t]
    except UnicodeDecodeError: # e.g. for upTeX >= 0x110000
                  return XUnicodeDecode(s)
  else:           return ''

def PutStrASCII(t): # used in Dump()
  s = ''
  for o in t:
    if o == 92:         s += '\\\\'
    elif 32 <= o < 127: s += chr(o)
    elif o < 256:       s += ('\\x%02x' % o)
    elif o < 65536:     s += ('\\u%04x' % o)
    else:               s += ('\\U%08x' % o)
  return "'%s'" % s

def PutStrLatin1(t): # used in Dump()
  s = ''
  for o in t:
    if o == 92:                           s += '\\\\'
    elif 32 <= o < 127 or 161 <= o < 256: s += chr(o)
    elif o < 256:                         s += ('\\x%02x' % o)
    elif o < 65536:                       s += ('\\u%04x' % o)
    else:                                 s += ('\\U%08x' % o)
  return "'%s'" % s

def DecodeISO2022JP(c):
  try:
    s = bytes.fromhex("1b 24 42 %02x %02x" % (c//256, c%256)).decode('iso2022-jp')
  except UnicodeDecodeError:
    s = chr(c)
  return s

def PutStrUTF8(t): # used in Dump()
  s = ''
  if is_subfont:
    for o in t:
      s += chr((subfont_idx << 8) + o)
  else: # not the case of subfont
    for o in t:
      if o == 92:         s += '\\\\'
      elif 32 <= o < 127: s += chr(o)
      elif o < 128:       s += ('\\x%02x' % o)
      elif is_ptex:
        s += DecodeISO2022JP(o)
      else:
        try:              s += chr(o)
        except ValueError: # upTeX may have >= 0x110000
          if o < 256:         s += '\\x%02x' % o
          elif o < 65536:     s += '\\u%04x' % o
          else:               s += '\\U%08x' % o
  return "'%s'" % s

def IsFontChanged(f, z):
  global cur_font, cur_ssize, subfont_idx, is_subfont
  for n in subfont_list:
    if n == f[:-2]:
      is_subfont = True
      subfont_idx = int(f[-2:], 16)
      if cur_font == n and cur_ssize == z:
        return False
      else:
        cur_font = n; cur_ssize = z
        return True
  else:
    is_subfont = False
    cur_font = f; cur_ssize = z
    return True

############################################################
# DVI class
############################################################
class DVI(object):
  def __init__(self, unit='pt'):
    if   unit == 'sp': self.byconv = self.by_sp_conv
    elif unit == 'bp': self.byconv = self.by_bp_conv
    elif unit == 'mm': self.byconv = self.by_mm_conv
    elif unit == 'cm': self.byconv = self.by_cm_conv
    elif unit == 'in': self.byconv = self.by_in_conv
    else:              self.byconv = self.by_pt_conv
    self.Initialize()

  ##########################################################
  # Initialize: Required by __init__(), Load(), and Parse()
  ##########################################################
  def Initialize(self):
    self.id = DVI_ID
    self.numerator   = 25400000
    self.denominator = 473628672
    self.mag = 1000
    self.ComputeConversionFactors()
    self.comment = ''
    self.font_def = {}
    self.max_v = self.max_h = self.max_s = self.total_pages = 0
    self.pages = []

  ##########################################################
  # Load: DVI -> Internal Format
  ##########################################################
  def Load(self, fn):
    fp = open(fn, 'rb')
    self.LoadFromFile(fp)
    fp.close()

  def LoadFromFile(self, fp):
    self.Initialize()
    fp.seek(0, 2)
    if fp.tell() < 53: BadDVI('less than 53 bytes long')
    self.ProcessPreamble(fp)
    self.ProcessPostamble(fp)
    loc = self.first_backpointer
    while loc >= 0:
      fp.seek(loc)
      if GetByte(fp) != BOP: BadDVI('byte %d is not bop' % fp.tell())
      cnt = [SignedQuad(fp) for i in range(10)]
      loc = SignedQuad(fp)
      page = self.ProcessPage(fp)
      self.pages.insert(0, {'count':cnt, 'content':page})

  def ProcessPreamble(self, fp):
    fp.seek(0)
    if GetByte(fp) != PRE: BadDVI("First byte isn't start of preamble")
    id = GetByte(fp)
    if not ValidID(id):
      warning("ID byte is %d; use the default %d!" % (id, DVI_ID))
    self.id = id
    numerator = SignedQuad(fp)
    if numerator <= 0:
      warning('numerator is %d; use the default 25400000!' % numerator)
    else:
      self.numerator = numerator
    denominator = SignedQuad(fp)
    if denominator <= 0:
      warning('denominator is %d; use the default 473628672!' % denominator)
    else:
      self.denominator = denominator
    mag = SignedQuad(fp)
    if mag <= 0:
      warning('magnification is %d; use the default 1000!' % mag)
    else:
      self.mag = mag
    self.comment = fp.read(GetByte(fp)).decode('utf8')
    self.ComputeConversionFactors()

  def ProcessPostamble(self, fp):
    fp.seek(-5, 2) # at least four 223's
    while True:
      k = GetByte(fp)
      if   k < 0:    BadDVI('all 223s; is it a DVI file?') # found EOF
      elif k != 223: break
      fp.seek(-2, 1)
    if not ValidID(k):
      warning('ID byte is %d' % k)
    fp.seek(-5, 1)
    q = SignedQuad(fp)
    m = fp.tell() # id_byte
    if q < 0 or q > m - 33: BadDVI('post pointer %d at byte %d' % (q, m - 4))
    fp.seek(q) # move to post
    k = GetByte(fp)
    if k != POST: BadDVI('byte %d is not post' % k)
    self.post_loc = q
    self.first_backpointer = SignedQuad(fp)

    if SignedQuad(fp) != self.numerator:
      warning("numerator doesn't match the preamble!")
    if SignedQuad(fp) != self.denominator:
      warning("denominator doesn't match the preamble!")
    if SignedQuad(fp) != self.mag:
      warning("magnification doesn't match the preamble!")
    self.max_v = SignedQuad(fp)
    self.max_h = SignedQuad(fp)
    self.max_s = Get2Bytes(fp)
    self.total_pages = Get2Bytes(fp)
    while True:
      k = GetByte(fp)
      if   k == FNT_DEF1: p = GetByte(fp)
      elif k == FNT_DEF2: p = Get2Bytes(fp)
      elif k == FNT_DEF3: p = Get3Bytes(fp)
      elif k == FNT_DEF4: p = SignedQuad(fp)
      elif k == NATIVE_FONT_DEF: p = SignedQuad(fp)
      elif k != NOP: break
      if k == NATIVE_FONT_DEF: self.DefineNativeFont(p, fp)
      else: self.DefineFont(p, fp)
    if k != POST_POST:
      warning('byte %d is not postpost!' % (fp.tell() - 1))
    if SignedQuad(fp) != self.post_loc:
      warning('bad postamble pointer in byte %d!' % (fp.tell() - 4))
    m = GetByte(fp)
    if not ValidID(m):
      warning('identification in byte %d should be one of: %s!' % (fp.tell() - 1, DVI_IDS))
    if not self.id == m:
      if not (self.id == 2 and m == 3): # pTeX/upTeX with dir allowed
        warning('ID byte mismatch: preamble %d vs postamble %d!' % (self.id, m))

  def DefineFont(self, e, fp):
    c = SignedQuad(fp) # font_check_sum
    q = SignedQuad(fp) # font_scaled_size
    d = SignedQuad(fp) # font_design_size
    n = fp.read(GetByte(fp) + GetByte(fp)).decode('utf8')
    try:
      f = self.font_def[e]
    except KeyError:
      self.font_def[e] = {'name':n, 'checksum':c, 'scaled_size':q, 'design_size':d}
      if q <= 0 or q >= 0o1000000000:
        warning("%s---not loaded, bad scale (%d)!" % (n, q))
      elif d <= 0 or d >= 0o1000000000:
        warning("%s---not loaded, bad design size (%d)!" % (n, d))
    else:
      if f['checksum'] != c:
        warning("\t---check sum doesn't match previous definition!")
      if f['scaled_size'] != q:
        warning("\t---scaled size doesn't match previous definition!")
      if f['design_size'] != d:
        warning("\t---design size doesn't match previous definition!")
      if f['name'] != n:
        warning("\t---font name doesn't match previous definition!")

  def DefineNativeFont(self, e, fp):
    size = Get4Bytes(fp) # scaled size
    flags = Get2Bytes(fp)
    l = GetByte(fp) # name length
    fnt_name = fp.read(l).decode('utf8')
    index = Get4Bytes(fp) # face index
    ext = []
    embolden = 0
    if flags & XDV_FLAG_VERTICAL: ext.append("vertical")
    if flags & XDV_FLAG_COLORED: ext.append("color=%08X" % Get4Bytes(fp))
    if flags & XDV_FLAG_EXTEND: ext.append("extend=%d" % SignedQuad(fp))
    if flags & XDV_FLAG_SLANT: ext.append("slant=%d" % SignedQuad(fp))
    if flags & XDV_FLAG_EMBOLDEN: ext.append("embolden=%d" % SignedQuad(fp))
    try:
      f = self.font_def[e]
    except KeyError:
      if index > 0:
        fnt_name += "[%d]" % index
      name = '"%s"' % fnt_name
      if ext:
        name = '"%s:%s"' % (fnt_name, ";".join(ext))

      self.font_def[e] = {
        'name': name,
        'checksum': 0,
        'scaled_size': size,
        'design_size': 655360, # hardcoded
        }

  def ProcessPage(self, fp):
    s = []
    while True:
      o = GetByte(fp)
      p = self.Get1Arg(o, fp)
      if o < SET_CHAR_0 + 128 or o in (SET1, SET2, SET3, SET4):
        q = [p]
        while True:
          o = GetByte(fp)
          p = self.Get1Arg(o, fp)
          if o < SET_CHAR_0 + 128 or o in (SET1, SET2, SET3, SET4):
            q.append(p)
          else:
            break
        s.append([SET1, q])
      if o == SET_RULE:
        s.append([SET_RULE, [p, SignedQuad(fp)]])
      elif o in (PUT1, PUT2, PUT3, PUT4):
        s.append([PUT1, [p]])
      elif o == PUT_RULE:
        s.append([PUT_RULE, [p, SignedQuad(fp)]])
      elif o == NOP:
        continue
      elif o == BOP:
        warning('bop occurred before eop!')
        break
      elif o == EOP:
        break
      elif o == PUSH:
        s.append([PUSH])
      elif o == POP:
        s.append([POP])
      elif o in (RIGHT1, RIGHT2, RIGHT3, RIGHT4):
        s.append([RIGHT1, p])
      elif o == W0:
        s.append([W0])
      elif o in (W1, W2, W3, W4):
        s.append([W1, p])
      elif o == X0:
        s.append([X0])
      elif o in (X1, X2, X3, X4):
        s.append([X1, p])
      elif o in (DOWN1, DOWN2, DOWN3, DOWN4):
        s.append([DOWN1, p])
      elif o == Y0:
        s.append([Y0])
      elif o in (Y1, Y2, Y3, Y4):
        s.append([Y1, p])
      elif o == Z0:
        s.append([Z0])
      elif o in (Z1, Z2, Z3, Z4):
        s.append([Z1, p])
      elif o < FNT_NUM_0 + 64 or o in (FNT1, FNT2, FNT3, FNT4):
        s.append([FNT1, p])
      elif o in (XXX1, XXX2, XXX3, XXX4):
        q = fp.read(p)
        s.append([XXX1, q])
      elif o in (FNT_DEF1, FNT_DEF2, FNT_DEF3, FNT_DEF4):
        self.DefineFont(p, fp)
      elif o == NATIVE_FONT_DEF:
        self.DefineNativeFont(p, fp)
      elif o == GLYPHS:
        s.append([GLYPHS, self.GetGlyphs(fp)])
      elif o == TEXT_GLYPHS:
        s.append([TEXT_GLYPHS, self.GetTextGlyphs(fp)])
      elif o == DIR:
        s.append([DIR, p])
      elif o == BEGIN_REFLECT:
        s.append([BEGIN_REFLECT])
      elif o == END_REFLECT:
        s.append([END_REFLECT])
      elif o == PRE:
        warning('preamble command within a page!')
        break
      elif o in (POST, POST_POST):
        warning('postamble command %d!' % o)
        break
      else:
        warning('undefined command %d!' % o)
        break
    return s

  def Get1Arg(self, o, fp):
    if o < SET_CHAR_0 + 128:
      return o - SET_CHAR_0
    if o in (SET1, PUT1, FNT1, XXX1, FNT_DEF1, DIR):
      return GetByte(fp)
    if o in (SET2, PUT2, FNT2, XXX2, FNT_DEF2):
      return Get2Bytes(fp)
    if o in (SET3, PUT3, FNT3, XXX3, FNT_DEF3):
      return Get3Bytes(fp)
    if o in (RIGHT1, W1, X1, DOWN1, Y1, Z1):
      return SignedByte(fp)
    if o in (RIGHT2, W2, X2, DOWN2, Y2, Z2):
      return SignedPair(fp)
    if o in (RIGHT3, W3, X3, DOWN3, Y3, Z3):
      return SignedTrio(fp)
    if o in (SET4, SET_RULE, PUT4, PUT_RULE, RIGHT4, W4, X4, DOWN4, Y4, Z4, FNT4, XXX4, FNT_DEF4, NATIVE_FONT_DEF):
      return SignedQuad(fp)
    if o in (NOP, BOP, EOP, PUSH, POP, PRE, POST, POST_POST) or o > POST_POST:
      return 0
    if o in (W0, X0, Y0, Z0, BEGIN_REFLECT, END_REFLECT):
      return 0
    if o < FNT_NUM_0 + 64:
      return o - FNT_NUM_0

  def GetGlyphs(self, fp):
    width = SignedQuad(fp)
    length = Get2Bytes(fp)
    glyphs = {}
    for i in range(length):
      glyphs[i] = {}
      glyphs[i]["x"] = SignedQuad(fp)
      glyphs[i]["y"] = SignedQuad(fp)

    for i in range(length):
      glyphs[i]["id"] = Get2Bytes(fp)

    return (width, glyphs)

  def GetTextGlyphs(self, fp):
    length = Get2Bytes(fp)
    chars = []
    for i in range(length):
      chars.append(Get2Bytes(fp))
    width, glyphs = self.GetGlyphs(fp)

    return (chars, width, glyphs)

  def ReadGlyphs(self, val):
    import re
    glyphs = []
    w, g = val.split(" ", 1)
    for m in re.finditer(r"gid(?P<id>\d+?)\((?P<pos>.*?.)\)", g):
      gid = m.group("id")
      pos = m.group("pos")

      if "," in pos:
        x, y = pos.split(",")
      else:
        x, y = pos, "0sp"

      glyphs.append({"id": int(gid), 'x': self.ConvLen(x), 'y': self.ConvLen(y)})

    return (self.ConvLen(w), glyphs)

  def ReadTextGlyphs(self, val):
    _, text, glyphs = val.split(val[0])
    text = "'%s'" % text
    glyphs = glyphs.lstrip()
    chars = GetStr(text)
    w, glyphs = self.ReadGlyphs(glyphs)

    return (chars, w, glyphs)

  ##########################################################
  # Save: Internal Format -> DVI
  ##########################################################
  def Save(self, fn):
    fp = open(fn, 'wb')
    self.SaveToFile(fp)
    fp.close()

  def SaveToFile(self, fp):
    # WritePreamble
    fp.write(b''.join([bytes.fromhex('%02x' % PRE), PutByte(self.id), PutSignedQuad(self.numerator), PutSignedQuad(self.denominator), PutSignedQuad(self.mag), PutByte(len(self.comment)), self.comment.encode('utf8')]))
    # WriteFontDefinitions
    self.WriteFontDefinitions(fp)
    # WritePages
    if not self.pages:
      # dvistd0.pdf, Section A.1:
      # > A DVI file consists of a ``preamble,'' followed by a sequence of
      # > one or more "pages," followed by a ``postamble.''
      warning('one or more pages required!')
      self.pages.append({'count':[1,0,0,0,0,0,0,0,0,0], 'content':[]})
    stackdepth = 0; loc = -1
    for page in self.pages:
      w = x = y = z = 0; stack = []
      s = [bytes.fromhex('%02x' % BOP)]
      s.extend([PutSignedQuad(c) for c in page['count']])
      s.append(PutSignedQuad(loc))
      for cmd in page['content']:
        if cmd[0] == SET1:
          for o in cmd[1]:
            if o < 128: s.append(bytes.fromhex('%02x' % (SET_CHAR_0 + o)))
            else:       s.append(self.CmdPairU([SET1, o]))
        elif cmd[0] in (SET_RULE, PUT_RULE):
          s.append(bytes.fromhex('%02x' % cmd[0]) + PutSignedQuad(cmd[1][0]) + PutSignedQuad(cmd[1][1]))
        elif cmd[0] == PUT1:
          s.append(self.CmdPairU([PUT1, cmd[1][0]]))
        elif cmd[0] in (RIGHT1, DOWN1):
          s.append(self.CmdPair(cmd))
        elif cmd[0] in (W0, X0, Y0, Z0):
          s.append(bytes.fromhex('%02x' % cmd[0]))
        elif cmd[0] == PUSH:
          s.append(bytes.fromhex('%02x' % PUSH))
          stack.append((w, x, y, z))
          if len(stack) > stackdepth: stackdepth = len(stack)
        elif cmd[0] == POP:
          s.append(bytes.fromhex('%02x' % POP))
          w, x, y, z = stack.pop()
        elif cmd[0] == W1:
          w = cmd[1]; s.append(self.CmdPair(cmd))
        elif cmd[0] == X1:
          x = cmd[1]; s.append(self.CmdPair(cmd))
        elif cmd[0] == Y1:
          y = cmd[1]; s.append(self.CmdPair(cmd))
        elif cmd[0] == Z1:
          z = cmd[1]; s.append(self.CmdPair(cmd))
        elif cmd[0] == FNT1:
          if cmd[1] < 64: s.append(bytes.fromhex('%02x' % (FNT_NUM_0 + cmd[1])))
          else:           s.append(self.CmdPairU(cmd))
        elif cmd[0] == XXX1:
          if options.xxx_encoding == "none":
            l = len(cmd[1]) # leave encoding untouched
          else:
            cmd1 = cmd[1].encode(options.xxx_encoding)
            l = len(cmd1)
          if l < 256:
            s.append(bytes.fromhex('%02x' % XXX1) + bytes.fromhex('%02x' % l))
          else:
            s.append(bytes.fromhex('%02x' % XXX4) + PutSignedQuad(l))
          if options.xxx_encoding == "none":
            for o in cmd[1]:
              s.append(bytes.fromhex('%02x' % ord(o)))
          else:
              s.append(cmd1)
        elif cmd[0] == DIR:
          s.append(bytes.fromhex('%02x' % DIR) + bytes.fromhex('%02x' % cmd[1]))
        elif cmd[0] == BEGIN_REFLECT:
          s.append(bytes.fromhex('%02x' % BEGIN_REFLECT))
        elif cmd[0] == END_REFLECT:
          s.append(bytes.fromhex('%02x' % END_REFLECT))
        elif cmd[0] == GLYPHS:
          s.append(PutByte(GLYPHS))
          s.append(PutGlyphs(cmd[1], cmd[2]))
        elif cmd[0] == TEXT_GLYPHS:
          s.append(PutByte(TEXT_GLYPHS))
          s.append(PutTextGlyphs(cmd[1], cmd[2], cmd[3]))
        else:
          warning('invalid command %s!' % cmd[0])
      s.append(bytes.fromhex('%02x' % EOP))
      loc = fp.tell()
      fp.write(b''.join(s))
    # WritePostamble
    post_loc = fp.tell()
    fp.write(b''.join([bytes.fromhex('%02x' % POST), PutSignedQuad(loc), PutSignedQuad(self.numerator), PutSignedQuad(self.denominator), PutSignedQuad(self.mag), PutSignedQuad(self.max_v), PutSignedQuad(self.max_h), Put2Bytes(stackdepth), Put2Bytes(len(self.pages))]))
    # WriteFontDefinitions
    self.WriteFontDefinitions(fp)
    # WritePostPostamble
    fp.write(b''.join([bytes.fromhex('%02x' % POST_POST), PutSignedQuad(post_loc), PutByte(self.id_post), b'\xdf\xdf\xdf\xdf']))
    loc = fp.tell()
    while (loc % 4) != 0:
      fp.write(b'\xdf'); loc += 1

  def WriteFontDefinitions(self, fp):
    s = []
    for e in sorted(self.font_def.keys()):
      try:
        self.font_def[e]['native']
        flags = self.font_def[e]['flags']
        s.append(PutByte(NATIVE_FONT_DEF))
        s.append(PutSignedQuad(e))
        s.append(PutSignedQuad(self.font_def[e]['scaled_size']))
        s.append(Put2Bytes(flags))
        s.append(PutByte(len(self.font_def[e]['name'])))
        s.append(self.font_def[e]['name'].encode('utf8'))
        s.append(PutSignedQuad(self.font_def[e]['index']))
        if flags & XDV_FLAG_COLORED: s.append(PutSignedQuad(self.font_def[e]['color']))
        if flags & XDV_FLAG_EXTEND: s.append(PutSignedQuad(self.font_def[e]['extend']))
        if flags & XDV_FLAG_SLANT: s.append(PutSignedQuad(self.font_def[e]['slant']))
        if flags & XDV_FLAG_EMBOLDEN: s.append(PutSignedQuad(self.font_def[e]['embolden']))
      except KeyError:
        l, q = PutUnsigned(e)
        s.append(PutByte(FNT_DEF1 + l))
        s.append(q)
        s.append(PutSignedQuad(self.font_def[e]['checksum']))
        s.append(PutSignedQuad(self.font_def[e]['scaled_size']))
        s.append(PutSignedQuad(self.font_def[e]['design_size']))
        s.append(b'\x00')
        s.append(PutByte(len(self.font_def[e]['name'])))
        s.append(self.font_def[e]['name'].encode('utf8'))
    fp.write(b''.join(s))

  def CmdPairU(self, cmd):
    l, q = PutUnsigned(cmd[1])
    return bytes.fromhex('%02x' % (cmd[0] + l)) + q

  def CmdPair(self, cmd):
    l, q = PutSigned(cmd[1])
    return bytes.fromhex('%02x' % (cmd[0] + l)) + q

  ##########################################################
  # Parse: Text -> Internal Format
  ##########################################################
  def Parse(self, fn, encoding=''):
    fp = open(fn, 'r', encoding=encoding)
    s = fp.read()
    fp.close()
    self.ParseFromString(s, encoding=encoding)

  def ParseFromString(self, s, encoding=''):
    global GetStr, cur_font, cur_dsize, cur_ssize, subfont_idx
    if encoding == 'ascii': GetStr = GetStrASCII
    else:                   GetStr = GetStrUTF8
    self.Initialize()
    self.fnt_num = 0
    dir_used = 0
    for l in s.split('\n'):
      l = l.strip()
      if not l or l[0] == '%': continue
      try:
        key, val = l.split(':', 1)
        key = key.strip(); val = val.strip()
      except:
        if l[-1] == ']': v = l[:-1].split(' ')
        else: v = l.split(' ')
        if v[0] == "[page":
          self.cur_page = []
          count = [GetInt(c) for c in v[1:]]
          if len(count) < 10: count += ([0] * (10-len(count)))
          self.pages.append({'count':count, 'content':self.cur_page})
        continue
      # ParsePreamble
      if key == "id":
        self.id = GetInt(val)
        if not ValidID(self.id):
          warning('identification byte %d should be one of: %s!' % (self.id, DVI_IDS))
      elif key == "numerator":
        d = GetInt(val)
        if d <= 0:
          warning('non-positive numerator %d!' % d)
        else:
          self.numerator = d
          self.ComputeConversionFactors()
      elif key == "denominator":
        d = GetInt(val)
        if d <= 0:
          warning('non-positive denominator %d!' % d)
        else:
          self.denominator = d
          self.ComputeConversionFactors()
      elif key == "magnification":
        d = GetInt(val)
        if d <= 0:
          warning('non-positive magnification %d!' % d)
        else:
          self.mag = d
      elif key == "comment":
        self.comment = val[1:-1]
      # Parse Postamble
      elif key == "maxv":
        self.max_v = self.ConvLen(val)
      elif key == "maxh":
        self.max_h = self.ConvLen(val)
      elif key == "maxs":
        self.max_s = GetInt(val)
      elif key == "pages":
        self.total_pages = GetInt(val)
      # Parse Font Definitions
      elif key == "fntdef":
        self.font_def[self.fnt_num] = self.GetFntDef(val)
        self.fnt_num += 1
      # Parse Pages
      elif key == 'xxx':
        self.cur_page.append([XXX1, eval(val)])
      elif key == 'set':
        ol = GetStr(val)
        if is_subfont:
          subfont_idx = (ol[0] >> 8)
          self.AppendFNT1()
          nl = [ol[0] & 0xff]
          for o in ol[1:]:
            idx = (o >> 8)
            if idx != subfont_idx:
              self.cur_page.append([SET1, nl])
              subfont_idx = idx
              self.AppendFNT1()
              nl = [o & 0xff]
            else:
              nl.append(o & 0xff)
          self.cur_page.append([SET1, nl])
        else:
          self.cur_page.append([SET1, ol])
      elif key == 'put':
        ol = GetStr(val)
        if len(ol) != 1:
          warning('only one character is allowed for put!')
        self.cur_page.append([PUT1, ol])
      elif key == 'setrule':
        v = val.split(' ')
        if len(v) != 2:
          warning('two values are required for setrule!')
          continue
        self.cur_page.append([SET_RULE, [self.ConvLen(c) for c in v]])
      elif key == 'putrule':
        v = val.split(' ')
        if len(v) != 2:
          warning('two values are required for putrule!')
          continue
        self.cur_page.append([PUT_RULE, [self.ConvLen(c) for c in v]])
      elif key == 'fnt':
        f = self.GetFntDef(val)
        n = f['name']
        d = f['design_size']
        q = f['scaled_size']
        if n in subfont_list:
          is_subfont = True
          cur_font = n; cur_dsize = d; cur_ssize = q
        else:
          is_subfont = False
          try:
            e = list(self.font_def.keys())[list(self.font_def.values()).index(f)]
          except:
            e = self.fnt_num
            self.font_def[self.fnt_num] = f
            self.fnt_num += 1
          self.cur_page.append([FNT1, e])
      elif key == 'right':
        self.cur_page.append([RIGHT1, self.ConvLen(val)])
      elif key == 'down':
        self.cur_page.append([DOWN1, self.ConvLen(val)])
      elif key == 'w':
        self.cur_page.append([W1, self.ConvLen(val)])
      elif key == 'x':
        self.cur_page.append([X1, self.ConvLen(val)])
      elif key == 'y':
        self.cur_page.append([Y1, self.ConvLen(val)])
      elif key == 'z':
        self.cur_page.append([Z1, self.ConvLen(val)])
      elif key == 'push':
        self.cur_page.append([PUSH])
      elif key == 'pop':
        self.cur_page.append([POP])
      elif key == 'w0':
        self.cur_page.append([W0])
      elif key == 'x0':
        self.cur_page.append([X0])
      elif key == 'y0':
        self.cur_page.append([Y0])
      elif key == 'z0':
        self.cur_page.append([Z0])
      elif key == 'dir':
        self.cur_page.append([DIR, GetInt(val)])
        dir_used = 1
      elif key == 'begin_reflect':
        self.cur_page.append([BEGIN_REFLECT])
      elif key == 'end_reflect':
        self.cur_page.append([END_REFLECT])
      elif key == 'setglyphs':
        w, glyphs = self.ReadGlyphs(val)
        self.cur_page.append([GLYPHS, w, glyphs])
      elif key == 'settextglyphs':
        text, w, glyphs = self.ReadTextGlyphs(val)
        self.cur_page.append([TEXT_GLYPHS, text, w, glyphs])
      else:
        warning('invalid command %s!' % key)
    if self.id == 2 and dir_used == 1: # standard DVI with dir -> force pTeX/upTeX spec
      self.id_post = 3
    else:
      self.id_post = self.id

  def AppendFNT1(self):
    f = {'name':cur_font+"%02x"%subfont_idx, 'design_size':cur_dsize, 'scaled_size':cur_ssize, 'checksum':0}
    try:
      e = list(self.font_def.keys())[list(self.font_def.values()).index(f)]
    except:
      e = self.fnt_num
      self.font_def[e] = f
      self.fnt_num += 1
    self.cur_page.append([FNT1, e])

  ##########################################################
  # Dump: Internal Format -> Text
  ##########################################################
  def Dump(self, fn, tabsize=2, encoding=''):
    fp = open(fn, 'w', encoding=encoding)
    self.DumpToFile(fp, tabsize=tabsize, encoding=encoding)
    fp.close()

  def DumpToFile(self, fp, tabsize=2, encoding=''):
    global PutStr
    if   encoding == 'ascii':  PutStr = PutStrASCII
    elif encoding == 'latin1': PutStr = PutStrLatin1
    else:                      PutStr = PutStrUTF8
    # DumpPreamble
    fp.write("[preamble]\n")
    fp.write("id: %d\n" % self.id)
    fp.write("numerator: %d\n" % self.numerator)
    fp.write("denominator: %d\n" % self.denominator)
    fp.write("magnification: %d\n" % self.mag)
    fp.write("comment: %s\n" % repr(self.comment))
    # DumpPostamble
    fp.write("\n[postamble]\n")
    fp.write("maxv: %s\n" % self.byconv(self.max_v))
    fp.write("maxh: %s\n" % self.byconv(self.max_h))
    fp.write("maxs: %d\n" % self.max_s)
    fp.write("pages: %d\n" % self.total_pages)
    # DumpFontDefinitions
    fp.write("\n[font definitions]\n")
    for e in sorted(self.font_def.keys()):
      fp.write("fntdef: %s " % self.font_def[e]['name'])
      if self.font_def[e]['design_size'] != self.font_def[e]['scaled_size']:
        fp.write("(%s) " % self.byconv(self.font_def[e]['design_size']))
      fp.write("at %s\n" % self.byconv(self.font_def[e]['scaled_size']))
    # DumpPages
    if not self.pages:
      # dvistd0.pdf, Section A.1:
      # > A DVI file consists of a ``preamble,'' followed by a sequence of
      # > one or more "pages," followed by a ``postamble.''
      warning('one or more pages required!')
      self.pages.append({'count':[1,0,0,0,0,0,0,0,0,0], 'content':[]})
    for page in self.pages:
      fp.write("\n[page" + (" %d"*10 % tuple(page['count'])) + "]\n")
      indent = 0
      for cmd in page['content']:
        if cmd[0] == POP:
          indent -= tabsize
          fp.write("%spop:\n" % (' ' * indent))
          continue
        fp.write("%s" % (' ' * indent))
        if cmd[0] == PUSH:
          fp.write("push:\n")
          indent += tabsize
        elif cmd[0] == XXX1:
          if options.xxx_encoding == "none":
            fp.write("xxx: %s\n" % PutStrASCII(cmd[1])) # leave encoding untouched
          else:
            fp.write("xxx: '%s'\n" % cmd[1].decode(options.xxx_encoding))
        elif cmd[0] == DIR:
          fp.write("dir: %d\n" % cmd[1])
        elif cmd[0] == BEGIN_REFLECT:
          fp.write("begin_reflect:\n")
        elif cmd[0] == END_REFLECT:
          fp.write("end_reflect:\n")
        elif cmd[0] == SET_RULE:
          fp.write("setrule: %s %s\n" % (self.byconv(cmd[1][0]), self.byconv(cmd[1][1])))
        elif cmd[0] == PUT_RULE:
          fp.write("putrule: %s %s\n" % (self.byconv(cmd[1][0]), self.byconv(cmd[1][1])))
        elif cmd[0] == SET1:
          fp.write("set: %s\n" % PutStr(cmd[1]))
        elif cmd[0] == PUT1:
          fp.write("put: %s\n" % PutStr(cmd[1]))
        elif cmd[0] == FNT1:
          f = self.font_def[cmd[1]]['name']
          z = self.font_def[cmd[1]]['scaled_size']
          if IsFontChanged(f, z):
            fp.write("fnt: %s " % cur_font)
            if self.font_def[cmd[1]]['design_size'] != self.font_def[cmd[1]]['scaled_size']:
              fp.write("(%s) " % self.byconv(self.font_def[cmd[1]]['design_size']))
            fp.write("at %s\n" % self.byconv(cur_ssize))
        elif cmd[0] == GLYPHS:
          fp.write("setglyphs: %s\n" % self.DumpGlyphs(cmd[1][0], cmd[1][1]))
        elif cmd[0] == TEXT_GLYPHS:
          fp.write("settextglyphs: %s\n" % self.DumpTextGlyphs(cmd[1][0], cmd[1][1], cmd[1][2]))
        elif cmd[0] == RIGHT1:
          fp.write("right: %s\n" % self.byconv(cmd[1]))
        elif cmd[0] == DOWN1:
          fp.write("down: %s\n" % self.byconv(cmd[1]))
        elif cmd[0] == W1:
          fp.write("w: %s\n" % self.byconv(cmd[1]))
        elif cmd[0] == X1:
          fp.write("x: %s\n" % self.byconv(cmd[1]))
        elif cmd[0] == Y1:
          fp.write("y: %s\n" % self.byconv(cmd[1]))
        elif cmd[0] == Z1:
          fp.write("z: %s\n" % self.byconv(cmd[1]))
        elif cmd[0] == W0:
          fp.write("w0:\n")
        elif cmd[0] == X0:
          fp.write("x0:\n")
        elif cmd[0] == Y0:
          fp.write("y0:\n")
        elif cmd[0] == Z0:
          fp.write("z0:\n")

  def DumpGlyphs(self, w, g):
    yPresent = False
    for i in g:
      if g[i]["y"] != 0:
        yPresent = True

    glyphs = []
    for i in g:
      gid = "gid%s" % g[i]["id"]
      x = self.byconv(g[i]["x"])
      y = self.byconv(g[i]["y"])
      if yPresent:
        glyphs.append("%s(%s, %s)" % (gid, x, y))
      else:
        glyphs.append("%s(%s)" % (gid, x))

    return "%s %s" % (self.byconv(w), " ".join(glyphs))

  def DumpTextGlyphs(self, t, w, g):
    return "%s %s" % (PutStrUTF8(t), self.DumpGlyphs(w, g))

  ##########################################################
  # Misc Functions
  ##########################################################
  def ComputeConversionFactors(self):
    self.sp_conv = (self.numerator / 25400000.) * (473628672. / self.denominator)
    self.pt_conv = (self.numerator / 25400000.) * (7227. / self.denominator)
    self.bp_conv = (self.numerator / 254000.) * (72. / self.denominator)
    self.mm_conv = (self.numerator / 10000.) / self.denominator
    self.cm_conv = (self.numerator / 100000.) / self.denominator
    self.in_conv = (self.numerator / 254000.) * (1. / self.denominator)

  def ConvLen(self, s):
    try:    return int(s)
    except: pass
    try:    f = float(s[:-2])
    except: return 0
    m = s[-2:]
    if   m == "pt": return int(round(f / self.pt_conv))
    elif m == "in": return int(round(f / self.in_conv))
    elif m == "mm": return int(round(f / self.mm_conv))
    elif m == "cm": return int(round(f / self.cm_conv))
    elif m == "bp": return int(round(f / self.bp_conv))
    elif m == "sp": return int(round(f / self.sp_conv))
    else:
      try:    return int(round(f / self.pt_conv))
      except: return 0

  def GetFntDef(self, s):
    f = {}
    try:
      n, size = s.split('(', 1)
      d, q = size.split(')', 1)
    except:
      n, q = s.split(' ', 1)
    n = n.strip(); q = q.strip()
    if n.startswith('"') and n.endswith('"'):
      f['native'] = True
      n = n.strip('"')
      flags = 0
      color = 0
      extend = 0
      slant = 0
      embolden = 0
      try:
        name, ext = n.split(':')
      except:
        name, ext = n, ""

      try:
        name, index = name.split('[')
        index = index.split(']')[0]
      except:
        index = 0

      if ext:
        ext = ext.split(';')
        for opt in ext:
          try:
            key, value = opt.split('=')
          except:
            key, value = opt, ""
          if key == "color":
            flags |= XDV_FLAG_COLORED
            color = int(value, 16)
          if key == "vertical":
            flags |= XDV_FLAG_VERTICAL
          if key == "extend":
            flags |= XDV_FLAG_EXTEND
            extend = int(value)
          if key == "slant":
            flags |= XDV_FLAG_SLANT
            slant = int(value)
          if key == "embolden":
            flags |= XDV_FLAG_EMBOLDEN
            embolden = int(value)

      f['name'] = name
      f['index'] = int(index)
      f['flags'] = flags
      f['color'] = color
      f['extend'] = extend
      f['slant'] = slant
      f['embolden'] = embolden
    else:
      f['name'] = n

    if q[:2] == "at": q = q[2:]
    q = self.ConvLen(q.strip())
    try:    d = self.ConvLen(d.strip())
    except: d = q

    f['design_size'] = d
    f['scaled_size'] = q
    f['checksum'] = 0

    return f

  def by_sp_conv(self, a):
    v = self.sp_conv * a
    return "%dsp" % int(v)

  def by_pt_conv(self, a):
    v = self.pt_conv * a
    if v == int(v): return "%dpt" % int(v)
    else:           return "%fpt" % v

  def by_bp_conv(self, a):
    v = self.bp_conv * a
    if v == int(v): return "%dbp" % int(v)
    else:           return "%fbp" % v

  def by_mm_conv(self, a):
    v = self.mm_conv * a
    if v == int(v): return "%dmm" % int(v)
    else:           return "%fmm" % v

  def by_cm_conv(self, a):
    v = self.cm_conv * a
    if v == int(v): return "%dcm" % int(v)
    else:           return "%fcm" % v

  def by_in_conv(self, a):
    v = self.in_conv * a
    if v == int(v): return "%din" % int(v)
    else:           return "%fin" % v

############################################################
# Misc Functions for Main Routine
############################################################
def ProcessOptions():
  usage = """%prog [options] dvi_file|dvi_dump_file

DVIasm is a Python script to support changing or creating DVI files
via disassembling into text, editing, and then reassembling into
binary format. It is fully documented at
  https://tug.org/TUGboat/Articles/tb28-2/tb89cho.pdf
  http://ajt.ktug.kr/assets/2008/5/1/0201cho.pdf

Please report bugs to
  https://github.com/aminophen/dviasm"""

  version = """This is %prog-20240725

Copyright (C) 2007-2008 by Jin-Hwan Cho <chofchof@ktug.or.kr>
Copyright (C) 2011-2017 by Khaled Hosny <khaledhosny@eglug.org>
Copyright (C) 2019      by Arthur Reutenauer <arthur@reutenauer.eu>
Copyright (C) 2019-2024 by Hironobu Yamashita <h.y.acetaminophen@gmail.com>

This is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version."""

  parser = OptionParser(usage=usage, version=version)
  parser.add_option("-u", "--unit",
                    action="store", type="string", dest="unit",
                    metavar="STR",
                    help="unit (sp, pt, bp, mm, cm, in) [default=%default]")
  parser.add_option("-o", "--output",
                    action="store", type="string", dest="output",
                    metavar="FILE",
                    help="filename for output instead of stdout")
  parser.add_option("-e", "--encoding",
                    action="store", type="string", dest="encoding",
                    metavar="STR",
                    help="encoding for input/output [default=%default]")
  parser.add_option("-x", "--xxx-encoding",
                    action="store", type="string", dest="xxx_encoding",
                    metavar="STR",
                    help="encoding for interpreting xxx strings [default=%default]")
  parser.add_option("-t", "--tabsize",
                    action="store", type="int", dest="tabsize",
                    metavar="INT",
                    help="tab size for push/pop [default=%default]")
  parser.add_option("-p", "--ptex",
                    action="store_true", dest="ptex", default=False,
                    help="ISO-2022-JP-encoded DVI for Japanese pTeX")
  parser.add_option("-s", "--subfont",
                    action="append", type="string", dest="subfont",
                    metavar="STR",
                    help="the list of fonts with UCS2 subfont scheme (comma separated); disable internal subfont list if STR is empty")
  parser.set_defaults(unit='pt', encoding='utf8', xxx_encoding='none', tabsize=2)
  (options, args) = parser.parse_args()
  if not options.unit in ['sp', 'pt', 'bp', 'mm', 'cm', 'in']:
    parser.error("invalid unit name '%s'!" % options.unit)
  if options.tabsize < 0:
    parser.error("negative tabsize!")
  if not options.xxx_encoding in ['none', 'utf8', 'sjis', 'eucjp']:
    parser.error("invalid xxx-encoding '%s'!" % options.xxx_encoding)
  if not options.encoding in ['ascii', 'latin1', 'utf8', 'sjis', 'eucjp']:
    parser.error("invalid encoding '%s'!" % options.encoding)
  if options.ptex:
    global is_ptex
    is_ptex = True
    if not options.encoding in ['utf8', 'sjis', 'eucjp']:
      parser.error("invalid encoding '%s' for Japanese pTeX!" % options.encoding)
  if options.subfont:
    global subfont_list
    if not options.subfont[0]: # disable subfont
      subfont_list = []
    for l in options.subfont:
      subfont_list.extend([f.strip() for f in l.split(',')])
  if len(args) != 1:
    parser.error("try with the option --help!")
  return (options, args)

def IsDVI(fname):
  from os.path import splitext
  if splitext(fname)[1] not in ('.dvi', '.xdv'): return False
  try:
    fp = open(fname, 'rb')
    fp.seek(0)
    if GetByte(fp) != PRE: return False
    fp.seek(-4, 2)
    if GetByte(fp) != 223: return False
    fp.close()
  except:
    sys.stderr.write('Failed to read %s\n' % fname)
    return False
  return True

############################################################
# Main Routine
############################################################
if __name__ == '__main__':
  (options, args) = ProcessOptions()
  aDVI = DVI(unit=options.unit)
  if os.path.isfile(args[0]): fname = args[0]
  elif os.path.isfile(args[0] + '.xdv'): fname = args[0] + '.xdv'
  elif os.path.isfile(args[0] + '.dvi'): fname = args[0] + '.dvi'
  else:
    sys.stderr.write('File %s not found\n' % args[0])
    sys.exit(1)
  if IsDVI(fname): # dvi -> dump
    aDVI.Load(fname)
    if options.output: aDVI.Dump(options.output, tabsize=options.tabsize, encoding=options.encoding)
    else:              aDVI.DumpToFile(sys.stdout, tabsize=options.tabsize, encoding=options.encoding)
  else: # dump -> dvi
    aDVI.Parse(fname, encoding=options.encoding)
    if options.output: aDVI.Save(options.output)
    else:              aDVI.SaveToFile(sys.stdout.buffer)
