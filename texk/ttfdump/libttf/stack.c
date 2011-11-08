/* stack.c -- primitives for managing instruction stack and stream
 * Copyright (C) 1996 Li-Da Lho, All right reserved.
 */
#include "config.h"
#include "ttf.h"
#include "ttfutil.h"

/* 	$Id: stack.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $	 */

#define stack (vm->Stack)
#define sp (vm->sp)
#define limit (vm->stklimit)
#define ins (vm->iStream)
#define ip  (vm->ip)

enum TTF_ERROR {StackOverflow ,InstructionOverflow};

/* Push LONG l onto the stack of vm */
inline void Push(VirtualMachine *vm, LONG l)
{
    if (sp < limit)
	{
	    stack[sp] = l;
	    sp += 1;
	}
    else
	vm->Error_State = StackOverflow;
}
/* Pop LONG from the stack of vm and store in l */
inline LONG Pop(VirtualMachine *vm)
{
    LONG l;

    if (sp > 0)
	{
	    l = stack[sp];
	    sp -= 1;
	    return l;
	}
    else
	{
	    vm->Error_State = StackOverflow;
	    return 0;
	}
}

/* read one byte from the instruction stream */
inline BYTE GetBYTE(VirtualMachine *vm)
{
    if (ip < vm->insLength+1)
	return ins[ip++];
    else
	{
	    vm->Error_State = InstructionOverflow;
	    return 0;
	}
}
/* read two bytes from the instruction stream */
inline SHORT GetSHORT(VirtualMachine *vm)
{
    SHORT n = 0;

    if (ip < vm->insLength + 2)
	{
	    n = ins[ip++] << 8;    
	    n |= ins[ip++];
	    return n;
	}
    else
	{
	    vm->Error_State = InstructionOverflow;
	    return 0;
	} 
}
#if 0
/* Not used */
/* read four bytes from the instruction steam */
inline LONG GetLONG(VirtualMachine *vm)
{
    LONG n = 0;

    if (ip < vm->insLength + 4)
	{
	    n = ins[ip++] << 24;
	    n |= ins[ip++] << 16;
	    n |= ins[ip++] << 8;
	    n |= ins[ip++] ;
	    return n;
	}
    else
	{
	    vm->Error_State = InstructionOverflow;
	    return 0;
	}
}
#endif
