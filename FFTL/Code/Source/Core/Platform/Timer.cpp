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


#include "Timer.h"


namespace FFTL
{

f64 FFTL::Timer::sm_TicksToUsScalar = 1.0;
#if !defined(_DURANGO)
f64 FFTL::CpuTimer::sm_TicksToUsScalar = 1.0;
#endif


# if defined(_MSC_VER)

void CpuTimer::StaticInit()
{
#if !defined(_DURANGO)
	CpuTimer::sm_TicksToUsScalar = 1.0 / (2.6 * 1000000000);
#else
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	CpuTimer::sm_TicksToUsScalar = 1000000.0 / f64(freq.QuadPart);
#endif
}

#if !defined(_DURANGO)
void Timer::StaticInit()
{
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	Timer::sm_TicksToUsScalar = 1000000.0 / (f64)freq.QuadPart;
}
#endif

#elif defined(__ORBIS__) || defined(__PROSPERO__)

void Timer::StaticInit()
{
	Timer::sm_TicksToUsScalar = 1.0;
}

void CpuTimer::StaticInit()
{
	const uint64_t freq = sceKernelGetTscFrequency();
	CpuTimer::sm_TicksToUsScalar = 1000000.0 / freq;
}

#elif defined(__ANDROID__)

void Timer::StaticInit()
{
	Timer::sm_TicksToUsScalar = 1.0 / 1000.0;
}

void CpuTimer::StaticInit()
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

	CpuTimer::sm_TicksToUsScalar = 1.0 / 1000.0;
}
#endif


}
