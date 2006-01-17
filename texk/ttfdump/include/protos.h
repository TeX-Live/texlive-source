#ifndef __PROTOS_H
#define __PROTOS_H

extern void ttfInitKERN(TTFontPtr);
extern void ttfInitPCLT(TTFontPtr);
extern void ttfInitVHEA(TTFontPtr);
extern void ttfInitVMTX(TTFontPtr);
extern void ttfFreeKERN(KERNPtr);
extern void ttfFreePCLT(PCLTPtr);
extern void ttfFreeVHEA(VHEAPtr);
extern void ttfFreeVMTX(VMTXPtr);
extern void ttfPrintKERN(FILE*, KERNPtr);
extern void ttfPrintPCLT(FILE*, PCLTPtr);
extern void ttfPrintVHEA(FILE*, VHEAPtr);
extern void ttfPrintVMTX(FILE*, VMTXPtr);
extern BYTE ttfGetBYTE(FILE*);
extern BYTE GetBYTE(VirtualMachine *);
extern SHORT GetSHORT(VirtualMachine *);
extern void Push(VirtualMachine *, LONG);
extern LONG Pop(VirtualMachine *);

#endif
