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

#if defined(__ANDROID__)
#	include <android/log.h>
#else
#	include <stdio.h>
#endif

namespace FFTL
{

#if defined(__ANDROID__)
#	define FFTL_LOG_MSG(...)					((void)__android_log_print(ANDROID_LOG_INFO,			"FFTL", __VA_ARGS__))
#	define FFTL_LOGV_MSG(_msg_, _argList_)		((void)__android_log_vprint(ANDROID_LOG_INFO,			"FFTL", _msg_, _argList_))
#	define FFTL_LOG_ERR(...)					((void)__android_log_print(ANDROID_LOG_ERROR,			"FFTL", __VA_ARGS__))
#	define FFTL_LOGV_ERR(_msg_, _argList_)		((void)__android_log_vprint(ANDROID_LOG_ERROR,			"FFTL", _msg_, _argList_))
#	define FFTL_LOG_WRN(...)					((void)__android_log_print(ANDROID_LOG_WARN,			"FFTL", __VA_ARGS__))
#	define FFTL_LOGV_WRN(_msg_, _argList_)		((void)__android_log_vprint(ANDROID_LOG_WARN,			"FFTL", _msg_, _argList_))
#	if defined(FFTL_ENABLE_ASSERT)
#		define FFTL_LOG_VRB(...)				((void)__android_log_print(ANDROID_LOG_VERBOSE,			"FFTL", __VA_ARGS__))
#		define FFTL_LOGV_VRB(_msg_, _argList_)	((void)__android_log_vprint(ANDROID_LOG_VERBOSE,		"FFTL", _msg_, _argList_))
#	endif
#else
#	define FFTL_LOG_MSG(...)					((void)printf(__VA_ARGS__))
#	define FFTL_LOGV_MSG(_msg_, _argList_)		((void)vprintf(_msg_, _argList_))
#	define FFTL_LOG_ERR(...)					((void)printf(__VA_ARGS__))
#	define FFTL_LOGV_ERR(_msg_, _argList_)		((void)vprintf(_msg_, _argList_))
#	define FFTL_LOG_WRN(...)					((void)printf(__VA_ARGS__))
#	define FFTL_LOGV_WRN(_msg_, _argList_)		((void)vprintf(_msg_, _argList_))
#	if defined(FFTL_ENABLE_ASSERT)
#		define FFTL_LOG_VRB(...)				((void)printf(__VA_ARGS__))
#		define FFTL_LOGV_VRB(_msg_, _argList_)	((void)vprintf(_msg_, _argList_))
#	endif
#endif

#if !defined(FFTL_ENABLE_ASSERT)
#	define FFTL_LOG_VRB(...) ((void)0)
#	define FFTL_LOGV_VRB(_msg_, _argList_) ((void)0)
#endif

}

