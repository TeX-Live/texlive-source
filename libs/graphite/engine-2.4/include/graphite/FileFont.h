/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001, 2005 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: XftFont.h
Responsibility: Keith Stribley
Last reviewed: Not yet.

Description:
		A Font is an object that represents a font-family + bold + italic setting, that contains
	Graphite tables.
----------------------------------------------------------------------------------------------*/
#ifndef FILEFONT_INCLUDED
#define FILEFONT_INCLUDED
#include "Font.h"


//:End Ignore


namespace gr
{

class FontTableCache;
class GrEngine;
/*-----------------------------------------------------------------------------
	Stock implementation of an Xft font class. 
------------------------------------------------------------------------------*/
/** 
* An implementation of the Graphite gr::Font class for Xft fonts for use on
* systems using X. Freetype is used to retrieve font information.
* If you are using Freetype directly, without Xft, you can pass in a FT_Face
* directly.
*/
class FileFont : public Font
{
public:
	/**
	* The main user constructor. Constructs the font from the supplied FILE pointer.
	* @param file the FILE pointer to initialise from (must not be null)
	*/
	FileFont(FILE * file, float pointSize, unsigned int DPIx, unsigned int DPIy = 0);
	
	/**
	* Alternative user constructor. Constructs the font from the supplied filename.
	* @param filename character array
	*/
	FileFont(char * filename, float pointSize, unsigned int DPIx, unsigned int DPIy = 0);
	
	/**
	* Alternative user constructor. Constructs the font from the supplied filename.
	* @param filename as a string
	*/
	FileFont(std::string filename, float pointSize, 
			 unsigned int DPIx, unsigned int DPIy = 0);
	
	
	/**
	* Destructor - cleans up the tables that it has allocated if all other copies
	* of this font have been deleted.
	*/
	virtual ~FileFont();
	
	/**
	* Returns a copy of the recipient. Specifically needed to store the Font 
	* in a segment. 
	* @internal
	*/
	virtual Font * copyThis();
	
	/**
	* Copy constructor - note the tables are shared between the copy and the
	* original font for efficiency reasons. 
	* The last copy to be deleted will delete the tables.
	* @param font the XftGrFont object to copy from.
	* @internal
	*/
	FileFont(const FileFont & font, float pointSize = 0, unsigned int DPIx = 0, unsigned int DPIy = 0);

	//virtual FontErrorCode isValidForGraphite(int * pnVersion = NULL, int * pnSubVersion = NULL);
	/**
	* Returns a pointer to the start of a table in the font. The ftblid32 type 
	* is a 32-bit unsigned integer.
	* If the Font class cannot easily determine the length of the table, it may 
	* set 0 as the length (while returning a non-NULL pointer to the table). This
	* means that certain kinds of error checking cannot be done by the Graphite
	* engine.
	* Throws an exception if there is some other error in reading the table, or if
	* the table asked for is not one of the expected ones (cmap, head, name, Sile,
	* Silf, Feat, Gloc, Glat). [If we want to be able to read “non-standard” 
	* tables, then the caller needs to be responsible for freeing the memory.]
	*/
	virtual const void * getTable(fontTableId32 tableID, size_t * pcbSize);
	
	/**
	* Returns the basic metrics of the font. It corresponds to the current
	* GrGraphics::get_FontAscent, get_FontDescent and GetFontEmSquare methods.
	* @param pAscent pointer to hold font ascent
	* @param pDescent pointer to hold font descent 
	* @param pEmSquare pointer to hold font EM square
	*/
	virtual void getFontMetrics(float * pAscent, float * pDescent = NULL,
					float * pEmSquare = NULL);
	
	/**
	* Returns true if the given arguments specify a font that contains Graphite
	* tables—specfically a “Silf” table.
	* @param xftFont XftFont pointer
	* @return true if this font has the silf table
	*/
	bool fontHasGraphiteTables();
	
	/**
	* Returns true if the given arguments specify a font that contains Graphite
	* tables—specfically a “Silf” table.
	* @param xftFont XftFont pointer
	* @return true if this font has the silf table
	*/
	static bool FontHasGraphiteTables(FILE * file);
	
	/**
	* Returns true if the given arguments specify a font that contains Graphite
	* tables—specfically a “Silf” table.
	* @param face FT_Face handle
	* @return true if this font has the silf table
	*/
	static bool FontHasGraphiteTables(char * filename);

	// Temporary, until interface gets thoroughly reworked:
	GrEngine * GraphiteEngine();
				
	/**
	* Returns the font ascent in pixels. Using floating point allows us to 
	* avoid rounding errors. 
	* Value is the same as that returned by getFontMetrics()
	* @return the font ascent
	*/
	virtual float ascent()
	{
		float pixAscent;
		getFontMetrics(&pixAscent);
		return pixAscent;
	}
	/**
	* Returns the font descent in pixels with a positive sign.
	* Value is the same as that returned by getFontMetrics()
	* @return the font descent in pixels.
	*/
	virtual float descent()
	{
		float pixDescent;
		getFontMetrics(NULL, &pixDescent);
		return pixDescent;
	}
	/**
	* Returns the total height of the font. 
	* Equivalent to ascent() + descent().
	* @return font height
	*/
	virtual float height()
	{
		float pixAscent;
		float pixDescent;
		getFontMetrics(&pixAscent, &pixDescent);
		return (pixAscent + pixDescent);
	}
	/**
	* @return true if the font is a bold styled one. 
	*/
	virtual bool bold()
	{
		return m_fBold;
	}
	/**
	* @return true if the font is italic
	*/
	virtual bool italic()
	{
		return m_fItalic;
	}
	/** @return the DPI for the x-axis.
	*/
	virtual unsigned int getDPIx() 
	{
		return m_dpiX;
	}
	/** @return Returns the DPI for the y-axis.
	*/
	virtual unsigned int getDPIy()
	{
		return m_dpiY;
	}
	// Specific to XftGrFont:		
	
	/** @return set the DPI for the x-axis.
	*/
	virtual void setDPIx(unsigned int dpi) 
	{
		m_dpiX = dpi;
		m_xScale = scaleFromDpi(dpi);
	}
	/** @return set the DPI for the y-axis.
	*/
	virtual void setDPIy(unsigned int dpi)
	{
		m_dpiY = dpi;
		m_yScale = scaleFromDpi(dpi);
	}
	
	bool isValid() { return m_fIsValid; };
protected:
	/** Default constructor is not used */
	FileFont();
	/**
	* Common initialisation between the XftFont and FT_Face constructors.
	*/
	void initializeFromFace(void);
	// this uses an int to avoid having to include TtfUtil.h in the include
	// directory
	byte * readTable(int /*TableId*/ tableId, size_t & size);
	
	// pixels-per-em = dpi * point-size / 72
	// pixel-coord = design-coord * pixels-per-em / font-em-square
	float scaleFromDpi(int dpi)
	{
		return (dpi * m_pointSize) / (72.0f * m_mEmSquare);
	}

	// Member variables:
	FILE * m_pfile;
	
	unsigned long m_clrFore;
	unsigned long m_clrBack;
	bool m_fBold;
	bool m_fItalic;
	// font table caches:
	FontTableCache * m_pTableCache;

	// KRS: I think these should be cached otherwise Segment::LineContextSegment doesn't work
	float m_mAscent;
	float m_mDescent;
	float m_mEmSquare;
	float m_pointSize;
	int m_dpiX;
	int m_dpiY;
	bool m_fIsValid;
	std::wstring m_stuFaceName;
	byte * m_pHeader;
	byte * m_pTableDir;
	float m_xScale;
	float m_yScale;
};


} // namespace gr

#endif

