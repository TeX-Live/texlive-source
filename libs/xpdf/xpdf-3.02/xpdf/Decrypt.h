//========================================================================
//
// Decrypt.h
//
// Copyright 1996-2003 Glyph & Cog, LLC
//
//========================================================================

/* ------------------------------------------------------------------------
* Changed by Martin Schröder <martin@pdftex.org>
* $Id: Decrypt.h 421 2008-04-26 21:59:55Z oneiros $
* Changelog:
* ------------------------------------------------------------------------
* r320 | oneiros | 2007-12-18 13:40:01 +0100 (Di, 18 Dez 2007) | 2 lines
* 
* Import libpng 1.2.24 into stable
* 
* ------------------------------------------------------------------------
* r151 | ms | 2007-06-25 18:53:17 +0200 (Mo, 25 Jun 2007) | 3 lines
* 
* Merging xpdf 3.02 from HEAD into stable
* svn merge -r149:150 --dry-run svn+ssh://svn/srv/svn/repos/pdftex/trunk/source/src/libs/xpdf .
* 
* ------------------------------------------------------------------------
* r38 | ms | 2005-08-21 14:00:00 +0200 (So, 21 Aug 2005) | 2 lines
* 
* 1.30.1
* 
* ------------------------------------------------------------------------
* r6 | ms | 2003-10-06 14:01:00 +0200 (Mo, 06 Okt 2003) | 2 lines
* 
* released v1.11b
* 
* ------------------------------------------------------------------------
* r4 | ms | 2003-10-05 14:00:00 +0200 (So, 05 Okt 2003) | 2 lines
* 
* Moved sources to src
* 
* ------------------------------------------------------------------------
* r1 | ms | 2003-08-02 14:00:00 +0200 (Sa, 02 Aug 2003) | 1 line
* 
* 1.11a
* ------------------------------------------------------------------------ */

#ifndef DECRYPT_H
#define DECRYPT_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include "gtypes.h"
#include "GString.h"
#include "Object.h"
#include "Stream.h"

//------------------------------------------------------------------------
// Decrypt
//------------------------------------------------------------------------

class Decrypt {
public:

  // Generate a file key.  The <fileKey> buffer must have space for at
  // least 16 bytes.  Checks <ownerPassword> and then <userPassword>
  // and returns true if either is correct.  Sets <ownerPasswordOk> if
  // the owner password was correct.  Either or both of the passwords
  // may be NULL, which is treated as an empty string.
  static GBool makeFileKey(int encVersion, int encRevision, int keyLength,
			   GString *ownerKey, GString *userKey,
			   int permissions, GString *fileID,
			   GString *ownerPassword, GString *userPassword,
			   Guchar *fileKey, GBool encryptMetadata,
			   GBool *ownerPasswordOk);

private:

  static GBool makeFileKey2(int encVersion, int encRevision, int keyLength,
			    GString *ownerKey, GString *userKey,
			    int permissions, GString *fileID,
			    GString *userPassword, Guchar *fileKey,
			    GBool encryptMetadata);
};

//------------------------------------------------------------------------
// DecryptStream
//------------------------------------------------------------------------

struct DecryptRC4State {
  Guchar state[256];
  Guchar x, y;
  int buf;
};

struct DecryptAESState {
  Guint w[44];
  Guchar state[16];
  Guchar cbc[16];
  Guchar buf[16];
  int bufIdx;
};

class DecryptStream: public FilterStream {
public:

  DecryptStream(Stream *strA, Guchar *fileKey,
		CryptAlgorithm algoA, int keyLength,
		int objNum, int objGen);
  virtual ~DecryptStream();
  virtual StreamKind getKind() { return strCrypt; }
  virtual void reset();
  virtual int getChar();
  virtual int lookChar();
  virtual GBool isBinary(GBool last);
  virtual Stream *getUndecodedStream() { return this; }

private:

  CryptAlgorithm algo;
  int objKeyLength;
  Guchar objKey[16 + 9];

  union {
    DecryptRC4State rc4;
    DecryptAESState aes;
  } state;
};

#endif
