/* gstate.h -- define data structures for Graphics State, graphics states are 
 * mananged by font instructions.
 * Copyright (C) 1996 Li-Da Lho, All right reserved.
 */

#ifndef __TTF_GSTATE_H
#define __TTF_GSTATE_H

/* $Id: gstate.h,v 1.2 1998/07/06 06:07:01 werner Exp $ */

/* unit vectors used for directional purposes */
#define F2Dot14_ONE 1<<14
#define F2Dot14_EPSILON 1

#define F26Dot6_ONE 1L<<26
#define F26Dot6_EPSILON 1L

typedef struct
{
  F2Dot14 x;
  F2Dot14 y;
}
TTFUnitVector;

/* Unit vectors pointing into x and y direction */
#define UnitX {F2Dot14_ONE,0}
#define UnitY {0,F2Dot14_ONE}

/* vectors used as positional vectors */
typedef struct
{
  F26Dot6 x;
  F26Dot6 y;
}
TTFVector;

/* the types of the following fields are not well defined in the True Type
 * Font specification.
 * We can define them as we like.
 */
typedef struct
{
  /* boolean values */
  BYTE auto_flip;

  BYTE scan_control;
  USHORT instruction_control;
  SHORT round_state;

  F26Dot6 control_value_cut_in;
  F26Dot6 minimum_distance;
  F26Dot6 single_width_cut_in;
  FWord single_width_value;

  ULONG delta_base;
  ULONG delta_shift;

  /* unit vectors */
  TTFUnitVector dual_projection_vector;
  TTFUnitVector freedom_vector;
  TTFUnitVector projection_vector;

  /* reference points */
  ULONG rp0;
  ULONG rp1;
  ULONG rp2;

  /* zone pointers: take only 0 or 1 as values */
  ULONG zp0;
  ULONG zp1;
  ULONG zp2;

  SHORT gep0;
  SHORT gep1;
  SHORT gep2;

  ULONG loop;
}
GraphicsState;

#define GS_DEFAULT

#endif /* __TTF_GSTATE_H */


/* end of gstate.h */
