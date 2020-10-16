/*

Original author:
Corey Shay
corey@signalflowtechnologies.com

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

*/

#pragma once

#include "../defs.h"

#if defined(FFTL_PLATFORM_ANDROID)
#	include <android/log.h>
#else
#	include <cstdio>
#endif

namespace FFTL
{
#if defined(FFTL_PLATFORM_ANDROID)
	inline int LogMsgV(const char* _Format, va_list _ArgList) { return __android_log_vprint(ANDROID_LOG_INFO,		"FFTL", _Format, _ArgList); }
	inline int LogErrV(const char* _Format, va_list _ArgList) { return __android_log_vprint(ANDROID_LOG_ERROR,		"FFTL", _Format, _ArgList); }
	inline int LogWrnV(const char* _Format, va_list _ArgList) { return __android_log_vprint(ANDROID_LOG_WARN,		"FFTL", _Format, _ArgList); }
	inline int LogVrbV(const char* _Format, va_list _ArgList) { return __android_log_vprint(ANDROID_LOG_VERBOSE,	"FFTL", _Format, _ArgList); }
#else
	inline int LogMsgV(const char* _Format, va_list _ArgList) { return vprintf(_Format, _ArgList); }
	inline int LogErrV(const char* _Format, va_list _ArgList) { return vprintf(_Format, _ArgList); }
	inline int LogWrnV(const char* _Format, va_list _ArgList) { return vprintf(_Format, _ArgList); }
	inline int LogVrbV(const char* _Format, va_list _ArgList) { return vprintf(_Format, _ArgList); }

#	if defined(FFTL_WCHAR)
	inline int LogMsgV(const tchar* _Format, va_list _ArgList) { return vwprintf(_Format, _ArgList); }
	inline int LogErrV(const tchar* _Format, va_list _ArgList) { return vwprintf(_Format, _ArgList); }
	inline int LogWrnV(const tchar* _Format, va_list _ArgList) { return vwprintf(_Format, _ArgList); }
	inline int LogVrbV(const tchar* _Format, va_list _ArgList) { return vwprintf(_Format, _ArgList); }
#	endif
#endif

	inline int LogMsg(const char* _Format, ...) { int _Result; va_list _ArgList; va_start(_ArgList, _Format); _Result = LogMsgV(_Format, _ArgList); va_end(_ArgList); return _Result; }
	inline int LogErr(const char* _Format, ...) { int _Result; va_list _ArgList; va_start(_ArgList, _Format); _Result = LogErrV(_Format, _ArgList); va_end(_ArgList); return _Result; }
	inline int LogWrn(const char* _Format, ...) { int _Result; va_list _ArgList; va_start(_ArgList, _Format); _Result = LogWrnV(_Format, _ArgList); va_end(_ArgList); return _Result; }
	inline int LogVrb(const char* _Format, ...) { int _Result; va_list _ArgList; va_start(_ArgList, _Format); _Result = LogVrbV(_Format, _ArgList); va_end(_ArgList); return _Result; }

#if defined(FFTL_WCHAR)
	inline int LogMsg(const tchar* _Format, ...) { int _Result; va_list _ArgList; va_start(_ArgList, _Format); _Result = LogMsgV(_Format, _ArgList); va_end(_ArgList); return _Result; }
	inline int LogErr(const tchar* _Format, ...) { int _Result; va_list _ArgList; va_start(_ArgList, _Format); _Result = LogErrV(_Format, _ArgList); va_end(_ArgList); return _Result; }
	inline int LogWrn(const tchar* _Format, ...) { int _Result; va_list _ArgList; va_start(_ArgList, _Format); _Result = LogWrnV(_Format, _ArgList); va_end(_ArgList); return _Result; }
	inline int LogVrb(const tchar* _Format, ...) { int _Result; va_list _ArgList; va_start(_ArgList, _Format); _Result = LogVrbV(_Format, _ArgList); va_end(_ArgList); return _Result; }
#endif
}

#define FFTL_LOG_MSG(...)					((void)FFTL::LogMsg(__VA_ARGS__))
#define FFTL_LOGV_MSG(_msg_, _argList_)		((void)FFTL::LogMsgV(_msg_, _argList_))
#define FFTL_LOG_ERR(...)					((void)FFTL::LogErr(__VA_ARGS__))
#define FFTL_LOGV_ERR(_msg_, _argList_)		((void)FFTL::LogErrV(_msg_, _argList_))
#define FFTL_LOG_WRN(...)					((void)FFTL::LogWrn(__VA_ARGS__))
#define FFTL_LOGV_WRN(_msg_, _argList_)		((void)FFTL::LogWrnV(_msg_, _argList_))
#if defined(FFTL_ENABLE_ASSERT)
#	define FFTL_LOG_VRB(...)				((void)FFTL::LogVrb(__VA_ARGS__))
#	define FFTL_LOGV_VRB(_msg_, _argList_)	((void)FFTL::LogVrbV(_msg_, _argList_))
#endif

#if !defined(FFTL_ENABLE_ASSERT)
#	define FFTL_LOG_VRB(...) ((void)0)
#	define FFTL_LOGV_VRB(_msg_, _argList_) ((void)0)
#endif

