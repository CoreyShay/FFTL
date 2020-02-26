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

#include "CpuInfo.h"

#if defined(_MSC_VER)
#	include <intrin.h>
#endif


namespace FFTL
{


//	Some platforms need no initialization at all because everything is known to the compiler.
#if !defined(FFTL_CPU_INFO_KNOWN_TO_COMPILER)


CpuInfo::ArchFlags CpuInfo::s_archFlags = CpuInfo::ArchFlags::DEFAULT;
CpuInfo::ExtFlags CpuInfo::s_extFlags = CpuInfo::ExtFlags::DEFAULT;

#if defined(__GNUC__) && defined(FFTL_SSE)
FFTL_FORCEINLINE void __cpuid(int regs[4], int i)
{
	__asm__ __volatile__ (
		"cpuid":
		"=a" (regs[0]),
		"=b" (regs[1]),
		"=c" (regs[2]),
		"=d" (regs[3]) :
		"a" (i), "c" (0)
		);
}
FFTL_FORCEINLINE u64 _xgetbv(uint)
{
	size_t xcr0;
	__asm__("xgetbv" : "=a" (xcr0) : "c" (0) : "%edx");
	return xcr0;
}
#define _XCR_XFEATURE_ENABLED_MASK 0
#endif

void CpuInfo::Init()
{
	s_archFlags = DetectArchitectureSupport();
	s_extFlags = DetectExtensionSupport();
}

#if !(defined(__ORBIS__) || defined(__PROSPERO__) || defined(_DURANGO)) && defined(FFTL_SSE)
CpuInfo::ArchFlags CpuInfo::DetectArchitectureSupport()
{
	//	https://en.wikipedia.org/wiki/CPUID

	ArchFlags retFlags = ArchFlags::DEFAULT;

	int cpuInfo[4];
	__cpuid(cpuInfo, 1);

	const int cpuFamily = ((cpuInfo[0] & 0xFF00000) >> 20) + ((cpuInfo[0] & 0xF00) >> 8);

	retFlags |= ((cpuInfo[3] & (1 << 25))) != 0 ? ArchFlags::SSE  : ArchFlags::DEFAULT;
	retFlags |= ((cpuInfo[3] & (1 << 26))) != 0 ? ArchFlags::SSE2 : ArchFlags::DEFAULT;
	retFlags |= ((cpuInfo[2] & (1 <<  0)) | (1 <<  9)) != 0 ? ArchFlags::SSE3 : ArchFlags::DEFAULT;
	retFlags |= ((cpuInfo[2] & (1 << 19)) | (1 << 20)) != 0 ? ArchFlags::SSE4 : ArchFlags::DEFAULT;

	const bool cpuAVXSupport =			(cpuInfo[2] & (1 << 28)) != 0;
	const bool osUsesXSAVE_XRSTORE =	(cpuInfo[2] & (1 << 27)) != 0;

	if (osUsesXSAVE_XRSTORE && cpuAVXSupport)
	{
		const u64 xcrFeatureMask = _xgetbv(_XCR_XFEATURE_ENABLED_MASK);
		if ((xcrFeatureMask & 0x6) == 0x6)
		{
			//	Now hold on. AMD 15h family CPU's handle AVX horribly, so make an exception for them.
			__cpuid(cpuInfo, 0);
			const int cpuName[4] = { cpuInfo[1], cpuInfo[3], cpuInfo[2], 0 };
			if (!strstr((const char*)cpuName, "AMD") || cpuFamily != 0x15)
			{
				retFlags |= ArchFlags::AVX;

				//	And finally, check for AVX2.
				__cpuid(cpuInfo, 0x80000000);
				const uint nExIds = cpuInfo[0];
				if (nExIds >= 7)
				{
					__cpuid(cpuInfo, 7);
					retFlags |= ((cpuInfo[1] & (1 <<  5))) != 0 ? ArchFlags::AVX2				: ArchFlags::DEFAULT;
					retFlags |= ((cpuInfo[1] & (1 << 16))) != 0 ? ArchFlags::AVX512_F			: ArchFlags::DEFAULT;
					retFlags |= ((cpuInfo[1] & (1 << 17))) != 0 ? ArchFlags::AVX512_DQ			: ArchFlags::DEFAULT;
					retFlags |= ((cpuInfo[1] & (1 << 21))) != 0 ? ArchFlags::AVX512_IFMA		: ArchFlags::DEFAULT;
					retFlags |= ((cpuInfo[1] & (1 << 26))) != 0 ? ArchFlags::AVX512_PF			: ArchFlags::DEFAULT;
					retFlags |= ((cpuInfo[1] & (1 << 27))) != 0 ? ArchFlags::AVX512_ER			: ArchFlags::DEFAULT;
					retFlags |= ((cpuInfo[1] & (1 << 28))) != 0 ? ArchFlags::AVX512_CD			: ArchFlags::DEFAULT;
					retFlags |= ((cpuInfo[1] & (1 << 30))) != 0 ? ArchFlags::AVX512_BW			: ArchFlags::DEFAULT;
					retFlags |= ((cpuInfo[1] & (1 << 31))) != 0 ? ArchFlags::AVX512_VL			: ArchFlags::DEFAULT;
					retFlags |= ((cpuInfo[2] & (1 <<  1))) != 0 ? ArchFlags::AVX512_VBMI		: ArchFlags::DEFAULT;
					retFlags |= ((cpuInfo[2] & (1 <<  6))) != 0 ? ArchFlags::AVX512_VBMI2		: ArchFlags::DEFAULT;
					retFlags |= ((cpuInfo[2] & (1 << 11))) != 0 ? ArchFlags::AVX512_VNNI		: ArchFlags::DEFAULT;
					retFlags |= ((cpuInfo[2] & (1 << 12))) != 0 ? ArchFlags::AVX512_BITALG		: ArchFlags::DEFAULT;
					retFlags |= ((cpuInfo[2] & (1 << 14))) != 0 ? ArchFlags::AVX512_VPOPCNTDQ	: ArchFlags::DEFAULT;
					retFlags |= ((cpuInfo[3] & (1 <<  2))) != 0 ? ArchFlags::AVX512_4VNNIW		: ArchFlags::DEFAULT;
					retFlags |= ((cpuInfo[3] & (1 <<  3))) != 0 ? ArchFlags::AVX512_4FMAPS		: ArchFlags::DEFAULT;
				}
			}
		}
	}

	return retFlags;
}
CpuInfo::ExtFlags CpuInfo::DetectExtensionSupport()
{
	ExtFlags retFlags = ExtFlags::DEFAULT;

	int cpuInfo[4];

	//	Check for Intel fused multiply-add
	__cpuid(cpuInfo, 1);
	if ((cpuInfo[2] & (1 << 12)) != 0)
		retFlags |= ExtFlags::FMA3;

	//	Check for AMD fused multiply-add
	__cpuid(cpuInfo, 0x80000001);
	if ((cpuInfo[2] & (1 << 16)) != 0)
		retFlags |= ExtFlags::FMA4;

	return retFlags;
}
#endif

ReturnCode CpuInfo::SetArchitectureEnabled(Architecture platform, bool bEnable)
{
	const ArchFlags platFlag = ToFlag(platform);

	if (bEnable)
	{
		if ((s_archFlags & platFlag) != ArchFlags::DEFAULT)
			return ReturnCode::OK;
		else if ((DetectArchitectureSupport() & platFlag) != ArchFlags::DEFAULT)
			s_archFlags |= platFlag;
		else
			return ReturnCode::ERROR_INCOMPATIBLE;
	}
	else
	{
		s_archFlags &= (~platFlag);
	}

	return ReturnCode::OK;
}

ReturnCode CpuInfo::SetExtensionEnabled(Extension ext, bool bEnable)
{
	const ExtFlags extFlag = ToFlag(ext);

	if (bEnable)
	{
		if ((s_extFlags & extFlag) != ExtFlags::DEFAULT)
			return ReturnCode::OK;
		else if ((DetectExtensionSupport() & extFlag) != ExtFlags::DEFAULT)
			s_extFlags |= extFlag;
		else
			return ReturnCode::ERROR_INCOMPATIBLE;
	}
	else
	{
		s_extFlags &= (~extFlag);
	}

	return ReturnCode::OK;
}


#endif// !defined(FFTL_CPU_INFO_KNOWN_TO_COMPILER)


}


