/* interp.c -- Instruction Interpreter
 * Copyright (C) 1996 Li-Da Lho, All right reserved.
 */
#include "config.h"
#include "ttf.h"
#include "ttfutil.h"

/* 	$Id: interp.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $	 */

/* A bit about error checking: The execution of a TrueType instruction is 
 * "atomic" which means that we don't process the error until a single
 * instruction is fully decoded and executed */

/* Pushing data onto the interpreter stack */

void Interp_NPUSHB (VirtualMachine *vm)
{
    BYTE i,n;
    ULONG l;

    n = GetBYTE(vm);

    for (i=1;i<=n;i++)
	{
	    /* unsigned extension to four bytes */
	    l = (ULONG) GetBYTE(vm);
	    Push(vm, l);
	}
}

void Interp_NPUSHW(VirtualMachine *vm)
{
    BYTE i,n;
    LONG l;

    n = GetBYTE(vm);

    for (i=1;i<=n;i++)
	{
	    /* signed extension to four bytes */
	    l = (LONG) GetSHORT(vm);
	    Push(vm, l);
	}

}

void Interp_PUSHB(VirtualMachine *vm)
{
    BYTE opcode,n,i;
    ULONG l;

    opcode = (vm->iStream)[vm->ip];
    n = opcode - 0xB0;

    for (i=0;i<=n;i++)
	{
	    l = (ULONG) GetBYTE(vm);
	    Push(vm, l);
	}
}
/* PUSHW[abc] Push Words 
 * Code Range 0xB8 - 0x0BF
 * From IS    w0,w1,...wn
 * Pushes     w0,w1,...wn */
void Interp_PUSHW(VirtualMachine *vm)
{
    BYTE opcode,n,i;
    LONG l;

    opcode = (vm->iStream)[vm->ip];
    n = opcode - 0xB8;

    for (i=0;i<=n;i++)
	{
	    l = (LONG) GetSHORT(vm);
	    Push(vm,l);
	}
}

/* Managing the Storage Area */

/* RS[]   Read Store
 * Code Range 0x43
 * Pops   location: Storage Area location (ULONG)
 * Pushes value: Storage Area value (ULONG)
 * Gets   Storage Area value */
void Interp_RS(VirtualMachine *vm)
{
    ULONG location,value;

    location = Pop(vm);    
    value = (vm->StorageArea)[location];
    Push(vm,value);
}
/* WS[]    Write Store
 * Code Range 0x42
 * Pops    value: Storage Area value (ULONG)
 *         location: Storage Area location (ULONG)
 * Pushes  -
 * Sets    Storage Area */
void Interp_WS(VirtualMachine *vm)
{
    ULONG location,value;
    
    value = Pop(vm);
    location = Pop(vm);
    (vm->StorageArea)[location] = value;
}

/* Managing the Control Value Table */

/* WCVTP[] Write control Value Table in Pixel units
 * Code Range 0x44
 * Pops    value: number in pixels (F26Dot6)
 *         location: Control Value Table location (ULONG)
 * Pushes  -
 * Sets    Control Value Table entry */
void Interp_WCVTP(VirtualMachine *vm)
{
    F26Dot6 value;
    ULONG location;

    value = Pop(vm);
    location = Pop(vm);

    (vm->cvt)[location] = value;
}
/* WCVTF[] Write Control Value Table in FUnits 
 * Code Range 0x70
 * Pops    value: number in FUnits (ULONG) (LONG actually,i think)
 *         location: Control Value (ULONG)
 * Pushes  -
 * Sets    Control Value Table entry
 */
void Interp_WCVTF(VirtualMachine *vm)
{
    ULONG location;
    LONG value;

    value = Pop(vm);
    location = Pop(vm);

    value = ScaleToPoints(vm,value);
    (vm->cvt)[location] = value;
}
/* RCVT[]  Read Control Value Table
 * Code Range 0x45
 * Pops    location: CVT entry number (ULONG)
 * Pushes  value: CVT value (F26Dot6)
 * Gets    Control Value Table entry
 */
void Interp_RCVT(VirtualMachine *vm)
{
    ULONG location;
    F26Dot6 value;

    location = Pop(vm);
    value = (vm->cvt)[location];
    Push(vm,value);
}

/* Managing Graphics State */

/* SVTCA[a] Set freedom and projection vector to Coordinate Axia
 * Code Range 0x00 - 0x01
 * a        0: set to y axis
 *          1: set to x axis
 * Pops     -
 * Pushes   -
 * Sets     projection_vector
 *          freedom_vector
 */
void Interp_SVTCA(VirtualMachine *vm)
{
    BYTE opcode;
    TTFUnitVector vect;

    opcode = (vm->iStream)[vm->ip];
    switch (opcode)
	{
	case 0x00:
	    vect.x = 0;
	    vect.y = F2Dot14_ONE;
	    break;
	case 0x01:
	    vect.x = F2Dot14_ONE;
	    vect.y = 0;
	    break;
	}
    vm->gstate.projection_vector = vm->gstate.freedom_vector = vect;
}
/* SPVCA[a] Set projection vector to coordinate axis 
 * Code Range 0x02 - 0x03
 * a        0: set to y axis
 *          1: set to x axis
 * Pops     -
 * Pushes   -
 * Sets     projection vector
 */
void Interp_SPVTCA(VirtualMachine *vm)
{
    BYTE opcode;
    TTFUnitVector vect;

    opcode = (vm->iStream)[vm->ip];
    switch (opcode)
	{
	case 0x02:
	    vect.x = 0;
	    vect.y = F2Dot14_ONE;
	    break;
	case 0x03:
	    vect.x = F2Dot14_ONE;
	    vect.y = 0;
	    break;
	}
    vm->gstate.projection_vector = vect;
}
/* SFVTCA[a] Set freedom vector to coordinate axis
 * Code Range 0x04 - 0x05
 * a        0: set to y axis
 *          1: set to x axis
 * Pops     -
 * Pushes   -
 * Sets     freedom vector
 */
void Interp_SFVTCA(VirtualMachine *vm)
{
    BYTE opcode;
    TTFUnitVector vect;

    opcode = (vm->iStream)[vm->ip];
    switch (opcode)
	{
	case 0x04:
	    vect.x = 0;
	    vect.y = F2Dot14_ONE;
	    break;
	case 0x05:
	    vect.x = F2Dot14_ONE;
	    vect.y = 0;
	    break;
	}
    vm->gstate.freedom_vector = vect;
}
/* SPVTL[a] Set projection vector to line
 * Code Range 0x06 - 0x07
 * a        0: set projection_vector to be parallel to line segment from p1 
 *             to p2
 *          1: set projection_vector to be perpendicular to line segment from 
 *             p1 to p2; the vector is retated counter clockwise 90 degrees
 * Pops     p1: point number (ULONG)
 *          p2: point number (ULONG)
 * Pushes   -
 * Uses     point p1 in the zone pointed at by zp2
 *          point p2 in the zone pointed at by zp1
 * Sets     projection vector
 */
void Interp_SPVTL(VirtualMachine *vm)
{
    ULONG p1,p2;
    BYTE opcode;
    TTFUnitVector vect;

    opcode = (vm->iStream)[vm->ip];
    p1 = Pop(vm);
    p2 = Pop(vm);

    switch (opcode)
	{
	    /* not finished yet */
	case 0x06:
	    break;
	case 0x07:
	    break;
	}
    vm->gstate.projection_vector = vect;
}
/* SFVTL[a] Set freedom vector to line
 * Code Range 0x08 - 0x09
 * a        0: set freedom_vector to be parallel to line segment from p1 
 *             to p2
 *          1: set freedom_vector to be perpendicular to line segment from 
 *             p1 to p2; the vector is retated counter clockwise 90 degrees
 * Pops     p1: point number (ULONG)
 *          p2: point number (ULONG)
 * Pushes   -
 * Uses     point p1 in the zone pointed at by zp2
 *          point p2 in the zone pointed at by zp1
 * Sets     freedom vector
 */
void Interp_SFVTL(VirtualMachine *vm)
{
    ULONG p1,p2;
    BYTE opcode;
    TTFUnitVector vect;

    opcode = (vm->iStream)[vm->ip];
    p1 = Pop(vm);
    p2 = Pop(vm);
    switch (opcode)
	{
	    /* not finished yet */
	case 0x08:
	    break;
	case 0x09:
	    break;
	}
    vm->gstate.freedom_vector = vect;
}
/* SFVTPV[] Set freedom vector to projection vector
 * Code Range 0x0E
 * Pops     -
 * Pushes   -
 * Sets     freedom vector
 */
void Interp_SFVTPV(VirtualMachine *vm)
{
    vm->gstate.freedom_vector = vm->gstate.projection_vector;
}
/* SDPVTL[a] Set dual projection vector to line
 * Code Range 0x86 - 0x87
 * a         0: vectors are parallel to line 
 *           1: vectors are perpendicular to line
 * Pops      p1: first point number (ULONG)
 *           p2: second point number (ULONG)
 * Pushes    -
 * Sets      dual_projection_vector and projection_vector
 * Uses      point p1 in the zone pointed by zp2
 *           point p2 in the zone pointed by zp1
 */
void Interp_SDPVTL(VirtualMachine *vm)
{
    ULONG p1,p2;
    BYTE opcode;
    TTFUnitVector vect;

    opcode = (vm->iStream)[vm->ip];
    p1 = Pop(vm);
    p2 = Pop(vm);
    switch (opcode)
	{
	    /* not finished yet */
	case 0x86:
	    break;
	case 0x87:
	    break;
	}
    vm->gstate.dual_projection_vector = vm->gstate.projection_vector = vect;
}
/* SPVFS[] Set projection vector from stack
 * Code Range 0x0A
 * Pops    y: y component of projection vector (2.14 padded with zeros)
 *         x: x component of projection vector (2.14 padded with zeros)
 * Pushes  -
 * Sets    projection_vector
 */
void Inpterp_SPVFS(VirtualMachine *vm)
{
    F2Dot14 x,y;

    x = Pop(vm);
    y = Pop(vm);

    vm->gstate.projection_vector.x = x;
    vm->gstate.projection_vector.y = y;
    /* vm->gstate.projection = Normalize(vm->gstate.projection); ?? */
}
/* SFVFS[] Set freedom vector form stack
 * Code Range 0x0B
 * Pops    y: y component of freedom vector (2.14 padded with zeros)
 *         x: x component of freedom vector (2.14 padded with zeros)
 * Pushes  -
 * Sets    freedom_vector
 */
void Inpterp_SFVFS(VirtualMachine *vm)
{
    F2Dot14 x,y;

    x = Pop(vm);
    y = Pop(vm);

    vm->gstate.freedom_vector.x = x;
    vm->gstate.freedom_vector.y = y;
    /* vm->gstate.freedom = Normalize(vm->gstate.freedom); ?? */
}
/* GPV[]   Get projection vector
 * Code Range 0x0C
 * Pops    -
 * Pushes  x: x component of projection vector (2.14 padded with zeros)
 *         y: y component of projection vector (2.14 padded with zeros)
 * Gets    projection vector
 */
void Interp_GPV(VirtualMachine *vm)
{
    ULONG l;

    l = (ULONG) vm->gstate.projection_vector.x;
    Push(vm,l);
    l = (ULONG) vm->gstate.projection_vector.y;
    Push(vm,l);
}
/* GFV[] Get freedom vector 
 * Code Range 0x0D
 * Pops    -
 * Pushes  x: x component of freedom vector (2.14 padded with zeros)
 *         y: y component of freedom vector (2.14 padded with zeros)
 * Gets    freedom vector
 */
void Interp_GFV(VirtualMachine *vm)
{
    ULONG l;

    l = (ULONG) vm->gstate.freedom_vector.x;
    Push(vm,l);
    l = (ULONG) vm->gstate.freedom_vector.y;
    Push(vm,l);
}
/* SRP0[]  Set reference point 0
 * Code Range 0x10
 * Pops    p: point number (ULONG)
 * Pushes  -
 * Sets    rp0
 * Affects IP,MDAP,MIAP,MIRP,MSIRP,SHC,SHE,SHP
 */
void Inpter_SRP0(VirtualMachine *vm)
{
    vm->gstate.rp0 = Pop(vm);
}
/* SRP1[]  Set reference point 1
 * Code Range 0x11
 * Pops    p: point number (ULONG)
 * Pushes  -
 * Sets    rp1
 * Affects IP,MDAP,MIAP,MIRP,MSIRP,SHC,SHE,SHP
 */
void Inpter_SRP1(VirtualMachine *vm)
{
    vm->gstate.rp1 = Pop(vm);
}
/* SRP2[]  Set reference point 2
 * Code Range 0x12
 * Pops    p: point number (ULONG)
 * Pushes  -
 * Sets    rp2
 * Affects IP,MDAP,MIAP,MIRP,MSIRP,SHC,SHE,SHP
 */
void Inpter_SRP2(VirtualMachine *vm)
{
    vm->gstate.rp2 = Pop(vm);
}
/* SZP0[]   Set zone pointer 0
 * Code Range 0x13
 * Pops     n: zone number (ULONG)
 * Pushes   -
 * Sets     zp0
 * Affects  ALIGNPTS,ALIGNRP,DELTAP1,DELTAP2,DELTAP3,IP,ISECT,MD,MDAP,MIAP,
 *          MIRP,MSIRP,SHC,SHE,SHP,UTP
 */
void Inpterp_SZP0(VirtualMachine *vm)
{
    vm->gstate.zp0 = Pop(vm);
}
/* SZP1[]   Set zone pointer 1
 * Code Range 0x14
 * Pops     n: zone number (ULONG)
 * Pushes   -
 * Sets     zp1
 * Affects  ALIGNPTS,ALIGNRP,DELTAP1,DELTAP2,DELTAP3,IP,ISECT,MD,MDAP,MIAP,
 *          MIRP,MSIRP,SHC,SHE,SHP,UTP
 */
void Inpterp_SZP1(VirtualMachine *vm)
{
    vm->gstate.zp1 = Pop(vm);
}
/* SZP2[]   Set zone pointer 2
 * Code Range 0x15
 * Pops     n: zone number (ULONG)
 * Pushes   -
 * Sets     zp2
 * Affects  ALIGNPTS,ALIGNRP,DELTAP1,DELTAP2,DELTAP3,IP,ISECT,MD,MDAP,MIAP,
 *          MIRP,MSIRP,SHC,SHE,SHP,UTP
 */
void Inpterp_SZP2(VirtualMachine *vm)
{
    vm->gstate.zp2 = Pop(vm);
}
/* SZPS[]   Set zone pointerS
 * Code Range 0x16
 * Pops     n: zone number (ULONG)
 * Pushes   -
 * Sets     zp0,zp1,zp2
 * Affects  ALIGNPTS,ALIGNRP,DELTAP1,DELTAP2,DELTAP3,IP,ISECT,MD,MDAP,MIAP,
 *          MIRP,MSIRP,SHC,SHE,SHP,UTP
 */
void Inpterp_SZPS(VirtualMachine *vm)
{
    vm->gstate.zp0 = vm->gstate.zp1 = vm->gstate.zp2 = Pop(vm);
}
/* RTHG[] Round to half grid
 * Code Range 0x19
 * Pops   -
 * Pushes -
 * Sets   round_state
 * Affects MDAP,MDRP,MDIAP,MIRP,ROUND
 * Uses   freedom_vector, projection_vetor
 */
void Interp_RTHG(VirtualMachine *vm)
{
    vm->gstate.round_state = 0;
}
/* RTG[] Round to grid
 * Code Range 0x18
 * Pops   -
 * Pushes -
 * Sets   round_state
 * Affects MDAP,MDRP,MDIAP,MIRP,ROUND
 * Uses   freedom_vector, projection_vetor
 */
void Interp_RTG(VirtualMachine *vm)
{
    vm->gstate.round_state = 1;
}
/* RTDG[] Round to double grid
 * Code Range 0x3D
 * Pops   -
 * Pushes -
 * Sets   round_state
 * Affects MDAP,MDRP,MDIAP,MIRP,ROUND
 * Uses   freedom_vector, projection_vetor
 */
void Interp_RTDG(VirtualMachine *vm)
{
    vm->gstate.round_state = 2;
}
/* RTHG[] Round down to grid
 * Code Range 0x7D
 * Pops   -
 * Pushes -
 * Sets   round_state
 * Affects MDAP,MDRP,MDIAP,MIRP,ROUND
 * Uses   freedom_vector, projection_vetor
 */
void Interp_RDTG(VirtualMachine *vm)
{
    vm->gstate.round_state = 3;
}
/* RUTG[] Round up to grid
 * Code Range 0x7C
 * Pops   -
 * Pushes -
 * Sets   round_state
 * Affects MDAP,MDRP,MDIAP,MIRP,ROUND
 * Uses   freedom_vector, projection_vetor
 */
void Interp_RUTG(VirtualMachine *vm)
{
    vm->gstate.round_state = 4;
}
/* ROFF[] Round off
 * Code Range 0x7A
 * Pops   -
 * Pushes -
 * Sets   round_state
 * Affects MDAP,MDRP,MDIAP,MIRP,ROUND
 * Uses   freedom_vector, projection_vetor
 */
void Interp_ROFF(VirtualMachine *vm)
{
    vm->gstate.round_state = 5;
}
/* SROUND[] Super round
 * Code Range 0x76
 * Pops     n: number (ULONG) decomposed to obtain period, phase, threshold
 * Pushes   -
 * Sets     round_state
 * Affects  MDAP,MDRP,MIAP,MIRP,ROUND
 */
void Interp_SROUND(VirtualMachine *vm)
{
    ULONG n;

    n = Pop(vm);
    /* not finished yet */
}
/* S45ROUND[] Super round 45 degrees
 * Code Range 0x77
 * Pops     n: number (ULONG) decomposed to obtain period, phase, threshold
 * Pushes   -
 * Sets     round_state
 * Affects  MDAP,MDRP,MIAP,MIRP,ROUND
 */
void Interp_S45ROUND(VirtualMachine *vm)
{
    ULONG n;

    n = Pop(vm);
    /* not finished yet */
}
/* SLOOP[] Set LOOP variable
 * Code Range 0x17
 * Pops    n: value for loop graphics state variable
 * Pushes  -
 * Set     loop
 * Affects ALIGNRP,FLIPPT,IP,SHP,SHIP
 */
void Interp_SLOOP(VirtualMachine *vm)
{
    vm->gstate.loop = Pop(vm);
}
/* SMD[] Set minimum_distance
 * Code Range 0x1a
 * Pops   distance: value for minimum_distance (F26Dot6)
 * Pushes -
 * Sets   minimum_distance
 */
void Interp_SMD(VirtualMachine *vm)
{
    vm->gstate.minimum_distance = Pop(vm);
}
/* INSTCTRL[] Instruction execution control
 * Code Range 0x8E
 * Pops       s: selector flags 
 *            value: USHORT padded to 32 bits used to set value of 
 *            instruction control
 * Pushes     -
 * Sets       instruction_control
 */
void Interp_INSTCTRL(VirtualMachine *vm)
{
    ULONG flag;
    
    flag = Pop(vm);
    vm->gstate.instruction_control = Pop(vm);
    /* not finished yet */
}
/* SCANCTRL[] Scan conversion cotrol
 * Code Range 0x85
 * Pops       n: flags indication when to turn on dropout mode (16 bits padded
 *               to 32 bits)
 * Pushes     -
 * Sets       scan_control
 */
void Interp_SCANCTRL(VirtualMachine *vm)
{
    USHORT flag;

    flag = Pop(vm);
    /* not finished yet */
}
/* SCANTYPE[] Scantype
 * Code Range 0x8D
 * Pops       n: 16 bit interger
 * Pushes     -
 * Sets       scan_control
 */
void Interp_SCANTYPE(VirtualMachine *vm)
{
    /* not finished yet */
}
/* SCVTCI[] Set control value table cut in
 * Code Range 0x1D
 * Pops       n: value for cut_in (F26Dot6)
 * Pushes     -
 * Sets       control_value_cut_in
 * Affects    MIAP,MIRP
 */
void Interp_SCVTCI(VirtualMachine *vm)
{
    vm->gstate.control_value_cut_in = Pop(vm);
}
/* SSWCI[] Single width cut in
 * Code Range 0x1E
 * Pops       n: value for single_width_cut_in
 * Pushes     -
 * Sets       signel_width_cut_in
 * Affects    MIAP,MIRP
 */
void Interp_SSWCI(VirtualMachine *vm)
{
    vm->gstate.single_width_cut_in = Pop(vm);
}
/* SSW[] Set single width
 * Code Range 0x1F
 * Pops       n: value for single_width_value (FUnits)
 * Pushes     -
 * Sets       single_width_value
 */
void Interp_SSW(VirtualMachine *vm)
{
    vm->gstate.single_width_cut_in = Pop(vm);
}
/* FLIPON[] Set the auto_flip on
 * Code Range 0x4D
 * Pops       -
 * Pushes     -
 * Sets       auto_flip
 * Affects    MIRP
 */
void Interp_FLIPON(VirtualMachine *vm)
{
    vm->gstate.auto_flip = 1;
}
/* FLIPOFF[] Set the auto_flip off
 * Code Range 0x4E
 * Pops       -
 * Pushes     -
 * Sets       auto_flip
 * Affects    MIRP
 */
void Interp_FLIPOFF(VirtualMachine *vm)
{
    vm->gstate.auto_flip = 0;
}
/* SDB Set delta_base in the graphics state
 * Code Range 0x5E
 * Pops       n: value for delta_base (ULONG)
 * Pushes     -
 * Sets       delta_base
 * Affects    DELTAP1,DELTAP2,DELTAP3,DELTAC1,DELTAC2,DELTAC3
 */
void Interp_SDB(VirtualMachine *vm)
{
    vm->gstate.delta_base = Pop(vm);
}
/* SDS Set delta_shift in the graphics state
 * Code Range 0x5F
 * Pops       n: value for delat_shift (ULONG)
 * Sets       delta_shift
 * Affects    DELTAP1,DELTAP2,DELTAP3,DELATC1,DELTAC2,DELTAC3
 */
void Interp_SDS(VirtualMachine *vm)
{
    vm->gstate.delta_shift = Pop(vm);
}

/* Reading and writing data */

/* GC[a] Get coordinate projected on to the projection_vector
 * Code Range 0x46 - 0x47
 * a          0: use current position of point p
 *            1: use the position of point p in the original outline
 * Pops       p: point number (ULONG)
 * Pushes     value: coordinate location (F26Dot6)
 * Uses       zp2,projection
 */
void Interp_GC(VirtualMachine *vm)
{
    ULONG point;
    TTFVector pos;

    point = Pop(vm);

    /* not finished yet */
}
/* SCFS[] Sets coordinate from the stack using projection_vector and 
 *        freedom_vector
 * Code Range 0x48
 * Pops       value: distance from origin to move point (F26Dot6)
 *            p: point number (ULONG)
 * Pushes     -
 * Uses       zp2, freedom_vector, projection_vector
 */
void Interp_SCFS(VirtualMachine *vm)
{
#ifdef WIN32
    __int64 tmp;
#else
    long long tmp;
#endif
    F26Dot6 value;
    ULONG point;

    value = Pop(vm);
    point = Pop(vm);
    /* not finished yet */
} 
/* MD[a] Measure Distance
 *
 * Code Range 0x49-0x4A
 * a          0: measure distance in grid-fitted outline
 *            1: measure distance in original outline
 * Pops       p1: point number (ULONG)
 *            p2: point number (ULONG);
 * Pushes     distance (F26Dot6)
 * uses       zp1 with point p1, zp2 withe point p2, projection_vector
 */
void Interp_MD(VirtualMachine *vm)
{
    ULONG p1,p2;
    BYTE opcode;
    TTFUnitVector vect;

    opcode = (vm->iStream)[vm->ip];   
    p1 = Pop(vm);
    p2 = Pop(vm);
    switch (opcode)
	{
	    /* not finished yet */
	case 0x49:
	    break;
	case 0x4A:
	    break;
	}
    /* not finished yet */
}
/* MPPEM[] Measure Pixel Per EM
 *
 * Code Range 0x4B
 * Pops       -
 * Pushes     ppem:pixel per em (ULONG)
 */
void Interp_MPPEM(VirtualMachine *vm)
{
    /* not finished yet */
}
/* MPS[]  Measure Point Size
 *
 * Code Range 0x4C
 * Pops       -
 * Pushes     pointSize: the size in points of the current glyph (F26Dot6)
 */
void Interp_MPS(VirtualMachine *vm)
{
    F26Dot6 pointSize = vm->pointsize;

    Push(vm,pointSize);
}

/* Mananging Outlines */

/* FLIPPT[] Flip point
 * Code Range 0x80
 * Pops       p: point number (ULONG)
 * Pushes     -
 * Uses       loop, p is referenced in zp0
 */
void Interp_FLIPPT(VirtualMachine *vm)
{
    ULONG point;

    point = Pop(vm);

    /* not finished yet */
}
/* FLIPRGON[] Filp range on
 * Code Range 0x81
 * Pops       hightpoint: highest point number in range of points to be flipped
 *            lowpoint: lowest point number in range of points to be flipped
 *            both are ULONG
 * Pushes     -
 */
void Interp_FLIPRGON(VirtualMachine *vm)
{
    
}
