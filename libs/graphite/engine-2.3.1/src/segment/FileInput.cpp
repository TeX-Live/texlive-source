/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: FileInput.cpp
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
    Contains the functions for reading from a TT file.  (These are functions, not methods
	associated with a class.)
----------------------------------------------------------------------------------------------*/

//:>********************************************************************************************
//:>	Include files
//:>********************************************************************************************

#include "GrCommon.h"
#include "GrData.h"
#ifndef _WIN32
#include "GrMstypes.h"
#endif
#include "GrDebug.h"
#include <fstream>
#include <iostream>
#include <string>
#include "FileInput.h"
#include "GrResult.h"

//#ifndef _MSC_VER
//#include "config.h"
//#endif

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
//:>	Methods of GrBufferIStream
//:>********************************************************************************************

/*----------------------------------------------------------------------------------------------
	Constructor.
----------------------------------------------------------------------------------------------*/
GrBufferIStream::GrBufferIStream()
{
	m_pbStart = NULL;
	m_pbNext = NULL;
	m_pbLim = NULL;
}

/*----------------------------------------------------------------------------------------------
	Destructor.
----------------------------------------------------------------------------------------------*/
GrBufferIStream::~GrBufferIStream()
{
	Close();
}

/*----------------------------------------------------------------------------------------------
	Initialize the stream.
----------------------------------------------------------------------------------------------*/
#ifdef GR_FW
bool GrBufferIStream::Open(std::wstring stuFileName, int kMode)
#else
bool GrBufferIStream::Open(const char * pcFileName, std::ios::openmode kMode)
#endif
{
	Assert(false); // use OpenBuffer
	return false;
}

/*----------------------------------------------------------------------------------------------
	Initialize the stream to a buffer.
----------------------------------------------------------------------------------------------*/
bool GrBufferIStream::OpenBuffer(const byte * pbBuffer, int cb)
{
	Assert(m_pbStart == NULL);
	Assert(m_pbNext == NULL);
	Assert(m_pbLim == NULL);

	m_pbStart = pbBuffer;
	m_pbNext = pbBuffer;
	if (cb > 0)
		m_pbLim = m_pbStart + cb;
	// otherwise we don't know the length

	return true;
}

/*----------------------------------------------------------------------------------------------
	Close the stream.
----------------------------------------------------------------------------------------------*/
void GrBufferIStream::Close()
{
	m_pbStart = NULL;
	m_pbNext = NULL;
	m_pbLim = NULL;
}

/*----------------------------------------------------------------------------------------------
	Read a byte from the stream.
----------------------------------------------------------------------------------------------*/
byte GrBufferIStream::ReadByteFromFont()
{
	byte bInput = *m_pbNext;
	m_pbNext += isizeof(byte);
	if (m_pbLim && m_pbNext > m_pbLim)
		THROW(kresReadFault);
	return bInput;
}

/*----------------------------------------------------------------------------------------------
	Read a short (signed 16-bit) word from the stream. Switch the bytes from big-endian 
	to little-endian format.
----------------------------------------------------------------------------------------------*/
short GrBufferIStream::ReadShortFromFont()
{
	short snInput = *(const short *)m_pbNext;
	m_pbNext += isizeof(short);
	if (m_pbLim && m_pbNext > m_pbLim)
		THROW(kresReadFault);
	snInput = lsbf(snInput);
	return snInput;
}

/*----------------------------------------------------------------------------------------------
	Read a wide character (unsigned 16-bit word) from the stream.
	Switch the bytes from big-endian to little-endian format.
----------------------------------------------------------------------------------------------*/
utf16 GrBufferIStream::ReadUShortFromFont()
{
	utf16 chwInput = *(const utf16 *)m_pbNext;
	m_pbNext += isizeof(utf16);
	if (m_pbLim && m_pbNext > m_pbLim)
		THROW(kresReadFault);
	chwInput = lsbf(chwInput);
	return chwInput;
}

/*----------------------------------------------------------------------------------------------
	Read a standard (32-bit) word from the stream. Switch the bytes from big-endian 
	to little-endian format.
----------------------------------------------------------------------------------------------*/
int GrBufferIStream::ReadIntFromFont()
{
	int nInput = *(const int *)m_pbNext;
	m_pbNext += isizeof(int);
	if (m_pbLim && m_pbNext > m_pbLim)
		THROW(kresReadFault);
	nInput = lsbf(nInput);
	return nInput;
}

/*----------------------------------------------------------------------------------------------
	Read a block of data from the stream. DON'T switch the bytes from big-endian 
	to little-endian format.
----------------------------------------------------------------------------------------------*/
void GrBufferIStream::ReadBlockFromFont(void * pvInput, int cb)
{
	std::copy(m_pbNext, m_pbNext + cb, reinterpret_cast<byte*>(pvInput));
	m_pbNext += cb;
	if (m_pbLim && m_pbNext > m_pbLim)
		THROW(kresReadFault);
}

/*----------------------------------------------------------------------------------------------
	Get the absolute position of the font-file stream (relative to the beginning of
	the file). For buffers, we just return the byte position in the buffer.
----------------------------------------------------------------------------------------------*/
void GrBufferIStream::GetPositionInFont(long * plPos)
{
	*plPos = (m_pbNext - m_pbStart);
}

/*----------------------------------------------------------------------------------------------
	Set the position of the font-file stream to the given absolute position (relative
	to the beginning of the file). For buffers, assume the position is relative to the
	beginning of the buffer.
----------------------------------------------------------------------------------------------*/
void GrBufferIStream::SetPositionInFont(long lPos)
{
	m_pbNext = m_pbStart + lPos;
	if (m_pbLim && m_pbNext > m_pbLim)
		THROW(kresReadFault);
}


//:>********************************************************************************************
//:>	Swap byte order.
//:>********************************************************************************************
int swapb(int nArg)
{
#if WORDS_BIGENDIAN
return nArg;
#else
	int b1, b2, b3, b4;
	b1 = ((nArg & 0xFF000000) >> 24)     & 0x000000FF;	// remove sign extension
	b2 = ((nArg & 0x00FF0000) >> 8);  // & 0x0000FF00;
	b3 = ((nArg & 0x0000FF00) << 8);  // & 0x00FF0000;
	b4 = ((nArg & 0x000000FF) << 24); // & 0xFF000000;
	int nRet = b1 | b2 | b3 | b4;
	return nRet;
#endif
}

unsigned int swapb(unsigned int nArg)
{
#if WORDS_BIGENDIAN
return nArg;
#else
	int b1, b2, b3, b4;
	b1 = ((nArg & 0xFF000000) >> 24)     & 0x000000FF;	// remove sign extension
	b2 = ((nArg & 0x00FF0000) >> 8);  // & 0x0000FF00;
	b3 = ((nArg & 0x0000FF00) << 8);  // & 0x00FF0000;
	b4 = ((nArg & 0x000000FF) << 24); // & 0xFF000000;
	int nRet = b1 | b2 | b3 | b4;
	return nRet;
#endif
}

utf16 swapb(utf16 chwArg)
{
#if WORDS_BIGENDIAN
return chwArg;
#else
	utf16 b1, b2;
	b1 = ((chwArg & 0xFF00) >> 8)     & 0x00FF;		// remove sign extension
	b2 = ((chwArg & 0x00FF) << 8); // & 0xFF00;
	utf16 chwRet = b1 | b2;
	return chwRet;
#endif
}

short swapb(short snArg)
{
#if WORDS_BIGENDIAN
return snArg;
#else
	short b1, b2;
	b1 = ((snArg & 0xFF00) >> 8)     & 0x00FF;		// remove sign extension
	b2 = ((snArg & 0x00FF) << 8); // & 0xFF00;
	short snRet = b1 | b2;
	return snRet;
#endif
}

} //namespace gr

