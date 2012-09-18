#ifndef __TTF_STACK_H
#define __TTF_STACK_H

extern BYTE GetBYTE(VirtualMachine *);
extern SHORT GetSHORT(VirtualMachine *);
extern void Push(VirtualMachine *, LONG);
extern LONG Pop(VirtualMachine *);

#endif /* __TTF_STACK_H */
