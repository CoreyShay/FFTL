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

#include "defs.h"

namespace FFTL
{


class TimerBase
{
public:
	TimerBase();

	void Reset();

	u64 GetTicks() const	{ return m_TotalTicks; }

protected:
	u64 m_TotalTicks;
	u64 m_StartTicks;
};

//	The CPU timer class offers the highest resolution timer possible on supported platforms
// as it uses the actual CPU time stamp counter which counts the number of elapsed CPU cycles.
class CpuTimer : public TimerBase
{
public:
	static void StaticInit();
	static u64 GetCurrentTicks();

	void Start();
	void Stop();		// Stamps the time from the last start
	void StopAccum();	// Adds the time from the last start to the current stamped value

	f64 GetMicroseconds() const				{ return ToMicroseconds(m_TotalTicks); }
	f64 GetMilliseconds() const				{ return ToMilliseconds(m_TotalTicks); }
	f64 GetSeconds() const					{ return ToSeconds(m_TotalTicks); }

	static inline f64 ToMicroseconds(u64 t)	{ return t * sm_TicksToUsScalar; }
	static inline f64 ToMilliseconds(u64 t)	{ return t * sm_TicksToUsScalar / 1000.0; }
	static inline f64 ToSeconds(u64 t)		{ return t * sm_TicksToUsScalar / 1000000.0; }

	static const char* GetTickUnitsString();

private:
	static f64 sm_TicksToUsScalar;
};

//	The timer class offers the highest time resolution supported by the kernel, which is usually
// not as fine grained as the CPU timer, but is the safest one to use when precise elapsed clock
// time in seconds to microseconds and thread consistency is required. Note, on Xbox One, this is
// exactly the same as the CPU timer class (notice the typedef), because QueryPerformanceCounter
// simply calls the CPU time stamp counter, and QueryPerformanceFrequency simply returns the
// constant value of 1745620598 Hz. FYI, PS4 runs at 1593726008 Hz.
#if !defined(_DURANGO)
class Timer : public TimerBase
{
public:
	static void StaticInit();
	static u64 GetCurrentTicks();

	void Start();
	void Stop();		// Stamps the time from the last start
	void StopAccum();	// Adds the time from the last start to the current stamped value

	f64 GetMicroseconds() const				{ return ToMicroseconds(m_TotalTicks); }
	f64 GetMilliseconds() const				{ return ToMilliseconds(m_TotalTicks); }
	f64 GetSeconds() const					{ return ToSeconds(m_TotalTicks); }

	static inline f64 ToMicroseconds(u64 t)	{ return t * sm_TicksToUsScalar; }
	static inline f64 ToMilliseconds(u64 t)	{ return t * sm_TicksToUsScalar / 1000.0; }
	static inline f64 ToSeconds(u64 t)		{ return t * sm_TicksToUsScalar / 1000000.0; }

	static const char* GetTickUnitsString();

private:
	static f64 sm_TicksToUsScalar;
};
#else
typedef CpuTimer Timer;
#endif

template <typename T>
class TimerScopeBase
{
public:
	TimerScopeBase(T* pTimer);
	~TimerScopeBase();
protected:
	T* m_pTimer;
};

template <typename T = Timer>
class TimerCondScopeBase
{
public:
	TimerCondScopeBase(T* pTimer);
	~TimerCondScopeBase();
protected:
	T* m_pTimer;
};

typedef TimerScopeBase<Timer> TimerScope;
typedef TimerCondScopeBase<Timer> TimerCondScope;

#if defined(FFTL_ENABLE_PROFILING)
#	define FFTL_PROFILE_TIMERSCOPE(__name__, __pTimer__) TimerScope __name__(__pTimer__)
#else
#	define FFTL_PROFILE_TIMERSCOPE(__name__, __pTimer__)
#endif


}


#include "Timer.inl"
