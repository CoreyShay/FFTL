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

#ifndef _FFTL_THREAD_WINDOWS_INL
#define _FFTL_THREAD_WINDOWS_INL



namespace FFTL
{


inline ThreadId GetThreadIdCurrent()
{
	return ::GetCurrentThreadId();
}

inline ThreadHandle GetThreadHandleCurrent()
{
	return ::GetCurrentThread();
}

inline void JoinThread(ThreadHandle h)
{
	FFTL_VERIFY_EQ( WAIT_OBJECT_0, ::WaitForSingleObject(h, INFINITE) );
}

inline void FreeThreadHandle(ThreadHandle h)
{
	FFTL_VERIFY_EQ( TRUE, ::CloseHandle(h) );
}

inline void SetThreadName(ThreadHandle handle, const char* pszName)
{
	typedef struct tagTHREADNAME_INFO
	{
		DWORD dwType;     // Must be 0x1000
		LPCSTR szName;    // Pointer to name (in user address space)
		DWORD dwThreadID; // Thread ID (-1 for caller thread)
		DWORD dwFlags;    // Reserved for future use; must be zero
	} THREADNAME_INFO;

	THREADNAME_INFO info;

	info.dwType = 0x1000;
	info.szName = pszName;
	info.dwThreadID = ::GetThreadId(handle);
	info.dwFlags = 0;

	__try
	{
		RaiseException(0x406D1388, 0, sizeof(info)/sizeof(DWORD), (const ULONG_PTR *)&info);
	}
	__except (GetExceptionCode()==0x406D1388 ? EXCEPTION_CONTINUE_EXECUTION : EXCEPTION_EXECUTE_HANDLER)
	{
	}
}

inline ThreadId CreateAndStartThread(ThreadHandle* outThreadHandle, FFTL_THREAD_RETURNTYPE (FFTL_THREAD_CALLCONV *pFunc)(void*), void* pParam, ThreadPriority priority, uint coreAffinityMask, const char* pszName, u32 stackSize)
{
	ThreadId id;
	HANDLE handle = ::CreateThread(nullptr, stackSize, pFunc, pParam, CREATE_SUSPENDED, &id);
	FFTL_ASSERT(handle);

	int osPriority;
	switch (priority)
	{
	case ThreadPriority::Lowest:	osPriority = THREAD_BASE_PRIORITY_IDLE;		break;
	case ThreadPriority::VeryLow:	osPriority = THREAD_PRIORITY_LOWEST;		break;
	case ThreadPriority::Low:		osPriority = THREAD_PRIORITY_BELOW_NORMAL;	break;
	case ThreadPriority::Normal:	osPriority = THREAD_PRIORITY_NORMAL;		break;
	case ThreadPriority::High:		osPriority = THREAD_PRIORITY_ABOVE_NORMAL;	break;
	case ThreadPriority::VeryHigh:	osPriority = THREAD_PRIORITY_HIGHEST;		break;
	case ThreadPriority::Highest:	osPriority = THREAD_PRIORITY_TIME_CRITICAL;	break;
	default:						osPriority = THREAD_PRIORITY_NORMAL;		break;
	}

	FFTL_VERIFY(::SetThreadPriority(handle, osPriority));

	if (coreAffinityMask != 0)
		FFTL_VERIFY_NEQ(0, ::SetThreadAffinityMask(handle, (DWORD_PTR)coreAffinityMask));

	FFTL_VERIFY_NEQ((DWORD)-1, ::ResumeThread(handle));

	//	Must be set after the thread has been resumed.
	if (pszName != nullptr)
		SetThreadName(handle, pszName);

	if (outThreadHandle != nullptr)
		*outThreadHandle = handle;
	else
		::CloseHandle(handle); // Since we won't need the handle anymore...

	return id;
}

inline void PauseThread(ThreadId id)
{
	ThreadHandle handle = ::OpenThread(THREAD_SUSPEND_RESUME, FALSE, id);
	FFTL_ASSERT(handle);
	FFTL_VERIFY_NEQ( -1, ::SuspendThread(handle) );
	FFTL_VERIFY_EQ( TRUE, ::CloseHandle(handle) );
}
inline void PauseThread(ThreadHandle handle)
{
	FFTL_VERIFY_NEQ(-1, ::SuspendThread(handle) );
}

inline void UnpauseThread(ThreadId id)
{
	ThreadHandle handle = ::OpenThread(THREAD_SUSPEND_RESUME, FALSE, id);
	FFTL_ASSERT(handle);
	FFTL_VERIFY_NEQ(-1, ::ResumeThread(handle) );
	FFTL_VERIFY_EQ( TRUE, ::CloseHandle(handle) );
}
inline void UnpauseThread(ThreadHandle handle)
{
	FFTL_VERIFY_NEQ(-1, ::ResumeThread(handle) );
}

inline void TerminateThread(ThreadId id)
{
	ThreadHandle handle = ::OpenThread(THREAD_TERMINATE, FALSE, id);
	FFTL_ASSERT(handle);
	FFTL_VERIFY_NEQ(-1, ::TerminateThread(handle, 0));
	FFTL_VERIFY_EQ(TRUE, ::CloseHandle(handle));
}
inline void TerminateThread(ThreadHandle handle)
{
	FFTL_VERIFY_NEQ(-1, ::TerminateThread(handle, 0));
}


} // namespace FFTL

#endif //_FFTL_THREAD_WINDOWS_INL
