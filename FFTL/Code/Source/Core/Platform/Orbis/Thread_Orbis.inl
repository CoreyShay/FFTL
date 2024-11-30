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

#ifndef _FFTL_THREAD_ORBIS_INL
#define _FFTL_THREAD_ORBIS_INL



namespace FFTL
{



inline ThreadId GetThreadIdCurrent()
{
	return ::scePthreadGetthreadid();
}

inline ThreadHandle GetThreadHandleCurrent()
{
	return ::scePthreadSelf();
}

inline void JoinThread(ThreadHandle h)
{
	FFTL_VERIFY_EQ( SCE_OK, ::scePthreadJoin(h, nullptr) );
}

inline void FreeThreadHandle(ThreadHandle h)
{
	(void)h;
}

inline void SetThreadName(ThreadHandle handle, const char* pszName)
{
	FFTL_VERIFY_EQ(SCE_OK, ::scePthreadRename(handle, pszName));
}

inline ThreadId CreateAndStartThread(ThreadHandle* outThreadHandle, FFTL_THREAD_RETURNTYPE (FFTL_THREAD_CALLCONV *pFunc)(void*), void* pParam, ThreadPriority priority, uint coreAffinityMask, const char* pszName, u32 stackSize)
{
	sched_param schedParam;

	const int schedMin = sched_get_priority_min(SCHED_FIFO);
	const int schedMax = sched_get_priority_max(SCHED_FIFO);
	const int schedAvg = (schedMin + schedMax) / 2;

	switch (priority)
	{
	case ThreadPriority::Lowest:	schedParam.sched_priority = schedMin;	break;
	case ThreadPriority::VeryLow:	schedParam.sched_priority = schedAvg-2;	break;
	case ThreadPriority::Low:		schedParam.sched_priority = schedAvg-1;	break;
	case ThreadPriority::Normal:	schedParam.sched_priority = schedAvg;	break;
	case ThreadPriority::High:		schedParam.sched_priority = schedAvg+1;	break;
	case ThreadPriority::VeryHigh:	schedParam.sched_priority = schedAvg+2;	break;
	case ThreadPriority::Highest:	schedParam.sched_priority = schedMax;	break;
	default:						schedParam.sched_priority = schedAvg;	break;
	}

	ScePthreadAttr attr;
	FFTL_VERIFY_EQ(SCE_OK, ::scePthreadAttrInit(&attr));

	FFTL_VERIFY_EQ(SCE_OK, ::scePthreadAttrSetinheritsched(&attr, SCE_PTHREAD_EXPLICIT_SCHED));
	FFTL_VERIFY_EQ(SCE_OK, ::scePthreadAttrSetschedpolicy(&attr, SCHED_FIFO));
	FFTL_VERIFY_EQ(SCE_OK, ::scePthreadAttrSetdetachstate(&attr, outThreadHandle ? SCE_PTHREAD_CREATE_JOINABLE : SCE_PTHREAD_CREATE_DETACHED));
	FFTL_VERIFY_EQ(SCE_OK, ::scePthreadAttrSetschedparam(&attr, &schedParam));
	
	if (stackSize > 0)
		FFTL_VERIFY_EQ(SCE_OK, ::scePthreadAttrSetstacksize(&attr, stackSize));

	if (coreAffinityMask != 0)
		FFTL_VERIFY_EQ(SCE_OK, ::scePthreadAttrSetaffinity(&attr, coreAffinityMask));

	ThreadHandle handle;
	FFTL_VERIFY_EQ(SCE_OK, ::scePthreadCreate(&handle, &attr, pFunc, pParam, pszName));
	FFTL_VERIFY_EQ(SCE_OK, ::scePthreadAttrDestroy(&attr));

	if (outThreadHandle)
		*outThreadHandle = handle;

	//	The thread id is actually the first data member behind the handle pointer.
	return *reinterpret_cast<ThreadId*>(handle);
}

inline void TerminateThread(ThreadId id)
{
	FFTL_ASSERT_MSG(0, "[TerminateThread(ThreadId)] Not supported on this platform");
	(void)id;
}
inline void TerminateThread(ThreadHandle handle)
{
	FFTL_VERIFY_EQ(SCE_OK, ::pthread_cancel(handle));
}



} // namespace FFTL

#endif //_FFTL_THREAD_ORBIS_INL
