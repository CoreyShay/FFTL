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

#include <type_traits>

#if defined(FFTL_PLATFORM_XBOX) || defined(FFTL_PLATFORM_PLAYSTATION)
#	define FFTL_TIMER_IS_CPUTIMER 1
#endif

namespace FFTL
{


class FFTL_NODISCARD TimerBase
{
public:
	TimerBase() = default;
	void Reset();
	void Accum();		// Increments the accumulation counter
	FFTL_NODISCARD u64 GetTicks() const	{ return m_TotalTicks; }
	FFTL_NODISCARD f64 GetAvgTicks() const { return m_TotalTicks / static_cast<f64>(m_nAccumCount); }

protected:
	u64 m_TotalTicks = 0;
	u64 m_StartTicks = 0;
	u32 m_nAccumCount = 0;

	class StaticInfo
	{
	public:
		enum class TimerType { CPU, CLOCK };
		StaticInfo(TimerType type);

		f64 m_TicksToUsScalar;
	};
};

//	The CPU timer class offers the highest resolution timer possible on supported platforms
// as it uses the actual CPU time stamp counter which counts the number of elapsed CPU cycles.
class FFTL_NODISCARD CpuTimer : public TimerBase
{
public:
	static u64 GetCurrentTicks();

	void Start();
	void Stop();		// Stamps the time from the last start
	void Pause();		// Adds the time from the last start to the current stamped value, but does not increment the accumulation counter
	void PauseAccum();	// Adds the time from the last start to the current stamped value, and increments the accumulation counter

	FFTL_NODISCARD f64 GetMicroseconds() const				{ return ToMicroseconds(m_TotalTicks); }
	FFTL_NODISCARD f64 GetMilliseconds() const				{ return ToMilliseconds(m_TotalTicks); }
	FFTL_NODISCARD f64 GetSeconds() const					{ return ToSeconds(m_TotalTicks); }
	FFTL_NODISCARD f64 GetAvgMicroseconds() const			{ return ToMicroseconds(m_TotalTicks) / m_nAccumCount; }

	FFTL_NODISCARD static inline f64 ToMicroseconds(u64 t)	{ return t * sm_StaticInfo.m_TicksToUsScalar; }
	FFTL_NODISCARD static inline f64 ToMilliseconds(u64 t)	{ return t * sm_StaticInfo.m_TicksToUsScalar / 1000.0; }
	FFTL_NODISCARD static inline f64 ToSeconds(u64 t)		{ return t * sm_StaticInfo.m_TicksToUsScalar / 1000000.0; }

	FFTL_NODISCARD static const char* GetTickUnitsString();

private:
	static const inline StaticInfo sm_StaticInfo{ StaticInfo::TimerType::CPU };
};

//	The timer class offers the highest time resolution supported by the kernel, which is usually
// not as fine grained as the CPU timer, but is the safest one to use when precise elapsed clock
// time in seconds to microseconds and thread consistency is required. Note, on Xbox One, this is
// exactly the same as the CPU timer class (notice the typedef), because QueryPerformanceCounter
// simply calls the CPU time stamp counter, and QueryPerformanceFrequency simply returns the
// constant value of 1745620598 Hz. FYI, PS4 runs at 1593726008 Hz.
#if !defined(FFTL_TIMER_IS_CPUTIMER)
class FFTL_NODISCARD Timer : public TimerBase
{
public:
	static u64 GetCurrentTicks();

	void Start();
	void Stop();		// Stamps the time from the last start
	void Pause();		// Adds the time from the last start to the current stamped value, but does not increment the accumulation counter
	void PauseAccum();	// Adds the time from the last start to the current stamped value, and increments the accumulation counter

	FFTL_NODISCARD f64 GetMicroseconds() const				{ return ToMicroseconds(m_TotalTicks); }
	FFTL_NODISCARD f64 GetMilliseconds() const				{ return ToMilliseconds(m_TotalTicks); }
	FFTL_NODISCARD f64 GetSeconds() const					{ return ToSeconds(m_TotalTicks); }
	FFTL_NODISCARD f64 GetAvgMicroseconds() const			{ return ToMicroseconds(m_TotalTicks) / m_nAccumCount; }

	FFTL_NODISCARD static inline f64 ToMicroseconds(u64 t)	{ return t * sm_StaticInfo.m_TicksToUsScalar; }
	FFTL_NODISCARD static inline f64 ToMilliseconds(u64 t)	{ return t * sm_StaticInfo.m_TicksToUsScalar / 1000.0; }
	FFTL_NODISCARD static inline f64 ToSeconds(u64 t)		{ return t * sm_StaticInfo.m_TicksToUsScalar / 1000000.0; }

	FFTL_NODISCARD static const char* GetTickUnitsString();

private:
	static const inline StaticInfo sm_StaticInfo{ StaticInfo::TimerType::CLOCK };
};
#else
typedef CpuTimer Timer;
#endif

template <typename T>
class TimerScope
{
public:
	TimerScope(T* pTimer);
	~TimerScope();
protected:
	T* m_pTimer;
};

template <typename T = Timer>
class TimerCondScope
{
public:
	TimerCondScope(T* pTimer);
	~TimerCondScope();
protected:
	T* m_pTimer;
};

template <typename T>
class TimerPauseScope
{
public:
	TimerPauseScope(T* pTimer);
	~TimerPauseScope();
protected:
	T* m_pTimer;
};

template <typename T = Timer>
class TimerPauseCondScope
{
public:
	TimerPauseCondScope(T* pTimer);
	~TimerPauseCondScope();
protected:
	T* m_pTimer;
};

#if defined(FFTL_ENABLE_PROFILING)
#	define FFTL_PROFILE_TIMERSCOPE(__name__, __pTimer__) TimerScope<typename std::remove_pointer<decltype(__pTimer__)>::type> __name__(__pTimer__)
#	define FFTL_PROFILE_TIMERSCOPE_PAUSE(__name__, __pTimer__) TimerPauseScope<typename std::remove_pointer<decltype(__pTimer__)>::type> __name__(__pTimer__)
#else
#	define FFTL_PROFILE_TIMERSCOPE(__name__, __pTimer__)
#	define FFTL_PROFILE_TIMERSCOPE_Pause(__name__, __pTimer__)
#endif


}


#include "Timer.inl"
