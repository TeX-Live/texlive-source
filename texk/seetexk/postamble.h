/*
 * Copyright (c) 1987, 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

/* Definitions for ScanPostAmble */

/*
 * ScanPostAmble reads the postamble of a DVI file from the (stdio)
 * file specified in its first argument.  It is handed two pointers to
 * functions.  The first (ScanPostAmble's second argument) is called
 * after the header information has been read, and given a pointer to a
 * PostAmbleInfo structure.  It is the job of this function to extract the
 * required information from this structure (which is deallocated when
 * ScanPostAmble returns).
 *
 * The second function is called once for each font definition occurring in
 * the postamble, and is given a pointer to a PostAmbleFont structure.  This
 * function should do whatever the device needs to read the actual font.
 *
 * If the DVI file appears malformed, ScanPostAmble will print an error
 * message and exit.  (Drastic, perhaps, but effective.)
 */

struct PostAmbleInfo {
	i32	pai_PrevPagePointer;	/* previous page pointer */
	i32	pai_Numerator;		/* numerator from dvi file */
	i32	pai_Denominator;	/* denominator from dvi file*/
	i32	pai_DVIMag;		/* \magnification */
	i32	pai_TallestPageHeight;	/* height of tallest page */
	i32	pai_WidestPageWidth;	/* width of widest page */
	int	pai_DVIStackSize;	/* DVI stack size required */
	int	pai_NumberOfPages;	/* total number of pages in DVI file */
};

struct PostAmbleFont {
	char	*paf_name;		/* name of font (null terminated) */
	int 	paf_n1;			/* length of first part of name */
	int 	paf_n2;			/* length of second part of name */
	i32 	paf_DVIFontIndex;	/* font index number */
	i32 	paf_DVIChecksum;	/* checksum from DVI file */
	i32 	paf_DVIMag;		/* "at size" */
	i32 	paf_DVIDesignSize;	/* design size of font */
};

int	ScanPostAmble();
