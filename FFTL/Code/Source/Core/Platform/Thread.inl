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

#ifndef _FFTL_THREAD_INL
#define _FFTL_THREAD_INL



namespace FFTL
{


//	Adding and subtracting integers? Why not?
constexpr inline ThreadPriority operator+(ThreadPriority t, int i)
{
	const int r = static_cast<int>(t) + i;
	FFTL_ASSERT(r >= static_cast<int>(ThreadPriority::Lowest) && r <= static_cast<int>(ThreadPriority::Highest));
	return static_cast<ThreadPriority>(r);
}
constexpr inline ThreadPriority operator-(ThreadPriority t, int i)
{
	const int r = static_cast<int>(t) - i;
	FFTL_ASSERT(r >= static_cast<int>(ThreadPriority::Lowest) && r <= static_cast<int>(ThreadPriority::Highest));
	return static_cast<ThreadPriority>(r);
}



#if !defined(_MSC_VER)
//	Only possible on MS compilers as far as I know right now...
inline void PauseThread(ThreadId id) { (void)id; }
inline void PauseThread(ThreadHandle handle) { (void)handle; }
inline void UnpauseThread(ThreadId id) { (void)id; }
inline void UnpauseThread(ThreadHandle handle) { (void)handle; }
#endif




inline void ThreadBase::Pause()
{
	PauseThread(m_Handle);
}

inline void ThreadBase::Unpause()
{
	UnpauseThread(m_Handle);
}

inline ThreadHandle ThreadBase::GetThreadHandle() const
{
	return m_Handle;
}

inline ThreadId ThreadBase::GetThreadId() const
{
#if !defined(FFTL_THREAD_HANDLE_IS_ID)
	return m_ID;
#else
	return m_Handle.GetId(); //	Handle and ID are the same for all non ms platforms.
#endif
}

inline ThreadResult ThreadBase::GetRunResult()
{
	FFTL_ASSERT(!GetIsRunning());
	return m_RunResult;
}

inline bool ThreadBase::GetIsRunning() const
{
	return m_bIsRunning;
}

inline bool ThreadBase::GetIsFlaggedForStop() const
{
	return m_bFlaggedForStop;
}



inline void ThreadMember::Start(ThreadOwner* pOwner, const char* pszName, ThreadPriority priority, uint coreAffinityMask, u32 stackSize)
{
	if (!GetIsRunning())
	{
		m_pOwner = pOwner;
		m_bFlaggedForStop = false;
#if defined(FFTL_THREAD_HANDLE_IS_ID)
		CreateAndStartThread(&m_Handle, &ThreadMember::InternalStartFunc, this, priority, coreAffinityMask, pszName, stackSize);
#else
		m_ID = CreateAndStartThread(&m_Handle, &ThreadMember::InternalStartFunc, this, priority, coreAffinityMask, pszName, stackSize);
#endif
	}
}

inline void ThreadMember::Join()
{
	if (m_Handle != 0)
	{
		m_bFlaggedForStop = true;
		JoinThread(m_Handle);

		m_pOwner = nullptr;
		FreeThreadHandle(m_Handle);
		m_Handle = 0;
#if !defined(FFTL_THREAD_HANDLE_IS_ID)
		m_ID = 0;
#endif
	}
}

inline void ThreadMember::Terminate()
{
	FFTL_ASSERT_MSG(GetIsRunning() && !m_bFlaggedForStop, "[ThreadMember::Terminate] Thread not running or flagged to stop. Not sure what could happen here...");
	if (m_Handle != 0)
	{
		TerminateThread(m_Handle);
		m_bIsRunning = false;
		FreeThreadHandle(m_Handle);
		m_pOwner = nullptr;
		m_Handle = 0;
#if !defined(FFTL_THREAD_HANDLE_IS_ID)
		m_ID = 0;
#endif
	}
}

inline void ThreadMember::Run()
{
	m_bIsRunning = true;
	m_RunResult = (m_pOwner->*m_funcRun)();
	m_bIsRunning = false;
}

inline FFTL_THREAD_RETURNTYPE FFTL_THREAD_CALLCONV ThreadMember::InternalStartFunc(void* pThis)
{
	auto* pThread = reinterpret_cast<ThreadMember*>(pThis);
	pThread->Run();
	return FFTL_THREAD_RETURNVALUE;
}



template <typename T_Functor>
inline void ThreadFunctor<T_Functor>::Start(const char* pszName, ThreadPriority priority, uint coreAffinityMask, u32 stackSize)
{
	if (!GetIsRunning())
	{
		m_bFlaggedForStop = false;
#if defined(FFTL_THREAD_HANDLE_IS_ID)
		CreateAndStartThread(&m_Handle, &ThreadFunctor<T_Functor>::InternalStartFunc, this, priority, coreAffinityMask, pszName, stackSize);
#else
		m_ID = CreateAndStartThread(&m_Handle, &ThreadFunctor<T_Functor>::InternalStartFunc, this, priority, coreAffinityMask, pszName, stackSize);
#endif
	}
}

template <typename T_Functor>
inline void ThreadFunctor<T_Functor>::Join()
{
	if (m_Handle != 0)
	{
		m_bFlaggedForStop = true;
		JoinThread(m_Handle);

		FreeThreadHandle(m_Handle);
		m_Handle = 0;
#if !defined(FFTL_THREAD_HANDLE_IS_ID)
		m_ID = 0;
#endif
	}
}

template <typename T_Functor>
inline void ThreadFunctor<T_Functor>::Terminate()
{
//	FFTL_ASSERT_MSG(GetIsRunning() && !m_bFlaggedForStop, "[ThreadMember::Terminate] Thread not running or flagged to stop. Not sure what could happen here...");
	if (m_Handle != 0)
	{
		TerminateThread(m_Handle);
		FreeThreadHandle(m_Handle);
		m_Handle = 0;
#if !defined(FFTL_THREAD_HANDLE_IS_ID)
		m_ID = 0;
#endif
		m_bIsRunning = false;
		m_bFlaggedForStop = false;
	}
}

template <typename T_Functor>
inline void ThreadFunctor<T_Functor>::Run()
{
	m_bIsRunning = true;
	m_funcRun();
	m_RunResult = ThreadResult::OK;
	m_bIsRunning = false;
}

template <typename T_Functor>
inline FFTL_THREAD_RETURNTYPE FFTL_THREAD_CALLCONV ThreadFunctor<T_Functor>::InternalStartFunc(void* pThis)
{
	auto* pThread = reinterpret_cast<ThreadFunctor<T_Functor>*>(pThis);
	pThread->Run();
	return FFTL_THREAD_RETURNVALUE;
}

inline void ThreadSleep(uint ms)
{
#if defined(FFTL_PLATFORM_PLAYSTATION)
	sceKernelUsleep(ms * 1000);
#elif defined(_MSC_VER)
	::Sleep(ms);
#elif defined(_POSIX_VERSION)
	usleep(ms * 1000);
#else
#	error "Implement your own platform sleep"
#endif
}



} // namespace FFTL

#endif //_FFTL_THREAD_INL
