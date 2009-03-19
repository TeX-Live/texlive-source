/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: GrCharStream.cpp
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
    Implements the GrCharStream class.
----------------------------------------------------------------------------------------------*/

//:>********************************************************************************************
//:>	Include files
//:>********************************************************************************************
#include "Main.h"

#ifdef _MSC_VER
#pragma hdrstop
#endif
#undef THIS_FILE
DEFINE_THIS_FILE

//:End Ignore

//:>********************************************************************************************
//:>	Forward declarations
//:>********************************************************************************************

//:>********************************************************************************************
//:>	Local Constants and static variables
//:>********************************************************************************************

namespace gr
{

//:>********************************************************************************************
//:>	Methods
//:>********************************************************************************************

/*----------------------------------------------------------------------------------------------
	Constructor.
----------------------------------------------------------------------------------------------*/
GrCharStream::GrCharStream(ITextSource * pgts, int ichrMin, int ichrLim,
	bool fStartLine, bool fEndLine)
	:	m_pgts(pgts),
		m_ichrMin(ichrMin),
		m_ichrLim(ichrLim),
		m_ichrPos(ichrMin),
		m_fStartLine(fStartLine),
		m_fEndLine(fEndLine),
		m_cchrBackedUp(0),
		m_ichrRunMin(0),
		m_ichrRunLim(0),
		m_ichlRunOffset(kPosInfinity),
		m_ichrRunOffset(kPosInfinity)
{
  #if 0 // ??? surely m_cnRunMax and m_prgnRunText are undefined at this point
	if (m_cchlRunMax > -1)
		delete[] m_prgchlRunText;
  #endif
	m_cchlRunMax = -1;
	m_prgchlRunText = NULL;

	m_vislotNextChunkMap.clear();

	m_utf = m_pgts->utfEncodingForm();
}

/*----------------------------------------------------------------------------------------------
	Restart the stream when an error has a occurred and we need to fall back to dumb rendering.
----------------------------------------------------------------------------------------------*/
void GrCharStream::Restart()
{
	if (m_cchlRunMax > -1)
		delete[] m_prgchlRunText;

	m_ichrPos = m_ichrMin;
	m_cchrBackedUp = 0;
	m_ichrRunMin = 0;
	m_ichrRunLim = 0;
	m_ichlRunOffset = kPosInfinity;
	m_ichrRunOffset = kPosInfinity;

	m_cchlRunMax = -1;
	m_prgchlRunText = NULL;

	m_vislotNextChunkMap.clear();
}

/*----------------------------------------------------------------------------------------------
	Get the next character from the stream.

	@param pichwSegOffset - index of this character from the beginning of the segment
	@param pcchRaw - number of raw (UTF-16 or UTF-8) chars consumed to get one complete
						Unicode codepoint.
----------------------------------------------------------------------------------------------*/
int GrCharStream::NextGet(GrTableManager * ptman,
	GrFeatureValues * pfval, int * pichrSegOffset, int * pcchr)
{
	if (AtEnd())
	{
		Assert(m_ichrPos == m_ichrLim);
		return 0;
	}

	int m_ichrMinBackedUp = m_ichrMin - m_cchrBackedUp;

	if ((m_ichrRunOffset + m_ichrRunMin) >= m_ichrRunLim)
	{
		//	Get the next run of characters

		std::pair<toffset, toffset> pairRange = m_pgts->propertyRange(m_ichrPos);
		m_ichrRunMin = pairRange.first;
		m_ichrRunLim = pairRange.second;

		// The only time the run extends before what we want is on the first run.
		Assert((m_ichrRunMin == m_ichrPos) // m_ichwPos is beginning of run
			|| (m_ichrPos == m_ichrMinBackedUp));  // first run
		// Pretend like the run starts where we need it:
		m_ichrRunMin = max(m_ichrRunMin, m_ichrMinBackedUp);
		Assert(m_ichrRunMin == m_ichrPos);

		if (m_cchlRunMax != -1 && m_cchlRunMax < m_ichrRunLim - m_ichrRunMin)
		{
			//	Text buffer is too small: delete it and create a new one.
			delete[] m_prgchlRunText;
			m_cchlRunMax = -1;
		}

		if (m_cchlRunMax == -1)
		{
			// Note that combining surrogates or UTF-8 will always result in the same or fewer
			// number of characters needed in m_prgchlRunText, so the following is safe:
			m_cchlRunMax = m_ichrRunLim - m_ichrRunMin;
			m_prgchlRunText = new int[m_cchlRunMax];
		}

		utf32 * prgchlRunText32 = NULL;
		utf16 * prgchwRunText16 = NULL;
		utf8  * prgchsRunText8 = NULL;

		int cchGot;
		switch (m_utf)
		{
		case kutf8:
			prgchsRunText8 = new utf8[m_ichrRunLim - m_ichrRunMin];
			cchGot = m_pgts->fetch(m_ichrRunMin, m_ichrRunLim - m_ichrRunMin, prgchsRunText8);
			break;
		case kutf16:
			prgchwRunText16 = new utf16[m_ichrRunLim - m_ichrRunMin];
			cchGot = m_pgts->fetch(m_ichrRunMin, m_ichrRunLim - m_ichrRunMin, prgchwRunText16);
			break;
		default:
			Assert(false);
			// Assume UTF-32: fall through
		case kutf32:
			prgchlRunText32 = new utf32[m_ichrRunLim - m_ichrRunMin];
			cchGot = m_pgts->fetch(m_ichrRunMin, m_ichrRunLim - m_ichrRunMin, prgchlRunText32);
			break;
		}

		int ichlUtf32 = 0;
		for (int ichr = 0; ichr < m_ichrRunLim - m_ichrRunMin; ichr++)
		{
			if (ichr == m_ichrPos - m_ichrRunMin)
			{
				m_ichrRunOffset = ichr;
				m_ichlRunOffset = ichlUtf32;
			}
			Assert(m_ichrRunOffset != kPosInfinity || ichr < m_ichrPos - m_ichrRunMin);

			int cchrUsed;
			switch (m_utf)
			{
			case kutf8:
				m_prgchlRunText[ichlUtf32] = Utf8ToUtf32(prgchsRunText8 + ichr,
					m_ichrRunLim - m_ichrRunMin - ichr, &cchrUsed);
				break;
			case kutf16:
                m_prgchlRunText[ichlUtf32] = Utf16ToUtf32(prgchwRunText16 + ichr,
					m_ichrRunLim - m_ichrRunMin - ichr, &cchrUsed);
				break;
			default:
				Assert(false);
				// Assume UTF-32: fall through
			case kutf32:
				m_prgchlRunText[ichlUtf32] = prgchlRunText32[ichlUtf32];
				cchrUsed = 1;
				break;
			}

			m_vislotNextChunkMap.push_back(ichlUtf32);
			for (int i = 1; i < cchrUsed; i++)
				m_vislotNextChunkMap.push_back(-1);
			ichr += cchrUsed - 1;
			ichlUtf32++;
		}
		delete[] prgchlRunText32;
		delete[] prgchwRunText16;
		delete[] prgchsRunText8;

		while (m_ichrLim - m_ichrRunMin < signed(m_vislotNextChunkMap.size())
			&& m_vislotNextChunkMap[m_ichrLim - m_ichrRunMin] == -1)
		{
			// Invalid surrogate boundary; this is really a bug in the application program,
			// but adjust here to avoid a crash!
			m_ichrLim--;
		}

		SetUpFeatureValues(ptman, m_ichrPos);

		Assert(m_ichrRunOffset >= 0);
		Assert(m_ichrRunOffset < m_ichrRunLim - m_ichrRunMin);
		Assert(m_vislotNextChunkMap.size() == (unsigned) m_ichrRunLim - m_ichrMinBackedUp);
	}

	int chlRet = m_prgchlRunText[m_ichlRunOffset];
	*pfval = m_fvalRunFeats;
	*pichrSegOffset = m_ichrPos - m_ichrMin;	// offset from the official start of the segment
												// of the first 16-bit char
	int ichrPosPrev = m_ichrPos;
	++m_ichlRunOffset;
	do {
		++m_ichrPos;
		++m_ichrRunOffset;
	} while (m_ichrPos - m_ichrMinBackedUp < signed(m_vislotNextChunkMap.size())
		&& m_vislotNextChunkMap[m_ichrPos - m_ichrMinBackedUp] == -1);

	*pcchr = m_ichrPos - ichrPosPrev;		// number of raw chars consumed
	Assert(m_utf != kutf8 || *pcchr <= 6);
	Assert(m_utf != kutf16 || *pcchr <= 2);

	Assert(m_ichrPos == (m_ichrRunMin + m_ichrRunOffset));
	Assert(m_ichrPos <= m_ichrLim);

	/* KLUDGE for debugging
	if (m_ichrPos == 1)
		nRet = 0x1018;
	else if (m_ichrPos == 2)
		nRet = 0x1039;
	else if (m_ichrPos == 3)
		nRet = 0x101b;
	else if (m_ichrPos == 4)
		nRet = 0x1032;
	else if (m_ichrPos == 5)
		nRet = 0x1037;
	else if (m_ichrPos == 6)
		nRet = 0x1037;
	*/

	return chlRet;
}

/*----------------------------------------------------------------------------------------------
	Convert UTF-8 to UTF-32--a single character's worth. Also pass back the number of 8-bit
	items consumed.
----------------------------------------------------------------------------------------------*/
utf32 GrCharStream::Utf8ToUtf32(utf8 * prgchs, int cchs, int * pcchsUsed)
{
	if (cchs <= 0)
	{
		*pcchsUsed = 0;
		return 0;
	}

	long chlRet = DecodeUtf8(prgchs, cchs, pcchsUsed);
	Assert(chlRet > 0);
	if (chlRet == -1)
	{
		// Some error occurred. Just treat the UTF-8 as UTF-32.
		*pcchsUsed = 1;
		return (utf32)prgchs[0];
	}
	else
		return chlRet;
}

/*----------------------------------------------------------------------------------------------
	Convert UTF-16 to UTF-32--a single character's worth. Also pass back the number of 16-bit
	items consumed.
----------------------------------------------------------------------------------------------*/
utf32 GrCharStream::Utf16ToUtf32(utf16 * prgchw, int cchw, int * pcchwUsed)
{
	if (cchw <= 0)
	{
		*pcchwUsed = 0;
		return 0;
	}

	unsigned int nUtf32;
	bool fSurrogate = FromSurrogatePair(prgchw[0], ((cchw < 2) ? 0 : prgchw[1]), &nUtf32);
	*pcchwUsed = (fSurrogate) ? 2 : 1;
	return (int)nUtf32;

/*
	// For testing:
	int nCharRet;
	if (cchw >= 2 && prgchw[0] == '^' && prgchw[1] == 'a')
	{
		nCharRet = 'A';
		*pcchwUsed = 2;
	}
	else if (cchw >= 3 && prgchw[0] == '#' && prgchw[1] == '#' &&
		prgchw[2] == 'B')
	{
		nCharRet = 'b';
		*pcchwUsed = 3;
	}
	else if (cchw >= 1)
	{
		nCharRet = prgchw[0];
		*pcchwUsed = 1;
	}
	else
	{
		nCharRet = 0;
		*pcchwUsed = 0;
	}
	return nCharRet;
*/
}

/*----------------------------------------------------------------------------------------------
	Return true if the given position is at the boundary of a Unicode character; return false
	if it is between two parts of pair of surrogates.
----------------------------------------------------------------------------------------------*/
bool GrCharStream::AtUnicodeCharBoundary(ITextSource * pgts, int ichr)
{
	int cchr = pgts->getLength();
	if (ichr <= 0 || ichr >= cchr)
		return true;

	UtfType utf = pgts->utfEncodingForm();

	// Note that we never need more than a handful of characters.
	int ichrMinGet = max(0, ichr - 1);
	int ichrLimGet = ichr + 1;
	utf16 rgchwText16[3];
	utf8 rgchsText8[3];
	bool fRet;
	switch (utf)
	{
	case kutf8:
		pgts->fetch(ichrMinGet, ichrLimGet - ichrMinGet, rgchsText8);
		fRet = AtUnicodeCharBoundary(rgchsText8, ichrLimGet - ichrMinGet,
			ichr - ichrMinGet, utf);
		break;
	case kutf16:
        pgts->fetch(ichrMinGet, ichrLimGet - ichrMinGet, rgchwText16);
		fRet = AtUnicodeCharBoundary(rgchwText16, ichrLimGet - ichrMinGet,
			ichr - ichrMinGet, utf);
		break;
	default:
		Assert(false);
	case kutf32:
		fRet = true;
		break;
	}

	return fRet;
}

/*----------------------------------------------------------------------------------------------
	Return true if the given position is at the boundary of a Unicode character; return false
	if it is between two parts of pair of surrogates.
----------------------------------------------------------------------------------------------*/
bool GrCharStream::AtUnicodeCharBoundary(utf16 * prgchw, int cchw, int ichr, UtfType utf)
{
	Assert(ichr >= 0);
	Assert(ichr <= cchw);

	if (ichr == 0)
		return true;
	if (ichr >= cchw)
		return true;

	switch (utf)
	{
	case kutf16:
		{
			unsigned int nUtf32;
			bool fMiddleOfPair = FromSurrogatePair(prgchw[ichr - 1], prgchw[ichr], &nUtf32);
			return !fMiddleOfPair;
		}
	case kutf8:
		{
			// 16-bit buffer being treated like UTF-8.
			utf8 rgchs[2];
			rgchs[1] = (utf8)prgchw[ichr];
			return AtUnicodeCharBoundary(rgchs, 2, 1, kutf8);
		}
	case kutf32:
	default:
		return true;
	}

/*
	// temp stuff for debugging:
	if (ichr > 0 && cchw >= 1 && prgchw[ichr-1] == '^' && prgchw[ichr] == 'a')
	{
		return false;
	}
	if (ichr > 0 && cchw >= 2 && prgchw[ichr-1] == '#' && prgchw[ichr] == '#' &&
		prgchw[ichr+1] == 'B')
	{
		return false;
	}
	if (ichr > 1 && cchw >= 1 && prgchw[ichr-2] == '#' && prgchw[ichr-1] == '#' &&
		prgchw[ichr] == 'B')
	{
		return false;
	}

	return true;
*/
}

bool GrCharStream::AtUnicodeCharBoundary(utf8 * prgchs, int cchs, int ichs, UtfType utf)
{
	Assert(ichs >= 0);
	Assert(ichs <= cchs);

	Assert(utf == kutf8);

	if (ichs == 0)
		return true;
	if (ichs >= cchs)
		return true;

	if (cchs == 0)
		return true;

	// A bit pattern of 10xxxxxx indicates a continuation of a sequence
	// (11xxxxxx means the first byte of a sequence, 0xxxxxxx means a single character).

	utf8 chsTest = prgchs[ichs] & 0xC0;
	return (chsTest != 0x80);	
}

/*----------------------------------------------------------------------------------------------
	Convert the given pair of characters into a single 32-bit Unicode character. Return
	true if they are a legitimate surrogate pair. If not, just return the first of the
	two 16-bit characters.
----------------------------------------------------------------------------------------------*/
bool GrCharStream::FromSurrogatePair(utf16 chwIn1, utf16 chwIn2, unsigned int * pchlOut)
{
	if ((chwIn1 < kzUtf16HighFirst) || (chwIn1 > kzUtf16HighLast)
		|| (chwIn2 < kzUtf16LowFirst) || (chwIn2 > kzUtf16LowLast))
	{
		// Not a surrogate
		*pchlOut = (unsigned int)chwIn1;
		return false;
	}
	else
	{
		*pchlOut = ((chwIn1 - kzUtf16HighFirst) << kzUtf16Shift) + chwIn2 + kzUtf16Inc;
		return true;
	}
}

/*----------------------------------------------------------------------------------------------
	Decode 1-6 bytes in the character string from UTF-8 format to Unicode (UCS-4).
	As a side-effect, cbOut is set to the number of UTF-8 bytes consumed.

	@param rgchUtf8 Pointer to a a character array containing UTF-8 data.
	@param cchUtf8 Number of characters in the array.
	@param cbOut Reference to an integer for holding the number of input (8-bit) characters
					consumed to produce the single output Unicode character.

	@return A single Unicode (UCS-4) character.  If an error occurs, return -1.
----------------------------------------------------------------------------------------------*/
long GrCharStream::DecodeUtf8(const utf8 * rgchUtf8, int cchUtf8, int * pcbOut)
{
	// check for valid input
	AssertArray(rgchUtf8, cchUtf8);
	if ((cchUtf8 == 0) || (rgchUtf8[0] == '\0'))
	{
		*pcbOut = (cchUtf8) ? 1 : 0;
		return 0;
	}
	//
	// decode the first byte of the UTF-8 sequence
	//
	long lnUnicode;
	int cbExtra;
	int chsUtf8 = *rgchUtf8++ & 0xFF;
	if (chsUtf8 >= kzUtf8Flag6)					// 0xFC
	{
		lnUnicode = chsUtf8 & kzUtf8Mask6;
		cbExtra = 5;
	}
	else if (chsUtf8 >= kzUtf8Flag5)			// 0xF8
	{
		lnUnicode = chsUtf8 & kzUtf8Mask5;
		cbExtra = 4;
	}
	else if (chsUtf8 >= kzUtf8Flag4)			// 0xF0
	{
		lnUnicode = chsUtf8 & kzUtf8Mask4;
		cbExtra = 3;
	}
	else if (chsUtf8 >= kzUtf8Flag3)			// 0xE0
	{
		lnUnicode = chsUtf8 & kzUtf8Mask3;
		cbExtra = 2;
	}
	else if (chsUtf8 >= kzUtf8Flag2)			// 0xC0
	{
		lnUnicode = chsUtf8 & kzUtf8Mask2;
		cbExtra = 1;
	}
	else										// 0x00
	{
		lnUnicode = chsUtf8;
		cbExtra = 0;
	}
	if (cbExtra >= cchUtf8)
	{
		return -1;
	}

	switch (cbExtra)
	{
	case 5:
		lnUnicode <<= kzUtf8ByteShift;
		chsUtf8 = *rgchUtf8++ & 0xFF;
		if ((chsUtf8 & ~kzByteMask) != 0x80)
			return -1;
		lnUnicode += chsUtf8 & kzByteMask;
		// fall through
	case 4:
		lnUnicode <<= kzUtf8ByteShift;
		chsUtf8 = *rgchUtf8++ & 0xFF;
		if ((chsUtf8 & ~kzByteMask) != 0x80)
			return -1;
		lnUnicode += chsUtf8 & kzByteMask;
		// fall through
	case 3:
		lnUnicode <<= kzUtf8ByteShift;
		chsUtf8 = *rgchUtf8++ & 0xFF;
		if ((chsUtf8 & ~kzByteMask) != 0x80)
			return -1;
		lnUnicode += chsUtf8 & kzByteMask;
		// fall through
	case 2:
		lnUnicode <<= kzUtf8ByteShift;
		chsUtf8 = *rgchUtf8++ & 0xFF;
		if ((chsUtf8 & ~kzByteMask) != 0x80)
			return -1;
		lnUnicode += chsUtf8 & kzByteMask;
		// fall through
	case 1:
		lnUnicode <<= kzUtf8ByteShift;
		chsUtf8 = *rgchUtf8++ & 0xFF;
		if ((chsUtf8 & ~kzByteMask) != 0x80)
			return -1;
		lnUnicode += chsUtf8 & kzByteMask;
		break;
	case 0:
		// already handled
		break;
	default:
		Assert(false);
	}
	if ((unsigned long)lnUnicode > kzUnicodeMax)
	{
		return -1;
	}
	*pcbOut = cbExtra + 1;
	return lnUnicode;
}

/*----------------------------------------------------------------------------------------------
	Get the current feature values and character properties from the stream.
----------------------------------------------------------------------------------------------*/
void GrCharStream::CurrentFeatures(GrTableManager * ptman, GrFeatureValues * pfval)
{
	if (m_ichrRunOffset == kPosInfinity)
	{
		//	Not yet set up.
		if (AtEnd())
		{
			//	Empty stream; no valid values.
			Assert(false);
			return;
		}

		// Get the settings from the first character.
		int ichrSegOffset;
		int ichrPosSave = m_ichrPos;
		int ichrRunOffsetSave = m_ichrRunOffset;
		int ichlRunOffsetSave = m_ichlRunOffset;
		int cMapSize = m_vislotNextChunkMap.size();
		int cchrConsumed;
		NextGet(ptman, pfval, &ichrSegOffset, &cchrConsumed);
		// Put the character back.
		m_ichrPos = ichrPosSave;
		m_ichrRunOffset = ichrRunOffsetSave;
		m_ichlRunOffset = ichlRunOffsetSave;
		while (signed(m_vislotNextChunkMap.size()) > cMapSize)
			m_vislotNextChunkMap.pop_back();
	}
	else
	{
		*pfval = m_fvalRunFeats;
		//*ppchrp = &m_chrpRun;
	}
}

/*----------------------------------------------------------------------------------------------
	Read the feature settings from the text properties.

	Eventually, handle style index here?
----------------------------------------------------------------------------------------------*/
void GrCharStream::SetUpFeatureValues(GrTableManager * ptman, int ichr)
{
	//	Set all the features to their default values.
	for (int i = 0; i < kMaxFeatures; ++i)
	{
		m_fvalRunFeats.m_rgnFValues[i] = ptman->DefaultForFeatureAt(i);
	}
	m_fvalRunFeats.m_nStyleIndex = 0;

	// Add in the defaults for the language of the text.
	std::vector<featid> vnFeats;
	std::vector<int> vnValues;
	isocode lgcode = m_pgts->getLanguage(ichr);
	ptman->DefaultsForLanguage(lgcode, vnFeats, vnValues);
	size_t ifeatLp;
	int ifeat;
	for (ifeatLp = 0; ifeatLp < vnFeats.size(); ifeatLp++)
	{
		ptman->FeatureWithID(vnFeats[ifeatLp], &ifeat);
		m_fvalRunFeats.m_rgnFValues[ifeat] = vnValues[ifeatLp];
	}

	// Add in the explicit feature settings.
	FeatureSetting rgfset[kMaxFeatures];
	int cfeat = m_pgts->getFontFeatures(ichr, rgfset);

	for (ifeatLp = 0; ifeatLp < (size_t)cfeat; ifeatLp++)
	{
		ptman->FeatureWithID(rgfset[ifeatLp].id, &ifeat);
		if (ifeat >= 0)
			m_fvalRunFeats.m_rgnFValues[ifeat] = rgfset[ifeatLp].value;
	}
}

} // namespace gr
