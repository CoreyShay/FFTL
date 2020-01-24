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


#if defined(_MSC_VER)
#	include <intrin.h>
#	if !defined(_DURANGO)
#		include <windows.h>
#		include <profileapi.h>
#	endif
#elif defined(__ORBIS__) || defined(__PROSPERO__)
#	include <kernel.h>
#	include <rtc.h>
#elif defined(__ANDROID__)
#	include <time.h>
#endif

#if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
#	include <x86intrin.h>
#endif


namespace FFTL
{


inline void TimerBase::Reset()
{
	m_TotalTicks = 0;
	m_StartTicks = 0;
	m_nAccumCount = 0;
}

//	Use rdtscp for XB1 because QueryPerformanceCounter does exactly that inside a function call anyway.
// Possibly we can do this on PS4 as well if we want CPU clock granularity. PC platforms have kernel
// level throttling, etc, so we should probably stick to QueryPerformanceCounter there.

#if defined(FFTL_SSE)
inline u64 CpuTimer::GetCurrentTicks()
{
	//	TODO: for PC platforms, consider surrounding this instruction with 2 cpuid instructions to fence instruction serialization.
	// http://www.intel.com/content/dam/www/public/us/en/documents/white-papers/ia-32-ia-64-benchmark-code-execution-paper.pdf
#if defined(__AVX__) // rdtscp is slightly more accurate because it serializes all prior instructions, but not available on older CPU's.
	uint x;
	return __rdtscp(&x);
#else
	return __rdtsc();
#endif
}
inline const char* CpuTimer::GetTickUnitsString()
{
	return "Clock cycles";
}
#elif defined(__ANDROID__)
inline u64 CpuTimer::GetCurrentTicks()
{
	struct timespec now;
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &now);
	return now.tv_sec * 1000000000ULL + (u64)now.tv_nsec;
}
inline const char* CpuTimer::GetTickUnitsString()
{
	return "Nanoseconds";
}
#endif

inline void CpuTimer::Start()
{
	m_StartTicks = GetCurrentTicks();
}

inline void CpuTimer::Stop()
{
	const u64 end = GetCurrentTicks();
	m_TotalTicks = end - m_StartTicks;
	m_nAccumCount = 0;
}

inline void CpuTimer::StopAccum()
{
	const u64 end = GetCurrentTicks();
	m_TotalTicks += end - m_StartTicks;
	m_nAccumCount += 1;
}


# if defined(_MSC_VER) && !defined(_DURANGO)

inline u64 Timer::GetCurrentTicks()
{
	u64 ticks;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&ticks));
	return ticks;
}

inline void Timer::Start()
{
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_StartTicks));
}

inline void Timer::Stop()
{
	LARGE_INTEGER end;
	QueryPerformanceCounter(&end);
	m_TotalTicks = end.QuadPart - m_StartTicks;
	m_nAccumCount = 0;
}

inline void Timer::StopAccum()
{
	LARGE_INTEGER end;
	QueryPerformanceCounter(&end);
	m_TotalTicks += end.QuadPart - m_StartTicks;
	m_nAccumCount += 1;
}

inline const char* Timer::GetTickUnitsString()
{
	return "Clock ticks";
}

#elif defined(__ORBIS__) || defined(__PROSPERO__)

inline u64 Timer::GetCurrentTicks()
{
	return sceKernelGetProcessTime();
}

inline void Timer::Start()
{
	m_StartTicks = sceKernelGetProcessTime();
}

inline void Timer::Stop()
{
	const u64 end = sceKernelGetProcessTime();
	m_TotalTicks += end - m_StartTicks;
	m_nAccumCount = 0;
}

inline void Timer::StopAccum()
{
	const u64 end = sceKernelGetProcessTime();
	m_TotalTicks += end - m_StartTicks;
	m_nAccumCount += 1;
}

inline const char* Timer::GetTickUnitsString()
{
	return "Microseconds";
}

#elif defined(__ANDROID__)

inline u64 Timer::GetCurrentTicks()
{
	struct timespec now;
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &now);
	return now.tv_sec * 1000000000ULL + (u64)now.tv_nsec;
}

inline void Timer::Start()
{
	m_StartTicks = GetCurrentTicks();
}

inline void Timer::Stop()
{
	u64 end = GetCurrentTicks();
	m_TotalTicks = end - m_StartTicks;
	m_nAccumCount = 0;
}

inline void Timer::StopAccum()
{
	u64 end = GetCurrentTicks();
	m_TotalTicks += end - m_StartTicks;
	m_nAccumCount += 1;
}

inline const char* Timer::GetTickUnitsString()
{
	return "Nanoseconds";
}

#endif

template <typename T>
inline TimerScope<T>::TimerScope(T* pTimer)
	: m_pTimer(pTimer)
{
	pTimer->Start();
}

template <typename T>
inline TimerScope<T>::~TimerScope()
{
	m_pTimer->StopAccum();
}

template <typename T>
inline TimerCondScope<T>::TimerCondScope(T* pTimer)
	: m_pTimer(pTimer)
{
	if (pTimer)
		pTimer->Start();
}

template <typename T>
inline TimerCondScope<T>::~TimerCondScope()
{
	if (m_pTimer)
		m_pTimer->StopAccum();
}




}