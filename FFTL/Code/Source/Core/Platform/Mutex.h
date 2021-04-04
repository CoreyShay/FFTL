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

#define FFTL_MUTEX_DEADLOCK_HELP 0 // Enable this to find possible deadlocks due to wrong mutex (un)locking

#if defined(FFTL_PLATFORM_PLAYSTATION)
#	include <kernel.h>
#	include <sys/_defines/_sce_ok.h>
#elif defined(_MSC_VER)
#	ifndef WIN32_LEAN_AND_MEAN
#		define WIN32_LEAN_AND_MEAN
#	endif
#	include <windows.h>
#elif defined(_POSIX_VERSION) || defined(FFTL_THREAD_USE_POSIX)
#	include <pthread.h>
#endif

#include "Thread.h"


namespace FFTL
{


class [[nodiscard]] Mutex
{
#if defined(FFTL_PLATFORM_PLAYSTATION) && !defined(FFTL_THREAD_USE_POSIX)
public:
	Mutex()
	{
		ScePthreadMutexattr attr;
		FFTL_VERIFY_EQ( SCE_OK, scePthreadMutexattrInit(&attr) );
		FFTL_VERIFY_EQ( SCE_OK, scePthreadMutexattrSettype(&attr, PTHREAD_MUTEX_RECURSIVE) );
		FFTL_VERIFY_EQ( SCE_OK, scePthreadMutexInit(&m_mutex, &attr, nullptr) );
		FFTL_VERIFY_EQ( SCE_OK, scePthreadMutexattrDestroy(&attr) );
	}
	~Mutex()
	{
		FFTL_VERIFY_EQ( SCE_OK, scePthreadMutexDestroy(&m_mutex) );
	}
	void Lock()
	{
		FFTL_VERIFY_EQ( SCE_OK, scePthreadMutexLock(&m_mutex) );
	}
	bool TryLock()
	{
		return SCE_OK == scePthreadMutexTrylock(&m_mutex);
	}
	void Unlock()
	{
		FFTL_VERIFY_EQ( SCE_OK, scePthreadMutexUnlock(&m_mutex) );
	}
	[[nodiscard]] ThreadId GetOwningThread() const
	{
		//	The owned thread id really is the first part of the data behind the pointer.
		return *reinterpret_cast<const ThreadId*>(m_mutex);
	}

private:
	ScePthreadMutex m_mutex;




#elif defined(_POSIX_VERSION) || defined(FFTL_THREAD_USE_POSIX)
public:
	Mutex()
	{
		pthread_mutexattr_t attr;
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
		pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE);
		pthread_mutex_init(&m_mutex, &attr);
		pthread_mutexattr_destroy(&attr);
	}
	~Mutex()
	{
		pthread_mutex_destroy(&m_mutex);
	}
	void Lock()
	{
		pthread_mutex_lock(&m_mutex);
	}
	bool TryLock()
	{
		return 0 == pthread_mutex_trylock(&m_mutex);
	}
	void Unlock()
	{
		pthread_mutex_unlock(&m_mutex);
	}
	[[nodiscard]] ThreadId GetOwningThread() const
	{
		//	TODO
		FFTL_ASSERT_MSG(0, "Don't call this. It doesn't work yet.");
		return *reinterpret_cast<const ThreadId*>(&m_mutex);
	}

private:
	pthread_mutex_t m_mutex;




#elif defined(_MSC_VER) //Microsoft platforms
public:
	Mutex() 
#if FFTL_MUTEX_DEADLOCK_HELP
	: lastThreadId(0)
	, lockCount(0)
#endif
	{
		InitializeCriticalSection(&m_cs);
	}
	~Mutex()
	{
		DeleteCriticalSection(&m_cs);
	}
	void Lock()
	{
		EnterCriticalSection(&m_cs);
#if FFTL_MUTEX_DEADLOCK_HELP
		lastThreadId = GetCurrentThreadId();
		lockCount++;
#endif
	}
	bool TryLock()
	{
#if FFTL_MUTEX_DEADLOCK_HELP
		bool ret = (FALSE != TryEnterCriticalSection(&m_cs));
		if (ret)
		{
			lastThreadId = GetCurrentThreadId();
			lockCount++;
		}
		return ret;
#else
		return FALSE != TryEnterCriticalSection(&m_cs);
#endif
	}
	void Unlock()
	{
#if FFTL_MUTEX_DEADLOCK_HELP
		FFTL_ASSERT(lockCount != 0);
		if(--lockCount == 0)
			lastThreadId = 0;
#endif
		LeaveCriticalSection(&m_cs);
	}
	[[nodiscard]] ThreadId GetOwningThread() const
	{
		return ::GetThreadId(m_cs.OwningThread);
	}

private:
#if FFTL_MUTEX_DEADLOCK_HELP
	DWORD lockCount;
	DWORD lastThreadId;
#endif
	CRITICAL_SECTION m_cs;

#endif //platform

public:

	[[nodiscard]] bool GetIsLockedByThisThread() const
	{
		return GetOwningThread() == GetThreadIdCurrent();
	}

	void AssertLockedByThisThread(bool bLocked) const
	{
#if defined(FFTL_ENABLE_ASSERT)
		const ThreadId owningThread = GetOwningThread();
		const ThreadId thisThread = GetThreadIdCurrent();
		FFTL_ASSERT_MSG(bLocked == (owningThread == thisThread), "Mutex locked by ThreadId %u is %slocked by the current thread, %u!", owningThread, bLocked ? "not " : "", thisThread);
#else
		(void)bLocked;
#endif
	}


	Mutex(const Mutex&) = delete;
	Mutex& operator=(const Mutex&) = delete;
};


class MutexScopedLock
{
public:
	MutexScopedLock(Mutex* pMutex)
	{
		m_pMutex = pMutex;
		pMutex->Lock();
	}
	~MutexScopedLock()
	{
		m_pMutex->Unlock();
	}

private:
	Mutex* m_pMutex;
};

} //namespace FFTL


