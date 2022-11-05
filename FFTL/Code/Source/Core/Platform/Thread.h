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
#include "../ReturnCodes.h"

#if defined(_MSC_VER)
#	if _MSC_VER < 1700
#		include <WinBase.h>
#	else
#		include <processthreadsapi.h>
#	endif
#elif defined(FFTL_PLATFORM_PLAYSTATION) && !defined(FFTL_THREAD_USE_POSIX)
#	include <pthread.h>
#	include <sys/_defines/_sce_ok.h>
#elif defined(_POSIX_VERSION)
#	if !defined(_GNU_SOURCE)
#		define _GNU_SOURCE
#	endif
#	include <pthread.h>
#	include <unistd.h>
#endif


#if defined(_MSC_VER)
	typedef DWORD FFTL_THREAD_RETURNTYPE;
#	define FFTL_THREAD_CALLCONV __stdcall
#	define FFTL_THREAD_RETURNVALUE 0
#else
	typedef void* FFTL_THREAD_RETURNTYPE;
#	define FFTL_THREAD_CALLCONV
#	define FFTL_THREAD_RETURNVALUE nullptr
#endif


namespace FFTL
{


enum class ThreadPriority
{
	Lowest,
	VeryLow,
	Low,
	Normal,
	High,
	VeryHigh,
	Highest
};

//	Adding and subtracting integers? Why not?
constexpr ThreadPriority operator+(ThreadPriority t, int i);
constexpr ThreadPriority operator-(ThreadPriority t, int i);


#if defined(_MSC_VER)
typedef DWORD				ThreadId;
typedef HANDLE				ThreadHandle;
#elif defined(FFTL_PLATFORM_PLAYSTATION) && !defined(FFTL_THREAD_USE_POSIX)
typedef uint				ThreadId;
typedef ScePthread			ThreadHandle;
#elif defined(_POSIX_VERSION)
typedef pthread_t			ThreadId;
#define FFTL_THREAD_HANDLE_IS_ID 1
#endif

typedef ReturnCode ThreadResult;

#if defined(FFTL_THREAD_HANDLE_IS_ID)

class ThreadHandle
{
public:
	ThreadHandle() {}
	ThreadHandle(ThreadId id) : m_ID(id) {}

	FFTL_NODISCARD ThreadId GetId() const { return m_ID; }
	ThreadHandle& operator=(ThreadId id) { m_ID = id; return *this; }
	FFTL_NODISCARD bool operator==(ThreadId id) const { return m_ID == id; }
	FFTL_NODISCARD bool operator!=(ThreadId id) const { return m_ID != id; }
	FFTL_NODISCARD bool operator>(ThreadId id) const { return m_ID > id; }
	FFTL_NODISCARD bool operator<(ThreadId id) const { return m_ID < id; }
	FFTL_NODISCARD bool operator>=(ThreadId id) const { return m_ID >= id; }
	FFTL_NODISCARD bool operator<=(ThreadId id) const { return m_ID <= id; }
private:
	ThreadId m_ID;
};

#endif







//	Gets the thread id of the thread of the calling function.
FFTL_NODISCARD ThreadId GetThreadIdCurrent();

//	Gets the thread handle of the thread of the calling function.
FFTL_NODISCARD ThreadHandle GetThreadHandleCurrent();

//	Sets the name of a thread. All platforms.
void SetThreadName(ThreadHandle handle, const char* pszName);

//	Cross platform thread starter function
ThreadId CreateAndStartThread(ThreadHandle* outThreadHandle, FFTL_THREAD_RETURNTYPE (FFTL_THREAD_CALLCONV *pFunc)(void*), void* pParam = nullptr, ThreadPriority priority = ThreadPriority::Normal, uint coreAffinityMask = 0, const char* pszName = nullptr, u32 stackSize = 0);
void WaitForThread(ThreadHandle h);
void FreeThreadHandle(ThreadHandle h);

//	Halts a thread's execution, usually for debug purposes. Only works on MS compilers as of now.
void PauseThread(ThreadId id);
void PauseThread(ThreadHandle handle);

//	Resumes a thread's execution after it was previously paused, usually for debug purposes. Only works on MS compilers as of now.
void UnpauseThread(ThreadId id);
void UnpauseThread(ThreadHandle handle);

//	Terminates a thread's execution, usually for debug purposes. Only works on MS compilers as of now.
void TerminateThread(ThreadId id);
void TerminateThread(ThreadHandle handle);


#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4407) //warning C4407: cast between different pointer to member representations, compiler may generate incorrect code
#endif

// This class must be derived and the Run function must be overridden.
class FFTL_DLLEXPORT ThreadOwner
{
public:
	virtual ~ThreadOwner() = default;
	typedef ThreadResult(ThreadOwner::*RunFunction)();
	template <typename T>
	FFTL_NODISCARD static RunFunction ToRunFunction(ThreadResult (T::*pf)()) { return static_cast<RunFunction>(pf); }
};
#if defined(_MSC_VER)
#	pragma warning(pop)
#endif



class ThreadBase
{
public:
	ThreadBase() = default;

	void Pause();
	void Unpause();

	FFTL_NODISCARD ThreadHandle GetThreadHandle() const;
	FFTL_NODISCARD ThreadId GetThreadId() const;
	FFTL_NODISCARD ThreadResult GetRunResult();
	FFTL_NODISCARD bool GetIsRunning() const;
	FFTL_NODISCARD bool GetIsFlaggedForStop() const;

protected:
	ThreadHandle					m_Handle = 0;
#if !defined(FFTL_THREAD_HANDLE_IS_ID)
	ThreadId						m_ID = 0; //	Handle and ID are the same on posix platforms.
#endif
	ThreadResult					m_RunResult = ReturnCode::OK;
	volatile bool					m_bIsRunning = false;
	volatile bool					m_bFlaggedForStop = false;
};


//	The ThreadMember class is a simple wrapper of a created thread and its platform specific handles.
// Derive your class from ThreadOwner and contain a ThreadMember as a member variable.
class ThreadMember : public ThreadBase
{
public:
	ThreadMember(ThreadOwner::RunFunction func) : m_funcRun(func) {}
	~ThreadMember() { Stop(true); }

	void Start(ThreadOwner* pOwner, const char* pszName = nullptr, ThreadPriority priority = ThreadPriority::Normal, uint coreAffinityMask = 0, u32 stackSize = 0);
	void Stop(bool bWaitForDone = true);
	void Terminate(); // Forces quit. Unsafe.

private:
	void Run();
	static FFTL_THREAD_RETURNTYPE FFTL_THREAD_CALLCONV InternalStartFunc(void* pThis);

private:
	ThreadOwner::RunFunction const	m_funcRun = nullptr;
	ThreadOwner*					m_pOwner = nullptr;
};

//	The ThreadFunctor class calls a function in a separate thread
template <typename T_Functor>
class ThreadFunctor : public ThreadBase
{
public:
	ThreadFunctor(T_Functor func) : m_funcRun(func) {}
	~ThreadFunctor() { Stop(true); }

	void Start(const char* pszName = nullptr, ThreadPriority priority = ThreadPriority::Normal, uint coreAffinityMask = 0, u32 stackSize = 0);
	void Stop(bool bWaitForDone = true);
	void Terminate(); // Forces quit. Unsafe.

private:
	void Run();
	static FFTL_THREAD_RETURNTYPE FFTL_THREAD_CALLCONV InternalStartFunc(void* pThis);

private:
	T_Functor const					m_funcRun = nullptr;
};




}

#include "Thread.inl"
#if defined(_MSC_VER)
#	include "Windows/Thread_Windows.inl"
#elif defined(FFTL_PLATFORM_PLAYSTATION) && !defined(FFTL_THREAD_USE_POSIX)
#	include "Orbis/Thread_Orbis.inl"
#elif defined(_POSIX_VERSION)
#	include "Posix/Thread_Posix.inl"
#endif

