#pragma once

#include "defs.h"

#if defined(_MSC_VER)
#	include <synchapi.h>
#elif (defined(__ORBIS__) ||defined(__PROSPERO__)) && !defined(FFTL_THREAD_USE_POSIX)
#	include <kernel.h>
#	include <sys/_defines/_sce_ok.h>
#elif defined(FFTL_THREAD_USE_POSIX)
#	include <semaphore.h>
#	include <errno.h>
#	include <sys/eventfd.h>
#	include <poll.h>
#endif


namespace FFTL
{


//	AKA: BinarySemaphore
//	The ThreadEvent class is useful usually where you want synchronization between two threads.
// Think of it as a traffic light that has two states (red and green). The semaphore defaults to red
// on construction. One thread updates, and when sync is needed, it will call Wait() on the semaphore
// which will block until the light turns green. A second thread spins and periodically calls Signal()
// which flips the light to green. The waiting thread sees this signal and wakes up, where the Wait()
// call unblocks. As the thread resumes (moves through the intersection), the semaphore immediately
// reverts back to non-signaled (red light), so in essence only a single thread passes through a signal
// event at a time. First in, first out based, not priority based.
class ThreadEvent
{
public:
	ThreadEvent(const char* pszName = nullptr);
	~ThreadEvent();

	ThreadEvent(const ThreadEvent&) = delete;
	ThreadEvent(ThreadEvent&&) = delete;
	ThreadEvent& operator=(const ThreadEvent&) = delete;
	ThreadEvent& operator=(ThreadEvent&&) = delete;

	void Wait();
	bool Wait(u32 timeOut_mS); // returns true if returned due to timeout.
	void Signal();
	void Unsignal();

#if defined(_MSC_VER)
	typedef HANDLE HandleType;
#elif (defined(__ORBIS__) || defined(__PROSPERO__)) && !defined(FFTL_THREAD_USE_POSIX)
	typedef SceKernelEventFlag HandleType;
#elif defined(FFTL_THREAD_USE_POSIX)
	typedef pollfd HandleType;
#endif //platform

#if (defined(__ORBIS__) || defined(__PROSPERO__)) && !defined(FFTL_THREAD_USE_POSIX)
	HandleType GetHandle() const { return m_Handle; }
	HandleType GetHandle() { return m_Handle; }
#else
	HandleType const GetHandle() const { return m_Handle; }
	HandleType GetHandle() { return m_Handle; }
#endif

private:
	HandleType m_Handle;
};


#if defined(_MSC_VER)
//	We just use the Event kernel object because the semaphore object only exists for Vista and above (and consoles).
// Event is all we really need in this case because otherwise we'd just use a semaphore with a single token.

inline ThreadEvent::ThreadEvent(const char* pszName)
	: m_Handle( ::CreateEvent(nullptr, FALSE, FALSE, pszName ? pszName : "ThreadEvent") )
{
	FFTL_ASSERT_MSG(m_Handle, "ThreadEvent() : CreateEvent failed!");
}
inline ThreadEvent::~ThreadEvent()
{
	FFTL_VERIFY_MSG( ::CloseHandle(m_Handle), "~ThreadEvent() : CloseHandle failed!" );
}
inline void ThreadEvent::Wait()
{
	FFTL_ASSERT_MSG(m_Handle, "ThreadEvent::Wait() : Handle is nullptr!");
	FFTL_VERIFY_MSG( WAIT_FAILED != ::WaitForSingleObject(m_Handle, INFINITE), "ThreadEvent::Wait() : WaitForSingleObject failed!" );
}
inline bool ThreadEvent::Wait(u32 timeOut_mS)
{
	FFTL_ASSERT_MSG(m_Handle, "ThreadEvent::Wait() : Handle is nullptr!");
	const DWORD result = ::WaitForSingleObject(m_Handle, timeOut_mS);
	FFTL_ASSERT_MSG( WAIT_FAILED != result, "ThreadEvent::Wait() : WaitForSingleObject failed!" );
	return result == WAIT_TIMEOUT;
}
inline void ThreadEvent::Signal()
{
	FFTL_ASSERT_MSG(m_Handle, "ThreadEvent::Signal() : Handle is nullptr!");
	FFTL_VERIFY_MSG( FALSE != ::SetEvent(m_Handle), "ThreadEvent::Signal() : SetEvent failed!" );
}
inline void ThreadEvent::Unsignal()
{
	FFTL_ASSERT_MSG(m_Handle, "ThreadEvent::Signal() : Handle is nullptr!");
	FFTL_VERIFY_MSG( FALSE != ::ResetEvent(m_Handle), "ThreadEvent::Unsignal() : ResetEvent failed!" );
}


#elif (defined(__ORBIS__) || defined(__PROSPERO__)) && !defined(FFTL_THREAD_USE_POSIX)

inline ThreadEvent::ThreadEvent(const char* pszName)
	: m_Handle(0)
{
//	FFTL_VERIFY_EQ_MSG( SCE_OK, ::sceKernelCreateSema(&m_Handle, "CDC Semaphore", SCE_KERNEL_SEMA_ATTR_TH_FIFO | SCE_KERNEL_EVF_ATTR_MULTI, 0, 1, nullptr), "ThreadEvent() : CreateEvent failed!" );
	FFTL_VERIFY_EQ_MSG( SCE_OK, ::sceKernelCreateEventFlag(&m_Handle, pszName ? pszName : "ThreadEvent", SCE_KERNEL_EVF_ATTR_TH_FIFO | SCE_KERNEL_EVF_ATTR_MULTI, 0, nullptr), "ThreadEvent() : sceKernelCreateEventFlag failed!" );
}
inline ThreadEvent::~ThreadEvent()
{
	FFTL_VERIFY_EQ_MSG( SCE_OK, sceKernelDeleteEventFlag(m_Handle), "~ThreadEvent() : sceKernelDeleteEventFlag failed!" );
}
inline void ThreadEvent::Wait()
{
	FFTL_ASSERT_MSG(m_Handle, "ThreadEvent::Wait() : Handle is nullptr!");
//	FFTL_VERIFY_EQ_MSG( SCE_OK, ::sceKernelWaitSema(m_Handle, 1, nullptr), "ThreadEvent::Wait() : sceKernelWaitSema failed!" );
	FFTL_VERIFY_EQ_MSG( SCE_OK, ::sceKernelWaitEventFlag(m_Handle, 1, SCE_KERNEL_EVF_WAITMODE_AND | SCE_KERNEL_EVF_WAITMODE_CLEAR_PAT, nullptr, nullptr), "ThreadEvent::Wait() : sceKernelWaitEventFlag failed!" );
}
inline bool ThreadEvent::Wait(u32 timeOut_mS)
{
	FFTL_ASSERT_MSG(m_Handle, "ThreadEvent::Wait() : Handle is nullptr!");
	SceKernelUseconds myTimer = (SceKernelUseconds)timeOut_mS * 1000;
//	const int result = ::sceKernelWaitSema(m_Handle, 1, &myTimer);
	const int result = ::sceKernelWaitEventFlag(m_Handle, 1, SCE_KERNEL_EVF_WAITMODE_AND | SCE_KERNEL_EVF_WAITMODE_CLEAR_PAT, nullptr, &myTimer);
	FFTL_ASSERT_MSG( SCE_OK == result || SCE_KERNEL_ERROR_ETIMEDOUT == result, "ThreadEvent::Wait() : sceKernelWaitEventFlag failed!" );
	return result == SCE_KERNEL_ERROR_ETIMEDOUT;
}
inline void ThreadEvent::Signal()
{
	FFTL_ASSERT_MSG(m_Handle, "ThreadEvent::Signal() : Handle is nullptr!");
//	FFTL_VERIFY_EQ_MSG( SCE_OK, ::sceKernelSignalSema(m_Handle, 1), "ThreadEvent::Signal() : sceKernelSignalSema failed!" );
	FFTL_VERIFY_EQ_MSG( SCE_OK, ::sceKernelSetEventFlag(m_Handle, 1), "ThreadEvent::Signal() : sceKernelSetEventFlag failed!" );
}
inline void ThreadEvent::Unsignal()
{
	FFTL_ASSERT_MSG(m_Handle, "ThreadEvent::Signal() : Handle is nullptr!");
	FFTL_VERIFY_EQ_MSG( SCE_OK, ::sceKernelClearEventFlag(m_Handle, 1), "ThreadEvent::Unsignal() : sceKernelClearEventFlag failed!" );
}

#elif defined(FFTL_THREAD_USE_POSIX)

inline ThreadEvent::ThreadEvent(const char* pszName)
	: m_Handle{ ::eventfd(POLLIN | POLLOUT, 0), 0, 0 }
{
	(void)pszName;
	FFTL_ASSERT_MSG(m_Handle.fd, "ThreadEvent() : eventfd failed!");
//	FFTL_VERIFY_EQ_MSG( 0, ::sem_init(&m_Handle, 0, 0), "ThreadEvent() : CreateEvent failed!" );
}
inline ThreadEvent::~ThreadEvent()
{
	FFTL_VERIFY_EQ_MSG( 0, ::close(m_Handle.fd), "~ThreadEvent() : close failed!" );
//	FFTL_VERIFY_EQ_MSG( 0, ::sem_destroy(&m_Handle), "~ThreadEvent() : sem_destroy failed!" );
}
inline void ThreadEvent::Wait()
{
	uint64_t value;
	FFTL_VERIFY_EQ_MSG( sizeof(value), ::read(m_Handle.fd, &value, sizeof(value)), "ThreadEvent::Wait() : read failed!" );
//	FFTL_VERIFY_EQ_MSG( 0, ::sem_wait(&m_Handle), "ThreadEvent::Wait() : sem_wait failed!" );
}
inline bool ThreadEvent::Wait(u32 timeOut_mS)
{
	uint64_t value;
	FFTL_VERIFY_EQ_MSG( sizeof(value), ::read(m_Handle.fd, &value, sizeof(value)), "ThreadEvent::Wait() : read failed!" );
	return false;

//	const int result = ::poll(&m_Handle, 1, timeOut_mS);
//	FFTL_ASSERT_MSG( result == 0 || result == 1, "ThreadEvent::Wait() : poll failed!" );
//	return result == 0;

//	timespec myTimer;
//	myTimer.tv_sec = timeOut_mS / 1000;
//	myTimer.tv_nsec = (timeOut_mS % 1000) * 1000000;
//	const int result = ::sem_timedwait(&m_Handle, &myTimer);
//	FFTL_ASSERT_MSG( -1 != result, "ThreadEvent::Wait() : sem_wait failed!" );
//	return result == ETIMEDOUT;
}
inline void ThreadEvent::Signal()
{
	uint64_t value = 1;
	FFTL_VERIFY_EQ_MSG( sizeof(value), ::write(m_Handle.fd, &value, sizeof(value)), "write failed!" );

	//	Don't allow the signal count to go above 1.
//	int sigCount;
//	FFTL_VERIFY_EQ_MSG( 0, ::sem_getvalue(&m_Handle, &sigCount), "ThreadEvent::Signal() : sem_getvalue failed!" );
//	if (sigCount <= 0)
//		FFTL_VERIFY_EQ_MSG( 0, ::sem_post(&m_Handle), "ThreadEvent::Signal() : sem_post failed!" );
}

#endif //platform


} //namespace FFTL

