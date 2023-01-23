//
// Debug Helpers
//
// Copyright (c) 2015 - 2017 Sean Farrell <sean.farrell@rioki.org>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#ifndef CORE_STACKTRACE_H
#define CORE_STACKTRACE_H

#include "Core/Allocator.h"
#include "Core/Macros.h"

#include <intrin.h>
#include <dbghelp.h>
#include <EASTL/vector.h>
#include <EASTL/string.h>

#if PLATFORM_WINDOWS
#pragma comment(lib, "dbghelp.lib")

#define STACK_TRACE(MSG, ...) ::StackTrace(MSG, __VA_ARGS__)

INLINE void StackTrace(const char* Msg, ...)
{
	char	lBuff[1024];
	va_list args;
	va_start(args, Msg);
	vsnprintf(lBuff, 1024, Msg, args);
	OutputDebugStringA(lBuff);
	va_end(args);
}

INLINE eastl::string Basename(const eastl::string& File)
{
	if (const size_t lI = File.find_last_of("\\/"); lI == eastl::string::npos)
	{
		return File;
	}
	else
	{
		return File.substr(lI + 1);
	}
}

struct StackFrame
{
	DWORD64		  Address;
	eastl::string Name;
	eastl::string Module;
	uint32_t	  Line;
	eastl::string File;
};

INLINE eastl::vector<StackFrame> StackTrace()
{
#if _WIN64
	DWORD lMachine = IMAGE_FILE_MACHINE_AMD64;
#else
	DWORD	   machine	   = IMAGE_FILE_MACHINE_I386;
#endif
	HANDLE lProcess = GetCurrentProcess();
	HANDLE lThread	= GetCurrentThread();

	if (SymInitialize(lProcess, NULL, TRUE) == FALSE)
	{
		STACK_TRACE(__FUNCTION__ ": Failed to call SymInitialize.");
		return eastl::vector<StackFrame>{DEBUG_NAME_VAL("StackTrace")};
	}

	SymSetOptions(SYMOPT_LOAD_LINES);

	CONTEXT lContext	  = {};
	lContext.ContextFlags = CONTEXT_FULL;
	RtlCaptureContext(&lContext);

#if _WIN64
	STACKFRAME lFrame		= {};
	lFrame.AddrPC.Offset	= lContext.Rip;
	lFrame.AddrPC.Mode		= AddrModeFlat;
	lFrame.AddrFrame.Offset = lContext.Rbp;
	lFrame.AddrFrame.Mode	= AddrModeFlat;
	lFrame.AddrStack.Offset = lContext.Rsp;
	lFrame.AddrStack.Mode	= AddrModeFlat;
#else
	STACKFRAME frame	   = {};
	frame.AddrPC.Offset	   = context.Eip;
	frame.AddrPC.Mode	   = AddrModeFlat;
	frame.AddrFrame.Offset = context.Ebp;
	frame.AddrFrame.Mode   = AddrModeFlat;
	frame.AddrStack.Offset = context.Esp;
	frame.AddrStack.Mode   = AddrModeFlat;
#endif

	bool lFirst = true;

	eastl::vector<StackFrame> lFrames{DEBUG_NAME_VAL("StackTrace")};
	lFrames.reserve(16ull);
	while (StackWalk(lMachine, lProcess, lThread, &lFrame, &lContext, NULL, SymFunctionTableAccess, SymGetModuleBase,
					 NULL))
	{
		StackFrame lF = {};
		lF.Address	  = lFrame.AddrPC.Offset;

#if _WIN64
		DWORD64 lModuleBase = 0;
#else
		  DWORD moduleBase = 0;
#endif

		lModuleBase = SymGetModuleBase(lProcess, lFrame.AddrPC.Offset);

		if (char lModuelBuff[MAX_PATH];
			lModuleBase && GetModuleFileNameA(reinterpret_cast<HINSTANCE>(lModuleBase), lModuelBuff, MAX_PATH))
		{
			lF.Module = Basename(lModuelBuff);
		}
		else
		{
			lF.Module = "Unknown Module";
		}
#if _WIN64
		DWORD64 lOffset = 0;
#else
		  DWORD offset	   = 0;
#endif
		char			 lSymbolBuffer[sizeof(IMAGEHLP_SYMBOL) + 255];
		PIMAGEHLP_SYMBOL lSymbol = reinterpret_cast<PIMAGEHLP_SYMBOL64>(lSymbolBuffer);
		lSymbol->SizeOfStruct	 = (sizeof IMAGEHLP_SYMBOL) + 255;
		lSymbol->MaxNameLength	 = 254;

		if (SymGetSymFromAddr(lProcess, lFrame.AddrPC.Offset, &lOffset, lSymbol))
		{
			lF.Name = lSymbol->Name;
		}
		else
		{
			DWORD lError = GetLastError();
			STACK_TRACE(__FUNCTION__ ": Failed to resolve address 0x%X: %u\n", lFrame.AddrPC.Offset, lError);
			lF.Name = "Unknown Function";
		}

		IMAGEHLP_LINE lIne;
		lIne.SizeOfStruct = sizeof(IMAGEHLP_LINE);

		DWORD lOffsetLn = 0;
		if (SymGetLineFromAddr(lProcess, lFrame.AddrPC.Offset, &lOffsetLn, &lIne))
		{
			lF.File = lIne.FileName;
			lF.Line = lIne.LineNumber;
		}
		else
		{
			DWORD lError = GetLastError();
			STACK_TRACE(__FUNCTION__ ": Failed to resolve line for 0x%X: %u\n", lFrame.AddrPC.Offset, lError);
			lF.Line = 0;
		}

		if (!lFirst)
		{
			lFrames.push_back(lF);
		}
		lFirst = false;
	}

	SymCleanup(lProcess);

	return lFrames;
}

INLINE eastl::string GetStackTraceString()
{
	eastl::string					 lString{DEBUG_NAME_VAL("StackTrace")};
	char							 lBuffer[1024ull];
	const eastl::vector<StackFrame>& lStackFrames = StackTrace();
	lString.reserve(lStackFrames.size() * sizeof lBuffer);
	for (auto lIt = lStackFrames.begin() + 1; lIt < lStackFrames.cend(); ++lIt)
	{
		strcpy(lBuffer, "");
		snprintf(lBuffer, sizeof lBuffer, "%llX: %s (%u) in %s\n", lIt->Address, lIt->Name.c_str(), lIt->Line,
				 lIt->Module.c_str());
		lString.append(lBuffer);
	}
	return lString;
}

#endif

#endif
