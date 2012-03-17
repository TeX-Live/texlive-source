/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: GenericResource.h
Responsibility: John Thomson
Last reviewed:

	Resources needed by certain generic components (but must be created in app's or AppCore's
	resource file).
----------------------------------------------------------------------------------------------*/
#ifdef _MSC_VER
#pragma once
#endif
#ifndef GenericResource_H
#define GenericResource_H 1
// Create suitable resources for these if your app uses StackDumper.
// For example
// STRINGTABLE DISCARDABLE 
// BEGIN
//	"A programming error (%s) has been detected in module %s.\n Please report this to the developers", kstidInternalError
//	"Out of memory. To attempt Save, close other apps and click OK. To quit without saving click Cancel", kstidOutOfMemory
// END


// Date qualifier string IDs:
#define	kstidDateQualBefore				24200
#define	kstidDateQualOn					24201
#define	kstidDateQualAbout				24202
#define	kstidDateQualAfter				24203
#define	kstidDateQualAbt				24204
#define	kstidDateQualBC					24205
#define	kstidDateQualAD					24206
#define	kstidDateBlank					24207
#define	kstidDateBlankM					24208
#define	kstidDateBlankD					24209


#define kstidInternalError			25900
#define khcidHelpOutOfMemory		25901
#define khcidNoHelpAvailable		25902 // default help module when there is no specific one.
#define kstidOutOfMemory			25903
#define kstidUndoFrame				25904
#define kstidRedoFrame				25905

#define kstidFileErrUnknown			25920
#define kstidFileErrNotFound		25921
#define kstidFileErrPathNotFound	25922
#define kstidFileErrTooManyFiles	25923
#define kstidFileErrAccDenied		25924
#define kstidFileErrBadHandle		25925
#define kstidFileErrBadDrive		25926
#define kstidFileErrWriteProtect	25927
#define kstidFileErrBadUnit			25928
#define kstidFileErrNotReady		25929
#define kstidFileErrSeek			25930
#define kstidFileErrNotDosDisk		25931
#define kstidFileErrBadSector		25932
#define kstidFileErrWriteFault		25933
#define kstidFileErrReadFault		25934
#define kstidFileErrGeneral			25935
#define kstidFileErrSharing			25936
#define kstidFileErrLock			25937
#define kstidFileErrEof				25938
#define kstidFileErrHandleDiskFull	25939
#define kstidFileErrBadNetPath		25940
#define kstidFileErrNetworkBusy		25941
#define kstidFileErrNoDevice		25942
#define kstidFileErrNoNetAccess		25943
#define kstidFileErrBadDevice		25944
#define kstidFileErrBadNetName		25945
#define kstidFileErrExists			25946
#define kstidFileErrCantMake		25947
#define kstidFileErrBadPassword		25948
#define kstidFileErrNetWriteFault	25949
#define kstidFileErrDriveLocked		25950
#define kstidFileErrOpenFailed		25951
#define kstidFileErrBufOverflow		25952
#define kstidFileErrDiskFull		25953
#define kstidFileErrBadName			25954
#define kstidFileErrNoVolLabel		25955
#define kstidFileErrAlreadyExists	25956

#endif // !GenericResource_H
