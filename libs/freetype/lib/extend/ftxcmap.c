/*******************************************************************
 *
 *  ftxcmap.c
 *
 *    API extension for iterating over Cmaps
 *
 *  Copyright 1996-2002 by Juliusz Chroboczek,
 *  David Turner, Robert Wilhelm, and Werner Lemberg.
 *
 *  This file is part of the FreeType project, and may only be used
 *  modified and distributed under the terms of the FreeType project
 *  license, LICENSE.TXT.  By continuing to use, modify, or distribute
 *  this file you indicate that you have read the license and
 *  understand and accept it fully.
 *
 *
 ******************************************************************/

#include "ftxcmap.h"

#include "tttypes.h"
#include "ttobjs.h"
#include "tttables.h"

static Long    charmap_first4  ( PCMap4, UShort* );
static Long    charmap_next4   ( PCMap4, UShort, UShort* );
static Long    charmap_last4   ( PCMap4, UShort* );
static UShort  charmap_find_id4( PCMap4, UShort, TCMap4Segment*, UShort );


/*******************************************************************
 *
 *  Function    :  TT_CharMap_First
 *
 *  Description :  Returns the first valid character code in a
 *                 given character map.  Also returns the corresponding
 *                 glyph index.
 *
 *  Input  :  charMap     handle to the target character map
 *            id          address where the glyph index will be
 *                        be returned in case of success
 *
 *  Output :  First valid character code.  -1 in case of failure.
 *
 ******************************************************************/

FT_EXPORT_FUNC( TT_Long )
TT_CharMap_First( TT_CharMap  charMap,
                  TT_UShort*  id )
{
  PCMapTable  cmap;
  UShort      i, c;


  if ( ( cmap = HANDLE_CharMap( charMap ) ) == 0 )
    return -1;

  switch ( cmap->format )
  {
  case 0:
    if ( id )
      *id = cmap->c.cmap0.glyphIdArray[0];
    return 0;

  case 4:
    return charmap_first4( &cmap->c.cmap4, id );

  case 6:
    if ( cmap->c.cmap6.entryCount < 1 )
      return -1;

    if ( id )
      *id = cmap->c.cmap6.glyphIdArray[0];
    return cmap->c.cmap6.firstCode;

  case 8:
  case 12:
    if ( id )
      *id = (TT_UShort)cmap->c.cmap8_12.groups[0].startGlyphID;
    return (TT_Long)cmap->c.cmap8_12.groups[0].startCharCode;

  default:
    /* Now loop from 0 to 65535. We can't use a simple "for' on */
    /* 16-bits systems, hence the "strange" loop here..         */
    i = 0;
    do
    {
      c = TT_Char_Index( charMap, i );
      if ( c > 0 )
      {
        if ( id )
          *id = c;
        return i;
      }
      i++;
    } while ( i != 0 );  /* because i is UShort! */

    return -1;
  }
}


static Long  charmap_first4( PCMap4   cmap4,
                             UShort*  id )
{
  UShort firstCode;


  if ( cmap4->segCountX2 / 2 < 1 )
    return -1;

  firstCode = cmap4->segments[0].startCount;

  if ( id )
    *id = charmap_find_id4( cmap4, firstCode, &(cmap4->segments[0]), 0 );

  return firstCode;
}


/*******************************************************************
 *
 *  Function    :  TT_CharMap_Next
 *
 *  Description :  Returns the next valid character code in a given
 *                 charMap.
 *
 *  Input  : charMap    handle to the target char. map
 *           index      starting character code
 *           id         address where the glyph index of the next
 *                      character will be returned
 *
 *  Output : Next valid character code after 'index'.  -1 in case
 *           of failure.
 *
 ******************************************************************/

FT_EXPORT_FUNC( TT_Long )
TT_CharMap_Next( TT_CharMap  charMap,
                 TT_ULong    index,
                 TT_UShort*  id )
{
  PCMapTable  cmap;
  UShort      i, c;


  cmap = HANDLE_CharMap( charMap );
  if ( !cmap )
    return -1;

  switch ( cmap->format )
  {
  case 0:
    if ( index < 255 )
    {
      if ( id )
        *id = cmap->c.cmap0.glyphIdArray[index + 1];
      return index + 1;
    }
    else
      return -1;

  case 4:
    return charmap_next4( &cmap->c.cmap4, index, id );

  case 6:
    {
      UInt  firstCode = cmap->c.cmap6.firstCode;


      if ( index + 1 < firstCode + cmap->c.cmap6.entryCount )
      {
        if ( id )
          *id = cmap->c.cmap6.glyphIdArray[index + 1 - firstCode];
        return index + 1;
      }
      else
        return -1;
    }

  case 8:
  case 12:
    {
      PCMapGroup  group, limit;


      limit = cmap->c.cmap8_12.groups + cmap->c.cmap8_12.nGroups;
      group = cmap->c.cmap8_12.last_group;

      if ( (ULong)( index              - group->startCharCode ) <
           (ULong)( group->endCharCode - group->startCharCode ) )
        goto Found;

      for ( group = cmap->c.cmap8_12.groups; group < limit; group++ )
      {
        if ( index > group->endCharCode )
          continue;

        if ( index >= group->startCharCode )
          goto Found;
      }

      return -1;

    Found:
      if ( index < group->endCharCode )
      {
        cmap->c.cmap8_12.last_group = group;
        *id = (UShort)( group->startGlyphID +
                        ( index - group->startCharCode ) + 1 );
        return index + 1;
      }

      group++;

      if ( group == limit )
        return -1;

      cmap->c.cmap8_12.last_group = group;
      *id = (UShort)group->startGlyphID;
      return group->startCharCode;
    }

  default:
    /* Now loop from 0 to 65535. We can't use a simple "for" on */
    /* 16-bits systems, hence the "strange" loop here..         */
    i = 0;
    do
    {
      c = TT_Char_Index( charMap, i );
      if ( c > 0 )
      {
        if ( id )
          *id = c;
        return i;
      }
      i++;
    } while ( i != 0 );  /* because i is UShort! */

    return -1;
  }
}


static Long  charmap_next4( PCMap4   cmap4,
                            UShort   charCode,
                            UShort*  id)
{
  UShort         segCount, nextCode;
  UShort         i;
  TCMap4Segment  seg4;


  if ( charCode == 0xFFFF )
    return -1;                /* get it out of the way now */

  segCount = cmap4->segCountX2 / 2;

  for ( i = 0; i < segCount; i++ )
    if ( charCode < cmap4->segments[i].endCount )
      break;

  /* Safety check - even though the last endCount should be 0xFFFF */
  if ( i >= segCount )
    return -1;

  seg4 = cmap4->segments[i];

  if ( charCode < seg4.startCount )
    nextCode = seg4.startCount;
  else
    nextCode = charCode + 1;

  if ( id )
    *id = charmap_find_id4( cmap4, nextCode, &seg4, i );

  return nextCode;
}


static UShort
charmap_find_id4( PCMap4          cmap4,
                  UShort          charCode,
                  TCMap4Segment*  seg4,
                  UShort          i )
{
  UShort  index1;


  if ( seg4->idRangeOffset == 0 )
    return (charCode + seg4->idDelta) & 0xFFFF;
  else
  {
    index1 = seg4->idRangeOffset / 2 + charCode-seg4->startCount -
             ( cmap4->segCountX2 / 2 - i );

    if ( index1 >= cmap4->numGlyphId || cmap4->glyphIdArray[index1] == 0 )
      return 0;
    else
      return (cmap4->glyphIdArray[index1] + seg4->idDelta) & 0xFFFF;
  }
}


/*******************************************************************
 *
 *  Function    :  TT_CharMap_Last
 *
 *  Description :  Returns the last valid character code in a
 *                 given character map.  Also returns the corresponding
 *                 glyph index.
 *
 *  Input  :  charMap     handle to the target character map
 *            id          address where the glyph index will be
 *                        be returned in case of success
 *
 *  Output :  Last valid character code.  -1 in case of failure.
 *
 ******************************************************************/

FT_EXPORT_FUNC( TT_Long )
TT_CharMap_Last( TT_CharMap  charMap,
                 TT_UShort*  id )
{
  PCMapTable  cmap;
  UShort      i, c;


  if ( ( cmap = HANDLE_CharMap( charMap ) ) == 0 )
    return -1;

  switch ( cmap->format )
  {
  case 0:
    if ( id )
      *id = cmap->c.cmap0.glyphIdArray[255];
    return 255;

  case 4:
    return charmap_last4( &cmap->c.cmap4, id );

  case 6:
    if ( cmap->c.cmap6.entryCount < 1 )
      return -1;

    if ( id )
      *id = cmap->c.cmap6.glyphIdArray[cmap->c.cmap6.entryCount - 1];
    return cmap->c.cmap6.firstCode + cmap->c.cmap6.entryCount - 1;

  case 8:
  case 12:
    {
      PCMapGroup  group;


      group = cmap->c.cmap8_12.groups + cmap->c.cmap8_12.nGroups - 1;

      if ( id )
        *id = (UShort)( group->startGlyphID +
                        ( group->endCharCode - group->startCharCode ) );
      return group->endCharCode;
    }

  default:
    i = 65535;
    do
    {
      c = TT_Char_Index( charMap, i );
      if ( c > 0 )
      {
        if ( id )
          *id = c;
        return i;
      }
      i--;
    } while ( i != 0 );

    return -1;
  }
}


static Long  charmap_last4( PCMap4   cmap4,
                            UShort*  id )
{
  UShort lastCode;


  if ( cmap4->segCountX2 / 2 < 1 )
    return -1;

  lastCode = cmap4->segments[cmap4->segCountX2 / 2 - 1].endCount;

  if ( id )
    *id = charmap_find_id4( cmap4,
                            lastCode,
                            &(cmap4->segments[cmap4->segCountX2 / 2 - 1]),
                            0 );

  return lastCode;
}


/* END */
