/****************************************************************************/
/*                                                                          */
/*  Erwin Dieterich,  15. 10. 1997                                          */
/*                  - 15. 08. 1999                                          */
/*                                                                          */
/*  TT_ErrToString: translate error codes to character strings              */
/*                                                                          */
/*  This extension provides internationalized error strings from the        */
/*  various error messages.  It uses the "gettext" package if available     */
/*  or returns English/American message strings if not.                     */
/*                                                                          */
/*  If you do not want to use it, or if you encounter some problems         */
/*  compiling this file, try to disable nls support when invoking           */
/*                                                                          */
/*       ./configure --disable-nls (on Unix).                               */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

#include "ftxerr18.h"
#include "ftxkern.h"
#include "ftxpost.h"
#include "ftxopen.h"

#include "ttconfig.h"   /* will pull up "ft_conf.h" */

#ifdef HAVE_LIBINTL_H

#include <libintl.h>

#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

#undef  _
#define _( String ) dgettext( "freetype",  String ) 

#if 0
char documentation[] = _(
  "Welcome to the translation of the Freetype v.1 library!\n"
  "\n"
  "This message is not really part of the library (it is bracketed by\n"
  " #if 0 / #endif), but is really intended to be the first one in the .po\n"
  "file, and the first one that a new translator should see, to give hints.\n"
  "\n"
  "First, while this .po might seems big, the real interesting part of it\n"
  "is only the messages in this file, lib/extend/ftxerr18.c. The other\n"
  "messages are part of the demonstration programs, and are more intended\n"
  "to help grasping how to use i18n with the Freetype library. So the real\n"
  "work is the about 65 first ones. Unfortunately they are the harder ones!\n"
  "\n"
  "A difficulty is that there are some specialized vocabulary, about\n"
  "``faces'', ``glyph'' and similar things. These are typographical things,\n"
  "and this should be researched before attempting to translate the whole\n"
  "thing. A quick look across the TrueType documentation might help to\n"
  "understand many things, too.\n"
  "\n"
  "Best wiches for your translation!\n"
  "Antoine Leca - 2002-11-08" );
#endif /* 0 */

#else /* ! HAVE_LIBINTL_H */

#define _( String ) ( String )

#endif /* HAVE_LIBINTL_H */


FT_EXPORT_FUNC( TT_String* )
TT_ErrToString18( TT_Error  error )
{

  switch ( error )
  {
    /* ----- high-level API error codes ----- */
  case TT_Err_Ok:
    return _( "Successful function call, no error." );

  case TT_Err_Invalid_Face_Handle:
    return _( "Invalid face handle." );
  case TT_Err_Invalid_Instance_Handle:
    return _( "Invalid instance handle." );
  case TT_Err_Invalid_Glyph_Handle:
    return _( "Invalid glyph handle." );
  case TT_Err_Invalid_CharMap_Handle:
    return _( "Invalid charmap handle." );
  case TT_Err_Invalid_Result_Address:
    return _( "Invalid result address." );
  case TT_Err_Invalid_Glyph_Index:
    return _( "Invalid glyph index." );
  case TT_Err_Invalid_Argument:
    return _( "Invalid argument." );
  case TT_Err_Could_Not_Open_File:
    return _( "Could not open file." );
  case TT_Err_File_Is_Not_Collection:
    return _( "File is not a TrueType collection." );
  case TT_Err_Table_Missing:
    return _( "Mandatory table missing." );
  case TT_Err_Invalid_Horiz_Metrics:
    return _( "Invalid horizontal metrics (hmtx table broken)." );
  case TT_Err_Invalid_CharMap_Format:
    return _( "Invalid charmap format." );
  case TT_Err_Invalid_PPem:
    return _( "Invalid ppem value." );
  case TT_Err_Invalid_Vert_Metrics:
    return _( "Invalid vertical metrics (vmtx table broken)." );

  case TT_Err_Invalid_File_Format:
    return _( "Invalid file format." );

  case TT_Err_Invalid_Engine:
    return _( "Invalid engine." );
  case TT_Err_Too_Many_Extensions:
    return _( "Too many extensions." );
  case TT_Err_Extensions_Unsupported:
    return _( "Extensions unsupported." );
  case TT_Err_Invalid_Extension_Id:
    return _( "Invalid extension id." );

  case TT_Err_No_Vertical_Data:
    return _( "No vertical data in font." );

  case TT_Err_Max_Profile_Missing:
    return _( "Maximum Profile (maxp) table missing." );
  case TT_Err_Header_Table_Missing:
    return _( "Font Header (head) table missing." );
  case TT_Err_Horiz_Header_Missing:
    return _( "Horizontal Header (hhea) table missing." );
  case TT_Err_Locations_Missing:
    return _( "Index to Location (loca) table missing." );
  case TT_Err_Name_Table_Missing:
    return _( "Naming (name) table missing." );
  case TT_Err_CMap_Table_Missing:
    return _( "Character to Glyph Index Mapping (cmap) tables missing." );
  case TT_Err_Hmtx_Table_Missing:
    return _( "Horizontal Metrics (hmtx) table missing." );
  case TT_Err_OS2_Table_Missing:
    return _( "OS/2 table missing." );
  case TT_Err_Post_Table_Missing:
    return _( "PostScript (post) table missing." );
  case TT_Err_Glyf_Table_Missing:
    return _( "Glyph (glyf) table missing." );


  /* ----- memory component error codes ----- */
  case TT_Err_Out_Of_Memory:
    return _( "Out of memory." );


  /* ----- file component error codes ----- */
  case TT_Err_Invalid_File_Offset:
    return _( "Invalid file offset." );
  case TT_Err_Invalid_File_Read:
    return _( "Invalid file read." );
  case TT_Err_Invalid_Frame_Access:
    return _( "Invalid frame access." );


  /* ----- glyph loader error codes ----- */
  case TT_Err_Too_Many_Points:
    return _( "Too many points." );
  case TT_Err_Too_Many_Contours:
    return _( "Too many contours." );
  case TT_Err_Invalid_Composite:
    return _( "Invalid composite glyph." );
  case TT_Err_Too_Many_Ins:
    return _( "Too many instructions." );
  case TT_Err_Too_Many_FDefs:
    return _( "Too many function definitions (FDEF)." );


  /* ----- byte-code interpreter error codes ----- */
  case TT_Err_Invalid_Opcode:
    return _( "Invalid opcode." );
  case TT_Err_Too_Few_Arguments:
    return _( "Too few arguments." );
  case TT_Err_Stack_Overflow:
    return _( "Stack overflow." );
  case TT_Err_Code_Overflow:
    return _( "Code overflow." );
  case TT_Err_Bad_Argument:
    return _( "Bad argument." );
  case TT_Err_Divide_By_Zero:
    return _( "Divide by zero." );
  case TT_Err_Storage_Overflow:
    return _( "Storage overflow." );
  case TT_Err_Cvt_Overflow:
    return _( "Control Value (cvt) table overflow." );
  case TT_Err_Invalid_Reference:
    return _( "Invalid reference." );
  case TT_Err_Invalid_Distance:
    return _( "Invalid distance." );
  case TT_Err_Interpolate_Twilight:
    return _( "Interpolate twilight points." );
  case TT_Err_Debug_OpCode:
    return _( "`DEBUG' opcode found." );
  case TT_Err_ENDF_In_Exec_Stream:
    return _( "`ENDF' in byte-code stream." );
  case TT_Err_Out_Of_CodeRanges:
    return _( "Out of code ranges." );
  case TT_Err_Nested_DEFS:
    return _( "Nested function definitions." );
  case TT_Err_Invalid_CodeRange:
    return _( "Invalid code range." );
  case TT_Err_Invalid_Displacement:
    return _( "Invalid displacement." );
  case TT_Err_Execution_Too_Long:
    return _( "Endless loop encountered while executing instructions." );


  /* ----- internal failure error codes ----- */
  case TT_Err_Nested_Frame_Access:
    return _( "Nested frame access." );
  case TT_Err_Invalid_Cache_List:
    return _( "Invalid cache list." );
  case TT_Err_Could_Not_Find_Context:
      return _( "Could not find context." );
  case TT_Err_Unlisted_Object:
    return _( "Unlisted object." );


  /* ----- scan-line converter error codes ----- */
  case TT_Err_Raster_Pool_Overflow:
    return _( "Raster pool overflow." );
  case TT_Err_Raster_Negative_Height:
    return _( "Raster: negative height encountered." );
  case TT_Err_Raster_Invalid_Value:
    return _( "Raster: invalid value." );
  case TT_Err_Raster_Not_Initialized:
    return _( "Raster not initialized." );


  /* ----- engine extensions error codes ----- */
  case TT_Err_Invalid_Kerning_Table_Format:
    return _( "Invalid kerning (kern) table format." );
  case TT_Err_Invalid_Kerning_Table:
    return _( "Invalid kerning (kern) table." );

  case TT_Err_Invalid_Post_Table_Format:
    return _( "Invalid PostScript (post) table format." );
  case TT_Err_Invalid_Post_Table:
    return _( "Invalid PostScript (post) table." );


  /* ----- TrueType Open extension error codes ----- */
  case TTO_Err_Invalid_SubTable_Format:
    return _( "Invalid TrueType Open subtable format." );
  case TTO_Err_Invalid_SubTable:
    return _( "Invalid TrueType Open subtable." );
  case TTO_Err_Not_Covered:
    return _( "Glyph(s) not covered by lookup." );
  case TTO_Err_Too_Many_Nested_Contexts:
    return _( "Too many nested context substitutions." );
  case TTO_Err_No_MM_Interpreter:
    return _( "No MM (Adobe MultipleMaster) interpreter available." );

  case TTO_Err_Invalid_GSUB_SubTable_Format:
    return _( "Invalid glyph substitution (GSUB) table format." );
  case TTO_Err_Invalid_GSUB_SubTable:
    return _( "Invalid glyph substitution (GSUB) table." );

  case TTO_Err_Invalid_GPOS_SubTable_Format:
    return _( "Invalid glyph positioning (GPOS) table format." );
  case TTO_Err_Invalid_GPOS_SubTable:
    return _( "Invalid glyph positioning (GPOS) table." );

  case TTO_Err_Invalid_GDEF_SubTable_Format:
    return _( "Invalid glyph definition (GDEF) table format." );
  case TTO_Err_Invalid_GDEF_SubTable:
    return _( "Invalid glyph definition (GDEF) table." );


  default:
    ;
  }

  return _( "Invalid Error Number." );
}


/* END */
