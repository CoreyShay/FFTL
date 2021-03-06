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
#	ifndef WIN32_LEAN_AND_MEAN
#		define WIN32_LEAN_AND_MEAN
#	endif
#	include <windows.h>
#	include <profileapi.h>
#	include <intrin.h>
#elif defined(FFTL_PLATFORM_PLAYSTATION)
#	include <kernel.h>
#	include <rtc.h>
#elif defined(FFTL_PLATFORM_ANDROID)
#	include <time.h>
#endif

#if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
#	include <x86intrin.h>
#endif

#if !defined(FFTL_SSE) && !defined(FFTL_ARM_NEON)
#	include <chrono>
#endif


namespace FFTL
{


inline TimerBase::StaticInfo::StaticInfo(TimerType type)
{
#if defined(_MSC_VER)
#	if !defined(FFTL_TIMER_IS_CPUTIMER)
	if (type == TimerType::CPU)
	{
		m_TicksToUsScalar = 1.0 / (2.6 * 1000000000);
	}
	else
#	endif
	{
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		m_TicksToUsScalar = 1000000.0 / static_cast<f64>(freq.QuadPart);
	}
#elif defined(FFTL_PLATFORM_PLAYSTATION)
	if (type == TimerType::CPU)
	{
		const uint64_t freq = sceKernelGetTscFrequency();
		m_TicksToUsScalar = 1000000.0 / freq;
	}
	else
	{
		m_TicksToUsScalar = 1.0;
	}
#elif defined(FFTL_PLATFORM_ANDROID)
	if (type == TimerType::CPU)
	{
#if 0//defined(__arm__)
		// in general enable all counters (including cycle counter)
		int32_t value = 1;

		// peform reset:
		//if (do_reset)
		{
			value |= 2;     // reset all counters to zero.
			value |= 4;     // reset cycle counter to zero.
		}

		//	if (enable_divider)
		//		value |= 8;     // enable "by 64" divider for CCNT.

		value |= 16;

		// program the performance-counter control-register:
		asm volatile ("MCR p15, 0, %0, c9, c12, 0\t\n" :: "r"(value));

		// enable all counters:  
		asm volatile ("MCR p15, 0, %0, c9, c12, 1\t\n" :: "r"(0x8000000f));

		// clear overflows:
		asm volatile ("MCR p15, 0, %0, c9, c12, 3\t\n" :: "r"(0x8000000f));
#endif
		m_TicksToUsScalar = 1.0 / 1000.0;
	}
	else
	{
		m_TicksToUsScalar = 1.0 / 1000.0;
	}
#else
	m_TicksToUsScalar = 1.0 / 1000.0;
#endif

	//	Silence warning on Durango
	(void)type;
}



inline void TimerBase::Reset()
{
	m_TotalTicks = 0;
	m_StartTicks = 0;
	m_nAccumCount = 0;
}

inline void TimerBase::Accum()
{
	m_nAccumCount += 1;
}

//	Use rdtscp for XB1 because QueryPerformanceCounter does exactly that inside a function call anyway.
// Possibly we can do this on PS4 as well if we want CPU clock granularity. PC platforms have kernel
// level throttling, etc, so we should probably stick to QueryPerformanceCounter there.

#if defined(FFTL_SSE)
inline u64 CpuTimer::GetCurrentTicks()
{
	//	TODO: for PC platforms, consider surrounding this instruction with 2 cpuid instructions to fence instruction serialization.
	// http://www.intel.com/content/dam/www/public/us/en/documents/white-papers/ia-32-ia-64-benchmark-code-execution-paper.pdf
#	if defined(__AVX__) // rdtscp is slightly more accurate because it serializes all prior instructions, but not available on older CPU's.
	uint x;
	return __rdtscp(&x);
#	else
	return __rdtsc();
#	endif
}
inline const char* CpuTimer::GetTickUnitsString()
{
	return "Clock cycles";
}
#elif defined(_POSIX_VERSION)
inline u64 CpuTimer::GetCurrentTicks()
{
	struct timespec t0;
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t0);
	return t0.tv_sec * 1000000000ULL + safestatic_cast<u64>(t0.tv_nsec);
}
inline const char* CpuTimer::GetTickUnitsString()
{
	return "Nanoseconds";
}
#else
inline u64 CpuTimer::GetCurrentTicks()
{
	const auto t0 = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::nanoseconds>(t0.time_since_epoch()).count();
}
inline const char* CpuTimer::GetTickUnitsString()
{
	return "Nanoseconds";
}
#endif // defined(_POSIX_VERSION)

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

inline void CpuTimer::Pause()
{
	const u64 end = GetCurrentTicks();
	m_TotalTicks += end - m_StartTicks;
}

inline void CpuTimer::PauseAccum()
{
	Pause();
	Accum();
}


#if !defined(FFTL_TIMER_IS_CPUTIMER)
#	if defined(_MSC_VER)
inline u64 Timer::GetCurrentTicks()
{
	u64 ticks;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&ticks));
	return ticks;
}

inline const char* Timer::GetTickUnitsString()
{
	return "Microseconds";
}
#elif defined(_POSIX_VERSION)

inline u64 Timer::GetCurrentTicks()
{
	struct timespec now;
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &now);
	return now.tv_sec * 1000000000ULL + safestatic_cast<u64>(now.tv_nsec);
}

inline const char* Timer::GetTickUnitsString()
{
	return "Nanoseconds";
}
#	else
#		error "Need platform definitions for Timer methods"
#	endif // defined(_POSIX_VERSION)
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

inline void Timer::Pause()
{
	u64 end = GetCurrentTicks();
	m_TotalTicks += end - m_StartTicks;
}

inline void Timer::PauseAccum()
{
	Pause();
	Accum();
}

#endif // !defined(FFTL_TIMER_IS_CPUTIMER)


template <typename T>
inline TimerScope<T>::TimerScope(T* pTimer)
	: m_pTimer(pTimer)
{
	pTimer->Start();
}

template <typename T>
inline TimerScope<T>::~TimerScope()
{
	m_pTimer->PauseAccum();
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
		m_pTimer->PauseAccum();
}

template <typename T>
inline TimerPauseScope<T>::TimerPauseScope(T* pTimer)
	: m_pTimer(pTimer)
{
	pTimer->Start();
}

template <typename T>
inline TimerPauseScope<T>::~TimerPauseScope()
{
	m_pTimer->Pause();
}

template <typename T>
inline TimerPauseCondScope<T>::TimerPauseCondScope(T* pTimer)
	: m_pTimer(pTimer)
{
	if (pTimer)
		pTimer->Start();
}

template <typename T>
inline TimerPauseCondScope<T>::~TimerPauseCondScope()
{
	if (m_pTimer)
		m_pTimer->Pause();
}




}
