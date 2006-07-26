/*
	TECkit REALbasic plugin
	
	Jonathan Kew
	25-Jan-2002
*/

/*
	Implements two REALbasic classes:
		TECkit_Compiler
		TECkit_Engine
	as well as the Win32 utility function
		WinNewDoc(long appWindowHWND, REALstring docPathName)
	which creates a new instance of the current app, launching it with the given document
*/

#include "rb_plugin.h"

#include <string.h>
#include <stdlib.h>

#pragma mark === Compiler ===

#include "TECkit_Compiler.h"

extern REALclassDefinition TECkit_Compiler_Class;

struct TECkit_Compiler_Class_Data {
	Byte*	compiledTable;
	UInt32	compiledSize;
};

static REALevent
TECkit_Compiler_Class_Events[] = {
	{ "ErrorMessage(msg as String, param as String, lineNo as Integer)" },	// 0
};

static
CALLBACK
void
errorFunction(void* userData, char* msg, char* param, UInt32 line)
{
	REALstring	msgStr = REALBuildString(msg, strlen(msg));
	REALstring	paramStr = (param != 0)
							? REALBuildString(param, strlen(param))
							: REALBuildString("", 0);

    REALcontrolInstance instance = (REALcontrolInstance)userData;
	void (*fp)(REALcontrolInstance, REALstring, REALstring, int);
	fp = (void (*)(REALcontrolInstance, REALstring, REALstring, int))
							REALGetEventInstance(instance, &TECkit_Compiler_Class_Events[0]);
	if (fp != 0)
		fp(instance, msgStr, paramStr, (int)line);

	REALUnlockString(paramStr);
	REALUnlockString(msgStr);
}

static int
myCompile(REALobject instance, REALstring s, long doCompression)
{
	if (s->Length() <= 0)
		return 0;

	ClassData(TECkit_Compiler_Class, instance, TECkit_Compiler_Class_Data, data);

	Byte*	outTable;
	UInt32	outLen;
	REALLockString(s);
	TECkit_Status	status = TECkit_Compile((char*)REALCString(s), s->Length(), doCompression,
											errorFunction, instance, &outTable, &outLen);
	REALUnlockString(s);

	if (status == kStatus_NoError) {
		data->compiledTable = outTable;
		data->compiledSize = outLen;
	}

	return status;
}

static void
myReset(REALobject instance)
{
	ClassData(TECkit_Compiler_Class, instance, TECkit_Compiler_Class_Data, data);

	if (data->compiledTable != 0) {
		TECkit_DisposeCompiled(data->compiledTable);
		data->compiledTable = 0;
		data->compiledSize = 0;
	}
}

static REALstring
myTableGetter(REALobject instance, long param)
{
#pragma unused(param)

	ClassData(TECkit_Compiler_Class, instance, TECkit_Compiler_Class_Data, data);
	if (data->compiledTable != 0) {
		return REALBuildString((char*)data->compiledTable, data->compiledSize);
	}
	return nil;
}

static void
TECkit_Compiler_Class_Constructor(REALobject instance)
{
	ClassData(TECkit_Compiler_Class, instance, TECkit_Compiler_Class_Data, data);
	data->compiledTable = 0;
	data->compiledSize = 0;
}

static void
TECkit_Compiler_Class_Destructor(REALobject instance)
{
	ClassData(TECkit_Compiler_Class, instance, TECkit_Compiler_Class_Data, data);
	if (data->compiledTable != 0) {
		TECkit_DisposeCompiled(data->compiledTable);
		data->compiledTable = 0;
		data->compiledSize = 0;
	}
}

static REALproperty 
TECkit_Compiler_Class_Properties[] = {
	{ nil, "CompiledTable", "String", 0, (REALproc)myTableGetter, nil, 0 },
	{ nil, "CompiledSize", "Integer", 0, REALstandardGetter, nil,
							FieldOffset(TECkit_Compiler_Class_Data, compiledSize) },
};

static REALmethodDefinition
TECkit_Compiler_Class_Methods[] = {
	{ (REALproc)TECkit_Compiler_Class_Constructor, REALnoImplementation, "TECkit_Compiler()" },
	{ (REALproc)myCompile, REALnoImplementation, "Compile(data as String, doCompression as Integer) as Integer" },	
	{ (REALproc)myReset, REALnoImplementation, "Reset()" },
};

static REALclassDefinition
TECkit_Compiler_Class = {
	kCurrentREALControlVersion,
	"TECkit_Compiler",
	nil,
	sizeof(TECkit_Compiler_Class_Data),
	0,
	nil,
	(REALproc)TECkit_Compiler_Class_Destructor,
	TECkit_Compiler_Class_Properties,
	sizeof(TECkit_Compiler_Class_Properties) / sizeof(REALproperty),
	TECkit_Compiler_Class_Methods,
	sizeof(TECkit_Compiler_Class_Methods) / sizeof(REALmethodDefinition),
	TECkit_Compiler_Class_Events,
	sizeof(TECkit_Compiler_Class_Events) / sizeof(REALevent)
};

#pragma mark === Engine ===

#include "TECkit_Engine.h"

extern REALclassDefinition TECkit_Engine_Class;

struct TECkit_Engine_Class_Data {
	TECkit_Converter	converter;
	TECkit_Status		status;
	UInt32				sourceFlags;
	UInt32				targetFlags;
};

static void
TECkit_Engine_Class_Constructor(REALobject instance, REALstring table, Boolean fwd, long inputForm, long outputForm)
{
	ClassData(TECkit_Engine_Class, instance, TECkit_Engine_Class_Data, data);
	data->converter = 0;
	data->status = TECkit_CreateConverter((Byte*)table->CString(), table->Length(), fwd, inputForm, outputForm, &data->converter);
	if (data->status == kStatus_NoError)
		data->status = TECkit_GetConverterFlags(data->converter, &data->sourceFlags, &data->targetFlags);
}

static void
TECkit_Engine_Class_Destructor(REALobject instance)
{
	ClassData(TECkit_Engine_Class, instance, TECkit_Engine_Class_Data, data);
	if (data->converter != 0) {
		TECkit_DisposeConverter(data->converter);
		data->converter = 0;
		data->status = kStatus_InvalidConverter;
	}
}

static REALstring
myConvert(REALobject instance, REALstring inputData, Boolean complete)
{
	ClassData(TECkit_Engine_Class, instance, TECkit_Engine_Class_Data, data);
	if (data->converter == 0)
		REALRaiseException(REALnewInstance("NilObjectException"));
	else {
		UInt32	outLength = inputData->Length() * 8 + 8;	// should be plenty, we hope
		Byte*	outBuffer;
		UInt32	inUsed, outUsed;

		REALmemoryBlock	outBufferBlock;
		while (1) {
			outBufferBlock = REALNewMemoryBlock(outLength);
			outBuffer = (Byte*)REALMemoryBlockGetPtr(outBufferBlock);
			data->status = TECkit_ConvertBuffer(data->converter,
												(Byte*)inputData->CString(), inputData->Length(), &inUsed,
												outBuffer, outLength, &outUsed, complete);
			if (data->status == kStatus_OutputBufferFull) {
				// try again with a bigger buffer
				REALUnlockObject((REALobject)outBufferBlock);
				outLength <<= 1;
				continue;
			}
			break;
		}
		
		REALstring	result = REALBuildString((char*)outBuffer, outUsed);
		REALUnlockObject((REALobject)outBufferBlock);

		if (complete)
			TECkit_ResetConverter(data->converter);

		return result;
	}
	return nil;
}

static REALstring
myNameGetter(REALobject instance, long param)
{
	ClassData(TECkit_Engine_Class, instance, TECkit_Engine_Class_Data, data);
	if (data->converter != 0) {
		const UInt16	bufferSize = 256;
		Byte			nameBuffer[bufferSize];
		UInt32			nameLength;
		data->status = TECkit_GetConverterName(data->converter,
												param,
												nameBuffer,
												bufferSize,
												&nameLength);
		if (data->status == kStatus_NoError)
			return REALBuildString((char*)nameBuffer, nameLength);
	}
	return nil;
}

static REALproperty
TECkit_Engine_Class_Properties[] = {
	{ nil, "status", "Integer", 0, REALstandardGetter, nil, FieldOffset(TECkit_Engine_Class_Data, status) },
	{ nil, "lhsName", "String", 0, (REALproc)myNameGetter, nil, kNameID_LHS_Name },
	{ nil, "rhsName", "String", 0, (REALproc)myNameGetter, nil, kNameID_RHS_Name },
	{ nil, "sourceFlags", "Integer", 0, REALstandardGetter, nil, FieldOffset(TECkit_Engine_Class_Data, sourceFlags) },
	{ nil, "targetFlags", "Integer", 0, REALstandardGetter, nil, FieldOffset(TECkit_Engine_Class_Data, targetFlags) },
};

static REALmethodDefinition
TECkit_Engine_Class_Methods[] = {
	{ (REALproc)TECkit_Engine_Class_Constructor, REALnoImplementation, "TECkit_Engine(table as String, fwd as Boolean, inputForm as Integer, outputForm as Integer)" },
	{ (REALproc)myConvert, REALnoImplementation, "Convert(inputData as String, complete as Boolean) as String" },
};

static REALevent
TECkit_Engine_Class_Events[] = {
	{ "ErrorMessage(msg as String, param as String, lineNo as Integer)" },	// 0
};

static REALclassDefinition
TECkit_Engine_Class = {
	kCurrentREALControlVersion,
	"TECkit_Engine",
	nil,
	sizeof(TECkit_Engine_Class_Data),
	0,
	nil,
	(REALproc)TECkit_Engine_Class_Destructor,
	TECkit_Engine_Class_Properties,	sizeof(TECkit_Engine_Class_Properties) / sizeof(REALproperty),
	TECkit_Engine_Class_Methods,	sizeof(TECkit_Engine_Class_Methods) / sizeof(REALmethodDefinition),
	TECkit_Engine_Class_Events,		sizeof(TECkit_Engine_Class_Events) / sizeof(REALevent)
};

#pragma mark === WinNewDoc ===

#if TARGET_WIN32
#include <winable.h>	// GetWindowModuleFileName is in one or other of these, depending on WINVER
#include <winuser.h>
#endif

static int
WinNewDoc(long appWindowHWND, REALstring docPathName)
{
#if TARGET_CARBON
#pragma unused(appWindowHWND, docPathName)
#endif
#if TARGET_WIN32
	char*	docCStr = "";
	if (docPathName != 0)
		docCStr = (char*)docPathName->CString();

	const int	kFileNameLimit = 2048;
	char		appFileName[kFileNameLimit];
	int			fileNameLen = GetWindowModuleFileName((HWND)appWindowHWND, appFileName, kFileNameLimit);
	appFileName[fileNameLen] = 0;
	
	int	len = 0;
	for (char* dp = docCStr; *dp; ++dp)
		if (*dp == '\\')
			len += 2;
		else
			++len;
	len += 2;	// for quotes around doc name
	len += 1;	// for space between app and doc names
	char*	ap = &appFileName[fileNameLen] - 1;
	while (ap >= &appFileName[0] && *ap != '\\' && *ap != '/' && *ap != ':') {
		if (*ap == '\\')
			len += 2;
		else
			len += 1;
		--ap;
	}
	len += 2;	// for quotes around app name
	char*	cmdLine = (char*)malloc(len + 1);
	if (cmdLine == 0)
		return 0;
	char*	cp = cmdLine;

	*cp++ = '"';
	while (*++ap) {
		if (*ap == '\\')
			*cp++ = '\\';
		*cp++ = *ap;
	}
	*cp++ = '"';
	*cp++ = ' ';

	*cp++ = '"';
	ap = docCStr;
	while (*ap) {
		if (*ap == '\\')
			*cp++ = '\\';
		*cp++ = *ap++;
	}
	*cp++ = '"';
	*cp = 0;
	
	STARTUPINFO startupInfo = {
    	sizeof(STARTUPINFO),	//DWORD   cb;
		0,	//LPSTR   lpReserved;
		0,	//LPSTR   lpDesktop;
		0,	//LPSTR   lpTitle;
		0,	//DWORD   dwX;
		0,	//DWORD   dwY;
		0,	//DWORD   dwXSize;
		0,	//DWORD   dwYSize;
		0,	//DWORD   dwXCountChars;
		0,	//DWORD   dwYCountChars;
		0,	//DWORD   dwFillAttribute;
		STARTF_USESHOWWINDOW,	//DWORD   dwFlags;
		SW_SHOWDEFAULT,	//WORD    wShowWindow;
		0,	//WORD    cbReserved2;
		0,	//LPBYTE  lpReserved2;
		0,	//HANDLE  hStdInput;
		0,	//HANDLE  hStdOutput;
		0	//HANDLE  hStdError;
	};
	PROCESS_INFORMATION	processInfo;
	if (CreateProcess(
			appFileName,
			cmdLine,
			0,
			0,
			false,
			0,
			0,
			0,
			&startupInfo,
			&processInfo
	    )) {
	   	CloseHandle(processInfo.hProcess);
	   	CloseHandle(processInfo.hThread);
    }
	free(cmdLine);
#endif
	return 0;
}

static REALmethodDefinition
WinNewDoc_Method = {
	(REALproc)WinNewDoc, REALnoImplementation,
		"WinNewDoc(appWindowHWND as Integer, docPathName as String) as Integer"
};

#pragma mark === Plugin setup ===

#ifdef WIN32
	extern "C" far void _RunInit(void);
	#define __InitCode__ _RunInit
#elif defined(powerc)
	extern "C" void __sinit(void);
	#define __InitCode__ __sinit
#else // it's 68K
	extern "C" far void __InitCode__ (void);
#endif

void
PluginEntry(void)
{
	__InitCode__();		// calls constructors for static and global C++ objects
	REALRegisterClass(&TECkit_Compiler_Class);
	REALRegisterClass(&TECkit_Engine_Class);

	REALRegisterMethod(&WinNewDoc_Method);
}
