/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: FileInput.h
Responsibility: Sharon Correll
Last reviewed: not yet

Description:
    Defines utility functions for reading from a font file.
----------------------------------------------------------------------------------------------*/

#ifdef _MSC_VER
#pragma once
#endif
#ifndef FILEINPUT_INCLUDED
#define FILEINPUT_INCLUDED

//:End Ignore

namespace gr
{

int swapb(int nArg);
unsigned int swapb(unsigned int nArg);
utf16 swapb(utf16 chwArg);
short swapb(short snArg);

//	Most significant byte first (converting from least-sig-first):
inline int msbf(int nArg)					{ return swapb(nArg); }
inline unsigned int msbf(unsigned int nArg)	{ return swapb(nArg); }
inline utf16 msbf(utf16 chwArg)				{ return swapb(chwArg); }
inline short msbf(short chwArg)				{ return swapb(chwArg); }

//	Least significant byte first (converting from most-sig first):
inline int lsbf(int nArg)					{ return swapb(nArg); }
inline unsigned int lsbf(unsigned int nArg)	{ return swapb(nArg); };
inline utf16 lsbf(utf16 chwArg)				{ return swapb(chwArg); }
inline short lsbf(short chwArg)				{ return swapb(chwArg); }

class GrIStream
{
public:
	virtual void Close() = 0;

	virtual byte ReadByteFromFont() = 0;
	virtual short ReadShortFromFont() = 0;
	virtual utf16 ReadUShortFromFont() = 0;
	virtual int ReadIntFromFont() = 0;
	virtual void ReadBlockFromFont(void * pvInput, int cb) = 0;

	virtual void GetPositionInFont(long * plPos) = 0;
	virtual void SetPositionInFont(long lPos) = 0;

	virtual bool OpenBuffer(const byte * pbBuffer, int cb) = 0;
	virtual void CloseBuffer() = 0;

protected:
    virtual ~GrIStream() {}
};


/*----------------------------------------------------------------------------------------------
	A stream that reads from a buffer rather than a file.
----------------------------------------------------------------------------------------------*/
class GrBufferIStream : public GrIStream
{
public:
	GrBufferIStream();
	~GrBufferIStream();

	#ifdef GR_FW
	virtual bool Open(std::wstring stuFileName, std::ios::openmode kMode);
	#else
	virtual bool Open(const char * pcFileName, std::ios::openmode kMode);
	#endif
	virtual void Close();

	virtual byte ReadByteFromFont();
	virtual short ReadShortFromFont();
	virtual utf16 ReadUShortFromFont();
	virtual int ReadIntFromFont();
	virtual void ReadBlockFromFont(void * pvInput, int cb);

	virtual void GetPositionInFont(long * plPos);
	virtual void SetPositionInFont(long lPos);

	virtual bool OpenBuffer(const byte * pbBuffer, int cb);
	virtual void CloseBuffer()
	{
		Close();
	}

protected:
	const byte * m_pbStart;
	const byte * m_pbNext;
	const byte * m_pbLim;
};

} // namespace gr

#endif // !FILEINPUT_INCLUDED
