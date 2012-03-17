/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 2005 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: SegmentAux.h
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
	Auxiliary classes for the Segment class:
	- GlyphInfo
	- GlyphIterator
	- LayoutEnvironment
----------------------------------------------------------------------------------------------*/
#ifdef _MSC_VER
#pragma once
#endif
#ifndef SEGMENTAUX_INCLUDED
#define SEGMENTAUX_INCLUDED

//:End Ignore

namespace gr
{

class Segment;
class GrSlotOutput;
class IGrJustifier;
class GlyphInfo;
class GlyphIterator;
class GlyphSetIterator;


/*----------------------------------------------------------------------------------------------
	The GlyphInfo class provides access to details about a single glyph within a segment.
----------------------------------------------------------------------------------------------*/
class GlyphInfo		// hungarian: ginf
{
	friend class Segment;

public:
	// Default constructor:
	GlyphInfo()
	{
		m_pseg = NULL;
		m_pslout = NULL;
		m_islout = kInvalid;
	}

	gid16 glyphID();
	gid16 pseudoGlyphID();

	// Index of this glyph in the logical sequence; zero-based.
	size_t logicalIndex();

	// glyph's position relative to the left of the segment
	float origin();
	float advanceWidth();		// logical units
	float advanceHeight();	// logical units; zero for horizontal fonts
	float yOffset();
	gr::Rect bb();				// logical units
	bool isSpace();

	// first char associated with this glyph, relative to start of seg
	toffset firstChar();
	// last char associated with this glyph, relative to start of seg
	toffset lastChar();

	// Unicode bidi value
	unsigned int directionality();
	// Embedding depth
	unsigned int directionLevel();
	bool insertBefore();
	int	breakweight();

	bool isAttached() const throw();
	gr::GlyphIterator attachedClusterBase() const throw();
	float attachedClusterAdvance() const throw();
	std::pair<gr::GlyphSetIterator, gr::GlyphSetIterator> attachedClusterGlyphs() const;

	float maxStretch(size_t level);
	float maxShrink(size_t level);
	float stretchStep(size_t level);
	byte justWeight(size_t level);
	float justWidth(size_t level);
	float measureStartOfLine();
	float measureEndOfLine();

	size_t numberOfComponents();
	gr::Rect componentBox(size_t icomp);
	toffset componentFirstChar(size_t icomp);
	toffset componentLastChar(size_t icomp);

	bool erroneous();

	const Segment & segment() const throw();
	Segment & segment() throw();

protected:
	Segment * m_pseg;
	GrSlotOutput * m_pslout;
	int m_islout;		// signed, so it can return kInvalid
};

inline Segment & GlyphInfo::segment() throw () {
	return * m_pseg;
}

inline const Segment & GlyphInfo::segment() const throw () {
	return * m_pseg;
}


/*----------------------------------------------------------------------------------------------
	The GlyphIterator class allows the Graphite client to iterate over a contiguous
	range of glyphs for the segment.
----------------------------------------------------------------------------------------------*/
class GlyphIterator
: public std::iterator<std::random_access_iterator_tag, gr::GlyphInfo>
{
	friend class GlyphInfo;
 	friend class Segment;

	// Pointers into the glyph info store
	GlyphInfo * m_pginf;

protected:
	// Constructor
	GlyphIterator(Segment & seg, size_t iginf);

public:
	// Default constructor.
	GlyphIterator() throw (): m_pginf(0) {}

	explicit GlyphIterator(const GlyphSetIterator &);

	// Forward iterator requirements.
	reference		operator*() const		{ assert(m_pginf != 0); return *m_pginf; }
	pointer			operator->() const		{ return m_pginf; }
	GlyphIterator &	operator++() throw()	{ ++m_pginf; return *this; }
	GlyphIterator	operator++(int) throw()	{ GlyphIterator tmp = *this; operator++(); return tmp; }

	// Bidirectional iterator requirements
	GlyphIterator &	operator--() throw()	{ --m_pginf; return *this; }
	GlyphIterator	operator--(int) throw()	{ GlyphIterator tmp = *this; operator--(); return tmp; }

	// Random access iterator requirements
	reference		operator[](difference_type n) const			{ return m_pginf[n]; }
	GlyphIterator &	operator+=(difference_type n) throw()		{ m_pginf += n; return *this; }
	GlyphIterator	operator+(difference_type n) const throw()	{ GlyphIterator r = *this; return r += n; }
	GlyphIterator &	operator-=(difference_type n) throw()		{ m_pginf -= n; return *this; }
	GlyphIterator	operator-(difference_type n) const throw()	{ GlyphIterator r = *this; return r += -n; }
 
	// Relational operators.
  	// Forward iterator requirements
	bool operator==(const GlyphIterator & rhs) const throw()	{ return m_pginf == rhs.m_pginf; }
	bool operator!=(const GlyphIterator & rhs) const throw()	{ return !(*this == rhs); }

	// Random access iterator requirements
	bool operator<(const GlyphIterator & rhs) const throw()	{ return m_pginf < rhs.m_pginf; }
	bool operator>(const GlyphIterator & rhs) const throw()	{ return m_pginf > rhs.m_pginf; }
	bool operator<=(const GlyphIterator & rhs) const throw() { return !(*this > rhs); }
	bool operator>=(const GlyphIterator & rhs) const throw() { return !(*this < rhs); }

	difference_type operator-(const GlyphIterator & rhs) const throw() { return m_pginf - rhs.m_pginf; }
};

inline GlyphIterator operator+(const GlyphIterator::difference_type n, const GlyphIterator & rhs)
{
	return rhs + n;
}


/*----------------------------------------------------------------------------------------------
	This reference-counted vector is used as the basis of GlyphSetIterator. This is because
	multiple iterators are based on the same vector, so it needs to hang around appropriately.
----------------------------------------------------------------------------------------------*/
class RcVector
{
	friend class GlyphSetIterator;

public:
	RcVector(std::vector<int> & vn)
	{
		m_vn = vn;
		m_cref = 0;
	}

protected:
	void IncRefCount() const
	{
		m_cref++;
	}
	void DecRefCount() const
	{
		m_cref--;
		if (m_cref <= 0)
			delete this;
	}

	const std::vector<int> & Vector() const { return m_vn; };

	// member variables:
	mutable int m_cref;
	std::vector<int> m_vn;
};

/*----------------------------------------------------------------------------------------------
	The GlyphSetIterator class allows the Graphite client to iterate over a non-contiguous
	set of glyphs for the segment, this is almost always the set of glyphs for a character.
----------------------------------------------------------------------------------------------*/
class GlyphSetIterator
: public std::iterator<std::random_access_iterator_tag, gr::GlyphInfo>
{
	friend class GlyphInfo;
	friend class Segment;
 
	// Segment containing the glyphs being iterated over.
	const Segment *	m_pseg;

	// Sometimes, in the case of character-to-glyph look-ups or attached
	// children, we need to represent a non-contiguous list; in these cases
	// we first map through a vector of output-slot objects into the actual 
	// glyph-info store.
	std::vector<int>::const_iterator m_vit;

	// Store a reliable copy of the vector we are iterating over:
	const RcVector * m_qvislout;

protected:
	// Constructor that includes output-slot mapping list, used for non-contiguous lists:
	GlyphSetIterator(Segment & seg, size_t islout, RcVector * qvislout)
		: m_pseg(&seg), m_vit(qvislout->Vector().begin() + islout)
	{
		m_qvislout = qvislout;
		m_qvislout->IncRefCount();
	}
public:
	// Default constructor--no output-slot mapping:
	GlyphSetIterator() throw() : m_pseg(0), m_vit(std::vector<int>::const_iterator())
	{
		m_qvislout = NULL;
	}

	// Copy constructor:
	GlyphSetIterator(const GlyphSetIterator & sit) throw()
	{
		m_pseg = sit.m_pseg;
		m_vit = sit.m_vit;
		m_qvislout = sit.m_qvislout;
		if (m_qvislout)
			m_qvislout->IncRefCount();
	}
	// Assignment operator:
	GlyphSetIterator & operator=(const GlyphSetIterator & sit) throw()
	{
		m_pseg = sit.m_pseg;
		m_vit = sit.m_vit;
		m_qvislout = sit.m_qvislout;
		if (m_qvislout)
			m_qvislout->IncRefCount();
		return *this;
	}
	// Destructor:
	~GlyphSetIterator()
	{
		if (m_qvislout)
			m_qvislout->DecRefCount();
	}

	// Forward iterator requirements.
	reference			operator*() const;
	pointer				operator->() const		{ return &(operator*()); }
	GlyphSetIterator &	operator++() throw()	{ assert(m_pseg); assert(m_qvislout); ++m_vit; return *this; }
	GlyphSetIterator	operator++(int) throw()	{ GlyphSetIterator tmp = *this; operator++(); return tmp; }

	// Bidirectional iterator requirements
	GlyphSetIterator &	operator--() throw()	{ assert(m_pseg); assert(m_qvislout); --m_vit; return *this; }
	GlyphSetIterator	operator--(int) throw()	{ GlyphSetIterator tmp = *this; operator--(); return tmp; }

	// Random access iterator requirements
	reference			operator[](difference_type n) const			{ return *operator+(n); }
	GlyphSetIterator &	operator+=(difference_type n) throw()		{ assert(m_pseg); assert(m_qvislout); m_vit += n; return *this; }
	GlyphSetIterator	operator+(difference_type n) const throw()	{ GlyphSetIterator r = *this; return r += n; }
	GlyphSetIterator &	operator-=(difference_type n) throw()		{ operator+=(-n); return *this; }
	GlyphSetIterator	operator-(difference_type n) const throw()	{ GlyphSetIterator r = *this; return r += -n; }
 
	// Relational operators.
  	// Forward iterator requirements
	bool operator==(const GlyphSetIterator & rhs) const throw()
	{
		assert(m_pseg == rhs.m_pseg);
		if (!m_pseg || !rhs.m_pseg)
		{
			// For an empty set, the range will be empty so any two iterators will be equal.
			return true;
		}
		assert(m_qvislout == rhs.m_qvislout);
		return (m_vit == rhs.m_vit);
	}
	bool operator!=(const GlyphSetIterator & rhs) const throw()	{ return !(*this == rhs); }

	// Random access iterator requirements
	bool operator<(const GlyphSetIterator & rhs) const throw()
	{
		assert(m_pseg == rhs.m_pseg);
		if (!m_pseg || !rhs.m_pseg)
		{
			// For an empty set, the range will be empty so any two iterators will be equal.
			return false;
		}
		assert(m_qvislout == rhs.m_qvislout);
		return (m_vit < rhs.m_vit);
	}
	bool operator>(const GlyphSetIterator & rhs) const throw()	{ return rhs < *this; }
	bool operator<=(const GlyphSetIterator & rhs) const throw()	{ return !(*this > rhs); }
	bool operator>=(const GlyphSetIterator & rhs) const throw()	{ return !(*this < rhs); }

	difference_type operator-(const GlyphSetIterator & rhs) const throw()
	{
		assert(m_pseg == rhs.m_pseg);
		if (!m_pseg || !rhs.m_pseg)
			return 0;	// empty set
		assert(m_qvislout == rhs.m_qvislout);
		return (m_vit - rhs.m_vit);
	}
};

inline GlyphSetIterator operator+(const GlyphSetIterator::difference_type n, const GlyphSetIterator & rhs)
{
	return rhs + n;
}


/*----------------------------------------------------------------------------------------------
	The GlyphInfo class provides access to details about a single glyph within a segment.
----------------------------------------------------------------------------------------------*/
class LayoutEnvironment
{
public:
	LayoutEnvironment()
	{
		// Defaults:
		m_fStartOfLine = true;
		m_fEndOfLine = true;
		m_lbBest = klbWordBreak;
		m_lbWorst = klbClipBreak;
		m_fRightToLeft = false;
		m_twsh = ktwshAll;
		m_pstrmLog = NULL;
		m_fDumbFallback = false;
		m_psegPrev = NULL;
		m_psegInit = NULL;
		m_pjust = NULL;
	}
	LayoutEnvironment(LayoutEnvironment & layout)
	{
		m_fStartOfLine = layout.m_fStartOfLine;
		m_fEndOfLine = layout.m_fEndOfLine;
		m_lbBest = layout.m_lbBest;
		m_lbWorst = layout.m_lbWorst;
		m_fRightToLeft = layout.m_fRightToLeft;
		m_twsh = layout.m_twsh;
		m_pstrmLog = layout.m_pstrmLog;
		m_fDumbFallback = layout.m_fDumbFallback;
		m_psegPrev = layout.m_psegPrev;
		m_psegInit = layout.m_psegInit;
		m_pjust = layout.m_pjust;
	}

	// Setters:
	inline void setStartOfLine(bool f)					{ m_fStartOfLine = f; }
	inline void setEndOfLine(bool f)					{ m_fEndOfLine = f; }
	inline void setBestBreak(LineBrk lb)				{ m_lbBest = lb; }
	inline void setWorstBreak(LineBrk lb)				{ m_lbWorst = lb; }
	inline void setRightToLeft(bool f)					{ m_fRightToLeft = f; }
	inline void setTrailingWs(TrWsHandling twsh)		{ m_twsh = twsh; }
	inline void setLoggingStream(std::ostream * pstrm)	{ m_pstrmLog = pstrm; }
	inline void setDumbFallback(bool f)					{ m_fDumbFallback = f; }
	inline void setPrevSegment(Segment * pseg)			{ m_psegPrev = pseg; }
	inline void setSegmentForInit(Segment * pseg)		{ m_psegInit = pseg; }
	inline void setJustifier(IGrJustifier * pjust)		{ m_pjust = pjust; }

	// Getters:
	inline bool startOfLine()				{ return m_fStartOfLine; }
	inline bool endOfLine()					{ return m_fEndOfLine; }
	inline LineBrk bestBreak()				{ return m_lbBest; }
	inline LineBrk worstBreak()				{ return m_lbWorst; }
	inline bool rightToLeft()				{ return m_fRightToLeft; }
	inline TrWsHandling trailingWs()		{ return m_twsh; }
	inline std::ostream * loggingStream()	{ return m_pstrmLog; }
	inline bool dumbFallback()				{ return m_fDumbFallback; }
	inline Segment * prevSegment()			{ return m_psegPrev; }
	inline Segment * segmentForInit()		{ return m_psegInit; }
	inline IGrJustifier * justifier()		{ return m_pjust; }

protected:
	bool m_fStartOfLine;
	bool m_fEndOfLine;
	LineBrk m_lbBest;
	LineBrk m_lbWorst;
	bool m_fRightToLeft;
	TrWsHandling m_twsh;
	std::ostream * m_pstrmLog;
	bool m_fDumbFallback;
	Segment * m_psegPrev;
	Segment * m_psegInit;
	IGrJustifier * m_pjust;
};

} // namespace gr

#if defined(GR_NO_NAMESPACE)
using namespace gr;
#endif

#endif  // !SEGMENTAUX_INCLUDED
