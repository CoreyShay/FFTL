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

#ifndef _FFTL_THREAD_POSIX_INL
#define _FFTL_THREAD_POSIX_INL



namespace FFTL
{

inline void SetThreadName(ThreadHandle handle, const char* pszName)
{
#if !defined(__ORBIS__) && (!defined(__ANDROID_API__) || __ANDROID_API__ >= 9)
	::pthread_setname_np(handle.GetId(), pszName);
#else
	(void)id; (void)pszName;
#endif
}

inline ThreadId CreateAndStartThread(ThreadHandle* outThreadHandle, FFTL_THREAD_RETURNTYPE (FFTL_THREAD_CALLCONV *pFunc)(void*), void* pParam, enThreadPriority priority, uint coreAffinityMask, const char* pszName, u32 stackSize)
{
	(void)coreAffinityMask;

	sched_param schedParam;

	const int schedMin = sched_get_priority_min(SCHED_FIFO);
	const int schedMax = sched_get_priority_max(SCHED_FIFO);
	const int schedAvg = (schedMin + schedMax) / 2;

	switch (priority)
	{
	case kThreadPriorityLowest:		schedParam.sched_priority = schedMin;	break;
	case kThreadPriorityVeryLow:	schedParam.sched_priority = schedAvg-2;	break;
	case kThreadPriorityLow:		schedParam.sched_priority = schedAvg-1;	break;
	case kThreadPriorityNormal:		schedParam.sched_priority = schedAvg;	break;
	case kThreadPriorityHigh:		schedParam.sched_priority = schedAvg+1;	break;
	case kThreadPriorityVeryHigh:	schedParam.sched_priority = schedAvg+2;	break;
	case kThreadPriorityHighest:	schedParam.sched_priority = schedMax;	break;
	default:						schedParam.sched_priority = schedAvg;	break;
	}

	pthread_attr_t attr;
	FFTL_VERIFY_EQ(0, ::pthread_attr_init(&attr));

#if !defined (__ANDROID__)
	FFTL_VERIFY_EQ(0, ::pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED));
	FFTL_VERIFY_EQ(0, ::pthread_attr_setschedpolicy(&attr, SCHED_FIFO));
#endif
	FFTL_VERIFY_EQ(0, ::pthread_attr_setdetachstate(&attr, outThreadHandle ? PTHREAD_CREATE_JOINABLE : PTHREAD_CREATE_DETACHED));
	FFTL_VERIFY_EQ(0, ::pthread_attr_setschedparam(&attr, &schedParam));
	
	if (stackSize > 0)
		FFTL_VERIFY_EQ(0, ::pthread_attr_setstacksize(&attr, stackSize));

#if 0 // no set affinity exists yet on posix
	if (coreAffinityMask != 0)
		FFTL_VERIFY_EQ(0, ::pthread_attr_setaffinity_np(&attr, sizeof(coreAffinityMask), (const cpu_set_t*)coreAffinityMask));
#else
	(void)coreAffinityMask;
#endif

	ThreadId id;
	FFTL_VERIFY_EQ(0, ::pthread_create(&id, &attr, pFunc, pParam));
	FFTL_VERIFY_EQ(0, ::pthread_attr_destroy(&attr));

#if 0
	FFTL_VERIFY_EQ(0, ::pthread_setname_np(id, pszName));
#else
	(void)pszName;
#endif

	if (outThreadHandle)
		*outThreadHandle = id;

	return id;
}



} // namespace FFTL

#endif //_FFTL_THREAD_POSIX_INL